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
#include "NmIndicator.h"

#include <QThreadPool>
#include <QRunnable>
#include <QByteArray>
#include <QProcess>
#include <QCoreApplication>

#include <xqservicerequest.h>
#include <email_services_api.h>

const int mailboxInfoItemCount = 5;

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
        HbIndicatorInterface::GroupPriorityHigh,
        InteractionActivated)
{
    NMLOG("NmIndicator::NmIndicator");
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
        showMailbox(mMailbox.mId.id());
        handled = true;
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
    NMLOG("NmIndicator::indicatorData");
    switch(role) {
        case TextRole:
            {
            return mMailbox.mName;
            }
        case SecondaryTextRole:
            {
            // These states will be implemented later
            //txt_mail_status_menu_waiting_connection
            //txt_mail_status_menu_sending
                
            if (mMailbox.mUnreadMails>0)
                return hbTrId("txt_mail_status_menu_new_mail");
            return QString();
            }
        case DecorationNameRole:
            {
			return QString("qtg_large_email");
		    }
        case IconNameRole:
            {
            QString iconName("qtg_large_email");
            return iconName;
            }
        default:
           return QVariant();
    }
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
			storeMailboxData(parameter);
            emit dataChanged();
            handled =  true;
            }
            break;
        case RequestDeactivate:
            {
            emit deactivate();
            }
            break;
        default:
            break;
    }
    return handled;
}

/*!
    Opens the mailbox that includes unread messages.
    \return true if inbox is succesfully opened
*/
bool NmIndicator::showMailbox(quint64 mailboxId)
{
    NMLOG("NmIndicator::showMailbox");
    bool ok = false;
    XQServiceRequest request(
        emailInterfaceNameMailbox,
        emailOperationViewInbox,
        true);

    QList<QVariant> list;
    list.append(QVariant(mailboxId));

    request.setArguments(list);

    int returnValue(-1);
    bool rval = request.send(returnValue);

    if (!rval) {
        NMLOG(QString("NmIndicator: showMailbox error: %1").arg(request.latestError()));
    }
    else {
        NMLOG(QString("NmIndicator: showMailbox succeeded: %1").arg(QString(returnValue)));
        ok = true;
    }
    return ok;
}

/*!
	Stores mailbox data to member variable mMailbox.
 */
void NmIndicator::storeMailboxData(QVariant mailboxData)
{
    NMLOG("NmIndicator::storeMailboxData");
    QList<QVariant> infoList = mailboxData.toList();

    if (infoList.count() == mailboxInfoItemCount) {
        mMailbox.mId.setId(infoList.at(0).value<quint64>());
        mMailbox.mName = infoList.at(1).toString();
        mMailbox.mUnreadMails = infoList.at(2).toInt();
        mMailbox.mSyncState = infoList.at(3).value<NmSyncState>();
        mMailbox.mConnectState = infoList.at(4).value<NmConnectState>();
    }
}
