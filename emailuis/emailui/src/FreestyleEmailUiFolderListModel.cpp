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
* Description:  FreestyleEmailUi folder list model implementation
*
*/

 
// SYSTEM INCLUDES
//<cmail>
#include "emailtrace.h"
#include "CFSMailBox.h"
#include "CFSMailFolder.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiFolderListModel.h"


// ---------------------------------------------------------------------------
// Folder list item constructor.
// ---------------------------------------------------------------------------
CFSEmailUiFolderListItem::CFSEmailUiFolderListItem()
	: iListId( KFsTreeNoneID ), 
	  iFolderId( TFSMailMsgId() ),
	  iMailboxId( TFSMailMsgId() ),
	  iSortField( EFSMailDontCare )
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// Folder list item destructor.
// ---------------------------------------------------------------------------
CFSEmailUiFolderListItem::~CFSEmailUiFolderListItem()
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListItem* CFSEmailUiFolderListItem::NewL()
    {
    FUNC_LOG;
    CFSEmailUiFolderListItem* self = CFSEmailUiFolderListItem::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListItem* CFSEmailUiFolderListItem::NewLC()
	{
    FUNC_LOG;
    CFSEmailUiFolderListItem* self = new (ELeave) CFSEmailUiFolderListItem();
    CleanupStack::PushL(self);
    return self;
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CFSEmailUiFolderListModel::CFSEmailUiFolderListModel()
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CFSEmailUiFolderListModel::~CFSEmailUiFolderListModel()
	{
    FUNC_LOG;
	RemoveAll();
	iItemArray.Close();
	}

// ---------------------------------------------------------------------------
// Add an item to the end of the list.
// ---------------------------------------------------------------------------
void CFSEmailUiFolderListModel::AppendL( TFsTreeItemId aListId, CFSMailFolder* aFolder )
	{
    FUNC_LOG;
	CFSEmailUiFolderListItem* item = CFSEmailUiFolderListItem::NewLC();
	item->iListId = aListId;
	item->iFolderId = aFolder->GetFolderId();
	iItemArray.AppendL( item );
    CleanupStack::Pop( item );
	}

// ---------------------------------------------------------------------------
// Add an item to the end of the list.
// ---------------------------------------------------------------------------
void CFSEmailUiFolderListModel::AppendL( TFsTreeItemId aListId, CFSMailBox* aMailbox )
	{
    FUNC_LOG;
	CFSEmailUiFolderListItem* item = CFSEmailUiFolderListItem::NewLC();
	item->iListId = aListId;
	item->iMailboxId = aMailbox->GetId();
	iItemArray.AppendL( item );
    CleanupStack::Pop( item );
	}

// ---------------------------------------------------------------------------
// Add an item to the end of the list.
// ---------------------------------------------------------------------------
void CFSEmailUiFolderListModel::AppendL( TFsTreeItemId aListId, TFSMailSortField aSortField )
	{
    FUNC_LOG;
	CFSEmailUiFolderListItem* item = CFSEmailUiFolderListItem::NewLC();
	item->iListId = aListId;
	item->iSortField = aSortField;
	iItemArray.AppendL( item );
    CleanupStack::Pop( item );
	}

// ---------------------------------------------------------------------------
// Remove item by list item id.
// ---------------------------------------------------------------------------
void CFSEmailUiFolderListModel::Remove( TFsTreeItemId aListId )
	{
    FUNC_LOG;
	TInt index = Index( aListId );
	if( index != KErrNotFound )
		{
		delete iItemArray[index];
		iItemArray.Remove( index );
		}
	}

// ---------------------------------------------------------------------------
// Remove all items.
// ---------------------------------------------------------------------------	
void CFSEmailUiFolderListModel::RemoveAll()
	{
    FUNC_LOG;
	iItemArray.ResetAndDestroy();
	iSeparatorCount = 0;
	}

// ---------------------------------------------------------------------------
// get an item by list item id.
// 
// @return NULL if not found.
// ---------------------------------------------------------------------------
CFSEmailUiFolderListItem* CFSEmailUiFolderListModel::ItemByListId( TFsTreeItemId aListId ) const
	{
    FUNC_LOG;
	TInt index = Index( aListId );
	if( index != KErrNotFound )
		{
		return iItemArray[ index ];
		}
	else
		{
		return NULL;
		}
	}
	
// ---------------------------------------------------------------------------
// get an item by list index.
// 
// @return NULL if not found.
// ---------------------------------------------------------------------------
CFSEmailUiFolderListItem* CFSEmailUiFolderListModel::ItemByIndex( TInt aModelIndex ) const
	{
    FUNC_LOG;
	return iItemArray[ aModelIndex ];
	}

// ---------------------------------------------------------------------------
// Get number of items in list.
// 
// @return Number of items.
// ---------------------------------------------------------------------------		
TInt CFSEmailUiFolderListModel::Count() const
	{
    FUNC_LOG;
	return iItemArray.Count();
	}
	
// ---------------------------------------------------------------------------      
// Get number of separators in list.
// 
// @return Number of separators in list.
// ---------------------------------------------------------------------------
TInt CFSEmailUiFolderListModel::SeparatorCount() const
    {
    FUNC_LOG;
    return iSeparatorCount;
    }

// ---------------------------------------------------------------------------      
// Increase separator count in list. 
// 
// @param aIncreaseBy How much separator count is increased, defaut is 1.
//                    If parameter is negative, then the count is decreased.
// ---------------------------------------------------------------------------      
void CFSEmailUiFolderListModel::IncreaseSeparatorCount( TInt aIncreaseBy )
    {
    FUNC_LOG;
    iSeparatorCount += aIncreaseBy;
    }

// ---------------------------------------------------------------------------
// Get the array index of the spcified item.
// 
// @return Array index of the item.
// ---------------------------------------------------------------------------		
TInt CFSEmailUiFolderListModel::Index( TFsTreeItemId aListId ) const
	{
    FUNC_LOG;
	for( TInt i = 0; i < Count(); i++ )
		{
		if( iItemArray[i]->iListId == aListId )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

