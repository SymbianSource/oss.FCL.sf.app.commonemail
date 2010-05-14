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

#ifndef NMAPIENGINE_H
#define NMAPIENGINE_H

#include "nmcommon.h"
#include "nmapiprivateclasses.h"

class NmApiDataPluginFactory;

namespace EmailClientApi
{
class NmApiMailbox;
class NmApiFolder;
class NmApiMessageEnvelope;
}

class NmApiEngine : public QObject
{
    Q_OBJECT
public:
    static NmApiEngine* instance();
    static void releaseInstance(NmApiEngine *&engine);

    void listMailboxes(QList<EmailClientApi::NmApiMailbox> &mailboxList);

    void listFolders(const quint64 mailboxId, QList<EmailClientApi::NmApiFolder> &folderList);
    void listEnvelopes(const quint64 mailboxId, const quint64 folderId, 
	                   QList<EmailClientApi::NmApiMessageEnvelope> &messageEnvelopeList);

    bool getEnvelopeById(
        const quint64 mailboxId,
        const quint64 folderId,
        const quint64 envelopeId,
        EmailClientApi::NmApiMessageEnvelope &envelope);
		
    bool getMailboxById(const quint64 mailboxId, EmailClientApi::NmApiMailbox &mailbox);

    void startCollectingEvents();
    
    signals:
    /*!
       It contains info about event in emailstore.
       
       \arg Contains info about event and related object (message or mailbox list)
     */
    void emailStoreEvent(NmApiMessage message);

private slots:
    void mailboxChangedArrived(NmMailboxEvent, const QList<NmId> &mailboxIds);
    void messageChangedArrived(
        NmMessageEvent messageEvent,
        const NmId &folderId,
        const QList<NmId> &messageIds,
        const NmId &mailboxId);

private:
    NmApiEngine();
    virtual ~NmApiEngine();

private:
    static NmApiEngine *mInstance;//!<Static instance of NmApiEngine. There can be only one instance of engine
    static quint32 mReferenceCount;//!<Count of refences to engine instance

    NmApiDataPluginFactory *mFactory;//!<Plugin factory. Is needed to get plugins for emails
};

#endif /* NMAPIENGINE_H */
