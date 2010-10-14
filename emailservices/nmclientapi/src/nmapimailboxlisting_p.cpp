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
NmApiMailboxListingPrivate::NmApiMailboxListingPrivate(QObject *parent) 
:QObject(parent), 
mEngine(NULL), 
mIsRunning(false)
{
    NM_FUNCTION;
    mEngine = NmApiEngine::instance();
}

NmApiMailboxListingPrivate::~NmApiMailboxListingPrivate()
{
    NM_FUNCTION;
    NmApiEngine::releaseInstance(mEngine);
}

/*!
   \brief Get mailboxes from engine.    
   \return Count of mailboxes 
 */
qint32 NmApiMailboxListingPrivate::listMailboxes()
{
    NM_FUNCTION;
    mIsRunning = true;
    mMailboxes.clear();
    mEngine->listMailboxes(mMailboxes);
    return mMailboxes.count();
}

/*! 
   \brief Returns results after mailboxesListed signal is received.
   
    Caller gets ownership of mailboxes. Returns true if results were available.
    It clears list of mailboxes (in private members) after be called.
    It also at start clear inputlist of NmApiMailbox.
    
    \return Return true if results were avaible
    \param mailboxes List of mailboxes to filled. On start is cleared. 
 */
bool NmApiMailboxListingPrivate::mailboxes(QList<NmApiMailbox> &mailboxes)
{
    mailboxes.clear();


    bool ret(mIsRunning);
    mailboxes.clear();
    while (!mMailboxes.isEmpty()) {
        mailboxes << mMailboxes.takeFirst();
    }
    mIsRunning = false;
    return ret;
}
/*!
   \brief Return info if listing is running
 */
bool NmApiMailboxListingPrivate::isRunning() const
{
    NM_FUNCTION;
    return mIsRunning;
}

/*!
   \brief Clears list of mailboxes.
 */
void NmApiMailboxListingPrivate::cancel() 
{
    mIsRunning = false;
    mMailboxes.clear();
}
}
