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
#include "nmframeworkadapterheaders.h"

/*!
    \class NmFwaMessageSendingOperation
    
    \brief NmFwaMessageSendingOperation is an async operation which creates a
           new message.
    
    NmFwaMessageSendingOperation is an async operation which creates a new
    message. \sa NmOperation
 */

/*!
    Constructor
    
    \param message pointer to the message under sent, ownership is transferred to operation
    \param mailClient Reference to mail client object.
 */
NmFwaMessageSendingOperation::NmFwaMessageSendingOperation(
    NmMessage *message,
    CFSMailClient &mailClient) :
        mMessage(message),
        mMailClient(mailClient),
        mRequestId(NmNotFoundError)
{
}

/*!
    Destructor
 */
NmFwaMessageSendingOperation::~NmFwaMessageSendingOperation()
{
    doCancelOperation();
    delete mMessage;
}

/*!
    returns pointer to message, ownership is not transferred
 */
const NmMessage *NmFwaMessageSendingOperation::getMessage()
{
    return mMessage;
}

/*!
    Slot, called after base object construction via timer event, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaMessageSendingOperation::runAsyncOperation()
{
    TInt err (KErrNone);
    TRAP(err, runAsyncOperationL());

    if (err == KErrNotFound) {
        completeOperation(NmNotFoundError);
    }
    else if (err != KErrNone){
        completeOperation(NmGeneralError);
    }
    // err == KErrNone means everything went well and the operation is 
    // proceeding
}

/*!
    Leaving version of runAsyncOperation
 */
void NmFwaMessageSendingOperation::runAsyncOperationL()
{
    CFSMailBox *currentMailbox( NULL );

    if (!mMessage) {
        User::Leave( KErrNotFound );
    }
    TFSMailMsgId mailboxId = NmConverter::nmIdToMailMsgId(mMessage->mailboxId());
    currentMailbox = mMailClient.GetMailBoxByUidL(mailboxId);
    CleanupStack::PushL(currentMailbox);
    if (!currentMailbox) {
        User::Leave( KErrNotFound );
    }

    CFSMailMessage *msg = CFSMailMessage::NewL(*mMessage); // no leave -> msg != NULL
    CleanupStack::PushL(msg);
	
    mRequestId = currentMailbox->SendMessageL(*msg, *this);
    CleanupStack::PopAndDestroy(2); // msg, currentMailbox
}

/*!
    
 */
void NmFwaMessageSendingOperation::doCompleteOperation()
{
    mRequestId = NmNotFoundError;
}

/*!
    
 */
void NmFwaMessageSendingOperation::doCancelOperation()
{
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
}
    
/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaMessageSendingOperation::RequestResponseL(TFSProgress aEvent,
                                                    TInt aRequestId)
{
    TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;

    if (aRequestId == mRequestId) {
        if (status == TFSProgress::EFSStatus_RequestComplete) {
            completeOperation(NmNoError);
        }
        else if (status == TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError);
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
}
