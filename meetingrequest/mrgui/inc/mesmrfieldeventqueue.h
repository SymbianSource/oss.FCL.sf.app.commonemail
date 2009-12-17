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
* Description:  Field event queue interface
*
*/


#ifndef MESMRFIELDEVENTQUEUE_H
#define MESMRFIELDEVENTQUEUE_H

class MESMRFieldEvent;
class MESMRFieldEventObserver;

/**
 *  MESMRFieldEventQueue defines an iterface for field event queue.
 *  It provides event queue for CESMRField to send events to its
 *  event observers. The interface provides both synchronous and
 *  asynchrounous event notification methods.
 */
class MESMRFieldEventQueue
    {

public:
    
    /**
     * Adds observer to the event queue.
     *
     * @param aObserver the observer.
     */
    virtual void AddObserverL( MESMRFieldEventObserver* aObserver ) = 0;
    
    /**
     * Removes observer from the event queue.
     *
     * @param aObserver the observer.
     */
    virtual void RemoveObserver( MESMRFieldEventObserver* aObserver ) = 0;
    
    /**
     * Notifies event to the event observers in the queue.
     * Event is notified synchronously.
     * 
     * @param aEvent the event.
     */
    virtual void NotifyEventL( const MESMRFieldEvent& aEvent ) = 0;

    /**
     * Enques event to event queue and notifies it asynchronously to
     * the observers. Ownership of the event is transferred to the queue.
     * 
     * @param aEvent the event to enque to the event queue.
     */
    virtual void NotifyEventAsyncL( MESMRFieldEvent* aEvent ) = 0;
    
    /**
     * Removes event from the event queue.
     * 
     * @param aEvent the event to remove from the event queue.
     */
    virtual void CancelEvent( MESMRFieldEvent* aEvent ) = 0;
    
protected:

    virtual ~MESMRFieldEventQueue() {}

    };

#endif // MESMRFIELDEVENTQUEUE_H
