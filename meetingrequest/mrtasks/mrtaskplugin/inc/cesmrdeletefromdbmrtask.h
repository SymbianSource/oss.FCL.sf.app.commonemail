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


#ifndef C_ESMRDELETEMRDROMDBTASK_H
#define C_ESMRDELETEMRDROMDBTASK_H

#include "cesmrtaskbase.h"

/**
 * CESMRDeleteMRFromDbTask is responsibe for deleting MR from calendar DB.
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRDeleteMRFromDbTask ) : public CESMRTaskBase
    {
public:
    /**
     * Creates and initializes new CESMRDeleteMRFromDbTask object.
     * Ownership is transferred to caller.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     */
    static CESMRDeleteMRFromDbTask* NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils );

    /**
     * C++ destructor.
     */
    ~CESMRDeleteMRFromDbTask();

public: // From MESMRTask
    void ExecuteTaskL();

private:
    CESMRDeleteMRFromDbTask(
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils );

    void ConstructL();

    void DeleteEntryFromDbL();
    };

#endif // C_ESMRDELETEMRDROMDBTASK_H
