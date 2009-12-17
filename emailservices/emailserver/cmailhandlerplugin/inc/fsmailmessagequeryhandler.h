/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class CFSMailMessageQueryHandler.
*
*/


#ifndef __FSMAIL_MESSAGEQUERY_HANDLER_H__
#define __FSMAIL_MESSAGEQUERY_HANDLER_H__

#include "fsnotificationhandlerbase.h"
#include "fsnotificationhandlernotifierinitiator.h"
#include "fsnotificationhandlernotifierinitiatorobserver.h"


/**
 *  Class to handle showing of authentication query in case
 *  needed.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSMailMessageQueryHandler :
    public CFSNotificationHandlerBase,
    public MFSNotificationHandlerNotifierInitiatorObserver
    {
public:

    /**
     * Two-phased constructor.
     * @param aOwner Owner and manager of this handler.
     */	
    static CFSMailMessageQueryHandler* NewL(
        MFSNotificationHandlerMgr& aOwner );
    /**
    * Destructor.
    */
    virtual ~CFSMailMessageQueryHandler();

public: // From base class MFSNotificationHandlerNotifieriInitiatorObserver

    void NotifierInitiatorDoneL(
        CFSNotificationHandlerNotifierInitiator* aInitiator );

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
    CFSMailMessageQueryHandler( MFSNotificationHandlerMgr& aOwner );
    void ConstructL();

    /**
     * Shows an message query dialog.
     *
     * @since S60 ?S60_version
     * @param aMailboxName The name of the mailbox.
     */ 
    void ShowMessageQueryL( TFSMailMsgId aMailbox,
    						TFsEmailNotifierSystemMessageType aMessageType,
    						const TDesC& aCustomMessageText,
    				        MFSMailExceptionEventCallback* aCallback  );


    /**
     * Searches for a initiator among initiators that
     * has the same pointer as given parameter.
     *
     * @since S60 ?S60_version
     * @param aInitiator Pointer to searched initiator.
     * @return Index for found initiator in initiators list.
     *         KErrNotFound if not found.
     */ 
    TInt FindInitiator(
        CFSNotificationHandlerNotifierInitiator* aInitiator ) const;

    /**
     * Searches for a initiator among initiators that
     * is related to the same mailbox as given as a parameter.
     *
     * @since S60 ?S60_version
     * @param aMailBoxId Mailbox id that the searched initiator
     *                   is related to.
     * @return Index for found initiator in initiators list.
     *         KErrNotFound if not found.
     */ 
    TInt FindInitiator( TFSMailMsgId aMailBoxId,
                        TFsEmailNotifierSystemMessageType aMessageType ) const;


// from base class CFSNotificationHandlerBase         
    virtual void TurnNotificationOn();
    virtual void TurnNotificationOff();

private: // data

    /**
     * Active objects handling asynchronous calls to FreestyleEmailNotifier.
     * Own.
     */
	RPointerArray<CFSNotificationHandlerNotifierInitiator>
	    iNotifierInitiators;
	
    };


#endif  //__FSMAIL_MESSAGEQUERY_HANDLER_H__
