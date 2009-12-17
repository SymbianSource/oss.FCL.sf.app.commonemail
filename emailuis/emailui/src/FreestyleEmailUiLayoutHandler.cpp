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
//#include <aknutils.h> <cmail>
// <cmail> SF
#include <alf/alftextstylemanager.h>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <alf/alfstatic.h>

// <cmail> Platform layout change
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
// </cmail>

// For generic treelist layout data setting
//<cmail>
#include "fstreelist.h"
//</cmail>

// INTERNAL INCLUDE FILES
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiLayoutData.h"

// <cmail> Platform layout change
template< typename T >
TUint32 GeneralHash( const T& aValue )
    {
    return DefaultHash::Des8( TPckgC< T >( aValue ) );
    }

template< typename T >
TBool GeneralPtrIdentity( const T& aP1, const T& aP2 )
    {
    return TPckgC< T >( aP1 ) == TPckgC< T >( aP2 );
    }
// </cmail> Platform layout change

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::CFSEmailUiLayoutHandler
// Base construction. May not leave.
// -----------------------------------------------------------------------------
//
CFSEmailUiLayoutHandler::CFSEmailUiLayoutHandler( CAlfEnv& aEnv )
	: iEnv(aEnv),
	iScreenResolution( EQvgaPortrait ),
	iTextStyleMap( THashFunction32< TAknTextComponentLayout >( GeneralHash ),
	    TIdentityRelation< TAknTextComponentLayout >( GeneralPtrIdentity< TAknTextComponentLayout > ) ),
    // <cmail>
    iNormalTextStyle( KErrNotFound ),
    iLargeTextStyle( KErrNotFound ),
    iSmallTextStyle( KErrNotFound )
    // </cmail>
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
// <cmail> Platform layout change
    DeleteTextStyles();
// </cmail> Platform layout change
    // <cmail>
    if ( iNormalTextStyle != KErrNotFound )
        {
        CAlfStatic::Env().TextStyleManager().DeleteTextStyle( 
                iNormalTextStyle );
        }
    if( iLargeTextStyle != KErrNotFound )
        {
        CAlfStatic::Env().TextStyleManager().DeleteTextStyle( 
                iLargeTextStyle );
        }
    if( iSmallTextStyle != KErrNotFound )
        {
        CAlfStatic::Env().TextStyleManager().DeleteTextStyle( 
                iSmallTextStyle );
        }
    // </cmail>
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ScreenResolution
// Function can be used to fetch currently active screen resolution
// -----------------------------------------------------------------------------
//	
TScreenResolution CFSEmailUiLayoutHandler::ScreenResolution() const
	{
    FUNC_LOG;
	return iScreenResolution;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ScreenOrientation
// Function can be used to fetch orientation of the current screen resolution
// -----------------------------------------------------------------------------
//  
TScreenOrientation CFSEmailUiLayoutHandler::ScreenOrientation() const
    {
    FUNC_LOG;
    /*
    TScreenOrientation orientation = EPortrait;
    switch ( iScreenResolution )
        {
        case ELowResLandscape:
        case EQvgaLandscape:
        case EDrLandscape:
        case EVgaLandscape:
        case EHvgaLandscape:
        case EWideLandscape:
            orientation = ELandscape;
            break;
        case ELowResPortrait:
        case EQvgaPortrait:
        case EDrPortrait:
        case EVgaPortrait:
        case EHvgaPortrait:
        default:
            orientation = EPortrait;
            break;
        }
        
    return orientation;*/
    return Layout_Meta_Data::IsLandscapeOrientation() ? ELandscape : EPortrait;
    }

// -----------------------------------------------------------------------------
// CTeamUiAppLayoutHandler::ScreenResolutionChanged
// Function can be used to update internal resolution value. Typically called
// by UI in startup and when dynamic layout variant switch has occured 
// -----------------------------------------------------------------------------
//	
void CFSEmailUiLayoutHandler::ScreenResolutionChanged()
	{
    FUNC_LOG;
   	TRect screenRect;
 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
	switch ( screenRect.Width() )
  		{
		case KLowResPortraitWidth:
			iScreenResolution = ELowResPortrait;
			break;
		case KLowResLandscapeWidth:
			iScreenResolution =  ELowResLandscape;
			break;
		case KQvgaPortraitWidth:
			iScreenResolution =  EQvgaPortrait;
			break;
		case KQvgaLandscapeWidth: // Also KHvgaPortraitWidth
			{
			if ( screenRect.Height() == KQvgaLandscapeHeight )
				{
				iScreenResolution =  EQvgaLandscape;					
				}
			else
				{
				iScreenResolution =  EHvgaPortrait;
				}			
			}
			break;
		case KDrPortraitWidth:
			iScreenResolution =  EDrPortrait;
			break;
		case KDrLandscapeWidth:
			iScreenResolution =  EDrLandscape;
			break;
		case KVgaPortraitWidth: // Also KHvgaLandscapeWidth
			{
			if ( screenRect.Height() == KHvgaLandscapeHeight )
				{
				iScreenResolution =  EHvgaLandscape;					
				}
			else
				{
				iScreenResolution =  EVgaPortrait;
				}							
			}
			break;
		case KVgaLandscapeWidth:
			iScreenResolution =  EVgaLandscape;
			break;
		case KWideLandscapeWidth:
			iScreenResolution =  EWideLandscape;
			break;
		default:
			break;
 		}
	}


TInt CFSEmailUiLayoutHandler::GridRowsInThisResolution() const
	{
    FUNC_LOG;
	TInt rowCount(0);
	switch ( iScreenResolution )
		{
 		case ELowResPortrait:
		case EQvgaPortrait:
 		case EVgaPortrait:
  		case EHvgaPortrait:
 		case EDrPortrait:

 		case ELowResLandscape:
 		case EQvgaLandscape:
		case EVgaLandscape:	
		case EHvgaLandscape:	
 		case EDrLandscape:
  		    rowCount = 3;
 		    break;
 		    
 		case EWideLandscape:
 		    rowCount = 2;
 		    break;
 		    
		default:
		    rowCount = 3;
		    break;
		}
	return rowCount;
	}


TInt CFSEmailUiLayoutHandler::GridColumnsInThisResolution() const
	{
    FUNC_LOG;
	TInt columnCount(0);
	switch ( iScreenResolution )
		{
 		case ELowResPortrait:
		case EQvgaPortrait:
 		case EVgaPortrait:
  		case EHvgaPortrait:
 		case EDrPortrait:
  		    columnCount = 3;
 		    break;

 		case ELowResLandscape:
 		case EQvgaLandscape:
		case EVgaLandscape:	
		case EHvgaLandscape:	
 		case EDrLandscape:
  		    columnCount = 4;
 		    break;
 		    
 		case EWideLandscape:
 		    columnCount = 5;
 		    break;
 		    
		default:
		    columnCount = 3;
		    break;
		}
	return columnCount;
	}


// Grid normal state text color, same as focused
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
 /*
    // Should use EAknsCIFsTextColorsCG3 if background is white
     if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDFsTextColors, EAknsCIFsTextColorsCG3 ) != KErrNone )
        {
        textColor = KRgbBlack;
        }
*/
    return textColor;	
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GridIconSize
// 
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
/*    
	TInt iconSize(0);
	switch ( iScreenResolution )
		{
		case EQvgaPortrait:
			iconSize = KGridIconSizeInQvgaPort;
	 		break;
 		case EQvgaLandscape:
			iconSize = KGridIconSizeInQvgaLand;
		 	break;
 		case EVgaPortrait:
 			iconSize = KGridIconSizeInVgaPort;
 			break;
		case EVgaLandscape:
			iconSize = KGridIconSizeInVgaLand;
			break;
  		case EHvgaPortrait:
 			iconSize = KGridIconSizeInHvgaPort;
 			break;
		case EHvgaLandscape:
			iconSize = KGridIconSizeInHvgaLand;
			break;
 		case EWideLandscape:
			iconSize = KGridIconSizeInWideLand;
 			break;
 		case EDrPortrait:
			iconSize = KGridIconSizeInDoubleResPort;
 			break;
 		case EDrLandscape:
		default:
			iconSize = KGridIconSizeInDoubleResLand;
			break;
		}
	return iconSize;*/
// </cmail>    
	}

	
TSize CFSEmailUiLayoutHandler::MailIconSizeInThisResolution() const
	{
    FUNC_LOG;
// <cmail> Use layout data instead of hard-coded values
	TAknLayoutRect rect;
	rect.LayoutRect( TRect( 0, 0, 0, 0 ),
	                AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ).LayoutLine() );
	return rect.Rect().Size();
	/*TSize mailListIconSize(0,0);
	switch ( iScreenResolution )
		{
		case EQvgaPortrait:
			mailListIconSize = KMailListIconSizeInQvgaPort;
	 		break;
 		case EQvgaLandscape:
			mailListIconSize = KMailListIconSizeInQvgaLand;
		 	break;
 		case EVgaPortrait:
 			mailListIconSize = KMailListIconSizeInVgaPort;
 			break;
		case EVgaLandscape:
			mailListIconSize = KMailListIconSizeInVgaLand;
			break;
  		case EHvgaPortrait:
 			mailListIconSize = KMailListIconSizeInHvgaPort;
 			break;
		case EHvgaLandscape:
			mailListIconSize = KMailListIconSizeInHvgaLand;
			break;
 		case EWideLandscape:
			mailListIconSize = KMailListIconSizeInWideLand;
 			break;
 		case EDrPortrait:
			mailListIconSize = KMailListIconSizeInDoubleResPort;
 			break;
 		case EDrLandscape:
		default:
			mailListIconSize = KMailListIconSizeInDoubleResLand;
			break;
		}
	TAknUiZoom zoomLevel(EAknUiZoomNormal);
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
 	switch ( zoomLevel )
		{
		// Make icon a little bit smaller in small zoom
		case EAknUiZoomSmall:
			{
			mailListIconSize.iHeight-=2;
			mailListIconSize.iWidth-=2;
			}
			break;
		case EAknUiZoomLarge:
		case EAknUiZoomNormal:
		default:
			{
			}
			break;
		}
	return mailListIconSize;*/
// </cmail> Use layout data instead of hard-coded values
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
        paneG1.LayoutRect( DropDownMenuListRect( ELeft ), g1Layout.LayoutLine() );
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

// <cmail>
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
// </cmail>

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

// <cmail>
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::DropDownMenuListRect
// Selector ring image visual size
// -----------------------------------------------------------------------------
//
TRect CFSEmailUiLayoutHandler::DropDownMenuListRect(
        const TDropDownMenuSide aSide ) const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
            mainPaneRect );
    mainPaneRect.Move( 0, -mainPaneRect.iTl.iY );
    TInt variety( aSide == ELeft ? 0 : 1 );
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
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
    return menuPane.Rect();
    
    }
// </cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::SelectorVisualSizeInThisResolution
// Selector ring image visual size
// -----------------------------------------------------------------------------
//
TSize CFSEmailUiLayoutHandler::SelectorVisualSizeInThisResolution() const
	{
    FUNC_LOG;
	TSize selectorVisualSize(0,0);
	switch ( iScreenResolution )
		{
		case EQvgaPortrait:
			selectorVisualSize = KSelectorVisualSizeInQvgaPort;
	 		break;
 		case EQvgaLandscape:
			selectorVisualSize = KSelectorVisualSizeInQvgaLand;
		 	break;
 		case EVgaPortrait:
 			selectorVisualSize = KSelectorVisualSizeInVgaPort;
 			break;
		case EVgaLandscape:	
			selectorVisualSize = KSelectorVisualSizeInVgaLand;
			break; 
  		case EHvgaPortrait:
 			selectorVisualSize = KSelectorVisualSizeInHvgaPort;
 			break;
		case EHvgaLandscape:	
			selectorVisualSize = KSelectorVisualSizeInHvgaLand;
			break; 
 		case EWideLandscape:
			selectorVisualSize = KSelectorVisualSizeInWideLand;
 			break;
 		case EDrPortrait:
			selectorVisualSize = KSelectorVisualSizeInDoubleResPort;
 			break;
 		case EDrLandscape:
		default:
			selectorVisualSize = KSelectorVisualSizeInDoubleResLand;
			break;
		}
	return selectorVisualSize;	
	}


// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::OneLineListNodeHeight
// List node item height 
// -----------------------------------------------------------------------------
//	
TInt CFSEmailUiLayoutHandler::OneLineListNodeHeight() const
	{
    FUNC_LOG;
	//<cmail>
    TRect mainPaneRect;
    TAknLayoutRect listItem;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    listItem.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::list_single_dyc_row_pane( 0 ) );
    return listItem.Rect().Height();
    //</cmail>
	}
	
	
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::OneLineListItemHeight
// List item height
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::OneLineListItemHeight() const
	{
    FUNC_LOG;
	//<cmail>
    TRect mainPaneRect;
    TAknLayoutRect listItem;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    listItem.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::list_single_dyc_row_pane( 0 ) );
    return listItem.Rect().Height();
    //</cmail>
	}

//<cmail>
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
/*    else
        {
        const TRect ddMenuRect( DropDownMenuListRect( ELeft ) );
        TAknLayoutRect ddMenuPane;
        bg_sp_fs_ctrlbar_ddmenu_pane
        return 0;
        }*/
    return 0;
    }
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarHeight
// Control bar height
// -----------------------------------------------------------------------------
//
TInt CFSEmailUiLayoutHandler::ControlBarHeight() const
	{
    FUNC_LOG;
	//<cmail>
	TRect mainPaneRect;
    TAknLayoutRect ctrlBarRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    ctrlBarRect.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane( 0 ) );
    return ctrlBarRect.Rect().Height();
	//</cmail>
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ControlBarHeight
// Control bar text height in twips
// -----------------------------------------------------------------------------
//
/*
TInt CFSEmailUiLayoutHandler::ControlBarTextHeight() const
	{
    TRect mainPaneRect;
    TAknLayoutText ctrlBarText;
    TInt orientation( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    ctrlBarText.LayoutText( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_t1( orientation ) );
    return ctrlBarText.Font()->FontSpecInTwips().iHeight;
	}
*/
//</cmail>  Platform layout change

// <cmail> changes to avoid unnecessary creating new text styles objects.
CAlfTextStyle* CFSEmailUiLayoutHandler::FSTextStyleFromIdL( TFSEmailFont aFontId )
    {
    FUNC_LOG;
    CAlfTextStyle* style = NULL;
    
    switch ( aFontId )
        {
        case EFSFontTypeNormal:
        case EFSFontTypeNormalBold:
        case EFSFontTypeNormalItalic:
            if ( iNormalTextStyle == KErrNotFound ) 
                {
                iNormalTextStyle = CAlfStatic::Env().TextStyleManager().
                    CreatePlatformTextStyleL( 
                            EAknLogicalFontPrimaryFont, 
                            EAlfTextStyleNormal );
                }
            style = CAlfStatic::Env().TextStyleManager().TextStyle( 
                        iNormalTextStyle );
            break;
        case EFSFontTypeLarge:
        case EFSFontTypeLargeBold:
        case EFSFontTypeLargeItalic:
            if ( iLargeTextStyle == KErrNotFound ) 
                {
                iLargeTextStyle = CAlfStatic::Env().TextStyleManager().
                    CreatePlatformTextStyleL( 
                            EAknLogicalFontTitleFont, 
                            EAlfTextStyleNormal );
                }
            style = CAlfStatic::Env().TextStyleManager().TextStyle( 
                        iLargeTextStyle );
            break;
        case EFSFontTypeSmallItalic:
        case EFSFontTypeSmallBold:
        case EFSFontTypeSmall:
        default:
            if ( iSmallTextStyle == KErrNotFound ) 
                {
                iSmallTextStyle = CAlfStatic::Env().TextStyleManager().
                    CreatePlatformTextStyleL( 
                            EAknLogicalFontSecondaryFont, 
                            EAlfTextStyleNormal );
                }
            style = CAlfStatic::Env().TextStyleManager().TextStyle( 
                    iSmallTextStyle );
            break;
        }
        
    switch ( aFontId )
        {
        case EFSFontTypeSmallBold:
            style->SetBold( EStrokeWeightBold );
            break;
        case EFSFontTypeSmallItalic:
            style->SetBold( EStrokeWeightNormal );
            style->SetItalic( EPostureItalic );    
            break;
        case EFSFontTypeNormal:
            style->SetBold( EStrokeWeightNormal );
            break;
        case EFSFontTypeNormalBold:
            style->SetBold( EStrokeWeightBold );
            break;
        case EFSFontTypeNormalItalic:
            style->SetBold( EStrokeWeightNormal );
            style->SetItalic( EPostureItalic );
            break;
        case EFSFontTypeLarge:
            style->SetBold( EStrokeWeightNormal );
            break;
        case EFSFontTypeLargeBold:
            style->SetBold( EStrokeWeightBold );
            break;
        case EFSFontTypeLargeItalic:
            style->SetBold( EStrokeWeightNormal );
            style->SetItalic( EPostureItalic ); 
            break;
        case EFSFontTypeSmall:
        default:
            {
            style->SetBold( EStrokeWeightNormal );
            }
            break;
        }
    
    return style;
    }
// </cmail>

TPoint CFSEmailUiLayoutHandler::ControlBarListPadding() const
	{
    FUNC_LOG;
	return KControlBarListPadding;
	}

TInt CFSEmailUiLayoutHandler::ControlBarListBorderRoundingSize() const
	{
    FUNC_LOG;
	return KControlBarListBorderRoundingSize;
	}

TInt CFSEmailUiLayoutHandler::ControlBarButtonBgRoundingSize() const
	{
    FUNC_LOG;
	return KControlBarbuttonBorderRoundingSize;	
	}

TInt CFSEmailUiLayoutHandler::ListFadeInEffectTime() const
	{
    FUNC_LOG;
	return KListFadeInEffectTime;
	}

TInt CFSEmailUiLayoutHandler::ListFadeOutEffectTime() const
    {
    FUNC_LOG;
    return KListFadeOutEffectTime;
    }

TInt CFSEmailUiLayoutHandler::StatusIndicatorFadeEffectTime() const
	{
    FUNC_LOG;
	return KStatusIndicatorFadeEffectTime;
	}

TInt CFSEmailUiLayoutHandler::CtrlBarListFadeEffectTime() const
	{
    FUNC_LOG;
	return KCtrlBarListFadeEffectTime;
	}

TBool CFSEmailUiLayoutHandler::ViewSlideEffectActive() const
	{
    FUNC_LOG;
	return KListSlideEffectActive;
	}

TBool CFSEmailUiLayoutHandler::ViewFadeEffectActive() const
	{
    FUNC_LOG;
	return KListFadeEffectActive;
	}

TInt CFSEmailUiLayoutHandler::ViewSlideEffectTime() const
	{
    FUNC_LOG;
	return KListSlideEffectTime;
	}

TInt CFSEmailUiLayoutHandler::ViewFadeInEffectTime() const
    {
    FUNC_LOG;
    return KListFadeInEffectTime;
    }

TInt CFSEmailUiLayoutHandler::ViewFadeOutEffectTime() const
    {
    FUNC_LOG;
    return KListFadeOutEffectTime;
    }

TInt CFSEmailUiLayoutHandler::ListScrollingTime() const
	{
    FUNC_LOG;
	return KListScrollingTime;
	}

TInt CFSEmailUiLayoutHandler::ListItemExpansionDelay() const
	{
    FUNC_LOG;
	return KListItemExpansionDelay;
	}

TInt CFSEmailUiLayoutHandler::ViewHidingDelay() const
	{
    FUNC_LOG;
	return KViewChangeDelay;
	}
	
TRgb CFSEmailUiLayoutHandler::ListFocusedStateTextSkinColor() const
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    // <cmail> Skin support
    if( AknsUtils::GetCachedColor( skin, textColor,
    		KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 ) != KErrNone )
    	{
        textColor = KRgbBlack;
    	}
    // </cmail>
    return textColor;
    }

TRgb CFSEmailUiLayoutHandler::ListNormalStateTextSkinColor() const
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
	// <cmail> S60 Skin support
    if ( AknsUtils::GetCachedColor( skin, textColor,
    	KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
    	{
        textColor = KRgbBlack;
    	}
	// </cmail>        
    return textColor;
    }
	
TRgb CFSEmailUiLayoutHandler::ListNodeBackgroundColor() const
	{
    FUNC_LOG;
//<cmail> skin support
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb nodeBgColor;
    if ( AknsUtils::GetCachedColor( skin, nodeBgColor,
            KAknsIIDFsOtherColors, EAknsCIFsOtherColorsCG19 ) != KErrNone )
        {
        nodeBgColor = KRgbDarkGray;
        }
//</cmail>
    return nodeBgColor;
	}

TRgb CFSEmailUiLayoutHandler::ListNodeTextColor() const
	{
    FUNC_LOG;
   MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    // <cmail> S60 Skin support
   TRgb nodeTextColor;
    if ( AknsUtils::GetCachedColor( skin, nodeTextColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
    	{
        nodeTextColor = KRgbWhite;
        }
    // </cmail>
    return nodeTextColor;	
	}

TRgb CFSEmailUiLayoutHandler::ListHeaderBackgroundColor() const
    {
    FUNC_LOG;
    // The header item bg color is gained by mixing 50/50 node bg color
    // and list item bg color.
    // <cmail>
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb listItemBgColor;
    if ( AknsUtils::GetCachedColor( skin, listItemBgColor,
        KAknsIIDQsnLineColors, EAknsCIFsTextColorsCG3 ) != KErrNone )
        //KAknsIIDQsnLineColors, EAknsCIFsTextColorsCG5 ) != KErrNone )            
        {
        listItemBgColor = KRgbWhite;
        }
    // </cmail>
    TRgb nodeItemBgColor = ListNodeBackgroundColor(); 
    TRgb headerBgColor;
    headerBgColor.SetRed( (listItemBgColor.Red()+nodeItemBgColor.Red()) / 2 );
    headerBgColor.SetGreen( (listItemBgColor.Green()+nodeItemBgColor.Green()) / 2 );
    headerBgColor.SetBlue( (listItemBgColor.Blue()+nodeItemBgColor.Blue()) / 2 );
    return headerBgColor;
    }

TInt CFSEmailUiLayoutHandler::ListTextStyle() const
	{
    FUNC_LOG;
	TInt listTextStyle( EAlfTextStyleSmall );
	TAknUiZoom zoomLevel(EAknUiZoomNormal);
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
 	switch ( zoomLevel ) 
		{
		case EAknUiZoomSmall:
			{
		    listTextStyle = EAlfTextStyleSmall;
			}
			break;
		case EAknUiZoomLarge:
			{
		    listTextStyle = EAlfTextStyleLarge;
			}
			break;
		case EAknUiZoomNormal:
		default:
			{
		    listTextStyle = EAlfTextStyleNormal;
			}
			break;
		}
	return listTextStyle;
	}

TRgb CFSEmailUiLayoutHandler::DropdownMenuTextColor() const
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;

    // <cmail> S60 Skin support
    if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
         textColor = KRgbBlack;
        }
	// </cmail>        

    return textColor;
    }


TInt CFSEmailUiLayoutHandler::ViewerFontHeightInTwips() const
	{
    FUNC_LOG;
	TAknUiZoom zoomLevel(EAknUiZoomNormal);
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
	TInt heightInTwips(0);
  	switch ( zoomLevel ) 
		{
		case EAknUiZoomSmall:
			{
		    heightInTwips = 130;
			}
			break;
		case EAknUiZoomLarge:
			{
		    heightInTwips = 190;
			}
			break;
		case EAknUiZoomNormal:
		default:
			{
		    heightInTwips = 160;
			}
			break;
		}
	return heightInTwips;
	}

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
    // <cmail> Platform layout changes
    /*
    TAknUiZoom zoomLevel;
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
    TInt heightInPixels = 0;
    switch ( zoomLevel )
        {
        case EAknUiZoomSmall:
            {
            heightInPixels = KViewerTitleFontSizeSmall;
            }
            break;
        case EAknUiZoomLarge:
            {
            heightInPixels = KViewerTitleFontSizeLarge;
            }
            break;
        case EAknUiZoomNormal:
        default:
            {
            heightInPixels = KViewerTitleFontSizeNormal;
            }
            break;
        }
    return heightInPixels;
    */
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText layoutText;
    layoutText.LayoutText( labelRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return layoutText.Font()->FontMaxHeight();
    // </cmail> Platform layout changes
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTextFontHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerTextFontHeight() const
	{
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
    TAknUiZoom zoomLevel;
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
    TInt heightInPixels = 0;
    switch ( zoomLevel )
        {
        case EAknUiZoomSmall:
            {
            heightInPixels = KViewerTextFontSizeSmall;
            }
            break;
        case EAknUiZoomLarge:
            {
            heightInPixels = KViewerTextFontSizeLarge;
            }
            break;
        case EAknUiZoomNormal:
        default:
            {
            heightInPixels = KViewerTextFontSizeNormal;
            }
            break;
        }
    return heightInPixels;
    */
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    TAknLayoutText layoutText;
    layoutText.LayoutText( labelRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( 0 ) );
    return layoutText.Font()->FontMaxHeight();
    // </cmail> Platform layout changes
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerTopMostSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerTopMostSpaceHeight() const
	{
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
	TAknUiZoom zoomLevel;
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
	TInt heightInPixels = 0;
  	switch ( zoomLevel )
		{
		case EAknUiZoomSmall:
			{
			heightInPixels = KViewerTopMostSpaceSmall;
			}
			break;
		case EAknUiZoomLarge:
			{
			heightInPixels = KViewerTopMostSpaceLarge;
			}
			break;
		case EAknUiZoomNormal:
		default:
			{
			heightInPixels = KViewerTopMostSpaceNormal;
			}
			break;
		}
	return heightInPixels;
	*/
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane( 0 ) );
    return labelRect.Rect().Height();
    // </cmail> Platform layout changes
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerRecipientSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerRecipientSpaceHeight() const
	{
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
	TAknUiZoom zoomLevel;
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
	TInt heightInPixels = 0;
  	switch ( zoomLevel )
		{
		case EAknUiZoomSmall:
			{
			heightInPixels = KViewerRecipientSpaceSmall;
			}
			break;
		case EAknUiZoomLarge:
			{
			heightInPixels = KViewerRecipientSpaceLarge;
			}
			break;
		case EAknUiZoomNormal:
		default:
			{
			heightInPixels = KViewerRecipientSpaceNormal;
			}
			break;
		}
	return heightInPixels;
	*/
    TAknLayoutRect labelRect;
    labelRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    return labelRect.Rect().Height();
    // </cmail> Platform layout changes
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerHeaderInfoSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerHeaderInfoSpaceHeight() const
	{
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
	TAknUiZoom zoomLevel;
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
	TInt heightInPixels = 0;
  	switch ( zoomLevel )
		{
		case EAknUiZoomSmall:
			{
			heightInPixels = KViewerHeaderInfoSpaceSmall;
			}
			break;
		case EAknUiZoomLarge:
			{
			heightInPixels = KViewerHeaderInfoSpaceLarge;
			}
			break;
		case EAknUiZoomNormal:
		default:
			{
			heightInPixels = KViewerHeaderInfoSpaceNormal;
			}
			break;
		}
	return heightInPixels;
	*/
	return ViewerTopMostSpaceHeight();
    // </cmail> Platform layout changes
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerBottomMostSpaceHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerBottomMostSpaceHeight() const
	{
	FUNC_LOG;
    // <cmail> Platform layout changes
	/*
	TAknUiZoom zoomLevel;
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
	TInt heightInPixels = 0;
  	switch ( zoomLevel )
		{
		case EAknUiZoomSmall:
			{
			heightInPixels = KViewerBottomMostSpaceSmall;
			}
			break;
		case EAknUiZoomLarge:
			{
			heightInPixels = KViewerBottomMostSpaceLarge;
			}
			break;
		case EAknUiZoomNormal:
		default:
			{
			heightInPixels = KViewerBottomMostSpaceNormal;
			}
			break;
		}
	return heightInPixels;
	*/
	return ViewerRecipientSpaceHeight();
    // </cmail> Platform layout changes
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
    // <cmail> S60 Skin support
    if (AknsUtils::GetCachedColor( skin, textColor,
		KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
        textColor = KBlackFallbackTextColor;
        }
    // </cmail>

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
    // <cmail> S60 Skin support
    if (AknsUtils::GetCachedColor( skin, textColor,
		KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
    	{
        textColor = KBlackFallbackTextColor;
        }
	// </cmail>

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
            //KAknsIIDFsTextColors, EAknsCIFsTextColorsCG3 ) != KErrNone )
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG13 ) != KErrNone ) // <cmail>
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
    // <cmail> S60 Skin support
	if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDQsnHighlightColors,
		EAknsCIQsnHighlightColorsCG3 ) != KErrNone )
		{
        textColor = KLightBlueFallbackTextColor;
        }
	// </cmail>

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
    // <cmail> S60 Skin support
	if ( AknsUtils::GetCachedColor( skin, textColor,
			KAknsIIDQsnHighlightColors,
			EAknsCIQsnHighlightColorsCG3 ) != KErrNone )
		{
        textColor = KLightBlueFallbackTextColor;
        }
	// </cmail>

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
    // <cmail> Platform layout changes
    /*
    return KViewerSpaceBetweenIconAndSubject;
    */
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
        
    // </cmail> Platform layout changes
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::ViewerIconSize() const
	{
	FUNC_LOG;
	// <cmail> Platform layout changes
	/*
	return KViewerIconSize;
	*/
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g1( 2 ) );
    return layoutRect.Rect().Size();
	// </cmail> Platform layout changes
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerIndentationIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::ViewerIndentationIconSize() const
	{
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
    return KViewerTransparentIndentationIconSize;
    */
	return ViewerIconSize();
    // </cmail> Platform layout changes
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerActionMenuIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::ViewerActionMenuIconSize() const
    {
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
    return KViewerActionMenuIconSize;
    */
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( ListCmailPaneRect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ) );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g2( 0 ) );
    return layoutRect.Rect().Size();
    // </cmail> Platform layout changes
    }
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerActionMenuIconMargin
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerActionMenuIconMargin() const
    {
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
    return KViewerActionMenuIconMargin;
    */
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
    // </cmail> Platform layout changes
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
    // <cmail> Platform layout changes
    /*
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        return KViewerLeftMarginWidthInPixels;
        }
    else
        {
        return KViewerRightMarginWidthInPixels;
        }
    */
    const TRect cmailPaneRect( ListCmailPaneRect() );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( cmailPaneRect, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText captionTextRect;
    captionTextRect.LayoutText( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return Abs( cmailPaneRect.iBr.iX - captionTextRect.TextRect().iBr.iX );
    // </cmail> Platform layout changes
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerLeftMarginInPixels
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ViewerLeftMarginInPixels() const
	{
	FUNC_LOG;
    // <cmail> Platform layout changes
    /*
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        return KViewerRightMarginWidthInPixels;
        }
    else
        {
        return KViewerLeftMarginWidthInPixels;
        }
    */
    const TRect cmailPaneRect( ListCmailPaneRect() );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( cmailPaneRect, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText captionTextRect;
    captionTextRect.LayoutText( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return Abs( cmailPaneRect.iTl.iX - captionTextRect.TextRect().iTl.iX );
    // </cmail> Platform layout changes
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerWaterMarkSizeInThisResolution
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::ViewerWaterMarkSizeInThisResolution() const
	{
    FUNC_LOG;
	TSize sizeWithCurrentResolution;
	// For now, set same size with all resolutions
	switch ( iScreenResolution )
		{
 		case ELowResPortrait:
		case EQvgaPortrait:
 		case EVgaPortrait:
  		case EHvgaPortrait:
 		case EDrPortrait:
 		case ELowResLandscape:
 		case EQvgaLandscape:
		case EVgaLandscape:	
		case EHvgaLandscape:	
 		case EDrLandscape:
 		case EWideLandscape:
		default:
		    sizeWithCurrentResolution = KViewerDefaultWaterMarkSizeInViewer;
		    break;
		}
	return sizeWithCurrentResolution;
	}
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ViewerWaterMarkPositionFromBottomRightCornerInThisResolution
// -----------------------------------------------------------------------------
TPoint CFSEmailUiLayoutHandler::ViewerWaterMarkPlaceFromBottomRightCornerInThisResolution() const
	{
    FUNC_LOG;
	TPoint placeFromBottomRightCorner;
	// For now, set place with all resolutions
	switch ( iScreenResolution )
		{
 		case ELowResPortrait:
		case EQvgaPortrait:
 		case EVgaPortrait:
  		case EHvgaPortrait:
 		case EDrPortrait:
 		case ELowResLandscape:
 		case EQvgaLandscape:
		case EVgaLandscape:	
		case EHvgaLandscape:	
 		case EDrLandscape:
 		case EWideLandscape:
		default:
		    placeFromBottomRightCorner = KViewerWaterMarkPositionFromBottomRightCorner;
		    break;
		}
	return placeFromBottomRightCorner;
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
// CFSEmailUiLayoutHandler::PcsPopupHighlightColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::PcsPopupHighlightColor() const
    {
    FUNC_LOG;
    return KPcsPopupHighlightColor;
    }
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::PcsPopupDimmedFontColor
// -----------------------------------------------------------------------------
TRgb CFSEmailUiLayoutHandler::PcsPopupDimmedFontColor() const
    {
    FUNC_LOG;
    return KPcsPopupDimmedFontColor;
    }
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListTextureNodeExpandedSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListTextureNodeExpandedSize() const
    {
    FUNC_LOG;
    //<cmail>
    TRect mainPaneRect;
    TAknLayoutRect listItem;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    listItem.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ) );
    return listItem.Rect().Height();
    //return KListTextureNodeExpandedSize;
    //</cmail>
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
    // <cmail> Platform layout change
    //return KListControlBarMailboxDefaultIconSize;
    return GetControlBarMailboxIconSize().iWidth;
    // </cmail> Platform layout change
    }
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::EMailListBarBgIconSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::EMailListBarBgIconSize() const
    {
    FUNC_LOG;
    // <cmail> Platform layout change
    return GetControlBarRect().Size().iHeight;
    //return KEMailListBarBgIconSize;
    // </cmail>
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
    // <cmail> Platform layout change
    return OneLineListItemHeight();
    //return KEListTextureSelectorSize;
    // </cmail>
    }
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::ListControlMenuIconSize
// -----------------------------------------------------------------------------
TInt CFSEmailUiLayoutHandler::ListControlMenuIconSize() const
    {
    FUNC_LOG;
    // <cmail> Platform layout change
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(0));
    TAknLayoutRect iconRect;
    iconRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g1(0));
    return iconRect.Rect().Height();
    //return KListControlMenuIconSize;
    // </cmail>
    }
// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::AttachmentIconSize
// -----------------------------------------------------------------------------
// <cmail>
TSize CFSEmailUiLayoutHandler::AttachmentIconSize() const
    {
    FUNC_LOG;
    TAknLayoutRect rect;
    rect.LayoutRect( TRect(0,0,0,0),
                    AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ).LayoutLine() );
    return rect.Rect().Size();
    //return KAttachmentIconSize;
// </cmail>    
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

// <cmail> Use layout data instead of hardcoded values

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
// CFSEmailUiLayoutHandler::GetControlBarMailboxIconPos
// -----------------------------------------------------------------------------
TPoint CFSEmailUiLayoutHandler::GetControlBarMailboxIconPos() const
    {
    FUNC_LOG;
    return GetControlBarMailboxIconRect().iTl;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarMailboxIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::GetControlBarMailboxIconSize() const
    {
    FUNC_LOG;
    return GetControlBarMailboxIconRect().Size();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarFolderListButtonRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarFolderListButtonRect() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(0));
    TAknLayoutRect folderListButtonRect;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        folderListButtonRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_button_pane_cp01(0));        
        }
    else
        {
        folderListButtonRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane(0));
        }
    return folderListButtonRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarFolderListButtonPos
// -----------------------------------------------------------------------------
TPoint CFSEmailUiLayoutHandler::GetControlBarFolderListButtonPos() const
    {
    FUNC_LOG;
    return GetControlBarFolderListButtonRect().iTl;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarFolderListButtonSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::GetControlBarFolderListButtonSize() const
    {
    FUNC_LOG;
    return GetControlBarFolderListButtonRect().Size();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarSortButtonRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarSortButtonRect() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(0));
    TAknLayoutRect sortButtonRect;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        sortButtonRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane(0));        
        }
    else
        {
        sortButtonRect.LayoutRect(controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_button_pane_cp01(0));
        }    
    return sortButtonRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarSortButtonPos
// -----------------------------------------------------------------------------
TPoint CFSEmailUiLayoutHandler::GetControlBarSortButtonPos() const
    {
    FUNC_LOG;
    return GetControlBarSortButtonRect().iTl;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarSortButtonSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::GetControlBarSortButtonSize() const
    {
    FUNC_LOG;
    return GetControlBarSortButtonRect().Size();
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
    // <cmail>
    if ( AknLayoutUtils::LayoutMirrored() )
        {        
        iconRect.LayoutRect( controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g1( 0 ) );
        }
    else
        {
        iconRect.LayoutRect( controlBarRect.Rect(), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_g2( 0 ) );
        }
    // </cmail>
    return iconRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarConnectionIconPos
// -----------------------------------------------------------------------------
TPoint CFSEmailUiLayoutHandler::GetControlBarConnectionIconPos() const
    {
    FUNC_LOG;
    return GetControlBarConnectionIconRect().iTl;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarConnectionIconSize
// -----------------------------------------------------------------------------
TSize CFSEmailUiLayoutHandler::GetControlBarConnectionIconSize() const
    {
    FUNC_LOG;
    return GetControlBarConnectionIconRect().Size();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetControlBarRect() const
    {
    FUNC_LOG;
    //TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0; //<cmail>
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect controlBarRect;
    controlBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane(0)); //<cmail>
    return controlBarRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetControlBarRect
// -----------------------------------------------------------------------------
TRect CFSEmailUiLayoutHandler::GetListRect() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    mainPaneRect.Move(-mainPaneRect.iTl);
    TAknLayoutRect listRect;
    listRect.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_listscroll_pane_te_cp01( 0 ) );
    return listRect.Rect();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiLayoutHandler::GetSearchListHeaderTextLayout
// -----------------------------------------------------------------------------
TAknLayoutText CFSEmailUiLayoutHandler::GetSearchListHeaderTextLayout() const
    {
    FUNC_LOG;
    //TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
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

// </cmail> Platform layout change

// <cmail> new methods

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

// </cmail> 

// End of file

