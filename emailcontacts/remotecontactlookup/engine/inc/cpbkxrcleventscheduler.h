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
* Description:  Definition of the class CPbkxRclEventScheduler.
*
*/


#ifndef CPBKXRCLEVENTSCHEDULER_H
#define CPBKXRCLEVENTSCHEDULER_H

#include <e32base.h>

/**
* Class that is notified when event is triggered.
*
* @lib pbkxrclengine.lib
* @since 3.1
*/
class MPbkxRclEventHandler
    {
public:
    
    /**
    * This method is called when event is triggered.
    */
    virtual void EventTriggered() = 0;
    
    };

/**
* Class that generates events with a little delay.
*
* @lib pbkxrclengine.lib
* @since 3.1
*/
class CPbkxRclEventScheduler : public CActive
    {
    
public: // constructor and destructor

    /**
    * Two-phased constructor.
    *
    * @param aHandler Interface which is notified when event is triggered.
    * @return Created object.
    */
    static CPbkxRclEventScheduler* NewL( MPbkxRclEventHandler& aHandler );

    /**
    * Destructor.
    */
    virtual ~CPbkxRclEventScheduler();

public: // new methods

    /**
    * Triggers event with a little delay.
    */
    void TriggerEvent();

protected: // from CActive

    /**
    * Handles asynchronous request.
    */
    virtual void RunL();

    /**
    * Cancels asynchronous request.
    */
    virtual void DoCancel();

private: // constructors

    /**
    * Constructor.
    *
    * @param aHandler Interface which is notified when event is triggered.
    */
    CPbkxRclEventScheduler( MPbkxRclEventHandler& aHandler );

    /**
    * Second-phase constructor.
    */
    void ConstructL();
    
private: // data

    // Handles triggered event.
    MPbkxRclEventHandler& iEventHandler;

    // Timer for creating a little delays in triggering events.
    RTimer iTimer;

    };

#endif
