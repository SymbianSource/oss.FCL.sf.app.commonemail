/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSMailServerAutoStart.
*
*/


#include "emailtrace.h"
#include "emailshutdownconst.h"
#include "fsmailserverautostart.h"

_LIT( KFSMailServerAutoStartName, "FSMailServerAutoStart" );

// Format string for process finder to find processes by executable name
_LIT( KFormatProcessFinder, "%S*" );

// ======== MEMBER FUNCTIONS ========

CFSMailServerAutoStart* CFSMailServerAutoStart::NewL()
    {
    FUNC_LOG;
    CFSMailServerAutoStart *self = new(ELeave) CFSMailServerAutoStart();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CFSMailServerAutoStart::CFSMailServerAutoStart()
    {
    FUNC_LOG;
    }

//----------------------------------------------------------------------//
// Currently is chosen that no errors are leaked out of this class
// if possible.
// In case of failure in timer construction this might not be possible,
// but that should be improbable.
//----------------------------------------------------------------------//
void CFSMailServerAutoStart::ConstructL()
    {
    FUNC_LOG;

    // start fs mail server
    TRAPD( startError, StartOneApplicationL( KEmailShutdownHandlerExe() ) );
    if ( startError != KErrNone )
        {
        ERROR_1( startError, "EmailServerMonitor start failed, error code: %d", startError );
        }
    }

// -----------------------------------------------------------------------------
// Starts one application by the executable name given as parameter
// -----------------------------------------------------------------------------
//
void CFSMailServerAutoStart::StartOneApplicationL( const TDesC& aAppName ) const
    {
    FUNC_LOG;
    
    // Start the application only if it's not already running
    if( !IsProcessRunningL( aAppName ) )
        {
        RProcess process;
        TInt err = process.Create( aAppName, KEmailShutdownHandlerArgRestart );
        
        if( err == KErrNone )
            {
            TRequestStatus stat = KRequestPending;
            process.Rendezvous( stat );
            
            if( stat != KRequestPending )
                {            
                ERROR_1( stat.Int(), "RProcess::Rendezvous failed, error code: %d", stat.Int() );
                process.Kill( KErrNone );
                }
            else
                {            
                process.Resume();
    
                User::WaitForRequest( stat );
                }
            }
        else
            {
            ERROR_1( err, "RProcess::Create failed, error code: %d", err );
            }
            
        process.Close();
        }
    }

// -----------------------------------------------------------------------------
// Checks if process with the specified name is running
// -----------------------------------------------------------------------------
TBool CFSMailServerAutoStart::IsProcessRunningL( const TDesC& aAppName ) const
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


//Main Thread Entry point
GLDEF_C TInt E32Main()
    {
    RThread::RenameMe( KFSMailServerAutoStartName );

    // Create the cleanup stack
    CTrapCleanup* cleanup = NULL;
    cleanup = CTrapCleanup::New();
    if ( cleanup == NULL )
        {
        // No errors leaked outside, return success.
        return KErrNone;
        }
    
    CFSMailServerAutoStart* autoStart = NULL;

    TRAPD( mainError, autoStart = CFSMailServerAutoStart::NewL() );
    if ( mainError != KErrNone )
        {
        ERROR_1( mainError, "EmailServerMonitor start failed, error code: %d", mainError );
        }
    
    // Delete all objects created above.	
    delete autoStart;
    delete cleanup;

    return KErrNone; 
    } // end E32Main
