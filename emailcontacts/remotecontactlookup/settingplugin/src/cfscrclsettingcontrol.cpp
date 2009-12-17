/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Remote Contact Lookup Extension setting control
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <coecntrl.h>
#include <aknsettingitemlist.h>
//<cmail>
#include "cpbkxremotecontactlookupenv.h"
//</cmail>
#include <fscrclsettingres.rsg>
#include "cfscrclsettingcontrol.h"

// -----------------------------------------------------------------------------
// CFscRclSettingControl::CFscRclSettingControl
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFscRclSettingControl::CFscRclSettingControl()
    {
    FUNC_LOG;
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CFscRclSettingControl::ConstructL
// Actual constructor.
// ----------------------------------------------------------------------------
//
void CFscRclSettingControl::ConstructL()
    {
    FUNC_LOG;
    ConstructFromResourceL( R_RCL_SETTING_LIST );
    }

// -----------------------------------------------------------------------------
// CFscRclSettingControl::NewLC
// CFscRclSettingControl two-phased constructor.
// -----------------------------------------------------------------------------
//
CFscRclSettingControl* CFscRclSettingControl::NewLC()
    {
    FUNC_LOG;
    CFscRclSettingControl* self = new (ELeave)CFscRclSettingControl();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CFscRclSettingControl::NewL
// CFscRclSettingControl two-phased constructor.
// -----------------------------------------------------------------------------
//
CFscRclSettingControl* CFscRclSettingControl::NewL()
    {
    FUNC_LOG;
    CFscRclSettingControl* self = CFscRclSettingControl::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CFscRclSettingControl::~CFscRclSettingControl
// CFscRclSettingControl Destructor
// -----------------------------------------------------------------------------
//
CFscRclSettingControl::~CFscRclSettingControl()
    {
    FUNC_LOG;
    delete iRclEnv;
    }

// -----------------------------------------------------------------------------
//              MPbk2SettingsControlExtension implementation
// -----------------------------------------------------------------------------
// CFscRclSettingControl::ModifySettingItemListL
// Adds RCL settings top item into the Phonebook2 settings list
// -----------------------------------------------------------------------------
//
void CFscRclSettingControl::ModifySettingItemListL( 
        CAknSettingItemList& aSettingItemList )
    {
    FUNC_LOG;
    // Get Phonebook2 settings list
    CAknSettingItemArray *pbk2settings = aSettingItemList.SettingItemArray();
    
    TRAP_IGNORE( iRclEnv = CPbkxRemoteContactLookupEnv::NewL() );
    
    if ( iRclEnv )
        {
        // try to add Remote Contact Lookup setting item
        // if adding fails, continue without rcl setting item
        TRAP_IGNORE(
            {
            MPbkxRemoteContactLookupSettingsUi* rclSettingsUi = 
                iRclEnv->SettingsUiL();
            iRclSettingItem = rclSettingsUi->
                NewDefaultProtocolAccountSelectorSettingItemL();

            pbk2settings->AppendL( iRclSettingItem );
            pbk2settings->RecalculateVisibleIndicesL();
            iRclSettingItem->SetHidden( EFalse );
        
            });    
        }
    }

// -----------------------------------------------------------------------------
// CFscRclSettingControl::CreateSettingItemL
// Creates settings item
// -----------------------------------------------------------------------------
//
CAknSettingItem* CFscRclSettingControl::CreateSettingItemL(TInt /*aSettingId*/)
    {
    FUNC_LOG;
    return iRclSettingItem;
    }

// -----------------------------------------------------------------------------
//                      MPbk2UiReleasable implementation
// -----------------------------------------------------------------------------
// CFscRclSettingControl::DoRelease
// Release this
// -----------------------------------------------------------------------------
//
void CFscRclSettingControl::DoRelease()
    {
    FUNC_LOG;
    delete this;
    }

//  End of File

