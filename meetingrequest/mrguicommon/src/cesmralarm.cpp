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
* Description:  ESMR alarm implementation
*
*/

#include "cesmralarm.h"

#include <StringLoader.h>

// DEBUG
#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAlarm::CESMRAlarm
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRAlarm::CESMRAlarm( )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAlarm::~CESMRAlarm
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRAlarm::~CESMRAlarm( )
    {
    FUNC_LOG;
    delete iText;
    }

// ---------------------------------------------------------------------------
// CESMRAlarm::ConstructFromResourceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAlarm::ConstructFromResourceL( TResourceReader& aReader )
    {
    FUNC_LOG;
    iId = aReader.ReadInt16();
    iValue = aReader.ReadInt16();
    iRelative = aReader.ReadInt16();
    iTxtLink = aReader.ReadInt32(); // link to TBUF
    }

// ---------------------------------------------------------------------------
// CESMRAlarm::LoadTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAlarm::LoadTextL( CCoeEnv* aEnv )
    {
    FUNC_LOG;
    delete iText;
    iText = NULL;
    
    if ( iValue != KErrNotFound )
        {
        iText = StringLoader::LoadL ( iTxtLink, iValue, aEnv );
        }
    else
        {
        iText = StringLoader::LoadL ( iTxtLink, aEnv );
        }
    }

// ---------------------------------------------------------------------------
// CESMRAlarm::RelativeTimeInMinutes
// ---------------------------------------------------------------------------
//
EXPORT_C TTimeIntervalMinutes CESMRAlarm::RelativeTimeInMinutes( )
    {
    FUNC_LOG;
    return TTimeIntervalMinutes ( iRelative );
    }

// ---------------------------------------------------------------------------
// CESMRAlarm::Text
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CESMRAlarm::Text( )
    {
    FUNC_LOG;
    return iText;
    }

// ---------------------------------------------------------------------------
// CESMRAlarm::Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRAlarm::Id( )
    {
    FUNC_LOG;
    return iId;
    }

// ---------------------------------------------------------------------------
// CESMRAlarm::Relative
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRAlarm::Relative( )
    {
    FUNC_LOG;
    return iRelative;
    }

// EOF

