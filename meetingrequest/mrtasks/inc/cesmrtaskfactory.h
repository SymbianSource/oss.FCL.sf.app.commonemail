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
* Description:  Interface definition for ESMR task factory implementation
*
*/


#ifndef CESMRTASKFACTORY_H
#define CESMRTASKFACTORY_H

#include <e32base.h>
#include "mesmrtaskfactory.h"

class MESMRCalDbMgr;
class CMRMailboxUtils;

/**
 *  CESMRTaskFactory implements interface for creating ES MR tasks.
 *  @see MESMRTaskFactory
 *
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS(CESMRTaskFactory) : public CBase,
                                      public MESMRTaskFactory
    {
public: // Construction and destruction
    /**
     * Creates and initialzes new ES MR task factory.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aMRMailboxUtils Reference to mailbox utilities.
     */
    IMPORT_C static CESMRTaskFactory* NewL(
            MESMRCalDbMgr& aCalDbMgr,
            CMRMailboxUtils& aMRMailboxUtils );

    /**
     * C++ destructor.
     */
    ~CESMRTaskFactory();

public: // From MESMRTaskFactory
    MESMRTask* CreateTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );

private: // Implementation
    CESMRTaskFactory(
            MESMRCalDbMgr& aCalDbMgr,
            CMRMailboxUtils& aMRMailboxUtils );
    MESMRTask* CreateSendMRResponseViaMailTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    MESMRTask* CreateSendMRTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    MESMRTask* CreateDeleteMRTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    MESMRTask* CreateStoreMRToLocalDBTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    MESMRTask* CreateForwardAsMeetingTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    MESMRTask* CreateForwardAsMailTaskL(
            MESMRMeetingRequestEntry& aEntry  );
    MESMRTask* CreateOrganizerDeleteMRTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    MESMRTask* CreateAttendeeDeleteMRTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    MESMRTask* CreateReplyAsMailTaskL(
                MESMRMeetingRequestEntry& aEntry,
                TBool aReplyAll);

private: // Data
    /*
    * Reference to cal db manager
    * Not own.
    */
    MESMRCalDbMgr& iCalDbMgr;

    /**
    * Reference to mr mailbox utilities
    * Not own.
    */
    CMRMailboxUtils& iMRMailboxUtils;
    };

#endif // C_ESMRTASKFACTORY_H
