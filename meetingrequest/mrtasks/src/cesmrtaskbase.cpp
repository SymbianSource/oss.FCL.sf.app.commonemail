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
#include "cesmrtaskbase.h"

#include <calentry.h>
#include "mesmrmeetingrequestentry.h"
#include "cesmrcaldbmgr.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTaskBase::CESMRTaskBase
// ---------------------------------------------------------------------------
//
CESMRTaskBase::CESMRTaskBase(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils )
:   iCalDbMgr(aCalDbMgr),
    iEntry(aEntry),
    iMRMailboxUtils( aMRMailboxUtils )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTaskBase::~CESMRTaskBase
// ---------------------------------------------------------------------------
//
CESMRTaskBase::~CESMRTaskBase()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTaskBase::BaseConstructL
// ---------------------------------------------------------------------------
//
void CESMRTaskBase::BaseConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

