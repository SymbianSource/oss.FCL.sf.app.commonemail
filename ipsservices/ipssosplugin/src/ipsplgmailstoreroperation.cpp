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
* Description:
*
*/

#include "ipsplgmailstoreroperation.h"
#include "CFSMailPlugin.h"
#include "emailtrace.h"


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation* CIpsPlgMailStorerOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CFSMailPlugin& aPlugin,
    RPointerArray<CFSMailMessage> &aMessages,
    MFSMailRequestObserver& aFSOperationObserver,
    const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMailStorerOperation* self =
        new (ELeave)CIpsPlgMailStorerOperation(
                aMsvSession,
                aObserverRequestStatus,
                aPlugin,
                aMessages,
                aFSOperationObserver,
                aRequestId);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation* CIpsPlgMailStorerOperation::NewLC(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CFSMailPlugin& aPlugin,
    RPointerArray<CFSMailMessage> &aMessages,
    MFSMailRequestObserver& aFSOperationObserver,
    const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMailStorerOperation* self = CIpsPlgMailStorerOperation::NewL(
        aMsvSession,
        aObserverRequestStatus,
        aPlugin,
        aMessages,
        aFSOperationObserver,
        aRequestId);
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation::~CIpsPlgMailStorerOperation()
    {
    FUNC_LOG;
    Cancel();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMailStorerOperation::DoCancel()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
const TDesC8& CIpsPlgMailStorerOperation::ProgressL()
    {
    return KNullDesC8;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation::CIpsPlgMailStorerOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CFSMailPlugin& aPlugin,
    RPointerArray<CFSMailMessage> &aMessages,
    MFSMailRequestObserver& aFSOperationObserver,
    const TInt aRequestId):
    CMsvOperation(aMsvSession, CActive::EPriorityStandard, aObserverRequestStatus),
    iPlugin(aPlugin),
    iMessages(aMessages),
    iFSOperationObserver(aFSOperationObserver),
    iRequestId(aRequestId)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMailStorerOperation::ConstructL()
    {
    FUNC_LOG;
    iError = KErrNone;
    iExecutionIndex = 0;

    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMailStorerOperation::RunL()
    {
    FUNC_LOG;
    if( iMessages.Count() &&
        iExecutionIndex < iMessages.Count() )
        {
        TInt error=KErrNone;
        CFSMailMessage *message = iMessages[0];

        TRequestStatus* status;

        if(message)
            {
            TFSMailMsgId id = message->GetMessageId();
            iPlugin.StoreMessageL( id, *message );

            iExecutionIndex++;

            status = &iStatus;
            }
        else
            {
            error = KErrNotFound;
            status = &iObserverRequestStatus;
            }
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete(status, error);
        }
    else
        {
        if( &iFSOperationObserver )
            {
            TFSProgress prog = { TFSProgress::EFSStatus_RequestComplete, 0, 0, 0 };

            TRAP_IGNORE(
                    iFSOperationObserver.RequestResponseL(
                            prog,
                            iRequestId ) );
            }

        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsPlgMailStorerOperation::RunError()
    {
    FUNC_LOG;
    return KErrNone;
    }
