/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MBUtils ECOM implementation
*
*/

#include "cmrbcplugincreatenewmrcmd.h"
#include "cesmrviewerctrl.h"
#include "esmrcommands.h"
#include "tmroutputparams.h"
#include "esmrconfig.hrh"

#include <calenservices.h>
#include <calencommands.hrh>
#include <CalenInterimUtils2.h>
#include <CMRViewers.h>
#include <calentry.h>
#include <caluser.h>
#include <cmrmailboxutils.h>
#include <ct/rcpointerarray.h>
#include <aknlists.h>
#include <aknPopup.h>
#include <utf.h>
#include <bldvariant.hrh>

#include "emailtrace.h"

namespace { // codescanner::namespace

/**
 * Literal for BC viewer / editor
 */
_LIT8( KBCViewer, "[2001F406]" );

/**
 * Default sequence number
 */
const TInt KDefaultSeqNo( 0 );

/**
 * Constant for default duration in minutes
 */
const TInt KDefaultDurationInMinutes( 60 );

#ifdef _DEBUG

/**
 * Panic literal
 */
_LIT( KMRBCPluginCreateNewMRCmd, "MRBCPluginCreateNewMRCmd" );

/**
 * Panic codes
 */
enum TMRBCPluginCreateNewMRCmdPanic
	{
	EInvalidCommand // Invalid command
	};

void Panic( TMRBCPluginCreateNewMRCmdPanic aPanic )
	{
	User::Panic( KMRBCPluginCreateNewMRCmd, aPanic );
	}

#endif // _DEBUG

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
 * Test if this is valid command to be executed by this object.
 * @param aCommand Reference to command object
 * @return ETrue if this is valid command, EFalse otherwise
 */
TBool IsValidCommand(
        const TCalenCommand& aCommand )
    {
    TBool retValue( EFalse );

    TInt aCommandId( aCommand.Command() );

    if ( ECalenNewMeetingRequest == aCommandId ||
            ECalenNewMeetingTimeSpan == aCommandId )
        {
        retValue = ETrue;
        }

    return retValue;
    }

}

// -----------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::CMRBCPluginCreateNewMRCmd
// -----------------------------------------------------------------------------
//
CMRBCPluginCreateNewMRCmd::CMRBCPluginCreateNewMRCmd(
		MCalenServices& aServices )
:	iServices( aServices )
	{
	FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::~CMRBCPluginCreateNewMRCmd
// -----------------------------------------------------------------------------
//
CMRBCPluginCreateNewMRCmd::~CMRBCPluginCreateNewMRCmd()
	{
	FUNC_LOG;
	delete iEntry;
	delete iEditor;
	}

// -----------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::ConstructL
// -----------------------------------------------------------------------------
//
CMRBCPluginCreateNewMRCmd* CMRBCPluginCreateNewMRCmd::NewL(
		MCalenServices& aServices )
	{
	FUNC_LOG;

	CMRBCPluginCreateNewMRCmd* self =
			new (ELeave) CMRBCPluginCreateNewMRCmd( aServices );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CMRBCPluginCreateNewMRCmd::ConstructL()
	{
	FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CMRBCPluginCreateNewMRCmd::ExecuteCommandL(
		const TCalenCommand& aCommand )
	{
	FUNC_LOG;

	TBool validCommand( IsValidCommand(aCommand) );

	__ASSERT_DEBUG(
	        validCommand,
			Panic( EInvalidCommand ) );

	if ( !validCommand )
		{
		ERROR( KErrArgument, "Invalid command" );
		User::Leave( KErrArgument );
		}


	CreateEntryL( aCommand );
    LaunchMREditorL( aCommand );
	}

// ---------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CMRBCPluginCreateNewMRCmd::ProcessCommandL(
		TInt /*aCommandId*/ )
	{
	FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::ProcessCommandWithResultL
// ---------------------------------------------------------------------------
//
TInt CMRBCPluginCreateNewMRCmd::ProcessCommandWithResultL( // codescanner::intleaves
		TInt /*aCommandId*/ )
	{
	FUNC_LOG;
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::LaunchMREditorL
// ---------------------------------------------------------------------------
//
void CMRBCPluginCreateNewMRCmd::LaunchMREditorL(
        const TCalenCommand& aCommand )
    {
    FUNC_LOG;

    const TUid KCalendarUid = { 0x10005901 };

    // Constructing input parameters
    INFO( "Constructing input parameters" );
    MAgnEntryUi::TAgnEntryUiInParams inParams(
            KCalendarUid,
            iServices.SessionL(),
            MAgnEntryUi::ECreateNewEntry );

    // Set time for input parameters
    INFO( "Set time for input parameters" );
    TCalTime startTime = iEntry->StartTimeL();
    TCalTime::TTimeMode timemode = startTime.TimeMode();
    if (timemode == TCalTime::EFloating )
        {
        inParams.iInstanceDate.SetTimeLocalFloatingL (
              startTime.TimeLocalL() );
        }
    else
        {
        inParams.iInstanceDate.SetTimeUtcL (
                startTime.TimeUtcL() );
        }
    inParams.iMsgSession = NULL;

    // Output parameters
    MAgnEntryUi::TAgnEntryUiOutParams outParams;
    TMROutputParams outputParams;
    outParams.iSpare = reinterpret_cast< TInt >( &outputParams );

    // Launch Entry UI
    INFO( "Launch Entry UI" );
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL(entries);
    entries.AppendL(iEntry);

    AddOrganizerL();

    CCalenInterimUtils2& utils = iServices.InterimUtilsL();
    if ( utils.MRViewersEnabledL( ETrue ) )
        {
        iEditor = CESMRViewerController::NewL(
                KBCViewer(),
                entries,
                inParams,
                outParams,
                *this );
        }
    else
        {
        // MR is not supported --> Leave
        User::Leave (KErrNotSupported );
        }

    iEditor->ExecuteL();

    TCalenNotification notification( ECalenNotifyEntryClosed );

    if ( MAgnEntryUi::ENoAction != outParams.iAction )
        {
        // Update context and issue notification before confirmation dialog
        // to avoid delay of updating title pane
        MCalenContext& context = iServices.Context();
        aCommand.GetContextL( context );

        // Update context and issue notification before confirmation dialog
        // to avoid delay of updating title pane
        TCalenInstanceId id = TCalenInstanceId::CreateL(
                *iEntry,
                outParams.iNewInstanceDate );

        context.SetInstanceIdL( id, context.ViewId() );
        notification = ECalenNotifyEntrySaved;
        }
    else
        {
        TMROutputParams* params =
            ( TMROutputParams* )( outParams.iSpare );

        // If viewer options menu exit is desired
        if( params->iCommand == EMRDialogOptionsMenuExit )
            {
            notification = ECalenNotifyRealExit;
            }
        }

    iServices.IssueNotificationL( notification );
    IssueCommandL( outParams );
    CleanupStack::PopAndDestroy( &entries );
    iEntry = NULL;
    }

// ---------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::CreateEntryL
// ---------------------------------------------------------------------------
//
void CMRBCPluginCreateNewMRCmd::CreateEntryL(
		const TCalenCommand& aCommand )
    {
    FUNC_LOG;

    CCalenInterimUtils2& utils = iServices.InterimUtilsL();

    // Create unique ID.
    HBufC8* guid = utils.GlobalUidL();
    CleanupStack::PushL(guid);
    iEntry = CCalEntry::NewL(
    		CCalEntry::EAppt,
    		guid, CCalEntry::EMethodRequest,
    		KDefaultSeqNo );
    CleanupStack::Pop( guid );

    MCalenContext& context = iServices.Context();
    aCommand.GetContextL( context );

    TCalTime startTime, stopTime;

#ifdef RD_USE_PS2_APIS
    // Business Calendar day view with visual selection
    TTime start( 0 );
    TTime end( 0 );

    context.GetStartAndEndTimeForNewInstance( start, end );

    const TTime zeroTime(0);
    if ( start != zeroTime && end != zeroTime &&
            ECalenNewMeetingTimeSpan == aCommand.Command() )
        {
        startTime.SetTimeLocalL( start );
        stopTime.SetTimeLocalL( end );

        context.SetStartAndEndTimeForNewInstance( zeroTime, zeroTime ); //clean the start/end time in Context

        }
    else
#endif // RD_USE_PS2_APIS
        {
        startTime = context.FocusDateAndTimeL();

        TTimeIntervalMinutes interval = KDefaultDurationInMinutes;

        stopTime.SetTimeLocalL(
               startTime.TimeLocalL() +
               interval );
        }

    iEntry->SetStartAndEndTimeL( startTime, stopTime );

    iEntry->SetPriorityL( EFSCalenMRPriorityNormal );
    }

// ---------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::AddOrganizerL
// ---------------------------------------------------------------------------
//
void CMRBCPluginCreateNewMRCmd::AddOrganizerL()
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
    CleanupStack::PushL(organizer );
    iEntry->SetOrganizerL(organizer );
    CleanupStack::Pop( organizer ); // Ownership trasferred

    CleanupStack::PopAndDestroy( mbUtils );
    }

// ---------------------------------------------------------------------------
// CMRBCPluginCreateNewMRCmd::IssueCommandL
// Issue command for calendar to handle
// ---------------------------------------------------------------------------
//
void CMRBCPluginCreateNewMRCmd::IssueCommandL(
        MAgnEntryUi::TAgnEntryUiOutParams aOutParams )
    {
    FUNC_LOG;
    TInt command( KErrNotFound );

    TMROutputParams* params = ( TMROutputParams* )( aOutParams.iSpare );

    switch ( params->iCommand )
        {
        case EMRDialogOptionsMenuExit:
            {
            // Calendar app is not supposed to issue EAknCmdExit command.
            // It is always soft exit and faster app FW takes the call, and
            // decides the correct action.
            //
            // This is why we use command EAknSoftkeyExit, to exit
            // application from editor options menu.

            command = EAknSoftkeyExit;
            }
            break;

        default:
            break;
        }

    // Issue command for RECAL
    if( command != KErrNotFound )
        {
        iServices.IssueCommandL( command );
        }
    }

// EOF
