/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  FreestyleEmailUi double line list visualiser definition 
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_MAILLISTVISUALISER_H__
#define __FREESTYLEEMAILUI_MAILLISTVISUALISER_H__

// SYSTEM INCLUDES
#include <EIKMOBS.H>
//<cmail>
#include "fscontrolbarobserver.h"
#include "fstreelistobserver.h"
#include "CFSMailCommon.h"
#include "MFSMailEventObserver.h"
#include "fsccontactactionmenudefines.h"
//</cmail>
#include "FSEmailBuildFlags.h"

// INTERNAL INCLUDES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiListVisualiser.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiControlBarCallback.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiContactHandlerObserver.h"

// FORWARD DECLARATIONS
class CAlfTextVisual;
class CAlfGridLayout;
class CFSEmailUiList;
class CAlfImageVisual;
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CFSEmailUiMailListModelItem;
class CFSEmailUiMailListModel;
class CListModeChanger;
class CFreestyleEmailUiTextureManager;
class CFSMailFolder;
class CFSMailClient;
class CFSMailAddress;
class CFsTreeList;
class CFsTreeVisualizerBase;
class MFsTreeItemData;
class MFsTreeItemVisualizer;
class CFreestyleEmailUiTextureManager;
class CFreestyleEmailUiMailListControl;
class CFreestyleEmailUiAppUi;
class CFsControlBar;
class CFSMailMessage;
class CFsMailFolder;
class CFsTreePlainOneLineNodeData;
class CFsTreePlainOneLineNodeVisualizer;
class MFsControlButtonInterface;
class CPbkxContactActionMenu;
class CMailListUpdater;
class CMsgMovedNoteTimer;
class CIdle;
class CESMRIcalViewer;
class CAknWaitDialog;
class CDateChangeTimer;
class CFsTreePlainTwoLineItemData;
class CFsTreePlainTwoLineItemVisualizer;
//<cmail>
class CEUiEmailListTouchManager;
//</cmail>
class CAknStylusPopUpMenu;

struct SMailListItem
	{
	TFsTreeItemId iListItemId;
	MFsTreeItemData* iTreeItemData;
	MFsTreeItemVisualizer* iTreeItemVisualiser;
	};

struct STimeDateFormats
	{
	TDateFormat iDateFormat;	
	TChar iDateSeparator;
	TTimeFormat iTimeFormat;
	TChar iTimeSeparator;
	TLocalePos iAmPmPosition;
	};
	
enum TMailListUiControls
	{
	EControlBarComponent=0,
	EMailListComponent
	};

enum TListControlType
	{
	EListControlTypeSingleLinePreviewOff=0,
	EListControlTypeSingleLinePreviewOn,
	EListControlTypeDoubleLinePreviewOff,
	EListControlTypeDoubleLinePreviewOn
	};

enum TListControlSeparatorValue
	{
	EListControlSeparatorDisabled=0,
	EListControlSeparatorEnabled
	};

// <cmail>
class CFSEmailUiMailListVisualiser : public CFsEmailUiViewBase, 
									 public MFsControlBarObserver, 
									 public MFSEmailUiFolderListCallback, 
									 public MFSEmailUiSortListCallback,
									 public MFsTreeListObserver,
									 public MFSEmailUiContactHandlerObserver,
									 public MFsActionMenuPositionGiver
// </cmail>
	{
friend class CMailListUpdater;
public:
	static CFSEmailUiMailListVisualiser* NewL(CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aMailListControlGroup);
	static CFSEmailUiMailListVisualiser* NewLC(CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aMailListControlGroup);
    virtual ~CFSEmailUiMailListVisualiser();
    void PrepareForExit();

	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	// Refresh the whole list
	void RefreshL( TFSMailMsgId* aFocusToMessage = NULL );
	// From view 
	TUid Id() const;	
	// <cmail> Toolbar
	/*void DoActivateL(const TVwsViewId& aPrevViewId,
	                 TUid aCustomMessageId,
	                 const TDesC8& aCustomMessage);*/
    // </cmail> Toolbar
	TInt NewEmailsInModelL() const;
	TInt EmailsInModelL() const;	              
	CFSEmailUiMailListModel* Model();	
	// Current active folder
	TFSMailMsgId FolderId();
	// Dynamic variant switch, called by appui
	void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
	void HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType aType );
	// Handle foregroundevent
	void HandleForegroundEventL();
	// Sets softkeys
	void SetViewSoftkeysL( TInt aResourceId );	
	void SetMskL();
	// Highlighted index
	TInt HighlightedIndex() const;
	void SetHighlightedIndexL( TInt aWantedIndex );
	// Event handling forwarded from control
	TBool OfferEventL( const TAlfEvent& aEvent ); 

    // <cmail>
	/**
	 * Open highlighted mail or node. 
	 * (Touchwork: Called from touch manager)
	 */
	void DoHandleListItemOpenL();

	/**
     * Open folderlist or shortlist.  
     * (Touchwork: Called from touch manager)
     * 
     * @param TInt button id. (shortlist or folderlist) 
     */
	void DoHandleControlBarOpenL( TInt aControlBarButtonId );

    /**
     * Open action menu.   
     * (Touchwork: Called from touch manager)
     */
	void DoHandleListItemLongTapL();

    /**
     * Get control which is currently focused.    
     * (Touchwork: Called from touch manager)
     * 
     * @return TInt id of control
     */
	TInt GetFocusedControl() const; 

    /**
     * Set control bar as focused control.   
     * (Touchwork: Called from touch manager)
     */
	void SetControlBarFocusedL();

    /**
     * Set tree list as focused control.   
     * (Touchwork: Called from touch manager)
     */
	void SetTreeListFocusedL(); 

    /**
     * Helper function for disable controls to send 
     * any events.    
     * (Touchwork)
     * 
     * @param TBool state of controls. 
     */
	void DisableMailList( TBool aValue );
	// </cmail>
	
	// Command handling
	void HandleCommandL(TInt aCommand);
	// Read status 
	void ChangeReadStatusOfHighlightedL( TBool aRead );
	void ChangeReadStatusOfMarkedL( TBool aRead );
	void ChangeReadStatusOfIndexL( TBool aRead, TInt aIndex );
	// Delete messages 
	void DeleteMessagesL(); 	
	// Move to drafts
	void MoveMessagesToDraftsL();
	
	// From base class MFSEmailUiFolderListCallback
	// Handle folder list selection
	void FolderSelectedL( TFSMailMsgId aSelectedFolderId,
	 					 TFSEmailUiCtrlBarResponse aResponse );
	void MailboxSelectedL( TFSMailMsgId aSelectedFolderId );
	TRect FolderButtonRect();
	
	// From base class MFSEmailUiSortListCallback
	void SortOrderChangedL( TFSMailSortField aSortField,
	 					   TFSEmailUiCtrlBarResponse aResponse );
	TRect SortButtonRect();
	
	// Navigation functions, used mainly from viewer
	TBool IsNextMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId, TFSMailMsgId& aFoundNextMsgFolder ) const;
	TBool IsPreviousMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId, TFSMailMsgId& aFoundPrevMsgFolder ) const;
	TInt MoveToNextMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId );
	TInt MoveToPreviousMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId );

	/**
     * Sets the manual mailbox syncronisation flag if the user chose   
     * to synchorise mail box from the menu
     */
    void ManualMailBoxSync( TBool aManualMailBoxSync );
    
    // Returns ETrue if mailbox is syncronizing and EFalse if not
    TBool GetLatestSyncState();

	void GetMarkedMessagesL( RArray<TFSMailMsgId>& aMessageIDs ) const;
	CFsTreeList& GetMailList();
	// Get message ID corresponding given list ID. If the list ID is for a node, its first child message ID is returned.
	TFSMailMsgId MsgIdFromListId( TFsTreeItemId aListId ) const;

    void HandleMailBoxEventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
        TAny* aParam1, TAny* aParam2, TAny* aParam3 );

// from base class MFsTreeListObserver

    /** Handles tree list events. */
    // <cmail> Touch
    void TreeListEventL( const TFsTreeListEvent aEvent, const TFsTreeItemId aId );
    // </cmail>

// from base class MFSEmailUiContactHandlerObserver

    /** Handles completed contact handler event. */
    void OperationCompleteL( TContactHandlerCmd aCmd,
        const RPointerArray<CFSEmailUiClsItem>& aContacts );
    void OperationErrorL( TContactHandlerCmd, TInt aError );

    /**
    * Called when CDateChangeTimer completes. This happens when either when date
    * changes or when user alters the system time. Redraws the list to ensure that
    * time stamp texts in emails and nodes are up-to-date.
    */ 
    void NotifyDateChangedL();
// <cmail>
    TPoint ActionMenuPosition();
// </cmail>
  
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
    void ChildDoDeactivate();
    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;
    /**
	 * @see CFsEmailUiViewBase::SetStatusBarLayout
	 */ 
    void SetStatusBarLayout();
    
private: // Private functions
    CFSEmailUiMailListVisualiser( CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi,
        CAlfControlGroup& aMailListControlGroup );
    void ConstructL();

	// Refresh funcions
    void InsertNewMessagesL( const RArray<TFSMailMsgId>& aMessages );
    void InsertNewMessageL( CFSMailMessage* aNewMessage, const TBool aAllowRefresh ); // ownership is transferred
    
    void RefreshListItemsL();
    // Refresh the list order only
    void RefreshOrderL();
    
    // To start refreshing asynchronously
    void RefreshDeferred( TFSMailMsgId* aFocusToMessage = NULL );
    static TInt DoRefresh( TAny* aSelfPtr );
    void CompletePendingRefresh();
    
    // Create and insert one list node item according to given model item.
    // Omitting the argument aChildIdx causes the new node to be appended at the end of the list.
    TFsTreeItemId InsertNodeItemL( TInt aModelIndex, TInt aChildIndex = KErrNotFound, const TBool aAllowRefresh = EFalse );
    // Create and insert one list item according the given model item. The item is added under the given node.
    // Omitting the argument aChildIdx causes the new item to be appended as last child of the node.
    TFsTreeItemId InsertListItemL( TInt aModelIndex, TFsTreeItemId aParentNodeId, TInt aChildIdx = KErrNotFound, TBool aAllowRefresh = ETrue );
	
	// Mail model update
	void UpdateMailListModelL();
	void CreateModelItemsL( RPointerArray<CFSMailMessage>& aMessages );
	
	// Create title divider model item for the given message. Separator text depends on active sorting mode.
	CFSEmailUiMailListModelItem* CreateSeparatorModelItemLC( CFSMailMessage& aMessage ) const;
	
	// Checks if the given messages belong under the same title divider. This depends on the currect sorting mode.
	TBool MessagesBelongUnderSameSeparatorL( const CFSMailMessage& aMessage1, const CFSMailMessage& aMessage2 ) const;
	
	// Rescale UI	
	void SetMailListLayoutAnchors();

	// Control bar functions
	void CreateControlBarLayoutL();
	void ScaleControlBarL();
	void SetSortButtonTextAndIconL();
    void HandleControlBarEvent( TFsControlBarEvent aEvent, TInt aData );

	// Followup flag
	void SetMessageFollowupFlagL();

	void CreatePlainNodeL( const TDesC* aItemDataBuff,
                           CFsTreePlainOneLineNodeData* &aItemData,   
                           CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer ) const;                           	

	// Move messages to folder
	TInt MoveMsgsToFolderL( const TFSMailMsgId& aDestinationFolderId );

	// Collapse/Expand nodes
	void ExpandAllNodesL();
	void CollapseAllNodesL();
	void ExpandOrCollapseL();

	// Mark control functions
	void MarkAllItemsL();
	void UnmarkAllItemsL();
	TInt CountMarkedItemsL();
	
	void MarkMessagesIfFoundL( const RArray<TFSMailMsgId>& aMessageIDs );

	// Message deletion internal functions
	void DeleteFocusedMessageL();
	void DeleteMarkedMessagesL();
	static TInt DoDeleteMarkedMessages( TAny* aSelfPtr );

	// Helper functions to access model data
	TFSMailMsgId MsgIdFromIndex( TInt aItemIdx ) const;
	CFSMailMessage& MsgPtrFromListIdL( TFsTreeItemId aListId );	
    const CFSMailMessage& MsgPtrFromListIdL( TFsTreeItemId aListId ) const; 

	// Item data and visualiser helper functions 
	MFsTreeItemData* ItemDataFromItemId( TFsTreeItemId aItemId );										
	MFsTreeItemVisualizer* ItemVisualiserFromItemId( TFsTreeItemId aItemId );
	
	// Helpers to get the ordinal of a message in the iModel
	TInt ItemIndexFromMessageId( const TFSMailMsgId& aMessageId ) const;
	TInt NextMessageIndex( TInt aCurMsgIdx ) const;
	TInt PreviousMessageIndex( TInt aCurMsgIdx ) const;
	TInt ModelIndexFromListId( TFsTreeItemId aItemId ) const;
	
	// Open highlighted mail
	void OpenHighlightedMailL();
	void OpenMailItemL( TFsTreeItemId aMailItem );

	// Create folder name buffer, either from database or from actual folder name
	HBufC* CreateFolderNameLC( const CFSMailFolder* aFolder ) const;

	// Shortcut specific toggles
	void ShortcutReadUnreadToggleL();
	void ShortcutCollapseExpandAllToggleL();
	
	// Utility functions to check if all nodes are expanded or collapsed
	TBool AllNodesCollapsed() const;
	TBool AllNodesExpanded() const;
	
	// Utility functions to check if mark as read/unread options should be available in menus
	TBool IsMarkAsReadAvailableL() const;
	TBool IsMarkAsUnreadAvailableL() const;
	
	// Utility function to get list of entries which will be targeted by the Actions menu commands.
	// The list contains either marked entries or the focused message entry or is empty.
	void GetActionsTargetEntriesL( RFsTreeItemIdList& aListItems ) const;
	
	// Action menu specific functions
	void LaunchActionMenuL();
	void HandleActionMenuCommandL( TActionMenuCustomItemId itemId );
	void LaunchStylusPopupMenuL();
	
	// Compose, reply, reply all and forward
	void CreateNewMsgL();
	void ReplyL( CFSMailMessage* aMsgPtr );
	void ReplyAllL( CFSMailMessage* aMsgPtr );
	void ForwardL( CFSMailMessage* aMsgPtr );
	void DoReplyForwardL( TEditorLaunchMode aMode, CFSMailMessage* aMsgPtr = NULL );
	
	// Some misc housekeeping functions
	void RemoveUnnecessaryNodesL();
	void UpdateMailListSettingsL();	
	void UpdateMailListTimeDateSettings();
	STimeDateFormats MailListTimeDateSettings();

	// Request to update list item data from given message pointer
    void UpdateItemDataL( CFsTreePlainTwoLineItemData* aData, const CFSMailMessage* aMsgPtr );
	
	// Request to update preview pane data for list id with msg ptr
	void UpdatePreviewPaneTextForItemL( TFsTreeItemId aListItemId, CFSMailMessage* aMsgPtr );
	void UpdatePreviewPaneTextForItemL( CFsTreePlainTwoLineItemData* aTwoLineItemData, CFSMailMessage* aMsgPtr );
    void UpdatePreviewPaneTextIfNecessaryL( TFsTreeItemId aListItemId, TBool aUpdateItem = ETrue );
	
	// Sender/Recipient text depending on the mail folder
	HBufC* GetListFirstTextLineL( const CFSMailMessage* aMsgPtr );

	// Reload message pointer from mail client and update list item contents to match it. Item is
	// removed if it isn't valid anymore.
	void UpdateItemAtIndexL( TInt aIndex );
	
	// Check validity of highlighted msg and remove from list if needed
	// EFalse indicates that mail was not valid anymore.
	TBool CheckValidityOfHighlightedMsgL();

	// Icon update for msg item
	void UpdateMsgIconAndBoldingL( CFSMailMessage* aMsgPtr );
	void UpdateMsgIconAndBoldingL( TInt aListIndex, TBool aRefreshItem = ETrue );
	void UpdateMsgIconAndBoldingL( CFsTreePlainTwoLineItemData* aItemData,
	                               CFsTreePlainTwoLineItemVisualizer* aItemVis,
	                               CFSMailMessage* aMsgPtr );

	// Message removing from list if found
	void RemoveMsgItemsFromListIfFoundL( const RArray<TFSMailMsgId>& aEntryIds );

	// Branding functions
	void SetBrandedMailBoxIconL();
	void SetMailboxNameToStatusPaneL();
	void SetBrandedListWatermarkL();

    // Gives the parent node if the item has one. Returns KErrNotFound if no parent node available.
    TFsTreeItemId ParentNode( TFsTreeItemId aItemId ) const;
    
    // Either marks or unmarks the current item depending on the current marking state
    // as stored in iListMarkItemsState. Used for marking when shift is depressed while
    // scrolling. Returns ETrue if the highlighted item is markable.
    TBool DoScrollMarkUnmarkL();
    
    /**
     * Mark/unmark items under current separator.
     * 
     * @param aMarked If ETrue mark all items else unmark.
     * @param aSeparatorId Id of the separator.
     */
    void MarkItemsUnderSeparatorL( TBool aMarked, TInt aSeparatorId );
    
    //When maillist is activated this method handles the connection icon change
    void ConnectionIconHandling();
    
    void SetListAndCtrlBarFocusL();
    
    void SetMailListItemsExtendedL();
    
    void DoFirstStartL();

    // Called when flip state is changed
    void FlipStateChangedL( TBool aFlipOpen );

    /**
     * Handles the focus state changes
     * @param aShow Indicates if focus should become visible or removed
     */
    void HandleTimerFocusStateChange( TBool aShow );

private: // data types

    
private: // Private objects
    // Reference to Alf environment
    CAlfEnv& iEnv;
  	// Mail list updater timer
	// Timer delays mail update to prevent unnesessary redraw events
	CMailListUpdater* iMailListUpdater; 
	// Msg moved note timer
	CMsgMovedNoteTimer* iMsgNoteTimer; 
  	// Screen layout uses iMailListControl
	CAlfAnchorLayout* iScreenAnchorLayout;
  	// Control bar control and layout and store visual pointer
	CFsControlBar* iControlBarControl;
	CAlfVisual* iControlBarVisual;
 	CAlfDeckLayout* iControlBarLayout;  
  	// Mail List control and layout
    CFreestyleEmailUiMailListControl* iMailListControl;
 	CAlfDeckLayout* iListLayout;
// <cmail> this feature is waiting for title divider bg graphics
    CAlfImageVisual* iTitleDividerBgVisual;
// </cmail>
    CFSEmailUiMailListModel* iModel; 
  	CFsTreeList* iMailList;
    CFsTreeVisualizerBase* iMailTreeListVisualizer;
  	TInt iNumSlots;		
   	CFSMailFolder* iMailFolder;
    TInt iNewEmailsInModel; 	
	RArray<SMailListItem> iTreeItemArray;   
    RArray<TInt> iButtonsIds;
    TInt iFocusedControl;
    TBool iToolbarInUse;
    TInt iListMode;    
    TInt iNodesInUse;
    TInt iNewEmailButtonId;
	TInt iFolderListButtonId;
	TInt iSortButtonId;
	TInt iImageButtonId;
	MFsControlButtonInterface* iIconButton;
	MFsControlButtonInterface* iNewEmailButton;
	MFsControlButtonInterface* iFolderListButton;
	MFsControlButtonInterface* iSortButton;	
	TBool iThisViewActive;	
	CPbkxContactActionMenu* iActionMenu;	
    TFSMailSortCriteria iCurrentSortCriteria;
	// Set to indicate whether items should be marked or unmarked
	TBool iListMarkItemsState;
	// Set to indicate if Shift key is being held down in which case selection key has mark/unmark functionality
	TBool iShiftDepressed;
	// Set to indicate has other key events occured during hash key press (in which case hash is used as shift and not handled individually)
	TBool iOtherKeyPressedWhileShiftDepressed;
	// Move to folder toggle
	TBool iMoveToFolderOngoing;
	// Timers
	CFSEmailUiGenericTimer* iMsgMoveTimer;
	//ETrue if user started the sync
	TBool iManualMailBoxSync;
	TAknUiZoom iCurrentZoomLevel;
	STimeDateFormats iDateFormats;
	TRect iCurrentClientRect;
	CAlfTexture* iMailBoxIconTexture;
	CAlfTexture* iMailBoxWatermarkTexture;
	TBool iSkinChanged;
	TBool iDateChanged;
	TFSMailMsgId iLatestOpenedMrId;
	TBool iListOrderMayBeOutOfDate;
	CAknWaitDialog* iDeletingWaitNote;
	CDateChangeTimer* iDateChangeTimer;
	TBool iFirstStartCompleted;
	// generic use async callback object
	CAsyncCallBack* iAsyncCallback; 
	// async callback for deferred redrawing
	CAsyncCallBack* iAsyncRedrawer;
	TFSMailMsgId iMsgToFocusAfterRedraw;

    //<cmail>
    CEUiEmailListTouchManager* iTouchManager; 
    //</cmail>
    CAknStylusPopUpMenu* iStylusPopUpMenu; 
    
    TBool iShowReplyAll;
  	};


// Definition of the mail updater timer. This timer is used for
// delayed drawing of the mail list to avoid viewsrv:11 panics.
class CMailListUpdater : public CTimer
    {
public:
    static CMailListUpdater* NewL( CFSEmailUiMailListVisualiser* aMailListVisualiser );
    ~CMailListUpdater();
    void StartL( TBool aIsSorting = EFalse );
    void Stop();
    void RunL();

protected:
    CMailListUpdater( CFSEmailUiMailListVisualiser* aMailListVisualiser );

private:
    CFSEmailUiMailListVisualiser* iMailListVisualiser;
    TBool iIsSorting;
    CAknWaitDialog* iWaitNote;
    };

// Delayd note timer 
class CMsgMovedNoteTimer : public CTimer
    {
public:
    static CMsgMovedNoteTimer* NewL( CFreestyleEmailUiAppUi* aAppUi, CFSEmailUiMailListVisualiser* aMailListVisualiser );
    ~CMsgMovedNoteTimer();
    void Start( TInt aMsgCount, const TFSMailMsgId aDestinationFolderId );
    void Stop();
    void RunL();

protected:
    CMsgMovedNoteTimer( CFreestyleEmailUiAppUi* aAppUi, CFSEmailUiMailListVisualiser* aMailListVisualiser );

private:
    CFreestyleEmailUiAppUi* iAppUi;
    CFSEmailUiMailListVisualiser* iMailListVisualiser;
    TFSMailMsgId iDestinationFolderId;
    TInt iMsgCount;
    };

// Date changing timer. Updates the mail list when date changes.
class CDateChangeTimer : public CTimer
    {
public:
    static CDateChangeTimer* NewL( CFSEmailUiMailListVisualiser& aMailListVisualiser );
    ~CDateChangeTimer();
    void Start();
    void RunL();
    
protected:
    CDateChangeTimer( CFSEmailUiMailListVisualiser& aMailListVisualiser );
    
private:
    CFSEmailUiMailListVisualiser& iMailListVisualiser;
    };

#endif 
