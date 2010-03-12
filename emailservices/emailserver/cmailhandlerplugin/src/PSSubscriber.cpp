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
* Description: Subscriber class for Publish & Subsribe events
*/

#include <e32svr.h>

#include "PSSubscriber.h"
#include "PSSubscribeHandler.h"
#include "emailtrace.h"

// ----------------------------------------------------------------------------
// CPSSubscriber::NewL()
// ----------------------------------------------------------------------------
CPSSubscriber* CPSSubscriber::NewL( MPSSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey )
    {
    FUNC_LOG;
    CPSSubscriber* self = new(ELeave) CPSSubscriber( aPropertyResponder, aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

// ----------------------------------------------------------------------------
// CPSSubscriber::ConstructL()
// ----------------------------------------------------------------------------
void CPSSubscriber::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    TInt ret = iProperty.Attach( iCategory, iKey );
    INFO_1("iProperty.Attach, ret == %d", ret);
    }

// ----------------------------------------------------------------------------
// CPSSubscriber::Subscribe()
// ----------------------------------------------------------------------------
void CPSSubscriber::Subscribe()
    {
    FUNC_LOG;
    if ( !IsActive() )
        {
        iProperty.Cancel();
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CPSSubscriber::CPSSubscriber()
// ----------------------------------------------------------------------------
CPSSubscriber::CPSSubscriber( MPSSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey ) :
    CActive( EPriorityStandard ),
    iPropertyResponder( aPropertyResponder ),
    iCategory( aCategory),
    iKey( aKey )
    {
    }

// ----------------------------------------------------------------------------
// CPSSubscriber::RunL()
// ----------------------------------------------------------------------------
void CPSSubscriber::RunL()
    {
    FUNC_LOG;
    Subscribe();
    iPropertyResponder.HandlePropertyChangedL( iCategory, iKey );
    }

// ----------------------------------------------------------------------------
// CPSSubscriber::DoCancel()
// ----------------------------------------------------------------------------
void CPSSubscriber::DoCancel()
    {
    FUNC_LOG;
    iProperty.Cancel();
    }

// ----------------------------------------------------------------------------
// CPSSubscriber::RunError()
// ----------------------------------------------------------------------------
TInt CPSSubscriber::RunError( TInt aError )
    {
    FUNC_LOG;
    return aError;
    }

// ----------------------------------------------------------------------------
// CPSSubscriber::~CPSSubscriber()
// ----------------------------------------------------------------------------
CPSSubscriber::~CPSSubscriber()
    {
    FUNC_LOG;
    Cancel();
    iProperty.Close();
    }
