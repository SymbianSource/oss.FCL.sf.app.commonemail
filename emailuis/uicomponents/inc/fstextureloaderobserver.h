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
* Description: This file defines class MFsTextureLoaderObserver.
*
*/


#ifndef __FSTEXTURELOADEROBSERVER_H__
#define __FSTEXTURELOADEROBSERVER_H__

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CFsTexture;

// CLASS DECLARATION

/**
*  MFsTextureLoaderObserver
*  Texture loader observer
*/

class MFsTextureLoaderObserver
    {
    public: // New methods
        /**
         * TimerExpiredL
         * @param aTexture Texture reference
         */
        virtual void TimerExpiredL( CFsTexture& aTexture ) = 0;

        /**
         * Loader timeout
         * @param aTimeOut Timeout in microseconds as return value
         */
        virtual void DefaultTimeOut( TTimeIntervalMicroSeconds32& aTimeOut ) = 0;

        /**
         * Texture position updated
         * @param aTexture Texture reference
         */
        virtual void PosUpdated( CFsTexture& aTexture ) = 0;

        /**
         * Texture destroyed
         * @param aTexture Texture reference
         */
        virtual void Deleted( CFsTexture& aTexture ) = 0;

        /**
         * Flags updated
         * @param aTexture Texture reference
         */
        virtual void FlagsUpdated( CFsTexture& aTexture ) = 0;

        /**
         * Size updated
         * @param aTexture Texture reference
         */
        virtual void SizeUpdated( CFsTexture& aTexture ) = 0;
    };

#endif // __FSTEXTURELOADEROBSERVER_H__

// End of File
