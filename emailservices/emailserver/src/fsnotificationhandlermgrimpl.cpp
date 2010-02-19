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
* Description:  Implementation of notification handler manager
*
*/


#include "emailtrace.h"
#include <ecom/ecom.h> // FinalClose()
//<cmail>
#include "cfsmailclient.h"
//</cmail>
#include <sysutil.h>
//<cmail>
#include <fsmailserver.rsg>
#include "cmailhandlerpluginuids.h"
#include "fsnotificationhandlerbase.h"
#include "FsEmailGlobalDialogsAppUi.h"
#include "FsEmailMessageQueryDialog.h"
#include "FsEmailAuthenticationDialog.h"
//</cmail>
#include "fsnotificationhandlermgrimpl.h"
//<cmail>
#include "FsEmailGlobalDialogsAppUi.h"
//</cmail>


static const TInt64 KMegaByte = 1048576;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
//<cmail> // aAppUi parameter no longer used, created by its own
CFSNotificationHandlerMgr::CFSNotificationHandlerMgr(CFsEmailGlobalDialogsAppUi* aAppUi) :
//</cmail>
    CActive( EPriorityStandard ),
    iAppUi( aAppUi )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::ConstructL()
    {
    FUNC_LOG;
    // Performs the time consuming initialization asynchronously in RunL, in order
    // to let the process startup finish quicker
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// ---------------------------------------------------------------------------
// Finishes the initialisation
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::RunL()
    {
    FUNC_LOG;
    // Create mail client
    iMailClient = CFSMailClient::NewL();
    if ( iMailClient == NULL )
        {
        User::Leave( KErrNoMemory );
        }

    // Once mail client is created ok, disk space needs to be checked
    // and cleaned if necessary
    CleanTempFilesIfNeededL();

    //<cmail>
    // Notification handlers are created next.
    // Notice that if a handler cannot be created it does not mean
    // that the construction of the manager would be stopped. This
    // approach is chosen so that if something goes wrong with
    // construction of a handler it can safely leave and get
    // destroyed but does not interfere other handlers.

    CreateAndStoreHandlerL( KMailIconHandlerUid );

    CreateAndStoreHandlerL( KLedHandlerUid );

#ifndef __WINS__
    CreateAndStoreHandlerL( KSoundHandlerUid );
    // Earlier RefreshData() was called for the soundhandler
    // object after creation, but as it does not do anything
    // it is not called anymore.
#endif

    CreateAndStoreHandlerL( KMtmHandlerUid );

    CreateAndStoreHandlerL( KOutofMemoryHandlerUid );

    CreateAndStoreHandlerL( KAuthenticationHandlerUid );

    CreateAndStoreHandlerL( KMessageQueryHandlerUid );

    CreateAndStoreHandlerL( KCMailCpsHandlerUid );
    //</cmail>

    StartObservingL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::DoCancel()
    {
    FUNC_LOG;
    // Nothing to cancel
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::CreateAndStoreHandlerL( TInt aImplementationUid )
    {
    FUNC_LOG;
    CFSNotificationHandlerBase* handler = NULL;
    TRAPD( error, handler = CFSNotificationHandlerBase::NewL( aImplementationUid, *this ) );
    if( handler && (error == KErrNone) )
        {
        iHandlers.Append( handler );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
//<cmail>
CFSNotificationHandlerMgr* CFSNotificationHandlerMgr::NewL(CFsEmailGlobalDialogsAppUi* aAppUi)
//</cmail>
    {
    FUNC_LOG;
    CFSNotificationHandlerMgr* self = CFSNotificationHandlerMgr::NewLC( aAppUi );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
//<cmail>
CFSNotificationHandlerMgr* CFSNotificationHandlerMgr::NewLC(CFsEmailGlobalDialogsAppUi* aAppUi)
//</cmail>
    {
    FUNC_LOG;
    CFSNotificationHandlerMgr* self = new( ELeave ) CFSNotificationHandlerMgr( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFSNotificationHandlerMgr::~CFSNotificationHandlerMgr()
    {
    FUNC_LOG;
    Cancel();

    StopObserving();

    iHandlers.ResetAndDestroy();

    iHSConnection = NULL;

	if( iMailClient )
		{
        iMailClient->Close();
        iMailClient = NULL;
		}

	//<cmail>
	iAppUi = NULL;
	//</cmail>

    // Finished using ECom
    // ECom used at least in CFSMailHSUpdateHandler
    REComSession::FinalClose();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::EventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* aParam2,
    TAny* aParam3 )
    {
    FUNC_LOG;
    // First we check each event here. This is done so we know
    // if something has must be done before passing the event on
    // to handlers. For example we may want to register to
    // mailbox events.

    switch ( aEvent )
        {
        case TFSEventNewMailbox:
            {
            iMailClient->SubscribeMailboxEventsL( aMailbox, *this );
            iMailClient->GetBrandManagerL().UpdateMailboxNamesL( aMailbox );
            break;
            }
        case TFSEventMailboxDeleted:
            {
            // Don't have to do anything here. Observing has ended when the
            // mailbox is deleted.
            break;
            }
        case TFSMailboxAvailable: // Flow through
        case TFSMailboxUnavailable: // Flow through
        case TFSEventMailboxRenamed:
            {
            break;
            }
        case TFSEventNewMail:
            {
            // If this is a preinstalled version and we receive a new mail we
            // update the current status of the HS here before passing
            // the events to handlers so they don't have to do it. If they
            // do it, it is done several times which is not desired.
            // The drawback is that by doing it here we might also do it
            // in situations where the handlers would actually not need it.
            // Possibly the best solution would be to initialise the
            // iHSConnection once and then let it observe for changes in
            // central repository. Currently that solution is not implemented
            // as it would require more time to implement.
            }
        default:
            {
            break;
            }
        }


    // Let's pass all events to handlers also.
    const TInt handlerCount( iHandlers.Count() );
    TInt handlerIndex( 0 );

    while ( handlerIndex < handlerCount )
        {
        // Event is passed to each handler. If one fails the
        // event is still passed to others as they are not
        // necessarily dependant on the same services. This way
        // If one fails the others can still succeed.
        TRAP_IGNORE(
            iHandlers[handlerIndex]->EventL( aEvent,
                                             aMailbox,
                                             aParam1,
                                             aParam2,
                                             aParam3 ) );
        ++handlerIndex;
        }
    }

CFSMailClient& CFSNotificationHandlerMgr::MailClient() const
    {
    FUNC_LOG;
    // Instance of this class does not exist without the
    // mail client so it is safe to return a reference to the
    // pointed object.
    return *iMailClient;
    }

CFSNotificationHandlerHSConnection* CFSNotificationHandlerMgr::HSConnection() const
    {
    FUNC_LOG;
    return iHSConnection;
    }

void CFSNotificationHandlerMgr::IncreaseDialogCount()
    {
    FUNC_LOG;
    iDialogCount++;
    }

void CFSNotificationHandlerMgr::DecreaseDialogCount()
    {
    FUNC_LOG;
    iDialogCount--;
    }

TInt CFSNotificationHandlerMgr::GetDialogCount()
    {
    FUNC_LOG;
    return iDialogCount;
    }

//<cmail>
void CFSNotificationHandlerMgr::MessageQueryL( TDesC& aMailboxName,
                                               TRequestStatus& aStatus,
                                               const TDesC& aCustomMessageText,
                                               TFsEmailNotifierSystemMessageType aMessageType )
    {
    FUNC_LOG;
    CFsEmailMessageQueryDialog* dialog =
            CFsEmailMessageQueryDialog::NewLC(
                aStatus, aMailboxName, aMessageType, aCustomMessageText );
    // RunLD pops dialog from cleanup stack
    dialog->RunLD();
    }
//</cmail>

TInt CFSNotificationHandlerMgr::AuthenticateL( TDes& aPassword,
                                               TDesC& aMailboxName,
                                               TRequestStatus& aStatus )
    {
    FUNC_LOG;
    CFsEmailAuthenticationDialog* dialog = CFsEmailAuthenticationDialog::NewL( aStatus, aMailboxName, aPassword );
    TRAPD( err, dialog->ExecuteLD( R_FS_MSERVER_DIALOG_AUTHENTICATION ) );
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::StartObservingL()
    {
    FUNC_LOG;
    iMailClient->AddObserverL( *this );

    RPointerArray<CFSMailBox> mailBoxList;
    // Null id given as a plugin id. mailboxes of all plugins retrieved.
    // Notice that ownership of the mailboxes is not passed to here.
    iMailClient->ListMailBoxes( TFSMailMsgId(), mailBoxList );

    TInt mailBoxCount( mailBoxList.Count() );
    TInt mailboxIndexer( 0 );
    while ( mailboxIndexer < mailBoxCount )
        {

        // Here could be prevented the observing of mailboxes
        // that don't contain inbox but it is probably rather
        // improbable and it also would complicate the implementation.
        // So not implemented.
        TFSMailMsgId currentMailboxId(
            mailBoxList[mailboxIndexer]->GetId() );
        TInt error = KErrNone;
        TRAP( error, iMailClient->SubscribeMailboxEventsL( currentMailboxId,
                                                           *this ) );

        // Although an error would occur in subscribing to some mailbox
        // the execution is still continued so that one mailbox won't
        // prevent mail server from using the other mailboxes.

        ++mailboxIndexer;
        }

    mailBoxList.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::StopObserving()
    {
    FUNC_LOG;

    // Removeobserver is also called for each mailbox
    // We should be an observer for each mailbox so this should not
    // bring up any problems normally.

    RPointerArray<CFSMailBox> mailBoxList;
    // Null id given as a plugin id. mailboxes of all plugins retrieved.
    // Notice that ownership of the mailboxes is not passed to here.
    iMailClient->ListMailBoxes( TFSMailMsgId(), mailBoxList );

    TInt mailBoxCount( mailBoxList.Count() );
    TInt mailboxIndexer( 0 );
    while ( mailboxIndexer < mailBoxCount )
        {
        TFSMailMsgId currentMailboxId(
            mailBoxList[mailboxIndexer]->GetId() );
        iMailClient->UnsubscribeMailboxEvents( currentMailboxId,
                                               *this );

        ++mailboxIndexer;
        }

    mailBoxList.ResetAndDestroy();


    iMailClient->RemoveObserver( *this );

    }

// ---------------------------------------------------------------------------
// Function cleans up downloaded files of mailboxes if disk space is low
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::CleanTempFilesIfNeededL()
	{
    FUNC_LOG;
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
    // Check whether disk space is below 3MB, in that case start cleaning up
    // downloaded attachments from mailboxes.
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fsSession, 3*KMegaByte, EDriveC ) )
    	{
	    RPointerArray<CFSMailBox> mailBoxList;
	    // Null id given as a plugin id. mailboxes of all plugins retrieved.
	    // Notice that ownership of the mailboxes is not passed to here.
	    iMailClient->ListMailBoxes( TFSMailMsgId(), mailBoxList );
		for ( TInt i = 0 ; i < mailBoxList.Count() ; ++i )
		    {
			TRAP_IGNORE( mailBoxList[i]->RemoveDownLoadedAttachmentsL() );
		    }
	    mailBoxList.ResetAndDestroy();
 	    }
    CleanupStack::PopAndDestroy( &fsSession );
	}


//<cmail>
// ---------------------------------------------------------------------------
// CFSNotificationHandlerMgr::SendAppUiToBackground()
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::SendAppUiToBackground()
    {
    FUNC_LOG;
    if(iAppUi)
        iAppUi->SendToBackground();
    }

// ---------------------------------------------------------------------------
// CFSNotificationHandlerMgr::BringAppUiToForeground()
// ---------------------------------------------------------------------------
//
void CFSNotificationHandlerMgr::BringAppUiToForeground()
    {
    FUNC_LOG;
    if(iAppUi)
        iAppUi->BringToForeground();
    }
//</cmail>

// End of file

