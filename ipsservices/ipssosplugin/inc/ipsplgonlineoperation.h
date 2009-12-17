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
* Description:  Base online operation class
*
*/

#ifndef IPSPLGONLINEOPERATION_H
#define IPSPLGONLINEOPERATION_H


#include "ipsplgbaseoperation.h"

class CMsvEntrySelection;
class TFSMailMsgId;
class MFSMailRequestObserver;
class CIpsPlgTimerOperation;
class CClientMtmRegistry;


class MIpsPlgConnectOpCallback
    {
public:
    virtual void CredientialsSetL( TInt aEvent )=0;
    };

/**
* class CIpsPlgOnlineOperation
*
* Common base class for email online operations.
*/
class CIpsPlgOnlineOperation : public CIpsPlgBaseOperation
    {
    public:
           
        /**
        *
        */
        virtual ~CIpsPlgOnlineOperation();

        /**
        *
        */
        virtual const TDesC8& ProgressL() = 0;

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
        virtual TInt IpsOpType() const;
        
		
    protected:
        /**
        * C++ constructor
        */
        // Construction.
        CIpsPlgOnlineOperation(
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            TBool aSignallingAllowed=ETrue );

        /**
        * Base constructor
        */
        void BaseConstructL(TUid aMtmType);

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        virtual void DoRunL() = 0;

        /**
        * From CActive
        */
        virtual TInt RunError( TInt aError );

        /**
        * Completes observer with status aStatus
        * @param aStatus: Status of the operation.
        * Override if needed to complete with other status than KErrNone.
        */
        void CompleteObserver( TInt aStatus = KErrNone );

        /**
        *
        */
        void CompleteThis();


        // REVIEW: is there any good reason for next two methods
        /**
        *
        */
        void InvokeClientMtmAsyncFunctionL(
            TInt aFunctionId,
            TMsvId aEntryId,
            TMsvId aContextId,
            const TDesC8& aParams=KNullDesC8);

        /**
        *
        */
        void InvokeClientMtmAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSel,
            TMsvId aContextId,
            const TDesC8& aParams=KNullDesC8);
            
        /**
        *
        */            
        //virtual TInt GetEngineProgress( const TDesC8& aProgress );

protected:

    /**
    *
    */
    void SignalFSObserver( TInt aStatus );
        
protected:
				// REVIEW: should tell whether the attributes are owned or not
				// REVIEW: CMsvOperation already has similar attribute, change the name
        CIpsPlgTimerOperation*  iActivityTimer;
        CBaseMtm*       iBaseMtm;
        CClientMtmRegistry* iMtmReg;
        // The sub-operation.
        CMsvOperation*  iOperation;
        // Status of current operation
        TInt            iError;
        // Return this if iOperation==NULL.
        TBuf8<1>        iDummyProg;
        
        
        //we need to be able to prevent completion signal
        //to framework, because online operations
        //may be nested ( fetch uses connect operation )
        //and we don't want sub operation to signal completion.
        TBool           iSignallingAllowed;
        //not owned
        MFSMailRequestObserver& iFSOperationObserver;
        
        
    };


#endif//IPSPLGONLINEOPERATION_H
