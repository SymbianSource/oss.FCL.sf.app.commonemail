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
* Description: This file implements class TIpsSetUtilsFlags.
*
*/


#include "emailtrace.h"
#include <e32base.h>        

#include "ipssetutilsflags.h"

// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::TIpsSetUtilsFlags()
// ----------------------------------------------------------------------------
//
TIpsSetUtilsFlags::TIpsSetUtilsFlags( const TUint64 aFlags ) 
    : 
    iFlags( aFlags )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::operator=()
// ----------------------------------------------------------------------------
//
TIpsSetUtilsFlags& TIpsSetUtilsFlags::operator=( 
    const TIpsSetUtilsFlags& aFlags )
    {
    FUNC_LOG;
    iFlags = aFlags.iFlags;
       
    return *this;
    }
    
// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::operator=()
// ----------------------------------------------------------------------------
//
TIpsSetUtilsFlags& TIpsSetUtilsFlags::operator=( const TUint64 aFlags )
    {
    FUNC_LOG;
    iFlags = aFlags;
       
    return *this;
    }    

// ---------------------------------------------------------------------------
// TIpsSetUtilsFlags::ValueForFlag()
// ---------------------------------------------------------------------------
//
TInt TIpsSetUtilsFlags::ValueForFlag(
    const TUint32 aFlag,
    const TInt aTrue,
    const TInt aFalse ) const
    {
    FUNC_LOG;
    if ( iFlags & MAKE_TUINT64( aFlag, 0 ) )
        {
        return aTrue;
        }
    else
        {
        return aFalse;
        }
    }


// End of File

