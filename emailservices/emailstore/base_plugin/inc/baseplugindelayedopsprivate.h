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
* Description: Support for executing asynchronously certain plugin
* operations.
*/
#ifndef BASEPLUGINDELAYEDOPSPRIVATE_H
#define BASEPLUGINDELAYEDOPSPRIVATE_H


#include "baseplugindelayedops.h"

/**
 * Lets the user enqueue and dequeue asynchronous jobs for handling
 * plugin-related actions. 
 */
NONSHARABLE_CLASS ( CDelayedOpsManager ) :
    public CBase,
    public MDelayedOpsManager
    {

public:

    static CDelayedOpsManager* NewL( CBasePlugin& aPlugin );
    
    virtual ~CDelayedOpsManager();
    
    // MDelayedOpsManager::EnqueueOpL
    virtual void EnqueueOpL( CDelayedOp* aOp );
    
    // MDelayedOpsManager::DequeueOpL
    virtual void DequeueOp( const CDelayedOp& aOp );
    
    // MDelayedOpsManager::Extension1
    virtual TInt Extension1(
        TUint /*aExtensionId*/, TAny*& /*a0*/, TAny* /*a1*/ );


private:
    
    CDelayedOpsManager( CBasePlugin& aPlugin );

    void ConstructL();
    
    void ExecutePendingOps();
    
    CDelayedOpsManager( const CDelayedOpsManager& );
    CDelayedOpsManager& operator= ( const CDelayedOpsManager& );    
    

private:
    CBasePlugin& iPlugin;
    RPointerArray<CDelayedOp> iDelayedOps;
    
    __LOG_DECLARATION
    };


/**
 * 
 */
NONSHARABLE_CLASS( CDelayedDeleteMessagesOp ) : public CDelayedOp
    {

public:

    static CDelayedDeleteMessagesOp* NewLC(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aFolderId,
        const RArray<TFSMailMsgId>& aMessages );
    
    static CDelayedDeleteMessagesOp* NewLC(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aFolderId,
        TMsgStoreId aMsgId );

    virtual ~CDelayedDeleteMessagesOp();

    //CDelayedOp::ExecuteOpL
    virtual TBool ExecuteOpL();

private:

    void ConstructL( const RArray<TFSMailMsgId>& aMessages );
    void ConstructL( TMsgStoreId aMsgId );
    
    CDelayedDeleteMessagesOp(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aFolderId );
    
    CDelayedDeleteMessagesOp( const CDelayedDeleteMessagesOp& );
    CDelayedDeleteMessagesOp& operator= ( const CDelayedDeleteMessagesOp& );    

private:
    
    TMsgStoreId iMailBoxId;
    TMsgStoreId iFolderId;
    RArray<TMsgStoreId> iMessages;
    TBool iImmediateDelete;
    TInt iIndex;
    __LOG_DECLARATION
    };


/**
 * This most likely needs "large-content" mode where the content buffer is not
 * copied in memory but stored on a fast drive and then do a
 * ReplaceContentWithFileL.
 */
NONSHARABLE_CLASS( CDelayedSetContentOp ) : public CDelayedOp
    {

public:

    static CDelayedSetContentOp* NewLC(
       TMsgStoreId aMailBoxId,
       TMsgStoreId aMessageId,
       TMsgStoreId aMessagePartId,
       const TDesC& aContent );

    static CDelayedSetContentOp* NewLC(
       TMsgStoreId aMailBoxId,
       TMsgStoreId aMessageId,
       TMsgStoreId aMessagePartId,
       TInt aContentLength );

    virtual ~CDelayedSetContentOp();

    //CDelayedOp::ExecuteOpL
    virtual TBool ExecuteOpL();
    
private:

    void ConstructL( const TDesC& aContent );
    void ConstructL();

    CDelayedSetContentOp(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aMessageId,
        TMsgStoreId aMessagePartId );

    CDelayedSetContentOp(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aMessageId,
        TMsgStoreId aMessagePartId,
        TInt aContentLength );
    
    CMsgStoreMessagePart* FetchMessagePartLC();    
    
    CDelayedSetContentOp( const CDelayedSetContentOp& );
    CDelayedSetContentOp& operator= ( const CDelayedSetContentOp& );    
    
private:
    
    TMsgStoreId iMailBoxId;
    TMsgStoreId iMessageId;
    TMsgStoreId iMessagePartId;
    HBufC* iContent;
    TInt iContentLength;
    TBool iStepOne;
    
    __LOG_DECLARATION
    };


#endif // BASEPLUGINDELAYEDOPSPRIVATE_H
