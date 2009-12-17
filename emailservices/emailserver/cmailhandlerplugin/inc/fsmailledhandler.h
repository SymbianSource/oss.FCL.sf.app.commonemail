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
* Description: This file defines class CFSMailLedHandler.
*
*/


#ifndef __FSMAIL_LED_HANDLER_H__
#define __FSMAIL_LED_HANDLER_H__

#include "fsnotificationhandlerbase.h"
#include "fsnotificationhandlertimerobserver.h"

class CFSNotificationHandlerTimer;


// Define startup Max retry counter for trying to get LED property
// value before assuming that the LED is not supported
const TInt KMaxLedRetry = 5;


/**
 *  Class to handle email led turning on.
 *  A handler for observing inputs into email framework, turning on the 
 *  Email LED if a message addition is performed on a Inbox folder.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSMailLedHandler : public CFSNotificationHandlerBase,
                          public MFSNotificationHandlerTimerObserver
    {
public:

    enum TState
        {
        EStarting,
        ESupportLED,
        ENotSupportLED
        };

    /**
     * Two-phased constructor.
     * @param aOwner Owner and manager of this handler.
     */	
    static CFSMailLedHandler* NewL( MFSNotificationHandlerMgr& aOwner );
    /**
    * Destructor.
    */
    virtual ~CFSMailLedHandler();
    
// From base class MFSNotificationHandlerTimerObserver
    virtual void TimerCallBackL( TInt aError );

// from base class CFSNotificationHandlerBase
    virtual void HandleEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 );
    
private:

    /**
     * Constructor.
     *
     * @since S60 ?S60_version
     * @param aOwner Owner and manager of this handler.
     */ 
    CFSMailLedHandler( MFSNotificationHandlerMgr& aOwner );
    void ConstructL();


// from base class CFSNotificationHandlerBase         
    virtual void TurnNotificationOn();
    virtual void TurnNotificationOff();

private: // data

	TState iState;
	TInt iRetryCount;

    /**
     * Timer service.
     * Own.
     */        
    CFSNotificationHandlerTimer* iTimer;
	
    };


#endif  //__FSMAIL_LED_HANDLER_H__
