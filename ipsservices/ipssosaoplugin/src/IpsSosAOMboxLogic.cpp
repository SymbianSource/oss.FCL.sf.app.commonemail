/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
//<QMail>

//</QMail>
#include "IpsSosAOSchedulerUtils.h"
#include "IpsSosAOPlugin.hrh"
#include "IpsSosAOImapPopLogic.h"

//<QMail>
//const TInt KAOSecondsInMinute = 60;
//</QMail>
const TInt KIpsSosAOMboxLogicMinGra = 1;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic::CIpsSosAOMBoxLogic( 
        CMsvSession& aSession, 
        TMsvId aMailboxId ) :
    iSession( aSession ), iMailboxId( aMailboxId ), iState( EStateError ),
    iErrorCounter( 0 ), iError( KErrNone ), iTimer( NULL ),
	//<QMail>
    iFetchMsgArray( KIpsSosAOMboxLogicMinGra ), iAgent( NULL ),
    iIsRoaming( EFalse )
	//</QMail>
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
	//<QMail>
	//</QMail>
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
	//<QMail>

	//</QMail>
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
            if ( iAgent->GetState() != CIpsSosAOBaseAgent::EStateCompleted ||
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
	//<QMail>
    /*
    TIpsSetDataEmnStates state = EMailEmnOff;
    if ( iExtendedSettings )
        {
        state = iExtendedSettings->EmailNotificationState();
        }
    if ( state == EMailEmnOff )
        {
        ret = EFalse;
        }
        */
	//</QMail>
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
//<QMail>
void CIpsSosAOMBoxLogic::GetEmailAddress(  
    TBuf<KIpsSosAOTextBufferSize>& /*aEmailAddress*/ ) const
    {
    FUNC_LOG;
    /*
    const TDesC& addr = iExtendedSettings->EmailAddress();
    __ASSERT_DEBUG( ( 
            addr.Length() <= KIpsSosAOTextBufferSize ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    if ( addr.Length() <= KIpsSosAOTextBufferSize )
        {
        aEmailAddress.Copy( addr );
        }
        */
	//<QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//<QMail>
//<cmail>
void CIpsSosAOMBoxLogic::SetEmnReceivedFlagL( TBool /*aNewValue*/ )
//</cmail>
    {
    LoadSettingsL();
    /*
    iExtendedSettings->SetEmnReceivedButNotSyncedFlag( aNewValue );
    iDataApi->SaveExtendedSettingsL(
        *iExtendedSettings );
    */
	//</QMail>
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
        event = EEventNop;
        iAgent->LoadSettingsL();
        iTimer->Cancel();
        iTimer->After( KIpsSosAOStartDelaySeconds );
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
                event = HandleTimerFiredL();
              break;
            case EEventStartSync:
                if ( iState == EStateWaitSyncStart )
                    {         
                    iError = KErrNone;
                    iAgent->StartSyncL();
                    iState = EStateSyncOngoing;
                    }
                else if ( iState == EStateIdleAndWaitCommands )
                    {
                    // start sync command in idle state is currently 
                    // received when emn arrives, checking scheduling 
                    // if user have selected week days / hours in setting
					//<QMail>
                /*
                    CIpsSosAOSchedulerUtils* scheduler = 
                        CIpsSosAOSchedulerUtils::NewLC( *iExtendedSettings );
                        */
                    TTimeIntervalSeconds seconds;
                    TIpsSosAOConnectionType type = EAOCConnectAfter; 
                        //= scheduler->SecondsToNextMark( seconds );
					//</QMail>
                    if ( type == EAOCConnectAfter )
                        {
                        iState = EStateWaitSyncStart;
                        iTimer->After( seconds );
                        //<cmail>
                        SetEmnReceivedFlagL( ETrue );
                        //</cmail>
                        }
                    else
                        {
                        iError = KErrNone;
                        iAgent->StartSyncL();
                        iState = EStateSyncOngoing;
                        }
					//<QMail>
                    //CleanupStack::PopAndDestroy( scheduler );
					//</QMail>
                    }
                // ignore in other states
                event = EEventNop;
                break;
            case EEventFetchMessages:
                if ( ( iState == EStateWaitSyncStart || 
                       iState == EStateIdleAndWaitCommands ) &&
                        iFetchMsgArray.Count() > 0 )
                    {
                    iError = KErrNone;
                    iAgent->StartFetchMessagesL( iFetchMsgArray );
                    iFetchMsgArray.Reset();
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
                event = HandleOperationCompletionL();
                break;
            case EEventSuspendOperations:
                SuspendOperations();
                iState = EStateSuspended;
                event = EEventNop;
                break;
            case EEventContinueOperations:
                if ( iState == EStateSuspended )
                    {
                    iTimer->After( KIpsSosAOContinueWaitTime );
                    }
                else
                    {
                    event = EEventNop;
                    }
                // ignore if in other states
                break;
            case EEventStopAndRemoveOps:
                // notify deletion
                iAgent->CancelAllAndDisconnectL();
                iTimer->Cancel();
                iState = EStateError;
                event = EEventNop;
                break;
            case EEventStart:
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
    // panic if we are in wrong state;
    __ASSERT_DEBUG( ( iState == EStateWaitSyncStart || 
            iState == EStateSuspended ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    // handle emn
    TInt event = EEventNop;

    CIpsSosAOBaseAgent::TAgentState agentState = iAgent->GetState();
    if ( iState == EStateSuspended &&
        ( agentState == CIpsSosAOBaseAgent::EStateConnectAndSyncOnHold || 
          agentState == CIpsSosAOBaseAgent::EStatePopulateOnHold ) )
        {
        iAgent->ContinueHoldOperations();
        iState = EStateSyncOngoing;
        }
    else if ( iState == EStateSuspended && 
            agentState == CIpsSosAOBaseAgent::EStateFetchOnHold )
        {
        iAgent->ContinueHoldOperations();
        iState = EStateFetchOngoing;
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
			//<QMail>
            /*
            iExtendedSettings->SetAlwaysOnlineState( EMailAoOff );
            // emn not swithced of if its going to be "always on" in
            // future
            iExtendedSettings->SetEmnReceivedButNotSyncedFlag( EFalse );
            iDataApi->SaveExtendedSettingsL(
                *iExtendedSettings );
                */
			//</QMail>
            iState = EStateError;
            iErrorCounter = 0;
            }
        else if ( iError != KErrNone && 
                iErrorCounter < KIpsSosAOMaxReTryTimes )
            {
            // not fatal error
			//<QMail>
        /*
            TIpsSetDataAoStates state = iExtendedSettings->AlwaysOnlineState();
            if ( state == EMailAoOff )
                {
                iState = EStateIdleAndWaitCommands;
                }
            else
                {
                if( !iTimer->IsActive() )
                    {
                iTimer->After( KIpsSosAOReTryInterval );
                    }
                iState = EStateWaitSyncStart;
                }
                */
			//</QMail>
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
    iState = EStateSuspended;
    // set suspend watchdog, if clien not continue this
    // ensure ao logic to continue
    if ( !iTimer->IsActive() )
        {
        iTimer->After( KIpsSosAOSuspendWatchdogTime );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//<QMail>
//<cmail>
TBool CIpsSosAOMBoxLogic::IsErrorFatalL( TInt /*aError*/ )
//</cmail>
    {
    FUNC_LOG;
    // error can be fatal only if we haven't got successful connect
    // in mailbox ever and error is something else than KErrNone or
    // KErrCancel, and retry interval is reached
    TBool ret = EFalse;
    LoadSettingsL();
    /*
    TAOInfo info = iExtendedSettings->LastUpdateInfo();
    if ( aError != KErrNone && aError != KErrCancel 
            && !info.iUpdateSuccessfulWithCurSettings
            && iErrorCounter >= KIpsSosAOMaxReTryTimes )
        {
        ret = ETrue;
        }
        */
	//</QMail>
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
	//<QMail>
    /*
        TAOInfo info = iExtendedSettings->LastUpdateInfo();
        if ( !info.iUpdateSuccessfulWithCurSettings )
            {
        
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
                iState = EStateWaitSyncStart;
                }
            else
                {
                event = EEventStartSync;
                }
            }
        
        }
    else if ( secondsToConnect.Int() == KErrNotFound )
        {
        // means that ao is not on (but emn is)
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
        iState = EStateWaitSyncStart;
        }
    */
	//</QMail>
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
		
		//<QMail>
       // iTimer->After( 
       //         iExtendedSettings->InboxRefreshTime() * KAOSecondsInMinute );
	   //</QMail>
        iState = EStateWaitSyncStart;
        }
    else if ( interval.Int() > 0 )
        {
        iTimer->After( interval );
        iState = EStateWaitSyncStart;
        }
    else 
        {
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
	//<QMail>
    /*
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
    */
	//</QMail>
    return seconds;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::LoadSettingsL()
    {
    FUNC_LOG;
    //<cmail> logs removed </cmail> 
	//<QMail>
    //removed loading
	//</QMail>
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
	//<QMail>
    /*
    TAOInfo info;
    info.iLastSuccessfulUpdate = now;
    info.iUpdateSuccessfulWithCurSettings = ETrue;
    iExtendedSettings->SetLastUpdateInfo( info );
    // clear flag
    iExtendedSettings->SetEmnReceivedButNotSyncedFlag( EFalse );
    iDataApi->SaveExtendedSettingsL(
            *iExtendedSettings );
            */
	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOMBoxLogic::CanConnectIfRoamingL()
    {
    FUNC_LOG;
    TBool ret = ETrue;
    LoadSettingsL();
	//<QMail>
    /*
    if ( ( iExtendedSettings->EmailNotificationState() == EMailEmnHomeOnly 
          || iExtendedSettings->AlwaysOnlineState() == EMailAoHomeOnly
          || iExtendedSettings->RoamHomeOnlyFlag() ) && iIsRoaming )
        {
        ret = EFalse;
        }
        */
	//</QMail>
    return ret;          
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOMBoxLogic::FirstEMNReceived()
    {
	//<QMail>
    //return iExtendedSettings->FirstEmnReceived();
    return false;
	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOMBoxLogic::SetFirstEMNReceived()
    {
    TRAP_IGNORE( LoadSettingsL() );
	//<QMail>
    /*
    iExtendedSettings->SetFirstEmnReceived( ETrue );
    
    //if alwaysonline was allowed to roam, so will EMN be.
    iExtendedSettings->SetEmailNotificationState(
            iExtendedSettings->AlwaysOnlineState() == 
            EMailAoAlways ? EMailEmnAutomatic : EMailEmnHomeOnly );
    
    
    // set always online state off when emn is on
    iExtendedSettings->SetAlwaysOnlineState( EMailAoOff );    
    iExtendedSettings->SetInboxRefreshTime( KErrNotFound );
    
    TRAP_IGNORE( iDataApi->SaveExtendedSettingsL(
       *iExtendedSettings ) );    
    */
	//</QMail>
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

