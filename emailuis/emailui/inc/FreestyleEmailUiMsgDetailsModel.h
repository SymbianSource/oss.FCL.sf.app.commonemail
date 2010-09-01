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
* Description:  FreestyleEmailUi message details model and model item 
*				 classes definitions
*
*/

 
 
#ifndef __FREESTYLEEMAILUI_MESSAGEDETAILSMODEL_H__
#define __FREESTYLEEMAILUI_MESSAGEDETAILSMODEL_H__

#include <e32base.h>

//<cmail>
#include "fstreelistconstants.h"
//</cmail>

class CFSMailAddress;

// LIST ITEM MODEL DEFINITION
class CFSEmailUiMsgDetailsItem : public CBase
    {
public:

    /** Two-phased constructor */
	static CFSEmailUiMsgDetailsItem* NewLC( TFsTreeItemId aListId );

	/** Destructor. */
	~CFSEmailUiMsgDetailsItem();
	
private:

	CFSEmailUiMsgDetailsItem( TFsTreeItemId aListId );

public:
	TFsTreeItemId iListId;
	
	// Not owned
	CFSMailAddress* iMailAddress;

	// Not owned
	TDesC* iText;
    };

/**
 * An interface for a message details model.
 */
class CFSEmailUiMsgDetailsModel : public CBase
    {
public:
	/**
	* Constructor.
	*/
	CFSEmailUiMsgDetailsModel();

	/**
	* Destructor.
	*/
	~CFSEmailUiMsgDetailsModel();
	
	/**
	* Add an item to the end of the list.
	*/
	void AppendL( TFsTreeItemId aListId, CFSMailAddress* aMailAddress );
	
	/**
	* Add an item to the end of the list.
	*/
	void AppendL( TFsTreeItemId aListId, TDesC* aText );
	
	/**
	* Remove item by list item id.
	*/	
	void Remove(TFsTreeItemId aListId);

	/**
	* Remove all items.
	*/	
	void RemoveAll();

	/**
	* Get an item by list item id.
	* 
	* @return Model item or NULL if not found. Ownership not transferred.
	*/
	CFSEmailUiMsgDetailsItem* ItemByListId( TFsTreeItemId aListId ) const;

	/**
	* Get an item by list index.
	* 
	* @return Model item or NULL if not found. Ownership not trnasferred.
	*/
	CFSEmailUiMsgDetailsItem* ItemByIndex( TInt aModelIndex ) const;

	/**
	* Get number of items in list.
	* 
	* @return Number of items.
	*/		
	TInt Count() const;

private:
	/**
	* Get the array index of the spcified item.
	* 
	* @return Array index of the item.
	*/		
	TInt Index( TFsTreeItemId aListId ) const;
	
private:
	RPointerArray<CFSEmailUiMsgDetailsItem> iItemArray;

    };

#endif //__FREESTYLEEMAILUI_MESSAGEDETAILSMODEL_H__
