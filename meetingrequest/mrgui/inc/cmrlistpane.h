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
* Description:  Interface definition for GUI List pane component
*
*/

#ifndef CMRLISTPANE_H
#define CMRLISTPANE_H

// INCLUDES
#include <coecntrl.h>
#include <aknlongtapdetector.h>
#include "mmrphysicsobserver.h"
#include "mmrfieldcontainerobserver.h"

// FORWARD DECLARATIONS
class MESMRFieldStorage;
class TAknDoubleSpanScrollBarModel;
class CMRFieldContainer;
class CMRListPanePhysics;
class CAknDoubleSpanScrollBar;
class MMRScrollBarObserver;
class CESMRField;
class MESMRCalEntry;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRListPane ) : 
        public CCoeControl,
        public MAknLongTapDetectorCallBack,
        public MMRPhysicsObserver,
        public MMRFieldContainerObserver
    {
    public: // Creation and destruction
        /**
         * Static constructor.
         * 
         * @return New instance of this class
         */
        static CMRListPane* NewL( const CCoeControl& aParent,
                                  MESMRFieldStorage& aFactory,
                                  TAknDoubleSpanScrollBarModel& aScrollModel, 
                                  CAknDoubleSpanScrollBar& aScroll, 
                                  MMRScrollBarObserver& aScrollBarObserver );
        /**
         * Destructor
         */
        ~CMRListPane();
        
    public: // Interface

        /**
         * Initializes all fields. Needed when theme changes
         */
        void InitializeL();
        
        /**
         * InternalizeL is used to obtain the data from current CCalEntry.
         * CCalEntry is wrapped inside MESMRCalEntry object. This call is
         * propagated to all list items in the list.
         *
         * @param aEntry Current entry
         * @see MESMRCalEntry
         */
        void InternalizeL( MESMRCalEntry& aEntry );

        /**
         * ExternalizeL is used to store the list item data into current
         * CCalEntry. CCalEntry is wrapped inside the MESMRCalEntry object.
         *
         * @param aEntry Current entry.
         * @param aForceValidation ETrue if force validation should be used
         * @see MESMRCalEntry
         */
        void ExternalizeL( MESMRCalEntry& aEntry, TBool aForceValidation = EFalse );
           
        /**
         * Disables or enables relayouting.
         *
         * @param aDisable. If ETrue, disables relayouting.
         */
        void DisableSizeChange(TBool aDisable );

         /**
          * Scroll the list to put the responeArea at the top and
          * set the focus on it
          */
         void InitialScrollL(); // TODO: Fix me. 
         
        /**
         * Return focused field.
         * @return Focused list item or NULL if no focused item
         */
        CESMRField* FocusedField() const;
        
        /**
         * Sets the field focused based on the given field id.
         * @param aFieldId Id of the field to be focused
         */
        void SetControlFocusedL( TESMREntryFieldId aFieldId );
        
        /**
         * Return clicked list item.
         * @return Clicked list item or NULL if no valid clicked item
         */
        CESMRField* ClickedField() const;
               
        /**
         * Sets field to be visible in the list.
         *
         * @param aField - id of the field.
         */
        void ShowControl( TESMREntryFieldId aFieldId );
        
        /**
         * Is field visible or not.
         *
         * @return Boolean ETrue/EFalse
         */
        TBool IsControlVisible( TESMREntryFieldId aFieldId );
                
        /**
         * Get response field type of current event.
         * @Return TESMREntryFieldId
         */
        TESMREntryFieldId GetResponseFieldsFieldId();
        
        /**
         * Activates new fields which has been added into the storage
         * after list pane construction.
         */
        void ReActivateL();
        
    public: // MMRFieldContainerObserver
        void ScrollFieldsUp( TInt aPx );
        void ScrollFieldsDown( TInt aPx );
        void UpdateScrollBarAndPhysics();
        
    public: // From CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                     TEventCode aType );
        void SizeChanged();

    private: // From CCoeControl
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex) const;
        void HandlePointerEventL( const TPointerEvent &aPointerEvent );
        void ActivateL();
        
    private: // From MMRPhysicsObserver
        void PhysicsEmulationEnded();
        
    private: // From MAknLongTapDetectorCallBack
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation, 
                const TPoint& aPenEventScreenLocation );
    
    private: // Implementation
        CMRListPane( MESMRFieldStorage& aFactory,
                     TAknDoubleSpanScrollBarModel& aScrollModel, 
                     CAknDoubleSpanScrollBar& aScroll, 
                     MMRScrollBarObserver& aScrollBarObserver );
        void ConstructL( const CCoeControl& aParent );
        void DoUpdateScrollBar( TInt aFocusPosition = KErrNotFound );
        TInt UpdatedFocusPosition();
        void SetFocusAfterPointerEventL( 
                const TPointerEvent &aPointerEvent );
        void UpdateClickedField( const TPointerEvent &aPointerEvent );
        TBool HiddenFocus();
        void ForwardReceivedPointerEventsToChildrenL();
        
    private: // Data
        /// Ref: Storage for list items.
        MESMRFieldStorage& iFactory;
        /// Ref: scrollbar model
        TAknDoubleSpanScrollBarModel& iScrollModel;
        /// Ref: List components scroll bar
        CAknDoubleSpanScrollBar& iScroll;
        /// Own: Long tap detector
        CAknLongTapDetector* iLongtapDetector;
        /// Own: Field container
        CMRFieldContainer* iFieldContainer;
        /// Own: Physics for kinetic scrolling
        CMRListPanePhysics* iPhysics;
        /// Ref: Observer for updating scroll bar revents
        MMRScrollBarObserver& iScrollBarObserver;
        /// Not owned: A field which got a pointer down event, 
        /// nulled when focus is changed via keyboard
        CESMRField* iClickedField;
        /// Own: Boolean for disabling SizeChanged handling
        TBool iDisableSizeChanged;
        /// Own: Boolean to check, if physics action is ongoing or not
        TBool iPhysicsActionOngoing;
        /// Own: Record if the long tapping event have been comsumed, if yes, 
        /// then do not handle signal pointer event anymore.
        TBool iLongTapEventConsumed;
    };
    
#endif // CMRLISTPANE_H
// End of file
