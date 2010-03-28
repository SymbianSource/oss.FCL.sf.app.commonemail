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
* Description:  FreestyleEmailUi layout data handler definition
*
*/


#ifndef __FREESTYLEEMAILUI_LAYOUTHANDLER_H__
#define __FREESTYLEEMAILUI_LAYOUTHANDLER_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <e32hashtab.h>
//<cmail> SF
#include <alf/alfenv.h>
#include <alf/alftextstyle.h>
//</cmail> SF
// <cmail> Platform layout change
#include <AknLayout2ScalableDef.h>
#include <AknUtils.h>
// </cmail> Platform layout change
// INTERNAL INCLUDES
#include "FreestyleEmailUiLayoutData.h"

// FORWARD DECLARATIONS
class CFsTreeList;

// CLASS DECLARATION
class CFSEmailUiLayoutHandler : public CBase
    {
    // <cmail>
	public:
    
        /*
         * Side parameter for drop down menu
         */
        enum TDropDownMenuSide 
            {
            ELeft,
            ERight
            };
    // </cmail>

    public: 
    	static CFSEmailUiLayoutHandler* NewL( CAlfEnv& aEnv  );
    	~CFSEmailUiLayoutHandler();
		
		// Function returns currently used screen resolution
		TScreenResolution ScreenResolution() const;
		
		// Function returns orientation of the currect screen resolution
		TScreenOrientation ScreenOrientation() const;
		
		// Function can be used to request screen resolution value update
		void ScreenResolutionChanged();

		// Main UI grid icon size
		TSize GridIconSize() const;

		// Grid rows and columns
		TInt GridRowsInThisResolution() const;
		TInt GridColumnsInThisResolution() const;

		// Grid normal state text color, same as focused
		TRgb GridNormalStateTextSkinColor() const;
		
		// One line node and item height
		TInt OneLineListNodeHeight() const;
		TInt OneLineListItemHeight() const;
		TInt TwoLineListItemHeight() const;
    	// <cmail>
		// Folder list item heigh
		TInt FolderListOneLineItemHeight( const TBool& aFullscreen ) const;
		
		// Mail icon size
		TSize MailIconSizeInThisResolution() const;

		// For setting list marquee behaviour 
		void SetListMarqueeBehaviour( CFsTreeList* aTreeList ) const;

        // Folder list icon size
		TSize FolderListIconSize( const TBool aPopup = EFalse ) const;

		// List pane size including scroll pane
	    TRect FolderListPaneRectInThisResolution( const TRect& aParent ) const;

		// Scroll pane list item size
		TSize FolderListItemSizeInThisResolution( const TRect& aParent ) const;
		
        // Drop down menu rect
		TRect DropDownMenuListRect( const TDropDownMenuSide aSide, TBool aStretch = ETrue ) const;
    	// </cmail>

		// Folder list pane indentation in pixels
		TInt FolderListIndentation( const TRect& aParent ) const;

		// Selector ring size for the current resolution
		TSize SelectorVisualSizeInThisResolution() const;

		// FS Email UI fonts, returns font style. Calling
		// View may set text visual style using SetTextStyle( CAlfTextStyle.Id() )
		CAlfTextStyle* FSTextStyleFromIdL( TFSEmailFont aFontId );
		
		// Control bar list padding
		TPoint ControlBarListPadding() const;

		// Control bar list border rounding size in pixels, used e.g. to
		// calculate the background image border size
		TInt ControlBarListBorderRoundingSize() const;

		// Control bar button bg rounding size
		TInt ControlBarButtonBgRoundingSize() const;

		// Control bar height
		TInt ControlBarHeight() const;

		// List fade effect times
        TInt ListFadeInEffectTime() const;
        TInt ListFadeOutEffectTime() const;

		// Status indicator fade effect time
		TInt StatusIndicatorFadeEffectTime() const;

		// Control bar list fade effect time
		TInt CtrlBarListFadeEffectTime() const;

		// View cross fade effect
		TBool ViewFadeEffectActive() const;
		TInt ViewFadeInEffectTime() const;
        TInt ViewFadeOutEffectTime() const;
		
		// View slide effect time
		TBool ViewSlideEffectActive() const;
		TInt ViewSlideEffectTime() const;

		// List scrolling delay
		TInt ListScrollingTime() const;
		
		// List item expansion delay
		TInt ListItemExpansionDelay() const;

		// Time to wait before previous view is hide
		// (to enable enough time for transition effect)
		TInt ViewHidingDelay() const;

		// List focused state text color
		TRgb ListFocusedStateTextSkinColor() const;

		// List normal state text color
		TRgb ListNormalStateTextSkinColor() const;
		
		// List node background color
		TRgb ListNodeBackgroundColor() const;

		// List node text color
		TRgb ListNodeTextColor() const;
		
		// List header item bacground color
		TRgb ListHeaderBackgroundColor() const;
		
		// List text style based on global zoom level
		TInt ListTextStyle() const;

		// Dropdown menu normal state text color
		TRgb DropdownMenuTextColor() const;

		// Text viewer font heigjht in twips global zoom level
		TInt ViewerFontHeightInTwips() const;

		// List items font heigjht in twips global zoom level
		TInt ListItemFontHeightInTwips( const TBool aPopup = EFalse ) const;
		
 		// Logical font id's for viewer fonts 		
		TAknLogicalFontId ViewerTitleFontAknLogicalFontId() const;
		TAknLogicalFontId ViewerTextFontAknLogicalFontId() const;

		// Viewer font heights based on global zoom level
		TInt ViewerTitleFontHeight() const;
		TInt ViewerTextFontHeight() const;

		// Viewer layout related heights between lines based on global zoom level
		TInt ViewerTopMostSpaceHeight() const;
		TInt ViewerRecipientSpaceHeight() const;
		TInt ViewerHeaderInfoSpaceHeight() const;
		TInt ViewerBottomMostSpaceHeight() const;
 
		// Maximum number of addresses shown in the To, Cc, and Bcc fields
		TInt ViewerMaxRecipientLineCount() const;
		
		// Maximum number of lines used to show the message subject
		TInt ViewerMaxSubjectLineCount() const;
		
		// Viewer text colors. These methods 
		// returns automatically fallback color if skin doesn't provide color
 		TRgb ViewerTitleTextColor() const;
 		TRgb ViewerTextTextColor() const;
 		TRgb ViewerBodyTextColor() const;
 		TRgb ViewerNormalHotspotTextColor() const;
 		TRgb ViewerHighlightedHotspotTextColor() const;
 		TRgb ViewerSentTextColor() const;
 
 		// Viewer pixels between msg status and follow up icons and subject text
		TInt ViewerPixelsBetweenMsgStatusIconAndSubject() const;
		
		// Viewer message status and follow up icon sizes in 
		TSize ViewerIconSize() const;
		
		// Viewer transparent indentation icon size which is used to get
		// certain amouth of pixel identation
		TSize ViewerIndentationIconSize() const;

		// Arrow icon used to indicate action menu
		TSize ViewerActionMenuIconSize() const;
        TInt ViewerActionMenuIconMargin() const;
		
		// Viewer right and left margins
		TInt ViewerRightMarginInPixels() const;
		TInt ViewerLeftMarginInPixels() const;
		
		// Viewer hotspot selector
		TReal ViewerSelectorOpacity() const;
		
		// Viewer watermark size and place from bottom right corner for current resolution
		TSize ViewerWaterMarkSizeInThisResolution() const;
		TPoint ViewerWaterMarkPlaceFromBottomRightCornerInThisResolution() const;
		
		// Composer address, subject, and attachment field colors
		TRgb ComposerFieldBorderColor() const;
		TRgb ComposerFieldBackgroundColor() const;
		
		// Composer text colors
		TRgb ComposerHeaderFieldTextColor() const;
		TRgb ComposerBodyFieldTextColor() const;
		TRgb ComposerDimmedTextColor() const;
		
		// Predictive contact search popup
		TInt MaxPcsMatches() const;
		TRgb PcsPopupHighlightColor() const;
		TRgb PcsPopupDimmedFontColor() const;
		
		//To retrieve respecitve Icon Sizes
		TInt ListTextureNodeExpandedSize() const;
		TInt SearchLookingGlassIconSize() const;
		TSize statusPaneIconSize() const;
		TInt ListControlBarMailboxDefaultIconSize() const;
		TInt EMailListBarBgIconSize() const;
		TInt ListTextureMailboxDefaultIconSize() const;
		TInt EListTextureSelectorSize() const;
		TInt ListControlMenuIconSize() const;
		//<cmail>
		TSize AttachmentIconSize() const;
        //</cmail>
		TInt ViewerHighlightTextureSize() const;
		TInt ControlBarArrowTextureXSize() const;
		TInt ControlBarArrowTextureYSize() const;

		TRect GetControlBarMailboxIconRect() const;
		TRect GetControlBarNewEmailButtonRect() const;
		TRect GetControlBarFolderListButtonRect() const;
        TRect GetControlBarSortButtonRect() const;
        TRect GetControlBarConnectionIconRect() const;

        TRect GetControlBarRect() const;

        TRect GetListRect( TBool aControlsOnTop = EFalse ) const;

		CAlfTextStyle& FSTextStyleFromLayoutL( const TAknTextComponentLayout& aLayout  );
//<cmail>
		TInt TitleCaptionPadding() const;

        TRect ListCmailPaneRect() const;

		TAknLayoutText GetSearchListHeaderTextLayout() const;

		// Separator size
        TSize SeparatorGraphicSizeInThisResolution() const;
        
		// Separator line color
        TRgb SeparatorLineColor() const;        

		// Padding value for drop down menu
        TInt DropDownMenuListPadding() const;

// </cmail>

    private:
		CFSEmailUiLayoutHandler( CAlfEnv& aEnv );
     	void ConstructL(); 
 
     	TRgb GetSkinColor( const TAknsItemID& aID, TInt aIndex, const TRgb& aFallbackColor ) const;
// <cmail> Platform layout change
        void DeleteTextStyles();
// </cmail> Platform layout change

	private:
		CAlfEnv& iEnv;
		TScreenResolution iScreenResolution;
// <cmail> Platform layout change
		RHashMap< TAknTextComponentLayout, CAlfTextStyle* > iTextStyleMap;
// </cmail> Platform layout change
// <cmail> Text Styles
        TInt iNormalTextStyle;
        TInt iLargeTextStyle;
        TInt iSmallTextStyle;
// </cmail>
	};

#endif //__FREESTYLEEMAILUI_LAYOUTHANDLER_H__

// End of File

 
