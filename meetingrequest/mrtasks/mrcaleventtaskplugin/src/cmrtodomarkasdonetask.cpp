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
* Description:  Implementation for ESMR calendar event task
*
*/

#include "cmrtodomarkasdonetask.h"
#include "cesmrcaldbmgr.h"
#include "mesmrcalentry.h"

#include <caltime.h>
#include <calentry.h>

#include "emailtrace.h"

namespace { // codescanner::namespace

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTodoMarkAsDoneTask::CESMRTodoMarkAsDoneTask
// ---------------------------------------------------------------------------
//
CESMRTodoMarkAsDoneTask::CESMRTodoMarkAsDoneTask( MESMRCalEntry& aEntry, 
        MESMRCalDbMgr& aCalDbMgr )
:   iEntry( aEntry ), 
    iCalDbMgr( aCalDbMgr )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTodoMarkAsDoneTask::~CESMRTodoMarkAsDoneTask
// ---------------------------------------------------------------------------
//
CESMRTodoMarkAsDoneTask::~CESMRTodoMarkAsDoneTask()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTodoMarkAsDoneTask::NewL
// ---------------------------------------------------------------------------
//
CESMRTodoMarkAsDoneTask* CESMRTodoMarkAsDoneTask::NewL( MESMRCalEntry& aEntry, 
        MESMRCalDbMgr& aCalDbMgr )
    {
    FUNC_LOG;
    CESMRTodoMarkAsDoneTask* self = 
        new( ELeave )CESMRTodoMarkAsDoneTask( aEntry, aCalDbMgr );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTodoMarkAsDoneTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRTodoMarkAsDoneTask::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRTodoMarkAsDoneTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRTodoMarkAsDoneTask::ExecuteTaskL()
    {
    FUNC_LOG;

    // Mark to-do as done
    MarkTodoAsDoneL();
    }

// ---------------------------------------------------------------------------
// CESMRTodoMarkAsDoneTask::MarkTodoAsDoneL
// ---------------------------------------------------------------------------
//
void CESMRTodoMarkAsDoneTask::MarkTodoAsDoneL()
    {
    FUNC_LOG;
    
    TTime now;
    now.HomeTime();
    TCalTime caltime;
    caltime.SetTimeLocalL( now );

    // Set to-do as completed
    TRAPD( err, iEntry.Entry().SetCompletedL( ETrue, caltime ) );

    // Update entry time stamp before storing
    iEntry.UpdateTimeStampL();    
    
    if( err == KErrNone )
        {
        // Check storing result
        MESMRUtilsTombsExt::TESMRUtilsDbResult res =
           iCalDbMgr.StoreEntryCondL( iEntry.Entry(), EFalse, ETrue );
        
        // Check that entry can be saved
        if ( MESMRUtilsTombsExt::ECheckedValidNew == res ||
            MESMRUtilsTombsExt::ECheckedValidUpdate == res || 
            iEntry.IsEntryEditedL() )
           {
           // Validate entry before storing it to calendar db
           CCalEntry* validatedEntry = iEntry.ValidateEntryL();
           CleanupStack::PushL( validatedEntry );
        
           iCalDbMgr.StoreEntryCondL( *validatedEntry, EFalse );
           
           CleanupStack::PopAndDestroy( validatedEntry );
           
           iEntry.UpdateEntryAfterStoringL();
           }
        }
    }

// EOF

