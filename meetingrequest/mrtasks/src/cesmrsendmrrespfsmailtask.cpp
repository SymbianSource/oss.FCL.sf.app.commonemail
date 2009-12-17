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
* Description:  Implementation for sending MR response via fs email
*
*/
#include "emailtrace.h"
#include "cesmrsendmrrespfsmailtask.h"

#include <caluser.h>
#include <calentry.h>
#include <utf.h>
//<cmail>
#include "mmrinfoprocessor.h"
//</cmail>
#include <f32file.h>
#include <coemain.h>
#include <bautils.h>
#include <cmrmailboxutils.h>

#include "cesmrcaldbmgr.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrmailplaitextformatter.h"
#include "cesmrfsemailmanager.h"
#include "cesmrcalimportexporter.h"
#include "tesmrinputparams.h"
#include "cesmrcaluserutil.h"
#include "esmrhelper.h"

// Unnamed namespace for local definitions
namespace {
// Literal for plain text separator
_LIT( KPlainTextResponseTxtSeparator,
      "\n----------------------------------------\n");
	  

// Literal for response iCal file
//<cmail> remove hard coded paths
//_LIT( KPath, "c:\\temp\\" ); //codescanner::driveletters
//_LIT( KFileAndPath, "c:\\temp\\response.ics" ); //codescanner::driveletters
//_LIT( KPath, "temp\\" );
_LIT( KFileName, "temp\\response.ics" );
//</cmail>

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::CESMRSendMRRespFSMailTask
// ---------------------------------------------------------------------------
//
CESMRSendMRRespFSMailTask::CESMRSendMRRespFSMailTask(
        TESMRCommand aCommand,
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRResponseType aResponseType )
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils ),
    iCommand( aCommand ),
    iResponseType( aResponseType )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::~CESMRSendMRRespFSMailTask
// ---------------------------------------------------------------------------
//
CESMRSendMRRespFSMailTask::~CESMRSendMRRespFSMailTask()
    {
    FUNC_LOG;
    delete iCaluserUtil;
    delete iEntryToSend;
    delete iResponseMessage;
    delete iEmailMgr;
    delete iTextFormatter;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::NewL
// ---------------------------------------------------------------------------
//
CESMRSendMRRespFSMailTask* CESMRSendMRRespFSMailTask::NewL(
        TESMRCommand aCommand,
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRResponseType aResponseType,
        const TDesC& aResponseMessage )
    {
    FUNC_LOG;
    CESMRSendMRRespFSMailTask* self =
        new (ELeave) CESMRSendMRRespFSMailTask(
            aCommand,
            aCalDbMgr,
            aEntry,
            aMRMailboxUtils,
            aResponseType );

    CleanupStack::PushL(self);
    self->ConstructL(aResponseMessage);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespFSMailTask::ConstructL(
        const TDesC& aResponseMessage )
    {
    FUNC_LOG;
    BaseConstructL();
    iEmailMgr        = CESMRFSEMailManager::NewL( MailboxUtils() );
    iTextFormatter   = CESMRMailPlainTextFormatter::NewL( MailboxUtils() );
    iResponseMessage = aResponseMessage.AllocL();
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespFSMailTask::ExecuteTaskL()
    {
    FUNC_LOG;
    iEntryToSend = ESMREntry().ValidateEntryL();

    // Checking input paramters
    if( CCalEntry::EMethodReply != iEntryToSend->MethodL() &&
            EESMRCmdRemoveFromCalendar != iCommand )
        {
        User::Leave( KErrArgument );
        }

    if ( ESMREntry().IsSyncObjectPresent( ) )
        {
        SendMailViaSyncL();
        }
    else
        {
        iCaluserUtil = CESMRCalUserUtil::NewL( *iEntryToSend );

        // create mail message:
        ConstructMailL();

        TInt ret = iEmailMgr->SendMessageL();
        User::LeaveIfError(ret);
        }
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::ConstructMailL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespFSMailTask::ConstructMailL()
    {
    FUNC_LOG;
    // Prepare email manager for sending
    CCalUser *mailboxUser = ESMREntry().Entry().PhoneOwnerL();
    iEmailMgr->PrepareForSendingL(
            ESMRHelper::AddressWithoutMailtoPrefix(
                    mailboxUser->Address() ) );

    // set the email subject
    HBufC* subject =
    iTextFormatter->Subject16LC(
            *iEntryToSend,
            ESMREntry().IsForwardedL(),
            ESMREntry().IsEntryEditedL() );

    iEmailMgr->SetSubjectL(*subject);
    CleanupStack::PopAndDestroy(subject);
    subject = NULL;

    // Set the To recipient
    iEmailMgr->AppendToRecipientL(
            iEntryToSend->OrganizerL()->Address(),
            iEntryToSend->OrganizerL()->CommonName() );

    // Set text/plain part
    AddPlainTextPartL();

    // Set text/calendar part:
    AddCalendarPartL();
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::AddPlainTextPartL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespFSMailTask::AddPlainTextPartL()
    {
    FUNC_LOG;

    HBufC* plainText = iTextFormatter->Body16LC( *iEntryToSend );
    if ( iResponseMessage )
        {
        TPtrC plainTextSeparator( KPlainTextResponseTxtSeparator() );

        plainText = plainText->ReAlloc(
                        plainText->Length() +
                        plainTextSeparator.Length() +
                        iResponseMessage->Length() );

        CleanupStack::Pop( plainText );
        CleanupStack::PushL( plainText );

        TPtr ptrPlainText( plainText->Des() );
        ptrPlainText.Append( plainTextSeparator );
        ptrPlainText.Append( *iResponseMessage );
        }

    iEmailMgr->CreateTextPlainPartL(*plainText);
    CleanupStack::PopAndDestroy(plainText);
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::AddCalendarPartL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespFSMailTask::AddCalendarPartL()
    {
    FUNC_LOG;
    SetAttendeesL();    
    
    CESMRCalImportExporter* calExporter =
            CESMRCalImportExporter::NewLC();

    HBufC* calendarPart =
            calExporter->ExportToICal16LC( *iEntryToSend );

    //<cmail> removing hardcoded paths
    TFileName fileName(KFileName);
    User::LeaveIfError(ESMRHelper::CreateAndAppendPrivateDirToFileName(fileName));
    SaveICalToFileL( *calendarPart, fileName );
    iEmailMgr->CreateTextCalendarPartL(
            CESMRFSEMailManager::EESMRMethodResponse,
                    fileName);
    //</cmail>

    CleanupStack::PopAndDestroy( calendarPart );
    CleanupStack::PopAndDestroy( calExporter );
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::SendMailViaSyncL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespFSMailTask::SendMailViaSyncL()
    {
    FUNC_LOG;
    TESMRInputParams startupParams;
 
    if ( ESMREntry().StartupParameters(startupParams) )
        {
        MMRInfoProcessor::TMRInfoResponseMode responseMode (
                MMRInfoProcessor::EMRInfoResponseSendAndSync );

        if ( EESMRResponseDontSend == iResponseType )
            {
            responseMode = MMRInfoProcessor::EMRInfoResponseSync;
            }
        else if ( EESMRCmdRemoveFromCalendar == iCommand )
            {
            responseMode = MMRInfoProcessor::EMRInfoRemoveFromCal;
            }

        MMRInfoObject& infoObject = ESMREntry().ValidateSyncObjectL();
        iEmailMgr->SendMailViaSyncL(
                startupParams,
                infoObject,
                responseMode,
                *iResponseMessage );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::SaveICalToFileL
// ---------------------------------------------------------------------------
//
TInt CESMRSendMRRespFSMailTask::SaveICalToFileL(
        const TDesC& aICal,
        const TDesC& aFilename  )
    {
    FUNC_LOG;
    //create 8 bit buffer for temp file content
    TInt length = aICal.Length() * 2;
    HBufC8* buffer = HBufC8::NewLC( length );
    TPtr8 des = buffer->Des();

    // covert the 16 bit iCal to 8 bit iCal:
    TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8(des, aICal);

    RFs& fs( CCoeEnv::Static()->FsSession() );

    // ensure the path exists:
    //<cmail> remove hard coded paths
    //BaflUtils::EnsurePathExistsL(fs, KPath);
    //</cmail>

    // delete previous file
    fs.Delete( aFilename );

    // save the iCal to file system:
    RFile file;
    User::LeaveIfError(file.Create(fs, aFilename, EFileWrite));
    CleanupClosePushL(file);
    User::LeaveIfError(file.Write(*buffer));

    // clean up:
    CleanupStack::PopAndDestroy(2, buffer); // file, fs, buffer

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::SetAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespFSMailTask::SetAttendeesL()
    {
    FUNC_LOG;
    CCalAttendee* thisAttendee = 
            ESMRHelper::CopyAttendeeLC( 
                              *(MailboxUtils().ThisAttendeeL( *iEntryToSend )) );
    
    RPointerArray<CCalAttendee>& attendeeList = iEntryToSend->AttendeesL();
    
    while ( attendeeList.Count() )
        {
        //remove attendees from entry that is to be forwarded
        iEntryToSend->DeleteAttendeeL( 0 );
        }
    
    // Ownership is transferred
    iEntryToSend->AddAttendeeL( thisAttendee );
    CleanupStack::Pop( thisAttendee );
    }

//EOF

