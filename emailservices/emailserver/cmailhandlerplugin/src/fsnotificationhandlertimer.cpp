/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSNotificationHandlerTimer.
*
*/


#include "emailtrace.h"
#include "fsnotificationhandlertimer.h"




// ======== MEMBER FUNCTIONS ========

CFSNotificationHandlerTimer::CFSNotificationHandlerTimer(
    MFSNotificationHandlerTimerObserver& aObserver ) :
    CTimer( EPriorityStandard ),
    iObserver( aObserver )
    {
    FUNC_LOG;
    }

void CFSNotificationHandlerTimer::ConstructL()
    {
    FUNC_LOG;
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

CFSNotificationHandlerTimer* CFSNotificationHandlerTimer::NewL(
    MFSNotificationHandlerTimerObserver& aObserver )
    {
    FUNC_LOG;
    CFSNotificationHandlerTimer* self =
        CFSNotificationHandlerTimer::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

CFSNotificationHandlerTimer* CFSNotificationHandlerTimer::NewLC(
    MFSNotificationHandlerTimerObserver& aObserver )
    {
    FUNC_LOG;
    CFSNotificationHandlerTimer* self =
        new( ELeave ) CFSNotificationHandlerTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CFSNotificationHandlerTimer::~CFSNotificationHandlerTimer()
    {
    FUNC_LOG;
    Cancel();
    }

void CFSNotificationHandlerTimer::DoCancel()
    {
    FUNC_LOG;
    // Cancel Base class
    CTimer::DoCancel(); 
    }

void CFSNotificationHandlerTimer::RunL()
    {
    FUNC_LOG;
    iObserver.TimerCallBackL( iStatus.Int() );
    }

// End of file

