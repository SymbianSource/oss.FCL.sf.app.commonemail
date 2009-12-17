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
* Description: This file implements class CFsTexture.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <e32std.h>
#include <fbs.h>
#include <bitmaptransforms.h>
#include "fsbitmapprovider.h"
#include "fstexture.h"
#include "fstextureloader.h"

// CONSTANTS

// ============================= LOCAL FUNCTIONS  =============================

// ----------------------------------------------------------------------------
// TimerCallbackL
// Timeout callback
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
TInt TimerCallbackL( TAny* aModel )
    {
    static_cast<CFsTexture*>( aModel )->TimerExpiredL();

    return 1;
    }  

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CFsTexture::NewL
// Two-phased constructor
// (static, may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//

CFsTexture* CFsTexture::NewL( CAlfTexture& aTexture,
                              MFsTextureLoaderObserver& aObserver,
                              CFsBitmapProvider* aProvider,
                              const TInt aId
                            )

    {
    FUNC_LOG;
    CFsTexture* pic = new (ELeave) CFsTexture( aTexture, 
                                               aObserver, 
                                               aProvider,
                                               aId
                                             );
    CleanupStack::PushL( pic );
    pic->ConstructL();
    CleanupStack::Pop( pic );
    
    return pic;
    }

// ----------------------------------------------------------------------------
// CFsTexture::CFsTexture
// C++ Constructor
// Status : Draft
// ----------------------------------------------------------------------------
// 

CFsTexture::CFsTexture( CAlfTexture& aTexture,
                        MFsTextureLoaderObserver& aObserver,
                        CFsBitmapProvider* aProvider,
                        const TInt aId
                      )

    : iTexture( aTexture ),
      iObserver( aObserver ),
      iProvider( aProvider ),
      iId( aId )     
    {
    FUNC_LOG;
    return;
    }                         
// ----------------------------------------------------------------------------
// CFsTexture::~CFsTexture
// C++ Destructor
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTexture::~CFsTexture()
    {
    FUNC_LOG;
    StopTimer();

    iObserver.Deleted( *this );
    
    if ( iProvider )
        {
        delete iProvider;
        iProvider = NULL;
        }
    
    return;
    }

// ----------------------------------------------------------------------------
// CFsTexture::Pos
// Return bitmap position, internal for the application
// Status : Draft
// ----------------------------------------------------------------------------
//
TInt CFsTexture::Pos() const
    {
    FUNC_LOG;
    return iPos;
    }

// ----------------------------------------------------------------------------
// CFsTexture::SetPos
// Set bitmap position, internal for the application
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTexture::SetPos( const TInt& aPos )
    {
    FUNC_LOG;
    iPos = aPos;
    iObserver.PosUpdated( *this );
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTexture::TextSize
// Return current size
// Status : Draft
// ----------------------------------------------------------------------------
//
TSize CFsTexture::TextSize() const
    {
    FUNC_LOG;
    return iTextSize;
    }

// ----------------------------------------------------------------------------
// CFsTexture::SetTextSize
// Set new size
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTexture::SetTextSize( const TSize& aSize )
    {
    FUNC_LOG;
    iTextSize = aSize;
    iObserver.SizeUpdated( *this );    
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTexture::TextFlags
// Return flag status
// Status : Draft
// ----------------------------------------------------------------------------
//
TInt CFsTexture::TextFlags() const
    {
    FUNC_LOG;
    return iTextFlags;
    }

// ----------------------------------------------------------------------------
// CFsTexture::SetTextFlags
// Set new flag status
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTexture::SetTextFlags( const TInt& aFlags )
    {
    FUNC_LOG;
    iTextFlags = aFlags;
    iObserver.FlagsUpdated( *this );
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTexture::StopTimer
// Stop unload timer and free timer resources
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTexture::StopTimer()
    {
    FUNC_LOG;
	if ( iUnloadTimer )
        {
	    if ( iUnloadTimer->IsActive() )
            {
            iUnloadTimer->Cancel();
            }
        delete iUnloadTimer;
        iUnloadTimer = NULL;
        }
    
    return;
    }

// ----------------------------------------------------------------------------
// CFsTexture::TimerExpiredL
// Timeout callback
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTexture::TimerExpiredL()
    {
    FUNC_LOG;
    StopTimer();
    
    iObserver.TimerExpiredL( *this );

    return;
    }    
    
// ----------------------------------------------------------------------------
// CFsTexture::Texture
// Return refence of Alf/Hitchock texture
// Status : Draft
// ----------------------------------------------------------------------------
//
CAlfTexture& CFsTexture::Texture()
    {
    FUNC_LOG;
    return iTexture;
    }
    
// ----------------------------------------------------------------------------
// CFsTexture::ConstructL
// 2nd phase contructor
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTexture::ConstructL( )
    {
    FUNC_LOG;
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTexture::UnloadL
// Unload texture, or cancel unload
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTexture::UnloadL( const TBool& aCancel )
    {
    FUNC_LOG;
    if ( aCancel )
        {
        if ( iUnloadTimer )
            {
            StopTimer();
            } // iUnloadTimer
        }
    else // aCancel
        {
        // Get current default
        TTimeIntervalMicroSeconds32 timeOut;
        iObserver.DefaultTimeOut( timeOut );
    
        if ( timeOut == TTimeIntervalMicroSeconds32( 0 ) )
            {
            TimerExpiredL();
            } // timeOut
        else if ( !iUnloadTimer )
            {
	        iUnloadTimer = CPeriodic::NewL( CActive::EPriorityStandard );

            // Start timer
            iUnloadTimer->Start( timeOut, 
                                 timeOut,
                                 TCallBack( TimerCallbackL, 
                                            this
                                          )
                               );        
            }
        else
            {
            // Left for Lint
            }
        } // !aCancel
    
    return;
    }    

// ----------------------------------------------------------------------------
// CFsTexture::Provider
// Return bitmap provider
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsBitmapProvider* CFsTexture::Provider()
    {
    FUNC_LOG;
    return iProvider;
    }

// ----------------------------------------------------------------------------
// CFsTexture::Id
// Return provider Id
// Status : Draft
// ----------------------------------------------------------------------------
//
TInt CFsTexture::Id() const
    {
    FUNC_LOG;
    return iId;
    } 

// ========================= OTHER EXPORTED FUNCTIONS =========================

// End of file

