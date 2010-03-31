/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ESMR priority implementation
 *
*/


#include "emailtrace.h"
#include "cesmrpriority.h"
#include <barsread.h>
#include <stringloader.h>

// ---------------------------------------------------------------------------
// CESMRPriority::CESMRPriority
// ---------------------------------------------------------------------------
//
CESMRPriority::CESMRPriority( )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRPriority::~CESMRPriority
// ---------------------------------------------------------------------------
//
CESMRPriority::~CESMRPriority( )
    {
    FUNC_LOG;
    delete iText;
    }

// ---------------------------------------------------------------------------
// CESMRPriority::ConstructFromResourceL
// ---------------------------------------------------------------------------
//
void CESMRPriority::ConstructFromResourceL( TResourceReader& aReader )
    {
    FUNC_LOG;
    iId = aReader.ReadInt16 ( );
    TInt textId = aReader.ReadInt32 ( );
    iText = StringLoader::LoadL ( textId );
    }

// ---------------------------------------------------------------------------
// CESMRPriority::Text
// ---------------------------------------------------------------------------
//
const TDesC& CESMRPriority::Text( )
    {
    FUNC_LOG;
    return *iText;
    }

// ---------------------------------------------------------------------------
// CESMRPriority::Id
// ---------------------------------------------------------------------------
//
TFSCalenMRPriority CESMRPriority::Id( )
    {
    FUNC_LOG;
    return static_cast<TFSCalenMRPriority>( iId );
    }

// EOF

