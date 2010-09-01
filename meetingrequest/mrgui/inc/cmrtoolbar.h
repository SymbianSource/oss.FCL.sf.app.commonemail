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
* Description:  MR Toolbar
*
*/
#ifndef CMRTOOLBAR_H
#define CMRTOOLBAR_H

// INCLUDES
#include <e32base.h>
#include <akntoolbarobserver.h>

// FORWARD DECLARATIONS
class CMRAsynchCmd;

// CLASS DECLARATIONS

NONSHARABLE_CLASS( CMRToolbar ) : public CBase, public MAknToolbarObserver
    {
    public:
        /// Context of the toolbar
        enum TToolbarContext
            {
            EMRViewerOrganizer = 1,
            EMRViewerAttendee,
            EMRViewerFromMailApp,
            ECustom = 100 // User has customized the toolbar with own buttons
            };

    public:
        /**
         * C'tor
         */
        static CMRToolbar* NewL();
        /**
         * D'tor
         */
        ~CMRToolbar();
        
    public: // Interface
        /**
         * Initialize toolbar with native toolbar and the context
         * @param aContext Toolbar's context, defined the button set
         * @param aNativeToolbar Avkon toolbar, if NULL the 
         *      Appui::CurrentFixedToolbar() is used to get the toolbar
         */
        void InitializeToolbarL( 
                TToolbarContext aCntx,
                CAknToolbar* aNativeToolbar = NULL );        
        /**
         * Sets the toolbar's, aNativeToolbar or CurrentFixedToolbar, 
         * visiblity as aShowToolbar. Toolbar's visibility is restored in 
         * class' d'tor
         * @param aShowToolbar If ETrue sets toolbar visible, 
         *          otherwise hides toolbar 
         */
        void ShowToolbar( TBool aShowToolbar );
        /**
         * Set on toolbar event observer
         * @param aObserver Pointer to observer, ownership is not transferred
         */
        void SetObserver( MAknToolbarObserver* aObserver );
        
        /*
         * Returns the rect of the current toolbar. If not in use, returns
         * zero rect.
         * @return Rect of the toolbar in use. 
         */
        TRect Rect();
        
    private: // From MAknToolbarObserver
        void DynInitToolbarL ( TInt aResourceId, CAknToolbar *aToolbar );
        void OfferToolbarEventL ( TInt aCommand );
        
    private: // Implementation
        CMRToolbar();
        void ConstructL();
        void ConstructToolbarL( TToolbarContext aCntx );
        void ConstructButtonL( TInt aResId, TInt aIndex );
        void RemovePreviousItems();
        
    private: // Data
        /// Ref: Current application toolbar
        CAknToolbar* iToolbar;
        /// Ref: Observer for toolbar events
        MAknToolbarObserver* iObserver;
        /// Own: Context of the toolbar
        TToolbarContext iCntx;
        /// Own: Item id array, needed for cleaning up the toolbar in d'tor
        RArray<TInt> iItemIdArray;
        /// Own: Asynchronous command notifier
        CMRAsynchCmd* iAsyncCmd;
        /// Own: Flag for the visibility change
        TBool iVisibilityChanged;
        /// Own: copy of previous application toolbar
        CAknToolbar* iPreviousToolbar;
        /// To record if the toolbar in previous view is visible.
        TBool iPreviousVisibility;
        /// Ref: Observer for toolbar visibility
    };

#endif // CMRTOOLBAR_H

// End of file

