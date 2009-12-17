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
* Implementation of Email Installation Initiator
*
*/

#include <apacmdln.h>
#include <apgcli.h>
#include <e32property.h>                // RProperty

#include "emailtrace.h"
#include "emailshutdownconst.h"
#include "emailinstallationinitiator.h"
#include "emailinstallationinitiatorconst.h"

// Format string for process finder to find processes by executable name
_LIT( KFormatProcessFinder, "%S*" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased class constructor.
// ---------------------------------------------------------------------------
//
CEmailInstallationInitiator* CEmailInstallationInitiator::NewL()
    {
    FUNC_LOG;
    CEmailInstallationInitiator* self = CEmailInstallationInitiator::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased class constructor, leaves newly created object in cleanup stack.
// ---------------------------------------------------------------------------
//
CEmailInstallationInitiator* CEmailInstallationInitiator::NewLC()
    {
    FUNC_LOG;
    CEmailInstallationInitiator* self = new(ELeave) CEmailInstallationInitiator();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CEmailInstallationInitiator::CEmailInstallationInitiator()
    : CActive( EPriorityStandard )
    {
    FUNC_LOG;
    }

CEmailInstallationInitiator::~CEmailInstallationInitiator()
    {
    FUNC_LOG;
    
    Cancel();
    iApaLsSession.Close();
    }

//----------------------------------------------------------------------//
// 2nd pahse constructor, start active scheduler and connect to
// application architecture server
//----------------------------------------------------------------------//
void CEmailInstallationInitiator::ConstructL()
    {
    FUNC_LOG;
    
    // Connect to application architecture server
    User::LeaveIfError( iApaLsSession.Connect() );
    INFO( "Created and connected to RApaLsSession" );
    
    User::LeaveIfError( iProperty.Attach( KEmailShutdownPsCategory,
                                          EEmailPsKeyInstallationStatus ));
    INFO( "Attached to RProperty" );

    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Starts observing P&S shutdown event
// ---------------------------------------------------------------------------
//
void CEmailInstallationInitiator::StartObservingShutdownEvent()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// RunL of active object
// ---------------------------------------------------------------------------
//
void CEmailInstallationInitiator::RunL()
    {
    FUNC_LOG;
    if( iStatus == KErrNone )
        {
        TInt value( 0 );
        TInt error = iProperty.Get( value );
        ERROR_1( error, "RProperty::Get error code: %d", error );
        
        // Verify that the P&S value indicates that installation can continue
        if( error == KErrNone &&
            value == EEmailPsValueInstallationOkToStart )
            {
            // Wait a bit so that Email Server Monitor really exits
            Wait();
            
            // Change the P&S key value as the installation starts once we are done
            error = iProperty.Set( EEmailPsValueInstallationStarted );
            ERROR_1( error, "RProperty::Set error code: %d", error );
    
            // Shutdown ourselves, so that installation may continue
            CActiveScheduler::Stop();
            }
        else
            {
            // Re-subscribe, if this was not the event we were waiting for
            StartObservingShutdownEvent();
            }
        }
    else
        {
        ERROR_1( iStatus.Int(), "CEmailShutter::RunL error code: %d", iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// Cancel active object
// ---------------------------------------------------------------------------
//
void CEmailInstallationInitiator::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// Starts all needed client applications
// -----------------------------------------------------------------------------
//
void CEmailInstallationInitiator::StartEmailServerMonitor()
    {
    FUNC_LOG;

    TRAPD( error, StartOneApplicationL( KEmailShutdownHandlerExe() ) );
    if( error != KErrNone )
        {
        ERROR_1( error, "EmailServerMonitor start failed, error code: %d", error );
        }
    }

// -----------------------------------------------------------------------------
// Starts one application by the executable name given as parameter
// -----------------------------------------------------------------------------
//
void CEmailInstallationInitiator::StartOneApplicationL( const TDesC& aAppName )
    {
    FUNC_LOG;
    
    // Start the application only if it's not already running
    if( !IsProcessRunningL( aAppName ) )
        {
        RProcess process;
        TInt err = process.Create( aAppName, KEmailShutdownHandlerArgOnlyShutter );
        TRequestStatus stat = KRequestPending;
        process.Rendezvous( stat );
        
        if( stat != KRequestPending )
            {            
            process.Kill( KErrNone );
            }
        else
            {            
            process.Resume();

            User::WaitForRequest( stat );
            }
            
        process.Close();

        // Wait a bit after starting process to make sure it's fully running.
        // This is here just in case, at least while we don't have multi core
        // processors we are ready to continue immediately.
        Wait();
        }
    }

// -----------------------------------------------------------------------------
// Checks if process with the specified name is running
// -----------------------------------------------------------------------------
TBool CEmailInstallationInitiator::IsProcessRunningL( const TDesC& aAppName )
    {
    TFullName fullName;
    
    HBufC* findBuf = HBufC::NewLC( aAppName.Length() + KFormatProcessFinder().Length() );
    findBuf->Des().Format( KFormatProcessFinder, &aAppName );

    TFindProcess findProcess( *findBuf );

    TBool found = EFalse;
    // Loop through all the found processes to check is any of those running
    while( !found && findProcess.Next( fullName ) == KErrNone )
        {
        RProcess process;
        TInt error = process.Open( findProcess );
        // Check is the found process alive
        if( error == KErrNone &&
            process.ExitType() == EExitPending )
            {
            found = ETrue;
            }
        process.Close();
        }

    CleanupStack::PopAndDestroy( findBuf );

    return found;
    }

// ---------------------------------------------------------------------------
// Send installation start event to Email Server Monitor / Shutter
// ---------------------------------------------------------------------------
//
void CEmailInstallationInitiator::SendInstallationEventAndStartObserving()
    {
    FUNC_LOG;
    TInt error = iProperty.Set( EEmailPsValueInstallationStarting );
    ERROR_1( error, "RProperty::Set error code: %d", error );
    
    StartObservingShutdownEvent();
    }


void CEmailInstallationInitiator::Wait( TInt aWaitTimeInSeconds /*= -1*/ )
    {
    if( aWaitTimeInSeconds <= 0 )
        {
        aWaitTimeInSeconds = KDefaultTimeToWaitInSeconds;
        }
    
    User::After( aWaitTimeInSeconds * KEmailInitiatorOneSecond );
    }

// ---------------------------------------------------------------------------
// Starts active scheduler, runs the needed operations, and then deletes
// the active scheduler when everything is done
// ---------------------------------------------------------------------------
//
LOCAL_C void DoStartL()
    {
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    
    CEmailInstallationInitiator* initiator = CEmailInstallationInitiator::NewLC();

    // Make sure that Email Server Monitor is running
    initiator->StartEmailServerMonitor();
    
    initiator->SendInstallationEventAndStartObserving();

    CActiveScheduler::Start();

    // Delete Installation Initiator
    CleanupStack::PopAndDestroy( initiator );

    // Delete active scheduler
    CleanupStack::PopAndDestroy( scheduler );
    }

//Main Thread Entry point
GLDEF_C TInt E32Main()
    {
    FUNC_LOG;
    __UHEAP_MARK;

    // Rename thread for debug purposes, return code can be ignored
    RThread::RenameMe( KEmailInstallatioInitiatorName );

    // Create the cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
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
        ERROR_1( mainError, "Email Installation Initiator main error code: %d", mainError );
        }

    delete cleanup;

    INFO( "Email Installation Initiator finished!" );

    __UHEAP_MARKEND;
    return KErrNone; 
    } // end E32Main
