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
* Description:  New forward message creation operation
*
*/

// <qmail>
#ifndef IPSPLGCREATEFORWARDMESSAGEOPERATION_H
#define IPSPLGCREATEFORWARDMESSAGEOPERATION_H

#include "ipsplgcreatemessageoperation.h"

/**
* class 
*
* Email operation to create a new forward message asynchronously.
*/
NONSHARABLE_CLASS ( CIpsPlgCreateForwardMessageOperation ) : public CIpsPlgCreateMessageOperation
    {
public:
    
    /**
    * Constructor.
    */
    static CIpsPlgCreateForwardMessageOperation* NewL(
        CIpsPlgSmtpService* aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TFSMailMsgId aMailBoxId,
        TMsvId aOriginalMessageId,
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );
        
    /**
    * Destructor.
    */
    virtual ~CIpsPlgCreateForwardMessageOperation();
    
private:

    /**
    * Constructor.
    */
    CIpsPlgCreateForwardMessageOperation(
        CIpsPlgSmtpService* aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TFSMailMsgId aMailBoxId,
        TMsvId aOriginalMessageId, 
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );

    /**
    * From CActive
    */
    void RunL();
    
    void StartMessageCreationL();

private:
    TMsvId iOriginalMessageId;
    };


#endif//IPSPLGCREATEFORWARDMESSAGEOPERATION_H

// </qmail>
