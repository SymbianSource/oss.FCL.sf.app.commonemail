/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html". 
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Source file for FSMailServer global dialogs app ui
*
*/


#include "emailtrace.h"
#include "fsmailserverdefinitions.h"
#include "fsnotificationhandlermgrimpl.h"
#include "FsEmailGlobalDialogsAppUi.h"
#include "emailservershutdownobserver.h"


// To be used as FSMailServer window group priority when there is some 
// displayable content. We need to keep FSMailServer always on top, because it
// is hided from task list so it's not possible switch FSMailServer back to
// foreground if user switches to Idle before dismissing the note.
// '- 1' is needed for the VKB to work correctly
const TInt KFsEmailDialogsWinPriorityActive = ECoeWinPriorityAlwaysAtFront - 1;

// To be used as FSMailServer window group priority when there is no displayable
// content.
const TInt KFsEmailDialogsWinPriorityInactive = ECoeWinPriorityNeverAtFront;

const TInt KPosBringToFront = 0;
const TInt KPosSendToBack = -1;

const TUid KFSMailServerUidAsTUid = { KFSMailServerUid };

const TInt KDefaultArrayGranularity = 5;


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CFsEmailGlobalDialogsAppUi::CFsEmailGlobalDialogsAppUi()
    {
    FUNC_LOG;
    // We are not showing any full screen content, only popup dialogs
    SetFullScreenApp( EFalse );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFsEmailGlobalDialogsAppUi::~CFsEmailGlobalDialogsAppUi()
    {
    FUNC_LOG;
    delete iShutdownObserver;
    iShutdownObserver = NULL;
    }

// ---------------------------------------------------------------------------
// Overriden CAknAppUi::HandleWsEventL
// ---------------------------------------------------------------------------
//
void CFsEmailGlobalDialogsAppUi::HandleWsEventL( const TWsEvent& aEvent,
                                                 CCoeControl* aDestination )
    {
    FUNC_LOG;
    // Updates the foreground flag
    CAknAppUi::HandleWsEventL( aEvent, aDestination );
    
    RWindowGroup& rwin = iEikonEnv->RootWin();
    
    TBool isActive =
        ( rwin.OrdinalPriority() == KFsEmailDialogsWinPriorityActive );
    
    if ( aEvent.Type() == EEventWindowGroupListChanged && isActive )
        {
        RWsSession& ws = iEikonEnv->WsSession();
        
        CArrayFixFlat<TInt>* wgList =
            new (ELeave) CArrayFixFlat<TInt>( KDefaultArrayGranularity );

        TInt err = ws.WindowGroupList(
            KFsEmailDialogsWinPriorityActive, wgList );
        
        if ( err == KErrNone )
            {
            TBool topmost = ( wgList->Count() > 0 ) &&
                            ( wgList->At( 0 ) == rwin.Identifier() );
            
            if ( iForeground && topmost && iForegroundNotTopmost )
                {
                // This ensures the screen is refreshed when going back
                // from the VKB window
                SendToBackground();
                BringToForeground();
                }
            
            // Update this flag all the time when we are active
            // The flag is true when VKB window is shown for the password
            // query.
            iForegroundNotTopmost = ( iForeground && !topmost );
            }
        
        delete wgList;
        wgList = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Overriden CAknAppUi::Exit
// ---------------------------------------------------------------------------
//
void CFsEmailGlobalDialogsAppUi::HandleForegroundEventL( TBool aForeground )
    {
    FUNC_LOG;
    iForeground = aForeground;
    }

// ---------------------------------------------------------------------------
// Overriden CAknAppUi::Exit
// ---------------------------------------------------------------------------
//
void CFsEmailGlobalDialogsAppUi::Exit()
    {
    FUNC_LOG;
    // Cancel shutdown observer as we are already shutting down
    if( iShutdownObserver )
        {
        iShutdownObserver->Cancel();
        }
    
    // We need to delete notification handler already here, because mail
    // client destructor might need some AppUi resources like CleanupStack
    delete iNotificationHandlerMgr;
    iNotificationHandlerMgr = NULL;
    
	// Call base class Exit
    CAknAppUi::Exit();
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CFsEmailGlobalDialogsAppUi::ConstructL()
    {
    FUNC_LOG;
    // For debug builds create log file directory automatically. Best way
    // would be to do this already when server is started, but because
    // EikonEnv's FsSession is not yet ready there, we can't do it until now.
#ifdef _DEBUG
    EnsureDebugDirExists();
#endif

    
    BaseConstructL( ENonStandardResourceFile|ENoScreenFurniture|EAknEnableSkin );
    
    // Hide application from fast swap window, and sent it to background
    HideApplicationFromFSW();
    SendToBackground();
    
    // Set Srv application to be system application. 
    // System applications can't be closed by the user.
    iEikonEnv->SetSystem( ETrue );
    
    // Create notification handler, which implements actual functionality
    iNotificationHandlerMgr = CFSNotificationHandlerMgr::NewL( this );

    // Create shutdown watcher, which will close the server gracefully
    // when it gets event indicating that sis-update is starting
    TRAP_IGNORE
        (
        // Errors can be ignored, as there's no point to shutdown whole server
        // just because shutdown watcher is not working. Sis-updates are
        // anyway happening quite rarely, and the shutter exe will anyway
        // kill all running processes if they don't respond to shutdown event.
        iShutdownObserver = CEmailServerShutdownObserver::NewL( *this );
        );
    
    }

// ---------------------------------------------------------------------------
// Brings the FSMailServer to the foreground so notes can be displayed.
// ---------------------------------------------------------------------------
//
void CFsEmailGlobalDialogsAppUi::BringToForeground()
    {
    FUNC_LOG;

    
    // Bring own application to foreground
    TApaTaskList taskList( iEikonEnv->WsSession() );
    TApaTask task = taskList.FindApp( KFSMailServerUidAsTUid );
    if ( task.Exists() )
        {
        task.BringToForeground();
        }
    
    // Bring window group to foreground
    iEikonEnv->BringOwnerToFront();
    iEikonEnv->RootWin().SetOrdinalPosition( 
        KPosBringToFront,
        KFsEmailDialogsWinPriorityActive );

    iEikonEnv->RootWin().EnableGroupListChangeEvents();
	
    // Enable keyboard focus when showing some content
    iEikonEnv->RootWin().EnableReceiptOfFocus( ETrue );	
    }

// ---------------------------------------------------------------------------
// Sends FSMailServer to the background when notes are dismissed.
// ---------------------------------------------------------------------------
//
void CFsEmailGlobalDialogsAppUi::SendToBackground()
    {
    FUNC_LOG;
    
    // Send own application to background
    TApaTaskList taskList( iEikonEnv->WsSession() );
    TApaTask task = taskList.FindApp( KFSMailServerUidAsTUid );
    if ( task.Exists() )
        {
        task.SendToBackground();
        }
    
    // Send window group to background
    iEikonEnv->RootWin().SetOrdinalPosition( 
        KPosSendToBack,
        KFsEmailDialogsWinPriorityInactive );
    
    // Disable keyboard focus when not showing any content
    iEikonEnv->RootWin().EnableReceiptOfFocus( EFalse );
    
    iEikonEnv->RootWin().DisableGroupListChangeEvents();
    }

#ifdef _DEBUG
// ---------------------------------------------------------------------------
// Creates debug logging directory automatically if it doesn't exist already
// ---------------------------------------------------------------------------
//
void CFsEmailGlobalDialogsAppUi::EnsureDebugDirExists()
    {
    FUNC_LOG;
    // OK to have hard-coded drive letter (despite of CodeScanner warning)
    // as we want the debug logs to go always to phone memory
    _LIT( KDebugLogDir, "c:\\logs\\debuglogger\\" );

    // Use MkDirAll instead of BaflUtils::EnsurePathExistsL so that we know
    // wheter the folder already existed or was it just created. This
    // might make a difference when reading the debug log as there are
    // some traces already before this point.
    TInt error=iEikonEnv->FsSession().MkDirAll( KDebugLogDir );


    // If the directory didn't exist before MkDirAll, KErrNone is returned.
    // If it existed before MkDirAll, KErrAlreadyExist is returned.
    if( error == KErrNone )
        {
        // Inform that it's OK that there are no logging before this point.
        // All other error codes can be ignored.
        }

    }
#endif

// End of File

