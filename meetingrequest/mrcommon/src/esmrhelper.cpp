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
#include "emailtrace.h"
#include "esmrhelper.h"
#include "esmrentryhelper.h"
#include <f32file.h>
#include <caluser.h>
#include <miuthdr.h>
#include <msvapi.h>
#include <msvuids.h>
#include <miutpars.h>
#include <bautils.h>
#include <coemain.h>
#include <msvids.h>
#include <SendUiConsts.h>
#include <data_caging_path_literals.hrh>
//<cmail> hardcoded paths removal
#include <pathinfo.h>
//</cmail>

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

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicNullMsvId = 1,
    EIllegalMsvEntryType,
    EPanicNoOrganizer
    };

// Panic message definition
_LIT( KPanicMsg, "ESMRHelper" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

#endif // _DEBUG

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
    TCopyFields aCopyType )
    {
    FUNC_LOG;
    CCalEntry* copy = CopyEntryLC( aEntry, aMethod, aCopyType );
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
    TCopyFields aCopyType )
    {
    FUNC_LOG;
    CCalEntry* copy = NULL;
    HBufC8* uid = aEntry.UidL().AllocLC();
    if ( ESMREntryHelper::IsModifyingEntryL( aEntry ) )
        {
        copy = CCalEntry::NewL( aEntry.EntryTypeL(),
                                uid,
                                aMethod,
                                aEntry.SequenceNumberL(),
                                aEntry.RecurrenceIdL(),
                                aEntry.RecurrenceRangeL() );
        }
    else
        {
        copy = CCalEntry::NewL( aEntry.EntryTypeL(),
                                uid,
                                aMethod,
                                aEntry.SequenceNumberL() );
        }
    CleanupStack::Pop( uid ); // ownership transferred to the copy entry
    CleanupStack::PushL( copy );

    CopyFieldsL( aEntry, *copy, aCopyType );
    copy->SetLastModifiedDateL();

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
EXPORT_C TInt ESMRHelper::CreateAndAppendPrivateDirToFileName(TFileName& aFileName)
    {
    TFileName KPath;
    RFs fsSession;
    TInt result = fsSession.Connect();
    if(result==KErrNone)
        {
        fsSession.PrivatePath(KPath);
        TFindFile findFile(fsSession);

        result = findFile.FindByDir(KPath, KNullDesC);
        if (result != KErrNone) //create path if doesn't exists
            {
            fsSession.CreatePrivatePath(TDriveUnit(PathInfo::PhoneMemoryRootPath()));
            result = findFile.FindByDir(KPath, KNullDesC);
            }
        KPath = findFile.File();
        aFileName.Insert(0,KPath);
        fsSession.MkDirAll(aFileName);
        fsSession.Close();
        }
    return result;
    }
//</cmail>

// EOF
