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
* Description:  Disconnection operation class
*
*/

#ifndef IPSPLGDISCONNECTOP_H
#define IPSPLGDISCONNECTOP_H


#include "ipsplgonlineoperation.h"


/**
* Disconnect operation.
* 
*/
class CIpsPlgDisconnectOp :
    public CIpsPlgOnlineOperation
    {
    public:

        /**
        *
        */
        static CIpsPlgDisconnectOp* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            TBool aDoRemoveAfterDisconnect = EFalse );

        /**
        *
        */
        virtual ~CIpsPlgDisconnectOp();

        /**
        *
        */
        const TDesC8& ProgressL();

        /**
        *
        */
        const TDesC8& GetErrorProgressL( TInt aError );
        
        /**
        * 
        */
       TFSProgress GetFSProgressL() const;

        /**
        *
        */
        TBool Connected() const;

    protected:


        /**
        * From CActive
        */
        void DoRunL();
        
    private:

        /**
        *
        */
        CIpsPlgDisconnectOp(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aServiceId,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            TBool aDoRemoveAfterDisconnect );

        /**
        *
        */
        void ConstructL();

        /**
        *
        */
        void DoDisconnectL();

        
    private: // Data
        
        TPckgBuf<TPop3Progress> iPopProgress;
        TPckgBuf<TImap4CompoundProgress> iImapProgress;
        TMsvEntry               iTEntry;
        TBool                   iDisconnected;
        TBool                   iDoRemoveAfterDisconnect;
        
    };

#endif //IPSPLGDISCONNECTOP_H
