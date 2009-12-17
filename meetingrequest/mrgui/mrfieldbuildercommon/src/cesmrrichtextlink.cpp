/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for CESMRRichTextViewer links
*
*/


#include "emailtrace.h"
#include "cesmrrichtextlink.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRRichTextLink::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRichTextLink* CESMRRichTextLink::NewL(
        TInt aStartPos,
        TInt aLength,
        const TDesC& aValue,
        TType aType,
        TTriggerKey aTriggerKey )
    {
    FUNC_LOG;
    CESMRRichTextLink* self = 
		new (ELeave) CESMRRichTextLink(aStartPos, aLength, aType, aTriggerKey);
    CleanupStack::PushL (self );
    self->ConstructL (aValue );
    CleanupStack::Pop (self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::~CESMRRichTextLink
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRichTextLink::~CESMRRichTextLink( )
    {
    FUNC_LOG;
    delete iValue;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::StartPos
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRRichTextLink::StartPos( ) const
    {
    FUNC_LOG;
    return iStartPos;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::Length
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRRichTextLink::Length( ) const
    {
    FUNC_LOG;
    return iLength;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::Value
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CESMRRichTextLink::Value( ) const
    {
    FUNC_LOG;
    return *iValue;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::TType
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRichTextLink::TType CESMRRichTextLink::Type( ) const
    {
    FUNC_LOG;
    return iType;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::TTriggerKey
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRichTextLink::TTriggerKey CESMRRichTextLink::TriggerKey( ) const
    {
    FUNC_LOG;
    return iTriggerKey;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::SetStartPos
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextLink::SetStartPos( TInt aPos )
    {
    FUNC_LOG;
    iStartPos = aPos;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::CESMRRichTextLink
// ---------------------------------------------------------------------------
//
CESMRRichTextLink::CESMRRichTextLink(
		TInt aStartPos, 
		TInt aLength, 
		TType aType,
        TTriggerKey aTriggerKey )
    {
    FUNC_LOG;
    iStartPos = aStartPos;
    iLength = aLength;
    iType = aType;
    iTriggerKey = aTriggerKey;
    }

// ---------------------------------------------------------------------------
// CESMRRichTextLink::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRRichTextLink::ConstructL(const TDesC& aValue )
    {
    FUNC_LOG;
    iValue = aValue.AllocL();
    }

// EOF

