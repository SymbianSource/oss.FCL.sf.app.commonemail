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


#ifndef C_ESMRSTOREMRTASK_H
#define C_ESMRSTOREMRTASK_H

#include "cesmrtaskbase.h"

/**
 * CESMRStoreMRTask is responsibe for storing MR to calendar DB.
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRStoreMRTask ) : public CESMRTaskBase
    {
public:
    /**
     * Creates and initializes new CESMRStoreMRTask object.
     * Ownership is transferred to caller.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     */
    static CESMRStoreMRTask* NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils );

    /**
     * C++ destructor.
     */
    ~CESMRStoreMRTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRStoreMRTask(
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils );

    void ConstructL();
    void StoreEntryToDbL();
    };

#endif // C_ESMRSTOREMRTASK_H
