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
* Description:  Asynch command notifier
*
*/
#include "cmrasynchcmd.h"

#include <akntoolbarobserver.h>

// ---------------------------------------------------------------------------
// CMRAsynchCmd::NewL
// ---------------------------------------------------------------------------
//
CMRAsynchCmd* CMRAsynchCmd::NewL()
    {
    CMRAsynchCmd* self = new (ELeave) CMRAsynchCmd();
    return self;
    }

// ---------------------------------------------------------------------------
// CMRAsynchCmd::~CMRAsynchCmd
// ---------------------------------------------------------------------------
//
CMRAsynchCmd::~CMRAsynchCmd()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CMRAsynchCmd::NotifyObserver
// ---------------------------------------------------------------------------
//
void CMRAsynchCmd::NotifyObserver( TInt aCmd, MAknToolbarObserver* aObserver )
    {
    iCommand = aCmd;
    iObserver = aObserver;
    Call();
    }

// ---------------------------------------------------------------------------
// CMRAsynchCmd::CMRAsynchCmd
// ---------------------------------------------------------------------------
//
CMRAsynchCmd::CMRAsynchCmd()
: CAsyncOneShot( CActive::EPriorityStandard )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRAsynchCmd::RunL
// ---------------------------------------------------------------------------
//
void CMRAsynchCmd::RunL()
    {
    if( iObserver )
        {
        iObserver->OfferToolbarEventL( iCommand );
        }
    }

// End of file

