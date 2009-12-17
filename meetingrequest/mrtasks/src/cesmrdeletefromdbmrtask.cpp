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
#include "cesmrdeletefromdbmrtask.h"

#include <calentry.h>
#include <calinstance.h>
#include <calinstanceview.h>
//<cmail>
#include "mesmrutilstombsext.h"
//</cmail>
#include "cesmrcaldbmgr.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrentryhelper.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRDeleteMRFromDbTask::CESMRDeleteMRFromDbTask
// ---------------------------------------------------------------------------
//
CESMRDeleteMRFromDbTask::CESMRDeleteMRFromDbTask(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils )
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRDeleteMRFromDbTask::~CESMRDeleteMRFromDbTask
// ---------------------------------------------------------------------------
//
CESMRDeleteMRFromDbTask::~CESMRDeleteMRFromDbTask()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRDeleteMRFromDbTask::NewL
// ---------------------------------------------------------------------------
//
CESMRDeleteMRFromDbTask* CESMRDeleteMRFromDbTask::NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils )
    {
    FUNC_LOG;
    CESMRDeleteMRFromDbTask* self =
        new (ELeave) CESMRDeleteMRFromDbTask(
            aCalDbMgr,
            aEntry,
            aMRMailboxUtils );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRDeleteMRFromDbTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRDeleteMRFromDbTask::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CESMRDeleteMRFromDbTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRDeleteMRFromDbTask::ExecuteTaskL()
    {
    FUNC_LOG;
    // Store to calendar DB
    DeleteEntryFromDbL();
    }

// ---------------------------------------------------------------------------
// CESMRDeleteMRFromDbTask::DeleteEntryFromDbL
// ---------------------------------------------------------------------------
//
void CESMRDeleteMRFromDbTask::DeleteEntryFromDbL()
    {
    FUNC_LOG;
    
    MESMRCalDbMgr& calDb = CalDbMgr();
    MESMRMeetingRequestEntry& mrEntry = ESMREntry();
    CCalEntry& entry = mrEntry.Entry();

	if ( mrEntry.IsSyncObjectPresent() )
		{
		}
	else
		{
		CCalEntry* entryToDeleted = NULL;
		
		
		if ( mrEntry.IsRecurrentEventL() &&
	    		(MESMRCalEntry::EESMRThisOnly == mrEntry.RecurrenceModRule()) &&
	    		mrEntry.IsOpenedFromMail() )
	    	{
	        entryToDeleted = mrEntry.RemoveInstanceFromSeriesL();
	    	CleanupStack::PushL( entryToDeleted );
	    	calDb.StoreEntryL( *entryToDeleted, ETrue );
	    	
	    	}
	    else
	    	{
	    	entryToDeleted = mrEntry.ValidateEntryL();
	        CleanupStack::PushL( entryToDeleted );
	        // <cmail>
	        TRAPD(err, calDb.DeleteEntryCondL( *entryToDeleted ) );
	        if ( err == KErrNotFound )
	        	{
	        	// If meeting request to be deleted is not found from the calendar
	        	// Do not care about error. This is just for avoiding leave
	        	}
	        // </cmail>
	        }
	    
	    CleanupStack::PopAndDestroy( entryToDeleted );
	    entryToDeleted = NULL;
	    }
	
    }

// EOF

