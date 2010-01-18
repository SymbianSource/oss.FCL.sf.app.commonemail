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
* Description:  FreestyleEmailUi attachments list model and model item definition.
*
*/


#ifndef __FREESTYLEEMAILUI_SENDATTACHMENTSLISTMODEL_H__
#define __FREESTYLEEMAILUI_SENDATTACHMENTSLISTMODEL_H__

// SYSTEM INCLUDES
#include <e32base.h>
//<cmail>
#include "fstreelistconstants.h"
#include "fstreelistobserver.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiListModel.h"

// FORWARD DECLARATIONS
class CFreestyleEmailUiAppUi;
class CFreestyleEmailUiSendAttachmentsListControl;
class CFsTreeVisualizerBase;
class CFsTreeList;
class CAlfEnv;
// <cmail>
// class TFsTreeListEvent;
// </cmail>

// CLASS DECLARATION
class CFSEmailUiSendAttachmentsListModelItem : 
    public CBase,
    public MFSListModelItem
    {
public:
    static CFSEmailUiSendAttachmentsListModelItem* NewL( 
        const TDesC& aFileName, 
        TInt aFileSize,
        TFileType aFileType,
        TBool aRemote, 
        TBool aReadOnly );
	    
    static CFSEmailUiSendAttachmentsListModelItem* NewLC( 
        const TDesC& aFileName, 
        TInt aFileSize,
        TFileType aFileType,
        TBool aRemote, 
        TBool aReadOnly );
	    
    virtual ~CFSEmailUiSendAttachmentsListModelItem(); 

private:
    CFSEmailUiSendAttachmentsListModelItem( 
        const TDesC& aFileName, 
        TInt aFileSize,
        TFileType aFileType,
        TBool aRemote, 
        TBool aReadOnly );

public:
    const TDesC& FileName() const;
    TPtrC FileExtension() const;
    TFSMailMsgId MailMsgPartId() const;
    void SetMailMsgPartId( const TFSMailMsgId aPartId );
    TInt FileSize() const;
    TFsTreeItemId ItemId() const;
    void SetItemId(  TFsTreeItemId aItemId );
    void SetRemote( TBool aIsRemote );
    TBool IsRemote();
    TBool IsReadOnly();
    TFileType FileType();

private:
	
    TFileName 		iFileName;
    TFSMailMsgId	iMailMsgPartId;
    TInt 			iFileSize;
    TFsTreeItemId	iItemId;
    TBool           iRemote;
    TBool           iReadOnly;
    TFileType       iFileType;
    }; 
 
// CLASS DECLARATION
class CFSEmailUiSendAttachmentsListModel : 
	public CBase, 
	public MFSEmailListModel, 
	public MFsTreeListObserver
	{
public: // 2nd phase constructors
    static CFSEmailUiSendAttachmentsListModel* NewL( 
        CFreestyleEmailUiAppUi* aAppUi,
        CAlfEnv& aEnv,
        CFreestyleEmailUiSendAttachmentsListControl* aControl );

    static CFSEmailUiSendAttachmentsListModel* NewLC( 
        CFreestyleEmailUiAppUi* aAppUi,
        CAlfEnv& aEnv,
        CFreestyleEmailUiSendAttachmentsListControl* aControl);
		
    virtual ~CFSEmailUiSendAttachmentsListModel();

public:
    void AppendL( MFSListModelItem* aItem );
		
    void InsertL( MFSListModelItem* aItem, TInt aIndex );
		
    void RemoveAndDestroy( TInt aIndex );
		
    MFSListModelItem* Item( TInt aIndex );
		
    TInt HighlightedIndex();
		
    TInt Count() const;
		
    CFreestyleEmailUiAppUi* AppUi();
		
    TFsTreeItemId AppendItemToAttachmentListL( 
        CFSEmailUiSendAttachmentsListModelItem* aItem );
		
    CFsTreeList* AttachmentList();
		
    void ReFreshListL();
	
    void ReScaleUiL();
    
    void UpdateHeaderItemTextL();

    /**
	* Removes item from model and list and also
	* calls fw to remove attachment from message
	*/
    void RemoveItemByIndexL( const TInt aIndex );
		
    /**
     * Removes all attachment items from the model, list, and message
     */
    void RemoveAllAttachmentsL();
    
    /**
    * Returns the total size of attachments in bytes.
    */
    TInt TotalAttachmentSize();
    
    TBool HasRemoteAttachments();
    
    TBool HasReadOnlyAttachments();
    
    CFSEmailUiSendAttachmentsListModelItem* GetSelectedItem();
    
    CFSEmailUiSendAttachmentsListModelItem* GetItemByNodeId( TFsTreeItemId aNodeId );
    
    TBool CanSelectPreviousItem();
	
    void GoToTopL();
    void GoToBottomL();
    void CollapseL();
    void ExpandL();
	void CollapseExpandAllToggleL();
	
	/**
	* Empties model from items
	* Does not call fw to remove attas from message
	*/
	void Clear();
	
	//<cmail> touch
	CFsTreeVisualizerBase* GetVisualizer();
    //</cmail> 
	
	CAlfLayout* GetParentLayout() const;

    /** Handles action for the focused item. */
    void HandleActionL();

protected: // from MFsTreeListObserver

    // <cmail> Touch
    void TreeListEventL( const TFsTreeListEvent aEvent, 
                         const TFsTreeItemId aId,
                         const TPoint& aPoint );
    // </cmail>

		
private: // constructors not available directly

    CFSEmailUiSendAttachmentsListModel( CFreestyleEmailUiAppUi* aAppUi );
    void ConstructL( 
        CAlfEnv& aEnv,
        CFreestyleEmailUiSendAttachmentsListControl* aControl );
	
private:

    //<cmail> touch
    void DoHandleActionL( ); 
    //</cmail>
    
    void CreateListSkeletonL();
    void CreateListItemsL();
    void InitAttachmentListL();
    void CreateListSeparatorL( 
        TInt aResourceId, 
        TFsTreeItemId aParentNodeId,
        CFsTreePlainOneLineNodeVisualizer*& aVisualizer, 
        TFsTreeItemId& aSeparatorNodeId );
    
    //<cmail>
    void SetupTitlePaneTextL();
    //</cmail>
    
private:

    CFreestyleEmailUiAppUi* iAppUi;
    RPointerArray<CFSEmailUiSendAttachmentsListModelItem> iItems;
	//<cmail>

	//</cmail>
    CAlfEnv* iEnv;
    CFreestyleEmailUiSendAttachmentsListControl* iControl;
	  	
    // Attachment list
    CFsTreeList* iAttachmentsList;
    CAlfDeckLayout* iListLayout;
    CFsTreeVisualizerBase* iAttachmentsTreeListVisualizer;

    TFsTreeItemId iLocalNode;
    TFsTreeItemId iRemoteNode;
	//not owned
	CFsTreePlainOneLineNodeVisualizer* iLocalNodeVisualiser;
	//not owned
	CFsTreePlainOneLineNodeVisualizer* iRemoteNodeVisualiser;
	};

#endif //__FREESTYLEEMAILUI_SENDATTACHMENTSLISTMODEL_H__

