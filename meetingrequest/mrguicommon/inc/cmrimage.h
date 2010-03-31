/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#ifndef CMRIMAGE_H
#define CMRIMAGE_H

//#include <aknsconstants.h>
#include <akniconutils.h>
#include <eikimage.h>

#include "esmrfieldbuilderdef.h"
#include "nmrbitmapmanager.h"

/**
 *  CMRImage is icon component for fields 
 */
NONSHARABLE_CLASS( CMRImage ) : public CEikImage
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aBitmapId
     * @param aScaleMode 
     * @return Pointer to created and initialized mr image object.
     */
    IMPORT_C static CMRImage* NewL(
            NMRBitmapManager::TMRBitmapId aBitmapId,
            TBool aIsMaskIcon = EFalse,
            TScaleMode aScaleMode = EAspectRatioPreserved
            );

   /**
    * C++ Destructor
    */
   IMPORT_C virtual ~CMRImage();
   
   void SizeChanged();

private:
   /**
    * C++ default constructor
    */
   CMRImage( TScaleMode aScaleMode );
   void ConstructL( NMRBitmapManager::TMRBitmapId aBitmapId, TBool aIsMaskIcon );
   void CreateIconL( NMRBitmapManager::TMRBitmapId aBitmapId );
   void CreateMaskIconL( NMRBitmapManager::TMRBitmapId aBitmapId );
    
private:
    TScaleMode iScaleMode;
    };

#endif //CMRIMAGE_H

// EOF
