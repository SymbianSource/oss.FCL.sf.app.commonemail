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
#include "nmapimailboxlisting.h"

namespace EmailClientApi
{

/*!
 * \class Class for creating list of all mailboxes
 */

/*!
 * Constructor of class. It set start values.
 */
NmMailboxListing::NmMailboxListing(QObject *parent) :
    NmMessageTask(parent)
{
    mNmMailboxListingPrivate = new NmMailboxListingPrivate(this);
    mNmMailboxListingPrivate->mIsRunning = false;
}

/*!
 * Destructor of class. It release engine to be safe if manual releasing won't work.
 */
NmMailboxListing::~NmMailboxListing()
{
    if (mNmMailboxListingPrivate->mIsRunning) {
        mNmMailboxListingPrivate->releaseEngine();
    }
    delete mNmMailboxListingPrivate;
}

/*! 
 * \brief Returns results after mailboxesListed signal is received.
 * 
 *  Caller gets ownership of messages. Returns true if results were available.
 *  It clears list of mailboxes (in private members) after be called.
 *  It also at start clear inputlist of NmMailbox.
 *  
 *  \return Return true if results were avaible
 *  \arg List of mailboxes to filled. On start is cleared. 
 */
bool NmMailboxListing::mailboxes(QList<EmailClientApi::NmMailbox> &mailboxes)
{
    mailboxes.clear();

    bool result = false;

    if (!mNmMailboxListingPrivate->mIsRunning) {
        result = false;
    }
    else
        if (mNmMailboxListingPrivate->mMailboxes.isEmpty()) {
            result = false;
        }
        else {
            mailboxes = mNmMailboxListingPrivate->mMailboxes;

            mNmMailboxListingPrivate->mMailboxes.clear();

            result = true;
        }
    return result;
}

/*!
 * \brief Starts gathering mailbox list.
 * 
 * In first turn it will get whole mailboxlist. 
 * Then it initialize core arguments and emits signal when ready.
 * 
 * To asynchronous operation can be used \sa QTimer::singleShot on this method.
 * Example:
 * <code> 
 * QTimer::singleShot(0,nmMailboxListing,SLOT(start());
 * </code>
 * 
 * \return Return true if everything go good and core of listing works good.
 * 
 */
bool NmMailboxListing::start()
{
    bool result = false;
    if (mNmMailboxListingPrivate->mIsRunning) {
        result = true;
    }
    else
        if (!mNmMailboxListingPrivate->initializeEngine()) {
            QMetaObject::invokeMethod(this, "mailboxesListed", Qt::QueuedConnection, Q_ARG(qint32,
                (qint32) MailboxListingFailed));
            result = false;
        }
        else {
            quint64 mailboxCount = mNmMailboxListingPrivate->grabMailboxes();

            mNmMailboxListingPrivate->mIsRunning = true;

            QMetaObject::invokeMethod(this, "mailboxesListed", Qt::QueuedConnection, Q_ARG(qint32,
                mailboxCount));

            result = true;
        }
    return result;
}

/*!
 * \brief Stop gathering mailbox list.
 * 
 * In first it change state of listing.
 * Then it release engine.
 * On end it clears list of mailboxes and emits \sa NmMessageTask::canceled() signal.
 */
void NmMailboxListing::cancel()
{
    if (mNmMailboxListingPrivate->mIsRunning) {

        mNmMailboxListingPrivate->mIsRunning = false;
        mNmMailboxListingPrivate->releaseEngine();
        mNmMailboxListingPrivate->mMailboxes.clear();

        QMetaObject::invokeMethod(this, "canceled", Qt::QueuedConnection);
    }
}

/*!
 * \brief Return info if listing is running
 * 
 * \return Return true if listing is running
 */
bool NmMailboxListing::isRunning() const
{
    return mNmMailboxListingPrivate->mIsRunning;
}

}

#include "moc_nmapimailboxlisting.cpp"
