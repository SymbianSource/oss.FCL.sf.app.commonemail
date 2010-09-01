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
* Description:  Interface definition for ESMR cal entry edit task
*
*/


#ifndef C_ESMRCALENTRYUIEDITTASK_H
#define C_ESMRCALENTRYUIEDITTASK_H

#include <e32base.h>

#include "mesmrtask.h"

// Forward declarations
class MESMRCalEntry;

/**
 * CESMRCalEntryUiEditTask is responsibe for creating entry edit task 
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRCalEntryUiEditTask ) : public CBase,
                                               public MESMRTask
    {
public:
    /**
     * Creates and initializes new CESMRCalEntryUiEditTask object.
     * Ownership is transferred to caller.
     */
    static CESMRCalEntryUiEditTask* NewL( MESMRCalEntry& aEntry );

    /**
     * C++ destructor.
     */
    ~CESMRCalEntryUiEditTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRCalEntryUiEditTask();

    void ConstructL();
    void EditCalEventL();
    };

#endif // C_ESMRSTORECALEVENTTASK_H
