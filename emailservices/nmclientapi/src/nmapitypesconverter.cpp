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

#include "nmapitypesconverter.h"

#include <nmapicommonheader.h>

#include <nmapiprivateheaders.h>

/*!
   converts nmmailbox to client api NmApiMailbox
 */
EmailClientApi::NmApiMailbox NmToApiConverter::NmMailbox2NmApiMailbox(const NmMailbox &mailbox)
{
    EmailClientApi::NmApiMailbox api_mailbox;
    api_mailbox.setId(mailbox.id().id());
    api_mailbox.setName(mailbox.name());
    NmMailbox tmp(mailbox);
    api_mailbox.setAddress(tmp.address().address());
    return api_mailbox;
}

/*!
   converts NmFolder to client api NmFolder
 */
EmailClientApi::NmApiFolder NmToApiConverter::NmFolder2NmApiFolder(const NmFolder &folder)
{
    EmailClientApi::NmApiFolder api_folder;

    api_folder.setParentFolderId(folder.parentId().id());
    api_folder.setId(folder.folderId().id());
    api_folder.setName(folder.name());
    api_folder.setFolderType((EmailClientApi::NmApiEmailFolderType) folder.folderType());
    return api_folder;
}

/*!
   converts NmMessageEnvelope to client api NmApiMessageEnvelope
 */
EmailClientApi::NmApiMessageEnvelope NmToApiConverter::NmMessageEnvelope2NmApiMessageEnvelope(
    const NmMessageEnvelope &envelope)
{
    EmailClientApi::NmApiMessageEnvelope api_env;
    
    QList<NmAddress> to = envelope.toRecipients();
    QList<NmAddress> cc = envelope.ccRecipients();
    QList<EmailClientApi::NmApiEmailAddress> to_api = NmAddress2QString(to);
    QList<EmailClientApi::NmApiEmailAddress> cc_api = NmAddress2QString(cc);

    api_env.setToRecipients(to_api);
    api_env.setCcRecipients(cc_api);

    api_env.setHasAttachments(envelope.hasAttachments());
    api_env.setId(envelope.messageId().id());
    api_env.setIsForwarded(envelope.isForwarded());
    api_env.setIsRead(envelope.isRead());
    api_env.setIsReplied(envelope.isReplied());
    api_env.setParentFolder(envelope.folderId().id());

    api_env.setSender(envelope.sender().address());
    api_env.setSentTime(envelope.sentTime());
    api_env.setSubject(envelope.subject());

    return api_env;
}

/*!
   converts QList of NmAddresses to Qlist of email client apis NmApiEmailAddresses
 */
QList<EmailClientApi::NmApiEmailAddress> NmToApiConverter::NmAddress2QString(
    const QList<NmAddress> &addresses)
{
    QList<EmailClientApi::NmApiEmailAddress> nmAddresses;
    for (int i = 0; i < addresses.count(); i++) {
        EmailClientApi::NmApiEmailAddress addr;
        addr.setAddress(addresses[i].address());
        addr.setDisplayName(addresses[i].displayName());

        nmAddresses << addr;
    }

    return nmAddresses;
}

