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
* Description:  ESMR Custom list component physics impl
*
*/

#ifndef CMRLISTPANEPHYSICS_H
#define CMRLISTPANEPHYSICS_H

// INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <aknphysicsobserveriface.h>

// FORWARD DECLARATIONS
class CAknPhysics;
class CMRFieldContainer;
class CCoeControl;
class MMRPhysicsObserver;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRListPanePhysics ): 
    public CBase, 
    public MAknPhysicsObserver
    {
    public: // C'tor and d'tor
        static CMRListPanePhysics* NewL(
                CCoeControl& aParent, 
                CMRFieldContainer& aViewControl,
                MMRPhysicsObserver& aPhysicsObserver );
        
        ~CMRListPanePhysics();
        
    public: // Interface
        /**
         * Handles the pointer event
         *
         * @param aPointerEvent Pointer event to handle.
         * @param aEventsBlocked ETrue if events are blocked.
         *          Set by this function.
         * @return ETrue if the physics has been started, EFalse otherwise.
         */
        TBool HandlePointerEventL(
                const TPointerEvent& aPointerEvent, TBool& aEventsBlocked );
        
        /**
         * Initializes physics.
         */
        void InitPhysics();
        
        /**
         * Sets world height.
         *
         * @param aWorldHeight New world height.
         */
        void SetWorldHeight( TInt aWorldHeight );
        
        /**
         * Sets the vertical scroll index, which is directly the position 
         * of the list pane on the Y-axis
         *
         * @param aVerScrollIndex New vertical scroll index.
         */
        void UpdateVerticalScrollIndex( TInt aVerScrollIndex );
        
        /**
         * Returns the current vertical scroll index.
         *
         * @return Current vertical scroll index
         */
        TInt VerticalScrollIndex();

        
    public:    // from base class MAknPhysicsObserver
        void ViewPositionChanged( const TPoint& aNewPosition,
                                  TBool aDrawNow,
                                  TUint aFlags );            
        void PhysicEmulationEnded();    
        TPoint ViewPosition() const;
        
    private: // Implementation
        CMRListPanePhysics( CCoeControl& aParent, 
                            CMRFieldContainer& aViewControl,
                            MMRPhysicsObserver& aPhysicsObserver );
        void ConstructL();

    private:
        /// Own: Physics engine handle
        CAknPhysics* iPhysics;   
        /// Ref: Parent control
        CCoeControl& iParent;
        /// Ref: Reference to field container that should be kinetic
        CMRFieldContainer& iViewControl;
        /// Feature enabled
        TBool iFeatureEnabled;
        /// Ref: Physics observer
        MMRPhysicsObserver& iPhysicsObserver;
        /// Own: Records previous pointer event
        TPointerEvent iPreviousPointerEvent;

        /**
         * Physics world size.
         */
        TSize iWorldSize;
        
        /**
         * Physics view size.
         */
        TSize iViewSize;

        /**
         * Vertical scroll index.
         */
        TInt iVerScrollIndex;
        
        /**
         * Pointer event start time.
         */
        TTime iStartTime;
        
        /**
         * Pointer event start position.
         */
        TPoint iStartPoint;

        /**
         * Pointer event drag position.
         */
        TPoint iDragPoint;             
    };

#endif // CMRLISTPANEPHYSICS_H
// End of file

