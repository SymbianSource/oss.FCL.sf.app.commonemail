/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition for sending meeting request via FS EMail
*
*/


#ifndef C_ESMRSENDMRFSMAILREPLYTASK_H
#define C_ESMRSENDMRFSMAILREPLYTASK_H

#include "cesmrtaskbase.h"

//Forward declarations
class MESMRCalDbMgr;
class MESMRMeetingRequestEntry;
class CMRMailboxUtils;
class CESMRFSEMailManager;
class CESMRCalUserUtil;
class CCalEntry;

/**
 *  Class definition for sending meeting request via FS EMail.
 *
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRSendMRFSMailReplyTask ) :
        public CESMRTaskBase
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRCancelMRFSEmailTask object.
     * Ownership is transferred to caller.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     */
    static CESMRSendMRFSMailReplyTask* NewL(
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TBool aReplyAll);

    /**
     * C++ destructor.
     */
    ~CESMRSendMRFSMailReplyTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRSendMRFSMailReplyTask(
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TBool aReplyAll);

    void ConstructL();
    void ConstructMailL();
    void AddAttendeesL();
    HBufC* ResolveUsedMailboxUserAddressLC();
    TInt LaunchFSEmailEditorL(TBool aReplAll);

private: // Data
    /// Own: FS email manager
    CESMRFSEMailManager* iEmailMgr;
    /// Own: Pointer to calendar entry to be sent
    CCalEntry* iEntryToSend;
    /// Own: Caluser utility object
    CESMRCalUserUtil* iCaluserUtil;
    
    TBool iReplyAll;
    };

#endif  // C_ESMRSENDMRFSMAILREPLYTASK_H
