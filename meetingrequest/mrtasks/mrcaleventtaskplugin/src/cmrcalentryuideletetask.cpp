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

#include "cmrcalentryuideletetask.h"
#include "cesmrcaldbmgr.h"
#include "mesmrcalentry.h"

#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCalEntryUiDeleteTask::CESMRCalEntryUiDeleteTask
// ---------------------------------------------------------------------------
//
CESMRCalEntryUiDeleteTask::CESMRCalEntryUiDeleteTask( MESMRCalEntry& aEntry, 
        MESMRCalDbMgr& aCalDbMgr )
:   iEntry( aEntry ), 
    iCalDbMgr( aCalDbMgr )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiDeleteTask::~CESMRCalEntryUiDeleteTask
// ---------------------------------------------------------------------------
//
CESMRCalEntryUiDeleteTask::~CESMRCalEntryUiDeleteTask()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiDeleteTask::NewL
// ---------------------------------------------------------------------------
//
CESMRCalEntryUiDeleteTask* CESMRCalEntryUiDeleteTask::NewL( 
        MESMRCalEntry& aEntry, 
        MESMRCalDbMgr& aCalDbMgr )
    {
    FUNC_LOG;
    CESMRCalEntryUiDeleteTask* self = 
        new (ELeave) CESMRCalEntryUiDeleteTask( aEntry, aCalDbMgr );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiDeleteTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCalEntryUiDeleteTask::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiDeleteTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRCalEntryUiDeleteTask::ExecuteTaskL()
    {
    FUNC_LOG;

    // Delete from calendar DB
    DeleteEntryFromDbL();
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiDeleteTask::DeleteEntryFromDbL
// ---------------------------------------------------------------------------
//
void CESMRCalEntryUiDeleteTask::DeleteEntryFromDbL()
    {
    FUNC_LOG;
    CCalEntry* entryToBeDeleted = iEntry.ValidateEntryL();
    CleanupStack::PushL( entryToBeDeleted );

    TRAPD( err, iCalDbMgr.DeleteEntryCondL( *entryToBeDeleted ) );
    if ( err != KErrNone )
        {
        #ifdef _DEBUG
        User::LeaveIfError( err );
        #endif // _DEBUG
        }
    
    CleanupStack::PopAndDestroy( entryToBeDeleted );
    entryToBeDeleted = NULL;    
    }

// EOF

