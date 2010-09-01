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
* Description: This file defines class CFsTextureLoader.
*
*/


#ifndef __FSTEXTURELOADER_H__
#define __FSTEXTURELOADER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag 
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag 
//<cmail> SF
#include <alf/alftexturemanager.h>
//</cmail>
#include "fstextureobserver.h"
#include "fstextureloaderobserver.h"

// FORWARD DECLARATIONS
class CFsTexture;
class CImageDecoder;
class RFs;
class TFrameInfo;

// CLASS DECLARATION

/**
*  CFsTextureLoader
*  Loader
*/
class CFsTextureLoader
    : public CBase,

      public MAlfTextureLoadingCompletedObserver,

      public MFsTextureLoaderObserver
    {
    public:
        // Flags
        enum
            {
            EFlagNoneDefined,
            EFlagLoadSyncronously = 1 << 0,
            EFlagSetStatic = 1 << 1
            };
    public:
        // Internal view structure
        struct TView
            {
            TView& operator= ( const TView& aView );
            TInt iStart;
            TInt iLength;
            };
    public: // Constructor, destructor
        /**
         * 2-phased constructor
         * @param aManager Texture manager reference
         * @param aObserver Observer reference
         * @return Instance of loader
         */

        static CFsTextureLoader* NewL( CAlfTextureManager& aManager,
                                       MFsTextureObserver& aObserver 
                                     );


        /**
         * C++ Destructor
         */
        ~CFsTextureLoader();
        
    public: // New methods
        /**
         * Search texture
         * @param aImageFileName File criteria
         * @param aIndex Index, 0 by default
         * @return null, or first found texture from beginning to index
         */
        CFsTexture* SearchByImageName( const TDesC& aImageFileName, TInt* aIndex = NULL );

        /**
         * Search texture
         * @param aPos Position criteria
         * @param aIndex Index, 0 by default
         * @return null, or first found texture from beginning to index
         */
        CFsTexture* SearchByPos( const TInt& aPos, TInt* aIndex = NULL );

        /**
         * Search texture
         * @param aId
         */
        CFsTexture* SearchById( TInt aId );

        /**
         * Create texture
         * @param aImageFileName File
         * @param aFlag Hitchock flags
         * @param aId Texture unique id, 0 for automation 
         * @param aLoaderFlags Texture loader flags
         * @return Texture reference
         */

        CFsTexture& LoadTextureL( const TDesC& aImageFileName,
                                  TSize aTextureMaxSize = TSize( 0, 0 ),
                                  TAlfTextureFlags aFlags = EAlfTextureFlagDefault,
                                  TInt aId = 0,
                                  const TInt& aLoaderFlags = 0
                                ); 


        /**
         * Create texture from bitmaps
         * @param aBitmap Bitmap
         * @param aBitmapMask Mask
         * @param aFlag Hitchock flags
         * @param aId Texture unique id, 0 for automation 
         * @param aLoaderFlags Texture loader flags
         * @return Texture reference
         */

        CFsTexture& AppendBitmapL( CFbsBitmap* aBitmap,
                                   CFbsBitmap* aBitmapMask,
                                   TAlfTextureFlags aFlags = EAlfTextureFlagDefault,
                                   TInt aId = 0,
                                   const TInt& aLoaderFlags = 0
                                 );

        
        /**
         * Set view
         * @param aStart Beginning of view area
         * @param aLength View area length
         */
        void SetViewL( const TInt& aStart, const TInt& aLength );

        /**
         * Set view
         * @param aView View structure
         */
        void SetViewL( const TView& aView );

        /**
         * Return current view area
         * @return View structure
         */
        TView View() const;

        /**
         * Unload texture
         * @param aTexture Texture reference
         */
        void Unload( CFsTexture& aTexture );

        /**
         * Texture count
         * @return Number of textures
         */
        TInt TextureCount() const;

        /**
         * Get texture by index
         * @param aIndex Texture index
         * @return null, or address of texture
         */
        CFsTexture* TextureAtIndex( const TInt& aIndex );

        /**
         * Read bitmap properties
         * @param aImageFileName Filename
         * @param aFrameInfo Bitmap properties as return value
         */
        static void GetFrameInfoL( const TDesC& aImageFileName, 
                                   TFrameInfo& aFrameInfo 
                                 );
        
        /**
         * Set default timeout for unloading the textures
         * @param aTimeOut Timeout in microseconds, 10 sec. by default
         */
        void SetDefaultTimeOut( const TTimeIntervalMicroSeconds32& aTimeOut );

    private: // New methods

        /**
         * Load texture
         * @param aTexture Address of texture
         */
        void LoadL( CFsTexture* aTexture = NULL );

        /**
         * Unload texture
         */
        void UnloadL();

        /**
         * Return texture id
         * @return Unique id
         */
        TInt GetId();
    private: // Constructors
        /**
         * C++ constructor
         * @param aManager Texture manager reference
         * @param aObserver Observer reference
         */

        CFsTextureLoader( CAlfTextureManager& aManager,
                          MFsTextureObserver& aObserver
                        );


        /**
         * 2nd phase constructor
         */
        void ConstructL();
        
    private: // MHuiTextureLoadingCompletedObserver / MAlfTextureLoadingCompletedObserver

        /**
         * Called to notify the observer that loading of a texture has
         * been completed.
         * @param aTexture Reference to the texture that has been loaded.
         * @param aTextureId Id of the texture in the texture manager. Can be used
         * to identify the loaded texture, provided that an id was assigned to the
         * texture.
         * @param aErrorCode KErrNone if the load was successful, otherwise one of
         * the system-wide error codes indicating reason why the texture loading
         * failed.
         * @note One should not commence loading of a new texture in this callback method.
         */
        void TextureLoadingCompleted( CAlfTexture& aTexture,
                                      TInt aTextureId,
                                      TInt aErrorCode
                                    );


    private: // MFsTextureLoaderObserver 
        /**
         * TimerExpiredL
         * @param aTexture Texture reference
         */
        virtual void TimerExpiredL( CFsTexture& aTexture );

        /**
         * Loader timeout
         * @param aTimeOut Timeout in microseconds as return value
         */
        virtual void DefaultTimeOut( TTimeIntervalMicroSeconds32& aTimeOut );

        /**
         * Texture position updated
         * @param aTexture Texture reference
         */
        virtual void PosUpdated( CFsTexture& aTexture );

        /**
         * Texture destroyed
         * @param aTexture Texture reference
         */
        virtual void Deleted( CFsTexture& aTexture );
        
        /**
         * Flags updated
         * @param aTexture Texture reference
         */
        virtual void FlagsUpdated( CFsTexture& aTexture );

        /**
         * Size updated
         * @param aTexture Texture reference
         */
        virtual void SizeUpdated( CFsTexture& aTexture );

    private: // data
        /** Texture manager reference */

        CAlfTextureManager& iManager;

        
        /**  File system reference */
        RFs& iFs;

        /** Owns: */
        CArrayPtr<CFsTexture>* iTextures;

        /**  View context */
        TView iView;

        /**  Scheduler */
        CActiveSchedulerWait iWait;
        
        /** Observer reference */
        MFsTextureObserver& iObserver;
        
        /** Id counter */
        TInt iId;
        
        /** Default timeout */
        TTimeIntervalMicroSeconds32 iDefaultTimeOut;        
    };

#endif // __FSTEXTURELOADER_H__

// End of File