/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MR Cal Event Task Plugin implementation
*
*/
#include "cmrcaleventtaskplugin.h"

#include "cmrstorecaleventtask.h"
#include "cmrcalentryuideletetask.h"
#include "cmrcalentryuiedittask.h"
#include "cmrtodomarkasdonetask.h"
#include "cmrtodomarkasnotdonetask.h"
#include "cesmrcombinedtask.h"

#include "emailtrace.h"

namespace { // codescanner::namespace

#ifdef _DEBUG

// Panic literal for ESMRTaskFactory
_LIT( KESMRTaskFactoryPanicTxt, "MRCalEventTaskPlugin" );

/** Panic code definitions */
enum TESMRTaskFactoryPanic
    {
    EESMRTaskFactoryInvalidTask // Trying to create invalid task
    };


// ---------------------------------------------------------------------------
// Panic wrapper method
// ---------------------------------------------------------------------------
//
void Panic( TESMRTaskFactoryPanic aPanic )
    {

    User::Panic( KESMRTaskFactoryPanicTxt, aPanic );
    }

#endif // _DEBUG

} // namespace

// -----------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CMRCalEventTaskPlugin
// -----------------------------------------------------------------------------
//
CMRCalEventTaskPlugin::CMRCalEventTaskPlugin( MESMRCalDbMgr& aCalDbMgr )
    : iCalDbMgr( aCalDbMgr )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRCalEventTaskPlugin::~CMRCalEventTaskPlugin
// -----------------------------------------------------------------------------
//
CMRCalEventTaskPlugin::~CMRCalEventTaskPlugin()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRCalEventTaskPlugin::NewL
// -----------------------------------------------------------------------------
//
CMRCalEventTaskPlugin* CMRCalEventTaskPlugin::NewL( TAny* aCalDbMgr )
    {
    FUNC_LOG;
    MESMRCalDbMgr* calDbMgr = static_cast< MESMRCalDbMgr* >( aCalDbMgr );

    CMRCalEventTaskPlugin* self =
        new( ELeave )CMRCalEventTaskPlugin( *calDbMgr );

    return self;
    }

// ---------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CreateTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRCalEventTaskPlugin::CreateTaskL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    MESMRTask* task = NULL;

    switch( aCommand )
        {
        case EESMRCmdSaveMR:
        case EESMRCmdCalEntryUISave:
            {
            task = CreateStoreCalEventToDBTaskL( aEntry );
            }
            break;
            
        case EESMRCmdCalEntryUIDelete:
            {
            task = CreateCalEntryUiDeleteTaskL( aEntry );
            }
            break;
            
        case EESMRCmdTodoMarkAsDone:
            {
            task = CreateTodoMarkAsDoneTaskL( aEntry );
            }
            break;

        case EESMRCmdTodoMarkAsNotDone:
            {
            task = CreateTodoMarkAsNotDoneTaskL( aEntry );
            }
            break;

        case EESMRCmdCalendarChange:
            {
            task = CreateMoveCalEntryToCurrentDBTaskL( aEntry );
            }
            break;

        default:
            __ASSERT_DEBUG( EFalse, Panic( EESMRTaskFactoryInvalidTask ) );
            User::Leave( KErrNotSupported );
            break;
        }

    return task;
    }

// ---------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CreateStoreCalEventToDBTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRCalEventTaskPlugin::CreateStoreCalEventToDBTaskL(
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    // Store calendar event to calendar database task
    return CESMRStoreCalEventTask::NewL( aEntry, iCalDbMgr );
    }

// ---------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CreateCalEntryUiDeleteTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRCalEventTaskPlugin::CreateCalEntryUiDeleteTaskL(
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    // Delete calendar event from calendar database task
    return CESMRCalEntryUiDeleteTask::NewL( aEntry, iCalDbMgr );
    }

// ---------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CreateCalEntryUiEditTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRCalEventTaskPlugin::CreateCalEntryUiEditTaskL(
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    // Edit calendar event task
    return CESMRCalEntryUiEditTask::NewL( aEntry );
    }

// ---------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CreateTodoMarkAsDoneTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRCalEventTaskPlugin::CreateTodoMarkAsDoneTaskL(
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    // Mark to-do as done task
    return CESMRTodoMarkAsDoneTask::NewL( aEntry, iCalDbMgr );
    }

// ---------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CreateTodoMarkAsNotDoneTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRCalEventTaskPlugin::CreateTodoMarkAsNotDoneTaskL(
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    // Mark to-do as not done task
    return CESMRTodoMarkAsNotDoneTask::NewL( aEntry, iCalDbMgr );
    }

// ---------------------------------------------------------------------------
// CMRCalEventTaskPlugin::CreateMoveCalEntryToCurrentDBTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRCalEventTaskPlugin::CreateMoveCalEntryToCurrentDBTaskL(
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    CESMRCombinedTask* task = CESMRCombinedTask::NewL(
            aEntry,
            CESMRCombinedTask::EESMRTrap );
    CleanupStack::PushL( task );

    AppendTaskL( *task, CreateCalEntryUiDeleteTaskL( aEntry ) );
    AppendTaskL( *task, CreateStoreCalEventToDBTaskL( aEntry ) );

    CleanupStack::Pop( task );
    return task;
    }

// EOF
