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
#include "mesmrmeetingrequestentry.h"
#include <e32cmn.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCombinedTask::CESMRCombinedTask
// ---------------------------------------------------------------------------
//
CESMRCombinedTask::CESMRCombinedTask(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRExecutionRule aRule )
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils ),
    iExecutionRule( aRule )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::~CESMRCombinedTask
// ---------------------------------------------------------------------------
//
CESMRCombinedTask::~CESMRCombinedTask()
    {
    FUNC_LOG;
    iTasks.ResetAndDestroy();
    iTasks.Close();
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::NewL
// ---------------------------------------------------------------------------
//
CESMRCombinedTask* CESMRCombinedTask::NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRExecutionRule aRule )
    {
    FUNC_LOG;
    CESMRCombinedTask* self =
        new (ELeave) CESMRCombinedTask(
            aCalDbMgr,
            aEntry,
            aMRMailboxUtils,
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
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::AppendTaskL
// ---------------------------------------------------------------------------
//
void CESMRCombinedTask::AppendTaskL( MESMRTask* aTask )
    {
    FUNC_LOG;
    User::LeaveIfError( iTasks.Append(aTask ) );
    }

// ---------------------------------------------------------------------------
// CESMRCombinedTask::RemoveTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRCombinedTask::RemoveTaskL( MESMRTask* aTask )
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
    
    MESMRMeetingRequestEntry& mrEntry( ESMREntry() );    
    if ( mrEntry.IsEntryEditedL() )
        {
        mrEntry.UpdateTimeStampL();
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

