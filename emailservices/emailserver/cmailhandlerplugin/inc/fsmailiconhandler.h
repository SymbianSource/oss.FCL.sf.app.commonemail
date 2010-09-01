/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of mail icon handler
*
*/


#ifndef __FSMAIL_ICON_HANDLER_H__
#define __FSMAIL_ICON_HANDLER_H__

#include <coreapplicationuisdomainpskeys.h>

#include "fsnotificationhandlerbase.h"
#include "fsnotificationhandlertimerobserver.h"

class CFSNotificationHandlerTimer;


/**
 *  Class to handle email icon.
 *  A handler for observing inputs into email framework, setting icon on
 *  if a message addition is performed on a Inbox folder.
 */ 
class CFSMailIconHandler : public CFSNotificationHandlerBase,
                           public MFSNotificationHandlerTimerObserver
{
public:

    enum TState
        {
        EStarting,
        EStarted,
        EFailed
        };
        
    /**
     * Two-phased constructor.
     * @param aOwner Owner and manager of this handler.
     */
    static CFSMailIconHandler* NewL( MFSNotificationHandlerMgr& aOwner );
    
    /**
     * Destructor.
     */
    virtual ~CFSMailIconHandler();
    
    
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
     * @param aOwner Owner and manager of this handler.
     */ 
    CFSMailIconHandler( MFSNotificationHandlerMgr& aOwner );
    void ConstructL();
    
    TInt TestIcon();
    
    TInt SetProperty( TUid aCategory, TUint aKey, TInt aValue );

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


#endif  //__FSMAIL_ICON_HANDLER_H__
