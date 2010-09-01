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
#include "cesmrstoremrtask.h"

#include <calentry.h>
#include <calinstance.h>
//<cmail>
#include "mesmrutilstombsext.h"
//</cmail>
#include "cesmrcaldbmgr.h"
#include "mesmrmeetingrequestentry.h"

namespace { // codescanner::namespace

#ifdef _DEBUG

// Literal for panics
_LIT( KESMRStoreMR, "ESMRStoreMR" );

// Panic codes
enum TESMRStoreMRPanic
    {
    EESMRStoreMRNotMeetingRequest // Entry is not meeting request
    };

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Panic wrapper method
// ---------------------------------------------------------------------------
//
void Panic( TESMRStoreMRPanic aPanic )
    {
    User::Panic( KESMRStoreMR, aPanic );
    }

#endif // _DEBUG

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRStoreMRTask::CESMRStoreMRTask
// ---------------------------------------------------------------------------
//
CESMRStoreMRTask::CESMRStoreMRTask(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils )
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRStoreMRTask::~CESMRStoreMRTask
// ---------------------------------------------------------------------------
//
CESMRStoreMRTask::~CESMRStoreMRTask()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRStoreMRTask::NewL
// ---------------------------------------------------------------------------
//
CESMRStoreMRTask* CESMRStoreMRTask::NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils )
    {
    FUNC_LOG;

    CESMRStoreMRTask* self =
        new (ELeave) CESMRStoreMRTask(
            aCalDbMgr,
            aEntry,
            aMRMailboxUtils );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRStoreMRTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRStoreMRTask::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CESMRStoreMRTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRStoreMRTask::ExecuteTaskL()
    {
    FUNC_LOG;

    // Store to calendar DB
    StoreEntryToDbL();

    }

// ---------------------------------------------------------------------------
// CESMRStoreMRTask::StoreEntryToDbL
// ---------------------------------------------------------------------------
//
void CESMRStoreMRTask::StoreEntryToDbL()
    {
    FUNC_LOG;

    MESMRMeetingRequestEntry& mrEntry = ESMREntry();
    CCalEntry& entry = mrEntry.Entry();

    const CCalEntry::TType KMRType(
            CCalEntry::EAppt );

    CCalEntry::TType entryType( entry.EntryTypeL() );

    // Check entry type
#ifdef _DEBUG

    __ASSERT_DEBUG(
            KMRType == entryType,
            Panic(EESMRStoreMRNotMeetingRequest) );

#else

    if ( KMRType != entryType )
        {
        User::Leave( KErrArgument );
        }

#endif // _DEBUG

    TBool isEdited( mrEntry.IsEntryEditedL() );
    TBool forwarded( mrEntry.IsForwardedL() );
    
    if ( !forwarded )
    	{
    	if ( isEdited )
    	    {
    	    // Entry is edited --> We will update time stamp
    	    mrEntry.UpdateTimeStampL();
    	    }
    	
		// Check storing result
		MESMRUtilsTombsExt::TESMRUtilsDbResult res =
				CalDbMgr().StoreEntryCondL( entry, EFalse, ETrue );
	
		// Check that entry can be saved
		if ( MESMRUtilsTombsExt::ECheckedValidNew == res ||
			 MESMRUtilsTombsExt::ECheckedValidUpdate == res || 
			 isEdited )
			{
			CCalEntry* validatedEntry = mrEntry.ValidateEntryL();
			CleanupStack::PushL( validatedEntry );
			
			CalDbMgr().StoreEntryCondL( *validatedEntry, EFalse );
			
			CleanupStack::PopAndDestroy( validatedEntry );
			
			mrEntry.UpdateEntryAfterStoringL();
			}
    	}

    }

// EOF

