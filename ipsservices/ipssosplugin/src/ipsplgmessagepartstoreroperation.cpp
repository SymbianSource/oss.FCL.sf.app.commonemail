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

#include <utf.h>

#include "ipsplgmessagepartstoreroperation.h"
#include "CFSMailPlugin.h"
#include "emailtrace.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation* CIpsPlgMessagePartStorerOperation::NewL(
        CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessagePart> &aMessageParts,
        MFSMailRequestObserver& aFSOperationObserver, const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMessagePartStorerOperation* self =
            new (ELeave) CIpsPlgMessagePartStorerOperation(aMsvSession,
                    aObserverRequestStatus, aPlugin, aMessageParts,
                    aFSOperationObserver, aRequestId);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation* CIpsPlgMessagePartStorerOperation::NewLC(
        CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessagePart> &aMessageParts,
        MFSMailRequestObserver& aFSOperationObserver, const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMessagePartStorerOperation* self =
            CIpsPlgMessagePartStorerOperation::NewL(aMsvSession,
                    aObserverRequestStatus, aPlugin, aMessageParts,
                    aFSOperationObserver, aRequestId);
    CleanupStack::PushL(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation::~CIpsPlgMessagePartStorerOperation()
	{
	FUNC_LOG;
	// Cancel outstanding request, clear data buffers
	Cancel();
    iMessageParts.ResetAndDestroy();
    iMessageParts.Close();
	if (iDataBuffer)
		{
		delete iDataBuffer;
		iDataBuffer = NULL;
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::DoCancel()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
const TDesC8& CIpsPlgMessagePartStorerOperation::ProgressL()
    {
    FUNC_LOG;
    return KNullDesC8;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation::CIpsPlgMessagePartStorerOperation(
        CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessagePart> &aMessageParts,
        MFSMailRequestObserver& aFSOperationObserver, const TInt aRequestId) :
    CMsvOperation(aMsvSession, CActive::EPriorityStandard,
            aObserverRequestStatus), iPlugin(aPlugin), iMessageParts(
            aMessageParts), iFSOperationObserver(aFSOperationObserver),
            iRequestId(aRequestId), iExecutionIndex(0), iDataBuffer(NULL)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::RunL()
	{
	FUNC_LOG;
	
	// Clear data buffer if it was used in last request
	if (iDataBuffer)
		{
		delete iDataBuffer;
		iDataBuffer = NULL;
		}
	
	if (!StoreNextPartL())
		{
		if (&iFSOperationObserver)
			{
			TFSProgress prog =
				{
				TFSProgress::EFSStatus_RequestComplete, 0, 0, 0
				};
			TRAP_IGNORE( iFSOperationObserver.RequestResponseL( prog, iRequestId ) );
			}
		TRequestStatus* status = &iObserverRequestStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsPlgMessagePartStorerOperation::RunError(TInt aError)
    {
    FUNC_LOG;
    TInt error = aError;
    switch (aError)
        {
        // Handling RunL errors can be done here
        default:
            error = KErrNone;
        }
    return error;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgMessagePartStorerOperation::StoreNextPartL()
	{
	TBool partFound = EFalse;
	TRequestStatus* status;
	TInt partsCount = iMessageParts.Count();
	if (partsCount && iExecutionIndex < partsCount)
		{
		partFound = ETrue;
		CFSMailMessagePart* part = iMessageParts[iExecutionIndex];

		// Change execution index to point next part
		iExecutionIndex++;

		// Check if part was found
		if (part)
			{
			if (part->GetContentType().Compare(KFSMailContentTypeTextPlain) == 0)
				{
				StoreTextPlainPartL(part);
				}
			else if (part->GetContentType().Compare(KFSMailContentTypeTextHtml) == 0)
				{
				StoreTextHtmlPartL(part);
				}
			}
		else
			{
			status = &iObserverRequestStatus;
			iStatus = KRequestPending;
			SetActive();
			User::RequestComplete(status, KErrNotFound);
			}
		}
	return partFound;
	}
	


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::StoreTextPlainPartL(
		CFSMailMessagePart* aPart)
	{
	User::LeaveIfNull(aPart);
	TRequestStatus* status;
	
	// Text buffer for plain text content - synchronous function
	HBufC* contentBuffer = aPart->GetLocalTextContentLC();
	iPlugin.SetContentL(*contentBuffer, aPart->GetMailBoxId(),
			aPart->GetFolderId(), aPart->GetMessageId(), aPart->GetPartId());
	CleanupStack::PopAndDestroy(contentBuffer);

	// Request should be completed immediately, because sync function was used
	status = &iStatus;
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::StoreTextHtmlPartL(
		CFSMailMessagePart* aPart)
	{
	User::LeaveIfNull(aPart);
	
	// Text buffer for html text content
	HBufC* data16 = aPart->GetLocalTextContentLC();

	// Convert from 16 to 8 bit data - data must exist until request is completed
	iDataBuffer = HBufC8::NewL((data16->Length() * 2) + 1);
	TPtr8 ptr8(iDataBuffer->Des());
	CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr8, *data16);

	// Get text/html part file for write
	RFile file = aPart->GetContentFileL();
	CleanupClosePushL(file);

	// Write new content to text/html part file - async function
	file.Write(0, *iDataBuffer, iDataBuffer->Length(), iStatus);

	CleanupStack::PopAndDestroy(2, data16);
	SetActive();
	}
