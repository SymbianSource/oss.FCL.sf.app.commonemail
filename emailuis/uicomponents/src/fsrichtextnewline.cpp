/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CFsRichTextNewLine
*
*/


#include "emailtrace.h"
#include "fsrichtextnewline.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFsRichTextNewLine* CFsRichTextNewLine::NewL()
    {
    FUNC_LOG;
    CFsRichTextNewLine* self = new ( ELeave ) CFsRichTextNewLine();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsRichTextText
// ---------------------------------------------------------------------------
//
CFsRichTextNewLine::~CFsRichTextNewLine()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CFsRichTextNewLine::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsRichTextText
// ---------------------------------------------------------------------------
//
CFsRichTextNewLine::CFsRichTextNewLine()
    {
    FUNC_LOG;
    iType = EFsNewLine; 
    }




