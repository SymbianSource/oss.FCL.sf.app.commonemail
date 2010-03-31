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
* Description:  FreestyleEmailUi search result list visualisation
*
*/

#ifndef __FREESTYLEEMAILUI_SEARCHLISTVISUALISER_H__
#define __FREESTYLEEMAILUI_SEARCHLISTVISUALISER_H__

// SYSTEM INCLUDES
#include <e32base.h>

//<cmail>
#include "cfsmailcommon.h"
#include "mfsmailboxsearchobserver.h"
#include "fstreevisualizerobserver.h"
#include "fstreelistobserver.h"
#include "fsccontactactionmenudefines.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiContactHandlerObserver.h"


// FORWARD DECLARATIONS
class CAlfTextVisual;
class CAlfImageVisual;
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CFSEmailUiMailListModelItem;
class CFSEmailUiMailListModel;
class CFreestyleEmailUiTextureManager;
class CFSMailFolder;
class CFSMailClient;
class CFSMailMessage;
class CFsTreeList;
class CFsTreeVisualizerBase;
class MFsTreeItemData;
class MFsTreeItemVisualizer;
class CFreestyleEmailUiTextureManager;
class CFreestyleEmailUiAppUi;
class CFsTreePlainOneLineNodeData;
class CFsTreePlainOneLineNodeVisualizer;
class CPbkxContactActionMenu;
class CFreestyleEmailUiSearchListControl;
class CFSMailFolder;
class CFSMailBox;
class CAsyncCallBack;
class CESMRIcalViewer;
class CFsTreePlainTwoLineItemData;
class CAknStylusPopUpMenu;

struct SSearchListItem
	{
	TInt iSearchListItemId;
	MFsTreeItemData* iTreeItemData;
	MFsTreeItemVisualizer* iTreeItemVisualiser;
	};

class CFSEmailUiSearchListVisualiser : public CFsEmailUiViewBase,
									   public MFSMailBoxSearchObserver,
									   //<cmail> touch
									   public MFsTreeListObserver,
									   //<cmail>
									   public MFSEmailUiGenericTimerCallback,
									   public MFSEmailUiContactHandlerObserver
	{
public:
	static CFSEmailUiSearchListVisualiser* NewL(CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aSeacrhListControlGroup);
	static CFSEmailUiSearchListVisualiser* NewLC(CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aSearchListControlGroup);
    virtual ~CFSEmailUiSearchListVisualiser();
    void PrepareForExit();

public: // from CFsEmailUiViewBase
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    // From view
	TUid Id() const;
	// <cmail> Toolbar
	/*void DoActivateL(const TVwsViewId& aPrevViewId,
	                 TUid aCustomMessageId,
	                 const TDesC8& aCustomMessage);*/
    // </cmail> Toolbar
    void ChildDoDeactivate();
    void HandleCommandL( TInt aCommand );

    /**
	 * @see CFsEmailUiViewBase::SetStatusBarLayout
	 */
    void SetStatusBarLayout();

    // Dynamic variant switch, called by appui
    void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;

    // Navigation functions, used mainly from viewer
    TBool IsNextMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId, TFSMailMsgId& aFoundNextMsgFolder ) const;
    TBool IsPreviousMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId, TFSMailMsgId& aFoundPrevMsgFolder ) const;
    TInt MoveToNextMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId );
    TInt MoveToPreviousMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId );
    TInt MoveToPreviousMsgAfterDeleteL( TFSMailMsgId aFoundPreviousMsgId );

public: // from MFSMailBoxSearchObserver
    void MatchFoundL( CFSMailMessage* aMatchMessage );
    void SearchCompletedL();
    /**
     * server asks client if to change the search priority (when calling)
     */
    void ClientRequiredSearchPriority( TInt *apRequiredSearchPriority ); // <cmail>


public: // new methods
    // Refresh the whole list
	void RefreshL();

	// Mail list current model data
	CFSEmailUiMailListModel* Model();

	// Event handling forwarded from control
	TBool OfferEventL( const TAlfEvent& aEvent );

	// Used from callback
	TInt LaunchSearchDialogL();

	// Used from appui to get the current index in search list
	TInt HighlightedIndex() const;

	// Start timer callback
	void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );

	// Helper functions to get highlighted message id and folder id
	TFSMailMsgId HighlightedMessageFolderId();
  	TFSMailMsgId HighlightedMessageId();

    void HandleMailBoxEventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
        TAny* aParam1, TAny* aParam2, TAny* aParam3 );
	void RemoveMsgItemsFromListIfFoundL( const RArray<TFSMailMsgId>& aEntryIds );

public: // From MFsTreeListObserver
    //<cmail> touch
    void TreeListEventL( const TFsTreeListEvent aEvent,
                         const TFsTreeItemId aId,
                         const TPoint& aPoint );

	//</cmail>

public: // From MFSEMailUiContactHandlerObserver

    void OperationCompleteL( TContactHandlerCmd aCmd,
        const RPointerArray<CFSEmailUiClsItem>& aContacts );
    void OperationErrorL( TContactHandlerCmd, TInt aError );

// <cmail> Toolbar
private: // from

    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

// </cmail> Toolbar

    // Called when flip state is changed
    void FlipStateChangedL( TBool aFlipOpen );

    /**
     * Handles the focus state changes
     * @param aVisible Indicates if focus should become visible or removed
     */
    void FocusVisibilityChange( TBool aVisible );

private:
	CFSEmailUiSearchListVisualiser( CFreestyleEmailUiAppUi* aAppUi, CAlfEnv& aEnv, CAlfControlGroup& aSearchListControlGroup );
	void ConstructL();

  	void ReScaleUiL();
	void SetSearchListLayoutAnchors();
	void SetSearchListTopBarLayoutAnchors();

	void CreatePlainNodeL( const TDesC& aItemDataBuff,
                           CFsTreePlainOneLineNodeData* &aItemData,
                           CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer ) const;
	void ExpandAllNodesL();
	void CollapseAllNodesL();

	// Helper functions to access model data
	TFSMailMsgId MsgIdFromIndex( TInt aItemIdx ) const;
	TFSMailMsgId MsgIdFromListId( TFsTreeItemId aListId ) const;
	CFSMailMessage& MsgPtrFromListId( TFsTreeItemId aListId );

	// Item data and visualiser helper functions
	MFsTreeItemData* ItemDataFromItemId( TFsTreeItemId aItemId );
	MFsTreeItemVisualizer* ItemVisualiserFromItemId( TFsTreeItemId aItemId );

	// Helpers to get the ordinal of a message in the iModel
	TInt ItemIndexFromMessageId( const TFSMailMsgId& aMessageId ) const;
	TInt NextMessageIndex( TInt aCurMsgIdx ) const;
	TInt PreviousMessageIndex( TInt aCurMsgIdx ) const;

	//<cmail> touch
	// Helper class for resolving touch events and actions
	void DoHandleActionL();
    //</cmail>

	// Open highlighted mail
	void OpenHighlightedMailL();

	// Compose, reply, reply all and forward
	void ReplyL( CFSMailMessage* aMsgPtr );
	void ReplyAllL( CFSMailMessage* aMsgPtr );
	void ForwardL( CFSMailMessage* aMsgPtr );
	void DoReplyForwardL( TEditorLaunchMode aMode, CFSMailMessage* aMsgPtr = NULL );

	void StartSearchL();
	void StopSearchL();

	// <cmail> fixed CS high cat. finding
    void ResetResultListL();

	void ChangeReadStatusOfHighlightedL( TInt aRead );

	void CheckAndUpdateFocusedMessageL();

	void RemoveFocusedFromListL();

	void UpdateMsgIconAndBoldingL( CFSMailMessage* aMsgPtr );
	void UpdateMsgIconAndBoldingL( TInt aListIndex );

	// Delete messages
	void DeleteFocusedMessageL();

	TBool CallToSenderL();

	void UpdateMailListSettingsL();

	void UpdatePreviewPaneTextForItemL( CFsTreePlainTwoLineItemData* aItemData, CFSMailMessage* aMsgPtr );
	void FilterPreviewPaneTextL( TDes& aText ) const;

	void SetMskL();

    void DoFirstStartL();

    void SetHeaderAttributesL();

    void LaunchStylusPopupMenuL( const TPoint& aPoint );

private:
    // Pointer to Alf environment
	CAlfEnv* iEnv;

	// Data to be filled from custom message
	CFSMailBox* iMailBox;

	// Layouts
 	CAlfAnchorLayout* iScreenAnchorLayout;
 	CAlfAnchorLayout* iSearchTopBarComponentLayout;

  	// Top bar visuals and layout
	//CAlfImageVisual* iBarFindImage; <cmail> not in cmail
	CAlfTextVisual* iBarTextVisual;

    	// Mail List control and layout
  	CFreestyleEmailUiSearchListControl* iSearchListControl;
 	CAlfDeckLayout* iSearchListLayout;
    CFSEmailUiMailListModel* iModel;
  	CFsTreeList* iSearchList;
    CFsTreeVisualizerBase* iSearchTreeListVisualizer;
  	TInt iNumSlots;

	RArray<SSearchListItem> iSearchListItemArray;

	HBufC* iLatestSearchText;

	TBool iThisViewActive;

	TBool iSearchOngoing;

	CFSEmailUiGenericTimer* iStartupCallbackTimer;

	RPointerArray<TDesC> iSearchStrings;


	TBool iMsgDataCouldBeChanged;

	// Previous view uid
	TUid iPreviousViewUid;

    TInt iListMode;
    TInt iNodesInUse;

    TFSMailMsgId iLatestOpenedMrId;

    TInt iSearchCount;

	TAknUiZoom iCurrentZoomLevel;

	TBool iFirstStartCompleted;

	TBool iListAddedToControlGroup;  //<cmail>
	TBool iPreparedForExit; //<cmail>

    CAknStylusPopUpMenu* iStylusPopUpMenu;
    TBool iStylusPopUpMenuVisible;

	//prevents improper executing of Call application - when call to contact
	TBool iConsumeStdKeyYes_KeyUp; 
	// decreases search priority to enable search for contact when call to contact
	TInt iRequiredSearchPriority; 
  	};

#endif
