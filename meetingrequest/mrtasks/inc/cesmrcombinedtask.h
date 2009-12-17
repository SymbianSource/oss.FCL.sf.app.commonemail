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


#ifndef C_ESMRCOMBINEDTASK_H
#define C_ESMRCOMBINEDTASK_H

#include "cesmrtaskbase.h"

/**
 * CESMRCombinedTask is responsibe for executing multiple tasks.
 *
 * CESMRCombinedTask executes the tasks in the order they are appended
 * to task list.
 *
 * It is possible first add some tasks and execute them. Then remove
 * some tasks and execute the task again.
 *
 * @code
 *   CESMRCombinedTask* task = CESMRCombinedTask::NewL( ... );
 *   CleanupStack::PushL( task );
 *   task->AppendTaskL( ... );
 *   task->AppendTaskL( ... );
 *   task->AppendTaskL( ... );
 *   task->ExecuteTaskL();
 *   task->RemoveTaskL( ... );
 *   task->ExecuteTaskL();
 *   CleanupStack::PopAndDestroy();
 * @endcode
 *
 * @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRCombinedTask ) : public CESMRTaskBase
    {
public:

    /**
     * Enumeration for combined task execution rule.
     * If EESMRTrap is used, each task is trapped when executed.
     * If any of the tasks leave, then combined task leave after
     * each task is executed.
     * However leave code KErrCancel is leaved right away.
     */
    enum TESMRExecutionRule
        {
        EESMRLeave,     // Leave if any of the tasks leave
        EESMRTrap       // TRAP each task and continue
        };

public:
    /**
     * Creates and initializes new CESMRCombinedTask object.
     * Ownership is transferred to caller.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     * @param aRule Exection rule
     */
    static CESMRCombinedTask* NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRExecutionRule aRule =  CESMRCombinedTask::EESMRLeave );

    /**
     * C++ destructor.
     */
    ~CESMRCombinedTask();

    /**
     * Appends task to be executed. Ownership of appended task
     * is transferred.
     * @param aTask Pointer to task to be executed.
     */
    void AppendTaskL( MESMRTask* aTask );

    /**
     * Removes the task from task list. Method returns the pointer to
     * removed task. Ownership of the removed task is transferred to caller.
     *
     * @param aTask Pointer to task to be removed.
     * @return Pointer to removed task.
     */
    MESMRTask* RemoveTaskL( MESMRTask* aTask );

public:// From MESMRTask
    void ExecuteTaskL();

private: // Implementation
    CESMRCombinedTask(
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TESMRExecutionRule aRule );

    void ConstructL();

private: // Data
    /**
    * Array of tasks to be executed
    * Own
    */
    RPointerArray<MESMRTask> iTasks;

    /**
    * Task execution rule
    */
    TESMRExecutionRule iExecutionRule;
    };

#endif // C_ESMRCOMBINEDTASK_H
