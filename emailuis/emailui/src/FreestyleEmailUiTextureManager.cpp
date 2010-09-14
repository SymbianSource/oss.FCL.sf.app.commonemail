/*
* Copyright (c) 2007 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FreestyleEmailUi generic texturemanager implementation
*
*/



// SYSTEM INCLUDES
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfstatic.h>
// </cmail>
#include <freestyleemailui.mbg>
#include <AknUtils.h>
// <cmail>
#include <alf/alfframebrush.h>
// </cmail>
#include <AknsConstants.h>
// <cmail>
#include <alf/alfimageloaderutil.h>
// </cmail>
#include <avkon.mbg>
#include <gulicon.h>
// <cmail> S60 Skin support
#include <AknsDrawUtils.h>
// </cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiUtilities.h"



// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewL
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiTextureManager* CFreestyleEmailUiTextureManager::NewL( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi )
	{
    FUNC_LOG;
    CFreestyleEmailUiTextureManager* self = new (ELeave) CFreestyleEmailUiTextureManager( aEnv, aAppUi );
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::CFreestyleEmailUiTextureManager
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiTextureManager::CFreestyleEmailUiTextureManager( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi )
	: CAlfControl(),
	iAppUi(aAppUi)
	{
    FUNC_LOG;
	// CHECKLATER - aEnv not used in CAlfControl anymore, but in this case its needed elsewhere, is this fine?
	iEnv = &aEnv;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::~CFreestyleEmailUiTextureManager
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiTextureManager::~CFreestyleEmailUiTextureManager()
	{
    FUNC_LOG;
	iTextures.Reset();

	iBrandedMailboxIcons.Reset();
	iPluginId.Reset();
	iMailboxId.Reset();
	iSize.Reset();
	delete iTmpBitmap;
	delete iTmpMaskBitmap;

	delete iFrameBrush;
	iFrameBrush = NULL;

	//<cmail>
	delete iNewFrameBrush;
	iNewFrameBrush = NULL;

	delete iTitleDividerBgBrush;
	//</cmail>
	}


// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::ConstructL
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiTextureManager::ConstructL(CAlfEnv& aEnv)
	{
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);
	iDelayLoadIndex = ETextureFirst;
	InitTextureArray();
	//CreateTexturesL();
	iTmpBitmap = NULL;
	iTmpMaskBitmap = NULL;
	}


// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::TextureByIndex
// -----------------------------------------------------------------------------
//
CAlfTexture& CFreestyleEmailUiTextureManager::TextureByIndex( TFSEmailUiTextures aIndex )
	{
    FUNC_LOG;
	// load the texture if it is not yet loaded
	if ( iTextures[aIndex] == &CAlfStatic::Env().TextureManager().BlankTexture() )
		{
		TRAPD( error, LoadTextureL( aIndex ) );
		if ( KErrNone != error )
			{
			}
		}
	return *iTextures[aIndex];
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::ClearTextureByIndex
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiTextureManager::ClearTextureByIndex( TFSEmailUiTextures aIndex )
    {
    FUNC_LOG;
    // unload the texture 
    CAlfStatic::Env().TextureManager().UnloadTexture( aIndex );
    iTextures[aIndex] = &CAlfStatic::Env().TextureManager().BlankTexture(); 
    }
// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::TextureByMailboxIdL
// Returns branded mailbox icon texture based on PluginId, MailboxId and IconSize.
// If not found. Will call LoadTexture to create new textures based on temporary
// variables which were set by CreateBrandedMailboxTexture call.
// Size(0,0) is used to get original size icon
// -----------------------------------------------------------------------------
CAlfTexture& CFreestyleEmailUiTextureManager::TextureByMailboxIdL( const TUid aPluginId, const TUint aMailboxId, const TSize& aSize )
	{
    FUNC_LOG;
	TInt foundIndex = 0;
	TBool found = EFalse;

	// Go through icon array and check if requested icon already exists
    for ( TInt i = 0; i < iBrandedMailboxIcons.Count(); i++ )
        {
        if ( iPluginId[i] == aPluginId && iMailboxId[i] == aMailboxId && iSize[i] == aSize )
        	{
            foundIndex = i;
        	found = ETrue;
      	    break;
        	}
        }
    if ( !found )
    	{
    	// Did not find icon -> add new
    	foundIndex = iBrandedMailboxIcons.Count();
    	TRAPD( error, LoadTextureL( TFSEmailUiTextures( EBrandedIconStart + foundIndex ) ) ); // Will add new
		if ( KErrNone != error )
			{
			return TextureByIndex( EFolderListEmailAccountTexture );
			}
    	}
	return *iBrandedMailboxIcons[foundIndex];
	}


// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::ListSelectorBrushL
// NOTE: Gen UI list doesn't get ownership of the selector brush, so we can
// use the same selector brush for all views.
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::ListSelectorBrushL()
	{
    FUNC_LOG;

	if ( !iFrameBrush )
		{
		iFrameBrush = NewCtrlBarSelectorBrushLC();
		CleanupStack::Pop( iFrameBrush );
		}

	return iFrameBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewCtrlBarSelectorBrushLC
// NOTE: Gen UI list gets the ownership of the selector brush and deletes it
// when the selector is reset, so we need to construct new brush every time.
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::NewCtrlBarSelectorBrushLC()
	{
    FUNC_LOG;

	// Use FS specific skin ID if it's found, otherwise use general list
    // highlight. This need to be checked before CHuiFrameBrush::NewL as it
    // never leaves even if the skin ID is not found. Another option would be
    // to try first with FS specific skin ID, TRAP SetFrameRectsL-function
    // call, and if it leaves, then recreate frame brush with general skin ID
    // and recall SetFrameRectsL.
	TAknsItemID skinId = KAknsIIDQgnFsList;
	CAknsItemData* temp =
		AknsUtils::SkinInstance()->GetCachedItemData( skinId );

	if( !temp )
		{
		skinId = KAknsIIDQsnFrList;
		}

	CAlfFrameBrush* frameBrush( 0 );
	frameBrush = CAlfFrameBrush::NewLC( *iEnv, skinId );

	TRect outerRect( 0, 0, 300, 80 );
	TRect innerRect( outerRect ); 
	innerRect.Shrink( KSelectorBorderWidth, KSelectorBorderWidth );

	frameBrush->SetFrameRectsL( innerRect, outerRect );

	return frameBrush;
	}

// <cmail>
// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewListSelectorBrushL
// NOTE: Gen UI list gets the ownership of the selector brush and deletes it
// when the selector is reset, so we need to construct new brush every time.
// Note: now its is changed to have ownership in texturemanager
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::NewListSelectorBrushL()
    {
    FUNC_LOG;

    if ( iNewFrameBrush )
    	{
    	// A brush is already constructed. Do return it.
    	return iNewFrameBrush;
    	}

    iNewFrameBrush = CAlfFrameBrush::NewL( *iEnv, KAknsIIDQsnFrList,
										   /* aEdgeOffsetX */ -2,
										   /* aEdgeOffsetY */ -1 );

    const TRect outerRect( 0, 0, 300, 80 );
    TRect innerRect( outerRect );
    innerRect.Shrink( 10, 10 );

    iNewFrameBrush->SetFrameRectsL( innerRect, outerRect);

    return iNewFrameBrush;
    }
// </cmail>


// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::GridSelectorBrushL
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::GridSelectorBrushL()
	{
    FUNC_LOG;
	return ListSelectorBrushL();
	}


// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewScrollBarTopBgBrushLC
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::NewScrollBarTopBgBrushLC()
	{
    FUNC_LOG;
	CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewLC( *iEnv, KAknsIIDQsnCpScrollBgTop );
	// Fix these
	TRect testOuterRect(10,10,57,57);
	TRect testInnerRect(12,12,56,56);
	frameBrush->SetFrameRectsL(testInnerRect, testOuterRect);
	return frameBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewScrollBarBottomBgBrushLC
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::NewScrollBarBottomBgBrushLC()
	{
    FUNC_LOG;
	CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewLC( *iEnv, KAknsIIDQsnCpScrollBgBottom );
	// Fix these
	TRect testOuterRect(10,10,57,57);
	TRect testInnerRect(12,12,56,56);
	frameBrush->SetFrameRectsL(testInnerRect, testOuterRect);
	return frameBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewScrollBarCenterBgBrushLC
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::NewScrollBarCenterBgBrushLC()
	{
    FUNC_LOG;
	CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewLC( *iEnv, KAknsIIDQsnCpScrollBgMiddle );
	// Fix these
	TRect testOuterRect(10,10,57,57);
	TRect testInnerRect(12,12,56,56);
	frameBrush->SetFrameRectsL(testInnerRect, testOuterRect);
	return frameBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewScrollBarThumbBrushLC
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::NewScrollBarThumbBrushLC()
	{
    FUNC_LOG;
	CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewLC( *iEnv, KAknsIIDQsnCpScrollHandleMiddle );
	// Fix these
	TRect testOuterRect(10,10,57,57);
	TRect testInnerRect(12,12,56,56);
	frameBrush->SetFrameRectsL(testInnerRect, testOuterRect);
	return frameBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewMailListSeparatorBgBrushLC
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::NewMailListSeparatorBgBrushLC()
	{
    FUNC_LOG;
	CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewLC( *iEnv, KAknsIIDQsnFrStatusFlat );
	// Fix these
	TRect testOuterRect(10,10,57,57);
	TRect testInnerRect(12,12,56,56);
	frameBrush->SetFrameRectsL(testInnerRect, testOuterRect);
	return frameBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewMailListMarkingModeBgBrushLC
// -----------------------------------------------------------------------------
//
CAlfImageBrush* CFreestyleEmailUiTextureManager::NewMailListMarkingModeBgBrushLC()
    {
    FUNC_LOG;
    CAlfTexture& circle = TextureByIndex( EMarkingModeBackgroundIcon );
    CAlfImageBrush* bgBrush = CAlfImageBrush::NewLC( *iEnv, TAlfImage(circle) );
    bgBrush->SetLayer( EAlfBrushLayerBackground );
    bgBrush->SetScaleMode( CAlfImageVisual::EScaleNormal );
    return bgBrush;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewControlBarListBgBrushLC
// -----------------------------------------------------------------------------
//
CAlfImageBrush* CFreestyleEmailUiTextureManager::NewControlBarListBgBrushLC()
	{
    FUNC_LOG;
    CAlfTexture& circle = TextureByIndex( EControlBarListBackgroundTexture );
    TInt borderSize = iAppUi->LayoutHandler()->ControlBarListBorderRoundingSize();
    CAlfImageBrush* bgBrush = CAlfImageBrush::NewLC( *iEnv, TAlfImage(circle) );
    bgBrush->SetBorders(-borderSize, -borderSize, -borderSize, -borderSize);
    bgBrush->SetLayer(EAlfBrushLayerBackground);
    return bgBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::NewControlBarButtonBgBrushLC
// -----------------------------------------------------------------------------
//
CAlfImageBrush* CFreestyleEmailUiTextureManager::NewControlBarButtonBgBrushLC()
	{
    FUNC_LOG;
    CAlfTexture& circle = TextureByIndex( EControlBarButtonBackgroundTexture );
    TInt borderSize = iAppUi->LayoutHandler()->ControlBarButtonBgRoundingSize();
    CAlfImageBrush* bgBrush = CAlfImageBrush::NewLC( *iEnv, TAlfImage(circle) );
    bgBrush->SetBorders(-borderSize, -borderSize, -borderSize, -borderSize);
    bgBrush->SetLayer(EAlfBrushLayerBackground);
    return bgBrush;
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::TitleDividerBgBrushL
// -----------------------------------------------------------------------------
//
CAlfFrameBrush* CFreestyleEmailUiTextureManager::TitleDividerBgBrushL()
    {
    FUNC_LOG;

    if ( !iTitleDividerBgBrush )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();

        TAknsItemID skinId = KAknsIIDQsnFrTitleDivider;

        CAknsItemData* temp = skin->GetCachedItemData( skinId );

        if ( !temp )
            {
            User::Leave( KErrNotFound );
            }

        CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewLC( *iEnv, skinId );

        TRect testOuterRect( 0, 0, 300, 80 );
        TRect testInnerRect( 4, 4, 296, 76 );

        frameBrush->SetFrameRectsL( testInnerRect, testOuterRect );

        CleanupStack::Pop( frameBrush );

        iTitleDividerBgBrush = frameBrush;
        }

    return iTitleDividerBgBrush;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::BackgroundForThisResolutionL
// -----------------------------------------------------------------------------
//
CAlfTexture& CFreestyleEmailUiTextureManager::BackgroundForThisResolutionL()
	{
    FUNC_LOG;
	return CAlfStatic::Env().TextureManager().BlankTexture();
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::WatermarkBackgroundForThisResolutionL
// -----------------------------------------------------------------------------
//
CAlfTexture& CFreestyleEmailUiTextureManager::WatermarkBackgroundForThisResolutionL()
	{
    FUNC_LOG;
	return CAlfStatic::Env().TextureManager().BlankTexture();
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::CreateTexturesL
// Textures loaded here are loaded on AppUi ConstructL
// I.e. they make application startup slower
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiTextureManager::CreateTexturesL()
	{
    FUNC_LOG;

//	for ( TInt i=ETextureGridFirst; i<ETextureFlagFirst; i++ )
	for ( TInt i=ETextureFirst; i<ETextureCount; i++ )
		{
		TFSEmailUiTextures idx = (TFSEmailUiTextures)i;
		LoadTextureL( idx );
		}
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::CreateBrandedMailboxTexture
// This function must be called from UI before TextureByMailboxIdL call.
// This function takes temporary copy of mailbox icon into TextureManager
// Caller has got the aIcon from the BrandingManager.
// TSize information is used because Alfred does not support SetSize for
// textures. Size(0,0) is used for original size
// -----------------------------------------------------------------------------
void CFreestyleEmailUiTextureManager::CreateBrandedMailboxTexture(
		  CGulIcon* aIcon, const TUid aPluginId, const TUint aMailboxId, const TSize& aSize )
	{
    FUNC_LOG;
	if ( iTmpBitmap || iTmpMaskBitmap )
		{
		iTmpBitmap = NULL;
		iTmpMaskBitmap = NULL;
		}
	TBool found = EFalse;
	// Check if icon already axcist
    for ( TInt i = 0; i < iBrandedMailboxIcons.Count(); i++ )
        {
        if ( iPluginId[i] == aPluginId && iMailboxId[i] == aMailboxId && iSize[i] == aSize )
        	{
        	found = ETrue;
        	}
        }
    if ( !found )
    	{
    	// Take ownership of the icon which is originally got from the BrandingManager
    	// Store bitmap and mask into temporary variables
    	aIcon->SetBitmapsOwnedExternally( ETrue );
    	iTmpBitmap = aIcon->Bitmap();
    	iTmpMaskBitmap = aIcon->Mask();
     	iTmpPluginId = aPluginId;
    	iTmpMailboxId = aMailboxId;
    	iTmpSize = aSize;
    	}
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::ProvideBitmapL
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiTextureManager::ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap)
	{
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRect screenRect;
 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
 	TSize iconSize;
 	if ( aId >= EBrandedIconStart && aId <= EBrandedIconEnd )
 		{
 		// Do not set size
 		}
 	else if ( aId >= ETextureGridFirst ) // GRID ICONS
		{
// <cmail> Platform layout change
	   	//TInt gridIconSize = iAppUi->LayoutHandler()->GridIconSize();
		//iconSize.SetSize(  gridIconSize, gridIconSize );
		iconSize = iAppUi->LayoutHandler()->GridIconSize();
// </cmail>
		}
	else  if ( aId >= ETextureMessageFirst /*&& aId < ETextureCalendarFirst*/ ) // MESSAGE ICONS
		{
		iconSize.SetSize( iAppUi->LayoutHandler()->MailIconSizeInThisResolution().iWidth,
						  iAppUi->LayoutHandler()->MailIconSizeInThisResolution().iHeight );
		}

    TFileName iconFileName;
    TFsEmailUiUtility::GetFullIconFileNameL( iconFileName );

  	CFbsBitmap* bitmap(0);
  	CFbsBitmap* mask(0);
	TScaleMode scalemode = EAspectRatioPreserved;

// <cmaill> icons changed
	switch ( aId )
		{
		// LIST TEXTURE READING STARTS HERE
		//
		case ESearchLookingGlassIcon:
			{
			TInt tempsize = iAppUi->LayoutHandler()->SearchLookingGlassIconSize();
			iconSize.SetSize( tempsize, tempsize );
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_search,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_search_mask);
			}
			break;
		case EMailListBarBgIcon:
			{
			TInt tempsize = iAppUi->LayoutHandler()->EMailListBarBgIconSize();
			// Get control bar grafix from skin, otherwise use default.
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFsGrafEmailCtrlbar, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_graf_cmail_email_ctrlbar,
            	EMbmFreestyleemailuiQgn_graf_cmail_email_ctrlbar_mask );
			iconSize.SetSize( screenRect.Width(), tempsize  );
			scalemode = EAspectRatioNotPreserved;
			}
			break;
		case EListControlBarMailboxDefaultIcon:
			{
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_drop_email_account,
	                             EMbmFreestyleemailuiQgn_indi_cmail_drop_email_account_mask);
			}
			break;
			// Same than previous, but needed in two places at same screen
		case EListTextureMailboxDefaultIcon:
			{
			TInt tempsize = iAppUi->LayoutHandler()->ListTextureMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_drop_inbox,
	                             EMbmFreestyleemailuiQgn_indi_cmail_drop_inbox_mask);
			}
			break;
		case EListTextureNodeExpanded:
			{
			TInt tempsize = iAppUi->LayoutHandler()->ListTextureNodeExpandedSize();
			iconSize.SetSize( tempsize, tempsize );
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFsIndiMinusSign, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_indi_cmail_collapse,
            	EMbmFreestyleemailuiQgn_indi_cmail_collapse_mask );
			}
			break;
		case EListTextureNodeCollapsed:
			{
            TInt tempsize = iAppUi->LayoutHandler()->ListTextureNodeExpandedSize();
            iconSize.SetSize( tempsize, tempsize );
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFsIndiPlusSign, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_indi_cmail_expand,
            	EMbmFreestyleemailuiQgn_indi_cmail_expand_mask );
			}
			break;
		case EListTextureSelector:
			{
			TInt tempsize = iAppUi->LayoutHandler()->EListTextureSelectorSize();
            iconSize.SetSize( tempsize, tempsize );
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_graf_cmail_list_selector,
	                             EMbmFreestyleemailuiQgn_graf_cmail_list_selector_mask);
			}
			break;
		case EListControlMarkIcon:
			{
			// <cmail> Platform layout change
			// Get mark icon from the skin, otherwise use default
            AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnFsListItemSelected,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG13,
                bitmap,
                mask,
                //KAvkonBitmapFile,
                iconFileName,
                EMbmFreestyleemailuiQgn_indi_marked_add,
                EMbmFreestyleemailuiQgn_indi_marked_add_mask,
                KRgbBlack );
            TInt tempsize = iAppUi->LayoutHandler()->SearchLookingGlassIconSize();
            iconSize.SetSize( tempsize, tempsize );
			scalemode = EAspectRatioNotPreserved;
			// </cmail> Platform layout change
			}
			break;
		case EListControlMenuIcon:
			{
			// Use mirrored version of the icon on Arabic/Hebrew layout
			if ( AknLayoutUtils::LayoutMirrored() )
			    {
	            AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
	                KAknsIIDQgnFsHscrActionArrowLeft, bitmap, mask, iconFileName,
	                EMbmFreestyleemailuiQgn_indi_cmail_arrow_left,
	                EMbmFreestyleemailuiQgn_indi_cmail_arrow_left_mask );
			    }
			else
			    {
	            AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
	                KAknsIIDQgnFsHscrActionArrowRight, bitmap, mask, iconFileName,
	                EMbmFreestyleemailuiQgn_indi_cmail_arrow_right,
	                EMbmFreestyleemailuiQgn_indi_cmail_arrow_right_mask );
			    }

            TInt tempsize = iAppUi->LayoutHandler()->ListControlMenuIconSize();
            iconSize.SetSize( tempsize, tempsize );
			scalemode = EAspectRatioNotPreserved;
			}
			break;
		case EListTextureNewEmailDefaultIcon:
			{
	        TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
	        iconSize.SetSize( tempsize, tempsize );
	        AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	        	EMbmFreestyleemailuiQgn_indi_tb_email_new,
	        	EMbmFreestyleemailuiQgn_indi_tb_email_new_mask );
			}
			break;

        case EListTextureCreateNewMessageIcon:
            {
            TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
            iconSize.SetSize( tempsize, tempsize );
            AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
                EMbmFreestyleemailuiQgn_prop_cmail_action_create_msg,
                EMbmFreestyleemailuiQgn_prop_cmail_action_create_msg_mask );
            }
            break;			
			
		// MESSAGE TEXTURE READING STARTS HERE
	 	//
		case EMessageReadIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_mask);
			}
			break;
		case EMessageReadIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_attach,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_attach_mask);
			}
			break;
		case EMessageUnreadIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_mask);
			}
			break;
		case EMessageLowPrioReadIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_low_prio_mask);
			}
			break;
		case EMessageLowPrioReadIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_attach_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_attach_low_prio_mask);
			}
			break;
		case EMessageHighPrioReadIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_attach_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_attach_high_prio_mask);
			}
			break;
		case EMessageUnreadIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_attach,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_attach_mask);
			}
			break;
		case EMessageHighPrioUnreadIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_attach_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_attach_high_prio_mask);
			}
			break;
		case EMessageLowPrioUnreadIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_low_prio_mask);
			}
			break;
		case EMessageLowPrioUnreadIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_attach_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_attach_low_prio_mask);
			}
			break;
		case EMessageHighPrioReadIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_high_prio_mask);
			}
			break;
		case EMessageHighPrioUnreadIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_high_prio_mask);
			}
			break;
		case EMessageReadRepliedIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_mask);
			}
			break;
		case EMessageReadRepliedIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_attach,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_attach_mask);
			}
			break;
		case EMessageReadRepliedHighPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_high_prio_mask);
			}
			break;
		case EMessageReadRepliedHighPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_attach_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_attach_high_prio_mask);
			}
			break;
		case EMessageReadRepliedLowPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_low_prio_mask);
			}
			break;
		case EMessageReadRepliedLowPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_attach_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_replied_attach_low_prio_mask);
			}
			break;
		case EMessageReadForwardedIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_mask);
			}
			break;
		case EMessageReadForwardedIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_attach,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_attach_mask);
			}
			break;
		case EMessageReadForwardedHighPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_high_prio_mask);
			}
			break;
		case EMessageReadForwardedHighPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_attach_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_attach_high_prio_mask);
			}
			break;
		case EMessageReadForwardedLowPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_low_prio_mask);
			}
			break;
		case EMessageReadForwardedLowPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_attach_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_read_forwarded_attach_low_prio_mask);
			}
			break;
		case EMessageUnreadForwardedIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_mask);
			}
			break;
		case EMessageUnreadForwardedHighPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_high_prio);
			}
			break;
		case EMessageUnreadForwardedLowPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_low_prio_mask);
			}
			break;
		case EMessageUnreadForwardedIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_attach,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_attach_mask);
			}
			break;
		case EMessageUnreadForwardedHighPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_attach_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_attach_high_prio_mask);
			}
			break;
		case EMessageUnreadForwardedLowPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_attach_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_forwarded_attach_low_prio_mask);
			}
			break;
		case EMessageUnreadRepliedIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_mask);
			}
			break;

		case EMessageUnreadRepliedIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_attach,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_attach_mask);
			}
			break;
		case EMessageUnreadRepliedHighPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_high_prio_mask);
			}
			break;
		case EMessageUnreadRepliedHighPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_attach_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_attach_high_prio_mask);
			}
			break;
		case EMessageUnreadRepliedLowPrioIcon:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_low_prio_mask);
			}
			break;
		case EMessageUnreadRepliedLowPrioIconAttachment:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_attach_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_unread_replied_attach_low_prio_mask);
			}
			break;
		case EAttachmentIcon:
			{
            //<cmail>
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //iconSize.SetSize( tempsize, tempsize );
			iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //</cmail>

	        AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                EMbmFreestyleemailuiQgn_indi_cmail_attachment,
	                EMbmFreestyleemailuiQgn_indi_cmail_attachment_mask );
			}
			break;
	 	//
	 	// MESSAGE ICON READING STOPS HERE


		// CALENDAR EVENT READING STARTS HERE
		case EMessageCalInvitationRead:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_mask);
			}
			break;
		case EMessageCalInvitation:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_mask);
			}
			break;
		case EMessageCalInvitationAttachments:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_attachments,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_attachments_mask);
			}
			break;
		case EMessageCalInvitationReadAttachments:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_attachments,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_attachments_mask);
			}
			break;
		case EMessageCalInvitationHighPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_high_prio_mask);
			}
			break;
		case EMessageCalInvitationReadHighPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_high_prio_mask);
			}
			break;
		case EMessageCalInvitationAttachmentsHighPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_attachments_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_attachments_high_prio_mask);
			}
			break;
		case EMessageCalInvitationReadAttachmentsHighPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_attachments_high_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_attachments_high_prio_mask);
			}
			break;
		case EMessageCalInvitationLowPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_low_prio_mask);
			}
			break;
		case EMessageCalInvitationReadLowPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_low_prio_mask);
			}
			break;
		case EMessageCalInvitationAttachmentsLowPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_attachments_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_unread_attachments_low_prio_mask);
			}
			break;
		case EMessageCalInvitationReadAttachmentsLowPrio:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_attachments_low_prio,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_attachments_low_prio_mask);
			}
			break;
		case EMessageCalInvitationCancelled:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_delete,
	                             EMbmFreestyleemailuiQgn_indi_cmail_calendar_delete_mask);
			}
			break;
		// CALENDAR EVENT READING STOPS HERE

	 	// FLAG ICONS
		case EFollowUpFlag:
			{
            TInt tempsize = iAppUi->LayoutHandler()->ListControlMenuIconSize();
            iconSize.SetSize( tempsize, tempsize );
            AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDNone,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG7,
                bitmap,
                mask,
                iconFileName,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up_mask,
                KRgbBlack );
			}
			break;
		case EFollowUpFlagComplete:
			{
            TInt tempsize = iAppUi->LayoutHandler()->ListControlMenuIconSize();
            iconSize.SetSize( tempsize, tempsize );
            AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDNone,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG7,
                bitmap,
                mask,
                iconFileName,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up_complete,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up_complete_mask,
                KRgbBlack );
			}
			break;
			
    case EFollowUpFlagList:
            {
            TInt tempsize = iAppUi->LayoutHandler()->ListControlMenuIconSize();
            iconSize.SetSize( tempsize, tempsize );
            AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
                                 EMbmFreestyleemailuiQgn_indi_cmail_viewer_follow_up,
                                 EMbmFreestyleemailuiQgn_indi_cmail_viewer_follow_up_mask);
            }
            break;
    case EFollowUpFlagCompleteList:
            {
            TInt tempsize = iAppUi->LayoutHandler()->ListControlMenuIconSize();
            iconSize.SetSize( tempsize, tempsize );
            AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
                                 EMbmFreestyleemailuiQgn_indi_cmail_viewer_follow_up_complete,
                                 EMbmFreestyleemailuiQgn_indi_cmail_viewer_follow_up_complete_mask);
            }
            break;
			
	 	// END OF FLAG ICON S


		// STATUS INDICATOR ICON READING STARTS HERE
	 	//
		case EStatusTextureSynchronising:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_sync,
	                             EMbmFreestyleemailuiQgn_indi_cmail_sync_mask);
			}
			break;

		case EStatusTextureConnected:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_connected,
	                             EMbmFreestyleemailuiQgn_indi_cmail_connected_mask);
			}
			break;

		case EStatusTextureConnecting:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_connecting,
	                             EMbmFreestyleemailuiQgn_indi_cmail_connecting_mask);
			}
			break;

		case EStatusTextureDisconnectedGeneral:
			{
			/*AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_disconnected,
	                             EMbmFreestyleemailuiQgn_indi_cmail_disconnected_mask);
	                             */
			TSize iconSize(1,1);
			bitmap = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL( bitmap );
            mask = new( ELeave ) CFbsBitmap();
            CleanupStack::Pop( bitmap );
            bitmap->Create( iconSize, CCoeEnv::Static()->ScreenDevice()->DisplayMode() );
            mask->Create( iconSize, CCoeEnv::Static()->ScreenDevice()->DisplayMode() );

            TBitmapUtil bitmapUtil( bitmap );
            TBitmapUtil bitmapMaskUtil( mask );
            bitmapUtil.Begin( TPoint( 0, 0 ) );
            bitmapMaskUtil.Begin( TPoint( 0, 0 ) );
            bitmapUtil.SetPixel( 0x0 );
            bitmapMaskUtil.SetPixel( 0xFFFFFFFF );

            bitmapUtil.End();
            bitmapMaskUtil.End();
			}
			break;

		case EStatusTextureDisconnectedError:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_disconnected_error,
	                             EMbmFreestyleemailuiQgn_indi_cmail_disconnected_error_mask);
			}
			break;

		case EStatusTextureDisconnectedLowBattery:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_disconnected_low_battery,
	                             EMbmFreestyleemailuiQgn_indi_cmail_disconnected_low_battery_mask);
			}
			break;

		case EStatusTextureMailSent:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_sent,
	                             EMbmFreestyleemailuiQgn_indi_cmail_sent_mask);
			}
			break;
		case EBackgroundTextureMailList:
			{
			// <cmail> S60 Skin support
			/*
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFsGrafEmailContent, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_graf_cmail_email_content,
            	EMbmFreestyleemailuiQgn_graf_cmail_email_content_mask );
            	*/
            GetBackgroundBitmapL (bitmap);
			// </cmail>

	 	    iconSize.SetSize( screenRect.Width(), screenRect.Height() );
			scalemode = EAspectRatioNotPreserved;
			}
			break;

		// CONTROL BAR BACKGROUND ICON READING STARTS HERE
	 	//
		case EControlBarListBackgroundTexture:
			{
            // <cmail> S60 Skin support
	        TInt borderSize =
	        	iAppUi->LayoutHandler()->ControlBarListBorderRoundingSize();
			iconSize.SetSize( borderSize * 2, borderSize * 2 );

			this->DrawContextL (bitmap, KAknsIIDQsnFrPopupCenter, iconSize);
            //</cmail>
			}
			break;

		case EControlBarButtonBackgroundTexture:
			{
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFsGrafDropOnepc, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_graf_cmail_drop_onepc,
            	EMbmFreestyleemailuiQgn_graf_cmail_drop_onepc_mask );
	        TInt borderSize = iAppUi->LayoutHandler()->ControlBarButtonBgRoundingSize();
			iconSize.SetSize( borderSize * 2, borderSize * 2 );
			}
			break;

// Ascending /descending textures are mixed in skin
		case EControlBarDescendingArrowTexture:
			{
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFseFolderListAscendingIcon, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_indi_cmail_folders_asc,
            	EMbmFreestyleemailuiQgn_indi_cmail_folders_asc_mask );
		    TInt TempXSize = iAppUi->LayoutHandler()->ControlBarArrowTextureXSize();
		    TInt TempYSize = iAppUi->LayoutHandler()->ControlBarArrowTextureYSize();
		    iconSize.SetSize( TempXSize, TempYSize );
		    }
		    break;
		case EControlBarAscendingArrowTexture:
			{
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFseFolderListDescendingIcon, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_indi_cmail_folders_desc,
            	EMbmFreestyleemailuiQgn_indi_cmail_folders_desc_mask );
	        TInt TempXSize = iAppUi->LayoutHandler()->ControlBarArrowTextureXSize();
	        TInt TempYSize = iAppUi->LayoutHandler()->ControlBarArrowTextureYSize();
	        iconSize.SetSize( TempXSize, TempYSize );
			}
			break;

		// FOLDER LIST ICON READING STARTS HERE
	 	//
        case EFolderListMoreFoldersTexture:
        case EFolderListMoreFoldersTexturePopup:
		   
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_indi_cmail_drop_more_folders,
									   EMbmFreestyleemailuiQgn_indi_cmail_drop_more_folders_mask);
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListMoreFoldersTexturePopup );
			}
			break;
        case EFolderListInboxTexture:
        case EFolderListInboxTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_inbox,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_inbox_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListInboxTexturePopup );
			}
			break;
        case EFolderListInboxSubfoldersTexture:
        case EFolderListInboxSubfoldersTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_subfolders,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_subfolders_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListInboxSubfoldersTexturePopup );
			}
			break;
        case EFolderListOutboxTexture:
        case EFolderListOutboxTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_outbox,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_outbox_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListOutboxTexturePopup );
			}
			break;
        case EFolderListDraftsTexture:
        case EFolderListDraftsTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_drafts,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_drafts_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListDraftsTexturePopup );
			}
			break;
        case EFolderListSentTexture:
        case EFolderListSentTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_sent,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_sent_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListSentTexturePopup );
			}
			break;
        case EFolderListDeletedItemsTexture:
        case EFolderListDeletedItemsTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_deleted_items,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_deleted_items_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListDeletedItemsTexturePopup );
			}
			break;
        case EFolderListServerFoldersTexture:
        case EFolderListServerFoldersTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_server_folder,
									   EMbmFreestyleemailuiQgn_prop_cmail_folders_server_folder_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListServerFoldersTexturePopup );
			}
			break;
        case EFolderListEmailAccountTexture:
        case EFolderListEmailAccountTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_indi_cmail_drop_email_account,
									   EMbmFreestyleemailuiQgn_indi_cmail_drop_email_account_mask);
            iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == EFolderListEmailAccountTexturePopup );
			}
			break;

		// FOLDER LIST ICON READING STOPS HERE
		//
			
		// SORT LIST ICON READING STARTS HERE
		//
        case ESortListAttachmentAscTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_attach_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_attach_asc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListAttachmentAscTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_attach_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_attach_asc_mask );
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListAttachmentAscTexturePopup );
			}
			break;	
        case ESortListAttachmentDescTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_attach_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_attach_desc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListAttachmentDescTexturePopup:
        	{
        	AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_attach_desc,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_attach_desc_mask );
        	iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListAttachmentDescTexturePopup );
        	}
        	break;
        case ESortListDateAscTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_date_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_date_asc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListDateAscTexturePopup:
        	{
        	AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_date_asc,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_date_asc_mask );
        	iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListDateAscTexturePopup );
        	}
        	break;
        case ESortListDateDescTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_date_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_date_desc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListDateDescTexturePopup:
        	{
        	AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_date_desc,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_date_desc_mask );
        	iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListDateDescTexturePopup );
        	}
        	break;
        case ESortListFollowAscTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_follow_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_follow_asc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListFollowAscTexturePopup:
        	{
        	AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_follow_asc,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_follow_asc_mask );
        	iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListFollowAscTexturePopup );
        	}
        	break;
        case ESortListFollowDescTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_follow_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_follow_desc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListFollowDescTexturePopup:
        	{
        	AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_follow_desc,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_follow_desc_mask );
        	iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListFollowDescTexturePopup );
        	}
        	break;
        case ESortListPriorityAscTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_priority_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_priority_asc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListPriorityAscTexturePopup:
        	{
        	AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_priority_asc,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_priority_asc_mask );
        	iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListPriorityAscTexturePopup );
        	}
        	break;
        case ESortListPriorityDescTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_priority_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_priority_desc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListPriorityDescTexturePopup:
        	{
        	AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_priority_desc,
        							   EMbmFreestyleemailuiQgn_prop_email_sort_priority_desc_mask );
        	iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListPriorityDescTexturePopup );
        	}
        	break;
        case ESortListSenderAscTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_asc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListSenderAscTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_asc_mask );
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListSenderAscTexturePopup );
			}
			break;
        case ESortListSenderDescTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_desc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListSenderDescTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_sender_desc_mask );
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListSenderDescTexturePopup );
			}
			break;
        case ESortListSubjectAscTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_asc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListSubjectAscTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_asc_mask );
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListSubjectAscTexturePopup );
			}
			break;
        case ESortListSubjectDescTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_desc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListSubjectDescTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_subject_desc_mask );
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListSubjectDescTexturePopup );
			}
			break;
        case ESortListUnreadAscTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_asc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListUnreadAscTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_asc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_asc_mask );
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListUnreadAscTexturePopup );
			}
			break;
        case ESortListUnreadDescTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_desc_mask );
			TInt tempsize = iAppUi->LayoutHandler()->ListControlBarMailboxDefaultIconSize();
			iconSize.SetSize( tempsize, tempsize );
			}
			break;
        case ESortListUnreadDescTexturePopup:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_desc,
									   EMbmFreestyleemailuiQgn_prop_email_sort_unread_desc_mask );
			iconSize = iAppUi->LayoutHandler()->FolderListIconSize( aId == ESortListUnreadDescTexturePopup );
			}
			break;
			// SORT LIST ICON READING STOPS HERE
		//
			
		// ATTACHMENT ICONS STARTS HERE
		//
        //<cmail>
		case EAttachmentsDocFile:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_doc,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_doc_mask);
            //TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsHtmlFile:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_html,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_html_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsImageFile:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_image,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_image_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsNotDownloaded:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_not_downloaded,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_not_downloaded_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsPptFile:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_ppt,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_ppt_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsRtfFile:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_rtf,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_rtf_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsUnknownFile:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_unidentified,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_unidentified_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsXls:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_xls,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_xls_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

		case EAttachmentsPdfFile:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_pdf,
	                             EMbmFreestyleemailuiQgn_indi_cmail_viewer_attach_pdf_mask);
			//TInt tempsize = iAppUi->LayoutHandler()->AttachmentIconSize();
			//iconSize.SetSize( tempsize, tempsize );
            iconSize = iAppUi->LayoutHandler()->AttachmentIconSize();
            //scalemode = EAspectRatioNotPreserved;
			}
			break;

        //</cmail>


		case EAttachmentsCancelDownload:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_cancel_download,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_cancel_download_mask);
			}
			break;

		case EAttachmentsDownload:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_download,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_download_mask);
			}
			break;

		case EAttachmentsSaveAll:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_save_all,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_save_all_mask);
			}
			break;

		case EAttachmentsCancelAll:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_cancel_all,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_cancel_all_mask);
			}
			break;

		case EAttachmentsViewAll:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_view_all,
	                             EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_view_all_mask);
			}
			break;



		// GRID ICON READING STARTS HERE
	 	//
		case EGridInboxTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
									   EMbmFreestyleemailuiQgn_indi_cmail_launcher_inbox,
									   EMbmFreestyleemailuiQgn_indi_cmail_launcher_inbox_mask);
			}
			break;
		case EGridDirectoryTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_remote_lookup,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_remote_lookup_mask);
			}
			break;
		case EGridSettingsTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_settings,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_settings_mask);
			}
			break;
		case EGridAddNewBoxTexture:
			{
			// <cmail>
			TAknsItemID skinId = KAknsIIDQgnPropCmailNewMailbox;
			CAknsItemData* temp = AknsUtils::SkinInstance()->GetCachedItemData( skinId );
			if ( !temp )
				{
				skinId = KAknsIIDNone;
				}

			AknsUtils::CreateIconL(
					AknsUtils::SkinInstance(),
	                skinId, bitmap,
	                mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_prop_cmail_new_mailbox,
	                             EMbmFreestyleemailuiQgn_prop_cmail_new_mailbox_mask);
			// </cmail>
			}
			break;
		case EGridHelpTexture:
			{
			AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_help,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_help_mask);
			}
			break;
		case EGridBackground:
			{
			// <cmail> S60 Skin support
			/*
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFsCaleBgLsc, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_graf_cmail_email_content,
            	EMbmFreestyleemailuiQgn_graf_cmail_email_content_mask );
            */
           	GetBackgroundBitmapL (bitmap);
           	// </cmail>
	 	    iconSize.SetSize( screenRect.Width(), screenRect.Height() );
			scalemode = EAspectRatioNotPreserved;
			}
			break;

		// VIEWER ICONS
		//
		case EViewerTextureHeaderBackGround:
			{
	 	    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
    			KAknsIIDQgnFsGrafEmailHeader, bitmap, mask, iconFileName,
    			EMbmFreestyleemailuiQgn_graf_cmail_email_header,
            	EMbmFreestyleemailuiQgn_graf_cmail_email_header_mask );
	 	    iconSize.SetSize( screenRect.Width(), screenRect.Height() );
			scalemode = EAspectRatioNotPreserved;
			}
			break;
		case EViewerHighlightTexture:
			{
            TInt tempsize = iAppUi->LayoutHandler()->ViewerHighlightTextureSize();
            iconSize.SetSize( tempsize, tempsize );

		    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		    TRgb imageColor;
		    if( AknsUtils::GetCachedColor( skin, imageColor,
		    		KAknsIIDFsHighlightColors, EAknsCIFsHighlightColorsCG4 ) != KErrNone )
		        {
			    if( AknsUtils::GetCachedColor( skin, imageColor,
			    		KAknsIIDQsnHighlightColors, EAknsCIQsnHighlightColorsCG2 ) != KErrNone )
			    	{
			        imageColor = KRgbBlue;
			    	}
		        }
			bitmap = new( ELeave ) CFbsBitmap();
			CleanupStack::PushL( bitmap );
			mask = new( ELeave ) CFbsBitmap();
			CleanupStack::Pop( bitmap );
			bitmap->Create( iconSize, CCoeEnv::Static()->ScreenDevice()->DisplayMode() );
			mask->Create( iconSize, CCoeEnv::Static()->ScreenDevice()->DisplayMode() );

			TBitmapUtil bitmapUtil( bitmap );
			TBitmapUtil bitmapMaskUtil( mask );
			bitmapUtil.Begin( TPoint( 0, 0 ) );
			bitmapMaskUtil.Begin( TPoint( 0, 0 ) );
			bitmapUtil.SetPixel( imageColor.Internal() );
			bitmapMaskUtil.SetPixel( 0x0 );

			bitmapUtil.End();
			bitmapMaskUtil.End();
			}
			break;

		// COMPOSER ICONS
		//
		case EComposerTexturePriorityLow:
		    {
		    AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDNone,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG7,
                bitmap,
                mask,
                iconFileName,
                EMbmFreestyleemailuiQgn_indi_todo_low_add,
                EMbmFreestyleemailuiQgn_indi_todo_low_add_mask,
                KRgbBlack );
		    }
		    break;
		case EComposerTexturePriorityHigh:
		    {
		    AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDNone,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG7,
                bitmap,
                mask,
                iconFileName,
                EMbmFreestyleemailuiQgn_indi_todo_high_add,
                EMbmFreestyleemailuiQgn_indi_todo_high_add_mask,
                KRgbBlack );
		    }
		    break;
        case EMarkingModeBackgroundIcon:
            {
            TRect mailListRect(iAppUi->LayoutHandler()->GetListRect());
            TRect appRect;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, appRect);            
            if ( mailListRect.Width() > mailListRect.Height() )
                {
                iconSize.SetSize( appRect.Width(), appRect.Width() );            
                }
            else
                {
                iconSize.SetSize( appRect.Height(), appRect.Height() );            
                }
            AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
                                 EMbmFreestyleemailuiCmail_marking_mode_bg,
                                 EMbmFreestyleemailuiCmail_marking_mode_bg);
            }            
            break;
		case EGridAboutTexture:
		default:
			// Branded mailbox icon
			if ( aId >= EBrandedIconStart && aId <= EBrandedIconEnd )
				{
				if ( !iTmpBitmap || !iTmpMaskBitmap )
					{
					// Leave if tmpvariables not set. This will happen if CreateBrandedMailboxTexture
					// is not called before TextureByMailboxIdL
					User::Leave( KErrNotFound );
					}
				// Add new icon into array based on temporary variables set by CreateBrandedMailboxTexture()
				bitmap = iTmpBitmap;
				mask = iTmpMaskBitmap;
	           	if ( iTmpSize.iHeight != 0 && iTmpSize.iWidth != 0 )
	           		{
	           		iconSize.SetSize( iTmpSize.iWidth, iTmpSize.iHeight );
	           		}
	           	else
	           		{
	           		// Use original size
	           		iconSize.SetSize( bitmap->SizeInPixels().iWidth, bitmap->SizeInPixels().iHeight );
	           		}
				// Set all search id's (PluginId, MailboxId and IconSize)
	           	iPluginId.Append( iTmpPluginId );
	           	iMailboxId.Append( iTmpMailboxId );
	           	iSize.Append( iTmpSize );
	           	// Clear temporary variables
	           	iTmpPluginId.iUid = 0;
	           	iTmpMailboxId = 0;
	           	iTmpSize.iHeight = 0;
	           	iTmpSize.iWidth = 0;
	           	iTmpBitmap = NULL;
	           	iTmpMaskBitmap = NULL;
	           	}
			else
				{
                AknIconUtils::CreateIconL( bitmap, mask, iconFileName,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_info,
	                             EMbmFreestyleemailuiQgn_indi_cmail_launcher_info_mask);
				}
			break;
		}
// </cmail>

	if ( bitmap )
		{
		AknIconUtils::DisableCompression( bitmap );
		AknIconUtils::SetSize( bitmap, iconSize, scalemode );
		aBitmap = bitmap;
		}
	if ( mask )
		{
		AknIconUtils::DisableCompression( mask );
		AknIconUtils::SetSize( mask, iconSize, scalemode );
		aMaskBitmap = mask;
		}
	}


// -----------------------------------------------------------------------------
// <cmail> S60 Skin support
// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::DrawContextL()
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiTextureManager::DrawContextL (
	CFbsBitmap*& bitmap, TAknsItemID id, TSize rsize) const
	{
    FUNC_LOG;
	CAknsBasicBackgroundControlContext *skinContext =
		CAknsBasicBackgroundControlContext::NewL(
		//KAknsIIDQsnBgAreaMain,
		id, rsize, EFalse );
	CleanupStack::PushL (skinContext);

	bitmap = new (ELeave) CFbsBitmap();

	const TInt err1 = bitmap->Create( rsize,
		CCoeEnv::Static()->ScreenDevice()->DisplayMode() );
	User::LeaveIfError( err1 );

	CFbsBitGc* gc = 0;
	CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( bitmap );
	CleanupStack::PushL (device);

	const TInt err2 = device->CreateContext(gc);
	CleanupStack::PushL (gc);
	User::LeaveIfError( err2 );

	// Draw background to bitmap device context
	AknsDrawUtils::DrawBackground( AknsUtils::SkinInstance(),
	                          skinContext,
	                          //this,
	                          0,
	                          *gc,
	                          TPoint(0,0),
	                          rsize,
	                          KAknsDrawParamDefault );

	CleanupStack::PopAndDestroy(gc);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(skinContext);
	}


void CFreestyleEmailUiTextureManager::GetBackgroundBitmapL (CFbsBitmap*& bitmap) const
	{
    FUNC_LOG;
	const TSize rsize = iAppUi->ClientRect().Size();
	this->DrawContextL (bitmap, KAknsIIDQsnBgAreaMain, rsize);
	}
// </cmail>


// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::DelayLoadCallback()
// Delayed loading callback
// -----------------------------------------------------------------------------
//
TInt CFreestyleEmailUiTextureManager::DelayLoadCallback()
	{
    FUNC_LOG;
	// if all is loaded return 0 to indicate that we don't want any more callbacs
	if ( iDelayLoadIndex >= iTextures.Count() )
		{
		return 0;
		}
	else
		{
		TRAPD( error, LoadTextureL( (TFSEmailUiTextures)iDelayLoadIndex++ ) );
		if ( KErrNone != error )
			{
			}
		return 1;
		}
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::InitTextureArray()
// Initialises texture array
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiTextureManager::InitTextureArray()
	{
    FUNC_LOG;
	for ( TInt i=0; i<ETextureCount; i++ )
		{
		iTextures.Append( &CAlfStatic::Env().TextureManager().BlankTexture() );
		}
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiTextureManager::LoadTextureL()
// Loads given texture if not already loaded
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiTextureManager::LoadTextureL( TFSEmailUiTextures aTextureId )
	{
    FUNC_LOG;
	// Check if branded icon already loded
	if ( aTextureId >= EBrandedIconStart && aTextureId <= EBrandedIconEnd )
		{
		if ( aTextureId - EBrandedIconStart <= iBrandedMailboxIcons.Count() - 1 )
		    {
			// Branded icon already loaded do nothing
			return;
		    }
		}
	// if texture has already been loaded do nothing
	else if ( &CAlfStatic::Env().TextureManager().BlankTexture() != iTextures[aTextureId] )
		{
		return;
		}

	// load texture
	switch ( aTextureId )
		{
		case EListTextureSelector: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EListTextureSelector, this, EAlfTextureFlagDefault ); break;
		case EListTextureNodeExpanded:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EListTextureNodeExpanded, this, EAlfTextureFlagSkinContent);
			}
			break;
		case EListTextureNodeCollapsed:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EListTextureNodeCollapsed, this, EAlfTextureFlagSkinContent);
			}
			break;
		case EListTextureControlButton: ; break;
		case EListTextureMailboxDefaultIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EListTextureMailboxDefaultIcon, this, EAlfTextureFlagDefault); break;
        case EListTextureNewEmailDefaultIcon:
        case EListTextureCreateNewMessageIcon:
		    {
		    iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( aTextureId, this, EAlfTextureFlagDefault ); 
		    }
            break;
		case EListControlBarMailboxDefaultIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EListControlBarMailboxDefaultIcon, this, EAlfTextureFlagDefault); break;
		case EBackgroundTextureViewerHeading: ; break;
		case EBackgroundTextureBar: ; break;
		case EBackgroundTextureThumb: ; break;
		case EBackgroundTextureMailList:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EBackgroundTextureMailList, this, EAlfTextureFlagSkinContent);
			}
			break;
		case EListControlMarkIcon:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EListControlMarkIcon, this, EAlfTextureFlagSkinContent);
			break;
			}
		case EListControlMenuIcon:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EListControlMenuIcon, this, EAlfTextureFlagSkinContent);
			break;
			}
		case EMailListBarBgIcon:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMailListBarBgIcon, this, EAlfTextureFlagSkinContent);
			break;
			}
		case ESearchLookingGlassIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESearchLookingGlassIcon, this, EAlfTextureFlagDefault); break;
		case EControlBarListBackgroundTexture:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EControlBarListBackgroundTexture, this, EAlfTextureFlagSkinContent);
			break;
			}
		case EControlBarButtonBackgroundTexture:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EControlBarButtonBackgroundTexture, this, EAlfTextureFlagSkinContent);
			break;
			}
		case EControlBarAscendingArrowTexture:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EControlBarAscendingArrowTexture, this, EAlfTextureFlagSkinContent);
			break;
			}
		case EControlBarDescendingArrowTexture:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EControlBarDescendingArrowTexture, this, EAlfTextureFlagSkinContent);
			}
			break;
		case EMarkingModeBackgroundIcon: 
		    {
		    iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMarkingModeBackgroundIcon, this, EAlfTextureFlagRetainResolution);
		    }
		    break;
			
		// MESSAGE TEXTURES
		case EMessageReadIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadIcon, this, EAlfTextureFlagDefault); break;
		case EMessageLowPrioReadIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageLowPrioReadIcon, this, EAlfTextureFlagDefault); break;
		case EMessageLowPrioReadIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageLowPrioReadIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageReadIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageReadRepliedIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadRepliedIcon, this,EAlfTextureFlagDefault); break;
		case EMessageReadRepliedIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadRepliedIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageReadRepliedHighPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadRepliedHighPrioIcon, this, EAlfTextureFlagDefault); break;
		case EMessageReadRepliedHighPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadRepliedHighPrioIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageReadRepliedLowPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadRepliedLowPrioIcon, this, EAlfTextureFlagDefault ); break;
		case EMessageReadRepliedLowPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadRepliedLowPrioIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageReadForwardedIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadForwardedIcon, this, EAlfTextureFlagDefault); break;
		case EMessageReadForwardedIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadForwardedIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageReadForwardedHighPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadForwardedHighPrioIcon, this, EAlfTextureFlagDefault); break;
		case EMessageReadForwardedHighPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadForwardedHighPrioIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageReadForwardedLowPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadForwardedLowPrioIcon, this,EAlfTextureFlagDefault); break;
		case EMessageReadForwardedLowPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageReadForwardedLowPrioIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadIcon, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageLowPrioUnreadIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageLowPrioUnreadIcon, this, EAlfTextureFlagDefault); break;
		case EMessageLowPrioUnreadIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageLowPrioUnreadIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageHighPrioReadIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageHighPrioReadIcon, this, EAlfTextureFlagDefault); break;
		case EMessageHighPrioReadIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageHighPrioReadIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageHighPrioUnreadIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageHighPrioUnreadIcon , this, EAlfTextureFlagDefault); break;
		case EMessageHighPrioUnreadIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageHighPrioUnreadIconAttachment, this, EAlfTextureFlagDefault); break;

		// <cmail> Platform layout change
		//case EAttachmentIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentIcon, this, EAlfTextureFlagDefault); break;
		case EAttachmentIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentIcon, this, EAlfTextureFlagSkinContent/*EAlfTextureFlagDefault*/); break;
		// </cmail> Platform layout change

		case EMessageUnreadForwardedIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadForwardedIcon, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadForwardedIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadForwardedIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadForwardedHighPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadForwardedHighPrioIcon, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadForwardedHighPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadForwardedHighPrioIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadForwardedLowPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadForwardedLowPrioIcon, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadForwardedLowPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadForwardedLowPrioIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadRepliedIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadRepliedIcon, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadRepliedIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadRepliedIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadRepliedHighPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadRepliedHighPrioIcon, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadRepliedHighPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadRepliedHighPrioIconAttachment, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadRepliedLowPrioIcon: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadRepliedLowPrioIcon, this, EAlfTextureFlagDefault); break;
		case EMessageUnreadRepliedLowPrioIconAttachment: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageUnreadRepliedLowPrioIconAttachment, this, EAlfTextureFlagDefault); break;

		// CALENDAR EVENT TEXTURES
		case EMessageCalInvitation: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitation, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationRead: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationRead, this, EAlfTextureFlagDefault); break;
		case EMessageCalInvitationAttachments: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationAttachments, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationReadAttachments: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationReadAttachments, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationHighPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationHighPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationReadHighPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationReadHighPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationAttachmentsHighPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationAttachmentsHighPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationReadAttachmentsHighPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationReadAttachmentsHighPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationLowPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationLowPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationReadLowPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationReadLowPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationAttachmentsLowPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationAttachmentsLowPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationReadAttachmentsLowPrio: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationReadAttachmentsLowPrio, this,EAlfTextureFlagDefault); break;
		case EMessageCalInvitationCancelled: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EMessageCalInvitationCancelled, this,EAlfTextureFlagDefault); break;

		// GRID TEXTURES
		case EGridInboxTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EGridInboxTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EGridDirectoryTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EGridDirectoryTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EGridSettingsTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EGridSettingsTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EGridAddNewBoxTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EGridAddNewBoxTexture, this,EAlfTextureFlagRetainResolution); break;
	    case EGridHelpTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EGridHelpTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EGridAboutTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EGridAboutTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EGridBackground: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EGridBackground, this, EAlfTextureFlagDefault); break;

	    // FLAG ICONS
		case EFollowUpFlag: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFollowUpFlag, this, EAlfTextureFlagRetainResolution); break;
		case EFollowUpFlagComplete: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFollowUpFlagComplete, this, EAlfTextureFlagRetainResolution); break;
		case EFollowUpFlagList: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFollowUpFlagList, this, EAlfTextureFlagRetainResolution); break;
        case EFollowUpFlagCompleteList: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFollowUpFlagCompleteList, this, EAlfTextureFlagRetainResolution); break;

	    // FOLDER LIST ICONS
	    case EFolderListMoreFoldersTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListMoreFoldersTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListInboxTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListInboxTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListInboxSubfoldersTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListInboxSubfoldersTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListOutboxTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListOutboxTexture , this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListDraftsTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListDraftsTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListSentTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListSentTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListDeletedItemsTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListDeletedItemsTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListServerFoldersTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListServerFoldersTexture, this, EAlfTextureFlagRetainResolution); break;
	    case EFolderListEmailAccountTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListEmailAccountTexture, this, EAlfTextureFlagRetainResolution); break;

        case EFolderListMoreFoldersTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListMoreFoldersTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListInboxTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListInboxTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListInboxSubfoldersTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListInboxSubfoldersTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListOutboxTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListOutboxTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListDraftsTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListDraftsTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListSentTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListSentTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListDeletedItemsTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListDeletedItemsTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListServerFoldersTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListServerFoldersTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case EFolderListEmailAccountTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EFolderListEmailAccountTexturePopup, this, EAlfTextureFlagRetainResolution); break;

        // SORT LIST ICONS
        case ESortListAttachmentAscTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListAttachmentAscTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListAttachmentDescTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListAttachmentDescTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListDateAscTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListDateAscTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListDateDescTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListDateDescTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListFollowAscTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListFollowAscTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListFollowDescTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListFollowDescTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListPriorityAscTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListPriorityAscTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListPriorityDescTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListPriorityDescTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSenderAscTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSenderAscTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSenderDescTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSenderDescTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSubjectAscTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSubjectAscTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSubjectDescTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSubjectDescTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListUnreadAscTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListUnreadAscTexture, this, EAlfTextureFlagRetainResolution); break;
        case ESortListUnreadDescTexture: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListUnreadDescTexture, this, EAlfTextureFlagRetainResolution); break;
        
        // SORT LIST ICONS POPUP
        case ESortListAttachmentAscTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListAttachmentAscTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListAttachmentDescTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListAttachmentDescTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListDateAscTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListDateAscTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListDateDescTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListDateDescTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListFollowAscTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListFollowAscTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListFollowDescTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListFollowDescTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListPriorityAscTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListPriorityAscTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListPriorityDescTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListPriorityDescTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSenderAscTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSenderAscTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSenderDescTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSenderDescTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSubjectAscTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSubjectAscTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListSubjectDescTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListSubjectDescTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListUnreadAscTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListUnreadAscTexturePopup, this, EAlfTextureFlagRetainResolution); break;
        case ESortListUnreadDescTexturePopup: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( ESortListUnreadDescTexturePopup, this, EAlfTextureFlagRetainResolution); break;
                
		// STATUS INDICATOR ICONS
		case EStatusTextureSynchronising: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EStatusTextureSynchronising, this, EAlfTextureFlagRetainResolution); break;
		case EStatusTextureConnected: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EStatusTextureConnected, this, EAlfTextureFlagRetainResolution); break;
		case EStatusTextureConnecting: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EStatusTextureConnecting, this, EAlfTextureFlagRetainResolution); break;
		case EStatusTextureDisconnectedGeneral: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EStatusTextureDisconnectedGeneral, this, EAlfTextureFlagRetainResolution); break;
		case EStatusTextureDisconnectedError: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EStatusTextureDisconnectedError, this, EAlfTextureFlagRetainResolution); break;
		case EStatusTextureDisconnectedLowBattery: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EStatusTextureDisconnectedLowBattery, this, EAlfTextureFlagRetainResolution); break;
		case EStatusTextureMailSent: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EStatusTextureMailSent, this, EAlfTextureFlagRetainResolution); break;

		// VIEWER TEXTURES
		case EViewerTextureHeaderBackGround:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EViewerTextureHeaderBackGround, this, TAlfTextureFlags(EAlfTextureFlagRetainResolution|EAlfTextureFlagSkinContent)); // CHECKLATER - the EAlfTextureFlagSkinContent flag (instead of EAlfTextureFlagDefault) replaces the SetSkinContent command, also added two flags, do they work like this?
			}
			break;
		case EViewerHighlightTexture:
			{
			iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EViewerHighlightTexture, this, TAlfTextureFlags(EAlfTextureFlagRetainResolution|EAlfTextureFlagSkinContent)); // CHECKLATER - the EAlfTextureFlagSkinContent flag (instead of EAlfTextureFlagDefault) replaces the SetSkinContent command, also added two flags, do they work like this?
			}
			break;
		// ATTACHMENT TEXTURES
		case EAttachmentsDocFile: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsDocFile, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsHtmlFile: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsHtmlFile, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsImageFile: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsImageFile, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsNotDownloaded: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsNotDownloaded, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsPptFile: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsPptFile, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsRtfFile: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsRtfFile, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsUnknownFile: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsUnknownFile, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsXls: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsXls, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsPdfFile: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsPdfFile, this, EAlfTextureFlagRetainResolution); break;

		case EAttachmentsCancelDownload: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsCancelDownload, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsDownload: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsDownload, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsSaveAll: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsSaveAll, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsCancelAll: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsCancelAll, this, EAlfTextureFlagRetainResolution); break;
		case EAttachmentsViewAll: iTextures[aTextureId] = &CAlfStatic::Env().TextureManager().CreateTextureL( EAttachmentsViewAll, this, EAlfTextureFlagRetainResolution); break;
		default:
			// Branded mailbox icons
            if ( aTextureId >= EBrandedIconStart && aTextureId <= EBrandedIconEnd )
                {
                iBrandedMailboxIcons.Append( &CAlfStatic::Env().TextureManager().CreateTextureL( aTextureId, this, EAlfTextureFlagRetainResolution ) );
                }
            break;
		}
	}

