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
* Description:  Interface definition for Meeting request task factory implementation
*
*/


#ifndef CMRRTASKPLUGIN_H
#define CMRRTASKPLUGIN_H

#include <e32base.h>
#include "cesmrtaskfactory.h"

class MESMRCalDbMgr;
class MESMRMeetingRequestEntry;

/**
 *  CESMRTaskFactory implements interface for creating ES MR tasks.
 *  @see MESMRTaskFactory
 *
 *  @lib esmrtaskplugin.lib
 */
NONSHARABLE_CLASS(CMRTaskPlugin) : public CESMRTaskFactory
    {
public: // Construction and destruction
    /**
     * Creates and initialzes new MR task factory.
     * @param aCalDbMgr Pointer to cal db manager.
     */
    static CMRTaskPlugin* NewL( TAny* aCalDbMgr );

    /**
     * C++ destructor.
     */
    ~CMRTaskPlugin();

public: // From MESMRTaskFactory
    MESMRTask* CreateTaskL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );

private: // Implementation
    CMRTaskPlugin( MESMRCalDbMgr& aCalDbMgr );
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
    MESMRTask* CreateMoveMRToCurrentDBTaskL(
            MESMRMeetingRequestEntry& aEntry );

private: // Data
    /*
     * Reference to cal db manager
     * Not own.
     */
    MESMRCalDbMgr& iCalDbMgr;

    };

#endif // C_ESMRTASKFACTORY_H
