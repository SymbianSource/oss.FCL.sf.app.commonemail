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
 * Description: Mail agent is responsible of monitoring mail accounts. It activates
 *              and deactivates indicator depending on received events. Mail agent is
 *              also responsible of monitoring silence mode, playing alert tone and vibrating
 *              phone when new mail is received.
 *
 */

#include "emailtrace.h"
#include "nmmailagentheaders.h"
#include "nmmailagent.h"

#include <xqaiwdeclplat.h>
#include <xqapplicationmanager.h>
#include <xqaiwrequest.h>

#include <xqsettingsmanager.h>
#include <xqcentralrepositoryutils.h>
#include <xqsystemtoneservice.h>
#include <xqsettingskey.h>
#include <ProfileEngineInternalCRKeys.h>

// CONSTANTS
const int NmAgentIndicatorNotSet = -1;
const int NmAgentAlertToneTimer = 60000; // 60s
const int NmAgentDefaultVibraDuration = 1000; // 1 second
static const quint32 NmRepositoryId = 0x2002C326;
static const QString NmMailboxIndicatorType = "com.nokia.nmail.indicatorplugin_%1/1.0";
static const QString NmSendIndicatorName = "com.nokia.nmail.indicatorplugin.send/1.0";
static const QString NmUnreadIndicatorName = "com.nokia.nmail.indicatorplugin.unread/1.0";
const XQCentralRepositorySettingsKey NmSilenceModeKey(KCRUidProfileEngine.iUid, KProEngSilenceMode);
const int NmSilenceModeOn = 1;

/*!
    Helper method for finding out if XQSettingsKey and XQCentralRepositorySettingsKey points to
    same key.

    \param settingKey XQSettingsKey
    \param cenrepSettingKey XQCentralRepositorySettingsKey
    \return <code>true</code> if target, uid and key matches otherwise returns <code>false</code>
 */
bool keysEqual(const XQSettingsKey& settingKey, const XQCentralRepositorySettingsKey& cenrepSettingKey)
{
    return ((settingKey.target() == cenrepSettingKey.target()) &&
        (settingKey.uid() == cenrepSettingKey.uid()) &&
        (settingKey.key() == cenrepSettingKey.key()));
}



/*!
    \class NmMailboxInfo

    \brief Main class for storing mailbox data.
 */
NmMailboxInfo::NmMailboxInfo()
: mId(0),
  mIndicatorIndex(NmAgentIndicatorNotSet),
  mInboxFolderId(0),
  mOutboxFolderId(0),
  mSyncState(SyncComplete),
  mConnectState(Disconnected),
  mInboxCreatedMessages(0),
  mInboxChangedMessages(0),
  mInboxDeletedMessages(0),
  mOutboxMails(0),
  mActive(false),
  mInboxActive(false)
  {
      NM_FUNCTION;
  }

/*!
    \class NmMailAgent

    \brief Main class for receiving email events. Activates and deactivates indicator.
 */
NmMailAgent::NmMailAgent() :
     mIndicator(NULL),
     mSystemTone(NULL),
     mPluginFactory(NULL),
     mVibra(NULL),
     mAlertToneAllowed(true),
     mLastOutboxCount(0),
     mUnreadIndicatorActive(false),
     mSettingManager(NULL),
     mSilenceMode(NmSilenceModeOn)  // by default silent mode is on
     {
     NM_FUNCTION;
     }

/*!
    Delayed start.
 */
void NmMailAgent::delayedStart()
{
    NM_FUNCTION;

    if (!init()) {
        // Initialisation failed. Quit the agent.
        QCoreApplication::exit(1);
    }
}

/*!
    Initialise the agent.

    \return true if succesfully started.
 */
bool NmMailAgent::init()
{
    NM_FUNCTION;

    mPluginFactory = NmDataPluginFactory::instance();
    if (!mPluginFactory) {
        NM_ERROR(1,"NmMailAgent::init(): PluginFactory not created");
        return false;
    }

    // Check status of silent mode.
    delete mSettingManager;
    mSettingManager = NULL;
    mSettingManager = new XQSettingsManager();
    QVariant silenceMode = mSettingManager->readItemValue(NmSilenceModeKey,
        XQSettingsManager::TypeInt);
    mSilenceMode = silenceMode.toInt();

    // Start monitoring silence mode key.
    bool monitoring(mSettingManager->startMonitoring(NmSilenceModeKey, XQSettingsManager::TypeInt));
    monitoring &= connect(mSettingManager,
                          SIGNAL(valueChanged(const XQSettingsKey &, const QVariant &)),
                          this,
                          SLOT(valueChanged(const XQSettingsKey &, const QVariant &)),
                          Qt::UniqueConnection);

    // If silence mode monitoring can't be started, then change silence mode on to be sure
    // that no tone is played if silence mode is turned on at somepoint.
    if (!monitoring) {
        mSilenceMode = NmSilenceModeOn;
    }

    delete mSystemTone;
    mSystemTone = NULL;
    mSystemTone = new XQSystemToneService();

    delete mIndicator;
    mIndicator = NULL;
    mIndicator = new HbIndicator();
    connect(mIndicator,SIGNAL(userActivated(const QString &, const QVariantMap &)),
        this, SLOT(indicatorActivated(const QString &, const QVariantMap &)));

    QList<QObject*> *plugins = mPluginFactory->pluginInstances();

    foreach (QObject *plugin, *plugins) {
        if (plugin) {
            // Start listening events
            connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                    this, SLOT(handleMailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                    Qt::UniqueConnection);

            connect(plugin, SIGNAL(messageEvent(
                        NmMessageEvent, const NmId &, const QList<NmId> &, const NmId &)),
                    this, SLOT(handleMessageEvent(
                        NmMessageEvent, const NmId &, const QList<NmId> &, const NmId &)),
                    Qt::UniqueConnection);

            connect(plugin, SIGNAL(syncStateEvent(NmSyncState, const NmOperationCompletionEvent &)),
                    this, SLOT(handleSyncStateEvent(NmSyncState, const NmOperationCompletionEvent &)),
                Qt::UniqueConnection);

            connect(plugin, SIGNAL(connectionEvent(NmConnectState, const NmId, int)),
                    this, SLOT(handleConnectionEvent(NmConnectState, const NmId, int)),
                    Qt::UniqueConnection);
        }
    }

    // Construct the vibra interface instance.
    delete mVibra;
    mVibra = NULL;
    TRAP_IGNORE(mVibra = CHWRMVibra::NewL());

    // load all current mailboxes
    initMailboxStatus();

    return true;
}

/*!
    Destructor of NmMailAgent.
 */
NmMailAgent::~NmMailAgent()
{
    NM_FUNCTION;

    delete mIndicator;
    delete mVibra;
    delete mSystemTone;
    if (mSettingManager) {
        mSettingManager->stopMonitoring(NmSilenceModeKey);
        delete mSettingManager;
    }
    qDeleteAll(mMailboxes);

    NmDataPluginFactory::releaseInstance(mPluginFactory);
}

/*!
    Initialize the mailbox list with the current state.
 */
void NmMailAgent::initMailboxStatus()
{
    NM_FUNCTION;

    QList<NmMailbox*> mailboxes;
    QList<QObject*> *plugins = mPluginFactory->pluginInstances();

    foreach(QObject* pluginObject, *plugins) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(pluginObject);
        if (plugin) {
            plugin->listMailboxes(mailboxes);
        }

        // Add the indicators
        // Must be made in reverse order to show them properly in
        // HbIndicator menu
        QListIterator<NmMailbox *> i(mailboxes);
        i.toBack();
        while (i.hasPrevious()) {
            const NmMailbox *mailbox = i.previous();
            if (mailbox) {
                NmMailboxInfo *mailboxInfo = createMailboxInfo(*mailbox, plugin);
                if (mailboxInfo) {
                    bool activate = updateUnreadCount(mailbox->id(), *mailboxInfo);
                    bool wasActive = isMailboxActive(mailbox->id());
                    if (activate) {
                        if (!wasActive) {
                            // do not activate the mailbox if it was left as hidden last time
                            activate = false;
                        }
                        else {
                            // otherwise, activate the mailbox and show inbox state
                            mailboxInfo->mInboxActive = true;
                        }
                    }

                    mailboxInfo->mOutboxMails = getOutboxCount(mailbox->id(),
                        mailboxInfo->mOutboxFolderId);
                    if (mailboxInfo->mOutboxMails > 0 && wasActive) {
                        activate = true;
                    }

                    // Create indicator for visible mailboxes
                    updateMailboxState(mailbox->id(), activate, false);
                }
            }
        }
        qDeleteAll(mailboxes);
    }
    updateUnreadIndicator();
    updateSendIndicator();
}

/*!
    Get mailbox unread count in inbox folder.

    \param mailboxId id of the mailbox
    \param mailboxInfo contains the list of unread messages
    \return true if new unread mails was found
 */
bool NmMailAgent::updateUnreadCount(const NmId &mailboxId, NmMailboxInfo &mailboxInfo)
{
    NM_FUNCTION;

    int newUnreadMessages(0);
    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);

    if (plugin) {
        NmId inboxId = mailboxInfo.mInboxFolderId.id();

        // Inbox folder ID may be still unknown
        if (inboxId == 0) {
            mailboxInfo.mInboxFolderId = plugin->getStandardFolderId(mailboxId, NmFolderInbox);
            inboxId = mailboxInfo.mInboxFolderId.id();
        }

        // get list of messages in inbox
        QList<NmMessageEnvelope*> messageList;
        plugin->listMessages(mailboxId, inboxId, messageList);

        QList<NmId> newUnreadMessageIdList;
        foreach (const NmMessageEnvelope* envelope, messageList) {
            // if the message is not read, it is "unread"
            if (!envelope->isRead()) {
                quint64 messageId = envelope->messageId().id();
                newUnreadMessageIdList.append(envelope->messageId());
                bool found(false);
                // Iterate through all known ids. If the id can't be found the mail is new.
                foreach (const NmId id, mailboxInfo.mUnreadMailIdList) {
                    if (id.id() == messageId) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    newUnreadMessages++;
                }
            }
        }
        qDeleteAll(messageList);

        // Save updated list of unread message IDs
        mailboxInfo.mUnreadMailIdList = newUnreadMessageIdList;
    }
    NM_COMMENT(QString("NmMailAgent::getUnreadCount(): count=%1, new=%2").
        arg(mailboxInfo.mUnreadMailIdList.count()).arg(newUnreadMessages));

    return (newUnreadMessages > 0);
}

/*!
    Get mailbox count in outbox folder.

    \param mailboxId id of the mailbox
    \param outboxId Outbox folder Id
    \return number of mails in the outbox
 */
int NmMailAgent::getOutboxCount(const NmId &mailboxId, const NmId &outboxId)
{
    NM_FUNCTION;

    int count(0);
    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);

    if (plugin) {
        // get list of messages in outbox
        QList<NmMessageEnvelope*> messageList;
        plugin->listMessages(mailboxId, outboxId, messageList);
        count = messageList.count();
        qDeleteAll(messageList);
    }
    NM_COMMENT(QString("NmMailAgent::getOutboxCount(): count=%1").arg(count));

    return count;
}

/*!
    Get list of unread counts in active mailboxes.

	\returns total number of unread mails
 */
int NmMailAgent::getTotalUnreadCount() const
{
    NM_FUNCTION;

    int unreads = 0;
    foreach (const NmMailboxInfo *mailbox, mMailboxes) {
        if (mailbox->mActive && mailbox->mInboxActive) {
            unreads += mailbox->mUnreadMailIdList.count();
        }
    }
    return unreads;
}

/*!
    Update the "@" indicator state according to unread state.

    \return true if the indicator was activated
 */
bool NmMailAgent::updateUnreadIndicator()
{
    NM_FUNCTION;

    int unreads = getTotalUnreadCount();
    return updateUnreadIndicator(unreads>0);
}

/*!
    Update the mailbox visibility and status.

    \param mailboxId id of the mailbox
    \param active visibility state of the mailbox
    \param refreshAlways true when the indicator should be always updated
    \return true if the mailbox state was changed
 */
bool NmMailAgent::updateMailboxState(const NmId &mailboxId,
    bool active, bool refreshAlways)
{
    NM_FUNCTION;

    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
    bool changed = false;
    if (mailboxInfo->mActive != active ||
        refreshAlways) {

        // store the new state to permanent storage
        storeMailboxActive(mailboxId, active);

        mailboxInfo->mActive = active;
        changed = true;
        if (active) {
            // Mailbox is not yet assigned to any indicator
            if (mailboxInfo->mIndicatorIndex < 0) {
                mailboxInfo->mIndicatorIndex = getFreeIndicatorIndex();
            }

            updateIndicator(true,*mailboxInfo);
        }
        else {
            // Indicator not anymore active. Release it.
            if (mailboxInfo->mIndicatorIndex>=0) {
                updateIndicator(false,*mailboxInfo);
                mailboxInfo->mIndicatorIndex = NmAgentIndicatorNotSet;
            }
        }
    }
    return changed;
}

/*!
    Updates indicator status.

    \param mailboxIndex index of the item shown in indicator menu
    \param active indicator visibility state
    \param mailboxInfo information of the mailbox
    \return true if indicator was updated with no errors
 */
bool NmMailAgent::updateIndicator(bool active,
    const NmMailboxInfo& mailboxInfo)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmMailAgent::updateIndicator(): index=%1, active=%2").
        arg(mailboxInfo.mIndicatorIndex).arg(active));

    bool ok = false;
    QString name = QString(NmMailboxIndicatorType).
        arg(mailboxInfo.mIndicatorIndex);

    QList<QVariant> list;
    list.append(mailboxInfo.mId.id());
    list.append(mailboxInfo.mName);
    list.append(mailboxInfo.mUnreadMailIdList.count());
    list.append(mailboxInfo.mSyncState);
    list.append(mailboxInfo.mConnectState);
    list.append(mailboxInfo.mOutboxMails);
    list.append(mailboxInfo.mIconName);
    list.append(mLastOutboxCount);

    if (active) {
        ok = mIndicator->activate(name,list);
    }
    else {
        ok = mIndicator->deactivate(name,list);
    }

    updateUnreadIndicator();

    return ok;
}

/*!
    Update the unread indicator state.

    \param active if true unread indicator is activated otherwise deactivated
    \return true if the indicator was activated
 */
bool NmMailAgent::updateUnreadIndicator(bool active)
{
    NM_FUNCTION;

    bool activated = false;
    if (active != mUnreadIndicatorActive) {
        if (active) {
            mIndicator->activate(NmUnreadIndicatorName);
            activated = true;
        }
        else {
            mIndicator->deactivate(NmUnreadIndicatorName);
        }
        mUnreadIndicatorActive = active;
    }

    return activated;
}

/*!
    Opens inbox view to specific mailbox.

    \param mailboxId Id of mailbox
    \return true if inbox is succesfully opened
 */
bool NmMailAgent::launchMailbox(quint64 mailboxId)
{
    NM_FUNCTION;

    bool ok(false);
    XQApplicationManager appManager;
    XQAiwRequest *request = appManager.create(
        XQI_EMAIL_INBOX_VIEW, XQOP_EMAIL_INBOX_VIEW, false);
    // Instance might be NULL if the service is not available.
    if (request) {
        QList<QVariant> list;
        list.append(QVariant(mailboxId));
        request->setArguments(list);
        ok = request->send();
        NM_COMMENT(QString("Launch ok=%1 error=%2").arg(ok).arg(request->lastError()));
        delete request;
    }
    return ok;
}

/*!
    Handles message created event.

    \param folderId Id of the folder that includes the message
    \param messageIds Message ids that are checked
    \param mailboxId Id of the mailbox that includes the message
    \param updateNeeded Set to <code>true</code> if update needed otherwise not touched
    \param activate Set to <code>true</code> if activation needed otherwise not touched
 */
void NmMailAgent::handleMessageCreatedEvent(const NmId &folderId, const QList<NmId> &messageIds,
    const NmId &mailboxId, bool &updateNeeded, bool &activate)
{
    NM_FUNCTION;

    // Check the new messages to make the indicator appear earlier
    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);

    // Inbox folder ID may be still unknown
    if (mailboxInfo->mInboxFolderId.id() == 0) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);

        if (plugin) {
            mailboxInfo->mInboxFolderId =
                plugin->getStandardFolderId(mailboxId, NmFolderInbox);
        }
    }

    if (folderId == mailboxInfo->mInboxFolderId) {
        foreach (NmId messageId, messageIds) {
            bool messageUnread = false;

            // Check the message if we can either play a tone or if the "@" is
            // not visible at the moment
            if (mAlertToneAllowed || !mUnreadIndicatorActive) {
                if (getMessageUnreadInfo(folderId, messageId, mailboxId, messageUnread)) {
                    if (messageUnread) {
                        mailboxInfo->mUnreadMailIdList.append(messageId);
                        mailboxInfo->mInboxActive = true;
                        updateMailboxState(mailboxId, true, false);

                        // make the "@" appear immediatelly
                        updateUnreadIndicator(true);

                        // Play the tone as well
                        playAlertTone();
                    }
                }
            }
        }
    }

    if (folderId==mailboxInfo->mInboxFolderId) {
        mailboxInfo->mInboxCreatedMessages += messageIds.count();
    }

    // When created a new mail in the outbox, we are in sending state
    if (mailboxInfo->mOutboxFolderId == folderId) {
        // The first mail created in the outbox
        if (mailboxInfo->mOutboxMails <= 0) {
            NM_COMMENT("NmMailAgent: first mail in outbox");
        }
        // Always activate the indicator
        activate = true;
        updateNeeded = true;

        mailboxInfo->mOutboxMails += messageIds.count();
        updateSendIndicator();
    }
}


/*!
    Handles message changed event.
    \sa updateUnreadCount

    \param folderId Id of the folder that includes the message
    \param messageIds Message ids that are checked
    \param mailboxId Id of the mailbox that includes the message
    \param updateNeeded Set to <code>true</code> if update needed otherwise not touched
    \param activate Set result value of updateUnreadCount method
 */
void NmMailAgent::handleMessageChangedEvent(const NmId &folderId, 
    const QList<NmId> &messageIds,
    const NmId &mailboxId,
    bool &updateNeeded, bool &activate)
{
    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);

    // we are interested only about changes in the inbox
    if (folderId == mailboxInfo->mInboxFolderId) {
        mailboxInfo->mInboxChangedMessages++;

        // If not currently syncronizing the mailbox, this may mean
        // that a message was read/unread
        if (mailboxInfo && mailboxInfo->mSyncState == SyncComplete) {
            // If there was no unread mails in the list, do nothing
            int oldCount(mailboxInfo->mUnreadMailIdList.count());
            if (oldCount>0) {
                // Check how many messages were unread earlier
                int unreadMessages = 0;
                foreach (NmId messageId, messageIds) {
                    if (mailboxInfo->mUnreadMailIdList.indexOf(messageId)>=0) {
                        unreadMessages++;
                    }
                }

                // Could lead the count drop to 0 => mailbox will be hidden
                if (unreadMessages > 0) {
                    // check the unread status again
                    activate = updateUnreadCount(mailboxId, *mailboxInfo);

                    // no more unread mails in the inbox
                    if (mailboxInfo->mUnreadMailIdList.count()==0) {
                        NM_COMMENT("NmMailAgent:handleMessageChangedEvent - no more unread msgs");
                        updateNeeded = true;
                    }
                }
            }
        }
    }
}

/*!
    Handles message deleted event.

    \param folderId Id of the folder that includes the message
    \param messageIds Message ids that are checked
    \param mailboxId Id of the mailbox that includes the message
    \param updateNeeded Set to <code>true</code> if update needed otherwise not touched
    \param activate Set to <code>true</code> if activation needed otherwise not touched
 */
void NmMailAgent::handleMessageDeletedEvent(const NmId &folderId, const QList<NmId> &messageIds,
    const NmId &mailboxId, bool &updateNeeded, bool &activate)
{
    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);

    if (folderId == mailboxInfo->mInboxFolderId) {
        mailboxInfo->mInboxDeletedMessages++;
    }

    // Deleted mails from the outbox
    if (mailboxInfo->mOutboxFolderId == folderId) {
        mailboxInfo->mOutboxMails -= messageIds.count();

        // Sanity check for the outbox count
        if (mailboxInfo->mOutboxMails < 0) {
            mailboxInfo->mOutboxMails = 0;
        }

        // The last mail was now deleted
        if (mailboxInfo->mOutboxMails == 0) {
            NM_COMMENT("NmMailAgent: last mail deleted from outbox");
            updateNeeded = true;

            // Keep it active if there is unread mails and inbox is still active
            if (mailboxInfo->mInboxActive &&
                mailboxInfo->mUnreadMailIdList.count() > 0) {
                activate = true;
            }
        }
        else {
            // Also update the indicator status if it is already shown
            if (mailboxInfo->mActive) {
                activate = true;
                updateNeeded = true;
            }
        }
        updateSendIndicator();
    }
}

/*!
    Get next free indicator index, starting from 0.

    \return index of the indicator that is available
 */
int NmMailAgent::getFreeIndicatorIndex()
{
    NM_FUNCTION;

    int index = 0;
    bool found(false);
    do {
        found = false;
        foreach (NmMailboxInfo *mailbox, mMailboxes) {
            if (mailbox->mIndicatorIndex == index &&
                mailbox->mActive) {
                found = true;
                index++;
            }
        }
    }
    while( found );
    return index;
}

/*!
    Received from NmFrameworkAdapter mailboxEvent signal
    \sa NmFrameworkAdapter

    \param event Mailbox event type
    \param mailboxIds Mailbox ids that are checked
 */
void NmMailAgent::handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmMailAgent::handleMailboxEvent(): event=%1").arg(event));

    switch(event) {
        case NmMailboxCreated:
            foreach (NmId mailboxId, mailboxIds) {
                NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId); // create a new mailbox if needed
                if (!mailboxInfo) {
                    // Unable to initialise the mailbox. Try again later.
                    NM_COMMENT("Cannot initialise mailbox");

                    qRegisterMetaType<NmId>("NmId");
                    QMetaObject::invokeMethod(this, "delayedMailboxCreated",
                        Qt::QueuedConnection, Q_ARG(NmId,mailboxId));
                }

                // make sure the mailbox activity data is reseted
                deleteStoredMailboxActivity(mailboxId);
            }
            break;
        case NmMailboxChanged:

            // Mailbox name may have been changed
            foreach (NmId mailboxId, mailboxIds) {
                NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
                NmMailbox *mailbox(NULL);
                NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);
                if (plugin) {
                    plugin->getMailboxById(mailboxId,mailbox);
                }
                if (mailbox && mailboxInfo) {
                    if(mailbox->name() != mailboxInfo->mName) {
                        mailboxInfo->mName = mailbox->name();

                        if (mailboxInfo->mActive) {
                            // Update the status of the mailbox
                            updateMailboxState(mailboxId, true, true);
                        }
                    }
                }
                delete mailbox;
            }
            break;
        case NmMailboxDeleted:
            foreach (NmId mailboxId, mailboxIds) {
                // Will hide also the indicator
                removeMailboxInfo(mailboxId);

                // make sure the mailbox activity data is deleted
                deleteStoredMailboxActivity(mailboxId);
            }
            updateUnreadIndicator();
            break;
        default:
            break;
    }
}

/*!
    Called when mailbox is initialised with a delay. This may happen at least when
    NmMailboxCreated event is received.

    \param mailboxId id of the mailbox
*/
void NmMailAgent::delayedMailboxCreated(const NmId mailboxId)
{
    NM_FUNCTION;

    // create and subscribe to the mailbox, if not done earlier
    getMailboxInfo(mailboxId);
}

/*!
   Map the type name to mailbox info.

   \param type Indicator type name
   \return NULL if no mailbox match the type
 */
NmMailboxInfo *NmMailAgent::getMailboxByType(const QString &type)
{
    NM_FUNCTION;

    NmMailboxInfo *foundMailbox = NULL;
    foreach (NmMailboxInfo *mailbox, mMailboxes) {
        // mailbox is shown in indicators
        if (mailbox->mIndicatorIndex >= 0 && mailbox->mActive) {
            QString typeName = QString(NmMailboxIndicatorType).arg(mailbox->mIndicatorIndex);

            // type names match(!)
            if (type == typeName) {
                foundMailbox = mailbox;
                break;
            }
        }
    }
    return foundMailbox;
}

/*!
    Called when indicator is clicked from the indicator menu
    - indicator will be hide from the menu
    - mailbox will be launched

    \param type Indicator type name
    \param data Data sent by indicator
 */
void NmMailAgent::indicatorActivated(const QString &type, const QVariantMap &data)
{
    NM_FUNCTION;
    Q_UNUSED(data);

    // map the indicator type to mailbox
    NmMailboxInfo *info = getMailboxByType(type);
    if (info) {
        info->mActive = false; // indicator is no longer active
        info->mInboxActive = false; // also inbox does not activate the indicator
        storeMailboxActive(info->mId, false);
        updateUnreadIndicator();

        launchMailbox(info->mId.id());
    }
}

/*!
    Called when cenrep key value has been changed.
    - only silence mode key handled

    \param key changed key
    \param value value for a key
 */
void NmMailAgent::valueChanged(const XQSettingsKey &key, const QVariant &value)
{
    NM_FUNCTION;

    if(keysEqual(key, NmSilenceModeKey)) {
        mSilenceMode = value.toInt();
    }
}

/*!
    Received from NmFrameworkAdapter messageEvent signal
    \sa NmFrameworkAdapter

    \param event Message event
    \param folderId Folder Id
    \param messageIds List of message Ids
    \param mailboxId Id of the mailbox
 */
void NmMailAgent::handleMessageEvent(
    NmMessageEvent event,
    const NmId &folderId,
    const QList<NmId> &messageIds,
    const NmId &mailboxId)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmMailAgent::handleMessageEvent(): event=%1, id=%2").
        arg(event).arg(mailboxId.id()));

    bool updateNeeded = false;
    bool activate = false;

    switch (event) {
        case NmMessageCreated: {
            handleMessageCreatedEvent(folderId, messageIds, mailboxId, updateNeeded, activate);
            break;
        }
        case NmMessageChanged: {
            handleMessageChangedEvent(folderId, messageIds, mailboxId, updateNeeded, activate);
            break;
        }
        case NmMessageDeleted: {
            handleMessageDeletedEvent(folderId, messageIds, mailboxId, updateNeeded, activate);
            break;
        }
        default:
            break;
    }

    if (updateNeeded) {
        updateUnreadIndicator();

        NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
        updateMailboxState(mailboxId,
            activate, true /* force refresh */);
    }
}

/*!
    Received from NmFrameworkAdapter syncStateEvent signal.
    \sa NmFrameworkAdapter

    \param state state of synchronization
    \param event Information related to asynchronous operation

 */
void NmMailAgent::handleSyncStateEvent(NmSyncState state, const NmOperationCompletionEvent &event)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmMailAgent::handleSyncStateEvent(): state=%1, id=%2").
        arg(state).arg(event.mMailboxId.id()));

    NmMailboxInfo *info = getMailboxInfo(event.mMailboxId);
    if (info) {
        info->mSyncState = state;

        if (state==Synchronizing) {
            // Reset counters when sync is started
            info->mInboxCreatedMessages = 0;
            info->mInboxChangedMessages = 0;
            info->mInboxDeletedMessages = 0;
        }
        else if (state==SyncComplete) {
            // Check the unread status here again
            bool updateMailbox = updateUnreadCount(event.mMailboxId, *info);
            int oldOutboxCount = info->mOutboxMails;
            info->mOutboxMails = getOutboxCount(event.mMailboxId, info->mOutboxFolderId);
            int unreadMailCount(info->mUnreadMailIdList.count());
            if (info->mOutboxMails > oldOutboxCount || unreadMailCount == 0) {
                // new mails in outbox
                updateMailbox = true;
            }
            bool active = unreadMailCount || info->mOutboxMails;

            // Refresh the indicator if messages created or changed
            NM_COMMENT(QString("NmMailAgent::handleSyncStateEvent(): "
                "created=%1, changed=%2, deleted=%3").
                arg(info->mInboxCreatedMessages).
                arg(info->mInboxChangedMessages).
                arg(info->mInboxDeletedMessages));
            bool refresh = (info->mInboxCreatedMessages > 0) || (info->mInboxChangedMessages > 0);

            updateUnreadIndicator();

            if (updateMailbox) {
                updateMailboxState(event.mMailboxId, active, refresh);
            }
        }
    }
}

/*!
    Received from NmFrameworkAdapter connectionState signal.
    \sa NmFrameworkAdapter

    \param state Connection state
    \param mailboxId Id of the mailbox
    \parma errorcode Error code
 */
void NmMailAgent::handleConnectionEvent(NmConnectState state, const NmId mailboxId, int errorcode)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmMailAgent::handleConnectionEvent(): state=%1, id=%2").
        arg(state).arg(mailboxId.id()));
    Q_UNUSED(errorcode);

    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
    if (mailboxInfo) {
        // Connecting, Connected, Disconnecting, Disconnected
        mailboxInfo->mConnectState = state;
    }
}

/*!
    Remove a mailbox info entry.

    \param id Id of the mailbox
    \return true if mailbox info was found.
 */
bool NmMailAgent::removeMailboxInfo(const NmId &id)
{
    NM_FUNCTION;

    bool found = false;
    foreach (NmMailboxInfo *mailbox, mMailboxes) {
        if (mailbox->mId == id) {
            // Hide the indicator too
            if(mailbox->mIndicatorIndex>=0) {
                updateIndicator(false,*mailbox);
            }

            found = true;
            mMailboxes.removeAll(mailbox);
        }
    }
    return found;
}

/*!
    Create a new mailbox info entry.

    \param id Id of the mailbox
    \return new mailbox info object
 */
NmMailboxInfo *NmMailAgent::createMailboxInfo(const NmId &id)
{
    NM_FUNCTION;

    // get information of the mailbox
    NmMailbox *mailbox = NULL;
    NmMailboxInfo *info = NULL;
    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(id);
    if (plugin) {
        plugin->getMailboxById(id, mailbox);
        if (mailbox) {
            info = createMailboxInfo(*mailbox,plugin);
        }
    }

    return info;
}

/*!
    Create a new mailbox info with given parameters

    \param mailbox Mailbox which mailbox info will be created
    \param plugin Interface to access email data
    \return new mailbox info object
 */
NmMailboxInfo *NmMailAgent::createMailboxInfo(const NmMailbox &mailbox, NmDataPluginInterface *plugin)
{
    NM_FUNCTION;

    NmMailboxInfo *mailboxInfo = new NmMailboxInfo();
    mailboxInfo->mId = mailbox.id();
    mailboxInfo->mName = mailbox.name();

    mMailboxes.append(mailboxInfo);

    // Subscribe to get all mailbox events
    plugin->subscribeMailboxEvents(mailboxInfo->mId);

    // get inbox folder ID. It might be still unknown (=0).
    mailboxInfo->mInboxFolderId = plugin->getStandardFolderId(
        mailbox.id(), NmFolderInbox );

    // get outbox folder ID
    mailboxInfo->mOutboxFolderId = plugin->getStandardFolderId(
        mailbox.id(), NmFolderOutbox );

    // Get branded mailbox icon
    NmMailbox mailbox2( mailbox );
    QString domainName = mailbox2.address().address();
    EmailMailboxInfo emailMailboxInfo;
    mailboxInfo->mIconName =
        emailMailboxInfo.mailboxIcon(domainName);

    return mailboxInfo;
}

/*!
    Return mailbox info class with mailbox id. If no class is found,
    create a new instance with given id.

    \param id Id of the mailbox
    \return mailbox info object
 */
NmMailboxInfo *NmMailAgent::getMailboxInfo(const NmId &id)
{
    NM_FUNCTION;

    foreach (NmMailboxInfo *mailbox, mMailboxes) {
        if (mailbox->mId == id) {
            return mailbox;
        }
    }

    // Not found. Create a new mailbox info.
    return createMailboxInfo(id);
}

/*!
    Finds out if the message is unread.

    \param folderId the id of the folder that includes the message
    \param messageIds the message ids that are checked
    \param mailboxId the id of the mailbox that includes the message
    \param unreadMessage true if there was unread messages
    \return true if info fetching was successful
 */
bool NmMailAgent::getMessageUnreadInfo(const NmId &folderId,
    const NmId &messageId, const NmId &mailboxId, bool &unreadMessage)
{
    NM_FUNCTION;

    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);
    bool ok(false);

    if (plugin) {
        NmMessage *message=NULL;
        plugin->getMessageById(mailboxId, folderId, messageId, message);
        if (message) {
            ok = true;
            NmMessageEnvelope envelope = message->envelope();
            if (!envelope.isRead()) {
                unreadMessage = true;
            }
            delete message;
        }
    }
    return ok;
}

/*!
    Plays email alert tone when new messages arrive.

	\returns true if the tone was played
 */
bool NmMailAgent::playAlertTone()
{
    NM_FUNCTION;
    bool played(false);

    if (mAlertToneAllowed) {
        // Play tone only if system tone service is available and
        // phone is not in silence mode.
        if (mSystemTone && !mSilenceMode) {
            mSystemTone->playTone(XQSystemToneService::EmailAlertTone);
        }

        // Execute the vibra effect.
        if (mVibra) {
            TRAP_IGNORE(mVibra->StartVibraL(NmAgentDefaultVibraDuration));
        }

        // play alert only once per minute
        mAlertToneAllowed = false;
        QTimer::singleShot(NmAgentAlertToneTimer, this, SLOT(enableAlertTone()));
        played = true;
    }

    return played;
}

/*!
    Allows alert tune to be played again.
 */
void NmMailAgent::enableAlertTone()
{
    NM_FUNCTION;

    mAlertToneAllowed = true;
}

/*!
    Update send indicator according to outbox state.
 */
void NmMailAgent::updateSendIndicator()
{
    NM_FUNCTION;

    // Get number of mails in outboxes
    int outboxMails(0);
    foreach (NmMailboxInfo *mailboxInfo, mMailboxes) {
        outboxMails += mailboxInfo->mOutboxMails;
    }

    if( outboxMails == 0 ) {
        mIndicator->deactivate(NmSendIndicatorName);
    }
    else if (outboxMails > mLastOutboxCount) {
        // New mails detected in outbox

        // indicator will disappear automatically after a delay
        mIndicator->activate(NmSendIndicatorName);
    }

    mLastOutboxCount = outboxMails;
}

/*!
    Store the mailbox active information to permanent storage.

    \param mailboxId Id of the mailbox
    \param active true if the mailbox is active
 */
void NmMailAgent::storeMailboxActive(const NmId &mailboxId, bool active)
{
    NM_FUNCTION;

    if (mSettingManager) {
        XQCentralRepositorySettingsKey key(NmRepositoryId, mailboxId.id());
        XQCentralRepositoryUtils utils(*mSettingManager);

        if (active) {
            // when mailbox is active, key can be deleted
            utils.deleteKey(key);
        }
        else {
            utils.createKey(key,(int)active);
        }
    }
}

/*!
    Get the mailbox activity state.

    \param mailboxId id of the mailbox
    \return true if the mailbox is active or no information was stored earlier
 */
bool NmMailAgent::isMailboxActive(const NmId &mailboxId)
{
    NM_FUNCTION;

    bool mailboxActive(true);
    if (mSettingManager) {
        XQCentralRepositorySettingsKey key(NmRepositoryId, mailboxId.id());
        QVariant value = mSettingManager->readItemValue(key, XQSettingsManager::TypeInt);
        if (value.isValid()) {
            NM_COMMENT(QString("NmMailAgent::isMailboxActive - value=%1").arg(value.toInt()));
            mailboxActive = value.toInt();
        }
    }
    return mailboxActive;
}

/*!
    Delete all stored activity information for the mailbox id.

    \param mailboxId id of the mailbox
 */
void NmMailAgent::deleteStoredMailboxActivity(const NmId &mailboxId)
{
    NM_FUNCTION;

    // deactivation delete the key too
    storeMailboxActive(mailboxId, false);
}

// End of file
