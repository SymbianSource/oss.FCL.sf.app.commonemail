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
#include <connect/sbdefs.h>

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
    delete iBackupRestoreSubscriber;
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
    iConfigData( 0 ),
    iRestoreStarted( EFalse ),
    iBackupOngoing( EFalse )
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
    iBackupRestoreSubscriber = CPSSubscriber::NewL(
        *this, KUidSystemCategory, conn::KUidBackupRestoreKey );
    iBackupRestoreSubscriber->Subscribe();
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
    if ( !BackupOrRestoreMode() )
        {
        StartObservingL();
        LoadSettingsL(); // mailboxes etc. user changeable data
        LoadConfigurationL(); // internal configuration data
        iObserver->SettingsChangedCallback();
        }
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
// 
// synchronizes the iMailboxArray with Central repository settings
// only existing mailboxes are synchronized
// usecases 
// 1. delete widget from homesceen - removed item from cenrep array
// 2. add widget to homescreen - added item to the end of cenrep array
// 3. change mailbox for widget - changed value of cenrep array item 
// 4. remove mailbox from server - mailbox absence for array item 
// 5. add mailbox to the server (leads to widget adding)
// ---------------------------------------------------------------------------

void CMailCpsSettings::LoadSettingsL()
    {
    FUNC_LOG;
    TInt ret( KErrNone );

    // mailbox keys
    RArray<TUint> cenrepMailboxes; 
    CleanupClosePushL( cenrepMailboxes );
    RArray<TBool> cenrepMailboxesExistence; 
    CleanupClosePushL( cenrepMailboxesExistence );

    // cenrep keys
    RArray<TUint32> keys; 
    CleanupClosePushL( keys );
    GetMailboxNonZeroKeysL( keys );
    TInt dMax( keys.Count() );
    
    cenrepMailboxes.ReserveL( dMax ); // cenrep mailbox ids array
    cenrepMailboxesExistence.ReserveL( dMax ); // auxiliary if it was found in iMailboxArray looping
    
    // initialize array of mailbox keys
    TInt value( 0 );
    TInt i; // main loop
    TInt j; // auxiliary / inner loop
    for ( i = 0; i < dMax; i++ )
        {
         User::LeaveIfError( ret = iCenRep->Get( keys[i], value ));
         cenrepMailboxes.AppendL( static_cast<TUint>(value) );
         cenrepMailboxesExistence.AppendL( EFalse );
        }
    CleanupStack::PopAndDestroy(&keys); 
    
    // Sync array of cenrep keys with iMailboxArray
    // 1. remove from iMailboxArray what is not in cenrep, mark what is. Distinct existence. 
    // 2. remove mailbox ids not detected on the server
    TInt dFound( KErrNotFound );
    for ( i = iMailboxArray.Count()-1; i >= 0; i -- ) // iArray loop
        {
        dFound = cenrepMailboxes.Find( iMailboxArray[i].Id() );
        if ( KErrNotFound != dFound ) // if found in cenrep
            { 
                 // mailbox is in iMailboxArray and in cenrep => check provider
            INFO_1("Mailbox both in cenrep and iMailboxArray: i = %d ", i );
            if ( ! cenrepMailboxesExistence[dFound] )
                { 
                ret = CheckMailboxExistence( iMailboxArray[i] );
                if ( KErrNotFound == ret)
                    {
                    // mailbox was removed from provider => remove from cenrep also
                    // cenrepMailboxes indexed the same way as keys => finding key not needed 
                    ret = iCenRep->Reset( KCMailMailboxIdBase + i );
                    ret = iCenRep->Reset( KCMailPluginIdBase + i );
                    ret = iCenRep->Reset( KCMailWidgetContentIdBase + i );
                    INFO_1("Mailbox removed from cenrep: dFound %d ", dFound );
                    // remove also from all arrays
                    iMailboxArray.Remove(i);
                    cenrepMailboxes.Remove( dFound );
                    cenrepMailboxesExistence.Remove( dFound );
                    dMax--;
                    }
                else 
                    {
                    User::LeaveIfError(ret); // for instance if no memory
                    INFO_1("Mailbox provider check ok: dFound = %d ", dFound );
                    cenrepMailboxesExistence[dFound] = ETrue; // not remove to ensure indexes are the same as in keys
                    // handle also possible mailbox duplications
                    for (j = dFound+1; j<dMax; j++)
                        {
                        if ( cenrepMailboxes[dFound] == cenrepMailboxes[j] )
                            { 
                            cenrepMailboxesExistence[j] = ETrue; 
                            }
                        }
                    }
                 } // else already tested for existence
            }
        else // not found in cenrep
            {
            // mailbox was removed from cenrep => remove from iArray as well
            iMailboxArray.Remove(i);
            INFO_1("Mailbox removed from iMailboxArray: i = %d ", i );
            }
        } // iArray loop
    
    // Add mailboxes existing only in cenrep to iMailboxArray in correct order
    // or check and correct order of mailboxes (swap already dot checked, copy duplicated)
    for ( i = 0; i < dMax; i++ )
        {
        // firstly new mailboxes in cenrep needs to be added to iMailboxArray
        if ( ! cenrepMailboxesExistence[i] ) 
            {
            TFSMailMsgId mailbox; 
            // Find mailbox by this unefficient function because cenrep does not hold pluginID
            if ( KErrNone == ResolveMailbox( cenrepMailboxes[i], mailbox ) )
                {
                iMailboxArray.InsertL( mailbox, i );
                INFO_1("Mailbox added to iMailboxArray: i = %d ", i );
                // ensured iMailboxArray[i].Id() == cenrepMailboxes[i]
                }
            else // KErrNotFound
                {
                // remove not valid mailbox from cenrep arrays
                cenrepMailboxes.Remove( i );
                cenrepMailboxesExistence.Remove( i );
                // remove from cenrep
                ret = iCenRep->Reset( KCMailMailboxIdBase + i );
                ret = iCenRep->Reset( KCMailPluginIdBase + i );
                ret = iCenRep->Reset( KCMailWidgetContentIdBase + i );
                INFO_1("Mailbox removed from cenrep: i = %d ", i );
                // update loop control expression
                dMax--;
                i--;
                }
            continue; // ensured equation or removed item
            } // end of if mailbox not in cenrep

        // Check if iMailboxArray is in correct order on i position
        TInt jMax( iMailboxArray.Count() );
        j = i; // assume iMailboxArray[i].Id() == cenrepMailboxes[i]
        if ( ( i < jMax ) && ( iMailboxArray[i].Id() != cenrepMailboxes[i] ) )
            { // not in correct order try to swap
            for ( j = i+1; j < jMax; j++ )
                {
                if ( iMailboxArray[j].Id() == cenrepMailboxes[i] )
                    { // swap is better from efficiency point of view
                    TFSMailMsgId tmp( iMailboxArray[i] ); 
                    iMailboxArray[i] = iMailboxArray[j]; 
                    iMailboxArray[j] = tmp; 
                    break; // j < jMax here
                    }
                }
            // as previous code does not detect existence of repeating id occurence
            // the loop may not find the repeated mailbox id - then j=jMax
            } 
        // if cenrep has multiplied mailbox widgets then add them 
        if ( j >= jMax ) // swapping did not help or item is missing
            {
            // if i=j=jMax then missing duplicated iArray element
            // if j>i then iArray contains not used element
            // two widgets shows the same mailbox then take already used iMailboxArray value
            dFound = cenrepMailboxes.Find( cenrepMailboxes[i] );
            if ( dFound < i )
                {
                // the arrays are synchronized below up to i, duplication to correct i place
                iMailboxArray.InsertL( iMailboxArray[dFound], i ); 
                }
            else
                {
                // unable to synchronize the arrays - algorithm is wrong
                INFO_1("CMAIL: CMailCpsSettings::LoadSettingsL() : FATAL ALGORITHM ERROR - ARRAY NOT SYNCHRONIZED  i = %d ", i );
                User::Leave( KErrNotFound );
                }
            } // end of j >= jMax 
        
        } // end check order for i = 0..dMax loop 

    // if iMailboxArray has more items than cenrepMailboxes then remove these items
    for( i = iMailboxArray.Count()-1; i >= dMax; i-- )
        {
        iMailboxArray.Remove(i);
        }

    CleanupStack::PopAndDestroy(&cenrepMailboxesExistence);
    CleanupStack::PopAndDestroy(&cenrepMailboxes);
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
	INFO_1("CMAIL CMailCpsSettings::ResolveMailbox():: ListMailBoxes() returns %d", err);
    if( !err ) // KErrNone = 0
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
                err = KErrNone; 
                break;
                }
            }
        }
    mailboxarray.ResetAndDestroy();
    return err;
    }

TInt CMailCpsSettings::CheckMailboxExistence( TFSMailMsgId& aMsg )
    {
    FUNC_LOG;
    CFSMailBox *mbox( NULL );
    TRAPD(err, mbox = iMailClient.GetMailBoxByUidL(aMsg));
    if ( mbox ) // mail box exists
        {
        delete mbox;
        return KErrNone;
        }
    if ( ( KErrNotFound == err ) || ( KErrNone == err ) )
        {
    // mail box not exists, chek if it is not in different plugin 
    // very safe, maybe return KErrNotFound would be enough
        return ResolveMailbox( aMsg.Id(), aMsg );
        }
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

    CleanupStack::PopAndDestroy(&keys); // keys
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

    CleanupStack::PopAndDestroy( &keys ); // keys 

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
            TBuf<KMaxDescLen> cid;
            GetContentId( aMailbox.Id(), 1, cid );
            DissociateWidgetFromSettingL( cid );
            iMailboxArray.Remove( ii );
            break;
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
void CMailCpsSettings::AssociateWidgetToSetting( const TDesC& aContentId,
                                                 const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    TInt setId (GetSettingToAssociate(aContentId));
    if (setId>=0)
        {
        TInt id = aMailbox.Id();
        TUid pluginId = aMailbox.PluginId();
        TInt pId(pluginId.iUid);
        iCenRep->Set( KCMailMailboxIdBase+setId, id );
        iCenRep->Set( KCMailPluginIdBase+setId, pId );
        iCenRep->Set( KCMailWidgetContentIdBase+setId, aContentId );        
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::DissociateWidgetFromSettingL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::DissociateWidgetFromSettingL( const TDesC& aContentId )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < KMaxMailboxCount; i++ )
        {
        TBuf<KMaxDescLen> value;
        TUint32 key( KCMailWidgetContentIdBase + i );
        iCenRep->Get( key, value );
        TInt result = value.Compare(aContentId);
        if (!result)
            {
            iCenRep->Reset( key );
            iCenRep->Reset( KCMailMailboxIdBase + i );
            iCenRep->Reset( KCMailPluginIdBase + i );
            break;
            }
        }        
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::GetSettingToAssociate
// ---------------------------------------------------------------------------
//
TInt CMailCpsSettings::GetSettingToAssociate( const TDesC& aContentId )
    {
    FUNC_LOG;    
    TInt ret( KErrNotFound );
    
    for ( TInt i = 0; i < KMaxMailboxCount; i++ )
        {
        TUint32 key( KCMailWidgetContentIdBase+ i );
        TInt err = iCenRep->Get( key, iCenrepText );
        if ( err == KErrNone )
            {
            if ( iCenrepText.Compare( aContentId ) == 0 )
                {
                ret = i;
                break;
                }
            }
        }
    
    if ( ret < 0 )
        {
        for ( TInt i = 0; i < KMaxMailboxCount; i++ )
            {       
            TUint32 key( KCMailWidgetContentIdBase + i );
            TInt err = iCenRep->Get( key, iCenrepText );
            if ( err == KErrNone )
                {
                if ( iCenrepText.Compare( KDissociated ) == 0 )
                    {
                    ret = i;
                    break;
                    }
                }
            }
        }
    
    return ret;
    }


// ---------------------------------------------------------------------------
// CMailCpsSettings::Associated
// ---------------------------------------------------------------------------
//
TBool CMailCpsSettings::Associated( const TDesC& aContentId )
    {
    FUNC_LOG;    
    TBool ret( EFalse );
    
    for ( TInt i = 0; i < KMaxMailboxCount; i++ )
        {
        TUint32 key( KCMailWidgetContentIdBase + i );
        TInt err = iCenRep->Get( key, iCenrepText );
        if ( err == KErrNone )
            {
            if ( iCenrepText.Compare( aContentId ) == 0 )
                {
                ret = ETrue;
                break;
                }
            }
        }
    
    return ret;
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
// CMailCpsSettings::WidgetCountByMailbox
// ---------------------------------------------------------------------------
//
TInt CMailCpsSettings::WidgetCountByMailbox( TInt aMailboxId )
    {
    FUNC_LOG;
    TInt ret(0);
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {       
        TInt value;
        TUint32 mailboxKey(KCMailMailboxIdBase+i);
        iCenRep->Get( mailboxKey, value );     
        if (aMailboxId == value)
            {
            ret++;
            }
        }
    return ret;
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
// CMailCpsSettings::GetMailboxUidByContentId
// ---------------------------------------------------------------------------
//
TUint CMailCpsSettings::GetMailboxUidByContentId( const TDesC& aContentId )
    {
    FUNC_LOG;
    
    TInt ret( KErrNone );
    for ( TInt i = 0; i < KMaxMailboxCount; i++ )
        {       
        TUint32 key( KCMailWidgetContentIdBase + i );
        TInt err = iCenRep->Get( key, iCenrepText );
        if ( err == KErrNone )
            {
            if ( iCenrepText.Compare( aContentId ) == 0 )
                {
                TUint32 key2( KCMailMailboxIdBase + i );
                err = iCenRep->Get( key2, ret );
                if ( err == KErrNone )
                    {
                    break;
                    }
                }
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
  
    TInt ret( KErrNone );
    for ( TInt i = 0; i < KMaxMailboxCount; i++ )
        {       
        TUint32 key( KCMailWidgetContentIdBase + i );
        TInt err = iCenRep->Get( key, iCenrepText );
        if ( err == KErrNone )
            {
            if ( iCenrepText.Compare( aContentId ) == 0 )
                {
                TUint32 key2( KCMailPluginIdBase + i );
                err = iCenRep->Get( key2, ret );
                if ( err == KErrNone )
                    {
                    break;
                    }
                }
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
    mailboxarray.ResetAndDestroy();
    return ret;
    }

// -----------------------------------------------------------------------------
// CMailCpsSettings::ToggleWidgetNewMailIconL
// -----------------------------------------------------------------------------
void CMailCpsSettings::ToggleWidgetNewMailIconL( TBool aIconOn, const TFSMailMsgId& aMailBox )
    {
    FUNC_LOG;
 
    _LIT( KFormat, "%S%d%S" );
    const TInt KStrLen = 64;
        
    TBuf<KStrLen> mailbox;
    mailbox.Format( KFormat, &KStartSeparator, aMailBox.Id(), &KEndSeparator );
    
    TUint32 key( KCMailMailboxesWithNewMail );
    TInt err = iCenRep->Get( key, iCenrepText );
    
    if ( err == KErrNone )
        {
        TInt pos = iCenrepText.Find( mailbox );
    
        if ( aIconOn )
            {
            if ( pos < 0 ) // Not found
                {
                iCenrepText.Append( mailbox );
                iCenRep->Set( key, iCenrepText );
                }
            }
        else
            {
            if ( pos >= 0 )
                {
                iCenrepText.Delete( pos, mailbox.Length() );
                iCenRep->Set( key, iCenrepText );
                }
            }
        }
    }



// -----------------------------------------------------------------------------
// CMailCpsSettings::GetNewMailStateL
// -----------------------------------------------------------------------------
TBool CMailCpsSettings::GetNewMailStateL( const TFSMailMsgId& aMailBox, TInt aUnreadCount )
    {
    FUNC_LOG;

    _LIT( KFormat, "%S%d%S" );
    const TInt KStrLen = 64;


    TBool ret(EFalse);
    if ( aUnreadCount )
        {
        TBuf<KStrLen> mailbox;
        mailbox.Format( KFormat, &KStartSeparator, aMailBox.Id(), &KEndSeparator );

        TUint32 key( KCMailMailboxesWithNewMail );
        TInt err = iCenRep->Get( key, iCenrepText );
        if ( err == KErrNone )
            {
            TInt pos = iCenrepText.Find( mailbox );
            if ( pos >= 0 )
                {
                ret = ETrue;
                }
            }
        }
    else
        {
        ToggleWidgetNewMailIconL( EFalse, aMailBox );
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::HandlePropertyChangedL
// ---------------------------------------------------------------------------
//
void CMailCpsSettings::HandlePropertyChangedL( const TUid& aCategory, TInt aKey )
    {
    FUNC_LOG;

    if ( aCategory == KUidSystemCategory && aKey == conn::KUidBackupRestoreKey )
        {
        TInt keyVal = 0;
        const TInt error = RProperty::Get(  KUidSystemCategory, conn::KUidBackupRestoreKey, keyVal );
        if( error == KErrNone )
            {
            const conn::TBURPartType partType = 
                    static_cast< conn::TBURPartType >( keyVal & conn::KBURPartTypeMask );

            if ( keyVal != 0 )
                {
                switch( partType )
                    {
                    case conn::EBURRestoreFull:
                    case conn::EBURRestorePartial:
                        iRestoreStarted = ETrue;
                        break;
                    case conn::EBURBackupFull:
                    case conn::EBURBackupPartial:                        
                        iBackupOngoing = ETrue;
                        break;
                    case conn::EBURUnset:
                    case conn::EBURNormal:
                    default:
                        iBackupOngoing = EFalse;
                        break;
                    }
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CMailCpsHandler::BackupOrRestoreMode()
// Check if phone is in backup/restore mode
// ----------------------------------------------------------------------------
//
TBool CMailCpsSettings::BackupOrRestoreMode()
    {
    FUNC_LOG;

    TBool backupOrRestore = EFalse;
    
    if ( iRestoreStarted || iBackupOngoing )
        {
        backupOrRestore = ETrue;
        }
    return backupOrRestore;
    }
