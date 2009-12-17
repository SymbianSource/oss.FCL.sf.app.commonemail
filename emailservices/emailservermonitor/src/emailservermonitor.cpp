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
* Implementation of EmailServerMonitor
*
*/

//  Include Files  
#include <e32base.h>
#include <e32std.h>
#include <e32des16.h>                   // Descriptors
#include <apacmdln.h>
#include <apgcli.h>

#include <AlwaysOnlineManagerClient.h>  // RAlwaysOnlineClientSession
#include <CPsRequestHandler.h>          // CPSRequestHandler

#include "emailtrace.h"
#include "emailservermonitorutilities.h"
#include "emailservermonitor.h"
#include "emailservermonitorconst.h"

const TUint KOneSecondInMicroSeconds = 1000000;


// ======== MEMBER FUNCTION DEFINITIONS ========

/**
 * Two-phased class constructor.
 */
CEmailServerMonitor* CEmailServerMonitor::NewL()
    {
    FUNC_LOG;
    CEmailServerMonitor* self = CEmailServerMonitor::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

/**
 * Two-phased class constructor.
 */
CEmailServerMonitor* CEmailServerMonitor::NewLC()
    {
    FUNC_LOG;
    CEmailServerMonitor* self = new (ELeave) CEmailServerMonitor();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
/**
 * Destructor of CEmailServerMonitor class.
 */
CEmailServerMonitor::~CEmailServerMonitor()
    {
    FUNC_LOG;
    iApaLsSession.Close();
    iProcess.Close();
    iDelayTimer.Close();
    }
    
/**
 * Default class constructor.
 * Only NewL can be called
 */
CEmailServerMonitor::CEmailServerMonitor()
    : CActive( EPriorityStandard ), iState( EEsmStateIdle ), iRestarts( 0 )
    {
    FUNC_LOG;
    }
    
/**
 * Second phase class constructor.
 */
void CEmailServerMonitor::ConstructL()
    {
    FUNC_LOG;
    User::LeaveIfError( iApaLsSession.Connect() );
    User::LeaveIfError( iDelayTimer.CreateLocal() );
    
    CActiveScheduler::Add(this);
    }
    
/**
 * Start email server monitoring.
 */
void CEmailServerMonitor::Start()
    {
    FUNC_LOG;
    iLastRestartTime.UniversalTime();
    
    // Set initializing state and complete self
    // -> functionality will continue in RunL
    iState = EEsmStateInitializing;
    TRequestStatus* status = &iStatus;
    SetActive();
    User::RequestComplete( status, KErrNone );
    }

/**
 * Start Email Server and start monitoring it
 */
TBool CEmailServerMonitor::StartEmailServerMonitoring()
    {
    FUNC_LOG;
    // Give iProcess as parameter for IsProcessRunning so that it will
    // be updated in case that the Email Server process is running.
    TBool running = IsProcessRunning( KEmailServerUid, &iProcess );
	
    // Start Email Server if it's not yet running
    if( !running )
        {
        iRestarts++;
        
        TThreadId threadId;
        TRequestStatus reqStatusForRendezvous;
        TRAPD( error, StartApplicationL( KEmailServerExe(), threadId, reqStatusForRendezvous ) );
        
        // If application was started succesfully
        if( error == KErrNone )
            {
            User::WaitForRequest( reqStatusForRendezvous );
            
            // If process started succesfully, we are ready for observing
            if( reqStatusForRendezvous.Int() == KErrNone )
                {
                // Close previous process handle
                iProcess.Close();
                
                // Get handle to newly created process using the threadId
                RThread thread;
                TInt error = thread.Open( threadId );
                if( error == KErrNone )
                    {
                    error = thread.Process( iProcess );
                    thread.Close();
                    // Indicate success only if iProcess update succeeded, as it's
                    // assumed to be up-to-date if Email Server process is running
                    if( error == KErrNone )
                        {
                        running = ETrue;
                        }
                    }
                }
            }
        }
    
    if( running )
        {
        DoStartMonitoring();
        }
    else
        {
        INFO( "Restart failed!" );
        }
    
    return running;
    }

/**
 * Starts the actual monitoring. Assumes that Email Server is running
 * and Monitor's internal variables are set accordingly.
 */
void CEmailServerMonitor::DoStartMonitoring()
    {
    FUNC_LOG;
    SetActive();
    iProcess.Logon( iStatus );
    iState = EEsmStateMonitoring;
    }

/**
 * Start specified application, leaves if failed to start the application
 */
void CEmailServerMonitor::StartApplicationL(
        const TDesC& aAppName,
        TThreadId& aThreadId,
        TRequestStatus& aReqStatusForRendezvous )
    {
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetExecutableNameL( aAppName );
    // Launch Email Server in background so that it doesn't steal
    // the focus during it's construction
    cmdLine->SetCommandL( EApaCommandBackground );
    
    User::LeaveIfError( iApaLsSession.StartApp( *cmdLine, aThreadId, &aReqStatusForRendezvous ) );
    
    CleanupStack::PopAndDestroy( cmdLine );
    }

/**
 * DoCancel of active object
 */
void CEmailServerMonitor::DoCancel()
    {
    FUNC_LOG;
    iState = EEsmStateIdle;
    iProcess.LogonCancel( iStatus );
    iDelayTimer.Cancel();
    }

/**
 * RunL of active object
 */
void CEmailServerMonitor::RunL()
    {
    FUNC_LOG;
    
    INFO_1( "iStatus: %d", iStatus.Int() );
    INFO_1( "iState: %d", iState );

    switch( iState )
        {
        case EEsmStateMonitoring:
            {
            // Write the process exit information to the debug log.
            INFO_1( "ExitType: %d", iProcess.ExitType() );
            INFO_1( "ExitReason: %d", iProcess.ExitReason() );
            HandleMonitorEvent();
            }
            break;
            
        case EEsmStateInitializing:
        case EEsmStateRestarting:
            {
            // Restart Email server and start monitoring it again
            if( !StartEmailServerMonitoring() )
                {
				// If start failed initiate new delayed restart
                INFO( "Initiating new delayed restart" );
                InitiateDelayedRestart();
                }
            }
            break;
            
        case EEsmStateIdle:
        default:
            {
            // Shouldn't happen
            INFO( "Unknown state or not observing!" );
            }
        }
    }

/**
 * Handle email server monitoring event
 */
void CEmailServerMonitor::HandleMonitorEvent()
    {
    FUNC_LOG;
    TExitType exitType = iProcess.ExitType(); 
    if ( exitType == EExitPanic )
        {
        INFO( "Initiating delayed restart of Email Server" );
        InitiateDelayedRestart();
        }
    else if ( exitType == EExitPending )
        {
        INFO( "Process is still alive, restarting monitoring" );
        DoStartMonitoring();
        }
    else
        {
        INFO( "Email Server terminated, not restarting!" );
        iState = EEsmStateIdle;
        }
    }

/**
 * Initiate an asynchronous delayed restart
 */
void CEmailServerMonitor::InitiateDelayedRestart()
    {
    FUNC_LOG;
    TTime now;
    now.UniversalTime();
    
    // Calculate minutes from last restart
    TTimeIntervalMinutes minutesSinceLastRestart = 0;
    now.MinutesFrom( iLastRestartTime, minutesSinceLastRestart );
    INFO_1( "Minutes since last restart=%d", minutesSinceLastRestart.Int() );
    
    iLastRestartTime = now;
    
    // If the process has been running successfully long enough then reset
    // the restart counter.
    if( minutesSinceLastRestart >= TTimeIntervalMinutes( KEsmUptimeToResetRestartCounter ) )
        {
        INFO( "Long enough uptime, reset restart counter" );
        iRestarts = 0;
        }

    // Initiate new delayed restart, if restart limit is not exceeded
    if( iRestarts < KEsmMaxRestarts )
        {
        iDelayTimer.After( iStatus, KEsmRestartDelayInSeconds * KOneSecondInMicroSeconds );
        SetActive();
        iState = EEsmStateRestarting;
        }
    else
        {
        INFO( "Maximum restarts exceeded!" );
        iState = EEsmStateIdle;
        }
    }
