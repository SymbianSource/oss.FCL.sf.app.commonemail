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

#include "nmapienvelopelisting_p.h"
#include "nmapienvelopelisting.h"
#include "nmapiengine.h"

namespace EmailClientApi
{
/*!
 * Constructor of class. It set start values.
 */
NmEnvelopeListing::NmEnvelopeListing(
    QObject *parent,
    const quint64 folderId,
    const quint64 mailboxId) :
    NmMessageTask(parent)
{
    mListingPrivate = new NmEnvelopeListingPrivate(this);
    mListingPrivate->mailboxId = mailboxId;
    mListingPrivate->folderId = folderId;
    mListingPrivate->mIsRunning = false;
}

/*!
 * Destructor of class. It release engine to be safe if manual releasing won't work.
 */
NmEnvelopeListing::~NmEnvelopeListing()
{
    if (mListingPrivate->mIsRunning) {
        mListingPrivate->releaseEngine();
    }
}

/*!
 * \brief Starts gathering envelopes list.
 * 
 * In first turn it will get whole folderlist. 
 * If start works, it do nothing.
 * 
 * To asynchronous operation ce be used \sa QTimer::singleShot on this method.
 * Example:
 * <code> 
 * QTimer::singleShot(0,nmEnvelopeListing,SLOT(start());
 * </code>
 * 
 */
bool NmEnvelopeListing::start()
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
 * \brief Stop gathering envelope list.
 * 
 * In first it change state of listing.
 * Then it release engine.
 * On end it clears list of envelopes and emits \sa NmMessageTask::canceled() signal.
 */
void NmEnvelopeListing::cancel()
{
    if (mListingPrivate->mIsRunning) {
        mListingPrivate->mIsRunning = false;
        mListingPrivate->releaseEngine();
        mListingPrivate->mEnvelopes.clear();

        emit canceled();
    }
}

/*! 
 * \brief Returns results after envelopesListed signal is received.
 * 
 *  Caller gets ownership of envelopes. Returns true if results were available.
 *  It clears list of envelopes after be called.
 *  It also at start clear inputlist of NmMessageEnvelope.
 */
bool NmEnvelopeListing::envelopes(QList<EmailClientApi::NmMessageEnvelope> &envelopes)
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
 * \brief Return info if listing is running
 */
bool NmEnvelopeListing::isRunning() const
{
    return mListingPrivate->mIsRunning;
}
}

#include "moc_nmapienvelopelisting.cpp"
