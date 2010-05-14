/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "nmmailagentheaders.h"
#include "nmmailagent.h"
#include "ssastartupwatcher.h"

#ifdef __WINSCW__
const int NmStartupDelay = 14000; // 14s
#else
const int NmStartupDelay = 4000; // 4s
#endif

NmMailAgent *agent = NULL;

/*!
    Callback for reporting startup state
*/
static void startupCallback(int status)
{
    NMLOG(QString("nmailagent: startupCallback %1").arg(status));
    Q_UNUSED(status);

    // either it is an error or 'non critical startup' state has been reached
    // Start the agent in both cases
    QTimer::singleShot(NmStartupDelay, agent, SLOT(delayedStart()));
}

/*!
	main
*/
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    agent = new NmMailAgent;

    CSSAStartupWatcher *startupWatcher = CSSAStartupWatcher::New(startupCallback);
    if (!startupWatcher) {
        NMLOG("nmmailagent - watcher start failed");
        QTimer::singleShot(NmStartupDelay, agent, SLOT(delayedStart()));
    }

    int retValue = app.exec();
    delete startupWatcher;
    delete agent;
    return retValue;
}
