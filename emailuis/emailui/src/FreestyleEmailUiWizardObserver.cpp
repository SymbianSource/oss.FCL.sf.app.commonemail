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
* Description:  Handles interaction with setup wizard implementation
*
*/


// SYSTEM INCLUDE FILES
#include "emailtrace.h"
#include <FreestyleEmailUi.rsg>
#include <aknnotewrappers.h>
#include <centralrepository.h>
#include <StringLoader.h>
//<cmail>
#include "CFSMailClient.h"
//</cmail>

// INTERNAL INCLUDE FILES
#include "FreestyleEmailUiWizardObserver.h"
#include "FreestyleEmailUiLauncherGridVisualiser.h"
#include "FreestyleEmailCenRepHandler.h"
#include "FreestyleEmailUiUtilities.h"


// CONSTANT VALUES


// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSEmailUiWizardObserver::CFSEmailUiWizardObserver
// -----------------------------------------------------------------------------
CFSEmailUiWizardObserver::CFSEmailUiWizardObserver( 
    CFreestyleEmailUiAppUi* aAppUi,
    CFSEmailUiLauncherGridVisualiser* aMainUiGridVisualiser )
    : iAppUi( aAppUi ), iMainUiGridVisualiser( aMainUiGridVisualiser ),
      iProcessComplete( ETrue ), iForegroundAfterWizardMessage( EFalse ), 
      iInformAboutParams( EFalse ), iNewMailboxEventHandlingAllowed( EFalse ),
      iNewMailboxCreated( EFalse )
    {
    FUNC_LOG;
    
    }

// -----------------------------------------------------------------------------
// CFSMailPluginManager::~CFSMailPluginManager
// -----------------------------------------------------------------------------
CFSEmailUiWizardObserver::~CFSEmailUiWizardObserver()
    {
    FUNC_LOG;
    // just in case: dismiss possible wait note
    if ( iMainUiGridVisualiser )
        {
        iMainUiGridVisualiser->SetWizardWaitnoteShown( EFalse );
        }
    
    delete iWaitNote;
    iWaitNote = NULL;
	}


// -----------------------------------------------------------------------------
// CFreestyleEmailUiWizardObserver::NewLC
// -----------------------------------------------------------------------------
CFSEmailUiWizardObserver* CFSEmailUiWizardObserver::NewLC( 
    CFreestyleEmailUiAppUi* aAppUi,
    CFSEmailUiLauncherGridVisualiser* aMainUiGridVisualiser )
    {
    FUNC_LOG;
    CFSEmailUiWizardObserver* self = 
        new (ELeave) CFSEmailUiWizardObserver( aAppUi, aMainUiGridVisualiser );
    CleanupStack:: PushL( self );
    self->ConstructL();
    return self;
    } 

// -----------------------------------------------------------------------------
// CFSEmailUiWizardObserver::NewL
// -----------------------------------------------------------------------------
CFSEmailUiWizardObserver* CFSEmailUiWizardObserver::NewL( 
    CFreestyleEmailUiAppUi* aAppUi,
    CFSEmailUiLauncherGridVisualiser* aMainUiGridVisualiser )
    {
    FUNC_LOG;
    CFSEmailUiWizardObserver* self = 
        CFSEmailUiWizardObserver::NewLC( aAppUi, aMainUiGridVisualiser );
    CleanupStack:: Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiWizardObserver::ConstructL
// -----------------------------------------------------------------------------
void CFSEmailUiWizardObserver::ConstructL()
    {
    FUNC_LOG;
    
    }

// -----------------------------------------------------------------------------
// CFSEmailUiWizardObserver::DoWizardStartupActionsL()
//
// Check if there are some params set by a setup wizard in the centrep
// location (meaning that we have been launched by a setup wizard) and
// inform FW about in that case
// -----------------------------------------------------------------------------
void CFSEmailUiWizardObserver::DoWizardStartupActionsL()
    {
    FUNC_LOG;
    
    // In this case we are not interested of the actual content of the cenrep,
    // our only interest is that is there something in the specified keys or
    // not. So to avoid unneccessary stack usage, we keep the size of
    // accountType buffer in minimum -> cenrep handler returns KErrOverflow
    // if there is something (and it doesn't fit to buffer).
    TBuf<1> accountType = KNullDesC();
    TBuf<KMaxUidName> centrepStartKey = KNullDesC();

    CFSEmailCRHandler* crHandler = iAppUi->GetCRHandler();
    if ( !crHandler )
        {
        return;
        }
    
    TInt accountTypeError = crHandler->SetupWizAccountType( accountType );
    TInt startKeyError = crHandler->SetupWizCentrepStartKey( centrepStartKey );
    


    // IF accountType had some content and it didn't fit to buffer OR
    //    accountType had some content and it fit to buffer
    // ==> there was something in accountType
    // AND
    // IF startKey had some content and it didn't fit to buffer OR
    //    startKey had some content and it fit to buffer
    // ==> there was something in startKey
    if ( ( accountTypeError == KErrOverflow ||
           ( accountTypeError == KErrNone && accountType.Length() > 0 ) ) &&
         ( startKeyError == KErrOverflow ||
           ( startKeyError == KErrNone && centrepStartKey.Length() > 0 ) ) )
        {
        
        // Launches the "Creating mailbox" wait note - 
        // execution continues in the method StepL() after that.
        StartWaitNoteL();
        }
    else
        {
        }
    
    }

void CFSEmailUiWizardObserver::HandleMessage( TUint32 /*aClientHandleOfTargetWindowGroup*/,
                                              TUid /*aMessageUid*/,
                                              const TDesC8& aMessageParameters )
    {
    FUNC_LOG;
    
    if ( aMessageParameters.Compare( KNullDesC8 ) == 0 )
        {
        
        TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
        TApaTask task = taskList.FindApp( KFSEmailUiUid );
        if ( task.Exists() )
            {
            if ( !iAppUi->IsForeground() )
                {
                // Bring Email UI to foreground
                
                iForegroundAfterWizardMessage = ETrue;
                task.BringToForeground();
                }
            else
                {
                // Launches the "Creating mailbox" wait note - 
                // execution continues in the method StepL() after that.
                TRAPD( err, StartWaitNoteL() );
                if ( err != KErrNone )
                	{
                	}
                }
            }
        }
    
    }

void CFSEmailUiWizardObserver::HandleForegroundEventL( TBool aForeground )
    {
    FUNC_LOG;
        
    // iForegroundAfterWizardMessage flag is turned into ETrue when we have received 
    // a KNullDesC message from mailbox setup wizard in HandleMessage() and if UI is
    // brought to foreground after that.
    if ( iForegroundAfterWizardMessage && aForeground )
        {
        iForegroundAfterWizardMessage = EFalse;
        
        // launches the "Creating mailbox" wait note - 
        // execution continues in the method StepL() after that.
        StartWaitNoteL();
        }
        
    }

void CFSEmailUiWizardObserver::StartWaitNoteL()
    {
    FUNC_LOG;
    
    // This wait note will be dismissed in callback method EventL()
    iProcessComplete = EFalse;
    iInformAboutParams = ETrue;
    iMainUiGridVisualiser->SetWizardWaitnoteShown( ETrue );
    
    TRAPD( waitNoteError,
        {
        if ( !iWaitNote )
            {
            iWaitNote = CAknWaitNoteWrapper::NewL();
            }
        
        iWaitNote->ExecuteL( R_FSE_CREATING_MAILBOX_DIALOG, *this );
        // after ExecuteL the execution continues in StepL()
        } );
    
    if ( waitNoteError != KErrNone )
        {
        
        // Only reason for the wait note showing to fail should be if we run
        // out of memory, so there's not much we can do, except give up. So
        // first reset our internal variables and clear the cenrep data so
        // that the user data is not left there.
        iProcessComplete = ETrue;
        iInformAboutParams = EFalse;
        iMainUiGridVisualiser->SetWizardWaitnoteShown( EFalse );

        iAppUi->GetCRHandler()->ClearWizardParams();
        
        // Our own error note showing is probably not working either,
        // but let's try it anyway
        TRAPD( errorNoteError,
            {
            TFsEmailUiUtility::ShowGlobalErrorNoteL( R_FREESTYLE_EMAIL_UI_LOGIN_INFO_NOTE_ERROR_MAILBOX_SETUP_TRY_AGAIN );
            } );

        // If our own error note showing failed, then we leave and let
        // the system show it's own error note
        if ( errorNoteError != KErrNone )
            {
            User::Leave( waitNoteError );
            }
        }
    
    }

void CFSEmailUiWizardObserver::InformFrameworkAboutParamsL()
    {
    FUNC_LOG;
    
    TInt mailboxConfigError( KErrNone );
    // tell framework - and thus the plugin - to check the parameters from centrep
    TRAPD( wDataAvailableError,
        {
        mailboxConfigError = iAppUi->GetMailClient()->WizardDataAvailableL();
        } );
    
    // clears the two centrep keys that were passed as parameters to plugins
    iAppUi->GetCRHandler()->ClearWizardParams();
    
    // The error filtering logic was moved here from CFSMailClient::WizardDataAvailableL.
    // if an error is received, some called plugin didn't support the mailbox 
    // creation process, or the parameters couldn't be read from central repository.
    if ( mailboxConfigError != KErrNone || wDataAvailableError != KErrNone )
        {
        
        // dismiss the wait note
        iProcessComplete = ETrue;
        iMainUiGridVisualiser->SetWizardWaitnoteShown( EFalse );
        
        // show an error note
        TRAP_IGNORE(
            {
            TFsEmailUiUtility::ShowGlobalErrorNoteL( R_FREESTYLE_EMAIL_UI_LOGIN_INFO_NOTE_ERROR_MAILBOX_SETUP_TRY_AGAIN );
            });
        }
    else
        {
        iNewMailboxEventHandlingAllowed = ETrue;
        }
    
    }

void CFSEmailUiWizardObserver::EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox, 
                                       TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    FUNC_LOG;
    
    switch ( aEvent )
    	{
    	case TFSEventNewMailbox:
    		{
            // update grid view now when we have new mailbox created
            iMainUiGridVisualiser->RefreshLauncherViewL();
    		
            if ( iNewMailboxEventHandlingAllowed )
                {
                iNewMailboxEventHandlingAllowed = EFalse; //reset the boolean
                
                // dismiss the wait note
                iProcessComplete = ETrue;
                iMainUiGridVisualiser->SetWizardWaitnoteShown( EFalse );
                        
                // clears the two centrep keys that were passed as parameters to plugins
                iAppUi->GetCRHandler()->ClearWizardParams();
                
                // <cmail>
                // new mailbox created -> open its Inbox folder after the 
                // wait dialog is dismissed -> continued in 
                // DialogDismissedL method.
                // This code needed to be moved because Wait dialog is 
                // still open and prevents the active view from receiving 
                // events. That's why the grid view was not deactivated. 
                iNewMailboxCreated = ETrue;
                iNewlyCreatedMailboxId = aMailbox;
                // </cmail>
                }
            else
                {
                }
            }
    		break;
   		
   		case TFSEventMailboxCreationFailure:
    		{
    		
	         // dismiss the wait note
	        iProcessComplete = ETrue;
	        iMainUiGridVisualiser->SetWizardWaitnoteShown( EFalse );
	        
	        // clears the two centrep keys that were passed as parameters to plugins
	        iAppUi->GetCRHandler()->ClearWizardParams();
	        
	        TRAP_IGNORE(
	            {
	            TFsEmailUiUtility::ShowGlobalErrorNoteL( R_FREESTYLE_EMAIL_UI_LOGIN_INFO_NOTE_ERROR_MAILBOX_SETUP_TRY_AGAIN );
	            });
	        }
    		break;
   		}           
    
    }

//
// MAknBackgroundProcess methods
//
void CFSEmailUiWizardObserver::StepL()
    {
    FUNC_LOG;
    // Tell framework that there are parameters in the centrep 
    // with which it can create a new mail account.
    if ( iInformAboutParams )
        {
        iInformAboutParams = EFalse;  //reset the value
        InformFrameworkAboutParamsL();
        }
    }

TBool CFSEmailUiWizardObserver::IsProcessDone() const
    {
    FUNC_LOG;
    return iProcessComplete;
    }

void CFSEmailUiWizardObserver::ProcessFinished()
    {
    FUNC_LOG;
    // nothing here
    }

void CFSEmailUiWizardObserver::DialogDismissedL( TInt aButtonId )
    {
    FUNC_LOG;
    // <cmail>
    if ( iNewMailboxCreated ) 
        {
        // new mailbox created -> open its Inbox folder if Cancel button 
        // not pressed
        iNewMailboxCreated = EFalse;
        if ( aButtonId != EAknSoftkeyCancel )
            {
            CFSMailBox* mailBox = iAppUi->GetMailClient()->GetMailBoxByUidL( 
                iNewlyCreatedMailboxId );
            CleanupStack::PushL( mailBox );
            if ( mailBox )
                {
                TFSMailMsgId inboxFolderId = mailBox->GetStandardFolderId( 
                        EFSInbox );
            
                // launch the inbox of the new mailbox
                TMailListActivationData tmp;    
                tmp.iFolderId = inboxFolderId; 
                tmp.iMailBoxId = iNewlyCreatedMailboxId; 
                const TPckgBuf<TMailListActivationData> pkgOut( tmp );
                iAppUi->EnterFsEmailViewL( MailListId, KStartListWithFolderId, 
                        pkgOut );
                // the App might have been, say, in Attachments list of some 
                // other mailbox but we should not return there with 'Back' 
                // button
                iAppUi->EraseViewHistory();
                }
                
            CleanupStack::PopAndDestroy( mailBox );
            }
        }
    // </cmail>
    }

TInt CFSEmailUiWizardObserver::CycleError( TInt /*aError*/ )
    {
    FUNC_LOG;
    return KErrNone;
    }

// End of file

