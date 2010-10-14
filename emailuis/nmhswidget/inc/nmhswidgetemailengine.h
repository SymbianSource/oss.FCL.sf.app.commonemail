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
class QTimer;
class XQAiwRequest;
class NmHsWidgetListModel;


enum NmHsWidgetEmailEngineExceptionCode
    {
    NmEngineExcFailure,
    NmEngineExcAccountDeleted
    };

class NmHsWidgetEmailEngine : public QObject
    {
    Q_OBJECT
    
public:
    NmHsWidgetEmailEngine( const NmId& monitoredMailboxId);
    bool initialize(); 
    ~NmHsWidgetEmailEngine();

    int unreadCount();
    QString accountName();
    void deleteAiwRequest();
        
public slots:
    void handleMessageEvent( 
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId);

    void handleFolderEvent(
                NmFolderEvent event,
                const QList<NmId> &folderIds,
                const NmId& mailboxId);

    
    void handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds);
   
    //Activity control
    void suspend();
    void activate();
    void forceUpdate();

    void launchMailAppInboxView();
    void launchMailAppMailViewer(const NmId &messageId);
    
    void aiwRequestOk(const QVariant& result);
    void aiwRequestError(int errorCode, const QString& errorMessage);
    
signals:
    //all mail data was refreshed (list contains all items)
    void mailDataRefreshed(const QList<NmMessageEnvelope*>&);   
    //all mail data was cleared
    void mailDataCleared();
    //new mails received (list contains the only items)
    void mailsReceived(const QList<NmMessageEnvelope*>&);
    //mail items updated (list contains changed items)
    void mailsUpdated(const QList<NmMessageEnvelope*>&);
    //mails deleted (list contains the id's of the deleted items)
    void mailsDeleted(const QList<NmId>&);
    
    void accountNameChanged(const QString& accountName);
    void unreadCountChanged(const int& unreadCount);
    void exceptionOccured(const int& err);

private:
    bool constructNmPlugin();
    bool updateData(); 
    bool updateUnreadCount();
    bool updateAccount();
    QList<NmMessageEnvelope*> getEnvelopesFromIds(const QList<NmId> messageIds);
    NmMessageEnvelope* envelopeById(const NmId &messageId);
    
private:
    NmId mMailboxId;
    NmId mFolderId;
    QString mAccountName;
    int mUnreadCount;
    NmDataPluginInterface *mEmailInterface;
    NmDataPluginFactory* mFactory;
    //suspension variables
    bool mAccountDataNeedsUpdate;
    bool mMessageDataNeedsUpdate;
    bool mSuspended; 
    XQAiwRequest* mAiwRequest;
    };

#endif /* NMHSWIDGETEMAILENGINE_H_ */
