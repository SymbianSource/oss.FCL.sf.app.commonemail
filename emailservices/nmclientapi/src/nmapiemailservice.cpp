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

#include <nmapiemailservice.h>
#include "nmapiengine.h"

#include <nmapimailbox.h>
#include <nmapimessageenvelope.h>

namespace EmailClientApi
{

/*!
   constructor for NmEmailService
 */
NmApiEmailService::NmApiEmailService(QObject *parent) :
    QObject(parent), mEngine(NULL), mIsRunning(false)
{

}

/*!
   destructor for NmApiEmailService
 */
NmApiEmailService::~NmApiEmailService()
{
    if (mEngine) {
        uninitialise();
    }
}

/*!
   gets mail message envelope by id (see also NmEventNotifier)
 */
bool NmApiEmailService::getEnvelope(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 envelopeId,
    EmailClientApi::NmApiMessageEnvelope &envelope)
{
    if (!mEngine) {
        return false;
    }
    return mEngine->getEnvelopeById(mailboxId, folderId, envelopeId, envelope);
}

/*!
    gets mailbox info by id (see also NmEventNotifier)
 */
bool NmApiEmailService::getMailbox(const quint64 mailboxId, EmailClientApi::NmApiMailbox &mailboxInfo)
{
    if (!mEngine) {
        return false;
    }
    return mEngine->getMailboxById(mailboxId, mailboxInfo);
}

/*!
   Initialises email service. this must be called and initialised signal received 
   before services of the library are used.
 */
void NmApiEmailService::initialise()
{
    if (!mEngine) {
        mEngine = NmApiEngine::instance();
    }

    if (mEngine) {
        mIsRunning = true;
        emit initialized(true);
    }
    else {
        emit initialized(false);
    }
}

/*!
    frees resources.
 */
void NmApiEmailService::uninitialise()
{
    NmApiEngine::releaseInstance(mEngine);
    mIsRunning = false;
}

/*!
   returns isrunning flag value
 */
bool NmApiEmailService::isRunning() const
{
    return mIsRunning;
}

}

