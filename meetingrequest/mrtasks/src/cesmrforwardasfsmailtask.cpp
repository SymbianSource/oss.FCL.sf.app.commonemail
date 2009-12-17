/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for forwarding mr as email
*
*/

#include "emailtrace.h"
#include "cesmrforwardasfsmailtask.h"

#include <caluser.h>
#include <calentry.h>
#include <bautils.h>
#include <coemain.h>
#include <f32file.h>
#include <coeaui.h>
//<cmail>
#include "FreestyleEmailUiConstants.h"
//</cmail>
#include "tesmrinputparams.h"

#include "cesmrcaldbmgr.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrmailplaitextformatter.h"
#include "cesmrcalimportexporter.h"
#include "cesmrfsemailmanager.h"
#include "cesmrcaluserutil.h"
#include "esmrinternaluid.h"
#include "esmrhelper.h"

// Unnamed namespace for local definitions
namespace {

//<cmail> remove hardcoded paths
// Literal for temporary iCal meeting path
//_LIT( KTemporaryIcsPath, "c:\\temp\\" ); //codescanner::driveletters
//</cmail>

// Literal for temporary iCal meeting file
//<cmail> remove hardcoded paths
//_LIT( KTemporaryIcsFullName, 
//	  "c:\\temp\\meeting.ics" ); //codescanner::driveletters
_LIT( KTemporaryIcsName, "temp\\meeting.ics" );
//</cmail>

#ifdef _DEBUG

// Literal for panics
_LIT( KESMRForwardAsFSMail, "ESMRForwardAsFSMail" );

// Panic codes
enum TESMRForwardAsFSMailPanic
    {
    EESMRForwardAsFSMailCoeNull,
    EESMRForwardAsFSMailAppUiNull,
    EESMRForwardCannotBeTriggeredFromMail
    };

// ---------------------------------------------------------------------------
// Raises system panic.
// ---------------------------------------------------------------------------
//
void Panic( TESMRForwardAsFSMailPanic aPanic )
    {
    User::Panic( KESMRForwardAsFSMail, aPanic );
    }

#endif // _DEBUG

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRForwardAsFSMailTask::CESMRForwardAsFSMailTask
// ---------------------------------------------------------------------------
//
CESMRForwardAsFSMailTask::CESMRForwardAsFSMailTask(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils )
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRForwardAsFSMailTask::~CESMRForwardAsFSMailTask
// ---------------------------------------------------------------------------
//
CESMRForwardAsFSMailTask::~CESMRForwardAsFSMailTask()
    {
    FUNC_LOG;
    delete iEntryToSend;
    delete iCaluserUtil;
    delete iEmailMgr;
    }

// ---------------------------------------------------------------------------
// CESMRForwardAsFSMailTask::NewL
// ---------------------------------------------------------------------------
//
CESMRForwardAsFSMailTask* CESMRForwardAsFSMailTask::NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils )
    {
    FUNC_LOG;
    CESMRForwardAsFSMailTask* self =
        new (ELeave) CESMRForwardAsFSMailTask(
            aCalDbMgr,
            aEntry,
            aMRMailboxUtils );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRForwardAsFSMailTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRForwardAsFSMailTask::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    iEmailMgr = CESMRFSEMailManager::NewL( MailboxUtils() );
    }

// ---------------------------------------------------------------------------
// CESMRForwardAsFSMailTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRForwardAsFSMailTask::ExecuteTaskL()
    {
    FUNC_LOG;
    iEntryToSend = ESMREntry().ValidateEntryL();
    iCaluserUtil = CESMRCalUserUtil::NewL( *iEntryToSend );

    TESMRInputParams startupparams;
    TBool isEmail = ESMREntry().StartupParameters(startupparams);

    // This command cannot be triggered from email
    __ASSERT_DEBUG( !isEmail, Panic(EESMRForwardCannotBeTriggeredFromMail) );

    if(isEmail)
        {
        // This command cannot be triggered from email
        User::Leave( KErrNotSupported );
        }

    // Create mail message:
    ConstructMailL();

    // Save the message to drafts folder:
    iEmailMgr->StoreMessageToDraftsFolderL();

    // Launch FS EMail Editor:
    LaunchFSEmailEditorL();
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::ConstructMailL
// ---------------------------------------------------------------------------
//
void CESMRForwardAsFSMailTask::ConstructMailL()
    {
    FUNC_LOG;
    // create text formatter:
    CESMRMailPlainTextFormatter* textFormatter =
            CESMRMailPlainTextFormatter::NewLC(MailboxUtils());

    // Prepare email manager for sending
    CCalUser *mailboxUser = iEntryToSend->PhoneOwnerL();
    iEmailMgr->PrepareForSendingL(
            ESMRHelper::AddressWithoutMailtoPrefix(mailboxUser->Address() ) );

    // set the email subject:
    HBufC* subject =
            textFormatter->Subject16LC(
                    *iEntryToSend,
                    ETrue,
                    EFalse );
    iEmailMgr->SetSubjectL(*subject);
    CleanupStack::PopAndDestroy(subject);
    subject = NULL;

    iEmailMgr->CreateTextPlainPartL( KNullDesC() );

    CreateTemporaryMeetingFileL();
    iEmailMgr->StoreMessageToDraftsFolderL();

    CleanupStack::PopAndDestroy( textFormatter );
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::CreateTemporaryMeetingFileL
// ---------------------------------------------------------------------------
//
void CESMRForwardAsFSMailTask::CreateTemporaryMeetingFileL()
    {
    FUNC_LOG;
    CCoeEnv* coe = CCoeEnv::Static();

    __ASSERT_DEBUG( coe, Panic(EESMRForwardAsFSMailCoeNull) );

    if ( !coe )
        {
        User::Leave( KErrNotSupported );
        }

    RFs& fs = coe->FsSession();

    // Ensure that path exists
    //<cmail> hardcoded paths removal
    /*TInt err = fs.MkDirAll( KTemporaryIcsPath );
    if ( KErrAlreadyExists != err )
        {
        User::LeaveIfError( err );
        }*/
    //</cmail>

    RFile calendarFile;
    //<cmail>hard coded paths removal
    TFileName fileName(KTemporaryIcsName);
    User::LeaveIfError(ESMRHelper::CreateAndAppendPrivateDirToFileName(fileName));
    TInt err = calendarFile.Replace(
                fs,
                fileName,
                EFileWrite );
    //</cmail>
    if ( KErrNone != err )
        User::LeaveIfError( err );

    CleanupClosePushL( calendarFile );

    CESMRCalImportExporter* calExporter = CESMRCalImportExporter::NewLC();
    HBufC8* calendarText = calExporter->ExportToICal8LC( *iEntryToSend );

    calendarFile.Write( *calendarText );
    calendarFile.Flush();

    // calendarText, calExporter and calendarFile
    CleanupStack::PopAndDestroy( 3, & calendarFile);
    iEmailMgr->SetAttachmentL( fileName );//<cmail>
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::LaunchFSEmailEditorL
// ---------------------------------------------------------------------------
//
TInt CESMRForwardAsFSMailTask::LaunchFSEmailEditorL()
    {
    FUNC_LOG;
    CCoeEnv* coe = CCoeEnv::Static();
    __ASSERT_DEBUG( coe, Panic(EESMRForwardAsFSMailCoeNull) );

    CCoeAppUi* appui = coe->AppUi();
    __ASSERT_DEBUG( appui, Panic(EESMRForwardAsFSMailAppUiNull) );

    // fill params
    TEditorLaunchParams params;
    params.iActivatedExternally = ETrue;
    params.iMailboxId = iEmailMgr->MailboxId();
    params.iFolderId = iEmailMgr->MessageFolderId();
    params.iMsgId = iEmailMgr->MessageId();

    TVwsViewId viewId(
            KFSEmailUiUid,
            MailEditorId );
    TUid editorCommand = TUid::Uid( KEditorCmdOpen );
    TPckgC<TEditorLaunchParams> buf( params );

    appui->ActivateViewL(
            viewId,
            editorCommand,
            buf );

    return KErrNone;
    }

//EOF

