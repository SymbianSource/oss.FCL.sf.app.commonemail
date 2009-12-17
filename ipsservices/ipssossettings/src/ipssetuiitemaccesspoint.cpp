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
* Description: This file implements classes CIpsSetUiItemAccessPoint, AccessPoint, AccessPoint. 
*
*/


#include "emailtrace.h"
#include <featmgr.h>
#include <badesca.h>                // CDesCArrayFlat
#include <StringLoader.h>
#include <cmdestination.h>
#include <cmconnectionmethoddef.h>
#include <ipssossettings.rsg>

#include "ipssetuiitemaccesspoint.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::CIpsSetUiItemAccessPoint()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemAccessPoint::CIpsSetUiItemAccessPoint()
    :
    iIapType( ECuuUserDefined ),
    iIapPref( ECommDbDialogPrefUnknown ),
    iIapId( 0 )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::~CIpsSetUiItemAccessPoint()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemAccessPoint::~CIpsSetUiItemAccessPoint()
    {
    FUNC_LOG;
    iIapList.Close();
    iCommMethodManager.Close();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiItemAccessPoint::ConstructL()
    {
    FUNC_LOG;
    CIpsSetUiItemLink::ConstructL();
    iCommMethodManager.OpenL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemAccessPoint* CIpsSetUiItemAccessPoint::NewL()
    {
    FUNC_LOG;
    CIpsSetUiItemAccessPoint* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemAccessPoint* CIpsSetUiItemAccessPoint::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiItemAccessPoint* self =
        new ( ELeave ) CIpsSetUiItemAccessPoint();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::operator=()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemAccessPoint& CIpsSetUiItemAccessPoint::operator=(
    const CIpsSetUiItemAccessPoint& aIapItem )
    {
    FUNC_LOG;
    iItemFlags = aIapItem.iItemFlags;
    iItemType = aIapItem.iItemType;
    iItemLabel->Copy( *aIapItem.iItemLabel );
    iItemId = aIapItem.iItemId;
    iItemResourceId = aIapItem.iItemResourceId;
    iItemLinkArray = aIapItem.iItemLinkArray;
    iItemSettingText->Copy( *aIapItem.iItemSettingText );
    iItemAppereance = aIapItem.iItemAppereance;
    iIapType = aIapItem.iIapType;
    //iIapRadioButton = aIapItem.iIapRadioButton;
    iIapPref = aIapItem.iIapPref;
    iIapId = aIapItem.iIapId;

    return *this;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::LaunchIapPageL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiItemAccessPoint::LaunchL()
    {
    FUNC_LOG;
    // You have been warned...    
    TInt result = KErrNone;
    TCuuAlwaysAskResults iapType = iIapType;
    TUint32 iapId = iIapId;

    InitializeSelectionL();

    // Always ask page is launched, no matter what.
    result = LaunchAlwaysAskPageL( iapType );

    if( result == KErrNone )
        {
        // Launch the iap page
        if( iapType == ECuuUserDefined )
            {
            result = LaunchIapPageL( iapId );
            }
        }
    
    // Everything is fine, finally update the setting item
    if ( result == KErrNone )
        {
        iIapPref = ECommDbDialogPrefUnknown;
        iIapType = iapType;
        iIapId = iapId;
        }

    // Access point has been changed, update the setting text.
    UpdateL();

    // Return the quit method
    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::UpdateL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiItemAccessPoint::UpdateL()
    {
    FUNC_LOG;
    if( iIapPref == ECommDbDialogPrefPrompt || iIapType == ECuuAlwaysAsk )
        {
        iIapType = ECuuAlwaysAsk;
        iIapPref = ECommDbDialogPrefPrompt;
        iIapId = 0;
        }
    else
        {
        iIapType = ECuuUserDefined;
        iIapPref = ECommDbDialogPrefDoNotPrompt;
        }
    
    // If a static access point is set, the name of the accesspoint can
    // be fetched from the list.
    if( iIapType == ECuuUserDefined )
        {
        iItemSettingText->Copy( AccessPointNameLC()->Des() );
        CleanupStack::PopAndDestroy();  //AccessPointNameLC()
        }
    else
        {
        // Access point set to always ask, display the correct text
        HBufC* text = StringLoader::LoadL( R_FSE_SETTINGS_IAP_ALWAYS_ASK );
        iItemSettingText->Copy( text->Left( KIpsSetUiMaxSettingsLongTextLength ) );
        delete text;
        text = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::AccessPointNameLC()
// ----------------------------------------------------------------------------
//
HBufC* CIpsSetUiItemAccessPoint::AccessPointNameLC()
    {
    FUNC_LOG;
    HBufC* bearerName( NULL );
    RCmConnectionMethod method = iCommMethodManager.ConnectionMethodL( iIapId );
    CleanupClosePushL( method );
    bearerName = method.GetStringAttributeL( CMManager::ECmName );
    CleanupStack::PopAndDestroy();  //method
    CleanupStack::PushL( bearerName );
    return bearerName;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::AccessPointNameLC()
// ----------------------------------------------------------------------------
//
HBufC* CIpsSetUiItemAccessPoint::AccessPointNameLC( TInt aIndex )
    {
    FUNC_LOG;
    HBufC* bearerName( NULL );
    if( aIndex >= 0 && aIndex < iIapList.Count() )
        {
        RCmConnectionMethod method = iCommMethodManager.ConnectionMethodL( iIapList[aIndex] );
        CleanupClosePushL( method );
        bearerName = method.GetStringAttributeL( CMManager::ECmName );
        CleanupStack::PopAndDestroy();  //method
        CleanupStack::PushL( bearerName );
        }
    return bearerName;
    }
        
// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::LaunchIapPageL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiItemAccessPoint::LaunchIapPageL( TUint32& aIapId )
    {
    FUNC_LOG;
    //This function handles the launching of the IAP setting page.
    //using the Connection Method Manager
    TInt result = KErrNotFound;
    //Show the access point list using Connection Method Manager
    TCmSettingSelection selection;
    TBearerFilterArray  filterArray;
    //Set focus to the currently selected access point 
    selection.iId = iIapId;
    selection.iResult = CMManager::EConnectionMethod;
    CCmApplicationSettingsUi* settingsUi = CCmApplicationSettingsUi::NewLC();
    TUint apFilter = CMManager::EShowConnectionMethods;
    result = settingsUi->RunApplicationSettingsL( selection, apFilter, filterArray );
    aIapId = result ? selection.iId : ( TUint32 )KErrNotFound;
    CleanupStack::PopAndDestroy( settingsUi );
    // Return the quit method
    return result ? KErrNone : KErrCancel;
    }    

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::LaunchAlwaysAskPageL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiItemAccessPoint::LaunchAlwaysAskPageL(TCuuAlwaysAskResults& aAlwaysAsk )
    {
    FUNC_LOG;
    // Create utilities object for always ask page
    CConnectionUiUtilities* dialog = CConnectionUiUtilities::NewL();
    CleanupStack::PushL( dialog );

    // Open the always ask -page
    TBool ok = dialog->AlwaysAskPageL( aAlwaysAsk );

    CleanupStack::PopAndDestroy( dialog );
    dialog = NULL;

    // Ok or cancel pressed
    return ok ? KErrNone : KErrCancel;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::InitL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiItemAccessPoint::InitL( const TImIAPChoice& aIapChoice )
    {
    FUNC_LOG;
    iIapPref = aIapChoice.iDialogPref;
    if( iIapPref == ECommDbDialogPrefPrompt )
        {
        iIapId = 0;
        iIapType = ECuuAlwaysAsk;
        }
    else
        {
        InitializeSelectionL();
        iIapId = aIapChoice.iIAP;
        iIapType = ECuuUserDefined;
        }
    UpdateL();
    }    

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::InitializeSelection()
// ----------------------------------------------------------------------------
//    
void CIpsSetUiItemAccessPoint::InitializeSelectionL()    
    {
    FUNC_LOG;
    TBool wlanSupported( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) );
    //fill the array of connection method id's
    iCommMethodManager.ConnectionMethodL( iIapList, ETrue, EFalse, wlanSupported );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemAccessPoint::GetIapIdL()
// ----------------------------------------------------------------------------
//
TUint32 CIpsSetUiItemAccessPoint::GetIapIdL()
    {
    FUNC_LOG;
    return iIapId;
    }

// End of File
  

