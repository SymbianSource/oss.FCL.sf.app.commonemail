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
* Description:  Panic helper functions for fs_generic.dll
*
*/


#include "emailtrace.h"
#include <e32std.h> // User::Panic

#include "fsgenericpanic.h" // TFsGenericPanicReason


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Panics the application with the requested panic code
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void FsGenericPanic( const TFsGenericPanicReason aReason )
    {
    /**
     * Panic category for User::Panic.
     */
    _LIT( KFsGenericPanicCategory, "Fs Generic" );

    User::Panic( KFsGenericPanicCategory, aReason );
    }
#else // !_DEBUG
void FsGenericPanic( const TFsGenericPanicReason )
    {
    FUNC_LOG;
    }
#endif // _DEBUG


