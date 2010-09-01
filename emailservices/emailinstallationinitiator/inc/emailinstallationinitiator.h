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
* This file defines class CEmailInstallationInitiator.
*
*/


#ifndef EMAILINSTALLATIONINITIATOR_H
#define EMAILINSTALLATIONINITIATOR_H


#include <e32base.h>
#include <e32property.h>                // RProperty

/**
*  Class CEmailInstallationInitiator implements functionality that launches
*  EmailServerMonitor (if it's not already running) and sends Publish &
*  Subscribe event to it to start email shutdown. After that it will start
*  observing callback event from Shutter indicating that installation
*  can continue. After that event initiator simply exits to enable
*  installation to continue.
*/
NONSHARABLE_CLASS( CEmailInstallationInitiator ) : public CActive
    {
public:
    
    /**
     * Construction/destruction
     */
    static CEmailInstallationInitiator* NewL();
    static CEmailInstallationInitiator* NewLC();
    ~CEmailInstallationInitiator();
    
    /**
     * Start Email Server Monitor, if it's not running already
     */
    void StartEmailServerMonitor();

    /**
     * Send installation start event to Email Server Monitor / Shutter and
     * start observing "installation OK to continue" event
     */
    void SendInstallationEventAndStartObserving();
    
    /**
     * Wait some time to give earlier processes some time to launch themselves
     * 
     * @param aWaitTimeInSeconds Time to wait seconds,
     *                           if <= 0, then use default value
     */
    void Wait( TInt aWaitTimeInSeconds = -1 );

private: // From base class CActive
    
    void RunL();
    void DoCancel();

private:
    
    /**
     * Private 2 phase constructors
     */
	CEmailInstallationInitiator();
    void ConstructL();
    
    /**
     * Starts one executable
     *
     * @param aAppName Binary name for the executable to be launched
     */
    void StartOneApplicationL( const TDesC& aAppName );

    /**
     * Checks if process with specified name is running
     * 
     * @param aAppName Name of the process
     * @return ETrue if the specified process is running, EFalse otherwise
     */
    TBool IsProcessRunningL( const TDesC& aAppName );

    /**
     * Starts observing "installation OK to continue" event
     * from Email Server Monitor / Shutter
     */
    void StartObservingShutdownEvent();

private:

    // A session with the application architecture server
    RApaLsSession iApaLsSession;
    
    // Publish & Subscribe property
    RProperty iProperty;

    };


#endif // EMAILINSTALLATIONINITIATOR_H
