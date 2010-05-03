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
* Description:  POP3 fetch message(s) operation
*
*/

#ifndef IPSPLGPOP3FETCHOPERATION_H
#define IPSPLGPOP3FETCHOPERATION_H

#include "ipsplgonlineoperation.h"

//class CMsvEmailConnectionProgressProvider;

/**
* CIpsPlgPop3FetchOperation
* Fetch message(s) operation, using client MTM Get Mail API.
*/
NONSHARABLE_CLASS ( CIpsPlgPop3FetchOperation ) :
    public CIpsPlgOnlineOperation
    {
    public:

        /**
        *
        */
        static CIpsPlgPop3FetchOperation* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aFunctionId,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            const TImPop3GetMailInfo& aGetMailInfo,
            const CMsvEntrySelection& aSel,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver* aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler );

       
        /**
        *
        */
        virtual ~CIpsPlgPop3FetchOperation();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL( TInt aError );
        
        /**
        *
        */
        virtual TFSProgress GetFSProgressL() const;
        
        // <qmail> new func to this op
        /**
         * Returns operation type
         */
        TIpsOpType IpsOpType() const;
        
    protected:

        /**
        *
        */
        CIpsPlgPop3FetchOperation(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aFunctionId,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            const TImPop3GetMailInfo& aGetMailInfo,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver* aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler);

            
        /**
        *
        */
        void ConstructL( const CMsvEntrySelection& aSel );

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void DoRunL();

        /**
        *
        */
        void DoConnectL();

        /**
        *
        */
        void DoFetchL();

        /**
        *
        */
        void DoDisconnectL();

    protected:

        enum TFetchState {
            EStateIdle,
            EStateConnecting,
            EStateFetching,
            EStateDisconnecting };
        TFetchState iState;

    protected:
        
        TDesC8* iFetchErrorProgress;
        TInt iFunctionId;
        TImPop3GetMailInfo iGetMailInfo;
        TPckgBuf<TPop3Progress> iProgressBuf;
        CMsvEntrySelection* iSelection;
        CIpsPlgEventHandler* iEventHandler;
        
    };


#endif //IPSPLGPOP3FETCHOPERATION_H

// End of File
