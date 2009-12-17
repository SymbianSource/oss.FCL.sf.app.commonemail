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
* Description:  FreestyleEmailUi folder list model and model item 
*				 classes definitions
*
*/

 
 
#ifndef __FREESTYLEEMAILUI_FOLDERLISTMODEL_H__
#define __FREESTYLEEMAILUI_FOLDERLISTMODEL_H__

#include <e32base.h>

//<cmail>
#include "fstreelistconstants.h"
//</cmail>

class CFSMailFolder;
class CFSMailBox;

// LIST ITEM MODEL DEFINITION
class CFSEmailUiFolderListItem : public CBase
    {
public:
	static CFSEmailUiFolderListItem* NewL();
	static CFSEmailUiFolderListItem* NewLC();

	~CFSEmailUiFolderListItem();

private:
	CFSEmailUiFolderListItem();
    
public:
	TFsTreeItemId iListId;
	
	TFSMailMsgId iFolderId;

	TFSMailMsgId iMailboxId;
	
	TFSMailSortField iSortField;
    };

/**
 * An interface for a folder list/mailbox model.
 */
class CFSEmailUiFolderListModel : public CBase
    {
public:
	/**
	* Constructor.
	*/
	CFSEmailUiFolderListModel();

	/**
	* Destructor.
	*/
	~CFSEmailUiFolderListModel();
	
	/**
	* Add an item to the end of the list.
	*/
	void AppendL( TFsTreeItemId aListId, CFSMailFolder* aFolder );

	/**
	* Add an item to the end of the list.
	*/
	void AppendL( TFsTreeItemId aListId, CFSMailBox* aMailbox );

	/**
	* Add an item to the end of the list.
	*/
	void AppendL( TFsTreeItemId aListId, TFSMailSortField aSortField );
	
	/**
	* Remove item by list item id.
	*/	
	void Remove( TFsTreeItemId aListId );

	/**
	* Remove all items.
	*/	
	void RemoveAll();

	/**
	* Get an item by list item id.
	* 
	* @return Model item or NULL if not found. Ownership not trnasferred.
	*/
	CFSEmailUiFolderListItem* ItemByListId(TFsTreeItemId aListId) const;

	/**
	* Get an item by list index
	* 
	* @return Model item or NULL if not found. Ownership not trnasferred.
	*/
	CFSEmailUiFolderListItem* ItemByIndex( TInt aModelIndex ) const;

	/**
	* Get number of items in list.
	* 
	* @return Number of items.
	*/
	TInt Count() const;

    /**
    * Get number of separators in list.
    * 
    * @return Number of separators in list.
    */
    TInt SeparatorCount() const;

    /**
    * Increase list's separator count. 
    * 
    * @param aIncreaseBy How much separator count is increased, defaut is 1.
    *                    If parameter is negative, then the count is decreased.
    */
    void IncreaseSeparatorCount( TInt aIncreaseBy = 1 );

private:
	/**
	* Get the array index of the spcified item.
	* 
	* @return Array index of the item.
	*/
	TInt Index( TFsTreeItemId aListId ) const;
	
private:
	RPointerArray<CFSEmailUiFolderListItem> iItemArray;
	
	// Number of separators, is zeroed in RemoveAll
	TInt iSeparatorCount;

    };

#endif //__FREESTYLEEMAILUI_FOLDERLISTMODEL_H__
