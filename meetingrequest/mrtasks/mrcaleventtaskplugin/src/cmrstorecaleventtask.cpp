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

#include "cmrstorecaleventtask.h"
#include "cesmrcaldbmgr.h"
#include "mesmrcalentry.h"

#include <calrrule.h>

#include "emailtrace.h"

namespace { // codescanner::namespace

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRStoreCalEventTask::CESMRStoreCalEventTask
// ---------------------------------------------------------------------------
//
CESMRStoreCalEventTask::CESMRStoreCalEventTask( MESMRCalEntry& aEntry, 
                                                MESMRCalDbMgr& aCalDbMgr )
:   iEntry( aEntry ), 
    iCalDbMgr( aCalDbMgr )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRStoreCalEventTask::~CESMRStoreCalEventTask
// ---------------------------------------------------------------------------
//
CESMRStoreCalEventTask::~CESMRStoreCalEventTask()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRStoreCalEventTask::NewL
// ---------------------------------------------------------------------------
//
CESMRStoreCalEventTask* CESMRStoreCalEventTask::NewL( 
        MESMRCalEntry& aEntry, 
        MESMRCalDbMgr& aCalDbMgr )
    {
    FUNC_LOG;
    CESMRStoreCalEventTask* self = 
        new (ELeave) CESMRStoreCalEventTask( aEntry, aCalDbMgr );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRStoreCalEventTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRStoreCalEventTask::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRStoreCalEventTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRStoreCalEventTask::ExecuteTaskL()
    {
    FUNC_LOG;

    // Store to calendar DB
    StoreCalEventToDbL();
    }

// ---------------------------------------------------------------------------
// CESMRStoreCalEventTask::StoreEntryToDbL
// ---------------------------------------------------------------------------
//
void CESMRStoreCalEventTask::StoreCalEventToDbL()
    {
    FUNC_LOG;
    
    // Update entry time stamp before storing
    iEntry.UpdateTimeStampL();
    
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

// EOF

