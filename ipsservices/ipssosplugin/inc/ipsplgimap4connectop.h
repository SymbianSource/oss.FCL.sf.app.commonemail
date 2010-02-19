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
* Description:  IMAP4 connect operation
*
*/

#ifndef __IPSPLGIMAP4CONNECTOP_H__
#define __IPSPLGIMAP4CONNECTOP_H__


#include "ipsplgonlineoperation.h"
#include "imapconnectionobserver.h"

class CIpsPlgEventHandler;

const TInt KIpsAutoPopulateByteLimit = KMaxTInt32;

/**
* class CIpsPlgImap4ConnectionOp
*
*/
    
class CIpsPlgImap4ConnectOp :
    public CIpsPlgOnlineOperation,
    public MMsvImapConnectionObserver,
    public MIpsPlgConnectOpCallback,
    public MFSMailRequestObserver
    {
    
public://from MIpsPlgConnectOpCallback
    
    void CredientialsSetL( TInt aEvent );
    
    public:
            
        /**
        * NewL()
        * Basic factory function - creates dialog with standard title resource
        * @param CMsvSession& CMsvSession reference
        * @param TInt priority
        * @param TRequestStatus& request status
        * @param TMsvId service (mailbox) id
        * @param TImapConnectionType connection type
        * @param 
        * @param 
        * @return CIpsPlgImap4ConnectionOp* self pointer
        */
		static CIpsPlgImap4ConnectOp* NewL(
			CMsvSession& aMsvSession,
			TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler,
            TBool aDoPlainConnect=EFalse,
            TBool aSignallingAllowed=ETrue );
            
        /**
        * ~CIpsPlgImap4ConnectionOp()
        * destructor
        */
        ~CIpsPlgImap4ConnectOp();

public: // from CIpsPlgBaseOperation

        virtual const TDesC8& ProgressL();

		/**
        *
        */
        virtual const TDesC8& GetErrorProgressL( TInt aError );
        
        /**
        * 
        */
        virtual TFSProgress GetFSProgressL() const;
        
        
        /**
        *
        */
        virtual TInt IpsOpType() const;
        
public: // from MMsvImapConnectionObserver
        
        /**
        * HandleImapConnectionEvent()
        * From MMsvImapConnectionObserver
        * @param TImapConnectionEvent, imap connection event
        */
        void HandleImapConnectionEvent(
            TImapConnectionEvent aConnectionEvent);  
        
        
public: // from MFSMailRequestObserver
    
        void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
        
public:

        /**
        *
        */
        TBool Connected() const;

protected:

		/**
        * From CActive
        */
        void DoRunL();
        
        /**
         * 
         */
        void DoCancel();
        
        /**
        * From CIpsPlgOnlineoperation
        */
        //TInt GetOperationErrorCodeL( );
        
	private:
		
		enum TImapConnectionState 
			{ 
			EStateStartConnect,
			EStateQueryingDetails,
			EStateQueryingDetailsBusy,
			EStateConnectAndSync,
			EStatePopulateAllCompleted,
			EStateCompleted,
			EStateIdle };
		
        /**
        * CIpsPlgImap4ConnectionOp()
        * @param CMsvSession&, CMsvSession reference
        * @param TInt, priority
        * @param TRequestStatus&, request status
        * @param TMsvId, service ( mailbox ) id
        * @param MMsvProgressReporter&, prog. reporter
        * @param TImapConnectionType, connection type
        */
        CIpsPlgImap4ConnectOp(
            CMsvSession& aMsvSession,
			TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aService,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver& aFSOperationObserver,
            TInt aFSRequestId,
            TBool aDoPlainConnect,
            TBool aSignallingAllowed,
            CIpsPlgEventHandler* aEventHandler );

        /**
        * ConstructL()
        */
        void ConstructL();
        
        /**
        *
        */        
        void DoConnectOpL();
        
        /**
        *
        */       
        void StartL();
        
        /**
        *
        */       
        void DoPopulateAllL();

        /**
         * Send user password query request to CIpsPlgEventHandler
         * @return ETrue - if query send
         */
        TBool QueryUserPassL();


        /**
         * 
         */
        void SignalSyncStarted();
        
        /**
         * 
         */
        void SignalSyncCompleted( TInt aError );
        
        
    private: //Data

        TImapConnectionState                iState;
        TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
        CMsvEntrySelection* 				iSelection;
        TBool                               iDoPlainConnect;
        
        
        
        // not owned
        CIpsPlgEventHandler*                iEventHandler;
        TBool                               iIsSyncStartedSignaled;

        // set to true if connection is already exists
        TBool 								iAlreadyConnected;
    };

#endif
