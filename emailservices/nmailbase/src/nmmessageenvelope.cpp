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

#ifndef NMMESSAGEENVELOPE_CPP_
#define NMMESSAGEENVELOPE_CPP_

#include "nmmessageenvelope.h"

/*!
    Constructor of NmMessageEnvelopePrivate object
 */
NmMessageEnvelopePrivate::NmMessageEnvelopePrivate()
:mId(0),
mMailboxId(0),
mMessageFlags(0)
{
}

/*!
    Destructor of NmMessageEnvelopePrivate object
 */
NmMessageEnvelopePrivate::~NmMessageEnvelopePrivate()
{
}

NmMessageFlags NmMessageEnvelopePrivate::flags() const
{
    return mMessageFlags;
}

void NmMessageEnvelopePrivate::setFlags(const NmMessageFlags flags, bool set)
{
    if (set) {
        mMessageFlags |= flags;
    } else {
        mMessageFlags &= ~flags;
    }
}

void NmMessageEnvelopePrivate::setFlag(const NmMessageFlag flag, bool set)
{
    if (set) {
        mMessageFlags |= flag;
    } else {
        mMessageFlags &= ~flag;
    }
}

bool NmMessageEnvelopePrivate::isFlagSet(const NmMessageFlag flag) const
{
    return mMessageFlags.testFlag(flag);
}


/*!
    \class NmMessageEnvelope
    \brief Represents model for message data that is needed to show one message on
           message list
 */


/*!
    Constructor
 */
NmMessageEnvelope::NmMessageEnvelope()
{
    d = new NmMessageEnvelopePrivate();
}

/*!
    Constructor with id
 */
NmMessageEnvelope::NmMessageEnvelope(const NmId &id)
{
    d = new NmMessageEnvelopePrivate();
    d->mId = id;
}

/*!
    Copy constructor
 */
NmMessageEnvelope::NmMessageEnvelope(const NmMessageEnvelope &envelope):d(envelope.d)
{
}

/*!
    Constructs new NmMessageEnvelope object from private envelope data
 */
NmMessageEnvelope::NmMessageEnvelope(QExplicitlySharedDataPointer<NmMessageEnvelopePrivate> nmPrivateMessageEnvelope)
{
    d = nmPrivateMessageEnvelope;
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


/*!
    Equal operator returns true if all values are same,
    function is case insensitive
 */
bool NmMessageEnvelope::operator==(const NmMessageEnvelope &envelope) const
{
    bool ret = false;
    if (0 == subject().compare(envelope.subject(), Qt::CaseInsensitive)
        && sender() == envelope.sender()
        && sentTime() == envelope.sentTime()
        && flags() == envelope.flags()
        && mailboxId() == envelope.mailboxId()
        && parentId() == envelope.parentId()) {
        ret = true;
    }
    return ret;
}

/*!
    Not equal operator return true if at least one value in not equal,
    function is case insensitive
 */
bool NmMessageEnvelope::operator!=(const NmMessageEnvelope &envelope) const
{
    return !(*this==envelope);
}

/*!
    Destructor
 */
NmMessageEnvelope::~NmMessageEnvelope()
{
}

/*!
    Sets message id
 */
void NmMessageEnvelope::setId(const NmId &id)
{
    d->mId = id;
}

/*!
    Returns id of message
 */
NmId NmMessageEnvelope::id() const
{
    return d->mId;
}

/*!
    Returns parent id of this envelope
 */
NmId NmMessageEnvelope::parentId() const
{
    return d->mParentId;
}

/*!
    Sets the parent id for this envelope
 */
void NmMessageEnvelope::setParentId(const NmId &id)
{
    d->mParentId = id;
}

/*!
    Returns mailbox id of this envelope
 */
NmId NmMessageEnvelope::mailboxId() const
{
    return d->mMailboxId;
}

/*!
    Sets the mailbox id for this envelope
 */
void NmMessageEnvelope::setMailboxId(const NmId &id)
{
    d->mMailboxId = id;
}

/*!
    Sets message subject
 */
void NmMessageEnvelope::setSubject(const QString &subject)
{
    d->mSubject = subject;
}

/*!
    Retruns subject string
 */
QString NmMessageEnvelope::subject() const
{
  return d->mSubject;
}

/*!
    Sets \a sender as a sender's address
 */
void NmMessageEnvelope::setSender(const NmAddress &sender)
{
    d->mSender = sender;
}

/*!
    Returns sender string
 */
NmAddress NmMessageEnvelope::sender() const
{
    return d->mSender;
}

/*!
    Sets sent time to \a sentTime
 */
void NmMessageEnvelope::setSentTime(const QDateTime &sentTime)
{
    d->mSentTime = sentTime;
}

/*!
    Returns sent time, if not set returns null time
 */
QDateTime NmMessageEnvelope::sentTime() const
{
    return d->mSentTime;
}

/*!
    Sets message contains attachments if \a hasAttachments is true
 */
void NmMessageEnvelope::setHasAttachments(bool hasAttachments)
{
    d->setFlag(NmMessageFlagAttachments, hasAttachments);
}

/*!
    Returns true if message contains attachments
 */
bool NmMessageEnvelope::hasAttachments() const
{
    return d->isFlagSet(NmMessageFlagAttachments);
}

/*!
    Set message to read if \a read is true
 */
void NmMessageEnvelope::setRead(bool read)
{
    d->setFlag(NmMessageFlagRead, read);
}

/*!
    Returns true if message is read
 */
bool NmMessageEnvelope::isRead() const
{
    return d->isFlagSet(NmMessageFlagRead);
}

/*!
    Sets message to replied state if \a replyed is true
 */
void NmMessageEnvelope::setReplied(bool replyed)
{
    d->setFlag(NmMessageFlagAnswered, replyed);
}

/*!
    Returns true if message is replied
 */
bool NmMessageEnvelope::isReplied() const
{
    return d->isFlagSet(NmMessageFlagAnswered);
}

/*!
    Sets message to forwarded state if \a replyed is true
 */
void NmMessageEnvelope::setForwarded(bool forwarded)
{
    d->setFlag(NmMessageFlagForwarded, forwarded);
}

/*!
    Returns true if message is forwarded
 */
bool NmMessageEnvelope::isForwarded() const
{
    return d->isFlagSet(NmMessageFlagForwarded);
}

/*!
    Sets message priority
 */
void NmMessageEnvelope::setPriority(NmMessagePriority priority)
{
    if (priority == NmMessagePriorityLow) {
        d->setFlag(NmMessageFlagLow, true);
        d->setFlag(NmMessageFlagImportant, false);
    }
    else if (priority == NmMessagePriorityHigh) {
        d->setFlag(NmMessageFlagLow, false);
        d->setFlag(NmMessageFlagImportant, true);
    }
    else {
        d->setFlag(NmMessageFlagLow, false);
        d->setFlag(NmMessageFlagImportant, false);
    }
}

/*!
    Returns message priority
 */
NmMessagePriority NmMessageEnvelope::priority() const
{
    NmMessagePriority ret = NmMessagePriorityNormal;
    if (d->isFlagSet(NmMessageFlagImportant)) {
        ret = NmMessagePriorityHigh;
    }
    else if (d->isFlagSet(NmMessageFlagLow)) {
        ret = NmMessagePriorityLow;
    }
    return ret;
}

/*!
    Returns message flags
 */
NmMessageFlags NmMessageEnvelope::flags() const
{
    return d->flags();
}

/*!
    Returns list of 'to recipients'
 */
QList<NmAddress>& NmMessageEnvelope::toRecipients() const
{
    return d->mToRecipients;
}

/*!
    Sets the list of 'to recipients'
 */
void NmMessageEnvelope::setToRecipients(QList<NmAddress> toRecipients)
{
    clearToRecipients();
    d->mToRecipients = toRecipients;
}

/*!
    Clears the list of 'to recipients'
 */
void NmMessageEnvelope::clearToRecipients()
{
    d->mToRecipients.clear();
}

/*!
    Returns list of 'cc recipients'
 */
QList<NmAddress>& NmMessageEnvelope::ccRecipients() const
{
    return d->mCcRecipients;
}

/*!
    Sets the list of 'cc recipients'
 */
void NmMessageEnvelope::setCcRecipients(QList<NmAddress> ccRecipients)
{
    clearCcRecipients();
    d->mCcRecipients = ccRecipients;
}

/*!
    Clears the list of 'cc recipients'
 */
void NmMessageEnvelope::clearCcRecipients()
{
    d->mCcRecipients.clear();
}

/*!
    Returns list of 'Bcc recipients'
 */
QList<NmAddress>& NmMessageEnvelope::bccRecipients() const
{
    return d->mBccRecipients;
}

/*!
    Sets the list of 'Bcc recipients'
 */
void NmMessageEnvelope::setBccRecipients(QList<NmAddress> bccRecipients)
{
    clearBccRecipients();
    d->mBccRecipients = bccRecipients;
}

/*!
    Clears the list of 'bcc recipients'
 */
void NmMessageEnvelope::clearBccRecipients()
{
    d->mBccRecipients.clear();
}

#endif /* NMMESSAGEENVELOPE_CPP_ */
