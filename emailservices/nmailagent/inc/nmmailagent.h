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

#ifndef NMMAILAGENT_H
#define NMMAILAGENT_H

#include <nmcommon.h>

class NmMailbox;
class NmDataPluginFactory;
class NmDataPluginInterface;

class NmMailboxInfo
{
public:
    NmId mId;
    int mIndicatorIndex;
    QString mName;
    NmId mInboxFolderId;
    NmId mOutboxFolderId;
    NmSyncState mSyncState;
    NmConnectState mConnectState;
    int mInboxCreatedMessages;
    int mInboxChangedMessages;
    int mInboxDeletedMessages;
    int mUnreadMails;
    int mOutboxMails;
    bool mActive;

    NmMailboxInfo();
};

class NmMailAgent : public QObject
{
    Q_OBJECT

public:

    NmMailAgent();

    ~NmMailAgent();

    bool init();

public slots:

    void handleMailboxEvent(NmMailboxEvent event,
            const QList<NmId> &mailboxIds);

    void handleMessageEvent(
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId);

    void handleSyncStateEvent(
            NmSyncState state,
            const NmOperationCompletionEvent &event);

    void handleConnectionEvent(NmConnectState state, const NmId mailboxId);

    void delayedStart();

private:

    void initMailboxStatus();

    int getUnreadCount(const NmId& mailboxId, int maxCount);

    int getOutboxCount(const NmId& mailboxId);

    int getIndicatorIndex();

    bool isMailboxActive(const NmMailboxInfo& mailboxInfo);

    bool updateIndicator(bool active,
        const NmMailboxInfo& mailboxInfo);

    NmMailboxInfo* getMailboxInfo(const NmId &id);

    NmMailboxInfo* createMailboxInfo(const NmId &id);

    NmMailboxInfo* createMailboxInfo(const NmMailbox &mailbox,
        NmDataPluginInterface *plugin);

    bool removeMailboxInfo(const NmId &id);

    bool updateMailboxState(const NmId &mailboxId,
        bool active, bool refreshAlways);

    static QStringList pluginFolders();

private: // data

    NmDataPluginFactory *mPluginFactory;
    QList<NmMailboxInfo*> mMailboxes;
    bool mSendingState;
};


#endif // NMMAILAGENT_H
