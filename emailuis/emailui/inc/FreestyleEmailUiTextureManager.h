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
* Description:  FreestyleEmailUi generic texturemanager definition
*
*/



#ifndef __FREESTYLEEMAILUI_TEXTUREMANAGER_H__
#define __FREESTYLEEMAILUI_TEXTUREMANAGER_H__


// SYSTEM INCLUDES
#include"FSDelayedLoader.h"
//<cmail> SF
#include <alf/alfimagebrush.h>
#include <alf/alfcontrol.h> 
#include <alf/alftexture.h>
#include <alf/alfbitmapprovider.h>
//</cmail>
#include <AknsBasicBackgroundControlContext.h>

// FORWARD DECLARATIONS
class CAlfEnv;
class CFreestyleEmailUiAppUi;
class CAlfFrameBrush;


enum TFSEmailUiTextures
	{
	// FIRST TEXTURE ID
	ETextureFirst = 6000, // non-texture
	
	// OTHER
	ETextureOtherFirst = 6001, // non-texture
	EListTextureSelector = ETextureOtherFirst,
	EListTextureNodeExpanded,
	EListTextureNodeCollapsed,
	EListTextureControlButton,
	EListTextureMailboxDefaultIcon,
	EListTextureNewEmailDefaultIcon,
	EListControlBarMailboxDefaultIcon,
	EBackgroundTextureViewerHeading,
	EBackgroundTextureBar,
	EBackgroundTextureThumb,
	EBackgroundTextureMailList,
	EListControlMarkIcon,
	EListControlMenuIcon,
	EMailListBarBgIcon,
	ESearchLookingGlassIcon,
	EControlBarListBackgroundTexture,
	EControlBarButtonBackgroundTexture,
	EControlBarAscendingArrowTexture,
	EControlBarDescendingArrowTexture,
	
	// MESSAGE TEXTURES
	ETextureMessageFirst, // non-texture
	EMessageReadIcon = ETextureMessageFirst,
	EMessageLowPrioReadIcon,
	EMessageLowPrioReadIconAttachment,
	EMessageReadIconAttachment,
	EMessageReadRepliedIcon,
	EMessageReadRepliedIconAttachment,
	EMessageReadRepliedHighPrioIcon,
	EMessageReadRepliedHighPrioIconAttachment,
	EMessageReadRepliedLowPrioIcon,		
	EMessageReadRepliedLowPrioIconAttachment,		
	EMessageReadForwardedIcon,
	EMessageReadForwardedIconAttachment,
	EMessageReadForwardedHighPrioIcon,
	EMessageReadForwardedHighPrioIconAttachment,
	EMessageReadForwardedLowPrioIcon,
	EMessageReadForwardedLowPrioIconAttachment,
	EMessageUnreadIcon,
	EMessageUnreadIconAttachment,
	EMessageLowPrioUnreadIcon,
	EMessageLowPrioUnreadIconAttachment,
	EMessageHighPrioReadIcon,
	EMessageHighPrioReadIconAttachment,
	EMessageHighPrioUnreadIcon,
	EMessageHighPrioUnreadIconAttachment,
	EAttachmentIcon,	
	EMessageUnreadForwardedIcon,
	EMessageUnreadForwardedIconAttachment,
	EMessageUnreadForwardedHighPrioIcon,
	EMessageUnreadForwardedHighPrioIconAttachment,
	EMessageUnreadForwardedLowPrioIcon,
	EMessageUnreadForwardedLowPrioIconAttachment,	
	EMessageUnreadRepliedIcon,
	EMessageUnreadRepliedIconAttachment,
	EMessageUnreadRepliedHighPrioIcon,
	EMessageUnreadRepliedHighPrioIconAttachment,
	EMessageUnreadRepliedLowPrioIcon,
	EMessageUnreadRepliedLowPrioIconAttachment,

	// CALENDAR EVENT TEXTURES
	ETextureCalendarFirst, // non-texture
	EMessageCalInvitation = ETextureCalendarFirst,
	EMessageCalInvitationRead,
	EMessageCalInvitationAttachments,
	EMessageCalInvitationReadAttachments,	
	EMessageCalInvitationHighPrio,
	EMessageCalInvitationReadHighPrio,
	EMessageCalInvitationAttachmentsHighPrio,
	EMessageCalInvitationReadAttachmentsHighPrio,
	EMessageCalInvitationLowPrio,
	EMessageCalInvitationReadLowPrio,
	EMessageCalInvitationAttachmentsLowPrio,
	EMessageCalInvitationReadAttachmentsLowPrio,
	EMessageCalInvitationCancelled,

	// GRID TEXTURES
	ETextureGridFirst, // non-texture
	EGridInboxTexture = ETextureGridFirst,
    EGridDirectoryTexture,
    EGridSettingsTexture,
    EGridAddNewBoxTexture,
    EGridHelpTexture,
    EGridAboutTexture,
    EGridBackground, 
    
    // FLAG ICONS
    ETextureFlagFirst, // non-texture
	EFollowUpFlag = ETextureFlagFirst,
	EFollowUpFlagComplete,
	EFollowUpFlagList,
	EFollowUpFlagCompleteList,
	
    // FOLDER LIST ICONS
    ETextureFolderFirst, // non-texture
    EFolderListMoreFoldersTexture = ETextureFolderFirst,
    EFolderListInboxTexture,
    EFolderListInboxSubfoldersTexture,
    EFolderListOutboxTexture,
    EFolderListDraftsTexture,
    EFolderListSentTexture,
    EFolderListDeletedItemsTexture,
    EFolderListServerFoldersTexture,
    EFolderListEmailAccountTexture,
    EFolderListMoreFoldersTexturePopup,
    EFolderListInboxTexturePopup,
    EFolderListInboxSubfoldersTexturePopup,
    EFolderListOutboxTexturePopup,
    EFolderListDraftsTexturePopup,
    EFolderListSentTexturePopup,
    EFolderListDeletedItemsTexturePopup,
    EFolderListServerFoldersTexturePopup,
    EFolderListEmailAccountTexturePopup,
    // SORT LIST ICONS
    ESortListAttachmentAscTexture,
    ESortListAttachmentDescTexture,
    ESortListDateAscTexture,
    ESortListDateDescTexture,
    ESortListFollowAscTexture,
    ESortListFollowDescTexture,
    ESortListPriorityAscTexture,
    ESortListPriorityDescTexture,
    ESortListSenderAscTexture,
    ESortListSenderDescTexture,
    ESortListSubjectAscTexture,
    ESortListSubjectDescTexture,
    ESortListUnreadAscTexture,
    ESortListUnreadDescTexture,

	// STATUS INDICATOR ICONS
	ETextureStatusFirst, // non-texture
	EStatusTextureSynchronising = ETextureStatusFirst,
	EStatusTextureConnected,
	EStatusTextureConnecting,
	EStatusTextureDisconnectedGeneral,
	EStatusTextureDisconnectedError,
	EStatusTextureDisconnectedLowBattery,
	EStatusTextureMailSent,

	// VIEWER TEXTURES
	ETextureViewerFirst, // non-texture
	EViewerTextureHeaderBackGround = ETextureViewerFirst,
	EViewerHighlightTexture,

	// COMPOSER TEXTURES
	ETextureComposerFirst, // no-texture
	EComposerTexturePriorityLow = ETextureComposerFirst,
	EComposerTexturePriorityHigh,
	
	// ATTACHMENT ICONS
	ETextureAttachmentsFirst, // non-texture
	EAttachmentsDocFile = ETextureAttachmentsFirst,
	EAttachmentsHtmlFile,
	EAttachmentsImageFile,
	EAttachmentsPdfFile,
	EAttachmentsPptFile,
	EAttachmentsRtfFile,
	EAttachmentsUnknownFile,
	EAttachmentsXls,
	EAttachmentsNotDownloaded,
	EAttachmentsCancelDownload,
	EAttachmentsDownload,
	EAttachmentsSaveAll,
	EAttachmentsCancelAll,
	EAttachmentsViewAll,

	// TEXTURE COUNT
	ETextureCount,
	
	// Branded mailbox icons. Max 101 icons 3 sizes of each
	// (Launcher grid, Inbox view and Folders dropdown view)
	EBrandedIconStart = 8000,
	EBrandedIconEnd = 8303,
	
	// GRID ECOM PLUGIN ICONS, Dynamically updated, 
	// do not add icons after this
	EGridPluginIconFirst 
	};

class CFreestyleEmailUiTextureManager: 
	public CAlfControl, 
	public MAlfBitmapProvider,
	public MFsDelayedLoadingInterface
    {
// to prevent creating multiple instances NewL has been hidden
// and AppUi has been declared as friend class
friend class CFreestyleEmailUiAppUi;

public:
	// Get correct backgroung image for current resolution
	CAlfTexture& BackgroundForThisResolutionL();
	// Get flush background image for the current resolution
	CAlfTexture& WatermarkBackgroundForThisResolutionL();	
	// Get any texture by ID
	CAlfTexture& TextureByIndex( TFSEmailUiTextures aIndex );
	// Get new skinned selector brush object for gen ui list.
	// Ownership is NOT transferred to caller.
	CAlfFrameBrush* ListSelectorBrushL();
	// Get new skinned grid selector brush object
	// Ownership is NOT transferred to caller.
    CAlfFrameBrush* GridSelectorBrushL();
	// Get new skinned selector brush object for gen ui control bar.
	// Ownership IS transferred to caller.
	CAlfFrameBrush* NewCtrlBarSelectorBrushLC();
	// <cmail>
    // Get new skinned selector brush object for gen ui popup list.
    // Ownership IS transferred to caller.	
	CAlfFrameBrush* NewListSelectorBrushL();
	// </cmail>
	// Get new scrollbar top background object. 
	// Ownership IS transferred to caller.
	CAlfFrameBrush* NewScrollBarTopBgBrushLC();
	// Get new scrollbar bottom background object. 
    // Ownership IS transferred to caller.
	CAlfFrameBrush* NewScrollBarBottomBgBrushLC();
	// Get new scrollbar center background object. 
    // Ownership IS transferred to caller.
	CAlfFrameBrush* NewScrollBarCenterBgBrushLC();
	// Get new scrollbar thumb object. 
    // Ownership IS transferred to caller.
	CAlfFrameBrush* NewScrollBarThumbBrushLC();
	// Get new mail list separator brush	
	// Ownership IS transferred to caller.
	CAlfFrameBrush* NewMailListSeparatorBgBrushLC();
	// Get new mail list separator brush	
    // Ownership IS transferred to caller.
	CAlfImageBrush* NewControlBarListBgBrushLC();
	// Get new controlbar button bg brush
    // Ownership IS transferred to caller.
	CAlfImageBrush* NewControlBarButtonBgBrushLC();

	// <cmail>
	// Ownerhip not transfered.
	CAlfFrameBrush* TitleDividerBgBrushL();
	// </cmail>

	// Create branded mailbox icon into TextureManager
	void CreateBrandedMailboxTexture( CGulIcon* aIcon, const TUid aPluginId, const TUint aMailboxId, const TSize& aSize );
	// Get branded mailbox icon
	CAlfTexture& TextureByMailboxIdL( const TUid aPluginId, const TUint aMailboxId, const TSize& aSize );
    
public: // from MAlfBitmapProvider

    void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap);
    
public: // from MFsDelayedLoadingInterface

    /**
    * DelayLoadCallback
    * Delayed loading callback
    * @return ETrue to keep calling, EFalse to stop callbacks
    */
	TInt DelayLoadCallback();
	
private: 
    // This class should not be used directly, use iAppUi->TextureManger() instance
    static CFreestyleEmailUiTextureManager* NewL( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi );
    virtual ~CFreestyleEmailUiTextureManager();
    
    CFreestyleEmailUiTextureManager( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi );
    void ConstructL(CAlfEnv& aEnv);
    void CreateTexturesL(); 
    
    void InitTextureArray();
    void LoadTextureL( TFSEmailUiTextures aTextureId ); // CHECKLATER - is this ok? see migration 3.12.1
	// <cmail> S60 Skin support
	void GetBackgroundBitmapL (CFbsBitmap*& bitmap) const; 
	void DrawContextL (CFbsBitmap*& bitmap, TAknsItemID id, TSize rsize) const;
	// </cmail>

private:
	// ALF environment - CHECKLATER - added this myself, but should be alright
    CAlfEnv* iEnv;

	CFreestyleEmailUiAppUi* iAppUi;
	CAlfFrameBrush* iFrameBrush;
	//<cmail>
	CAlfFrameBrush* iNewFrameBrush;
	CAlfFrameBrush* iTitleDividerBgBrush;
	//</cmail>
	
    /**
    * array for holding texture objects
    */
    RPointerArray<CAlfTexture> iTextures;

    /**
    * array for holding branded mailbox icons
    */
    RPointerArray<CAlfTexture> iBrandedMailboxIcons;
    RArray<TUid> iPluginId;
    RArray<TUint> iMailboxId;
    RArray<TSize> iSize;
    /*
     * Temporary variables for holding the mailbox icon and id
     * before create.
     */
    CFbsBitmap*   iTmpBitmap;
    CFbsBitmap*   iTmpMaskBitmap;
    TUid          iTmpPluginId;
    TUint         iTmpMailboxId;
    TSize         iTmpSize;

    /**
    * index indicating delay loading progress within
    * textures array
    */
    TInt iDelayLoadIndex;
    
	};

#endif // __FREESTYLEEMAILUI_LISTCONTROL_H__

