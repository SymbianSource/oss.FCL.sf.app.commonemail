/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "emailtrace.h"
#include <caltime.h>
#include <calentry.h>
#include <layoutmetadata.cdl.h>
#include <e32math.h>
#include <aknphysics.h>

#include "cesmrlistcomponent.h"
#include "cesmrlayoutmgr.h"
#include "nmrlayoutmanager.h"
#include "mesmrfieldstorage.h"
#include "mesmrcalentry.h"
#include "cesmrfield.h"
#include "cmrlistpane.h"
// <cmail> Removed profiling. </cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRListComponent::NewL()
// ---------------------------------------------------------------------------
//
CESMRListComponent* CESMRListComponent::NewL( const CCoeControl* aParent,
                                             MESMRFieldStorage* aStorage,
                                             CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    CESMRListComponent* self = new (ELeave) CESMRListComponent( aStorage,
                                                                aLayout );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::CESMRListComponent()
// ---------------------------------------------------------------------------
//
CESMRListComponent::CESMRListComponent( MESMRFieldStorage* aFactory,
                                        CESMRLayoutManager* aLayout ) :
    iFactory( aFactory ),
    iLayout( aLayout )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::~CESMRListComponent
// ---------------------------------------------------------------------------
//
CESMRListComponent::~CESMRListComponent()
    {
    FUNC_LOG;
    delete iFactory;
    delete iScroll;
    delete iContainer;
    
    delete iPhysics;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::ConstructL( const CCoeControl* aParent )
    {
    FUNC_LOG;
    CreateWindowL( aParent );

    iScroll = new (ELeave) CAknDoubleSpanScrollBar( this );
    iScroll->ConstructL( EFalse, this, this, CEikScrollBar::EVertical, 0 );
    iScroll->SetModel( &iScrollModel );
    iScroll->MakeVisible( ETrue );
    
    const TInt count(iFactory->Count());
    for ( TInt i = 0; i < count; i++ )
        {
        CESMRField* field = iFactory->Field(i);
        
        field->SetListObserver( this );
        field->SetLayoutManager( iLayout );
        }
    
    iContainer = CMRListPane::NewL( *this, *iFactory, iScrollModel );
    iPrevThumbPos = iScroll->ThumbPosition();

    if ( CAknPhysics::FeatureEnabled() )
        {
        iPhysics = CAknPhysics::NewL( *this, iContainer );
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::InitializeL()
    {
    FUNC_LOG;
    const TInt count(iFactory->Count());
    for ( TInt i = 0; i < count; i++ )
        {
        iFactory->Field(i)->InitializeL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    iFactory->InternalizeL( aEntry );
    // This is called to make sure everything is drawn correctly
    DrawDeferred();
    }

void CESMRListComponent::DisableSizeChange(TBool aDisable )
    {
    FUNC_LOG;
    iDisableSizeChanged = aDisable;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::ExternalizeL( 
		MESMRCalEntry& aEntry, 
		TBool aForceValidation )
    {
    FUNC_LOG;
    CESMRField* currentFocus = FocusedItem();
    ASSERT( currentFocus );
    TESMREntryFieldId id = currentFocus->FieldId();

    if ( aForceValidation )
        {
        // force validate the values:
        TInt err = iFactory->Validate( id, aForceValidation );
        // fill the calendar entry with data in fields.
        iFactory->ExternalizeL( aEntry );
        }
    else
        {
        TInt err = iFactory->Validate( id );
        if ( err )
            {
            SetControlFocusedL( id );
            User::Leave( err );
            }
        else
            {
            // fill the calendar entry with data in fields.
            iFactory->ExternalizeL( aEntry );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRListComponent::CountComponentControls() const
    {
    FUNC_LOG;
    return 2;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::HandleFontChange()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::HandleFontChange()
    {
    FUNC_LOG;
    // Disable SizeChanged handling until all fonts are updated
    iDisableSizeChanged = ETrue;
    const TInt count(iFactory->Count());
    for ( TInt i = 0; i < count; i++ )
        {
        // Updates Font depending on the type of a field
        TRAP_IGNORE( iFactory->Field(i)->FontChangedL() );
        }
    // Enable and call SizeChanged
    iDisableSizeChanged = EFalse;
    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRListComponent::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    CCoeControl* control = NULL;
    switch ( aInd )
        {
        case 0:
            control = iContainer;
            break;
        case 1:
            control = iScroll;
            break;
        default:
            break;
        }
    return control;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRListComponent::SizeChanged()
    {
    FUNC_LOG;
    if ( iDisableSizeChanged || Rect() == TRect( 0, 0, 0, 0 ) )
        {
        return;
        }

    TRect containerRect( Rect() );
    
    TAknLayoutRect listareaLayoutRect =
        NMRLayoutManager::GetLayoutRect( containerRect, NMRLayoutManager::EMRLayoutListArea );
    TRect listareaRect( listareaLayoutRect.Rect() );
    iContainer->SetRect( listareaRect );
    
    TAknWindowComponentLayout scrollLayout =
        NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutScrollBar );
    AknLayoutUtils::LayoutControl( iScroll, containerRect, scrollLayout );

    ScrollItemVisible( KErrNotFound ); // Update scroll position

    // physics needs to be initialized before screensaver gets activated    
    TRAP_IGNORE( UpdatePhysicsL() );
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRListComponent::OfferKeyEventL(
		const TKeyEvent &aEvent, 
		TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response = iContainer->OfferKeyEventL( aEvent, aType );
    
    if ( EKeyWasConsumed == response )
        {
        iScroll->SetModel( &iScrollModel );
        }
    
    TInt thumbPos = iScroll->ThumbPosition();
    iPrevThumbPos = thumbPos;

    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::HandleScrollEventL
// ---------------------------------------------------------------------------
//
void CESMRListComponent::HandleScrollEventL(
    CEikScrollBar* aScrollBar,
    TEikScrollEvent /*aEventType*/ )
    {
    FUNC_LOG;
    if ( aScrollBar == iScroll )
        {
        TInt thumbPos = aScrollBar->ThumbPosition();
        TInt delta = thumbPos - iPrevThumbPos;
        if ( delta != 0 )
            {
            iPrevThumbPos = thumbPos;
            iContainer->ScrollView( delta );
            }
        }
    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::UpdatePhysicsL
// ---------------------------------------------------------------------------
//
void CESMRListComponent::UpdatePhysicsL()
    {
    FUNC_LOG;
    if ( iPhysics )
        {
        const TSize viewSize( Size() );
        TInt wSize = iScrollModel.ScrollSpan();
        const TSize worldSize( viewSize.iWidth, wSize );


        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        iPhysics->InitPhysicsL( worldSize, viewSize, EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ViewPositionChanged
// ---------------------------------------------------------------------------
//
void CESMRListComponent::ViewPositionChanged(
        const TPoint& aNewPosition,
        TBool /*aDrawNow*/,
        TUint /*aFlags*/ )
    {
    // Sometimes ViewPositionChanged events come after view is already changed,
    // so adding a safety check here
    if( IsVisible() )
        {
        TPoint centerPos = iContainer->GetViewCenterPosition();
        TInt scrollOffset = aNewPosition.iY - centerPos.iY;

        iContainer->ScrollView( scrollOffset );
        iPrevThumbPos = iScroll->ThumbPosition();
        // updates scrollbar thumb position and does redrawing
        iScroll->SetModel( &iScrollModel );
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::PhysicEmulationEnded
// ---------------------------------------------------------------------------
//
void CESMRListComponent::PhysicEmulationEnded()
    {
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ViewPosition
// ---------------------------------------------------------------------------
//
TPoint CESMRListComponent::ViewPosition() const
    {
    TPoint viewPos = iContainer->GetViewCenterPosition();
    return viewPos;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ScrollItemVisible()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::ScrollItemVisible( TInt aInd/*, TBool*/ /*aSizeChanged*/ )
    {
    FUNC_LOG;
    iContainer->ScrollItemVisible( aInd );
    UpdateScrollBar();
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::UpdateScrollBar()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::UpdateScrollBar()
    {
    FUNC_LOG;
    iContainer->UpdateFocusPosition();
    iScroll->SetModel( &iScrollModel );
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ListObserver
// ---------------------------------------------------------------------------
//
MESMRListObserver& CESMRListComponent::ListObserver()
    {
    FUNC_LOG;
    return *this;
    }

CESMRField* CESMRListComponent::FocusedItem() const
    {
    FUNC_LOG;
    return iContainer->FocusedItem();
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::IsControlVisible()
// ---------------------------------------------------------------------------
//
TBool CESMRListComponent::IsControlVisible( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    TBool ret( EFalse );
    
    CESMRField* field = iFactory->FieldById( aField );
    if ( field && field->IsVisible() )
        {
        ret = ETrue;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ControlSizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::ControlSizeChanged( CESMRField* aField )
    {
    FUNC_LOG;
    /* Called whenever one of the controls size changed
     * E.G. when a eik edwin expands */
    iContainer->ControlSizeChanged( aField );
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::InsertControl()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::InsertControl( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    iContainer->InsertControl( aField );
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::RemoveControl()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::RemoveControl( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    iContainer->RemoveControl( aField );
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::SetControlFocused()
// ---------------------------------------------------------------------------
//
void CESMRListComponent::SetControlFocusedL( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    iContainer->SetControlFocusedL( aField );
    UpdateScrollBar();
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::MoveListAreaDownL
// ---------------------------------------------------------------------------
//
void CESMRListComponent::MoveListAreaDownL( TInt aAmount )
    {
    FUNC_LOG;
    iContainer->MoveListAreaDownL( aAmount );
    
    // Do the view scrolling if needed:
    if ( aAmount )
        {
        UpdateScrollBar();
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::MoveListAreaUpL
// ---------------------------------------------------------------------------
//
void CESMRListComponent::MoveListAreaUpL( TInt aAmount )
    {
    FUNC_LOG;
    iContainer->MoveListAreaUpL( aAmount );
    
    // do the scrolling if needed:
    if ( aAmount > 0 )
        {
        UpdateScrollBar();
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::ListHeight()
// ---------------------------------------------------------------------------
//
TInt CESMRListComponent::ListHeight()
    {
    FUNC_LOG;
    return iContainer->Size().iHeight;
    }

// ---------------------------------------------------------------------------
// CESMRListComponent::IsFieldBottomVisible()
// ---------------------------------------------------------------------------
//
 TBool CESMRListComponent::IsFieldBottomVisible()
    {
    FUNC_LOG;
    return iContainer->IsFieldBottomVisible();
    }

 // -----------------------------------------------------------------------------
 // CESMRListComponent::HandlePointerEventL
 // -----------------------------------------------------------------------------
 //
 void CESMRListComponent::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;


    CCoeControl::HandlePointerEventL(aPointerEvent);
    TPoint position = aPointerEvent.iPosition;

    switch ( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            iPreviousPosition = iOriginalPosition = aPointerEvent.iPosition;

            iIsScrolling = iScroll->Rect().Contains(aPointerEvent.iPosition );
            iCurrentScrollSpan = iScrollModel.ScrollSpan();


            iIsDragging = EFalse;
            if( iPhysics )
                {
                iStartTime.HomeTime();
                UpdatePhysicsL();
                }
            break;

        case TPointerEvent::EDrag:
            if (!iIsScrolling)
                {
                // if when dragging scrollbar pointer touches container rectangle, 
                // do not activate physics!
                iIsDragging = ETrue;
            
                if( iPhysics )
                    {
                    TPoint delta( 0, iPreviousPosition.iY - position.iY );
                    iPhysics->RegisterPanningPosition( delta );
                    }
                }
            // Save current position as previous pos for future calculations
            iPreviousPosition = position;
            break;
         
        case TPointerEvent::EButton1Up:
            if( iIsDragging && iPhysics )
                {
                TPoint drag(iOriginalPosition - aPointerEvent.iPosition);
                iPhysics->StartPhysics(drag, iStartTime);
                }
                
        // some fields may change their size due to as result of click, update model if needed
        if (iCurrentScrollSpan != iScrollModel.ScrollSpan())
            {
            iScroll->SetModel( &iScrollModel );
            }
            break;
        }
     }
     
     
 // -----------------------------------------------------------------------------
 // CESMRListComponent::MakeVisible
 // -----------------------------------------------------------------------------
 //
void CESMRListComponent::MakeVisible( TBool  aIsVisible )
    {
    if(!aIsVisible && IsVisible() )
        {
        // hiding the control, stop physics for performance gain
        if( iPhysics )
            {
            iPhysics->StopPhysics();
            iPhysics->ResetFriction();
            }
        }
    }
// End of file

