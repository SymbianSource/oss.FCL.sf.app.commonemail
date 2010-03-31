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


#ifndef C_ESMRSENDMRFFSEMAILTASK_H
#define C_ESMRSENDMRFFSEMAILTASK_H

#include "cesmrtaskbase.h"
//<cmail>
#include "esmrcommands.h"
//</cmail>

class CCalEntry;
class MESMRCalDbMgr;
class MESMRMeetingRequestEntry;
class CMRMailboxUtils;
class CESMRFSEMailManager;
class CESMRCalUserUtil;
class CCalAttendee;

/**
 *  Class definition for sending meeting request via FS EMail.
 *
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRSendMRFSMailTask ) :
        public CESMRTaskBase
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRCancelMRFSEmailTask object.
     * Ownership is transferred to caller.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     * @param aCommand Command to be executed
     */
    static CESMRSendMRFSMailTask* NewL(
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TESMRCommand aCommand );

    /**
     * C++ destructor.
     */
    ~CESMRSendMRFSMailTask();

public: // From MESMRTask
    void ExecuteTaskL();

private: // Definitios
    /** Enumeration for update sending type */
    enum TESMRSendType
        {
        // MR is sent to all
        ESendToAll,
        // MR is sent to added and removed
        ESentToAddedAndRemoved
        };

private: // Implementation
    CESMRSendMRFSMailTask(
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TESMRCommand aCommand );
    void ConstructL();

    void ConstructMailL();
    void AddAttendeesL();
    TInt SaveICalToFileL(
            const TDesC& aICal,
            const TDesC& aFilename );
    HBufC* ResolveUsedMailboxUserAddressLC();
    void ResolveSendTypeL();
    TBool NeedToSendInvitationL();
    TBool NeedToSendCancellationL();
    void SetCorrectAttendeesToEntryL();
    void GetAttendeesToBeSentL(
            RArray<CCalAttendee*>& aRequiredAttendees,
            RArray<CCalAttendee*>& aOptionalAttendees );
    void GetEntriesToBeSentL();
    void AddAttachmentsL();
    
private: // Data
    /// Own: FS email manager
    CESMRFSEMailManager* iEmailMgr;
    /// Ref: Pointer to calendar entry to be sent
    CCalEntry* iEntryToSend;
    /// Own: Pointer to calendar entries to be sent
    RPointerArray<CCalEntry> iEntriesToSend;
    /// Own: Caluser utility object
    CESMRCalUserUtil* iCaluserUtil;
    /// Own: Send type definition
    TESMRSendType iSendType;
    /// Own: Command to be executed
    TESMRCommand iCommand;
    };

#endif  // C_ESMRSENDMRFFSEMAILTASK_H

// EOF
