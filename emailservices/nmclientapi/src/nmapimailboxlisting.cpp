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
   \class Class for creating list of all mailboxes
 */

/*!
   Constructor of class. It set start values.
 */
NmApiMailboxListing::NmApiMailboxListing(QObject *parent) :
    NmApiMessageTask(parent)
{
    NM_FUNCTION;
    
    mNmApiMailboxListingPrivate = new NmApiMailboxListingPrivate(this);
}

/*!
   Destructor of class. It release engine to be safe if manual releasing won't work.
 */
NmApiMailboxListing::~NmApiMailboxListing()
{
    NM_FUNCTION;
}

/*! 
   \brief Returns results after mailboxesListed signal is received.
   
    Caller gets ownership of messages. Returns true if results were available.
    It clears list of mailboxes (in private members) after be called.
    It also at start clear inputlist of NmApiMailbox.
    
    \return Return true if results were avaible
    \param mailboxes List of mailboxes to filled. On start is cleared. 
 */
bool NmApiMailboxListing::getMailboxes(QList<NmApiMailbox> &mailboxes)
{
    NM_FUNCTION;
    return mNmApiMailboxListingPrivate->mailboxes(mailboxes);
}

/*!
   \brief Starts gathering mailbox list.
   
   In first turn it will get whole mailboxlist. 
   Then it initialize core arguments and emits signal when ready.
   
   To asynchronous operation can be used \sa QTimer::singleShot on this method.
   Example:
   <code> 
   QTimer::singleShot(0,nmMailboxListing,SLOT(start());
   </code>
   
   \return Return true if everything go good and core of listing works good.
   
 */
bool NmApiMailboxListing::start()
{
    NM_FUNCTION;
    qint32 mailboxCount = mNmApiMailboxListingPrivate->listMailboxes();
    QMetaObject::invokeMethod(this, "mailboxesListed", Qt::QueuedConnection,
        Q_ARG(qint32, mailboxCount));

    return true;
}

/*!
   \brief Stop gathering mailbox list.
   
   In first it change state of listing.
   Then it release engine.
   On end it clears list of mailboxes and emits \sa NmApiMessageTask::canceled() signal.
 */
void NmApiMailboxListing::cancel()
{
    NM_FUNCTION;
    
    mNmApiMailboxListingPrivate->cancel();
    emit canceled();
}

/*!
   \brief Return info if listing is running
   
   \return Return true if listing is running
 */
bool NmApiMailboxListing::isRunning() const
{
    NM_FUNCTION;
    return mNmApiMailboxListingPrivate->isRunning();
}
}


