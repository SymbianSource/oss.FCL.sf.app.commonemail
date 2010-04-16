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
class NmFrameworkAdapter;

class NmMailboxInfo 
{
public:    
    NmId mId;
    QString mName;
    NmSyncState mSyncState;
    NmConnectState mConnectState;
    int mUnreadMails;
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
            const NmId mailboxId);

    void handleConnectionEvent(NmConnectState state, const NmId mailboxId);
    
private:
    
    bool loadAdapter();
    
    void initMailboxStatus();
    
    int getUnreadCount(const NmId& mailboxId, int maxCount);
    
    void updateStatus();

    bool isMailboxActive(const NmMailboxInfo& mailboxInfo);
    
    bool updateIndicator(int mailboxIndex, 
        bool active,
        const NmMailboxInfo& mailboxInfo);
    
    NmMailboxInfo* getMailboxInfo(const NmId &id);
    
    NmMailboxInfo* createMailboxInfo(const NmId &id);

    NmMailboxInfo* createMailboxInfo(const NmMailbox &mailbox);
    
    bool removeMailboxInfo(const NmId &id);
    
    bool updateMailboxActivity(const NmId &mailboxId, bool active);
    
    static QStringList pluginFolders();
    
private: // data
    
    NmFrameworkAdapter *mAdapter; // Owned
    QList<NmMailboxInfo*> mMailboxes;
    int mActiveIndicators;
};


#endif // NMMAILAGENT_H
