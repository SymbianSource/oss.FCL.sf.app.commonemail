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

#ifndef NMAPIMESSAGEENVELOPEINFO_H
#define NMAPIMESSAGEENVELOPEINFO_H

#include <QString>
#include <QDateTime>
#include <QExplicitlySharedDataPointer>

#include "nmenginedef.h"
#include "nmapiemailaddress.h"
#include "nmapimessagebody.h"

namespace EmailClientApi
{
class NmMessageEnvelopePrivate : public QSharedData
{
public:
    NmMessageEnvelopePrivate();
    virtual ~NmMessageEnvelopePrivate();

    bool read;
    bool attachments;
    bool forwarded;
    bool replied;

    QList<EmailClientApi::NmEmailAddress> ccRecipients;
    QList<EmailClientApi::NmEmailAddress> toRecipients;

    quint64 id;
    quint64 parentFolder;

    QString contentType;
    QString subject;
    QString sender;

    QString plainText;
    quint64 totalSize;
    quint64 fetchedSize;

    QDateTime sentTime;
};

// read-only envelope
class NMENGINE_EXPORT NmMessageEnvelope
{
public:
    /*
     * constructor for nmmessageenvelope
     */
    NmMessageEnvelope();
    /*
     * destructor for nmmessageenvelope
     */
    virtual ~NmMessageEnvelope();
    NmMessageEnvelope &operator=(const NmMessageEnvelope &envelope);

    /*!        
     * getter for id
     */
    quint64 id() const;

    /*
     * getter for id of parent folder
     */
    quint64 parentFolder() const;

    /*
     * getter for subject
     */
    QString subject() const;

    /*
     * getter for sender
     */
    QString sender() const;

    /*
     * getter for to recipients
     */
    void getToRecipients( QList<EmailClientApi::NmEmailAddress> &toRecipients );

    /*
     * getter for cc recipients
     */
    void getCcRecipients( QList<EmailClientApi::NmEmailAddress> &ccRecipients );

    /*
     * getter for sent time
     */
    QDateTime sentTime() const;

    /*
     * getter for is read flag
     */
    bool isRead() const;

    /*
     * getter for has attachments flag
     */
    bool hasAttachments() const;

    /*
     * getter for is forwarded flag
     */
    bool isForwarded() const;

    /*
     * getter for is replied flag
     */
    bool isReplied() const;

    /*
     * getter for content type
     */
    QString contentType() const;

    /*
     * getter for plaintext body
     */
    void getPlainTextBody( EmailClientApi::NmMessageBody &body );

    /*
     * getter for plaintext body
     */
    QString plainText() const;

    /*
     * getter for messages fetched size 
     */
    quint64 totalSize() const;

    /*
     * getter for messages fetched size 
     */
    quint64 fetchedSize() const;

    /*
     * setter for id 
     */
    void setId(quint64 id);

    /*
     * setter for parent folder 
     */
    void setParentFolder(quint64 parentFolder);

    /*
     * setter for subject 
     */
    void setSubject(const QString& subject);

    /*
     * setter for sender  
     */
    void setSender(const QString& sender);

    /*
     * setter for to recipients 
     */
    void setToRecipients(const QList<EmailClientApi::NmEmailAddress>& toRecipients);

    /*
     * setter for cc recipients 
     */
    void setCcRecipients(const QList<EmailClientApi::NmEmailAddress> &ccRecipients);

    /*
     * setter for sent time 
     */
    void setSentTime(QDateTime sentTime);

    /*
     * setter for is read flag
     */
    void setIsRead(bool isRead);

    /*
     * setter for has attachments flag 
     */
    void setHasAttachments(bool hasAttachments);

    /*
     * setter for is forwarded flag 
     */
    void setIsForwarded(bool isForwarded);

    /*
     * setter for is replied flag 
     */
    void setIsReplied(bool isReplied);

    /*
     * setter for content type 
     */
    void setContentType(const QString &contentType);

    /*
     * getter for messages plain text 
     */
    void setPlainText(const QString &plainText);

    /*
     * getter for messages fetched size 
     */
    void setTotalSize(quint64 totalSize);

    /*
     * getter for messages fetched size 
     */
    void setFetchedSize(quint64 fetchedSize);

private:
    QExplicitlySharedDataPointer<NmMessageEnvelopePrivate> d;
};
}
#endif

