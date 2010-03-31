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
* Description: This file defines class CESMRFieldEventQueue.
*
*/


#ifndef CESMRFIELDEVENTQUEUE_H
#define CESMRFIELDEVENTQUEUE_H

#include <e32base.h>

#include "mesmrfieldeventqueue.h"

/**
 *  CESMRFieldEventQueue defines an field event queue for ESMR GUI.
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRFieldEventQueue ) : public CBase,
                                            public MESMRFieldEventQueue
    {

public:

    static CESMRFieldEventQueue* NewL();
    static CESMRFieldEventQueue* NewLC();

    /**
     * Destructor.
     */
    virtual ~CESMRFieldEventQueue();

// from base class MESMRFieldEventQueue

    /**
     * From MESMRFieldEventQueue.
     * Adds observer to the event queue.
     *
     * @param aObserver the observer.
     */
    void AddObserverL( MESMRFieldEventObserver* aObserver );
    
    /**
     * From MESMRFieldEventQueue.
     * Removes observer from the event queue.
     *
     * @param aObserver the observer.
     */
    void RemoveObserver( MESMRFieldEventObserver* aObserver );
    
    /**
     * From MESMRFieldEventQueue.
     * Notifies event to the event observers in the queue.
     * Event is notified synchronously.
     * 
     * @param aEvent the event.
     */
    void NotifyEventL( const MESMRFieldEvent& aEvent );

    /**
     * From MESMRFieldEventQueue.
     * Enques event to event queue and notifies it asynchronously to
     * the observers. Ownership of the event is transferred to the queue.
     * 
     * @param aEvent the event to enque to the event queue.
     */
    void NotifyEventAsyncL( MESMRFieldEvent* aEvent );
    
    /**
     * From MESMRFieldEventQueue.
     * Removes event from the event queue.
     * 
     * @param aEvent the event to remove from the event queue.
     */
    void CancelEvent( MESMRFieldEvent* aEvent );

private:

    CESMRFieldEventQueue();

    void ConstructL();

    static TInt NotifyCallBack( TAny* aPtr );
    
    void DoNotifyEventAsyncL();
    
    TBool HasEvents() const;
    
    void Start();
    
private: // data

    /**
     * Observer array
     */
    RPointerArray< MESMRFieldEventObserver > iObservers;

    /**
     * Event queue
     */
    RPointerArray< MESMRFieldEvent > iEventQueue;
    
    /**
     * Current observer index
     */
    TInt iNotifyIndex;
    
    /**
     * Idle callback. Own.
     */
    CIdle* iIdle;
    };

#endif // CESMRFIELDEVENTQUEUE_H
