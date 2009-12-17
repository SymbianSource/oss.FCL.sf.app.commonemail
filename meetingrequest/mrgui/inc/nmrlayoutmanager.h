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
* Description:  Layout manager for MRGUI usage.
*
*/

#ifndef NMRLAYOUTMANAGER_H
#define NMRLAYOUTMANAGER_H

#include <e32base.h>
#include <AknUtils.h>
#include <AknLayout2ScalableDef.h>


namespace NMRLayoutManager
    {

    /**
     * Enumeration for getting window component sizes.
     */
    enum TMRLayout {
        // Table 1.1
        EMRLayoutDialog,
        // Table 1.48 (Mrui)
        EMRLayoutCtrlBar,
        EMRLayoutContentArea,
        EMRLayoutDialogBg,
        // Table 1.49
        EMRLayoutDialogBgGraphics,
        // Table 1.50
        EMRLayoutListArea,
        EMRLayoutScrollBar,
        
        // Table 1.53
        // Option 0
        // Option 1
        EMRLayoutTextEditorIcon,
        EMRLayoutTextEditorBg,
        // Option 2
        EMRLayoutTimeEditorIcon,
        EMRLayoutTimeEditorStartBg,
        EMRLayoutTimeEditorEndBg,
        // Option 3
        EMRLayoutDateEditorIcon,
        EMRLayoutDateEditorBg,
        // Option 4
        // Option 5
        EMRLayoutCheckboxIcon,
        
        // Table 1.67
        // Option 0
        // Option 1
        // Option 2
        EMRLayoutCtrlBarGraphics
    };
    
    /**
     * Enumeration for getting text component sizes, fonts, etc.
     */
    enum TMRTextLayout {
        // Table 1.53
        // Option 0
        EMRTextLayoutText,
        // Option 1
        EMRTextLayoutTextEditor,
        // Option 2
        EMRTextLayoutTimeEditorStart,
        EMRTextLayoutTimeEditorSeparator,
        EMRTextLayoutTimeEditorEnd,
        // Option 3
        EMRTextLayoutDateEditor,
        // Option 4
        EMRTextLayoutMultiRowTextEditor,
        // Option 5
        EMRTextLayoutCheckboxEditor,
        
        // Table 1.67
        // Option 0
        // Option 1
        // Option 2
        EMRLayoutCtrlBarTitle
    };
    
    
    /**
     * Returns TAknWindowComponentLayout for logical TMRLayout from xml laf.
     * 
     * @param aLayout logical mr component id
     * @return layout struct from laf
     */
    IMPORT_C TAknWindowComponentLayout GetWindowComponentLayout( TMRLayout aLayout );
    
    /**
     * Returns TAknTextComponentLayout for logical TMRTextLayout from xml laf.
     * 
     * @param aLayout logical mr component id
     * @return layout struct from laf
     */
    IMPORT_C TAknTextComponentLayout GetTextComponentLayout( TMRTextLayout aLayout );
    
    /**
     * Returns window component's rect from layout data.
     * 
     * @param aParentRect parent component's rect.
     * @param aLayout component enumeration.
     * @return layout rect of the component.
     */
    IMPORT_C TAknLayoutRect GetLayoutRect( const TRect& aParentRect, TMRLayout aLayout );
    
    /**
     * Returns field's size from layout data.
     * 
     * @param aParentRect parent component's rect.
     * @param aRows field row count.
     * @return layout rect of the field.
     */
    IMPORT_C TAknLayoutRect GetFieldLayoutRect( const TRect& aParentRect, TInt aRows );
    
    /**
     * Returns rect for one field's row.
     * 
     * @param aParentRect field's rect.
     * @param aRow row which's rect to return.
     * @return layout rect of field's row.
     */
    IMPORT_C TAknLayoutRect GetFieldRowLayoutRect( const TRect& aParentRect, TInt aRow );
    
    /**
     * Returns text component's rect from layout data.
     * 
     * @param aParentRect parent component's rect.
     * @param aLayout text component enumeration.
     * @return text layout rect of text component.
     */
    IMPORT_C TAknLayoutText GetLayoutText( const TRect& aParentRect, TMRTextLayout aLayout );
    
    }

#endif // NMRLAYOUTMANAGER_H

// EOF
