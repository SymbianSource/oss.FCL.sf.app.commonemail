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
#include "nmapienvelopelisting_p.h"

namespace EmailClientApi
{
NmEnvelopeListingPrivate::NmEnvelopeListingPrivate(QObject* parent) :
    QObject(parent), mEngine(NULL)
{

}

NmEnvelopeListingPrivate::~NmEnvelopeListingPrivate()
{
    releaseEngine();
}

/*!
 * \brief It initialize engine for email operations. 
 * 
 * When use initializeEngine need to remember release it.
 * It return value if initialization go good.
 * \sa releaseEngine 
 */
bool NmEnvelopeListingPrivate::initializeEngine()
{
    mEngine = NmEngine::instance();
    return mEngine ? true : false;
}

/*!
 * \brief It release engine for email operations.
 * 
 * It release Engine and return value if release go good.
 * 
 * \arg engine Is used to get info if engine was released, if yes, then argument have value 0.
 * 
 * \sa initializeEngine
 */
void NmEnvelopeListingPrivate::releaseEngine()
{
    NmEngine::releaseInstance(mEngine);
}

/*!
 * \brief It grab envelopes from engine. 
 * 
 * When it start grabing, it release all old.
 * Because it uses NmMessageEnvelope with sharedData we don't need care about release memory.
 * 
 * \return Count of envelopes
 */
qint32 NmEnvelopeListingPrivate::grabEnvelopes()
{
    mEnvelopes.clear();
    mEngine->listEnvelopes(mailboxId, folderId, mEnvelopes);
    return mEnvelopes.count();
}
}

