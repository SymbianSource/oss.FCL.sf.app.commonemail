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



#include "nmuiengineheaders.h"

/*!
    \class NmMessageListModelItem
    \brief The NmMessageListModelItem class represents data for NmMessageListModel.
    @alpha

    The NmMessageListModelItem class provides an item that is 
    used by NmMessageListModel and its' data
    method to get all information needed for one list row for a widget by calling the method once.
*/

class NmMessageListModelItemPrivate
{
public:
    // mEnvelope is not created here to avoid unnecessary create/delete operations,
    // usual use case is setEnvelope
	NmMessageListModelItemPrivate()
	:mEnvelope(NULL), 
    mType(NmMessageListModelItem::NmMessageItemMessage),
	mExpanded(false) {}
	virtual ~NmMessageListModelItemPrivate(){ delete mEnvelope; }
	NmMessageEnvelope *mEnvelope;
    QString mDivider;
    NmMessageListModelItem::NmMessageItemType mType;
    bool mExpanded;
};


/*!
    Constructor.
*/
NmMessageListModelItem::NmMessageListModelItem()
{
    d = new NmMessageListModelItemPrivate();
}


/*!
    Destructor.
*/
NmMessageListModelItem::~NmMessageListModelItem()
{
    delete d;
}

/*!
    Sets message envelope to list model item. Makes own copy from envelope.
*/
void NmMessageListModelItem::setEnvelope(const NmMessageEnvelope &envelope)
{
    delete d->mEnvelope;
    d->mEnvelope = new NmMessageEnvelope(envelope);
	emitDataChanged();
}

/*!
    Sets message envelope pointer to list model item. Takes envelope ownership.
*/
void NmMessageListModelItem::setEnvelope(NmMessageEnvelope *envelope)
{
    if (envelope) {
        delete d->mEnvelope;
        d->mEnvelope = envelope;
	    emitDataChanged();
    }
}

/*!
    Returns const reference to envelope data
*/
const NmMessageEnvelope &NmMessageListModelItem::envelope() const
{
    if (!d->mEnvelope) {
        d->mEnvelope = new NmMessageEnvelope();
    }
	return *(d->mEnvelope);
}

/*!
    Returns pointer to envelope data, ownership of envelope object is not transferred. Do not
    delete or null envelope object
*/
NmMessageEnvelope *NmMessageListModelItem::envelopePtr()
{
    if (!d->mEnvelope) {
        d->mEnvelope = new NmMessageEnvelope();
    }
	return d->mEnvelope;
}

/*!
    Returns item type, list item can be divider or message
*/
NmMessageListModelItem::NmMessageItemType NmMessageListModelItem::itemType() const
{
    return d->mType;
}

/*!
    Sets item type
*/
void NmMessageListModelItem::setItemType(NmMessageListModelItem::NmMessageItemType type)
{
    d->mType = type;
}

/*!
    Title divider accessor
*/
QString NmMessageListModelItem::titleDivider() const
{
    return d->mDivider;
}

/*!
    Set Title divider.
*/
void NmMessageListModelItem::setTitleDivider(const QString &divider)
{
    d->mDivider = divider;
}

/*!
    Returns boolean is divider expanded
*/
bool NmMessageListModelItem::expanded() const
{
    return d->mExpanded;
}

/*!
    Sets item expanded
*/
void NmMessageListModelItem::setExpanded(bool expanded)
{
    d->mExpanded = expanded;
}

/*
    Emits data changed signal
 */
void NmMessageListModelItem::callEmitDataChanged()
{
    emitDataChanged();
}
