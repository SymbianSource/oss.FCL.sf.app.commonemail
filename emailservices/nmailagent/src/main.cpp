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


#include <QCoreApplication>
#include "nmmailagent.h"

/*!
	int main
*/
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    NmMailAgent agent;
    if( !agent.init() ) {
        // failed to start the framework
        return 1;
    }
    return app.exec();
}
