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
* Description:  Contains the implementation of response area
 *
*/

#ifndef CESMRRESPONSEFIELD_H
#define CESMRRESPONSEFIELD_H

// INCLUDES
#include <e32base.h>
#include <fbs.h>

#include "cesmrfield.h"
#include "resmrpluginextensionstatic.h"

// FORWARD DECLARATIONS
class CLayoutManager;
class MESMRResponseObserver;
class CESMRConflictPopup;
class CESMRResponseItem;

/**
 * Response field is the actual field that receives focus when scrolling in the
 * viewer list. This field constructs the items and sets texts, fonts and sizes for those,
 * This field is also responsible of scrolling between the items and calling the response observer
 * when answer is selected (e.g. by pressing accept item).
 */
NONSHARABLE_CLASS( CESMRResponseField ) : public CESMRField
    {
    public:
        /**
         * Creates new response field. Response field is responsible
         * of creating all the items needed in the area.
         * @return New response field object.
         */
        static CESMRResponseField* NewL(MESMRResponseObserver* aResponseObserver);

        /**
         * C++ Destructor.
         */
        ~CESMRResponseField();

    public: // From CCoeControl
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aInd ) const;
        void SizeChanged();
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,  TEventCode aType );
        void FocusChanged( TDrawNow aDrawNow );
        TSize MinimumSize();

    public: // From CESMRField
        void InitializeL();
        void InternalizeL( MESMRCalEntry& aEntry );
        TBool ExecuteGenericCommandL( TInt aCommand );
        void SetOutlineFocusL( TBool aFocus );
        TBool HandleSingletapEventL( const TPoint& aPosition );
        void HandleLongtapEventL( const TPoint& aPosition );

    public: // New methods
        /**
         * Set the response observer for this class.
         *
         * @param aResponseObserver - Response observer
         */
        void AddObserver( MESMRResponseObserver* aResponseObserver );

        /**
         * Calls onwards the SetFont() method for all the items in the array
         *
         * @param aFont - Font to be set.
         */
        void SetFont( const CFont* aFont );

        /**
         * Selects the current item.
         * @return ETrue if item is selectable
         */
        TBool ItemSelectedL();

    private: // Implementation
        CESMRResponseField(MESMRResponseObserver* aResponseObserver);
        void ConstructL();
        void HandleCancelledEventItemsL( MESMRCalEntry& aEntry );
        TBool HandleTapEventL( const TPoint& aPosition );

    private:
        /// Own: Conflict popup
        CESMRConflictPopup* iConfPopup ;
        /// Array for the response items. Owns the items.
        RPointerArray <CESMRResponseItem> iResponseItemArray;
        /// Observer for the response
        MESMRResponseObserver* iResponseObserver;
        /// Currently selected index
        TInt iSelectionIndex;
        /// Font for the label text
        const CFont* iFont;
        /// Own: Static TLS data handler
        RESMRPluginExtensionStatic iESMRStatic;
        /// iESMRstatic accessed
        TBool iESMRStaticAccessed;
        /// whether a normal response
        TBool iNormalResponse;
        /// whether a remove response
        TBool iRemoveResponse;
        /// Judge if there is any event conflict with current event.
        TBool iConflicted;
        /// Judge whether it has implemented HandleSingletapEventL
        TBool iPointEvent;
        /// Judge whether the long tap have been consumed.
        /// To avoid the same event be handled by
        /// HandleSingletapEventL() when HandleLongtapEventL().
        TBool iLongTapEventConsumed;

    };


#endif // CESMRRESPONSEFIELD_H
