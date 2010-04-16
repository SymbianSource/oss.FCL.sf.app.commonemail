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

#include "nmmailagentheaders.h"
#include "nmmailagent.h"

// CONSTS
const int maxUnreadCount = 1; // 1 is enough


/*!
    \class NmMailAgent

    \brief Main class for receiving email events and passing them to the HbIndicator
*/

/*!
    Creates list of folder paths where plug-ins can be loaded from.
    \return folder path list.
*/
QStringList NmMailAgent::pluginFolders()
{
    const QString nmPluginPath("resource/plugins");
    QStringList pluginDirectories;
    QFileInfoList driveList = QDir::drives();

    foreach(const QFileInfo &driveInfo, driveList) {
        QString pluginDirectory =
            driveInfo.absolutePath() + nmPluginPath;

        if (QFileInfo(pluginDirectory).exists()) {
            pluginDirectories.append(pluginDirectory);
        }
    }

    return pluginDirectories;
}

NmMailboxInfo::NmMailboxInfo()
{
    mId = 0;
    mSyncState = SyncComplete;
    mConnectState = Disconnected;
    mUnreadMails = 0;
    mActive = false;
}

NmMailAgent::NmMailAgent() :
 mAdapter(NULL),
 mActiveIndicators(0)
{
    NMLOG("NmMailAgent::NmMailAgent");
}

/*!
    Initialise the agent. \return true if succesfully started.
*/
bool NmMailAgent::init()
{
    if (!loadAdapter()) {
        // Failed to load NmFrameworkAdapter
        return false;
    }

    // Start listening events
    connect(mAdapter, SIGNAL(mailboxEvent(NmMailboxEvent, const QList<NmId>&)),
        this, SLOT(handleMailboxEvent(NmMailboxEvent, const QList<NmId> &)));

    connect(mAdapter, SIGNAL(messageEvent(
            NmMessageEvent, const NmId &, const QList<NmId> &, const NmId&)),
        this, SLOT(handleMessageEvent(
            NmMessageEvent, const NmId &, const QList<NmId> &, const NmId&)));

    connect(mAdapter, SIGNAL(syncStateEvent(NmSyncState, const NmId)),
        this, SLOT(handleSyncStateEvent(NmSyncState, const NmId)));

    connect(mAdapter, SIGNAL(connectionEvent(NmConnectState, const NmId)),
        this, SLOT(handleConnectionEvent(NmConnectState, const NmId)));

    // load all current mailboxes
    initMailboxStatus();

    updateStatus();
    return true;
}

NmMailAgent::~NmMailAgent()
{
    delete mAdapter;
    qDeleteAll(mMailboxes);
}

/*!
    Initialize the mailbox list with the current state
*/
void NmMailAgent::initMailboxStatus()
{
    NMLOG("NmMailAgent::initMailboxStatus");
    QList<NmMailbox*> mailboxes;
    mAdapter->listMailboxes(mailboxes);
    foreach (const NmMailbox* mailbox, mailboxes) {
        if (mailbox) {
            NmMailboxInfo *mailboxInfo = createMailboxInfo(*mailbox);
            if (mailboxInfo) {
                mailboxInfo->mUnreadMails = getUnreadCount(mailbox->id(),maxUnreadCount);
                updateMailboxActivity(mailbox->id(), isMailboxActive(*mailboxInfo));
            }
        }
    }
    qDeleteAll(mailboxes);
}

/*!
    Get mailbox unread count in inbox folder
    \param mailboxId id of the mailbox
    \param maxCount max number of unread mails that is needed
    \return number of unread mails in the mailbox
*/
int NmMailAgent::getUnreadCount(const NmId &mailboxId, int maxCount)
{
    NMLOG("NmMailAgent::getUnreadCount");
    int count(0);

    // get inbox folder ID
    NmId inboxId = mAdapter->getStandardFolderId(
            mailboxId, NmFolderInbox );

    // get list of messages in inbox
    QList<NmMessageEnvelope*> messageList;
    mAdapter->listMessages(mailboxId, inboxId, messageList);

    foreach (const NmMessageEnvelope* envelope, messageList) {
        // if the message is not read, it is "unread"
        if (!envelope->isRead()) {
            count++;

            // No more unread mails are needed
            if (count >= maxCount) {
                break;
            }
        }
    }
	qDeleteAll(messageList);
	NMLOG(QString("NmMailAgent::getUnreadCount count=%1").arg(count));

    return count;
}

/*!
    Load NmFrameworkAdapter from plugins.
    \return true if adapter is loaded succesfully.
*/
bool NmMailAgent::loadAdapter()
{
    QStringList directories(pluginFolders());

     foreach (const QString &pluginPath, directories) {
         QPluginLoader *loader =
             new QPluginLoader(pluginPath + "/nmframeworkadapter.qtplugin");
         if (loader) {
             mAdapter = static_cast<NmFrameworkAdapter *>(loader->instance());
             if (mAdapter) {
                 return true;
             }
         }
     }
     NMLOG("NmMailAgent::loadAdapter failed");
     return false;
}

/*!
    Update the mailbox visibility
    \param mailboxId id of the mailbox
    \param active visibility state of the mailbox
    \return true if the mailbox state was changed
*/
bool NmMailAgent::updateMailboxActivity(const NmId &mailboxId, bool active)
{
    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
    bool changed = false;
    if (mailboxInfo->mActive != active) {
        mailboxInfo->mActive = active;
        changed = true;
        if (active) {
            // Mailbox becomes active again. Move to the bottom of the list.
            mMailboxes.removeAll(mailboxInfo);
            mMailboxes.append(mailboxInfo);
        }
    }
    return changed;
}

/*!
    Updates status according to current information
*/
void NmMailAgent::updateStatus()
{
    NMLOG("NmMailAgent::updateStatus");

    int activeIndicators = 0;

    // Update the indicators
    foreach (NmMailboxInfo *mailboxInfo, mMailboxes) {
        // Show only active mailboxes
        if (mailboxInfo->mActive) {
            updateIndicator(activeIndicators,true,*mailboxInfo);
            activeIndicators++;
        }
    }

    // Hide the indicator that are not needed anymore
    for (int i=activeIndicators;i<mActiveIndicators;i++) {
        NmMailboxInfo mailboxInfo;
        updateIndicator(i,false,mailboxInfo);
    }
    mActiveIndicators = activeIndicators;
}

/*!
    Check if the mailbox indicator should be active, according to current state
    \param mailboxInfo information of the mailbox
    \return true if indicator should be now active
*/
bool NmMailAgent::isMailboxActive(const NmMailboxInfo& mailboxInfo)
{
    if (mailboxInfo.mUnreadMails>0) {
        return true;
    }
    return false;
}

/*!
    Updates indicator status
    \param mailboxIndex index of the item shown in indicator menu
    \param active indicator visibility state
    \param mailboxInfo information of the mailbox
    \return true if indicator was updated with no errors
*/
bool NmMailAgent::updateIndicator(int mailboxIndex, bool active,
    const NmMailboxInfo& mailboxInfo)
{
    NMLOG(QString("NmMailAgent::updateIndicator index=%1 active=%2 unread=%3").
        arg(mailboxIndex).arg(active).arg(mailboxInfo.mUnreadMails));

    bool ok = false;
    QString name = QString("com.nokia.nmail.indicatorplugin_")+mailboxIndex+"/1.0";

    QList<QVariant> list;
    list.append(mailboxInfo.mId.id());
    list.append(mailboxInfo.mName);
    list.append(mailboxInfo.mUnreadMails);
    list.append(mailboxInfo.mSyncState);
    list.append(mailboxInfo.mConnectState);

    HbIndicator indicator;
    if (active) {
        ok = indicator.activate(name,list);
    }
    else {
        ok = indicator.deactivate(name,list);
    }
    return ok;
}

/*!
    Received from NmFrameworkAdapter mailboxEvent signal
    \sa NmFrameworkAdapter
*/
void NmMailAgent::handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds)
{
    NMLOG(QString("NmMailAgent::handleMailboxEvent %1").arg(event));
    bool updateNeeded(false);

    switch(event) {
        case NmMailboxCreated:
            foreach (NmId mailboxId, mailboxIds) {
                getMailboxInfo(mailboxId); // create a new mailbox if needed
                updateNeeded = true;
            }
            break;
        case NmMailboxChanged:
            // Mailbox name may have been changed
            foreach (NmId mailboxId, mailboxIds) {
                NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
                NmMailbox *mailbox(NULL);
                mAdapter->getMailboxById(mailboxId,mailbox);
                if (mailbox && mailboxInfo) {
                    if(mailbox->name() != mailboxInfo->mName) {
                        mailboxInfo->mName = mailbox->name();
                        updateNeeded = true;
                    }
                }
                delete mailbox;
            }
            break;
        case NmMailboxDeleted:
            foreach (NmId mailboxId, mailboxIds) {
                if (removeMailboxInfo(mailboxId)) {
                    updateNeeded = true;
                }
            }
            break;
        default:
            break;
    }

    if (updateNeeded) {
        updateStatus();
    }
}

/*!
    Received from NmFrameworkAdapter messageEvent signal
    \sa NmFrameWorkAdapter
*/
void NmMailAgent::handleMessageEvent(
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId)
{
    NMLOG(QString("NmMailAgent::handleMessageEvent %1 %2").arg(event).arg(mailboxId.id()));
    Q_UNUSED(folderId);
    Q_UNUSED(messageIds);

    switch (event) {
        case NmMessageChanged: {
            NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
            // If not currently syncronizing the mailbox, this may mean
            // that a message was read/unread
            if (mailboxInfo && mailboxInfo->mSyncState==SyncComplete) {
                // check the unread status here again
                mailboxInfo->mUnreadMails = getUnreadCount(mailboxId,maxUnreadCount);
                if(updateMailboxActivity(mailboxId, isMailboxActive(*mailboxInfo))) {
                    updateStatus();
                }
            }
			break;
		}
        default:
            break;
    }

    // Do not perform an update here, just in handleSyncState
}

/*!
    Received from NmFrameworkAdapter syncStateEvent signal
    \sa NmFrameWorkAdapter
*/
void NmMailAgent::handleSyncStateEvent(
            NmSyncState state,
            const NmId mailboxId)
{
    NMLOG(QString("NmMailAgent::handleSyncStateEvent %1 %2").arg(state).arg(mailboxId.id()));
    NmMailboxInfo *info = getMailboxInfo(mailboxId);
    if (info) {
        info->mSyncState = state;

        if (state==SyncComplete) {
            // check the unread status here again
            info->mUnreadMails = getUnreadCount(mailboxId,maxUnreadCount);
            if(updateMailboxActivity(mailboxId, isMailboxActive(*info))) {
                updateStatus();
            }
        }
    }
}

/*!
    Received from NmFrameworkAdapter connectionState signal
    \sa NmFrameWorkAdapter
*/
void NmMailAgent::handleConnectionEvent(NmConnectState state, const NmId mailboxId)
{
    NMLOG(QString("NmMailAgent::handleConnectionEvent %1 %2").arg(state).arg(mailboxId.id()));
    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
    if (mailboxInfo) {
        // Connecting, Connected, Disconnecting, Disconnected
        mailboxInfo->mConnectState = state;
    }
    updateStatus();
}

/*!
    Remove a mailbox info entry
    \return true if mailbox info was found
*/
bool NmMailAgent::removeMailboxInfo(const NmId &id)
{
    bool found = false;
    foreach (NmMailboxInfo *mailbox, mMailboxes) {
        if (mailbox->mId == id) {
            found = true;
            mMailboxes.removeAll(mailbox);
        }
    }
    return found;
}

/*!
    Create a new mailbox info entry
    \return new mailbox info object
*/
NmMailboxInfo *NmMailAgent::createMailboxInfo(const NmId &id)
{
    // get information of the mailbox
    NmMailbox *mailbox = NULL;
    mAdapter->getMailboxById(id, mailbox);
    if (mailbox) {
        return createMailboxInfo(*mailbox);
    }
    return NULL;
}

/*!
    Create a new mailbox info with given parameters
    \return new mailbox info object
*/
NmMailboxInfo *NmMailAgent::createMailboxInfo(const NmMailbox& mailbox)
{
    NmMailboxInfo *mailboxInfo = new NmMailboxInfo();
    mailboxInfo->mId = mailbox.id();
    mailboxInfo->mName = mailbox.name();

    mMailboxes.append(mailboxInfo);

    // Subscribe to get all mailbox events
    mAdapter->subscribeMailboxEvents(mailboxInfo->mId);
    return mailboxInfo;
}

/*!
    Return mailbox info class with mailbox id. If no class is found, create a new instance with given id.
    \return mailbox info object
*/
NmMailboxInfo *NmMailAgent::getMailboxInfo(const NmId &id)
{
    foreach (NmMailboxInfo *mailbox, mMailboxes) {
        if (mailbox->mId == id) {
            return mailbox;
        }
    }

    // Not found. Create a new mailbox info.
    return createMailboxInfo(id);
}

// End of file

