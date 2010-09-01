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
* Description: This file defines class CFsBitmapProvider.
*
*/


#ifndef __FSBITMAPPROVIDER_H__
#define __FSBITMAPPROVIDER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

//<cmail> removed __FS_ALFRED_SUPPORT flag 
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag 
//<cmail> SF
#include <alf/alfbitmapprovider.h>
#include <alf/alftexture.h>
//</cmail>

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
*  CFsTextureLoader
*  Loader
*/
class CFsBitmapProvider
    : public CBase
      ,public MAlfBitmapProvider


    {
    public:
        static CFsBitmapProvider* NewL( const CFbsBitmap* aBitmap,
                                        const CFbsBitmap* aBitmapMask,
                                        const TInt aId
                                      );
    public: //
        virtual ~CFsBitmapProvider();
            
    public:
        TInt Id() const;
    
    private: // Constructors
        /**
         * C++ Constructor
         * @param aId Provider identifier
         */
        CFsBitmapProvider( const TInt aId );

        /**
         * Symbian 2nd-phase constructor
         * @param aId Provider identifier
         */
        void ConstructL( const CFbsBitmap* aBitmap, 
                         const CFbsBitmap* aBitmapMask
                       );
                       
    private: // from MHuiBitmapProvider
        /**
         * Provide a bitmap and mask from the given UID.
         * This method should create a bitmap and its mask, usually by loading it from disk.
         *
         * @param aId      TextureManager id of this bitmap, may be used for extracting
         *                 an icon resource.
         * @param aBitmap  Output parameter for the provider: On return, has to store 
         *                 a pointer to the generated bitmap. Ownership transferred to caller.
	     *                 The support for input CFbsBitmap formats varies between 
	     *                 different renderers. See \ref cfbsbitmapsupport 
	     *                 Table describing renderer bitmap support.      
	     * @param aMaskBitmap  
	     *                 Output parameter for the provider: On return, has to store 
         *                 a pointer to the generated alpha mask or <code>NULL</code> 
         * 				   if there is no mask. Ownership of the bitmap is transferred 
         *                 to the caller.
         * 
         * @see CHuiTextureManager::CreateTextureL()
         */
     
        virtual void ProvideBitmapL( TInt aId, 
                              CFbsBitmap*& aBitmap, 
                              CFbsBitmap*& aMaskBitmap
                            );
    private: // Data
        /** Owns: */
        CFbsBitmap* iBitmap;
    
        /** Owns: */
        CFbsBitmap* iBitmapMask;

        /** Identifier */
        const TInt iId;
    };

#endif // __FSBITMAPPROVIDER_H__

// End of File
