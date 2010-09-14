/*
 * ============================================================================
 *  Name        : timeouttimer.h
 *  Part of     : ipsservice / profiletester
 *  Description : timer implementation
 *  Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
 *
 *  Copyright ©2010-2010 Nokia and/or its subsidiary(-ies). All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 */

#ifndef C_TIMEOUTTIMER_H
#define C_TIMEOUTTIMER_H

#include <e32base.h>
#include <e32std.h>
class MTimeoutObserver;

/**
 *  TimeoutObserver 
 *  Api for timeout observers
 */
class MTimeoutObserver
    {

public:
    virtual void TimeoutNotify() = 0;

    };

class CTimeoutTimer : protected CTimer
    {
public:

    static CTimeoutTimer* NewL(MTimeoutObserver& aHandle);
    static CTimeoutTimer* NewLC(MTimeoutObserver& aHandle);
    void Start(TInt aTimePeriod);
    void Stop();
    virtual ~CTimeoutTimer();

protected:

    // from CTimer
    void RunL();
    TInt RunError(TInt aError);

private:

    CTimeoutTimer(MTimeoutObserver& aHandle);
    void ConstructL();

private:

    // timetout notifier handle
    MTimeoutObserver& iNotifyHandle;
    };

#endif // C_TIMEOUTTIMER_H
