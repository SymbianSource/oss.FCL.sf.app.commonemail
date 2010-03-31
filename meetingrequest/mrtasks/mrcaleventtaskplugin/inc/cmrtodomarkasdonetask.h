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
* Description:  Interface definition for ESMR todo mark as done task
*
*/


#ifndef C_ESMRTODOMARKASDONETASK_H
#define C_ESMRTODOMARKASDONETASK_H

#include <e32base.h>

#include "mesmrtask.h"

// Forward declarations
class MESMRCalEntry;
class MESMRCalDbMgr;

/**
 * CESMRTodoMarkAsDoneTask is responsibe for marking todo as done
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRTodoMarkAsDoneTask ) : public CBase,
                                               public MESMRTask
    {
public:
    /**
     * Creates and initializes new CESMRTodoMarkAsDoneTask object.
     * Ownership is transferred to caller.
     */
    static CESMRTodoMarkAsDoneTask* NewL( MESMRCalEntry& aEntry, 
                                          MESMRCalDbMgr& aCalDbMgr );

    /**
     * C++ destructor.
     */
    ~CESMRTodoMarkAsDoneTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRTodoMarkAsDoneTask( MESMRCalEntry& aEntry, 
                             MESMRCalDbMgr& aCalDbMgr );
    void ConstructL();
    void MarkTodoAsDoneL();
    
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

#endif // C_ESMRTODOMARKASDONETASK_H
