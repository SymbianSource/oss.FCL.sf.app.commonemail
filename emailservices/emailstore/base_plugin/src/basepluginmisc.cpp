/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Email interface implementation + utilities.
*
*/



//<cmail>
#include "MsgStore.h"
#include "MsgStoreSearchCriteria.h"
#include "MFSMailBoxSearchObserver.h"
//</cmail>

#include "BasePlugin.h"
#include "BasePluginPanic.h"
#include "baseplugincommonutils.h"
#include "baseplugindelayedopsprivate.h"

/**
 * Base plugin has no particular id.
 */
EXPORT_C TUint CBasePlugin::GetPluginId()
    {
    return 0;
    }


/**
 *
 */
EXPORT_C TFSMailBoxStatus CBasePlugin::GetMailBoxStatus(
    const TFSMailMsgId& /*aMailBoxId*/ )
    {
    return EFSMailBoxOnline;
    }


/**
 *
 */
EXPORT_C TBool CBasePlugin::MailboxHasCapabilityL(
    TFSMailBoxCapabilities /*aCapability*/,
    TFSMailMsgId /*aMailBoxId*/ )
    {
    return EFalse;
    }


/**
 *
 */
EXPORT_C TInt CBasePlugin::WizardDataAvailableL()
    {
    return KErrNotSupported;
    }


/**
 *
 */
EXPORT_C void CBasePlugin::AuthenticateL(
    MFSMailRequestObserver& /*aOperationObserver*/,
    TInt /*aRequestId*/ )
    {

    }


/**
 */
EXPORT_C void CBasePlugin::SetCredentialsL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TDesC& /* aUsername */,
    const TDesC& /* aPassword */ )
    {
    User::Leave( KErrNotSupported );
    }


/**
 *
 */
EXPORT_C TDesC& CBasePlugin::GetBrandingIdL( const TFSMailMsgId& aMailboxId )
    {
    CMailboxInfo& mailbox = GetMailboxInfoL( aMailboxId.Id() );
    return *mailbox.iBrandingId;
    }


/**
 *
 */
EXPORT_C TInt CBasePlugin::CancelSyncL( const TFSMailMsgId& /*aMailBoxId*/ )
	{
	return KErrNotSupported;
	}


#pragma mark -
#pragma mark ---  SEARCH  ---


/**
 *
 */
class CSearchHandler : public CBase, public MMsgStoreSearchClient
    {
    public:
        CSearchHandler(
            MFSMailBoxSearchObserver& aFsObserver,
            CBasePlugin& aBasePlugin,
            const TFSMailMsgId& aMailBox )
            : iFsObserver( aFsObserver ), iBasePlugin( aBasePlugin ), iMailBox( aMailBox )
            {
            };

        ~CSearchHandler()
            {
            TRAP_IGNORE( iFsObserver.SearchCompletedL() );
            };

    public:
        void MatchFound(
            CMsgStorePropertyContainer*& aMessageSummary )
            {
            TRAP_IGNORE( MatchFoundL( aMessageSummary ) );
            };

        void MatchFoundL(
            CMsgStorePropertyContainer*& aMessageSummary )
            {
            TFSMailMsgId msgId( iBasePlugin.GetPluginId(), aMessageSummary->Id() );

            CFSMailMessage* fsMsg = CFSMailMessage::NewLC( msgId );

            fsMsg->SetContentType( KNullDesC() );
            fsMsg->SetContentDescription( KNullDesC() );
            fsMsg->SetContentDisposition( KNullDesC() );
            fsMsg->SetMailBoxId( iMailBox );

            iBasePlugin.TranslateMsgStorePropsL( iMailBox, *aMessageSummary, *fsMsg );

            CleanupStack::Pop( fsMsg );
            iFsObserver.MatchFoundL( fsMsg );
            };

        void SearchCompleted()
            {
            delete this;
            };

    private:
        MFSMailBoxSearchObserver& iFsObserver;
        CBasePlugin& iBasePlugin;
        TFSMailMsgId iMailBox;
    };


/**
 *
 */
EXPORT_C void CBasePlugin::SearchL(
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aFolderIds,
    const RPointerArray<TDesC>& aSearchStrings,
    const TFSMailSortCriteria& aSortCriteria,
    MFSMailBoxSearchObserver& aSearchObserver )

    {
    /**@ search result details?*/

    RMsgStoreSearchCriteria criteria;
    CleanupClosePushL( criteria );

    criteria.iSortBy = static_cast<TMsgStoreSortByField>( aSortCriteria.iField - 1 );
    criteria.iSortOrder = static_cast<TMsgStoreSortOrder>( aSortCriteria.iOrder );

    TInt count = aSearchStrings.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        criteria.AddSearchStringL( *(aSearchStrings[i]) );
        }

    count = aFolderIds.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        criteria.AddFolderId( aFolderIds[i].Id() );
        }

    criteria.AddResultPropertyL( KMsgStorePropertyReceivedAt );
    criteria.AddResultPropertyL( KMsgStorePropertyFlags );
    criteria.AddResultPropertyL( KMsgStorePropertySize );
    criteria.AddResultPropertyL( KMsgStorePropertyFrom );
    criteria.AddResultPropertyL( KMsgStorePropertySubject );

    /**@ figure the lifecycle mgmnt of the handler.*/
    CSearchHandler* searchHandler = new (ELeave) CSearchHandler( aSearchObserver, *this, aMailBoxId );
    CleanupStack::PushL( searchHandler );

    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    mailBox().SearchL( criteria, *searchHandler );

    CleanupStack::Pop( searchHandler );
    CleanupStack::PopAndDestroy( &criteria );
    }


/**
 *
 */
EXPORT_C void CBasePlugin::CancelSearch(
    const TFSMailMsgId& aMailBoxId )

    {
    TRAP_IGNORE( DoCancelSearchL( aMailBoxId ) );
    }


/**
 *
 */
EXPORT_C void CBasePlugin::ClearSearchResultCache(
    const TFSMailMsgId& aMailBoxId )

    {
    TRAP_IGNORE( DoClearSearchResultCacheL( aMailBoxId ) );
    }


#pragma mark --- "MFSMAILPLUGIN - SYNC PLUGIN CONTROL" ---


/**
 *
 */
EXPORT_C void CBasePlugin::GoOfflineL(
    const TFSMailMsgId& /*aMailBoxId*/ )
    {
    }


/**
 *
 */
EXPORT_C void CBasePlugin::GoOnlineL(
    const TFSMailMsgId& /*aMailBoxId*/ )
    {
    }


/**
 *
 */
EXPORT_C void CBasePlugin::RefreshNowL(
    const TFSMailMsgId& /*aMailBoxId*/,
 	MFSMailRequestObserver& /*aOperationObserver*/,
 	TInt /*aRequestId*/ )
    {

    }


#pragma mark -
#pragma mark --- MRU SUPPORT ---


/**
 *
 */
EXPORT_C MDesCArray* CBasePlugin::GetMrusL(
    const TFSMailMsgId& aMailBoxId )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    RPointerArray<CMsgStoreAddress> addressArray;
    CleanupResetAndDestroyClosePushL( addressArray );
    mailBox().MruAddressesL( addressArray );

    //defione consts to avoid codescanner errors
    const TUint KDefaultGranularity = 8;

    TInt granularity = addressArray.Count() > 0 ? 2 * addressArray.Count() : KDefaultGranularity;
    CDesCArrayFlat* result = new (ELeave) CDesCArrayFlat( granularity );
    CleanupStack::PushL( result );

    TInt count = addressArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        result->AppendL( addressArray[i]->DisplayName() );
        result->AppendL( addressArray[i]->EmailAddress() );
        }

    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( &addressArray );

    return result;
    }


/**
 *
 */
EXPORT_C void CBasePlugin::SetMrusL(
    const TFSMailMsgId& aMailBoxId,
    MDesCArray* aNewMruList )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    __ASSERT_DEBUG(
        0 == ( aNewMruList->MdcaCount() % 2 ),
        ::BasePluginPanic( ESetMrusInvalidAssert ) );

    RPointerArray<CMsgStoreAddress> addressArray;
    CleanupResetAndDestroyClosePushL( addressArray );

    CMsgStoreAddress* address;
    for ( TInt i = 0; i < aNewMruList->MdcaCount(); i+=2 )
        {
        TPtrC display = aNewMruList->MdcaPoint( i );
        TPtrC email = aNewMruList->MdcaPoint( 1 + i );

        address = CMsgStoreAddress::NewL( email, display );
        CleanupStack::PushL( address );
        addressArray.AppendL( address );
        CleanupStack::Pop( address );
        }

    mailBox().AddMruAddressesL( addressArray );
    CleanupStack::PopAndDestroy( &addressArray );
    }


#pragma mark --- "MFSMAILPLUGINAPI - STATUS INFO" ---


/**
 *
 */
EXPORT_C TFSProgress CBasePlugin::StatusL( TInt /*aRequestId*/ )
    {
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, 0 };
    return result;
    }


/**
 * Concrete plugins need to call the base plugin's implementation.
 * @param aRequestId
 */
EXPORT_C void CBasePlugin::CancelL( TInt aRequestId )
    {
    //find the fetch request and notify the observer.
    TInt count = iReqs.Count();
    for ( TInt i = 0; i < count; i++ )
    	{
    	if ( iReqs[i]->iRequestId == aRequestId )
    		{
    		CFetchRequester* request = iReqs[i];

    		TFSProgress progress = TFSProgress();
    		progress.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
    		progress.iCounter = progress.iMaxCount = 1;
    		progress.iError = KErrNone;
    		request->iObserver.RequestResponseL( progress, aRequestId );

    		iReqs.Remove( i );
    		delete request;
    		break;
    		}
    	}
    }


/**
 *
 */
EXPORT_C const TFSProgress CBasePlugin::GetLastSyncStatusL(
    const TFSMailMsgId& /*aMailBoxId*/ )

    {
    User::Leave( KErrNotSupported );

    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, 0 };
    return result;
    }


/**
 *
 */
//<qmail>
EXPORT_C TSSMailSyncState CBasePlugin::CurrentSyncState(
    const TFSMailMsgId& /*aMailboxId*/ )
    {
    return Idle;
    }
//</qmail>

/**
 *
 */
EXPORT_C CMsgStoreAccount* CBasePlugin::GetAccountForMsgBoxL(
    const TFSMailMsgId& aMailboxId )

    {
    CMsgStoreAccount* result = NULL;

    RPointerArray< CMsgStoreAccount > accounts;
    iMsgStore->AccountsL( accounts );
    CleanupResetAndDestroyClosePushL( accounts );

    TInt count = accounts.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( accounts[i]->Owner() == GetPluginId() )
            {
            CMsgStoreMailBox* msbox = iMsgStore->OpenAccountL( *accounts[i] );
            CleanupStack::PushL( msbox );

            if ( msbox->Id() == aMailboxId.Id() )
                {
                CleanupStack::PopAndDestroy( msbox );
                result = accounts[i];
                accounts.Remove( i );
                break;
                }

            CleanupStack::PopAndDestroy( msbox );
            }
        }

    CleanupStack::PopAndDestroy( &accounts );

    return result;
    }

/**
 *
 */
void CBasePlugin::DoCancelSearchL(
    const TFSMailMsgId& aMailBoxId )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    mailBox().CancelSearch();
    }

void CBasePlugin::DoClearSearchResultCacheL(
    const TFSMailMsgId& aMailBoxId )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    mailBox().ClearSearchResultCache();
    }

/**
 *
 */
EXPORT_C /*virtual*/ void CBasePlugin::SetMailboxName(
    const TFSMailMsgId& /*aMailboxId*/,
    const TDesC& /*aMailboxName*/ )
    {
    }

/**
 *
 */
/*protected*/ EXPORT_C void CBasePlugin::ResetCache()
    {
    __LOG_ENTER_SUPPRESS( "ResetCache" );

    ResetBodyCache();

    iCacheLine.iMsgChildren.ResetAndDestroy();
    iCacheLine.iMsgChildren.Close();

    delete iCacheLine.iMsg;
    iCacheLine.iMsg = NULL;

    __LOG_WRITE_INFO( "Reset the base plugin msg and msg parts cache." );
    }

/**
 *
 */
/*protected*/ EXPORT_C void CBasePlugin::ResetBodyCache()
    {
    __LOG_ENTER_SUPPRESS( "ResetCache" );
    iCacheLine.iBodyChildren.ResetAndDestroy();
    iCacheLine.iBodyChildren.Close();

    delete iCacheLine.iBody;
    iCacheLine.iBody = NULL;

    __LOG_WRITE_INFO( "Reset the base plugin body and body parts cache." );
    }

/**
 *
 */
/*protected*/ EXPORT_C CMsgStoreMessage* CBasePlugin::GetCachedMsgL(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMsgId )
    {
    __LOG_ENTER_SUPPRESS( "GetCachedMsgL" );
    CMsgStoreMessage* result = NULL;

    if ( iCacheLine.iMsg )
        {
        if ( aMsgId == iCacheLine.iMsg->Id() )
            {
            __LOG_WRITE8_FORMAT1_INFO( "Using cached msg, id: 0x%X.", aMsgId );
            result = iCacheLine.iMsg;
            }
        else
            {
            ResetCache();
            }
        }

    if ( NULL == result )
        {
        CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId );
        iCacheLine.iMsg = mailBox().FetchMessageL( aMsgId, KMsgStoreInvalidId );
        result = iCacheLine.iMsg;
        }

    return result;
    }


/**
 *
 */
/*protected*/
EXPORT_C CMsgStoreMessagePart* CBasePlugin::GetCachedBodyL(
    TMsgStoreId aPartId )
    {
    __LOG_ENTER_SUPPRESS( "GetCachedBodyL" );
    __ASSERT_DEBUG( NULL != iCacheLine.iMsg,
        BasePluginPanic( EInvalidMsgStoreCacheState ) );

    CMsgStoreMessagePart* result = NULL;

    if ( iCacheLine.iBody && aPartId == iCacheLine.iBody->Id() )
        {
        __LOG_WRITE8_FORMAT1_INFO(
            "Using cached body part, id: 0x%X.", aPartId );
        result = iCacheLine.iBody;
        }
    else
        {
        ResetBodyCache();
        iCacheLine.iBody = iCacheLine.iMsg->ChildPartL( aPartId, ETrue );
        }

    return result;
    }

/**
 *
 */
/*protected*/
EXPORT_C RPointerArray<CMsgStoreMessagePart>& CBasePlugin::GetCachedMsgChildrenL()
    {
    __LOG_ENTER_SUPPRESS( "GetCachedMsgChildrenL" );

    __ASSERT_DEBUG( NULL != iCacheLine.iMsg,
        BasePluginPanic( EInvalidMsgStoreCacheState ) );

    if ( 0 == iCacheLine.iMsgChildren.Count() )
        {
        iCacheLine.iMsg->ChildPartsL( iCacheLine.iMsgChildren );
        }
    else
        {
        __LOG_WRITE8_FORMAT1_INFO( "Using cached msg children, count: %d.",
            iCacheLine.iMsgChildren.Count() );
        }

    return iCacheLine.iMsgChildren;
    }

/**
 *
 */
/*protected*/
EXPORT_C RPointerArray<CMsgStoreMessagePart>& CBasePlugin::GetCachedBodyChildrenL()
    {
    __LOG_ENTER_SUPPRESS( "GetCachedBodyChildrenL" );

    __ASSERT_DEBUG( NULL != iCacheLine.iBody,
        BasePluginPanic( EInvalidMsgStoreCacheState ) );

    if ( 0 == iCacheLine.iBodyChildren.Count() )
        {
        iCacheLine.iBody->ChildPartsL( iCacheLine.iBodyChildren );
        }
    else
        {
        __LOG_WRITE8_FORMAT1_INFO( "Using cached body children, count: %d.",
            iCacheLine.iBodyChildren.Count() );
        }

    return iCacheLine.iBodyChildren;
    }

/**
 *
 */
/*protected*/ EXPORT_C void CBasePlugin::InvalidateCacheIfNecessary(
    TMsgStoreId aId,
    TMsgStoreId aParentId,
    TMsgStoreId aOtherId )
    {
    if ( iCacheLine.iMsg )
        {
        TMsgStoreId id = iCacheLine.iMsg->Id();
        if ( aId == id || aParentId == id || aOtherId == id )
            {
            ResetCache();
            }
        }
    }


/**
 *
 */
/*public*/ EXPORT_C MDelayedOpsManager& CBasePlugin::GetDelayedOpsManager()
    {
    return *iDelayedOpsManager;
    }
