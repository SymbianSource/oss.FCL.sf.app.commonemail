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
* Description:  FreestyleEmailUi download manager visualiser definition 
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_DOWNLOADMANAGERVISUALISER_H__
#define __FREESTYLEEMAILUI_DOWNLOADMANAGERVISUALISER_H__

// SYSTEM INCLUDES
//<cmail>
#include "fstreelistconstants.h"
#include "fstreelistobserver.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiAttachmentsListModel.h"

// FORWARD DECLARATIONS
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CFsTreeList;
class CFsTreeVisualizerBase;
class CFreestyleEmailUiDownloadManagerControl;
class CFreestyleEmailUiAppUi;
class CFsControlBar;
class CFSEmailUiDownloadManagerModel;

/**
 * Download manager visualiser.
 */
class CFSEmailUiDownloadManagerVisualiser : public CFsEmailUiViewBase,
    public MFSEmailUiAttachmentsStatusObserver, public MFsTreeListObserver
	{
public:
	static CFSEmailUiDownloadManagerVisualiser* NewL(CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup);
	static CFSEmailUiDownloadManagerVisualiser* NewLC(CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup);
    virtual ~CFSEmailUiDownloadManagerVisualiser();

	// from MFSEmailUiAttachmentsStatusObserver
	virtual void DownloadStatusChangedL( TInt aIndex );

	// from MFsTreeListObserver
	// <cmail> Touch
    virtual void TreeListEventL( const TFsTreeListEvent aEvent, 
                                    const TFsTreeItemId aId = KFsTreeNoneID );
    // </cmail>

    // From CAknView
	TUid Id() const;			   
    // <cmail> Toolbar
	/*void DoActivateL(const TVwsViewId& aPrevViewId,
	                 TUid aCustomMessageId,
	                 const TDesC8& aCustomMessage);*/
    // </cmail> Toolbar
	void ChildDoDeactivate();                
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	// From CFsEmailUiViewBase 
              
	void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
    void HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType aType );
    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;

	// Refresh
	void RefreshL();

	// Mail list current model data
	CFSEmailUiDownloadManagerModel* Model();
	
	// Highlighted index
	TInt HighlightedIndex();

	void ReScaleUiL();
	void ResetColorsL();
	void SetMskL();
	// Event handling forwarded from control
	TBool OfferEventL( const TAlfEvent& aEvent ); 
	void HandleCommandL(TInt aCommand);
	
	CAlfControl* ViewerControl();
	
	void HandleMailBoxEventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox, 
		TAny* aParam1, TAny* /*aParam2*/, TAny* /*aParam3*/ );

// <cmail> Toolbar    
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
   
// </cmail> Toolbar    
	
private:
	CFSEmailUiDownloadManagerVisualiser( CAlfEnv& aEnv,
	    CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup );
	void ConstructL();

    TBool IsCommandAvailableOnItem( TInt aCommandId, const TAttachmentData* aFocusedItem ) const;
    HBufC* CreateSecondLineTextLC( const TAttachmentData& aAttachment ) const;
    void TryToOpenItemL( TFsTreeItemId aId );
    void RemoveAttachmentL( TFsTreeItemId aId );
	void LaunchActionMenuL();
	
	void ShortcutCollapseExpandAllToggleL();
	void GoToTopL();
	void GoToBottomL();

	void SetMailboxNameToStatusPaneL();

private:
    // Reference to Alf environment
    CAlfEnv& iEnv;

  	// Screen control
    CFreestyleEmailUiDownloadManagerControl* iScreenControl;
  
  	// Control bar  
	CFsControlBar* iControlBar;
  	
  	// Mail List 
  	CFsTreeList* iDownloadManagerList;
    CFSEmailUiDownloadManagerModel* iModel; 
  	CFsTreeList* iDownloadsList;
 	CAlfDeckLayout* iListLayout;
    CFsTreeVisualizerBase* iDownloadManagerTreeListVisualizer;
  	TFsTreeItemId iHeaderItemId;
  	};




#endif 
