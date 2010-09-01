/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FreestyleEmailUi exit guardian implementation.
*
*/

#include "FreestyleEmailUiAppui.h"
#include "ceuiexitguardian.h"
#include "emailtrace.h"


// ---------------------------------------------------------------------------
// CEUiExitGuardian::NewL
//
// ---------------------------------------------------------------------------
//
CEUiExitGuardian* CEUiExitGuardian::NewL( CFreestyleEmailUiAppUi& aAppUi )
    {
    FUNC_LOG;

    CEUiExitGuardian* self = new (ELeave) CEUiExitGuardian( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::CEUiExitGuardian
//
// ---------------------------------------------------------------------------
//
CEUiExitGuardian::CEUiExitGuardian( CFreestyleEmailUiAppUi& aAppUi )
    : iAppUi( aAppUi )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::~CEUiExitGuardian
//
// ---------------------------------------------------------------------------
//
CEUiExitGuardian::~CEUiExitGuardian()
    {
    FUNC_LOG;

    if ( iPeriodicTimer )
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        }
    }


// ---------------------------------------------------------------------------
// CEUiExitGuardian::ConstructL
//
// ---------------------------------------------------------------------------
//
void CEUiExitGuardian::ConstructL()
    {
    FUNC_LOG;

    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iPeriodicTimerStarted = EFalse;
    }


// ---------------------------------------------------------------------------
// CEUiExitGuardian::PeriodicCallBack
//
// ---------------------------------------------------------------------------
//
TInt CEUiExitGuardian::PeriodicCallBack( TAny* aPtr )
    {
    FUNC_LOG;

    reinterpret_cast<CEUiExitGuardian*>(aPtr)->TryExitApplication();
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CEUiExitGuardian::TryExitApplication
//
// ---------------------------------------------------------------------------
//
TInt CEUiExitGuardian::TryExitApplication()
    {
    FUNC_LOG;
    const TInt KDelay = 200*1000; // 200 ms
    
    TInt ret = KRequestPending;

    CActiveScheduler* scheduler = CActiveScheduler::Current();
    TInt nestedLevelCount = scheduler->StackDepth();
    if ( nestedLevelCount == 1 )
        {
        iPeriodicTimer->Cancel();
        iAppUi.ExitNow();
        ret = KErrNone;
        }
    else if ( !iPeriodicTimerStarted )
        {
        iPeriodicTimer->Start( KDelay, KDelay, TCallBack(PeriodicCallBack, this) );
        iPeriodicTimerStarted = ETrue;
        }
    
    return ret;
    }

// end of file
