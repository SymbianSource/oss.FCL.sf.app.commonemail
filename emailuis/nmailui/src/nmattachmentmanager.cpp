/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmuiheaders.h"

const int NmAttachmentManagerInitialProgressPercent = 5;

/*!
    \class NmAttachmentManager
    \brief Attachment download manager, shares attachment download between editor and viewer
*/

/*!

*/
NmAttachmentManager::NmAttachmentManager(NmUiEngine &uiEngine) :
    mUiEngine(uiEngine),
    mFetchOperation(NULL),
    mFetchObserver(NULL),
    mAttaId(0),
    mProgressValue(0),
    mIsFetching(false)
{

}

/*!

*/
NmAttachmentManager::~NmAttachmentManager()
{
    // cancel fetch
    cancelFetch();
}

/*!
    Starts attachment fetch. If earlier fetch operation exist it is cancelled.
    Set observer with setObserver method to get process and complete events
*/
void NmAttachmentManager::fetchAttachment(
        const NmId &mailboxId, 
        const NmId &folderId, 
        const NmId &messageId, 
        const NmId &messagePartId)
{
    // cancel old fetch operation, Does nothing if fetch not ongoing
    cancelFetch();

    mFetchOperation = mUiEngine.fetchMessagePart(
            mailboxId, folderId, messageId, messagePartId);
    
    if (mFetchOperation) {
        mAttaId = messagePartId;
        mIsFetching = true;
        QObject::connect(mFetchOperation, SIGNAL(operationCompleted(int)),
                this, SLOT(attachmentFetchCompleted(int)));
        
        QObject::connect(mFetchOperation, SIGNAL(operationProgressChanged(int)),
                this, SLOT(changeProgress(int)));
        // set progress to 5 % already in start
        changeProgress(NmAttachmentManagerInitialProgressPercent);
    }
}

/*!
    Retruns true if fetch operation is ongoing
*/
bool NmAttachmentManager::isFetching() const
{
    return mIsFetching;
}

/*!
    Retrunrs part id of attachment if fetch operation is ongoing. Zero id is returned otherwise
*/
NmId NmAttachmentManager::partIdUnderFetch() const
{
    return mAttaId;
}

/*!
    Cancels fetch operation. Does nothing if fetch not ongoing
*/
void NmAttachmentManager::cancelFetch()
{
    if (mFetchOperation && mFetchOperation->isRunning()) { 
        mFetchOperation->cancelOperation();
    }
    mIsFetching = false;
    mAttaId = 0;
    mProgressValue = 0;
}

/*!
    Used by message part fetch operation
*/
void NmAttachmentManager::changeProgress(int value)
{
    if (mFetchObserver && value > mProgressValue) {
        mProgressValue = value;
        mFetchObserver->progressChanged(value);
    }
}

/*!
    Used by message part fetch operation
*/
void NmAttachmentManager::attachmentFetchCompleted(int result)
{
    if (mFetchObserver) {
        mFetchObserver->fetchCompleted(result);
    }
    mAttaId = 0;
    mProgressValue = 0;
    mIsFetching = false;
}

/*!
    Sets fetch observer
*/
void NmAttachmentManager::setObserver(NmAttachmentFetchObserver *observer)
{
    mFetchObserver = observer;
    // send progress event wheng observer changes if fetch ongoing 
    // to get progress bar updating
    if (mIsFetching) {
        changeProgress(mProgressValue);
    }
}

/*!
    Clear observer
*/
void NmAttachmentManager::clearObserver()
{
    mFetchObserver = NULL;
}

/*!
    Returns progress value if fetch ongoing. Otherwise returns 0.
*/
int NmAttachmentManager::progressValue() const
{
    return mProgressValue;
}

