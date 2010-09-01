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

#include "nmrcolormanager.h"

#include <gdi.h>
#include <eikmfne.h>
#include <AknsItemID.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <esmrgui.mbg>
#include <data_caging_path_literals.hrh>

using namespace NMRColorManager;

namespace { // codescanner::unnamednamespace

#ifdef _DEBUG

// Definition for panic text
_LIT( KNMRColorManager, "NMRColorManager" );

/**
 * ES MR Entry panic codes
 */
enum TNMRColorManager
    {
    KUnhandledColorId = 1
    };

/**
 * Raises panic.
 * @param aPanic Panic code
 */
void Panic( TNMRColorManager aPanic )
    {
    User::Panic( KNMRColorManager, aPanic);
    }

#endif // _DEBUG

}

// ---------------------------------------------------------------------------
// NMRColorManager::GetColorStruct
// ---------------------------------------------------------------------------
//
EXPORT_C TMRColorStruct NMRColorManager::GetColorStruct( TMRColorId aColorId )
    {
    TMRColorStruct colorStruct;
    
    switch ( aColorId )
        {
        case EMRMainAreaTextColor:
            {
            colorStruct.iColorGroup = KAknsIIDQsnTextColors;
            colorStruct.iColorIndex = EAknsCIQsnTextColorsCG6;
            break;
            }
        case EMRCutCopyPasteHighlightColor:
            {
            colorStruct.iColorGroup = KAknsIIDQsnHighlightColors;
            colorStruct.iColorIndex = EAknsCIQsnHighlightColorsCG2; 
            break;
            }
        case EMRCopiedTextColor:
            {
            colorStruct.iColorGroup = KAknsIIDQsnTextColors;
            colorStruct.iColorIndex = EAknsCIQsnTextColorsCG24; 
            break;
            }
        case EMRNaviPaneText:
            {
            colorStruct.iColorGroup = KAknsIIDQsnTextColors;
            colorStruct.iColorIndex = EAknsCIQsnTextColorsCG2; 
            break;
            }
        case EMRPredictiveSearchPopUpBgColor:
            {
            colorStruct.iColorGroup = KAknsIIDFsOtherColors;
            colorStruct.iColorIndex = EAknsCIFsOtherColorsCG13; 
            break;
            }
        case EMRMainAreaTextColorDimmed:
            {
            colorStruct.iColorGroup = KAknsIIDFsOtherColors;
            colorStruct.iColorIndex = EAknsCIFsOtherColorsCG1; 
            }
        case EMRLastColorItemId:
            {
            colorStruct.iColorGroup = KAknsIIDQsnTextColors;
            colorStruct.iColorIndex = EAknsCIQsnTextColorsCG74; 
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( KUnhandledColorId ) );
            break;
            }
        }
    return colorStruct;
    }

// ---------------------------------------------------------------------------
// NMRColorManager::Color
// ---------------------------------------------------------------------------
//
EXPORT_C TRgb NMRColorManager::Color( TMRColorId aColorId )
    {
    TRgb color;
    TMRColorStruct colorStruct = GetColorStruct( aColorId );

    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                               color,
                               colorStruct.iColorGroup,
                               colorStruct.iColorIndex );
    return color;
    }

// ---------------------------------------------------------------------------
// NMRColorManager::SetColor
// ---------------------------------------------------------------------------
//
EXPORT_C void NMRColorManager::SetColor( CCoeControl& aControl, 
                                         TMRColorId aColorId )
    {
    // If color setting fails, we just don't set it
    TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( 
                        aControl, 
                        EColorLabelText,
                        Color( aColorId ) ) );
    }

// ---------------------------------------------------------------------------
// NMRColorManager::SetColor
// ---------------------------------------------------------------------------
//
EXPORT_C void NMRColorManager::SetColor( CEikTTimeEditor& aEditor, 
                                         TMRColorId aColorId )
    {
    TMRColorStruct colorStruct = GetColorStruct( aColorId );

    // If color setting fails, we just don't set it
    TRAP_IGNORE( aEditor.SetSkinTextColorL( colorStruct.iColorIndex ) );
    }

// EOF

