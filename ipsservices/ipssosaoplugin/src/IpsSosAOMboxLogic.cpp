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
* Description: 
*     Contains mailbox specified always online logic
*
*/


//<cmail> internal dependency removed
/*#ifdef _DEBUG
#include "../../internal/IpsSosAOPluginTester/inc/IpsSosAOPluginTester.hrh"
#endif // _DEBUG*/
//</cmail>

#include <cmmanager.h>

#include "emailtrace.h"
#include "IpsSosAOMboxLogic.h"
#include "ipssetdataapi.h"
#include "IpsSosAOSchedulerUtils.h"
#include "IpsSosAOPlugin.hrh"
#include "IpsSosAOImapPopLogic.h"

const TInt KAOSecondsInMinute = 60;
const TInt KIpsSosAOMboxLogicMinGra = 1;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic::CIpsSosAOMBoxLogic( 
        CMsvSession& aSession, 
        TMsvId aMailboxId ) :
    iSession( aSession ), iMailboxId( aMailboxId ), iState( EStateError ),
    iErrorCounter( 0 ), iError( KErrNone ), iTimer( NULL ),
    iFetchMsgArray( KIpsSosAOMboxLogicMinGra ), iDataApi( NULL ), iAgent( NULL ),
    iExtendedSettings( NULL ), iIsRoaming( EFalse )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic::~CIpsSosAOMBoxLogic()
    {
    FUNC_LOG;
    delete iTimer;
    delete iDataApi;
    delete iExtendedSettings;
    delete iAgent;
    iFetchMsgArray.Close();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic* CIpsSosAOMBoxLogic::NewL( 
        CMsvSession& aSession,
        TMsvId aMailboxId )
    {
    FUNC_LOG;
    CIpsSosAOMBoxLogic* self = new (ELeave) CIpsSosAOMBoxLogic( 
            aSession,  aMailboxId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::ConstructL()
    {
    FUNC_LOG;
    iTimer = CIpsSosAOPluginTimer::NewL( CActive::EPriorityStandard, *this );
    iDataApi = CIpsSetDataApi::NewL( iSession );
    iAgent = CIpsSosAOBaseAgent::CreateAgentL( iSession, *this, iMailboxId );
    LoadSettingsL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//   
void CIpsSosAOMBoxLogic::TimerFiredL()
    {
    FUNC_LOG;
    HandleEventAndSwitchStateL( EEventTimerFired );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//   
void CIpsSosAOMBoxLogic::OperationCompletedL( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    HandleEventAndSwitchStateL( EEventOperationCompleted );
    iError = KErrNone;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
void CIpsSosAOMBoxLogic::SendCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case ECommandStart:
            HandleEventAndSwitchStateL( EEventStart );
            break;
        case ECommandStop:
            HandleEventAndSwitchStateL( EEventStopAndRemoveOps );
            break;
        case ECommandSuspend:
            HandleEventAndSwitchStateL( EEventSuspendOperations );
            break;
        case ECommandContinue:
            HandleEventAndSwitchStateL( EEventContinueOperations );
            break;
        case ECommandStartSync:
            HandleEventAndSwitchStateL( EEventStartSync );
            if ( iState != EStateSyncOngoing )
                {
                User::Leave( KErrGeneral );
                }
            break;
        case ECommandStartedRoaming:
            iIsRoaming = ETrue;
            if ( !CanConnectIfRoamingL() )
                {
                HandleEventAndSwitchStateL( EEventStopAndRemoveOps );
                }
            break;
        case ECommandStoppedRoaming:
            iIsRoaming = EFalse;
            HandleEventAndSwitchStateL( EEventStart );
            break;
        case ECommandCancelAndDisconnect:
            iAgent->CancelAllAndDisconnectL();
            CalculateToNextIntervalL();
            break;
        case ECommandCancelDoNotDiconnect:
            if ( iAgent->GetState() != CIpsSosAOBaseAgent::EStateCompleted &&
                    iAgent->GetState() != CIpsSosAOBaseAgent::EStateIdle  )
                {
                iAgent->CancelAllAndDoNotDisconnect();
                CalculateToNextIntervalL();
                }
            break;
        case ECommandClearDoNotDisconnect:
            iAgent->ClearDoNotDisconnect();
            break;
        case ECommandDoNotDisconnect:
            iAgent->DoNotDisconnect();
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
void CIpsSosAOMBoxLogic::FetchMessagesL( const RArray<TMsvId> aArray )
    {
    FUNC_LOG;
    LoadSettingsL();
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        iFetchMsgArray.AppendL( aArray[i] );
        }
    HandleEventAndSwitchStateL( EEventFetchMessages );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
TMsvId CIpsSosAOMBoxLogic::GetMailboxId()
    {
    FUNC_LOG;
    return iMailboxId;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
CIpsSosAOBaseAgent& CIpsSosAOMBoxLogic::GetAgent() const
    {
    FUNC_LOG;
    return *iAgent;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TBool CIpsSosAOMBoxLogic::IsEmnOn() const
    {
    FUNC_LOG;
    TBool ret = ETrue;
    TIpsSetDataEmnStates state = EMailEmnOff;
    if ( iExtendedSettings )
        {
        state = iExtendedSettings->EmailNotificationState();
        }
    if ( state == EMailEmnOff )
        {
        ret = EFalse;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
void CIpsSosAOMBoxLogic::GetEmailAddress(  
    TBuf<KIpsSosAOTextBufferSize>& aEmailAddress ) const
    {
    FUNC_LOG;
    const TDesC& addr = iExtendedSettings->EmailAddress();
    __ASSERT_DEBUG( ( 
            addr.Length() <= KIpsSosAOTextBufferSize ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    if ( addr.Length() <= KIpsSosAOTextBufferSize )
        {
        aEmailAddress.Copy( addr );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
//<cmail>
void CIpsSosAOMBoxLogic::SetEmnReceivedFlagL( TBool aNewValue )
//</cmail>
    {
    LoadSettingsL();
    iExtendedSettings->SetEmnReceivedButNotSyncedFlag( aNewValue );
    iDataApi->SaveExtendedSettingsL(
        *iExtendedSettings );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//<cmail>
TBool CIpsSosAOMBoxLogic::IsMailboxRoamingStoppedL()
//</cmail>
    {
    TBool ret = EFalse;
    if ( iIsRoaming && !CanConnectIfRoamingL() )
        {
        ret = ETrue;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
void CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL( 
        TInt aEvent )
    {
    FUNC_LOG;
    TInt event = aEvent;
    TInt cycles = 0;
    
    if ( event == EEventStart && iState == EStateError && 
            CanConnectIfRoamingL() )
        {
        INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventStart" );
        event = EEventNop;
        iAgent->LoadSettingsL();
        iTimer->Cancel();
        iTimer->After( KIpsSosAOStartDelaySeconds );
        INFO( "CIpsSosAOMBoxLogic switching state: EStateWaitSyncStart" );
        iState = EStateWaitSyncStart;
        }
    
    while ( event != EEventNop && iState != EStateError )
        {
//<cmail> removing flags    
/*#ifdef IPSSOSIMAPPOLOGGING_ON
        WriteDebugData( event );
#endif*/
//</cmail>
        switch ( event )
            {
            case EEventTimerFired:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventTimerFired" );
                event = HandleTimerFiredL();
              break;
            case EEventStartSync:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventStartSync" );
                if ( iState == EStateWaitSyncStart )
                    {
                    if (iTimer->IsActive()) 
                        {
                        iTimer->Cancel();
                        }
                    iTimer->After(KIpsSosAOSyncWatchdogTime);
                    iError = KErrNone;
                    iAgent->StartSyncL();
                    INFO( "CIpsSosAOMBoxLogic: switching state: EStateSyncOngoing" );
                    iState = EStateSyncOngoing;
                    }
                else if ( iState == EStateIdleAndWaitCommands )
                    {
                    INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: state was EStateIdleAndWaitCommands" );
                    // start sync command in idle state is currently 
                    // received when emn arrives, checking scheduling 
                    // if user have selected week days / hours in setting
                    CIpsSosAOSchedulerUtils* scheduler = 
                        CIpsSosAOSchedulerUtils::NewLC( *iExtendedSettings );
                    TTimeIntervalSeconds seconds;
                    TIpsSosAOConnectionType type = 
                        scheduler->SecondsToNextMark( seconds );
                    if ( type == EAOCConnectAfter )
                        {
                        INFO( "CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart" );
                        iState = EStateWaitSyncStart;
                        iTimer->After( seconds );
                        //<cmail>
                        SetEmnReceivedFlagL( ETrue );
                        //</cmail>
                        }
                    else
                        {
                        if (iTimer->IsActive()) 
                            {
                            iTimer->Cancel();
                            }
                        iTimer->After(KIpsSosAOSyncWatchdogTime);
                        iError = KErrNone;
                        iAgent->StartSyncL();
                        INFO( "CIpsSosAOMBoxLogic: switching state: EStateSyncOngoing" );
                        iState = EStateSyncOngoing;
                        }
                    CleanupStack::PopAndDestroy( scheduler );
                    }
                // ignore in other states
                event = EEventNop;
                break;
            case EEventFetchMessages:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventFetchMessages" );
                if ( ( iState == EStateWaitSyncStart || 
                       iState == EStateIdleAndWaitCommands ) &&
                        iFetchMsgArray.Count() > 0 )
                    {
                    iError = KErrNone;
                    iAgent->StartFetchMessagesL( iFetchMsgArray );
                    iFetchMsgArray.Reset();
                    INFO( "CIpsSosAOMBoxLogic: switching state: EStateFetchOngoing" );
                    iState = EStateFetchOngoing;
                    }
                else 
                    {
                    // ignore event in other states
                    iFetchMsgArray.Reset();
                    }
                event = EEventNop;
                break;
            case EEventOperationCompleted:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventOperationCompleted" );
                event = HandleOperationCompletionL();
                break;
            case EEventSuspendOperations:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventSuspendOperations" );
                SuspendOperations();
                INFO( "CIpsSosAOMBoxLogic: switching state: EStateSuspended" );
                iState = EStateSuspended;
                event = EEventNop;
                break;
            case EEventContinueOperations:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventContinueOperations" );
                if ( iState == EStateSuspended )
                    {
                    iTimer->After( KIpsSosAOContinueWaitTime );
                    }
                event = EEventNop;
                // ignore if in other states
                break;
            case EEventStopAndRemoveOps:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventStopAndRemoveOps" );
                // notify deletion
                iAgent->CancelAllAndDisconnectL();
                iTimer->Cancel();
                INFO( "CIpsSosAOMBoxLogic: switching state: EStateError" );
                iState = EStateError;
                event = EEventNop;
                break;
            case EEventStart:
                INFO( "CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL: EEventStart" );
            default:
                // ignore other events
                event = EEventNop;
                break;
            }
        
        // TO Prevent forever loop (and freezing) 
        // if there is programming error in state machine logic
        __ASSERT_ALWAYS( ( 
                cycles < KIpsSosAOMaxStateCycles ), 
                User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
        ++cycles;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TInt CIpsSosAOMBoxLogic::GetCurrentState() const
    {
    FUNC_LOG;
    return iState;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TInt CIpsSosAOMBoxLogic::GetCurrentError() const
    {
    FUNC_LOG;
    return iError;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TInt CIpsSosAOMBoxLogic::HandleTimerFiredL()
    {
    FUNC_LOG;
    INFO( "CIpsSosAOMBoxLogic::HandleTimerFiredL" );
    // panic if we are in wrong state;
    __ASSERT_DEBUG( ( iState == EStateWaitSyncStart || 
            iState == EStateSuspended ||
            iState == EStateSyncOngoing), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    // handle emn
    TInt event = EEventNop;

    CIpsSosAOBaseAgent::TAgentState agentState = iAgent->GetState();
    if ( iState == EStateSuspended &&
        ( agentState == CIpsSosAOBaseAgent::EStateConnectAndSyncOnHold || 
          agentState == CIpsSosAOBaseAgent::EStatePopulateOnHold ) )
        {
        iAgent->ContinueHoldOperations();
        INFO( "CIpsSosAOMBoxLogic: switching state: EStateSyncOngoing" );
        iState = EStateSyncOngoing;
        }
    else if ( iState == EStateSuspended && 
            agentState == CIpsSosAOBaseAgent::EStateFetchOnHold )
        {
        iAgent->ContinueHoldOperations();
        INFO( "CIpsSosAOMBoxLogic: switching state: EStateFetchOngoing" );
        iState = EStateFetchOngoing;
        }
    else if ( iState == EStateSyncOngoing ) 
        {
        iAgent->CancelAllAndDisconnectL();
        event = CheckSchedulingAndSwitchStateL();
        }
    else
        {
        event = CheckSchedulingAndSwitchStateL();
        }
    // in other states, timer event is ignored
    
    return event;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TInt CIpsSosAOMBoxLogic::HandleOperationCompletionL()
    {
    FUNC_LOG;
    TInt event = EEventNop;
    if ( iState == EStateSyncOngoing )
        {
        iTimer->Cancel();
        if ( !( iError == KErrNone || iError == KErrCancel ) )
            {
            ++iErrorCounter;
            }
        else
            {
            iError = KErrNone;
            iErrorCounter = 0;
            }
        //<cmail>
        if ( iError != KErrNone && 
                IsErrorFatalL( iError ) )
        //</cmail>
            {
            iAgent->CancelAllAndDisconnectL();
            // switch ao off
            iExtendedSettings->SetAlwaysOnlineState( EMailAoOff );
            // emn not swithced of if its going to be "always on" in
            // future
            iExtendedSettings->SetEmnReceivedButNotSyncedFlag( EFalse );
            iDataApi->SaveExtendedSettingsL(
                *iExtendedSettings );
            INFO( "CIpsSosAOMBoxLogic: switching state: EStateError" );
            iState = EStateError;
            iErrorCounter = 0;
            }
        else if ( iError != KErrNone && 
                iErrorCounter < KIpsSosAOMaxReTryTimes )
            {
            // not fatal error
            TIpsSetDataAoStates state = iExtendedSettings->AlwaysOnlineState();
            if ( state == EMailAoOff )
                {
                INFO( "CIpsSosAOMBoxLogic: switching state: EStateIdleAndWaitCommands" );
                iState = EStateIdleAndWaitCommands;
                }
            else
                {
                if( !iTimer->IsActive() )
                    {
                    iTimer->After( KIpsSosAOReTryInterval );
                    }
                INFO( "CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart" );
                iState = EStateWaitSyncStart;
                }
            }
        else if ( iError != KErrNone && 
                iErrorCounter >= KIpsSosAOMaxReTryTimes )
            {
            //<cmail>
            SetEmnReceivedFlagL( EFalse );
            //</cmail>
            // not fatal, but all re trys are gone
            CalculateToNextIntervalL();
            }
        else
            {
            // no errors
            // update successfull sync time to settings
            INFO( "CIpsSosAOMBoxLogic::HandleOperationCompletionL" );
            SaveSuccessfulSyncTimeL();
            // and adjust timer to sync interval
            CalculateToNextIntervalL();
            }
        }
    else if ( iState == EStateFetchOngoing )
        {
        iError = KErrNone;
        if ( iTimer->IsActive() )
            {
            INFO( "CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart" );
            iState = EStateWaitSyncStart;
            }
        else
            {
            event = CheckSchedulingAndSwitchStateL();          
            }
        }
    return event;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
void CIpsSosAOMBoxLogic::SuspendOperations()
    {
    FUNC_LOG;
    if ( iState == EStateSyncOngoing || iState == EStateFetchOngoing )
        {
        iAgent->HoldOperations();
        }

    // set suspend watchdog, if clien not continue this
    // ensure ao logic to continue
    if ( !iTimer->IsActive() || iState == EStateSyncOngoing || iState == EStateFetchOngoing)
        {
        iTimer->After( KIpsSosAOSuspendWatchdogTime );
        }
    INFO( "CIpsSosAOMBoxLogic: switching state: EStateSuspended" );
    iState = EStateSuspended;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//<cmail>
TBool CIpsSosAOMBoxLogic::IsErrorFatalL( TInt aError )
//</cmail>
    {
    FUNC_LOG;
    // error can be fatal only if we haven't got successful connect
    // in mailbox ever and error is something else than KErrNone or
    // KErrCancel, and retry interval is reached
    TBool ret = EFalse;
    LoadSettingsL();
    TAOInfo info = iExtendedSettings->LastUpdateInfo();
    if ( aError != KErrNone && aError != KErrCancel 
            && !info.iUpdateSuccessfulWithCurSettings
            && iErrorCounter >= KIpsSosAOMaxReTryTimes )
        {
        INFO( "CIpsSosAOMBoxLogic::IsErrorFatalL - FATAL ERROR" );
        ret = ETrue;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic::TMBoxLogicEvent 
    CIpsSosAOMBoxLogic::CheckSchedulingAndSwitchStateL()
    {
     //<cmail> logs removed </cmail>
    iTimer->Cancel();
    TMBoxLogicEvent event = EEventNop;
    TTimeIntervalSeconds secondsToConnect = CalculateScheduledSyncTimeL();
    //<cmail> logs removed </cmail>
    if ( secondsToConnect.Int() == 0 )
        {
        //check last successfull sync time and check is interval reached
        TAOInfo info = iExtendedSettings->LastUpdateInfo();
        if ( !info.iUpdateSuccessfulWithCurSettings )
            {
            INFO( "CIpsSosAOMBoxLogic: switching state: EEventStartSync" );
            event = EEventStartSync;
            }
        else 
            {
            TTime now;
            now.HomeTime();
            TInt interval = 
                iExtendedSettings->InboxRefreshTime() * KAOSecondsInMinute;
            TTimeIntervalSeconds secsFromLastSync;
            now.SecondsFrom( info.iLastSuccessfulUpdate, secsFromLastSync );
            if ( (secsFromLastSync.Int() > 0) && ( 
                    secsFromLastSync.Int() < interval ) )
                {
                // adjust timer to correct sync time
                iTimer->After(interval - secsFromLastSync.Int());
                INFO( "CIpsSosAOMBoxLogic: switching state: EEventStartSync" );
                iState = EStateWaitSyncStart;
                }
            else
                {
                INFO( "CIpsSosAOMBoxLogic: switching state: EEventStartSync" );
                event = EEventStartSync;
                }
            }
        }
    else if ( secondsToConnect.Int() == KErrNotFound )
        {
        // means that ao is not on (but emn is)
        INFO( "CIpsSosAOMBoxLogic: switching state: EStateIdleAndWaitCommands" );
        iState = EStateIdleAndWaitCommands;
        if ( iExtendedSettings->EmnReceivedButNotSyncedFlag() )
            {
            // got emn when logic was stopped
            event = EEventStartSync;
            }
        }
    else
        {
        iTimer->After( secondsToConnect );
        INFO( "CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart" );
        iState = EStateWaitSyncStart;
        }
    
    return event;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::CalculateToNextIntervalL()
    {
    FUNC_LOG;
    //<cmail> logs removed </cmail>
    iTimer->Cancel();
    TTimeIntervalSeconds interval;
    // first check and calculate scheduling function also loads settings
    interval = CalculateScheduledSyncTimeL();
    //<cmail> logs removed </cmail>
    if ( interval.Int() == 0 )
        {
        // add next interval, if interval is grater than 0 it means
        // that off-line time is going, user have selected days / times from
        // settings
        iTimer->After( 
                iExtendedSettings->InboxRefreshTime() * KAOSecondsInMinute );
        INFO( "CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart" );
        iState = EStateWaitSyncStart;
        }
    else if ( interval.Int() > 0 )
        {
        iTimer->After( interval );
        INFO( "CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart" );
        iState = EStateWaitSyncStart;
        }
    else 
        {
        INFO( "CIpsSosAOMBoxLogic: switching state: EStateIdleAndWaitCommands" );
        iState = EStateIdleAndWaitCommands;
        }
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TTimeIntervalSeconds CIpsSosAOMBoxLogic::CalculateScheduledSyncTimeL()
    {
    FUNC_LOG;
    //<cmail> logs removed </cmail> 
    LoadSettingsL();
    TTimeIntervalSeconds seconds;
    
    // first check is ao or emn on at all
    if ( (iExtendedSettings->AlwaysOnlineState() == EMailAoOff && 
            iExtendedSettings->EmailNotificationState() == EMailEmnOff  )||
        iExtendedSettings->SelectedWeekDays() == 0 ) // no week days chosen
        {
        // no timed sync on
        return KErrNotFound;
        }
    
    if ( iExtendedSettings->EmailNotificationState() != EMailEmnOff && 
         !iExtendedSettings->EmnReceivedButNotSyncedFlag()   )
        {
        return KErrNotFound;
        }
    
    
    CIpsSosAOSchedulerUtils* scheduler = CIpsSosAOSchedulerUtils::NewLC(
        *iExtendedSettings );

    TIpsSosAOConnectionType type = scheduler->SecondsToNextMark( seconds );
    if ( type == EAOCDisconnectAfter ) 
        {
        // if type is EAOCDisconnectAfter it means that scheduled days / times
        // end after interval function returned in seconds reference
        // when connection is not kept open we not need timer for 
        // disconnection. Just return 0 as a mark that sync can be started
        seconds = 0;
        }
    else
        {
        // this means that user have choosed time/day schedule at 
        // timed sync and now we have to wait sync time
        }
    CleanupStack::PopAndDestroy( scheduler );
    return seconds;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::LoadSettingsL()
    {
    FUNC_LOG;
    //<cmail> logs removed </cmail> 
    delete iExtendedSettings;
    iExtendedSettings = NULL;
    iExtendedSettings = CIpsSetDataExtension::NewL();
    iDataApi->LoadExtendedSettingsL( iMailboxId, *iExtendedSettings );
    //<cmail> logs removed </cmail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::SaveSuccessfulSyncTimeL()
    {
    FUNC_LOG;
    TTime now;
    now.HomeTime();
    LoadSettingsL();
    TAOInfo info;
    info.iLastSuccessfulUpdate = now;
    info.iUpdateSuccessfulWithCurSettings = ETrue;
    info.iLastUpdateFailed=EFalse;
    iExtendedSettings->SetLastUpdateInfo( info );
    // clear flag
    iExtendedSettings->SetEmnReceivedButNotSyncedFlag( EFalse );
    iDataApi->SaveExtendedSettingsL(
            *iExtendedSettings );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOMBoxLogic::CanConnectIfRoamingL()
    {
    FUNC_LOG;
    TBool ret = ETrue;

    RCmManager cmManager;
    cmManager.OpenLC();
    TCmGenConnSettings OccSettings;
    
    //ask roaming settings from occ
    cmManager.ReadGenConnSettingsL(OccSettings);
    if(OccSettings.iCellularDataUsageVisitor == ECmCellularDataUsageDisabled && iIsRoaming)
        {
        ret = EFalse;
        }
    
    CleanupStack::PopAndDestroy(&cmManager); // cmManager
    
    return ret;          
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOMBoxLogic::FirstEMNReceived()
    {
    return iExtendedSettings->FirstEmnReceived();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOMBoxLogic::SetFirstEMNReceived()
    {
    TRAP_IGNORE( LoadSettingsL() );
    iExtendedSettings->SetFirstEmnReceived( ETrue );
    
    //if alwaysonline was allowed to roam, so will EMN be.
    iExtendedSettings->SetEmailNotificationState( EMailEmnAutomatic );
    
    
    // set always online state off when emn is on
    iExtendedSettings->SetAlwaysOnlineState( EMailAoOff );    
    iExtendedSettings->SetInboxRefreshTime( KErrNotFound );
    
    TRAP_IGNORE( iDataApi->SaveExtendedSettingsL(
       *iExtendedSettings ) );    
    
    }
//<cmail> removing flags
/*
#ifdef IPSSOSIMAPPOLOGGING_ON
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::WriteDebugData( TInt aEvent )
    {
    switch ( aEvent )
        {
        case EEventTimerFired:
            break;
        case EEventStartSync:
            break;
        case EEventFetchMessages:
            break;
        case EEventOperationCompleted:
            break;
        case EEventSuspendOperations:
            break;
        case EEventContinueOperations:
            break;
        case EEventStart:
            break;
        case EEventStopAndRemoveOps:
            break;
        case EEventNop:
            break;
        default:
            break;
        }

    switch ( iState )
        {
        case EStateWaitSyncStart:
            break;
        case EStateSyncOngoing:
            break;
        case EStateFetchOngoing:
            break;
        case EStateSuspended:
            break;
        case EStateIdleAndWaitCommands:
            break;
        case EStateError:
            break;
        default:
            break;
        };
    }
#endif // IPSSMTPLOGGING_ON*/
//</cmail>    

// End of file

