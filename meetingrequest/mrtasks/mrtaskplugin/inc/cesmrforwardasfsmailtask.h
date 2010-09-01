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
* Description:  Class definition for forwarding meeting request via FS EMail
*
*/


#ifndef C_ESMRFORWARDMRFSEMAILTASK_H
#define C_ESMRFORWARDMRFSEMAILTASK_H

// INCLUDES:
#include "cesmrtaskbase.h"

// FORWARD DECLARATIONS:
class MESMRCalDbMgr;
class MESMRMeetingRequestEntry;
class CMRMailboxUtils;
class CESMRFSEMailManager;
class CESMRCalUserUtil;
class CCalEntry;

/**
 *  Class definition for forwarding meeting request as attachment
 *  of email message via FS EMail. Class creates temporary file into
 *
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRForwardAsFSMailTask ) : public CESMRTaskBase
    {

public: // Construction and destruction
    /**
     * Creates and initializes new CESMRCancelMRFSEmailTask object.
     * Ownership is transferred to caller.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     */
    static CESMRForwardAsFSMailTask* NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils );

    /**
     * C++ constructor.
     */
    CESMRForwardAsFSMailTask(MESMRCalDbMgr& aCalDbMgr,
                              MESMRMeetingRequestEntry& aEntry,
                              CMRMailboxUtils& aMRMailboxUtils);

    /**
     * C++ Destructor.
     */
     ~CESMRForwardAsFSMailTask();

public:// From MESMRTask

    void ExecuteTaskL();

private:
    void ConstructL();
    void ConstructMailL();
    void CreateTemporaryMeetingFileL();
    TInt LaunchFSEmailEditorL();

private: // Data
    /// Own: FS Email manager
    CESMRFSEMailManager* iEmailMgr;
    /// Own: Pointer to calendar entry to be sent
    CCalEntry* iEntryToSend;
    /// Own: Caluser utility object
    CESMRCalUserUtil* iCaluserUtil;
    };

#endif  // C_ESMRFORWARDMRFSEMAILTASK_H
