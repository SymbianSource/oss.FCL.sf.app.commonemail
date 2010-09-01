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
* Description:  Field event queue implementation
*
*/

#include "cmrsystemnotifier.h"
#include "cesmrfieldeventqueue.h"
#include "cesmrfieldcommandevent.h"
#include "esmrdef.h"
#include "emailtrace.h"

#include <bacntf.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRSystemNotifier::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
CMRSystemNotifier::CMRSystemNotifier( CESMRFieldEventQueue& aEventQueue )
:   iEventQueue( aEventQueue )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSystemNotifier::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
CMRSystemNotifier::~CMRSystemNotifier()
    {
    FUNC_LOG;
    delete iSystemNotifier;
    }

// ---------------------------------------------------------------------------
// CMRSystemNotifier::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
CMRSystemNotifier* CMRSystemNotifier::NewL( 
            CESMRFieldEventQueue& aEventQueue )
    {
    FUNC_LOG;
    CMRSystemNotifier* self = new (ELeave) CMRSystemNotifier( aEventQueue );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRSystemNotifier::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
void CMRSystemNotifier::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSystemNotifier::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
void CMRSystemNotifier::StartL()
    {
    FUNC_LOG;
    
    if ( !iSystemNotifier )
        {
        TCallBack envCallback( EnvChangeCallbackL, this );
        
        iSystemNotifier = 
                CEnvironmentChangeNotifier::NewL(
                        EPriorityNormal,
                        envCallback );
        
        iSystemNotifier->Start();
        }    
    }

// ---------------------------------------------------------------------------
// CMRSystemNotifier::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
void CMRSystemNotifier::Stop()
    {
    FUNC_LOG;
    delete iSystemNotifier;
    iSystemNotifier = NULL;
    }

// ---------------------------------------------------------------------------
// CMRSystemNotifier::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
TInt CMRSystemNotifier::EnvChangeCallbackL( TAny* aThisPtr )
    {
    CMRSystemNotifier* thisObject = static_cast<CMRSystemNotifier*>(aThisPtr);

    ASSERT( thisObject );
    
    thisObject->DoEnvChangeL();
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMRSystemNotifier::DoEnvChangeL
// ---------------------------------------------------------------------------
//
void CMRSystemNotifier::DoEnvChangeL()
    {
    FUNC_LOG;
    
    // EChangesLocale
    TInt change( iSystemNotifier->Change() );
    
    if ( EChangesLocale & change )
        {
        // Broadcast MRUI event
        iEventQueue.NotifyEventAsyncL( 
                CESMRFieldCommandEvent::NewL( NULL, EMRCmdDoEnvironmentChange) );
        }
    }

// EOF
