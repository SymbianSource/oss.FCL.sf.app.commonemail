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

class CClientMtmRegistry;

/**
 *  Class for smtp related operations
 *
 *  @lib ipssosplugin.lib
 *  @since FS 1.0
 */
//should this class inherited from online operation
NONSHARABLE_CLASS( CIpsPlgSmtpOperation ) : public CMsvOperation
    {
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
        EMovingOutbox,
        ESending
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
    
private:

    CSmtpClientMtm*     iSmtpMtm;
    CMsvOperation*      iOperation;
    CMsvEntrySelection* iSelection;
    CClientMtmRegistry* iMtmRegistry;
    TInt                iState;
    TMsvId              iSmtpService;
    };

#endif /* IPSPLGSENDOPERATION_H */
