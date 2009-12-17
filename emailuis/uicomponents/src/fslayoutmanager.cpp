/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFsLayoutManager.
*
*/


#include "emailtrace.h"
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknLayoutFont.h>
#include <CdlRefs.h>
#include <apgcli.h>
#include <eikapp.h>
#include <layoutmetadata.cdl.h>
// <cmail> SF
#include <alf/alfstatic.h>
#include <alf/alfenv.h>
// </cmail>
#include <centralrepository.h>
#include <AvkonInternalCRKeys.h>

//<CMAIL> As a device user, I want Message List items to follow platform layouts to be consistent with other apps
#include <aknlayoutscalable_apps.cdl.h>
//</CMAIL>

//<cmail>
//A lot of stuff removed, dependencies to FSLayout REMOVED!!
//</cmail>

#include "fslayoutmanager.h"

// Layout file to use if a supported one wasn't found.
// <cmail> S60 UID update
//_LIT( KFsLayoutpackFile, "2001E284.dll" );
// </cmail> S60 UID update


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Release given pointer.
// ---------------------------------------------------------------------------
//
void CleanupLayoutPack( TAny* /*aPtr*/ )
	{
	}


// ======== MEMBER FUNCTIONS ========


EXPORT_C CFsLayoutManager::CFsLayoutManager()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Determine if the mirroring is in use.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsLayoutManager::IsMirrored()
    {
    FUNC_LOG;
    return Layout_Meta_Data::IsMirrored();
    }

// ---------------------------------------------------------------------------
// Fills given TRect with rectangle for given layout component. Returns EFalse
// for status pane descendants if requested layout component is not available
// in current layout. For other components returns always ETrue (returned
// rectangle is from layout definition).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsLayoutManager::LayoutMetricsRect(
    const TRect &aParent,
    TFsLayoutMetrics aParam,
    TRect& aRect,
    TInt aVariety,
    TInt aRow,
    TInt /*aCol*/ )
    {
    FUNC_LOG;

    //<CMAIL>

    TBool retVal( ETrue );
    TAknLayoutRect rect;

    switch ( aParam )
        {
        case EFsLmScreen:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::Screen().LayoutLine() );
            break;
        case EFsLmApplicationWindow:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::application_window().LayoutLine() );
            break;
        case EFsLmAreaTopPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::area_top_pane( aVariety ).LayoutLine() );
            break;
        case EFsLmBgSpFsCtrlbarPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::bg_sp_fs_ctrlbar_pane( aVariety ).LayoutLine() );
            break;
/*        case EFsLmListMediumLine:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line().LayoutLine() );
            break;*/
        case EFsLmListMediumLineG1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineG2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_g2_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineG2G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_g2_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineG3G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_g3_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineG3G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_g3_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineG3G3:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_g3_g3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineRightIconG1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_right_icon_g1().LayoutLine() );
            break;
        case EFsLmListMediumLineRightIconx2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_right_iconx2_g1().LayoutLine() );
            break;
        case EFsLmListMediumLineRightIconx2G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_right_iconx2_g2().LayoutLine() );
            break;
        case EFsLmListMediumLineT2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g2_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G2G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g2_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G3G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g3_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G3G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g3_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G3G3:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g3_g3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2RightIconG1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_right_icon_g1().LayoutLine() );
            break;
        case EFsLmListMediumLineT2RightIconx2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_right_iconx2_g1().LayoutLine() );
            break;
        case EFsLmListMediumLineT2RightIconx2G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_right_iconx2_g2().LayoutLine() );
            break;
        case EFsLmListMediumLineT3G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g2_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G2G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g2_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G3G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g3_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G3G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g3_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G3G3:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g3_g3( aVariety ).LayoutLine() );
            break;
        case EFsLmFolderListMediumLineG1: // Values for folderlist
		    rect.LayoutRect( aParent, AknLayoutScalable_Avkon::list_single_large_graphic_pane_g1( aVariety ).LayoutLine() );		      
            break;
        case EFsLmListMediumLineT3RightIconx2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_right_iconx2_g1().LayoutLine() );
            break;
        case EFsLmListMediumLineT3RightIconx2G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_right_iconx2_g2().LayoutLine() );
            break;
        case EFsLmListMediumLineX2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2().LayoutLine() );
            break;
        case EFsLmListMediumLineX2G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G3G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g3_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G3G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g3_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G3G3:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g3_g3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G4G1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g4_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G4G2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g4_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G4G3:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g4_g3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T4G4G4:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t4_g4_g4( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX3:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x3().LayoutLine() );
            break;
        case EFsLmListMediumLineX3T4G4G4:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_medium_line_x3_t4_g4_g4( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_pane( aVariety, 0, aRow ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowPaneG1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowPaneG2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_pane_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowPaneG3:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_pane_g3( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowPaneG4:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_pane_g4( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowTextPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleFsDycPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::list_single_fs_dyc_pane( aVariety ).LayoutLine() );
            break;
        case EFsLmMainPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_pane( aVariety ).LayoutLine() );
            break;
        case EFsLmMainSpFsCtrlbarButtonPaneCp01:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_button_pane_cp01().LayoutLine() );
            break;
        case EFsLmMainSpFsCtrlbarDdmenuPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane().LayoutLine() );
            break;
        case EFsLmMainSpFsCtrlbarDdmenuPaneG1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane_g1( aVariety ).LayoutLine() );
            break;
        case EFsLmMainSpFsCtrlbarDdmenuPaneG2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane_g2( aVariety ).LayoutLine() );
            break;
        case EFsLmMainSpFsCtrlbarPane:
            {
            const TInt variety( Layout_Meta_Data::IsPenEnabled() ? 2 : 0 );
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane( variety ).LayoutLine() );
            }
            break;
        case EFsLmMainSpFsCtrlbarPaneG1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g1().LayoutLine() );
            break;
        case EFsLmMainSpFsCtrlbarPaneG2:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g2().LayoutLine() );
            break;
        case EFsLmMainSpFsListscrollPaneTe:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::main_sp_fs_listscroll_pane_te().LayoutLine() );
            break;
        case EFsLmPopupSpFsActionMenuPane:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::popup_sp_fs_action_menu_pane( aVariety ).LayoutLine() );
            break;
        case EFsLmMainSpFsListPane:
		    rect.LayoutRect( aParent, AknLayoutScalable_Avkon::list_gen_pane( aVariety ).LayoutLine() );
            break;
        case EFsLmSpFsScrollPane:
		    rect.LayoutRect( aParent, AknLayoutScalable_Avkon::scroll_pane( aVariety ).LayoutLine() );
            break;
        case EFsLmSpFsActionMenuListGenePaneG1:
            rect.LayoutRect( aParent,
                AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane_g1( aVariety ).LayoutLine() );
            break;

        //</CMAIL>

        default:
            // This is error case
            retVal = EFalse;
            break;
        }

    if ( retVal )
        {
        aRect = rect.Rect();
        }

    return retVal;
    }


// ---------------------------------------------------------------------------
// This method returns size of rectangle for given layout component. Returns
// EFalse for status pane descendants if requested layout component is not
// available in current layout. For other components returns always ETrue
// (returned size is from layout definition).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsLayoutManager::LayoutMetricsSize(
    const TRect &aParent,
    TFsLayoutMetrics aParam,
    TSize& aSize,
    TInt aVariety,
    TInt aRow,
    TInt aCol )
	{
    FUNC_LOG;
    TRect rect;
	TBool retVal(
	    LayoutMetricsRect( aParent, aParam, rect, aVariety, aRow, aCol ) );
	aSize = rect.Size();
	return retVal;
	}


// ---------------------------------------------------------------------------
// This method returns position of top left corner for given layout component.
// Returns EFalse for status pane descendants if requested layout component is
// not available in current layout. For other components returns always ETrue
// (returned position is from layout definition).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsLayoutManager::LayoutMetricsPosition(
    const TRect &aParent,
    TFsLayoutMetrics aParam,
    TPoint& aPos,
    TInt aVariety,
    TInt aRow,
    TInt aCol )
	{
    FUNC_LOG;
	TRect rect;
	TBool retVal(
	    LayoutMetricsRect( aParent, aParam, rect, aVariety, aRow, aCol ) );
	aPos = rect.iTl;
	return retVal;
	}

// ---------------------------------------------------------------------------
// This method returns text position and font information of given layout
// component. Returns EFalse for status pane descendants if requested layout
// component is not available in current layout. For other components returns
// always ETrue (returned position is from layout definition).
// ---------------------------------------------------------------------------
//
/*
TRect ListSingleDycRow( const TRect& aParent, TInt aRows, TInt aRow )
    {
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(aParent, AknLayoutScalable_Apps::list_single_dyc_row_pane(aRows - 1, 0, aRow - 1).LayoutLine());
    return layoutRect.Rect();
    }*/

// ---------------------------------------------------------------------------
// This method returns text position and font information of given layout
// component. Returns EFalse for status pane descendants if requested layout
// component is not available in current layout. For other components returns
// always ETrue (returned position is from layout definition).
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsLayoutManager::LayoutMetricsText(
    const TRect &aParent,
    TFsLayoutMetrics aParam,
    TFsText& aText,
    TInt aVariety,
    TInt /*aRow*/ )
    {
    FUNC_LOG;

    //<CMAIL>

    TBool retVal( ETrue );
    TAknLayoutText textLayout;

    switch ( aParam )
        {
        case EFsLmListMediumLineG2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_g2_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2RightIconT1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_right_icon_t1().LayoutLine() );
            break;
        case EFsLmListMediumLineT2RightIconT2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_right_icon_t2().LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G3T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g3_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G3T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g3_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g2_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G2T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g2_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T2G4T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t2_g4_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T2G3T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t2_g3_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T2G2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t2_g2_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3T3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_t3().LayoutLine() );
            break;
        case EFsLmListMediumLineT4G4T3:
        /*
            TAknLayoutRect textParent;
            textParent.LayoutRect(ListSingleDycRow(aParent, 3, 3),
                    AknLayoutScalable_Apps::list_single_dyc_row_text_pane( 5 ).LayoutLine());
            textLayout.LayoutText( textParent.Rect(),
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t1( 5 ).LayoutLine() ); //list_medium_line_t4_g4_t3*/
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t4_g4_t3( aVariety ).LayoutLine() ); //list_medium_line_t4_g4_t3*/
            break;
        case EFsLmListMediumLineX3T4G4T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x3_t4_g4_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX3T4G4T3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x3_t4_g4_t3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX3T4G4T4:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x3_t4_g4_t4( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3RightIconx2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_right_iconx2_t1().LayoutLine() );
            break;
        case EFsLmListMediumLineT3RightIconx2T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_right_iconx2_t2().LayoutLine() );
            break;
        case EFsLmListMediumLineT2RightIconx2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_right_iconx2_t1().LayoutLine() );
            break;
        case EFsLmListMediumLineT2RightIconx2T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_right_iconx2_t2().LayoutLine() );
            break;
        case EFsLmListMediumLineRightIconx2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_right_iconx2_t1().LayoutLine() );
            break;
        case EFsLmListMediumLineT3G3T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g3_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G3T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g3_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G3T3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g3_t3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G3T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g3_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G3T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g3_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineG3T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_g3_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLinePlainT1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_plain_t1().LayoutLine() );
            break;
        case EFsLmListMediumLineT3T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3T3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_t3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineRightIconT1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_right_icon_t1().LayoutLine() );
            break;
        case EFsLmListMediumLineT3G2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g2_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G2T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g2_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT3G2T3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t3_g2_t3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g2_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineT2G2T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t2_g2_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G4T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g4_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G4T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g4_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLineX2T3G4T3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_x2_t3_g4_t3( aVariety ).LayoutLine() );
            break;
        case EFsLmListMediumLinePlainT3T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_plain_t3_t1().LayoutLine() );
            break;
        case EFsLmListMediumLinePlainT3T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_plain_t3_t2().LayoutLine() );
            break;
        case EFsLmListMediumLinePlainT3T3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_plain_t3_t3().LayoutLine() );
            break;
        case EFsLmListMediumLinePlainT2T1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_plain_t2_t1().LayoutLine() );
            break;
        case EFsLmListMediumLinePlainT2T2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_plain_t2_t2().LayoutLine() );
            break;
        case EFsLmListMediumLineT1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_medium_line_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmFolderListMediumLineT1: // Values for folderlist
		    textLayout.LayoutText( aParent, AknLayoutScalable_Avkon::list_single_large_graphic_pane_t1( aVariety ).LayoutLine() );		      		                
            break;
        case EFsLmMainSpFsCtrlbarDdmenuPaneT1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowTextPaneT1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t1( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowTextPaneT2:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t2( aVariety ).LayoutLine() );
            break;
        case EFsLmListSingleDycRowTextPaneT3:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t3( aVariety ).LayoutLine() );
            break;
        case EFsLmSpFsActionMenuListGenePaneT1:
            textLayout.LayoutText( aParent,
                AknLayoutScalable_Apps::sp_fs_action_menu_list_gene_pane_t1( aVariety ).LayoutLine() );
            break;

        //</CMAIL>


        default:
            retVal = EFalse;
            break;
        }

    if ( retVal )
        {
        aText.iTextRect = textLayout.TextRect();
        aText.iFont = textLayout.Font();
        //aText.iColor = textLayout.Color();
        const CAknLayoutFont* layoutFont(
            CAknLayoutFont::AsCAknLayoutFontOrNull( aText.iFont ) );
        //TInt textPaneHeight( 0 );
        TInt textPaneAscent( 0 );
        if ( layoutFont )
            {
            textPaneAscent = layoutFont->TextPaneTopToBaseline();
            }
        else
            {
            textPaneAscent = aText.iFont->AscentInPixels();
            }
        aText.iOffset = textPaneAscent;
        aText.iAlign = textLayout.Align();
        }

    return retVal;
    }

