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

#include "nmfwastoreenvelopesoperation.h"
#include "nmframeworkadapterheaders.h"

NmFwaStoreEnvelopesOperation::NmFwaStoreEnvelopesOperation(
        const NmId &mailboxId,
        RPointerArray<CFSMailMessage> messages,
        CFSMailClient &mailClient) :
        mMailboxId(mailboxId),
        mMailClient(mailClient),
        mRequestId(0)
{
    mMessages = messages;
}

NmFwaStoreEnvelopesOperation::~NmFwaStoreEnvelopesOperation()
{
    NMLOG("NmFwaStoreEnvelopesOperation::~NmFwaStoreEnvelopesOperation() <---");
    mMessages.ResetAndDestroy();
    NMLOG("NmFwaStoreEnvelopesOperation::~NmFwaStoreEnvelopesOperation() --->");
}

void NmFwaStoreEnvelopesOperation::runAsyncOperation()
{
    NMLOG("NmFwaStoreEnvelopesOperation::runAsyncOperation() <---");
    const TFSMailMsgId mailboxId(mMailboxId.pluginId32(), mMailboxId.id32());
    CFSMailBox *mailbox(NULL);
    TRAP_IGNORE( mailbox = mMailClient.GetMailBoxByUidL(mailboxId) );
    // no possibility to leave; no need for CleanupStack
    if ( mailbox && mMessages.Count() ) {
        TRAPD(err, mRequestId = mailbox->UpdateMessageFlagsL( mailboxId, mMessages, *this ));
        if (err != KErrNone) {
            completeOperation(NmGeneralError);
        }
    }
    else {
        completeOperation(NmNotFoundError);
    }
    // cleanup
    if (mailbox) {
        delete mailbox;
        mailbox = NULL;
    }
    NMLOG("NmFwaStoreEnvelopesOperation::runAsyncOperation() --->");
}

void NmFwaStoreEnvelopesOperation::doCancelOperation()
{
    NMLOG("NmFwaStoreEnvelopesOperation::doCancelOperation() <---");
    TRAP_IGNORE(mMailClient.CancelL(mRequestId));
    NMLOG("NmFwaStoreEnvelopesOperation::doCancelOperation() --->");
}

/**
 * asynchronous request response message
 *
 * @param aEvent plugin event description
 * @param aRequestId request id of asyncronous operation
 */
void NmFwaStoreEnvelopesOperation::RequestResponseL(TFSProgress aEvent, TInt aRequestId)
{
    NMLOG("NmFwaStoreEnvelopesOperation::RequestResponseL() <---");
    if (aRequestId == mRequestId) {
        if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete && aEvent.iParam) {
            completeOperation(NmNoError);
        }
        else if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled) {
            operationCancelled();
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
    NMLOG("NmFwaStoreEnvelopesOperation::RequestResponseL() --->");
}
