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
* Description: 
*       Move a selection of messages that may or may not be complete
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> priority parameter has been removed

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4MoveRemoteOp* CIpsPlgImap4MoveRemoteOp::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TMsvId& aDestinationFolderId,
    const CMsvEntrySelection& aSelection,    
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId )
    {
    FUNC_LOG;
    CIpsPlgImap4MoveRemoteOp* op = new (ELeave) CIpsPlgImap4MoveRemoteOp(
        aMsvSession,
        aObserverRequestStatus,
        aService,
        aActivityTimer,
        aDestinationFolderId,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId );
        
    CleanupStack::PushL( op );
    op->ConstructL( aSelection );
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
CIpsPlgImap4MoveRemoteOp::CIpsPlgImap4MoveRemoteOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TMsvId& aDestinationFolderId,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId )
    :
    CIpsPlgOnlineOperation(
        aMsvSession,
        aObserverRequestStatus,
        aActivityTimer,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId ),
    iState( EIdle ),
    iDestinationFolderId( aDestinationFolderId )
    {
    FUNC_LOG;
    iService = aService;    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
CIpsPlgImap4MoveRemoteOp::~CIpsPlgImap4MoveRemoteOp()
    {
    FUNC_LOG;
    delete iLocalSel;
    delete iRemoteSel;
    delete iMoveErrorProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::ConstructL( const CMsvEntrySelection& aSel )
    {
    FUNC_LOG;
    BaseConstructL(KUidMsgTypeIMAP4);
    SortMessageSelectionL( aSel );
    DoConnectL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::DoConnectL()
    {
    FUNC_LOG;
    iState = EConnecting;
    iStatus = KRequestPending;

    // <qmail> priority parameter has been removed
    CIpsPlgImap4ConnectOp* connOp = CIpsPlgImap4ConnectOp::NewL(
        iMsvSession,
        iStatus, 
        iService,
        iActivityTimer,
        iFSMailboxId,
        NULL, // no operationobserver for suboperation
        0,    // no requestId needed
        NULL, // event handler not needed whin plain connect
        ETrue ); // do only connect
        
    delete iSubOperation;
    iSubOperation = connOp;

    SetActive();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgImap4MoveRemoteOp::ProgressL()
    {
    FUNC_LOG;
    if( iMoveErrorProgress && (iState == EIdle) )
        {
        // Completed, but with an error during move.
        return *iMoveErrorProgress;
        }
    TImap4SyncProgress progg;
    progg.iErrorCode = KErrNone;
    TPckgBuf<TImap4SyncProgress> param(progg);
    iSyncProgress.Copy(param);
    return iSyncProgress;  
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------  
const TDesC8& CIpsPlgImap4MoveRemoteOp::GetErrorProgressL(TInt aError)
    {
    FUNC_LOG;
    // Called to report a leave in DoRunL()
    if(!iProgressBuf().iGenericProgress.iErrorCode)
        {
        TImap4CompoundProgress& prog = iProgressBuf();
        prog.iGenericProgress.iOperation = TImap4GenericProgress::EMoveWithinService;
        prog.iGenericProgress.iState = TImap4GenericProgress::EMoving;
        prog.iGenericProgress.iErrorCode = aError;
        }
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgImap4MoveRemoteOp::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    if ( iState == EConnecting )
        {
        result.iProgressStatus = TFSProgress::EFSStatus_Connecting;
        }
    else
        {
        result.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::RunL()
    {
    FUNC_LOG;
    TRAPD(err, DoRunL());
    if(err != KErrNone)
        {
        iSyncProgress().iErrorCode = err;
        Complete();
        }
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::DoRunL()
    {
    FUNC_LOG;
    TInt err = iStatus.Int();
    switch(iState)
        {
        case EConnecting:
            {
            // <qmail> Connected() usage
            if ( !Connected() )
                {
                CompleteObserver( KErrCouldNotConnect );
                return;
                }
            DoMoveLocalL();
            }
            break;
        case ELocalMsgs:
            // Local move complete
            if( err != KErrNone )
                {
                // Failed to copy local messages.
                iState = EIdle;
                CompleteObserver( err );
                }
            else
                {
                DoMoveRemoteL();
                }
            break;
        case ERemoteMsgs:
            // Remote move complete.
            if( err != KErrNone && iSubOperation )
                {
                iMoveErrorProgress = iSubOperation->ProgressL().AllocL();
                }
            iState = EIdle;
            // to be considered
            // if this fails, should we move the messages back to local 
            // directory
            CompleteObserver( err );
            break;
        case EIdle:
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::SortMessageSelectionL(const CMsvEntrySelection& aSel)
    {
    FUNC_LOG;
    if ( aSel.Count() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    // Sort messages into complete and incomplete selections.
    iLocalSel = new(ELeave) CMsvEntrySelection;
    iRemoteSel = new(ELeave) CMsvEntrySelection;

    TInt err;
    TMsvId id;
    TMsvId service;
    TMsvEntry tEntry;
    TInt selCount( aSel.Count() );
    for( TInt count(0); count < selCount; count++ )
        {
        id = aSel[count];
        err = iMsvSession.GetEntry( id, service, tEntry );
        if( KErrNone == err )
            {
            // local move is not needed, if the message is not fetched
            if( tEntry.Complete() )
                {
                if ( 0 < count )
                    {
                    // service id is not added to local, 
                    // service is already
                    // added in MoveMessagesL
                    iLocalSel->AppendL( id );
                    }
                }
            iRemoteSel->AppendL( id );
            }
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::Complete()
    {
    FUNC_LOG;
    TRequestStatus* observer = &iObserverRequestStatus;
    User::RequestComplete( observer, KErrNone );
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::DoMoveLocalL()
    {
    FUNC_LOG;
    iState = ELocalMsgs;
    iStatus = KRequestPending;
    if( iLocalSel->Count() )
        {
        // this gets the first msg to be moved
        CMsvEntry* cEntry = iMsvSession.GetEntryL( (*iLocalSel)[0] );
        CleanupStack::PushL( cEntry );
        // find the parent of the moved message...
        TMsvId parent = cEntry->Entry().Parent();
        // and use it as a context
        cEntry->SetEntryL( parent );

        delete iSubOperation;
        iSubOperation = NULL;
        iSubOperation = cEntry->MoveL( *iLocalSel, iDestinationFolderId, iStatus );
        CleanupStack::PopAndDestroy( cEntry ); 
        SetActive();
        }
    else
        {
        // <qmail> SetActive(); moved inside CompleteThis();
        CompleteThis();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::DoMoveRemoteL()
    {
    FUNC_LOG;
    iState = ERemoteMsgs;
    // first element of the CMsvEntrySelection is the service which is then
    // followed by any messages
    if( iRemoteSel->Count() > 1 )
        {
        // <qmail> deletion of iSubOperation is done inside InvokeClientMtmAsyncFunctionL </qmail>
    
        // Filters are not used when performing 'move' operation, use normal 
        // getmail info instead
        TPckgBuf<TImImap4GetMailInfo> paramBuf;
        TImImap4GetMailInfo& mailInfo = paramBuf();
        mailInfo.iMaxEmailSize = KMaxTInt32;
        mailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
        mailInfo.iDestinationFolder = iDestinationFolderId;

        InvokeClientMtmAsyncFunctionL( KIMAP4MTMMoveMailSelectionWhenAlreadyConnected, *iRemoteSel, paramBuf );
        SetActive();
        }
    else
        {
        // <qmail> SetActive(); moved inside CompleteThis();
        CompleteThis();        
        }    
    }
    
// <qmail> removed CIpsPlgImap4MoveRemoteOp::GetEngineProgress( const TDesC8& aProgress )

// <qmail> new func to this op
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgImap4MoveRemoteOp::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeMoveRemoteOp;
    }
