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


#include <nmapienvelopelisting.h>
#include "nmapienvelopelisting_p.h"

#include <nmapicommonheader.h>

#include "nmapiengine.h"



namespace EmailClientApi
{
/*!
   Constructor of class. It set start values.
 */
NmApiEnvelopeListing::NmApiEnvelopeListing(
    QObject *parent,
    const quint64 folderId,
    const quint64 mailboxId) :
    NmApiMessageTask(parent)
{
    mListingPrivate = new NmApiEnvelopeListingPrivate(this);
    mListingPrivate->mailboxId = mailboxId;
    mListingPrivate->folderId = folderId;
    mListingPrivate->mIsRunning = false;
}

/*!
   Destructor of class. It release engine to be safe if manual releasing won't work.
 */
NmApiEnvelopeListing::~NmApiEnvelopeListing()
{
    if (mListingPrivate->mIsRunning) {
        mListingPrivate->releaseEngine();
    }
}

/*!
   \brief Starts gathering envelopes list.
   
   In first turn it will get whole folderlist. 
   If start works, it do nothing.
   
   To asynchronous operation ce be used \sa QTimer::singleShot on this method.
   Example:
   <code> 
   QTimer::singleShot(0,nmEnvelopeListing,SLOT(start());
   </code>
   
 */
bool NmApiEnvelopeListing::start()
{
    bool result = false;

    if (mListingPrivate->mIsRunning) {
        result = true;
    }
    else {

        bool started = mListingPrivate->initializeEngine();
        if (!started) {
            QMetaObject::invokeMethod(this, "envelopesListed", Qt::QueuedConnection, Q_ARG(qint32,
                (qint32) EnvelopeListingFailed));
            result = false;
        }
        else {
            qint32 envelopesCount = mListingPrivate->grabEnvelopes();

            mListingPrivate->mIsRunning = true;
            QMetaObject::invokeMethod(this, "envelopesListed", Qt::QueuedConnection, Q_ARG(qint32,
                envelopesCount));
            result = true;
        }
    }
    return result;
}

/*!
   \brief Stop gathering envelope list.
   
   In first it change state of listing.
   Then it release engine.
   On end it clears list of envelopes and emits \sa NmApiMessageTask::canceled() signal.
 */
void NmApiEnvelopeListing::cancel()
{
    if (mListingPrivate->mIsRunning) {
        mListingPrivate->mIsRunning = false;
        mListingPrivate->releaseEngine();
        mListingPrivate->mEnvelopes.clear();

        emit canceled();
    }
}

/*! 
   \brief Returns results after envelopesListed signal is received.
   
    Caller gets ownership of envelopes. Returns true if results were available.
    Before calling cancel and start should be called,
    because after second calling it return empty list.
    It also at start clear inputlist of NmMessageEnvelope.
 */
bool NmApiEnvelopeListing::getEnvelopes(QList<EmailClientApi::NmApiMessageEnvelope> &envelopes)
{
    envelopes.clear();

    bool result = false;

    if (!mListingPrivate->mIsRunning) {
        result = false;
    }
    else {
        envelopes = mListingPrivate->mEnvelopes;
        mListingPrivate->mEnvelopes.clear();
        result = true;
    }
    return result;
}

/*!
   \brief Return info if listing is running
 */
bool NmApiEnvelopeListing::isRunning() const
{
    return mListingPrivate->mIsRunning;
}

}
