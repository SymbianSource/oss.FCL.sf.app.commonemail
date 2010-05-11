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
* Description:  Helper class for recurrence text
*
*/

#include "emailtrace.h"
#include <StringLoader.h>
#include "cesmrrecurrence.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRRecurrence::CESMRRecurrence()
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRecurrence::CESMRRecurrence()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRRecurrence::~CESMRRecurrence()
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRecurrence::~CESMRRecurrence()
    {
    FUNC_LOG;
    delete iTxt;
    }

// ---------------------------------------------------------------------------
// CESMRRecurrence::ConstructFromResourceL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRecurrence::ConstructFromResourceL( 
		CCoeEnv* aEnv, 
		TResourceReader& aReader )
    {
    FUNC_LOG;
    iId = static_cast<TESMRRecurrenceValue>(aReader.ReadInt16()); // WORD
    TInt llink = aReader.ReadInt32(); // LLINK
    iTxt = StringLoader::LoadL( llink, aEnv ); // load text from TBUF llink
    }

// ---------------------------------------------------------------------------
// CESMRRecurrence::RecurrenceValue()
// ---------------------------------------------------------------------------
//
EXPORT_C TESMRRecurrenceValue CESMRRecurrence::RecurrenceValue()
    {
    FUNC_LOG;
    return iId;
    }

// ---------------------------------------------------------------------------
// CESMRRecurrence::RecurrenceText()
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC& CESMRRecurrence::RecurrenceText()
    {
    FUNC_LOG;
    return *iTxt;
    }

// EOF

