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
NmFwaMessagePartsFetchingOperation::NmFwaMessagePartsFetchingOperation(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const QList<NmId> &messagePartIds,
        CFSMailClient &mailClient)
  : mMailboxId(mailboxId), 
    mFolderId(folderId), 
    mMessageId(messageId), 
    mMailClient(mailClient), 
    mLastProgressValue(0), 
    mRequestId(0)

{
    // Take own copy of the message part id list.
    mMessagePartIds.Reset();
    for (int i=0; i<messagePartIds.count(); ++i) {
        mMessagePartIds.Append(
            NmConverter::nmIdToMailMsgId(
                messagePartIds.at(i)));
    }
    
}

/*!

 */
NmFwaMessagePartsFetchingOperation::~NmFwaMessagePartsFetchingOperation()
{
    doCancelOperation();
}

/*!

 */
void NmFwaMessagePartsFetchingOperation::RequestResponseL(TFSProgress aEvent, TInt aRequestId)
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
void NmFwaMessagePartsFetchingOperation::doRunAsyncOperation()
{
    TRAPD(err, doRunAsyncOperationL());
    if (err != KErrNone) {
        completeOperation(NmGeneralError);
    }
}

/*!
    Leaving version from doRunAsyncOperation
 */
void NmFwaMessagePartsFetchingOperation::doRunAsyncOperationL()
{
    if (mMessagePartIds.Count() > 0) {
        
        const TFSMailMsgId mailboxId(mMailboxId.pluginId32(), mMailboxId.id32());
        const TFSMailMsgId folderId(mFolderId.pluginId32(), mFolderId.id32());
        const TFSMailMsgId messageId(mMessageId.pluginId32(), mMessageId.id32());
        
        CFSMailMessage *message(NULL);
        message = mMailClient.GetMessageByUidL(mailboxId, folderId, messageId, EFSMsgDataEnvelope);
        CleanupStack::PushL(message);
       
        if (message) {
            CFSMailMessagePart* messagePart = message->ChildPartL( mMessagePartIds[0] );
            CleanupStack::PushL(messagePart);
            if (messagePart) {            
                mRequestId = messagePart->FetchMessagesPartsL(mMessagePartIds,*this,0);
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
    else {
        completeOperation(NmNotFoundError);
    }
}

/*!
 * Complete the operation
 */
void NmFwaMessagePartsFetchingOperation::doCompleteOperation()
{
    mRequestId = NmNotFoundError;
}

/*!
    Cancels the async operation. \sa NmOperation
 */
void NmFwaMessagePartsFetchingOperation::doCancelOperation()
{
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
}