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
NONSHARABLE_CLASS ( CIpsPlgPop3ConnectOp ) :
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

// <qmail> Connected() used from baseclass
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

        // <qmail>
        /**
         * Do.. functions handle certain state of this operation
         */
        void DoConnectL();
        void DoPopulateL();
// <qmail> TBool ValidateL() removed (did nothing)
// <qmail> void DoQueryPasswordL() not used any more
        void DoDisconnect();
        // </qmail>
        
        // <qmail> removed flag methods as they were not used or even defined anywhere
        
    private: // Data
    
        enum TPopConnectStates 
            {
            EStartConnect,
            EConnected,
            EPopulate,
            // <qmail> new EDisconnecting state, removed querydetails state
            EDisconnecting,
            // </qmail>
            EErrInvalidDetails,
            EIdle
            };

        TInt                                            iState;
        CMsvEntry*                                      iEntry;        
        TPckgBuf<TPop3Progress>                         iProgress;
        TInt                                            iPopulateLimit;
        TBool                                           iForcePopulate;
        CMsvEntrySelection*                             iSelection;
        CIpsPlgEventHandler*                            iEventHandler; // not owned
    };

#endif
