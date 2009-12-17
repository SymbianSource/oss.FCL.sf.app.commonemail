/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  FreestyleEmailUi folder list visualiser definition
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_FOLDERLISTVISUALISER_H__
#define __FREESTYLEEMAILUI_FOLDERLISTVISUALISER_H__

    
#include <e32base.h>

//<cmail>
#include "fstreelistconstants.h"
//</cmail>

#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiControlBarCallback.h"
#include "fstreelistobserver.h"
#include <alf/alfanchorlayout.h>
#include <alf/alfimagebrush.h>
// forward declarations
class CAlfTextVisual;
class CAlfGridLayout;
class CAlfImageVisual;
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CFSEmailUiFolderListControl;
class CFSEmailUiFolderListModel;
class CAlfBorderBrush;
class CFSMailMessage;
class CScrollerTimer;
class CMessageChangerTimer;
class CFreestyleEmailUiTextureManager;
class CFreestyleEmailUiAppUi;

class CFsTreeVisualizerBase;
class CFsTreeList;

class MFsTreeItemVisualizer;
class CFsTreePlainOneLineItemData;
class CFsTreePlainOneLineNodeData;
class CFsTreePlainOneLineItemVisualizer;
class CFsTreePlainOneLineNodeVisualizer;
class MFsControlButtonInterface;

class TFSMailMsgId;
class CFSMailClient;
class CFSMailBox;
class CFSMailFolder;

class CAlfFrameBrush;

//REMOVE WHEN DONE TESTING
//class CSimpleAOTest;
// <cmail> Touch
class CFSEmailUiFolderListVisualiser : public CFsEmailUiViewBase, public MFsTreeListObserver
// </cmail>
	{
public: // Data types
    enum TExpandCollapseType
        {
        EFolderListExpand,
        EFolderListCollapse,
        EFolderListAutomatic
        };
    
public: // Construction and destruction
    /**
     * Destructor
     */
	~CFSEmailUiFolderListVisualiser();

    /**
     * Two phase constructors
     *
     * @param aEnv Alfred environmet
     * @param aControlGroup Alfred control group
     * @param aAppUi Application main UI class
     * @return Pointer to newly created object
     */
	static CFSEmailUiFolderListVisualiser* NewL( CAlfEnv& aEnv,
                                                 CAlfControlGroup& aControlGroup,
                                                 CFreestyleEmailUiAppUi& aAppUi );
	static CFSEmailUiFolderListVisualiser* NewLC( CAlfEnv& aEnv,
                                                  CAlfControlGroup& aControlGroup,
                                                  CFreestyleEmailUiAppUi& aAppUi );				

public: // From base class CAknView
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void HandleCommandL( TInt aCommand );
    TUid Id() const;
    // <cmail> Toolbar
    /*
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );*/            
    // </cmail> Toolbar
    void ChildDoDeactivate();                
    void DoTransitionEffect( TBool aDirectionOut );
    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;

public: // Own public functions
    
    virtual TBool HandleWsEventL( const TWsEvent& aEvent );
    
    // Handle foreground event (called by FreestyleEmailUiMailListVisualiser)
    void HandleForegroundEventL();

    /**
	 * Dynamic switch handling and background updating
	 */
	void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );

    /**
     * Event handling forwarded from control
     *
     * @param aEvent Event forwarded from control
     * @return ETrue if event was consumed, otherwise EFalse
     */
	TBool OfferEventL( const TAlfEvent& aEvent );
	
	// <cmail> Touch
    /**
     * Handling pointer events. 
     *
     * @param aEvent Event forwarded from control
     * @return ETrue if event was consumed, otherwise EFalse
     */
	TBool HandlePointerEventL(const TAlfEvent& aEvent);
	// </cmail>
	
	/**
	 * Expands or collpases the currently highlighted folder
	 */
	void ExpandOrCollapseL( const TExpandCollapseType& aType );

    /**
	 * Tells wheter the folder list is shown in full screen or in popup
	 *
     * @return ETrue if list is shown in fullscreen, otherwise EFalse
	 */
	TBool IsFullScreen() const;

    /**
	 * Show folder list in control bar popup
	 *
     * @param aFolderId Folder id of the currently visible folder in mail list
     * @param aCallback Call back to inform the result of the folder query
     * @param aButton Control bar button from where the popup is opened
   	 */
    void ShowInPopupL( const TFSMailMsgId aFolderId,
                       MFSEmailUiFolderListCallback* aCallback,
    				   MFsControlButtonInterface* aButton = NULL );
	
    /**
	 * Quick and dirty solution to show the sort list with the same list.
	 * Could be changed to some more elegant implementation (e.g. folder list
	 * and sort list derive from some generic control base class that
	 * implements the common operations), if time permits.
	 *
	 * @param aCurrentSortOrder Current sort order in mail list
     * @param aFolderType Folder type of the currently visible folder in mail list
     * @param aCallback Call back to inform the result of the sort order query
     * @param aButton Control bar button from where the popup is opened
	 */
    void ShowSortListPopupL( const TFSMailSortField aCurrentSortOrder,
                             const TFSFolderType aFolderType,
    						 MFSEmailUiSortListCallback* aCallback,
    						 MFsControlButtonInterface* aButton  = NULL );
    
    /**
     * Does a horizontal scroll for the list if needed
     *
     * @param aForceRecalculation If ETrue, recalculates the list rect always.
     *                            If EFalse, recalculates the list rect only
     *                            if list level has changed.
     */
    void DoHorizontalScrollL( TBool aForceRecalculation = EFalse );

    /**
     * Function to check whether folder list is displaying popup
     */ 
    TBool IsPopupShown();

    /**
     * Function to hide popup
     */ 
    void HidePopupL();
    
    /**
     * From MFsTreeListObserver
     */
// <cmail> Touch
    void TreeListEventL( const TFsTreeListEvent aEvent, 
                                    const TFsTreeItemId aId = KFsTreeNoneID );
// </cmail>

// <cmail> Receiving online/offline events
    void HandleMailBoxEventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
        TAny* aParam1, TAny* aParam2, TAny* aParam3 );
// </cmail>
// <cmail> Toolbar    

private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
    
    /**
     *  @see CFsEmailUiViewBase::OfferToolbarEventL
     */
    void OfferToolbarEventL( TInt aCommand );

    /**
     * @see CFsEmailUiViewBase::ToolbarResourceId
     */
    TInt ToolbarResourceId() const;
   
// </cmail> Toolbar    
    
private: // Private functions

    /**
     * Comparison function for TLinearOrder
     *
     * @param aFirst First folder to compare
     * @param aSecond Second folder to compare
     * @return Zero if parameters are equal,
     *         negative if first parameter is less than second,
     *         positive if first parameter is greater than second
     */
    static TInt CompareFolders( const CFSMailFolder& aFirst, const CFSMailFolder& aSecond );
       
	// Construction
	CFSEmailUiFolderListVisualiser( CAlfEnv& aEnv, CFreestyleEmailUiAppUi &aAppUi, CAlfControlGroup& aControlGroup );
	void ConstructL();
	void LoadIconsL();

	// Internal functions used when showing and hiding the list
	// <cmail>
	//void DoShowInPopupL();
	void DoShowInPopupL( MFsControlButtonInterface* aButton,
	        MFSEmailUiFolderListCallback* aFolderCallback,
	        MFSEmailUiSortListCallback* aSortCallback );
	// </cmail>
	void PrepareFolderListL();
    void PopulateFolderListL();
    void PopulateFolderListDeferred();
    static TInt DoPopulateFolderList( TAny* aSelfPtr );

	// Recreate list content
	void UpdateFolderListL();
	void UpdateSortListL();
	
	// View layout handling and item position calculation
	void UpdateListSizeAttributes();
	void ResizeListIcons();
	void ResizeListItemsL();
	void AdaptScreenRectToListContent();
	// <cmail>
	void AdjustWidthByContent( TRect& aRect ) const;
	// </cmail>
	void SetAnchors();
	void SetItemVisualizerPropertiesL( MFsTreeItemVisualizer* aItemVisualizer );

	// Popup list position calcualtion
	TRect LeftListRectInThisResolution();
	TRect RightListRectInThisResolution();
	TRect FolderListRectInThisResolution();
	TRect SortListRectInThisResolution();
	// <cmail>
	//void PopupListWidthHeightInThisResolution( TInt& aWidth, TInt& aHeight );
	// </cmail>

	// Handle the selection or cancelation of the list
	void HandleSelectionL( TFSEmailUiCtrlBarResponse aSelection );
	TBool HandleCallbackL();

	// Set header text
	void SetHeaderTextByResourceIdL( TInt aResourceId );

	// Append folder list data
	void AppendActiveMailboxFoldersL();
	void AppendMoreFoldersL();
	void AppendMailboxesL();
	void AppendSeparatorLineL();
	
	// Append sort list data
	void AppendSortListItemsL();

	// Helper functions used to append different kind of items to list
	void AppendSubfoldersL( TFSMailMsgId aFolder,
	                        TFsTreeItemId aParentNode,
	                        TBool aRefreshLastItem );
	TFsTreeItemId AppendNodeFolderL( CFSMailFolder* aFolder, TFsTreeItemId aParentNode );
	TFsTreeItemId AppendLeafFolderL( CFSMailFolder* aFolder, TFsTreeItemId aParentNode, TBool aAllowRefresh );
	
	TFsTreeItemId AppendNodeToListFromResourceL( TInt aResourceId,
												 TFsTreeItemId aParentNode,
												 CAlfTexture* aIcon,
												 TUint aUnreadCnt = 0);
	
	TFsTreeItemId AppendNodeToListL( TDesC* aItemData,
									 TFsTreeItemId aParentNode,
									 CAlfTexture* aIcon,
									 TUint aUnreadCnt);

	TFsTreeItemId AppendItemToListFromResourceL( TInt aResourceId, 
												 TFsTreeItemId aParentNode,
												 CAlfTexture* aIcon,
                                                 TBool aAllowRefresh,
                                                 TUint aUnreadCnt = 0);
	
	TFsTreeItemId AppendItemToListL( TDesC* aItemData, 
									 TFsTreeItemId aParentNode,
									 CAlfTexture* aIcon,
                                     TBool aAllowRefresh,
                                     TUint aUnreadCnt = 0);
														
	void CreatePlainItemLC2( const TDesC* aItemDataBuff,
                             CFsTreePlainOneLineItemData* &aItemData,
                             CFsTreePlainOneLineItemVisualizer* &aItemVisualizer,
                             CAlfTexture* aIcon );
	void CreatePlainNodeLC2( TDesC* aItemDataBuff,
                             CFsTreePlainOneLineNodeData* &aItemData,
                             CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer,
                             CAlfTexture* aIcon );

	TBool GetStandardFolderResouceIdAndIconL( const TInt aFolderType, TInt &aResourceId, CAlfTexture* &aIcon ) const;
	TBool ShowThisTypeOfFolderL( CFSMailFolder* folder ) const;

	// Hepler function for folder comparison, return internal folder priority
	// used when sorting the list
    static TInt GetFolderPriorityByType( const CFSMailFolder& aFolder );

	// Set and clear soft keys when popup list shown
	void SetPopupSoftkeysL();
	void ClearPopupSoftkeys();

	// Set initial list focus by current/previous activity
    void SetFocusToCurrentSortItemL();
    void SetFocusToCurrentFolderL();
	void SetFocusToLatestMovedFolderL();

	// Move list focus to first or to last item
	void GoToTopL();
	void GoToBottomL();

    // Set toolbar buttons' dimmed/undimmed statuses.
    void UpdateToolbarButtons();

	// Check wheter all list nodes are expanded or collapsed
	TBool AllNodesCollapsed() const;
	TBool AllNodesExpanded( TFsTreeItemId aParentNodeId = KFsTreeRootID ) const;
	
	// Recursive function to get item's root level parent
	TFsTreeItemId GetRootParent( const TFsTreeItemId aItemId ) const;
	
    void DoFirstStartL();
    //<cmail>
    void SetHeaderTextAttributesL();
    void ConnectToMailboxL();
    void DisconnectFromMailboxL();
    TBool CbaButtonPressed( TPoint aPosition );
    void GetCbaRects( TRect& aLeftCbaRect, TRect&  aRightCbaRect );
    TBool IsAreaSideRightPaneActive();
    //</cmail>
	
private:
    
    // Array of list item infos
    RPointerArray< CFsTreePlainOneLineItemData > iListItemDatas;
    
private: // Typedefs and member variables
	// Array type for the list icons
	typedef RPointerArray<CAlfTexture> TFsEmailFolderListIconArray;

	// ALF environment
    CAlfEnv& iEnv;

	// View layouts
    CAlfDeckLayout* iFaderLayout;
    CAlfAnchorLayout* iParentLayout;
    CAlfLayout* iListLayout;
	CAlfLayout* iHeaderLayout;

	// ALF visuals etc. for list background and header
	CAlfImageVisual* iListBgImageVisual;
	CAlfImageVisual* iHeaderImageVisual;
    CAlfTextVisual* iHeaderTextVisual;
	// <cmail>
    //CAlfImageBrush* iBackgroundBrush;
    CAlfFrameBrush* iBackgroundBrush;
	// </cmail>
	
// <cmail> Touch
	// ALF visual for background
	CAlfVisual* iFaderVisual;
// </cmail>

	// Pointers to control and model
    CFSEmailUiFolderListControl* iControl;	// not owned (owned by ALF env)
    CFSEmailUiFolderListModel* iModel;		// owned

	// Callback pointers
	MFSEmailUiFolderListCallback* iCallback;
	MFSEmailUiSortListCallback* iSortListCallback;
	
	// Type of the currently visible folder in mail list
	TFSFolderType iCurrentFolderType;
	
	// Custom message ID passed in view activation
	TUid iCustomMessageId;
	
	// List/view size attributes
   	TRect iScreenRect;
	TRect iCtrlButtonRect;
    TRect iPreviousListRect;
	TInt iListItemHeight;
	TInt iListHeaderHeight;
    TInt iListSeparatorHeight;
	TSize iListIconSize;
	TSize iPreviousListIconSize;
	
	TPoint iPadding;
	
	// ETrue if list shown in fullscreen
	TBool iFullScreen;
	
	// List id of the "More folders" item in folder list
	TFsTreeItemId iMoreFoldersItemId;

    // Pointer to the tree list component visualizer.
    CFsTreeVisualizerBase* iTreeVisualizer;

    // Pointer to the tree list component, owned
    CFsTreeList* iTreeList;

	// Mailbox that is currently active in mail list
 	CFSMailBox* iActiveMailbox;

	// Folder list icons, items not owned (are owned by texture manager)
	TFsEmailFolderListIconArray iIconArray;
	
	// Previously selected items
	TFSMailMsgId iLatestSelectedMoveFolderId;
    TFSMailMsgId iCurrentFolderId;
	TFSMailSortField iCurrentSortOrder;
	
	// Pointers to list items (and nodes)
	RPointerArray<MFsTreeItemVisualizer> iListItemVisulizers;
	
    // Previous level in list, used in horizontal scrolling
	TInt iPreviousListLevel;
	
	TBool iFirstStartCompleted;
	
	TBool iPopupListShown;

	//For header text caption
  	TBool iTitleCaptionVisible;

  	// Flag to see if view opened with move/copy command
  	TBool iMoveOrCopyInitiated;
  	// Flag to see if connection was started
  	TBool iWaitingToGoOnline;
    // Do we need to connect the mailbox before showing the folder list
    TBool iNoConnectNeeded;
	
	// Should we use expansion animation for the next transition effect
	TBool iAnimateNextActivation;
	
	CAsyncCallBack* iAsyncCallback;
	
	TBool iConsumeUntilNextUpEvent;
	};


#endif //__FREESTYLEEMAILUI_FOLDERLISTVISUALISER_H__
