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
* Description:  Implementation of the class CPbkxRclSettingItem.
*
*/


#include "emailtrace.h"
#include <pbkxrclengine.rsg>
#include <ecom/ecom.h>
//<cmail>
#include "cpbkxremotecontactlookupprotocoladapter.h"
#include "cpbkxremotecontactlookupprotocolaccount.h"
//</cmail>
#include <StringLoader.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include "cpbkxrclsettingitem.h"
#include "pbkxrclutils.h"
#include "cpbkxrclprotocolenvimpl.h"
#include "pbkxrclengineconstants.h"
#include "pbkxremotecontactlookuppanic.h"

// <cmail>  S60 UID update
const TInt KPbkxRemoteContactLookupAdapterIfUid = 0x2001FE1F;
// </cmail>

_LIT( KEmpty, "Empty" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclSettingItem* CPbkxRclSettingItem::NewL()
    {
    FUNC_LOG;
    CPbkxRclSettingItem* item = CPbkxRclSettingItem::NewLC();
    CleanupStack::Pop( item );
    return item;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::NewLC
// ---------------------------------------------------------------------------
//
CPbkxRclSettingItem* CPbkxRclSettingItem::NewLC()
    {
    FUNC_LOG;
    CPbkxRclSettingItem* item = new ( ELeave ) CPbkxRclSettingItem();
    CleanupStack::PushL( item );
    item->ConstructL();
    return item;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::CPbkxRclSettingItem
// ---------------------------------------------------------------------------
//
CPbkxRclSettingItem::CPbkxRclSettingItem() :
    CAknEnumeratedTextPopupSettingItem( KPbkxRclSettingItemId, iSelectedItem )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::~CPbkxRclSettingItem
// ---------------------------------------------------------------------------
//
CPbkxRclSettingItem::~CPbkxRclSettingItem()
    {
    FUNC_LOG;
    iProtocolAccounts.ResetAndDestroy();
    CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclSettingItem::ConstructL()
    {
    FUNC_LOG;

    // first load resource file
    CCoeEnv* coeEnv = CCoeEnv::Static();
    TFileName dllFileName;
    Dll::FileName( dllFileName );

    TParse parse;
    parse.Set( KResourceFile, &KDC_APP_RESOURCE_DIR, &dllFileName );
    TFileName resourceFile = parse.FullName();
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    
    TRAPD( status,
           iResourceFileOffset = coeEnv->AddResourceFileL( resourceFile ) );
    
    if( status != KErrNone )
        {
    	PbkxRclPanic( EPbkxRclPanicGeneral );		
        }
    else 
        {
        }

    // set empty item text although it should not be used ever
    SetEmptyItemTextL( KEmpty );

    HBufC* settingName = StringLoader::LoadLC( R_QTN_RCL_SETTING_RCL_SERVER );

    // call base class ConstructL, this will create stuff we need
    CAknSettingItem::ConstructL( 
        EFalse, 
        KPbkxRclSettingItemId, 
        *settingName, 
        NULL, 
        R_RCL_SETTING_ITEM_PAGE,
        -1,
        0,
        R_RCL_POPUP_SETTING_TEXTS );

    CleanupStack::PopAndDestroy( settingName );

    // get text arrays
    iEnumTexts = EnumeratedTextArray();
    iPoppedUpTexts = PoppedUpTextArray();

    // get all accounts
    LoadProtocolAccountsL();
    
    // Fills enumeration and popup text array
    FillTextArraysL();

    // sets selected account
    SetSelectedAccountL();

    // load the selection
    LoadL();

    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::EditItemL
// ---------------------------------------------------------------------------
//
void CPbkxRclSettingItem::EditItemL( TBool aCalledFromMenu )
    {
    FUNC_LOG;
    TInt oldSelection = iSelectedItem;
    CAknEnumeratedTextPopupSettingItem::EditItemL( aCalledFromMenu );
    StoreL();

    if ( oldSelection != iSelectedItem )
        {
        StoreSettingL();
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::LoadProtocolAccountsL
// ---------------------------------------------------------------------------
//
void CPbkxRclSettingItem::LoadProtocolAccountsL()
    {
    FUNC_LOG;
    
    // create environment given to plugins
    CPbkxRclProtocolEnvImpl* env = CPbkxRclProtocolEnvImpl::NewLC();

    // load all adapter information
    RImplInfoPtrArray implArray;
    TUid pluginIfUid = {KPbkxRemoteContactLookupAdapterIfUid };
    REComSession::ListImplementationsL( pluginIfUid, implArray );

    // create adapters
    CPbkxRemoteContactLookupProtocolAdapter* adapter = NULL;
    for ( TInt i = 0; i < implArray.Count(); i++ )
        {
        TRAPD( err,
            {
            adapter = CPbkxRemoteContactLookupProtocolAdapter::NewL(
                implArray[i]->ImplementationUid(),
                *env );
            CleanupStack::PushL( adapter );
            adapter->GetAllProtocolAccountsL( iProtocolAccounts );
            CleanupStack::PopAndDestroy( adapter );
            } );
        if ( err != KErrNone )
            {
            // log error
            }
        }
    
    implArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( env );

    // if no protocol accounts found, leave
    if ( iProtocolAccounts.Count() == 0 )
        {
        User::Leave( KErrNotReady );
        }

    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::FillTextArraysL
// ---------------------------------------------------------------------------
//
void CPbkxRclSettingItem::FillTextArraysL()
    {
	FUNC_LOG;

    // add "none" selection in first place
    HBufC* noneText = StringLoader::LoadLC( R_QTN_RCL_SETTING_RCL_NOT_DEFINED );

    CAknEnumeratedText* enumItem = 
        new ( ELeave ) CAknEnumeratedText( 0, noneText );
    CleanupStack::PushL( enumItem );
    iEnumTexts->AppendL( enumItem );
    CleanupStack::Pop( enumItem );
    CleanupStack::Pop( noneText );

    HBufC* notDefinedText = 
        StringLoader::LoadLC( R_QTN_RCL_SETTING_RCL_SERVER_NONE );

    iPoppedUpTexts->AppendL( notDefinedText );

    CleanupStack::Pop( notDefinedText );

    for ( TInt i = 0; i < iProtocolAccounts.Count(); i++ )
        {
        CPbkxRemoteContactLookupProtocolAccount* account = iProtocolAccounts[i];
        TUint accountId = account->Id().iAccountId;
        const TDesC& accountName = account->Name();
        enumItem = 
            new ( ELeave ) CAknEnumeratedText( i + 1, accountName.AllocL() );
        CleanupStack::PushL( enumItem );
        iEnumTexts->AppendL( enumItem );
        iPoppedUpTexts->AppendL( accountName.AllocL() );
        CleanupStack::Pop( enumItem );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::SetSelectedAccountL
// ---------------------------------------------------------------------------
//
void CPbkxRclSettingItem::SetSelectedAccountL()
    {
	FUNC_LOG;
    TPbkxRemoteContactLookupProtocolAccountId id = 
        PbkxRclUtils::DefaultProtocolAccountIdL();
    
    if ( id.iProtocolUid == KNullUid )
        {
        // no default account selected
        iSelectedItem = 0;
        }
    else
        {
        for ( TInt i = 0; i < iProtocolAccounts.Count(); i++ )
            {
            if ( iProtocolAccounts[i]->Id().iProtocolUid == id.iProtocolUid &&
                 iProtocolAccounts[i]->Id().iAccountId == id.iAccountId )
                {
                iSelectedItem = i + 1;
                break;
                }
            }
        
        if ( iSelectedItem == 0 )
            {
            // adapter stored in cenrep not found, store new value
            StoreSettingL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingItem::StoreSettingL
// ---------------------------------------------------------------------------
//
void CPbkxRclSettingItem::StoreSettingL()
    {
	FUNC_LOG;
    if ( iSelectedItem == 0 )
        {
        // none selected
        TPbkxRemoteContactLookupProtocolAccountId emptyId( TUid::Uid( 0 ), 0 );
        PbkxRclUtils::StoreDefaultProtocolAccountIdL( emptyId );
        }
    else
        {
        // none selection is not in the array, so decrease by one
        CPbkxRemoteContactLookupProtocolAccount* account = 
            iProtocolAccounts[iSelectedItem - 1];
        PbkxRclUtils::StoreDefaultProtocolAccountIdL( account->Id() );
        }
    }

