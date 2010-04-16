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
* Description:  New message creation operation
*
*/

// <qmail>
#ifndef IPSPLGCREATEMESSAGEOPERATION_H
#define IPSPLGCREATEMESSAGEOPERATION_H


class TFSMailMsgId;
class MFSMailRequestObserver;

/**
* class 
*
* Email operation to create a new message asynchronously.
*/
NONSHARABLE_CLASS ( CIpsPlgCreateMessageOperation ) : public CMsvOperation
    {
public:
    
    /**
    * Constructor.
    */
    static CIpsPlgCreateMessageOperation* NewL(
        CIpsPlgSmtpService* aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aSmtpServiceId, 
        TMsvPartList aPartList,
        TMsvId aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
        
    /**
    * Destructor.
    */
    virtual ~CIpsPlgCreateMessageOperation();
    
    /**
    *
    */
    virtual const TDesC8& ProgressL();

protected:

    /**
    * Constructor.
    */
    CIpsPlgCreateMessageOperation(
        CIpsPlgSmtpService* aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aSmtpServiceId, 
        TMsvPartList aPartList,
        TMsvId aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
    
    /**
    * Constructor.
    */
    void ConstructL();
    
    void SignalFSObserver( TInt aStatus, CFSMailMessage* aMessage );
    
    TMsvId GetIdFromProgressL( const TDesC8& aProg );

    /**
    * From CActive
    */
    void DoCancel();
    
    /**
    * From CActive
    */
    void RunL();
    
    /**
    * From CActive
    */
    TInt RunError( TInt aError );

    void StartMessageCreation();

    virtual void StartMessageCreationL();

protected:
    CIpsPlgSmtpService* iSmtpService;   // not owned
    CMsvOperation* iOperation;   // owned
    TMsvId iSmtpServiceId; 
    TMsvPartList iPartList;
    TBuf8<1> iBlank;
    TMsvId iMailBoxId;
    MFSMailRequestObserver& iOperationObserver;  // not owned
    TInt iRequestId;
    };


#endif//IPSPLGCREATEMESSAGEOPERATION_H

// </qmail>
