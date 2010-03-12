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
* Declaration of common functions of Email Server Monitor
*
*/
//  Include Files  

#ifndef EMAILSERVERMONITORUTILITIES_H
#define EMAILSERVERMONITORUTILITIES_H

#include <e32base.h>
#include <e32cmn.h>     // TSecureId

// Forward declarations
class CEmailServerMonitorTimer;

const TInt KDefaultTimeToWaitInSeconds = 3;

/////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION DECLARATIONS

/**
 * Wait some time to give other applications and servers some time to shut
 * down themselves gracefully or to start themselves
 * 
 * @param aWaitTimeInSeconds Time to wait in seconds
 */
void Wait( TInt aWaitTimeInSeconds = KDefaultTimeToWaitInSeconds );

/**
 * Check is specified process running. If we are checking other UID than
 * ourselves, we just check is there specified process running. If we are
 * checking our own UID, then we check is there another instance already
 * running.
 * 
 * @param aUid Process UID (=Secure ID) to be searched
 * @param aProcess If process handle is given, it will updated to have a
 *                 a handle to the found process
 * @param aProcessUpdated On return ETrue, if aProcess handle was given and
 *                        it was updated succesfully
 * @return ETrue if the specified process is running, otherwise EFalse.
 */
TBool IsProcessRunning( TSecureId aUid, RProcess* aProcess = NULL );


/**
*  Generic timer's callback
*
*  Classes inherited from this one can be used as a callback for the generic
*  timer. TimerEvent() is called when the timer elapses.
*
*/
class MEmailServerMonitorTimerCallback
    {
public:
    /**
     * @param   aTriggeredTimer The timer which launched this event.
     */
    virtual void TimerEventL( CEmailServerMonitorTimer* aTriggeredTimer ) = 0;
    };

/**
*  Generic timer
*
*  Caller gives MEmailServerMonitorTimerCallback inherited pointer as a
*  parameter, and that pointer is used as a callback when the timer elapses.
*  Timer interval is given as a parameter for the Start function.
*
*/
class CEmailServerMonitorTimer : public CTimer
    {
public:
    
    /**
    * Symbian two-pahse constructor.
    *
    * @param aCallback Callback class
    * @param aPriority Timers priority, EPriorityStandard by default
    */
    static CEmailServerMonitorTimer* NewL(
        MEmailServerMonitorTimerCallback* aCallback,
        const TInt aPriority = CActive::EPriorityStandard );

    /**
    * Symbian two-pahse constructor.
    *
    * @param aCallback Callback class
    * @param aPriority Timers priority, EPriorityStandard by default
    */
    static CEmailServerMonitorTimer* NewLC(
        MEmailServerMonitorTimerCallback* aCallback,
        const TInt aPriority = CActive::EPriorityStandard );
        
    /**
    * Destructor.
    */
    ~CEmailServerMonitorTimer();
        
    /**
    * Starts the timer with specified interval, or with the default value.
    *
    * @param aInterval Timer interval as microseconds
    */
    void Start( TInt aInterval );

    /**
    * Stops the timer.
    */
    void Stop();

    /**
    * CActive object's RunL
    */
    void RunL();
        
protected:
    /**
    * Constructor.
    */
    CEmailServerMonitorTimer( MEmailServerMonitorTimerCallback* aCallback,
                              const TInt aPriority );
        
private:
    /**
    * 2nd phase constructor.
    */
    void ConstructL();

private:
    /* Pointer to callback class */
    MEmailServerMonitorTimerCallback* iCallback;
    };


#endif // EMAILSERVERMONITORUTILITIES_H
