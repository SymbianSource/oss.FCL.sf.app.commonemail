/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for new event commands
*
*/


#include "cmrbcpluginnewcaleventcommand.h"
#include "tesmrscenariodata.h"
#include "cesmrviewerctrl.h"
#include "esmrcommands.h"
#include "tmroutputparams.h"
#include "esmrconfig.hrh"

#include <ct/rcpointerarray.h>
#include <eikenv.h>
#include <avkon.hrh>
#include <calentry.h>
#include <eikappui.h>
#include <eikcmobs.h>
#include <magnentryui.h>
#include <calenservices.h>
#include <aknglobalnote.h>
#include <caleninterimutils2.h>
#include <calencommandhandler.h>
#include <bccaleventpolicies.rsg>
#include <data_caging_path_literals.hrh>

#include "emailtrace.h"

/// Unnamed namespace for local definitions
namespace {

/**
 * Constant for default duration in minutes
 */
const TInt KDefaultDurationInMinutes( 60 );

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::CMRPluginNewCalEntryCommand
// ---------------------------------------------------------------------------
//
CMRPluginNewCalEntryCommand::CMRPluginNewCalEntryCommand(
        MCalenServices& aServices,
        CCalEntry::TType aEntryType )
    : iServices( aServices ),
      iEntryType( aEntryType )
    {
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::~CMRPluginNewCalEntryCommand
// ---------------------------------------------------------------------------
//
CMRPluginNewCalEntryCommand::~CMRPluginNewCalEntryCommand()
    {
    delete iCalEntry;
    delete iViewer;
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::NewL
// ---------------------------------------------------------------------------
//
CMRPluginNewCalEntryCommand* CMRPluginNewCalEntryCommand::NewL(
        MCalenServices& aServices,
        CCalEntry::TType aEntryType )
    {
    FUNC_LOG;

    CMRPluginNewCalEntryCommand* self = CMRPluginNewCalEntryCommand::NewLC(
            aServices,
            aEntryType );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::NewLC
// ---------------------------------------------------------------------------
//
CMRPluginNewCalEntryCommand* CMRPluginNewCalEntryCommand::NewLC(
        MCalenServices& aServices,
        CCalEntry::TType aEntryType )
    {
    FUNC_LOG;

    CMRPluginNewCalEntryCommand* self =
            new(ELeave) CMRPluginNewCalEntryCommand(
                    aServices,
                    aEntryType );
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::ConstructL
// ---------------------------------------------------------------------------
//
void CMRPluginNewCalEntryCommand::ConstructL()
    {
    FUNC_LOG;
    // No implementation yet
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::ExecuteCommandL
// ---------------------------------------------------------------------------
//
void CMRPluginNewCalEntryCommand::ExecuteCommandL(
        const TCalenCommand& aCommand )
    {
    FUNC_LOG;

    CreateEntryL( aCommand );
    CreateAndExecuteViewL( aCommand );
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CMRPluginNewCalEntryCommand::ProcessCommandL(
        TInt aCommandId )
    {
    FUNC_LOG;
    User::LeaveIfError( ProcessCommandWithResultL(aCommandId) );
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::ProcessCommandWithResultL
// ---------------------------------------------------------------------------
//
TInt CMRPluginNewCalEntryCommand::ProcessCommandWithResultL(
        TInt /*aCommandId*/ )
    {
    FUNC_LOG;
    ASSERT( EFalse );

    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::CreateAndExecuteViewL
// ---------------------------------------------------------------------------
//
void CMRPluginNewCalEntryCommand::CreateAndExecuteViewL(
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
    TCalTime startTime = iCalEntry->StartTimeL();
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
    RCPointerArray<CCalEntry> entries;
    CleanupClosePushL(entries);
    entries.AppendL(iCalEntry);

    CCalenInterimUtils2& utils = iServices.InterimUtilsL();

    iViewer = CESMRViewerController::NewL(
            KNullDesC8(),
            entries,
            inParams,
            outParams,
            *this );

    iViewer->ExecuteL();

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
                *iCalEntry,
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

    iServices.IssueNotificationL(  notification );
    IssueCommandL( outParams );
    CleanupStack::PopAndDestroy( &entries );
    iCalEntry = NULL;

    delete iViewer;
    iViewer = NULL;
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::CreateEntryL
// ---------------------------------------------------------------------------
//
void CMRPluginNewCalEntryCommand::CreateEntryL(
        const TCalenCommand& aCommand )
    {
    FUNC_LOG;

    if ( iCalEntry )
        {
        delete iCalEntry;
        iCalEntry = NULL;
        }
    CCalenInterimUtils2& utils = iServices.InterimUtilsL();
    // Create unique ID.
    HBufC8* guid = utils.GlobalUidL();
    CleanupStack::PushL(guid);
    iCalEntry = CCalEntry::NewL(
            iEntryType,
            guid,
            CCalEntry::EMethodNone,
            0 );

    // Ownership transferred
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

        //clean the start/end time in Context
        context.SetStartAndEndTimeForNewInstance( zeroTime, zeroTime );
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

    iCalEntry->SetStartAndEndTimeL( startTime, stopTime );

    // Set default priority for the entry also
    if ( CCalEntry::ETodo == iCalEntry->EntryTypeL() )
        {
        iCalEntry->SetPriorityL( EFSCalenTodoPriorityNormal );
        }
    else
        {
        iCalEntry->SetPriorityL( EFSCalenMRPriorityNormal );
        }
    }

// ---------------------------------------------------------------------------
// CMRPluginNewCalEntryCommand::IssueCommandL
// Issue command for calendar to handle
// ---------------------------------------------------------------------------
//
void CMRPluginNewCalEntryCommand::IssueCommandL(
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

