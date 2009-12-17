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

#include "nmrlayoutmanager.h"

#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>


using namespace NMRLayoutManager;


// Unnamed namespace for local definitions
namespace { // codescanner::unnamednamespace

    const TInt KVarietyZero = 0;

    // Numerals for layout data usage.
    const TInt KLayoutValueZero = 0;
    const TInt KLayoutValueOne = 1;
    const TInt KLayoutValueTwo = 2;
    const TInt KLayoutValueThree = 3;
    const TInt KLayoutValueFour = 4;
    const TInt KLayoutValueFive = 5;
    
#ifdef _DEBUG
    _LIT( KNMRLayoutManagerPanicText, "NMRLayoutManager" );
    enum TNMRLayoutManagerPanic
        {
        ENMRLayoutManagerUnhandledCase = 1,
        };
    void Panic( TNMRLayoutManagerPanic aPanic )
        {
        User::Panic( KNMRLayoutManagerPanicText, aPanic );
        }
#endif
    
} // Unnamed namespace


// ---------------------------------------------------------------------------
// NMRLayoutManager::GetWindowComponentLayout
// ---------------------------------------------------------------------------
//
EXPORT_C TAknWindowComponentLayout NMRLayoutManager::GetWindowComponentLayout( TMRLayout aLayout )
    {
    TAknWindowComponentLayout layout;
    switch ( aLayout )
        {
        case EMRLayoutDialog:
            {
            layout = AknLayoutScalable_Apps::main_sp_fs_calendar_pane( KLayoutValueZero );
            break;
            }
        case EMRLayoutDialogBg:            
            {
            layout = AknLayoutScalable_Apps::bg_main_sp_fs_cale_pane( KLayoutValueOne );
            break;
            }
        case EMRLayoutDialogBgGraphics:            
            {
            layout = AknLayoutScalable_Apps::bgmain_sp_fs_calendar_pane_g1( KLayoutValueZero );
            break;
            }
        case EMRLayoutCtrlBar:
            {
            layout = AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_cp01( KLayoutValueOne );
            break;
            }
        case EMRLayoutCtrlBarGraphics:
            {
            layout = AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g1( KLayoutValueOne );
            break;
            }
        case EMRLayoutContentArea:
            {
            layout = AknLayoutScalable_Apps::listscroll_cale_mrui_pane( KLayoutValueOne );
            break;
            }
        case EMRLayoutListArea:
            {
            layout = AknLayoutScalable_Avkon::list_gen_pane( KVarietyZero ).LayoutLine();
            break;
            }
        case EMRLayoutScrollBar:
            {
            layout = AknLayoutScalable_Avkon::scroll_pane( KVarietyZero ).LayoutLine();
            break;
            }
        case EMRLayoutTextEditorIcon:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_g1( KLayoutValueOne );
            break;
            }
        case EMRLayoutTextEditorBg:
            {
            layout = AknLayoutScalable_Apps::bg_main_sp_fs_cale_pane( KLayoutValueOne );
            //layout = AknLayoutScalable_Apps::list_single_cmail_header_editor_pane_bg_cp01( KLayoutValueOne );
            break;
            }
        case EMRLayoutTimeEditorIcon:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_g1( KLayoutValueTwo );
            break;
            }
        case EMRLayoutTimeEditorStartBg:
            {
            layout = AknLayoutScalable_Apps::list_single_cmail_header_editor_pane_bg_cp01( KLayoutValueTwo );
            break;
            }
        case EMRLayoutTimeEditorEndBg:
            {
            layout = AknLayoutScalable_Apps::list_single_cmail_header_editor_pane_bg_cp02( KLayoutValueTwo );
            break;
            }
        case EMRLayoutDateEditorIcon:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_g1( KLayoutValueThree );
            break;
            }
        case EMRLayoutDateEditorBg:
            {
            layout = AknLayoutScalable_Apps::list_single_cmail_header_editor_pane_bg_cp01( KLayoutValueThree );
            break;
            }
        case EMRLayoutCheckboxIcon:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_g1( KLayoutValueFive );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( ENMRLayoutManagerUnhandledCase ) );
            break;
            }
        }
    return layout;
    }

// ---------------------------------------------------------------------------
// NMRLayoutManager::GetTextComponentLayout
// ---------------------------------------------------------------------------
//
EXPORT_C TAknTextComponentLayout NMRLayoutManager::GetTextComponentLayout( TMRTextLayout aLayout )
    {
    TAknTextComponentLayout layout;
    switch ( aLayout )
        {
        case EMRTextLayoutText:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t1( KLayoutValueZero );
            break;
            }
        case EMRTextLayoutTextEditor:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t2( KLayoutValueOne );
            break;
            }
        case EMRTextLayoutTimeEditorStart:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t2( KLayoutValueTwo );
            break;
            }
        case EMRTextLayoutTimeEditorSeparator:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t3( KLayoutValueTwo );
            break;
            }
        case EMRTextLayoutTimeEditorEnd:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t4( KLayoutValueTwo );
            break;
            }
        case EMRTextLayoutDateEditor:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t2( KLayoutValueThree );
            break;
            }
        case EMRTextLayoutMultiRowTextEditor:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t2( KLayoutValueFour );
            break;
            }
        case EMRTextLayoutCheckboxEditor:
            {
            layout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_t2( KLayoutValueFive );
            break;
            }
        case EMRLayoutCtrlBarTitle:
            {
            layout = AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_t1( KLayoutValueZero );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( ENMRLayoutManagerUnhandledCase ) );
            break;
            }
        }
    return layout;
    }

// ---------------------------------------------------------------------------
// NMRLayoutManager::GetLayoutRect
// ---------------------------------------------------------------------------
//
EXPORT_C TAknLayoutRect NMRLayoutManager::GetLayoutRect( const TRect& aParentRect, TMRLayout aLayout )
    {
    TAknLayoutRect layoutRect;
    TAknWindowComponentLayout layout = GetWindowComponentLayout( aLayout );
    layoutRect.LayoutRect( aParentRect, layout );
    return layoutRect;
    }

// ---------------------------------------------------------------------------
// NMRLayoutManager::GetFieldLayoutRect
// ---------------------------------------------------------------------------
//
EXPORT_C TAknLayoutRect NMRLayoutManager::GetFieldLayoutRect( const TRect& aParentRect, TInt aRows )
    {
    TAknWindowComponentLayout fieldLayout;
    
    TAknLayoutScalableParameterLimits limits = 
        AknLayoutScalable_Apps::list_single_cale_mrui_pane_ParamLimits( 0 );
    
    // Map aRows to lower variety limit
    TInt variety = limits.FirstVariety() + ( aRows - 1 );
    // Make sure  min <= variety <= max
    variety = Min( limits.LastVariety(), Max( limits.FirstVariety(), variety ) );
    
    fieldLayout = AknLayoutScalable_Apps::list_single_cale_mrui_pane( variety );    
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( aParentRect, fieldLayout );
    return layoutRect;
    }

// ---------------------------------------------------------------------------
// NMRLayoutManager::GetFieldRowLayoutRect
// ---------------------------------------------------------------------------
//
EXPORT_C TAknLayoutRect NMRLayoutManager::GetFieldRowLayoutRect( const TRect& aParentRect, TInt aRow )
    {
    TAknWindowComponentLayout rowLayout;
    
    TAknLayoutScalableParameterLimits limits = 
        AknLayoutScalable_Apps::list_single_cale_mrui_row_pane_ParamLimits( 0 );
    
    // Map aRows to lower variety limit
    TInt variety = limits.FirstVariety() + ( aRow - 1 );
    // Make sure  min <= variety <= max
    variety = Min( limits.LastVariety(), Max( limits.FirstVariety(), variety ) );
    
    rowLayout = AknLayoutScalable_Apps::list_single_cale_mrui_row_pane( variety );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( aParentRect, rowLayout );
    return layoutRect;
    }

// ---------------------------------------------------------------------------
// NMRLayoutManager::GetLayoutText
// ---------------------------------------------------------------------------
//
EXPORT_C TAknLayoutText NMRLayoutManager::GetLayoutText( const TRect& aParentRect, TMRTextLayout aLayout )
    {
    TAknLayoutText layoutText;
    TAknTextComponentLayout layout = GetTextComponentLayout( aLayout );
    layoutText.LayoutText( aParentRect, layout );
    return layoutText;
    }

// EOF
