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

#include "nmapiheaders.h"


const quint32 IntervalEmitingSignals = 10000;

namespace EmailClientApi
{
/*!
   Constructor
 */
NmApiEventNotifier::NmApiEventNotifier(QObject *parent) :
    NmApiMessageTask(parent)

{
    NM_FUNCTION;
    
    //set timer
    mNmApiEventNotifierPrivate = new NmApiEventNotifierPrivate(this);
    mNmApiEventNotifierPrivate->mEmitSignals = new QTimer(this);
    mNmApiEventNotifierPrivate->mEmitSignals->setInterval(IntervalEmitingSignals);
    connect(mNmApiEventNotifierPrivate->mEmitSignals, SIGNAL(timeout()), this, SLOT(
        sendEventsFromBuffer()), Qt::QueuedConnection);

}

/*!
   Destructor
 */
NmApiEventNotifier::~NmApiEventNotifier()
{
    NM_FUNCTION;
    
    if (mNmApiEventNotifierPrivate->mIsRunning) {
        mNmApiEventNotifierPrivate->releaseEngine();
    }
}

/*!
   Start monitoring email events
   
   \return Value tells about monitoring system running
 */
bool NmApiEventNotifier::start()
{
    NM_FUNCTION;
    
    bool result = false;

    if (mNmApiEventNotifierPrivate->mIsRunning) {
        result = true;
    }
    else {
        if (!mNmApiEventNotifierPrivate->initializeEngine()) {
            mNmApiEventNotifierPrivate->mIsRunning = false;
            result = false;
        }
        else {
            qRegisterMetaType<QList<quint64> > ("QList<quint64>");
            qRegisterMetaType<NmApiMessage> ("NmApiMessage");

            connect(mNmApiEventNotifierPrivate->mEngine, SIGNAL(emailStoreEvent(NmApiMessage)), mNmApiEventNotifierPrivate,
                SLOT(emailStoreEvent(NmApiMessage)), Qt::QueuedConnection);
            
            // initiate event listening
            mNmApiEventNotifierPrivate->mEngine->startCollectingEvents();
            
            mNmApiEventNotifierPrivate->mEmitSignals->start();
            mNmApiEventNotifierPrivate->mIsRunning = true;
            result = true;
        }
    }
    return result;
}

/*!
   Cancels monitoring.
   
   In user responsibility is to cancel monitoring.
   On end it clear buffer events and emits \sa NmApiMessageTask::canceled() signal.
 */
void NmApiEventNotifier::cancel()
{
    NM_FUNCTION;
    
    mNmApiEventNotifierPrivate->cancel();
    emit canceled();
}

/*!
   Informs if event notifier is running
 */
bool NmApiEventNotifier::isRunning() const
{
    NM_FUNCTION;
    
    return mNmApiEventNotifierPrivate->mIsRunning;
}

/*!
   It check each object in buffer and emit signal with it.
   
   After end of work of this function buffer is empty.
   It is called by timeout signal from timer.
 */
void NmApiEventNotifier::sendEventsFromBuffer()
{
    NM_FUNCTION;
    
    qRegisterMetaType<EmailClientApi::NmApiMailboxEvent> ("EmailClientApi::NmApiMailboxEvent");
    qRegisterMetaType<EmailClientApi::NmApiMessageEvent> ("EmailClientApi::NmApiMessageEvent");
    
    NmApiMessage events;
    while (!mNmApiEventNotifierPrivate->mBufferOfEvents.isEmpty()) {
        events = mNmApiEventNotifierPrivate->mBufferOfEvents.takeFirst();
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

