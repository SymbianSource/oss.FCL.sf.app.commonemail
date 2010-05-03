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

#ifndef NMHSWIDGETEMAILENGINE_H_
#define NMHSWIDGETEMAILENGINE_H_

#include <QObject>
#include "nmcommon.h"

class NmMessageEnvelope;
class NmDataPluginFactory;
class NmDataPluginInterface;
class QPluginLoader;



enum NmHsWidgetEmailEngineErrorCode
    {
    NmEngineNoErr,
    NmEngineErrNotFound,
    NmEngineErrFailure
    };

//Maximum amount of envelopes that can be provided to client in getData function
//This is also the amount of envelopes that is kept in mData all the time
const int KMaxNumberOfEnvelopesProvided = 2;

//Maximum value for unread count. Counting will stop when this limit is reached
const int KMaxUnreadCount = 999; 

class NmHsWidgetEmailEngine : public QObject
    {
    Q_OBJECT
public:
    NmHsWidgetEmailEngine( const NmId& monitoredMailboxId );
    ~NmHsWidgetEmailEngine();

    int getEnvelopes(QList<NmMessageEnvelope> &list, int maxEnvelopeAmount);
    int unreadCount();
    QString accountName();
        
public slots:
    void handleMessageEvent( 
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId);

    void handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds);
   
    //Activity control
    void suspend();
    void activate();
    void launchMailAppInboxView();
    void launchMailAppMailViewer(const NmId &messageId);
    
signals:
    void mailDataChanged();    
    void accountNameChanged(const QString& accountName);
    void unreadCountChanged(const int& unreadCount);
    void errorOccured(NmHsWidgetEmailEngineErrorCode err);
    
private:
    void constructNmPlugin();
    void updateData(); 
    void updateAccount();
    void resetEnvelopeList();
    
private:
    NmId mMailboxId;
    NmId mFolderId;
    QString mAccountName;
    int mUnreadCount;
    QList<NmMessageEnvelope*> mEnvelopeList;
    NmDataPluginInterface *mEmailInterface;
    NmDataPluginFactory* mFactory;
    //suspension variables
    bool mAccountEventReceivedWhenSuspended;
    bool mMessageEventReceivedWhenSuspended;
    bool mSuspended; 
    };

#endif /* NMHSWIDGETEMAILENGINE_H_ */
