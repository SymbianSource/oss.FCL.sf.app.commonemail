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
* Description:  IMAP4 populate messages operation
*
*/

#ifndef __IPSPLGIMAP4POPULATEOP_H__
#define __IPSPLGIMAP4POPULATEOP_H__

#include "ipsplgonlineoperation.h"

class CIpsPlgEventHandler;

/**
* class CIpsPlgImap4PopulateOp
*
*/

class CIpsPlgImap4PopulateOp :
    public CIpsPlgOnlineOperation
    {
    public:

        /**
        * NewL()
        * Basic factory function - creates dialog with standard title resource
        * @param CMsvSession& 
        *
        *
        *
        *
        *
        *
        * @return CIpsPlgImap4PopulateOperation* self pointer
        */

        IMPORT_C static CIpsPlgImap4PopulateOp* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aPriority,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            const TImImap4GetPartialMailInfo& aPartialMailInfo,
            const CMsvEntrySelection& aSel,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler,  
            TBool aDoFilterSelection = ETrue );

        /**
        * ~CIpsPlgImap4PopulateOp()
        * destructor
        */

        virtual ~CIpsPlgImap4PopulateOp();

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
        virtual TInt IpsOpType() const;
        
    private:


        /**
        * CIpsPlgImap4PopulateOp()
        * @param 
        *
        *
        *
        *
        *
        *
        
        */

        CIpsPlgImap4PopulateOp(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aPriority,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            const TImImap4GetPartialMailInfo& aGetMailInfo,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler );

        /**
        * 
        */

        void ConstructL( 
                const CMsvEntrySelection& aSel, 
                TBool aDoFilterSelection );


        /**
        * 
        */

        void RunL();

        /**
        * 
        */
        void DoRunL();
        
        /**
        * 
        */
        void DoCancel();

        /**
        * 
        */

        void Complete();
        
        /**
        *
        */
        void DoConnectL();
        
        /**
        *
        */
        void FilterSelectionL( const CMsvEntrySelection& aSelection );
        
        /**
        *
        */
        TBool IsPartialPopulate( );

    protected:
        
        /**
        *
        */
        void DoPopulateL();
        
        /**
        * From CIpsPlgOnlineoperation
        */
        TInt GetEngineProgress( const TDesC8& aProgress );
        
    protected:

        enum TFetchState 
            {
            EStateIdle,
            EStateConnecting,
            EStateFetchAndCheckConnectionChange
            };

   
    private: //Data
        
        TImImap4GetPartialMailInfo        iPartialMailInfo;
        RArray<TMsvId>                    iSelection;
        // not owned
        CIpsPlgEventHandler*              iEventHandler;
        CMsvEntrySelection*               iTempSelection;
        TDesC8*                           iFetchErrorProgress;
        TPckgBuf<TImap4SyncProgress>      iSyncProgress;
        TMsvId                            iParent;
        TFetchState                       iState;
        TBool                             iSyncStartedSignaled;
        
    };

#endif //__IPSPLGIMAP4POPULATEOP_H__
