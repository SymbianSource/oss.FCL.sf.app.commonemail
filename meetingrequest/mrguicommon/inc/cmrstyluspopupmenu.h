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
* Description:  Meeting request stylus popup menu
 *
*/

#ifndef CMRSTYLUSPOPUPMENU_H
#define CMRSTYLUSPOPUPMENU_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MEikMenuObserver;
class CAknStylusPopUpMenu;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRStylusPopupMenu ): public CBase
    {
    public: // Types
        /**
         * Class for capsulating one menu item in popupmenu
         */
        NONSHARABLE_CLASS( TMenuItem )
            {
            public:
                IMPORT_C TMenuItem();
                IMPORT_C TMenuItem( const TDesC& aTxt, TInt aCommandId );
                
            public: // Data
                /// Own: Text for the menu item
                TPtrC iTxt;
                /// Own: Command id for the menu item
                TInt iCommandId;
            };
        
    public: // d'tor & c'tor
        /**
         * Static c'tor
         */
        IMPORT_C static CMRStylusPopupMenu* NewL( MEikMenuObserver& aObserver );
        
        /**
         * D'tor
         */
        IMPORT_C ~CMRStylusPopupMenu();
        
    public: // Interface
        /**
         * Launch stylus popup with specified items
         * @param aItems Array of menu items to be shown in popup menu
         * @param aPosition Position for the popupmenu
         */
        IMPORT_C void LaunchPopupL( TArray<TMenuItem> aItems, const TPoint &aPosition );

    private: // Implementation
        CMRStylusPopupMenu( MEikMenuObserver& aObserver );
        void ConstructL();
        
    private: // Data
        /// Own: Stylus popup menu
        CAknStylusPopUpMenu* iPopup;
        /// Ref: Menu observer
        MEikMenuObserver& iObserver;
    };

#endif // CMRSTYLUSPOPUPMENU_H

// End of file
