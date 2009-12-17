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
* Description:  Edit before send list pop-up query
*
*/

#ifndef CESMRICONFIELD_H
#define CESMRICONFIELD_H

#include "cesmrfield.h"
#include "esmrfieldbuilderdef.h"
#include <AknsConstants.h>

class CEikImage;

/**
 *  CESMRIconField is base class for all the fields that has icon.
 */
class CESMRIconField : public CESMRField
    {
public:

    /**
     * C++ default constructor
     */
    IMPORT_C CESMRIconField();

    /**
     * C++ Destructor
     */
    IMPORT_C virtual ~CESMRIconField();

protected:
    /**
     * Second phase constructor for the CESMRIconField.
     *
     * @param aIconID - ID for the icon that this field should show
     * @param aControl - control that needs to have the highlight or border
     * @param aFocusType - Focus type (border/highlight or none)
     */
    IMPORT_C void ConstructL(
            TAknsItemID aIconID,
            CCoeControl* aControl,
            TESMRFieldFocusType aFocusType = EESMRBorderFocus  );

    /*
     * Calculates icon field border size. 
     * Sub classes may override this if border size needs to be adjusted.
     * 
     * @return border size
     */
    IMPORT_C virtual TSize BorderSize() const;

public:
     // This method can be called by the derived classes to update the icon
    IMPORT_C virtual void IconL( TAknsItemID aIconID );

public: // From CCoeControl
    IMPORT_C void SetContainerWindowL(const CCoeControl& aContainer);
    IMPORT_C TInt CountComponentControls() const;
    IMPORT_C CCoeControl* ComponentControl( TInt aInd ) const;
    IMPORT_C void SizeChanged();

private:
    /// Own: Pointer to the image that is drawn in the screen
    CEikImage* iIcon;
    /// Own: Pointer to the bitmap. iIcon is created by using this bitmap
    CFbsBitmap* iBitmap;
    /// Own: Pointer to the bitmap mask. iIcon is created by using this bitmap mask
    CFbsBitmap* iBitmapMask;
    /// Own: ID of the needed bitmap
    TAknsItemID iIconID;
    };

#endif //CESMRICONFIELD_H

// EOF
