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

#include <nmapimessagetask.h>
#include <nmapidef.h>
#include <nmapicommon.h>

namespace EmailClientApi
{
class NmApiEventNotifierPrivate;

/*!
   Notifier for mailbox and message events
   
   Example of use:
   \code
   NmApiEventNotifier *notifier = new NmApiEventNotifier(this);
   notifier->start();
   connect(notifier,SIGNAL(messageEvent(EmailClientApi::MessageEvent,quint64,quint64,QList<quint64>)),this,
                    SLOT(processMessage(EmailClientApi::MessageEvent,quint64,quint64,QList<quint64>)),Qt::QueuedConnection);
   \endcode
   And then when want to end need to do:
   \code
   notifier->cancel();
   delete notifier; // or notifier->deleteLater();
   \endcode
 */
class NMAPI_EXPORT NmApiEventNotifier : public NmApiMessageTask
{
    Q_OBJECT
public:

    NmApiEventNotifier(QObject *parent);
    virtual ~NmApiEventNotifier();
    virtual bool start();
    virtual void cancel();
    bool isRunning() const;

signals:
    
    void mailboxEvent(EmailClientApi::NmApiMailboxEvent event, QList<quint64> id);
    void messageEvent(EmailClientApi::NmApiMessageEvent event, quint64 mailboxId, quint64 folderId, QList<quint64> envelopeIdList);

public slots:

    void sendEventsFromBuffer();

private:
    NmApiEventNotifierPrivate *mNmApiEventNotifierPrivate;
};

}

Q_DECLARE_METATYPE(QList<quint64>)

#endif
