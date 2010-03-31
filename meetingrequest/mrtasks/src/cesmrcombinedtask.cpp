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
* Description:  Implementation for ESMR base task
*
*/


#include "emailtrace.h"
#include "cesmrcombinedtask.h"
#include "mesmrcalentry.h"
#include <e32cmn.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCombinedTask::CESMRCombinedTask
// ---------------------------------------------------------------------------
//
CESMRCombinedTask::CESMRCombinedTask(
        MESMRCalEntry& aEntry,
        TESMRExecutionRule aRule )
:   iEntry( aEntry ),
    iExecutionRule( aRule )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::~CESMRCombinedTask
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCombinedTask::~CESMRCombinedTask()
    {
    FUNC_LOG;
    iTasks.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCombinedTask* CESMRCombinedTask::NewL(
        MESMRCalEntry& aEntry,
        TESMRExecutionRule aRule )
    {
    FUNC_LOG;
    CESMRCombinedTask* self =
        new (ELeave) CESMRCombinedTask(
            aEntry,
            aRule );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCombinedTask::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::AppendTaskL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRCombinedTask::AppendTaskL( MESMRTask* aTask )
    {
    FUNC_LOG;
    iTasks.AppendL( aTask );
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::RemoveTaskL
// ---------------------------------------------------------------------------
//
EXPORT_C MESMRTask* CESMRCombinedTask::RemoveTaskL( MESMRTask* aTask )
    {
    FUNC_LOG;
    TInt index = iTasks.Find( aTask );
    if ( KErrNotFound == index )
        {
        User::Leave( KErrNotFound );
        }

    MESMRTask* task = iTasks[index];
    iTasks.Remove( index );

    return task;
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRCombinedTask::ExecuteTaskL()
    {
    FUNC_LOG;

    TInt err( KErrNone );
    TInt taskCount( iTasks.Count() );
    
    if ( iEntry.IsEntryEditedL() )
        {
        iEntry.UpdateTimeStampL();
        }
    
    for( TInt i(0); i < taskCount; ++i )
        {
        MESMRTask* task = iTasks[i];

        if ( EESMRTrap == iExecutionRule )
            {
            TRAPD( taskErr, task->ExecuteTaskL() );


            if ( taskErr == KErrCancel )
                {
                // Task cancellation is leaved
                User::Leave( KErrCancel );
                }
            else if ( KErrNone != taskErr )
                {
                err = taskErr;
                }
            }
        else if ( EESMRLeave == iExecutionRule )
            {
            task->ExecuteTaskL();
            }
        }

    User::LeaveIfError( err );

    }

// ---------------------------------------------------------------------------
// AppendTaskL
// ---------------------------------------------------------------------------
//
EXPORT_C void AppendTaskL( CESMRCombinedTask& aContainer, MESMRTask* aTask )
    {
    CleanupDeletePushL( aTask );
    aContainer.AppendTaskL( aTask );
    CleanupStack::Pop( aTask );
    }
