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
* Description:  Class to handle mail led turning on.
*
*/


//<cmail>
#include "emailtrace.h"
#include "CFSMailClient.h"
//</cmail>
#include <e32base.h> // needed because of the following hrh
#include <data_caging_path_literals.hrh> // KDC_MTM_INFO_FILE_DIR
#include <mtclreg.h>
#include <w32std.h>
#include <apgtask.h>
//<cmail>
#include "cfsclientmtm.h"
#include "fsmtmsconstants.h"
//</cmail>
#include <coemain.h>

#include "fsmailmtmhandler.h"


// FREESTYLE EMAIL FRAMEWORK INCLUDES
//<cmail>
#include "CFSMailClient.h"
#include "MFSMailBrandManager.h"
//</cmail>


#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

_LIT( KMessageServerProcessIdentifier, "MSEXE*" );


CFSMailMtmHandler* CFSMailMtmHandler::NewL(
    MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
    CFSMailMtmHandler* self =
        new (ELeave) CFSMailMtmHandler( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CFSMailMtmHandler::CFSMailMtmHandler(
    MFSNotificationHandlerMgr& aOwner ) :
    CFSNotificationHandlerBase( aOwner ),
    iStartingUp( ETrue )
    {
    FUNC_LOG;
    }

void CFSMailMtmHandler::ConstructL()
    {
    FUNC_LOG;
    CFSNotificationHandlerBase::ConstructL();
    
    TRAPD( installError, InstallMtmL() );
    if ( installError != KErrNone )
        {
        User::Leave( installError );
        }
    
    CFSMailClient* mail = CFSMailClient::NewL();

    RPointerArray<CFSMailBox> mailBoxes;
    mailBoxes.Reset();

    TFSMailMsgId plugin;
    mail->ListMailBoxes(plugin,mailBoxes);
    
    RArray<TFSMailMsgId> mailBoxIds;
    
    for(int i=0; i<mailBoxes.Count(); i++)
    	{
    	TDesC& name( mailBoxes[i]->GetName() );
    	TDesC& ownAddress( mailBoxes[i]->OwnMailAddress().GetEmailAddress() );
    	TRAPD(createError, iMtmClient->CreateAccountL( name, ownAddress, mailBoxes[i]->GetId() ));
    	if(createError != KErrNone)
    		{
    		// Error
    		} 	
    	mailBoxIds.Append(mailBoxes[i]->GetId());
    	}
    // Remove rendundant accounts
    TRAPD(removeRedundantError, iMtmClient->RemoveRedundantAccountsL( mailBoxIds ));
    if(removeRedundantError != KErrNone)
        {
        }
    
    
    mailBoxIds.Close();
    mailBoxes.ResetAndDestroy();
    mailBoxes.Close();

    mail->Close();
    
    }

CFSMailMtmHandler::~CFSMailMtmHandler()
    {
    FUNC_LOG;
    UninstallMtm();
    }

// ---------------------------------------------------------------------------
// This is here just to absorb callbacks.
// ---------------------------------------------------------------------------
//
void CFSMailMtmHandler::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* /*aArg1*/,
    TAny* aArg2,
    TAny* /*aArg3*/)
    {
    FUNC_LOG;
    
    // Just being really really sure that the mtm client is created only
    // when we are in the start up phase.
    if ( iStartingUp )
        {
        // Normally this would have been done after the registry creation.
        // If NewMtmL is called immediately after registry creation,
        // it seems to cause panic when active scheduler is started.
        // This seems to be because the previous DeInstallMtmGroup and
        // InstallMtmGroup get done only after active scheduler is started.
        // If they are done at that point, the CFsMtmClient is already
        // created and the DeInstallMtmGroup seem to cause panic.
        // The panic is avoided by first waiting for that the deinstall and
        // install are done before creating the CFsMtmClient.
        if ( aEvent == EMsvMtmGroupInstalled &&
             KUidMsgValTypeFsMtmVal == *static_cast<TUid*>(aArg2) )
            {
            }
        }
    }

TBool CFSMailMtmHandler::CapabilitiesToContinueL(
    TFSMailEvent /*aEvent*/,
    TFSMailMsgId aMailbox,
    TAny* /*aParam1*/,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ ) const
    {
    FUNC_LOG;
    CFSMailBox* mailBox( MailClient().GetMailBoxByUidL( aMailbox ) );
    if ( mailBox == NULL )
        {
        User::Leave( KErrArgument );
        }

    if ( mailBox->HasCapability( EFSMBoxCapaSymbianMsgIntegration ) )
        {
        delete mailBox;
        return EFalse;
        }
    else
        {
        delete mailBox;
        return ETrue;
        }
    }

void CFSMailMtmHandler::HandleEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* /*aParam1*/,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ )
    {
    FUNC_LOG;
    switch ( aEvent )
        {
        case TFSEventMailboxSettingsChanged:
      	    {
            // At the moment this event is only for handling the case when
            // Email address of the mailbox has been changed
            CFSMailBox* mailBox( MailClient().GetMailBoxByUidL( aMailbox ) );
            User::LeaveIfNull( mailBox );
            CleanupStack::PushL( mailBox );
                    TDesC& ownAddress( mailBox->OwnMailAddress().GetEmailAddress() );
                    User::LeaveIfError(
                iMtmClient->RenameAccountL( KNullDesC, ownAddress, aMailbox ) );
            CleanupStack::PopAndDestroy( mailBox );
            break;
            }
        case TFSEventNewMailbox:
            {
            CFSMailBox* mailBox(
                MailClient().GetMailBoxByUidL( aMailbox ) );
            User::LeaveIfNull( mailBox );
            CleanupStack::PushL( mailBox );
            
            // Branded mailbox name is nowadays set in new mailbox event
            // handling, so we don't need to use brand manager here anymore.
            
            TDesC& name( mailBox->GetName() );
            TDesC& ownAddress( mailBox->OwnMailAddress().GetEmailAddress() );
            
            User::LeaveIfError(
                iMtmClient->CreateAccountL( name, ownAddress, aMailbox ) );
            
            CleanupStack::PopAndDestroy( mailBox );
            break;
            }
        case TFSEventMailboxDeleted:
            {

            // Entry details cannot necessarily be fetched anymore as the
            // entry might be deleted from the Message Server.
            User::LeaveIfError(
                iMtmClient->DeleteAccountL( KNullDesC, aMailbox ) );
            
            break;
            }
        case TFSEventMailboxRenamed:
            {
            CFSMailBox* mailBox(
                MailClient().GetMailBoxByUidL( aMailbox ) );
            User::LeaveIfNull( mailBox );
            CleanupStack::PushL( mailBox );
            TDesC& name( mailBox->GetName() );
            TDesC& ownAddress( mailBox->OwnMailAddress().GetEmailAddress() );

            User::LeaveIfError(
                iMtmClient->RenameAccountL( name, ownAddress, aMailbox ) );
            
            CleanupStack::PopAndDestroy( mailBox );
            break;
            }
        case TFSMailboxAvailable: // Flow through
        case TFSMailboxUnavailable: // Flow through
            {
            break;
            }
        default:
            {
            break;
            }
        }
    }

void CFSMailMtmHandler::InstallMtmL()
    {
    FUNC_LOG;
    iMsvSession = CMsvSession::OpenSyncL( *this );

    //Construct the path information for the Installation file.    
    TFileName resourceFileName = InstallationFilenameL();


    //Install MTMs.
    TInt err = iMsvSession->InstallMtmGroup( resourceFileName  );
    if( err!=KErrNone && err!=KErrAlreadyExists  )
        {
        User::LeaveIfError( err );
        }
    
    
    // Create client side mtm account.
    iMtmRegistry = CClientMtmRegistry::NewL( *iMsvSession );


    //Jagan: 19th Oct - Added this by looking at TxtMTM Example
    if( !iMtmRegistry->IsPresent( KUidMsgValTypeFsMtmVal ) )
        {
        //Freestyle not present in Client MTM Registry
        User::Leave( KErrNotFound );
        }

    TRAPD( creationError, iMtmClient =
        (CFsMtmClient*) iMtmRegistry->NewMtmL(
        KUidMsgValTypeFsMtmVal ); )
	if ( creationError != KErrNone )
    	{
    	User::Leave( creationError );
    	}
	iStartingUp = EFalse;
	// We are now ready to observe mail events
	SetObserving( ETrue );
    }
    
void CFSMailMtmHandler::UninstallMtm()
    {
    FUNC_LOG;
    delete iMtmClient;
    iMtmClient = NULL;
    
    delete iMtmRegistry;
    iMtmRegistry = NULL;
    
    // KILL THE MESSAGE SERVER PROCESS
    KillProcess( KMessageServerProcessIdentifier );
    User::After( KOneSecond );
    
    // UNINSTALL THE MTM

    TInt err;

    //Construct the path information for the Installation file.
    TFileName resourceFileName;    
    TRAP( err, resourceFileName = InstallationFilenameL() );

    if ( err == KErrNone && iMsvSession )
        {
        // Uninstall the MTMs.
        TInt retryCount = 0;
        do 
            {
            err = iMsvSession->DeInstallMtmGroup( resourceFileName  );
                if ( err == KErrNone )
                {
                // success
                }
            else if ( err == KErrInUse )
                {
                // MTMs in use, killing apps and retry
                TRAP_IGNORE( KillAppsL() );
                //wait one second
                User::After( KOneSecond );
                }
            else
                {
                // failure
                }
            } while ( err == KErrInUse && retryCount++ < KMAxRetry );
        }
     else
        {
        // Cannot define the filename or session not created.
        // Just continue with the session deletion.
        
        // Notice: Although deinstallation would fail
        // (and installation was done earlier) it seems that
        // crash does not occur even if we continue onwards.
        }
    
    delete iMsvSession;
    iMsvSession = NULL;
    }

TFileName CFSMailMtmHandler::InstallationFilenameL()
    {
    FUNC_LOG;
    //Construct the path information for the Installation file.
    TFileName fileName;

    TFindFile finder( CCoeEnv::Static()->FsSession() );
    User::LeaveIfError(
        finder.FindByDir( KFsMtmDirAndFile, KDC_MTM_INFO_FILE_DIR ) );
    fileName = finder.File();

    return fileName;
    }

void CFSMailMtmHandler::KillProcess( const TDesC& aName )
    {
    FUNC_LOG;
    TFullName name;
    TFindProcess find( aName );
    RProcess process;

    while ( find.Next( name ) == KErrNone )
        {
        if ( process.Open( find ) == KErrNone )
            {
            process.Kill( KErrGeneral );
            process.Close();
            }
        } // end while
    }

void CFSMailMtmHandler::KillAppsL()
    {
    FUNC_LOG;
    RWsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

    TInt appCount = ARRAY_SIZE( KAppsToKill );

    TApaTaskList taskList( session );

    for ( int i = 0 ; i < appCount ; i++ ) 
        {
        TApaTask task = taskList.FindApp( KAppsToKill[i] );
        if ( task.Exists() )
            {
            task.EndTask();
            }
        }
    CleanupStack::PopAndDestroy( &session );
    }

void CFSMailMtmHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    // This is not used. Instead CFsMtmClient is used
    // directly in HandleEventL.
    }

void CFSMailMtmHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    // This is not used. Instead CFsMtmClient is used
    // directly in HandleEventL.
    }

