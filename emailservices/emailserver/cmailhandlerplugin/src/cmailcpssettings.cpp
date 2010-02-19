/*
* Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  handles interface to CenRep settings
*
*/


#include "emailtrace.h"
#include <e32base.h>
// CRepository
#include <centralrepository.h>

// Email Framework APIs
//<cmail>
#include "cfsmailcommon.h"
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cfsmailfolder.h"
//</cmail>

#include "cmailcpssettings.h"
#include "cmailwidgetcenrepkeysinternal.h"
#include "cmailcpsifconsts.h"
#include "cmailexternalaccount.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMailCpsSettings::NewL
// ---------------------------------------------------------------------------
//
CMailCpsSettings* CMailCpsSettings::NewL( CFSMailClient& aMailClient )
    {
    FUNC_LOG;
    CMailCpsSettings* self =  new(ELeave) CMailCpsSettings( aMailClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::~CMailCpsSettings
// ---------------------------------------------------------------------------
//
CMailCpsSettings::~CMailCpsSettings()
    {
    FUNC_LOG;
    Cancel();
    iMailboxArray.Close();
    delete iCenRep;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::CMailCpsSettings
// ---------------------------------------------------------------------------
//
CMailCpsSettings::CMailCpsSettings( CFSMailClient& aMailClient ) :
    CActive( EPriorityStandard ),
    iMailClient( aMailClient ),
    iCenRep( NULL ),
    iConfigData( 0 )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::ConstructL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::ConstructL()
    {
    FUNC_LOG;
    // In case there is no repository available, following call will leave
    // Trapping is done by MailServer infrastructure, not by CPS handler
    // In practice, this is fatal to cps handling, and widget won't work
    iCenRep = CRepository::NewL( KCRUidCmailWidget );
    LoadSettingsL(); // mailboxes etc. user changeable data
    LoadConfigurationL(); // internal configuration data
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::RunL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::RunL()
    {
    FUNC_LOG;
    StartObservingL();
    LoadSettingsL(); // mailboxes etc. user changeable data
    LoadConfigurationL(); // internal configuration data
    iObserver->SettingsChangedCallback();
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::DoCancel
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::DoCancel()
    {
    FUNC_LOG;
    // Always returns KErrNone
    iCenRep->NotifyCancel( KCmailPartialKey, KCmailMask );
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::LoadSettingsL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::LoadSettingsL()
    {
    FUNC_LOG;
    // Clean up local settings cache
    iMailboxArray.Reset();
    TInt ret( KErrNone );
    
    // Load mailbox array
    RArray<TUint32> keys;
    CleanupClosePushL( keys );
    GetMailboxNonZeroKeysL( keys );
    const TInt iiMax( keys.Count() );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        TInt value( 0 );
        ret = iCenRep->Get( keys[ii], value );
        if ( ret )
            {
            User::Leave( ret );
            }
        else
            {
            TFSMailMsgId mailbox; 
            ret = ResolveMailbox( value, mailbox );
            if ( ret )
                {
                // Resolving encountered error, ignore this entry
                ret = iCenRep->Reset( KCMailMailboxIdBase+ii );
                ret = iCenRep->Reset( KCMailPluginIdBase+ii );
                ret = iCenRep->Reset( KCMailWidgetContentIdBase+ii );
                if ( ret )
                    {
                    }
                }
            else
                {
                iMailboxArray.AppendL( mailbox );
                }
            }
        }
    CleanupStack::PopAndDestroy(); // CleanupClosePushL( keys )
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::LoadConfigurationL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::LoadConfigurationL()
    {
    FUNC_LOG;
    TInt ret( KErrNone );
    TInt value( 0 );
    ret = iCenRep->Get( KCmailCPConfiguration, value );
    if ( !ret )
        {
        INFO_2("iConfigData: %d -> %d", iConfigData, value);
        iConfigData = static_cast<TInt32>( value );
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetMailboxNonZeroKeysL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::GetMailboxNonZeroKeysL( RArray<TUint32>& aKeys )
    {
    FUNC_LOG;
    TInt ret = iCenRep->FindNeqL( KCmailPartialKeyRange, KCmailRangeMask, 0, aKeys );
    if ( ret != KErrNone && ret != KErrNotFound )
        {
        User::Leave( ret );
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetMailboxNonZeroKeysL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::GetExtMailboxNonZeroKeysL( RArray<TUint32>& aKeys )
    {
    FUNC_LOG;
    TInt ret = iCenRep->FindNeqL( KCmailExtMailboxKeyRange, KCmailExtMailboxRangeMask, 0, aKeys );
    if ( ret != KErrNone && ret != KErrNotFound )
        {
        User::Leave( ret );
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::ResolveMailbox
// ---------------------------------------------------------------------------
//
TInt CMailCpsSettings::ResolveMailbox( const TInt aMailboxId, TFSMailMsgId& aMsg )
    {
    FUNC_LOG;
    RPointerArray<CFSMailBox> mailboxarray;
    TInt err = iMailClient.ListMailBoxes( TFSMailMsgId(), mailboxarray );
    if( !err )
        {
        err = KErrNotFound;
        aMsg.SetId( aMailboxId );
        const TInt iiMax( mailboxarray.Count() );
        for ( TInt ii = 0; ii < iiMax; ii++ )
            {
            if ( mailboxarray[ii]->GetId().Id() == aMsg.Id() )
                {
                // Mailbox found
                aMsg.SetPluginId( mailboxarray[ii]->GetId().PluginId() );
                return err = KErrNone;
                }
            }
        }
    else
        {
        }
    mailboxarray.ResetAndDestroy();
    return err;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::StartObservingL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::StartObservingL( MMailCpsSettingsCallback* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    StartObservingL();
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::StartObservingL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::StartObservingL()
    {
    FUNC_LOG;
    TInt ret = iCenRep->NotifyRequest( KCmailPartialKey, KCmailMask, iStatus );
    if ( ret )
        {
        User::Leave( ret );
        }
    SetActive();
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::StopObserving
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::StopObserving()
    {
    FUNC_LOG;
    Cancel();
    // observer not owned by settings
    iObserver = NULL;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::Mailboxes
// ---------------------------------------------------------------------------
//
RArray<TFSMailMsgId>& CMailCpsSettings::Mailboxes()
    {
    FUNC_LOG;
    return iMailboxArray;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::ExternalMailboxes
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::GetExtMailboxesL( RPointerArray<CMailExternalAccount>& aAccounts )
    {
    FUNC_LOG;
    RArray<TUint32> keys;
    CleanupClosePushL( keys );
    GetExtMailboxNonZeroKeysL( keys );

    for ( TInt i = 0; i < keys.Count(); i++ )
        {
        CMailExternalAccount* account = GetExtMailboxL( keys[i] );
        CleanupStack::PushL( account );
        aAccounts.AppendL( account );
        CleanupStack::Pop( account );
        }

    CleanupStack::PopAndDestroy(); // keys
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetExtMailboxL
// ---------------------------------------------------------------------------
//
CMailExternalAccount* CMailCpsSettings::GetExtMailboxL( TInt aKey )
    {
    FUNC_LOG;
    TInt mailboxId( 0 );
    TInt pluginId( 0 );
    HBufC* contentIdBuf = HBufC::NewL( KMaxDescLen );
    TPtr contentId = contentIdBuf->Des();

    User::LeaveIfError( iCenRep->Get( aKey, mailboxId ) );
    User::LeaveIfError( iCenRep->Get( aKey + KCMailExtPluginIdOffset, pluginId ) );
    User::LeaveIfError( iCenRep->Get( aKey + KCMailExtWidgetCidOffset, contentId ) );

    return CMailExternalAccount::NewL(
        mailboxId, pluginId, contentIdBuf );
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::AddMailboxL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::AddMailboxL( const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    
    // Ensure message is not already in settings
    RArray<TUint32> keys;
    CleanupClosePushL( keys );
    GetMailboxNonZeroKeysL( keys );
    const TInt iiMax( keys.Count() );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        TInt value( 0 );
        iCenRep->Get( keys[ii], value );
        if( value == aMailbox.Id() )
            {
            // mailbox already in settings, simply return
            CleanupStack::PopAndDestroy( &keys );
            return;
            }
        }

    // Add mailbox to settings
    keys.Reset();
    TInt ret = iCenRep->FindEqL( KCmailPartialKeyRange, KCmailRangeMask, 0, keys );
    if ( ret != KErrNone && ret != KErrNotFound )
        {
        User::Leave( ret );
        }
    if( keys.Count() > 0 )
        {
        // Casting uint32 to int32 below is safe because following is always true:
        // aMailbox.Id() < 0x7FFFFFFF
        iCenRep->Set( keys[0], static_cast<TInt>( aMailbox.Id() ) );
        }
    else
        {
        }

    CleanupStack::PopAndDestroy(); // CleanupClosePushL( keys );

    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::RemoveMailboxL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::RemoveMailboxL( const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    TInt iiMax( iMailboxArray.Count() );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        if( iMailboxArray[ii].Id() == aMailbox.Id() )
            {
            RemoveMailboxL( ii );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::RemoveMailboxL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::RemoveMailboxL( const TInt aIndex )
    {
    FUNC_LOG;
    
    // Remove mailbox from local array
    TFSMailMsgId mailboxId = iMailboxArray[aIndex];
    iMailboxArray.Remove( aIndex );
    
    // Remove mailbox from widget settings
    RArray<TUint32> keys;
    GetMailboxNonZeroKeysL( keys );
    const TInt iiMax( keys.Count() );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        TInt value( 0 );
        iCenRep->Get( keys[ii], value );
        if( value == mailboxId.Id() )
            {
            iCenRep->Set( keys[ii], 0 );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::MaxRowCount
// ---------------------------------------------------------------------------
//
TInt CMailCpsSettings::MaxRowCount()
    {
    FUNC_LOG;
    return KMaxRowCount;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::MaxMailboxCount
// ---------------------------------------------------------------------------
//
TInt CMailCpsSettings::MaxMailboxCount()
    {
    FUNC_LOG;
    return KMaxMailboxCount;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::AssociateWidgetToSetting
// ---------------------------------------------------------------------------
//
TBool CMailCpsSettings::AssociateWidgetToSetting( const TDesC& aContentId )
    {
    FUNC_LOG;
    TBool alreadyAssociated(ETrue);
    
    if (!IsAlreadyAssociated(aContentId))
        {
        TUint32 key (GetSettingToAssociate());
        iCenRep->Set( key, aContentId );
        alreadyAssociated = EFalse;
        }
    return alreadyAssociated;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::DissociateWidgetFromSettingL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::DissociateWidgetFromSettingL( const TDesC& aContentId )
    {
    FUNC_LOG;
    TUint32 key(0);
    TUint32 mailboxKey(0);

    RemoveFromContentIdListL( aContentId );

    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {
        TBuf<KMaxDescLen> value;
        TUint32 tempKey(KCMailWidgetContentIdBase+i);
        iCenRep->Get( tempKey, value );
        TInt result = value.Compare(aContentId);
        if (!result)
            {
            key = tempKey;
            mailboxKey = KCMailMailboxIdBase + i;
            iCenRep->Reset(key);
            iCenRep->Reset(mailboxKey);
            break;
            }
        }        
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetContentId
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::GetContentId( TInt aMailboxId, TInt aNext, TDes16& aValue )
    {
    FUNC_LOG;
    TInt found(0);
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {       
        TInt value;
        TUint32 mailboxKey(KCMailMailboxIdBase+i);
        iCenRep->Get( mailboxKey, value );     
        if (aMailboxId == value)
            {
            found++;
            if ( found == aNext )
                {
                iCenRep->Get( KCMailWidgetContentIdBase+i, aValue );
                break;
                }            
            }
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::IsSelected
// ---------------------------------------------------------------------------
//
TBool CMailCpsSettings::IsSelected( TInt aId )
    {
    FUNC_LOG;    
    TBool ret(EFalse);
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {       
        TInt value;
        TUint32 mailboxKey(KCMailMailboxIdBase+i);
        iCenRep->Get( mailboxKey, value );
        if (aId == value)
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetSettingToAssociate
// ---------------------------------------------------------------------------
//
TUint CMailCpsSettings::GetMailboxUidByContentId( const TDesC& aContentId )
    {
    FUNC_LOG;
    TInt ret(KErrNone);
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {       
        TBuf<KMaxDescLen> cid;
        TUint32 key(KCMailWidgetContentIdBase+i);
        iCenRep->Get( key, cid );
        TInt result = cid.Compare(aContentId);
        if (!result)
            {
            TUint32 key2(KCMailMailboxIdBase+i);
            iCenRep->Get( key2, ret );
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetPluginUidByContentId
// ---------------------------------------------------------------------------
//
TUint CMailCpsSettings::GetPluginUidByContentId( const TDesC& aContentId )
    {
    FUNC_LOG;
    TInt ret(KErrNone);
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {       
        TBuf<KMaxDescLen> cid;
        TUint32 key(KCMailWidgetContentIdBase+i);
        iCenRep->Get( key, cid );
        TInt result = cid.Compare(aContentId);
        if (!result)
            {
            TUint32 key2(KCMailPluginIdBase+i);
            iCenRep->Get( key2, ret );
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::IsAlreadyAssociated
// ---------------------------------------------------------------------------
//
TBool CMailCpsSettings::IsAlreadyAssociated( const TDesC& aContentId )
    {
    FUNC_LOG;
    TBool ret(EFalse);
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {       
        TBuf<KMaxDescLen> value;
        TUint32 key(KCMailWidgetContentIdBase+i);
        iCenRep->Get( key, value );
        TInt result = value.Compare(aContentId);
        if (!result)
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetSettingToAssociate
// ---------------------------------------------------------------------------
//
TUint32 CMailCpsSettings::GetSettingToAssociate()
    {
    FUNC_LOG;
    TUint32 ret(KErrNone);
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {       
        TBuf<KMaxDescLen> value;
        TUint32 key(KCMailWidgetContentIdBase+i);
        iCenRep->Get( key, value );
        TInt result = value.Compare(KDissociated);
        if (!result)
            {
            ret = KCMailWidgetContentIdBase + i;
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::Configuration
// ---------------------------------------------------------------------------
//
TInt32 CMailCpsSettings::Configuration()
    {
    FUNC_LOG;
    return iConfigData;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::TotalIntMailboxCount
// ---------------------------------------------------------------------------
//
TInt CMailCpsSettings::TotalIntMailboxCount()
    {
    FUNC_LOG;
    RPointerArray<CFSMailBox> mailboxarray;
    iMailClient.ListMailBoxes( TFSMailMsgId(), mailboxarray );    
    TInt ret = mailboxarray.Count(); 
    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::FindFromContentIdListL
// ---------------------------------------------------------------------------
//
TBool CMailCpsSettings::FindFromContentIdListL( const TDesC& aContentId )
    {
    FUNC_LOG;
    TBool ret(EFalse);

    TBuf<KMaxDescLen> cid;    
    cid.Copy(KStartSeparator);    
    cid.Append(aContentId);
    cid.Append(KEndSeparator);
    
    TBuf<KMaxDescLen> value;
    TUint32 key(KCMailContentIdList);
    iCenRep->Get( key, value );
    
    TInt result = value.Find(cid);
    
    if (result >= 0)
        {
        ret = ETrue;        
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::AddToContentIdListL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::AddToContentIdListL( const TDesC& aContentId )
    {
    FUNC_LOG;
    TBuf<KMaxDescLen> value;

    TUint32 key(KCMailContentIdList);
    iCenRep->Get( key, value );

    value.Append(KStartSeparator);
    value.Append(aContentId);
    value.Append(KEndSeparator);  
    
    iCenRep->Set( key, value );    
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::RemoveFromContentIdListL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::RemoveFromContentIdListL( const TDesC& aContentId )
    {
    FUNC_LOG;
    TBuf<KMaxDescLen> cid;
    cid.Copy(KStartSeparator);    
    cid.Append(aContentId);
    cid.Append(KEndSeparator);
    
    TBuf<KMaxDescLen> value;
    TUint32 key(KCMailContentIdList);
    iCenRep->Get( key, value );
    
    TInt result = value.Find(cid);
    if (result >= 0)
        {
        value.Delete(result, cid.Length());
        iCenRep->Set( key, value );        
        }
    }

// -----------------------------------------------------------------------------
// CMailCpsSettings::ToggleWidgetNewMailIconL
// -----------------------------------------------------------------------------
void CMailCpsSettings::ToggleWidgetNewMailIconL( TBool aIconOn, const TFSMailMsgId& aMailBox )
    {
    FUNC_LOG;
    TBuf<KMaxDescLen> mailbox;
    mailbox.Num(aMailBox.Id());

    TBuf<KMaxDescLen> str;
    str.Copy(KStartSeparator);    
    str.Append(mailbox);
    str.Append(KEndSeparator);    

    TBuf<KMaxDescLen> stored;
    TUint32 key(KCMailMailboxesWithNewMail);
    iCenRep->Get( key, stored );
    
    TInt result = stored.Find(str);
    
    if (aIconOn)
        {
        if (result < 0) // Not found
            {
            stored.Append(str);
            iCenRep->Set( key, stored );
            }
        }
    else
        {
        if (result >= 0)
            {
            stored.Delete(result, str.Length());
            iCenRep->Set( key, stored );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMailCpsSettings::GetNewMailState
// -----------------------------------------------------------------------------
TBool CMailCpsSettings::GetNewMailState( const TFSMailMsgId& aMailBox )
    {
    FUNC_LOG;
    TBool ret(EFalse);
    TBuf<KMaxDescLen> mailbox;
    mailbox.Num(aMailBox.Id());

    TBuf<KMaxDescLen> str;
    str.Copy(KStartSeparator);    
    str.Append(mailbox);
    str.Append(KEndSeparator);    

    TBuf<KMaxDescLen> stored;
    TUint32 key(KCMailMailboxesWithNewMail);
    iCenRep->Get( key, stored );
    
    TInt result = stored.Find(str);
    if (result >= 0)
        {
        ret = ETrue;
        }
    return ret;
    }
