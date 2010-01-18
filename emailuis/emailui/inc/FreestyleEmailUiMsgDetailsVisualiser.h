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
* Description:  FreestyleEmailUi message details visualiser definition
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_MSGDETAILSVISUALISER_H__
#define __FREESTYLEEMAILUI_MSGDETAILSVISUALISER_H__

    
#include <e32base.h>

//<cmail>
#include "fstreelistconstants.h"
#include "fstreelistobserver.h"
#include "fsccontactactionmenudefines.h"
//</cmail>

#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiContactHandlerObserver.h"

// forward declarations
class CFSEmailUiMsgDetailsControl;
class CFSEmailUiMsgDetailsModel;
class CFreestyleEmailUiAppUi;
class CFreestyleEmailUiTextureManager;
class CFSMailMessage;
class CFSMailAddress;

class MFsTreeItemVisualizer;
class CFsTreeVisualizerBase;
class CFsTreeList;
class CFsTreePlainOneLineNodeData;
class CFsTreePlainOneLineItemData;
class CFsTreePlainTwoLineItemData;
class CFsTreePlainOneLineNodeVisualizer;
class CFsTreePlainOneLineItemVisualizer;
class CFsTreePlainTwoLineItemVisualizer;

// <cmail> Touch
class CFSEmailUiMsgDetailsVisualiser : public CFsEmailUiViewBase,
									   public MFSEmailUiContactHandlerObserver,
									   public MFsTreeListObserver,
									   public MFsActionMenuPositionGiver
// </cmail>
	{ 

public: // Construction and destruction
    /**
     * Destructor
     */
	~CFSEmailUiMsgDetailsVisualiser();
    void PrepareForExit();
    
    /**
     * Two phase constructors
     *
     * @param aEnv Alfred environmet
     * @param aControlGroup  control group
     * @param aAppUi Application main UI class
     * @return Pointer to newly created object
     */
	static CFSEmailUiMsgDetailsVisualiser* NewL( CAlfEnv& aEnv,
                                                 CAlfControlGroup& aControlGroup,
                                                 CFreestyleEmailUiAppUi& aAppUi );
	static CFSEmailUiMsgDetailsVisualiser* NewLC( CAlfEnv& aEnv,
                                                  CAlfControlGroup& aControlGroup,
                                                  CFreestyleEmailUiAppUi& aAppUi );
public: // From base class CAknView
	void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

	void HandleCommandL( TInt aCommand );

    TUid Id() const;			   

public: // Own public member functions

    /**
     * Dynamic switch handling
     */
	void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
	void SetMskL();
    /**
     * Event handling forwarded from control
     *
     * @param aEvent Event forwarded from control
     * @return ETrue if event was consumed, otherwise EFalse
     */
	TBool OfferEventL( const TAlfEvent& aEvent ); 

public: // From MFSEmailUiContactHandlerObserver

    void OperationCompleteL( TContactHandlerCmd aCmd,
        const RPointerArray<CFSEmailUiClsItem>& aContacts );
    void OperationErrorL( TContactHandlerCmd, TInt aError );

// <cmail> Touch
public: // From MFsTreeListObserver
    
    void TreeListEventL( const TFsTreeListEvent aEvent, 
                         const TFsTreeItemId aId,
                         const TPoint& aPoint );
    
    // From MFsActionMenuPositionGiver
    TPoint ActionMenuPosition();
// </cmail>

// <cmail> Toolbar    
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );  
// </cmail> Toolbar  
    void ChildDoDeactivate();                
    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;

private:
    /**
     * Constructors
     */
	CFSEmailUiMsgDetailsVisualiser( CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aControlGroup );
	void ConstructL();

    /**
     * Sets title pane text
     *
     * @param aViewStarted ETrue when starting view (title set to "Message/Meeting details")
     *                     EFalse when closing view (title set back to previous)
     */
	void ChangeTitleBarTextL( TBool aViewStarted );
	
    /**
     * Delete contents from list and model
     */
	void ClearMsgDetailsModelL();

	/**
     * Update contents to list and model
     */
	void UpdateMsgDetailsModelL();

    /**
     * Updates variables that specify the list size
     */
	void UpdateListSizeAttributes();

    /**
     * Append specified email fields to list
     */
    void AppendFromLinesL();
    void AppendSubjectLinesL(); 
   	void AppendToLinesL();
    void AppendCcLinesL();
	void AppendBccLinesL();
    void AppendSizeLinesL(); 
    void AppendSentLinesL();
   	void AppendPriorityLinesL();
    void AppendMessageTypeLinesL();

    /**
     * Actual creation of the list items and nodes
     *
     * @param aItemDataBuff List item data
     * @param aItemData Pointer to item data object
     * @param aItemVisualizer/aNodeVisualizer Pointer to item/node visualizer
     */
	void CreateOneLinePlainItemLC2( const TDesC& aItemDataBuff,
	                       		    CFsTreePlainOneLineItemData* &aItemData,
                           		    CFsTreePlainOneLineItemVisualizer* &aItemVisualizer );
	void CreateTwoLinePlainItemLC2( const TDesC& aPrimaryDataBuff,
								    const TDesC& aSecondaryDataBuff,
								    CFsTreePlainTwoLineItemData* &aItemData,
								    CFsTreePlainTwoLineItemVisualizer* &aItemVisualizer );
	void SetItemVisualizerCommonProperties( MFsTreeItemVisualizer& aItemVisualizer );

	void CreatePlainNodeLC2( const TDesC& aItemDataBuff,
                             CFsTreePlainOneLineNodeData* &aItemData,
                             CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer );
	void SetNodeVisualizerProperties( MFsTreeItemVisualizer& aItemVisualizer );
	void SetChildVisualizersProperties( TFsTreeItemId aNodeId );
	
    /**
     * Append heading line to list as a node
     *
     * @param aResourceId Resource id for the heading text
     * @return List item id of the created node
     */
	TFsTreeItemId AppendHeadingToListL( TInt aResourceId );

    /**
     * Append item to list under the specified node
     *
     * @param aItemData/aPrimaryDataBuff/aSecondaryDataBuff List item data
     * @param aParentNode Parent node in list
     * @return List item id of the created item
     */
	TFsTreeItemId AppendOneLineItemToListL( const TDesC& aItemData,
											TFsTreeItemId aParentNode );
	TFsTreeItemId AppendTwoLineItemToListL( const TDesC& aPrimaryDataBuff,
								  			const TDesC& aSecondaryDataBuff,
								  			TFsTreeItemId aParentNode,
								  			TBool aItemHasActionMenu = EFalse );
	TFsTreeItemId AppendDateTimeItemToListL( const TDesC& aPrimaryDataBuff,
											 const TDesC& aSecondaryDataBuff,
											 const TDesC& aDateTimeDataBuff,
											 TFsTreeItemId aParentNode );
    /**
     * Get display name and email address information from the address data
     *
     * @param aAddressData Mail address data where the information is got
     * @param aDisplayName On return holds the display name
     * @param aEmailAddress On return holds the email address
     * @return EFalse if both display name and email address are empty, otherwise ETrue
     */
	TBool GetDisplayNameAndEmailAddressL( CFSMailAddress* aAddressData,
										  TDesC* &aDisplayName, 
										  TDesC* &aEmailAddress );
	
    /**
     * Collapse all nodes except the specified one
     *
     * @param aExcludedNode Node that is not collapsed
     */
	void CollapseNodesExceptL( TFsTreeItemId aExcludedNode );

	// Action menu handling
	void LaunchActionMenuL();
	void HandleActionMenuCommandL( TActionMenuCustomItemId itemId );

	// Action menu action handling, use UI utility class
	TBool SendEmailToFocusedItemL() const; // Return value tells wheter operation succeeded or not
	void CallToFocusedItemL();
	void CreateMessageToFocusedItemL() const;
	void LaunchRemoteLookupForFocusedItemL() const;
	void AddFocusedItemToContactsL() const;
	void ShowContactDetailsForFocusedItemL() const;	
	void CopyFocusedItemToClipboardL() const;
	
	// Shortuct handling of collpase/expand all toggle and go to first/last
	void ShortcutCollapseExpandAllToggleL();
	void GoToTopL();
	void GoToBottomL();

	// Tells wheter the action menu should be shown for the currently focused list item
	TBool HasFocusedItemActionMenu() const;

    /**
     * Get email of address of the focused list item
     *
     * @return Pointer to email address if found, NULL if not found
     */
	CFSMailAddress* GetEmailAddressForFocusedItem() const;
	TDesC* GetEmailAddressForFocusedItemAsTDes() const;
	
	// Recursive function to get item's root level parent
	TFsTreeItemId GetRootParent( const TFsTreeItemId aItemId ) const;

	// Tell wheter all nodes are currently collpased/expanded
	TBool AllNodesCollapsed() const;
	TBool AllNodesExpanded( TFsTreeItemId aParentNodeId = KFsTreeRootID ) const;

	// First start function. 
	void DoFirstStartL();

private:
	enum TExpandCollapseMode
	    {
	    EExpandAll = 0,
	    ECollapseAllExceptTo,
	    ECollapseAllExceptCc,
	    ECollapseAllExceptBcc
	    };
	
private:
	// Pointers to ALF related objects
    CAlfEnv& iEnv;
    CAlfLayout* iParentLayout;

	// Pointers to mail framework related objects
	CFSMailMessage* iViewedMsg;

	// Pointers to own control and model objects
    CFSEmailUiMsgDetailsControl* iControl;	// Not owned, owned by ALF env
    CFSEmailUiMsgDetailsModel* iModel;		// Owned

	// Lists screen rect and node heights
    TRect iScreenRect;
	TInt iListNodeHeight;

	// Nodes that can be focused at startup
	TFsTreeItemId iToNodeId;
	TFsTreeItemId iCcNodeId;
	TFsTreeItemId iBccNodeId;

	// All nodes
	RArray<TFsTreeItemId> iNodeIds;
	
    // Pointer to the tree list component visualizer.
    CFsTreeVisualizerBase* iTreeVisualizer;

    // Pointer to the tree list component, owned.
    CFsTreeList* iTreeList;

   	// Previous view uid
	TUid iPreviousViewUid; 

	// Previous title bar text, restored when returning from this view
	HBufC* iPreviousTitleText;
	
	// Pointers to "error texts" shown in list
	TDesC* iNoDisplayNameAvailableText;
	TDesC* iNoEmailAddressAvailableText;
	
	TBool iFirstViewActivation;
	TBool iFirstStartCompleted;
	
	// When this flag is set, list (scroll bar) refresh is allowed when
	// inserting new items to list
	TBool iAllowListRefreshInInsert;
	
    // Flags for expand/collapse mode and when to expand + highlight next node
	TExpandCollapseMode iExpandCollapseMode;
	TBool iExpandAndHighlightNextNode;
	
	TBool iRCLSupported;
	
    // <cmail> video call
    // Flag for making video call
    TBool iVideoCall;
    // </cmail>
	};


#endif //__FREESTYLEEMAILUI_MSGDETAILSVISUALISER_H__
