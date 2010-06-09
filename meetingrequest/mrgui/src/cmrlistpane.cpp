/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  List pane component for UI fields
*
*/
#include "cmrlistpane.h"
#include "mesmrfieldstorage.h"
#include "mmrscrollbarobserver.h"
#include "esmrdef.h"
#include "cesmrfield.h"
#include "cmrfieldcontainer.h"
#include "cmrlistpanephysics.h"
#include "nmrlayoutmanager.h"

#include <eikscrlb.h>
#include <AknUtils.h>
#include <touchfeedback.h>

//DEBUG
#include "emailtrace.h"

namespace { // codescanner::namespace

const TInt KLongTapDelay( 700000 ); // 0,7 sec
const TInt KLongTapAnimationDelay( 300000 ); // 0,3 sec

// ----------------
// IndexByFieldId
// ----------------
//
TInt IndexByFieldId( const MESMRFieldStorage& aFactory,
                     TESMREntryFieldId aFieldId )
    {
    TInt index( KErrNotFound );
    TInt count( aFactory.Count() );

    for ( TInt i = 0; i < count; ++i )
        {
        if ( aFactory.Field( i )->FieldId() == aFieldId )
            {
            index = i;
            break;
            }
        }

    return index;
    }
}

//----- MEMBER FUNCTIONS ----

// ---------------------------------------------------------------------------
// CMRListPane::CMRListPane
// ---------------------------------------------------------------------------
//
CMRListPane::CMRListPane( MESMRFieldStorage& aFactory,
                          TAknDoubleSpanScrollBarModel& aScrollModel,
                          CAknDoubleSpanScrollBar& aScroll,
                          MMRScrollBarObserver& aScrollBarObserver )
    : iFactory( aFactory ),
      iScrollModel( aScrollModel ),
      iScroll( aScroll ),
      iScrollBarObserver( aScrollBarObserver )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRListPane::~CMRListPane
// ---------------------------------------------------------------------------
//
CMRListPane::~CMRListPane()
    {
    FUNC_LOG;
    delete iLongtapDetector;
    delete iPhysics;
    delete iFieldContainer;
    }

// ---------------------------------------------------------------------------
// CMRListPane::NewL
// ---------------------------------------------------------------------------
//
CMRListPane* CMRListPane::NewL( const CCoeControl& aParent,
                                MESMRFieldStorage& aFactory,
                                TAknDoubleSpanScrollBarModel& aScrollModel,
                                CAknDoubleSpanScrollBar& aScroll,
                                MMRScrollBarObserver& aScrollBarObserver )
    {
    FUNC_LOG;
    CMRListPane* self = new( ELeave )CMRListPane(
            aFactory,
            aScrollModel,
            aScroll,
            aScrollBarObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRListPane::ConstructL
// ---------------------------------------------------------------------------
//
void CMRListPane::ConstructL( const CCoeControl& aParent )
    {
    FUNC_LOG;
    CCoeControl::SetComponentsToInheritVisibility( ETrue );
    SetContainerWindowL( aParent );

    iLongtapDetector = CAknLongTapDetector::NewL( this );
    iLongtapDetector->SetLongTapDelay( KLongTapDelay );
    iLongtapDetector->SetTimeDelayBeforeAnimation( KLongTapAnimationDelay );

    iFieldContainer = CMRFieldContainer::NewL( iFactory, *this );
    iFieldContainer->SetFieldContainerObserver( this );

    // Physics: Create physics
    // Give pointer to control that should be able to flick/drag
    iPhysics = CMRListPanePhysics::NewL( *this, *iFieldContainer, *this );
    }

// ---------------------------------------------------------------------------
// CMRListPane::InitializeL()
// ---------------------------------------------------------------------------
//
void CMRListPane::InitializeL()
    {
    FUNC_LOG;
    const TInt count( iFactory.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        iFactory.Field(i)->InitializeL();
        }
    iClickedField = NULL;
    }


// ---------------------------------------------------------------------------
// CMRListPane::InternalizeL()
// ---------------------------------------------------------------------------
//
void CMRListPane::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    iFactory.InternalizeL( aEntry );

    // This is called to make sure everything is drawn correctly
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRListPane::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CMRListPane::ExternalizeL(
        MESMRCalEntry& aEntry,
        TBool aForceValidation )
    {
    FUNC_LOG;
    CESMRField* currentFocus = FocusedField();
    ASSERT( currentFocus );
    TESMREntryFieldId id = currentFocus->FieldId();

    if ( aForceValidation )
        {
        // force validate the values:
        TInt err = iFactory.Validate( id, aForceValidation );
        // fill the calendar entry with data in fields.
        iFactory.ExternalizeL( aEntry );
        }
    else
        {
        TInt err = iFactory.Validate( id );
        if ( err )
            {
            //SetControlFocusedL( id ); Why would we set this focused here?
            User::Leave( err );
            }
        else
            {
            // fill the calendar entry with data in fields.
            iFactory.ExternalizeL( aEntry );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::DisableSizeChange()
// ---------------------------------------------------------------------------
//
void CMRListPane::DisableSizeChange(TBool aDisable )
    {
    FUNC_LOG;
    iDisableSizeChanged = aDisable;
    }

// ---------------------------------------------------------------------------
// CMRListPane::InitialScroll
// ---------------------------------------------------------------------------
//
void CMRListPane::InitialScrollL()
    {
    FUNC_LOG;

    // Check if the ResponseArea exist
    TESMREntryFieldId id = GetResponseFieldsFieldId();

    if( id == EESMRFieldResponseArea )
        {
        // Set the focus on the ResponseArea
        iFieldContainer->SetControlFocusedL( id );
        //iFactory.FieldById(id)->SetFocus( ETrue );

        // Scroll the list to put the ResponseArea on the top
        CESMRField* focusField = FocusedField();
        TPoint pos( focusField->Position() );
        ScrollFieldsUp(pos.iY);
        }
    }


// ---------------------------------------------------------------------------
// CMRListPane::FocusedItem
// ---------------------------------------------------------------------------
//
CESMRField* CMRListPane::FocusedField() const
    {
    FUNC_LOG;
    return iFieldContainer->FocusedField();
    }

// ---------------------------------------------------------------------------
// CMRListPane::SetControlFocusedL
// ---------------------------------------------------------------------------
//
void CMRListPane::SetControlFocusedL( TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    iFieldContainer->SetControlFocusedL( aFieldId );
    }

// ---------------------------------------------------------------------------
// CMRListPane::ClickedItem
// ---------------------------------------------------------------------------
//
CESMRField* CMRListPane::ClickedField() const
    {
    return iClickedField;
    }

// ---------------------------------------------------------------------------
// CMRListPane::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRListPane::CountComponentControls() const
    {
    FUNC_LOG;
    return 1; // iFieldContainer

    }

// ---------------------------------------------------------------------------
// CMRListPane::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRListPane::ComponentControl( TInt /*aIndex*/ ) const
    {
    FUNC_LOG;
    return iFieldContainer;
    }

// ---------------------------------------------------------------------------
// CMRListPane::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRListPane::SizeChanged()
    {
    FUNC_LOG;
    if ( iDisableSizeChanged || Rect() == TRect( 0, 0, 0, 0 ) )
        {
        return;
        }

    TSize containerSize( iFieldContainer->MinimumSize() );
    iFieldContainer->SetSize( containerSize );

    // Physics:
    iPhysics->InitPhysics();

    DoUpdateScrollBar();
    }

// ---------------------------------------------------------------------------
// CMRListPane::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRListPane::OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                          TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );

    // First check if the focused item needs the key event
    response = FocusedField()->OfferKeyEventL( aKeyEvent, aType );

    if ( aType == EEventKey
         && response == EKeyWasNotConsumed )
        {
        // Check if the focus should be changed
        switch ( aKeyEvent.iScanCode )
            {
            case EStdKeyUpArrow:
                {
                response = iFieldContainer->MoveFocusUpL( HiddenFocus() );

                // Focus changed via keyboard, iClickedItem is no
                // longer valid
                iClickedField = NULL;
                break;
                }
            case EStdKeyDownArrow:
                {
                response = iFieldContainer->MoveFocusDownL( HiddenFocus() );

                // Focus changed via keyboard, iClickedItem is no
                // longer valid
                iClickedField = NULL;
                break;
                }
            case EStdKeyEnter:
                {
                FocusedField()->ExecuteGenericCommandL( EAknCmdOpen );
                break;
                }
            default:
                {
                break;
                }
            }
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CMRListPane::HandleLongTapEventL
// ---------------------------------------------------------------------------
//
void CMRListPane::HandleLongTapEventL(
        const TPoint& aPenEventLocation,
        const TPoint& /* aPenEventScreenLocation */ )
    {
    FUNC_LOG;
    iLongTapEventInProgess = EFalse;
    // Long tap functionality may vary between fields
    // ==> Command field to execute action related to long tap
    TInt count( iFactory.Count() );

    for ( TInt i = 0; i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );

        if ( field->IsVisible()
             && field->Rect().Contains( aPenEventLocation ) )
            {
            field->LongtapDetectedL( aPenEventLocation );
            iLongTapEventInProgess = ETrue;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::DoUpdateScrollBar
// ---------------------------------------------------------------------------
//
void CMRListPane::DoUpdateScrollBar( TInt aFocusPosition )
    {
    FUNC_LOG;
    // Set this lispane's size as scroll bar's window size
    iScrollModel.SetWindowSize( iSize.iHeight );
    // Set fieldcontainer's height as scrolbar's scroll span
    iScrollModel.SetScrollSpan( iFieldContainer->MinimumSize().iHeight );

    // Update scrollbar focus position.
    if( aFocusPosition == KErrNotFound )
        {
        iScrollModel.SetFocusPosition( iPhysics->VerticalScrollIndex() );
        }
    else
        {
        iScrollModel.SetFocusPosition( aFocusPosition );
        }

    iScroll.SetModel( &iScrollModel );

    // finally update the new thumb position to view's
    // iScrollBarThumbPosition member.
    iScrollBarObserver.ScrollBarPositionChanged(
            iScroll.ThumbPosition() );
    }

// ---------------------------------------------------------------------------
// CMRListPane::UpdatedFocusPosition
// ---------------------------------------------------------------------------
//
TInt CMRListPane::UpdatedFocusPosition()
    {
    FUNC_LOG;
    TInt focusPos = 0;
    TInt count = iFactory.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );
        if ( field->IsVisible() )
            {
            TRect rect( field->Rect() );
            // Check if the field's top is above scroll area origo.
            if ( rect.iTl.iY  < 0 )
                {
                // Whole field is above origo
                if ( rect.iBr.iY < 0 )
                    {
                    focusPos += rect.Height();
                    }
                // Part of the field is above origo
                else
                    {
                    focusPos += Abs( rect.iTl.iY );
                    }
                }
            else
                {
                // Rest of the fields are below scroll area origo
                break;
                }
            }
        }

    return focusPos;
    }

// ---------------------------------------------------------------------------
// CMRListPane::ScrollFieldsUp
// ---------------------------------------------------------------------------
//
void CMRListPane::ScrollFieldsUp( TInt aPx )
    {
    FUNC_LOG;
    TPoint point = iFieldContainer->Position();
    point.iY -= aPx;

    // This initializes Draw also
    iFieldContainer->SetPosition( point );

    // Non-kinetic scrolling executed. Update
    // new position to physics.
    iPhysics->UpdateVerticalScrollIndex( UpdatedFocusPosition() );

    DoUpdateScrollBar( UpdatedFocusPosition() );
    }

// ---------------------------------------------------------------------------
// CMRListPane::ScrollFieldsDown
// ---------------------------------------------------------------------------
//
void CMRListPane::ScrollFieldsDown( TInt aPx )
    {
    FUNC_LOG;
    TPoint point = iFieldContainer->Position();
    point.iY += aPx;

    // This initializes Draw also
    iFieldContainer->SetPosition( point );
    // Non-kinetic scrolling executed. Update
    // new position to physics.
    iPhysics->UpdateVerticalScrollIndex( UpdatedFocusPosition() );

    DoUpdateScrollBar( UpdatedFocusPosition() );
    }


// ---------------------------------------------------------------------------
// CMRListPane::UpdateScrollBarAndPhysics
// ---------------------------------------------------------------------------
//
void CMRListPane::UpdateScrollBarAndPhysics()
    {
    // Update physics world size
    iPhysics->InitPhysics();

    // Update scrollbar
    DoUpdateScrollBar();
    }


// ---------------------------------------------------------------------------
// CMRListPane::ShowControl
// ---------------------------------------------------------------------------
//
void CMRListPane::ShowControl( TESMREntryFieldId aFieldId )
    {
    iFieldContainer->ShowControl( aFieldId );
    }

// ---------------------------------------------------------------------------
// CMRListPane::ShowControl
// ---------------------------------------------------------------------------
//
TBool CMRListPane::IsControlVisible( TESMREntryFieldId aFieldId )
    {
    return iFieldContainer->IsControlVisible( aFieldId );
    }
// ---------------------------------------------------------------------------
// CMRListPane::GetResponseFieldsFieldId
// ---------------------------------------------------------------------------
//
TESMREntryFieldId CMRListPane::GetResponseFieldsFieldId()
    {
    CESMRField* rfield = iFactory.FieldById( EESMRFieldResponseArea );

    if ( rfield && rfield->IsVisible() && !rfield->IsNonFocusing() )
        {
        return EESMRFieldResponseArea;
        }
    else
        {
        return iFactory.Field(0)->FieldId();
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::ReActivateL
// ---------------------------------------------------------------------------
//
void CMRListPane::ReActivateL()
    {
    FUNC_LOG;
    TInt count = iFactory.Count();

    for ( TInt i = 0; i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );

        if ( !field->IsFieldActivated() )
            {
            field->SetContainerWindowL( *iFieldContainer );
            field->SetListObserver( iFieldContainer );
            }
        }

    // This "for" circle can not be mixed with the above one, since the
    // field->ActivateL() will call some functions which will traverse
    // all the fields, but that time, not all the fields have set the
    // container window.
    for ( TInt i = 0; i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );
        if ( !field->IsFieldActivated() )
            {
            field->ActivateL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CMRListPane::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    // Check if touch is enabled or not
    if( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }

    // If new down event is received, and
    // iLongTapEventInProgess flag is still ETrue, we need to
    // set it back to EFalse -> Long tap event cannot be in progress
    // in this case anymore.
    if( aPointerEvent.iType == TPointerEvent::EButton1Down &&
    		iLongTapEventInProgess )
    	{
		iLongTapEventInProgess = EFalse;
    	}

    // Forward all listpane related events to physics api first.
    if ( iPhysics->HandlePointerEventL( aPointerEvent, iPhysicsActionOngoing ) )
        {
        DoUpdateScrollBar();
        // Physics in action. If long tap detection is active,
        // it should be cancelled.
        if( iLongtapDetector->IsActive() )
        	{
			iLongtapDetector->Cancel();
        	}
        }

    if( !iPhysicsActionOngoing )
    	{
		UpdateClickedField( aPointerEvent );

		// Offer pointer event to long tap detector if field supports long tap
		// functionality
		if( ClickedField()->SupportsLongTapFunctionalityL( aPointerEvent ) )
			{
			iLongtapDetector->PointerEventL( aPointerEvent );
			}

		SetFocusAfterPointerEventL( aPointerEvent );

        // If longtap event is in progress, do not pass events to coecontrol
        if( !iLongTapEventInProgess )
            {
            CCoeControl::HandlePointerEventL( aPointerEvent );
            }
        // Longtap event executed after up event ->
        // Let's set iLongTapEventInProgess to EFalse
        else if( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
			iLongTapEventInProgess = EFalse;
            }
    	}
    }

// ---------------------------------------------------------------------------
// CMRListPane::ActivateL
// ---------------------------------------------------------------------------
//
void CMRListPane::ActivateL()
    {
    FUNC_LOG;
    // This ActiveteL is required only for setting the initial position
    // of the field container. After setting the position, physics is
    // initialized with new values also.

    CCoeControl::ActivateL();
    iFieldContainer->SetPosition( Position() );

    // Physics:
    iPhysics->InitPhysics();
    }

// ---------------------------------------------------------------------------
// CMRListPane::PhysicsEmulationEnded
// ---------------------------------------------------------------------------
//
void CMRListPane::PhysicsEmulationEnded()
    {
    FUNC_LOG;
    DoUpdateScrollBar();
    iPhysicsActionOngoing = EFalse;
    Parent()->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRListPane::UpdateScrollBarDuringOngoingPhysics
// ---------------------------------------------------------------------------
//
void CMRListPane::UpdateScrollBarDuringOngoingPhysics()
    {
    FUNC_LOG;
    TInt verticalScrollIndex( iPhysics->VerticalScrollIndex() );
    iScrollModel.SetFocusPosition( verticalScrollIndex );

	iScroll.SetModel( &iScrollModel );

	// Update the new thumb position to view's
	// iScrollBarThumbPosition member.
	iScrollBarObserver.ScrollBarPositionChanged(
			iScroll.ThumbPosition() );

	iScroll.DrawDeferred();

	if( FeedbackScrollMarginExceeded(
			Abs( verticalScrollIndex - iPreviousVerticalScrollIndex ) ) )
		{
		HandleTactileFeedback( ETouchFeedbackSlider );

		iPreviousVerticalScrollIndex = verticalScrollIndex;
		}
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleTactileFeedback
// ---------------------------------------------------------------------------
//
void CMRListPane::HandleTactileFeedback(
		const TTouchLogicalFeedback& aType )
	{
	FUNC_LOG;

	if( !iTactileFeedback )
		{
		// Aquire tactile feedback pointer from TLS
		iTactileFeedback = MTouchFeedback::Instance();
		}

	if ( iTactileFeedback && iTactileFeedback->FeedbackEnabledForThisApp() )
		{
		iTactileFeedback->InstantFeedback( aType );
		}
    }

// ---------------------------------------------------------------------------
// CMRListPane::SetFocusAfterPointerEventL
// ---------------------------------------------------------------------------
//
void CMRListPane::SetFocusAfterPointerEventL(
        const TPointerEvent &aPointerEvent )
    {
    FUNC_LOG;

    if( aPointerEvent.iType == TPointerEvent::EButton1Down )
    	{
		TInt count( iFactory.Count() );
		for( TInt i = 0; i < count; ++i )
			{
			CESMRField* field = iFactory.Field( i );
			if ( field->IsVisible() &&
					field->Rect().Contains( aPointerEvent.iPosition ) )
				{
				CESMRField* focusedField = iFieldContainer->FocusedField();

				if ( field != focusedField )
					{
					TBool canLoseFocus(
							focusedField->OkToLoseFocusL( field->FieldId() ) );

					if ( canLoseFocus )
						{
						iFieldContainer->SetControlFocusedL( field->FieldId() );
						}
					}

				break;
				}
			}
    	}
    }

// ---------------------------------------------------------------------------
// CMRListPane::UpdateClickedField
// ---------------------------------------------------------------------------
//
void CMRListPane::UpdateClickedField( const TPointerEvent &aPointerEvent )
    {
    FUNC_LOG;
    TInt fieldCount( iFactory.Count() );

    for( TInt i = 0; i < fieldCount; ++i )
        {
        if( iFactory.Field( i )->Rect().Contains(
                aPointerEvent.iPosition ) )
            {
            if( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                iClickedField = iFactory.Field( i );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::HiddenFocus
// ---------------------------------------------------------------------------
//
TBool CMRListPane::HiddenFocus()
    {
    FUNC_LOG;
    TBool hiddenFocus( EFalse );

    CESMRField* focusedField( iFieldContainer->FocusedField() );
    TInt focusedFieldIndex( IndexByFieldId(
            iFactory, focusedField->FieldId() ) );

    if ( focusedFieldIndex < iFactory.Count() )
        {
        TRect focusedFieldRect( focusedField->Rect() );
        TRect listPaneRect( Rect() );

        TInt fieldTopY( focusedFieldRect.iTl.iY );
        TInt fieldBottomY( focusedFieldRect.iBr.iY );

        TInt listTopY( listPaneRect.iTl.iY );
        TInt listBottomY( listPaneRect.iBr.iY );

        if ( ( fieldBottomY > listBottomY ||
                fieldTopY < listTopY ) &&
                    focusedFieldRect.Height() < listPaneRect.Height() )
            {
            hiddenFocus = ETrue;
            }
        }
    return hiddenFocus;
    }

// ---------------------------------------------------------------------------
// CMRListPane::FeedbackScrollMarginExceeded
// ---------------------------------------------------------------------------
//
TBool CMRListPane::FeedbackScrollMarginExceeded( TInt aMargin )
    {
    FUNC_LOG;
	/*
	 * This compares given margin to default one row
	 * field height, and returns ETrue if margin is exceeded.
	 * Otherwise EFalse.
	 */
	TBool ret( EFalse );

	if( !iDefaultFieldHeight )
		{
		TAknLayoutRect fieldLayoutRect(
			NMRLayoutManager::GetFieldLayoutRect(
					iFieldContainer->Rect(), 1 ) );

		iDefaultFieldHeight = fieldLayoutRect.Rect().Height();
		}

	if( aMargin > iDefaultFieldHeight )
		{
		ret = ETrue;
		}

    return ret;
    }

// End of file
