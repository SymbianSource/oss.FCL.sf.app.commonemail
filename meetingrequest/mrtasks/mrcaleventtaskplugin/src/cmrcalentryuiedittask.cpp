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

#include "cmrcalentryuiedittask.h"

#include "emailtrace.h"

namespace { // codescanner::namespace

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCalEntryUiEditTask::CESMRCalEntryUiEditTask
// ---------------------------------------------------------------------------
//
CESMRCalEntryUiEditTask::CESMRCalEntryUiEditTask()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiEditTask::~CESMRCalEntryUiEditTask
// ---------------------------------------------------------------------------
//
CESMRCalEntryUiEditTask::~CESMRCalEntryUiEditTask()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiEditTask::NewL
// ---------------------------------------------------------------------------
//
CESMRCalEntryUiEditTask* CESMRCalEntryUiEditTask::NewL( MESMRCalEntry& /*aEntry*/ )
    {
    FUNC_LOG;
    CESMRCalEntryUiEditTask* self = new (ELeave) CESMRCalEntryUiEditTask();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiEditTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCalEntryUiEditTask::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiEditTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRCalEntryUiEditTask::ExecuteTaskL()
    {
    FUNC_LOG;

    // Edit calendar event
    EditCalEventL();
    }

// ---------------------------------------------------------------------------
// CESMRCalEntryUiEditTask::EditCalEventL
// ---------------------------------------------------------------------------
//
void CESMRCalEntryUiEditTask::EditCalEventL()
    {
    FUNC_LOG;

    }

// EOF

