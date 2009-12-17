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
* Description: This file implements class CFsTextureLoader.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <e32std.h>
#include <eikenv.h> 
#include <imageconversion.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag 
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag 

#include "fstexture.h"
#include "fsbitmapprovider.h"
#include "fstextureloader.h"

// CONSTANTS
const TInt KGranularity = 5;

// ============================= LOCAL FUNCTIONS  =============================

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CFsTextureLoader::TView::operator= ( const CFsTextureLoader::TView& )
// Operator =
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTextureLoader::TView& CFsTextureLoader::TView::operator=( const CFsTextureLoader::TView& aView )
    {
    FUNC_LOG;
    iStart = aView.iStart;
    iLength = aView.iLength;
    
    return *this;
    }    

// ----------------------------------------------------------------------------
// CFsTextureLoader::NewL
// Two-phased constructor
// (static, may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//

CFsTextureLoader* CFsTextureLoader::NewL( CAlfTextureManager& aManager,
                                          MFsTextureObserver& aObserver
                                        )

    {
    FUNC_LOG;
    CFsTextureLoader* loader = new (ELeave) CFsTextureLoader( aManager, 
                                                              aObserver 
                                                            );
    CleanupStack::PushL( loader );
    loader->ConstructL();
    CleanupStack::Pop( loader );
    
    return loader;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::~CFsTextureLoader
// C++ Destructor
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTextureLoader::~CFsTextureLoader()
    {
    FUNC_LOG;
    SetDefaultTimeOut( TTimeIntervalMicroSeconds32( 0 ) );
    

    iManager.RemoveLoadObserver( this );

   
    if ( iTextures )
        {
        const TInt count = iTextures->Count();
        for ( TInt i = count - 1; i >= 0; --i )
            {
            delete iTextures->At( i );
            }
        delete iTextures;
        iTextures = NULL;
        }
   
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::TextureLoadingCompleted
// Loader callback
// Status : Draft
// ----------------------------------------------------------------------------
//

void CFsTextureLoader::TextureLoadingCompleted( CAlfTexture& aTexture,
                                                TInt /* aTextureId */,
                                                TInt /* aErrorCode */
                                              )

    {
    FUNC_LOG;
    TInt index = 0;

    HBufC* fn = aTexture.FileName();
    TFileName fileName;
    if ( fn )
        {
        fileName.Copy( fn->Des() );
        }
    else
        {
        fileName.Zero();
        }
    CFsTexture* p = SearchByImageName( fileName, &index );

    while ( p )
        {
        iObserver.FsTextureEvent( MFsTextureObserver::EUpdated, *p );

        fn = aTexture.FileName();
        if ( fn )
            {
            fileName.Copy( fn->Des() );
            }
        else
            {
            fileName.Zero();
            }
        p = SearchByImageName( fileName, &index );

        }
    
    TRAP_IGNORE( UnloadL() )
        {
        TRAP_IGNORE( LoadL() )
        }
    
    return;
    }                                               
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::SetView
// Set current view area
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::SetViewL( const TInt& aStart, 
                                 const TInt& aLength 
                               )
    {
    FUNC_LOG;
    iView.iStart = aStart;
    iView.iLength = aLength;

    UnloadL();
    LoadL();
    
    return;
    }

// ----------------------------------------------------------------------------
// CFsTextureLoader::SetView
// Set current view area
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::SetViewL( const TView& aView )
    {
    FUNC_LOG;
    SetViewL( aView.iStart, aView.iLength );
    
    return;
    }

// ----------------------------------------------------------------------------
// CFsTextureLoader::CFsTextureLoader
// C++ Constructor
// Status : Draft
// ----------------------------------------------------------------------------
//

CFsTextureLoader::CFsTextureLoader( CAlfTextureManager& aManager,
                                    MFsTextureObserver& aObserver
                                  )

    : iManager( aManager ),
      iFs( CEikonEnv::Static()->FsSession() ),
      iTextures( NULL ),
      iObserver( aObserver ),
      iId( 0 ),
      iDefaultTimeOut( 10000000 ) // 10 sec.
    {
    FUNC_LOG;
    return;
    }  

// ----------------------------------------------------------------------------
// CFsTextureLoader::ConstructL
// 2nd phase contructor
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::ConstructL()
    {
    FUNC_LOG;

    iManager.AddLoadObserverL( this );

    
    iTextures = new ( ELeave ) CArrayPtrFlat<CFsTexture>( KGranularity );

    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::TimerExpiredL
// Timer callback passed from texture
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::TimerExpiredL( CFsTexture& aTexture )
    {
    FUNC_LOG;
    Unload( aTexture );
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::TextureCount
// Return number of textures
// Status : Draft
// ----------------------------------------------------------------------------
//
TInt CFsTextureLoader::TextureCount() const
    {
    FUNC_LOG;
    const TInt count = iTextures ? iTextures->Count() : 0;

    return count;
    }    

// ----------------------------------------------------------------------------
// CFsTextureLoader::TextureAtIndex
// Return texture
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTexture* CFsTextureLoader::TextureAtIndex( const TInt& aIndex )
    {
    FUNC_LOG;
    CFsTexture* texture = NULL;
    if ( iTextures &&
         aIndex >= 0 && 
         aIndex < TextureCount() 
       )
        {
        texture = iTextures->At( aIndex );
        }

    return texture;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::Unload
// Unload texture
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::Unload( CFsTexture& aTexture )
    {
    FUNC_LOG;
    const TInt c = iTextures->Count();
    TInt refCount = 0;
    TInt index = KErrNotFound;
    for ( TInt i = 0; i < c; i++ )
        {
        if ( iTextures->At( i ) == &aTexture )
            {
            index = i;
            }
        if ( &iTextures->At( i )->Texture() == &aTexture.Texture() )
            {
            refCount++;
            }
        }
    
    if ( refCount == 1 )
        {

        HBufC* p = aTexture.Texture().FileName();
        if ( p )
            {
            iManager.UnloadTexture( p->Des() );
            delete &(aTexture.Texture());
            }

        }
        
    if ( index != KErrNotFound )
        {
        CFsTexture* p = iTextures->At( index );
        CFsBitmapProvider* provider = p->Provider();
        if ( provider )
            {
            const TInt flags = p->TextFlags();
            if ( !( flags & CFsTexture::EFlagPreserveProvider ) )
                {
                iManager.UnloadTexture( provider->Id() );
                delete &(aTexture.Texture());
                }
            }
        }
    
    return;
    }    
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::SearchByImageName
// Search textures by filename
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTexture* CFsTextureLoader::SearchByImageName( const TDesC& aImageFileName,
                                                 TInt* aIndex
                                               )
    {
    FUNC_LOG;
    CFsTexture* r = NULL;
    const TInt c = iTextures->Count();
    TInt i; // Temporaly index
    for ( i = aIndex ? *aIndex : 0; i < c && !r; i++ )
        {
        CFsTexture* tmp = iTextures->At( i );

        CAlfTexture& texture = tmp->Texture();
        HBufC* p = texture.FileName();
        if ( p && !p->Des().Compare( aImageFileName ) )

            {
            r = tmp;
            }
        }
    
    // Update index
    if ( aIndex )
        {
        *aIndex = i;
        }
    
    return r;
    }

// ----------------------------------------------------------------------------
// CFsTextureLoader::SearchByPos
// Search textures by position
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTexture* CFsTextureLoader::SearchByPos( const TInt& aPos,
                                           TInt* aIndex )
    {
    FUNC_LOG;
    CFsTexture* r = NULL;
    const TInt c = iTextures->Count();
    TInt i; // Temporaly index
    for ( i = aIndex ? *aIndex : 0; i < c && !r; i++ )
        {
        CFsTexture* tmp = iTextures->At( i );
        const TInt pos = tmp->Pos();

        CAlfTexture& texture = tmp->Texture();

        if ( pos >= 0 && pos == aPos )
            {
            r = tmp;
            }
        }

    // Update index
    if ( aIndex )
        {
        *aIndex = i;
        }
    
    return r;
    }

// ----------------------------------------------------------------------------
// CFsTextureLoader::SearchById
// Search textures by id
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTexture* CFsTextureLoader::SearchById( TInt aId )
    {
    CFsTexture* r = NULL;
    const TInt c = iTextures->Count();
    TInt i; // Temporaly index
    for ( i = 0 ; i < c && !r ; ++i )
        {
        CFsTexture* tmp = iTextures->At( i );
        
        if ( tmp->Id() == aId )
            {
            r = tmp;
            }
        }
    
    return r;
    }
// ----------------------------------------------------------------------------
// CFsTextureLoader::LoadTextureL
// Create texture from file
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//

CFsTexture& CFsTextureLoader::LoadTextureL( const TDesC& aImageFileName,
                                            TSize aTextureMaxSize,
                                            TAlfTextureFlags aFlags,
                                            TInt aId,
                                            const TInt& aLoaderFlags
                                          )

    {                                            
    // If id's not given by user, generate unique id
    if ( !aId )
       {
       aId = GetId();
       }


    CAlfTexture& texture = iManager.LoadTextureL( aImageFileName,
                                                  aTextureMaxSize,

                                                  aFlags,
                                                  aId
                                                );
    CFsTexture* holder = CFsTexture::NewL( texture, *this, NULL, aId );
    // Verify if this has been previously loaded or was loaded syncronously
    if ( texture.HasContent() )
        {
        iObserver.FsTextureEvent( MFsTextureObserver::EUpdated, *holder );
        }
    // Pass flagging
    if ( aLoaderFlags & EFlagSetStatic )
        {
        holder->SetTextFlags( holder->TextFlags() | CFsTexture::EFlagStatic );
        }
    CleanupStack::PushL( holder );
    iTextures->AppendL( holder );
    CleanupStack::Pop( holder );
    
    return *holder;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::AppendBitmapL
// Create texture from bitmap(s) 
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//

CFsTexture& CFsTextureLoader::AppendBitmapL( CFbsBitmap* aBitmap,
                                             CFbsBitmap* aBitmapMask,
                                             TAlfTextureFlags aFlags,
                                             TInt aId,
                                             const TInt& aLoaderFlags
                                           )

    {                                            
    // If id's not given by user, generate unique id
    if ( !aId )
       {
       aId = GetId();
       }
    
    CFsBitmapProvider* provider = CFsBitmapProvider::NewL( aBitmap,
                                                           aBitmapMask,
                                                           aId
                                                         );

    CAlfTexture& texture = iManager.CreateTextureL( aId,
                                                    provider,
                                                    aFlags
                                                  );
    CFsTexture* holder  = CFsTexture::NewL( texture, *this, provider, aId );
    // Pass flagging
    if ( aLoaderFlags & EFlagSetStatic )
        {
        holder->SetTextFlags( holder->TextFlags() | CFsTexture::EFlagStatic );
        }
    CleanupStack::PushL( holder );
    iTextures->AppendL( holder );
    CleanupStack::Pop( holder );
    iObserver.FsTextureEvent( MFsTextureObserver::EUpdated, *holder );

    return *holder;
    }    

// ----------------------------------------------------------------------------
// CFsTextureLoader::LoadL
// Load next picture in queue
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::LoadL( CFsTexture* aTexture )
    {
    FUNC_LOG;
    const TInt c = iTextures->Count();
    CFsTexture* texture = aTexture;
    for ( TInt i = 0; i < c && !aTexture; i++ )
        {
        CFsTexture* tmp = iTextures->At( i );
        const TInt flags = tmp->TextFlags();
        const TInt pos = tmp->Pos();
        const TBool hasContent = tmp->Texture().HasContent();
        if ( !hasContent && 
             flags & CFsTexture::EFlagStatic )
            {
            texture = tmp;
            }
        else if ( !hasContent &&
                  pos != KFsUndefined &&
                  iView.iStart <= pos &&
                  pos <= ( iView.iStart + iView.iLength )
                )
            {
            texture = tmp;
            }
        else
            {
            // Left for Lint
            }
        } // i
   
    if ( texture )
        {
        const TInt id = texture->Id();
        if ( id )
            {
            const TBool isLoaded = iManager.IsLoaded( id );
            if ( !isLoaded )
                {
                iManager.LoadTextureL( id );
                } // !isLoaded
            } // id
        } // texture
    
    return;
    } 

// ----------------------------------------------------------------------------
// CFsTextureLoader::UnloadL
// Unload picture when possible
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::UnloadL()
    {
    FUNC_LOG;
    const TInt c = iTextures->Count();
    CFsTexture* texture = NULL;
    for ( TInt i = 0; i < c ; i++ )
        {
        texture = iTextures->At( i );
        const TBool isStatic = ( texture->TextFlags() & CFsTexture::EFlagStatic );
        const TInt pos = texture->Pos();
        const TBool isVisible = iView.iStart <= pos && pos <= ( iView.iStart + iView.iLength );
        if ( texture->Texture().HasContent() && 
             !isStatic &&
             !isVisible
           )
            {
            texture->UnloadL( EFalse );
            }
        else if ( isVisible )
            {
            texture->UnloadL( ETrue ); // cancel possible unload
            }
        else
            {
            // Left for Lint
            }
        } // i
        
    return;
    } 

// ----------------------------------------------------------------------------
// CFsTextureLoader::View
// Return view
// Status : Draft
// ----------------------------------------------------------------------------
//
CFsTextureLoader::TView CFsTextureLoader::View() const
    {
    FUNC_LOG;
    return iView;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::GetFrameInfoL
// Extract frameinfo from picture header
// (may leave, static)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::GetFrameInfoL( const TDesC& aImageFileName, 
                                      TFrameInfo& aFrameInfo 
                                    )
    {
    FUNC_LOG;
    CImageDecoder* imageDecoder = CImageDecoder::FileNewL( CEikonEnv::Static()->FsSession(), 
                                                           aImageFileName 
                                                         );
    CleanupStack::PushL( imageDecoder );
    aFrameInfo = imageDecoder->FrameInfo();
    CleanupStack::PopAndDestroy( imageDecoder );

    return;
    }                                            
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::GetId
// Get unique id
// Status : Draft
// ----------------------------------------------------------------------------
//
TInt CFsTextureLoader::GetId()
   {
   FUNC_LOG;
   TInt err;
   TRAP( err, iManager.TextureL( ++iId ) )
   while ( err != KErrNotFound )
       {
       TRAP( err, iManager.TextureL( ++iId ) )
       } // err
   
   return iId;
   }
   
// ----------------------------------------------------------------------------
// CFsTextureLoader::SetDefaultTimeOut
// Set default timeout
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::SetDefaultTimeOut( const TTimeIntervalMicroSeconds32& aTimeOut )
    {
    FUNC_LOG;
    iDefaultTimeOut = aTimeOut;
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::DefaultTimeOut
// From MFsTextureLoaderObserver
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::DefaultTimeOut( TTimeIntervalMicroSeconds32& aTimeOut )
    {
    FUNC_LOG;
    aTimeOut = iDefaultTimeOut;
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::PosUpdated
// From MFsTextureLoaderObserver
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::PosUpdated( CFsTexture& aTexture )
    {
    FUNC_LOG;
    iObserver.FsTextureEvent( MFsTextureObserver::EPositionUpdated, aTexture );
    
    const TInt newPos = aTexture.Pos();
    if ( newPos < iView.iStart ||
         newPos > ( iView.iStart + iView.iLength )
       )
        {
        const TInt flags = aTexture.TextFlags();
        if ( !( flags & CFsTexture::EFlagStatic ) )
            {
            Unload( aTexture );
            } // flags
        }
    else
        {
        const TInt id = aTexture.Id();
        if ( !iManager.IsLoaded( id ) )
            {
            TRAP_IGNORE( iManager.LoadTextureL( id ) )
            }
        TRAP_IGNORE( aTexture.UnloadL( ETrue ) ) // cancel possible unload
        }
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::Deleted
// From MFsTextureLoaderObserver
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::Deleted( CFsTexture& aTexture )
    {
    FUNC_LOG;
    Unload( aTexture );
    
    const TInt c = iTextures->Count();
    for ( TInt i = 0; i < c; i++ )
        {
        CFsTexture* p = iTextures->At( i );
        if ( p == &aTexture )
            {
            iTextures->Delete( i );
            i = ( c + 1 );
            }
        }
    
    return;
    }
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::FlagsUpdated
// From MFsTextureLoaderObserver
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::FlagsUpdated( CFsTexture& aTexture )
    {
    FUNC_LOG;
    const TInt flags = aTexture.TextFlags();
    if ( flags & CFsTexture::EFlagStatic )
        {
        TRAP_IGNORE( LoadL() );
        }
    else
        {
        TRAP_IGNORE( UnloadL() );
        }
    
    return;
    }    
    
// ----------------------------------------------------------------------------
// CFsTextureLoader::SizeUpdated
// From MFsTextureLoaderObserver
// Status : Draft
// ----------------------------------------------------------------------------
//
void CFsTextureLoader::SizeUpdated( CFsTexture& aTexture )
    {
    FUNC_LOG;
    // Forward observation
    iObserver.FsTextureEvent( MFsTextureObserver::EUpdated, aTexture );
    
    return;
    }    
       
    
// ========================= OTHER EXPORTED FUNCTIONS =========================

// End of file


