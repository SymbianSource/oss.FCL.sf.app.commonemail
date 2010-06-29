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


namespace EmailClientApi
{

/*!
   \class Class for creating list of all folders
 */

/*!
   Constructor of class. It set start values.
 */
NmApiFolderListing::NmApiFolderListing(QObject *parent, const quint64 &nmMailboxId) :
    NmApiMessageTask(parent)
{
    NM_FUNCTION;
    
    mFolderListing = new NmApiFolderListingPrivate(this);
    mFolderListing->mIsRunning = false;
    mFolderListing->mMailboxId = nmMailboxId;
}

/*!
   Destructor of class. It release engine to be safe if manual releasing won't work.
 */
NmApiFolderListing::~NmApiFolderListing()
{
    NM_FUNCTION;
    
    if (mFolderListing->mIsRunning) {
        mFolderListing->releaseEngine();
    }
}

/*! 
   \brief Returns results after foldersListed signal is received.
   
    Caller gets ownership of messages. Returns true if results were available.
    It clears list of folders after be called.
    It also at start clear inputlist of NmFolder.
 */
bool NmApiFolderListing::getFolders(QList<EmailClientApi::NmApiFolder> &folders)
{
    NM_FUNCTION;
    
    folders.clear();
    if (!mFolderListing->mIsRunning || mFolderListing->mFolders.isEmpty()) {
        return false;
    }
    folders = mFolderListing->mFolders;
    mFolderListing->mFolders.clear();
    return true;
}

/*!
   \brief Starts gathering folders list.
   
   In first turn it will get whole folderlist. and then
   it emits information signal \sa folderListed 
   
   To asynchronous operation ce be used \sa QTimer::singleShot on this method.
   Example:
   <code> 
   QTimer::singleShot(0,nmFolderListing,SLOT(start());
   </code>
   
 */
bool NmApiFolderListing::start()
{
    NM_FUNCTION;
    
    if (mFolderListing->mIsRunning) {
        return true;
    }

    if (!mFolderListing->initializeEngine()) {
        QMetaObject::invokeMethod(this, "foldersListed", Qt::QueuedConnection, Q_ARG(qint32,
            (qint32) FolderListingFailed));
        return false;
    }

    qint32 folderCount = mFolderListing->grabFolders();

    mFolderListing->mIsRunning = true;
    QMetaObject::invokeMethod(this, "foldersListed", Qt::QueuedConnection,
        Q_ARG(qint32, folderCount));

    return true;
}

/*!
   \brief Stop gathering folder list.
   
   In first it change state of listing.
   Then it release engine.
   On end it clears list of folders and emits \sa NmApiMessageTask::canceled() signal.
 */
void NmApiFolderListing::cancel()
{
    NM_FUNCTION;
    
    if (!mFolderListing->mIsRunning) {
        return;
    }

    mFolderListing->mIsRunning = false;
    mFolderListing->releaseEngine();
    mFolderListing->mFolders.clear();

    emit canceled();
}

/*!
   \brief Return info if listing is running
 */
bool NmApiFolderListing::isRunning() const
{
    NM_FUNCTION;
    
    return mFolderListing->mIsRunning;
}

}


