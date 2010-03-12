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
* Implementation of EmailServerMonitor utility functions
*
*/
//  Include Files  

#include <e32base.h>
#include <e32std.h>
#include <e32des16.h>                   // Descriptors

#include "emailtrace.h"
#include "emailservermonitorutilities.h"

// One second as micro seconds
const TInt KEmailServerMonitorOneSecond = 1000000;

// ---------------------------------------------------------------------------
// Wait some time to give other applications and servers some time to shut
// down themselves gracefully
// ---------------------------------------------------------------------------
//
void Wait( TInt aWaitTimeInSeconds /*= KDefaultTimeToWaitInSeconds*/ )
    {
    FUNC_LOG;
    User::After( aWaitTimeInSeconds * KEmailServerMonitorOneSecond );
    }

// ---------------------------------------------------------------------------
// Checks if the process is already running
// ---------------------------------------------------------------------------
//
TBool IsProcessRunning( TSecureId aUid, RProcess* aProcess /* = NULL */ )
    {
    FUNC_LOG;
    
    // Get our own UID, if we are checking wheter our own process is running,
    // we are basically checking is there some other instance already running.
    // So it's allowed to have one process running (ourselves), but not more.
    // In normal case already one instance is counted as running.
    TInt numberOfInstancesRunning = 0;
    TInt numberOfInstancesAllowed = 0;
    RProcess ownProcess;
    if( ownProcess.SecureId() == aUid )
        {
        numberOfInstancesAllowed++;
        }
    ownProcess.Close();
    
    TFileName fileName;
    TFindProcess finder;
    while ( finder.Next( fileName ) == KErrNone )
        {
        RProcess process;
        TInt outcome = process.Open( fileName );
        if ( outcome != KErrNone )
            {
            INFO( "Could not open process. Checking next one." );
            continue;
            }
        
        // Check is this the searched process and is the process running
        if ( process.SecureId() == aUid && 
             process.ExitType() == EExitPending )
            {
            numberOfInstancesRunning++;

            if ( numberOfInstancesRunning > numberOfInstancesAllowed )
                {
                // We either found second instance of ourselves or first
                // instance of other searched process
                process.Close();
                // If process handle given, update it with found process
                if( aProcess )
                    {
                    aProcess->Close();
                    TInt error = aProcess->Open( fileName );
                    // If process handle was given, it's assumed to be
                    // up-to-date if we return success, so we need to return
                    // failure if we can't reopen any of the found process'.
                    // This should be very very rare case.
                    if( error != KErrNone )
                        {
                        continue;
                        }
                    }
                return ETrue;
                }
            }
        
        process.Close();
        }
    
    return EFalse;
    }


/////////////////////////////////////////////////////////////////////////////////
// GENERIC TIMER
//

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
CEmailServerMonitorTimer* CEmailServerMonitorTimer::NewL(
    MEmailServerMonitorTimerCallback* aCallback,
    const TInt aPriority )
    {
    FUNC_LOG;
    CEmailServerMonitorTimer* self = NewLC( aCallback, aPriority );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
CEmailServerMonitorTimer* CEmailServerMonitorTimer::NewLC(
    MEmailServerMonitorTimerCallback* aCallback,
    const TInt aPriority )
    {
    FUNC_LOG;
    CEmailServerMonitorTimer* self = new (ELeave) CEmailServerMonitorTimer( aCallback, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
void CEmailServerMonitorTimer::ConstructL()
    {
    FUNC_LOG;
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::~CEmailServerMonitorTimer
// D'tor
// -----------------------------------------------------------------------------
//
CEmailServerMonitorTimer::~CEmailServerMonitorTimer()
    {
    FUNC_LOG;
    Cancel();
    iCallback = NULL;
    }

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::CEmailServerMonitorTimer
// C'tor
// -----------------------------------------------------------------------------
//
CEmailServerMonitorTimer::CEmailServerMonitorTimer(
    MEmailServerMonitorTimerCallback* aCallback,
    const TInt aPriority )
    : CTimer( aPriority ),
    iCallback( aCallback )
    {
    }

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::RunL
// Timer trigger function.
// -----------------------------------------------------------------------------
//
void CEmailServerMonitorTimer::RunL()
    {
    FUNC_LOG;
    if ( iCallback )
        {
        iCallback->TimerEventL( this );
        }
    }

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::Start
// Timer starting function.
// -----------------------------------------------------------------------------
//
void CEmailServerMonitorTimer::Start( TInt aInterval )
    {
    FUNC_LOG;
    Cancel();
    After( TTimeIntervalMicroSeconds32( aInterval ) );
    }

// -----------------------------------------------------------------------------
// CEmailServerMonitorTimer::Stop
// Timer stopping function
// -----------------------------------------------------------------------------
//
void CEmailServerMonitorTimer::Stop()
    {
    FUNC_LOG;
    Cancel();
    }
