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

#ifndef CMRButton_H
#define CMRButton_H

#include <AknIconUtils.h>
#include <aknbutton.h>

#include "esmrfieldbuilderdef.h"
#include "nmrbitmapmanager.h"

/**
 *  CMRButton is graphical button component for fields 
 */
NONSHARABLE_CLASS( CMRButton ) : public CAknButton
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aBitmapId
     * @param aScaleMode 
     * @return Pointer to created and initialized mr image object.
     */
     IMPORT_C static CMRButton* NewL(
             NMRBitmapManager::TMRBitmapId aBitmapId, 
             TScaleMode aScaleMode = EAspectRatioPreserved );

    /**
     * C++ Destructor
     */
    IMPORT_C virtual ~CMRButton();
    
    void SizeChanged();

private:
    /**
     * C++ default constructor
     */
    CMRButton( TScaleMode aScaleMode );
    void ConstructL( NMRBitmapManager::TMRBitmapId aBitmapId );
    
private:
    TScaleMode iScaleMode;
    };

#endif //CMRButton_H

// EOF
