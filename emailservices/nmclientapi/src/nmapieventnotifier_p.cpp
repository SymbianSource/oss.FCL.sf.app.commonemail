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

namespace EmailClientApi
{
/*!
 * Constructor
 */
NmEventNotifierPrivate::NmEventNotifierPrivate(QObject *parent) :
    QObject(parent), mEmitSignals(NULL), mEngine(NULL), mIsRunning(false)
{

}

/*!
 * Destructor
 */
NmEventNotifierPrivate::~NmEventNotifierPrivate()
{

}

/*!
 * \brief It initialize engine for email operations. 
 * 
 * When use initializeEngine need to remember release it.
 * It return value if initialization go good.
 * \sa releaseEngine 
 * \return Return true if engine works.
 */
bool NmEventNotifierPrivate::initializeEngine()
{
    if (!mEngine) {
        mEngine = NmEngine::instance();
    }

    return mEngine ? true : false;
}

/*!
 * \brief It release engine for email operations.
 * 
 * It release Engine and return value if release go good.
 * 
 * \arg engine Is used to get info if engine was released, if yes, then argument have value 0.
 * 
 * \sa initializeEngine
 */
void NmEventNotifierPrivate::releaseEngine()
{
    if (mIsRunning) {
        cancel();
    }
    else {
        NmEngine::releaseInstance(mEngine);
    }
}

/*!
 * Add one email event into buffer.
 * 
 * It is run by \sa NmEngine::emailStoreEvent signal.
 * \sa NmApiMessage
 * \arg It contains full info about object and it event.
 */
void NmEventNotifierPrivate::emailStoreEvent(const NmApiMessage &events)
{
    mBufferOfEvents << events;
}

void NmEventNotifierPrivate::cancel()
{

}

}

