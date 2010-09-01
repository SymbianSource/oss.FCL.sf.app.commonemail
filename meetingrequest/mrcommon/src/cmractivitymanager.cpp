/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR policy implementation
*
*/

#include "cmractivitymanager.h"

// ----------------------------------------------------------------------------
// CMRActivityManager::CMRActivityManager
// ----------------------------------------------------------------------------
//
CMRActivityManager::CMRActivityManager(
                MMRActivityManagerObserver& aObserver,
                TInt aTimeoutInSecs )
: iObserver( aObserver ), iTimeoutInSecs( aTimeoutInSecs)
    {
    
    }

// ----------------------------------------------------------------------------
// CMRActivityManager::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CMRActivityManager* CMRActivityManager::NewL(
                MMRActivityManagerObserver& aObserver,
                TInt aTimeoutInSecs )
    {
    CMRActivityManager* self = new 
            ( ELeave )CMRActivityManager( aObserver, aTimeoutInSecs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CMRActivityManager::~CMRActivityManager()
// ----------------------------------------------------------------------------
//
EXPORT_C CMRActivityManager::~CMRActivityManager()
    {
    if( iPeriodicTimer )
        {
        // Calling Cancel without checking if the timer is active is safe
        iPeriodicTimer->Cancel();
        }
    delete iPeriodicTimer;
    }
 
// ----------------------------------------------------------------------------
// CMRActivityManager::ConstructL()
// ----------------------------------------------------------------------------
//
void CMRActivityManager::ConstructL()
    {
    // Initialize the periodic timer.
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityIdle );
    }

// ----------------------------------------------------------------------------
// CMRActivityManager::SetTimeout()
// ----------------------------------------------------------------------------
//

EXPORT_C void CMRActivityManager::SetTimeout( TInt aTimeout )
    {
    iTimeoutInSecs = aTimeout;
    }

// ----------------------------------------------------------------------------
// CMRActivityManager::Start()
// ----------------------------------------------------------------------------
//

EXPORT_C void CMRActivityManager::Start()
    {
    if ( iPeriodicTimer )
        {
        iPeriodicTimer->Cancel();
        }
    else
        {
        TRAP_IGNORE(iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityIdle ));
        }
    if ( iPeriodicTimer && !iPeriodicTimer->IsActive() )
        {
        TInt secs( 1000000 * iTimeoutInSecs );
        iPeriodicTimer->Start(
            secs, secs,
            TCallBack( PeriodicTimerCallBack, this ) );
        }
    }

// ----------------------------------------------------------------------------
// CMRActivityManager::Reset()
// ----------------------------------------------------------------------------
//
EXPORT_C void CMRActivityManager::Reset()
    {
    if( iPeriodicTimer )
        {
        // Calling Cancel without checking if the timer is active is safe
        iPeriodicTimer->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CMRActivityManager::PeriodicTimerCallBack(TAny* aAny)
// The call back function.
// ----------------------------------------------------------------------------
//
TInt CMRActivityManager::PeriodicTimerCallBack(TAny* aAny)
    {
    CMRActivityManager* self = static_cast<CMRActivityManager*>( aAny );
    self->NotifyObserver();
 
    // Cancel the timer when the callback should not be called again.
    self->iPeriodicTimer->Cancel();
 
    return KErrNone; // Return value ignored by CPeriodic
    }
 
// ----------------------------------------------------------------------------
// CMRActivityManager::SomeFunction(TAny* aAny)
// Notice that this is a sample fuction.
// ----------------------------------------------------------------------------
//
void CMRActivityManager::NotifyObserver()
    {
    iObserver.PeriodCompleted();
    }
