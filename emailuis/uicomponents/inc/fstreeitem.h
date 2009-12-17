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
* Description:  Data item classes used by tree list component
 *
*/


#ifndef C_FSTREEITEM_H
#define C_FSTREEITEM_H

#include <e32cmn.h>
#include <e32base.h>

#include "fstreelistconstants.h"

class CFsTreeItem;

/** Array of pointers to the tree items */
typedef RPointerArray<CFsTreeItem> RFsTreeItemList;

class CFsTreeNode;
class MFsTreeItemData;
class MFsTreeItemVisualizer;


/**
 *  CFsTreeItem is a class representing leaf in tree data structure
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib
 */
NONSHARABLE_CLASS( CFsTreeItem ) : public CBase
    {
public: // Construction

    /**
     * Two-phased constructor
     *
     * @param aParent Reference to the parent node (cannot be NULL!)
     * @param aData Reference to the item data instance
     * @param aVisualizer Reference to the item visualizer instance
     */
    IMPORT_C static CFsTreeItem* NewL( CFsTreeNode& aParent, MFsTreeItemData& aData,
            MFsTreeItemVisualizer& aVisualizer);

    /**
     * Two-phased constructor
     *
     * @param aParent Reference to the parent node (cannot be NULL!)
     * @param aData Reference to the item data class
     * @param aVisualizer Reference to the item visualizer class
     */
    IMPORT_C static CFsTreeItem* NewLC( CFsTreeNode& aParent, MFsTreeItemData& aData,
            MFsTreeItemVisualizer& aVisualizer);

public: // Item content

    /**
     * Gets reference to the item visualizer
     *
     * @return Reference to the item visualizer
     */
    MFsTreeItemVisualizer* ItemVisualizer() const;

    /**
     * Gets refrence to the item data
     *
     * @return Reference to the item data
     */
    MFsTreeItemData& ItemData() const;

    /**
     * Sets visualizer for the item
     *
     * @param aVisualizer Reference to the item visualizer
     */
    void SetItemVisualizer( MFsTreeItemVisualizer& aVisualizer );

    /**
     * Sets data for the item
     *
     * @param aData Reference to the item data
     */
    void SetItemData( MFsTreeItemData& aData );


    /**
     * C++ destructor
     */
    ~CFsTreeItem();

public: // Hierchy management

    /**
     * Checks if tree item is a node
     *
     * @return EFalse
     */
    virtual TBool IsNode() const;

    /**
     * Return pointer to node object. Returned value is @c NULL, if the item
     * is not derived from @c CFsTreeNode.
     *
     * @return @c CFsTreeNode pointer, if the item is a node.
     */
    virtual CFsTreeNode* Node();

    /**
     * Gets the parent node of the item
     *
     * @return Pointer to the parent node of the item
     */
    CFsTreeNode* Parent() const;

    /**
     * Sets the parent node of the item
     *
     * @param aParent Pointer to the parent node
     */
    void SetParent( CFsTreeNode& aParent );

    /**
     * Returns the level of the item in the tree. The returned value matches
     * the number of parents in the items parent chain.
     *
     * @return The level of the item in the tree.
     */
    TUint Level() const;

    /**
     * Returns recursively the number of all children under the node
     * (NOTE: this method is overriden in class CFsTreeNode)
     * @return Always 0
     */
    virtual TUint CountChildrenRecursively() const;

protected: //Construction

    /**
     * C++ constructor
     */
    CFsTreeItem( CFsTreeNode* aParent, MFsTreeItemData& aData,
            MFsTreeItemVisualizer& aVisualizer );

    /**
     * Second phase constructor
     *
     */
    void ConstructL( );

protected: //Data

    /**
     * Item's parent node
     * Not own
     */
    CFsTreeNode* iParent;

    /**
     * Item's data
     * Not own
     */
    MFsTreeItemData* iData;

    /**
     * Item's visualizer
     * Not own
     */
    MFsTreeItemVisualizer* iVisualizer;

    };

#endif // C_FSTREEITEM_H

