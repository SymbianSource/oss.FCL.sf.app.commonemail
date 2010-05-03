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

// <qmail> remove priority const

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
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId )
    {
    FUNC_LOG;
    // <qmail> aDoRemoveAfterDisconnect removed
    CIpsPlgDisconnectOp* op = new(ELeave) CIpsPlgDisconnectOp(
        aMsvSession,
        aObserverRequestStatus,
        aService,
        aActivityTimer,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId );
        
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
    if ( iSubOperation && iError == KErrNone )
        {
        return iSubOperation->ProgressL();
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

// <qmail> Connected() moved to base class

// ----------------------------------------------------------------------------

// CIpsPlgDisconnectOp::DoRunL()
// ----------------------------------------------------------------------------
//
void CIpsPlgDisconnectOp::DoRunL()
    {
    FUNC_LOG;
    if( Connected() )
        {        
        DoDisconnectL();
        }
    else
        {                
        // <qmail> iDoRemoveAfterDisconnect feature from this op has been removed
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
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId )
    :
    CIpsPlgOnlineOperation(
        aMsvSession,
        aObserverRequestStatus,
        aActivityTimer,
        aFSMailBoxId,
        aFSOperationObserver, 
        aFSRequestId )
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
    TMsvId service;
    iMsvSession.GetEntry( iService, service, iTEntry );
    
    if ( iTEntry.iType.iUid == KUidMsvServiceEntryValue )
        {
        BaseConstructL( iTEntry.iMtm );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    
    // <qmail> SetActive(); moved inside CompleteThis();
    CompleteThis();
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgDisconnectOp::DoDisconnectL()
// ----------------------------------------------------------------------------
//
void CIpsPlgDisconnectOp::DoDisconnectL()
    {
    FUNC_LOG;
    TInt cmd = (iTEntry.iMtm == KUidMsgTypePOP3) ? KPOP3MTMDisconnect : KIMAP4MTMDisconnect;
    InvokeClientMtmAsyncFunctionL( cmd, iService ); // <qmail> 1 param removed
    SetActive();
    }

// <qmail> adding this func
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TIpsOpType CIpsPlgDisconnectOp::IpsOpType() const
    {
    FUNC_LOG;
    if ( iTEntry.iMtm == KUidMsgTypePOP3 )
        {
        return EIpsOpTypePop3Disconnect;
        }
    else
        {
        return EIpsOpTypeImap4Disconnect;
        }
    }
