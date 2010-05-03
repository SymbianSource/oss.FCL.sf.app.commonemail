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

#include "nmapiemailservice.h"
#include "nmapiengine.h"
namespace EmailClientApi
{
NmEmailService::NmEmailService(QObject *parent) :
    QObject(parent), mEngine(NULL)
{

}

NmEmailService::~NmEmailService()
{
    if (mEngine) {
        uninitialise();
    }
}

bool NmEmailService::getEnvelope(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 envelopeId,
    NmMessageEnvelope &envelope)
{
    if (!mEngine) {
        return false;
    }
    return mEngine->envelopeById(mailboxId, folderId, envelopeId, envelope);
}

bool NmEmailService::getMailbox(const quint64 mailboxId, NmMailbox &mailboxInfo)
{
    if (!mEngine) {
        return false;
    }
    return mEngine->mailboxById(mailboxId, mailboxInfo);
}

void NmEmailService::initialise()
{
    if (!mEngine) {
        mEngine = NmEngine::instance();
    }

    if (mEngine) {
        mIsRunning = true;
        emit initialized(true);
    }
    else {
        emit initialized(false);
    }
}

void NmEmailService::uninitialise()
{
    NmEngine::releaseInstance(mEngine);
    mIsRunning = false;
}

bool NmEmailService::isRunning() const
{
    return mIsRunning;
}

}
