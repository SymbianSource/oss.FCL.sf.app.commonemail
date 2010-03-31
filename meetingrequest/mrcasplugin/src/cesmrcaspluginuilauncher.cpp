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
* Description: This file implements class CESMRCasPluginUiLauncher.
 *
*/


#include "emailtrace.h"
#include "cesmrcaspluginuilauncher.h"
#include "mesmrcaspluginuilauncherobserver.h"
#include "esmrinternaluid.h"
// <cmail> Profiler removed. </cmail>

#include <badesca.h>
//<cmail>
#include "tfsccontactactionpluginparams.h"
#include <mvpbkstorecontact.h>
#include <caleninterimutils2.h>
#include <cmrmailboxutils.h>
#include <calsession.h>
#include <calentry.h>
#include <cagnentryui.h>
#include <caluser.h>
#include "mfscactionutils.h"
//</cmail>
#include <aknpopup.h>
#include <aknlists.h>
#include <utf.h>
#include <ecom/ecom.h>
#include "esmrcommands.h"
#include <esmrcasplugindata.rsg>
#include <stringloader.h>
#include <aknnotewrappers.h>


// Unnamed namespace for local definitions and functions
namespace { // codescanner::namespace

#ifdef _DEBUG
    enum TPanic
        {
        EPanicIndexOOB = 1
        };

    void Panic( TPanic aPanic )
        {
        _LIT( KCntxt, "CESMRCasPluginUiLauncher" );
        User::Panic( KCntxt(), aPanic );
        }
#endif // _DEBUG

const TUint KMRSequenceNo( 0 );
const TInt KOneHour( 1 );

// Constants for rounding up MR start time
const TInt KMinuteRoundToHourStart( 46 );
const TInt KMinuteRoundToHourEnd( 15 );

const TInt KHourMinutes( 0 );
const TInt KHalfHourMinutes( 30 );

/**
 * Prompts user to select default mailbox.
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

    RImplInfoPtrArrayOwn implArray;
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
             __ASSERT_DEBUG( aMailBoxes.Count() > index, Panic( EPanicIndexOOB ) );
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

}//namespace


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::CESMRCasPluginUiLauncher
// ---------------------------------------------------------------------------
//
inline CESMRCasPluginUiLauncher::CESMRCasPluginUiLauncher(
        CCalSession& aCalSession,
        CCalenInterimUtils2& aCalUtils,
        CMRMailboxUtils& aMBUtils,
        const TFscContactActionPluginParams& aParams,
        MESMRCasPluginUiLauncherObserver& aExecuteObserver )
:   iInParams(
            TUid::Uid( KUidCalendarApplication ),
            aCalSession,
            MAgnEntryUi::ECreateNewEntry ),
    iCalSession( aCalSession ),
    iParams( aParams ),
    iCalUtils( aCalUtils ),
    iMBUtils( aMBUtils ),
    iExecuteObserver( aExecuteObserver )
    {
    FUNC_LOG;
    iInParams.iMsgSession = NULL;
    iInParams.iSpare = 0;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::~CESMRCasPluginUiLauncher
// ---------------------------------------------------------------------------
//
CESMRCasPluginUiLauncher::~CESMRCasPluginUiLauncher()
    {
    FUNC_LOG;
    delete iMtmUid;
    delete iCalEntry;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::NewL
// ---------------------------------------------------------------------------
//
CESMRCasPluginUiLauncher* CESMRCasPluginUiLauncher::NewL(
        CCalSession& aCalSession,
        CCalenInterimUtils2& aCalUtils,
        CMRMailboxUtils& aMBUtils,
        const TFscContactActionPluginParams& aParams,
        MESMRCasPluginUiLauncherObserver& aExecuteObserver )
    {
    FUNC_LOG;
    CESMRCasPluginUiLauncher* self =
            new (ELeave ) CESMRCasPluginUiLauncher(
                    aCalSession,
                    aCalUtils,
                    aMBUtils,
                    aParams,
                    aExecuteObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginUiLauncher::ConstructL()
    {
    FUNC_LOG;
    //nothing to do here
    }


// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::LaunchMeetingRequestViewL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginUiLauncher::LaunchMeetingRequestViewL(
        const RPointerArray<CCalUser>& aAttendees  )
    {
    FUNC_LOG;

    HBufC8* guidBuf = iCalUtils.GlobalUidL();
    CleanupStack::PushL( guidBuf );

    delete iCalEntry; iCalEntry = NULL;
    iCalEntry = CCalEntry::NewL(
                    CCalEntry::EAppt,
                    guidBuf,
                    CCalEntry::EMethodRequest,
                    KMRSequenceNo );

    CleanupStack::Pop (guidBuf ); // Ownership is transferred

    AddOrganizerL();
    AddAttendeesL( aAttendees );
    SetStartAndEndTimeL();

    // Launch Entry UI
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL ( entries );
    entries.AppendL( iCalEntry );

    MAgnEntryUi* editorsPlugin =  NULL;
    if ( iCalUtils.MRViewersEnabledL( ETrue ) )
        {
        //CAgnEntryUi resolves Meeting request viewer based on mtm uid
        TBuf8<KMaxUidName> mrMtm;
        CnvUtfConverter::ConvertFromUnicodeToUtf8(
                mrMtm,
                *iMtmUid );
        editorsPlugin = CAgnEntryUi::NewL( mrMtm );
        CleanupDeletePushL(editorsPlugin );
        }
    else
        {
        // MR is not supported --> Leave
        User::Leave (KErrNotSupported );
        }

    editorsPlugin->ExecuteViewL( entries, iInParams, iOutParams, *this );

    if (iOutParams.iAction == MAgnEntryUi::EMeetingSaved)
    	{
    	TInt res = R_FS_MR_NOTE_SAVED;
        HBufC* prompt = StringLoader::LoadLC( res, CEikonEnv::Static() );// codescanner::eikonenvstatic
        CAknConfirmationNote* dialog = new( ELeave )CAknConfirmationNote();
        dialog->ExecuteLD( *prompt );
        CleanupStack::PopAndDestroy( prompt );
    	}

    CleanupStack::PopAndDestroy( editorsPlugin );
    CleanupStack::PopAndDestroy( &entries );

    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginUiLauncher::ProcessCommandL(
        TInt aCommandId )
    {
    FUNC_LOG;
    ProcessCommandWithResultL( aCommandId );
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::ProcessCommandWithResultL
// ---------------------------------------------------------------------------
//
TInt CESMRCasPluginUiLauncher::ProcessCommandWithResultL(
        TInt aCommandId )
    {
    FUNC_LOG;
    if ( aCommandId == EESMRCmdEditorInitializationComplete )
        {
        iExecuteObserver.MREditorInitializationComplete();
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::IsCommandAvailable
// ---------------------------------------------------------------------------
//
TBool CESMRCasPluginUiLauncher::IsCommandAvailable(
        TInt aCommandId )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( aCommandId == EESMRCmdEditorInitializationComplete )
        {
        retVal = ETrue;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::AddOrganizerL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginUiLauncher::AddOrganizerL()
    {
    FUNC_LOG;
    CMRMailboxUtils::TMailboxInfo defaultMailBox;
    if ( KErrNotFound == iMBUtils.GetDefaultMRMailBoxL(defaultMailBox) )
        {
        RArray<CMRMailboxUtils::TMailboxInfo> mailBoxes;
        CleanupClosePushL( mailBoxes );

        SupportedMailboxesL( iMBUtils, mailBoxes );

        TInt selectedMailbox( PromptForDefaultMailboxL(mailBoxes) );

        if ( KErrCancel != selectedMailbox )
            {
            iMBUtils.SetDefaultMRMailBoxL( mailBoxes[selectedMailbox].iEntryId );
            iMBUtils.GetDefaultMRMailBoxL(defaultMailBox);
            }
        CleanupStack::PopAndDestroy( &mailBoxes );

        // This will leave if user cancelled the mailbox selection
        User::LeaveIfError( selectedMailbox );
        }

    delete iMtmUid; iMtmUid = NULL;
    iMtmUid = defaultMailBox.iMtmUid.Name().AllocL();
    
    // iMtmUid needs to be upper case due to resource file definition, 
    // so let's make sure 
    TPtr mtm( iMtmUid->Des() );
    mtm.UpperCase();

    //Set the organizer from the selected mailbox
    CCalUser* organizer = CCalUser::NewL( defaultMailBox.iEmailAddress );
    CleanupStack::PushL(organizer );
    iCalEntry->SetOrganizerL(organizer );
    CleanupStack::Pop( organizer ); // Ownership trasferred
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::AddAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginUiLauncher::AddAttendeesL(
        const RPointerArray<CCalUser>& aAttendees   )
    {
    FUNC_LOG;
    //Add attendees from contact set
    TInt attendeeCount( aAttendees.Count() );
    for (TInt i=0; i < attendeeCount; ++i )
        {
        CCalAttendee* attendee = CCalAttendee::NewL( aAttendees[i]->Address() );
        CleanupStack::PushL( attendee );

        attendee->SetCommonNameL( aAttendees[i]->CommonName() );
        attendee->SetRoleL( CCalAttendee::EReqParticipant );

        iCalEntry->AddAttendeeL( attendee );

        CleanupStack::Pop( attendee );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginUiLauncher::SetStartAndEndTimeL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginUiLauncher::SetStartAndEndTimeL()
    {
    FUNC_LOG;
    // Adjust start time to future
    TTime start;
    start.UniversalTime();
    start = start + TTimeIntervalMinutes( KHalfHourMinutes );
    
    // Round up start time. 
    // Minutes between 46 ... 15 ==> 00
    // Minutes between 16 ... 45 ==> 30
    
    TDateTime currentTime = start.DateTime();
    TInt minute( currentTime.Minute() );
    
    if ( minute >= KMinuteRoundToHourStart || 
         minute <= KMinuteRoundToHourEnd   )
        {
        // Rounding to hour
        currentTime.SetMinute( KHourMinutes );
        }
    else
        {
        currentTime.SetMinute( KHalfHourMinutes );
        }
    
    start = currentTime;
    
    TTime end = start + TTimeIntervalHours( KOneHour );

    TCalTime startCalTime;
    startCalTime.SetTimeUtcL( start );

    TCalTime endCalTime;
    endCalTime.SetTimeUtcL( end );

    iCalEntry->SetStartAndEndTimeL( startCalTime, endCalTime );

    // Set time for input parameters
    TUint8 timemode = iCalEntry->StartTimeL().TimeMode ( );
    if (timemode == TCalTime::EFloating )
        {
        iInParams.iInstanceDate.SetTimeLocalFloatingL( start );
        }
    else
        {
        iInParams.iInstanceDate.SetTimeUtcL (start );
        }
    }

// EOF

