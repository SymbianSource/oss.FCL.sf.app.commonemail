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
* Description:  IMAP4 attachment fetch operation
*
*/

#ifndef __IPSPLGIMAP4FETCHATTACHMENTOP_H__
#define __IPSPLGIMAP4FETCHATTACHMENTOP_H__


class CIpsPlgImap4FetchAttachmentOp;

/**
* class CIpsPlgImap4FetchAttachmentOp
*
*/
class CIpsFetchProgReport : public CActive
    {
    public:
        /**
        *
        */
        static CIpsFetchProgReport* NewL( 
            CIpsPlgImap4FetchAttachmentOp& aAttaOp );

        /**
        *
        */    
        ~CIpsFetchProgReport();

        /**
        *
        */    
        virtual void DoCancel();

        /**
        *
        */	
	    virtual void RunL();
	
	private:

        /**
        *
        */	
	    CIpsFetchProgReport( 
	        CIpsPlgImap4FetchAttachmentOp& aAttaOp );
        
        /**
        *
        */	    
	    void ConstructL();

        /**
        *
        */	
	    void AdjustTimer();
	
	private:
	
	    RTimer iTimer;
	    
	    CIpsPlgImap4FetchAttachmentOp& iAttaOp;
	
    };

/**
* class CIpsPlgImap4FetchAttachmentOp
*
*/
class CIpsPlgImap4FetchAttachmentOp :
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
        * @return CIpsPlgImap4FetchAttachmentOp* self pointer
        */

        static CIpsPlgImap4FetchAttachmentOp* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aFunctionId,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            const TImImap4GetMailInfo& aGetMailInfo,
            const CMsvEntrySelection& aSel,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId );

        /**
        * ~CIpsPlgImap4FetchAttachmentOp()
        * destructor
        */

        virtual ~CIpsPlgImap4FetchAttachmentOp();

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

        CIpsPlgImap4FetchAttachmentOp(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aFunctionId,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            const TImImap4GetMailInfo& aGetMailInfo,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId );

        /**
        * ConstructL()
        */

        void ConstructL( const CMsvEntrySelection& aSel );


        /**
        * RunL()
        */

        void RunL();

        /**
        * DoRunL()
        */

        void DoRunL();

        /**
        * DoCancel()
        */

        //virtual void DoCancel();

        /**
        * Complete()
        */

        void Complete();
        
        /**
        *
        */
        void DoConnectL();
        
        /**
        * 
        */
        void ReportProgressL();  

    protected:
        
        /**
        * From CIpsPlgOnlineoperation
        */
        TInt GetEngineProgress( const TDesC8& aProgress );
        
        /**
        *
        */
        void DoFetchAttachmentL( );
        
        enum TFetchState {
            EStateIdle,
            EStateConnecting,
            EStateFetching,
            EStateDisconnecting };
        TFetchState iState;
        
    private: //Data
    
        // because use of ReportProgressL()
        friend class CIpsFetchProgReport;
            
        CMsvEntrySelection*                     iSelection;
        TDesC8*                                 iFetchErrorProgress;
        TImImap4GetMailInfo                     iGetMailInfo;
        TInt                                    iFunctionId;
        TPckgBuf<TImap4CompoundProgress>        iProgress;
        TMsvId                                  iService;
        TBool                                   iPopulated;
        CIpsFetchProgReport*                    iProgReport;
    };

#endif //__IPSPLGIMAP4FETCHATTACHMENTOP_H__
