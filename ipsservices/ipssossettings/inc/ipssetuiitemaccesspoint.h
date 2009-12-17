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
* Description:  Access point item declearation.
*
*/


#ifndef CIPSSETUIITEMACCESSPOINT_H
#define CIPSSETUIITEMACCESSPOINT_H

#include <cdbcols.h>                //TCommDbDialogPref
#include <iapprefs.h>               //TImIAPChoice
#include <cmmanager.h>              //RCmManager
#include <ConnectionUiUtilities.h>  //TCuuAlwaysAskResults
#include <cmapplicationsettingsui.h>

#include "ipssetuiitemlink.h"       //CIpsSetUiItemLink
#include "ipssetutilsconsts.h"      //TIpsSetUiSettingsType

/**
*
*  @lib IpsSosSettings.lib
*  @since FS v1.0
*/
class CIpsSetUiItemAccessPoint : public CIpsSetUiItemLink
    {
    public:  // Constructors and destructor
    
        /**
        * Creates object from CIpsSetUiItem and leaves it to cleanup stack
        * @return, Constructed object
        */
        static CIpsSetUiItemAccessPoint* NewLC();
    
        /**
        * Create object from CIpsSetUiItem
        * @return, Constructed object
        */
        static CIpsSetUiItemAccessPoint* NewL();
    
        /**
        * Destructor
        */
        virtual ~CIpsSetUiItemAccessPoint();
    
    public: // Operators
    
        /**
        * @param aApItem Copies the contents of the item.
        */
        CIpsSetUiItemAccessPoint& operator=( const CIpsSetUiItemAccessPoint& aApItem );
    
        /**
        * Mother of all access point handling.
        *
        * @return result of the editing the access point.
        */
        TInt LaunchL();
    
        /**
        * @param aIapChoice Iap settings used for initialization.
        */
        void InitL( const TImIAPChoice& aIapChoice );
    
        /**
        * Updates the setting item text
        */
        void UpdateL();
        
        /**
        * Get the access point ID
        */
        TUint32 GetIapIdL();
        
        /**
        * Returns the name of the specified access point
        *
        * @return HBufC* Access point name
        */
        HBufC* AccessPointNameLC( TInt aIndex );
    
    protected:  // Constructors
    
        /**
        * Default constructor for classCIpsSetUiItem
        *
        * @return Constructed object
        */
        CIpsSetUiItemAccessPoint();
    
        /**
        * Symbian 2-phase constructor
        */
        void ConstructL();
    
    private:            // New Functions
    
        /**
        * Launches the access point page using networking services.
        *
        * @param aWapId Output parameter to selected Wap ID.
        * @param aBeaerFilter Filter for supported bearers.
        * @return KErrNone for no errors.
        */
        TInt LaunchIapPageL( TUint32& aIapId );
    
        /**
        * Launches always ask page using networking services.
        *
        * @param aAlwaysAsk Always ask selection.
        * @return KErrNone for no errors.
        */
        TInt LaunchAlwaysAskPageL( TCuuAlwaysAskResults& aAlwaysAsk );
    
        /**
        * Initializes Comms Database.
        *
        * @param aBearerFilter Filter for supported bearers.
        */
        void InitializeL( const TInt aBearerFilter );
         
        /**
        * Returns Wap index based on its Id.
        *
        * @return Index of the wap.
        */
        void InitializeSelectionL();
         
        /**
        * Returns Wap index based on its Id.
        *
        * @return Index of the wap.
        */
        HBufC* AccessPointNameLC();
    
    public:     // Data
    
        /**
        * Type of current access point
        */
        TCuuAlwaysAskResults iIapType;
        
        /**
        * Connection method manager
        */
        RCmManager          iCommMethodManager;
    
        /**
        * Slot to store the preferences
        */
        TCommDbDialogPref    iIapPref;
        
        /**
        * Slot to store the IAP Id
        */
        TUint32              iIapId;
    
        /**
        * Another list.
        * Owned.
        */
        RArray<TUint32>     iIapList;
    };

#endif /* CIPSSETUIITEMACCESSPOINT_H */

// End of File
