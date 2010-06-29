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
 * Description:
 *
 */
 
#include "nmapiheaders.h"


namespace EmailClientApi {


NmApiMailboxSettings::NmApiMailboxSettings(QObject *parent)
   :QObject(parent)
{
   d = new NmApiMailboxSettingsPrivate(this); 
   connect(d, SIGNAL(mailboxDeleted(int)),this,SIGNAL(mailboxDeleted(int)));
}

NmApiMailboxSettings::~NmApiMailboxSettings()
{
    delete d;
}

/*!
 \fn listMailboxIds
 \param idList - QList consists of ids as quint64.
 \return true is successful, otherwise return false

 Lists ids of mailboxes
 */
bool NmApiMailboxSettings::listMailboxIds(QList<quint64> &idList)
{
    return d->listMailboxIds(idList);
}

bool NmApiMailboxSettings::loadSettings(quint64 mailboxId, NmApiMailboxSettingsData &data)
{
	return d->loadSettings(mailboxId, data);
}

bool NmApiMailboxSettings::saveSettings(const NmApiMailboxSettingsData &data)
{
	return d->saveSettings(data);
}

/*!
 \fn createMailbox
 \param mailboxType - mailbox type as QString
 \param data - mailbox settings data

 Creates a mailbox
 */
bool NmApiMailboxSettings::createMailbox(const QString &mailboxType, NmApiMailboxSettingsData &data)
{
    return d->createMailbox(mailboxType, data);
}

/*!
 \fn deleteMailbox
 \param mailboxId - mailbox id.
 \return quint64 - the mailbox id.
 
 Deletes mailbox with specific id.
 */
bool NmApiMailboxSettings::deleteMailbox(quint64 mailboxId)
{
    return d->deleteMailbox(mailboxId);
}

/*!
 \fn populateDefaultSettings
 \param mailboxType - mailbox type as QString
 \param data - mailbox settings data, on return contains dafault settings for mailbox protocol
 \return true if default settings are set, otherwise false
 
 Populates NmApiMailboxSettingsData with protocol specific settings data.
 */
bool NmApiMailboxSettings::populateDefaultSettings(const QString &mailboxType, NmApiMailboxSettingsData &data)
{
    return d->populateDefaultSettings(mailboxType, data);
}

}//end namespace
