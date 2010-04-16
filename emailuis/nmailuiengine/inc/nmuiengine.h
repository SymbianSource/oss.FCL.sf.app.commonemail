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

#ifndef NMUIENGINE_H
#define NMUIENGINE_H

#include <QObject>
#include <QList>
#include "nmcommon.h"
#include "nmuienginedef.h"

class NmMailboxListModel;
class NmMessageListModel;
class NmDataManager;
class NmDataPluginFactory;
class NmMailboxMetaData;
class NmMessage;
class NmMessagePart;
class NmMessageEnvelope;
class NmMessageCreationOperation;
class NmStoreEnvelopesOperation;
class NmAddAttachmentsOperation;
class NmOperation;
class NmCheckOutboxOperation;
class NmMessageSendingOperation;

class NMUIENGINE_EXPORT NmUiEngine: public QObject
{
    Q_OBJECT
public:

    static NmUiEngine *instance();
    static void releaseInstance(NmUiEngine *&instance);

    NmMailboxListModel &mailboxListModel();

    void refreshMailboxListModel();

    NmMessageListModel &messageListModel(const NmId &mailboxId, const NmId &folderId);

    void releaseMessageListModel(const NmId &mailboxId);

    NmId standardFolderId(const NmId &mailboxId, NmFolderType folderType);

    NmMessage *message(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId);

    NmOperation *fetchMessage(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId);

    NmOperation *fetchMessagePart( 
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId& messagePartId);
    
    NmMailboxMetaData *mailboxById(const NmId &mailboxId);

    int contentToMessagePart(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        NmMessagePart &messagePart);

    NmMessage *createMessage(
        const NmId &mailboxId,
        const NmId &folderId);

    int deleteMessages(
        const NmId &mailboxId,
        const NmId &folderId,
        const QList<NmId> &messageIdList);

    NmStoreEnvelopesOperation *setEnvelopes(
            const NmId &mailboxId,
            const NmId &folderId,
            NmEnvelopeProperties property,
            const QList<const NmMessageEnvelope*> &envelopeList);


    NmMessageCreationOperation *createNewMessage(const NmId &mailboxId);

    NmMessageCreationOperation *createForwardMessage(
            const NmId &mailboxId,
            const NmId &originalMessageId);

    NmMessageCreationOperation *createReplyMessage(
            const NmId &mailboxId,
            const NmId &originalMessageId,
            bool replyAll);

    int saveMessage(const NmMessage &message);

    NmOperation *saveMessageWithSubparts(const NmMessage &message);

    int refreshMailbox(const NmId &mailboxId);

    int removeMessage(
                const NmId &mailboxId,
                const NmId &folderId,
                const NmId &messageId);

    void storeOperation(NmOperation *op);

    void sendMessage(NmMessage *message);

    bool isSendingMessage() const;

    const NmMessage *messageBeingSent();
    NmAddAttachmentsOperation *addAttachments(
            const NmMessage &message,
            const QList<QString> &fileList);
    
    NmOperation *removeAttachment(const NmMessage &message, const NmId &attachmentPartId);

    NmCheckOutboxOperation *checkOutbox(const NmId &mailboxId);
    
    NmSyncState syncState(const NmId& mailboxId);
    NmConnectState connectionState(const NmId& mailboxId);

public slots:
    void handleCompletedOperation();

    void handleCompletedSendOperation();
    void handleCompletedSaveOperation(int error);

    void cleanupSendOperation();
    
signals:
    void syncStateEvent(NmSyncState, const NmId &);
    void connectionEvent(NmConnectState, const NmId &);

private:

    NmUiEngine();
    virtual ~NmUiEngine();

private:

    static NmUiEngine *mInstance;
    static int	mReferenceCount;

    NmDataPluginFactory *mPluginFactory;
    NmDataManager *mDataManager;                // Owned
    NmMailboxListModel *mMailboxListModel;      // Owned
    NmMessageListModel *mMessageListModel;      // Owned
    QList<NmOperation*> mOperations;			// Owned

    NmMessageSendingOperation *mSendOperation;  // Owned
    NmOperation *mSaveOperation;                // Owned

    // Flag that idicates if the message should be sent after storing
    bool mMessageToBeSent;

	NmMessage *mMessage;                        // Owned
};


#endif /* NMUIENGINE_H */

