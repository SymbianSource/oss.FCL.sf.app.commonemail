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
* Description: This file defines class CFSMailOutOfMemoryHandler.
*
*/


#ifndef __FSMAIL_OUTOFMEMORY_HANDLER_H__
#define __FSMAIL_OUTOFMEMORY_HANDLER_H__

#include "fsnotificationhandlerbase.h"
#include "fsnotificationhandlertimerobserver.h"
#include "fsnotificationhandlernotifierinitiator.h"
#include "fsnotificationhandlernotifierinitiatorobserver.h"

class CFSNotificationHandlerTimer;

/**
 *  Class to handle showing of out of memory in case the error
 *  occurs while synchronizing a mail box.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSMailOutOfMemoryHandler :
    public CFSNotificationHandlerBase,
    public MFSNotificationHandlerTimerObserver,
    public MFSNotificationHandlerNotifierInitiatorObserver
    {
public:

    /**
     * Two-phased constructor.
     * @param aOwner Owner and manager of this handler.
     */	
    static CFSMailOutOfMemoryHandler* NewL(
        MFSNotificationHandlerMgr& aOwner );
    /**
    * Destructor.
    */
    virtual ~CFSMailOutOfMemoryHandler();

    /**
     * Informs instance of this class that notification
     * showing has completed and that the initiator
     * can be removed.
     *
     * @since S60 ?S60_version
     * @param aInitiator Pointer to the initiator that has completed.
     */
    void NotifierInitiatorDoneL(
        CFSNotificationHandlerNotifierInitiator* aInitiator );

    // From base class MFSNotificationHandlerTimerObserver
    virtual void TimerCallBackL( TInt aError );

protected:


// from base class CFSNotificationHandlerBase  

    /**
     * From CFSNotificationHandlerBase.
     * Implementation of CapabilitiesToContinueL.
     *
     * @see CFSNotificationHandlerBase::CapabilitiesToContinueL 
     *
     * @since S60 ?S60_version
     */
    virtual TBool CapabilitiesToContinueL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 ) const;

    /**
     * Function that does the actual event handling.
     *
     * @see CFSNotificationHandlerBase::EventL
     *
     * @since S60 ?S60_version
     */ 
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
    CFSMailOutOfMemoryHandler( MFSNotificationHandlerMgr& aOwner );
    void ConstructL();

    /**
     * Shows an error note stating that out of memory error
     * occurred while synchronizing the mailbox whose name was
     * given as a parameter.
     *
     * @since S60 ?S60_version
     * @param aMailboxName The name of the mailbox.
     */ 
    void ShowOutOfMemoryNoteL( TFSMailMsgId aMailboxId );

// from base class CFSNotificationHandlerBase         
    virtual void TurnNotificationOn();
    virtual void TurnNotificationOff();

private: // data
	

    /**
     * Timer service.
     * Own.
     */        
    CFSNotificationHandlerTimer* iTimer;

	/**
     * Active object handling asynchronous calls to FreestyleEmailNotifier.
     * Own.
     */  
	CFSNotificationHandlerNotifierInitiator* iNotifierInitiator;

    };


#endif  //__FSMAIL_OUTOFMEMORY_HANDLER_H__
