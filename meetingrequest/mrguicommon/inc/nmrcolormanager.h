/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Color manager for MRGUI usage.
*
*/

#ifndef NMRCOLORMANAGER_H
#define NMRCOLORMANAGER_H

#include <e32base.h>
#include <aknsitemid.h>
#include <gdi.h> // TRgb

// FORWARD DECLARATIONS
class CCoeControl;
class CEikTTimeEditor;
class CEikDateEditor;

namespace NMRColorManager
    {    
    enum TMRColorId 
        {
        EMRColorNotSet = 0x00,
        EMRMainAreaTextColor = 0x01,
        EMRCutCopyPasteHighlightColor,
        EMRCopiedTextColor,
        EMRNaviPaneText,
        EMRPredictiveSearchPopUpBgColor,
        EMRMainAreaTextColorDimmed,
        EMRLastColorItemId = 0x200 // Always last item in the list
        };
    
    class TMRColorStruct 
        {
        public:
            TMRColorStruct() : 
                    iColorIndex( 0 ) 
                        { iColorGroup.iMajor = 0; iColorGroup.iMinor = 0; }
            
        public: // Data
            TAknsItemID iColorGroup;
            TInt iColorIndex;
        };
    
    /**
     * Converts logical TMRColorId to internal TMRColorStruct
     * which contains the correct color group and color id
     * 
     * @param aColorId Logical color id.
     * @return TMRColorStruct Contains color group and color id.
     */
    IMPORT_C TMRColorStruct GetColorStruct( TMRColorId aColorId );
    
    /**
     * Returns correct Rgb color based on the given color enumeration
     *
     * @param aColorId Logical color id.
     * @return TRgb The Rgb color based on the logical color id
     */
    IMPORT_C TRgb Color( TMRColorId aColorId );
    
    /**
     * Sets font color based on the given color id for the given coecontrol
     *
     * @param aControl The control for which the color is set to.
     * @param aColorId Logical color id.
     */
    IMPORT_C void SetColor( CCoeControl& aControl, TMRColorId aColorId );
    
    /**
     * Sets font color based on the given color id for the given CEikTTimeEditor
     *
     * @param aEditor The control for which the color is set to.
     * @param aColorId Logical color id.
     */
    IMPORT_C void SetColor( CEikTTimeEditor& aEditor, TMRColorId aColorId );
    }

#endif // NMRCOLORMANAGER_H
