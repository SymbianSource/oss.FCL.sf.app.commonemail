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


#include "emailtrace.h"
#include "cesmrfieldeventqueue.h"

#include <eikenv.h> // for CEikonEnv::HandleError

#include "mesmrfieldevent.h"
#include "mesmrfieldeventobserver.h"
#include "mesmrfieldeventnotifier.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
CESMRFieldEventQueue::CESMRFieldEventQueue()
    {
    FUNC_LOG;
    // Do nothing
    }


// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::ConstructL()
    {
    FUNC_LOG;
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    }


// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::NewL
// ---------------------------------------------------------------------------
//
CESMRFieldEventQueue* CESMRFieldEventQueue::NewL()
    {
    FUNC_LOG;
    CESMRFieldEventQueue* self = CESMRFieldEventQueue::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::NewLC
// ---------------------------------------------------------------------------
//
CESMRFieldEventQueue* CESMRFieldEventQueue::NewLC()
    {
    FUNC_LOG;
    CESMRFieldEventQueue* self = new( ELeave ) CESMRFieldEventQueue;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::~CESMRFieldEventQueue
// ---------------------------------------------------------------------------
//
CESMRFieldEventQueue::~CESMRFieldEventQueue()
    {
    FUNC_LOG;
    if ( iIdle )
        {
        iIdle->Cancel();
        delete iIdle;
        }
    iObservers.Reset();
    iEventQueue.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// From class MESMRFieldEventQueue
// CESMRFieldEventQueue::AddObserverL
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::AddObserverL( MESMRFieldEventObserver* aObserver )
    {
    FUNC_LOG;
    TInt error = iObservers.InsertInAddressOrder( aObserver );
    if ( error && error != KErrAlreadyExists )
        {
        User::Leave( error );
        }
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEventQueue
// CESMRFieldEventQueue::RemoveObserver
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::RemoveObserver( MESMRFieldEventObserver* aObserver )
    {
    FUNC_LOG;
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if ( index > KErrNotFound )
        {
        iObservers.Remove( index );
        if ( index < iNotifyIndex )
            {
            // Notifying is on going.
            // aObserver has been already notified, so indexes after it will
            // decrease by one. Decrease iNotifyIndex also by one.
            iNotifyIndex = Max( 0, --iNotifyIndex );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// From class MESMRFieldEventQueue
// CESMRFieldEventQueue::NotifyEventL
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::NotifyEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    // Reset observer index
    iNotifyIndex = 0;
    
    // Get event sender
    MESMRFieldEventObserver* sender = NULL;
    if ( aEvent.Source() )
        {
        sender = aEvent.Source()->EventObserver();
        }
    
    while ( iNotifyIndex < iObservers.Count() )
        {
        MESMRFieldEventObserver* observer = iObservers[ iNotifyIndex++ ];
        if ( observer && observer != sender )
            {
            // Notify event to other observers but the sender
            observer->HandleFieldEventL( aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEventQueue
// CESMRFieldEventQueue::NotifyEventAsyncL
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::NotifyEventAsyncL( MESMRFieldEvent* aEvent )
    {
    FUNC_LOG;
    // Append event to queue
    iEventQueue.AppendL( aEvent );
    
    // Start queue
    Start();
    }
    
// ---------------------------------------------------------------------------
// From class MESMRFieldEventQueue
// CESMRFieldEventQueue::CancelEvent
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::CancelEvent( MESMRFieldEvent* aEvent )
    {
    FUNC_LOG;
    // Find event from queue
    TInt index = iEventQueue.Find( aEvent );
    if ( index > KErrNotFound )
        {
        // Remove and delete event
        iEventQueue.Remove( index );
        delete aEvent;
        
        // Stop queue if it is empty.
        if ( iEventQueue.Count() == 0 )
            {
            iIdle->Cancel();
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::NotifyCallback
// CIdle callback function
// ---------------------------------------------------------------------------
//
TInt CESMRFieldEventQueue::NotifyCallBack( TAny* aPtr )
    {
    FUNC_LOG;
    TBool ret = EFalse;
    CESMRFieldEventQueue* self = static_cast< CESMRFieldEventQueue* >( aPtr );
    if ( self )
        {
        TRAPD( error, self->DoNotifyEventAsyncL() )
        if ( error != KErrNone &&
             error != KErrCancel &&
             error != KErrArgument )
            {
            // Show error note 
            CEikonEnv::Static()->HandleError( error ); // codescanner::eikonenvstatic
            }
        ret = self->HasEvents();
        }
    
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::DoNotifyEventAsyncL
// Removes event from the queue and notifies it to the observers.
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::DoNotifyEventAsyncL()
    {
    FUNC_LOG;
    // Remove first event from queue
    MESMRFieldEvent* event = iEventQueue[ 0 ];
    iEventQueue.Remove( 0 );
    
    // Push the event into CleanupStack  for NotifyEventL
    CleanupDeletePushL( event );

    // Notify event to observers synchronously.
    NotifyEventL( *event );
    CleanupStack::PopAndDestroy( event );
    }
    
// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::HasEvents
// Checks if event queue has events.
// ---------------------------------------------------------------------------
//
TBool CESMRFieldEventQueue::HasEvents() const
    {
    FUNC_LOG;
    return ( iEventQueue.Count() > 0 ); 
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventQueue::Start
// Starts event queue
// ---------------------------------------------------------------------------
//
void CESMRFieldEventQueue::Start()
    {
    FUNC_LOG;
    if ( !iIdle->IsActive() && iEventQueue.Count() > 0 )
        {
        iIdle->Start( TCallBack( NotifyCallBack, this ) );
        }
    }

