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
* Main entry point of EmailServerMonitor
*
*/

//  Include Files  
#include <e32base.h>
#include <e32std.h>
#include <e32des16.h>                   // Descriptors
#include <bacline.h>                    // CCommandLineArguments

#include "emailtrace.h"
#include "emailshutdownconst.h"
#include "emailservermonitorconst.h"
#include "emailservermonitorutilities.h"
#include "emailservermonitor.h"
#include "emailshutter.h"


// ---------------------------------------------------------------------------
// Defines operation mode by parsing command line parameters
// ---------------------------------------------------------------------------
//
LOCAL_C TEmailServerMonitorMode ParseCommandLineArgumentsL()
    {
    FUNC_LOG;
    TEmailServerMonitorMode mode = EEsmModeNormal;
    
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    // Check if some command line arguments were given
    if( args->Count() > 1 )
       {
       TPtrC argumentPtr( args->Arg(1) );
       if( argumentPtr == KEmailShutdownHandlerArgOnlyShutter )
           {
           mode = EEsmModeOnlyShutter;
           }
       else if( argumentPtr == KEmailShutdownHandlerArgRestart )
           {
           mode = EEsmModeRestartAfterInstallation;
           }
       }
    CleanupStack::PopAndDestroy(args);
    
    INFO_1( "TEmailServerMonitorMode: %d", mode );
    return mode;
    }

// ---------------------------------------------------------------------------
// Starts active scheduler, runs the needed operations, and then deletes
// the active scheduler when everything is done
// ---------------------------------------------------------------------------
//
LOCAL_C void DoStartL()
    {
    FUNC_LOG;
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    
    TEmailServerMonitorMode mode = ParseCommandLineArgumentsL();

    // Start email shutter functionality
    CEmailShutter* shutter = CEmailShutter::NewLC();
    if( mode == EEsmModeRestartAfterInstallation )
        {
        shutter->SetPsKeyInstallationFinished();
        shutter->RestartServicesAfterInstallation();
        }
    shutter->StartObservingShutdownEvent();

    CEmailServerMonitor* monitor = NULL;
    if( mode != EEsmModeOnlyShutter )
        {
        // Start email server observing functionality
        monitor = CEmailServerMonitor::NewLC();
        monitor->Start();
        shutter->SetMonitor( monitor );
        }

    // Complete client's Rendezvous
    RProcess::Rendezvous( KErrNone );
    
    // Start asynchronous services
    CActiveScheduler::Start();
    
    INFO( "ActiveScheduler stopped, closing down" );
    // Cancel any pending requests and delete handler objects
    // after active scheduler is stopped
    
    if( monitor )
        {
        monitor->Cancel();
        CleanupStack::PopAndDestroy( monitor );
        }
    
    shutter->Cancel();
    CleanupStack::PopAndDestroy( shutter );

    // Delete active scheduler
    CleanupStack::PopAndDestroy( scheduler );
    }

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Executable main entry point
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    FUNC_LOG;
    __UHEAP_MARK;

    RProcess ownProcess;
    TSecureId ownUid = ownProcess.SecureId();
    INFO_1( "%S", &ownProcess.FileName() );
    ownProcess.Close();

    // Just exit if there is another process already running
    if( IsProcessRunning( ownUid ) )
        {
        INFO( "Another instance of Email Server Monitor already running!" );
        }
    else
        {
        // Rename thread for debug purposes, return code can be ignored
        RThread::RenameMe( KEmailServerMonitorName );
    
        // Create the cleanup stack
        CTrapCleanup* cleanup = NULL;
        cleanup = CTrapCleanup::New();
        if ( !cleanup )
            {
            // No errors leaked outside, return success.
            INFO( "Could not create clean up stack!" );
            return KErrNone;
            }
    
        // Run application code inside TRAP harness
        TRAPD( mainError, DoStartL() );
        
        if( mainError != KErrNone )
            {
            ERROR_1( mainError, "Email Server Monitor main error code: %d", mainError );
            }
        
        delete cleanup;
        }

    INFO( "Email Server Monitor finished!" );
    __UHEAP_MARKEND;
    return KErrNone;
    }
