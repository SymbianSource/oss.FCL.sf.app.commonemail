/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef NMAPIEVENTNOTIFIER_H
#define NMAPIEVENTNOTIFIER_H

#include <QtCore>
#include "nmapimessagetask.h"
#include "nmenginedef.h"
#include "nmcommon_api.h"

struct NmApiMessage;

namespace EmailClientApi
{
class NmEventNotifierPrivate;

/*
 * Notifier for mailbox and message events
 * 
 * Example of use:
 * NmEventNotifier *notifier = new NmEventNotifier(this);
 * notifier->start();
 * connect(notifier,SIGNAL(messageEvent(MessageEvent,quint64,quint64,QList<quint64>)),this,
 *                  SLOT(processMessage(MessageEvent,quint64,quint64,QList<quint64>)),Qt::QueuedConnection);
 * 
 * And then when want to end need to do:
 * notifier->cancel();
 * delete notifier; // or notifier->deleteLater();
 */
class NMENGINE_EXPORT NmEventNotifier : public NmMessageTask
{
    Q_OBJECT
public:
    /*!
     * Constructor
     */
    NmEventNotifier(QObject *parent);

    /*!
     * Destructor
     */
    virtual ~NmEventNotifier();

    /*!
     * Start monitoring email events
     * 
     * Return value tells about monitoring system running
     */
    virtual bool start();

    /*!
     * Cancels monitoring.
     * 
     * In user responsibility is to cancel monitoring.
     * On end it clear buffer events and emits \sa NmMessageTask::canceled() signal.
     */
    virtual void cancel();

    /*!
     * It check if if event notifier is running.
     */
    bool isRunning() const;

    signals:
    /*!
     * This signal is emited when buffer of maiblox events is ready to send. Buffer is list of mailbox events with id.
     * 
     * 
     * It emits signals grouped by events. So there will be as much signals as much there are events.
     * For safety when connect it need to use \sa Qt::QueuedConnection.
     * After that use \sa EmailClientApi::NmEmailService::getMailbox to get object.
     * \arg Event of mailbox
     * \arg List of mailbox ids where event occur.
     */
    void mailboxEvent(MailboxEvent event,QList<quint64> id);

    /*!
     * This signal is emited when buffer of messages events is ready to send. Buffer is list of messages events with id.
     *
     * It emits signals grouped by events, mailbox and folder. So there will be as much signals as much there are events.
     * For safety when connect it need to use \sa Qt::QueuedConnection.
     * After that use \sa EmailClientApi::NmEmailService::getEnvelope to get object.
     * 
     * \arg Event of messages
     * \arg Message mailbox
     * \arg Message folder
     * \arg List of messages ids where event occur
     */
    void messageEvent(MessageEvent event,quint64 mailboxId,quint64 folderId,QList<quint64> envelopeIdList);

public slots:
    /*!
     * It check each object in buffer and emit signal with it.
     * 
     * After end of work of this function buffer is empty.
     * It is called by timeout signal from timer.
     */
    void sendEventsFromBuffer();

private:
    NmEventNotifierPrivate *mNmEventNotifierPrivate;
};

}

Q_DECLARE_METATYPE(QList<quint64>)

#endif
