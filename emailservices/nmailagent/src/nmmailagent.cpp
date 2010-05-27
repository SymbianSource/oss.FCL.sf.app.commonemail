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
const int NmAgentIndicatorNotSet = -1;
const int NmAgentAlertToneTimer = 60000; // 60s


/*!
    \class NmMailAgent

    \brief Main class for receiving email events and passing them to the HbIndicator
*/

NmMailboxInfo::NmMailboxInfo()
{
    mId = 0;
    mIndicatorIndex = NmAgentIndicatorNotSet;
    mSyncState = SyncComplete;
    mConnectState = Disconnected;
    mOutboxMails = 0;
    mInboxFolderId = 0;
    mOutboxFolderId = 0;
    mInboxCreatedMessages = 0;
    mInboxChangedMessages = 0;
    mInboxDeletedMessages = 0;
    mActive = false;
}

NmMailAgent::NmMailAgent() :
 mPluginFactory(NULL),
 mSendingState(false),
 mAlertToneAllowed(true)
{
    NMLOG("NmMailAgent::NmMailAgent");
}

/*!
    Delayed start
*/
void NmMailAgent::delayedStart()
{
	NMLOG("NmMailAgent::delayedStart");
	if (!init()) {
		// Initialisation failed. Quit the agent.
		QCoreApplication::exit(1);
	}
}

/*!
    Initialise the agent. \return true if succesfully started.
*/
bool NmMailAgent::init()
{
	NMLOG("NmMailAgent::init");
    mPluginFactory = NmDataPluginFactory::instance();
    if (!mPluginFactory) {
        NMLOG("NmMailAgent::init PluginFactory not created");
        return false;
    }

    QList<QObject*> *plugins = mPluginFactory->pluginInstances();

    foreach (QObject *plugin, *plugins) {
    	if (plugin) {
			// Start listening events
			connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, const QList<NmId>&)),
				this, SLOT(handleMailboxEvent(NmMailboxEvent, const QList<NmId> &)),
				Qt::UniqueConnection);

			connect(plugin, SIGNAL(messageEvent(
					NmMessageEvent, const NmId &, const QList<NmId> &, const NmId&)),
				this, SLOT(handleMessageEvent(
					NmMessageEvent, const NmId &, const QList<NmId> &, const NmId&)),
				Qt::UniqueConnection);

			connect(plugin, SIGNAL(syncStateEvent(NmSyncState, const NmOperationCompletionEvent&)),
				this, SLOT(handleSyncStateEvent(NmSyncState, const NmOperationCompletionEvent&)),
				Qt::UniqueConnection);

			connect(plugin, SIGNAL(connectionEvent(NmConnectState, const NmId, int)),
				this, SLOT(handleConnectionEvent(NmConnectState, const NmId, int)),
				Qt::UniqueConnection);
    	}
    }

    // load all current mailboxes
    initMailboxStatus();

    return true;
}

NmMailAgent::~NmMailAgent()
{
    qDeleteAll(mMailboxes);
    NmDataPluginFactory::releaseInstance(mPluginFactory);
}

/*!
    Initialize the mailbox list with the current state
*/
void NmMailAgent::initMailboxStatus()
{
    NMLOG("NmMailAgent::initMailboxStatus");
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
                NmMailboxInfo *mailboxInfo = createMailboxInfo(*mailbox,plugin);
                if (mailboxInfo) {
                    bool activate = updateUnreadCount(mailbox->id(), *mailboxInfo);
                    mailboxInfo->mOutboxMails = getOutboxCount(mailbox->id());
                    if (mailboxInfo->mOutboxMails > 0) {
                        activate = true;
                    }

                    // Create indicator for visible mailboxes
                    updateMailboxState(mailbox->id(),
                        activate, false);
                }
            }
        }
        qDeleteAll(mailboxes);
    }
}

/*!
    Get mailbox unread count in inbox folder
    \param mailboxId id of the mailbox
    \param mailboxInfo contains the list of unread messages
    \return true if new unread mails was found
*/
bool NmMailAgent::updateUnreadCount(const NmId &mailboxId, NmMailboxInfo &mailboxInfo)
{
    NMLOG("NmMailAgent::getUnreadCount");
    int newUnreadMessages(0);

    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);

    if (plugin) {
		// get inbox folder ID
		NmId inboxId = plugin->getStandardFolderId(
				mailboxId, NmFolderInbox );

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
	NMLOG(QString("NmMailAgent::getUnreadCount count=%1 new=%2").
	    arg(mailboxInfo.mUnreadMailIdList.count()).arg(newUnreadMessages));

    return (newUnreadMessages > 0);
}

/*!
    Get mailbox count in outbox folder
    \param mailboxId id of the mailbox
    \param maxCount max number of outbox mails that is needed
    \return number of mails in the outbox
*/
int NmMailAgent::getOutboxCount(const NmId &mailboxId)
{
    NMLOG("NmMailAgent::getOutboxCount");
    int count(0);

    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);

    if (plugin) {
		// get outbox folder ID
		NmId outboxId = plugin->getStandardFolderId( mailboxId, NmFolderOutbox );

		// get list of messages in outbox
		QList<NmMessageEnvelope*> messageList;
		plugin->listMessages(mailboxId, outboxId, messageList);
		count = messageList.count();
		qDeleteAll(messageList);
    }
    NMLOG(QString("NmMailAgent::getOutboxCount count=%1").arg(count));

    return count;
}

/*!
    Update the mailbox visibility and status
    \param mailboxId id of the mailbox
    \param active visibility state of the mailbox
    \param refreshAlways true when the indicator should be always updated
    \return true if the mailbox state was changed
*/
bool NmMailAgent::updateMailboxState(const NmId &mailboxId,
    bool active, bool refreshAlways)
{
    // Update the global sending state
    mSendingState = false;
    foreach (NmMailboxInfo *mailboxInfo, mMailboxes) {
        if (mailboxInfo->mOutboxMails>0) {
            mSendingState = true;
            break;
        }
    }

    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
    bool changed = false;
    if (mailboxInfo->mActive != active ||
        refreshAlways) {
        mailboxInfo->mActive = active;
        changed = true;
        if (active) {
            // Mailbox is not yet assigned to any indicator
            if (mailboxInfo->mIndicatorIndex < 0) {
                mailboxInfo->mIndicatorIndex = getIndicatorIndex();
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
    Updates indicator status
    \param mailboxIndex index of the item shown in indicator menu
    \param active indicator visibility state
    \param mailboxInfo information of the mailbox
    \return true if indicator was updated with no errors
*/
bool NmMailAgent::updateIndicator(bool active,
    const NmMailboxInfo& mailboxInfo)
{
    NMLOG(QString("NmMailAgent::updateIndicator index=%1 active=%2").
        arg(mailboxInfo.mIndicatorIndex).arg(active));

    bool ok = false;
    QString name = QString("com.nokia.nmail.indicatorplugin_%1/1.0").
        arg(mailboxInfo.mIndicatorIndex);

    QList<QVariant> list;
    list.append(mailboxInfo.mId.id());
    list.append(mailboxInfo.mName);
    list.append(mailboxInfo.mUnreadMailIdList.count());
    list.append(mailboxInfo.mSyncState);
    list.append(mailboxInfo.mConnectState);
    list.append(mailboxInfo.mOutboxMails);
    list.append(mailboxInfo.mIconName);
    list.append(mSendingState);

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
    Get next free indicator index, starting from 0
    @return index of the indicator that is available
 */
int NmMailAgent::getIndicatorIndex()
{
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
*/
void NmMailAgent::handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds)
{
    NMLOG(QString("NmMailAgent::handleMailboxEvent %1").arg(event));

    switch(event) {
        case NmMailboxCreated:
            foreach (NmId mailboxId, mailboxIds) {
                getMailboxInfo(mailboxId); // create a new mailbox if needed
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
            }
            break;
        default:
            break;
    }
}

/*!
    Received from NmFrameworkAdapter messageEvent signal
    \sa NmFrameworkAdapter
*/
void NmMailAgent::handleMessageEvent(
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId)
{
    NMLOG(QString("NmMailAgent::handleMessageEvent %1 %2").arg(event).arg(mailboxId.id()));
    bool updateNeeded = false;
    bool activate = false;

    switch (event) {
        case NmMessageCreated: {
            NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);

            // Check the new messages to make the indicator appear earlier
            if (mailboxInfo->mSyncState == Synchronizing &&
                mailboxInfo->mUnreadMailIdList.count()==0) {

                // Inbox folder ID may be still unknown
                if (mailboxInfo->mInboxFolderId.id()==0) {
                    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);
                    if (plugin) {
                        mailboxInfo->mInboxFolderId =
                            plugin->getStandardFolderId(mailboxId, NmFolderInbox);
                    }
                }

                if (folderId == mailboxInfo->mInboxFolderId) {
                    bool messageUnread = false;
                    foreach (NmId messageId, messageIds) {
                        if (getMessageUnreadInfo(folderId, messageId, mailboxId, messageUnread)) {
                            if (messageUnread) {
                                mailboxInfo->mUnreadMailIdList.append(messageId);
                                updateMailboxState(mailboxId, true, false);
                            }
                        }
                    }
                }
            }
            if (folderId==mailboxInfo->mInboxFolderId) {
                mailboxInfo->mInboxCreatedMessages++;
            }

            // When created a new mail in the outbox, we are in sending state
            if (mailboxInfo->mOutboxFolderId == folderId) {
                // The first mail created in the outbox
                if (mailboxInfo->mOutboxMails <= 0) {
					NMLOG("NmMailAgent: first mail in outbox");
					activate = true;
                    updateNeeded = true;
                }
                mailboxInfo->mOutboxMails += messageIds.count();
            }
            break;
        }
        case NmMessageChanged: {
            NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);

            if (folderId==mailboxInfo->mInboxFolderId) {
                mailboxInfo->mInboxChangedMessages++;
            }

            // If not currently syncronizing the mailbox, this may mean
            // that a message was read/unread
            if (mailboxInfo && mailboxInfo->mSyncState==SyncComplete) {
                // check the unread status again
                int oldCount = mailboxInfo->mUnreadMailIdList.count();
                activate = updateUnreadCount(mailboxId, *mailboxInfo);

                // new unread mails found or no more unread mails in the mailbox
                if (activate || (oldCount>0 && mailboxInfo->mUnreadMailIdList.count()==0)) {
                    updateNeeded = true;
                }
            }
			break;
		}
        case NmMessageDeleted: {
            NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);

            if (folderId==mailboxInfo->mInboxFolderId) {
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
					NMLOG("NmMailAgent: last mail deleted from outbox");

                    // Keep it active if there is unread mails
                    activate = mailboxInfo->mUnreadMailIdList.count() > 0;
                    updateNeeded = true;
                }
            }
            break;
        }
        default:
            break;
    }

    if (updateNeeded) {
        NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
        updateMailboxState(mailboxId,
            activate, true /* force refresh */);
    }
}

/*!
    Received from NmFrameworkAdapter syncStateEvent signal
    \sa NmFrameworkAdapter
*/
void NmMailAgent::handleSyncStateEvent(
            NmSyncState state,
            const NmOperationCompletionEvent &event)
{
    NMLOG(QString("NmMailAgent::handleSyncStateEvent %1 %2").arg(state).arg(event.mMailboxId.id()));
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
            bool activate = updateUnreadCount(event.mMailboxId, *info);
            int oldOutboxCount = info->mOutboxMails;
            info->mOutboxMails = getOutboxCount(event.mMailboxId);
            if (info->mOutboxMails > oldOutboxCount) {
                // new mails in outbox
                activate = true;
            }
            bool active = info->mUnreadMailIdList.count() ||
                info->mOutboxMails;

            // Refresh the indicator if messages created or changed
            NMLOG(QString(" created=%1, changed=%2, deleted=%3").
                arg(info->mInboxCreatedMessages).
                arg(info->mInboxChangedMessages).
                arg(info->mInboxDeletedMessages));
            bool refresh = (info->mInboxCreatedMessages > 0) || (info->mInboxChangedMessages > 0);

            if (activate) {
                updateMailboxState(event.mMailboxId, active, refresh);
            }
        }
    }
}

/*!
    Received from NmFrameworkAdapter connectionState signal
    \sa NmFrameworkAdapter
*/
void NmMailAgent::handleConnectionEvent(NmConnectState state, const NmId mailboxId, int errorcode)
{
	Q_UNUSED(errorcode);
    NMLOG(QString("NmMailAgent::handleConnectionEvent %1 %2 %3").arg(state).arg(mailboxId.id()).arg(errorcode));
    NmMailboxInfo *mailboxInfo = getMailboxInfo(mailboxId);
    if (mailboxInfo) {
        // Connecting, Connected, Disconnecting, Disconnected
        mailboxInfo->mConnectState = state;
    }
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
    Create a new mailbox info entry
    \return new mailbox info object
*/
NmMailboxInfo *NmMailAgent::createMailboxInfo(const NmId &id)
{
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
    \return new mailbox info object
*/
NmMailboxInfo *NmMailAgent::createMailboxInfo(const NmMailbox &mailbox, NmDataPluginInterface *plugin)
{
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
    NMLOG("NmMailAgent::messageInfo");

    NmDataPluginInterface *plugin = mPluginFactory->interfaceInstance(mailboxId);
    bool ok = false;

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
    Plays email alert tune when new messages arrive
*/
void NmMailAgent::playAlertTone()
{
    if (mAlertToneAllowed) {
        // play alert
        mAlertToneAllowed = false;
        QTimer::singleShot(NmAgentAlertToneTimer, this, SLOT(enableAlertTone()));
    }
}

/*!
    Allows alert tune to be played again
*/
void NmMailAgent::enableAlertTone()
{
    mAlertToneAllowed = true;
}

// End of file

