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
* Description:  IMAP4 remote move message operation
*
*/

#ifndef __IPSPLGIMAP4MOVEREMOTEOP_H__
#define __IPSPLGIMAP4MOVEREMOTEOP_H__


#include "ipsplgonlineoperation.h"
#include "mfsmailrequestobserver.h"

/**
* Move a selection of messages that may or may not be complete.
* Always fetches entire messages.
*/
class CIpsPlgImap4MoveRemoteOp :
    public CIpsPlgOnlineOperation
    {
    public:
        /**
        *
        * @param aSession: Server session.
        * @param aStatus: Observer completion status.
        * @param aReporter: Progress reporter.
        * @param aSel: The message ids to get.
        * @param aTargetId: Where to fetch the messages to.
        * @param aCopy: Copying or moving.
        */
        static CIpsPlgImap4MoveRemoteOp* NewL(
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
        *
        */
        virtual ~CIpsPlgImap4MoveRemoteOp();
        
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
        * for explanation of parameters, see NewL
        */
        CIpsPlgImap4MoveRemoteOp(
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
        *
        */
        void ConstructL(const CMsvEntrySelection& aSel);
        
        /**
        * RunL()
        */
        void RunL();
        
        /**
        *
        */
        void DoRunL();
        
        
        /**
        *
        */
        void DoConnectL();
        
        /**
        *
        * Sort messages into complete and incomplete.
        */
        void SortMessageSelectionL(const CMsvEntrySelection& aSel);
        
        /**
        *
        */

        void Complete();

        /**
        *
        */
        void DoMoveRemoteL();
        
    protected:
    
        /**
        * From CIpsPlgOnlineoperation
        */
        TInt GetEngineProgress( const TDesC8& aProgress );

    private:
        enum TState { EIdle, EConnecting, ELocalMsgs, ERemoteMsgs };
        TState                              iState;
        TInt                                iFunctionId;
        TDesC8*                             iMoveErrorProgress;
        TImImap4GetMailInfo                 iGetMailInfo;
        CMsvEntrySelection*                 iSelection;
        CMsvEntrySelection*                 iRemoteSel;     // Incomplete messages to be fetched.
        TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
        TPckgBuf<TImap4SyncProgress>        iSyncProgress;
    };


NONSHARABLE_CLASS( CIpsPlgImap4MoveRemoteOpObserver ) : public CBase,
    public MFSMailRequestObserver
    {
public:

    static CIpsPlgImap4MoveRemoteOpObserver* NewL( CMsvSession& aSession,
        CIpsPlgEventHandler& aEventHandler, const TFSMailMsgId& aSourceFolder,
        const RArray<TFSMailMsgId>& aMessageIds );
    ~CIpsPlgImap4MoveRemoteOpObserver();

    // From base class MFSMailRequestObserver
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

private:

    CIpsPlgImap4MoveRemoteOpObserver( CMsvSession& aSession,
        CIpsPlgEventHandler& aEventHandler,
        TMsvId aSourceFolderId );
    void ConstructL( const RArray<TFSMailMsgId>& aMessageIds );

private:

    // data
    CMsvSession& iSession;
    CIpsPlgEventHandler& iEventHandler;
    TMsvId iSourceFolderId;
    CMsvEntrySelection* iSelection;
    };

#endif //__IPSPLGIMAP4MOVEREMOTEOP_H__

// End of File
