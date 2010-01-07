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
* Implementation of EmailShutter
*
*/
//  Include Files  

#include <e32base.h>
#include <e32std.h>
#include <e32des16.h>                   // Descriptors

#include <e32property.h>                // RProperty
#include <apgtask.h>                    // TApaTaskList
#include <w32std.h>                     // RWsSession

#include <AlwaysOnlineManagerClient.h>  // RAlwaysOnlineClientSession
#include <CPsRequestHandler.h>          // CPSRequestHandler
#include <centralrepository.h>          // CRepository

#include "FreestyleEmailUiConstants.h"  // FS Email UI UID
#include "fsmtmsconstants.h"            // MCE, Phonebook & Calendar UIDs
#include "EmailStoreUids.hrh"           // KUidMessageStoreExe

#include "emailtrace.h"
#include "emailshutdownconst.h"
#include "emailshutter.h"
#include "emailservermonitorconst.h"
#include "emailservermonitorutilities.h"
#include "emailservermonitor.h"


// UI Applications to be closed
const TUid KApplicationsToClose[] =
    {
    KFSEmailUiUid,              // Freestyle Email UI
    { SettingWizardUidAsTInt }, // TP Wizard
    { KMceAppUid },             // MCE
    { KPhoneBookUid },          // Phonebook 1 & 2
    { KCalendarAppUid1 }        // Calendar
    };

// Non-UI clients that need to be closed
const TUid KOtherClientsToClose[] =
    {
    { KPcsServerProcessUidAsTInt }, // PCS server
    { FSMailServerUidAsTInt },      // FSMailServer
    };

// Plugin processes that need to be closed
const TUid KPluginProcessesToClose[] =
    {
    // MfE plugin
    { 0x20012BEE },      // KUidEasStartup
    { 0x20012BEC },      // KUidEasTarmAccess
    { 0x20012BD4 },      // KEasLogSenderServer
    { 0x20012BE6 },      // KUidEasServer
    // Oz plugin
    { 0x2002136A },      // monitor
    { 0x20021367 },      // server
    };

// Message store processes that need to be closed
const TUid KMsgStoreProcessesToClose[] =
    {
    { KUidMessageStoreExe },        // MessageStoreExe.exe
    { KUidEmailStorePreInstallExe } // MessageStorePreInstallExe
    };


// ======== MEMBER FUNCTION DEFINITIONS ========

// ---------------------------------------------------------------------------
// Two-phased class constructor.
// ---------------------------------------------------------------------------
//
CEmailShutter* CEmailShutter::NewL()
    {
    FUNC_LOG;
    CEmailShutter* self = CEmailShutter::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased class constructor, leaves newly created object in cleanup stack.
// ---------------------------------------------------------------------------
//
CEmailShutter* CEmailShutter::NewLC()
    {
    FUNC_LOG;
    CEmailShutter* self = new (ELeave) CEmailShutter();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CEmailShutter::CEmailShutter()
    : CActive( EPriorityStandard ), iMonitor( NULL )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Default destructor
// ---------------------------------------------------------------------------
//
CEmailShutter::~CEmailShutter()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Second phase class constructor.
// ---------------------------------------------------------------------------
//
void CEmailShutter::ConstructL()
    {
    FUNC_LOG;
    // Define and attach/subscribe to P&S to get the shutdown event from
    // installation initiator(s)
    TInt error = iInstStatusProperty.Define( EEmailPsKeyInstallationStatus,
                                             RProperty::EInt,
                                             KAllowAllPolicy,
                                             KPowerMgmtPolicy );
    if( error != KErrNone && error != KErrAlreadyExists )
        {
        ERROR_1( error, "RProperty::Define failed, error code: ", error );
        User::Leave( error );
        }

    error = iInstStatusProperty.Attach( KEmailShutdownPsCategory,
                                        EEmailPsKeyInstallationStatus );
    if( error != KErrNone )
        {
        ERROR_1( error, "RProperty::Attach failed, error code: ", error );
        User::Leave( error );
        }
    
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Starts observing P&S shutdown event
// ---------------------------------------------------------------------------
//
void CEmailShutter::StartObservingShutdownEvent()
    {
    FUNC_LOG;
    SetActive();
    iInstStatusProperty.Subscribe( iStatus );
    }

// ---------------------------------------------------------------------------
// Set P&S key after installation is done
// ---------------------------------------------------------------------------
//
void CEmailShutter::SetPsKeyInstallationFinished()
    {
    FUNC_LOG;
    TInt error = iInstStatusProperty.Set( EEmailPsValueInstallationFinished );
    ERROR_1( error, "RProperty::Set error code: %d", error );
    }

// ---------------------------------------------------------------------------
// Restart needed services after installation is finished
// ---------------------------------------------------------------------------
//
void CEmailShutter::RestartServicesAfterInstallation()
    {
    TRAP_IGNORE( StartAoPluginL() );
    TRAP_IGNORE( StartPcsServerL() );
    }

// ---------------------------------------------------------------------------
// Set pointer to Email Server Monitor object
// ---------------------------------------------------------------------------
//
void CEmailShutter::SetMonitor( CEmailServerMonitor* aMonitor )
    {
    iMonitor = aMonitor;
    }

// ---------------------------------------------------------------------------
// RunL of active object
// ---------------------------------------------------------------------------
//
void CEmailShutter::RunL()
    {
    FUNC_LOG;
    if( iStatus == KErrNone )
        {
        TInt value( 0 );
        TInt error = iInstStatusProperty.Get( value );
        ERROR_1( error, "RProperty::Get error code: %d", error );
        
        // Verify that the P&S value indicates that installation is starting
        if( error == KErrNone &&
            value == EEmailPsValueInstallationStarting )
            {
            StartShutdown();
    
            // Send "installation OK to continue" event to installation
            // initiator once everyting is shutdown
            error = iInstStatusProperty.Set( EEmailPsValueInstallationOkToStart );
            ERROR_1( error, "RProperty::Set error code: %d", error );
    
            // Finally shutdown ourselves also
            CActiveScheduler::Stop();
            }
        else
            {
            // Re-subscribe
            StartObservingShutdownEvent();
            }
        }
    else
        {
        ERROR_1( iStatus.Int(), "CEmailShutter::RunL error code: %d", iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// Cancel active object
// ---------------------------------------------------------------------------
//
void CEmailShutter::DoCancel()
    {
    FUNC_LOG;
    iInstStatusProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// End client applications gracefully
// ---------------------------------------------------------------------------
//
void CEmailShutter::EndClients()
    {
    FUNC_LOG;
    // End UI applications, ignore errors
    TRAP_IGNORE( EndApplicationsL() );
    
    // Define and publish the P&S key to give the clients change to close
    // themselves gracefully
    PublishPsKey( EEmailPsKeyShutdownClients );
    }

// ---------------------------------------------------------------------------
// End UI applications gracefully
// ---------------------------------------------------------------------------
//
void CEmailShutter::EndApplicationsL()
    {
    FUNC_LOG;
    RWsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

    TApaTaskList taskList( session );

    TInt count = sizeof(KApplicationsToClose) / sizeof(TUid);
    for( TInt i = 0; i<count; i++ )
        {
        TApaTask task = taskList.FindApp( KApplicationsToClose[i] );
        if ( task.Exists() )
            {
            INFO_1( "Closing UI app with UID: %d", KApplicationsToClose[i].iUid );

            task.EndTask();
            }
        }

    CleanupStack::PopAndDestroy( &session );
    }

// ---------------------------------------------------------------------------
// Define and publish the P&S key to inform all related services about the
// installation, so that they can shutdown themselves gracefully
// ---------------------------------------------------------------------------
//
void CEmailShutter::PublishPsKey( TInt aKey )
    {
    FUNC_LOG;
    TInt error = RProperty::Define( aKey,
                                    RProperty::EInt,
                                    KAllowAllPolicy,
                                    KPowerMgmtPolicy);
    
    ERROR_1( error, "RProperty::Define error code: %d", error );
    
    RProperty psProperty;
    error = psProperty.Attach( KEmailShutdownPsCategory,
                               aKey );
    ERROR_1( error, "RProperty::Attach error code: %d", error );
    
    if( error == KErrNone )
        {
        error = psProperty.Set( KEmailShutterPsValue );

        ERROR_1( error, "RProperty::Set error code: %d", error );
        }
    psProperty.Close();
    }

// -----------------------------------------------------------------------------
// Closes all non-email related plugins/services
// -----------------------------------------------------------------------------
//
void CEmailShutter::Close3rdPartyServices()
    {
    FUNC_LOG;
    TRAP_IGNORE( ClosePcsServerL() );
    TRAP_IGNORE( CloseAOPluginL() );
    }

// -----------------------------------------------------------------------------
// Sends command to Always Online server to stop fs email plugin
// -----------------------------------------------------------------------------
//
void CEmailShutter::CloseAOPluginL()
    {
    FUNC_LOG;

    RAlwaysOnlineClientSession aoSession;
    CleanupClosePushL( aoSession );
    TPckgBuf<TUid> id( KAlwaysOnlineEmailPluginUid );
    aoSession.SendSinglePacketL( EServerAPIBaseCommandStop, id );
    CleanupStack::PopAndDestroy( &aoSession );
    }


// -----------------------------------------------------------------------------
// Sends command to Always Online server to start fs email plugin
// -----------------------------------------------------------------------------
//
void CEmailShutter::StartAoPluginL() const
    {
    FUNC_LOG;
    RAlwaysOnlineClientSession aoSession;
    CleanupClosePushL( aoSession );
    TPckgBuf<TUid> id( KAlwaysOnlineEmailPluginUid );
    aoSession.SendSinglePacketL( EServerAPIBaseCommandStart, id );
    CleanupStack::PopAndDestroy( &aoSession );
    }

// -----------------------------------------------------------------------------
// Closes PCS (Predictive Contact Search) server
// -----------------------------------------------------------------------------
//
void CEmailShutter::ClosePcsServerL()
    {
    FUNC_LOG;
    // If PCS server not running, don't request shutdown, because in this case 
    // the server need to be started before it can process the shutdown request
    if( IsProcessRunning( KPcsServerProcessUidAsTInt ) )
        {
        INFO( "Closing PCS server" );
        CPSRequestHandler* pcs = CPSRequestHandler::NewLC();
        pcs->ShutdownServerL();
        CleanupStack::PopAndDestroy( pcs );
        }
    }

// -----------------------------------------------------------------------------
// Starts PCS (Predictive Contact Search) server
// -----------------------------------------------------------------------------
//
void CEmailShutter::StartPcsServerL() const
    {
    FUNC_LOG;
    CPSRequestHandler* pcs = CPSRequestHandler::NewL();
    delete pcs;
    }

// ---------------------------------------------------------------------------
// Try to find the UID given as parameter from the array given as parameter.
//
// @param aSid Process UID to be searched
// @param aArray Array from where to search
// @param aArrayCount Item count of the aArray
// @return ETrue if the UID can be found from the array, otherwise EFalse.
// ---------------------------------------------------------------------------
//
TBool CEmailShutter::FindFromArray(
        const TUid aSid,
        const TUid aArray[],
        const TInt aArrayCount )
    {
    for( TInt i = 0; i < aArrayCount; i++ )
        {
        if( aArray[i] == aSid )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Checks does this UID belong to the list of the services that we need to
// close down.
//
// @param aSid Process UID to check
// @param aMode Killing mode, are we now killing clients, plugins or msg store
// @return ETrue if this is one of the services we need to close in specified
//         mode, otherwise EFalse
// ---------------------------------------------------------------------------
//
TBool CEmailShutter::NeedToKillThisProcess(
        const TUid aSid,
        const TEmailShutterKillingMode aMode )
    {
    if( aMode == EKillingModeClients ||
        aMode == EKillingModeAll )
        {
        // In case of clients we need to check applications and other clients
        TInt count = sizeof(KApplicationsToClose) / sizeof(TUid);
        if( FindFromArray( aSid, KApplicationsToClose, count ) )
            {
            return ETrue;
            }
        
        count = sizeof(KOtherClientsToClose) / sizeof(TUid);
        if( FindFromArray( aSid, KOtherClientsToClose, count ) )
            {
            return ETrue;
            }
        }

    if( aMode == EKillingModePlugins ||
        aMode == EKillingModeAll )
        {
        TInt count = sizeof(KPluginProcessesToClose) / sizeof(TUid);
        if( FindFromArray( aSid, KPluginProcessesToClose, count ) )
            {
            return ETrue;
            }
        }

    if( aMode == EKillingModeMsgStore ||
        aMode == EKillingModeAll )
        {
        TInt count = sizeof(KMsgStoreProcessesToClose) / sizeof(TUid);
        if( FindFromArray( aSid, KMsgStoreProcessesToClose, count ) )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// Kills all the related processes gracelessly. This is used as a backup for
// those processes that didn't close themselves gracefully.
// ---------------------------------------------------------------------------
//
TBool CEmailShutter::KillEmAll(
        const TEmailShutterKillingMode aMode, /* = EKillingModeAll*/
        const TBool aOnlyCheckIfRunning /* = EFalse */ )
    {
    FUNC_LOG;
    // Having tried graceful shutdown, we need to kill any remaining processes
    // matching the SID.  Note that killing a process may re-order the list of
    // remaining processes, so the search must start from the top again.

    TBool found( EFalse );
    TBool needToScanFullList;
    TFullName fullName;
    
    do
        {
        needToScanFullList = EFalse;
        TFindProcess findProcess;

        while( findProcess.Next(fullName) == KErrNone )
            {
            RProcess process;
            TInt error = process.Open( findProcess );
            // In case of error just skip this process, don't leave
            if( error == KErrNone )
                {
                TUid sid( process.SecureId() );
                if ( NeedToKillThisProcess( sid, aMode ) && // Is this our process and
                     process.ExitType() == EExitPending )  // and is the process alive
                    {
                    found = ETrue;

                    // Kill the found process if aOnlyCheckIfRunning flag not set
                    if( !aOnlyCheckIfRunning )
                        {
                        INFO_1( "Killing process with UID: %d", sid.iUid );
    
                        process.Kill(KErrNone);
                        needToScanFullList = ETrue;
                        }
                    }
                }
            process.Close();
            }
        
        } while (needToScanFullList);
    
    return found;
    }

// ---------------------------------------------------------------------------
// Highest level function to close everything in right order
// ---------------------------------------------------------------------------
//
void CEmailShutter::StartShutdown()
    {
    FUNC_LOG;
    // Cancel Email Server monitoring before closing the server
    if( iMonitor )
        {
        iMonitor->Cancel();
        }
    
    // End all clients
    EndClients();
    // Wait some time to give the clients some time to shut down themselves
    Wait( KTimeToWaitApplicationsInSeconds );
    
    // End all 3rd party clients
    Close3rdPartyServices();
    WaitInCycles( KMaxTimeToWaitClientsInSeconds, EKillingModeClients );

    // Kill gracelessly all remaining clients
    KillEmAll( EKillingModeClients );

    // Define and publish the P&S key to give the plugin servers change to
    // close themselves gracefully
    PublishPsKey( EEmailPsKeyShutdownPlugins );
    // Wait some time to give the plugins some time to shut down themselves
    WaitInCycles( KMaxTimeToWaitPluginsInSeconds, EKillingModePlugins );
    // Kill gracelessly all remaining plugin processes
    KillEmAll( EKillingModePlugins );

    // Define and publish the P&S key to give the msg store change to close
    // itself gracefully
    PublishPsKey( EEmailPsKeyShutdownMsgStore );
    // Wait some time to give the msg store some time to shut down itself
    WaitInCycles( KMaxTimeToWaitMsgStoreInSeconds, EKillingModeMsgStore );
    // Kill gracelessly all remaining message store processes
    KillEmAll( EKillingModeMsgStore );
    
    // Kill gracelessly all remaining processes
    KillEmAll();

    INFO( "Finished Shutdown" );
    }

// ---------------------------------------------------------------------------
// Waits in cycles and checks between cycles have all relevant processes
// closed or do we still need to wait. Returns when either aMaxWaitTime
// has elapsed or when all relevant processes have been shutdown.
// ---------------------------------------------------------------------------
//
void CEmailShutter::WaitInCycles(
        const TInt aMaxWaitTime,
        const TEmailShutterKillingMode aMode )
    {
    FUNC_LOG;
    TInt totalWaitTime = 0;
    do
        {
        // Do the wait and increase total waiting time counter
        Wait( KWaitTimeCycleInSeconds );
        totalWaitTime += KWaitTimeCycleInSeconds;
        // Do this as long as aMaxWaitTime has not elapsed and 
        // there are some process(es) to wait for
        } while ( ( totalWaitTime < aMaxWaitTime ) && KillEmAll( aMode, ETrue ) );
    }
