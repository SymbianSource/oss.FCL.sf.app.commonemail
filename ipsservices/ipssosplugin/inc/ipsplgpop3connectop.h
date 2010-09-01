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
* Description:  POP connect operation
*
*/

#ifndef IPSPLGPOP3CONNECTOP_H
#define IPSPLGPOP3CONNECTOP_H


#include "ipsplgonlineoperation.h"
class CIpsPlgEventHandler;

/**
* Connect operation.
* Encapsulates connection validation.
*/
class CIpsPlgPop3ConnectOp :
    public CIpsPlgOnlineOperation,
    public MIpsPlgConnectOpCallback
    {
public://from MIpsPlgConnectOpCallback
    
    void CredientialsSetL( TInt aEvent );
    public:

        /**
        *
        */
        static CIpsPlgPop3ConnectOp* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aService,
            TBool aForcePopulate,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler=NULL,
            TBool aSignallingAllowed=ETrue );

        /**
        *
        */
        virtual ~CIpsPlgPop3ConnectOp();

        /**
        *
        */
        const TDesC8& GetErrorProgressL(TInt aError);
        
        /**
        *
        */
        TFSProgress GetFSProgressL() const;

        /**
        *
        */
        TBool Connected() const;
        
        /**
        *
        */
        virtual TInt IpsOpType() const;

    protected:

        /**
        * 
        */
        const TDesC8& ProgressL();
        
        /**
         * 
         */
        void DoCancel();

        /**
        * From CActive
        */
        void DoRunL();
        
        /**
        * 
        */
        TInt GetOperationErrorCodeL( );
        
    private:

        /**
        *
        */
        CIpsPlgPop3ConnectOp(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aServiceId,
            TBool aForcePopulate,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler,
            TBool aSignallingAllowed );

        /**
        *
        */
        void ConstructL();

        /**
        *
        */
        void DoConnectL();

        /**
        *
        *
        */
        void DoPopulateL();
        /**
        *
        */
        TBool ValidateL();

        /**
         * Send user password query request to CIpsPlgEventHandler
         * @return ETrue - if query send
         */
        TBool QueryUserPassL();
        
         
        /**
        *
        */
        inline void SetFlag(TUint32 aFlag);

        /**
        *
        */
        inline void UnsetFlag(TUint32 aFlag);

        /**
        *
        */
        inline TBool FlagIsSet(TUint32 aFlag) const;

    private: // Data
    
        enum TPopConnectStates 
            {
            EStartConnect,
            EConnected,
            EPopulate,
            EQueryingDetails,
            EQueryingDetailsBusy,
            EIdle
            };

        TInt                                            iState;
        CMsvEntry*                                      iEntry;        
        TPckgBuf<TPop3Progress>                         iProgress;
        TInt                                            iPopulateLimit;
        TBool                                           iForcePopulate;
        CMsvEntrySelection*                             iSelection;
        // not owned
        CIpsPlgEventHandler*                            iEventHandler;
        // set to true if connection is already exists
        TBool iAlreadyConnected;
    };

#endif
