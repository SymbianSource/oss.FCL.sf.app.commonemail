/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Static helper methods
*
*/


// INCLUDE FILES
#include "esmrhelper.h"
#include "esmrentryhelper.h"
#include "esmrconfig.hrh"
#include <f32file.h>
#include <caluser.h>
#include <miuthdr.h>
#include <msvapi.h>
#include <msvuids.h>
#include <miutpars.h>
#include <bautils.h>
#include <coemain.h>
#include <msvids.h>
#include <senduiconsts.h>
#include <caleninterimutils2.h>
#include <calalarm.h>
#include <calrrule.h>
#include <calattachment.h>
#include <centralrepository.h>
#include <cmrmailboxutils.h>
#include <ct/rcpointerarray.h>
#include <utf.h>
#include <aknlists.h>
#include <aknpopup.h>
#include <calendarinternalcrkeys.h>
#include <data_caging_path_literals.hrh>
//<cmail> hardcoded paths removal
#include <pathinfo.h>
//</cmail>

#include "emailtrace.h"

// CONSTANTS

/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

_LIT( KMailtoMatchPattern, "mailto:*" ); // these are never localized
const TInt KMailtoLength = 7; // "mailto:" length

// Resource file format
_LIT( KResourceFileLocFormat1, "r%02d");
_LIT( KResourceFileLocFormat2, "r%d");

const TInt KMinTwoDigitLanguageCode = 10;

_LIT (KResourceFormat, "rsc" );

// Definition for max hours, minutes, seconds in day for Memo
const TInt KMaxHoursForMemo( 23 );
const TInt KMaxMinutesForMemo( 59 );
const TInt KMaxSecondsForMemo( 59 );

// Definition for max hours, minutes, seconds in day for To-Do
const TInt KMaxHoursForTodo( 23 );
const TInt KMaxMinutesForTodo( 59 );
const TInt KMaxSecondsForTodo( 00 );

const TInt KDefaultAnniversaryAlarmHours( 12 );

const TInt KDefaultTodoAlarmHours( 12 );

// Definition for default alarm time for meeting
const TInt KDefaultMeetingAlarmMinutes( 15 );

// Constant for default meeeting start hour
const TInt KDefaultMeetingStartHour( 8 );

// Constant for default meeeting end hour
const TInt KDefaultMeetingEndHour( 9 );


#ifdef _DEBUG

enum TPanicCode
    {
    EPanicNullMsvId = 1,
    EIllegalMsvEntryType,
    EPanicNoOrganizer,
    EInvalidEntryType,
    EInvalidSwitchType
    };

// Panic message definition
_LIT( KPanicMsg, "ESMRHelper" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

#endif // _DEBUG

CCalEntry::TType MapType( TESMRCalendarEventType aEntryType )
    {
    CCalEntry::TType type( CCalEntry::EAppt );

    switch ( aEntryType )
        {
        case EESMREventTypeAppt:
        case EESMREventTypeMeetingRequest:
            {
            type = CCalEntry::EAppt;
            break;
            }
        case EESMREventTypeETodo:
            {
            type = CCalEntry::ETodo;
            break;
            }
        case EESMREventTypeEEvent:
            {
            type = CCalEntry::EEvent;
            break;
            }
        case EESMREventTypeEReminder:
            {
            type = CCalEntry::EReminder;
            break;
            }
        case EESMREventTypeEAnniv:
            {
            type = CCalEntry::EAnniv;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidEntryType ) );
            }
        }

    return type;
    }

/**
 * Fetches the supported mailboxes.
 * @param aMBUtils utils class
 * @param aMailBoxes to test
 */
void SupportedMailboxesL(
        CMRMailboxUtils& aMBUtils,
        RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes )
    {
    FUNC_LOG;
    aMBUtils.ListMailBoxesL( aMailBoxes );

    RCPointerArray<CImplementationInformation> implArray;
    CleanupClosePushL( implArray );

    //Get all MRViewers Implementation
    const TUid mrViewersIface = TUid::Uid(KMRViewersInterfaceUID);
    REComSession::ListImplementationsL(mrViewersIface, implArray );
    TInt mrviewerCount( implArray.Count() );

    TInt index(0);
    TInt mailboxCount( aMailBoxes.Count() );
    while ( index < mailboxCount )
         {
         TBool supported( EFalse );

         for ( TInt i(0); (i < mrviewerCount) && !supported; ++ i )
             {
             TBuf16<KMaxUidName> mbName;
             CnvUtfConverter::ConvertToUnicodeFromUtf8(
                     mbName,
                     implArray[i]->DataType() );

             if( aMailBoxes[index].iMtmUid.Name().CompareF(mbName) == 0)
                  {
                  supported = ETrue;
                 }
             }

         if ( supported )
             {
             index++;
             }
         else
             {
             aMailBoxes.Remove( index );
             mailboxCount = aMailBoxes.Count();
             }
         }
    CleanupStack::PopAndDestroy( &implArray );
    }

/**
 * Prompts user to select default mailbox.
 * @param aMailBoxes Reference to mailbox list
 */
TInt PromptForDefaultMailboxL(
        RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes )
    {
    FUNC_LOG;
    TInt selected( KErrCancel );

    TInt mbCount = aMailBoxes.Count();
    if( mbCount > 0)
        {
        CAknSinglePopupMenuStyleListBox* list =
            new (ELeave) CAknSinglePopupMenuStyleListBox;
        CleanupStack::PushL(list);

        CAknPopupList* popupList = CAknPopupList::NewL(
                                            list,
                                            R_AVKON_SOFTKEYS_OK_CANCEL);
        CleanupStack::PushL(popupList);

        list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
        list->CreateScrollBarFrameL(ETrue);
        list->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

        CEikonEnv* eikEnv = CEikonEnv::Static();// codescanner::eikonenvstatic

        CDesCArrayFlat* items = new (ELeave)CDesCArrayFlat(mbCount);
        CleanupStack::PushL(items);
        for(TInt i=0; i<mbCount; ++i)
            {
            items->AppendL( aMailBoxes[i].iName );
            }
        CleanupStack::Pop(items);
        CTextListBoxModel* model = list->Model();

        //Pass ownersip of items to model
        model->SetItemTextArray(items);

        HBufC* title = KNullDesC().AllocLC();
        popupList->SetTitleL(*title);
        CleanupStack::PopAndDestroy(title);

        TBool accepted = popupList->ExecuteLD();
        CleanupStack::Pop( popupList );

        if(accepted)
            {
            selected = list->CurrentItemIndex();
            }
        else
            {
            selected = KErrCancel;
            }

        CleanupStack::PopAndDestroy( list );
        }
    else
        {
        //No mailboxes defined.  Could prompt user to define one here.
        selected = KErrCancel;
        }

    return selected;
    }

/**
 * Converts to-do specific priority to normal priority
 * @param Calendar entry, which priority is converted
 */
void ConvertTodoPriorityToNormalL( CCalEntry& entry )
    {
    FUNC_LOG;
    TInt priority( 0 );
    if( entry.PriorityL() == EFSCalenTodoPriorityHigh )
        {
        priority = EFSCalenMRPriorityHigh;
        }

    else if( entry.PriorityL() == EFSCalenTodoPriorityNormal )
        {
        priority = EFSCalenMRPriorityNormal;
        }

    else if( entry.PriorityL() == EFSCalenTodoPriorityLow )
        {
        priority = EFSCalenMRPriorityLow;
        }

    else
        {
        // Priority unknown, let's set it to normal then
        priority = EFSCalenMRPriorityNormal;
        }

    entry.SetPriorityL( priority );
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// ESMRHelper::CopyAttendeeL
// ----------------------------------------------------------------------------
//
EXPORT_C CCalAttendee* ESMRHelper::CopyAttendeeL( CCalAttendee& aSource )
    {
    FUNC_LOG;
    CCalAttendee* copy = CopyAttendeeLC( aSource );
    CleanupStack::Pop( copy );
    return copy;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CopyAttendeeLC
// ----------------------------------------------------------------------------
//
EXPORT_C CCalAttendee* ESMRHelper::CopyAttendeeLC( CCalAttendee& aSource )
    {
    FUNC_LOG;
    CCalAttendee* copy = CCalAttendee::NewL( aSource.Address(),
                                             aSource.SentBy() );
    CleanupStack::PushL( copy );
    copy->SetCommonNameL( aSource.CommonName() );
    copy->SetRoleL( aSource.RoleL() );
    copy->SetStatusL( aSource.StatusL() );
    copy->SetResponseRequested( aSource.ResponseRequested() );
    return copy;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CopyUserL
// ----------------------------------------------------------------------------
//
EXPORT_C CCalUser* ESMRHelper::CopyUserL( CCalUser& aSource )
    {
    FUNC_LOG;
    CCalUser* copy = CopyUserLC( aSource );
    CleanupStack::Pop( copy );
    return copy;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CopyUserLC
// ----------------------------------------------------------------------------
//
EXPORT_C CCalUser* ESMRHelper::CopyUserLC( CCalUser& aSource )
    {
    FUNC_LOG;
    CCalUser* copy = CCalUser::NewL( aSource.Address(),
                                     aSource.SentBy() );
    CleanupStack::PushL( copy );
    copy->SetCommonNameL( aSource.CommonName() );
    return copy;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CopyEntryL
// ----------------------------------------------------------------------------
//
EXPORT_C CCalEntry* ESMRHelper::CopyEntryL(
    const CCalEntry& aEntry,
    CCalEntry::TMethod aMethod,
    TCopyFields aCopyType,
    TESMRCalendarEventType aEntryType )
    {
    FUNC_LOG;
    CCalEntry* copy = CopyEntryLC( aEntry, aMethod, aCopyType, aEntryType );
    CleanupStack::Pop( copy );
    return copy;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CopyEntryLC
// ----------------------------------------------------------------------------
//
EXPORT_C CCalEntry* ESMRHelper::CopyEntryLC(
    const CCalEntry& aEntry,
    CCalEntry::TMethod aMethod,
    TCopyFields aCopyType,
    TESMRCalendarEventType aEntryType )
    {
    FUNC_LOG;
    CCalEntry* copy = NULL;

    TESMRCalendarEventType type = ESMREntryHelper::EventTypeL( aEntry );

    if( aEntryType == EESMREventTypeNone ||
            type == aEntryType )
        {
        HBufC8* uid = aEntry.UidL().AllocLC();

        CCalEntry::TType entryType = aEntry.EntryTypeL();

        if ( aEntryType != EESMREventTypeNone )
            {
            entryType = MapType( aEntryType );
            }

        if ( ESMREntryHelper::IsModifyingEntryL( aEntry ) )
            {
            copy = CCalEntry::NewL( entryType,
                                    uid,
                                    aMethod,
                                    aEntry.SequenceNumberL(),
                                    aEntry.RecurrenceIdL(),
                                    aEntry.RecurrenceRangeL() );
            }
        else
            {
            copy = CCalEntry::NewL( entryType,
                                    uid,
                                    aMethod,
                                    aEntry.SequenceNumberL() );
            }
        CleanupStack::Pop( uid ); // ownership transferred to the copy entry
        CleanupStack::PushL( copy );

        CopyFieldsL( aEntry, *copy, aCopyType );
        copy->SetLastModifiedDateL();

        }
    else
        {
        copy = CopyEntryAndSwitchTypeL( aEntry, aEntryType );
        CleanupStack::PushL( copy );
        }

    return copy;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CopyFieldsL
// ----------------------------------------------------------------------------
//
EXPORT_C void ESMRHelper::CopyFieldsL(
    const CCalEntry& aSource,
    CCalEntry& aTarget,
    TCopyFields aCopyType )
    {
    FUNC_LOG;
    if ( aCopyType == ECopyFull )
        {
        CCalEntry::TMethod tmpMethod( aTarget.MethodL() );
        aTarget.CopyFromL( aSource );
        // We must re-set the method to the original value
        aTarget.SetMethodL( tmpMethod );

        if ( Time::NullTTime() == aTarget.DTStampL().TimeUtcL() )
            {
            TCalTime dtStamp( aSource.DTStampL() );
            if ( Time::NullTTime() == dtStamp.TimeUtcL() )
                {
                TTime now; now.UniversalTime();
                dtStamp.SetTimeUtcL( now );
                }
            aTarget.SetDTStampL( dtStamp );
            }
        }
    else
        {
        if ( aCopyType == ECopyOrganizer )
            {
            __ASSERT_DEBUG( aSource.OrganizerL(), Panic( EPanicNoOrganizer ) );
            CCalUser* organizer = CopyUserLC( *( aSource.OrganizerL() ) );
            aTarget.SetOrganizerL( organizer );
            CleanupStack::Pop( organizer ); // ownership transferred
            }

        // these are required for entries (start and end time actually not
        // for cancels and responses, but won't do any harm either):
        aTarget.SetStartAndEndTimeL( aSource.StartTimeL(), aSource.EndTimeL() );
        aTarget.SetDTStampL( aSource.DTStampL() );
        }
    }

// ----------------------------------------------------------------------------
// ESMRHelper::AddressWithoutMailtoPrefix
// ----------------------------------------------------------------------------
//
EXPORT_C TPtrC ESMRHelper::AddressWithoutMailtoPrefix( const TDesC& aAddress )
    {
    FUNC_LOG;
    TPtrC addrWithoutPrefix( aAddress );
    TInt pos = KErrNotFound;

    do
        {
        pos = addrWithoutPrefix.MatchF( KMailtoMatchPattern );
        if ( pos != KErrNotFound )
        {
            addrWithoutPrefix.Set( addrWithoutPrefix.Mid( KMailtoLength ) );
            }
        }
    while ( pos != KErrNotFound );

    return addrWithoutPrefix;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::GetCorrectDllDriveL
// ----------------------------------------------------------------------------
//
EXPORT_C void ESMRHelper::GetCorrectDllDriveL( TFileName& aDriveName )
    {
    FUNC_LOG;
    TParse parse;
    Dll::FileName( aDriveName );
    User::LeaveIfError( parse.Set( aDriveName, NULL, NULL ) );
    aDriveName = parse.Drive(); // contains drive, e.g. "c:"
    }

// ----------------------------------------------------------------------------
// ESMRHelper::LoadResourceL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt ESMRHelper::LoadResourceL( // codescanner::intleaves
    const TDesC& aResourceFile,
    const TDesC& aResourcePath )
    {
    FUNC_LOG;
    TFileName pathAndFile;
    // contains drive, e.g. "c:"
    ESMRHelper::GetCorrectDllDriveL( pathAndFile );
    // e.g. "c:MyResource.rsc"
    pathAndFile.Append( aResourceFile );

    TParse parse;
    parse.Set( pathAndFile, &aResourcePath, NULL );
    pathAndFile = parse.FullName(); // now we have full (unlocalized) file name

    // Find the resource file for the nearest language
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(),
                                    pathAndFile );
    TInt offset = CCoeEnv::Static()->AddResourceFileL( pathAndFile );
    return offset;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CorrespondingSmtpServiceL
// ----------------------------------------------------------------------------
//
EXPORT_C TMsvId ESMRHelper::CorrespondingSmtpServiceL(
            TMsvId aRelatedService,
            CMsvSession& aMsvSession )
    {
    FUNC_LOG;
    TMsvEntry entry;
    TMsvId    dummyService;

    User::LeaveIfError( aMsvSession.GetEntry( aRelatedService,
                                              dummyService,
                                              entry ) );
    TMsvId smtpId( KMsvNullIndexEntryId );

    switch ( entry.iMtm.iUid )
        {
        case KSenduiMtmImap4UidValue: // flow through
        case KSenduiMtmPop3UidValue:
            {
            // In these cases smtp entry is available in iRelatedId:
            smtpId = entry.iRelatedId;
            break;
            }
        case KSenduiMtmSmtpUidValue:
        case KSenduiMtmSyncMLEmailUidValue:
            {
            // In these cases we already know the msvid for the smtp settings
            // (for syncml there is also smtp settings):
            smtpId = aRelatedService;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    return smtpId;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::PopulateChildFromParentL
// ----------------------------------------------------------------------------
//
EXPORT_C void ESMRHelper::PopulateChildFromParentL (
        CCalEntry &aChild,
        const CCalEntry &aParent)
    {
    FUNC_LOG;
    if ( aChild.DescriptionL() == KNullDesC )
        {
        aChild.SetDescriptionL( aParent.DescriptionL() );
        }
    if ( aChild.LocationL() == KNullDesC )
        {
        aChild.SetLocationL( aParent.LocationL() );
        }
    if ( aChild.PriorityL() == 0 )
        {
        // zero is undefined priority according to iCal spec.
        aChild.SetPriorityL( aParent.PriorityL() );
        }
    if ( aChild.SummaryL() == KNullDesC )
        {
        aChild.SetSummaryL( aParent.SummaryL() );
        }
    if ( aChild.StatusL() == CCalEntry::ENullStatus )
        {
        aChild.SetStatusL( aParent.StatusL() );
        }
    if (aChild.ReplicationStatusL() != aParent.ReplicationStatusL())
        {
        aChild.SetReplicationStatusL(aParent.ReplicationStatusL());
        }
    if (aChild.MethodL() == CCalEntry::EMethodNone)
        {
        aChild.SetMethodL(aParent.MethodL());
        }

    CCalAlarm* childAlarm = aChild.AlarmL();
    CleanupStack::PushL( childAlarm );
    CCalAlarm* parentAlarm = aChild.AlarmL();
    CleanupStack::PushL( parentAlarm );

    if( !childAlarm && parentAlarm )
        {
        aChild.SetAlarmL( parentAlarm );
        }
    CleanupStack::PopAndDestroy( parentAlarm );
    CleanupStack::PopAndDestroy( childAlarm );

    // Organizer
    if ( !aChild.OrganizerL() && aParent.OrganizerL() )
        {
        CCalUser* owner =
                ESMRHelper::CopyUserLC( *( aParent.OrganizerL() ) );
        aChild.SetOrganizerL( owner );
        CleanupStack::Pop(owner); // ownership transferred
        }

    // Attendees
    RPointerArray<CCalAttendee>& childAtt = aChild.AttendeesL();
    RPointerArray<CCalAttendee>& parentAtt = aParent.AttendeesL();

    CCalUser *po = NULL;
    if ( !aChild.PhoneOwnerL() )
        { // Phone owner is internal data, not part of ical protocol, it's
          // benefit is to avoid heavy "who am I" resolving operation
        po = aParent.PhoneOwnerL();
        }
    TInt parentAttCount( parentAtt.Count() );
    if ( childAtt.Count() == 0 && parentAttCount > 0 )
        {
        for ( TInt i( 0 ); i < parentAttCount; ++i )
            {
            CCalAttendee* copy =
                    ESMRHelper::CopyAttendeeLC( *( parentAtt[i] ) );
            aChild.AddAttendeeL( copy );
            CleanupStack::Pop(copy); // ownership transferred
            if(po == parentAtt[i])
                {
                aChild.SetPhoneOwnerL(copy);
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// ESMRHelper::LocateResourceFile
// ----------------------------------------------------------------------------
//
EXPORT_C TInt ESMRHelper::LocateResourceFile(
        const TDesC& aResource,
        const TDesC& aPath,
        TFileName &aResourceFile,
        RFs* aFs )
    {
    FUNC_LOG;
    RFs* fsSession;
    if ( aFs )
        {
        fsSession = aFs;
        }
    else
        {
        fsSession=&CCoeEnv::Static()->FsSession();//codescanner::eikonenvstatic
        }

    TFindFile resourceFile( *fsSession );
    TInt err = resourceFile.FindByDir(
            aResource,
            aPath );

    if ( KErrNone == err )
        {
        aResourceFile.Copy( resourceFile.File() );
        }
    else
        {
        const TChar KFileFormatDelim( '.' );
        TFileName locResourceFile;


        TInt pos = aResource.LocateReverse( KFileFormatDelim );
        if ( pos != KErrNotFound )
            {
            locResourceFile.Copy( aResource.Mid(0, pos + 1) );

            TInt language( User::Language() );
            if (language < KMinTwoDigitLanguageCode )
                {
                locResourceFile.AppendFormat( KResourceFileLocFormat1, language );
                }
            else
                {
                locResourceFile.AppendFormat( KResourceFileLocFormat2, language );
                }

            TFindFile resourceFile( *fsSession );
            err = resourceFile.FindByDir(
                    locResourceFile,
                    aPath );

            if ( KErrNone == err )
                {
                TFileName tempName;
                tempName.Copy( resourceFile.File() );
                TInt dotpos= tempName.LocateReverse(KFileFormatDelim);
                aResourceFile.Copy(tempName.Mid(0,dotpos+1) );
                aResourceFile.Append(KResourceFormat);
                }
            }
        }


    if ( KErrNone != err )
        {
        }
    return err;
    }

//<cmail>
// ----------------------------------------------------------------------------
// ESMRHelper::CreateAndAppendPrivateDirToFileName
// ----------------------------------------------------------------------------
//
EXPORT_C TInt ESMRHelper::CreateAndAppendPrivateDirToFileName(
        TFileName& aFileName)
    {
    FUNC_LOG;

    TFileName path;
    RFs fsSession;
    TInt result = fsSession.Connect();
    if ( KErrNone == result )
        {
        TBool createPrivatePath( ETrue );

        fsSession.PrivatePath(path);

        TFindFile findFile(fsSession);
        result = findFile.FindByDir(path, KNullDesC);
        if (KErrNone == result )
            {
            // Check that we are able to drive into private path location
            createPrivatePath = EFalse;

            path = findFile.File();
            TInt drive = TDriveUnit( path );
            TInt KRomDrive = TDriveUnit( PathInfo::RomRootPath() );

            if ( drive == KRomDrive )
                {
                // Private path locates in ROM
                createPrivatePath = ETrue;
                }
            }

        TPtrC phoneMemoryPath( PathInfo::PhoneMemoryRootPath() );

        if ( createPrivatePath )
            {
            //create path if doesn't exists
            TInt drive = TDriveUnit(phoneMemoryPath);
            fsSession.CreatePrivatePath( drive );
            }

        result = findFile.FindByDir(path, KNullDesC);
        path = findFile.File();
        path[0] = phoneMemoryPath[0];
        aFileName.Insert(0,path);

        fsSession.MkDirAll(aFileName);
        result = KErrNone;
        }

    // fsSession
    fsSession.Close();
    return result;
    }
//</cmail>

// ----------------------------------------------------------------------------
// ESMRHelper::CreateAndAppendOthersDirToFileName
// ----------------------------------------------------------------------------
//
EXPORT_C TInt ESMRHelper::CreateAndAppendOthersDirToFileName(
            TFileName& aFileName)
    {
    FUNC_LOG;

    TFileName path;
    RFs fsSession;
    TInt result = fsSession.Connect();
    if ( KErrNone == result )
        {
        aFileName.Insert(0, PathInfo::OthersPath() );
        aFileName.Insert(0, PathInfo::PhoneMemoryRootPath() );

        fsSession.MkDirAll(aFileName);

        result = KErrNone;
        }

    // fsSession
    fsSession.Close();
    return result;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CopyEntryAndSwitchTypeL
// ----------------------------------------------------------------------------
//
CCalEntry* ESMRHelper::CopyEntryAndSwitchTypeL(
        const CCalEntry& aSourceEntry,
        TESMRCalendarEventType aTargetType )
    {
    FUNC_LOG;
    CCalEntry* entry = NULL;

    TESMRSwitchEventType switchType;

    switchType = ResolveSwitchType(
            ESMREntryHelper::EventTypeL( aSourceEntry ), aTargetType );

    switch ( switchType )
        {
        case EMRSwitchMRToMeeting:
            {
            entry = SwitchToMeetingL( aSourceEntry );
            break;
            }
        case EMRSwitchMRToMemo:
            {
            entry = SwitchToMemoL( aSourceEntry );
            break;
            }
        case EMRSwitchMRToAnniversary:
            {
            entry = SwitchToAnniversaryL( aSourceEntry );
            break;
            }
        case EMRSwitchMRToTodo:
            {
            entry = SwitchToTodoL( aSourceEntry );
            break;
            }
        case EMRSwitchMeetingToMR:
            {
            entry = SwitchToMRL( aSourceEntry );
            break;
            }
        case EMRSwitchMeetingToMemo:
            {
            entry = SwitchToMemoL( aSourceEntry );
            break;
            }
        case EMRSwitchMeetingToAnniversary:
            {
            entry = SwitchToAnniversaryL( aSourceEntry );
            break;
            }
        case EMRSwitchMeetingToTodo:
            {
            entry = SwitchToTodoL( aSourceEntry );
            break;
            }
        case EMRSwitchMemoToMR:
            {
            entry = SwitchToMRL( aSourceEntry );
            break;
            }
        case EMRSwitchMemoToMeeting:
            {
            entry = SwitchToMeetingL( aSourceEntry );
            break;
            }
        case EMRSwitchMemoToAnniversary:
            {
            entry = SwitchToAnniversaryL( aSourceEntry );
            break;
            }
        case EMRSwitchMemoToTodo:
            {
            entry = SwitchToTodoL( aSourceEntry );
            break;
            }
        case EMRSwitchAnniversaryToMR:
            {
            entry = SwitchToMRL( aSourceEntry );
            break;
            }
        case EMRSwitchAnniversaryToMeeting:
            {
            entry = SwitchToMeetingL( aSourceEntry );
            break;
            }
        case EMRSwitchAnniversaryToMemo:
            {
            entry = SwitchToMemoL( aSourceEntry );
            break;
            }
        case EMRSwitchAnniversaryToTodo:
            {
            entry = SwitchToTodoL( aSourceEntry );
            break;
            }
        case EMRSwitchTodoToMR:
            {
            entry = SwitchToMRL( aSourceEntry );
            break;
            }
        case EMRSwitchTodoToMeeting:
            {
            entry = SwitchToMeetingL( aSourceEntry );
            break;
            }
        case EMRSwitchTodoToMemo:
            {
            entry = SwitchToMemoL( aSourceEntry );
            break;
            }
        case EMRSwitchTodoToAnniversary:
            {
            entry = SwitchToAnniversaryL( aSourceEntry );
            break;
            }

        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidEntryType ) );
            }
        }

    return entry;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::ResolveSwitchType
// ----------------------------------------------------------------------------
//
ESMRHelper::TESMRSwitchEventType ESMRHelper::ResolveSwitchType(
        TESMRCalendarEventType aSourceType,
        TESMRCalendarEventType aTargetType )
    {
    FUNC_LOG;

    TESMRSwitchEventType type( EMRSwitchUnknown );

    if( aSourceType == EESMREventTypeMeetingRequest ) // Meeting request
        {
        if( aTargetType == EESMREventTypeAppt )
            {
            type = EMRSwitchMRToMeeting;
            }

        if( aTargetType == EESMREventTypeEEvent )
             {
             type = EMRSwitchMRToMemo;
             }

        if( aTargetType == EESMREventTypeEAnniv )
             {
             type = EMRSwitchMRToAnniversary;
             }

        if( aTargetType == EESMREventTypeETodo )
             {
             type = EMRSwitchMRToTodo;
             }
        }

    if( aSourceType == EESMREventTypeAppt ) // Meeting
        {
        if( aTargetType == EESMREventTypeMeetingRequest )
            {
            type = EMRSwitchMeetingToMR;
            }

        if( aTargetType == EESMREventTypeEEvent )
             {
             type = EMRSwitchMeetingToMemo;
             }

        if( aTargetType == EESMREventTypeEAnniv )
             {
             type = EMRSwitchMeetingToAnniversary;
             }

        if( aTargetType == EESMREventTypeETodo )
             {
             type = EMRSwitchMeetingToTodo;
             }
        }

    if( aSourceType == EESMREventTypeEEvent ) // Memo
        {
        if( aTargetType == EESMREventTypeMeetingRequest )
            {
            type = EMRSwitchMemoToMR;
            }

        if( aTargetType == EESMREventTypeAppt )
             {
             type = EMRSwitchMemoToMeeting;
             }

        if( aTargetType == EESMREventTypeEAnniv )
             {
             type = EMRSwitchMemoToAnniversary;
             }

        if( aTargetType == EESMREventTypeETodo )
             {
             type = EMRSwitchMemoToTodo;
             }
        }

    if( aSourceType == EESMREventTypeEAnniv ) // Anniversary
        {
        if( aTargetType == EESMREventTypeMeetingRequest )
            {
            type = EMRSwitchAnniversaryToMR;
            }

        if( aTargetType == EESMREventTypeAppt  )
             {
             type = EMRSwitchAnniversaryToMeeting;
             }

        if( aTargetType == EESMREventTypeEEvent  )
             {
             type = EMRSwitchAnniversaryToMemo;
             }

        if( aTargetType == EESMREventTypeETodo )
             {
             type = EMRSwitchAnniversaryToTodo;
             }
        }

    if( aSourceType == EESMREventTypeETodo ) // To-do
        {
        if( aTargetType == EESMREventTypeMeetingRequest )
            {
            type = EMRSwitchTodoToMR;
            }

        if( aTargetType == EESMREventTypeAppt )
             {
             type = EMRSwitchTodoToMeeting;
             }

        if( aTargetType == EESMREventTypeEEvent )
             {
             type = EMRSwitchTodoToMemo;
             }

        if( aTargetType == EESMREventTypeEAnniv )
             {
             type = EMRSwitchTodoToAnniversary;
             }
        }

    __ASSERT_DEBUG( type != EMRSwitchUnknown, Panic( EInvalidSwitchType ) );

    return type;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::SwitchToMemoL
// ----------------------------------------------------------------------------
//
CCalEntry* ESMRHelper::SwitchToMemoL(
        const CCalEntry& aSourceEntry )
    {
    FUNC_LOG;
    CCalEntry* entry = CreateEntryL(
                CCalEntry::EEvent,
                aSourceEntry.UidL(),
                CCalEntry::EMethodNone,
                CalCommon::EThisOnly );

    CleanupStack::PushL( entry );

    // Copy all data from existing entry to new entry
    entry->CopyFromL( aSourceEntry );
    // We must re-set the method to the original value
    entry->SetMethodL( CCalEntry::EMethodNone );

#ifdef RD_USE_PS2_APIS
    // If source entry is a meeting request...
    if( CCalenInterimUtils2::IsMeetingRequestL( *entry ) )
        {
        // ... we need to clear all MR specific data
        entry->ClearMRSpecificDataL();
        }
#endif // RD_USE_PS2_APIS

    // Memo is never a repeating event
    entry->ClearRepeatingPropertiesL();

    // Memo has start and stop date without time value.
    // Let's change the entry's start and stop times to memo specific
    // Setting start date and stop date
    TDateTime start = entry->StartTimeL().TimeLocalL().DateTime();
    start.SetHour( 0 );
    start.SetMinute( 0 );
    start.SetSecond( 0 );

    TTime tempEnd = entry->EndTimeL().TimeLocalL();

    if( ESMREntryHelper::IsAllDayEventL( aSourceEntry ) )
        {
        // If source is all day event, we need to reduce one from the day,
        // otherwise the new memo would be spanned over one day too long
        tempEnd -= TTimeIntervalDays( 1 );
        }

    TDateTime end = tempEnd.DateTime();
    end.SetHour( KMaxHoursForMemo );
    end.SetMinute( KMaxMinutesForMemo );
    end.SetSecond( KMaxSecondsForMemo );

    TCalTime startDate;
    startDate.SetTimeLocalFloatingL( start );
    TCalTime stopDate;
    stopDate.SetTimeLocalFloatingL( end );
    entry->SetStartAndEndTimeL( startDate, stopDate );

    // Memo does not have any alarm value
    entry->SetAlarmL( NULL );

    // If source entry is to-do, we need to convert the
    // priority from to-do priority to normal priority
    if( aSourceEntry.EntryTypeL() == CCalEntry::ETodo )
        {
        ConvertTodoPriorityToNormalL( *entry );
        }

    CleanupStack::Pop( entry ); // ownership transferred to caller
    return entry;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::SwitchToAnniversaryL
// ----------------------------------------------------------------------------
//
CCalEntry* ESMRHelper::SwitchToAnniversaryL(
        const CCalEntry& aSourceEntry )
    {
    FUNC_LOG;
    // Create an entry
    CCalEntry* entry = CreateEntryL(
            CCalEntry::EAnniv,
            aSourceEntry.UidL(),
            CCalEntry::EMethodNone,
            CalCommon::EThisOnly );

    CleanupStack::PushL( entry );

    // Copy all data from existing entry to new entry
    entry->CopyFromL( aSourceEntry );
    // We must re-set the method to the original value
    entry->SetMethodL( CCalEntry::EMethodNone );

#ifdef RD_USE_PS2_APIS
    // If source entry is a meeting request...
    if( CCalenInterimUtils2::IsMeetingRequestL( *entry ) )
        {
        // ... we need to clear all MR specific data
        entry->ClearMRSpecificDataL();
        }
#endif // RD_USE_PS2_APIS

    // Anniversary repeats once a year, so
    // we clear existing repeating rules...
    entry->ClearRepeatingPropertiesL();

    // ... and create a new rule, to repeat once a year
    TCalRRule rrule( TCalRRule::EYearly );
    rrule.SetDtStart( entry->StartTimeL() );
    rrule.SetInterval( 1 );
    rrule.SetCount( 0 );
    entry->SetRRuleL( rrule );

    // Anniversary default alarm is previous day at noon.
    // Let's set that also.
    TTime eventTime = entry->StartTimeL().TimeLocalL();
    TTime alarmTime = eventTime - TTimeIntervalDays( 1 );
    TDateTime alarmDateTime = alarmTime.DateTime();
    alarmDateTime.SetHour( KDefaultAnniversaryAlarmHours );
    alarmDateTime.SetMinute( 0 );
    alarmDateTime.SetSecond( 0 );
    alarmDateTime.SetMicroSecond( 0 );
    alarmTime = alarmDateTime;

    // Calculating alarm offset
    TTimeIntervalMinutes alarmOffset( 0 );
    eventTime.MinutesFrom( alarmTime, alarmOffset );

    // Creating alarm
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL( alarm );
    alarm->SetTimeOffset( alarmOffset );

    // Setting alarm
    entry->SetAlarmL( alarm );
    CleanupStack::PopAndDestroy( alarm );
    alarm = NULL;

    // If source entry is to-do, we need to convert the
    // priority from to-do priority to normal priority
    if( aSourceEntry.EntryTypeL() == CCalEntry::ETodo )
        {
        ConvertTodoPriorityToNormalL( *entry );
        }

    CleanupStack::Pop( entry ); // ownership transferred to caller
    return entry;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::SwitchToTodoL
// ----------------------------------------------------------------------------
//
CCalEntry* ESMRHelper::SwitchToTodoL(
        const CCalEntry& aSourceEntry )
    {
    FUNC_LOG;
    CCalEntry* entry = CreateEntryL(
            CCalEntry::ETodo,
            aSourceEntry.UidL(),
            CCalEntry::EMethodNone,
            CalCommon::EThisOnly );

    CleanupStack::PushL( entry );

    // Copy all data from existing entry to new entry
    entry->CopyFromL( aSourceEntry );
    // We must re-set the method to the original value
    entry->SetMethodL( CCalEntry::EMethodNone );

#ifdef RD_USE_PS2_APIS
    // If source entry is a meeting request...
    if( CCalenInterimUtils2::IsMeetingRequestL( *entry ) )
        {
        // ... we need to clear all MR specific data
        entry->ClearMRSpecificDataL();
        }
#endif // RD_USE_PS2_APIS

    // To-do is never a repeating event
    entry->ClearRepeatingPropertiesL();

    // To-do has a due date only. Let's change the entry's start and stop
    // times to To-do specific ones
    TTime tempEnd = entry->EndTimeL().TimeLocalL();

    if( ESMREntryHelper::IsAllDayEventL( aSourceEntry ) )
        {
        // If source is all day event, we need to reduce one from the day,
        // otherwise the new todos due date would be one day late
        tempEnd -= TTimeIntervalDays( 1 );
        }

    TDateTime tempDue = tempEnd.DateTime();

    tempDue.SetHour( KMaxHoursForTodo );
    tempDue.SetMinute( KMaxMinutesForTodo );
    tempDue.SetSecond( KMaxSecondsForTodo );

    TCalTime dueDate;
    dueDate.SetTimeLocalFloatingL( tempDue );

    entry->SetStartAndEndTimeL( dueDate, dueDate );

    // Setting to-do specific alarm
    if( entry->AlarmL() )
        {
        // Setting default To-do alarm for entry
        TTime eventTime = entry->EndTimeL().TimeLocalL();

        // Calculate alarm time (due date at 12:00am)
        TTime alarmTime = eventTime;
        TDateTime alarmDateTime = alarmTime.DateTime();
        alarmDateTime.SetHour( KDefaultTodoAlarmHours );
        alarmDateTime.SetMinute( 0 );
        alarmDateTime.SetSecond( 0 );
        alarmDateTime.SetMicroSecond( 0 );
        alarmTime = alarmDateTime;

        TTime currentTime;
        currentTime.HomeTime();

        // If alarm is in the future
        if ( alarmTime > currentTime )
            {
            CCalAlarm* alarm = CCalAlarm::NewL();
            CleanupStack::PushL( alarm );
            TTimeIntervalMinutes alarmOffset( 0 );
            eventTime.MinutesFrom( alarmTime, alarmOffset );
            alarm->SetTimeOffset( alarmOffset );
            entry->SetAlarmL( alarm );
            CleanupStack::PopAndDestroy( alarm );
            }
        // Alarm is in the past, let's set it to NULL
        else
            {
            entry->SetAlarmL( NULL );
            }
        }

    // Setting to-do specific priority
    TInt priority( 0 );
    if( entry->PriorityL() == EFSCalenMRPriorityHigh )
        {
        priority = EFSCalenTodoPriorityHigh;
        }

    else if( entry->PriorityL() == EFSCalenMRPriorityNormal )
        {
        priority = EFSCalenTodoPriorityNormal;
        }

    else if( entry->PriorityL() == EFSCalenMRPriorityLow )
        {
        priority = EFSCalenTodoPriorityLow;
        }

    else
        {
        // Priority unknown, let's set it to normal then
        priority = EFSCalenTodoPriorityNormal;
        }

    entry->SetPriorityL( priority );

    CleanupStack::Pop( entry ); // ownership transferred to caller
    return entry;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::SwitchToMeetingL
// ----------------------------------------------------------------------------
//
CCalEntry* ESMRHelper::SwitchToMeetingL(
        const CCalEntry& aSourceEntry )
    {
    FUNC_LOG;

    CCalEntry* entry = CreateEntryL(
            CCalEntry::EAppt,
            aSourceEntry.UidL(),
            CCalEntry::EMethodNone,
            CalCommon::EThisOnly );

    CleanupStack::PushL( entry );

    // Copy all data from existing entry to new entry
    entry->CopyFromL( aSourceEntry );

    // If source was to-do, priority needs converting
    if( aSourceEntry.EntryTypeL() == CCalEntry::ETodo )
    	{
		ConvertTodoPriorityToNormalL( *entry );
    	}

    // We must re-set the method to the original value
    entry->SetMethodL( CCalEntry::EMethodNone );

#ifdef RD_USE_PS2_APIS
    // If source entry is a meeting request...
    if( CCalenInterimUtils2::IsMeetingRequestL( *entry ) )
        {
        // ... we need to clear all MR specific data,
        // but otherwise the enrty is now converted
        // correctly from MR to meeting
        entry->ClearMRSpecificDataL();
        }
    else
#endif // RD_USE_PS2_APIS
        {
        // Repeating rules from memo, to-do and anniversary
        // are not compatible with a meeting
        entry->ClearRepeatingPropertiesL();

        // Setting meeting time, since memo, to-do and anniversary
        // start / end times are not compatible with meeting's corresponding
        // values
        SetMeetingTimeL( *entry, aSourceEntry );

        // If source is not an appointment, we'll set default
        // alarm for meeting, since memo, to-do and anniversary
        // alarms are not compatible with meeting alarm
        if( entry->AlarmL() &&
                aSourceEntry.EntryTypeL() != CCalEntry::EAppt )
            {
            SetDefaultAlarmForMeetingL( *entry );
            }

        // If source entry is to-do, we need to convert the
        // priority from to-do priority to normal priority
        if( aSourceEntry.EntryTypeL() == CCalEntry::ETodo )
            {
            ConvertTodoPriorityToNormalL( *entry );
            }
        }

    CleanupStack::Pop( entry ); // ownership transferred to caller
    return entry;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::SwitchToMRL
// ----------------------------------------------------------------------------
//
CCalEntry* ESMRHelper::SwitchToMRL(
        const CCalEntry& aSourceEntry )
    {
    FUNC_LOG;
    CCalEntry* entry = CreateEntryL(
            CCalEntry::EAppt,
            aSourceEntry.UidL(),
            CCalEntry::EMethodRequest,
            CalCommon::EThisOnly );

    CleanupStack::PushL( entry );

    // Copy all data from existing entry to new entry
    entry->CopyFromL( aSourceEntry );

    // If source was to-do, priority needs converting
    if( aSourceEntry.EntryTypeL() == CCalEntry::ETodo )
    	{
		ConvertTodoPriorityToNormalL( *entry );
    	}

    // We must re-set the method to the original value
    entry->SetMethodL( CCalEntry::EMethodRequest );

    // If source is a meeting...
    if( aSourceEntry.EntryTypeL() != CCalEntry::EAppt )
        {
        // Repeating rules from memo, to-do and anniversary
        // are not compatible with MR
        entry->ClearRepeatingPropertiesL();

        // Setting meeting time, since memo, to-do and anniversary
        // start / end times are not compatible with MR's corresponding
        // values
        SetMeetingTimeL( *entry, aSourceEntry );

        // If source is not an appointment, we'll set default
        // alarm for MR, since memo, to-do and anniversary
        // alarms are not compatible with MR alarm
        if( entry->AlarmL() &&
                aSourceEntry.EntryTypeL() != CCalEntry::EAppt )
            {
            SetDefaultAlarmForMeetingL( *entry );
            }

        // If source entry is to-do, we need to convert the
        // priority from to-do priority to normal priority
        if( aSourceEntry.EntryTypeL() == CCalEntry::ETodo )
            {
            ConvertTodoPriorityToNormalL( *entry );
            }
        }

    // Add organizer
    AddOrganizerL( *entry );

    // Set organizer as phone owner
    entry->SetPhoneOwnerL( entry->OrganizerL() );

    CleanupStack::Pop( entry ); // ownership transferred to caller
    return entry;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::CreateEntryL
// ----------------------------------------------------------------------------
//
CCalEntry* ESMRHelper::CreateEntryL(
        CCalEntry::TType aType,
        const TDesC8& aUid,
        CCalEntry::TMethod aMethod,
        CalCommon::TRecurrenceRange aRange )
    {
    FUNC_LOG;

    HBufC8* guid = aUid.AllocLC();
    CCalEntry* entry = CCalEntry::NewL( aType, guid, aMethod, aRange );
    CleanupStack::Pop( guid );

    return entry;
    }

// ----------------------------------------------------------------------------
// ESMRHelper::SetMeetingTimeL
// ----------------------------------------------------------------------------
//
void ESMRHelper::SetMeetingTimeL(
        CCalEntry& aTargetEntry,
        const CCalEntry& aSourceEntry )
    {
    FUNC_LOG;
    TCalTime start;
    TCalTime end;

    TDateTime tempStart;
    TDateTime tempEnd;

    CCalEntry::TType type = aSourceEntry.EntryTypeL();

    switch ( type )
        {
        case CCalEntry::EAppt:
            {
            start.SetTimeLocalL( aSourceEntry.StartTimeL().TimeLocalL() );
            end.SetTimeLocalL( aSourceEntry.EndTimeL().TimeLocalL() );
            break;
            }
        case CCalEntry::ETodo:
            {
            // Todo has only due date => Adding date from that,
            // Hour, minute and second as default values
            tempStart = aSourceEntry.StartTimeL().TimeLocalL().DateTime();
            tempStart.SetHour( KDefaultMeetingStartHour );
            tempStart.SetMinute( 0 );
            tempStart.SetSecond( 0 );

            tempEnd = aSourceEntry.StartTimeL().TimeLocalL().DateTime();
            tempEnd.SetHour( KDefaultMeetingEndHour );
            tempEnd.SetMinute( 0 );
            tempEnd.SetSecond( 0 );

            start.SetTimeLocalL( tempStart );
            end.SetTimeLocalL( tempEnd );
            break;
            }
        case CCalEntry::EEvent: //Memo
            {
            // Memo is one day event with start time as 00:00 and end as 23:59
            // Let's add start date from that, and set hour, minute and second
            // as default values
            tempStart = aSourceEntry.StartTimeL().TimeLocalL().DateTime();
            tempStart.SetHour( KDefaultMeetingStartHour );
            tempStart.SetMinute( 0 );
            tempStart.SetSecond( 0 );

            tempEnd = aSourceEntry.StartTimeL().TimeLocalL().DateTime();
            tempEnd.SetHour( KDefaultMeetingEndHour );
            tempEnd.SetMinute( 0 );
            tempEnd.SetSecond( 0 );

            start.SetTimeLocalL( tempStart );
            end.SetTimeLocalL( tempEnd );

            break;
            }
        case CCalEntry::EAnniv:
            {
            // Anniversary has start date, which is taken as date of the meeting.
            // Hour, minute and second are set as teh default values.
            tempStart = aSourceEntry.StartTimeL().TimeLocalL().DateTime();
            tempStart.SetHour( KDefaultMeetingStartHour );
            tempStart.SetMinute( 0 );
            tempStart.SetSecond( 0 );

            tempEnd = aSourceEntry.StartTimeL().TimeLocalL().DateTime();
            tempEnd.SetHour( KDefaultMeetingEndHour );
            tempEnd.SetMinute( 0 );
            tempEnd.SetSecond( 0 );

            start.SetTimeLocalL( tempStart );
            end.SetTimeLocalL( tempEnd );
            break;
            }

        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidEntryType ) );
            }
        }

    aTargetEntry.SetStartAndEndTimeL( start, end );
    }

// ----------------------------------------------------------------------------
// ESMRHelper::SetDefaultAlarmForMetingL
// ----------------------------------------------------------------------------
//
void ESMRHelper::SetDefaultAlarmForMeetingL(
        CCalEntry& aTargetEntry )
    {
    FUNC_LOG;

    // Get default alarm time from central repository
    TInt defaultAlarmTime;
    CRepository* repository = CRepository::NewLC( KCRUidCalendar );
    TInt err = repository->Get( KCalendarDefaultAlarmTime, defaultAlarmTime );
    CleanupStack::PopAndDestroy( repository );

    if ( err != KErrNone )
        {
        defaultAlarmTime = KDefaultMeetingAlarmMinutes;
        }

    // Getting current time
    TTime currentTime;
    currentTime.HomeTime();

    // Getting meeting start time
    TTime start = aTargetEntry.StartTimeL().TimeLocalL();

    // Create default alarm
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL( alarm );

    TTimeIntervalMinutes alarmOffset( defaultAlarmTime );

    // If alarm time is in past
    if ( ( start - alarmOffset ) < currentTime )
        {
        // Setting alarm off
        aTargetEntry.SetAlarmL( NULL );
        }
    else
        {
        // Set default alarm time
        alarm->SetTimeOffset( alarmOffset );
        aTargetEntry.SetAlarmL( alarm );
        }
    CleanupStack::PopAndDestroy( alarm );

    }

// ---------------------------------------------------------------------------
// ESMRHelper::AddOrganizerL
// ---------------------------------------------------------------------------
//
void ESMRHelper::AddOrganizerL( CCalEntry& aTargetEntry )
    {
    FUNC_LOG;
    CMRMailboxUtils* mbUtils = CMRMailboxUtils::NewL( NULL );
    CleanupStack::PushL( mbUtils );

    CMRMailboxUtils::TMailboxInfo defaultMailBox;
    TInt err = mbUtils->GetDefaultMRMailBoxL( defaultMailBox );

    if ( KErrNone != err )
        {
        RArray<CMRMailboxUtils::TMailboxInfo> mailBoxes;
        CleanupClosePushL( mailBoxes );

        SupportedMailboxesL( *mbUtils, mailBoxes );

        TInt selectedMailbox( PromptForDefaultMailboxL(mailBoxes) );

        if ( KErrCancel != selectedMailbox )
            {
            mbUtils->SetDefaultMRMailBoxL(
                    mailBoxes[selectedMailbox].iEntryId ); // codescanner::accessArrayElementWithoutCheck2
            mbUtils->GetDefaultMRMailBoxL(defaultMailBox);
            }
        CleanupStack::PopAndDestroy( &mailBoxes );

        // This will leave if user cancelled the mailbox selection
        User::LeaveIfError( selectedMailbox );
        }

    //Set the organizer from the selected mailbox
    CCalUser* organizer = CCalUser::NewL( defaultMailBox.iEmailAddress );
    CleanupStack::PushL( organizer );
    aTargetEntry.SetOrganizerL( organizer );
    CleanupStack::Pop( organizer ); // Ownership trasferred

    CleanupStack::PopAndDestroy( mbUtils );
    }

// EOF

