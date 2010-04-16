/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*       folder refresh operation
*
*/


#if !defined(__IPSPLGCONNECTANDREFRESHFOLDERLIST_H__)
#define __IPSPLGCONNECTANDREFRESHFOLDERLIST_H__

#include <AknWaitNoteWrapper.h>
// <cmail>
#include <imapset.h>
// </cmail>

// Specific includes
#include "ipsplgonlineoperation.h"


/**
* Superoperation which wraps a UI around going online, updating
* an IMAP service's folder list and disconnecting again.
*/
NONSHARABLE_CLASS ( CIpsPlgConnectAndRefreshFolderList ) : public CIpsPlgOnlineOperation,
                                           public MAknBackgroundProcess
    {
    
public: // from MAknBackgroundProcess

    /**
     * Completes one cycle of the process.
     */
    virtual void StepL();

    /**
     * Return true when the process is done.
     */
    virtual TBool IsProcessDone() const;
    
    virtual void DialogDismissedL( TInt aButtonId );
    
public:

    /**
    * NewL()
    * @param CMsvSession&
    * @param TInt
    * @param TRequestStatus&
    * @param TMsvId
    * @return CImapConnectAndRefreshFolderList*
    */
    IMPORT_C static CIpsPlgConnectAndRefreshFolderList* NewL(
        CMsvSession& aSession, TInt aPriority, TRequestStatus& aObserverRequestStatus,
        TMsvId aService, TFSMailMsgId& aMailboxId, CMsvEntrySelection& aMsvEntry,
        MFSMailRequestObserver& aFSObserver, CIpsPlgTimerOperation& aTimer );

    /**
    * Destructor
    * ~CIpsPlgConnectAndRefreshFolderList()
    */
    virtual ~CIpsPlgConnectAndRefreshFolderList();
    
    /**
    *
    */
    const TDesC8& ProgressL();

    /**
    *
    */
    const TDesC8& GetErrorProgressL(TInt aError);
    
    /**
    * 
    */
    TFSProgress GetFSProgressL() const;

protected:


    /**
    * From 
    * DoRunL()
    */
    virtual void DoRunL();

    virtual void DoCancel();
    
private:

    enum TIpsSetFolderRefreshStates
        {
        EIdle,
        EStartConnect,
        EConnecting,
        ERefreshing,
        EDisconnecting,
        ECompleted
        };

    /**
    * C++ constructor
    * CIpsPlgConnectAndRefreshFolderList()
    */
    CIpsPlgConnectAndRefreshFolderList( CMsvSession& aSession, TInt aPriority,
        TRequestStatus& aObserverRequestStatus, TMsvId aService, TFSMailMsgId& aMailboxId,
        MFSMailRequestObserver& aFSObserver, CIpsPlgTimerOperation& aTimer );

    /**
    * ConstructL()
    */
    void ConstructL( CMsvEntrySelection& aMsvEntry );

    //data
private:
    
    TInt                                iState;
    TFSMailMsgId 						iMailboxId;
    CMsvSession&						iSession;
    TMsvId								iService;
    CMsvOperation* 						iOperation;
    CIpsPlgTimerOperation*				iTimer;
    CMsvEntrySelection*					iMsvEntry;
    TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
    TRequestStatus						iObserverRequestStatus;
    CAknWaitNoteWrapper*                iAsyncWaitNote;
    };

#endif
