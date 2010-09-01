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
* Description:  Operation waiter for running asyncronous operations
*                synchronously
*
*/

#ifndef IPSOPERATIONWAIT_H
#define IPSOPERATIONWAIT_H


#include <e32base.h>

NONSHARABLE_CLASS( CIpsPlgOperationWait ) : public CActive
    {
    public:
    
        /**
        *
        */    
        static CIpsPlgOperationWait* NewL( TInt aPriority = EPriorityStandard );
        
        /**
        *
        */    
        static CIpsPlgOperationWait* NewLC( TInt aPriority = EPriorityStandard );
    
        /**
        *
        */
        CIpsPlgOperationWait( TInt aPriority );

        /**
        *
        */
        virtual ~CIpsPlgOperationWait();

        /**
        *
        */
        void ConstructL();

        /**
        *
        */
        void Start();


    protected:

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

    private:
        
        /**
        *
        */    
        void StopScheduler();

    private: // Data

        CActiveSchedulerWait iWait;

    };



#endif /* IPSOPERATIONWAIT_H */
