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
* Constant definitions for EmailShutter
*
*/

#ifndef EMAILSERVERMONITORCONST_H
#define EMAILSERVERMONITORCONST_H

#include <e32cmn.h>						// TUid
#include "emailservermonitor.hrh"


/**
 * Maximum number of retries to try to start the Email Server
 */
const TInt KEsmMaxRestarts = 5;

/**
 * How long uptime (in minutes) is required to reset the restart counter.
 */
const TInt KEsmUptimeToResetRestartCounter = 10;

/**
 * Delay before restarting the server after it has been closed
 */
const TInt KEsmRestartDelayInSeconds = 10;

/**
 * Executable startup mode
 */
enum TEmailServerMonitorMode
    {
    EEsmModeNormal,
    EEsmModeOnlyShutter,
    EEsmModeRestartAfterInstallation
    };

/**
 * Name of the Email Server Monitor process/thread
 */
_LIT( KEmailServerMonitorName, "EmailServerMonitor" );

/**  Name of the EmailServer executable
 */
_LIT( KEmailServerExe, "fsmailserver.exe" );

/**
 * Always Online email plugin and Email Server UIDs
 */
const TUid KAlwaysOnlineEmailPluginUid =  { KAlwaysOnlineEmailPluginUidAsTInt };
const TUid KEmailServerUid = { FSMailServerUidAsTInt };


#endif // EMAILSERVERMONITORCONST_H
