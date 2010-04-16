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
* Description:  Email interface implementation.
*
*/
#include <utf.h>

#include "MsgStoreWritablePropertyContainer.h"

#include "baseplugindelayedops.h"
#include "baseplugindelayedopsprivate.h"


///////////////////////////////////////////////////
// CDelayedOp                                    //
///////////////////////////////////////////////////

/**
 * 
 */
/*public virtual*/  CDelayedOp::~CDelayedOp()
    {
    Cancel();
    __LOG_DESTRUCT
    }

/**
 * 
 */
/*public*/  void CDelayedOp::SetContext(
    CBasePlugin& aPlugin, MDelayedOpsManager& aManager )
    {
    iPlugin = &aPlugin;
    iManager = &aManager;
    } 

/**
 * 
 */
/*protected*/  CDelayedOp::CDelayedOp()
    : CAsyncOneShot( CActive::EPriorityIdle )
    {    
    }
    
/**
 * 
 */
/*private virtual*/  void CDelayedOp::RunL()
    {
    __LOG_ENTER_SUPPRESS( "Run" );
    TRAPD( err, ExecuteOpL() );
    
    if ( KErrNone != err )
        {
        __LOG_WRITE8_FORMAT1_INFO(
            "Error while executing delayed operation: %d.", err );
        }
    
    //self-destroy.
    iManager->DequeueOp( *this );
    delete this;
    }
    
/**
 * 
 */
/*protected*/  CBasePlugin& CDelayedOp::GetPlugin()
    {
    return *iPlugin;
    }

/**
 * 
 */
/*private virtual*/  void CDelayedOp::DoCancel()
    {
    }


///////////////////////////////////////////////////
// CDelayedOpsManager                            //
///////////////////////////////////////////////////

/**
 * 
 */
/*static public*/
CDelayedOpsManager* CDelayedOpsManager::NewL( CBasePlugin& aPlugin )
    {
    CDelayedOpsManager* self = new (ELeave) CDelayedOpsManager( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
/**
 * 
 */
/*public virtual*/ CDelayedOpsManager::~CDelayedOpsManager()
    {
    ExecutePendingOps();
    
    iDelayedOps.ResetAndDestroy();
    iDelayedOps.Close();
    
    __LOG_DESTRUCT
    }
    
/**
 * 
 */
/*public virtual*/ void CDelayedOpsManager::EnqueueOpL( CDelayedOp* aOp )
    {
    iDelayedOps.AppendL( aOp );
    aOp->SetContext( iPlugin, *this );        
    aOp->Call();
    }
    
/**
 * 
 */
/*public virtual*/ void CDelayedOpsManager::DequeueOp( const CDelayedOp& aOp )
    {
    TInt count = iDelayedOps.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CDelayedOp* op = iDelayedOps[i];
        if ( &aOp == op )
            {
            iDelayedOps.Remove( i );
            break;
            }
        }
    }
    
/**
 * 
 */
/*public virtual*/ TInt CDelayedOpsManager::Extension1(
    TUint /*aExtensionId*/, TAny*& /*a0*/, TAny* /*a1*/ )
    {
    return KErrNotSupported;
    }

/**
 * 
 */
/*private*/ CDelayedOpsManager::CDelayedOpsManager( CBasePlugin& aPlugin )
    : iPlugin ( aPlugin )
    {
    }

/**
 * 
 */
/*private*/ void CDelayedOpsManager::ConstructL()
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedOpsManager" );
    }

/**
 * 
 */
/*private*/ void CDelayedOpsManager::ExecutePendingOps()
    {
    TInt count = iDelayedOps.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CDelayedOp* op = iDelayedOps[i];
        
        op->Cancel();
        TRAP_IGNORE( op->ExecuteOpL() );
        }
    }    


///////////////////////////////////////////////////
// CDelayedDeleteMessagesOp                      //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static */ CDelayedDeleteMessagesOp* CDelayedDeleteMessagesOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aFolderId,
    const RArray<TFSMailMsgId>& aMessages )
    {
    CDelayedDeleteMessagesOp* self = new (ELeave) CDelayedDeleteMessagesOp(
        aMailBoxId, aFolderId );
    CleanupStack::PushL( self );
    self->ConstructL( aMessages );
    return self;
    }

/**
 * 
 */
/*public static */ CDelayedDeleteMessagesOp* CDelayedDeleteMessagesOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aFolderId,
    TMsgStoreId aMsgId )
    {
    CDelayedDeleteMessagesOp* self = new (ELeave) CDelayedDeleteMessagesOp(
        aMailBoxId, aFolderId );
    CleanupStack::PushL( self );
    self->ConstructL( aMsgId );
    return self;
    }

/**
 * 
 */
/*public virtual*/ CDelayedDeleteMessagesOp::~CDelayedDeleteMessagesOp()
    {
    iMessages.Close();
    __LOG_DESTRUCT
    }


/**
 * 
 */
/*private*/
void CDelayedDeleteMessagesOp::ConstructL(
    const RArray<TFSMailMsgId>& aMessages )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedDeleteMessagesOp" );
            
    TInt count = aMessages.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iMessages.AppendL( aMessages[i].Id() );
        }
    }

/**
 * 
 */
/*private*/
void CDelayedDeleteMessagesOp::ConstructL(
    TMsgStoreId aMsgId )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedDeleteMessagesOp" );
    iImmediateDelete = ETrue;
    iMessages.AppendL( aMsgId );
    }

/**
 * 
 */
/*private*/ CDelayedDeleteMessagesOp::CDelayedDeleteMessagesOp(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aFolderId )
    :
    iMailBoxId( aMailBoxId ), iFolderId( aFolderId ),
    iImmediateDelete( EFalse )
    {
    }

/**
 * 
 */
/*private*/ void CDelayedDeleteMessagesOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" );
    
    CMailboxInfo& mailBoxInfo
        = GetPlugin().GetMailboxInfoL( iMailBoxId );
    CMsgStoreMailBox& mailBox = mailBoxInfo();

    TInt count = iMessages.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        TMsgStoreId msgId = iMessages[i];
        
        if ( EFalse == iImmediateDelete )
            {
            //try to find the message in the deleted items folder.
            CMsgStoreMessage* theMessage = NULL;
            TRAP_IGNORE( theMessage = mailBox.FetchMessageL(
              msgId, mailBoxInfo.iRootFolders.iFolders[EFSDeleted] ) );
            
            if ( NULL == theMessage )
                {
                //if not in deleted items then move it there.
                __LOG_WRITE8_FORMAT1_INFO(
                     "Moving message 0x%X to the deleted items.", msgId );
                mailBox.MoveMessageL(
                   msgId, KMsgStoreInvalidId,
                   mailBoxInfo.iRootFolders.iFolders[EFSDeleted] );
                }
            else
                {
                //in deleted items, really delete it.
                __LOG_WRITE8_FORMAT1_INFO( "Deleting message 0x%X.", msgId );

                delete theMessage;
                mailBox.DeleteMessageL( msgId, iFolderId );
                }
            }
        else
            {
            mailBox.DeleteMessageL( msgId, iFolderId );
            }
        }

    __LOG_EXIT;
    }


///////////////////////////////////////////////////
// CDelayedStorePropertiesOp                     //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static*/
CDelayedSetContentOp* CDelayedSetContentOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId,
    const TDesC& aContent )
    {
    CDelayedSetContentOp* self = new (ELeave) CDelayedSetContentOp(
        aMailBoxId, aMessageId, aMessagePartId );
    CleanupStack::PushL( self );
    self->ConstructL( aContent );
    return self;
    }

/**
 * 
 */
/*public static*/
CDelayedSetContentOp* CDelayedSetContentOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId,
    TInt aContentLength )
    {
    CDelayedSetContentOp* self = new (ELeave) CDelayedSetContentOp(
        aMailBoxId, aMessageId, aMessagePartId, aContentLength );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/**
 * 
 */
/*public virtual*/ CDelayedSetContentOp::~CDelayedSetContentOp()
    {
    delete iContent;
    
    __LOG_DESTRUCT
    }

/**
 * 
 */
/*private*/ void CDelayedSetContentOp::ConstructL( const TDesC& aContent )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedSetContentOp1" );
    
    iContent = aContent.AllocL();
    }

/**
 * 
 */
/*private*/ void CDelayedSetContentOp::ConstructL()
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedSetContentOp2" );
    }

/**
 * CDelayedOp::ExecuteOpL
 */
/*public virtual*/ void CDelayedSetContentOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" )

    CMsgStoreMessagePart* part = FetchMessagePartLC();

    if ( iStepOne )
        {
        part->ReplaceContentL(
            TPtrC8( reinterpret_cast<const TUint8*>(
                iContent->Ptr() ), iContent->Size() ) );
        __LOG_WRITE8_FORMAT1_INFO(
            "Stored the content of part 0x%X.", part->Id() )
        }
    else
        {
        part->AddOrUpdatePropertyL(
            KMsgStorePropertySize, static_cast<TUint32>( iContentLength ) );
        part->AddOrUpdatePropertyL(
            KMsgStorePropertyRetrievedSize,
            static_cast<TUint32>( iContentLength ) );
        
        part->StorePropertiesL();
        __LOG_WRITE8_FORMAT1_INFO(
            "Updated the properties of part 0x%X.", part->Id() )
        }
    
    CleanupStack::PopAndDestroy( part );    
    __LOG_EXIT
    }

/**
 * 
 */
/*private*/ CMsgStoreMessagePart* CDelayedSetContentOp::FetchMessagePartLC()
    {
    __LOG_ENTER_SUPPRESS( "FetchMessagePartLC" )
    CMailboxInfo& mailBox = GetPlugin().GetMailboxInfoL( iMailBoxId );
    
    CMsgStoreMessage* msg = mailBox().FetchMessageL(
        iMessageId, KMsgStoreInvalidId );
    CleanupStack::PushL( msg );
    
    CMsgStoreMessagePart* part;
    if ( KMsgStoreInvalidId == iMessagePartId )
        {
        __LOG_WRITE_INFO( "Fetched the message itself." );
        //watch the cleanupstack, part will point to the same thing thus the
        //popanddestroy is ok.
        part = msg;
        }
    else
        {
        __LOG_WRITE_INFO( "Fetched a child part of the message." );
        part = msg->ChildPartL( iMessagePartId, ETrue );
        CleanupStack::PopAndDestroy( msg );
        CleanupStack::PushL( part );
        }
    
    return part;
    }

/**
 * 
 */
/*private*/ CDelayedSetContentOp::CDelayedSetContentOp(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId )
    : iMailBoxId( aMailBoxId ), iMessageId( aMessageId ),
      iMessagePartId( aMessagePartId ), iStepOne( ETrue )
    {
    }

/**
 * 
 */
/*private*/ CDelayedSetContentOp::CDelayedSetContentOp(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId,
    TInt aContentLength )
    : iMailBoxId( aMailBoxId ), iMessageId( aMessageId ),
      iMessagePartId( aMessagePartId ), iContentLength( aContentLength ),
      iStepOne( EFalse )
    {
    }

///////////////////////////////////////////////////
// CDelayedMessageStorerOp                      //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static */ CDelayedMessageStorerOp* CDelayedMessageStorerOp::NewLC(
        const TFSMailMsgId& aMailBox,
        RPointerArray<CFSMailMessage> &messages,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId)
    {
    CDelayedMessageStorerOp* self = new (ELeave) CDelayedMessageStorerOp(
            aMailBox, messages, aOperationObserver,aRequestId);
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }

/**
 * 
 */
/*public static */ CDelayedMessageStorerOp* CDelayedMessageStorerOp::NewLC(
        RPointerArray<CFSMailMessagePart>& aMessageParts,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId)
    {
    CDelayedMessageStorerOp* self = new (ELeave) CDelayedMessageStorerOp(
            aMessageParts, aOperationObserver, aRequestId);
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }

/**
 * 
 */
/*public virtual*/ CDelayedMessageStorerOp::~CDelayedMessageStorerOp()
    {
    __LOG_DESTRUCT
    iMessages.Reset();
    iMessageParts.Reset();
    if (iDataBuffer)
        {
        delete iDataBuffer;
        iDataBuffer = NULL;
        }
    }

/**
 * 
 */
/*private*/
void CDelayedMessageStorerOp::ConstructL( )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedMessageStorerOp" );
        
    }

/**
 * 
 */
/*private*/ CDelayedMessageStorerOp::CDelayedMessageStorerOp(
    const TFSMailMsgId& aMailBox,
    RPointerArray<CFSMailMessage> &messages,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    : iMailBox( aMailBox ), 
      iOperationObserver( aOperationObserver ),
      iRequestId( aRequestId ),
      iType(EHeaders)
    {
    for(TInt i=0; i < messages.Count(); i++)
        {
        iMessages.Append(messages[i]);
        }
    }

/**
 * 
 */
/*private*/ CDelayedMessageStorerOp::CDelayedMessageStorerOp(
    RPointerArray<CFSMailMessagePart>& aMessageParts,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    :iOperationObserver( aOperationObserver ),
    iRequestId( aRequestId ),
    iType(EParts)
    {
    for(TInt i=0; i < aMessageParts.Count(); i++)
        {
        iMessageParts.Append(aMessageParts[i]);
        }
    }

/**
 * 
 */
/*private*/ void CDelayedMessageStorerOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" );
   
    TFSProgress progress = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, 0 };
    progress.iError = KErrNotFound;
    TInt err(KErrNone);
    
    switch( iType )
        {
        case EHeaders: 
            {
            for ( TInt i = 0; i < iMessages.Count(); i++ )
                {
                TRAP(err, GetPlugin().StoreMessageL(iMailBox, *iMessages[i] ));
                if(err!=KErrNone)
                    {
                    break;
                    }
                }
            break;
            }
            
        case EParts:
            {
                for ( TInt i = 0; i < iMessageParts.Count(); i++ )
                {
                CFSMailMessagePart& part= *iMessageParts[i];
                TFSMailMsgId messageId= part.GetMessageId();
                TFSMailMsgId folderId= part.GetFolderId();
                
                if (part.GetContentType().Compare(KFSMailContentTypeTextPlain) == 0 ||
                        part.GetContentType().Compare(KFSMailContentTypeTextHtml) == 0)
                    {
                    TRAP(err,StorePartL(&part));
                    if(err!=KErrNone)
                        {
                        break;
                        }
                    TRAP(err, GetPlugin().StoreMessagePartL( part.GetMailBoxId(), folderId, messageId, part ));
                    }
                else
                    {
                    TRAP(err, GetPlugin().StoreMessagePartL( part.GetMailBoxId(), folderId, messageId, part ));
                    }
               if(err!=KErrNone)
                    {
                    break;
                    }
                }
            }
        
        default:
        break;
        }
    
    if( err == KErrNone )
        {
        progress.iError = KErrNone;
        progress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        }

    iOperationObserver.RequestResponseL( progress, iRequestId );
    
    __LOG_EXIT;
    }


/**
 * 
 */
void CDelayedMessageStorerOp::StorePartL(
        CFSMailMessagePart* aPart)
    {
    User::LeaveIfNull(aPart);
    
    // Text buffer for html text content
    HBufC* data16 = aPart->GetLocalTextContentLC();

    TPtrC8 ptr8(reinterpret_cast<const TUint8*>( data16->Ptr() ),
            data16->Size() );
    
    //get msgstore part
    CMailboxInfo& mailBox = GetPlugin().GetMailboxInfoL( aPart->GetMailBoxId().Id() );
    
    CMsgStoreMessage* msg = mailBox().FetchMessageL(
            aPart->GetMessageId().Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( msg );
    
    CMsgStoreMessagePart* part= msg->ChildPartL( aPart->GetPartId().Id(), ETrue );
    
    CleanupStack::PopAndDestroy( msg );
    CleanupStack::PushL( part );

    //replace content
    part->ReplaceContentL(ptr8);
    
    CleanupStack::PopAndDestroy( part );
    CleanupStack::PopAndDestroy( data16 );
       
    }


///////////////////////////////////////////////////
// CDelayedMessageToSendOp                      //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static */ CDelayedMessageToSendOp* CDelayedMessageToSendOp::NewLC(
        CBasePlugin& aPlugin,
        const TFSMailMsgId& aMailBox,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId)
    {
    CDelayedMessageToSendOp* self = new (ELeave) CDelayedMessageToSendOp(
            aPlugin,aMailBox, aOperationObserver,aRequestId);
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }


/**
 * 
 */
/*public virtual*/ CDelayedMessageToSendOp::~CDelayedMessageToSendOp()
    {
    __LOG_DESTRUCT
    }

/**
 * 
 */
/*private*/
void CDelayedMessageToSendOp::ConstructL( )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedMessageToSendOp" );
        
    }

/**
 * 
 */
/*private*/ CDelayedMessageToSendOp::CDelayedMessageToSendOp(
    CBasePlugin& aPlugin,
    const TFSMailMsgId& aMailBox,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    : iBasePlugin(aPlugin), 
      iMailBox( aMailBox ), 
      iOperationObserver( aOperationObserver ),
      iRequestId( aRequestId )
    {

    }


/**
 * 
 */
/*private*/ void CDelayedMessageToSendOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" );
   
    TFSProgress progress = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, 0 };
    progress.iError = KErrNotFound;
    TInt err(KErrNone);

    TRAP(err, progress.iParam = iBasePlugin.CreateMessageToSendL( iMailBox ));
    
    if( err == KErrNone  )
        {
        progress.iError = KErrNone;
        progress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        }

    iOperationObserver.RequestResponseL( progress, iRequestId );
    
    __LOG_EXIT;
    }
