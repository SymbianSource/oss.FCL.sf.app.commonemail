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
* Description:  ESMR policy implementation
*
*/

#include "cesmrfield.h"

#include "esmrcommands.h"
#include "nmrlayoutmanager.h"
#include "mesmrfieldeventqueue.h"
#include "cesmrfieldcommandevent.h"
#include "cmrbackground.h"
#include "esmrcommands.h"
#include "cmrrecordinggc.h"

#include "emailtrace.h"

#include <txtrich.h>
#include <eikenv.h>
#include <baclipb.h>
#include <eikbtgpc.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <touchfeedback.h>

namespace
{
// Off-screen x coordinate for fields
const TInt KOffScreenPositionX = 1000;
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRField::CESMRField
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRField::CESMRField()
    : iCustomMsk( EFalse ),
      iDisableRedraw( EFalse ),
      iDefaultMskVisible( EFalse ),
      iMskVisible( EFalse ),
      iLocked( EFalse )
    {
    FUNC_LOG;
    // do nothing
    }


// ---------------------------------------------------------------------------
// CESMRField::~CESMRField
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRField::~CESMRField()
    {
    FUNC_LOG;
    delete iExtControl;
    if ( iEventQueue )
        {
        iEventQueue->RemoveObserver( this );
        }
    delete iBackground;
    delete iRecordingGc;
    }

// ---------------------------------------------------------------------------
// CESMRField::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ConstructL(
        CCoeControl* aControl )
    {
    FUNC_LOG;
    iExtControl = aControl;

    iBackground = CMRBackground::NewL();
    this->SetBackground( iBackground );
    iRecordingGc = new( ELeave ) CMRRecordingGc( SystemGc() );
    }

// ---------------------------------------------------------------------------
// CESMRField::InitializeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::InitializeL()
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::FontChangedL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::FontChangedL()
    {
    FUNC_LOG;
    // This method re-initializes field
    InitializeL();
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFieldId
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFieldId( TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    iFieldId = aFieldId;
    }


// ---------------------------------------------------------------------------
// CESMRField::SetPreItemIndex
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetPreItemIndex( TInt aPreItemIndex )
    {
    FUNC_LOG;
    iPreItemIndex = aPreItemIndex;
    }


// ---------------------------------------------------------------------------
// CESMRField::SetCurrentItemIndex
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetCurrentItemIndex( TInt aCurrentItemIndex )
    {
    FUNC_LOG;
    iCurrentItemIndex = aCurrentItemIndex;
    }

// ---------------------------------------------------------------------------
// CESMRField::FieldId
// ---------------------------------------------------------------------------
//
EXPORT_C TESMREntryFieldId CESMRField::FieldId() const
    {
    FUNC_LOG;
    return iFieldId;
    }

// ---------------------------------------------------------------------------
// CESMRField::PreItemIndex
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRField::PreItemIndex() const
    {
    FUNC_LOG;
    return iPreItemIndex;
    }

// ---------------------------------------------------------------------------
// CESMRField::CurrentItemIndex
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRField::CurrentItemIndex() const
    {
    FUNC_LOG;
    return iCurrentItemIndex;
    }

// ---------------------------------------------------------------------------
// CESMRField::SizeChanged
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );
    if ( iExtControl )
        {
        iExtControl->SetRect( rect );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::CountComponentControls
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRField::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iExtControl )
    	{
        ++count;
    	}

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRField::ComponentControl
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CESMRField::ComponentControl( TInt /*aInd*/ ) const
    {
    FUNC_LOG;
    return iExtControl;
    }

// ---------------------------------------------------------------------------
// CESMRField::MinimumSize
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CESMRField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );
    TAknLayoutRect listareaLayoutRect =
        NMRLayoutManager::GetLayoutRect( parentRect, NMRLayoutManager::EMRLayoutListArea );
    TAknLayoutRect fieldLayoutRect =
        NMRLayoutManager::GetFieldLayoutRect( listareaLayoutRect.Rect(), 1 );
    return TSize( Parent()->Size().iWidth, fieldLayoutRect.Rect().Height() );
    }

// ---------------------------------------------------------------------------
// CESMRField::Draw
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;

    // Draw only if field is not on screen
    if ( Position().iX != Parent()->Position().iX )
        {
        // Calculate field visible area. Intersection with list pane
        TRect clippingRect( aRect );
        clippingRect.Intersection( Parent()->Parent()->Rect() );

        // Flush cached drawing commands from custom graphics context
        iRecordingGc->FlushBuffer( aRect, clippingRect );
        }

    }

// ---------------------------------------------------------------------------
// CESMRField::HandlePointerEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::HandlePointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;
    // Sanity check that pointer event occurs in field area
    if ( Rect().Contains( aPointerEvent.iPosition ) )
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            if ( !HandleSingletapEventL( aPointerEvent.iPosition ) )
                {
                // Provide raw pointer event to field
                HandleRawPointerEventL( aPointerEvent );
                }
            }
        else // Provide other pointer events to fields
            {
            if ( !HandleRawPointerEventL( aPointerEvent ) )
                {
                // Provide pointer events to child components if field
                // did not already consume the event
                CCoeControl::HandlePointerEventL( aPointerEvent );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleResourceChange
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;

    CCoeControl::HandleResourceChange( aType );

    switch ( aType )
        {
        case KAknsMessageSkinChange:
        case KEikMessageColorSchemeChange:
        case KEikDynamicLayoutVariantSwitch:
        case KAknLocalZoomLayoutSwitch:
            {
            iRecorded = EFalse;
            break;
            }
        default:
            break;
        }
    }

/// ---------------------------------------------------------------------------
// CESMRField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetContainerWindowL(const CCoeControl& aContainer)
    {
    FUNC_LOG;

    // Use container window from aContainer
    // This will set also aContainer as MOP parent and CCoeControl parent
    CCoeControl::SetContainerWindowL( aContainer );

    // Set this same window and this as parent for component controls
    TInt count( CountComponentControls() );

    for ( TInt i = 0; i < count; ++i )
        {
        ComponentControl( i )->SetContainerWindowL( *this );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::SetListObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetListObserver( MESMRListObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    ListObserverSet();
    }

// ---------------------------------------------------------------------------
// CESMRField::ListObserverSet
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ListObserverSet()
    {
    FUNC_LOG;
    // default implementation.
    }

// ---------------------------------------------------------------------------
// CESMRField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CESMRField::OfferKeyEventL(
		const TKeyEvent& aEvent, TEventCode aType )
    {
    FUNC_LOG;

    TKeyResponse response( EKeyWasNotConsumed );

    if ( iExtControl )
        {
        response = iExtControl->OfferKeyEventL( aEvent, aType );
        }

    // If key event was not consumed, and it is the Enter,
    // let's execute generic command Open
    if ( response == EKeyWasNotConsumed
         && aType == EEventKey
         && ( aEvent.iCode == EKeyEnter
              || aEvent.iScanCode == EStdKeyEnter ) )
        {
        ExecuteGenericCommandL( EAknCmdOpen );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    iOutlineFocus = aFocus;

    if ( iExtControl )
        {
        iExtControl->SetFocus( aFocus );
        }
    if ( aFocus )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if ( cba )
            {
            iDefaultMskVisible = cba->IsCommandVisibleByPosition(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition );
            iMskVisible = iDefaultMskVisible;
            }
        }
    else
        {
        RestoreMiddleSoftKeyL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::OkToLoseFocusL( // codescanner::LFunctionCantLeave
        TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRField::GetMinimumVisibleVerticalArea
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::GetMinimumVisibleVerticalArea(
		TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    aUpper = 0;
    aLower = Rect().iBr.iY - Rect().iTl.iY;
    }

// ---------------------------------------------------------------------------
// CESMRField::GetCursorLineVerticalPos
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    aUpper = 0;
    aLower = Rect().iBr.iY - Rect().iTl.iY;
    }

// ---------------------------------------------------------------------------
// CESMRField::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::InternalizeL( // codescanner::LFunctionCantLeave
        MESMRCalEntry& /*aEntry*/ )
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    iRecorded = EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRField::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ExternalizeL( // codescanner::LFunctionCantLeave
        MESMRCalEntry& /*aEntry*/ )
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::ExecuteGenericCommandL( TInt /*aCommand*/ )
    {
    FUNC_LOG;
    return EFalse;
    /* Subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::LongtapDetectedL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::LongtapDetectedL( const TPoint& aPosition )
    {
    FUNC_LOG;

    HandleLongtapEventL( aPosition );
    }

// ---------------------------------------------------------------------------
// CESMRField::SetTitlePaneObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetTitlePaneObserver(
		MESMRTitlePaneObserver* /*aObserver*/ )
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::ChangeMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ChangeMiddleSoftKeyL(
        TInt aCommandId,
        TInt aResourceId )
    {
    FUNC_LOG;

    if ( AknLayoutUtils::MSKEnabled() )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if ( cba )
            {
            HBufC* middleSKText = StringLoader::LoadLC( aResourceId,
                                                        iCoeEnv );
            cba->SetCommandL(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    aCommandId, *middleSKText );
            CleanupStack::PopAndDestroy( middleSKText );
            if ( !iMskVisible )
                {
                cba->MakeCommandVisibleByPosition(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                        ETrue );
                iMskVisible = ETrue;
                }
            cba->DrawDeferred();
            iCustomMsk = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::SetValidatorL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetValidatorL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;

    iValidator = aValidator;
    }

// ---------------------------------------------------------------------------
// CESMRField::IsFieldActivated
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::IsFieldActivated() const
    {
    FUNC_LOG;

    return IsActivated();
    }

// ---------------------------------------------------------------------------
// CESMRField::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::DynInitMenuPaneL(
        TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRField::ChangeMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ChangeMiddleSoftKeyL( TInt aResourceId )
    {
    FUNC_LOG;

    if ( AknLayoutUtils::MSKEnabled() )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if ( cba )
            {
            cba->SetCommandL(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    aResourceId );
            if ( !iMskVisible )
                {
                cba->MakeCommandVisibleByPosition(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                        ETrue );
                iMskVisible = ETrue;
                }
            cba->DrawDeferred();
            iCustomMsk = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::SetEventQueueL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetEventQueueL( MESMRFieldEventQueue* aEventQueue )
    {
    FUNC_LOG;
    if ( aEventQueue != iEventQueue )
        {
        if ( aEventQueue )
            {
            // Add self to new queue
            aEventQueue->AddObserverL( this );
            }

        if ( iEventQueue )
            {
            // Remove self from old queue
            iEventQueue->RemoveObserver( this );
            }

        iEventQueue = aEventQueue;
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::HasOutlineFocus
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::HasOutlineFocus() const
    {
    FUNC_LOG;
    return iOutlineFocus;
    }

// ---------------------------------------------------------------------------
// CESMRField::GetFocusRect
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CESMRField::GetFocusRect() const
    {
    FUNC_LOG;
    return iFocusRect;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFocusRect
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFocusRect( const TRect& aFocusRect )
    {
    FUNC_LOG;
    iFocusRect = aFocusRect;
    }

// ---------------------------------------------------------------------------
// CESMRField::GetFocusType
// ---------------------------------------------------------------------------
//
EXPORT_C TESMRFieldFocusType CESMRField::GetFocusType() const
    {
    FUNC_LOG;
    return iFocusType;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFocusType
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFocusType( TESMRFieldFocusType aFocusType )
    {
    FUNC_LOG;
    iFocusType = aFocusType;
    }

// ---------------------------------------------------------------------------
// CESMRField::RecordEnabled
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::RecordEnabled()
    {
    FUNC_LOG;

    TBool enabled( ETrue );

    if ( iFieldMode == EESMRFieldModeView )
        {
        enabled = !iRecorded;
        }

    return enabled;
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    if ( iEventQueue )
        {
        iEventQueue->NotifyEventL( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventL( TInt aCommand )
    {
    FUNC_LOG;
    CESMRFieldCommandEvent* event = CESMRFieldCommandEvent::NewLC( this,
                                                                   aCommand );
    NotifyEventL( *event );
    CleanupStack::PopAndDestroy( event );
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventAsyncL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventAsyncL( MESMRFieldEvent* aEvent )
    {
    FUNC_LOG;
    if ( iEventQueue )
        {
        iEventQueue->NotifyEventAsyncL( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventAsyncL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventAsyncL( TInt aCommand )
    {
    FUNC_LOG;
    CESMRFieldCommandEvent* event = CESMRFieldCommandEvent::NewLC( this,
                                                                   aCommand );
    NotifyEventAsyncL( event );
    CleanupStack::Pop( event );
    }

// ---------------------------------------------------------------------------
// CESMRField::RestoreMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::RestoreMiddleSoftKeyL()
    {
    FUNC_LOG;
    if ( AknLayoutUtils::MSKEnabled() )
        {
        if ( iMskVisible != iDefaultMskVisible )
            {
            SetMiddleSoftKeyVisible( iDefaultMskVisible );
            }

        if ( iCustomMsk )
            {
            NotifyEventL( EESMRCmdRestoreMiddleSoftKey );
            iCustomMsk = EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::SetMiddleSoftKeyVisible
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetMiddleSoftKeyVisible( TBool aVisible )
    {
    FUNC_LOG;
    if ( AknLayoutUtils::MSKEnabled() )
        {
        if ( iMskVisible != aVisible )
            {
            CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
            if ( cba )
                {
                cba->MakeCommandVisibleByPosition(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                        aVisible );
                iMskVisible = aVisible; // Visibility has been changed
                }
            }

        iCustomMsk = ETrue; // Field has modified editor default MSK
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::HandleLongtapEventL( const TPoint& /*aPosition*/ )
    {
    FUNC_LOG;
    // Default action for long tap event.
    ExecuteGenericCommandL( EESMRCmdLongtapDetected );
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleSingletapEventL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::HandleSingletapEventL( const TPoint& /*aPosition*/ )
    {
    FUNC_LOG;
    // Subclasses may override for field specific actions
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleRawPointerEventL
// Default implementation for pointer event handling in field
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::HandleRawPointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;

    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
		// Default action for touch release
		if ( !ExecuteGenericCommandL( EAknCmdOpen ) )
			{
			// If the base class implementation does not use the command
			// then the pointer event is propagated to children.
			CCoeControl::HandlePointerEventL( aPointerEvent );
			}
        }
    else
        {
        // Propagate the pointer event to child components
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleFieldEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::HandleFieldEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    if ( aEvent.Type() == MESMRFieldEvent::EESMRFieldCommandEvent )
        {
        TInt* command = static_cast<TInt*>( aEvent.Param( 0 ) );
        if ( EESMRCmdSizeExceeded != *command )
        	{
            ExecuteGenericCommandL( *command );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::EventObserver
// ---------------------------------------------------------------------------
//
EXPORT_C MESMRFieldEventObserver* CESMRField::EventObserver() const
    {
    FUNC_LOG;
    return const_cast< CESMRField* >( this );
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFieldMode
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFieldMode( TESMRFieldMode aMode )
    {
    FUNC_LOG;
    iFieldMode = aMode;
    }

// ---------------------------------------------------------------------------
// CESMRField::FieldMode
// ---------------------------------------------------------------------------
//
EXPORT_C TESMRFieldMode CESMRField::FieldMode() const
    {
    FUNC_LOG;
    return iFieldMode;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFieldViewMode
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFieldViewMode( TESMRFieldType aViewMode )
    {
    FUNC_LOG;
    iFieldViewMode = aViewMode;
    }

// ---------------------------------------------------------------------------
// CESMRField::FieldViewMode
// ---------------------------------------------------------------------------
//
EXPORT_C TESMRFieldType CESMRField::FieldViewMode() const
    {
    FUNC_LOG;
    return iFieldViewMode;
    }

// ---------------------------------------------------------------------------
// CESMRField::UpdateExtControlL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::UpdateExtControlL(
             CCoeControl* aControl )
    {
    FUNC_LOG;
    delete iExtControl;
    iExtControl = aControl;

    // Set also container window
    iExtControl->SetContainerWindowL( *this );
    }

// ---------------------------------------------------------------------------
// CESMRField::Lock
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::LockL()
	{
    FUNC_LOG;
	iLocked = ETrue;
	}

// ---------------------------------------------------------------------------
// CESMRField::IsLocked
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::IsLocked()
	{
    FUNC_LOG;
	return iLocked;
	}

// ---------------------------------------------------------------------------
// CESMRField::SupportsLongTapFunctionalityL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::SupportsLongTapFunctionalityL(
		const TPointerEvent& /*aPointerEvent*/ )
	{
    FUNC_LOG;
    // Subclasses may override for field specific actions
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CESMRField::MoveToScreen
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::MoveToScreen( TBool aVisible )
    {
    FUNC_LOG

    // Check that field is activated
    if ( IsActivated() )
        {
        TPoint pos( iPosition );

        if ( aVisible )
            {
            // Set x coordinate to parent (field container) x coordinate
            pos.iX = Parent()->Position().iX;
            }
        else
            {
            // Move field outside screen

            pos.iX = KOffScreenPositionX;

            if ( pos != iPosition )
                {
                // Record field drawing commands
                RecordField();
                }
            }

        if ( pos != iPosition )
            {
            // Set new position only if it different from current one
            // Setting new position potentially causes relayout in field
            SetPosition( pos );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleTactileFeedbackL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::HandleTactileFeedbackL()
    {
    FUNC_LOG;

    AquireTactileFeedback();

    if ( iTactileFeedback && iTactileFeedback->FeedbackEnabledForThisApp() )
        {
        iTactileFeedback->InstantFeedback( ETouchFeedbackBasic );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::AquireTactileFeedback
// ---------------------------------------------------------------------------
//
void CESMRField::AquireTactileFeedback()
    {
	if( !iTactileFeedback )
		{
		// Aquire tactile feedback pointer from TLS
		iTactileFeedback = MTouchFeedback::Instance();
		}
    }

// ---------------------------------------------------------------------------
// CESMRField::RecordField
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::RecordField()
    {
    FUNC_LOG;

    // Record only activated field
    if ( IsActivated() && RecordEnabled() )
        {
        iRecorded = ETrue;

        // Purge old draw commands
        iRecordingGc->PurgeBuffer();

        // Set recording graphics context to be used for drawing
        SetCustomGc( iRecordingGc );

        // Set gc origin to point (-x,-y)
        // This will cause drawing commands to be relative to point (0,0)
        iRecordingGc->SetOrigin( -iPosition );

        // Draw custom background if available
        const MCoeControlBackground* bg = Background();
        if ( bg )
            {
            bg->Draw( *iRecordingGc, *this, Rect() );
            }

        // Draw child controls
        DrawControl( this );

        // Reset custom gc
        SetCustomGc( NULL );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::DrawControl
// ---------------------------------------------------------------------------
//
void CESMRField::DrawControl( CCoeControl* aControl ) const
    {
    TInt count( aControl->CountComponentControls() );

    for ( TInt i = 0; i < count; ++i )
        {
        CCoeControl* control = aControl->ComponentControl( i );

        // Draw only non-window owning children
        if ( !control->OwnsWindow() && control->IsVisible() )
            {
            TRect rect( control->Rect() );

            // Draw control background if available
            const MCoeControlBackground* bg = control->Background();
            if ( bg )
                {
                bg->Draw( *iRecordingGc, *control, rect );
                }

            // Draw control foreground
            control->DrawForeground( rect );

            // Draw child components
            DrawControl( control );
            }
        }
    }

// EOF

