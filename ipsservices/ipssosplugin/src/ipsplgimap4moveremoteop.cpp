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

const TInt KMoveRemoteOpPriority = CActive::EPriorityStandard;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4MoveRemoteOp* CIpsPlgImap4MoveRemoteOp::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TInt aFunctionId,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImImap4GetMailInfo& aGetMailInfo,
    const CMsvEntrySelection& aSel,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId )
    {
    FUNC_LOG;
    CIpsPlgImap4MoveRemoteOp* op = new (ELeave) CIpsPlgImap4MoveRemoteOp(
        aMsvSession,
        aObserverRequestStatus,
        aFunctionId,
        aService,
        aActivityTimer,
        aGetMailInfo,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId );
        
    CleanupStack::PushL( op );
    op->ConstructL( aSel );
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4MoveRemoteOp::CIpsPlgImap4MoveRemoteOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TInt aFunctionId,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImImap4GetMailInfo& aGetMailInfo,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId )
    :
    CIpsPlgOnlineOperation(
    aMsvSession,
    KMoveRemoteOpPriority,
    aObserverRequestStatus,
    aActivityTimer,
    aFSMailBoxId,
    aFSOperationObserver,
    aFSRequestId ),
    iFunctionId(aFunctionId),
    iGetMailInfo(aGetMailInfo)
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

    CIpsPlgImap4ConnectOp* connOp = CIpsPlgImap4ConnectOp::NewL(
        iMsvSession,
        KMoveRemoteOpPriority,
        iStatus, 
        iService,
        *iActivityTimer,
        iFSMailboxId,
        iFSOperationObserver,
        iFSRequestId,
        NULL, // event handler not needed whin plain connect
        ETrue,
        EFalse );
        
    delete iOperation;
    iOperation = connOp;

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
        prog.iGenericProgress.iOperation = 
            TImap4GenericProgress::EMoveWithinService;
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
            TBool connected = STATIC_CAST(CIpsPlgImap4ConnectOp*, iOperation)->Connected();
            if( !connected )
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
            if( err != KErrNone && iOperation )
                {
                iMoveErrorProgress = iOperation->ProgressL().AllocL();
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

        delete iOperation;
        iOperation = NULL;
        iOperation = cEntry->MoveL( *iLocalSel, 
                                    iGetMailInfo.iDestinationFolder, 
                                    iStatus );
        CleanupStack::PopAndDestroy( cEntry ); 
        SetActive();
        }
    else
        {
        SetActive();
        CompleteThis();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4MoveRemoteOp::DoMoveRemoteL()
    {
    FUNC_LOG;
    iState = ERemoteMsgs;
    iStatus = KRequestPending;
    // first element of the CMsvEntrySelection is the service which is then
    // followed by any messages
    if( iRemoteSel->Count() > 1 )
        {
        // Switch operations.
        delete iOperation;
        iOperation = NULL;
    
        // Filters are not used when performing 'move' operation, use normal 
        // getmail info instead
        TPckg<TImImap4GetMailInfo> param( iGetMailInfo );
        InvokeClientMtmAsyncFunctionL( iFunctionId, *iRemoteSel, iService, param );
        SetActive();
        }
    else
        {
        SetActive();
        CompleteThis();
        }
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TInt CIpsPlgImap4MoveRemoteOp::GetEngineProgress( const TDesC8& aProgress )
    {
    FUNC_LOG;
    if( !aProgress.Length() )
        {
        return KErrNone;
        }
    else
        {
        TPckgBuf<TImap4CompoundProgress> paramPack;
        paramPack.Copy( aProgress );
        const TImap4GenericProgress& progress = paramPack().iGenericProgress;

        return progress.iErrorCode;
        }
    }

// class CIpsPlgImap4MoveRemoteOpObserver
//

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
CIpsPlgImap4MoveRemoteOpObserver* CIpsPlgImap4MoveRemoteOpObserver::NewL(
    CMsvSession& aSession, CIpsPlgEventHandler& aEventHandler,
    const TFSMailMsgId& aSourceFolder,
    const RArray<TFSMailMsgId>& aMessageIds )
    {
    FUNC_LOG;
    CIpsPlgImap4MoveRemoteOpObserver* self
        = new ( ELeave ) CIpsPlgImap4MoveRemoteOpObserver( aSession,
            aEventHandler, aSourceFolder.Id() );
    CleanupStack::PushL( self );
    self->ConstructL( aMessageIds );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
CIpsPlgImap4MoveRemoteOpObserver::~CIpsPlgImap4MoveRemoteOpObserver()
    {
    FUNC_LOG;
    delete iSelection;
    }

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
void CIpsPlgImap4MoveRemoteOpObserver::RequestResponseL( TFSProgress aEvent,
    TInt /*aRequestId*/ )
    {
    FUNC_LOG;
    if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled ||
         aEvent.iError != KErrNone )
        {
        // Assumes that still existing entries have not been moved.
        for ( TInt ii = iSelection->Count() - 1; ii >= 0; --ii )
            {
            TMsvId id = iSelection->At(ii);
            TMsvId dummy = KMsvNullIndexEntryIdValue;
            TMsvEntry entry;
            if ( iSession.GetEntry( id, dummy, entry ) != KErrNone )
                {
                iSelection->Delete( ii );
                }
            }

        if ( iSelection->Count() )
            {
            iEventHandler.HandleSessionEventL(
                MMsvSessionObserver::EMsvEntriesMoved,
                iSelection, &iSourceFolderId, &iSourceFolderId );
            }
        }
    }

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
CIpsPlgImap4MoveRemoteOpObserver::CIpsPlgImap4MoveRemoteOpObserver(
    CMsvSession& aSession, CIpsPlgEventHandler& aEventHandler,
    TMsvId aSourceFolderId )
    : iSession( aSession ), iEventHandler( aEventHandler ),
    iSourceFolderId( aSourceFolderId )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------     
void CIpsPlgImap4MoveRemoteOpObserver::ConstructL(
    const RArray<TFSMailMsgId>& aMessageIds )
    {
    FUNC_LOG;
    TInt count = aMessageIds.Count();
    iSelection = new ( ELeave ) CMsvEntrySelection;
    for ( TInt ii = 0; ii < count; ++ii )
        {
        iSelection->AppendL( aMessageIds[ii].Id() );
        }
    }

