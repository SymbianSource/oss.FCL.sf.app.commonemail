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
* Description:  FreestyleEmailUi message details model implementation
*
*/


// SYSTEM INCLUDES
//<cmail>
#include "emailtrace.h"
#include "CFSMailAddress.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiMsgDetailsModel.h"

// ---------------------------------------------------------------------------
// Folder list item constructor.
// ---------------------------------------------------------------------------
CFSEmailUiMsgDetailsItem::CFSEmailUiMsgDetailsItem( TFsTreeItemId aListId )
	: iListId( aListId ), iMailAddress( NULL ), iText( NULL )
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// Folder list item destructor.
// ---------------------------------------------------------------------------
CFSEmailUiMsgDetailsItem::~CFSEmailUiMsgDetailsItem()
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CFSEmailUiMsgDetailsItem* CFSEmailUiMsgDetailsItem::NewLC(
    TFsTreeItemId aListId )
    {
    FUNC_LOG;
    CFSEmailUiMsgDetailsItem* self = new (ELeave) CFSEmailUiMsgDetailsItem( aListId );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CFSEmailUiMsgDetailsModel::CFSEmailUiMsgDetailsModel()
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CFSEmailUiMsgDetailsModel::~CFSEmailUiMsgDetailsModel()
	{
    FUNC_LOG;
	RemoveAll();
	iItemArray.Close();
	}

// ---------------------------------------------------------------------------
// Add an item to the end of the list.
// ---------------------------------------------------------------------------
void CFSEmailUiMsgDetailsModel::AppendL( TFsTreeItemId aListId, CFSMailAddress* aMailAddress )
	{
    FUNC_LOG;
	CFSEmailUiMsgDetailsItem* item = CFSEmailUiMsgDetailsItem::NewLC( aListId );
	item->iMailAddress = aMailAddress;
	iItemArray.AppendL( item );
    CleanupStack::Pop( item );
	}

// ---------------------------------------------------------------------------
// Add an item to the end of the list.
// ---------------------------------------------------------------------------
void CFSEmailUiMsgDetailsModel::AppendL( TFsTreeItemId aListId, TDesC* aText )
	{
    FUNC_LOG;
	CFSEmailUiMsgDetailsItem* item = CFSEmailUiMsgDetailsItem::NewLC( aListId );
	item->iText = aText;
	iItemArray.AppendL( item );
    CleanupStack::Pop( item );
	}

// ---------------------------------------------------------------------------
// Remove item by list item id.
// ---------------------------------------------------------------------------
void CFSEmailUiMsgDetailsModel::Remove( TFsTreeItemId aListId )
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
void CFSEmailUiMsgDetailsModel::RemoveAll()
	{
    FUNC_LOG;
	iItemArray.ResetAndDestroy();
	}

// ---------------------------------------------------------------------------
// Get an item by list item id.
// 
// @return NULL if not found.
// ---------------------------------------------------------------------------
CFSEmailUiMsgDetailsItem* CFSEmailUiMsgDetailsModel::ItemByListId( TFsTreeItemId aListId ) const
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
// Get an item by list index.
// 
// @return NULL if not found.
// ---------------------------------------------------------------------------
CFSEmailUiMsgDetailsItem* CFSEmailUiMsgDetailsModel::ItemByIndex( TInt aModelIndex ) const
	{
    FUNC_LOG;
	return iItemArray[ aModelIndex ];
	}

// ---------------------------------------------------------------------------
// Get number of items in list.
// 
// @return Number of items.
// ---------------------------------------------------------------------------		
TInt CFSEmailUiMsgDetailsModel::Count() const
	{
    FUNC_LOG;
	return iItemArray.Count();
	}
	
// ---------------------------------------------------------------------------
// Get the array index of the spcified item.
// 
// @return Array index of the item.
// ---------------------------------------------------------------------------		
TInt CFSEmailUiMsgDetailsModel::Index( TFsTreeItemId aListId ) const
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

