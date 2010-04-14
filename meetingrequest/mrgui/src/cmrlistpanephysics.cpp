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
* Description:  MR listpane physics impl
*
*/

#include "cmrlistpanephysics.h"
#include "cmrlistpane.h"
#include "nmrlayoutmanager.h"
#include "cmrfieldcontainer.h"

#include <aknphysics.h>

//DEBUG
#include "emailtrace.h"


namespace { // codescanner::namespace

const TInt KCustomDragTreshold( 10 );

} // namespace

// ---------------------------------------------------------------------------
// CMRListPanePhysics::CMRListPanePhysics
// ---------------------------------------------------------------------------
//
CMRListPanePhysics::CMRListPanePhysics(
        CCoeControl& aParent, 
        CMRFieldContainer& aViewControl, 
        MMRPhysicsObserver& aPhysicsObserver )
    : iParent( aParent ),
      iViewControl( aViewControl ),
      iPhysicsObserver( aPhysicsObserver )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::ConstructL
// ---------------------------------------------------------------------------
//
void CMRListPanePhysics::ConstructL()
    {
    FUNC_LOG;
    iFeatureEnabled = CAknPhysics::FeatureEnabled();
    if ( iFeatureEnabled )
        {
        // Physics: Create physics
        // Give pointer to control that should be able to flick/drag
        iPhysics = CAknPhysics::NewL( *this, &iViewControl );
        }
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::NewL
// ---------------------------------------------------------------------------
//
CMRListPanePhysics* CMRListPanePhysics::NewL( 
        CCoeControl& aParent, 
        CMRFieldContainer& aViewControl,
        MMRPhysicsObserver& aPhysicsObserver )
    {
    FUNC_LOG;
    CMRListPanePhysics* self = 
        new ( ELeave ) CMRListPanePhysics( 
                aParent, 
                aViewControl, 
                aPhysicsObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
        
// ---------------------------------------------------------------------------
// CMRListPanePhysics::~CMRListPanePhysics
// ---------------------------------------------------------------------------
//
CMRListPanePhysics::~CMRListPanePhysics()
    {
    FUNC_LOG;
    delete iPhysics;
    }
        
// ---------------------------------------------------------------------------
// CMRListPanePhysics::HandlePointerEventL
// ---------------------------------------------------------------------------
//
TBool CMRListPanePhysics::HandlePointerEventL(
        const TPointerEvent& aPointerEvent, TBool& aEventsBlocked )
    {
    FUNC_LOG;
    TBool physicsStarted( EFalse );
    if ( !iFeatureEnabled || iPhysics->OngoingPhysicsAction() ==
            CAknPhysics::EAknPhysicsActionBouncing )
        {
        return physicsStarted;
        }

    // Down
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        // Save start time and start point
        iStartTime.HomeTime();
        iStartPoint = aPointerEvent.iPosition;
        iDragPoint = iStartPoint;
        }
    
    // Drag
    else if ( aPointerEvent.iType == TPointerEvent::EDrag )
        {
        // Check how position was changed and report to physics
        TPoint deltaPoint( iDragPoint - aPointerEvent.iPosition );
        if( Abs( deltaPoint.iY ) < KCustomDragTreshold )
        	{
			deltaPoint.iX = 0;
			deltaPoint.iY = 0;
        	}
        
        iDragPoint = aPointerEvent.iPosition;
        iPhysics->RegisterPanningPosition( deltaPoint );
        }

    // Up
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        // Calculate dragging distance
        TPoint drag( iStartPoint - aPointerEvent.iPosition );
        
        if( Abs( drag.iY ) < KCustomDragTreshold )
        	{
			drag.iX = 0;
			drag.iY = 0;
        	}
        
        // Start physics
        physicsStarted = iPhysics->StartPhysics( drag, iStartTime );
        
        if( physicsStarted )
        	{
			aEventsBlocked = ETrue;
        	}
        else
        	{
			aEventsBlocked = EFalse;
        	}
        }    
    
    // Record previous pointer event
    iPreviousPointerEvent = aPointerEvent;
    
    return physicsStarted;
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::InitPhysicsL
// ---------------------------------------------------------------------------
//
void CMRListPanePhysics::InitPhysics()
    {
    FUNC_LOG;
    if ( !iFeatureEnabled )
        {
        return;
        }    
    TRect parentRect( iParent.Rect() );

    iWorldSize.iHeight = iViewControl.MinimumSize().iHeight;
    iWorldSize.iWidth = iViewControl.MinimumSize().iWidth;
    
    iViewSize.iHeight = parentRect.Height();
    iViewSize.iWidth = parentRect.Width();
    
    if( iWorldSize.iHeight < iViewSize.iHeight )
        {
    	iWorldSize.iHeight = iViewSize.iHeight;
        }
    

    
    iPhysics->ResetFriction();
    
    TRAP_IGNORE( iPhysics->InitPhysicsL( iWorldSize, iViewSize, EFalse ) );
    iPhysics->UpdateViewWindowControl( &iParent );
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::SetWorldHeight
// ---------------------------------------------------------------------------
//
void CMRListPanePhysics::SetWorldHeight( TInt aWorldHeight )
    {
    FUNC_LOG;
    if ( !iFeatureEnabled )
        {
        return;
        }    
    iWorldSize.iHeight = aWorldHeight;
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::ViewPositionChanged
// ---------------------------------------------------------------------------
//
void CMRListPanePhysics::ViewPositionChanged( const TPoint& aNewPosition,
                          TBool aDrawNow,
                          TUint /*aFlags*/ )
    {
    FUNC_LOG;
    if ( !iFeatureEnabled )
        {
        return;
        }
    if ( iPhysics->OngoingPhysicsAction() ==
            CAknPhysics::EAknPhysicsActionBouncing )
        {
        return;
        }

    iVerScrollIndex = aNewPosition.iY - iViewSize.iHeight / 2;
    
    // Parents position is taken into account, by
    // adding the extra x and y coordinates to field containers
    // new position.
    iViewControl.SetPosition( TPoint( 
            iParent.Position().iX, 
            -iVerScrollIndex + iParent.Position().iY ) );
    
    // Draw only when drawing is allowed
    if ( aDrawNow )
        {
        iParent.DrawDeferred();
        }
    
    // Vertical scroll index has changed, we need to update scroll bar also
    iPhysicsObserver.UpdateScrollBarDuringOngoingPhysics();
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::PhysicEmulationEnded
// ---------------------------------------------------------------------------
//
void CMRListPanePhysics::PhysicEmulationEnded()
    {
    FUNC_LOG;
    iPhysicsObserver.PhysicsEmulationEnded();
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::ViewPosition
// ---------------------------------------------------------------------------
//
TPoint CMRListPanePhysics::ViewPosition() const
    {
    FUNC_LOG;
    if ( !iFeatureEnabled )
        {
        return TPoint( iParent.Position() );
        }
    
    // This is the default implementation
    TPoint viewPosition( 
            iViewSize.iWidth / 2, 
            iViewSize.iHeight / 2 + iVerScrollIndex );
    
    return viewPosition;
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::UpdateVerticalScrollIndex
// ---------------------------------------------------------------------------
//
void CMRListPanePhysics::UpdateVerticalScrollIndex( TInt aVerScrollIndex )
    {
    FUNC_LOG;
    if ( iFeatureEnabled )
        {
        // Physics' new position is updated to this member, when field 
        // container is scrolled by someone else than physics.
        iVerScrollIndex = aVerScrollIndex;
        }
    }

// ---------------------------------------------------------------------------
// CMRListPanePhysics::VerticalScrollIndex
// ---------------------------------------------------------------------------
//
TInt CMRListPanePhysics::VerticalScrollIndex()
    {
    return iVerScrollIndex;
    }
// End of file

