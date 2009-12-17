/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Custom key definition
*
*/


#include "emailtrace.h"
#include <e32keys.h>

#include "fstextviewerkeys.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CFsTextViewerKeys* CFsTextViewerKeys::NewL()
    {
    FUNC_LOG;
    CFsTextViewerKeys* self = new ( ELeave ) CFsTextViewerKeys();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerKeys
// ---------------------------------------------------------------------------
EXPORT_C CFsTextViewerKeys::~CFsTextViewerKeys()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// SetKeyUp
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyUp( TInt aKey )
    {
    FUNC_LOG;
    iKeyUp = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyDown
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyDown( TInt aKey )
    {
    FUNC_LOG;
    iKeyDown = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyScrollUp
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyScrollUp( TInt aKey )
    {
    FUNC_LOG;
    iKeyScrollUp = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyScrollDown
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyScrollDown( TInt aKey )
    {
    FUNC_LOG;
    iKeyScrollDown = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyRight
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyRight( TInt aKey )
    {
    FUNC_LOG;
    iKeyRight = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyLeft
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyLeft( TInt aKey )
    {
    FUNC_LOG;
    iKeyLeft = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyPgUp
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyPgUp( TInt aKey )
    {
    FUNC_LOG;
    iKeyPgUp = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyPgDown
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyPgDown( TInt aKey )
    {
    FUNC_LOG;
    iKeyPgDown = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyMark
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyMark( TInt aKey )
    {
    FUNC_LOG;
    iKeyMark = aKey;
    }

// ---------------------------------------------------------------------------
// SetKeyClick
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerKeys::SetKeyClick( TInt aKey )
    {
    FUNC_LOG;
    iKeyClick = aKey;
    }

// ---------------------------------------------------------------------------
// GetKeyUp
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyUp()
    {
    FUNC_LOG;
    return iKeyUp;
    }

// ---------------------------------------------------------------------------
// GetKeyDown
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyDown()
    {
    FUNC_LOG;
    return iKeyDown;
    }

// ---------------------------------------------------------------------------
// GetKeyScrollUp
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyScrollUp()
    {
    FUNC_LOG;
    return iKeyScrollUp;
    }

// ---------------------------------------------------------------------------
// GetKeyScrollDown
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyScrollDown()
    {
    FUNC_LOG;
    return iKeyScrollDown;
    }

// ---------------------------------------------------------------------------
// GetKeyRight
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyRight()
    {
    FUNC_LOG;
    return iKeyRight;
    }

// ---------------------------------------------------------------------------
// GetKeyLeft
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyLeft()
    {
    FUNC_LOG;
    return iKeyLeft;
    }

// ---------------------------------------------------------------------------
// GetKeyPgUp
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyPgUp()
    {
    FUNC_LOG;
    return iKeyPgUp;
    }

// ---------------------------------------------------------------------------
// GetKeyPgDown
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyPgDown()
    {
    FUNC_LOG;
    return iKeyPgDown;
    }

// ---------------------------------------------------------------------------
// GetKeyMark
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyMark()
    {
    FUNC_LOG;
    return iKeyMark;
    }

// ---------------------------------------------------------------------------
// GetKeyClick
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewerKeys::GetKeyClick()
    {
    FUNC_LOG;
    return iKeyClick;
    }

// ---------------------------------------------------------------------------
// CFsTextViewerKeys()
// ---------------------------------------------------------------------------
CFsTextViewerKeys::CFsTextViewerKeys()
    {
    FUNC_LOG;
    iKeyUp = EKeyUpArrow;
    iKeyDown = EKeyDownArrow;
    iKeyScrollUp = '2';
    iKeyScrollDown = '8';
    iKeyPgUp = '*';
    iKeyPgDown = '#';
    iKeyRight = EKeyRightArrow;
    iKeyLeft = EKeyLeftArrow;
    iKeyMark = '0';
    iKeyClick = EKeyDevice3;
    }
 
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerKeys::ConstructL()
    {
    FUNC_LOG;
    }

