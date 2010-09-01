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
* Description:  Layout data of the application.
*
*/


/////////////////////////////////////////////////////////////////////////////
// THIS IS A LAYOUT DATA FILE FOR FREESTYLE. 
// RESOLUTIONS ARE:
//
// LOW RESOLUTION PORTRAIT  	178x208
// LOW RESOLUTION LANDSCAPE 	208x178
// QVGA RESOLUTION PORTRAIT 	240x320
// QVGA RESOLUTION LANDSCAPE 	320x240
// DOUBLE RESOLUTION PORTRAIT	352x416
// DOUBLE RESOLUTION LANDSCAPE	416x352
// VGA RESOLUTION PORTRAIT		480x640
// VGA RESOLUTION LANDSCAPE		640x480
// HVGA RESOLUTION PORTRAIT		320x480
// HVGA RESOLUTION LANDSCAPE	480x320
// WIDE RESOLUTION LANDSCAPE	800x352
//
////////////////////////////////////////////////////////////////////////////

#ifndef __FREESTYLEEMAILUI_LAYOUTDATA_H__
#define __FREESTYLEEMAILUI_LAYOUTDATA_H__

#include <avkon.hrh>
//<cmail>
#include "fstreelistconstants.h"
//</cmail>

// SCREEN RESOLUTIONS
enum TScreenResolution
	{
	ELowResPortrait,
	ELowResLandscape,
	EQvgaPortrait,
	EQvgaLandscape,
	EDrPortrait,	
	EDrLandscape,
	EVgaPortrait,
	EVgaLandscape,
	EHvgaPortrait,
	EHvgaLandscape,
	EWideLandscape 
	};

// SCREEN ORIENTATIONS
enum TScreenOrientation 
    {
    EPortrait,
    ELandscape
    };

enum TFSEmailFont
	{
	EFSFontTypeSmall = 0,
	EFSFontTypeSmallBold,
	EFSFontTypeSmallItalic,
	EFSFontTypeSmallUnderlined,
	EFSFontTypeNormal,
	EFSFontTypeNormalBold,
	EFSFontTypeNormalItalic,
	EFSFontTypeNormalUnderlined,
	EFSFontTypeLarge,
	EFSFontTypeLargeBold,
	EFSFontTypeLargeItalic,
	EFSFontTypeLargeUnderlined,	
	};

// Transition effect times
static const TInt KListFadeInEffectTime = 270;
static const TInt KListFadeOutEffectTime = KListFadeInEffectTime + 100;
static const TInt KListSlideEffectTime = 270;
static const TBool KListSlideEffectActive = EFalse; 
static const TBool KListFadeEffectActive = EFalse;

static const TInt KStatusIndicatorFadeEffectTime = 350;
static const TInt KCtrlBarListFadeEffectTime = 400;

static const TInt KViewChangeDelay = 100;
static const TInt KListScrollingTime = 300;
static const TInt KListItemExpansionDelay = 300;

// Different resolution screen widths
static const TInt KLowResPortraitWidth = 176;		
static const TInt KLowResLandscapeWidth = 208;
static const TInt KQvgaPortraitWidth = 240;
static const TInt KQvgaLandscapeWidth = 320;
static const TInt KDrPortraitWidth = 352;
static const TInt KDrLandscapeWidth = 416;
static const TInt KVgaPortraitWidth = 480;
static const TInt KVgaLandscapeWidth = 640;
static const TInt KHvgaPortraitWidth = 320;
static const TInt KHvgaLandscapeWidth = 480;
static const TInt KWideLandscapeWidth = 800;

// Different resolution screen heights
static const TInt KLowResPortraitHeight = 208;
static const TInt KLowResLandscapeHeight = 176;
static const TInt KQvgaPortraitHeight = 320;
static const TInt KQvgaLandscapeHeight = 240;
static const TInt KDrPortraitHeight = 416;
static const TInt KDrLandscapeHeight = 352;
static const TInt KVgaPortraitHeight = 640;
static const TInt KVgaLandscapeHeight = 480;
static const TInt KHvgaPortraitHeigh = 480;
static const TInt KHvgaLandscapeHeight = 320;
static const TInt KWideLandscapeHeight = 352;

// Main UI grid icon size
static const TInt KGridIconSizeInQvgaPort = 58;
static const TInt KGridIconSizeInQvgaLand = 47;
static const TInt KGridIconSizeInDoubleResPort = 100;
static const TInt KGridIconSizeInDoubleResLand = 80;
static const TInt KGridIconSizeInHvgaPort = 100;
static const TInt KGridIconSizeInHvgaLand = 100;
static const TInt KGridIconSizeInVgaPort = 100;
static const TInt KGridIconSizeInVgaLand = 100;
static const TInt KGridIconSizeInWideLand = 80;

static const TSize KMailListIconSizeInQvgaPort(21,21);
static const TSize KMailListIconSizeInQvgaLand(21,21);
static const TSize KMailListIconSizeInDoubleResPort(23,23);
static const TSize KMailListIconSizeInDoubleResLand(23,23);
static const TSize KMailListIconSizeInHvgaPort(23,23);
static const TSize KMailListIconSizeInHvgaLand(23,23);
static const TSize KMailListIconSizeInVgaPort(23,23);
static const TSize KMailListIconSizeInVgaLand(23,23);
static const TSize KMailListIconSizeInWideLand(23,23);

// Folder list icon relative size compared to item size
static const TReal KFolderListIconRelativeSize = 0.8;

static const TSize KSelectorVisualSizeInQvgaPort(60,60);
static const TSize KSelectorVisualSizeInQvgaLand(50,50);
static const TSize KSelectorVisualSizeInDoubleResPort(120,80);
static const TSize KSelectorVisualSizeInDoubleResLand(100,65);
static const TSize KSelectorVisualSizeInHvgaPort(75,85);
static const TSize KSelectorVisualSizeInHvgaLand(75,85);
static const TSize KSelectorVisualSizeInVgaPort(75,85);
static const TSize KSelectorVisualSizeInVgaLand(75,85);
static const TSize KSelectorVisualSizeInWideLand(80,80);

static const TInt KOneLineListNodeHeightInQvgaPort = 24;
static const TInt KOneLineListNodeHeightInQvgaLand = 24;
static const TInt KOneLineListNodeHeightInDoubleResPort = 24;
static const TInt KOneLineListNodeHeightInDoubleResLand = 24;
static const TInt KOneLineListNodeHeightInHvgaPort = 24;
static const TInt KOneLineListNodeHeightInHvgaLand = 24;
static const TInt KOneLineListNodeHeightInVgaPort = 24;
static const TInt KOneLineListNodeHeightInVgaLand = 24;
static const TInt KOneLineListNodeHeightInWideLand = 24;

static const TInt KOneLineListItemHeightInQvgaPort = 24;
static const TInt KOneLineListItemHeightInQvgaLand = 24;
static const TInt KOneLineListItemHeightInDoubleResPort = 27;
static const TInt KOneLineListItemHeightInDoubleResLand = 27;
static const TInt KOneLineListItemHeightInHvgaPort = 27;
static const TInt KOneLineListItemHeightInHvgaLand = 27;
static const TInt KOneLineListItemHeightInVgaPort = 27;
static const TInt KOneLineListItemHeightInVgaLand = 27;
static const TInt KOneLineListItemHeightInWideLand = 27;

// Control bar heigth in different resolutions
static const TInt KControlBarHeightInQvgaPort = 27;
static const TInt KControlBarHeightInQvgaLand = 27;
static const TInt KControlBarHeightInHvgaPort = 30;
static const TInt KControlBarHeightInHvgaLand = 27;
static const TInt KControlBarHeightInWideLand = 28;

// Control bar text height
static const TInt KControlBarTextHeightSmall = 120;
static const TInt KControlBarTextHeightNormal = 130;
static const TInt KControlBarTextHeightLarge = 140;

// Control bar list size
static const TInt KControlBarListLeftOffset = 25;
static const TReal KControlBarListRelativeHeight = 0.95;
static const TReal KControlBarListRelativeWidth = 0.60;
static const TInt KControlBarListMinimumWidth = 150;
static const TInt KControlBarListMaximumWidth = 250;
static const TPoint KControlBarListPadding( 0, 2 );
static const TInt KControlBarListBorderRoundingSize = 6;
static const TInt KControlBarbuttonBorderRoundingSize = 3;

// List font height
static const TInt KListItemFontHeightSmall = 125;
static const TInt KListItemFontHeightLarge = 170;
static const TInt KListItemFontHeightNormal = 140;

// List text marquee behaviour, mail list, search list, msg details, etc.
static const TFsTextMarqueeType KListTextMarqueeType = EFsTextMarqueeForth; // Type
static const TInt KListTextMarqueeSpeed = 25;		 // Pixels per second.
static const TInt KListTextMarqueeStartDelay = 1500; // Milliseconds
static const TInt KListTextMarqueeSpeedRepeats = 0;  // repeats

// Border widths.
const TInt KSelectorBorderWidth( 8 );

// Compose screen specific layout constants
// these are constants for all zoom levels
const TInt KAifLabelMarginHorizontal = 9;
const TInt KAifEditorMarginHorizontalNear = 35;
const TInt KAifEditorMarginHorizontalFar = 23;
const TInt KMaxAddressFieldLines = 3;
const TInt KMaxVisibleAddressFieldLines = 2;
const TInt KMaxVisibleSubjectFieldLines = 2;
const TInt KIconWidthPriority = 21;
const TInt KIconHeightPriority = 20;
const TInt KIconWidthFollowUp = 21;
const TInt KIconHeightFollowUp = 20;
const TInt KIconWidthAttachment = 14;
const TInt KIconHeightAttachment = 17;
const TInt KInputFieldRoundingRadius = 3;
const TSize KInputFieldRoundingSize( KInputFieldRoundingRadius * 2,
									 KInputFieldRoundingRadius * 2 );

// these are defined for each zoom level
// normal zoom level
const TInt KAifHeight = 45;
const TInt KAifLabelMarginVertical = 4;
const TInt KAifEditorMarginVertical = 23;
const TInt KAifEditorHeight = 22;
const TInt KSubjectExtraHeightBottom = 8;
const TInt KAttachmentExtraHeightBottom = 8;
const TInt KIconMarginPriorityVertical = KAifEditorMarginVertical;
const TInt KIconMarginFollowUpVertical = KIconMarginPriorityVertical + KIconHeightPriority;
const TInt KIconMarginAttachmentVertical = KIconMarginPriorityVertical;
const TInt KLabelFontHeightPx = 14;
const TInt KEditorFontHeightPx = 17;

// small zoom level
const TInt KAifHeightSmall = 40;
const TInt KAifLabelMarginVerticalSmall = 3;
const TInt KAifEditorMarginVerticalSmall = 20;
const TInt KAifEditorHeightSmall = 20;
const TInt KSubjectExtraHeightBottomSmall = 6;
const TInt KAttachmentExtraHeightBottomSmall = 6;
const TInt KIconMarginPriorityVerticalSmall = KAifEditorMarginVerticalSmall;
const TInt KIconMarginFollowUpVerticalSmall = KIconMarginPriorityVerticalSmall + KIconHeightPriority;
const TInt KIconMarginAttachmentVerticalSmall = KIconMarginPriorityVerticalSmall;
const TInt KLabelFontHeightPxSmall = 13;
const TInt KEditorFontHeightPxSmall = 15;

// large zoom level
const TInt KAifHeightLarge = 52;
const TInt KAifLabelMarginVerticalLarge = 5;
const TInt KAifEditorMarginVerticalLarge = 26;
const TInt KAifEditorHeightLarge = 26;
const TInt KSubjectExtraHeightBottomLarge = 10;
const TInt KAttachmentExtraHeightBottomLarge = 10;
const TInt KIconMarginPriorityVerticalLarge = KAifEditorMarginVerticalLarge;
const TInt KIconMarginFollowUpVerticalLarge = KIconMarginPriorityVerticalLarge + KIconHeightPriority;
const TInt KIconMarginAttachmentVerticalLarge = KIconMarginPriorityVerticalLarge;
const TInt KLabelFontHeightPxLarge = 15;
const TInt KEditorFontHeightPxLarge = 19;


// Viewer layout spesific fonts, sizes etc.

// Msg status and follow up flag icon sizes
static const TSize KViewerIconSize( 20,20 );
// Size of the transparent icon which is used to mark certain size indentation in pixels
static const TSize KViewerTransparentIndentationIconSize( 31, 1 );
// Size of the action menu icon in mail viewer
static const TSize KViewerActionMenuIconSize( 12, 12 );
// Margin of the action button in the mail viewer (this means the space between
// action button and scrollbar in both normal and mirrored layout)
static const TInt KViewerActionMenuIconMargin( 1 );
// Hotspot seletor visual opacity
static const TReal KViewerSelectorOpacity( 0.90 );

// Viewer margins
static const TInt KViewerRightMarginWidthInPixels = 10;
static const TInt KViewerLeftMarginWidthInPixels = 5;

// watermark place and size
static const TSize KViewerDefaultWaterMarkSizeInViewer( 100, 100 );
static const TPoint KViewerWaterMarkPositionFromBottomRightCorner( 150, 150 );

// font contants
static const TAknLogicalFontId KViewerLogicalTitleFontId = EAknLogicalFontTitleFont;
static const TAknLogicalFontId KViewerLogicalTextFontId = EAknLogicalFontPrimaryFont;

// viewer text fallback colors, these are used if current skin doesn't provide color
static const TRgb KGrayBackgroundFallbackColor( 220,220,220 );
static const TRgb KGraySentFallbackTextColor( 138,138,138 );
static const TRgb KBlackFallbackTextColor( 0,0,0 );
static const TRgb KLightBlueFallbackTextColor( 57,166, 214 );

// viewer text sizes 
static const TInt KViewerTitleFontSizeSmall = 13;
static const TInt KViewerTitleFontSizeNormal = 14;
static const TInt KViewerTitleFontSizeLarge = 15;

static const TInt KViewerTextFontSizeSmall = 15;
static const TInt KViewerTextFontSizeNormal = 17;
static const TInt KViewerTextFontSizeLarge = 19;

// viewer spaces in different places with all text sizes
static const TInt KViewerTopMostSpaceSmall = 15;
static const TInt KViewerRecipientSpaceSmall = 20;
static const TInt KViewerHeaderInfoSpaceSmall = 20;
static const TInt KViewerBottomMostSpaceSmall = 11;

static const TInt KViewerTopMostSpaceNormal = 17;
static const TInt KViewerRecipientSpaceNormal = 23;
static const TInt KViewerHeaderInfoSpaceNormal = 22;
static const TInt KViewerBottomMostSpaceNormal = 13;

static const TInt KViewerTopMostSpaceLarge = 19;
static const TInt KViewerRecipientSpaceLarge = 26;
static const TInt KViewerHeaderInfoSpaceLarge = 26;
static const TInt KViewerBottomMostSpaceLarge = 15;

// size of the transparent icon between msgstatus&follow up icon and subject
static const TInt KViewerSpaceBetweenIconAndSubject = 10;

// Maximum number of recipients shown in the To, Cc, and Bcc fields
static const TInt KViewerMaxRecipientLineCount = 4;

// Maximum number of lines used to show the Mail subject. Subject is truncated after this is reached.
static const TInt KViewerMaxSubjectLineCount = 10;

// Maximum number of contacts included in the Predictive contact search popup.
// Number is limited to enhance performance in case of large contact DB.
static const TInt KMaxPcsMatches = 50;

// Predictive contact search popup colors
const TInt KPcsPopupDimmedFontTransparency  = 100;

//Icon sizes
static const TInt KListTextureNodeExpandedSize = 14;
static const TInt KSearchLookingGlassIconSize = 20;
static const TInt KListControlBarMailboxDefaultIconSize = 19;
static const TInt KEMailListBarBgIconSize = 25;
static const TInt KListTextureMailboxDefaultIconSize = 15;
static const TInt KEListTextureSelectorSize = 30;
static const TInt KListControlMenuIconSize = 13;
static const TInt KAttachmentIconSize = 20;
static const TInt KViewerHighlightTextureSize = 1;
static const TInt KControlBarArrowTextureXSize = 12;
static const TInt KControlBarArrowTextureYSize = 17;


#endif // __FREESTYLEEMAILUI_LAYOUTDATA_H__     

// End of File
