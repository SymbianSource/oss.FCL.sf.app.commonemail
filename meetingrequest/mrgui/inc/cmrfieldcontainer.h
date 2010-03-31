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
* Description:  Interface for field container
*
*/

#ifndef CMRFIELDCONTAINER_H
#define CMRFIELDCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "mesmrlistobserver.h"

// FORWARD DECLARATIONS
class MESMRFieldStorage;
class MMRFieldContainerObserver;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRFieldContainer ) : 
    public CCoeControl,
    public MESMRListObserver
    {
    public: // Creation and destruction
        /**
         * Static constructor.
         * 
         * @return New instance of this class
         */
        static CMRFieldContainer* NewL( 
                MESMRFieldStorage& aFactory, 
                const CCoeControl& aParent );
        /**
         * Destructor
         */
        ~CMRFieldContainer();
        
    public: // Interface
                 
         /**
          * Return focused field.
          * @return Focused list item or NULL if no focused item
          */
         CESMRField* FocusedField() const;
         
         /**
          * Moves focus up one step.
          * 
          * @param aHiddenFocus, in case of hidden focus use case, ETrue. 
          * @return TKeyResponse, if key response was used or not
          */
         TKeyResponse MoveFocusUpL( TBool aHiddenFocus );

         /**
          * Moves focus down one step.
          * 
          * @param aHiddenFocus, in case of hidden focus use case, ETrue. 
          * @return TKeyResponse, if key response was used or not
          */
         TKeyResponse MoveFocusDownL( TBool aHiddenFocus );
         
         /**
          * Moves focus visible if it is partly or completely out of
          * viewable area.
          */
         void SetFieldContainerObserver( MMRFieldContainerObserver* aObserver );
        
    public: // From MESMRListObserver
        void ControlSizeChanged( CESMRField* aField );
        void ShowControl( TESMREntryFieldId aFieldId );
        void HideControl( TESMREntryFieldId aFieldId );
        TBool IsControlVisible( TESMREntryFieldId aFieldId );
        void SetControlFocusedL( TESMREntryFieldId aFieldId );
        TInt ListHeight();
        TBool IsFocusedControlsBottomVisible();
        void ScrollControlVisible( TInt aInd );
        void RePositionFields( TInt aAmount );
        TRect ViewableAreaRect();
        
    public: // From CCoeControl
        TSize MinimumSize();
        
    private: // From CCoeControl
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        void SizeChanged();
        
    private: // Implementation
        CMRFieldContainer( MESMRFieldStorage& aFactory );
        void ConstructL( const CCoeControl& aParent );
        void MoveFields( TInt aIndex, TPoint& aTl );
        void LayoutField( CESMRField& aField, const TPoint& aTl );
        TInt LastVisibleField( TESMREntryFieldId aFieldId );
        void DoSetFocusL( TInt aNewFocusIndex );
        TKeyResponse MoveFocusVisibleL();
        
    private: // Data
        // Own: Informs listapane about needed position changes
        MMRFieldContainerObserver* iObserver;
        /// Ref: Storage for list items.
        MESMRFieldStorage& iFactory;
        /// Own: Current focus index
        TInt iFocusedFieldIndex;

    };
    
#endif // CMRFIELDCONTAINER_H

// End of file
