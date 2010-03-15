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
* Description: This file implements class CEmailClientApi.
*
*/



#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#include <vwsdefpartner.h>
#else
#include <viewcli.h>
#include <vwsdef.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS
#include <coemain.h> // CCoeEnv
#include <coeaui.h> // CCoeAppUi
#include <centralrepository.h>
#include "emailclientapiimpl.h"
#include "emailapiutils.h"
#include "emailmailbox.h"
#include <memailaddress.h>
#include "cfsmailplugin.h"
#include "cfsclientapi.h"
#include "emailclientapiimpldefs.h"
#include "emailmailboxcache.h"
#include "FreestyleEmailUiConstants.h"
#include "emailclientapi.hrh"
#include "freestyleemailcenrepkeys.h"
#include "FreestyleEmailUiConstants.h"
// ---------------------------------------------------------------------------
// CEmailClientApi::MailboxL
// ---------------------------------------------------------------------------
//
MEmailMailbox* CEmailClientApi::MailboxL( const TMailboxId& aId )
    {
    UpdateMailboxInfoCacheL();
    CPluginData* pluginData = MailboxInfoCacheL().PluginDataL( aId );
    MEmailMailbox* mailbox = NULL;
    if ( pluginData )
        {
        mailbox = CEmailMailbox::NewL( *pluginData, aId );
        }
    return mailbox;
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMailbox* CEmailClientApi::MailboxL( const TPtrC& aAddress )
    {
    MEmailMailbox* mailbox = NULL;
    REmailMailboxIdArray mailboxes;
    CleanupClosePushL( mailboxes );
    TInt count( GetMailboxIdsL( mailboxes ) );
    while ( count-- )
        {
        const TMailboxId mailboxId = mailboxes[count];
        MEmailMailbox* refMailbox = MailboxL( mailboxId );
        if ( refMailbox )
            {
            TPtrC address( refMailbox->AddressL()->Address() );
            if ( !address.Compare( aAddress ) )
                {
                mailbox = refMailbox; // addresses match
                count = 0;
                }
            else
                {
                refMailbox->Release();
                }
            }
        }
    CleanupStack::PopAndDestroy();
    // find mailbox or leave KErrNotFound
    if ( !mailbox)
        {
        User::Leave( KErrNotFound );
        }
    return mailbox;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailClientApi::GetMailboxIdsL( 
    REmailMailboxIdArray& aMailboxes )
    {
    UpdateMailboxInfoCacheL();

    aMailboxes.Reset();
    CEmailMailboxCache& mbcache = MailboxInfoCacheL();
    mbcache.GetIdsL( aMailboxes ); 

    const TInt mailboxesFound( aMailboxes.Count() );
    return mailboxesFound;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailClientApi::GetMailboxesL( RMailboxPtrArray& aMailboxes )
    {
    REmailMailboxIdArray mailboxIdArray;
    CleanupClosePushL( mailboxIdArray );
    
    // note! GetMailboxIdsL loads plugin and keeps loaded if it contains at
    // least one mailbox
    TInt count = GetMailboxIdsL( mailboxIdArray );
    while ( count-- )
        {
        // mailbox creation increases plugin ref count by one
        MEmailMailbox* mailbox = MailboxL( mailboxIdArray[count] );
        CleanupReleasePushL( *mailbox );
        aMailboxes.AppendL( mailbox );
        CleanupStack::Pop( mailbox );
        }
    // Created mailboxes still hold plugin references so we can decrease 
    // ref count.
    ReleaseAllPlugins(); 
    
    CleanupStack::PopAndDestroy(); // mailboxIdArray
    return aMailboxes.Count();
    }

// -----------------------------------------------------------------------------
// implement this
// -----------------------------------------------------------------------------
void CEmailClientApi::LaunchEmailL( const TLaunchPolicy aPolicy )
    {
    if ( aPolicy == EShowLastUnseenMailbox)
        {
        CRepository* emailRepository = CRepository::NewL( KFreestyleEmailCenRep );
        CleanupStack::PushL(emailRepository);
        TInt numberOfMailboxes(0);
        TInt inboxId(0);
        TInt pluginId(0);
        emailRepository->Get(KNumberOfMailboxesWithNewEmails,numberOfMailboxes);
        if(numberOfMailboxes > 0)
            {
            emailRepository->Get(KNumberOfMailboxesWithNewEmails+(numberOfMailboxes*2-1),pluginId);            
            emailRepository->Get(KNumberOfMailboxesWithNewEmails+(numberOfMailboxes*2),inboxId);
           
            TUid pluginUid = {pluginId};
            TMailListActivationData activationData;
    
            activationData.iMailBoxId.SetId(inboxId);
            activationData.iMailBoxId.SetPluginId(pluginUid);
            activationData.iFolderId.SetPluginId(pluginUid);
            TPckgBuf<TMailListActivationData> pkgBuf(activationData);
    
            CCoeEnv::Static()->AppUi()->CreateActivateViewEventL( 
                    TVwsViewId(KFSEmailUiUid, MailListId), KStartListWithFolderId, 
                    pkgBuf );
            }
        else
            {
            // We should never come here since the mailbox count should be more than 0 if launchemailL is called
            // but just to be safe launch the email in launchergrid if something went wrong with the repository
            const TUid dummy = {0};
            CCoeEnv::Static()->AppUi()->CreateActivateViewEventL(TVwsViewId(KFSEmailUiUid, AppGridId),
            dummy, KNullDesC8() );        
            }
        CleanupStack::PopAndDestroy();//emailRepository
        }
    else if ( aPolicy == EDefault )
        {
        const TUid dummy = {0};                    
        CCoeEnv::Static()->AppUi()->CreateActivateViewEventL(TVwsViewId(KFSEmailUiUid, AppGridId),
        dummy, KNullDesC8() );
        }
    }    

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CPluginData* CEmailClientApi::TPluginIterator::Next()
    {
    CPluginData* item = NULL;
    if ( iIndex < iArray.Count() )
        {
        item = iArray[ iIndex++ ];
        }
    return item;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailClientApi* CEmailClientApi::NewL()
    {
    CEmailClientApi* self = new ( ELeave ) CEmailClientApi();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailClientApi::~CEmailClientApi()
    {
    // ensure this doesn't held plugin references
    ReleaseAllPlugins();

    // ResetAndDestroy doesn't work because ~CPluginData is private
    TInt count( iPluginDataArray.Count() );
    while ( count-- )
        {
        delete iPluginDataArray[count];
        }
    iPluginDataArray.Close();
    iLoadedPluginsArray.Close();
    delete iMailboxCache;
    delete iClientAPI;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailClientApi::CEmailClientApi()
    {        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailClientApi::CleanupImplInfoPushL( RPointerArray<CImplementationInformation>& aArray )
    {
    TCleanupItem item( &CEmailClientApi::CleanupImplInfo, &aArray );
    CleanupStack::PushL( item );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailClientApi::CleanupImplInfo( TAny* aAny )
    {
    RPointerArray<CImplementationInformation>* array = 
        reinterpret_cast<RPointerArray<CImplementationInformation>*>( aAny );
    array->ResetAndDestroy();
    }
 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailClientApi::ConstructL()
    {
    RPointerArray<CImplementationInformation> implInfoArray;
    REComSession::ListImplementationsL( KFSMailPluginInterface, implInfoArray );
    CleanupImplInfoPushL( implInfoArray );
    TInt err = KErrNone;
    TInt count( implInfoArray.Count() );
    // add implementation UIDs to plugin info array, no instantiation at this
    // phase
    while ( count-- )
        {
        const CImplementationInformation* info = implInfoArray[count];
        CPluginData* pluginData = new ( ELeave ) CPluginData( info->ImplementationUid() );
        err = iPluginDataArray.Append( pluginData );
        if ( err != KErrNone )
            {
            // failed to append, give up
            delete pluginData;
            count = 0;
            }
        }
    iClientAPI = CFSClientAPI::NewL(this);
    CleanupStack::PopAndDestroy(); // CleanupImplInfoPushL
    User::LeaveIfError( err );    
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailClientApi::InterfaceId() const
    {
    return KEmailClientApiInterface;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailClientApi::Release()
    {
    delete this;
    }
       
// -----------------------------------------------------------------------------
// Returns plugin instance from plugin data. If we already have "claimed"
// instance once, prevent increment of reference count 
// -----------------------------------------------------------------------------
CFSMailPlugin* CEmailClientApi::UsePlugin( CPluginData& aPluginData )
    {
    // use 'data' as search key for IndexOfLoadedPluginData()
    TPluginData data( aPluginData.Uid() );
    TPluginData* pluginDataPtr = &data;
    // check if we have plugin already "in use".
    const TInt index( IndexOfLoadedPluginData( data ) );
    if ( index == KErrNotFound )
        { // we don't have plugin instance so take it and add to loaded plugins
        data.iPlugin = aPluginData.ClaimInstance();
        if ( data.iPlugin && iLoadedPluginsArray.Append( data ) != KErrNone )
            {                
            aPluginData.ReleaseInstance(); // failed to append, don't proceed..
            data.iPlugin = NULL;           // but return null
            }          
        }
    else
        {
        // already in use, obtain plugin pointer from the array
        pluginDataPtr = &iLoadedPluginsArray[index];
        }
    return pluginDataPtr->iPlugin;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailClientApi::ReleasePlugin( CPluginData& aPluginData )
    {
    // release plugin but only if it is not already claimed
    TPluginData data( aPluginData.Uid() );
    const TInt index( IndexOfLoadedPluginData( data ) );
     if ( index != KErrNotFound )
        {
        aPluginData.ReleaseInstance();
        iLoadedPluginsArray.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailClientApi::ReleaseAllPlugins()
    {
    for ( TInt i = 0; i < iPluginDataArray.Count(); i++ )
        {
        CPluginData* pdata = iPluginDataArray[i];
        ReleasePlugin( *pdata );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMailboxCache& CEmailClientApi::MailboxInfoCacheL()
    {
    if ( !iMailboxCache) 
        {
        iMailboxCache = CEmailMailboxCache::NewL();
        }
    return *iMailboxCache;
    }

// -----------------------------------------------------------------------------
// Lists all mailboxes in protocol plugins and adds them to cache. Plugins may 
// not be loaded so loading is done. The plugin is kept in memory if it contains
// at least one mailbox to avoid loading again later as typical use case is 
// creation of a mailbox object => the plugin is again needed 
//( see ::GetMailboxesL which calls ReleaseAllPlugins()
// -----------------------------------------------------------------------------
void CEmailClientApi::UpdateMailboxInfoCacheL()
    {
    CEmailMailboxCache& mbcache = MailboxInfoCacheL();
    if ( !mbcache.IsCached() )
        {
        // cache update needed
        mbcache.StartCachingPushL();
        TPluginIterator iter( iPluginDataArray );
        CPluginData* pluginData = iter.Next();
        while ( pluginData )
            {
            TBool containsMailbox( EFalse );
            // loads plugin if needed
            CFSMailPlugin* plugin = UsePlugin( *pluginData );
            if ( plugin )
                {
                // if one plugin fails, it should not block other plugins
                // ==> trap it             
                
                TRAPD( err, containsMailbox = CachePluginMailboxesL(
                    *pluginData, 
                    *plugin ) );
                if ( !containsMailbox || err )
                    {
                    // plugins with no mailboxes (or failed to cache) is
                    // released (unloaded) to optimize RAM usage.
                    ReleasePlugin( *pluginData );
                    }            
                }
            else if ( pluginData->iPluginLoadError == KErrNoMemory )
                {
                // don't continue if OOM
                User::Leave( KErrNoMemory );
                }
            pluginData = iter.Next();
            }
        mbcache.EndCachingPop();    
        }    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TBool CEmailClientApi::CachePluginMailboxesL( CPluginData& aPluginData, CFSMailPlugin& aPlugin )
    {
    TBool containsMailbox( EFalse );
    RArray<TFSMailMsgId> pluginMailboxes;
    CleanupClosePushL( pluginMailboxes );
    aPlugin.ListMailBoxesL( pluginMailboxes );
    TInt mailboxCount = pluginMailboxes.Count();
    while ( mailboxCount-- )
        {
        const TFSMailMsgId& mailboxId = pluginMailboxes[mailboxCount];
        TMailboxId id( mailboxId.Id() );                    
        MailboxInfoCacheL().AddMailboxL( aPluginData, id );
        containsMailbox = ETrue;
        }
    CleanupStack::PopAndDestroy();  // pluginMailboxes
    return containsMailbox;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailClientApi::IndexOfLoadedPluginData( const TPluginData& aPluginData ) const
    {
    TIdentityRelation<TPluginData> relation( CEmailClientApi::PluginDataEquals );    
    return iLoadedPluginsArray.Find( aPluginData, relation );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TBool CEmailClientApi::PluginDataEquals( const TPluginData& a1, const TPluginData& a2 )
    {
    return ( a1.iUid == a2.iUid );
    }

CFSMailPlugin* CEmailClientApi::GetPluginByUid(TUid aUid)
    {
    CPluginData *p = NULL;
    CFSMailPlugin* plugin = NULL;

    TRAP_IGNORE( p = iMailboxCache->PluginDataL(aUid) );
    if ( p )
        {
        plugin = p->ClaimInstance();       
        p->ReleaseInstance();
        }
    return plugin;
    }

// End of file.
