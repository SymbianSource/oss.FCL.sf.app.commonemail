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
* Description:  Interface class for storing tree item data
*
*/


#ifndef M_FSTREEITEMDATA_H
#define M_FSTREEITEMDATA_H


typedef TInt TFsTreeItemDataType;

/**
 * MFsTreeItemData is an interface for the tree item data.
 *
 * @code
 *
 * @endcode
 *
 * @lib
 */
NONSHARABLE_CLASS( MFsTreeItemData )
    {
public:

    /**
     * C++ destructor
     */
    virtual ~MFsTreeItemData() {};
    
public:
    
    /**
     * Type of the data item.
     */
    virtual TFsTreeItemDataType Type() const = 0;
    
    };
    
    
#endif // M_FSTREEITEMDATA_H
