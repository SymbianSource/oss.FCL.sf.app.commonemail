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
#include "nmapimailboxlisting_p.h"

namespace EmailClientApi
{
NmMailboxListingPrivate::NmMailboxListingPrivate(QObject* parent) :
    QObject(parent), mNmEngine(NULL)
{

}

NmMailboxListingPrivate::~NmMailboxListingPrivate()
{

}

/*!
 * \brief It initialize engine for email operations. 
 * 
 * When use initializeEngine need to remember release it.
 * It return value if initialization go good.
 * \sa releaseEngine 
 * \return Return true if engine is good initilialized.
 */
bool NmMailboxListingPrivate::initializeEngine()
{
    if (!mNmEngine) {
        mNmEngine = NmEngine::instance();
    }

    return mNmEngine ? true : false;
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
void NmMailboxListingPrivate::releaseEngine()
{
    NmEngine::releaseInstance(mNmEngine);
}

/*!
 * \brief It grab mailboxes from engine. 
 * 
 * When it start grabing, it release all old.
 * Because it uses NmMailbox with sharedData we don't need care about release memory.
 * 
 * \return Count of mailboxes
 */
qint32 NmMailboxListingPrivate::grabMailboxes()
{
    mMailboxes.clear();

    mNmEngine->listMailboxes(mMailboxes);
    return mMailboxes.count();
}
}
