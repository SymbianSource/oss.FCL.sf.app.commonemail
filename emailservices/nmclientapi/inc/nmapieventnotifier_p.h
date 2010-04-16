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

#ifndef NMAPIEVENTNOTIFIERPRIVATE_H
#define NMAPIEVENTNOTIFIERPRIVATE_H

#include "nmprivateclasses.h"
#include "nmcommon_api.h"
#include "nmapiengine.h"

namespace EmailClientApi
{
class NmEventNotifierPrivate : public QObject
{
    Q_OBJECT
public:
    NmEventNotifierPrivate(QObject *parent = 0);
    virtual ~NmEventNotifierPrivate();

    bool initializeEngine();
    void releaseEngine();
    void cancel();

public slots:
    void emailStoreEvent(const NmApiMessage &events);

public:
    QTimer *mEmitSignals;
    NmEngine *mEngine;
    QList<NmApiMessage> mBufferOfEvents;
    bool mIsRunning;
};

}

#endif
