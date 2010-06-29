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
NmApiEnvelopeListingPrivate::NmApiEnvelopeListingPrivate(QObject *parent) :
    QObject(parent), mEngine(NULL)
{
    NM_FUNCTION;
}

NmApiEnvelopeListingPrivate::~NmApiEnvelopeListingPrivate()
{
    NM_FUNCTION;
    
    releaseEngine();
}

/*!
   \brief It initialize engine for email operations. 
   
   When use initializeEngine need to remember release it.
   It return value if initialization go good.
   \sa releaseEngine 
 */
bool NmApiEnvelopeListingPrivate::initializeEngine()
{
    NM_FUNCTION;
    
    mEngine = NmApiEngine::instance();
    return mEngine ? true : false;
}

/*!
   \brief It release engine for email operations.
   
   \sa initializeEngine
 */
void NmApiEnvelopeListingPrivate::releaseEngine()
{
    NM_FUNCTION;
    
    NmApiEngine::releaseInstance(mEngine);
}

/*!
   \brief It grab envelopes from engine. 
   
   When it start grabing, it release all old.
   Because it uses NmApiMessageEnvelope with sharedData we don't need care about release memory.
   
   \return Count of envelopes or "-1" if there is no engine initialised
 */
qint32 NmApiEnvelopeListingPrivate::grabEnvelopes()
{
    NM_FUNCTION;
    
    if(!mEngine){
        return -1;
    }
    
    mEnvelopes.clear();
    mEngine->listEnvelopes(mailboxId, folderId, mEnvelopes);
    return mEnvelopes.count();
}
}

