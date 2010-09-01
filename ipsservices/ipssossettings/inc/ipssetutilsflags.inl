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
* Description:  Definitions for flag operations.
*
*/


// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::SetFlag()
// ----------------------------------------------------------------------------
//
inline void TIpsSetUtilsFlags::SetFlag( const TUint64 aFlag )
    {
    iFlags |= aFlag;
    }


// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::ClearFlag()
// ----------------------------------------------------------------------------
//
inline void TIpsSetUtilsFlags::ClearFlag( const TUint64 aFlag )
    {
    iFlags &= ~aFlag;
    }

// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::Flag()
// ----------------------------------------------------------------------------
//
inline TBool TIpsSetUtilsFlags::Flag( const TUint64 aFlag ) const
    {
    return ( iFlags & aFlag ) > 0;
    }

// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::SetFlag32()
// ----------------------------------------------------------------------------
//
inline void TIpsSetUtilsFlags::SetFlag32( const TUint32 aFlag )
    {
    iFlags |= MAKE_TUINT64( aFlag, 0 );
    }


// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::ClearFlag32()
// ----------------------------------------------------------------------------
//
inline void TIpsSetUtilsFlags::ClearFlag32( const TUint32 aFlag )
    {
    // Set the flag that are above the 32-bit space.
    iFlags &= ~MAKE_TUINT64( aFlag, 0 );
    }

// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::Flag32()
// ----------------------------------------------------------------------------
//
inline TBool TIpsSetUtilsFlags::Flag32( const TUint32 aFlag ) const
    {
    // Check the flag exists and return the value as boolean.
    return ( iFlags & MAKE_TUINT64( aFlag, 0 ) ) > 0;
    }
        
// ----------------------------------------------------------------------------
// TIpsSetUtilsFlags::ChangeFlag()
// ----------------------------------------------------------------------------
//
inline void TIpsSetUtilsFlags::ChangeFlag(
    const TUint64 aFlag,
    const TBool aNewState )
    {
    if ( aNewState )
        {
        SetFlag( aFlag );
        }
    else
        {
        ClearFlag( aFlag );
        }
    }    
    

// End of File
