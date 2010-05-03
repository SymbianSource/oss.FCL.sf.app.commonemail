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

#include "nmapimessageenvelope.h"

namespace EmailClientApi
{
NmMessageEnvelopePrivate::NmMessageEnvelopePrivate()
{

}

NmMessageEnvelopePrivate::~NmMessageEnvelopePrivate()
{

}

/*
 * constructor for nmmessageenvelope
 */
NmMessageEnvelope::NmMessageEnvelope()
{
    d = new NmMessageEnvelopePrivate();
}

/*
 * destructor for nmmessageenvelope
 */
NmMessageEnvelope::~NmMessageEnvelope()
{

}

/*!
 Assignment operator
 */
NmMessageEnvelope &NmMessageEnvelope::operator=(const NmMessageEnvelope &envelope)
{
    if (this != &envelope) {
        d = envelope.d;
    }
    return *this;
}

/*
 * getter for id
 */
quint64 NmMessageEnvelope::id() const
{
    return d->id;
}

/*
 * getter for id of parent folder
 */
quint64 NmMessageEnvelope::parentFolder() const
{
    return d->parentFolder;
}

/*
 * getter for subject
 */
QString NmMessageEnvelope::subject() const
{
    return d->subject;
}

/*
 * getter for sender
 */
QString NmMessageEnvelope::sender() const
{
    return d->sender;
}

/*
 * getter for to recipients
 */
void NmMessageEnvelope::getToRecipients(QList<EmailClientApi::NmEmailAddress> &toRecipients)
{
    toRecipients = d->toRecipients;
}

/*
 * getter for cc recipients
 */
void NmMessageEnvelope::getCcRecipients(QList<EmailClientApi::NmEmailAddress> &ccRecipients)
{
    ccRecipients = d->ccRecipients;
}

/*
 * getter for sent time
 */
QDateTime NmMessageEnvelope::sentTime() const
{
    return d->sentTime;
}

/*
 * getter for is read flag
 */
bool NmMessageEnvelope::isRead() const
{
    return d->read;
}

/*
 * getter for has attachments flag
 */
bool NmMessageEnvelope::hasAttachments() const
{
    return d->attachments;
}

/*
 * getter for is forwarded flag
 */
bool NmMessageEnvelope::isForwarded() const
{
    return d->forwarded;
}

/*
 * getter for is replied flag
 */
bool NmMessageEnvelope::isReplied() const
{
    return d->replied;
}

/*
 * getter for content type
 */
QString NmMessageEnvelope::contentType() const
{
    return d->contentType;
}

/*
 * getter for plaintext body
 */
void NmMessageEnvelope::getPlainTextBody(EmailClientApi::NmMessageBody &body)
{
    body.setContent(d->plainText);
    body.setFetchedSize(d->fetchedSize);
    body.setTotalSize(d->totalSize);
}

/*
 * setter for id 
 */
void NmMessageEnvelope::setId(quint64 id)
{
    d->id = id;
}

/*
 * setter for parent folder 
 */
void NmMessageEnvelope::setParentFolder(quint64 parentFolder)
{
    d->parentFolder = parentFolder;
}

/*
 * setter for subject 
 */
void NmMessageEnvelope::setSubject(const QString& subject)
{
    d->subject = subject;
}

/*
 * setter for sender  
 */
void NmMessageEnvelope::setSender(const QString& sender)
{
    d->sender = sender;
}

/*
 * setter for to recipients 
 */
void NmMessageEnvelope::setToRecipients(
    const QList<EmailClientApi::NmEmailAddress>& toRecipients)
{
    d->toRecipients = toRecipients;
}

/*
 * setter for cc recipients 
 */
void NmMessageEnvelope::setCcRecipients(
    const QList<EmailClientApi::NmEmailAddress> &ccRecipients)
{
    d->ccRecipients = ccRecipients;
}

/*
 * setter for sent time 
 */
void NmMessageEnvelope::setSentTime(QDateTime sentTime)
{
    d->sentTime = sentTime;
}

/*
 * setter for si read flag
 */
void NmMessageEnvelope::setIsRead(bool isRead)
{
    d->read = isRead;
}

/*
 * setter for has attachments flag 
 */
void NmMessageEnvelope::setHasAttachments(bool hasAttachments)
{
    d->attachments = hasAttachments;
}

/*
 * setter for is forwarded flag 
 */
void NmMessageEnvelope::setIsForwarded(bool isForwarded)
{
    d->forwarded = isForwarded;
}

/*
 * setter for is replied flag 
 */
void NmMessageEnvelope::setIsReplied(bool isReplied)
{
    d->replied = isReplied;
}

/*
 * setter for content type 
 */
void NmMessageEnvelope::setContentType(const QString &contentType)
{
    d->contentType = contentType;
}

/*
 * setter for messages plain text 
 */
void NmMessageEnvelope::setPlainText(const QString& plainText)
{
    d->plainText = plainText;
}

/*
 * setter for messages total size 
 */
void NmMessageEnvelope::setTotalSize(quint64 totalSize)
{
    d->totalSize = totalSize;
}

/*
 * setter for messages fetched size 
 */
void NmMessageEnvelope::setFetchedSize(quint64 fetchedSize)
{
    d->fetchedSize = fetchedSize;
}

/*
 * getter for messages plain text 
 */
QString NmMessageEnvelope::plainText() const
{
    return d->plainText;
}

/*
 * getter for messages fetched size 
 */
quint64 NmMessageEnvelope::totalSize() const
{
    return d->totalSize;
}

/*
 * getter for messages fetched size 
 */
quint64 NmMessageEnvelope::fetchedSize() const
{
    return d->fetchedSize;
}
}
