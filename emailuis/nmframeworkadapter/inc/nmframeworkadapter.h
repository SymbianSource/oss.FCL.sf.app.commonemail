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

#ifndef NMFRAMEWORKADAPTER_H
#define NMFRAMEWORKADAPTER_H


#include <nmcommon.h>
#include <nmdataplugininterface.h>
#include <CFSMailCommon.h>
#include <MFSMailEventObserver.h>

class NmMailbox;
class NmMessage;
class NmFolder;
class NmMessageEnvelope;
class NmMessagePart;
class NmOperation;
class NmMessageCreationOperation;
class CFSMailClient;
class CFSMailMessage;
class CFSMailMessagePart;
class NmStoreEnvelopesOperation;
class NmAddAttachmentsOperation;
class NmOperation;
class NmCheckOutboxOperation;
class NmMessageSendingOperation;

class NmFrameworkAdapter :
    public QObject,
    public NmDataPluginInterface,
    public MFSMailEventObserver
{
    Q_OBJECT
    Q_INTERFACES(NmDataPluginInterface)

public:

    NmFrameworkAdapter( );

    ~NmFrameworkAdapter( );

    int listMailboxIds(QList<NmId>& mailboxIdList);

    int listMailboxes(QList<NmMailbox*>& mailboxList);

    int getMailboxById(const NmId& id, NmMailbox*& mailbox);

    int deleteMailboxById(const NmId& id);

    int getMessageById(
            const NmId& mailboxId,
            const NmId& folderId,
            const NmId& messageId,
            NmMessage*& message);

    int listFolders(
            const NmId& mailboxId,
            QList<NmFolder*> &folderList);
   
    int listMessages(
            const NmId& mailboxId,
            const NmId& folderId,
            QList<NmMessageEnvelope*> &messageMetaDataList);

    NmOperation *fetchMessage( 
            const NmId &mailboxId, 
            const NmId &folderId,
            const NmId &messageId);
    
    NmOperation *fetchMessagePart( 
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId);
           
    NmId getStandardFolderId(
            const NmId& mailbox,
            NmFolderType folderType );

    int refreshMailbox(NmId mailboxId);

    int contentToMessagePart(
            const NmId &mailboxId,
            const NmId &folderId,
            const NmId &messageId,
            NmMessagePart &messagePart);
 
    int deleteMessages(
            const NmId &mailboxId,
            const NmId &folderId,
            const QList<NmId> &messageIdList);

    NmStoreEnvelopesOperation *storeEnvelopes(
            const NmId &mailboxId,
            const NmId &folderId,
            const QList<const NmMessageEnvelope*> &envelopeList);

    NmMessageCreationOperation *createNewMessage(
        const NmId &mailboxId);

    NmMessageCreationOperation *createForwardMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId);
     
    NmMessageCreationOperation *createReplyMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        const bool replyAll);
    
    int saveMessage(const NmMessage &message);

    NmOperation *saveMessageWithSubparts(const NmMessage &message);

    void EventL(
        TFSMailEvent event,
        TFSMailMsgId mailbox,
        TAny* param1,
        TAny* param2,
        TAny* param3);

    int removeMessage(const NmId& mailboxId, const NmId& folderId, const NmId& messageId);
    
    void subscribeMailboxEvents(const NmId& mailboxId);
    
    void unsubscribeMailboxEvents(const NmId& mailboxId);

    NmMessageSendingOperation *sendMessage(NmMessage *message);
    
    NmAddAttachmentsOperation *addAttachments(
            const NmMessage &message, 
            const QList<QString> &fileList);

    NmOperation *removeAttachment(const NmMessage &message, const NmId &attachmentPartId);

    NmCheckOutboxOperation *checkOutbox(const NmId& mailboxId);
    
    NmSyncState syncState(const NmId& mailboxId) const;
    
    NmConnectState connectionState(const NmId& mailboxId) const;
    
signals:
    void mailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds);
    void messageEvent(
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId); //added to provide mailboxId
    
    void syncStateEvent(
            NmSyncState state,
            const NmId mailboxId);
    
    void connectionEvent(NmConnectState state, const NmId mailboxId);

private:
    
    void getMessageByIdL(
        const NmId& mailboxId,
        const NmId& folderId,
        const NmId& messageId,
        NmMessage*& message);

    void contentToMessagePartL(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        NmMessagePart &messagePart);
        
    void listMessagesL(  
            const NmId &mailboxId,
            const NmId &folderId,
            QList<NmMessageEnvelope*> &messageMetaDataList);
    
    NmId getMailboxIdByMailMsgId(TFSMailMsgId mailbox);

    void removeMessageL(const NmId& mailboxId,
                        const NmId& folderId,
                        const NmId& messageId);

    CFSMailMessage* mailMessageFromEnvelopeL( 
        const NmMessageEnvelope& envelope);

    void childrenToNmMessagePartL(
            CFSMailMessagePart *cfsParent, 
            NmMessagePart *nmParent);

    int RefreshMailboxL(NmId mailboxId);
    
    void handleMailboxEvent(
            TFSMailMsgId mailbox,
            NmMailboxEvent event);
    
    void handleMessageEvent(TAny* param1, TAny* param2, NmMessageEvent event, TFSMailMsgId mailbox);
    
    void handleMailMoved(TAny* param1,TAny* param2,TAny* param3, TFSMailMsgId mailbox);
    
    void handleMailCopied(TAny* param1,TAny* param2, TFSMailMsgId mailbox);
    
    void handleSyncstateEvent(TAny* param1, TFSMailMsgId mailbox);
    
private:
    CFSMailClient*  mFSfw;//singleton, not owned
};


#endif // NMFRAMEWORKADAPTER_H
