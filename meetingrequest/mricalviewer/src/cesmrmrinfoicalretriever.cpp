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
* Description:  ESMR mrinfo calendar entry retriever implementation
*
*/


#include "emailtrace.h"
#include "cesmrmrinfoicalretriever.h"
#include "cesmrinforecurrencehandler.h"
#include "cesmrattachmentinfo.h"
#include "mesmricalviewerobserver.h"
#include "tesmrinputparams.h"
#include "esmricalviewerutils.h"
#include "esmrconfig.hrh"

//<cmail>
#include "esmricalvieweropcodes.hrh"
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cfsmailmessage.h"
#include "cfsmailmessagepart.h"
#include "cfsmailcommon.h"
#include "mmrinfoprocessor.h"
#include "mmrattendee.h"
#include "mmrrecurrencerule.h"
//</cmail>

#include <calentry.h>
#include <caluser.h>
#include <calalarm.h>
#include <CalenInterimUtils2.h>
#include <utf.h>

// Unnamed namespace for local definitions and functions
namespace {

/**
 * Raises system leave if parameter is different than KErrNone.
 *
 * @param aError Error code
 */
void LeaveIfError( TInt aError )
    {
    if ( KErrNone != aError )
        {

        User::Leave( aError );
        }
    }

/**
 * Tests if certain field is supported by MRINFO object.
 * @param aFields Reference to MRINFO fields'
 * @param aFieldType Tested fieldtype
 */
TBool IsFieldSupported(
        RArray<MMRInfoObject::TESMRInfoField>& aFields,
        MMRInfoObject::TESMRInfoField aFieldType)
    {
    TInt fieldSupported( ETrue );

    TInt fieldCount( aFields.Count() );
    for (TInt i=0; i < fieldCount; ++i)
        {
        if (aFields[i] == aFieldType)
            {
            fieldSupported = ETrue;
            break;
            }
        }
    return fieldSupported;
    }

/**
 * Converts MRINFO method to CCalEntry method.
 * @param aMRMethod MRINFO method.
 * @return CCalEntry method.
 */
CCalEntry::TMethod ConverMRMethodToCalEntryMethod(
        MMRInfoObject::TMRMethod aMRMethod )
    {
    CCalEntry::TMethod ret(CCalEntry::EMethodNone);
    switch (aMRMethod)
        {
        case MMRInfoObject::EMRMethodRequest:
            {
            ret = CCalEntry::EMethodRequest;
            break;
            }
         case MMRInfoObject::EMRMethodCancel:
            {
            ret = CCalEntry::EMethodCancel;
            break;
            }
         case MMRInfoObject::EMRMethodResponse:
            {
            ret = CCalEntry::EMethodReply;
            break;
            }
         default:
            break;
        }
    return ret;
    }

/**
 * Converts MMRINFO response status to CCalEntry response status.
 * @param aMRResponseStatus MRINFO response status.
 * @return CCalEntry response status.
 */
CCalEntry::TStatus ConvertMRResponseToCalEntryStatus(
        MMRInfoObject::TResponse aMRResponseStatus )
    {
    FUNC_LOG;
    CCalEntry::TStatus ret(CCalEntry::ENullStatus);
    switch (aMRResponseStatus)
        {
        case MMRInfoObject::EMrCmdResponseAccept:
            {
            ret = CCalEntry::EConfirmed;
            break;
            }
         case MMRInfoObject::EMrCmdResponseTentative:
            {
            ret = CCalEntry::ETentative;
            break;
            }
         case MMRInfoObject::EMrCmdResponseDecline:
            {
            ret = CCalEntry::ECancelled;
            break;
            }
         default:
            break;
        }
    return ret;
    }

/**
 * Converts MRINFO attendee role to MRINFO attendee role.
 * @param aMRRole MRINFO attendee role.
 * @return CCalEntry attendee role.
 */
CCalAttendee::TCalRole ConvertMRAttendeeRoleToCalEntryRole(
        MMRAttendee::TAttendeeRole aMRRole)
    {
    CCalAttendee::TCalRole ret(CCalAttendee::ENonParticipant);
    switch (aMRRole)
        {
        case MMRAttendee::EMRAttendeeNotSet:
            {
            ret = CCalAttendee::ENonParticipant;
            break;
            }
         case MMRAttendee::EMRAttendeeChair:
            {
            ret = CCalAttendee::EChair;
            break;
            }
         case MMRAttendee::EMRAttendeeParticipant:
            {
            ret = CCalAttendee::EReqParticipant;
            break;
            }
         case MMRAttendee::EMRAttendeeOptionalParticipant:
            {
            ret = CCalAttendee::EOptParticipant;
            break;
            }
         case MMRAttendee::EMRAttendeeNonParticipant:
            {
            ret = CCalAttendee::ENonParticipant;
            break;
            }
         default:
            break;
        }
    return ret;
    }

/**
 * Converts MMRINFO response status to CCalEntry response status.
 * @param aMRStatus MRINFO response status.
 * @return CCalEntry response status.
 */
CCalAttendee::TCalStatus ConvertMRAttendeeStatusToCalEntryStatus(
        MMRAttendee::TAttendeeStatus aMRStatus )
    {
    FUNC_LOG;
    CCalAttendee::TCalStatus ret(CCalAttendee::ENeedsAction);
    switch (aMRStatus)
        {
        case MMRAttendee::EMRAttendeeActionNotSet:
            {
            ret = CCalAttendee::ENeedsAction;
            break;
            }
         case MMRAttendee::EMRAttendeeActionNeeded:
            {
            ret = CCalAttendee::ENeedsAction;
            break;
            }
         case MMRAttendee::EMRAttendeeActionAccepted:
            {
            ret = CCalAttendee::EAccepted;
            break;
            }
         case MMRAttendee::EMRAttendeeActionTentative:
            {
            ret = CCalAttendee::ETentative;
            break;
            }
         case MMRAttendee::EMRAttendeeActionConfirmed:
            {
            ret = CCalAttendee::EConfirmed;
            break;
            }
         case MMRAttendee::EMRAttendeeActionDeclined:
            {
            ret = CCalAttendee::EDeclined;
            break;
            }
        case MMRAttendee::EMRAttendeeActionCompleted:
            {
            ret = CCalAttendee::ECompleted;
            break;
            }
        default:
           break;
        }
    return ret;
    }

/**
 * Copies attendees from MRINFO object to CCalEntry object.
 * @param attendeesArray array of meeting request attendees
 * @param calendar entry of the meeting request
 */
void FillAttendeesL(
        RPointerArray<MMRAttendee>& attendeesArray,
        CCalEntry& aEntry)
    {

    CCalAttendee* attendee = NULL;
    TInt attendeeArrayCount( attendeesArray.Count() );
    for (TInt i=0; i < attendeeArrayCount; ++i)
        {
        // New attendee + address
        attendee = CCalAttendee::NewL( attendeesArray[i]->Address() );
        CleanupStack::PushL(attendee);
        // Common name:
        if ( attendeesArray[i]->CommonName().Length() > 0 )
            {
            attendee->SetCommonNameL( attendeesArray[i]->CommonName() );
            }

        // Set attendeee role:
        attendee->SetRoleL(
                ConvertMRAttendeeRoleToCalEntryRole(
                        attendeesArray[i]->AttendeeRole() ) );

        // add attendee status:
        attendee->SetStatusL(
                ConvertMRAttendeeStatusToCalEntryStatus(
                        attendeesArray[i]->AttendeeStatus() ) );

        // add attendee to entry
        aEntry.AddAttendeeL( attendee );
        CleanupStack::Pop(attendee);
        }
    }

/**
 * Tests if MRINFO object is modifying entry
 * @param aEntry Reference to MRINFO object.
 */
TBool IsModifyingEntryL(
        MMRInfoObject& aEntry )
    {
    TBool modifying( EFalse );
    TTime recurrenceId = aEntry.MRRecurrenceId();

    if ( recurrenceId != Time::NullTTime() )
        {
        modifying = ETrue;
        }

    return modifying;
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::CESMRInfoIcalRetrieverCmd
// ---------------------------------------------------------------------------
//
CESMRInfoIcalRetrieverCmd::CESMRInfoIcalRetrieverCmd(
        CCalSession& aCalSession,
        TESMRInputParams& aInputParameters )
:   CESMRIcalViewerCommandBase( EESMRLoadMRInfoData, aCalSession ),
    iInputParameters( aInputParameters )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::~CESMRInfoIcalRetrieverCmd
// ---------------------------------------------------------------------------
//
CESMRInfoIcalRetrieverCmd::~CESMRInfoIcalRetrieverCmd()
    {
    FUNC_LOG;
    iSupportedFields.Reset();
    iSupportedFields.Close();
    delete iAttachmentInfo;
    delete iConvertedEntry;
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetriever::NewL
// ---------------------------------------------------------------------------
//
CESMRInfoIcalRetrieverCmd* CESMRInfoIcalRetrieverCmd::NewL(
        CCalSession& aCalSession,
        TESMRInputParams& aInputParameters )
    {
    FUNC_LOG;
    CESMRInfoIcalRetrieverCmd* self =
            new (ELeave) CESMRInfoIcalRetrieverCmd( aCalSession, aInputParameters );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRInfoIcalRetrieverCmd::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetriever::ExecuteCommandL
// ---------------------------------------------------------------------------
//
void CESMRInfoIcalRetrieverCmd::ExecuteCommandL(
        CFSMailMessage& aMessage,
        MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    SetMessage( &aMessage );
    SetObserver( &aObserver );

    iResult.iOpType     = OperationType();
    iResult.iResultCode = KErrNone;
    iResult.iMessage    = &aMessage;

    // Fetch MRINFO object
    if ( !aMessage.IsMRInfoSet() )
        {
        iResult.iResultCode = KErrNotFound;
        aObserver.OperationError( iResult );
        }
    else
        {
        iMRInfoObject =  &(aMessage.GetMRInfo());

        CreateEntryL();
        FillCommonFieldsL();

        if ( !IsModifyingEntryL( *iMRInfoObject ) )
            {
            FillRecurrenceL();
            }

        FillAttachmentInfoL();

        // Fill input parameters
        // Ownership is not trasferred
        iInputParameters.iCalEntry = iConvertedEntry;

        iInputParameters.iMRInfoObject = iMRInfoObject;
        iMRInfoObject = NULL;

        aObserver.OperationCompleted( iResult );
        }
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::CancelCommand
// ---------------------------------------------------------------------------
//
void CESMRInfoIcalRetrieverCmd::CancelCommand()
    {
    FUNC_LOG;
    // No implementation required because this is synchronous command
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::CreateEntryL
// ---------------------------------------------------------------------------
//
void CESMRInfoIcalRetrieverCmd::CreateEntryL()
    {
    FUNC_LOG;
    iMRInfoObject->SupportedFields( iSupportedFields );

    HBufC8* uid8 = NULL;        // buffer for UID
    TInt seqNum(0);             // Sequence Number
    CCalEntry::TMethod method( CCalEntry::EMethodNone );  // Method

    // fetch the UID
    if (IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldId))
        {
        // CnvUtfConverter
        TPtrC uid16( iMRInfoObject->UidL() );
        uid8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( uid16 );

        CleanupStack::PushL( uid8 );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }

    // Fetch the sequence nunber
    if (IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldSeqNo))
        {
        seqNum = iMRInfoObject->MRSequenceNumberL();
        }

    // Fetch the method
    if ( IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldMethod) )
        {
        MMRInfoObject::TMRMethod mrMethod = iMRInfoObject->MRMethodL();
        method = ConverMRMethodToCalEntryMethod(mrMethod);
        if (method == CCalEntry::EMethodNone)
            {
            LeaveIfError( KErrCorrupt );
            }
        }
    else
        {
        LeaveIfError( KErrCorrupt );
        }

    // create new entry:
    delete iConvertedEntry; iConvertedEntry = NULL;

    if ( !IsModifyingEntryL( *iMRInfoObject ) )
        {
        iConvertedEntry = CCalEntry::NewL (CCalEntry::EAppt,
                                            uid8,
                                            method,
                                            seqNum );
        }
    else
        {
        TBool seqNoSupported(
                IsFieldSupported( iSupportedFields, MMRInfoObject::EESMRInfoFieldSeqNo ) );

        if( !seqNoSupported )
            {
             LeaveIfError( KErrCorrupt );
            }

        TInt seqwNo( iMRInfoObject->MRSequenceNumberL() );
        TCalTime recurrenceID;
        recurrenceID.SetTimeLocalL( iMRInfoObject->MRRecurrenceId() );

        iConvertedEntry = CCalEntry::NewL(
                                CCalEntry::EAppt,
                                uid8,
                                method,
                                seqwNo,
                                recurrenceID,
                                CalCommon::EThisOnly );
        }

    CleanupStack::Pop(uid8 );
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::FillCommonFieldsL
// ---------------------------------------------------------------------------
//
void CESMRInfoIcalRetrieverCmd::FillCommonFieldsL()
    {
    FUNC_LOG;
    if (IsFieldSupported(
            iSupportedFields,
            MMRInfoObject::EESMRInfoFieldMStatus))
        {
        CCalEntry::TStatus status =
            ConvertMRResponseToCalEntryStatus(iMRInfoObject->MRResponse());
        iConvertedEntry->SetStatusL(status);
        }

    if (IsFieldSupported(
                iSupportedFields,
                MMRInfoObject::EESMRInfoFieldCreationDateTime))
        {
        TTime creationTime = iMRInfoObject->CreationTimeInUtcL();
        iConvertedEntry->SetLastModifiedDateL();
        }

    if (IsFieldSupported(
                iSupportedFields,
                MMRInfoObject::EESMRInfoFieldOrganizer))
        {
        MMROrganizer* mrOrganizer = &iMRInfoObject->MROrganizerL();
        CCalUser* organizer = CCalUser::NewL(mrOrganizer->Address());
        CleanupStack::PushL(organizer);
        if ( mrOrganizer->CommonName().Length() > 0 )
            {
            organizer->SetCommonNameL(mrOrganizer->CommonName());
            }

        iConvertedEntry->SetOrganizerL(organizer);
        CleanupStack::Pop(organizer);
        }

    if (IsFieldSupported(
            iSupportedFields, MMRInfoObject::EESMRInfoFieldAttendee) ||
        IsFieldSupported(
                iSupportedFields, MMRInfoObject::EESMRInfoFieldOptAttendee) )
        {
        RPointerArray<MMRAttendee> attendeesArray = iMRInfoObject->AttendeesL();
        // go through attendee array and append attendees
        FillAttendeesL(attendeesArray, *iConvertedEntry );
        }

    if (IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldStartDateTime) &&
        IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldStopDateTime) )
        {
        TTime start = iMRInfoObject->StartTimeInUtcL();
        TTime end = iMRInfoObject->EndTimeInUtcL();
        TCalTime calStart;
        calStart.SetTimeUtcL(start);
        TCalTime calEnd;
        calEnd.SetTimeUtcL(end);
        iConvertedEntry->SetStartAndEndTimeL(calStart, calEnd);
        }


    if (IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldLocation))
        {
        TPtrC location( iMRInfoObject->LocationL() );
        iConvertedEntry->SetLocationL( location );
        }

    if ( IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldStartDateTime) &&
         IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldAlarm))
        {
        CCalAlarm* alarm = CCalAlarm::NewL();
        CleanupStack::PushL( alarm );

        TTime start = iMRInfoObject->StartTimeInUtcL();
        TTime alarmTime = iMRInfoObject->AlarmInUtcL();

        TDateTime startDateTime = start.DateTime();
        TDateTime alarmDateTime = alarmTime.DateTime();

        TTimeIntervalMinutes minutesBetween;
        TInt err = start.MinutesFrom(alarmTime, minutesBetween);

        if ( KErrNone == err && minutesBetween.Int() > 0 )
            {
            alarm->SetTimeOffset( minutesBetween );
            // this method copies alarm details and does not take ownership 
            iConvertedEntry->SetAlarmL( alarm );
            }
        // that's why we can destroy it here
        CleanupStack::PopAndDestroy( alarm );
        }

    if (IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldSubject))
        {
        TPtrC summary( iMRInfoObject->SubjectL() );
        iConvertedEntry->SetSummaryL(summary);
        }

    if (IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldDescription))
        {
        TPtrC description( iMRInfoObject->DescriptionL() );
        iConvertedEntry->SetDescriptionL(description);
        }

    if (IsFieldSupported(iSupportedFields, MMRInfoObject::EESMRInfoFieldPriority))
        {
        iConvertedEntry->SetPriorityL( iMRInfoObject->PriorityL() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::FillCommonFieldsL
// ---------------------------------------------------------------------------
//
void CESMRInfoIcalRetrieverCmd::FillRecurrenceL()
    {
    FUNC_LOG;
    if (IsFieldSupported(
            iSupportedFields,
            MMRInfoObject::EESMRInfoFieldRecurrence))
        {
        CESMRInfoRecurrenceHandler* recurrenceHandler =
                CESMRInfoRecurrenceHandler::NewLC();

        recurrenceHandler->ParseRecurrenceInforationL(
                    *iConvertedEntry,
                    *iMRInfoObject );

        CleanupStack::PopAndDestroy( recurrenceHandler );
        }
    }

// ---------------------------------------------------------------------------
// CESMRInfoIcalRetrieverCmd::FillAttachmentInfoL
// ---------------------------------------------------------------------------
//
void CESMRInfoIcalRetrieverCmd::FillAttachmentInfoL()
    {
    FUNC_LOG;
    CFSMailMessage* msg = Message();

    if ( msg->IsFlagSet( EFSMsgFlag_Attachments ) )
        {
        RPointerArray<CFSMailMessagePart> attachmentParts;
        CleanupStack::PushL(
                TCleanupItem(
                    ESMRIcalViewerUtils::MessagePartPointerArrayCleanup,
                    &attachmentParts    ) );

        msg->AttachmentListL( attachmentParts );

        TInt attachmentCount( attachmentParts.Count() );
        if ( attachmentCount > 0 )
            {
            delete iAttachmentInfo;
            iAttachmentInfo = NULL;
            
            iInputParameters.iAttachmentInfo = NULL;

            CESMRAttachmentInfo* attachmentInfo = CESMRAttachmentInfo::NewL();
            CleanupStack::PushL( attachmentInfo );

            for( TInt i(0); i < attachmentCount; ++i )
                {
                CESMRAttachment::TESMRAttachmentState state(
                        CESMRAttachment::EAttachmentStateDownloaded );

                if ( EFSFull != attachmentParts[i]->FetchLoadState() )
                    {
                    state = CESMRAttachment::EAttachmentStateNotDownloaded;
                    }

                TInt contentSize( attachmentParts[i]->ContentSize() );
                TPtrC attachmentName( attachmentParts[i]->AttachmentNameL() );
                if ( contentSize >= 0 && attachmentName.Length() )
                    {
                    attachmentInfo->AddAttachmentInfoL(
                            attachmentName,
                            contentSize,
                            state );
                    }
                }

            if ( attachmentInfo->AttachmentCount() )
                {
                iAttachmentInfo = attachmentInfo;
                CleanupStack::Pop( attachmentInfo );
                // ownership does not change
                iInputParameters.iAttachmentInfo = iAttachmentInfo;
                }
            else
                {
                CleanupStack::PopAndDestroy( attachmentInfo );
                }

            attachmentInfo = NULL;
            }
        CleanupStack::PopAndDestroy(); // attachmentparts
        }
    }

// EOF

