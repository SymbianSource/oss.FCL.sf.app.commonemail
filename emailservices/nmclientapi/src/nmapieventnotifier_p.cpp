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

#include "nmapiengine.h"
#include "nmapieventnotifier_p.h"

#include <QTimer>

namespace EmailClientApi
{
/*!
   Constructor
 */
NmApiEventNotifierPrivate::NmApiEventNotifierPrivate(QObject *parent) :
    QObject(parent), mEmitSignals(NULL), mEngine(NULL), mIsRunning(false)
{

}

/*!
   Destructor
 */
NmApiEventNotifierPrivate::~NmApiEventNotifierPrivate()
{

}

/*!
   \brief It initialize engine for email operations. 
   
   When use initializeEngine need to remember release it.
   It return value if initialization go good.
   \sa releaseEngine 
   \return Return true if engine works.
 */
bool NmApiEventNotifierPrivate::initializeEngine()
{
    if (!mEngine) {
        mEngine = NmApiEngine::instance();
    }

    return mEngine ? true : false;
}

/*!
   \brief It release engine for email operations.
   
   \sa initializeEngine
 */
void NmApiEventNotifierPrivate::releaseEngine()
{
    if (mIsRunning) {
        cancel();
    }
    else {
        NmApiEngine::releaseInstance(mEngine);
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
    mBufferOfEvents << events;
}

void NmApiEventNotifierPrivate::cancel()
{
    if (!mIsRunning) {
        return;
    }

    mIsRunning = false;
    mEmitSignals->stop();

    if (mEngine) {
        disconnect(mEngine, SIGNAL(emailStoreEvent(NmApiMessage)), this,
            SLOT(emailStoreEvent(NmApiMessage)));
    }

    releaseEngine();

    mBufferOfEvents.clear();
}

}

