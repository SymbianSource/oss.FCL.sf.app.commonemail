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


#ifndef C_FSTREENODE_H
#define C_FSTREENODE_H

#include "fstreeitem.h"

class MFsTreeNodeVisualizer;

/** Default item index in children array */
const TInt KFsTreeChildIndexLast = -1;

/**
 *  CFsTreeNode is a class representing node in tree data structure
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib
 */
NONSHARABLE_CLASS( CFsTreeNode ) : public CFsTreeItem
    {

public: // Construction

    /**
     * Two-phased constructor
     *
     * @param aParent Pointer to the parent node (cannot be NULL!)
     * @param aData Reference to the item data instance
     * @param aVisualizer Reference to the item visualizer instance
     */
    IMPORT_C static CFsTreeNode* NewL( CFsTreeNode& aParent, MFsTreeItemData& aData,
            MFsTreeNodeVisualizer& aVisualizer );

    /**
     * C++ destructor
     */
    virtual ~CFsTreeNode();

public: //node content

    /**
     * Returns reference to the node's visualizer
     *
     * @return Reference to the node's visualizer
     */
    MFsTreeNodeVisualizer* NodeVisualizer() const;

public: // Hierarchy management

    /**
     * Inserts new child item on given position
     *
     * @param aItem  Pointer to the item that is to be inserted.
     * @param aIndex Index (position) of a child, if omitted item is added as
     *               last child.
     *
     * @return KErrNone, if the insertion is successful, otherwise one of the
     *         system wide error codes.
     */
    TInt InsertChild( CFsTreeItem* aItem,
            const TInt aIndex = KFsTreeChildIndexLast );

    /**
     * Removes a child
     *
     * @param aIndex Index of the child to be removed
     *
     * @return Address of the removed item. NULL if index is out of range.
     */
    CFsTreeItem* RemoveChild( const TInt aIndex );

    /**
     * Gets child with given index (position)
     *
     * @param aIndex Index of a child
     *
     * @return Pointer to the child tree item, NULL if the index is out of
     *         range.
     */
    CFsTreeItem* Child( const TInt aIndex ) const;

    /**
     * Gets an index of desired child tree item
     *
     * @param aItem Pointer to the child tree item
     *
     * @return Index (position) of the child, KErrNotFound is returned if item
     *         is not not found
     */
    TInt Index( const CFsTreeItem* aItem ) const;

    /**
     * Returns the number of children for the node
     *
     * @return Number of children
     */
    TUint CountChildren() const;

    /**
     * Returns recursively the number of all children under the node
     * @return Number of children
     */
    TUint CountChildrenRecursively() const;

    /**
     * Checks if tree item is a node
     *
     * @returns ETrue
     */
    virtual TBool IsNode() const;

    /**
     * Return pointer to node object. Returned value is @c NULL, if the item
     * is not derived from @c CFsTreeNode.
     *
     * @return @c CFsTreeNode pointer, if the item is a node.
     */
    virtual CFsTreeNode* Node();

protected: // Construction

    /**
     * C++ constructor
     *
     * @param aParent Pointer to the parent node
     * @param aData Reference to the item data instance
     * @param aVisualizer Reference to the item visualizer instance
     */
    CFsTreeNode( CFsTreeNode* aParent, MFsTreeItemData& aData,
            MFsTreeNodeVisualizer& aVisualizer );

    /**
     * Second phase constructor
     *
     */
    void ConstructL( );

protected:

    /**
     * Array of pointers to node's children
     */
    RFsTreeItemList iChildren;

    };

#endif // C_FSTREENODE_H

