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
* Description:  FreestyleEmailUi folder list view implementation
*
*/

// SYSTEM INCLUDES
#include "emailtrace.h"
#include <AknUtils.h>
#include <StringLoader.h>
#include <AknBidiTextUtils.h>
#include <gulicon.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknStatuspaneUtils.h>
#include <akntoolbar.h>
#include <eikcolib.h>
#include <alf/alfdecklayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfbrusharray.h>
#include <alf/alfshadowborderbrush.h>
#include <alf/alfevent.h>
#include <alf/alfframebrush.h>
#include <alf/alfstatic.h>

// GENERIC LIST COMPONENT INCLUDES
#include "fstreelist.h"
#include "fstreevisualizerbase.h"
#include "fstreeplainonelineitemdata.h"
#include "fstreeplainonelineitemvisualizer.h"
#include "fstreeplaintwolineitemdata.h"
#include "fstreeplaintwolineitemvisualizer.h"
#include "fstreeplainonelinenodedata.h"
#include "fstreeplainonelinenodevisualizer.h"
#include "fscontrolbuttoninterface.h"
#include "fsseparatordata.h"
#include "fsseparatorvisualizer.h"

// FREESTYLE EMAIL FRAMEWORK INCLUDES
#include "CFSMailCommon.h"
#include "CFSMailClient.h"
#include "CFSMailBox.h"
#include "CFSMailFolder.h"
#include <csxhelp/cmail.hlp.hrh>
#include <featmgr.h>

// INTERNAL INCLUDES
#include <freestyleemailui.mbg>
#include <FreestyleEmailUi.rsg>

#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiFolderListControl.h"
#include "FreestyleEmailUiFolderListModel.h"
#include "FreestyleEmailUiFolderListVisualiser.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiControlBarCallback.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLayoutData.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "ESMailSettingsPlugin.h"
#include "FreestyleEmailUiStatusIndicator.h"

// Defines the order of the folders in UI
enum TFSEmailUiFolderListPriorities
	{
    EFolderListPriorityInbox = 1,
    EFolderListPriorityDrafts,
    EFolderListPrioritySent,
    EFolderListPriorityOutbox,
    EFolderListPriorityDeleted,
    EFolderListPriorityOtherFolder
	};

// Defines the order of the list icons in the icon array
enum TFsEmailUiFolderListIcons
	{
    EFolderListIconInbox = 0,
    EFolderListIconInboxSubfolders,
    EFolderListIconOutbox,
    EFolderListIconDrafts,
    EFolderListIconSent,
    EFolderListIconDeleted,
    EFolderListIconServerFolders,
	EFolderListIconMoreFolders,
    EFolderListIconEmailAccount
	};

// Defines the order of the sort list icons in the sort icon array
enum TFsEmailUiSortListIcons
	{
	ESortListAttachmentAscIcon = 0,
	ESortListAttachmentDescIcon,
	ESortListDateAscIcon,
	ESortListDateDescIcon,
	ESortListFollowAscIcon,
	ESortListFollowDescIcon,
	ESortListPriorityAscIcon,
	ESortListPriorityDescIcon,
	ESortListSenderAscIcon,
	ESortListSenderDescIcon,
	ESortListSubjectAscIcon,
	ESortListSubjectDescIcon,
	ESortListUnreadAscIcon,
	ESortListUnreadDescIcon
	};

// List level for the root folders
static const TInt KListRootLevel( 1 );
// List supports three levels, so basically root folder + two levels of subfolders
static const TInt KListLastBasicLevel( KListRootLevel + 2 );

const TInt KDefaultShadowBorderWidth( 15 );
const TReal KShadowBorderOpacity( 0.08 );

// Constants for drawing header text
const TReal KFSHeaderTextBackgroundOpacity = 0.3f;
const TAlfTimedValue KFSVisible( 1 );
const TAlfTimedValue KFSInvisible( 0 );
// Considering that there are 99999 unread emails + ( + ) + SPACE size should be 5+1+1+1 =8
static const TInt KFmtUnRdCntMaxLength( 8 );
_LIT(KFormatUnreadCnt, " (%d)");

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListVisualiser* CFSEmailUiFolderListVisualiser::NewL( CAlfEnv& aEnv,
                                                 CAlfControlGroup& aControlGroup,
                                                 CFreestyleEmailUiAppUi& aAppUi )
    {
    FUNC_LOG;
    CFSEmailUiFolderListVisualiser* self = CFSEmailUiFolderListVisualiser::NewLC(aEnv, aControlGroup, aAppUi );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListVisualiser* CFSEmailUiFolderListVisualiser::NewLC( CAlfEnv& aEnv,
                                                 CAlfControlGroup& aControlGroup,
                                                 CFreestyleEmailUiAppUi& aAppUi )
	{
    FUNC_LOG;
    CFSEmailUiFolderListVisualiser* self = new (ELeave) CFSEmailUiFolderListVisualiser( aEnv, aAppUi, aControlGroup );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListVisualiser::CFSEmailUiFolderListVisualiser( CAlfEnv& aEnv,
                                                                CFreestyleEmailUiAppUi& aAppUi,
                                                                CAlfControlGroup& aControlGroup )
    : CFsEmailUiViewBase( aControlGroup, aAppUi ),
      iEnv( aEnv ),
      iFullScreen( ETrue ),
      iPreviousListLevel( KListRootLevel ),
      iMoveOrCopyInitiated(EFalse),
      iWaitingToGoOnline(EFalse),
      iNoConnectNeeded(ETrue)
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListVisualiser::~CFSEmailUiFolderListVisualiser()
	{
    FUNC_LOG;
	delete iModel;
	iModel = NULL;

	delete iBackgroundBrush;
	iBackgroundBrush = NULL;

	delete iShadowBrush;
	
	// No need for ResetAndDestroy because list items are owned by generic list
	iListItemVisulizers.Close();

    //delete iNotifierTest;

    delete iTreeList;
    iTreeList = NULL;

    iIconArray.Close();
    iSortIconArray.Close();
    
    if ( iAsyncCallback )
        {
        iAsyncCallback->Cancel();
        delete iAsyncCallback;
        }

    iListItemDatas.Close();
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ConstructL()
	{
    FUNC_LOG;

    BaseConstructL( R_FSEMAILUI_FOLDER_LIST_VIEW );
    iFirstStartCompleted = EFalse;
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::DoFirstStartL
// Purpose of this function is to do first start things only when list is
// really needed to be shown. Implemented to make app startuo faster.
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::DoFirstStartL()
    {
    FUNC_LOG;
    iControl = CFSEmailUiFolderListControl::NewL( iEnv, *this );
    iModel = new (ELeave) CFSEmailUiFolderListModel();

    UpdateListSizeAttributes();
    //LoadIconsL();

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    iFaderLayout = CAlfDeckLayout::AddNewL( *iControl );
    iFaderLayout->SetRect( mainPaneRect );

    iParentLayout = CAlfAnchorLayout::AddNewL( *iControl );
    iParentLayout->SetRect( iScreenRect );
    iParentLayout->SetFlags( EAlfVisualFlagManualLayout );

    iParentLayout->EnableBrushesL();
    SetAnchors();

    iFaderVisual = CAlfImageVisual::AddNewL( *iControl, iParentLayout );

    iHeaderLayout = CAlfDeckLayout::AddNewL( *iControl, iParentLayout );
    iHeaderLayout->SetFlags( EAlfVisualFlagAutomaticLocaleMirroringEnabled );

    iHeaderTextVisual = CAlfTextVisual::AddNewL( *iControl, iHeaderLayout );
    iHeaderTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
    iHeaderTextVisual->SetTextL( KNullDesC );

    TRgb headerTextColor( KRgbBlack );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                               headerTextColor, KAknsIIDFsTextColors, EAknsCIFsTextColorsCG10 );
    iHeaderTextVisual->SetColor( headerTextColor );
    iHeaderTextVisual->SetTextStyle( iAppUi.LayoutHandler()->
        FSTextStyleFromLayoutL( AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_t1( 0 ) ).Id() );

    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iHeaderTextVisual->SetAlign( EAlfAlignHRight, EAlfAlignVCenter );
        }
    else
        {
        iHeaderTextVisual->SetAlign( EAlfAlignHLeft, EAlfAlignVCenter );
        }

	iHeaderTextVisual->SetOpacity( KFSInvisible );
	iHeaderLayout->SetOpacity( KFSInvisible );

    iListLayout = CAlfDeckLayout::AddNewL( *iControl, iParentLayout );
    iListLayout->SetFlags(EAlfVisualFlagLayoutUpdateNotification|EAlfVisualFlagAutomaticLocaleMirroringEnabled);

    iTreeVisualizer = CFsTreeVisualizerBase::NewL(iControl, *iListLayout);
    iTreeVisualizer->SetItemExpansionDelay( iAppUi.LayoutHandler()->ListItemExpansionDelay() );
    iTreeVisualizer->SetScrollTime( iAppUi.LayoutHandler()->ListScrollingTime() );
    iTreeVisualizer->SetFadeInEffectTime( iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime() );
    iTreeVisualizer->SetFadeOutEffectTime( iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime() );
    iTreeVisualizer->SetItemSeparatorSize( TSize(0, 0) );
    iTreeVisualizer->SetExpandCollapseAllOnLongTap( EFalse );

    iTreeList = CFsTreeList::NewL( *iTreeVisualizer, iEnv );

    // Compared to S60 3.2.3 in S60 5.0 Alf offers the key events in
    // opposite order.
    ControlGroup().AppendL( iControl );
    ControlGroup().AppendL( iTreeList->TreeControl() );

    iTreeList->AddObserverL(*this);

    iTreeList->SetLoopingType( EFsTreeListLoopingJumpToFirstLast );
    iTreeList->SetScrollbarVisibilityL( EFsScrollbarAuto );
    iAppUi.LayoutHandler()->SetListMarqueeBehaviour( iTreeList );
    iTreeList->HideListL();

    // Set page up and page down keys
    iTreeVisualizer->AddCustomPageUpKey( EStdKeyPageUp );
    iTreeVisualizer->AddCustomPageDownKey( EStdKeyPageDown );
    iTreeVisualizer->SetIndentationL(
            iAppUi.LayoutHandler()->FolderListIndentation(iScreenRect) );

    iTreeVisualizer->SetSelectorPropertiesL(
            iAppUi.FsTextureManager()->NewListSelectorBrushL(), 1.0,
            CFsTreeVisualizerBase::EFsSelectorMoveSmoothly );

    iAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );

    //iNotifierTest = CSimpleAOTest::NewL();
    iFirstStartCompleted = ETrue;
    }


// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::Id
// ---------------------------------------------------------------------------
//
TUid CFSEmailUiFolderListVisualiser::Id() const
	{
    FUNC_LOG;
	return FolderListId;
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::PrepareFolderListL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::PrepareFolderListL()
    {
    FUNC_LOG;
    iTreeList->RemoveAllL();
    iListItemVisulizers.Reset();
    iModel->RemoveAll();
    iMoreFoldersItemId = KFsTreeNoneID;

    // SetRect need to be called twice for the parent layout (before and after
    // list construction), otherwise the layout is incorrect in some cases.
    UpdateListSizeAttributes();
    iParentLayout->SetRect( iScreenRect );
    SetAnchors();
    ResizeListIcons();

    // Show empty screen when offline
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();

    if( iModel->Count() == 0 )
        {
        cba->SetCommandSetL( R_FREESTYLE_EMAUIL_UI_SK_OPTIONS_BACK );
        }

    if( iMoveOrCopyInitiated )
        {
        iTreeVisualizer->SetEmptyListTextL( KNullDesC );
        iTreeVisualizer->RefreshListViewL();
        }
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::PopulateFolderListL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::PopulateFolderListL()
    {
    FUNC_LOG;
    if ( !iFullScreen && iSortListCallback )
        {
        UpdateSortListL();
        SetFocusToCurrentSortItemL();
        }
  	else
        {
        UpdateFolderListL();
        if( !iFullScreen )
            {
            SetFocusToCurrentFolderL();
            }
        }

    if( !iFullScreen )
        {
        AdaptScreenRectToListContent();
        }

  	iParentLayout->SetRect(iScreenRect);

    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();

  	if( iModel->Count() > 0 )
  	    {
        cba->SetCommandSetL( R_FREESTYLE_EMAUIL_UI_SK_OPTIONS_BACK__SELECT );
  	    }

    if( iMoveOrCopyInitiated )
        {
        HBufC* emptyListText = StringLoader::LoadLC(
               R_FREESTYLE_EMAIL_UI_FOLDER_LIST_NO_FOLDERS_AVAILABLE );
        iTreeVisualizer->SetEmptyListTextL( *emptyListText );
        CleanupStack::PopAndDestroy( emptyListText );

        iTreeVisualizer->RefreshListViewL();
        }
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::PopulateFolderListDeferred
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::PopulateFolderListDeferred()
    {
    FUNC_LOG;
    if ( iAsyncCallback )
        {
        iAsyncCallback->Cancel();
        iAsyncCallback->Set( TCallBack( DoPopulateFolderList, this ) );
        iAsyncCallback->CallBack();
        }
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::DoPopulateFolderList
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiFolderListVisualiser::DoPopulateFolderList( TAny* aSelfPtr )
    {
    FUNC_LOG;
    CFSEmailUiFolderListVisualiser* self =
        static_cast< CFSEmailUiFolderListVisualiser* >( aSelfPtr );
    TRAPD( err,
        self->PopulateFolderListL();
        if ( self->iCustomMessageId == KFolderListMoveMessage ||
             self->iCustomMessageId == KFolderListMoveMessages ||
             self->iCustomMessageId == KFolderListCopyMessage ||
             self->iCustomMessageId == KFolderListCopyMessages )
            {
            self->SetFocusToLatestMovedFolderL();
            }
        self->iTreeList->SetFocusedL( ETrue );
        );
    return err;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::IsPopupShown
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::IsPopupShown()
    {
    return iPopupListShown;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::HidePopupL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::HidePopupL()
	{
    FUNC_LOG;

    iParentLayout->SetRect( iCtrlButtonRect, iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime() );

    TAlfTimedValue bgOpacity;

	// Hiding delay was removed to get rid of flickering if changing between
	// folderlist / sortlist popups. If delay is used in SetTarget,
	// folderlistview would be quickly drawn as fullscreen after HideListL
	// call, effecting ugly flickering
    bgOpacity.SetTarget( 0, 0 );
    iParentLayout->SetOpacity( bgOpacity );
	iTreeList->HideListL();
	iTreeList->DisableKineticScrolling( EFalse );
	ClearPopupSoftkeys();
	iPopupListShown = EFalse;
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::SetPopupSoftkeysL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::SetPopupSoftkeysL()
	{
    FUNC_LOG;
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->AddCommandSetToStackL( R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT);
    cba->UpdateCommandObserverL( cba->PositionById( EAknSoftkeySelect ), *this );
    cba->UpdateCommandObserverL( cba->PositionById( EEikBidCancel ), *this );
    cba->DrawNow();
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::ClearPopupSoftkeys
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ClearPopupSoftkeys()
	{
    FUNC_LOG;
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();

    if (cba->UpdatedCommandObserverExists(CEikButtonGroupContainer::ELeftSoftkeyPosition))
        {
        cba->RemoveCommandObserver( cba->PositionById( EAknSoftkeySelect ) );
        }
    if (cba->UpdatedCommandObserverExists(CEikButtonGroupContainer::ERightSoftkeyPosition))
        {
        cba->RemoveCommandObserver( cba->PositionById( EEikBidCancel ) );
        }
    TInt pos = -1;
    if ((pos = cba->PositionById( EAknSoftkeySelect )) >= 0)
        {
        cba->RemoveCommandFromStack( pos, EAknSoftkeySelect );
        }
    if ((pos = cba->PositionById( EEikBidCancel )) >= 0)
        {
        cba->RemoveCommandFromStack( pos, EEikBidCancel );
        }
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::DoShowInPopupL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::DoShowInPopupL(
        MFsControlButtonInterface* aButton,
        MFSEmailUiFolderListCallback* aFolderCallback,
        MFSEmailUiSortListCallback* aSortCallback )
	{
    FUNC_LOG;

    if ( ( aFolderCallback == NULL && aSortCallback == NULL ) ||
            aButton == NULL )
        {
        return;
        }

    // In case of popup we are not moving or copying messages,
    // so there's no need to connect the mailbox
    iNoConnectNeeded = ETrue;

    iCallback = aFolderCallback;
    iSortListCallback = aSortCallback;
    const TPoint buttonPos( aButton->Pos().Target() );
    const TSize buttonSize( aButton->Size().Target().AsSize() );

    // TRect from where to start the transformation effect
    TRect cbr = iAppUi.LayoutHandler()->GetControlBarRect();
    iCtrlButtonRect.SetRect( buttonPos + cbr.iTl, buttonSize );

    iFullScreen = EFalse;

    LoadIconsL();

    if ( !iFirstStartCompleted )
        {
        DoFirstStartL();
        }

	// Make parent layout invisible to avoid any flashes in display
	// during the construction of the list. Hide header also, not used.
	TAlfTimedValue opacity;
	opacity.SetValueNow( 0 );
	iHeaderLayout->SetOpacity( opacity );
    iParentLayout->SetOpacity( opacity );

	if( iListBgImageVisual )
		{
		iListBgImageVisual->SetOpacity( opacity );
		}

	PrepareFolderListL();
    PopulateFolderListL();

    const TInt paddingValue(
            iAppUi.LayoutHandler()->DropDownMenuListPadding() );
    const TAlfMetric paddingMetric( paddingValue );
    iParentLayout->SetPadding( paddingMetric );

	// Set list background
	if ( !iBackgroundBrush )
		{
		iBackgroundBrush =
            CAlfFrameBrush::NewL( iEnv, KAknsIIDQsnFrPopup, 1, 1 );
        TRect outerRect( iScreenRect );
        outerRect.Shrink( paddingValue, paddingValue );
        iBackgroundBrush->SetFrameRectsL( outerRect, iScreenRect );
		}

	   // Append brush if it's not yet appended. Currently this is our only
	    // brush so if brush count is more than zero, this brush is already
	    // appended. If other brush(es) are added later, this implementation
	    // need to be changed.
	    if ( iParentLayout->Brushes()->Count() == 0 )
	        {
	        // Keep the ownership of the brush to avoid unneeded object
	        // deletion / reconstruction
	        iParentLayout->Brushes()->AppendL( iBackgroundBrush, EAlfDoesNotHaveOwnership );
	        }
	
	if( !iShadowBrush )
	    {
        iShadowBrush = CAlfShadowBorderBrush::NewL( 
            iEnv, TAlfMetric( KDefaultShadowBorderWidth ) );
        iShadowBrush->SetOpacity( KShadowBorderOpacity );
        iShadowBrush->SetLayer( EAlfBrushLayerBackground );
        iParentLayout->Brushes()->AppendL( iShadowBrush,
                                           EAlfDoesNotHaveOwnership );
	    }

	SetPopupSoftkeysL();
	// SetRect need to be called also here, otherwise the list layout might
	// be wrong in mirrored layout
    iParentLayout->SetRect( iCtrlButtonRect );

    opacity.SetValueNow( 1 );
    //opacity.SetTarget( 1, iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime() );
    iParentLayout->SetOpacity( opacity );
    iParentLayout->SetRect( iScreenRect, iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime() );
    iParentLayout->UpdateChildrenLayout();
    iTreeList->DisableKineticScrolling( ETrue );
    iTreeList->ShowListL( ETrue );
    iTreeList->SetFocusedL( ETrue );
    iFocusVisible = iAppUi.IsFocusShown();
    iTreeVisualizer->SetFocusVisibility( iFocusVisible );
    iPopupListShown = ETrue;
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::ShowInPopupL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ShowInPopupL(
    const TFSMailMsgId aFolderId,
	MFSEmailUiFolderListCallback* aCallback,
	MFsControlButtonInterface* aButton )
	{
    FUNC_LOG;
    iCustomMessageId = KFolderListSelectFolder;
    iCurrentFolderId = aFolderId;
    DoShowInPopupL( aButton, aCallback, NULL );
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::ShowSortListPopupL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ShowSortListPopupL(
    const TFSMailSortCriteria aCurrentSortCriteria,
	const TFSFolderType aFolderType,
	MFSEmailUiSortListCallback* aCallback,
	MFsControlButtonInterface* aButton )
	{
    FUNC_LOG;
    iCurrentSortCriteria = aCurrentSortCriteria;
    iCurrentFolderType = aFolderType;
    DoShowInPopupL( aButton, NULL, aCallback );
	}

// ---------------------------------------------------------------------------
// Activate this view
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ChildDoActivateL(const TVwsViewId& /*aPrevViewId*/,
	                 TUid aCustomMessageId,
	                 const TDesC8& aCustomMessage)
	{
    FUNC_LOG;
    iFullScreen = ETrue;

    LoadIconsL();

    if ( !iFirstStartCompleted )
	    {
	    DoFirstStartL();
	    }

	// Set title pane text
	iAppUi.SetActiveMailboxNameToStatusPaneL();

    // Different effect is used when opening folder selection list
    iAnimateNextActivation = ( aCustomMessageId == KFolderListSelectFolder );

    // Create header image visual if not yet created
    if( !iHeaderImageVisual )
        {
        iHeaderImageVisual = CAlfImageVisual::AddNewL( *iControl, iHeaderLayout );
        CAlfTexture& headerBgTexture = iAppUi.FsTextureManager()->TextureByIndex( EMailListBarBgIcon );
        headerBgTexture.Size().SetSize( iScreenRect.Width(), iListHeaderHeight );
        iHeaderImageVisual->SetImage( TAlfImage( headerBgTexture ) );

        TAlfTimedValue headerImageOpacity;
        headerImageOpacity.SetValueNow( 0 );
        iHeaderImageVisual->SetOpacity( headerImageOpacity );

        // Make header text visible by moving it on front of the header background image
        iHeaderTextVisual->MoveToFront();
        }

    // Create list background image if not yet created
    if( !iListBgImageVisual )
        {
        iListBgImageVisual = CAlfImageVisual::AddNewL( *iControl, iListLayout );

        TAlfTimedValue bgImageVisualOpacity;
        bgImageVisualOpacity.SetValueNow( 0 );
        iListBgImageVisual->SetOpacity( bgImageVisualOpacity );
        // Move to background to make the list visible
        iListLayout->MoveVisualToBack( *iListBgImageVisual );
        }

    // Returning from the settings view needs to handled as special case where
    // the previous state of the folder list is preserved.
    if ( aCustomMessageId.iUid == KMailSettingsReturnFromPluginSettings )
        {
        iTreeList->HideListL();

        // Then just refresh the list as in dynamic variant switch,
        // because screen orientation might have changed
        HandleDynamicVariantSwitchL( EScreenLayoutChanged );

        // Set flag for moving or copying
        iMoveOrCopyInitiated = (iCustomMessageId == KFolderListMoveMessage ||
                                iCustomMessageId == KFolderListMoveMessages ||
                                iCustomMessageId == KFolderListCopyMessage ||
                                iCustomMessageId == KFolderListCopyMessages);
        // Update toolbar button statuses.
        UpdateToolbarButtons();
        }

    // Normal case of entering the folder list. Set the header text and callback function
    // according the view activation parameters.
    else
        {
        iCustomMessageId = aCustomMessageId;
        iTitleCaptionVisible = ETrue;

        // No header in folders list if not moving or copying messages
        if( aCustomMessageId == KFolderListMoveMessage )
            {
            SetHeaderTextByResourceIdL( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_MOVE_ONE_HEADING );
            }
        else if( aCustomMessageId == KFolderListMoveMessages )
            {
            SetHeaderTextByResourceIdL( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_MOVE_MORE_HEADING );
            }
        else if( aCustomMessageId == KFolderListCopyMessage )
            {
            SetHeaderTextByResourceIdL( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_COPY_ONE_HEADING );
            }
        else if( aCustomMessageId == KFolderListCopyMessages )
            {
            SetHeaderTextByResourceIdL( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_COPY_MORE_HEADING );
            }
        else
            {
            // Hide text visual
            iTitleCaptionVisible = EFalse;
            iHeaderTextVisual->SetOpacity( KFSInvisible );
            }

        // Set flag for moving or copying
        iMoveOrCopyInitiated = (aCustomMessageId == KFolderListMoveMessage ||
                                aCustomMessageId == KFolderListMoveMessages ||
                                aCustomMessageId == KFolderListCopyMessage ||
                                aCustomMessageId == KFolderListCopyMessages);

        // Set callback function
        TFolderListActivationData subView;
        TPckgBuf<TFolderListActivationData> viewData( subView );
        viewData.Copy( aCustomMessage );
        iCallback = viewData().iCallback;
        iCurrentFolderType = viewData().iSourceFolderType;

       	iTreeList->HideListL();

    	// Clear padding when shown in full screen
       	const TAlfMetric padding( 0 );
    	iParentLayout->SetPadding( padding );
    	iListLayout->SetPadding( padding );

        PrepareFolderListL();

        // Remove need to be called for all brushes before Reset, otherwise the
        // brush resources are not completely freed from server side -> brushes
        // keep "hanging" there and make future drawing slower.
        while( iParentLayout->Brushes()->Count() > 0 )
            {
            iParentLayout->Brushes()->Remove( 0 );
            }
    	iParentLayout->Brushes()->Reset();

    	TAlfTimedValue layoutOpacity = iHeaderLayout->Opacity();
    	layoutOpacity.SetTarget( 1, iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime()/2 );
    	iHeaderLayout->SetOpacity( layoutOpacity );

    	TAlfTimedValue bgImageOpacity = iListBgImageVisual->Opacity();
    	bgImageOpacity.SetTarget( 1, iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime()/2 );
    	iListBgImageVisual->SetOpacity( bgImageOpacity );

    	// Reset the variables used in horizontal scrolling
    	iPreviousListRect = iListLayout->DisplayRectTarget();
        iPreviousListLevel = KListRootLevel;

        iParentLayout->SetRect( iScreenRect );
        iTreeList->ShowListL();

        TAlfTimedValue opacity;
        opacity.SetValueNow( 1 );
        iParentLayout->SetOpacity( opacity );
        }

    // See if we are already connected
    CFSMailBox* mb = iAppUi.GetActiveMailbox();
    // Just in case, shouldn't happen
    User::LeaveIfNull( mb );

    TFSMailBoxStatus onlineStatus = mb->GetMailBoxStatus();

    // If we are not moving or copying messages, there's no need to connect
    // the mailbox. In case of move/copy we need to check the mailbox
    // capability to see if we need to connect the mailbox before showing the
    // folder list.
    iNoConnectNeeded = ETrue;
    if( iMoveOrCopyInitiated )
        {
        iNoConnectNeeded = mb->HasCapability( EFSMBoxCapaMoveMessageNoConnectNeeded );
        }

    iWaitingToGoOnline = EFalse;
    // If we are already online or we don't need to be online for
    // this operation, then we can show the list immediately
    if( onlineStatus == EFSMailBoxOnline || iNoConnectNeeded )
        {
        if( aCustomMessageId.iUid != KMailSettingsReturnFromPluginSettings )
            {
            // Populate list after it has been made visible. List should not
            // be focused before it's fully populated to prevent jumping of the
            // highlight.
            iTreeList->SetFocusedL( EFalse );
            PopulateFolderListDeferred();
            }
        }
    else
        {
        // If we come here it means that the mailbox is offline but connection
        // is needed for this operation, so we need to connect first
        ConnectToMailboxL();
        }
    FocusVisibilityChange( iAppUi.IsFocusShown() );
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::OfferToolbarEventL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::OfferToolbarEventL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EFsEmailUiTbCmdSelect:
            HandleCommandL( EFsEmailUiCmdSelect );
            break;
        case EFsEmailUiTbCmdCollapseAll:
            HandleCommandL( EFsEmailUiCmdActionsCollapseAll );
            break;
        case EFsEmailUiTbCmdExpandAll:
            HandleCommandL( EFsEmailUiCmdActionsExpandAll );
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::ToolbarResourceId
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiFolderListVisualiser::ToolbarResourceId() const
    {
    return R_FREESTYLE_EMAIL_UI_TOOLBAR_FOLDER_LIST;
    }

// ---------------------------------------------------------------------------
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ChildDoDeactivate()
	{
    FUNC_LOG;
    iMoveOrCopyInitiated = EFalse;
    iWaitingToGoOnline = EFalse;
    if ( !iAppUi.AppUiExitOngoing() )
        {
        iTreeVisualizer->NotifyControlVisibilityChange( EFalse );
        }
	}

// ---------------------------------------------------------------------------
// In case of sort list, this function sets focus to current sort item
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::SetFocusToCurrentSortItemL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        for ( TInt i = 0; i < iModel->Count(); i++ )
        	{
        	CFSEmailUiFolderListItem* modelItem = iModel->ItemByIndex( i );
        	if ( modelItem && ( modelItem->iSortField == iCurrentSortCriteria.iField ) )
        		{
        		iTreeVisualizer->SetFocusedItemL( modelItem->iListId, EFalse );
        		break;
        		}
        	}
	
		// Bold the focused item's text
		TInt arraySize = iListItemVisulizers.Count();
		for( TInt i = 0; i < arraySize; i++ )
			{
			TFsTreeItemId id = iTreeList->FocusedItem();
			if ( iListItemVisulizers[ i ] == &( iTreeList->ItemVisualizer( id ) ) )
				{
				iListItemVisulizers[i]->SetTextBold( ETrue );
				break;
				}
			}
        }
    }

// ---------------------------------------------------------------------------
// In case of folder list popup, this function sets focus to current folder,
// if that folder can be found from the list. If current folder is not found
// from the list, then move focus to "More folders" item.
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::SetFocusToCurrentFolderL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        TFsTreeItemId itemToFocus = iMoreFoldersItemId;
        for ( TInt i=0; i < iModel->Count() ; i++ )
            {
            CFSEmailUiFolderListItem* modelItem = iModel->ItemByIndex( i );
            if ( modelItem && ( modelItem->iFolderId == iCurrentFolderId ) )
                {
                itemToFocus = modelItem->iListId;
                break;
                }
            }
        iTreeVisualizer->SetFocusedItemL( itemToFocus, EFalse );
        }
    }

// ---------------------------------------------------------------------------
// In case of move, this function sets focus to latest selected item
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::SetFocusToLatestMovedFolderL()
	{
    FUNC_LOG;
	if ( iFirstStartCompleted ) // Safety
	    {
	    TBool match( EFalse );
	    for ( TInt i=0; i < iModel->Count() ; i++ )
	         {
	         CFSEmailUiFolderListItem* modelItem = iModel->ItemByIndex( i );
	         if ( modelItem && // Safety check
	              !modelItem->iFolderId.IsNullId() && // Filter mailbox and sort items
	              modelItem->iFolderId == iLatestSelectedMoveFolderId )
	             {
	             match = ETrue;
	             iTreeVisualizer->SetFocusedItemL( modelItem->iListId );
	             break;
	             }
	         }

	     if ( match )
	         {
             // Make sure that the horizontal position is recalculated
             DoHorizontalScrollL( ETrue );
	         }
	     else
	         {
             iLatestSelectedMoveFolderId.SetId( 0 );
             iLatestSelectedMoveFolderId.SetNullId();
	         }
	    }
	}

// ---------------------------------------------------------------------------
// Handle events
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool result( EFalse );
    if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
        {
        // Swap left and right with mirrored layout
        TInt scanCode( aEvent.KeyEvent().iScanCode );
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            if( scanCode == EStdKeyLeftArrow )
                {
                scanCode = EStdKeyRightArrow;
                }
            else if( scanCode == EStdKeyRightArrow )
                {
                scanCode = EStdKeyLeftArrow;
                }
            }

        // Handle possible focus visibility change
        if( ( scanCode == EStdKeyRightArrow ) ||
            ( scanCode == EStdKeyLeftArrow ) ||
            ( scanCode == EStdKeyUpArrow ) ||
            ( scanCode == EStdKeyDownArrow ) ||
            ( scanCode == EStdKeyEnter ) ||
            ( scanCode == EStdKeyDeviceA ) ||
            ( scanCode == EStdKeyDevice3 ) )
            {
            // If the focus was not active already, ignore the key press
            if( !iAppUi.SetFocusVisibility( ETrue ) )
                {
                return ETrue;
                }
            }

        switch ( scanCode )
            {
            case EStdKeyDevice3:	// CENTER CLICK
            case EStdKeyEnter:		// ENTER
            	{
            	// Handle center click and enter as selection
            	HandleCommandL( EAknSoftkeySelect );
            	result = ETrue;
            	}
            	break;

            case EStdKeyLeftArrow:
            	{
            	// Check is the folder list open
            	if( !iFullScreen && iCallback )
            		{
            		HandleSelectionL( EFSEmailUiCtrlBarResponseCancel );
            		}
            	// Check is the sort list open
            	else if( !iFullScreen && iSortListCallback )
            		{
            		HandleSelectionL( EFSEmailUiCtrlBarResponseSwitchList );
            		}
 	           	}
                break;

	        case EStdKeyRightArrow:
            	{
            	// Check is the folder list open
            	if( !iFullScreen && iCallback )
            		{
            		HandleSelectionL( EFSEmailUiCtrlBarResponseSwitchList );
            		}
            	// Check is the sort list open
            	else if( !iFullScreen && iSortListCallback )
            		{
            		HandleSelectionL( EFSEmailUiCtrlBarResponseCancel );
            		}
            	}
				break;

             default:
	       	    // Check keyboard shortcuts.
	       	    TInt shortcutCommand =
	       	        iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
	       	                                                         CFSEmailUiShortcutBinding::EContextFolderList );
	       	    if ( shortcutCommand != KErrNotFound )
	       	        {
       	            HandleCommandL( shortcutCommand );
	       	        result = ETrue;
	       	        }
                break;
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::HandlePointerEventL
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::HandlePointerEventL(const TAlfEvent& aEvent)
    {
    TBool eventHandled( EFalse );

    if( ( aEvent.Visual() == iFaderVisual || aEvent.Visual() == iFaderLayout ) &&
        ( aEvent.PointerEvent().iType == TPointerEvent::EButton1Down  ) &&
        ( iPopupListShown ) )
        {
        iAppUi.SetFocusVisibility( EFalse );
        HandleSelectionL( EFSEmailUiCtrlBarResponseCancel );
        eventHandled = ETrue;
        }
    else
        {
        // Handle pointer events to fadervisual. If the list is shown in popup,
        // then the popup will be closed.
        if( ( aEvent.IsPointerEvent() ) &&
            ( aEvent.PointerEvent().iType == TPointerEvent::EButton1Up ) )
            {
            eventHandled = iTreeList->TreeControl()->OfferEventL(aEvent);
            // Hide focus visibility always after pointer up event.
            iAppUi.SetFocusVisibility( EFalse );
            }
        else 
        	{
        	eventHandled = iTreeList->TreeControl()->OfferEventL(aEvent);
        	}
        }
    return eventHandled;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        // Hide expand/collapse all if not needed
        if ( aResourceId == R_FSEMAILUI_FOLDERLIST_MENUPANE )
            {
            if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
        	   {
        	   // remove help support in pf5250
        	   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
        	   }

            // OFFLINE/ONLINE MENU SELECTION
            if( iMoveOrCopyInitiated )
                {
                TFSMailBoxStatus onlineStatus = iAppUi.GetActiveMailbox()->GetMailBoxStatus();
                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnline, onlineStatus == EFSMailBoxOnline );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOffline, onlineStatus == EFSMailBoxOffline );
                }
            else
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnline, ETrue );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOffline, ETrue );
                }

            // select option
            aMenuPane->SetItemDimmed( EFsEmailUiCmdSelect, iModel->Count() == 0 );

            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCollapseAll, AllNodesCollapsed() );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsExpandAll, AllNodesExpanded() );
            }

        // Add shortcut hints
        iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane,
                                    CFSEmailUiShortcutBinding::EContextFolderList );
        }
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::HandleSelectionL( TFSEmailUiCtrlBarResponse aSelection )
    {
    FUNC_LOG;
    // If popup is not open or the view is not active, then do nothing
    if ( !(iPopupListShown || IsViewActive()) )
        {
        return;
        }

    TBool popupHiddenAlready(EFalse);
    switch( aSelection )
        {
        case EFSEmailUiCtrlBarResponseSelect:
            {
            TFsTreeItemId focusedItem = iTreeList->FocusedItem();
            if (focusedItem != KFsTreeNoneID)
                {
                if ( !iFullScreen && iTreeList->ItemData(focusedItem).Type() != KFsSeparatorDataType)
                    {
                    HidePopupL();
                    popupHiddenAlready = ETrue;
                    }
                }
            if( !HandleCallbackL() )
                {
                break;
                }
            }
            // fall through
        case EFSEmailUiCtrlBarResponseSwitchList:
        case EFSEmailUiCtrlBarResponseCancel:
            {
            if ( IsFullScreen() )
                {
                iAppUi.ReturnToPreviousViewL();
                }
            else
                {
                if ( !popupHiddenAlready )
                    {
                    HidePopupL();
                    }

                iAppUi.FolderPopupClosed();
                }

            if ( !iFullScreen && iSortListCallback )
                {
                iSortListCallback->SortOrderChangedL( EFSMailDontCare, aSelection );
                iSortListCallback = NULL;
                }
            else if ( iCallback )
                {
                iCallback->FolderSelectedL( TFSMailMsgId(), aSelection );
                iCallback = NULL;
                }
            }
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Recursive function to get the root parent of given item
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiFolderListVisualiser::GetRootParent( const TFsTreeItemId aItemId ) const
	{
    FUNC_LOG;
	TFsTreeItemId parentId = iTreeList->Parent( aItemId );
	// If current item's parent is KFsTreeRootID, return its id
	if( parentId == KFsTreeRootID || parentId == KFsTreeNoneID )
		{
		return aItemId;
		}
	else
		{
		// Get the root parent recursively
		return GetRootParent( parentId );
		}
	}

// ---------------------------------------------------------------------------
// Override the default transition effect in case full screen mode is launched
// from popup mode.
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::DoTransitionEffect( TBool aDirectionOut )
    {
    FUNC_LOG;
    if ( !aDirectionOut && IsFullScreen() && iAnimateNextActivation )
        {
        TInt rectTransitionTime = iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime();
        TInt opacityTransitionTime = iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime() / 2;

        TAlfTimedValue opacity;
        opacity.SetValueNow( 0 );
        opacity.SetTarget( 1, opacityTransitionTime );
        iParentLayout->SetOpacity( opacity );

        // Do animated enlargening from popup window rect to full screen rect
        iParentLayout->SetRect( FolderListRectInThisResolution() );
        iParentLayout->SetRect( iScreenRect, rectTransitionTime );
        }
    else
        {
        // Use generic transition effect in all other cases
        CFsEmailUiViewBase::DoTransitionEffect( aDirectionOut );
        }
    }

// ---------------------------------------------------------------------------
// Gets the parent layout holding all the visials of this view
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::GetParentLayoutsL(
        RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    FUNC_LOG;
    aLayoutArray.AppendL( iParentLayout );
    }

// ---------------------------------------------------------------------------
// Handle commands
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
         {
         switch(aCommand)
              {
              // Key presses
              case EAknSoftkeySelect:
                  {
                  // If focus is not visible, make it visible
                  // otherwise fall through ( call HandleSelectionL )
                  if( !iFocusVisible )
                      {
                      iAppUi.SetFocusVisibility( ETrue );
                      break;
                      }
                  }
              case EAknSoftkeyOpen:
              case EFsEmailUiCmdOpen:
              case EFsEmailUiCmdSelect:
                  {
                  HandleSelectionL( EFSEmailUiCtrlBarResponseSelect );
                  }
                  break;

              case EAknSoftkeyBack:
              case EEikBidCancel:
                  {
                  HandleSelectionL( EFSEmailUiCtrlBarResponseCancel );
                  }
                  break;

              // Options menu commands
              case EFsEmailUiCmdGoOnline:
                  {
                  if( iMoveOrCopyInitiated )
                      {
      			      ConnectToMailboxL();
                      }
                  }
            	  break;
              case EFsEmailUiCmdGoOffline:
                  {
                  if( iMoveOrCopyInitiated )
                      {
                      DisconnectFromMailboxL();
                      }
                  }
                  break;
              case EFsEmailUiCmdActionsCollapseAll:
                  {
                  TFsTreeItemId prevId = iTreeList->FocusedItem();
                  prevId = GetRootParent( prevId );
                  iTreeVisualizer->CollapseAllL();
                  if ( prevId != KFsTreeRootID && prevId != KFsTreeNoneID )
                      {
                      iTreeVisualizer->SetFocusedItemL( prevId );
                      }
                  }
                  break;

              case EFsEmailUiCmdActionsExpandAll:
                  {
                  TFsTreeItemId prevId = iTreeList->FocusedItem();
                  iTreeVisualizer->ExpandAllL();
                  if ( prevId != KFsTreeRootID && prevId != KFsTreeNoneID )
                      {
                      iTreeVisualizer->SetFocusedItemL( prevId );
                      }
                  }
                  break;

              case EFsEmailUiCmdSettings:
                  {
                  CESMailSettingsPlugin::TSubViewActivationData activationData;
                  activationData.iAccount = iAppUi.GetActiveMailbox()->GetId();
                  activationData.iSubviewId = 1;
                  activationData.iLaunchFolderSettings = ETrue;

                  TUid pluginUid = iAppUi.GetActiveMailbox()->GetSettingsUid();
                  // register plugin view if not exists so that activation can be made
                  if ( !iAppUi.View( pluginUid ) )
                      {
                      CESMailSettingsPlugin* plugin = CESMailSettingsPlugin::NewL( pluginUid );
                      CleanupStack::PushL( plugin );
                      iAppUi.AddViewL( plugin );
                      CleanupStack::Pop( plugin );
                      }
                  TUid messageId = TUid::Uid( CESMailSettingsPlugin::EActivateMailSettingsSubview );
                  const TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> pluginMessagePkg( activationData );
                  iAppUi.EnterPluginSettingsViewL( pluginUid, messageId, pluginMessagePkg );

                  // Hide list to avoid it flashing when returning back
                  iTreeList->HideListL();
                  }
                  break;

              case EFsEmailUiCmdHelp:
                  {
                  TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
                  }
                  break;

              case EFsEmailUiCmdExit:
                  {
                  iAppUi.Exit();
                  }
                  break;

              case EFsEmailUiCmdGoToTop:
                  {
                  GoToTopL();
                  }
                  break;

              case EFsEmailUiCmdGoToBottom:
                  {
                  GoToBottomL();
                  }
                  break;

              case EFsEmailUiCmdPageUp:
                  {
                  TKeyEvent simEvent = { EKeyPageUp, EStdKeyPageUp, 0, 0 };
                  iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
                  }
                  break;

              case EFsEmailUiCmdPageDown:
                  {
                  TKeyEvent simEvent = { EKeyPageDown, EStdKeyPageDown, 0, 0 };
                  iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
                  }
                  break;

              case EFsEmailUiCmdActionsCollapseExpandAllToggle:
                  {
                  if ( !AllNodesCollapsed() )
                      {
                      HandleCommandL( EFsEmailUiCmdActionsCollapseAll );
                      }
                  else
                      {
                      HandleCommandL( EFsEmailUiCmdActionsExpandAll );
                      }
                  }
                  break;

              default:
                  break;
              }
         }
    }

// ---------------------------------------------------------------------------
// Handle call to callback function
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::HandleCallbackL()
	{
    FUNC_LOG;
    TFsTreeItemId focusedItem = iTreeList->FocusedItem();

    if( focusedItem == KFsTreeNoneID )
    	{
    	return ETrue;
    	}

    if( focusedItem == iMoreFoldersItemId )
    	{
        //HidePopupL();
        iFullScreen = ETrue;

		TFolderListActivationData tmp;
	    tmp.iCallback = iCallback;
		const TPckgBuf<TFolderListActivationData> pkgOut( tmp );

		iAppUi.EnterFsEmailViewL( FolderListId, KFolderListSelectFolder, pkgOut );

        return EFalse;
    	}

    CFSEmailUiFolderListItem* modelItem = iModel->ItemByListId( focusedItem );
	if( !modelItem ) // Safety check, shouldn't be possible
		{
		return EFalse;
		}

	if( iCallback )
		{
		const TFSMailMsgId& selectedFolderId = modelItem->iFolderId;
		const TFSMailMsgId& selectedMailboxId = modelItem->iMailboxId;
        if( !selectedFolderId.IsNullId() )
        	{
    		if ( iCustomMessageId == KFolderListMoveMessage ||
    			 iCustomMessageId == KFolderListMoveMessages ||
    		     iCustomMessageId == KFolderListCopyMessage ||
    		     iCustomMessageId == KFolderListCopyMessages )
    			{
    			iLatestSelectedMoveFolderId = selectedFolderId;
    			}
  	       	iCallback->FolderSelectedL( selectedFolderId, EFSEmailUiCtrlBarResponseSelect );
        	}
        else if( !selectedMailboxId.IsNullId() )
        	{
        	iCallback->MailboxSelectedL( selectedMailboxId );
        	}
        // Set callback to NULL after callback is called so that the
        // same callback is not reused unintentionally.
        iCallback = NULL;
		}
	else if( iSortListCallback )
		{
        iSortListCallback->SortOrderChangedL( modelItem->iSortField, EFSEmailUiCtrlBarResponseSelect );
        // Set callback to NULL after callback is called so that the
        // same callback is not reused unintentionally.
        iSortListCallback = NULL;
		}

    return ETrue;
	}


// ---------------------------------------------------------------------------
// Append node to list by getting the item data from resource
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::SetHeaderTextByResourceIdL(
        TInt aResourceId )
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        HBufC* headingText = StringLoader::LoadLC( aResourceId );
        SetHeaderTextAttributesL();
        iHeaderTextVisual->SetTextL( *headingText );
        CleanupStack::PopAndDestroy( headingText );
        }
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::SetHeaderTextAttributesL()
//
// Apply header text attributes
// This creates "Move message to:" title caption and background graphics
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::SetHeaderTextAttributesL()
    {
    iHeaderTextVisual->SetOpacity( KFSVisible );
    CAlfTextStyle* textStyle = iAppUi.LayoutHandler()->FSTextStyleFromIdL(
            EFSFontTypeNormalBold );
    iHeaderTextVisual->SetTextStyle ( textStyle->Id() );
    iHeaderTextVisual->SetColor( iAppUi.LayoutHandler()->
            ListNormalStateTextSkinColor() );
    iHeaderTextVisual->SetPadding(
            iAppUi.LayoutHandler()->TitleCaptionPadding() );

    if( iHeaderTextVisual->Brushes() )
        {
        iHeaderTextVisual->Brushes()->Reset();
        }
    iHeaderTextVisual->EnableBrushesL();
    CAlfBrush* selectorBrush = iAppUi.FsTextureManager()->
                   NewCtrlBarSelectorBrushLC();
    selectorBrush->SetOpacity( KFSHeaderTextBackgroundOpacity );
    iHeaderTextVisual->Brushes()->AppendL( selectorBrush, EAlfHasOwnership );
    CleanupStack::Pop( selectorBrush );
    }


// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::ConnectMailbox
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ConnectToMailboxL()
    {
	CFSMailBox* mb = iAppUi.GetActiveMailbox();
    iWaitingToGoOnline = ETrue;
    mb->GoOnlineL();
    }


// ---------------------------------------------------------------------------
// CFSEmailUiFolderListVisualiser::DisconnectMailbox
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::DisconnectFromMailboxL()
    {
    iAppUi.GetActiveMailbox()->GoOfflineL();
    iWaitingToGoOnline = EFalse;
    }


// ---------------------------------------------------------------------------
// Construct folder list data
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::UpdateFolderListL()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
		// <cmail>
        iListItemDatas.Reset();
		// </cmail>
        if( iFullScreen )
            {
            // Append lines to the model
            AppendActiveMailboxFoldersL();

            // Update toolbar button statuses.
            UpdateToolbarButtons();
            }
        else
            {
            //Mailboxes needed to be at the top followed by seperator
            AppendMailboxesL();

            // Append lines to the model followed by seperator
            AppendActiveMailboxFoldersL();
            AppendSeparatorLineL();

            // Append the folder list view selector
            AppendMoreFoldersL();
            }
        }
  	}

// ---------------------------------------------------------------------------
// Construct sort list data
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::UpdateSortListL()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        iListItemDatas.Reset();
        iTreeList->RemoveAllL();
        iListItemVisulizers.Reset();
        iModel->RemoveAll();
        iMoreFoldersItemId = KFsTreeNoneID;
        // Append lines to the model
        AppendSortListItemsL();
        }
	}

// ---------------------------------------------------------------------------
// Append active mailbox's folders
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::AppendActiveMailboxFoldersL()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        iActiveMailbox = iAppUi.GetActiveMailbox();
        if( iActiveMailbox )
            {
            AppendSubfoldersL( TFSMailMsgId(), KFsTreeRootID, ETrue );
            }
        }
	}

// ---------------------------------------------------------------------------
// Compare two mail folders, used in sorting done with TLinearOrder
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiFolderListVisualiser::CompareFolders( const CFSMailFolder& aFirst, const CFSMailFolder& aSecond )
	{
    FUNC_LOG;
	TInt priorityOfFirst = GetFolderPriorityByType( aFirst );
	TInt priorityOfSecond = GetFolderPriorityByType( aSecond );

	// First sort the folders by folder type priority.
	// If priority is same, sort alphabetically by folder name.
	if( priorityOfFirst != priorityOfSecond )
		{
		return priorityOfFirst - priorityOfSecond;
		}
	else
		{
		TDesC& nameOfFirst = aFirst.GetFolderName();
		TDesC& nameOfSecond = aSecond.GetFolderName();

		return nameOfFirst.CompareC( nameOfSecond );
		}
	}

// ---------------------------------------------------------------------------
// Compare two mail folders, used in sorting done with TLinearOrder
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiFolderListVisualiser::GetFolderPriorityByType( const CFSMailFolder& aFolder )
	{
    FUNC_LOG;
	TInt folderType = aFolder.GetFolderType();
	switch( folderType )
		{
		case EFSInbox:
			return EFolderListPriorityInbox;

		case EFSOutbox:
			return EFolderListPriorityOutbox;

		case EFSDraftsFolder:
			return EFolderListPriorityDrafts;

		case EFSSentFolder:
			return EFolderListPrioritySent;

		case EFSDeleted:
			return EFolderListPriorityDeleted;

		default:
			return EFolderListPriorityOtherFolder;
		}
	}

void CFSEmailUiFolderListVisualiser::AppendSortListItemsL()
	{
    FUNC_LOG;

    // Select correct icon depending on the selected sort criteria
    CAlfTexture* icon( NULL );
    icon = iCurrentSortCriteria.iField == EFSMailSortByDate ? 
    	   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
		   iSortIconArray[ ESortListDateDescIcon ] : 
		   iSortIconArray[ ESortListDateAscIcon ] : 
		   iSortIconArray[ ESortListDateDescIcon ];
		   
	TFsTreeItemId itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_DATE, KFsTreeRootID, icon, EFalse );
	iModel->AppendL( itemId, EFSMailSortByDate );

	switch( iCurrentFolderType )
		{
		case EFSSentFolder:
		case EFSDraftsFolder:
		case EFSOutbox:
			{
		    icon = iCurrentSortCriteria.iField == EFSMailSortByRecipient ? 
		    	   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
				   iSortIconArray[ ESortListSenderDescIcon ] : 
				   iSortIconArray[ ESortListSenderAscIcon ] : 
				   iSortIconArray[ ESortListSenderDescIcon ];
			itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_RECIPIENT, KFsTreeRootID, icon, EFalse );
			iModel->AppendL( itemId, EFSMailSortByRecipient );
			}
			break;

		default:
			{
		    icon = iCurrentSortCriteria.iField == EFSMailSortBySender ? 
		    	   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
				   iSortIconArray[ ESortListSenderDescIcon ] : 
				   iSortIconArray[ ESortListSenderAscIcon ] : 
				   iSortIconArray[ ESortListSenderDescIcon ];
			itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_SENDER, KFsTreeRootID, icon, EFalse );
			iModel->AppendL( itemId, EFSMailSortBySender );
			}
			break;
		}

	icon = iCurrentSortCriteria.iField == EFSMailSortBySubject ? 
		   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
		   iSortIconArray[ ESortListSubjectDescIcon ] : 
		   iSortIconArray[ ESortListSubjectAscIcon ] : 
		   iSortIconArray[ ESortListSubjectDescIcon ];
	
	itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_SUBJECT, KFsTreeRootID, icon, EFalse );
	iModel->AppendL( itemId, EFSMailSortBySubject );

	if ( TFsEmailUiUtility::IsFollowUpSupported( *iAppUi.GetActiveMailbox() ) )
		{
		icon = iCurrentSortCriteria.iField == EFSMailSortByFlagStatus ? 
			   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
			   iSortIconArray[ ESortListFollowDescIcon ] : 
			   iSortIconArray[ ESortListFollowAscIcon ] : 
			   iSortIconArray[ ESortListFollowDescIcon ];
			   
		itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_FLAG, KFsTreeRootID, icon, EFalse );
		iModel->AppendL( itemId, EFSMailSortByFlagStatus );
		}

	icon = iCurrentSortCriteria.iField == EFSMailSortByPriority ? 
		   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
		   iSortIconArray[ ESortListPriorityDescIcon ] : 
		   iSortIconArray[ ESortListPriorityAscIcon ] : 
		   iSortIconArray[ ESortListPriorityDescIcon ];
	
	itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_PRIORITY, KFsTreeRootID, icon, EFalse );
	iModel->AppendL( itemId, EFSMailSortByPriority );

	if ( iCurrentFolderType != EFSSentFolder &&
		 iCurrentFolderType != EFSDraftsFolder &&
		 iCurrentFolderType != EFSOutbox )
		{
		icon = iCurrentSortCriteria.iField == EFSMailSortByUnread ? 
			   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
			   iSortIconArray[ ESortListUnreadDescIcon ] : 
			   iSortIconArray[ ESortListUnreadAscIcon ] : 
			   iSortIconArray[ ESortListUnreadDescIcon ];
		
		itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_UNREAD, KFsTreeRootID, icon, EFalse );
		iModel->AppendL( itemId, EFSMailSortByUnread );
		}

	// Allow scroll bar refresh only for the last item
	icon = iCurrentSortCriteria.iField == EFSMailSortByAttachment ? 
		   iCurrentSortCriteria.iOrder == EFSMailAscending ? 
		   iSortIconArray[ ESortListAttachmentDescIcon ] : 
		   iSortIconArray[ ESortListAttachmentAscIcon ] : 
		   iSortIconArray[ ESortListAttachmentDescIcon ];
		   
	itemId = AppendItemToListFromResourceL( R_FREESTYLE_EMAIL_UI_SORT_BY_ATTACHMENT, KFsTreeRootID, icon, ETrue );
	iModel->AppendL( itemId, EFSMailSortByAttachment );
	}

// ---------------------------------------------------------------------------
// Recursive function to get the folder's subfolder data
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::AppendSubfoldersL(
	TFSMailMsgId aFolder, TFsTreeItemId aParentNode, TBool aRefreshLastItem )
	{
    FUNC_LOG;
	RPointerArray<CFSMailFolder> folders;
    CleanupResetAndDestroyClosePushL( folders );

	iActiveMailbox->ListFolders( aFolder, folders );

	TLinearOrder<CFSMailFolder> order(
		CFSEmailUiFolderListVisualiser::CompareFolders );
	folders.Sort( order );

	// First check which folders are not shown, and remove those from the list.
	// This is done in separate loop to be able to check in next loop which
	// folder is last one to be added. This information is needed to enhance
	// list performance by allowing scroll bar refresh only for the last item.
	for( TInt i = folders.Count() - 1; i >= 0; i-- )
	    {
        if( !ShowThisTypeOfFolderL( folders[i] ) )
            {
            // Skip this folder
            delete folders[i];
            folders.Remove(i);
            }
	    }

	TBool allowItemRefresh = EFalse;
	while( folders.Count() > 0 )
		{
		// If last item of this level is to be refreshed, and we are in the
		// last item, allow the refresh. If this last item is leaf folder,
		// then list is refreshed during it's drawn. If this last item is
		// node (it has some subfolders), then we don't refresh it, but
		// instead we forward the info to next level with next
		// AppendSubfoldersL function call.
		if( aRefreshLastItem && folders.Count() == 1 )
		    {
		    allowItemRefresh = ETrue;
		    }

		// Performance optimisations concerning FS Generic scroll bar update
		// logic are implemented so that they assume that all leaf folders
		// are added to list as leaf items, not nodes
		if( iFullScreen && folders[0]->GetSubFolderCount() > 0 )
			{
			TFsTreeItemId nodeId = AppendNodeFolderL( folders[0], aParentNode );
            // If the returned item id is KFsTreeNoneID, it means that
            // the current folder was not added to folder list, so we
            // should not append it to model either
			if( nodeId != KFsTreeNoneID )
				{
                // Note that ownership of the folder object is NOT transferred
                // to model anymore, so we need to delete it here in all cases
				iModel->AppendL( nodeId, folders[0] );

				AppendSubfoldersL( folders[0]->GetFolderId(), nodeId, allowItemRefresh );
				}
            delete folders[0];
            folders.Remove(0);
            }
		else
			{
			TFsTreeItemId leafId = AppendLeafFolderL( folders[0], aParentNode, allowItemRefresh );
            // If the returned item id is KFsTreeNoneID, it means that
            // the current folder was not added to folder list, so we
            // should not append it to model either
			if( leafId != KFsTreeNoneID )
				{
                // Note that ownership of the folder object is NOT transferred
                // to model anymore, so we need to delete it here in all cases
				iModel->AppendL( leafId, folders[0] );
				}
			delete folders[0];
            folders.Remove(0);
			}
		}

    CleanupStack::PopAndDestroy( &folders );
	}

// ---------------------------------------------------------------------------
// Get "standard" folder's resource id (for name) and icon by folder type
// Returns ETrue if folder type is one of the "standard" folders
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::GetStandardFolderResouceIdAndIconL( const TInt aFolderType, TInt &aResourceId, CAlfTexture* &aIcon ) const
	{
    FUNC_LOG;
	TBool found(EFalse);
	if( aFolderType == EFSInbox )
		{
		if( iFullScreen )
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_FOLDER_LIST_INBOX;
			aIcon = iIconArray[EFolderListIconInbox];
			}
		else
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_INBOX;
			// Get branded mailbox icon
			aIcon = &iAppUi.FsTextureManager()->TextureByMailboxIdL( iActiveMailbox->GetId().PluginId(),
																	 iActiveMailbox->GetId().Id(),
																     iListIconSize);
			}
		found = ETrue;
		}
	else if( aFolderType == EFSOutbox )
		{
		if( iFullScreen )
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_FOLDER_LIST_OUTBOX;
			aIcon = iIconArray[EFolderListIconOutbox];
			}
		else
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_OUTBOX;
			}
		found = ETrue;
		}
	else if( aFolderType == EFSDraftsFolder )
		{
		if( iFullScreen )
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_FOLDER_LIST_DRAFTS;
			aIcon = iIconArray[EFolderListIconDrafts];
			}
		else
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_DRAFTS;
			}
		found = ETrue;
		}
	else if( aFolderType == EFSSentFolder )
		{
		if( iFullScreen )
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_FOLDER_LIST_SENT;
			aIcon = iIconArray[EFolderListIconSent];
			}
		else
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_SENT;
			}		
		found = ETrue;
		}
	else if( aFolderType == EFSDeleted )
		{
		if( iFullScreen )
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_FOLDER_LIST_DELETED;
			aIcon = iIconArray[EFolderListIconDeleted];
			}
		else
			{
			aResourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_DELETED;
			}
		found = ETrue;
		}
	return found;
	}

// ---------------------------------------------------------------------------
// Returns true if the specified folder type should be shown in current view
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::ShowThisTypeOfFolderL(
	CFSMailFolder* aFolder ) const
	{
    FUNC_LOG;
	if( iCustomMessageId == KFolderListMoveMessage ||
    	iCustomMessageId == KFolderListMoveMessages )
		{
		// When moving or copying message(s), hide Outbox from list
		if( aFolder->GetFolderType() == EFSOutbox )
			{
			return EFalse;
			}
		// In case of other folders, ask from the folder itself
		else
			{
			return aFolder->SupportsMoveFromL( iCurrentFolderType );
			}
		}
	else if( iCustomMessageId == KFolderListCopyMessage ||
			 iCustomMessageId == KFolderListCopyMessages )
		{
		// When moving or copying message(s), hide Outbox from list
		if( aFolder->GetFolderType() == EFSOutbox )
			{
			return EFalse;
			}
		// In case of other folders, ask from the folder itself
		else
			{
			return aFolder->SupportsCopyFromL( iCurrentFolderType );
			}
		}

	// In case of folder selection list, show all types of folders. In case of
	// control bar list, subfolders are filtered AppendNode/LeafFolderL().
	return ETrue;
	}

// ---------------------------------------------------------------------------
// Append the folder to list as node
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiFolderListVisualiser::AppendNodeFolderL(
	CFSMailFolder* aFolder, TFsTreeItemId aParentNode )
	{
    FUNC_LOG;
	TInt folderType = aFolder->GetFolderType();
	TInt resourceId( 0 );
	CAlfTexture* icon( NULL );
	TFsTreeItemId nodeId( KFsTreeNoneID );

	// Check if the folder is one of the "standard" folders
	if ( GetStandardFolderResouceIdAndIconL( folderType, resourceId, icon ) )
		{
		// For "standard" folders use the (localised) name from the resource
		nodeId = AppendNodeToListFromResourceL( resourceId, aParentNode, icon, aFolder->GetUnreadCount() );
		}
	else
		{
		// Append non-standard folders only if in Fullscreen
		if( iFullScreen )
			{
			// For other folders get the name from the folder object

			nodeId = AppendNodeToListL(
			        &aFolder->GetFolderName(),
			        aParentNode,
			        iIconArray[EFolderListIconInboxSubfolders],
			        aFolder->GetUnreadCount());
			}
		}
	return nodeId;
	}

// ---------------------------------------------------------------------------
// Append the folder to list as leaf
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiFolderListVisualiser::AppendLeafFolderL(
	CFSMailFolder* aFolder, TFsTreeItemId aParentNode, TBool aAllowRefresh )
	{
    FUNC_LOG;
	TInt folderType = aFolder->GetFolderType();
	TInt resourceId( 0 );
	CAlfTexture* icon( NULL );
	TFsTreeItemId itemId( KFsTreeNoneID );

	// Check if the folder is one of the "standard" folders
	if ( GetStandardFolderResouceIdAndIconL( folderType, resourceId, icon ) )
		{
		itemId = AppendItemToListFromResourceL( resourceId, aParentNode, icon, aAllowRefresh, aFolder->GetUnreadCount() );
		}
	else
		{
		// Append non-standard folders only if in Fullscreen
		if ( iFullScreen )
			{
			// For other folders get the name from the folder object
			itemId = AppendItemToListL(
			        &aFolder->GetFolderName(),
			        aParentNode,
			        iIconArray[EFolderListIconInboxSubfolders],
			        aAllowRefresh,
			        aFolder->GetUnreadCount());
			}
		}
	return itemId;
	}

// ---------------------------------------------------------------------------
// Append mailbox data
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::AppendMailboxesL()
	{
    FUNC_LOG;
	RPointerArray<CFSMailBox> mailboxes;
    CleanupResetAndDestroyClosePushL( mailboxes );
	TFSMailMsgId id;
    iAppUi.GetMailClient()->ListMailBoxes( id, mailboxes );

    TInt count = mailboxes.Count();

	while( mailboxes.Count() > 0 )
		{
		if( !( mailboxes[0]->GetId() == iAppUi.GetActiveMailboxId() ) )
			{
			MFSMailBrandManager& brandManager = iAppUi.GetMailClient()->GetBrandManagerL();

			CAlfTexture* mailBoxTexture;
			CGulIcon* mailboxIcon( NULL );
			TRAPD( err, mailboxIcon = brandManager.GetGraphicL( EFSMailboxIcon, mailboxes[0]->GetId() ) );
			if ( err == KErrNone && mailboxIcon )
				{
				CleanupStack::PushL( mailboxIcon );
				AknIconUtils::SetSize(mailboxIcon->Bitmap(), iListIconSize);
			    AknIconUtils::SetSize(mailboxIcon->Mask(), iListIconSize);

			    // Create texture into TextureManager, If not already existing
			    iAppUi.FsTextureManager()->CreateBrandedMailboxTexture( mailboxIcon,
			    		                                                mailboxes[0]->GetId().PluginId(),
			    		                                                mailboxes[0]->GetId().Id(),
			    		                                                iListIconSize);
			    // Get branded mailbox icon
			    mailBoxTexture = &iAppUi.FsTextureManager()->TextureByMailboxIdL( mailboxes[0]->GetId().PluginId(),
			    		                                                          mailboxes[0]->GetId().Id(),
			    		                                                          iListIconSize);

				CleanupStack::PopAndDestroy( mailboxIcon );
				}
			else
				{
				mailBoxTexture = iIconArray[EFolderListIconEmailAccount];
				}

			// Branded mailbox name is nowadays set in new mailbox event
			// handling, so we don't need to use brand manager here anymore.
			// Mailboxes are the first items in the list, and there probably
			// aren't that much of those, so let's allow scroll bar refresh
			// for all of them.
			TFsTreeItemId itemId = AppendItemToListL( &mailboxes[0]->GetName(),
													  KFsTreeRootID,
													  mailBoxTexture,
													  ETrue );

			// If the returned item id is KFsTreeNoneID, it means that
            // the current mailbox was not added to mailbox list, so we
			// should not append it to model either
            if( itemId != KFsTreeNoneID )
                {
                // Note that ownership of the mailbox object is NOT transferred
                // to model anymore, so we need to delete it here in all cases
                iModel->AppendL( itemId, mailboxes[0] );
                }

			}

		delete mailboxes[0];
        mailboxes.Remove(0);
		}
	//There is more then 1 mailbox so we add the seperator at the end
	if( count > 1 )
		{
		AppendSeparatorLineL();
		}

    CleanupStack::PopAndDestroy( &mailboxes );
	}


// ---------------------------------------------------------------------------
// Append separator between active mailbox's folders and other mailboxes
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::AppendSeparatorLineL()
	{
    FUNC_LOG;
	CFsSeparatorData* data = CFsSeparatorData::NewL();

	CFsSeparatorVisualizer* visualizer( NULL );
    visualizer = CFsSeparatorVisualizer::NewL(*iTreeList->TreeControl());

    // We use the default size of the separator, but we need to save it
    // here to have it available later when calculating the list height
    iListSeparatorHeight = visualizer->Size().iHeight;

    // This should never be the last item in the list, so disable the refresh
    iTreeList->InsertItemL( *data, *visualizer, KFsTreeRootID, KErrNotFound, EFalse);

    iModel->IncreaseSeparatorCount();
	}

// ---------------------------------------------------------------------------
// Append separator between active mailbox's folders and other mailboxes
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::AppendMoreFoldersL()
	{
    FUNC_LOG;
	TInt resourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_MORE_FOLDERS;
	CAlfTexture* icon = iIconArray[EFolderListIconMoreFolders];

	// This might be the last item in the list, so allow scroll bar refresh
	iMoreFoldersItemId = AppendItemToListFromResourceL( resourceId, KFsTreeRootID, icon, ETrue );
	}

// ---------------------------------------------------------------------------
// Append node to list by getting the item data from resource
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiFolderListVisualiser::AppendNodeToListFromResourceL( TInt aResourceId, TFsTreeItemId aParentNode, CAlfTexture* aIcon, TUint aUnreadCnt )
	{
    FUNC_LOG;
 	HBufC* headingText = StringLoader::LoadLC( aResourceId );

	TFsTreeItemId nodeId = AppendNodeToListL( headingText, aParentNode, aIcon, aUnreadCnt);

   	CleanupStack::PopAndDestroy(headingText);

   	return nodeId;
	}

// ---------------------------------------------------------------------------
// Append node to list
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiFolderListVisualiser::AppendNodeToListL( TDesC* aItemData, TFsTreeItemId aParentNode, CAlfTexture* aIcon, TUint aUnreadCnt )
	{
    FUNC_LOG;
    CFsTreePlainOneLineNodeData* plainItemData;
    CFsTreePlainOneLineNodeVisualizer* plainNodeVisualizer;

    HBufC* itemDispName = HBufC::NewLC(aItemData->Length() + KFmtUnRdCntMaxLength);
    itemDispName->Des().Copy(*aItemData);
    if( aUnreadCnt > 0 )
        {
        itemDispName->Des().AppendFormat(KFormatUnreadCnt, aUnreadCnt);
        }
	CreatePlainNodeLC2( itemDispName, plainItemData, plainNodeVisualizer, aIcon );

	// We assume that node can never be the last item to be drawn, there will
	// always be leaf folder under it. So we can always deny scroll bar update.
    TFsTreeItemId nodeId = iTreeList->InsertNodeL( *plainItemData, *plainNodeVisualizer, aParentNode, KErrNotFound, EFalse);
    CleanupStack::Pop( 2 ); // plainItemData & plainNodeVisualizer
	CleanupStack::PopAndDestroy( itemDispName );
	if( nodeId != KFsTreeNoneID )
		{
	    iTreeList->ExpandNodeL(nodeId);
		iListItemVisulizers.Append( plainNodeVisualizer );
		}

   	return nodeId;
	}

// ---------------------------------------------------------------------------
// Append leaf to list by getting the item data from resource
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiFolderListVisualiser::AppendItemToListFromResourceL( TInt aResourceId,
																			 TFsTreeItemId aParentNode,
																			 CAlfTexture* aIcon,
																			 TBool aAllowRefresh,
																			 TUint aUnreadCnt)
	{
    FUNC_LOG;
 	HBufC* headingText = StringLoader::LoadLC( aResourceId );
 	
	TFsTreeItemId itemId = AppendItemToListL( headingText, aParentNode, aIcon, aAllowRefresh,  aUnreadCnt);

   	CleanupStack::PopAndDestroy( headingText );

   	return itemId;
	}

// ---------------------------------------------------------------------------
// Append leaf to list
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiFolderListVisualiser::AppendItemToListL( TDesC* aItemData,
																 TFsTreeItemId aParentNode,
																 CAlfTexture* aIcon,
                                                                 TBool aAllowRefresh,
                                                                 TUint aUnreadCnt)
	{
    FUNC_LOG;
    CFsTreePlainOneLineItemData* plainItemData;
    CFsTreePlainOneLineItemVisualizer* plainItemVisualizer;

    HBufC* itemDispName = HBufC::NewLC(aItemData->Length() + KFmtUnRdCntMaxLength);
    itemDispName->Des().Copy(*aItemData);
    if( aUnreadCnt > 0 )
        {
        itemDispName->Des().AppendFormat(KFormatUnreadCnt, aUnreadCnt);
        }
    CreatePlainItemLC2( itemDispName, plainItemData, plainItemVisualizer, aIcon );


    TFsTreeItemId itemId = iTreeList->InsertItemL( *plainItemData, *plainItemVisualizer, aParentNode, KErrNotFound, aAllowRefresh);
    CleanupStack::Pop( 2 ); // plainItemData & plainItemVisualizer
	CleanupStack::PopAndDestroy( itemDispName );
	if( itemId != KFsTreeNoneID )
		{
		iListItemVisulizers.Append( plainItemVisualizer );
		}

	return itemId;
	}

// ---------------------------------------------------------------------------
// Create plain leaf item
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::CreatePlainItemLC2( const TDesC* aItemDataBuff,
                                                         CFsTreePlainOneLineItemData* &aItemData,
                                                         CFsTreePlainOneLineItemVisualizer* &aItemVisualizer,
                                                         CAlfTexture* aIcon )
	{
    FUNC_LOG;
    aItemData = CFsTreePlainOneLineItemData::NewL();
    CleanupStack::PushL( aItemData );

    aItemData->SetDataL( *aItemDataBuff );
    if( aIcon )
    	{
		aItemData->SetIcon( *aIcon );
    	}

    aItemVisualizer = CFsTreePlainOneLineItemVisualizer::NewL( *iTreeList->TreeControl() );
    CleanupStack::PushL( aItemVisualizer );
    iListItemDatas.AppendL( aItemData );
    // Set folder view specific layouts to be used
    if ( iFullScreen )
        {
        aItemVisualizer->SetLayoutHints( CFsTreeItemVisualizerBase::EFolderLayout );
        }
    else
        {
        aItemVisualizer->SetLayoutHints( CFsTreeItemVisualizerBase::EPopupLayout );
        }

    SetItemVisualizerPropertiesL( aItemVisualizer );
	}

// ---------------------------------------------------------------------------
// Create plain node itme
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::CreatePlainNodeLC2( TDesC* aItemDataBuff,
                                                         CFsTreePlainOneLineNodeData* &aItemData,
                                                         CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer,
                                                         CAlfTexture* aIcon )
	{
    FUNC_LOG;
    aItemData = CFsTreePlainOneLineNodeData::NewL();
    CleanupStack::PushL( aItemData );

    aItemData->SetDataL( *aItemDataBuff );
    if( aIcon )
    	{
		aItemData->SetIconExpanded( *aIcon );
		aItemData->SetIconCollapsed( *aIcon );
    	}

    aNodeVisualizer = CFsTreePlainOneLineNodeVisualizer::NewL( *iTreeList->TreeControl() );
    CleanupStack::PushL( aNodeVisualizer );

    // Set folder view specific layouts to be used
    if ( iFullScreen )
        {
        aNodeVisualizer->SetLayoutHints( CFsTreeItemVisualizerBase::EFolderLayout );
        }
    else
        {
        aNodeVisualizer->SetLayoutHints( CFsTreeItemVisualizerBase::EPopupLayout );
        }
    SetItemVisualizerPropertiesL( aNodeVisualizer );
	}

void CFSEmailUiFolderListVisualiser::SetItemVisualizerPropertiesL( MFsTreeItemVisualizer* aItemVisualizer )
	{
    FUNC_LOG;
	aItemVisualizer->SetFlags(aItemVisualizer->Flags() & ~KFsTreeListItemManagedLayout);

	if (iFullScreen)
		{
	    aItemVisualizer->SetSize( iAppUi.LayoutHandler()->FolderListItemSizeInThisResolution( iScreenRect ) );
		}
	else
	    {
	    aItemVisualizer->SetSize(TSize(iScreenRect.Width(), iListItemHeight));
        }

    aItemVisualizer->SetExtendable( EFalse );

  	// Set correct skin text colors for the list items
   	TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
	// If list is in full screen, it has general list background, so we use
	// general list text color. If it's not in full screen, we use dropdown
	// menu specific text color.
	TRgb normalColor = KRgbBlack;
	if( iFullScreen )
		{
		normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
		}
	else
		{
		normalColor = iAppUi.LayoutHandler()->DropdownMenuTextColor();
		}

    aItemVisualizer->SetFocusedStateTextColor( focusedColor );
    aItemVisualizer->SetNormalStateTextColor( normalColor );

	// Set font size
    aItemVisualizer->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips( !iFullScreen ) );
	// Set node bolded
	aItemVisualizer->SetTextBold( EFalse );
	}

// ---------------------------------------------------------------------------
// If event is pointer event and popup is showing, close popup and return
// ETrue to tell the caller that event was consumed. If the popup is not
// showing but pointer up event haven't been received since the popup was
// closed, then consume event.
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::HandleWsEventL( const TWsEvent& aEvent )
    {
    FUNC_LOG;
    TBool eventHandled( EFalse );
    if ( aEvent.Type() == EEventPointer )
        {
        const TPointerEvent* pointerEvent( aEvent.Pointer() );
        if ( iPopupListShown )
            {
            const TPoint& position( pointerEvent->iParentPosition );
            if ( !CbaButtonPressed( position ) && pointerEvent->iType == TPointerEvent::EButton1Down )
                {
                // get popup rect in local coordinates
                TRect popupRect( iScreenRect );
                // translate to screen coorinates
                popupRect.Move( iEnv.PrimaryDisplay().VisibleArea().iTl );
                // check if the event happened outside of the popup
                if ( !popupRect.Contains( position ) )
                    {
                    HandleSelectionL( EFSEmailUiCtrlBarResponseCancel );
                    iConsumeUntilNextUpEvent = eventHandled = ETrue;
                    }
                }
            }
        else if ( iConsumeUntilNextUpEvent )
            {
            if ( pointerEvent->iType == TPointerEvent::EButton1Up )
                {
                iConsumeUntilNextUpEvent = EFalse;
                }
            eventHandled = ETrue;
            }
        }
    return eventHandled;
    }

// Handle foreground event (called by FreestyleEmailUiMailListVisualiser)
void CFSEmailUiFolderListVisualiser::HandleForegroundEventL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iCallback != NULL || iSortListCallback != NULL )
        {
        ResizeListIcons();
        ResizeListItemsL();
        iParentLayout->SetRect( iScreenRect );
        SetAnchors();
        }
    }

// ---------------------------------------------------------------------------
// Handle dynamic switch
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType /*aType*/ )
	{
    FUNC_LOG;
    if (IsPopupShown() || IsFullScreen())
        {
        if ( iFirstStartCompleted ) // Safety
            {
            // Resize fader layout
            TRect mainPaneRect;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
            iFaderLayout->SetRect( mainPaneRect );

            iTreeList->HideListL( EFalse );
            UpdateListSizeAttributes();
            ResizeListIcons();
            ResizeListItemsL();
            SetAnchors();
            if( !iFullScreen )
                {
                AdaptScreenRectToListContent();
                TAlfMetric padding(
                        iAppUi.LayoutHandler()->DropDownMenuListPadding() );
                iParentLayout->SetPadding( padding );
                }
            else
                {
                TAlfMetric padding( 0 );
                iParentLayout->SetPadding( padding );
                if ( iHeaderTextVisual->Opacity().ValueNow() )
                    {
                    // Update text caption colors only if visible
                    SetHeaderTextAttributesL();
                    }
                }
            iParentLayout->SetRect( iScreenRect );

            // Make sure that the horizontal position is recalculated
            DoHorizontalScrollL( ETrue );

            // RefreshListViewL not working correctly
            //iTreeVisualizer->RefreshListViewL();

            iTreeList->ShowListL();
            }
        }
	}

// ---------------------------------------------------------------------------
// Expand/collapse the focused list item
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ExpandOrCollapseL(
        const TExpandCollapseType& aType )
	{
    FUNC_LOG;
	TFsTreeItemId focusedItem = iTreeList->FocusedItem();

	if( focusedItem != KFsTreeNoneID && iTreeList->IsNode( focusedItem ) )
		{
		if( aType == EFolderListCollapse ||
		    ( aType == EFolderListAutomatic && iTreeList->IsExpanded( focusedItem ) ) )
			{
			iTreeList->CollapseNodeL( focusedItem );
			}
		else
			{
			iTreeList->ExpandNodeL( focusedItem );
			}
		}
	}

// ---------------------------------------------------------------------------
// ETrue if in full screen mode, otherwise EFalse
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::IsFullScreen() const
	{
    FUNC_LOG;
	return iFullScreen;
	}

// ---------------------------------------------------------------------------
// Update list size members variables
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::UpdateListSizeAttributes()
	{
    FUNC_LOG;
    iListItemHeight = iAppUi.LayoutHandler()->OneLineListItemHeight();

	if( iFullScreen )
		{
	 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, iScreenRect );
	 	iScreenRect.SetRect( 0, 0, iScreenRect.Width(), iScreenRect.Height() );
        }
	else
		{
        TRect cbr = iAppUi.LayoutHandler()->GetControlBarRect();
		if( iCallback )
			{
			iCtrlButtonRect = iCallback->FolderButtonRect();
            iCtrlButtonRect.Move( cbr.iTl );
			iScreenRect = FolderListRectInThisResolution();
			}
		else if ( iSortListCallback )
			{
            iCtrlButtonRect = iSortListCallback->SortButtonRect();
            iCtrlButtonRect.Move( cbr.iTl );
            iScreenRect = SortListRectInThisResolution();
			}
		else
		    {
		    // Do nothing. This happens if HandleDynamicVariantSwitch() is called while the popup list is not visible.
		    }
		}
	iListIconSize = iAppUi.LayoutHandler()->FolderListIconSize( !iFullScreen );
	}

TRect CFSEmailUiFolderListVisualiser::FolderListRectInThisResolution()
	{
    FUNC_LOG;
    return LeftListRectInThisResolution();
	}

TRect CFSEmailUiFolderListVisualiser::SortListRectInThisResolution()
	{
    FUNC_LOG;
    return RightListRectInThisResolution();
    }

TRect CFSEmailUiFolderListVisualiser::LeftListRectInThisResolution()
	{
    FUNC_LOG;
    return iAppUi.LayoutHandler()->DropDownMenuListRect(
            CFSEmailUiLayoutHandler::ELeft );
	}

TRect CFSEmailUiFolderListVisualiser::RightListRectInThisResolution()
	{
    FUNC_LOG;
    return iAppUi.LayoutHandler()->DropDownMenuListRect(
            CFSEmailUiLayoutHandler::ERight );
	}


// ---------------------------------------------------------------------------
// Load the needed icons
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::LoadIconsL()
	{
    FUNC_LOG;
	iIconArray.Reset();
	if ( iFullScreen )
	    {
        // NOTE: Must be appended same order as are in TFsEmailUiFolderListIcons!
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListInboxTexture ) );   			// EFolderListIconInbox
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListInboxSubfoldersTexture ) );	// EFolderListIconInboxSubfolders
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListOutboxTexture ) );  			// EFolderListIconOutbox
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListDraftsTexture ) );  			// EFolderListIconDrafts
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListSentTexture ) );    			// EFolderListIconSent
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListDeletedItemsTexture ) ); 	// EFolderListIconDeleted
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListServerFoldersTexture ) ); 	// EFolderListIconServerFolders
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListMoreFoldersTexture ) ); 		// EFolderListIconMoreFolders
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListEmailAccountTexture ) ); 	// EFolderListIconEmailAccount
	    }
	else
	    {
        // NOTE: Must be appended same order as are in TFsEmailUiFolderListIcons!
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListInboxTexturePopup ) );            // EFolderListIconInbox
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListInboxSubfoldersTexturePopup ) );  // EFolderListIconInboxSubfolders
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListOutboxTexturePopup ) );           // EFolderListIconOutbox
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListDraftsTexturePopup ) );           // EFolderListIconDrafts
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListSentTexturePopup ) );             // EFolderListIconSent
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListDeletedItemsTexturePopup ) );     // EFolderListIconDeleted
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListServerFoldersTexturePopup ) );    // EFolderListIconServerFolders
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListMoreFoldersTexturePopup ) );      // EFolderListIconMoreFolders
        iIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( EFolderListEmailAccountTexturePopup ) );     // EFolderListIconEmailAccount
	    
	    // NOTE: Must be appended same order as are in TFsEmailUiSortListIcons!
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListAttachmentAscTexture ) );       // ESortListAttachmentAscIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListAttachmentDescTexture ) );      // ESortListAttachmentDescIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListDateAscTexture ) );       		 // ESortListDateAscIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListDateDescTexture ) );       	 // ESortListDateDescIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListFollowAscTexture ) );       	 // ESortListFollowAscIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListFollowDescTexture ) );        	 // ESortListFollowDescIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListPriorityAscTexture ) );         // ESortListPriorityAscIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListPriorityDescTexture ) );        // ESortListPriorityDescIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListSenderAscTexture ) );       	 // ESortListSenderAscIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListSenderDescTexture ) );       	 // ESortListSenderDescIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListSubjectAscTexture ) );       	 // ESortListSubjectAscIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListSubjectDescTexture ) );       	 // ESortListSubjectDescIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListUnreadAscTexture ) );       	 // ESortListUnreadAscIcon
        iSortIconArray.AppendL( &iAppUi.FsTextureManager()->TextureByIndex( ESortListUnreadDescTexture ) );       	 // ESortListUnreadDescIcon
	    }
	}

void CFSEmailUiFolderListVisualiser::ResizeListIcons()
	{
    FUNC_LOG;
	// Resize icons only if the size has changed since previous resize operation
	if( iListIconSize != iPreviousListIconSize )
		{
		iPreviousListIconSize = iListIconSize;

		//       Resize of textures is not working anymore in Alfred, instead
		//       we should resize the visuals. But that's not possible with
		//       with the current implementation of generic list.
		// First resize "standard" list icons
		TInt arraySize = iIconArray.Count();
		for( TInt i = 0 ; i < arraySize ; i++ )
			{
			iIconArray[i]->Size().SetSize( iListIconSize.iWidth, iListIconSize.iHeight );
			}

		// Then resize used branded mailbox icons
		//       Branded mailbox icons are not stored anymore in our own
		//       array, instead those are owned by texture manager. So
		//       those should be resized by other means. But currently
		//       there is no way to do it (see above comment).
		
		// Then resize sort list icons
		TInt arraySizeSort = iSortIconArray.Count();
		for( TInt i = 0; i < arraySizeSort; i++ )
			{
			iSortIconArray[i]->Size().SetSize( iListIconSize.iWidth, iListIconSize.iHeight );
			}
		}
	}

// ---------------------------------------------------------------------------
// Resizes the list items without recreating all items
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::ResizeListItemsL()
	{
    FUNC_LOG;
	TInt arraySize = iListItemVisulizers.Count();
	for( TInt i = 0 ; i < arraySize ; i++ )
		{
		SetItemVisualizerPropertiesL( iListItemVisulizers[i] );
		}
	}

// ---------------------------------------------------------------------------
// Optimises the screen rect by the list content
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::AdaptScreenRectToListContent()
	{
    FUNC_LOG;
    const TInt popupHeight( iScreenRect.Height() );
    const TInt separatorHeight( iListSeparatorHeight *
            iModel->SeparatorCount() );
    const TInt itemCount( iTreeList->Count() - iModel->SeparatorCount() );
    TInt visibleItemCount( ( popupHeight - separatorHeight ) /
            iListItemHeight );
    visibleItemCount = Min( visibleItemCount, itemCount );
    iScreenRect.SetHeight( visibleItemCount * iListItemHeight +
            separatorHeight +
            iAppUi.LayoutHandler()->DropDownMenuListPadding() * 2 +
            iAppUi.LayoutHandler()->ControlBarListPadding().iY );
	AdjustWidthByContent( iScreenRect );
	}

// ---------------------------------------------------------------------------
// Adjusts list width according to content.
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::AdjustWidthByContent( TRect& aRect ) const
    {
    const TRect oldButtonRect( iScreenRect );
    const TInt buttonWidth( iCtrlButtonRect.Width() );
    const TInt currentWidth( aRect.Width() );

    // only do adjusting if the buttonWidth is smaller than current popup width
    if ( buttonWidth < currentWidth )
        {
        TBool landscape( Layout_Meta_Data::IsLandscapeOrientation() );
        TAknLayoutRect scrollPane;
        scrollPane.LayoutRect( aRect,
                AknLayoutScalable_Apps::sp_fs_scroll_pane_cp01( 6 ) );
        const TInt scrollPaneWidth( scrollPane.Rect().Width() );

        TAknLayoutText textLayout;
        textLayout.LayoutText( aRect,
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t1( 0 ) );
        const CFont& font( *(textLayout.Font()) );
        const TInt padding(
                iAppUi.LayoutHandler()->DropDownMenuListPadding() * 2 );
        if ( iListItemDatas.Count() > 0 )
            {
            TInt width( 0 );
            for ( TInt i = 0; i < iListItemDatas.Count(); i++ )
                {
                TInt totalWidth( scrollPaneWidth );
                const CFsTreePlainOneLineItemData& data( *iListItemDatas[i] );
                totalWidth += font.TextWidthInPixels( data.Data() );
                totalWidth += padding;
                if ( data.IsIconSet() )
                    {
                    TAknLayoutRect iconPane;
                    iconPane.LayoutRect( aRect,
                            AknLayoutScalable_Apps::list_single_dyc_row_pane_g1( 0 ) );
                    totalWidth += iconPane.Rect().Width();
                    }
                if (totalWidth > width)
                    {
                    width = totalWidth;
                    }
                }
            if ( currentWidth > width )
                {
                if ( width < buttonWidth )
                    {
                    width = buttonWidth;
                    }
                aRect.SetWidth( width );
                if ( AknLayoutUtils::LayoutMirrored() )
                    {
                    if ( iCallback )
                        {
                        const TInt targetX( iCtrlButtonRect.iBr.iX - width );
                        aRect.Move( -aRect.iTl.iX + targetX, 0 );
                        }
                    else
                        {
                        aRect.Move( -aRect.iTl.iX + iCtrlButtonRect.iTl.iX, 0 );
                        }
                    }
                else
                    {
                    if ( !iCallback )
                        {
                        const TInt targetX( iCtrlButtonRect.iBr.iX - width );
                        aRect.Move( -aRect.iTl.iX + targetX, 0 );
                        }
                    else
                        {
                        aRect.Move( -aRect.iTl.iX + iCtrlButtonRect.iTl.iX, 0 );
                        }
                    }
                
                // Keep the right edge position unchanged
                if( landscape )
                    {
                    aRect.Move( oldButtonRect.iBr.iX - aRect.iBr.iX, 0 );
                    }
                }
            }
        }
    else
        {
        aRect.SetWidth( buttonWidth );
        if ( !iCallback ) // is sort menu?
            {
            aRect.Move( currentWidth - buttonWidth, 0 );
            }
        }
    }

void CFSEmailUiFolderListVisualiser::SetAnchors()
	{
    FUNC_LOG;
    if ( iFullScreen && iTitleCaptionVisible )
        {
        iListHeaderHeight = iAppUi.LayoutHandler()->ControlBarHeight();
        }
    else
        {
        iListHeaderHeight = 0;
        }

    TSize mainPaneSize;
    AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, mainPaneSize);

    // Set anchor for background visual
    iParentLayout->SetAnchor(EAlfAnchorTopLeft,
                    0,
                    EAlfAnchorOriginLeft,
                    EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint(0, 0));
    iParentLayout->SetAnchor(EAlfAnchorBottomRight,
                    0,
                    EAlfAnchorOriginRight,
                    EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint(mainPaneSize.iWidth,mainPaneSize.iHeight));

	// Set anchor for header text visual
	iParentLayout->SetAnchor(EAlfAnchorTopLeft,
					1,
					EAlfAnchorOriginLeft,
					EAlfAnchorOriginTop,
					EAlfAnchorMetricRelativeToSize,
					EAlfAnchorMetricRelativeToSize,
					TAlfTimedPoint(0, 0));
	iParentLayout->SetAnchor(EAlfAnchorBottomRight,
					1,
					EAlfAnchorOriginRight,
					EAlfAnchorOriginTop,
					EAlfAnchorMetricRelativeToSize,
					EAlfAnchorMetricAbsolute,
					TAlfTimedPoint(0,iListHeaderHeight));

	// Set anchor for list's parent layout
	iParentLayout->SetAnchor(EAlfAnchorTopLeft,
					2,
					EAlfAnchorOriginLeft,
					EAlfAnchorOriginTop,
					//EAlfAnchorMetricRelativeToSize,
					EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
					TAlfTimedPoint(0, iListHeaderHeight));
	iParentLayout->SetAnchor(EAlfAnchorBottomRight,
					2,
					EAlfAnchorOriginRight,
					EAlfAnchorOriginBottom,
					//EAlfAnchorMetricRelativeToSize,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
					TAlfTimedPoint(0, 0));
	}

// ---------------------------------------------------------------------------
// Moves the focus to the topmost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::GoToTopL()
    {
    FUNC_LOG;
    TInt topLevelCount = iTreeList->CountChildren( KFsTreeRootID );
    if ( topLevelCount )
        {
        TFsTreeItemId topId = iTreeList->Child(KFsTreeRootID, 0);
        iTreeVisualizer->SetFocusedItemL( topId );
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the bottommost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::GoToBottomL()
    {
    FUNC_LOG;
    // There may be arbitrary number of nested sub folders. Make sure we focus
    // the bottommost visible subfolder.
    TFsTreeItemId bottomId = KFsTreeRootID;

    while ( iTreeList->IsNode(bottomId) &&
            iTreeList->IsExpanded(bottomId) &&
            iTreeList->CountChildren(bottomId) )
        {
        bottomId = iTreeList->Child( bottomId, iTreeList->CountChildren(bottomId)-1 );
        }

    if ( bottomId != KFsTreeRootID )
        {
        iTreeVisualizer->SetFocusedItemL( bottomId );
        }
    }

// ---------------------------------------------------------------------------
// Updates the toolbar buttons (sets the dimmed status of the collapse
// all/expand all buttons).
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::UpdateToolbarButtons()
    {
    FUNC_LOG;
    Toolbar()->SetItemDimmed( EFsEmailUiTbCmdExpandAll, AllNodesExpanded(), ETrue );
    Toolbar()->SetItemDimmed( EFsEmailUiTbCmdCollapseAll, AllNodesCollapsed(), ETrue );
    }

// ---------------------------------------------------------------------------
// Tells if all expandable nodes are collapsed
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::AllNodesCollapsed() const
    {
    FUNC_LOG;
    TFsTreeItemId itemId = KFsTreeNoneID;
    TInt count = iTreeList->CountChildren(KFsTreeRootID);

	// If top level is collapsed, then everything is collapsed. There's no need
	// to crawl any deeper in the tree hierarchy.
	for ( TInt i=0 ; i<count ; ++i )
	    {
	    itemId = iTreeList->Child( KFsTreeRootID, i );
	    if ( iTreeList->IsNode(itemId) &&
	         iTreeList->IsExpanded(itemId) )
	        {
	        return EFalse;
	        }
	    }

	return ETrue;
    }

// ---------------------------------------------------------------------------
// Tells if all expandable nodes are expanded
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::AllNodesExpanded( TFsTreeItemId aParentNodeId ) const
    {
    FUNC_LOG;
    // We must crawl through the whole tree to see, if there are any collapsed nodes
    // at any level. We do this with recursive depth-first-search.

    TFsTreeItemId itemId = KFsTreeNoneID;
    TInt count = iTreeList->CountChildren(aParentNodeId);

    for ( TInt i=0 ; i<count ; ++i )
        {
        itemId = iTreeList->Child( aParentNodeId, i );
        if ( iTreeList->IsNode(itemId) )
            {
            if ( !iTreeList->IsExpanded(itemId) ||
                 !AllNodesExpanded(itemId) )
                {
                return EFalse;
                }
            }
        }

	return ETrue;
    }

// ---------------------------------------------------------------------------
// Does a horizontal scrolling for the list by resizing and moving the list
// layout rect out of the screen from the left side.
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::DoHorizontalScrollL( TBool aForceRecalculation )
    {
    FUNC_LOG;
    // Horizontal scrolling needed only in full screen list,
    // because popup list doesn't contain any subfolders
    if( iFirstStartCompleted && iFullScreen )
        {
        TFsTreeItemId focusedId = iTreeList->FocusedItem();
        if ( focusedId > KFsTreeRootID )
            {
            // Get the list level
            TUint level = iTreeList->Level( focusedId );

            // Little optimisation, handle all levels up to KListLastBasicLevel
            // as root level because horizontal scroll amount is same for all
            if( level <= KListLastBasicLevel )
                {
                level = KListRootLevel;
                }

            // Recalculate the list rect only if level has changed or if
            // recalculation is forced by parameter (e.g. in case of
            // dynamic variant switch)
            if( ( level != iPreviousListLevel ) || aForceRecalculation )
                {
                TRect listRect = iListLayout->DisplayRectTarget();

                // Calulate list rect by taking x-coordinates from iScreenRect
                // and y-coordinates from list layouts target rect (to take
                // into account the list header)
                listRect.SetRect( iScreenRect.iTl.iX, listRect.iTl.iY,
                                  iScreenRect.iBr.iX, listRect.iBr.iY );

                if( level > KListLastBasicLevel )
                    {
                    // Calculate rect according to current level
                    TInt rectChange = (level - KListLastBasicLevel) * iTreeList->Indentation();
                    listRect.SetWidth( iScreenRect.Width() + rectChange );
                    listRect.Move( -rectChange, 0 );
                    }

                // Set the list rect only if it has changed from previous
                // (because list refresh causes a little twitch)
                if( listRect != iPreviousListRect )
                    {
                    iListLayout->SetRect( listRect, iAppUi.LayoutHandler()->CtrlBarListFadeEffectTime() );
                    iTreeVisualizer->RefreshListViewL();
                    iPreviousListRect = listRect;
                    }

                iPreviousListLevel = level;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Process a treelist event
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::TreeListEventL( const TFsTreeListEvent aEvent,
                                    const TFsTreeItemId /*aId*/ )
    {
    switch (aEvent)
        {
        case EFsTreeListItemTouchAction:
            {
            HandleSelectionL( EFSEmailUiCtrlBarResponseSelect );
            break;
            }

        // Note that in the handling of the following collapse/expand
        // events, we assume that if we have received a collapse (expand)
        // event then there must now be something to expand (collapse).
        // I.e. we assume that if there is no nesting in the tree list then
        // we never get any collapse/expand events.
        case EFsTreeListItemCollapsed:
            {
            Toolbar()->SetItemDimmed( EFsEmailUiTbCmdExpandAll, EFalse, ETrue );
            // Check whether or not everything is now collapsed: if so,
            // disable collapsing.
            if ( AllNodesCollapsed() )
                {
                Toolbar()->SetItemDimmed( EFsEmailUiTbCmdCollapseAll, ETrue, ETrue );
                }
            break;
            }
        case EFsTreeListItemExpanded:
            {
            Toolbar()->SetItemDimmed( EFsEmailUiTbCmdCollapseAll, EFalse, ETrue );
            // Check whether or not everything is now expanded: if so,
            // disable expanding.
            if ( AllNodesExpanded() )
                {
                Toolbar()->SetItemDimmed( EFsEmailUiTbCmdExpandAll, ETrue, ETrue );
                }
            break;
            }
        case EFsTreeListCollapsedAll:
            {
            // Everything has been collapsed, so disable collapsing and
            // enable expanding.
            Toolbar()->SetItemDimmed( EFsEmailUiTbCmdCollapseAll, ETrue, ETrue );
            Toolbar()->SetItemDimmed( EFsEmailUiTbCmdExpandAll, EFalse, ETrue );
            break;
            }
        case EFsTreeListExpandedAll:
            {
            // Everything has been expanded, so disable expanding and
            // enable collapsing.
            Toolbar()->SetItemDimmed( EFsEmailUiTbCmdCollapseAll, EFalse, ETrue );
            Toolbar()->SetItemDimmed( EFsEmailUiTbCmdExpandAll, ETrue, ETrue );
            break;
            }
        case EFsFocusVisibilityChange:
            {
            iAppUi.SetFocusVisibility( EFalse );
            break;
            }
        default:
            {
            // No need to handle other events.
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleMailBoxEventL
// Mailbox event handler, responds to events sent by the plugin.
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::HandleMailBoxEventL( TFSMailEvent aEvent,
    TFSMailMsgId aMailboxId, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    FUNC_LOG;

    // Handle the event
    if ( iMoveOrCopyInitiated &&
         iFirstStartCompleted &&
         aMailboxId == iAppUi.GetActiveMailboxId() &&
         iAppUi.CurrentActiveView() == this )
        {
		if ( aEvent == TFSEventMailboxOnline )
			{
            if( iWaitingToGoOnline )
                {
                PrepareFolderListL();
                PopulateFolderListL();

                if ( iModel->Count() )
                    {
                    SetFocusToLatestMovedFolderL();
                    }
                iTreeList->SetFocusedL( ETrue );
                iWaitingToGoOnline = EFalse;
                }
			}
   		else if ( aEvent == TFSEventMailboxOffline )
   			{
            // Empty the list when disconnected if connection is needed for
   			// the current operation
   			if( !iNoConnectNeeded )
   			    {
   	            PrepareFolderListL();
   			    }
			}
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::FocusVisibilityChange( TBool aVisible )
    {
    FUNC_LOG;
    CFsEmailUiViewBase::FocusVisibilityChange( aVisible );
    iTreeVisualizer->SetFocusVisibility( aVisible );
    }

// ---------------------------------------------------------------------------
// CbaButtonPressed
// Check if aPosition is on CBA touch area
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::CbaButtonPressed( TPoint aPosition )
    {
    TBool cbaPressed( EFalse );

    TRect leftCbaRect = TRect();
    TRect rightCbaRect = TRect();
    GetCbaRects( leftCbaRect, rightCbaRect );
    if ( leftCbaRect.Contains( aPosition ) ||
         rightCbaRect.Contains( aPosition ))
        {
        cbaPressed = ETrue;
        }

    return cbaPressed;
    }

// ---------------------------------------------------------------------------
// GetCbaRects
// Resolve CBA touch areas from layouts
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListVisualiser::GetCbaRects( TRect& aLeftCbaRect, TRect& aRightCbaRect )
    {
    TBool rightPaneActive( IsAreaSideRightPaneActive() );
    TBool bskLandscape( Layout_Meta_Data::IsLandscapeOrientation() &&
                        !rightPaneActive );
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    if ( rightPaneActive )
        {
        TAknWindowComponentLayout rightAreaLayout(
            AknLayoutScalable_Avkon::area_side_right_pane( 0 ) );

        TAknLayoutRect layoutRect;
        // Read right (top in landscape) softkey layout.
        layoutRect.LayoutRect(
                screen,
                TAknWindowComponentLayout::Compose(
                rightAreaLayout,
                AknLayoutScalable_Avkon::sctrl_sk_bottom_pane() ).LayoutLine() );
        TRect bottomSKRect( layoutRect.Rect() );

        // Read left (bottom in landscape) softkey layout.
        layoutRect.LayoutRect(
                screen,
                TAknWindowComponentLayout::Compose(
                rightAreaLayout,
                AknLayoutScalable_Avkon::sctrl_sk_top_pane() ).LayoutLine() );
        TRect topSKRect( layoutRect.Rect() );

        layoutRect.LayoutRect(
            bottomSKRect,
            AknLayoutScalable_Avkon::aid_touch_sctrl_bottom().LayoutLine() );
        aLeftCbaRect = layoutRect.Rect();

        layoutRect.LayoutRect(
            topSKRect,
            AknLayoutScalable_Avkon::aid_touch_sctrl_top().LayoutLine() );
        aRightCbaRect = layoutRect.Rect();
        }
    else
        {
        TAknWindowComponentLayout applicationWindow(
            AknLayoutScalable_Avkon::application_window( 0 ) );
        TAknLayoutRect cbarect;
        cbarect.LayoutRect(
            screen,
            TAknWindowComponentLayout::Compose(
                applicationWindow,
                TAknWindowComponentLayout::Compose(
                    AknLayoutScalable_Avkon::area_bottom_pane( bskLandscape ? 2 : 1 ),
                    AknLayoutScalable_Avkon::control_pane() ) ).LayoutLine() );
        aLeftCbaRect = cbarect.Rect();
        }
    }

// ---------------------------------------------------------------------------
// IsAreaSideRightPaneActive
// Checks if right side pane is active.
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListVisualiser::IsAreaSideRightPaneActive()
    {
    // Currently the widescreen status pane layout is in use only
    // when pen input (touch) is enabled.
    TBool result = EFalse;

    if ( Layout_Meta_Data::IsLandscapeOrientation() &&
         Layout_Meta_Data::IsPenEnabled() )
        {
        if ( iAvkonEnv->StatusPaneResIdForCurrentLayout(
                 AknStatuspaneUtils::CurrentStatusPaneLayoutResId() ) ==
                     R_AVKON_WIDESCREEN_PANE_LAYOUT_IDLE_FLAT_NO_SOFTKEYS )
            {
            result = EFalse;
            }
        else
            {
            result = ETrue;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// Only for testing
// ---------------------------------------------------------------------------
//
//void CFSEmailUiFolderListVisualiser::CancelNotifierTestL()
//	{
//	iNotifierTest->Cancel();
//	}

