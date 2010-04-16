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

#include "nmfwacheckoutboxoperation.h"
#include "nmframeworkadapterheaders.h"

NmFwaCheckOutboxOperation::NmFwaCheckOutboxOperation(
    const NmId &mailboxId,
    CFSMailClient &mailClient) :
        mMailboxId(mailboxId),
        mMailClient(mailClient),
        mFound(false),
        mMessageId(0)
{
}

NmFwaCheckOutboxOperation::~NmFwaCheckOutboxOperation()
{
    doCancelOperation();
}

void NmFwaCheckOutboxOperation::runAsyncOperation()
{
    TRAPD(err, runAsyncOperationL());
    
    if (err) {
        completeOperation(NmGeneralError);
    }
}

/*!
    Gets the id of the message with index '0' in the Outbox .

    \return true if there is a message in Outbox.
 */
bool NmFwaCheckOutboxOperation::getMessageId(NmId &messageId) const
{
    messageId = mMessageId;
    return mFound;
}

void NmFwaCheckOutboxOperation::runAsyncOperationL()
{
    int err = NmNotFoundError;

    TFSMailMsgId mailboxId(mMailboxId.pluginId32(), mMailboxId.id32());
    
    CFSMailBox *mailbox = mMailClient.GetMailBoxByUidL(mailboxId);
    
    if (mailbox) {
        CleanupStack::PushL(mailbox);
    
        TFSMailMsgId folderId = mailbox->GetStandardFolderId(EFSOutbox);

        CFSMailFolder *folder =
            mMailClient.GetFolderByUidL(mailboxId, folderId);

        if (folder) {
            CleanupStack::PushL(folder);

            TFSMailSortCriteria sortCriteria;
            sortCriteria.iField = EFSMailSortByDate;
            sortCriteria.iOrder = EFSMailDescending;
            
            RArray<TFSMailSortCriteria> sortArray;
            CleanupClosePushL(sortArray);
            sortArray.AppendL(sortCriteria);
            
            MFSMailIterator *iter = folder->ListMessagesL(
                EFSMsgDataIdOnly, sortArray);
            
            CleanupStack::PopAndDestroy(); // sortArray

            if (iter) {
                CleanupDeletePushL(iter);

                TFSMailMsgId nullId;
                TInt numberOfItems = 1;
                RPointerArray<CFSMailMessage> messages;

                CleanupClosePushL(messages);
                TBool unused = iter->NextL(nullId, numberOfItems, messages);

                if (messages.Count() > 0) {
                    mMessageId = messages[0]->GetMessageId().GetNmId();
                    mFound = true;
                }

                messages.ResetAndDestroy();
                CleanupStack::PopAndDestroy(); // messages

                CleanupStack::PopAndDestroy(iter);
                
                err = NmNoError;
            }
            CleanupStack::PopAndDestroy(folder);
        }
        CleanupStack::PopAndDestroy(mailbox);
    }

    completeOperation(err);
}
