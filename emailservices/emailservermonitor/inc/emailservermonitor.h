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
* Header definition for EmailServerMonitor
*
*/

#ifndef EMAILSERVERMONITOR_H
#define EMAILSERVERMONITOR_H

//  Include Files

#include <e32base.h>
#include <e32cmn.h>						// TUid
#include <apgcli.h>                     // RApaLsSession

#include "emailservermonitorconst.h"

/////////////////////////////////////////////////////////////////////////////
//  FORWARD DECLARATIONS
class CEmailShutter;

/**
*  Class CEmailServerMonitor implements functionality that starts up
*  EmailServer and monitors it. If EmailServer crashes, then
*  CEmailServerMonitor restarts it. CEmailServerMonitor also starts up needed
*  email related 3rd party services after IAD update.
*/
NONSHARABLE_CLASS( CEmailServerMonitor ) : public CActive, public MEmailServerMonitorTimerCallback
    {
    
public: // Public construcor and destructor
    /**
     * Two-phased class constructor.
     */
    static CEmailServerMonitor* NewL();

    /**
     * Two-phased class constructor.
     */
    static CEmailServerMonitor* NewLC();

    /**
     * Destructor of CEmailServerMonitor class.
     */
    ~CEmailServerMonitor();
        
public: // Public member functions
    
    /**
     * Start email server monitoring.
     */
    void Start();
    
    /**
     * Set pointer to Shutter object
     */
    void SetShutter( CEmailShutter* aShutter );
    
    /**
     * If Restart External Services flag is set to true, then external
     * services will be restarted after Email Server is up and running
     */
    void SetRestartExternalServicesFlag( TBool aRestartFlag = ETrue );
    
public: // From base class MEmailServerMonitorTimerCallback
    
    void TimerEventL( CEmailServerMonitorTimer* aTriggeredTimer );

private: // Private constructors
    /**
     * Default class constructor.
     * Only NewL can be called
     */
    CEmailServerMonitor();
        
    /**
     * Second phase class constructor.
     */
    void ConstructL();      

    /**
     * Start Email Server and its monitoring
     * 
     * @return ETrue if Email Server and its monitoring started succesfully,
     *         otherwise EFalse
     */
    TBool StartEmailServerMonitoring();
    
    /**
     * Starts the actual monitoring. Assumes that Email Server is running
     * and Monitor's internal variables are set accordingly.
     */
    void DoStartMonitoring();
    
    /**
     * Start specified application, leaves if failed to start the application
     * 
     * @param aAppName Name of the application to be started
     * @param aThreadId On return, the id of the main thread started
     * @param aReqStatusForRendezvous Asynchronous status request passed to
     *                                RProcess::Rendezvous() when starting app
     */
    void StartApplicationL(
            const TDesC& aAppName,
            TThreadId& aThreadId,
            TRequestStatus& aReqStatusForRendezvous );
    
    /**
     * Handles monitoring event
     */
    void HandleMonitorEvent();
    
    /**
     * Initiates an asynchronous delayed restart
     */
    void InitiateDelayedRestart();

private: // From base class CActive
    
    void RunL();
    void DoCancel();

private: // Private types
    
    enum TEmailServerMonitorState
        {
        EEsmStateIdle,
        EEsmStateInitializing,
        EEsmStateMonitoring,
        EEsmStateRestarting
        };

    enum TExternalServiceRestartState
        {
        EEsmEsrStateRestartNotNeeded,
        EEsmEsrStateRestartNeeded,
        EEsmEsrStateRestartInitiated,
        EEsmEsrStateFirstServiceRestarted,
        };
    
private: // Member variables

    // A session with the application architecture server
    RApaLsSession iApaLsSession;
    
    // A handle to email server process
    RProcess iProcess;
    
    // A timer object used to make delayed restarts
    RTimer iDelayTimer;
    
    // Internal state
    TEmailServerMonitorState iState;
    
    // Number of server restarts
    TInt iRestarts;
    
    // Time of the previous restart, used to reset restart counter if
    // server has been running succesfully long enough
    TTime iLastRestartTime;

    /**
     * Pointer to Shutter
     * Not owned
     */
    CEmailShutter* iShutter;

    /**
     * Should we restart external services after Email Server is up and running
     */
    TExternalServiceRestartState iExternalServicesRestartState;
    
    /**
     * Generic timer used to make delayed restart of the external services
     * Owned
     */
    CEmailServerMonitorTimer* iExternalServiceRestartTimer;

    };


#endif  // EMAILSERVERMONITOR_H
