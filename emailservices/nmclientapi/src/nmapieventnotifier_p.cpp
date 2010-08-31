/*
 * Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

namespace EmailClientApi
{
const quint32 IntervalEmitingSignals = 10000;

/*!
   Constructor
 */
NmApiEventNotifierPrivate::NmApiEventNotifierPrivate(QObject *parent) :
    QObject(parent), mEmitSignals(NULL), mEngine(NULL), mIsRunning(false)
{
    NM_FUNCTION;
    mEmitSignals = new QTimer(this);
    mEmitSignals->setInterval(IntervalEmitingSignals);
    connect(mEmitSignals, SIGNAL(timeout()), this, SIGNAL(
        timedOut()));
    mEngine = NmApiEngine::instance();
}

/*!
   Destructor
 */
NmApiEventNotifierPrivate::~NmApiEventNotifierPrivate()
{
    NM_FUNCTION;
    mEmitSignals->stop();
    NmApiEngine::releaseInstance(mEngine);
}

/*!
   Returns true if the 
 */
bool NmApiEventNotifierPrivate::isRunning() const
{
    return mIsRunning;
}

/*!
   Start monitoring email events
   
   \return Value tells about monitoring system running
 */
bool NmApiEventNotifierPrivate::start()
{
    NM_FUNCTION;
    
    bool result(false);

    if (mIsRunning) {
        result = true;
    }
    else {
        qRegisterMetaType<QList<quint64> > ("QList<quint64>");
        qRegisterMetaType<NmApiMessage> ("NmApiMessage");

        connect(mEngine, SIGNAL(emailStoreEvent(NmApiMessage)), this,
                SLOT(emailStoreEvent(NmApiMessage)));
            
        mEngine->startCollectingEvents();
            
        mEmitSignals->start();
        mIsRunning = true;
        result = true;
    }
    return result;
}
/*!
   \brief Stop listening events.
 */
void NmApiEventNotifierPrivate::stop()
{
    mIsRunning = false;
    mEmitSignals->stop();
    disconnect(mEngine, SIGNAL(emailStoreEvent(NmApiMessage)), this,
            SLOT(emailStoreEvent(NmApiMessage)));
}

/*!
    Returns event buffer, after function call the buffer is empty
 */
void NmApiEventNotifierPrivate::events(QList<NmApiMessage> &events)
{
    while (!mBufferOfEvents.isEmpty()) {
        events << mBufferOfEvents.takeFirst();
    }
}

/*!
   Add one email event into buffer.
   
   It is run by \sa NmApiEngine::emailStoreEvent signal.
   \sa NmApiMessage
   \param events It contains full info about object and it event.
 */
void NmApiEventNotifierPrivate::emailStoreEvent(const NmApiMessage &events)
{
    NM_FUNCTION;
    mBufferOfEvents << events;
}

void NmApiEventNotifierPrivate::cancel()
{
    NM_FUNCTION;
    
    if (!mIsRunning) {
        return;
    }

    mIsRunning = false;
    mEmitSignals->stop();

    disconnect(mEngine, SIGNAL(emailStoreEvent(NmApiMessage)), this,
            SLOT(emailStoreEvent(NmApiMessage)));

    mBufferOfEvents.clear();
}

}

