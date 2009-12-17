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
* Description:  FreestyleEmailUi download manager model and model item definition.
*
*/


#ifndef __FREESTYLEEMAILUI_DOWNLOADMANAGERMODEL_H__
#define __FREESTYLEEMAILUI_DOWNLOADMANAGERMODEL_H__

// SYSTEM INCLUDES
#include <e32base.h>
//<cmail>
#include "fstreelist.h"
//</cmail>
// INTERNAL INCLUDES
#include "FreestyleEmailUiAttachmentsListModel.h"

// FORWARD DECLARATIONS
class CFreestyleEmailUiAppUi;
class CFSMailMessage;

struct TMessageData
	{
	HBufC* mailSubject;
	RArray<TAttachmentData> mailAttachments;
	TPartData partData;
	TFsTreeItemId treeId;
	};
 
class CFSEmailUiDownloadManagerModel : public CFSEmailUiAttachmentsModelBase
  {
public:
	static CFSEmailUiDownloadManagerModel* NewL( CFreestyleEmailUiAppUi& aAppUi, MFSEmailUiAttachmentsStatusObserver& aObserver );
	static CFSEmailUiDownloadManagerModel* NewLC(CFreestyleEmailUiAppUi& aAppUi, MFSEmailUiAttachmentsStatusObserver& aObserver );
	~CFSEmailUiDownloadManagerModel();

	TBool RemoveItem( TFsTreeItemId aTreeId );
	void SetItemIdL(TUint aMailIndex, TUint aAttachmentIndex, TFsTreeItemId aTreeId);
	const RArray<TMessageData>& GetModel();
	TInt AttachmentCount() const;
	void SetNodeIdL( TInt aNodeIndex, TFsTreeItemId aTreeItemId );

	virtual void RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart );

public: // from CFSEmailUiAttachmentsModelBase
    /** 
     * Returns pointer to attachment data of the given tree item.
     * @param aTreeId ID identifying the tree list item.
     * @return Pointer to attachment data, or NULL, if the item was not found.
     */ 
    TAttachmentData* GetItem( TFsTreeItemId aTreeId );

    /**
     * Returns reference to attachment data of given tree list item.
     * @param aTreeId ID identifying the tree list item.
     * @return Reference to attachemtn data.
     * @leave KErrNotFound The given item is not found.
     */
    TAttachmentData& GetItemL( TFsTreeItemId aTreeId );

    TPartData GetMessageL( TFsTreeItemId aTreeId );
    
    TBool DownloadAllAttachmentsL();
    TBool SaveAllAttachmentsL( const TDesC& fileName );
    void CancelAllDownloadsL();
    TBool IsThereAnyMessageAttachments();
    
    /**
     * Function can be used to clear items from the model
     * that are related to deleted messages.
     * @return  ETrue if at least one item was removed, EFalse otherwise
     */
    TBool ClearInvalidItemsL();

private:
	CFSEmailUiDownloadManagerModel( CFreestyleEmailUiAppUi& aAppUi, MFSEmailUiAttachmentsStatusObserver& aObserver );
	void ConstructL();

private:
	MFSEmailUiAttachmentsStatusObserver& iObserver;
	RArray<TMessageData> iMails;
	CFSMailClient* iMailClient;
  };

#endif //__FREESTYLEEMAILUI_MAILLISTMODEL_H__

