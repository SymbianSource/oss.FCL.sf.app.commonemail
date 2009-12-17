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
* Description: This file defines class CFSNotificationHandlerNotifierInitiator.
*
*/


#ifndef __FSNOTIFICATIONHANDLERNOTIFIERINITIATOR_H__
#define __FSNOTIFICATIONHANDLERNOTIFIERINITIATOR_H__

#include <e32base.h>
#include <e32std.h>

#include <barsc.h>
//<cmail>
#include "CFSMailCommon.h"

#include "fsmailserverconst.h"
//</cmail>

class MFSNotificationHandlerNotifierInitiatorObserver;
class MFSNotificationHandlerMgr;
class MFSMailExceptionEventCallback;
class CAknQueryDialog;

/**
 *  Active object that handles asynchronous usage of
 *  FreestyleEmailNotifier.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSNotificationHandlerNotifierInitiator : public CActive
    {
public:

    static CFSNotificationHandlerNotifierInitiator* NewL(
        MFSNotificationHandlerNotifierInitiatorObserver& aOwner,
        MFSNotificationHandlerMgr& aNotificationHandlerMgr );
    static CFSNotificationHandlerNotifierInitiator* NewLC(
        MFSNotificationHandlerNotifierInitiatorObserver& aOwner,
        MFSNotificationHandlerMgr& aNotificationHandlerMgr);

    /**
     * Destructor.
     */
    virtual ~CFSNotificationHandlerNotifierInitiator();


    /**
     * Sets mailbox id that this notification is related to.
     *
     * @since S60 ?S60_version
     * @param aNewMailBoxId Mailbox id that the message is related to.
     */ 
    void SetMailBoxId( TFSMailMsgId aNewMailBoxId );

    /**
     * Gets mailbox id that this notification is related to.
     *
     * @since S60 ?S60_version
     * @return Mailbox id that the message is related to. NULL if not set.
     */ 
    TFSMailMsgId MailBoxId() const;

    /**
     * Sets message type that this notification is related to.
     * This function is valid only in case of message query.
     *
     * @since S60 ?S60_version
     * @param aMessageType Message type that the message is related to.
     */ 
    void SetMessageType( TFsEmailNotifierSystemMessageType aMessageType );

    /**
     * Gets message type that this notification is related to.
     * This function is valid only in case of message query.
     *
     * @since S60 ?S60_version
     * @return Message type that the message is related to. Negative if not set.
     */ 
    TFsEmailNotifierSystemMessageType MessageType() const;

    /**
     * Callback for the response.
     *
     * @since S60 ?S60_version
     * @param aCallback Callback for the response.
     */ 
    void SetCallback( MFSMailExceptionEventCallback* aCallback );

    /**
     * Callback for the response.
     *
     * @since S60 ?S60_version
     * @return Callback for the response. NULL if not set.
     */ 
    MFSMailExceptionEventCallback* Callback() const;

    
    /**
     * Shows a custom error note of type EFsEmailNotifErrCustom.
     * No output is available through this service.
     * KUidFsEmailNotifierMessageQuery plugin is used to show
     * this notification.
     *
     * @since S60 ?S60_version
     * @param aMailboxName Mailbox name that the message concerns.
     * @param aMessageType Type of the message to show.
     */ 
    void RequestMessageQueryL(
        TDesC& aMailboxName,
        TFsEmailNotifierSystemMessageType aMessageType,
        const TDesC& aCustomMessageText = KNullDesC );

    /**
     * Shows an authentication query.
     * KUidFsEmailNotifierAuthentication plugin is used to show
     * the query.
     *
     * @since S60 ?S60_version
     * @param aMailboxName Mailbox name that the message concerns.
     */ 
    void RequestAuthenticationL( TDesC& aMailboxName );

    /**
     * Function to read user input given in last authentication
     * request.
     *
     * @since S60 ?S60_version
     * @param aPassword User input for password.
     */ 
    void GetLastAuthenticationOutput(
        TBuf<KFsEmailNotifierPasswordMaxLength>& aPassword ) const;

    /**
     * Function to read user response in last query.
     *
     * @since S60 ?S60_version
     * @return ETrue if user selected OK, Continue, etc. (LSK)
     *         EFalse if user selected Cancel (RSK)
     */ 
    TBool GetLastResponse() const;

protected:
  
    /**
     * Constructor.
     *
     * @since S60 ?S60_version
     * @param aOwner Owner and manager of this initiator.
     */ 
    CFSNotificationHandlerNotifierInitiator(
        MFSNotificationHandlerNotifierInitiatorObserver& aOwner,
        MFSNotificationHandlerMgr& aNotificationHandlerMgr );
    
    void ConstructL();

// from base class CActive
    virtual void DoCancel();
    virtual void RunL();

private:

    void BeforeDialog();
    void AfterDialog();

        	
private: // data
    
    /**
     * Input/output buffer for authentication.
     */
    TBuf<KFsEmailNotifierPasswordMaxLength> iPassword;

    /**
     * ETrue if user pressed LSK (OK, Login, etc.), EFalse if RSK (Cancel).
     */
    TBool iLogin;

    /**
     * Owner and user of this class.
     */
    MFSNotificationHandlerNotifierInitiatorObserver& iOwner;

    /**
     * Owner and manager of the instance of this handler class.
     */
    MFSNotificationHandlerMgr& iNotificationHandlerMgr;

    /**
     * Handle to a session with the extended notifier server.
     */
    RNotifier iNotifier;
    
    /**
     * Id of the mailbox that this notifier is related to. NULL if not set.
     */
    TFSMailMsgId iMailBoxId;
    
    /**
     * Message type that this notifier is related to. Negative if not set.
     */
    TFsEmailNotifierSystemMessageType iMessageType;

    /**
     * Callback function for the respoonse. NULL if not set.
     */
    MFSMailExceptionEventCallback* iCallback;
    
    //CAknQueryDialog* iDialog;
    
    };


#endif  //__FSNOTIFICATIONHANDLERNOTIFIERINITIATOR_H__
