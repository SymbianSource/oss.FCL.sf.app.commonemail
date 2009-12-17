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
* Description:  Control class for TextViewer component
*
*/


#ifndef C_FSTEXTVIEWERCONTROL_H
#define C_FSTEXTVIEWERCONTROL_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include <alf/alfcontrol.h>
#include <eiksbobs.h>                // MEikScrollBarObserver
#include <aknphysicsobserveriface.h> // MAknPhysicsObserver
// </cmail>

#include "fstextviewerobserver.h"
#include "fstextviewervisualizer.h"

class CAknPhysics;
class CFsTextViewerVisualizer;
class MFsTextViewerObserver;
class CFsTextViewerKeys;

class CFsTextViewerControl :
    public CAlfControl,
    public MEikScrollBarObserver,
    public MAknPhysicsObserver
    {

    public:
        static CFsTextViewerControl* NewL( CAlfEnv& aEnv );
        ~CFsTextViewerControl();

    public:
        void SetVisualizer( CFsTextViewerVisualizer* aVis );
        void SetObserver( MFsTextViewerObserver* aObserver );
        void EnableCursorScroll( TBool aStatus = ETrue );
        TBool CursorScroll();
        void SetKeys( CFsTextViewerKeys* aKeys );
        CFsTextViewerKeys* Keys();
        TBool IsScrollEventHandlingOngoing();

        // Scroll specified offset, direction is calculated based on the
        // given offset and view's current position.
        void ScrollL(
                const TInt aOffset,
                const TInt aTime = CFsTextViewerVisualizer::EUseDefaultScrollTime );
        // Scroll specified offset to specified direction. If aOffset is
        // less than zero, scrolls default amount.
        void ScrollL( 
                const MFsTextViewerObserver::TFsTextViewerEvent aEventDirection,
                const TInt aOffset = CFsTextViewerVisualizer::EUseDefaultScrollOffset,
                const TInt aTime = CFsTextViewerVisualizer::EUseDefaultScrollTime );
        
    protected:
        /**
         * From CAlfControl
         */
        void NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& aDisplay );
        
    private:
        CFsTextViewerControl(  );
        void ConstructL( CAlfEnv& aEnv );

    private:
        TBool OfferEventL( const TAlfEvent& aEvent );
        TBool HandleKeyEventL( const TAlfEvent& aEvent );
        TBool HandlePointerEventL( const TAlfEvent& aEvent );
        void UpdatePhysicsL();
        
    public:
        /**
         * From MEikScrollBarObserver
         * 
         * Callback method for scroll bar events
         *
         * Scroll bar observer should implement this method to get scroll bar events.
         *
         * @since S60 0.9
         * @param aScrollBar A pointer to scrollbar which created the event
         * @param aEventType The event occured on the scroll bar
         */
        void HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );
        
    private: // from MAknPhysicsObserver

       /**
        * @see MAknPhysicsObserver::ViewPositionChanged
        */
       virtual void ViewPositionChanged( const TPoint& aNewPosition, TBool aDrawNow, TUint aFlags );

       /**
        * @see MAknPhysicsObserver::PhysicEmulationEnded
        */
       virtual void PhysicEmulationEnded();

       /**
        * @see MAknPhysicsObserver::ViewPosition
        */
       virtual TPoint ViewPosition() const;

    private:
        CFsTextViewerVisualizer* iVisualizer;
        MFsTextViewerObserver* iObserver;
        TBool iCursorUDScroll;
        CFsTextViewerKeys* iKeys;
        CAknPhysics* iPhysics;
        TBool iScrollEventHandlingOngoing;
        TBool iIsVisible;
        TBool iStartHotspotAction;
        TBool iIsDragging;
        TBool iIsFlicking;
        TPoint iPreviousPosition;
        TPoint iOriginalPosition;
        TTime iStartTime;
    };

#endif // C_FSTEXTVIEWERCONTROL_H

