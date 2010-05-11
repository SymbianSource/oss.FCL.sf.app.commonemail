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

#include "cmrbcpluginopenmrviewercmd.h"
#include "cesmrviewerctrl.h"
#include "esmrcommands.h"
#include "tmroutputparams.h"
#include "esmrhelper.h"
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
#include <calentryview.h>
#include <calinstanceview.h>

#include "emailtrace.h"


namespace {

/**
 * Literal for BC viewer / editor
 */
_LIT8( KBCViewer, "[2001F406]" );


#ifdef _DEBUG

/**
 * Panic literal
 */
_LIT( KMRBCPluginOpenMRViewerCmd, "MRBCPluginOpenMRViewerCmd" );

/**
 * Panic codes
 */
enum TMRBCPluginOpenMRViewerCmdPanic
    {
    EInvalidCommand // Invalid command
    };

void Panic( TMRBCPluginOpenMRViewerCmdPanic aPanic )
    {
    User::Panic( KMRBCPluginOpenMRViewerCmd, aPanic );
    }

#endif // _DEBUG

template <class T>
class CleanupResetAndDestroy
    {
    public:
        inline static void PushL( T& aRef )
            {
            CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
            }

    private:
        inline static void ResetAndDestroy( TAny* aPtr )
            {
            static_cast< T*>( aPtr )->ResetAndDestroy();
            }
    };

template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }
}


CCalInstance* FindPossibleInstanceL(
        const TCalenInstanceId& aId, CCalInstanceView& aInstanceView )
        {
        FUNC_LOG;

        RPointerArray<CCalInstance> instances;
        CleanupResetAndDestroyPushL( instances );

        TCalTime moment;
        moment.SetTimeLocalL( aId.iInstanceTime );
        CalCommon::TCalTimeRange range( moment, moment );
        aInstanceView.FindInstanceL( instances, CalCommon::EIncludeAll, range );

        CCalInstance* result = NULL;

        // For instances finishing the next day (now possible with unified DateTime editor),
        // we have to do our best to match the instance time exactly - otherwise we could
        // match the LocalUid to the incorrect instance in a series.
        for ( TInt i=0; i < instances.Count() && !result; ++i )
            {
            if( instances[i]->Entry().LocalUidL() == aId.iEntryLocalUid )
                {
                // Check the instance time matches.
                if( instances[i]->StartTimeL().TimeLocalL() == aId.iInstanceTime )
                    {
                    result = instances[i];
                    instances.Remove(i);
                    }
                }
            }

        if( !result )
            {
            // Couldn't match the instance time exactly - just use the instance
            // with the same LocalUid as the one we're looking for.
            for ( TInt i=0; i < instances.Count() && !result; ++i )
                {
                if( instances[i]->Entry().LocalUidL() == aId.iEntryLocalUid )
                    {
                    result = instances[i];
                    instances.Remove(i);
                    }
                }
            }

        CleanupStack::PopAndDestroy( &instances );
        return result;
        }



// -----------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::CMRBCPluginOpenMRViewerCmd
// -----------------------------------------------------------------------------
//
CMRBCPluginOpenMRViewerCmd::CMRBCPluginOpenMRViewerCmd(
        MCalenServices& aServices )
:   iServices( aServices )
            {
            FUNC_LOG;
            }

// -----------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::~CMRBCPluginOpenMRViewerCmd
// -----------------------------------------------------------------------------
//
CMRBCPluginOpenMRViewerCmd::~CMRBCPluginOpenMRViewerCmd()
    {
    FUNC_LOG;
    delete iEntry;
    delete iEditor;
    }

        // -----------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::NewL
// -----------------------------------------------------------------------------
//
CMRBCPluginOpenMRViewerCmd* CMRBCPluginOpenMRViewerCmd::NewL(
        MCalenServices& aServices)
    {
    FUNC_LOG;

    CMRBCPluginOpenMRViewerCmd* self = new (ELeave) CMRBCPluginOpenMRViewerCmd(
            aServices);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CMRBCPluginOpenMRViewerCmd::ConstructL()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CMRBCPluginOpenMRViewerCmd::ExecuteCommandL(
        const TCalenCommand& aCommand )
    {
    FUNC_LOG;

    __ASSERT_DEBUG(
            aCommand.Command() == ECalenEventView,
            Panic( EInvalidCommand ) );

    if (aCommand.Command() != ECalenEventView)
        {
        ERROR( KErrArgument, "Invalid command" );
        User::Leave(KErrArgument);
        }

    GetEntryL(aCommand);
    LaunchMRViewerL( aCommand );
    }

// ---------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CMRBCPluginOpenMRViewerCmd::ProcessCommandL( TInt /* aCommandId */ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::ProcessCommandWithResultL
// ---------------------------------------------------------------------------
//
TInt CMRBCPluginOpenMRViewerCmd::ProcessCommandWithResultL(TInt /*aCommandId*/)
    {
    FUNC_LOG;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::LaunchMRViewerL
// ---------------------------------------------------------------------------
//
void CMRBCPluginOpenMRViewerCmd::LaunchMRViewerL(
        const TCalenCommand& aCommand )
    {
    FUNC_LOG;

    const TUid KCalendarUid =
        {
        0x10005901
        };
    // Constructing input parameters
    INFO( "Constructing input parameters" );
    MAgnEntryUi::TAgnEntryUiInParams inParams(
            KCalendarUid,
            iServices.SessionL(),
            MAgnEntryUi::EViewEntry);

    // Set time for input parameters
    INFO( "Set time for input parameters" );
    TCalTime startTime = iEntry->StartTimeL();
    TCalTime::TTimeMode timemode = startTime.TimeMode();
    if (timemode == TCalTime::EFloating)
        {
        inParams.iInstanceDate.SetTimeLocalFloatingL(startTime.TimeLocalL());
        }
    else
        {
        RArray<TInt> colIds;
        CleanupClosePushL( colIds );
        TInt colId = iServices.Context().InstanceId().iColId;
        colIds.AppendL( colId );
        CCalInstance* instance = FindPossibleInstanceL(
                iServices.Context().InstanceId(), *(iServices.InstanceViewL(colIds)));
        CleanupStack::PushL( instance );
        inParams.iInstanceDate.SetTimeUtcL(instance->Time().TimeUtcL());
        CleanupStack::PopAndDestroy( instance );
        CleanupStack::PopAndDestroy( &colIds );
        }

    inParams.iMsgSession = NULL;

    // Output parameters
    MAgnEntryUi::TAgnEntryUiOutParams outParams;
    TMROutputParams outputParams;
    outParams.iSpare = reinterpret_cast< TInt >( &outputParams );

    // Launch Entry UI
    INFO( "Launch Entry UI" );
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );
    entries.AppendL( iEntry ); // Ownership not transferred.

    iEditor = CESMRViewerController::NewL(
            KBCViewer(),
            entries,
            inParams,
            outParams,
            *this );

    TRAPD( err, iEditor->ExecuteL() );
    if( KErrArgument == err )
        {
        delete iEditor;
        iEditor = NULL;

        // ES MR VIEWER controller was unable to show the
        // passed meeting request. Let's launch normal meeting viewer
        // for showing the entry
        CCalEntry* meetingEntry = ESMRHelper::CopyEntryLC(
                *entries[0],
                entries[0]->MethodL(),
                ESMRHelper::ECopyFull,
                EESMREventTypeAppt );

#ifdef RD_USE_PS2_APIS
        meetingEntry->ClearMRSpecificDataL();
#endif // RD_USE_PS2_APIS

        // Delete old entry and reset entry array
        delete iEntry;
        iEntry = NULL;
        entries.Reset();

        // Append entry to array.
        entries.AppendL( meetingEntry );

        // Take ownership of meeting entry
        iEntry = meetingEntry;
        CleanupStack::Pop( meetingEntry );

        iEditor = CESMRViewerController::NewL(
                KBCViewer(),
                entries,
                inParams,
                outParams,
                *this );

        iEditor->ExecuteL();
        }

    // Issue notification if required.
    IssueNotifyL( aCommand, outParams );

    // Issue command if required.
    IssueCommandL( outParams );

    // Close and reset entry array
    CleanupStack::PopAndDestroy( &entries );
    }

// ---------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::GetEntryL
// ---------------------------------------------------------------------------
//
void CMRBCPluginOpenMRViewerCmd::GetEntryL(const TCalenCommand& /*aCommand*/ )
    {
    FUNC_LOG;

    if (iEntry)
        {
        delete iEntry;
        iEntry = NULL;
        }

    MCalenContext& context = iServices.Context();
    TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
    TInt colId = context.InstanceId().iColId;

    iEntry = iServices.EntryViewL(colId)->FetchL(instanceId);
    }

// ---------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::IssueNotifyL
// Handle notify message
// ---------------------------------------------------------------------------
//
void CMRBCPluginOpenMRViewerCmd::IssueNotifyL(
        const TCalenCommand& aCommand,
        MAgnEntryUi::TAgnEntryUiOutParams aOutParams )
    {
    FUNC_LOG;

    TCalenNotification notification = ECalenNotifyEntryClosed;
    switch ( aOutParams.iAction )
        {
        case MAgnEntryUi::EMeetingDeleted:
        case MAgnEntryUi::EInstanceDeleted:
            {
            notification = ECalenNotifyEntryDeleted;
            }
            break;

        case MAgnEntryUi::EMeetingSaved:
            {
            notification = ECalenNotifyEntrySaved;
            }
            break;

        case MAgnEntryUi::EInstanceRescheduled:
            {
            notification = ECalenNotifyEntrySaved;
            }
            break;

        case MAgnEntryUi::ENoAction:
            {
            TMROutputParams* params =
                ( TMROutputParams* )( aOutParams.iSpare );

            // If viewer options menu exit is desired
            if( params->iCommand == EMRDialogOptionsMenuExit )
                {
                notification = ECalenNotifyRealExit;
                }
            }
            break;

        default:
            break;
        }

    if ( MAgnEntryUi::ENoAction != aOutParams.iAction )
        {
        // Update context and issue notification before confirmation dialog
        // to avoid delay of updating title pane
        MCalenContext& context = iServices.Context();
        aCommand.GetContextL( context );

        // Update context and issue notification before confirmation dialog
        // to avoid delay of updating title pane
        TCalenInstanceId id = TCalenInstanceId::CreateL(
                *iEntry,
                aOutParams.iNewInstanceDate );

        context.SetInstanceIdL( id, context.ViewId() );
        }

    // Notify RECAL
    iServices.IssueNotificationL( notification );
    }

// ---------------------------------------------------------------------------
// CMRBCPluginOpenMRViewerCmd::IssueCommandL
// Issue command for calendar to handle
// ---------------------------------------------------------------------------
//
void CMRBCPluginOpenMRViewerCmd::IssueCommandL(
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
            // application from viewer options menu.

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
