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

#include "nmapiheaders.h"

namespace EmailClientApi
{
/*!
   converts nmmailbox to client api NmApiMailbox
 */
NmApiMailbox NmToApiConverter::NmMailbox2NmApiMailbox(const NmMailbox &mailbox)
{
    NM_FUNCTION;
    
    NmApiMailbox apiMailbox;
    apiMailbox.setId(mailbox.id().id());
    apiMailbox.setName(mailbox.name());
    NmMailbox tmp(mailbox);
    apiMailbox.setAddress(tmp.address().address());
    return apiMailbox;
}

/*!
   converts NmFolder to client api NmFolder
 */
NmApiFolder NmToApiConverter::NmFolder2NmApiFolder(const NmFolder &folder)
{
    NM_FUNCTION;
    
    NmApiFolder apiFolder;

    apiFolder.setParentFolderId(folder.parentId().id());
    apiFolder.setId(folder.folderId().id());
    apiFolder.setName(folder.name());

    switch ( folder.folderType() )
        {
        case NmFolderInbox:
        	apiFolder.setFolderType(EmailClientApi::Inbox);
            break;
        case NmFolderOutbox:
        	apiFolder.setFolderType(EmailClientApi::Outbox);
            break;
        case NmFolderDrafts:
        	apiFolder.setFolderType(EmailClientApi::Drafts);
            break;
        case NmFolderSent:
        	apiFolder.setFolderType(EmailClientApi::Sent);
            break;
        case NmFolderDeleted:
        	apiFolder.setFolderType(EmailClientApi::Deleted);
            break;
        case NmFolderOther:
        default:
        	apiFolder.setFolderType(EmailClientApi::EOther);
            break;
        }
    return apiFolder;
}

/*!
   converts NmMessageEnvelope to client api NmApiMessageEnvelope
 */
NmApiMessageEnvelope NmToApiConverter::NmMessageEnvelope2NmApiMessageEnvelope(
    const NmMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    NmApiMessageEnvelope apiEnv;
    
    QList<NmAddress> to = envelope.toRecipients();
    QList<NmAddress> cc = envelope.ccRecipients();
    QList<NmAddress> bcc = envelope.bccRecipients();
    QList<NmApiEmailAddress> to_api = NmAddress2QString(to);
    QList<NmApiEmailAddress> cc_api = NmAddress2QString(cc);
    QList<NmApiEmailAddress> bcc_api = NmAddress2QString(bcc);

    apiEnv.setToRecipients(to_api);
    apiEnv.setCcRecipients(cc_api);
    apiEnv.setBccRecipients(bcc_api);

    apiEnv.setHasAttachments(envelope.hasAttachments());
    apiEnv.setId(envelope.messageId().id());
    apiEnv.setIsForwarded(envelope.isForwarded());
    apiEnv.setIsRead(envelope.isRead());
    apiEnv.setIsReplied(envelope.isReplied());
    apiEnv.setParentFolder(envelope.folderId().id());

    apiEnv.setSender(envelope.sender().address());
    apiEnv.setSentTime(envelope.sentTime());
    apiEnv.setSubject(envelope.subject());
    NmId mailboxId(envelope.mailboxId());
    apiEnv.setMailboxId(mailboxId.id());
    apiEnv.setFlags(static_cast<NmApiMessageFlags>(static_cast<int>(envelope.flags())), true);

    return apiEnv;
}

/*!
   converts QList of NmAddresses to Qlist of email client apis NmApiEmailAddresses
 */
QList<NmApiEmailAddress> NmToApiConverter::NmAddress2QString(
    const QList<NmAddress> &addresses)
{
    NM_FUNCTION;
    
    QList<NmApiEmailAddress> nmAddresses;
    for (int i = 0; i < addresses.count(); i++) {
        NmApiEmailAddress addr;
        addr.setAddress(addresses[i].address());
        addr.setDisplayName(addresses[i].displayName());

        nmAddresses << addr;
    }

    return nmAddresses;
}

/*!
   converts nmmessage to client api NmApiMessage
 */
NmApiMessage NmToApiConverter::NmMessage2NmApiMessage(NmMessage &message)
{
    NM_FUNCTION;
    
    NmApiMessage apiMessage;
    QList<NmMessagePart*> attachments;
    message.attachmentList(attachments);

    if (attachments.size() > 0){
        QList<NmApiAttachment> apiAttachments; 
        for (int i = 0; i < attachments.size(); i++){
            NmMessagePart *part = attachments.at(i);
            if (part->contentType().compare(NmApiContentTypeTextPlain) != 0 && 
                part->contentType().compare(NmApiContentTypeTextHtml) != 0) {
                NmApiAttachment apiAttachment;
                apiAttachment.setFileName(part->attachmentName());
                apiAttachment.setId(part->partId().id());
                apiAttachment.setContentType(part->contentType());
                apiAttachment.setSize(part->size());
                apiAttachments.append(apiAttachment);
            }
        }
        apiMessage.setAttachments(apiAttachments);
    }
    
    NmApiMessageEnvelope apiEnvelope = NmMessageEnvelope2NmApiMessageEnvelope(message.envelope());
    apiMessage.setEnvelope(apiEnvelope);
    const NmMessagePart *plaintext = message.plainTextBodyPart();
    if (plaintext) {
        NmApiTextContent apiPlainText;
        apiPlainText.setContent(plaintext->textContent());
        apiPlainText.setId(plaintext->partId().id());
        apiPlainText.setContentType(plaintext->contentType());
        apiPlainText.setSize(plaintext->size());
        apiMessage.setPlainTextContent(apiPlainText);
    }
    
    const NmMessagePart *htmlcontent = message.htmlBodyPart();
    if (htmlcontent) {
        NmApiTextContent apiHtmlContent;
        apiHtmlContent.setContent(htmlcontent->textContent());
        apiHtmlContent.setId(htmlcontent->partId().id());
        apiHtmlContent.setContentType(htmlcontent->contentType());
        apiHtmlContent.setSize(htmlcontent->size());
        apiMessage.setHtmlContent(apiHtmlContent);
    }
    
    return apiMessage;
}
}



