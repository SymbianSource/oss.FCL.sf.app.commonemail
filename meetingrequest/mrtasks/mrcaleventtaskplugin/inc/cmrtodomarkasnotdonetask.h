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
* Description:  Interface definition for ESMR marking to-do as not done
*
*/


#ifndef C_ESMRTODOMARKASNOTDONETASK_H
#define C_ESMRTODOMARKASNOTDONETASK_H

#include <e32base.h>

#include "mesmrtask.h"

// Forward declarations
class MESMRCalEntry;
class MESMRCalDbMgr;

/**
 * CESMRTodoMarkAsNotDoneTask is responsibe marking to-do as not done.
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRTodoMarkAsNotDoneTask ) : public CBase,
                                                  public MESMRTask
    {
public:
    /**
     * Creates and initializes new CESMRTodoMarkAsNotDoneTask object.
     * Ownership is transferred to caller.
     */
    static CESMRTodoMarkAsNotDoneTask* NewL( MESMRCalEntry& aEntry, 
                                             MESMRCalDbMgr& aCalDbMgr );

    /**
     * C++ destructor.
     */
    ~CESMRTodoMarkAsNotDoneTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRTodoMarkAsNotDoneTask( MESMRCalEntry& aEntry, 
                                MESMRCalDbMgr& aCalDbMgr);

    void ConstructL();
    void MarkTodoAsNotDoneL();

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

#endif // C_ESMRTODOMARKASNOTDONETASK_H
