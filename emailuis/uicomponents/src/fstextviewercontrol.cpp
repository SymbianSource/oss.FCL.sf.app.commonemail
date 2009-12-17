/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CFsTextViewerControl
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfevent.h>
#include <alf/alfstatic.h>
#include <alf/alfenv.h>
// </cmail>

// <cmail> Needed for pointer events. "Base class modifications for using touch"
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
#include <aknphysics.h>
// </cmail>

#include "fstextviewercontrol.h"
#include "fstextviewervisualizer.h"
#include "fstextviewervisualizerdata.h"
#include "fstextviewerobserver.h"
#include "fstextviewerkeys.h"

// Dragging treshold in pixels, so the amount of pixels that the pointer
// poisition can move without indicating it as dragging
const TInt KDraggingTresholdInPixels = 20;

// To be removed after aknphysics is working
#define FS_VIEWER_DISABLE_AKN_PHYSICS

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerControl* CFsTextViewerControl::NewL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CFsTextViewerControl* self = new ( ELeave ) CFsTextViewerControl();
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFsTextViewerControl
// ---------------------------------------------------------------------------
CFsTextViewerControl::CFsTextViewerControl( )
    : CAlfControl(), 
      iCursorUDScroll( EFalse ),
      iScrollEventHandlingOngoing( EFalse ),
      iIsVisible( EFalse ),
      iStartHotspotAction( EFalse )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);

#ifndef FS_VIEWER_DISABLE_AKN_PHYSICS
    if ( CAknPhysics::FeatureEnabled() )
        {
        CCoeControl* ctrl = NULL;
        //ctrl = CCoeEnv::Static()->AppUi()->TopFocusedControl();
        
        iPhysics = CAknPhysics::NewL( *this, ctrl );
        }
#endif //FS_VIEWER_DISABLE_AKN_PHYSICS
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerControl
// ---------------------------------------------------------------------------
CFsTextViewerControl::~CFsTextViewerControl()
    {
    FUNC_LOG;
	
    delete iPhysics;
    iPhysics = NULL;
    }

// ---------------------------------------------------------------------------
// SetVisualizer
// ---------------------------------------------------------------------------
void CFsTextViewerControl::SetVisualizer( CFsTextViewerVisualizer* aVis )
    {
    FUNC_LOG;
    iVisualizer = aVis;
    }

// ---------------------------------------------------------------------------
// SetObserver
// ---------------------------------------------------------------------------
void CFsTextViewerControl::SetObserver( MFsTextViewerObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// EnableCursorScroll
// ---------------------------------------------------------------------------
void CFsTextViewerControl::EnableCursorScroll( TBool aStatus )
    {
    FUNC_LOG;
    iCursorUDScroll = aStatus;
    }

// ---------------------------------------------------------------------------
// CursorScroll
// ---------------------------------------------------------------------------
TBool CFsTextViewerControl::CursorScroll()
    {
    FUNC_LOG;
    return iCursorUDScroll;
    }

// ---------------------------------------------------------------------------
// SetKeys
// ---------------------------------------------------------------------------
void CFsTextViewerControl::SetKeys( CFsTextViewerKeys* aKeys )
    {
    FUNC_LOG;
    iKeys = aKeys;
    }

// ---------------------------------------------------------------------------
// Keys
// ---------------------------------------------------------------------------
CFsTextViewerKeys* CFsTextViewerControl::Keys()
    {
    FUNC_LOG;
    return iKeys;
    }

TBool CFsTextViewerControl::IsScrollEventHandlingOngoing()
    {
    return iScrollEventHandlingOngoing;
    }

void CFsTextViewerControl::NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& aDisplay )
    {
    iVisualizer->NotifyControlVisibilityChange(aIsVisible);
    
    //Add & remove extra touch events. 
    if(aIsVisible && !iIsVisible)
        {
        TInt dragTreshold_X = KDraggingTresholdInPixels;
		// If AknPhysics used, get drag treshold from there
        if( iPhysics )
            {
            dragTreshold_X = iPhysics->DragThreshold();
            }

        // Currently only panning (vertical dragging) is implemented,
		// so horizontal drag treshold should be a bit bigger
        TInt dragTreshold_Y = dragTreshold_X * 2;
            
        // Add observer for long tap and drag events, set treshold for drag
        // events and disable long tap events while dragging
        aDisplay.Roster().AddPointerEventObserver( EAlfPointerEventReportLongTap, *this );        
        aDisplay.Roster().AddPointerEventObserver( EAlfPointerEventReportDrag, *this );
        aDisplay.Roster().SetPointerDragThreshold(
                *this, TAlfXYMetric( TAlfMetric(dragTreshold_X), TAlfMetric(dragTreshold_Y) ) );
        aDisplay.Roster().DisableLongTapEventsWhenDragging( *this );
        iIsVisible = ETrue;
        }
    else if(!aIsVisible && iIsVisible )
        {
        if( iPhysics )
            {
            iPhysics->StopPhysics();
            iPhysics->ResetFriction();
            }

        aDisplay.Roster().RemovePointerEventObserver(EAlfPointerEventReportLongTap, *this);
        aDisplay.Roster().RemovePointerEventObserver(EAlfPointerEventReportDrag, *this);
        
        iIsVisible = EFalse;
        }

    }

// ---------------------------------------------------------------------------
// OfferEventL
// ---------------------------------------------------------------------------
TBool CFsTextViewerControl::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    if ( aEvent.IsPointerEvent() )
        {
        retVal = HandlePointerEventL( aEvent );
        }
    else if( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
        {
        retVal = HandleKeyEventL( aEvent );
        }

    return retVal;
    }
    
// ---------------------------------------------------------------------------
// CFsTextViewerControl::HandleKeyEventL
// Handle keyboard events
// ---------------------------------------------------------------------------
//
TBool CFsTextViewerControl::HandleKeyEventL(const TAlfEvent& aEvent)
    {
    TBool eventHandled( EFalse );
    
    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }

    if ( aEvent.KeyEvent().iCode == iKeys->GetKeyUp() )
        {
        TBool hotspotChanged = EFalse;
        if ( iCursorUDScroll )
            {
            iVisualizer->MoveUpL();
            }
        else
            {
            if ( iVisualizer->PrevVisibleHotspotL( hotspotChanged ) )
                {
                if ( iObserver && hotspotChanged )
                    {
                    iObserver->HandleTextViewerEventL(
                            MFsTextViewerObserver::EFsTextViewerHotspotChanged );
                    }
                }
            else 
                {
                ScrollL( MFsTextViewerObserver::EFsTextViewerScrollUp );
                }
            }
        eventHandled = ETrue;
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyDown() )
        {
        TBool hotspotChanged = EFalse;
        if ( iCursorUDScroll )
            {
            iVisualizer->MoveDownL();
            }
        else
            {
            if ( iVisualizer->NextVisibleHotspotL( hotspotChanged ) )
                {
                if ( iObserver && hotspotChanged )
                    {
                    iObserver->HandleTextViewerEventL(
                            MFsTextViewerObserver::EFsTextViewerHotspotChanged );
                    }
                }
            else 
                {
                ScrollL( MFsTextViewerObserver::EFsTextViewerScrollDown );
                }
            }
        eventHandled = ETrue;
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyPgUp() )
        {
        iVisualizer->PageUpL();
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyPgDown() )
        {
        iVisualizer->PageDownL();
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyRight() )
        {
        iVisualizer->MoveRightL();
        if ( iVisualizer->MarkEnabled() && iObserver )
            {
            iObserver->HandleTextViewerEventL( 
                MFsTextViewerObserver::EFsTextViewerMarkChange );
            }
        eventHandled = ETrue;
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyLeft() )
        {
        iVisualizer->MoveLeftL();
        if ( iVisualizer->MarkEnabled() && iObserver )
            {
            iObserver->HandleTextViewerEventL( 
                MFsTextViewerObserver::EFsTextViewerMarkChange );
            }
        eventHandled = ETrue;
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyMark() )
        {
        iVisualizer->SwitchMarkL();
        if ( iVisualizer->MarkEnabled() && iObserver )
            {
            iObserver->HandleTextViewerEventL(
                MFsTextViewerObserver::EFsTextViewerMarkStart );
            }
        else if ( iObserver ) 
            {
            iObserver->HandleTextViewerEventL( 
                MFsTextViewerObserver::EFsTextViewerMarkEnd );
            }
        eventHandled = ETrue;
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyClick() )
        {
        iVisualizer->ClickedL();
        if ( iVisualizer->IsHotspotHighlighted() && iObserver )
            {
            iObserver->HandleTextViewerEventL( 
                MFsTextViewerObserver::EFsTextViewerHotspotClicked );
            }
        eventHandled = ETrue;
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyScrollUp() )
        {
        ScrollL( MFsTextViewerObserver::EFsTextViewerScrollUp );
        eventHandled = ETrue;
        }
    else if ( aEvent.KeyEvent().iCode == iKeys->GetKeyScrollDown() )
        {
        ScrollL( MFsTextViewerObserver::EFsTextViewerScrollDown );
        eventHandled = ETrue;
        }
    
    return eventHandled;
    }

// ---------------------------------------------------------------------------
// CFsTextViewerControl::HandlePointerEventL
// Handle pointer (touch) events
// ---------------------------------------------------------------------------
//
TBool CFsTextViewerControl::HandlePointerEventL(const TAlfEvent& aEvent)
    {
    TBool eventHandled( EFalse );

    // Check if this event is bound to some of our visuals
    CAlfVisual* selectedVis = aEvent.Visual();
    TFsRangedVisual* rangedVis = NULL;
    if( selectedVis )
        {
        rangedVis = iVisualizer->Navigator()->GetRangedVisual( selectedVis );
        }

    switch ( aEvent.PointerEvent().iType )
        {
        case TPointerEvent::EButton1Down:
            {
            // Save current and original position so that those can be used in
            // drag/scrolling calculations
            iPreviousPosition = iOriginalPosition = aEvent.PointerEvent().iParentPosition;
            iIsDragging = EFalse;
            iIsFlicking = EFalse;

            if( iPhysics )
                {
                iPhysics->StopPhysics();
                iPhysics->ResetFriction();
                iStartTime.HomeTime();
                UpdatePhysicsL();
                }
            
            // On button down event, move focus to highlighted hotspot
            if( rangedVis && rangedVis->iHotspot )
                {
                // Allow future hotspot actions
                iStartHotspotAction = ETrue;

                // Move focus to currently pointed hotspot and report event
                // to observers
                iVisualizer->SetCurrentHotspotByCharL( rangedVis->iStart );
                if ( iObserver )
                    {
                    iObserver->HandleTextViewerEventL( MFsTextViewerObserver::EFsTextViewerHotspotChanged );
                    }

                eventHandled = ETrue;
                }
            }
            break;

        case TPointerEvent::EButton1Up:
            {
            if( iIsDragging && iPhysics )
                {
                TPoint drag(iOriginalPosition - aEvent.PointerEvent().iParentPosition);
                iPhysics->StartPhysics(drag, iStartTime);
                iIsFlicking = ETrue;
                }

            // On button up event start action for focused hotspot, if hotspot
            // action allowed
            else if( iStartHotspotAction && rangedVis && rangedVis->iHotspot )
                {
                // Hotspot action started, so disable all future actions
                iStartHotspotAction = EFalse;

                // Move focus to currently pointed hotspot and report event
                // to observers
                iVisualizer->SetCurrentHotspotByCharL( rangedVis->iStart );
                if ( iObserver )
                    {
                    iObserver->HandleTextViewerEventL( MFsTextViewerObserver::EFsTextViewerHotspotClicked );
                    }

                eventHandled = ETrue;
                }
            }
            break;
        
        case TPointerEvent::EButtonRepeat:
            {
            // On repeat event report long tap event to observers, if hotspot
            // actions are allowed
            if( iStartHotspotAction )
                {
                // Hotspot action started, so disable all future actions
                iStartHotspotAction = EFalse;
                
                // Report long tap event to observers
                if ( iObserver )
                    {
                    iObserver->HandleTextViewerEventL( MFsTextViewerObserver::EFsTextViewerHotspotLongTap );
                    }
                
                eventHandled = ETrue;
                }
            break;
            }
        
        case TPointerEvent::EDrag:
            {
            // Get current pointer position
            TPoint position = aEvent.PointerEvent().iParentPosition;

            // If user started dragging, cancel hotspot actions
            iStartHotspotAction = EFalse;
            iIsDragging = ETrue;

            if( iPhysics )
                {
                TPoint delta( 0, iPreviousPosition.iY - position.iY );
                iPhysics->RegisterPanningPosition( delta );
                }
            else
                {
                // Calculate new scroll offset based on current and previous Y-positions
                TInt scrollOffset = iVisualizer->GetViewTopPosition() + ( iPreviousPosition.iY - position.iY );
                // Ensure that thumb position is in correct range
                scrollOffset = Max( scrollOffset, 0 );
                scrollOffset = Min( scrollOffset, iVisualizer->GetTotalHeight() - iVisualizer->iContentSize.iHeight );

                ScrollL( scrollOffset );
                }

            // Save current position as previous pos for future calculations
            iPreviousPosition = position;
            
            eventHandled = ETrue;
            }
            break;
        
        default:
            {
            // Unknown event, ignore.
            }
            break;
        }
        
    return eventHandled;
    }

// <cmail> Change scrollbar to avkon (to support skinning & touch)
// ---------------------------------------------------------------------------
//  Handle scrollbar events
// ---------------------------------------------------------------------------
//
void CFsTextViewerControl::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
    {
    iScrollEventHandlingOngoing = ETrue;
    
    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }

    if ( aScrollBar == iVisualizer->GetScrollBar() )
        {
        TInt thumbPos = -1;
        switch( aEventType )
            {
            case EEikScrollHome :
                {
                //Jump to beginning
                thumbPos = 0;
                }
                break;
            case EEikScrollEnd :
                {
                //Jump to end
                thumbPos = iVisualizer->GetTotalHeight() - iVisualizer->iContentSize.iHeight;
                }
                break;
            default:
                {
                thumbPos = aScrollBar->ThumbPosition();
                }
                break;
            }
                
        if( thumbPos >= 0 )
            {
            // Ensure that thumb position is in correct range
            thumbPos = Min( thumbPos, iVisualizer->GetTotalHeight() - iVisualizer->iContentSize.iHeight );

            // Scroll text viewer to thumb position with default scroll time
            ScrollL( thumbPos );
            }
        }

    iScrollEventHandlingOngoing = EFalse;
    }

void CFsTextViewerControl::ScrollL(
        const TInt aOffset,
        const TInt aTime /*= CFsTextViewerVisualizer::EUseDefaultScrollTime*/ )
    {
    // Scroll text viewer to the calculated position.
    // If position is above current line offset, scroll up.
    // If position is below current line offset, scroll down.
    // If position is same as current line offset, do nothing.
    if ( aOffset < iVisualizer->GetViewTopPosition() )
        {
        ScrollL( MFsTextViewerObserver::EFsTextViewerScrollUp, aOffset, aTime );
        }
    else if ( aOffset > iVisualizer->GetViewTopPosition() )
        {
        ScrollL( MFsTextViewerObserver::EFsTextViewerScrollDown, aOffset, aTime );
        }
    }

void CFsTextViewerControl::ScrollL(
        const MFsTextViewerObserver::TFsTextViewerEvent aEventDirection,
        const TInt aOffset /*= CFsTextViewerVisualizer::EUseDefaultScrollOffset*/,
        const TInt aTime /*= CFsTextViewerVisualizer::EUseDefaultScrollTime*/ )
    {
    switch( aEventDirection )
        {
        case MFsTextViewerObserver::EFsTextViewerScrollUp:
            {
            iVisualizer->ScrollUpL( aOffset, aTime );
            }
            break;

        case MFsTextViewerObserver::EFsTextViewerScrollDown:
            {
            iVisualizer->ScrollDownL( aOffset, aTime );
            }
            break;
            
        // No need to handle any other events here
        default:
            return;
        }
    
    if ( iObserver )
        {
        iObserver->HandleTextViewerEventL( aEventDirection );
        }
    }

// ---------------------------------------------------------------------------
// Update physics
// ---------------------------------------------------------------------------
//
void CFsTextViewerControl::UpdatePhysicsL()
    {
    FUNC_LOG;
    if ( iPhysics )
        {
        const TSize viewSize( iVisualizer->iContentSize );
        const TSize worldSize( viewSize.iWidth, iVisualizer->GetTotalHeight() );
        iPhysics->InitPhysicsL( worldSize, viewSize, EFalse );
        }
    }

/**
 * @see MAknPhysicsObserver::ViewPositionChanged
 */
void CFsTextViewerControl::ViewPositionChanged(
        const TPoint& aNewPosition,
        TBool /*aDrawNow*/,
        TUint /*aFlags*/ )
    {
    // Sometimes ViewPositionChanged events come after view is already changed,
    // so adding a safety check here
    if( iIsVisible )
        {
        TInt scrollOffset = aNewPosition.iY - iVisualizer->iContentSize.iHeight / 2;

        //CAlfStatic::Env().SetRefreshMode( EAlfRefreshModeManual );
        if( iIsFlicking )
            {
            TRAP_IGNORE( ScrollL( scrollOffset, 1 ) );
            }
        else
            {
            TRAP_IGNORE( ScrollL( scrollOffset, 250 ) );
            }
        //iListLayout->Env().RefreshCallBack( &iListLayout->Env() );
        //iVisualizer->RootLayout()->UpdateChildrenLayout();
        //CAlfStatic::Env().RefreshCallBack( &CAlfStatic::Env() );
        //CAlfStatic::Env().SetRefreshMode( EAlfRefreshModeAutomatic );
        }
    }

/**
 * @see MAknPhysicsObserver::PhysicEmulationEnded
 */
void CFsTextViewerControl::PhysicEmulationEnded()
    {
    iIsFlicking = EFalse;
    }

/**
 * @see MAknPhysicsObserver::ViewPosition
 */
TPoint CFsTextViewerControl::ViewPosition() const
    {
    return iVisualizer->GetViewCenterPosition();
    }
