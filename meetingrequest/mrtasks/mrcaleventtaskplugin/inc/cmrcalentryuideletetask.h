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
* Description:  Interface definition for ESMR delete calendar event from db task
*
*/


#ifndef C_ESMRCALENTRYUIDELETETASK_H
#define C_ESMRCALENTRYUIDELETETASK_H

#include <e32base.h>

#include "mesmrtask.h"

// Forward declarations
class MESMRCalEntry;
class MESMRCalDbMgr;

/**
 * CESMRCalEntryUiDeleteTask is responsibe for deleting cal event from calendar DB.
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRCalEntryUiDeleteTask ) : public CBase,
                                                 public MESMRTask
    {
public:
    /**
     * Creates and initializes new CESMRCalEntryUiDeleteTask object.
     * Ownership is transferred to caller.
     */
    static CESMRCalEntryUiDeleteTask* NewL( MESMRCalEntry& aEntry, 
                                            MESMRCalDbMgr& aCalDbMgr );

    /**
     * C++ destructor.
     */
    ~CESMRCalEntryUiDeleteTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRCalEntryUiDeleteTask( MESMRCalEntry& aEntry, 
                               MESMRCalDbMgr& aCalDbMgr);

    void ConstructL();
    void DeleteEntryFromDbL();
    
private: // Data
    /**
    * Reference to ES MR calendar entry
    * Not Own.
    */
    MESMRCalEntry& iEntry;
    /**
    * Reference to cal db manager
    * Not own.
    */
    MESMRCalDbMgr& iCalDbMgr;
    };

#endif // C_ESMRCALENTRYUIDELETETASK_H
