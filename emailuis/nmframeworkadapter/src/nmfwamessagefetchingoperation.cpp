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

NmFwaMessageFetchingOperation::NmFwaMessageFetchingOperation(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    CFSMailClient &mailClient) 
      : mMailboxId(mailboxId),
        mFolderId(folderId),
        mMessageId(messageId),
        mMailClient(mailClient),
        mRequestId(0)

{
}

NmFwaMessageFetchingOperation::~NmFwaMessageFetchingOperation()
{
    doCancelOperation();
    NMLOG("NmFwaMessageFetchingOperation::~NmFwaMessageFetchingOperation --->");
}

void NmFwaMessageFetchingOperation::doRunAsyncOperation()
{
    const TFSMailMsgId mailboxId(mMailboxId.pluginId32(), mMailboxId.id32());
    const TFSMailMsgId folderId(mFolderId.pluginId32(), mFolderId.id32());
    const TFSMailMsgId messageId(mMessageId.pluginId32(), mMessageId.id32());

    CFSMailFolder *folder( NULL );
    TRAP_IGNORE(folder = mMailClient.GetFolderByUidL(mailboxId, folderId));
   
    if (folder) {
        RArray<TFSMailMsgId> messageIds; // Cleanupstack not needed
        messageIds.Append( messageId );
        TRAPD(err, mRequestId = folder->FetchMessagesL( messageIds, EFSMsgDataStructure, *this ));
        messageIds.Close();
        if (err != KErrNone) {
            completeOperation(NmGeneralError);
        }
        delete folder;
        folder = NULL;
    }
    else {
        completeOperation(NmNotFoundError);
    }
}

void NmFwaMessageFetchingOperation::doCancelOperation()
{
    TRAP_IGNORE(mMailClient.CancelL(mRequestId));
}

/*!
 asynchronous request response message
 
 \param aEvent plugin event description
 \param mailClient Reference to mail client object
 */
void NmFwaMessageFetchingOperation::RequestResponseL(TFSProgress aEvent, TInt aRequestId)
{
    if (aRequestId == mRequestId) {
        if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete ) {
            completeOperation(NmNoError);
        }
        else if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError); 
        }
    }
}