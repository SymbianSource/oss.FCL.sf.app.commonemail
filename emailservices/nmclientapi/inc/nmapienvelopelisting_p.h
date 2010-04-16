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

#ifndef NMAPIENVELOPELISTINGPRIVATE_H
#define NMAPIENVELOPELISTINGPRIVATE_H

#include <QObject>

#include "nmapimessageenvelope.h"

class NmEngine;

namespace EmailClientApi
{

class NmEnvelopeListingPrivate : public QObject
{
public:

    NmEnvelopeListingPrivate(QObject *parent = 0);
    virtual ~NmEnvelopeListingPrivate();

    bool initializeEngine();
    void releaseEngine();
    qint32 grabEnvelopes();

    bool mIsRunning;
    quint64 folderId;
    quint64 mailboxId;
    NmEngine* mEngine;

    QList<EmailClientApi::NmMessageEnvelope> mEnvelopes;
};
}
#endif /* NMENVELOPELISTINGPRIVATE_H_ */
