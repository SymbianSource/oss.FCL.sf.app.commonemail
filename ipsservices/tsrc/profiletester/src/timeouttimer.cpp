/*
 * ============================================================================
 *  Name        :  timeouttimer.cpp
 *  Part of     :  ipsservice / profiletester 
 *  Description :: timer implementation
 *  Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
 *
 *  Copyright © 2010-2010 Nokia and/or its subsidiary(-ies).  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 */

#include "timeouttimer.h"

// -----------------------------------------------------------------------------
// CTimeoutTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeoutTimer* CTimeoutTimer::NewL(MTimeoutObserver& aHandle)
    {
    CTimeoutTimer* self = CTimeoutTimer::NewLC(aHandle);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeoutTimer* CTimeoutTimer::NewLC(MTimeoutObserver& aHandle)
    {
    CTimeoutTimer* self = new (ELeave) CTimeoutTimer(aHandle);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::CTimeoutTimer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CTimeoutTimer::CTimeoutTimer(MTimeoutObserver& aHandle) :
    CTimer(EPriorityStandard), iNotifyHandle(aHandle)
    {
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::~CTimeoutTimer
// Destructor
// -----------------------------------------------------------------------------
//
CTimeoutTimer::~CTimeoutTimer()
    {
    Stop();
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTimeoutTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::Start
// Start
// -----------------------------------------------------------------------------
//
void CTimeoutTimer::Start(TInt aTimePeriod)
    {
    After(aTimePeriod);
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::Stop
// Stop
// -----------------------------------------------------------------------------
//
void CTimeoutTimer::Stop()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::RunL
// RunL
// -----------------------------------------------------------------------------
//
void CTimeoutTimer::RunL()
    {
    if (iStatus == KErrNone)
        {
        iNotifyHandle.TimeoutNotify();
        }
    else
        {
        User::Leave(iStatus.Int());
        }
    }

// -----------------------------------------------------------------------------
// CTimeoutTimer::RunError
// RunError
// -----------------------------------------------------------------------------
//
TInt CTimeoutTimer::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }
