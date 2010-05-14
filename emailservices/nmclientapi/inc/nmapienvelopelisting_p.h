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

class NmApiEngine;


namespace EmailClientApi
{
class NmApiMessageEnvelope;

class NmApiEnvelopeListingPrivate : public QObject
{
public:

    NmApiEnvelopeListingPrivate(QObject *parent = 0);
    virtual ~NmApiEnvelopeListingPrivate();

    bool initializeEngine();
    void releaseEngine();
    qint32 grabEnvelopes();

    bool mIsRunning;
    quint64 folderId;
    quint64 mailboxId;
    NmApiEngine *mEngine;

    QList<EmailClientApi::NmApiMessageEnvelope> mEnvelopes;
};
}
#endif /* NMAPIENVELOPELISTINGPRIVATE_H */
