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

#include "nmenginedef.h"
#include "nmcommon.h"
#include "nmapimailbox.h"
#include "nmapifolder.h"
#include "nmprivateclasses.h"
#include "nmapimessageenvelope.h"

class NmDataPluginFactory;

class NmEngine : public QObject
{
    Q_OBJECT
public:
    static NmEngine* instance();
    static void releaseInstance(NmEngine *&engine);

    void listMailboxes(QList<EmailClientApi::NmMailbox> &mailboxList);

    void listFolders(const quint64 mailboxId, QList<EmailClientApi::NmFolder> &folderList);
    void listEnvelopes(const quint64 mailboxId, const quint64 folderId, QList<
        EmailClientApi::NmMessageEnvelope> &messageEnvelopeList);

    bool envelopeById(
        const quint64 mailboxId,
        const quint64 folderId,
        const quint64 envelopeId,
        EmailClientApi::NmMessageEnvelope &envelope);
    bool mailboxById(const quint64 mailboxId, EmailClientApi::NmMailbox &mailbox);

    void startCollectingEvents();
    signals:
    /*!
     * It contains info about event in emailstore.
     * 
     * \arg Contains info about event and related object (message or mailbox list)
     */
    void emailStoreEvent(NmApiMessage message);

public slots:
    void mailboxChangedArrived(NmMailboxEvent, const QList<NmId> &mailboxIds);
    void messageChangedArrived(
        NmMessageEvent messageEvent,
        const NmId &folderId,
        const QList<NmId> &messageIds,
        const NmId &mailboxId);

private:
    NmEngine();
    virtual ~NmEngine();

private:
    static NmEngine *mInstance;//!<Static instance of NmEngine. There can be only one instance of engine
    static quint32 mReferenceCount;//!<Count of refences to engine instance

    NmDataPluginFactory *mFactory;//!<Plugin factory. Is needed to get plugins for emails
};

#endif /* NMENGINE_H_ */
