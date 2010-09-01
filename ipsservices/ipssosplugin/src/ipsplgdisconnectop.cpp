/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMAP4 connect operation
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"

// Constants and defines
const TInt KConnectOpPriority = CActive::EPriorityStandard;

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::NewL()
// ----------------------------------------------------------------------------
//
CIpsPlgDisconnectOp* CIpsPlgDisconnectOp::NewL(
    CMsvSession& aMsvSession,
                                           TRequestStatus& aObserverRequestStatus,
                                           TMsvId aService,
                                           CIpsPlgTimerOperation& aActivityTimer,
                                           TFSMailMsgId aFSMailBoxId,
                                           MFSMailRequestObserver& aFSOperationObserver,
                                           TInt aFSRequestId,
                                           TBool aDoRemoveAfterDisconnect )
    {
    FUNC_LOG;
    CIpsPlgDisconnectOp* op = new(ELeave) CIpsPlgDisconnectOp(aMsvSession,
        aObserverRequestStatus,
        aService,
        aActivityTimer,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId,
        aDoRemoveAfterDisconnect );
        
    CleanupStack::PushL(op);
    op->ConstructL();
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::~CIpsPlgDisconnectOp()
// ----------------------------------------------------------------------------
//
CIpsPlgDisconnectOp::~CIpsPlgDisconnectOp()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgDisconnectOp::ProgressL()
    {
    FUNC_LOG;
    if ( iTEntry.iMtm == KUidMsgTypePOP3 )
        {
        return iPopProgress;
        }
    else
        {
        return iImapProgress;
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::GetErrorProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgDisconnectOp::GetErrorProgressL(TInt aError)
    {
    FUNC_LOG;
    iError = aError;
    if ( iOperation && iError == KErrNone )
        {
        return iOperation->ProgressL();
        }
    
    if ( iTEntry.iMtm == KUidMsgTypePOP3 )
        {
        return iPopProgress;
        }
    else
        {
        return iImapProgress;
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::GetFSProgressL()
// ----------------------------------------------------------------------------
//
TFSProgress CIpsPlgDisconnectOp::GetFSProgressL() const
    {
    FUNC_LOG;
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    if ( Connected() )
        {
        result.iProgressStatus = TFSProgress::EFSStatus_Connected;
        result.iError = KErrNone;
        }
    else
        {
        result.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        result.iError = iStatus.Int();
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::Connected()
// ----------------------------------------------------------------------------
//
TBool CIpsPlgDisconnectOp::Connected() const
    {
    FUNC_LOG;
    return iTEntry.Connected();
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::DoRunL()
// ----------------------------------------------------------------------------
//
void CIpsPlgDisconnectOp::DoRunL()
    {
    FUNC_LOG;
    if( !iDisconnected )
        {        
        DoDisconnectL();
        iDisconnected = ETrue;
        }
    else
        {                
        if ( iDoRemoveAfterDisconnect )
            {
            CIpsSetDataApi* settings = CIpsSetDataApi::NewL( iMsvSession );
            CleanupStack::PushL( settings );
            settings->RemoveAccountL( iTEntry, iMsvSession );
            CleanupStack::PopAndDestroy( settings );

            // delete the activitytimer here and set it to null so we don't try 
            // to use it in CompleteObserver. This is a special case that 
            // relates to deleting a connected mailbox.
            delete iActivityTimer;
            iActivityTimer = NULL;
            }

        CompleteObserver( KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::CIpsPlgDisconnectOp()
// ----------------------------------------------------------------------------
//
CIpsPlgDisconnectOp::CIpsPlgDisconnectOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aServiceId,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId,
    TBool aDoRemoveAfterDisconnect )
    :
    CIpsPlgOnlineOperation(
    aMsvSession,
    KConnectOpPriority,
    aObserverRequestStatus,
    aActivityTimer,
    aFSMailBoxId,
    aFSOperationObserver, 
    aFSRequestId),
    iDoRemoveAfterDisconnect( aDoRemoveAfterDisconnect )
    {
    iService = aServiceId;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsPlgDisconnectOp::ConstructL()
    {    
    FUNC_LOG;
    iDisconnected = EFalse;
    TMsvId service;
    
    iMsvSession.GetEntry( iService, service, iTEntry );
    
    if ( iTEntry.iType.iUid == KUidMsvServiceEntryValue )
        {
        BaseConstructL( iTEntry.iMtm );
        }
    else
        {
        //should we panic with own codes?
        User::Leave( KErrNotSupported );
        }
    
    SetActive();
    CompleteThis();
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::DoDisconnectL()
// ----------------------------------------------------------------------------
//
void CIpsPlgDisconnectOp::DoDisconnectL()
    {
    FUNC_LOG;
    iStatus = KRequestPending;
    
    TInt cmd = (iTEntry.iMtm == KUidMsgTypePOP3) ? KPOP3MTMDisconnect : KIMAP4MTMDisconnect;
    
    InvokeClientMtmAsyncFunctionL(cmd, iService, iService);
    SetActive();
    }

