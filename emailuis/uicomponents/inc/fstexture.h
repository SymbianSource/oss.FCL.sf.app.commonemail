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
* Description: This file defines class CFsTexture.
*
*/


#ifndef __FSTEXTURE_H__
#define __FSTEXTURE_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
//<cmail> SF
#include <alf/alftexture.h>
//</cmail>
#include "fstextureloader.h"
#include "fstextureobserver.h"

// FORWARD DECLARATIONS
class CFbsBitmap;
class CAlfPicture;
class CAlfTexture;
class CFsBitmapProvider;

// CONSTANTS
#define KFsUndefined (-1)

// CLASS DECLARATION

/**
*  CFsTexture
*  -
*/
class CFsTexture
    : public CBase
    {
    public:
        // Flags
        enum
            {
            EFlagNoneDefined,
            EFlagStatic = 1 << 1,
            EFlagPreserveProvider = 1 << 2
            };
    public: // Constructor, destructor

        /**
         * 2-phased constructor
         * @param aTexture
         * @param aObserver Observer reference
         * @param aProvider (Optional) Bitmap provider
         * @param aId Unique id
         * @return Instance of texture
         */
        static CFsTexture* NewL( CAlfTexture& aTexture,
                                 MFsTextureLoaderObserver& aObserver,
                                 CFsBitmapProvider* aProvider,
                                 const TInt aId
                               );


        /**
         * C++ Destructor
         */
        ~CFsTexture();
    public:
        /**
         * Return reference of Alf/Hitchock texture
         * @return Reference
         */

        CAlfTexture& Texture();


        /**
         * Return address of provider
         * @return null, or address of bitmap provider
         */
        CFsBitmapProvider* Provider();

        /**
         * Return position of texture
         * @return Position, -1 stands for undefined position
         */
        TInt Pos() const;

        /**
         * Set position
         * @param aPos New position
         */
        void SetPos( const TInt& aPos );

        /**
         * Unload bitmap, or cancel unload
         * @param aCancel, ETrue cancels possible unload procedure
         */
        void UnloadL( const TBool& aCancel );

        /**
         * Return internal size of texture
         * @return texture size, or overwritten internal size
         */
        TSize TextSize() const;

        /**
         * Overwrite default size
         * @param aSize New size
         */
        void SetTextSize( const TSize& aSize );

        /**
         * Return current flag status
         * @return Flag status
         */
        TInt TextFlags() const;

        /**
         * Set flag status
         * @aparam New staus
         */
        void SetTextFlags( const TInt& aFlags );

        /**
         * Return unique id of texture
         * @return Id
         */
        TInt Id() const;

    public: // for Timer callback access
        /**
         * Timer callback
         */
        void TimerExpiredL();
        
    private: // New methods
        /**
         * Stop timer
         */
        void StopTimer();

    private: // Constructor

        /**
         * C++ constructor
         * @param aTexture
         * @param aObserver Observer reference
         * @param aProvider (Optional) Bitmap provider
         * @param aId Unique id
         */
        CFsTexture( CAlfTexture& aTexture,
                    MFsTextureLoaderObserver& aObserver,
                    CFsBitmapProvider* aProvider,
                    const TInt aId
                  );


       /**
         * 2nd phase constructor
         */
        void ConstructL();

    private: // data
        /** Texture position */
        TInt iPos;
        
        /** Owns:*/
        CPeriodic* iUnloadTimer;        

        /** Texture reference */

        CAlfTexture& iTexture;

        
        /** Observer reference */
        MFsTextureLoaderObserver& iObserver;
        
        /** Internal text size, not related to texture size */
        TSize iTextSize;
        
        /** Internal flags */
        TInt iTextFlags;
        
        /** Owns: */
        CFsBitmapProvider* iProvider;
        
        /** Texture id */
        const TInt iId;
    };

/**
* Timer callback
* @aModel Address of instance, which will handle callback
* @return Response
*/
TInt TimerCallbackL( TAny* aModel );

#endif // __FSTEXTURE_H__

// End of File
