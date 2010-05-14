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
#include "nmindicator.h"

#include <QThreadPool>
#include <QRunnable>
#include <QByteArray>
#include <QProcess>
#include <QTimer>
#include <QVariant>
#include <QCoreApplication>

#include <HbStringUtil>

const int NmMailboxInfoItemCount = 8;
const int NmSendingStateDelay = 2000; // delay for 'send in progress' indicator
const int NmMaxOutboxCount = 99;

/*!
    \class NmMailboxInfo
    \brief Stores info that indicator displays
*/

/*!
     Class constructor.
*/
NmMailboxInfo::NmMailboxInfo()
{
    NMLOG("NmMailboxInfo::NmMailboxInfo");
    mId = 0;
    mSyncState = SyncComplete;
    mConnectState = Disconnected;
    mUnreadMails = 0;
    mOutboxMails = 0;
}

/*!
    \class NmIndicator
    \brief Handles client request and showing the indications.
*/

/*!
     Class constructor.
*/
NmIndicator::NmIndicator(const QString &indicatorType)
:HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::NotificationCategory,
        InteractionActivated)
{
    NMLOG("NmIndicator::NmIndicator");
    mShowIndicator = false;
    mSendingState = false;
    mShowSendProgress = false;
    mActive = false;
}

/*!
    Class destructor.
*/
NmIndicator::~NmIndicator()
{
    NMLOG("NmIndicator::~NmIndicator");
}

/*!
    called by the framework, when the user interacts with the indicator.
    \a type is a type of the interaction.

    The method should be reimplemented, when interaction type is set to other than
    HbIndicatorInterface::NoInteraction.

    should return true, if interaction was handled, false otherwise. Default implementation
    returns false.

    \sa interactionTypes
*/
bool NmIndicator::handleInteraction(InteractionType type)
{
    NMLOG("NmIndicator::handleInteraction");
    bool handled = false;
    if (type == InteractionActivated) {
        mActive = false;
        handled = true;

        // This indicator is responsible showing the status bar icon
        if (mShowIndicator) {
            // Do not show any indicators anymore
            mShowIndicator = false;
            emit dataChanged();

            // Notify that the icon will be lost now
            emit indicatorIconLost();
        }
        emit mailboxLaunched(mMailbox.mId.id());
        emit deactivate();
    }
    return handled;
}

/*!
    \fn QVariant HbIndicatorInterface::indicatorData(int role) const = 0

    Returns the data stored under the given role. The indicator should at least have data elements for
    HbIndicatorInterface::TextRole and HbIndicatorInterface::DecorationNameRole. If data element for
    HbIndicatorInterface::IconNameRole doesn't exist, the same icon retrieved by \c DecorationNameRole
    is used both in status indicator area and universal indicator popup.  If the indicator is
    low priority indicator, it is shown only in universal indicator popup, and data element for
    \c IconNameRole is ignored

    \a role the data role for the indicator. A value from the HbIndicatorInterface::DataRole - enum.

    For the roles not supported by the plugin, empty QVariant should be returned.
*/
QVariant NmIndicator::indicatorData(int role) const
{
    NMLOG(QString("NmIndicator::indicatorData %1").arg(role));
    switch(role) {
        case PrimaryTextRole:
            {
            return mMailbox.mName;
            }
        case SecondaryTextRole:
            {
            if (mMailbox.mOutboxMails>0) {
                int outboxCount = mMailbox.mOutboxMails;
                if (outboxCount>NmMaxOutboxCount) {
                    outboxCount = NmMaxOutboxCount;
                }
                QString text = QString(hbTrId("txt_mail_status_menu_waiting_to_send")).
                    arg(HbStringUtil::convertDigits(QString::number(outboxCount)));
                return text;
		    }
            else if (mMailbox.mUnreadMails>0) {
                return hbTrId("txt_mail_status_menu_new_mail");
			}
            break;
            }
        case DecorationNameRole:
            {
			// Icon for the mailbox in the menu
            if (mActive) {
                if (!mMailbox.mMailboxIconName.isEmpty()) {
                    return mMailbox.mMailboxIconName;
                }
                else {
                    return QString("qtg_large_email");
                }
            }
            break;
		    }
        case MonoDecorationNameRole:
			if (mShowIndicator) {
				return QString("qtg_status_new_email");
			}
			break;
        default:
        	break;
    }
    return QVariant();
}

/*!
    Timer callback for hiding 'send in progress' indicator
*/
void NmIndicator::hideSendIndicator()
{
    if (mShowSendProgress) {
        NMLOG("NmIndicator::hideSendIndicator - hide progress state");
        mShowSendProgress = false;
        emit dataChanged();
    }
}

/*!
    Checks if any mailbox is in sending state at the moment
    \return true if any mailbox is in sending state
*/
bool NmIndicator::isSending() const
{
	return mSendingState;
}

/*!
    Return type of the indicator
    \sa HbIndicatorInterface
*/
HbIndicatorInterface::Category NmIndicator::category() const
{
	NMLOG("NmIndicatorPlugin::Category");
    if (mMailbox.mOutboxMails>0 && mShowSendProgress) {
        return HbIndicatorInterface::ProgressCategory;
    }
    return HbIndicatorInterface::NotificationCategory;
}

/*!
    Handles the requests sent from the client
    \a type is a type of the request.
    \a parameter is extra parameter from the client. Can be invalid, if not given.

    Should return true, when the request is handled.
    Default implementation returns false.

    \sa RequestType, HbIndicator
*/
bool NmIndicator::handleClientRequest( RequestType type,
        const QVariant &parameter)
{
    NMLOG("NmIndicator::handleClientRequest");
    bool handled(false);
    switch (type) {
        case RequestActivate:
            {
            mActive = true;
			storeMailboxData(parameter);

            emit dataChanged();
            handled =  true;
            }
            break;
        case RequestDeactivate:
            {
            mActive = false;

            // also the deactivation may give updated data
			storeMailboxData(parameter);

            // This indicator was responsible showing the status bar icon
            if (mShowIndicator) {
                // Notify that the icon will be lost now
                emit indicatorIconLost();
            }

            emit deactivate();
            }
            break;
        default:
            break;
    }
    return handled;
}

/*!
    Start showing the 'send in progress' indicator
 */
void NmIndicator::showSendProgress()
{
    // Activate the progress indicator
    if (!mShowSendProgress && mActive) {
        mShowSendProgress = true;

        // Hide the progress state after some delay
        QTimer::singleShot(NmSendingStateDelay, this, SLOT(hideSendIndicator()));
    }
}

/*!
	Stores mailbox data to member variable mMailbox.
 */
void NmIndicator::storeMailboxData(QVariant mailboxData)
{
    NMLOG("NmIndicator::storeMailboxData");
    QList<QVariant> infoList = mailboxData.toList();

    if (infoList.count() >= NmMailboxInfoItemCount) {
        mMailbox.mId.setId(infoList.at(0).value<quint64>());
        mMailbox.mName = infoList.at(1).toString();
        mMailbox.mUnreadMails = infoList.at(2).toInt();
        mMailbox.mSyncState = infoList.at(3).value<NmSyncState>();
        mMailbox.mConnectState = infoList.at(4).value<NmConnectState>();
        mMailbox.mOutboxMails = infoList.at(5).toInt();
        mMailbox.mMailboxIconName = infoList.at(6).toString();

        bool oldSendingState = mSendingState;
        mSendingState = infoList.at(7).toInt();

        // Sending state now activated
        if (!oldSendingState && mSendingState) {
            showSendProgress();
        }

        // Notify the global state
        emit globalStatusChanged(mSendingState);
    }
}

/*!
    Used for asking if this indicator can take status bar icon responsibility.
    \param sending global sending state
    \return true if the icon was accepted
 */
bool NmIndicator::acceptIcon(bool sending)
{
    mSendingState = sending;

    if (mActive) {
        mShowIndicator = true;
        emit dataChanged();
    }
    return mActive;
}

