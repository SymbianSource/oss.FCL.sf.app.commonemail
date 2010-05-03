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

#include <QList>
#include <QVariant>
#include <QString>

#include "nmapiengine.h"
#include "nmapieventnotifier_p.h"
#include "nmapieventnotifier.h"


quint32 IntervalEmitingSignals = 10000;

namespace EmailClientApi
{
/*!
 * Constructor
 */
NmEventNotifier::NmEventNotifier(QObject *parent) :
    NmMessageTask(parent)

{
    //set timer
    mNmEventNotifierPrivate = new NmEventNotifierPrivate(this);
    mNmEventNotifierPrivate->mEmitSignals = new QTimer(this);
    mNmEventNotifierPrivate->mEmitSignals->setInterval(IntervalEmitingSignals);
    connect(mNmEventNotifierPrivate->mEmitSignals, SIGNAL(timeout()), this, SLOT(
        sendEventsFromBuffer()), Qt::QueuedConnection);

}

/*!
 * Destructor
 */
NmEventNotifier::~NmEventNotifier()
{
    if (mNmEventNotifierPrivate->mIsRunning) {
        mNmEventNotifierPrivate->releaseEngine();
    }

    delete mNmEventNotifierPrivate;
}

/*!
 * Start monitoring email events
 * 
 * \return Value tells about monitoring system running
 */
bool NmEventNotifier::start()
{
    bool result = false;

    if (mNmEventNotifierPrivate->mIsRunning) {
        result = true;
    }
    else
        if (!mNmEventNotifierPrivate->initializeEngine()) {
            mNmEventNotifierPrivate->mIsRunning = false;
            result = false;
        }
        else {
            qRegisterMetaType<QList<quint64> > ("QList<quint64>");
            qRegisterMetaType<NmApiMessage> ("NmApiMessage");

            connect(mNmEventNotifierPrivate->mEngine, SIGNAL(emailStoreEvent(NmApiMessage)), this,
                SLOT(emailStoreEvent(NmApiMessage)), Qt::QueuedConnection);

            mNmEventNotifierPrivate->mEmitSignals->start();
            mNmEventNotifierPrivate->mIsRunning = true;
            result = true;
        }
    return result;
}

/*!
 * Cancels monitoring.
 * 
 * In user responsibility is to cancel monitoring.
 * On end it clear buffer events and emits \sa NmMessageTask::canceled() signal.
 */
void NmEventNotifier::cancel()
{
    if (!mNmEventNotifierPrivate->mIsRunning) {
        return;
    }

    mNmEventNotifierPrivate->mIsRunning = false;
    mNmEventNotifierPrivate->mEmitSignals->stop();

    if (mNmEventNotifierPrivate->mEngine) {
        disconnect(mNmEventNotifierPrivate->mEngine, SIGNAL(emailStoreEvent(NmApiMessage)), this,
            SLOT(emailStoreEvent(NmApiMessage)));
    }

    mNmEventNotifierPrivate->releaseEngine();

    mNmEventNotifierPrivate->mBufferOfEvents.clear();

    emit canceled();
}

bool NmEventNotifier::isRunning() const
{
    return mNmEventNotifierPrivate->mIsRunning;
}

/*!
 * It check each object in buffer and emit signal with it.
 * 
 * After end of work of this function buffer is empty.
 * It is called by timeout signal from timer.
 */
void NmEventNotifier::sendEventsFromBuffer()
{
    qRegisterMetaType<MailboxEvent> ("MailboxEvent");
    qRegisterMetaType<MailboxEvent> ("MessageEvent");
    NmApiMessage events;
    while (!mNmEventNotifierPrivate->mBufferOfEvents.isEmpty()) {
        events = mNmEventNotifierPrivate->mBufferOfEvents.takeFirst();
        switch (events.objectType) {
            case EMailbox:
                switch (events.action) {
                    case ENew:
                        emit mailboxEvent(MailboxCreated, events.objectIds);
                        break;
                    case EChange:

                        break;
                    case EDeleted:
                        emit mailboxEvent(MailboxDeleted, events.objectIds);
                        break;
                    default:
                        break;
                }
                break;
            case EFolder:
                switch (events.action) {
                    case ENew:

                        break;
                    case EChange:

                        break;
                    case EDeleted:

                        break;
                    default:
                        break;
                }
                break;
            case EMessage:
                switch (events.action) {
                    case ENew:
                        emit messageEvent(MessageCreated, events.mailboxId, events.folderId,
                            events.objectIds);
                        break;
                    case EChange:
                        emit messageEvent(MessageChanged, events.mailboxId, events.folderId,
                            events.objectIds);
                        break;
                    case EDeleted:
                        emit messageEvent(MessageDeleted, events.mailboxId, events.folderId,
                            events.objectIds);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

} //End of EmailClientApi

#include "moc_nmapieventnotifier.cpp"
