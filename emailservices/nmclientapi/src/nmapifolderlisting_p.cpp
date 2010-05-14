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

#include <nmapifolder.h>

namespace EmailClientApi
{

/*!
   Constructor form NmApiFolderListingPrivate
 */
NmApiFolderListingPrivate::NmApiFolderListingPrivate(QObject *parent) :
    QObject(parent), mEngine(NULL)
{

}

/*!
   Destructor for NmApiFolderListingPrivate 
 */
NmApiFolderListingPrivate::~NmApiFolderListingPrivate()
{
    releaseEngine();
}

/*!
   \brief It initialize engine for email operations. 
   
   When use initializeEngine need to remember release it.
   It return value if initialization go good.
   \sa releaseEngine 
 */
bool NmApiFolderListingPrivate::initializeEngine()
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
void NmApiFolderListingPrivate::releaseEngine()
{
    NmApiEngine::releaseInstance(mEngine);
}

/*!
   \brief It grab folders from engine. 
   
   When it start grabing, it release all old.
   Because it uses NmFolder with sharedData we don't need care about release memory.
   
   \return Count of folders or "-1" if there is no engine initialised
 */
qint32 NmApiFolderListingPrivate::grabFolders()
{
    if (!mEngine) {
        return -1;
    }
    
    mFolders.clear();
    mEngine->listFolders(mMailboxId, mFolders);
    return mFolders.count();
}

}

