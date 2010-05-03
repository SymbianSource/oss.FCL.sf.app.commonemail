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

#ifdef __WINSCW__
const int NmStartupDelay = 14000; // 14s
#else
const int NmStartupDelay = 4000; // 4s
#endif


/*!
	int main
*/
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    NmMailAgent agent;

    // Agent will be initialised with a delay to avoid startup problems
    QTimer::singleShot(NmStartupDelay, &agent, SLOT(delayedStart()));

    return app.exec();
}
