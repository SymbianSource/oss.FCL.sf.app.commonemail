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
* Description:  ESMR synchronization implementation
 *
*/

#include "emailtrace.h"
#include "cesmrsync.h"

#include <barsread.h>
#include <StringLoader.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSync::CESMRSync
// ---------------------------------------------------------------------------
//
CESMRSync::CESMRSync( )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSync::~CESMRSync
// ---------------------------------------------------------------------------
//
CESMRSync::~CESMRSync( )
    {
    FUNC_LOG;
    delete iText;
    }

// ---------------------------------------------------------------------------
// CESMRSync::ConstructFromResourceL
// ---------------------------------------------------------------------------
//
void CESMRSync::ConstructFromResourceL( TResourceReader& aReader )
    {
    FUNC_LOG;
    iId = aReader.ReadInt16 ( );
    TInt textId = aReader.ReadInt32 ( );
    iText = StringLoader::LoadL ( textId );
    }

// ---------------------------------------------------------------------------
// CESMRSync::TextL
// ---------------------------------------------------------------------------
//
TDesC& CESMRSync::TextL( )
    {
    FUNC_LOG;
    return *iText;
    }

// ---------------------------------------------------------------------------
// CESMRSync::Id
// ---------------------------------------------------------------------------
//
TESMRSyncValue CESMRSync::Id( )
    {
    FUNC_LOG;
    return static_cast<TESMRSyncValue>( iId );
    }

// EOF

