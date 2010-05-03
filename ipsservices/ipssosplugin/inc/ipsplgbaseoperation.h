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
* Description:  Base operation class
*
*/

#ifndef IPSPLGBASEOPERATION_H
#define IPSPLGBASEOPERATION_H

class CMsvOperation;

/**
* class CIpsPlgBaseOperation
* Common base class for email operations
*/
NONSHARABLE_CLASS ( CIpsPlgBaseOperation ) : public CMsvOperation
    {
public:
    virtual ~CIpsPlgBaseOperation();

    /**
    * For reporting if DoRunL leaves
    * All operations must implement this
    */
    virtual const TDesC8& GetErrorProgressL(TInt aError) = 0;

    /**
    * Returns pending asynchronous request status to caller
    * All operations must implement this
    */
    virtual TFSProgress GetFSProgressL() const = 0;

    /**
    * returns request id
    * (given by caller during instantiation)
    * @return request id
    */
	TInt FSRequestId() const;
	
	/**
	* returns mailbox id that this operation is related to
	* (given by caller during instantiation)
	*/
	TFSMailMsgId FSMailboxId() const;
    
    /**
    * All concrete derived classes must have a type identifier
    * @return operation type
    */
    virtual TIpsOpType IpsOpType() const = 0; // <qmail>
	
protected:

    // <qmail> priority parameter has been removed
    CIpsPlgBaseOperation(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TInt aFSRequestId,
        TFSMailMsgId aFSMailboxId );

protected:
    TInt            iFSRequestId;
    TFSMailMsgId    iFSMailboxId;
    };

#endif // IPSPLGBASEOPERATION_H
