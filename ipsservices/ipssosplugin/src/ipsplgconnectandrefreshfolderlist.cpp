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
*       folder refresh operation
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

#include <ipssossettings.rsg>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C CIpsPlgConnectAndRefreshFolderList* 
            CIpsPlgConnectAndRefreshFolderList::NewL(
                    CMsvSession& aSession, 
                    TInt aPriority, 
                    TRequestStatus& aObserverRequestStatus,
                    TMsvId aService, 
                    TFSMailMsgId& aMailboxId, 
                    CMsvEntrySelection& aMsvEntry, 
                    MFSMailRequestObserver& aFSObserver, 
                    CIpsPlgTimerOperation& aTimer )
    {
    FUNC_LOG;
    CIpsPlgConnectAndRefreshFolderList* self=
        new (ELeave) CIpsPlgConnectAndRefreshFolderList( 
                aSession, 
                aPriority, 
                aObserverRequestStatus, 
                aService, 
                aMailboxId, 
                aFSObserver, 
                aTimer );
    CleanupStack::PushL( self );
    self->ConstructL( aMsvEntry );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgConnectAndRefreshFolderList::CIpsPlgConnectAndRefreshFolderList(
    CMsvSession& aSession, 
    TInt aPriority, 
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService, 
    TFSMailMsgId& aMailboxId, 
    MFSMailRequestObserver& aFSObserver,
    CIpsPlgTimerOperation& aTimer )
	:
	CIpsPlgOnlineOperation(
	        aSession,
	        aPriority,
	        aObserverRequestStatus,
	        aTimer,
	        aMailboxId,
	        aFSObserver,
	        0, // FSRequestId
	        EFalse), // SignallingAllowed
	iState( EIdle ),
	iMailboxId( aMailboxId ),
    iSession( aSession ),
    iService( aService ),
    iOperation( NULL ),
    iTimer( NULL ),
    iMsvEntry( NULL ),
    iObserverRequestStatus( aObserverRequestStatus )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgConnectAndRefreshFolderList::ConstructL( 
        CMsvEntrySelection& aMsvEntry )
    {
    FUNC_LOG;
    BaseConstructL( KSenduiMtmImap4Uid );
    iMsvEntry = aMsvEntry.CopyL();    
    iState = EStartConnect;
    DoRunL();
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgConnectAndRefreshFolderList::~CIpsPlgConnectAndRefreshFolderList()
    {
    FUNC_LOG;
    Cancel();
    delete iMsvEntry;
    delete iOperation;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgConnectAndRefreshFolderList::StepL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CIpsPlgConnectAndRefreshFolderList::IsProcessDone() const
    {
    FUNC_LOG;
    TBool ret = EFalse;
    if ( iState == ECompleted )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgConnectAndRefreshFolderList::DialogDismissedL(TInt /*aButtonId*/) 
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgConnectAndRefreshFolderList::ProgressL()
    {
    FUNC_LOG;
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgConnectAndRefreshFolderList::GetErrorProgressL( 
        TInt aError )
    {
    FUNC_LOG;
    TImap4CompoundProgress& prog = iProgressBuf();
    prog.iGenericProgress.iErrorCode = aError;
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgConnectAndRefreshFolderList::GetFSProgressL() const
    {
    FUNC_LOG;
    // Hardly ever called
    return TFSProgress();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgConnectAndRefreshFolderList::DoRunL()
    {
    FUNC_LOG;
    MFSMailRequestObserver* observer = NULL;
    
    if( iStatus.Int() != KErrNone )
        {
        iState = ECompleted;
        CompleteObserver();
        return;
        }
    
    switch(iState)
        {
        case EStartConnect:
            delete iOperation;
            iOperation = NULL;
            iOperation = CIpsPlgImap4ConnectOp::NewL(
                            iSession,
                            CActive::EPriorityStandard,
                            iStatus,
                            iService,
                            *iTimer,
                            iMailboxId,
                            *observer,
                            NULL, //FSRequestId
                            NULL, // Event handler
                            ETrue, // Plain connect
                            EFalse ); // No signalling needed )
            iState = EConnecting;
            SetActive();
            break;
	    case EConnecting:
	        {
	        //  We have successfully completed connecting
	        TBuf8<1> dummyParam;
	        delete iOperation;
	        iOperation = NULL;
	        InvokeClientMtmAsyncFunctionL( 
	                    KIMAP4MTMSyncTree, 
	                    *iMsvEntry,
	                    iService,
	                    dummyParam ); 
	        iState = ERefreshing;
	        SetActive();
	        }
	        break;
	    case ERefreshing:
	        //  We have successfully completed refreshing the folder list
	        delete iOperation;
	        iOperation = NULL;
	        iOperation = CIpsPlgDisconnectOp::NewL( 
	                        iSession, 
	                        iStatus, 
	                        iService, 
	                        *iTimer,
	                        iMailboxId, 
	                        *observer, 
	                        NULL );
	        iState = EDisconnecting;
	        SetActive();
	        break;
	    case EDisconnecting:
	        iState = ECompleted;
	        CompleteObserver();
	        break;
	    default:
	        User::Panic( KIpsPlgPanicCategory, EIpsPlgNoParameters );
	    	break;
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::DoCancel()
// ----------------------------------------------------------------------------
//
void CIpsPlgConnectAndRefreshFolderList::DoCancel()
    {
    FUNC_LOG;
    if( iOperation )
        {
        iOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    iState = ECompleted;
    }

// End of File

