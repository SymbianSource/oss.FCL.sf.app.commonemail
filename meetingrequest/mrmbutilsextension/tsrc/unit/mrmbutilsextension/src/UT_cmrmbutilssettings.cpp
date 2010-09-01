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
* Description: 
*
*/

#include "UT_cmrmbutilssettings.h"
#include "mrmbutilsextensioncrkeys.h"
#include "cmrmbutilssettings.h"
#include "cfsmailclient.h"

#include <eunitmacros.h>
#include <eunitdecorators.h>
#include <centralrepository.h>

namespace { // codescanner::namespace


// Initial cenrep value
const TInt KCenRepInitValue( 0 );

// Dummy cenrep value used in tests
const TInt KDummyCenRepValue( 1000 );

// Length for CRC buffer 
const TInt KCRCBufferLen(32);

}

// - Construction -----------------------------------------------------------

UT_CMRMBUtilsSettings::UT_CMRMBUtilsSettings()
    {
    }

UT_CMRMBUtilsSettings::~UT_CMRMBUtilsSettings()
    {
    }

UT_CMRMBUtilsSettings* UT_CMRMBUtilsSettings::NewL()
    {
    UT_CMRMBUtilsSettings* self = UT_CMRMBUtilsSettings::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

UT_CMRMBUtilsSettings* UT_CMRMBUtilsSettings::NewLC()
    {
    UT_CMRMBUtilsSettings* self = new( ELeave ) UT_CMRMBUtilsSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void UT_CMRMBUtilsSettings::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------

void UT_CMRMBUtilsSettings::SetupL(  )
    {
    iMailClient = CFSMailClient::NewL();
    iSettings = CMRMBUtilsSettings::NewL( *iMailClient );
    iCenRep = CRepository::NewL( KCRUidMRDefaultCMailMRMailbox );
    
    ResetcenRep();
    }

void UT_CMRMBUtilsSettings::SetupWithMailboxesL()
    {
    SetupL();
    
    TFSMailMsgId msgId;
    iMailClient->ListMailBoxes( msgId, iMailboxes );
    }

void UT_CMRMBUtilsSettings::ResetcenRep()
    {
    iCenRep->Set( KMRUIS60DefMailbox,   KCenRepInitValue ); 
    iCenRep->Set( KMRUICMailMailboxUID, KCenRepInitValue ); 
    iCenRep->Set( KMRUICMailMailboxID,  KCenRepInitValue );    
    }

void UT_CMRMBUtilsSettings::Teardown()
    {
    ResetcenRep();    

    iMailboxes.ResetAndDestroy();
    iMailClient->Close();
    
    if ( iCenRep )
        {
        delete iCenRep;
        iCenRep = NULL;
        }
    
    if ( iSettings )
        {
        delete iSettings;
        iSettings = NULL;
        }
    }

void UT_CMRMBUtilsSettings::T_SetDefaultMailboxSettingL()
    {
    TInt mailboxCount( iMailboxes.Count() );
    
    EUNIT_ASSERT_DESC(
            iMailboxes.Count(),
            "No any mailboxes defined in the system" );
    
    for (TInt i(0); i < mailboxCount; ++i )
        {
        CFSMailBox* mailbox( iMailboxes[i] );
        TFSMailMsgId mailboxId( mailbox->GetId() );
        
        TBuf<KCRCBufferLen> crcBuffer;
        crcBuffer.AppendNum( mailboxId.PluginId().iUid );
        crcBuffer.AppendNum( mailboxId.Id() );
        
        TUint32 cmailMailboxCrc( 0 );
        Mem::Crc32( cmailMailboxCrc, 
                crcBuffer.Ptr(), 
                crcBuffer.Size() );        
        
        iSettings->SetDefaultMailboxSettingL( cmailMailboxCrc );
        
        TInt cenRepValue( KCenRepInitValue );
        TInt err = iCenRep->Get( KMRUIS60DefMailbox, cenRepValue );
        
        EUNIT_ASSERT_DESC( 
                KErrNone == err,
                "Error reading cenrep ");
        
        EUNIT_ASSERT_EQUALS_DESC( 
                cenRepValue, 
                cmailMailboxCrc, 
                "KMRUIS60DefMailbox mismatch" );

        cenRepValue = KCenRepInitValue;
        err = iCenRep->Get( KMRUICMailMailboxUID, cenRepValue );
        
        EUNIT_ASSERT_DESC( 
                KErrNone == err,
                "Error reading cenrep ");
        
        EUNIT_ASSERT_EQUALS_DESC( 
                cenRepValue, 
                mailboxId.PluginId().iUid, 
                "KMRUICMailMailboxUID mismatch" );

        cenRepValue = KCenRepInitValue;
        err = iCenRep->Get( KMRUICMailMailboxID, cenRepValue );
        
        EUNIT_ASSERT_DESC( 
                KErrNone == err,
                "Error reading cenrep ");
        
        EUNIT_ASSERT_EQUALS_DESC( 
                cenRepValue, 
                mailboxId.Id(), 
                "KMRUICMailMailboxID mismatch" );        
        }
    }

void UT_CMRMBUtilsSettings::T_GetDefaultMailboxSettingsL()
    {
    TInt mailboxCount( iMailboxes.Count() );
    
    EUNIT_ASSERT_DESC(
            iMailboxes.Count(),
            "No any mailboxes defined in the system" );
    
    for (TInt i(0); i < mailboxCount; ++i )
        {
        CFSMailBox* mailbox( iMailboxes[i] );
        TFSMailMsgId mailboxId( mailbox->GetId() );
        
        TBuf<KCRCBufferLen> crcBuffer;
        crcBuffer.AppendNum( mailboxId.PluginId().iUid );
        crcBuffer.AppendNum( mailboxId.Id() );
        
        TUint32 cmailMailboxCrc( 0 );
        Mem::Crc32( cmailMailboxCrc, 
                crcBuffer.Ptr(), 
                crcBuffer.Size() );
        
        TInt S60mailboxId( cmailMailboxCrc );
        TInt cmailMailboxUID( mailboxId.PluginId().iUid );
        TInt cmailMailboxId( mailboxId.Id() );
        
        iCenRep->Set( KMRUIS60DefMailbox,   S60mailboxId );
        iCenRep->Set( KMRUICMailMailboxUID, cmailMailboxUID ); 
        iCenRep->Set( KMRUICMailMailboxID,  cmailMailboxId );
        
        S60mailboxId = KCenRepInitValue;
        iSettings->GetDefaultMailboxSettingsL( S60mailboxId );
        
        EUNIT_ASSERT_EQUALS_DESC( 
                S60mailboxId, 
                cmailMailboxCrc, 
                "GetDefaultMailboxSettingsL mismatch" ); 
        }
    }

void UT_CMRMBUtilsSettings::T_CenRepSetDefaultMailboxSettingL()
    {
    iSettings->SetDefaultCMailMRMailBoxL( 
            KDummyCenRepValue,
            TUid::Uid( KDummyCenRepValue ),
            KDummyCenRepValue );
    
    TInt cenRepValue( KCenRepInitValue );
    TInt err = iCenRep->Get( KMRUIS60DefMailbox, cenRepValue );
    
    EUNIT_ASSERT_DESC( 
            KErrNone == err,
            "Error reading cenrep ");
    
    EUNIT_ASSERT_EQUALS_DESC( 
            cenRepValue, 
            KDummyCenRepValue, 
            "KMRUIS60DefMailbox mismatch" );

    cenRepValue = KCenRepInitValue;
    err = iCenRep->Get( KMRUICMailMailboxUID, cenRepValue );
    
    EUNIT_ASSERT_DESC( 
            KErrNone == err,
            "Error reading cenrep ");
    
    EUNIT_ASSERT_EQUALS_DESC( 
            cenRepValue, 
            KDummyCenRepValue, 
            "KMRUICMailMailboxUID mismatch" );

    cenRepValue = KCenRepInitValue;
    err = iCenRep->Get( KMRUICMailMailboxID, cenRepValue );
    
    EUNIT_ASSERT_DESC( 
            KErrNone == err,
            "Error reading cenrep ");
    
    EUNIT_ASSERT_EQUALS_DESC( 
            cenRepValue, 
            KDummyCenRepValue, 
            "KMRUICMailMailboxID mismatch" );    
    }

void UT_CMRMBUtilsSettings::T_CenRepGetDefaultMailboxSettingsL()
    {    
    iCenRep->Set( KMRUIS60DefMailbox,   KDummyCenRepValue ); 
    iCenRep->Set( KMRUICMailMailboxUID, KDummyCenRepValue ); 
    iCenRep->Set( KMRUICMailMailboxID,  KDummyCenRepValue );      
    
    TInt defaultBox( KCenRepInitValue );
    TUid mailboxPluginUID = TUid::Uid( KCenRepInitValue );
    TInt mailboxId = KCenRepInitValue;
    
    iSettings->GetDefaultCMailMRMailBoxL( 
            defaultBox,
            mailboxPluginUID,
            mailboxId );
    
    EUNIT_ASSERT_EQUALS_DESC( 
            defaultBox, 
            KDummyCenRepValue, 
            "KMRUIS60DefMailbox mismatch" );  
    
    EUNIT_ASSERT_EQUALS_DESC( 
            mailboxPluginUID.iUid, 
            KDummyCenRepValue, 
            "KMRUICMailMailboxUID mismatch" );
    
    EUNIT_ASSERT_EQUALS_DESC( 
            mailboxId, 
            KDummyCenRepValue, 
            "KMRUICMailMailboxID mismatch" );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CMRMBUtilsSettings,
    "Unit test: mrmbutilsextension",
    "UNIT" )

EUNIT_TEST(
    "Testing setting default mailbox",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupWithMailboxesL, T_SetDefaultMailboxSettingL, Teardown)

EUNIT_TEST(
    "Testing getting default mailbox",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupWithMailboxesL, T_GetDefaultMailboxSettingsL, Teardown)    
    
EUNIT_TEST(
    "Testing setting default mailbox to cenrep",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupL, T_CenRepSetDefaultMailboxSettingL, Teardown)
    
EUNIT_TEST(
    "Testing getting default mailbox from cenrep",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupL, T_CenRepGetDefaultMailboxSettingsL, Teardown)
    
EUNIT_END_TEST_TABLE

// EOF
