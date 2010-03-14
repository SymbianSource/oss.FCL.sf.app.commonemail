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
* Description:  FreestyleEmailUi  list model and model item 
*				 abstract base classes definitions
*
*/

 
 
#ifndef __FREESTYLEEMAILUI_LISTMODEL_H__
#define __FREESTYLEEMAILUI_LISTMODEL_H__

#include <e32base.h>
//<cmail>
#include "cfsmailmessage.h"
//</cmail>

// LIST ITEM MODEL DEFINITION
class MFSListModelItem
    {
public:
    /** Virtual destructor. */
    virtual ~MFSListModelItem() {}

    };

/**
 * An interface for a list model. This mixin class provides storage methods for 
 * accessing, adding and removing list data.
 * 
 * Logic code and visualisation is done elsewhere.
 */
class MFSEmailListModel
    {
public:
	/**
	* Add an item to the end of the list.
	*/
	virtual void AppendL(MFSListModelItem* aItem) = 0;
	
	/**
	* Insert an item at the given index.
	*/	
	virtual void InsertL(MFSListModelItem* aItem, TInt aIndex) = 0;

	/**
	* Remove item by its index.
	*/	
	virtual void RemoveAndDestroy(TInt aIndex) = 0;

	/**
	* get an item by its index.
	* 
	* @return NULL if out of range.
	*/
	virtual MFSListModelItem* Item(TInt aIndex) = 0;

	/**
	* Get number of items in list.
	* 
	* @return Number of items.
	*/		
	virtual TInt Count() const = 0;
    };

#endif //__FREESTYLEEMAILUI_LISTMODEL_H__
