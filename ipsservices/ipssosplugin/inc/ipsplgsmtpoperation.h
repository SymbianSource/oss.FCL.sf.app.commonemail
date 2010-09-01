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
* Description: This file defines class CIpsPlgSmtpOperation.
*
*/


#ifndef IPSPLGSENDOPERATION_H
#define IPSPLGSENDOPERATION_H

#include <smtcmtm.h>
#include "ipsplgcommon.h"
#include "ipsplgonlineoperation.h" // for MIpsPlgConnectOpCallback

class CClientMtmRegistry;

/**
 *  Class for smtp related operations
 *
 *  This class encapsulates SMTP send new message and send pending messages
 *  operations.
 *
 *  This class requires CIpsPlgEventHandler for QueryUsrPassL method
 *  for handling login problems. Because this class is exported and
 *  CIpsPlgEventHandler isn`t, to avoid problems, iEventHandler is passed
 *  as TAny* and can be set only from ipssosplugin.
 *
 *  @lib ipssosplugin.lib
 *  @since FS 1.0
 */
//should this class inherited from online operation
NONSHARABLE_CLASS( CIpsPlgSmtpOperation ) :
    public CMsvOperation,
    public MIpsPlgConnectOpCallback
    {
public: //from MIpsPlgConnectOpCallback

    void CredientialsSetL( TInt aEvent );

public:

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    IMPORT_C static CIpsPlgSmtpOperation* NewL( 
        CMsvSession& aMsvSession, 
        TInt aPriority, 
        TRequestStatus& aObserverRequestStatus,
        TBool aUsePublishSubscribe );

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    IMPORT_C static CIpsPlgSmtpOperation* NewLC(
        CMsvSession& aMsvSession, 
        TInt aPriority, 
        TRequestStatus& aObserverRequestStatus,
        TBool aUsePublishSubscribe );

    /**
     * Class destructor
     *
     * @since FS 1.0
     * @return None
     */ 
    virtual ~CIpsPlgSmtpOperation( );
    
    /**
     * From CMsvOperation
     */ 
    virtual const TDesC8& ProgressL();
    
    /**
     * Stard sending operation
     *
     * @param aSelection message selection for send messages
     * @since FS 1.0
     * @return None
     */     
    IMPORT_C void StartSendL( TMsvId aMessageId );
    
    IMPORT_C TInt EmptyOutboxFromPendingMessagesL( TMsvId aMailboxId );

    IMPORT_C CMsvEntrySelection* GetOutboxChildrensL( );

	/**
	 *  Sets CIpsPlgEventHandler
	 */
	void SetEventHandler( TAny* aEventHandler );
    
protected:

    /**
     * Class constructor
     *
     * @since FS 1.0
     * @return None
     */
    CIpsPlgSmtpOperation( 
        CMsvSession& aMsvSession, 
        TInt aPriority, 
        TRequestStatus& aObserverRequestStatus );
    
    /**
     * Constructor for leaving methods
     *
     * @since FS 1.0
     * @return None
     */    
    void ConstructL( );

    /**
     * Completes observer with status aStatus
     * @param aStatus: Status of the operation.
     */
    void CompleteObserver( TInt aStatus = KErrNone );

    /**
     * Completes itself with KErrNone status
     */
    void CompleteThis();

private: // From CActive

    enum TIpsSendState
        {
        EIdle,
        EMovingOutbox,          // moving mail to OutBox folder
        ESending,               // sending mail
        EQueryingDetails,       // querying for password
        EQueryingDetailsBusy,   // another operation is querying for details
        };

    /**
     * From CActive
     *
     * @since FS 1.0
     */
    void RunL( );
    void DoCancel( );
    
    /**
     *
     * @since FS 1.0
     */
    void CallSendL( );
    
    /**
     *
     * @since FS 1.0
     */
    void ConstructSmtpMtmL();
    
    /**
     * Validates all recipient addresses (to, cc, bcc)
     * Either returns silently if everything id ok, or leaves with KErrBadName
     * @param aEntry refers to message to be validated
     */
    void ValidateRecipientsL( CMsvEntry& aEntry );
    
    /**
     * Validates an array of address
     * Either returns silently if everything id ok, or leaves with KErrBadName
     * @param aRecipients array of addresses
     */
    void ValidateAddressArrayL( const CDesCArray& aRecipients );

    /**
     * Send user password query request to CIpsPlgEventHandler
     * @return ETrue - if query send
     */
    TBool QueryUserPassL();

private:

    CSmtpClientMtm*     iSmtpMtm;
    CMsvOperation*      iOperation;
    CMsvEntrySelection* iSelection;
    CClientMtmRegistry* iMtmRegistry;
    TInt                iState;
    TMsvId              iSmtpService;
    // not owned
    TAny*               iEventHandler; // pointer to CIpsPlgEventHandler
    };

#endif /* IPSPLGSENDOPERATION_H */
