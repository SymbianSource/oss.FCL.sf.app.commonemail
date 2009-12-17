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

    CEUiExitGuardian* self = new (ELeave) CEUiExitGuardian(aAppUi);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// ---------------------------------------------------------------------------
// CEUiExitGuardian::CEUiExitGuardian
//
// ---------------------------------------------------------------------------
//
CEUiExitGuardian::CEUiExitGuardian( CFreestyleEmailUiAppUi& aAppUi )
    : iAppUi(aAppUi)
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

    delete iIdle;
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::ConstructL
//
// ---------------------------------------------------------------------------
//
void CEUiExitGuardian::ConstructL()
    {
    FUNC_LOG;

    iIdle = CIdle::NewL(CActive::EPriorityStandard);
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::EnterLC
//
// ---------------------------------------------------------------------------
//
void CEUiExitGuardian::EnterLC()
    {
    FUNC_LOG;

    if (iExitPending)
        {
        User::Leave(KErrNotReady);
        }
    iEnterCount++;
    CleanupStack::PushL(TCleanupItem(Exit, this));
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::Exit
//
// ---------------------------------------------------------------------------
//
void CEUiExitGuardian::Exit(TAny* aPtr)
    {
    FUNC_LOG;

    reinterpret_cast<CEUiExitGuardian*>(aPtr)->DoExit();
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::DoExit
//
// ---------------------------------------------------------------------------
//
void CEUiExitGuardian::DoExit()
    {
    FUNC_LOG;

    iEnterCount--;
    if (iExitPending && !iEnterCount && !iIdle->IsActive())
        {
        // Start async exit processing to rewind recursion before the actual
        // exit
        iIdle->Start( TCallBack( IdleCallBack, this ) );
        }
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::IdleCallBack
//
// ---------------------------------------------------------------------------
//
TInt CEUiExitGuardian::IdleCallBack( TAny* aPtr )
    {
    FUNC_LOG;

    reinterpret_cast<CEUiExitGuardian*>( aPtr )->iAppUi.ExitNow();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CEUiExitGuardian::ExitApplication
//
// ---------------------------------------------------------------------------
//
TInt CEUiExitGuardian::ExitApplication()
    {
    FUNC_LOG;

    TInt result( KRequestPending );
    iExitPending = ETrue;
    if (!iEnterCount)
        {
        result = KErrNone;
        iAppUi.ExitNow();
        }
    return result;
    }

// end of file
