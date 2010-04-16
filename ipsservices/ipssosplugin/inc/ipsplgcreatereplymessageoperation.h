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
* Description:  New reply message creation operation
*
*/

// <qmail>
#ifndef IPSPLGCREATEREPLYMESSAGEOPERATION_H
#define IPSPLGCREATEREPLYMESSAGEOPERATION_H

#include "ipsplgcreatemessageoperation.h"

/**
* class 
*
* Email operation to create a new reply message asynchronously.
*/
NONSHARABLE_CLASS ( CIpsPlgCreateReplyMessageOperation ) : public CIpsPlgCreateMessageOperation
    {
public:
    
    /**
    * Constructor.
    */
    static CIpsPlgCreateReplyMessageOperation* NewL(
        CIpsPlgSmtpService* aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TMsvId aMailBoxId,
        TMsvId aOriginalMessageId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
        
    /**
    * Destructor.
    */
    virtual ~CIpsPlgCreateReplyMessageOperation();
    
private:

    /**
    * Constructor.
    */
    CIpsPlgCreateReplyMessageOperation(
        CIpsPlgSmtpService* aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TMsvId aMailBoxId,
        TMsvId aOriginalMessageId, 
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );

    /**
    * From CActive
    */
    void RunL();
    
    void StartMessageCreationL();

private:
    TMsvId iOriginalMessageId;
    };


#endif//IPSPLGCREATEREPLYMESSAGEOPERATION_H

// </qmail>
