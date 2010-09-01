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
* Description: This file defines class MFsTextureObserver.
*
*/


#ifndef __FSTEXTUREOBSERVER_H__
#define __FSTEXTUREOBSERVER_H__

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CFsTexture;

// CLASS DECLARATION

/**
*  MFsTextureObserver
*  Texture observer
*/
class MFsTextureObserver
    {
    public: // Events
        enum TFsTextureEvent
            {
            EPositionUpdated,
            EUpdated,
            ETextureUnloaded
            };

    public: // New methods
        /**
         * FsTextureEvent
         * @param aEventype Event type
         * @param aTexture Texture reference
         */
        virtual void FsTextureEvent( const TFsTextureEvent& aEventType,
                                     CFsTexture& aTexture 
                                   ) = 0;
    };

#endif // __FSTEXTUREOBSERVER_H__

// End of File
