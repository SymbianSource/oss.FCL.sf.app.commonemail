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
* Description: Email services API to be used through Qt Highway.
*
*/

#ifndef EMAIL_SERVICES_API_H
#define EMAIL_SERVICES_API_H

/*!
    Keys for mail send service data.
*/
static const QString emailSendSubjectKey = "subject";
static const QString emailSendToKey = "to";
static const QString emailSendCcKey = "cc";
static const QString emailSendBccKey = "bcc";

/*!
    Mail service name
*/
static const QString emailServiceName("nmail");

/*!
    Mail send service interface name.
*/
static const QString emailInterfaceNameSend = "com.nokia.symbian.IEmailMessageSend";

/*!
    Mail send service interface name.
*/
static const QString emailFullServiceNameSend = emailServiceName + "." + emailInterfaceNameSend;


/*!
    \fn send(QVariant data)
    \param data The email message data e.g. recipients.
    \return 1 if success, 0 otherwise.

    The method opens the mail editor view and adds the given data into the view.

    The data is of type QMap<QString, QVariant> where the keys define the type
    of the data (e.g. recipient or mail subject). The values in the map for
    recipients (to, cc and bcc) may contain either strings (for a single
    recipient) or string lists (for multiple recipients).

    The method supports the following mail data:
        - recipients (to, cc and bcc) and
        - mail subject.

    This service is used via Qt Highway.

    Usage example:

        bool syncronous;

        XQServiceRequest request(emailFullServiceNameSend,
                                 emailOperationSendMail,
                                 syncronous);

        QMap<QString, QVariant> map;

        QStringList recipients;
        QString subject;

        ...

        map.insert(emailSendToKey, QVariant::fromValue(recipients));
        map.insert(emailSendSubjectKey, QVariant(subject));

        QList<QVariant> data;
        data.append(map);
        request.setArguments(data);

        QVariant returnValue;
        bool retVal = request.send(returnValue);
*/
static const QString emailOperationSendMail = "send(QVariant)";


/*!
    Mailbox service interface name.
*/
static const QString emailInterfaceNameMailbox = "com.nokia.symbian.IEmailInboxView";

/*!
    Mailbox service full name.
*/
static const QString emailFullServiceNameMailbox = emailServiceName + "." + emailInterfaceNameMailbox;


/*!
    Message service interface name.
*/
static const QString emailInterfaceNameMessage = "com.nokia.symbian.IEmailMessageView";

/*!
    Message service full name.
*/
static const QString emailFullServiceNameMessage = emailServiceName + "." + emailInterfaceNameMessage;


/*!
    \fn displayInboxByMailboxId(QVariant data)
    \param data The ID of the mailbox to open
    \return 1 if mailbox opened. 0 if mailbox ID was incorrect.

    This method opens a view for displaying the inbox of the given mailbox.
    It is intended to be used via Qt Highway.

    Usage example:

    XQServiceRequest request(
        emailFullServiceNameMailbox,
        emailOperationViewInbox,
        syncronous);

    QList<QVariant> list;
    list.append(QVariant(id));
    request.setArguments(list);

    QVariant returnValue;
    bool rval = request.send(returnValue);
*/
static const QString emailOperationViewInbox = "displayInboxByMailboxId(QVariant)";


/*!
    \fn viewMessage(QVariant mailboxId, QVariant folderId, QVariant messageId)
    \param mailboxId The ID of the mailbox where message is
    \param folderId The ID of the folder where message is
    \param messageId The ID of the message to be shown
    \return 1 if message was opened. 0 if one of the IDs was incorrect.

    This method opens a view for a specific message.
    It is intended to be used via Qt Highway.

    Usage example:

    XQServiceRequest request(
       emailFullServiceNameMessage,
       emailOperationViewMessage,
       syncronous);

    QList<QVariant> list;
    list.append(mailboxId);
    list.append(folderId);
    list.append(messageId);
    request.setArguments(list);

    QVariant returnValue;
    bool rval = request.send(returnValue);
*/
static const QString emailOperationViewMessage = "viewMessage(QVariant,QVariant,QVariant)";

#endif // EMAIL_SERVICES_API_H
