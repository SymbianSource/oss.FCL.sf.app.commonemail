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
* Description:  FreestyleEmailUi attachments list visualiser definition 
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_ATTACHMENTSLISTVISUALISER_H__
#define __FREESTYLEEMAILUI_ATTACHMENTSLISTVISUALISER_H__

// SYSTEM INCLUDES
//<cmail>
#include "fstreelistobserver.h"
#include "fsccontactactionmenudefines.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiAttachmentsListModel.h"

// FORWARD DECLARATIONS
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CFsTreeList;
class CFsTreeVisualizerBase;
class CFreestyleEmailUiAttachmentsListControl;
class CFreestyleEmailUiAppUi;
class CFsControlBar;
class CFsTreePlainOneLineNodeVisualizer;
struct TAttachmentData;


/**
 * Attachment list visualiser.
 */
// <cmail>
class CFSEmailUiAttachmentsListVisualiser : public CFsEmailUiViewBase,
    public MFSEmailUiAttachmentsStatusObserver, public MFsTreeListObserver, public MFsActionMenuPositionGiver
    {
// </cmail>
public: // constructors and destructor

    /**
     *  Two-phased constructor.
     *  
     * @param aEnv Reference to Alfred environment.
     * @param aAppUi Pointer to app ui.
     * @param aControlGroup Reference to visualiser's control group.
     */
    static CFSEmailUiAttachmentsListVisualiser* NewL( CAlfEnv& aEnv,
        CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup );

    /**
     * @see CFSEmailUiAttachmentsListVisualiser::NewL
     */
    static CFSEmailUiAttachmentsListVisualiser* NewLC( CAlfEnv& aEnv,
        CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup );

    /** Destructor. */
    virtual ~CFSEmailUiAttachmentsListVisualiser();

public: // from CAknView

// from base class MFsTreeListObserver

    /** Handles events from the tree list. */
    // <cmail> Touch
    void TreeListEventL( const TFsTreeListEvent aEvent, const TFsTreeItemId aId );
    TPoint ActionMenuPosition();
    // </cmail>
    TUid Id() const;
    // <cmail> Toolbar
    /*void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
        const TDesC8& aCustomMessage );*/
    // </cmail> Toolbar
    void ChildDoDeactivate();
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

public: // from CFsEmailUiViewBase


	void HandleDynamicVariantSwitchL( TDynamicSwitchType aType );
    void HandleDynamicVariantSwitchOnBackgroundL( TDynamicSwitchType aType );

    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;

public: // from MFSEmailUiAttachmentsStatusObserver

    void DownloadStatusChangedL( TInt aIndex );



public: // new methods

    // Mail list current model data
    CFSEmailUiAttachmentsListModel* Model();

    // Sets softkeys
    void SetViewSoftkeysL( TInt aResourceId );	

    void CreateHeadersAndSeparatorsL();
    void MoveItemToNodeL( TFsTreeItemId aItemId, TFsTreeItemId aTargetNodeId );
    void SetMskL();
    // Event handling forwarded from control
    TBool OfferEventL( const TAlfEvent& aEvent ); 
    void HandleCommandL( TInt aCommand );

    void HandleMailBoxEventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
    TAny* aParam1, TAny* aParam2, TAny* aParam3 );

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
    
private: // constructors

    // C++ Constructor.
    CFSEmailUiAttachmentsListVisualiser( CAlfEnv& aEnv,
        CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aControlGroup );
    // Second phase constructor.
    void ConstructL();

private: // new methods

    // Refresh list, recreates it from empty
    void RefreshL( TPartData aPart );

    void ReScaleUiL();
    void ResetColorsL();

    TBool IsCommandAvailableOnItem( TInt aCommandId, const TAttachmentData* aFocusedItem ) const;
    HBufC* CreateSecondLineTextLC( const TAttachmentData& aAttachment ) const;
    void TryToOpenItemL( TFsTreeItemId aId );
    void RemoveAttachmentContentL( TFsTreeItemId aId );

    // launches action menu
    void LaunchActionMenuL();

    void ShortcutCollapseExpandAllToggleL();
    void GoToTopL();
    void GoToBottomL();

    void SetupTitlePaneTextL();
    
    //<cmail> Not used anywhere anymore.

    //</cmail>
    CAlfControl* ViewerControl();

    void DoFirstStartL();
    void UpdateSize();
    
    //<cmail> touch
    void DoHandleActionL( );    
    //</cmail> touch
    
private: // data

    // Reference to Alf environment
    CAlfEnv& iEnv;

    // Screen control
    CFreestyleEmailUiAttachmentsListControl* iScreenControl;

    CFSEmailUiAttachmentsListModel* iModel; 
    CFsTreeList* iAttachmentsList;
    CAlfDeckLayout* iListLayout;
    CAlfAnchorLayout* iAnchorLayout;
    CFsTreeVisualizerBase* iAttachmentsTreeListVisualizer;

    TFsTreeItemId iDownloadedNode;
    TFsTreeItemId iNotDownloadedNode;

// <cmail> Prevent Download Manager opening with attachments 
    //TFsTreeItemId iDownloadManagerLink;
// </cmail>

    //not owned
    CFsTreePlainOneLineNodeVisualizer* iDownloadedVisualiser;
    //not owned
    CFsTreePlainOneLineNodeVisualizer* iNotDownloadedVisualiser;

    // The text shown in the title pane when this view was activated
    //<cmail> removed as not used anywhere.

	//</cmail>
    TBool iFirstStartCompleted;
    // Flag for embedded message mode
    TBool iEmbeddedMsgMode;
    };


#endif 
