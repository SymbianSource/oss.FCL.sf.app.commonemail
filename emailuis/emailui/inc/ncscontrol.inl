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
* Description:  Inline method implementations
*
*/


// ---------------------------------------------------------------------------
// CNcsLabel::LineCount
// ---------------------------------------------------------------------------
//
inline TInt CNcsLabel::LineCount() const
    {
    return 1;    
    }   

// ---------------------------------------------------------------------------
// CNcsLabel::ScrollableLines
// ---------------------------------------------------------------------------
//
inline TInt CNcsLabel::ScrollableLines() const
    {
    return 1;
    }

// ---------------------------------------------------------------------------
// CNcsLabel::GetNumChars
// ---------------------------------------------------------------------------
//
inline TInt CNcsLabel::GetNumChars() const
    {
    return 1;
    }

// ---------------------------------------------------------------------------
// CNcsLabel::CursorLineNumber
// ---------------------------------------------------------------------------
//
inline TInt CNcsLabel::CursorLineNumber() const
    {
    return 1;
    }

// ---------------------------------------------------------------------------
// CNcsLabel::CursorPosition
// ---------------------------------------------------------------------------
//
inline TInt CNcsLabel::CursorPosition() const
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CNcsLabel::GetLabelText
// ---------------------------------------------------------------------------
//
inline const TDesC& CNcsLabel::GetLabelText() const
    {
    return *Text();
    }
