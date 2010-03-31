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
* Description: MBUtils settings implementation
*
*/

// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "cmrmbutilssettings.h"
#include "mrmbutilsextensioncrkeys.h"
#include "CFSMailBox.h"
#include "CFSMailClient.h"
#include "emailtrace.h"

#include <e32std.h>
#include <centralrepository.h>
#include <ct/rcpointerarray.h>

namespace {  // codescanner::namespace
// Length for CRC buffer 
const TInt KCRCBufferLen(32);
}

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRMBUtilsSettings::CMRUtilsSettingsMgr
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRMBUtilsSettings::CMRMBUtilsSettings(
        CFSMailClient& aMailClient )
:   iMailClient( aMailClient ) 
    {    
    // No implementation
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::~CMRUtilsSettingsMgr
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRMBUtilsSettings::~CMRMBUtilsSettings()
    {   
    // No implementation
    }

// ----------------------------------------------------------------------------
// CMRMBUtilsSettings::NewL
// ----------------------------------------------------------------------------
//
CMRMBUtilsSettings* CMRMBUtilsSettings::NewL(
        CFSMailClient& aMailClient )
    {
    FUNC_LOG;
    
    CMRMBUtilsSettings* self = 
            new( ELeave ) CMRMBUtilsSettings( aMailClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRMBUtilsSettings::ConstructL()
    {
    FUNC_LOG;
    
    // Not yet implementation
    }

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::ConstructL
// ----------------------------------------------------------------------------
//
void CMRMBUtilsSettings::SetDefaultMailboxSettingL( 
        TInt aId )
    {
    FUNC_LOG;
    
    RCPointerArray<CFSMailBox> mailBoxes;
    CleanupClosePushL( mailBoxes );
        
    TFSMailMsgId msgId;
    iMailClient.ListMailBoxes( msgId, mailBoxes );

    TBool mailboxFound( EFalse );
    TInt mailboxCount( mailBoxes.Count() );
    for (TInt i(0); i < mailboxCount && !mailboxFound; ++i )
        {
        CFSMailBox* mailbox( mailBoxes[i] );
        TFSMailMsgId mailboxId( mailbox->GetId() );
        
        TBuf<KCRCBufferLen> crcBuffer;
        crcBuffer.AppendNum( mailboxId.PluginId().iUid );
        crcBuffer.AppendNum( mailboxId.Id() );
        
        TUint32 cmailMailboxCrc( 0 );
        Mem::Crc32( cmailMailboxCrc, 
                crcBuffer.Ptr(), 
                crcBuffer.Size() );
        
        if ( cmailMailboxCrc == aId )
            {
            SetDefaultCMailMRMailBoxL( 
                    aId,
                    mailboxId.PluginId(),
                    mailboxId.Id() );
            
            mailboxFound = ETrue;
            }
        }
    
    if ( !mailboxFound )
        {
        // CMAIL mailbox not found --> Reset the setting
        SetDefaultCMailMRMailBoxL( 
                KMRUICMailInvalidIntValue,
                TUid::Uid( KMRUICMailInvalidIntValue ),
                KMRUICMailInvalidIntValue );
        }
    
    // mailBoxes
    CleanupStack::PopAndDestroy(); // codescanner::cleanup
    }

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::ConstructL
// ----------------------------------------------------------------------------
//
void CMRMBUtilsSettings::GetDefaultMailboxSettingsL( TInt& aId )
    {
    FUNC_LOG;
    
    TUid mailboxUID;
    TInt mailboxId;
    
    GetDefaultCMailMRMailBoxL( 
            aId,
            mailboxUID,
            mailboxId );
    
    RCPointerArray<CFSMailBox> mailBoxes;
    CleanupClosePushL( mailBoxes );
        
    TFSMailMsgId msgId;
    iMailClient.ListMailBoxes( msgId, mailBoxes );

    TBool mailboxFound( EFalse );
    TInt mailboxCount( mailBoxes.Count() );
    for (TInt i(0); i < mailboxCount && !mailboxFound; ++i )
        {
        CFSMailBox* mailbox( mailBoxes[i] );
        TFSMailMsgId mailboxId( mailbox->GetId() );
        
        TBuf<KCRCBufferLen> crcBuffer;
        crcBuffer.AppendNum( mailboxId.PluginId().iUid );
        crcBuffer.AppendNum( mailboxId.Id() );
        
        TUint32 cmailMailboxCrc( 0 );
        Mem::Crc32( cmailMailboxCrc, 
                crcBuffer.Ptr(), 
                crcBuffer.Size() );
                
        if ( cmailMailboxCrc == aId )
            {
            mailboxFound = ETrue;
            }
        }
    
    if ( !mailboxFound )
        {
        aId = KMRUICMailInvalidIntValue;
        }
    
    // mailBoxes
    CleanupStack::PopAndDestroy(); // codescanner::cleanup    
    }

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::GetDefaultCMailMRMailBoxL
// ----------------------------------------------------------------------------
//    
TInt CMRMBUtilsSettings::GetDefaultCMailMRMailBoxL( 
        TInt& aDefaultBox,
        TUid& aMailboxPluginUID,
        TInt& aMailboxId )
    {
    FUNC_LOG;
    
    TInt retValue( KErrNotFound );
    
    TInt defaultBox( KMRUICMailInvalidIntValue );
    TInt mailboxPluginUID( KMRUICMailInvalidIntValue );
    TInt mailboxId( KMRUICMailInvalidIntValue );    

    aDefaultBox = defaultBox;
    aMailboxPluginUID = TUid::Uid( mailboxPluginUID );
    aMailboxId = mailboxId;
    
    // Create central repository
    CRepository* cenRep = 
            CRepository::NewLC( KCRUidMRDefaultCMailMRMailbox );
    
    // Get default meeting request mailbox id from CentRep
    TInt err = cenRep->Get( KMRUIS60DefMailbox, defaultBox ); 
    if ( KErrNone == err  )
        {
        err = cenRep->Get( KMRUICMailMailboxUID, mailboxPluginUID );         
        if ( KErrNone == err )
            {
            err = cenRep->Get( KMRUICMailMailboxID, mailboxId );
            }
        }
    
    // If some other error than KErrNotFound Leave
    if ( err != KErrNotFound )
        {
        // KErrNone won't leave
        User::LeaveIfError( err ); 
        }
    
    CleanupStack::PopAndDestroy( cenRep );
    
    if ( KErrNone == err )
        {
        aDefaultBox = defaultBox;
        aMailboxPluginUID = TUid::Uid( mailboxPluginUID );
        aMailboxId = mailboxId;        
        }
        
    return retValue;    
    }

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::SetDefaultCMailMRMailBoxL
// ----------------------------------------------------------------------------
//                
TInt CMRMBUtilsSettings::SetDefaultCMailMRMailBoxL( 
        TInt aDefaultBox,
        TUid aMailboxPluginUID,
        TInt aMailboxId )
    {
    FUNC_LOG;
    
    // Create central repository
    CRepository* cenRep = 
            CRepository::NewLC( KCRUidMRDefaultCMailMRMailbox );  
    
    TInt err = cenRep->Set( KMRUIS60DefMailbox, aDefaultBox ); 
    
    if ( err == KErrNone )
        {
        TInt uid( static_cast<TInt>(aMailboxPluginUID.iUid ) );
        err = cenRep->Set( KMRUICMailMailboxUID, uid ); 
        if ( err == KErrNone )
            {
            err = cenRep->Set( KMRUICMailMailboxID, aMailboxId ); 
            }
        }    
    
    CleanupStack::PopAndDestroy( cenRep ); 
    
    User::LeaveIfError( err );
    
    return err;   
    }

// End of file
