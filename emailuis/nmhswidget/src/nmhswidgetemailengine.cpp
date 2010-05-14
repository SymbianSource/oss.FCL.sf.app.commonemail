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

#include <xqservicerequest.h>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <qpluginloader.h>
#include "email_services_api.h"
#include "nmcommon.h"
#include "nmmessageenvelope.h"
#include "nmhswidgetemailengine.h"
#include "nmdataplugininterface.h"
#include "nmmailbox.h"
#include "nmfolder.h"
#include "nmdatapluginfactory.h"
#include "nmhswidgetconsts.h"

/*!
 Constructor
 */
NmHsWidgetEmailEngine::NmHsWidgetEmailEngine(const NmId& monitoredMailboxId) :
    mMailboxId(monitoredMailboxId), 
    mFolderId(0), 
    mAccountName(0), 
    mUnreadCount(-1),
    mEmailInterface(0), 
    mFactory(0), 
    mAccountEventReceivedWhenSuspended(false),
    mMessageEventReceivedWhenSuspended(false), 
    mSuspended(false),
    mUpdateTimer(0)
{
    qDebug() << "NmHsWidgetEmailEngine() -- START";

    qDebug() << "NmHsWidgetEmailEngine() -- END";
}

/*!
 initilaize - Initializes the engine
 Works as a second phase constructor. Must be called before any other operations.
 Client should connect to exceptionOccured signal before calling this method.
 /return true if initialize succeedes otherwise false. If false the object is unusable.
 */
bool NmHsWidgetEmailEngine::initialize()
{
    qDebug() << "initialize() -- START";
        
    if (!constructNmPlugin()) {
        //if plugin connection fails, there's no reason to proceed
        return false;
    }
    updateData();
    updateAccount();
    
    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(NmHsWidgetEmailEngineUpdateTimerValue);
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(handleUpdateTimeout()) );
    
    qDebug() << "initialize() -- END";
    return true;
}

/*!
 constructNmPlugin
 /post Constructs a connection to an instance of NmDataPluginInterface
 /return true if construction succeeded, otherwise false.
 */
bool NmHsWidgetEmailEngine::constructNmPlugin()
{
    qDebug() << "NmHsWidgetEmailEngine::constructNmPlugin() -- START";

    QObject* pluginInstance(0);
    //Get data plugin factory instance
    mFactory = NmDataPluginFactory::instance();
    if (!mFactory) {
        qDebug() << "NmHsWidgetEmailEngine::constructNmPlugin() -- mFactory FAILED";
        return false;
    }

    //Get plugin instance
    pluginInstance = mFactory->pluginInstance(mMailboxId);
    if (!pluginInstance) {
        qDebug() << "NmHsWidgetEmailEngine::constructNmPlugin() -- pluginInstance FAILED";
        return false;
    }

    //get interface instance
    mEmailInterface = mFactory->interfaceInstance(pluginInstance);

    if (!mEmailInterface) {
        qDebug() << "NmHsWidgetEmailEngine::constructNmPlugin() -- mEmailInterface FAILED";
        return false;       
    }
    //Verify that the mailbox we are interested actually exists.
    //Otherwise emit account deleted event (instead of just returning)
    QList<NmId> ids; 
    mEmailInterface->listMailboxIds(ids);
    if(!ids.contains(mMailboxId)){
        qDebug() << "NmHsWidgetEmailEngine::constructNmPlugin() -- !ids.contains(mMailboxId) FAILED";
        emit exceptionOccured(NmEngineExcAccountDeleted);
        return false;
    }
    
    //retrieve folderID for this mailbox's inbox
    //If NmId equals zero, we don't have the Initial sync done for the account
    //This is valid at least for IMAP accounts. 
    //Folder ID is then retrieved later when first message event is received
    mFolderId = mEmailInterface->getStandardFolderId(mMailboxId, NmFolderInbox);
    qDebug() << "NmHsWidgetEmailEngine::constructNmPlugin() -- mFolderId==" << mFolderId.id();

    //Subscription is needed - otherwise the signals will not be received
    mEmailInterface->subscribeMailboxEvents(mMailboxId);

    //Connect to events
    connect(pluginInstance, SIGNAL( messageEvent(NmMessageEvent, const NmId&,
            const QList<NmId>&, const NmId&) ), this,
        SLOT( handleMessageEvent(NmMessageEvent, const NmId&,
                const QList<NmId>&, const NmId&) ));

    connect(pluginInstance, SIGNAL( mailboxEvent(NmMailboxEvent, const QList<NmId>& ) ), this,
        SLOT( handleMailboxEvent(NmMailboxEvent, const QList<NmId>&) ));

    qDebug() << "NmHsWidgetEmailEngine::constructNmPlugin() -- OK";

    return true;
}

/*!
 Reset envelope list
 \post mEnvelopeList.isEmpty() == true && all contained objects are deleted
 */
void NmHsWidgetEmailEngine::resetEnvelopeList()
{
    qDebug() << "NmHsWidgetEmailEngine::resetEnvelopeList() -- START";

    while (!mEnvelopeList.isEmpty()) {
        delete mEnvelopeList.takeFirst();
    }

    qDebug() << "NmHsWidgetEmailEngine::resetEnvelopeList() -- END";
}

/*!
 Destructor
 */
NmHsWidgetEmailEngine::~NmHsWidgetEmailEngine()
{
    qDebug() << "~NmHsWidgetEmailEngine -- START";

    resetEnvelopeList();
    if (mFactory) {
        NmDataPluginFactory::releaseInstance(mFactory);
    }
    
    if (mUpdateTimer){
        mUpdateTimer->stop();
        delete mUpdateTimer;
    }
    
    qDebug() << "~NmHsWidgetEmailEngine -- END";
}

/*!
 getEnvelopes() provides message envelopes as a list of stack objects
 Amount of message envelopes in the list parameter is the smallest of the following factors:
 'KMaxNumberOfEnvelopesProvided', 'maxEnvelopeAmount', 'amount of available envelopes'. 
 
 \param list list to be filled with message envelopes
 \param maxEnvelopeAmount Client side limit for amount of message envelope count. 
 \return count of envelopes added to list
 */
int NmHsWidgetEmailEngine::getEnvelopes(QList<NmMessageEnvelope> &list, int maxEnvelopeAmount)
{
    qDebug() << "NmHsWidgetEmailEngine::getEnvelopes()";
    list.clear(); //Reset the parameter list to avoid side effects
    int i = 0;
    for (; i < mEnvelopeList.count() && i < maxEnvelopeAmount; i++) {
        NmMessageEnvelope env(*mEnvelopeList[i]);
        list.append(env);
    }
    return i;
}

/*!
 UnreadCount

 \return count of unread mails, or -1 if there are no mails at all in the mailbox.
 */
int NmHsWidgetEmailEngine::unreadCount()
{
    qDebug() << "NmHsWidgetEmailEngine::unreadCount()";
    return mUnreadCount;
}

/*!
 accountName

 \return name of the monitored account
 */
QString NmHsWidgetEmailEngine::accountName()
{
    qDebug() << "NmHsWidgetEmailEngine::accountName()";
    return mAccountName;
}

/*!
 Refresh email data.
 \post mEnvelopeList is refreshed with valid content so that it has
       valid data with maximum of KMaxNumberOfEnvelopesProvided envelopes. 
       
 - emits exceptionOccured(NmEngineExcFailure) if fatal error occurs.
 - emits mailDataChanged() if new mail data is set into mEnvelopeList
 - emits unreadCountChanged(mUnreadCount), if mUnreadCount is updated
 
 \return true if everything succeeded, otherwise false
 */
bool NmHsWidgetEmailEngine::updateData()
{
    qDebug() << "NmHsWidgetEmailEngine::updateData() -- START";
    if (!mEmailInterface) {
        qDebug() << "NmHsWidgetEmailEngine::updateData() -- Interface missing";
        emit exceptionOccured(NmEngineExcFailure); //fatal error
        return false; //if interface is missing there's nothing to do
    }

    //reset envelope list before retrieving new items
    resetEnvelopeList();

    //get messages from inbox
    int msgErr = mEmailInterface->listMessages(mMailboxId, mFolderId, mEnvelopeList,
        KMaxNumberOfEnvelopesProvided);
    if (msgErr) {
        //retrieval of messages failed.  
        return false;
    }
    //emit signal about new message data right away
    emit mailDataChanged();
    //retrieve new unread count to mUnreadCount
    NmFolder* folder = NULL;
    int folderErr = mEmailInterface->getFolderById(mMailboxId, mFolderId, folder);
    if (folderErr) {
        //retrieval of valid folder failed.
        return false;
    }
    if (folder) {
        //If messageCount in the folder is zero we must indicate unread count to be -1
        if (folder->messageCount() == 0) {
            mUnreadCount = -1;
        }
        else {
            mUnreadCount = folder->unreadMessageCount();
        }
        delete folder;
        folder = NULL;
        //limit the unread count to KMaxUnreadCount
        if (mUnreadCount > KMaxUnreadCount) {
            mUnreadCount = KMaxUnreadCount;
        }
        //emit signal about changed unread count
        emit unreadCountChanged(mUnreadCount);
    }else{
        return false;
    }

    qDebug() << "NmHsWidgetEmailEngine::updateData() -- END";
    return true;
}

/*!
 handleMessageEvent slot.
 */
void NmHsWidgetEmailEngine::handleMessageEvent(
    NmMessageEvent event,
    const NmId &folderId,
    const QList<NmId> &messageIds,
    const NmId& mailboxId)
{
    qDebug() << "NmHsWidgetEmailEngine::handleMessageEvent() -- START";
    Q_UNUSED(event);
    Q_UNUSED(messageIds);
    
    if (!mEmailInterface) {
        qDebug() << "NmHsWidgetEmailEngine::handleMessageEvent() -- Interface missing";
        emit exceptionOccured(NmEngineExcFailure); //fatal error
        return; //if interface is missing there's nothing to do
    }
    
    if (mFolderId == NmId(0)) {
        // inbox was not created in construction phase, so let's
        // get id now as received first mail event
        mFolderId = mEmailInterface->getStandardFolderId(mMailboxId, NmFolderInbox);
    }
    if ((folderId == mFolderId) && (mailboxId == mMailboxId)) {
        //Data is updated only if the engine is not suspended
        if (mSuspended) {
            mMessageEventReceivedWhenSuspended = true;
        }
        else {
            //start or restart the timer. Update is started when timer expires
            mUpdateTimer->start();
        }
    }
    qDebug() << "NmHsWidgetEmailEngine::handleMessageEvent() -- END";
}

/*!
 handleMailboxEvent slot.
 */
void NmHsWidgetEmailEngine::handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds)
{
    qDebug() << "NmHsWidgetEmailEngine::handleMailboxEvent() -- START";
    if (mailboxIds.contains(mMailboxId)) {
        switch (event) {
            case (NmMailboxChanged): {
                if (mSuspended) {
                    mAccountEventReceivedWhenSuspended = true;
                }
                else {
                    updateAccount();
                }
                break;
            }
            case (NmMailboxDeleted): {
                emit exceptionOccured(NmEngineExcAccountDeleted);
                break;
            }
            default:
                //ignored
                break;
        }
    }
    qDebug() << "NmHsWidgetEmailEngine::handleMailboxEvent() -- END";
}

/*!
 * handleUpdateTimeout slot
 */
void NmHsWidgetEmailEngine::handleUpdateTimeout()
{
    qDebug() << "NmHsWidgetEmailEngine::handleUpdateTimeout() -- START";
    if (mUpdateTimer){
        mUpdateTimer->stop();
    }
    updateData();
    qDebug() << "NmHsWidgetEmailEngine::updateAccount() -- END";
}

/*!
 Update Account data
 \post if mEmailInterface exists, the mAccountName is refreshed from adapter 
 and accountNameChanged signal is emitted.
 */
bool NmHsWidgetEmailEngine::updateAccount()
{
    qDebug() << "NmHsWidgetEmailEngine::updateAccount() -- START";

    NmMailbox* box = NULL;
    if (mEmailInterface) {
        int err = mEmailInterface->getMailboxById(mMailboxId, box);
        if (err) {
            //By specification return and wait another event
            return false;
        }
    }
    if (box) {
        mAccountName = box->name();
        delete box;
        box = NULL;
        emit accountNameChanged(mAccountName);
    }
    qDebug() << "NmHsWidgetEmailEngine::updateAccount() -- END";
    return true;
}

/*!
 suspend slot.
 \post engine will not emit signals or refresh its data during suspension.
 */
void NmHsWidgetEmailEngine::suspend()
{
    qDebug() << "NmHsWidgetEmailEngine::suspend() -- START";
    mSuspended = true;
    qDebug() << "NmHsWidgetEmailEngine::suspend() -- END";
}

/*!
 activate slot.
 \post Engine will immediately refresh all the data that has been announced to
 have changed during the suspension. Events are enabled.
 */
void NmHsWidgetEmailEngine::activate()
{
    qDebug() << "NmHsWidgetEmailEngine::activate() -- START";
    mSuspended = false;
    if (mAccountEventReceivedWhenSuspended) {
        mAccountEventReceivedWhenSuspended = false;
        updateAccount();
    }
    if (mMessageEventReceivedWhenSuspended) {
        mMessageEventReceivedWhenSuspended = false;
        updateData();
    }
    qDebug() << "NmHsWidgetEmailEngine::activate() -- END";
}

/*!
 launchMailAppInboxView slot.
 \post Mail application is launched to inbox view corresponding widget's mailbox id
 */
void NmHsWidgetEmailEngine::launchMailAppInboxView()
{
    qDebug() << "NmHsWidgetEmailEngine::launchMailAppInboxView() -- START";

    XQServiceRequest request(
        emailFullServiceNameMailbox,
        emailOperationViewInbox,
        false);
    
    QList<QVariant> list;
    list.append(QVariant(mMailboxId.id()));

    request.setArguments(list);
    request.send();

    qDebug() << "NmHsWidgetEmailEngine::launchMailAppInboxView() -- END";
}

/*!
 launchMailAppMailViewer slot.
 \param messageId Defines the message opened to viewer
 \post Mail application is launched and viewing mail specified by
 */
void NmHsWidgetEmailEngine::launchMailAppMailViewer(const NmId &messageId)
{
    qDebug() << "NmHsWidgetEmailEngine::launchMailAppMailViewer() -- START";

    XQServiceRequest request(
       emailFullServiceNameMessage,
       emailOperationViewMessage,
       false);
    
    QList<QVariant> list;
    list.append(QVariant(mMailboxId.id()));
    list.append(QVariant(mFolderId.id()));
    list.append(QVariant(messageId.id()));

    request.setArguments(list);
    request.send();

    qDebug() << "NmHsWidgetEmailEngine::launchMailAppMailViewer() -- END";
}