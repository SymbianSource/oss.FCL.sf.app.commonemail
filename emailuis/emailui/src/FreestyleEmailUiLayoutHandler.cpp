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
* Description:  FreestyleEmailUi layout data handler implementation
*
*/


// SYSTEM INCLUDE FILES
#include "emailtrace.h"
#include <alf/alftextstylemanager.h>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <alf/alfstatic.h>

#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <fslayoutmanager.h>

// For generic treelist layout data setting
#include "fstreelist.h"

// INTERNAL INCLUDE FILES
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiLayoutData.h"

const TInt KPopupMargin = 8;

// -----------------------------------------------------------------------------
// GeneralHash
// -----------------------------------------------------------------------------
//
template< typename T >
TUint32 GeneralHash( const T& aValue )
    {
    return DefaultHash::Des8( TPckgC< T >( aValue ) );
    }

// -----------------------------------------------------------------------------
// GeneralPtrIdentity
// -----------------------------------------------------------------------------
//
template< typename T >
TBool GeneralPtrIdentity( const T& aP1, const T& aP2 )
    {
    return TPckgC< T >( aP1 ) == TPckgC< T >( aP2 );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::CFSEmailUiLayoutHandler
// Base construction. May not leave.
// -----------------------------------------------------------------------------
//
CFSEmailUiLayoutHandler::CFSEmailUiLayoutHandler( CAlfEnv& aEnv )
	: iEnv(aEnv),
	iTextStyleMap( THashFunction32< TAknTextComponentLayout >( GeneralHash ),
	    TIdentityRelation< TAknTextComponentLayout >( GeneralPtrIdentity< TAknTextComponentLayout > ) )
 	{
    FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::NewL
// First phase construction function.
// -----------------------------------------------------------------------------
//	
CFSEmailUiLayoutHandler* CFSEmailUiLayoutHandler::NewL( CAlfEnv& aEnv )
	{
    FUNC_LOG;
	CFSEmailUiLayoutHandler* self = new ( ELeave ) CFSEmailUiLayoutHandler( aEnv );
	CleanupStack::PushL( self );
 	self->ConstructL();
	CleanupStack::Pop( self );	
	return self;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ConstructL
// Symbian second phase construction. May leave.
// -----------------------------------------------------------------------------
//	
void CFSEmailUiLayoutHandler::ConstructL()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::~CFSEmailUiLayoutHandler
// D'tor
// -----------------------------------------------------------------------------
//
CFSEmailUiLayoutHandler::~CFSEmailUiLayoutHandler()
    {
    FUNC_LOG;
    DeleteTextStyles();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ScreenOrientation
// Function can be used to fetch orientation of the current screen resolution
// -----------------------------------------------------------------------------
//  
TScreenOrientation CFSEmailUiLayoutHandler::ScreenOrientation() const
    {
    FUNC_LOG;
    return Layout_Meta_Data::IsLandscapeOrientation() ? ELandscape : EPortrait;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GridRowsInThisResolution
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::GridRowsInThisResolution() const
	{
    FUNC_LOG;
    const TInt var( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );
    return AknLayoutScalable_Apps::cell_cmail_l_pane_ParamLimits( var ).LastRow() + 1;
	}


// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GridColumnsInThisResolution
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::GridColumnsInThisResolution() const
	{
    FUNC_LOG;
    const TInt var( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );
    return AknLayoutScalable_Apps::cell_cmail_l_pane_ParamLimits( var ).LastColumn() + 1;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GridIconSize
// -----------------------------------------------------------------------------
//
TRgb CFSEmailUiLayoutHandler::GridNormalStateTextSkinColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    // Should use EAknsCIFsTextColorsCG3 if background is white
     if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
        textColor = KRgbBlack;
        }
    return textColor;	
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GridIconSize
// -----------------------------------------------------------------------------
//
TSize CFSEmailUiLayoutHandler::GridIconSize() const
	{
    FUNC_LOG;
// <cmail> Use layout data instead of hard-coded values
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

    TAknLayoutRect scrollBarRect;
    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());
    TRect gridRect = mainPaneRect;
    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    TAknLayoutRect itemRect;
    itemRect.LayoutRect(gridRect, AknLayoutScalable_Apps::cell_cmail_l_pane(var,0,0));

    TAknLayoutRect gridIconLRect;
    gridIconLRect.LayoutRect( itemRect.Rect(), AknLayoutScalable_Apps::cell_cmail_l_pane_g1( 0 ) );
    TSize iconSize = gridIconLRect.Rect().Size();
    return iconSize;
	}

	
TSize CFSEmailUiLayoutHandler::MailIconSizeInThisResolution() const
	{
    FUNC_LOG;
	TAknLayoutRect rect;
	rect.LayoutRect( TRect( 0, 0, 0, 0 ),
	                AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ).LayoutLine() );
	return rect.Rect().Size();
	}


// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::SetListMarqueeBehaviour
// Function sets FS Email UI default values for marquee behavioutr for treelist object
// -----------------------------------------------------------------------------
//
void CFSEmailUiLayoutHandler::SetListMarqueeBehaviour( CFsTreeList* aTreeList ) const
	{
    FUNC_LOG;
	if ( aTreeList ) // Safety check
		{
		aTreeList->SetTextMarqueeType( KListTextMarqueeType );					
		aTreeList->SetTextMarqueeSpeed ( KListTextMarqueeSpeed );
		aTreeList->SetTextMarqueeStartDelay ( KListTextMarqueeStartDelay );
		aTreeList->SetTextMarqueeRepetitions ( KListTextMarqueeSpeedRepeats );  
		}	
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::FolderListIconSizeInThisResolution
// Folder list icon size, calculated based on the item size
// -----------------------------------------------------------------------------
//
TSize CFSEmailUiLayoutHandler::FolderListIconSize( const TBool aPopup ) const
    {
    FUNC_LOG;
    if ( aPopup )
        {
        TAknWindowComponentLayout g1Layout( TAknWindowComponentLayout::Compose(
                AknLayoutScalable_Apps::list_single_dyc_row_pane( 0 ),
                AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ) ) );
        TAknLayoutRect paneG1;
        paneG1.LayoutRect( DropDownMenuListRect( ELeft, EFalse ), g1Layout.LayoutLine() );
        return paneG1.Rect().Size();        
        }
    else
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        
        TAknLayoutRect scrollBarRect;
        scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::scroll_pane());
        TRect gridRect = mainPaneRect;
        gridRect.iBr.iX -= scrollBarRect.Rect().Width();

        TAknLayoutRect itemRect;
        itemRect.LayoutRect( gridRect, AknLayoutScalable_Avkon::list_single_large_graphic_pane_g1( 0 ).LayoutLine() );

        return itemRect.Rect().Size();
        }
    }      

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::FolderListPaneRectInThisResolution
// Folder list pane rect, taken from Avkon layouts
// -----------------------------------------------------------------------------
//
TRect CFSEmailUiLayoutHandler::FolderListPaneRectInThisResolution( const TRect& aParent) const
    {
    FUNC_LOG;
    TAknLayoutRect itemRect;
    itemRect.LayoutRect(aParent, AknLayoutScalable_Avkon::listscroll_gen_pane(0,0,0));
    return itemRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::FolderListIndentation
// Folder list pane indentation in pixels, taken from Avkon layouts
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::FolderListIndentation( const TRect& aParent ) const
    {
    FUNC_LOG;
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( aParent,
        AknLayoutScalable_Avkon::aid_indentation_list_msg().LayoutLine() );
    return layoutRect.Rect().Width();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::FolderListItemSizeInThisResolution
// Folder list item size, taken from Avkon layouts
// -----------------------------------------------------------------------------
//
TSize CFSEmailUiLayoutHandler::FolderListItemSizeInThisResolution( const TRect& aParent ) const
	{
    FUNC_LOG;
    TAknLayoutRect itemRect;
     itemRect.LayoutRect( aParent, // Only zero variety with this one
     	AknLayoutScalable_Avkon::list_single_large_graphic_pane( 0, 0 ,0 ) );

    return itemRect.Rect().Size();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::DropDownMenuListRect
// Selector ring image visual size
// -----------------------------------------------------------------------------
//
TRect CFSEmailUiLayoutHandler::DropDownMenuListRect(
        const TDropDownMenuSide aSide, TBool aStretch ) const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
            mainPaneRect );
    mainPaneRect.Move( 0, -mainPaneRect.iTl.iY );
    TInt variety( aSide == ELeft ? 0 : 1 );
    TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();
    if ( landscape )
        {
        // landscape orientation variety is portrait + 2
        variety += 2;
        }
    
    TAknWindowComponentLayout ddMenuPane( 
            TAknWindowComponentLayout::Compose(
                AknLayoutScalable_Apps::main_sp_fs_email_pane(),
                AknLayoutScalable_Apps::popup_sp_fs_action_menu_pane_cp01( variety ) 
                ) );
    
    TAknLayoutRect menuPane;
    menuPane.LayoutRect( mainPaneRect, ddMenuPane.LayoutLine() );
    TRect rect = menuPane.Rect();
    if ( aStretch )
        {
        rect.iBr.iY = mainPaneRect.iBr.iY;
        
        if( !landscape ) // landscape layout occupy the right side of the display
            {
            rect.iBr.iY -= ControlBarHeight();
            }
        }
    
    // Calculate the placement next to the button (below or left side)
    TRect buttonRect;
    TRect cbr = GetControlBarRect();
    if( aSide == ELeft )
        {
        buttonRect = GetControlBarFolderListButtonRect();
        }
    else
        {
        buttonRect = GetControlBarSortButtonRect();
        }
    buttonRect.Move( cbr.iTl ); // relative to control bar
    
    if( landscape )
        {
        TSize menuSize = rect.Size();
        if ( !CFsLayoutManager::IsMirrored() )
           {
        	// position left from the button
            rect.SetRect( TPoint( buttonRect.iTl.iX - menuSize.iWidth, 
                buttonRect.Center().iY - menuSize.iHeight / 2 ), menuSize );
           }
        else
           {
           // position right from the button
           rect.SetRect( TPoint( buttonRect.Width(), 
                       buttonRect.Center().iY - menuSize.iHeight / 2 ), menuSize );
           }
        }
    else
        {
        // position below the button
        rect.Move( 0, buttonRect.iBr.iY - rect.iTl.iY );
        }
    
    // keep the popup menu inside the screen area
    if( landscape )
        {
        if( rect.iTl.iX < KPopupMargin )
            {
            rect.Move( KPopupMargin - rect.iTl.iX , 0 );
            }
        if( rect.iBr.iY > mainPaneRect.iBr.iY - KPopupMargin )
            {
            rect.Move( 0, mainPaneRect.iBr.iY - rect.iBr.iY - KPopupMargin );
            }
        }
    if( rect.iTl.iY < KPopupMargin )
        {
        rect.iTl.iY = KPopupMargin;
        }
    return rect;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::OneLineListNodeHeight
// List node item height 
// -----------------------------------------------------------------------------
//	
TInt CFSEmailUiLayoutHandler::OneLineListNodeHeight() const
	{
    FUNC_LOG;
    TRect mainPaneRect;
    TAknLayoutRect listItem;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    listItem.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::list_single_fs_dyc_pane( 0 ) );
    return listItem.Rect().Height();
	}
	
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::OneLineListItemHeight
// List item height
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::OneLineListItemHeight() const
	{
    FUNC_LOG;
    TRect mainPaneRect;
    TAknLayoutRect listItem;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    listItem.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::list_single_fs_dyc_pane( 0 ) );
    return listItem.Rect().Height();
	}

TInt CFSEmailUiLayoutHandler::TwoLineListItemHeight() const
    {
    TRect mainPaneRect;
    TAknLayoutRect listItem;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    listItem.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::list_single_fs_dyc_pane( 1 ) );
    return listItem.Rect().Height();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::FolderListOneLineItemHeight
// List item height for folder list
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::FolderListOneLineItemHeight( 
        const TBool& aFullscreen ) const
    {
    FUNC_LOG;
    if ( aFullscreen )
        {        
        return OneLineListItemHeight();
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarHeight
// Control bar height
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::ControlBarHeight() const
	{
    FUNC_LOG;
	TRect mainPaneRect;
    TAknLayoutRect ctrlBarRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    ctrlBarRect.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane( 0 ) );
    return ctrlBarRect.Rect().Height();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarListPadding
// -----------------------------------------------------------------------------
TPoint CFSEmailUiLayoutHandler::ControlBarListPadding() const
	{
    FUNC_LOG;
	return KControlBarListPadding;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarListBorderRoundingSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ControlBarListBorderRoundingSize() const
	{
    FUNC_LOG;
	return KControlBarListBorderRoundingSize;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarButtonBgRoundingSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ControlBarButtonBgRoundingSize() const
	{
    FUNC_LOG;
	return KControlBarbuttonBorderRoundingSize;	
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListFadeInEffectTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListFadeInEffectTime() const
	{
    FUNC_LOG;
	return KListFadeInEffectTime;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListFadeOutEffectTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListFadeOutEffectTime() const
    {
    FUNC_LOG;
    return KListFadeOutEffectTime;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::StatusIndicatorFadeEffectTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::StatusIndicatorFadeEffectTime() const
	{
    FUNC_LOG;
	return KStatusIndicatorFadeEffectTime;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::CtrlBarListFadeEffectTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::CtrlBarListFadeEffectTime() const
	{
    FUNC_LOG;
	return KCtrlBarListFadeEffectTime;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewSlideEffectActive
// -----------------------------------------------------------------------------
TBool CFSEmailUiLayoutHandler::ViewSlideEffectActive() const
	{
    FUNC_LOG;
	return KListSlideEffectActive;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewFadeEffectActive
// -----------------------------------------------------------------------------
TBool CFSEmailUiLayoutHandler::ViewFadeEffectActive() const
	{
    FUNC_LOG;
	return KListFadeEffectActive;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewSlideEffectTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewSlideEffectTime() const
	{
    FUNC_LOG;
	return KListSlideEffectTime;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewFadeInEffectTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewFadeInEffectTime() const
    {
    FUNC_LOG;
    return KListFadeInEffectTime;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewFadeOutEffectTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewFadeOutEffectTime() const
    {
    FUNC_LOG;
    return KListFadeOutEffectTime;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListScrollingTime
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListScrollingTime() const
	{
    FUNC_LOG;
	return KListScrollingTime;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListItemExpansionDelay
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListItemExpansionDelay() const
	{
    FUNC_LOG;
	return KListItemExpansionDelay;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListFocusedStateTextSkinColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ListFocusedStateTextSkinColor() const
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if( AknsUtils::GetCachedColor( skin, textColor,
    		KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 ) != KErrNone )
    	{
        textColor = KRgbBlack;
    	}
    return textColor;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListNormalStateTextSkinColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ListNormalStateTextSkinColor() const
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if ( AknsUtils::GetCachedColor( skin, textColor,
    	KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
    	{
        textColor = KRgbBlack;
    	}
    return textColor;
    }
	
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListNodeBackgroundColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ListNodeBackgroundColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb nodeBgColor;
    if ( AknsUtils::GetCachedColor( skin, nodeBgColor,
            KAknsIIDFsOtherColors, EAknsCIFsOtherColorsCG19 ) != KErrNone )
        {
        nodeBgColor = KRgbDarkGray;
        }
    return nodeBgColor;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListNodeTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ListNodeTextColor() const
	{
    FUNC_LOG;
   MAknsSkinInstance* skin = AknsUtils::SkinInstance();
   TRgb nodeTextColor;
    if ( AknsUtils::GetCachedColor( skin, nodeTextColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
    	{
        nodeTextColor = KRgbWhite;
        }
    return nodeTextColor;	
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListHeaderBackgroundColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ListHeaderBackgroundColor() const
    {
    FUNC_LOG;
    // The header item bg color is gained by mixing 50/50 node bg color
    // and list item bg color.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb listItemBgColor;
    if ( AknsUtils::GetCachedColor( skin, listItemBgColor,
        KAknsIIDQsnLineColors, EAknsCIFsTextColorsCG3 ) != KErrNone )
        {
        listItemBgColor = KRgbWhite;
        }
    TRgb nodeItemBgColor = ListNodeBackgroundColor(); 
    TRgb headerBgColor;
    headerBgColor.SetRed( (listItemBgColor.Red()+nodeItemBgColor.Red()) / 2 );
    headerBgColor.SetGreen( (listItemBgColor.Green()+nodeItemBgColor.Green()) / 2 );
    headerBgColor.SetBlue( (listItemBgColor.Blue()+nodeItemBgColor.Blue()) / 2 );
    return headerBgColor;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::DropdownMenuTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::DropdownMenuTextColor() const
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
         textColor = KRgbBlack;
        }
    return textColor;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListItemFontHeightInTwips
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListItemFontHeightInTwips( const TBool aPopup ) const
	{
    FUNC_LOG;
    if ( aPopup )
        {
        TRect mainPaneRect;
        TAknLayoutText itemText;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        itemText.LayoutText( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane_t1( 0 ) );
        return itemText.Font()->FontSpecInTwips().iHeight;        
        }
    else
        {
        TRect mainPaneRect;
        TAknLayoutText itemText;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        itemText.LayoutText( mainPaneRect, AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t1( 0 ) );
        return itemText.Font()->FontSpecInTwips().iHeight;
        }
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTitleFontAknLogicalFontId
// -----------------------------------------------------------------------------
TAknLogicalFontId CFSEmailUiLayoutHandler::ViewerTitleFontAknLogicalFontId() const
	{
    FUNC_LOG;
	return KViewerLogicalTitleFontId;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTextFontAknLogicalFontId
// -----------------------------------------------------------------------------
TAknLogicalFontId CFSEmailUiLayoutHandler::ViewerTextFontAknLogicalFontId() const
	{
    FUNC_LOG;
	return KViewerLogicalTextFontId;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTitleFontHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerTitleFontHeight() const
	{
	FUNC_LOG;
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText layoutText;
    layoutText.LayoutText( labelRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return layoutText.Font()->FontMaxHeight();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTextFontHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerTextFontHeight() const
	{
	FUNC_LOG;
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    TAknLayoutText layoutText;
    layoutText.LayoutText( labelRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( 0 ) );
    return layoutText.Font()->FontMaxHeight();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTopMostSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerTopMostSpaceHeight() const
	{
	FUNC_LOG;
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane( 0 ) );
    return labelRect.Rect().Height();
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerRecipientSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerRecipientSpaceHeight() const
	{
	FUNC_LOG;
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    return labelRect.Rect().Height();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerHeaderInfoSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerHeaderInfoSpaceHeight() const
	{
	FUNC_LOG;
	return ViewerTopMostSpaceHeight();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerBottomMostSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerBottomMostSpaceHeight() const
	{
	FUNC_LOG;
	return ViewerRecipientSpaceHeight();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerMaxRecipientLineCount
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerMaxRecipientLineCount() const
    {
    FUNC_LOG;
    return KViewerMaxRecipientLineCount;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerMaxSubjectLineCount
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerMaxSubjectLineCount() const
    {
    FUNC_LOG;
    return KViewerMaxSubjectLineCount;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTitleTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ViewerTitleTextColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if (AknsUtils::GetCachedColor( skin, textColor,
		KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
        textColor = KBlackFallbackTextColor;
        }
    return textColor;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTextTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ViewerTextTextColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if (AknsUtils::GetCachedColor( skin, textColor,
		KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
    	{
        textColor = KBlackFallbackTextColor;
        }
    return textColor;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerBodyTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ViewerBodyTextColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG13 ) != KErrNone )
        {
        textColor = KBlackFallbackTextColor;
        }
    return textColor;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerNormalHotspotTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ViewerNormalHotspotTextColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
	if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDQsnHighlightColors,
		EAknsCIQsnHighlightColorsCG3 ) != KErrNone )
		{
        textColor = KLightBlueFallbackTextColor;
        }
    return textColor;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerHighlightedHotspotTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ViewerHighlightedHotspotTextColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
	if ( AknsUtils::GetCachedColor( skin, textColor,
			KAknsIIDQsnHighlightColors, EAknsCIQsnHighlightColorsCG3 ) 
                != KErrNone )
		{
        textColor = KLightBlueFallbackTextColor;
        }
    return textColor;
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerSentTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ViewerSentTextColor() const
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDFsTextColors, EAknsCIFsTextColorsCG8 ) != KErrNone )
        {
        textColor = KBlackFallbackTextColor;
        }
    return textColor;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerPixelsBetweenMsgStatusIconAndSubject
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerPixelsBetweenMsgStatusIconAndSubject() const
	{
	FUNC_LOG;
    const TRect cmailPaneRect( ListCmailPaneRect() );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( cmailPaneRect, AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    TAknLayoutRect iconRect;
    iconRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g1( 0 ) );
    if (AknLayoutUtils::LayoutMirrored())
        {
        return cmailPaneRect.iBr.iX - iconRect.Rect().iBr.iX;
        }
    else
        {
        return Abs( cmailPaneRect.iTl.iX - iconRect.Rect().iTl.iX );
        }
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::ViewerIconSize() const
	{
	FUNC_LOG;
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g1( 2 ) );
    return layoutRect.Rect().Size();
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerIndentationIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::ViewerIndentationIconSize() const
	{
	FUNC_LOG;
	return ViewerIconSize();
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerActionMenuIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::ViewerActionMenuIconSize() const
    {
	FUNC_LOG;
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g2( 0 ) );
    return layoutRect.Rect().Size();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerActionMenuIconMargin
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerActionMenuIconMargin() const
    {
	FUNC_LOG;
	if (AknLayoutUtils::LayoutMirrored())
	    {
	    TAknLayoutRect layoutRect;
        layoutRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
        TInt x( layoutRect.Rect().iTl.iX );
        layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g2( 0 ) );
        return Abs( layoutRect.Rect().iTl.iX - x );
	    }
	else
	    {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
        TInt x( layoutRect.Rect().iBr.iX );
        layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g2( 0 ) );
        return Abs( x - layoutRect.Rect().iBr.iX );
	    }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerSelectorOpacity
// -----------------------------------------------------------------------------
TReal CFSEmailUiLayoutHandler::ViewerSelectorOpacity() const
    {
    FUNC_LOG;
    return KViewerSelectorOpacity;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerRightMarginInPixels
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerRightMarginInPixels() const
	{
	FUNC_LOG;
    const TRect cmailPaneRect( ListCmailPaneRect() );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( cmailPaneRect, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText captionTextRect;
    captionTextRect.LayoutText( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return Abs( cmailPaneRect.iBr.iX - captionTextRect.TextRect().iBr.iX );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerLeftMarginInPixels
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerLeftMarginInPixels() const
	{
	FUNC_LOG;
    const TRect cmailPaneRect( ListCmailPaneRect() );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( cmailPaneRect, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText captionTextRect;
    captionTextRect.LayoutText( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return Abs( cmailPaneRect.iTl.iX - captionTextRect.TextRect().iTl.iX );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ComposerFieldBorderColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ComposerFieldBorderColor() const
    {
    FUNC_LOG;
    return GetSkinColor( KAknsIIDFsOtherColors, EAknsCIFsOtherColorsCG14, KRgbBlack );
    }
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ComposerFieldBackgroundColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ComposerFieldBackgroundColor() const
    {
    FUNC_LOG;
    return GetSkinColor( KAknsIIDFsOtherColors, EAknsCIFsOtherColorsCG12, KRgbWhite );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ComposerHeaderFieldTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ComposerHeaderFieldTextColor() const
        {
    FUNC_LOG;
    return GetSkinColor( KAknsIIDFsTextColors, EAknsCIFsTextColorsCG4, KRgbBlack );
        }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ComposerBodyFieldTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ComposerBodyFieldTextColor() const
    {
    FUNC_LOG;
    return GetSkinColor( KAknsIIDFsTextColors, EAknsCIFsTextColorsCG3, KRgbBlack );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ComposerDimmedTextColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::ComposerDimmedTextColor() const
    {
    FUNC_LOG;
    return GetSkinColor( KAknsIIDFsTextColors, EAknsCIFsTextColorsCG18, KRgbGray );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::MaxPcsMatches
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::MaxPcsMatches() const
    {
    FUNC_LOG;
    return KMaxPcsMatches;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::PcsPopupDimmedFontTransparency
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::PcsPopupDimmedFontTransparency() const
    {
    FUNC_LOG;
    return KPcsPopupDimmedFontTransparency;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListTextureNodeExpandedSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListTextureNodeExpandedSize() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    TAknLayoutRect listItem;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    listItem.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ) );
    return listItem.Rect().Height();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::SearchLookingGlassIconSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::SearchLookingGlassIconSize() const
    {
    FUNC_LOG;
    return KSearchLookingGlassIconSize;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::statusPaneIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::statusPaneIconSize() const
	{
    FUNC_LOG;
    TRect titlePaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::ETitlePane, titlePaneRect);
    TAknLayoutRect iconRect;
    iconRect.LayoutRect(titlePaneRect, AknLayoutScalable_Avkon::title_pane_g2(0));
    return iconRect.Rect().Size();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListControlBarMailboxDefaultIconSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListControlBarMailboxDefaultIconSize() const
    {
    FUNC_LOG;
    return GetControlBarMailboxIconRect().Size().iWidth;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::EMailListBarBgIconSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::EMailListBarBgIconSize() const
    {
    FUNC_LOG;
    return GetControlBarRect().Size().iHeight;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListTextureMailboxDefaultIconSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListTextureMailboxDefaultIconSize() const
    {
    FUNC_LOG;
    return KListTextureMailboxDefaultIconSize;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::EListTextureSelectorSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::EListTextureSelectorSize() const
    {
    FUNC_LOG;
    return OneLineListItemHeight();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListControlMenuIconSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListControlMenuIconSize() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(0));
    TAknLayoutRect iconRect;
    iconRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g1(0));
    return iconRect.Rect().Height();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::AttachmentIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::AttachmentIconSize() const
    {
    FUNC_LOG;
    TAknLayoutRect rect;
    rect.LayoutRect( TRect(0,0,0,0),
                    AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ).LayoutLine() );
    return rect.Rect().Size();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerHighlightTextureSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerHighlightTextureSize() const
    {
    FUNC_LOG;
    return KViewerHighlightTextureSize;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarArrowTextureXSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ControlBarArrowTextureXSize() const
    {
    FUNC_LOG;
    return KControlBarArrowTextureXSize;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarArrowTextureYSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ControlBarArrowTextureYSize() const
    {
    FUNC_LOG;
    return KControlBarArrowTextureYSize;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetSkinColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::GetSkinColor( const TAknsItemID& aId, 
        TInt aIndex, const TRgb& aFallbackColor ) const
    {
    FUNC_LOG;
    TRgb color;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TInt err = AknsUtils::GetCachedColor(
        skin,
        color,
        aId,
        aIndex );
    if ( err != KErrNone )
        {
        color = aFallbackColor;
        }
    
    return color;    
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarMailboxIconRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarMailboxIconRect() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(0));
    TAknLayoutRect iconRect;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iconRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g2(0));        
        }
    else
        {
        iconRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g1(0));
        }
    
    return iconRect.Rect();
    }

// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarNewEmailButtonRect() const
    {
    FUNC_LOG;
    TAknLayoutRect iconRect;
    TRect cbr = GetControlBarRect();
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        iconRect.LayoutRect( cbr, 
                     AknLayoutScalable_Apps::cmail_ddmenu_btn02_pane(3) );
        }
    else
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            iconRect.LayoutRect( cbr, 
                             AknLayoutScalable_Apps::cmail_ddmenu_btn03_pane(2) );
            }
        else
            {
            iconRect.LayoutRect( cbr, 
                             AknLayoutScalable_Apps::cmail_ddmenu_btn01_pane(2) );
            }
        }
    
    TRect rect = iconRect.Rect();
    rect.Move( -cbr.iTl );
    return rect;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarFolderListButtonRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarFolderListButtonRect() const
    {
    FUNC_LOG;

    TAknLayoutRect folderListButtonRect;
    TRect cbr = GetControlBarRect();
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        folderListButtonRect.LayoutRect( cbr, 
                         AknLayoutScalable_Apps::cmail_ddmenu_btn02_pane_cp1(3) );
        }
    else
        {
        folderListButtonRect.LayoutRect( cbr, 
                             AknLayoutScalable_Apps::cmail_ddmenu_btn02_pane(2) );
        }
    
    TRect rect = folderListButtonRect.Rect();
    rect.Move( -cbr.iTl );
    return rect;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarSortButtonRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarSortButtonRect() const
    {
    FUNC_LOG;

    TAknLayoutRect sortButtonRect;
    TRect cbr = GetControlBarRect();
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        sortButtonRect.LayoutRect( cbr, 
                     AknLayoutScalable_Apps::cmail_ddmenu_btn02_pane_cp2(3) );
        }
    else
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            sortButtonRect.LayoutRect( cbr, 
                             AknLayoutScalable_Apps::cmail_ddmenu_btn01_pane(2) );
            }
        else
            {
            sortButtonRect.LayoutRect( cbr, 
                             AknLayoutScalable_Apps::cmail_ddmenu_btn03_pane(2) );
            }
        }
    
    TRect r = sortButtonRect.Rect();
    r.Move( -cbr.iTl );
    return r;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarConnectionIconRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarConnectionIconRect() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(0 ) );
    TAknLayoutRect iconRect;
    
    if ( AknLayoutUtils::LayoutMirrored() )
        {        
        iconRect.LayoutRect( controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g1( 0 ) );
        }
    else
        {
        iconRect.LayoutRect( controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g2( 0 ) );
        }
    
    return iconRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarRect() const
    {
    FUNC_LOG;
    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 3 : 2;
    
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    
    TRect emailRect = mainPaneRect;
    TAknLayoutRect emailPaneRect;
    emailPaneRect.LayoutRect(mainPaneRect, 
                    AknLayoutScalable_Apps::main_sp_fs_email_pane());
    emailRect = emailPaneRect.Rect();
   
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect( emailRect, 
                    AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(var));
    TRect rect = controlBarRect.Rect();
    
    return rect;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetListRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetListRect( TBool aControlsOnTop ) const
    {
    FUNC_LOG;
    TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();
    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 3 : 2;
    
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    
    TAknLayoutRect listRect;
    listRect.LayoutRect( mainPaneRect, 
                        AknLayoutScalable_Apps::main_sp_fs_listscroll_pane_te_cp01( var ));
    TRect rect = listRect.Rect();
  
    // if control bar is required, reserve space on top of the listbox  
    if( aControlsOnTop && landscape )
        {
        listRect.LayoutRect( mainPaneRect, 
                        AknLayoutScalable_Apps::main_sp_fs_listscroll_pane_te_cp01( 0 ));
        rect = listRect.Rect();
        }
    
    return rect;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetSearchListHeaderTextLayout
// -----------------------------------------------------------------------------
TAknLayoutText CFSEmailUiLayoutHandler::GetSearchListHeaderTextLayout() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect headerRect;
    headerRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane());
    TAknLayoutText textLayout;
    textLayout.LayoutText(headerRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_t1());
    return textLayout;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::FSTextStyleFromLayoutL
// -----------------------------------------------------------------------------
CAlfTextStyle& CFSEmailUiLayoutHandler::FSTextStyleFromLayoutL( const TAknTextComponentLayout& aLayout  )
    {
    FUNC_LOG;
    CAlfTextStyle** findResult = iTextStyleMap.Find( aLayout );
    CAlfTextStyle* style = NULL;
    if( !findResult )
        {
        TInt fontStyle(0);
        fontStyle = CAlfStatic::Env().TextStyleManager().CreatePlatformTextStyleL( aLayout.Font(), EAlfTextStyleNormal );
        style = CAlfStatic::Env().TextStyleManager().TextStyle( fontStyle );
        TInt insertError( iTextStyleMap.Insert( aLayout, style ) );
        if( insertError )
            {
            CAlfStatic::Env().TextStyleManager().DeleteTextStyle( style->Id() );
            User::Leave( insertError );
            }
        TAknLayoutText textLayout;
        textLayout.LayoutText(TRect(0,0,0,0), aLayout);
        style->SetBold(textLayout.Font()->FontSpecInTwips().iFontStyle.StrokeWeight());
        style->SetItalic(textLayout.Font()->FontSpecInTwips().iFontStyle.Posture());
        }
    else
        {
        style = *findResult;
        }
    return *style;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::DeleteTextStyles
// -----------------------------------------------------------------------------
void CFSEmailUiLayoutHandler::DeleteTextStyles()
    {
    FUNC_LOG;
    THashMapIter< TAknTextComponentLayout, CAlfTextStyle* >
        mapIterator( iTextStyleMap );
    CAlfTextStyle** value = mapIterator.CurrentValue();
    while( value )
        {
        CAlfStatic::Env().TextStyleManager().DeleteTextStyle( ( *value )->Id() );
        mapIterator.RemoveCurrent();
        mapIterator.NextValue();
        value = mapIterator.CurrentValue();
        }
    iTextStyleMap.Close();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListCmailPaneRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::ListCmailPaneRect() const
    {
    FUNC_LOG; 
    
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_email_pane() );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::listscroll_cmail_pane() );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_cmail_pane() );
    return layoutRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::TitleCaptionPadding
// 
// Returns padding value for title caption visual
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::TitleCaptionPadding() const
    {   
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    
    TAknLayoutRect ctrlBarPane;
    ctrlBarPane.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane( 0 ) );
    const TRect ctrlBarPaneRect( ctrlBarPane.Rect() );
    
    TAknLayoutText ctrlBarPaneT1;
    ctrlBarPaneT1.LayoutText( ctrlBarPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_t1( 0 ) );
    const TRect ctrlBarPaneT1Rect( ctrlBarPaneT1.TextRect() );
    
    return Abs( ctrlBarPaneRect.iTl.iY - ctrlBarPaneT1Rect.iTl.iY );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::SeparatorGraphicSizeInThisResolution
// -----------------------------------------------------------------------------
//
TSize CFSEmailUiLayoutHandler::SeparatorGraphicSizeInThisResolution() const
    {   
    FUNC_LOG;
    
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);    
    
    TAknLayoutRect listRect;
    listRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::list_cmail_pane());         

    TAknLayoutRect bodyRect;  
    bodyRect.LayoutRect(listRect.Rect(), AknLayoutScalable_Apps::list_cmail_body_pane());   
    
    TAknLayoutRect itemRect;  
    itemRect.LayoutRect(bodyRect.Rect(), AknLayoutScalable_Apps::list_cmail_body_pane_g1());       
    
    return itemRect.Rect().Size();      
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::SeparatorLineColor
// -----------------------------------------------------------------------------
//
TRgb CFSEmailUiLayoutHandler::SeparatorLineColor() const
    {
    FUNC_LOG;    
    return ViewerNormalHotspotTextColor();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::DropDownMenuListPadding
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::DropDownMenuListPadding() const
    {   
    FUNC_LOG;   
    // main pane
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( 
            AknLayoutUtils::EMainPane, mainPaneRect );
    
    // menu pane
    TAknLayoutRect menuPane;
    menuPane.LayoutRect( mainPaneRect, 
            AknLayoutScalable_Apps::popup_sp_fs_action_menu_pane( 2 ) );
    const TRect menuPaneRect( menuPane.Rect() );
    
    // Menu gene pane
    TAknLayoutRect menuListPane;
    menuListPane.LayoutRect( menuPaneRect, 
            AknLayoutScalable_Apps::sp_fs_action_menu_list_pane( 0 ) );
    const TRect menuListPaneRect( menuListPane.Rect() );
           
    return Abs( menuListPaneRect.iTl.iY - menuPaneRect.iTl.iY );
    }

// End of file

