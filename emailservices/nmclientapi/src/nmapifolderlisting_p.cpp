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
#include "nmapifolderlisting_p.h"

namespace EmailClientApi
{
NmFolderListingPrivate::NmFolderListingPrivate(QObject* parent) :
    QObject(parent), mEngine(NULL)
{

}

NmFolderListingPrivate::~NmFolderListingPrivate()
{
    releaseEngine();
}

/*!
 * \brief It initialize engine for email operations. 
 * 
 * When use initializeEngine need to remember release it.
 * It return value if initialization go good.
 * \sa releaseEngine 
 */
bool NmFolderListingPrivate::initializeEngine()
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
void NmFolderListingPrivate::releaseEngine()
{
    NmEngine::releaseInstance(mEngine);
}

/*!
 * \brief It grab folders from engine. 
 * 
 * When it start grabing, it release all old.
 * Because it uses NmFolder with sharedData we don't need care about release memory.
 * 
 * \return Count of folders
 */
qint32 NmFolderListingPrivate::grabFolders()
{
    mFolders.clear();
    mEngine->listFolders(mMailboxId, mFolders);
    return mFolders.count();
}

}

