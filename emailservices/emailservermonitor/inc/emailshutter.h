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
* Header definition for EmailShutter
*
*/

#ifndef EMAILSHUTTER_H
#define EMAILSHUTTER_H

//  Include Files
#include <e32base.h>        // CActive
#include <e32cmn.h>         // TUid
#include <e32property.h>    // RProperty

/////////////////////////////////////////////////////////////////////////////
//  FORWARD DECLARATIONS
class CEmailServerMonitor;

/////////////////////////////////////////////////////////////////////////////
//  CONSTANT DEFINITIONS

// Security policies used when defining P&S keys
static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_C1(KPowerMgmtPolicy,ECapabilityPowerMgmt);

// Value to be set in shutdown P&S key.
const TInt KEmailShutterPsValue = 1;

// Maximum waiting times used to wait processes to close themselves gracefully
const TInt KTimeToWaitApplicationsInSeconds = 2;
const TInt KMaxTimeToWaitClientsInSeconds = 10;
const TInt KMaxTimeToWaitPluginsInSeconds = 10;
const TInt KMaxTimeToWaitMsgStoreInSeconds = 5;

// Cycle length (in seconds) to check if some processes are still running
const TInt KWaitTimeCycleInSeconds = 2;

// Format string used with process finder to find processes by their UID
_LIT( KFormatProcessFinderByUid, "*[%x]*" );
// Length of the formatted process finder string, basically length of
// KFormatProcessFinderByUid + length of 32 bit number in hexadecimal format
const TInt KFormatProcessFinderByUidLength = 12;


/////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATION

/**
*  Class CEmailShutter implements functionality that shuts down all email
*  related services when IAD/sisx update starts. Shutdown is initiated by a
*  Publish & Subscribe event sent by email installation initiator.
*/
NONSHARABLE_CLASS( CEmailShutter ) : public CActive
    {
    
public: // Public construcor and destructor
    /**
     * Two-phased class constructor.
     */
    static CEmailShutter* NewL();

    /**
     * Two-phased class constructor, leaves created object in cleanup stack.
     */
    static CEmailShutter* NewLC();

    /**
     * Destructor of CEmailShutter class.
     */
    ~CEmailShutter();
        
public: // Public member functions
    
    /**
     * Start observing shutdown event.
     */
    void StartObservingShutdownEvent();

    /**
     * Set P&S key after installation is done
     */
    void SetPsKeyInstallationFinished();

    /**
     * Restart needed services after installation is finished
     */
    void RestartServicesAfterInstallation();

    /**
     * Set pointer to Email Server Monitor object
     */
    void SetMonitor( CEmailServerMonitor* aMonitor );
    
private: // Private definitions
    
    /**
     * Definition of possible killing modes when killing processes gracelessly
     */
    enum TEmailShutterKillingMode
        {
        EKillingModeAll,
        EKillingModeClients,
        EKillingModePlugins,
        EKillingModeMsgStore
        };

private: // Private constructors
    /**
     * Default class constructor.
     * Only NewL can be called
     */
    CEmailShutter();
        
    /**
     * Second phase class constructor.
     */
    void ConstructL();  
    
private: // From base class CActive
    
    void RunL();
    void DoCancel();

private: // Private member functions

    /**
     * Highest level function to close everything in right order
     */
    void StartShutdown();
    
    /**
     * End client applications gracefully
     */
    void EndClients();
    
    /**
     * End UI applications gracefully
     */
    void EndApplicationsL();
    
    /**
     * Define and publish the P&S key to inform all related services about the
     * installation, so that they can shutdown themselves gracefully
     */
    void PublishPsKey( TInt aKey );
    
    /**
     * Closes all non-email related plugins/services
     */
    void Close3rdPartyServices();
    
    /**
     * Sends command to Always Online server to stop fs email plugin
     */
    void CloseAOPluginL();

    /**
     * Sends command to Always Online server to start fs email plugin
     */
    void StartAoPluginL() const;
    
    /**
     * Closes PCS (Predictive Contact Search) server
     */
    void ClosePcsServerL();
    
    /**
     * Starts PCS (Predictive Contact Search) server
     */
    void StartPcsServerL() const;

    /**
     * Try to find the UID given as parameter from the array given as parameter.
     * 
     * @param aSid Process UID to be searched
     * @param aArray Array from where to search
     * @param aArrayCount Item count of the aArray
     * @return ETrue if the UID can be found from the array, otherwise EFalse.
     */
    TBool FindFromArray(
            const TUid aSid,
            const TUid aArray[],
            const TInt aArrayCount );
    
    /**
     * Checks does this UID belong to the list of the services that we need to
     * close down.
     * 
     * @param aSid Process UID to check
     * @param aMode Killing mode, are we now killing clients, plugins or msg store
     * @return ETrue if this is one of the services we need to close in specified
     *         mode, otherwise EFalse
     */
    TBool NeedToKillThisProcess(
            const TUid aSid,
            const TEmailShutterKillingMode aMode );
    
    /**
     * Kills all the related processes gracelessly. This is used as a backup for
     * those processes that didn't close themselves gracefully.
     * 
     * @param aMode Process killing mode as defined in TEmailShutterKillingMode
     * @param aOnlyCheckIfRunning If EFalse (default value) when calling this 
     *           function, it kills all the processes that match the mode and
     *           are running. If ETrue when calling this function, it only
     *           checks if at least one process needs to be killed (is running).
     * @return ETrue if at least one process was killed or need to be killed.
     */
    TBool KillEmAll( const TEmailShutterKillingMode aMode = EKillingModeAll,
                     const TBool aOnlyCheckIfRunning = EFalse );

    /**
     * Waits in cycles and checks between cycles have all relevant processes
     * closed or do we still need to wait. Returns when either aMaxWaitTime
     * has elapsed or when all relevant processes have been shutdown.
     * 
     * @param aMaxWaitTime Maximum time to wait in seconds
     * @param aMode Killing mode, are we now killing clients, plugins or msg
     *              store. Used to determine are there still some processes
     *              that have not closed themselves.
     */
    void WaitInCycles( const TInt aMaxWaitTime,
                       const TEmailShutterKillingMode aMode );
    
private: // Member variables

    /**
     * Publish & Subscribe property to be observed.
     */
    RProperty iInstStatusProperty;

    /**
     * Pointer to Email Server Monitor
     * Not owned
     */
    CEmailServerMonitor* iMonitor;

    };


#endif  // EMAILSHUTTER_H

