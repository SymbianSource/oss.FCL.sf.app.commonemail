/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmframeworkadapterheaders.h"

/*!

 */
NmFwaMessagePartFetchingOperation::NmFwaMessagePartFetchingOperation(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId,
        CFSMailClient &mailClient) 
  : mMailboxId(mailboxId), 
    mFolderId(folderId), 
    mMessageId(messageId), 
    mMessagePartId(messagePartId),
    mMailClient(mailClient), 
    mLastProgressValue(0), 
    mRequestId(0)

{
    
}

/*!

 */
NmFwaMessagePartFetchingOperation::~NmFwaMessagePartFetchingOperation()
{
    doCancelOperation();
    NMLOG("NmFwaMessagePartFetchingOperation::~NmFwaMessagePartFetchingOperation --->");
}

/*!

 */
void NmFwaMessagePartFetchingOperation::RequestResponseL(TFSProgress aEvent, TInt aRequestId)
{
    if (aRequestId == mRequestId) {
        if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete ) {
            completeOperation(aEvent.iError);
        }
        else if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError);
        }
        else if (aEvent.iProgressStatus == TFSProgress::EFSStatus_Status) {
            int progress = 0;
            if (aEvent.iMaxCount > 0) {
                // calculate progress per cents
                qreal counterValue = aEvent.iCounter;
                qreal maxCount = aEvent.iMaxCount;
                progress = (counterValue / maxCount)*100;
            }
            if (progress > mLastProgressValue) {
                // send only increasing values to prevent downward changing percentage
                mLastProgressValue = progress;
                updateOperationProgress(mLastProgressValue);
            }
        }
    }
}

/*!

 */
void NmFwaMessagePartFetchingOperation::doRunAsyncOperation()
{
    TRAPD(err, doRunAsyncOperationL());
    if (err != KErrNone) {
        completeOperation(NmGeneralError);
    }
}

/*!
    Leaving version from doRunAsyncOperation
 */
void NmFwaMessagePartFetchingOperation::doRunAsyncOperationL()
{
    const TFSMailMsgId mailboxId(mMailboxId.pluginId32(), mMailboxId.id32());
    const TFSMailMsgId folderId(mFolderId.pluginId32(), mFolderId.id32());
    const TFSMailMsgId messageId(mMessageId.pluginId32(), mMessageId.id32());
    const TFSMailMsgId messagePartId(mMessagePartId.pluginId32(), mMessagePartId.id32());

    CFSMailMessage *message(NULL);
    message = mMailClient.GetMessageByUidL(mailboxId, folderId, messageId, EFSMsgDataEnvelope);
    CleanupStack::PushL(message);
   
    if (message) {
        CFSMailMessagePart* messagePart = message->ChildPartL( messagePartId );
        CleanupStack::PushL(messagePart);
        if (messagePart) {            
            mRequestId = messagePart->FetchMessagePartL(messagePartId, *this, 0);
        }
        else {
            completeOperation(NmNotFoundError);
        }
        CleanupStack::PopAndDestroy(messagePart);
    }
    else {
        completeOperation(NmNotFoundError);
    }
    CleanupStack::PopAndDestroy(message);
}

/*!

 */
void NmFwaMessagePartFetchingOperation::doCancelOperation()
{
    TRAP_IGNORE(mMailClient.CancelL(mRequestId));
}
