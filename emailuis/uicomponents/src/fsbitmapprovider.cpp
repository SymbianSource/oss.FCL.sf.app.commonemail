/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFsBitmapProvider.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <e32std.h>
#include <eikenv.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag 
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag 
#include <imageconversion.h>
#include "fsbitmapprovider.h"

// ============================= LOCAL FUNCTIONS  =============================

// ============================= MEMBER FUNCTIONS =============================
    
// ----------------------------------------------------------------------------
// CFsBitmapProvider::NewL
// Symbian 2-phased constructor
// (static, may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsBitmapProvider* CFsBitmapProvider::NewL( const CFbsBitmap* aBitmap,
                                            const CFbsBitmap* aBitmapMask,
                                            const TInt aId
                                          )
    {
    FUNC_LOG;
    CFsBitmapProvider* provider = new (ELeave) CFsBitmapProvider( aId );
    CleanupStack::PushL( provider );
    provider->ConstructL( aBitmap,
                          aBitmapMask
                        );
    CleanupStack::Pop( provider );
    
    return provider;
    }
                                          
// ----------------------------------------------------------------------------
// CFsBitmapProvider::~CFsBitmapProvider
// C++ Destructor
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsBitmapProvider::~CFsBitmapProvider()
    {
    FUNC_LOG;
    return;
    }
            
// ----------------------------------------------------------------------------
// CFsBitmapProvider::CFsBitmapProvider
// C++ Constructor
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsBitmapProvider::CFsBitmapProvider( const TInt aId )
    : iBitmap( NULL ),
      iBitmapMask( NULL ),
      iId( aId )
    {
    FUNC_LOG;
    return;
    }

// ----------------------------------------------------------------------------
// CFsBitmapProvider::ConstructL
// 2nd phase constructor
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsBitmapProvider::ConstructL( const CFbsBitmap* aBitmap, 
                                    const CFbsBitmap* aBitmapMask
                                  )
    {
    FUNC_LOG;
    if ( aBitmap )
        {
        iBitmap = new (ELeave) CFbsBitmap;
        iBitmap->Duplicate( aBitmap->Handle() );
        }
    
    if ( aBitmapMask )
        {
        iBitmapMask = new (ELeave) CFbsBitmap;
        iBitmapMask->Duplicate( aBitmapMask->Handle() );
        }    
    
    return;
    }

// ----------------------------------------------------------------------------
// CFsBitmapProvider::ProvideBitmapL
// Interface callback
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsBitmapProvider::ProvideBitmapL( TInt aId, 
                                        CFbsBitmap*& aBitmap, 
                                        CFbsBitmap*& aMaskBitmap
                                      )
    {
    FUNC_LOG;
    User::LeaveIfError( aId == iId ? KErrNone : KErrArgument );
    
    aBitmap = iBitmap;
    aMaskBitmap = iBitmapMask;
    
    return;
    }

// ----------------------------------------------------------------------------
// CFsBitmapProvider::Id
// Return provider Id
// Status : Draft
// ----------------------------------------------------------------------------
//
TInt CFsBitmapProvider::Id() const
    {
    FUNC_LOG;
    return iId;
    }

// ========================= OTHER EXPORTED FUNCTIONS =========================

// End of file

