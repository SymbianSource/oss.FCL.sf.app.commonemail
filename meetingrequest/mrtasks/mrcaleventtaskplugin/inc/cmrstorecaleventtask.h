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
* Description:  Interface definition for ESMR store calendar event to db task
*
*/


#ifndef C_ESMRSTORECALEVENTTASK_H
#define C_ESMRSTORECALEVENTTASK_H

#include <e32base.h>

#include "mesmrtask.h"


// Forward declarations
class MESMRCalEntry;
class MESMRCalDbMgr;

/**
 * CESMRStoreCalEventTask is responsibe for storing cal event to calendar DB.
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRStoreCalEventTask ) : public CBase,
                                              public MESMRTask
    {
public:
    /**
     * Creates and initializes new CESMRStoreCalEventTask object.
     * Ownership is transferred to caller.
     */
    static CESMRStoreCalEventTask* NewL( 
            MESMRCalEntry& aEntry, 
            MESMRCalDbMgr& aCalDbMgr );

    /**
     * C++ destructor.
     */
    ~CESMRStoreCalEventTask();

public:// From MESMRTask
    void ExecuteTaskL();

private: // Implementation
    
    CESMRStoreCalEventTask( 
            MESMRCalEntry& aEntry, 
            MESMRCalDbMgr& aCalDbMgr );
    void ConstructL();

    void StoreCalEventToDbL();
    
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

#endif // C_ESMRSTORECALEVENTTASK_H
