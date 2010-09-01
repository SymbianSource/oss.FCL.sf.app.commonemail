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

#include "cfsmailcommon.h" // for TFSMailMsgId, TFSProgress

class CMsvOperation;

/**
* class CIpsPlgBaseOperation
*
* Common base class for email online operations.
*/
class CIpsPlgBaseOperation : public CMsvOperation
    {
public:
    /**
    *
    */
    virtual ~CIpsPlgBaseOperation();

    /**
    *
    */
    virtual const TDesC8& ProgressL()=0;

    /**
    * For reporting if DoRunL leaves
    */
    virtual const TDesC8& GetErrorProgressL(TInt aError) = 0;

    /**
    * 
    */
    virtual TFSProgress GetFSProgressL() const = 0;

    /**
    *
    */
	TInt FSRequestId() const;
	
	/**
	*
	*/
	TFSMailMsgId FSMailboxId() const;
    
    /**
    *
    */
    virtual TInt IpsOpType() const;
	
protected:

    /**
    * C++ constructor
    */
    // Construction.
    CIpsPlgBaseOperation(
        CMsvSession& aMsvSession,
        TInt aPriority,
        TRequestStatus& aObserverRequestStatus,
        TInt aFSRequestId,
        TFSMailMsgId aFSMailboxId );


    /**
    * From CActive
    */
    virtual void DoCancel()=0;

    /**
    * From CActive
    */
    virtual void RunL()=0;

    /**
    * From CActive
    */
    virtual TInt RunError( TInt aError )=0;

        
protected:
        
    TInt            iFSRequestId;
    TFSMailMsgId    iFSMailboxId;
        

    };
#endif//IPSPLGBASEOPERATION_H
