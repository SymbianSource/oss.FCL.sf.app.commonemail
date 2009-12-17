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
* Description:  Implementation of FSMailServer startup and other basic 
*                functionality
*
*/


#include "emailtrace.h"
#include <e32base.h>
#include <eikstart.h>
#include <eikenv.h>
#include <bautils.h>

#include "fsmailserverdefinitions.h"
#include "FsEmailGlobalDialogsApplication.h"

_LIT( KFSMailServerName, "FSMailServer" );

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Checks if the process is already running
// ---------------------------------------------------------------------------
//
LOCAL_C TBool IsMailServerRunning()
    {
    TBool oneIsRunning( EFalse );
    
    TFileName fileName;
    TFindProcess finder;
    while ( finder.Next( fileName ) == KErrNone )
        {
        RProcess process;
        TInt outcome = process.Open( fileName );
        if ( outcome != KErrNone )
            {
            continue;
            }
        
        // Check is this mail server process and is it running
        if ( process.SecureId() == KFSMailServerUid && 
             process.ExitType() == EExitPending )
            {
            if ( oneIsRunning )
                {
                // Found second process with the same sid. This
                // means that one other process besides this process
                // with the same sid is already running.
                // So one instance of fsmailserver is already running.
                process.Close();
                return ETrue;
                }
            else
                {
                oneIsRunning = ETrue;
                }
            }
        
        process.Close();
        }
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Create new FSMailServer application, called by UI framework
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    FUNC_LOG;
    return new CFsEmailGlobalDialogsApplication;
    }

// ---------------------------------------------------------------------------
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {

    
    TInt error = KErrNone;
    
    // First check that the server isn't already running
    if ( IsMailServerRunning() )
        {
        error = KErrAlreadyExists;
        }
    
    // Start application server if we aren't already running
    if( error == KErrNone )
        {
        // Rename to help identification in error situations
        error = User::RenameThread( KFSMailServerName );
        
        if( error != KErrNone )
            {
            // Keep on going even if thread renaming fails
        }
        error = EikStart::RunApplication( NewApplication );
        }
    
    return error;
    }

