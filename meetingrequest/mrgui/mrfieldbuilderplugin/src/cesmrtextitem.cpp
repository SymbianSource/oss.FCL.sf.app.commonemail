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
* Description:  ESMR text item implementation, used to map text resource to id
 *
*/

#include "emailtrace.h"
#include "cesmrtextitem.h"

#include <barsread.h>
#include <StringLoader.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ESMRTextItem::CESMRTextItem
// ---------------------------------------------------------------------------
//
CESMRTextItem::CESMRTextItem( )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTextItem::~CESMRTextItem
// ---------------------------------------------------------------------------
//
CESMRTextItem::~CESMRTextItem( )
    {
    FUNC_LOG;
    delete iText;
    }

// ---------------------------------------------------------------------------
// CESMRTextItem::ConstructFromResourceL
// ---------------------------------------------------------------------------
//
void CESMRTextItem::ConstructFromResourceL( TResourceReader& aReader )
    {
    FUNC_LOG;
    iId = aReader.ReadInt16 ( );
    TInt textId = aReader.ReadInt32 ( );
    iText = StringLoader::LoadL ( textId );
    }

// ---------------------------------------------------------------------------
// CESMRTextItem::TextL
// ---------------------------------------------------------------------------
//
TDesC& CESMRTextItem::TextL( )
    {
    FUNC_LOG;
    return *iText;
    }

// ---------------------------------------------------------------------------
// CESMRTextItem::Id
// ---------------------------------------------------------------------------
//
TInt CESMRTextItem::Id( )
    {
    FUNC_LOG;
    return iId;
    }

// EOF

