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

#include "nmmessage.h"
#include "nmmessageenvelope.h"


/*!
    \class NmMessage
    \brief Data model for mail message
 */

/*!
    Constructor
 */
NmMessage::NmMessage()
{
}

/*!
    Constructs message with id \a id, parent id and mailbox id is set to null id
 */
NmMessage::NmMessage(const NmId &id):NmMessagePart(id)
{
    mEnvelope.setId(id);
}

/*!
    Constructs message part with id \a id and with parent id \a parentId,
    mailbox id is set to null id
 */
NmMessage::NmMessage(const NmId &id, const NmId &parentId)
:NmMessagePart(id, parentId)
{
	mEnvelope.setId(id);
}

/*!
    Constructs message with id \a id, with parent id \a parentId and
    with mailbox id \a mailboxId
 */
NmMessage::NmMessage(const NmId &id,
                     const NmId &parentId,
                     const NmId &mailboxId)
:NmMessagePart(id, parentId, mailboxId)
{
	mEnvelope.setId(id);
}

/*!
    Constructs message with meta data
 */
NmMessage::NmMessage(const NmMessageEnvelope &envelope)
{
	mEnvelope = envelope;
    // set message id same as meta data id
    this->setId(envelope.id());
}

/*!
    Copy constructor
 */
NmMessage::NmMessage(const NmMessagePart& message):NmMessagePart(message)
{
	mEnvelope.setId( message.id() );
}

NmMessage::NmMessage(const NmMessage& message):NmMessagePart(message)
{
	mEnvelope.setId( message.id() );
}

/*!
    Assignment operator
 */
NmMessage &NmMessage::operator=(const NmMessage &message)
{
	if (this != &message) {
		mEnvelope.setId( message.id() );
	}
	return *this;
}

/*!
    Destructor
 */
NmMessage::~NmMessage()
{
}

/*!
    returns id of envelope
 */
NmId NmMessage::id() const
{
    return mEnvelope.id();
}


/*!
    Sets id to envelope
 */
void NmMessage::setId(const NmId &id)
{
    //NmMessagePart::setId(id);
    mEnvelope.setId(id);
}

/*!
    Returns pointer to plain text body, if plain text body is not found
    returns null pointer. Ownership of message part object is not transferred.
    Do not delete returned pointer.
 */
NmMessagePart *NmMessage::plainTextBodyPart()
{
    NmMessagePart *ret = NULL;
    ret = findContentPart(NmContentTypeTextPlain);
    return ret;
}

/*!
    Returns pointer to html body, if html body is not found
    returns null pointer. Ownership of message part object is not transferred.
    Do not delete returned pointer.
 */
NmMessagePart *NmMessage::htmlBodyPart()
{
    NmMessagePart *ret = NULL;
    ret = findContentPart(NmContentTypeTextHtml);
    return ret;
}

/*!
    Sets message meta data. If meta data id is different than null id, messages
    id is set to \a envelope's id
 */
void NmMessage::setEnvelope(const NmMessageEnvelope &envelope)
{
    mEnvelope = envelope;
    if (envelope.id() != 0) {
        this->setId(envelope.id());
    } else {
        mEnvelope.setId(this->id());
    }
}

/*!
    Returns reference to message envelope
 */
NmMessageEnvelope &NmMessage::envelope()
{
    return mEnvelope;
}

/*!
    Returns reference to message envelope
 */
const NmMessageEnvelope &NmMessage::envelope() const
{
    return mEnvelope;
}

/**
* Returns a flat list of message parts that can be handled as attachments.
* Excludes message parts that are multipart and parts that are considered
* plain text or html body.
* 
* @param flat list of attachments to be filled
*/        
void NmMessage::attachmentList(QList<NmMessagePart*> &parts)
{
    parts.clear();
    appendAttachments(parts);

    // special case, if single part content type is not given,
    // default type is text/plain
    if ( parts.count() == 1 && parts.at(0)->contentType().length() == 0) {
        parts.clear();
    }
    else {
        // find plain text body part from the list
        NmMessagePart* txtPart = findContentPart(NmContentTypeTextPlain);
        if ( txtPart ) {
            // remove plain text body part from attachment list
            for ( int ii = parts.count() - 1; ii >= 0; --ii ) {
                if ( parts.at(ii)->id() == txtPart->id() ) {
                    parts.removeAt(ii);
                    break;
                }
            }
        }
        // find  html body part from the list
        NmMessagePart* htmlPart = findContentPart(NmContentTypeTextHtml);
        if ( htmlPart ) {
            // remove html body part from attachment list
            for ( int ii = parts.count() - 1; ii >= 0; --ii ) {
                if ( parts.at(ii)->id() == htmlPart->id() ) {
                    parts.removeAt(ii);
                    break;
                }
            }
        }
    }
}
