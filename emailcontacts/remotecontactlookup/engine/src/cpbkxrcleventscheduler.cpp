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
* Description:  Implementation of the class CPbkxRclServiceUiContextImpl.
*
*/


#include "emailtrace.h"
#include "cpbkxrcleventscheduler.h"

const TInt KDelay = 1;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclEventScheduler::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclEventScheduler* CPbkxRclEventScheduler::NewL( MPbkxRclEventHandler& aHandler )
    {
    FUNC_LOG;
    CPbkxRclEventScheduler* scheduler = new ( ELeave ) CPbkxRclEventScheduler( aHandler );
    CleanupStack::PushL( scheduler );
    scheduler->ConstructL();
    CleanupStack::Pop( scheduler );
    return scheduler;
    }

// ---------------------------------------------------------------------------
// CPbkxRclEventScheduler::CPbkxRclEventScheduler
// ---------------------------------------------------------------------------
//
CPbkxRclEventScheduler::CPbkxRclEventScheduler( MPbkxRclEventHandler& aHandler ) :
    CActive( EPriorityStandard ), iEventHandler( aHandler )
    {
    FUNC_LOG;
    }

CPbkxRclEventScheduler::~CPbkxRclEventScheduler()
    {
    FUNC_LOG;
    Cancel();
    iTimer.Close();
    }

void CPbkxRclEventScheduler::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );
    }

void CPbkxRclEventScheduler::TriggerEvent()
    {
    FUNC_LOG;
    if ( !IsActive() )
        {
        iTimer.After( iStatus, TTimeIntervalMicroSeconds32( KDelay ) );
        SetActive();
        }
    }

void CPbkxRclEventScheduler::RunL()
    {
    FUNC_LOG;
    if ( iStatus.Int() == KErrNone )
        {
        iEventHandler.EventTriggered();
        }
    }

void CPbkxRclEventScheduler::DoCancel()
    {
    FUNC_LOG;
    iTimer.Cancel();
    }
    

