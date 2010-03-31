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
* Description:  Interface definition for ESMR send mr response via mail task
*
*/


#ifndef C_ESMRSENDMRRESPMAILTASK_H
#define C_ESMRSENDMRRESPMAILTASK_H

//<cmail>
#include "esmrdef.h"
//</cmail>
#include "cesmrtaskbase.h"

class CCalEntry;

/**
 * Implementation for sending meeting request response over MTM based
 * mail box.
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRSendMRRespMailTask ) : public CESMRTaskBase
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRSendMRRespMailTask object.
     * Ownership is transferred to caller.
     * @param aCommand Response command.
     * @param aResponseType Response message type.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     */
    static CESMRSendMRRespMailTask* NewL(
            TESMRCommand aCommand,
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TESMRResponseType aResponseType,
            const TDesC& aResponseMessage );

    /**
     * C++ destructor.
     */
    ~CESMRSendMRRespMailTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRSendMRRespMailTask(
            TESMRCommand aCommand,
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TESMRResponseType aResponseType );

    void ConstructL(
            const TDesC& aResponseMessage );

    void SendResponseMessageL();

private: // Data
    /**
    * Response command
    */
    TESMRCommand iCommand;

    /**
    * Response message type
    */
    TESMRResponseType iResponseType;

    /**
    * Response message text
    * Own.
    */
    HBufC* iResponseMessage;
    
    /**
    * Response entry
    * Own.
    */
    CCalEntry* iResponseEntry;
    };

#endif // C_ESMRSENDMRRESPMAILTASK_H
