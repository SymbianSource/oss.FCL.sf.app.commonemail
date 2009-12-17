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
* Description:  Data structure class for tree list component
 *
*/


#ifndef C_FSTREE_H
#define C_FSTREE_H

#include <e32base.h>

#include "fstreelistconstants.h"
#include "fstreeobserver.h"
#include "fstreenode.h"
#include "fstreeiterator.h"

class MFsTreeNodeVisualizer;
class MFsTreeItemData;
class MFsTreeItemVisualizer;

/**
 *  Data structure class for the tree list component.
 *  It is a specialized tree node (root of a tree), with additional methods
 *  useful for executing tree operation based on item ids, that are
 *  visible for the component user.
 *
 *
 *  @lib
 */
NONSHARABLE_CLASS( CFsTree ) : private CFsTreeNode
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aObserver Reference to the tree observer object
     * @param aItemD Reference to the tree item data object, that is used
     *               to store data displayed when the list is empty
     * @param aItemV Reference to the node visualizer object that is used
     *               to display info that the list is empty
     *
     * @return Pointer to the new tree object
     */
    IMPORT_C static CFsTree* NewL( MFsTreeObserver& aObserver,
            MFsTreeItemData& aItemD,
            MFsTreeNodeVisualizer& aItemV );

    /**
     * C++ destructor
     */
    virtual ~CFsTree();

public: // general tree management

    /**
     * Inserts new item as a child of parent given by Id at the given
     * position.
     *
     * @param aItemD Pointer to item's data
     * @param aItemV Pointer to item's visualizer
     * @param aParentId Id of parent node
     * @param aIndex Position of new item in the list of parent's children
     *               (if omitted, the item is inserted as last child)
     *
     * @return Id of the inserted item
     *
     * @panic EFsListPanicParentIdNotFound The specified parent id was not
     *                                      found
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                      number of children
     */
    TFsTreeItemId InsertItemL( MFsTreeItemData& aItemD,
            MFsTreeItemVisualizer& aItemV,
            const TFsTreeItemId aParentId,
            const TInt aIndex = KFsTreeChildIndexLast );

    /**
     * Inserts new node as a child of parent given by Id at the given
     * position.
     *
     * @param aItemD Pointer to node's data
     * @param aItemV Pointer to node's visualizer
     * @param aParentId Id of parent node
     * @param aIndex Position of new node in the list of parent's children
     *               (if omitted, the node is inserted as last)
     *
     * @return Id of the inserted node
     *
     * @panic EFsListPanicParentIdNotFound The specified parent id was not
     *                                      found
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                      number of children
     */
    TFsTreeItemId InsertNodeL( MFsTreeItemData& aItemD,
            MFsTreeNodeVisualizer& aItemV,
            const TFsTreeItemId aParentId,
            const TInt aIndex = KFsTreeChildIndexLast );

    /**
     * Removes the item with given Id from the list. If aItemId is a node all
     * its children will be removed as well.
     *
     * @param aItemId Id of the item to be removed
     *
     * @return Returns ETrue if operation was succeded, EFalse otherwise
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the tree.
     */
    TBool RemoveL( const TFsTreeItemId aItem );

    /**
     * Moves the specified item to the specified target node. The constant
     * @c KFsTreeRootID can be used as the ID of target node, when moving
     * the item to the top-most level of the tree.
     *
     * @param aItemId ID of the item to be moved.
     * @param aTargetNodeId ID of the node, where the item is to be moved.
     * @param aIndex Index in the target node's children array where item
     *               should be placed.
     *
     * @return Returns ETrue if operation was succeded, EFalse otherwise
     *
     * @panic EFsListPanicInvalidItemId Specified item id was not found
     *                                  in the tree.
     * @panic EFsListPanicInvalidItemType Specified target node is not a node.
     */
    TBool MoveL( const TFsTreeItemId aItemId,
            const TFsTreeItemId aTargetNodeId,
            const TInt aIndex = KFsTreeChildIndexLast );

    /**
     * Function returns the number of elements in the tree.
     *
     * @return Number of elements in the tree.
     */
    TUint Count() const;

    /**
     * Function checks whether an item with a given id exists in the tree.
     *
     * @param aItemId Id of an item to look for
     *
     * @return ETrue if an item with a given id exists in the tree.
     */
    TBool Contains( const TFsTreeItemId aItemId ) const;

    /**
     * Creates an iterator object set to point at the specified node in
     * the tree.
     *
     * @param aNodeId Id of the node that the iterator will initially point
     *                at. If omitted, root id is passed.
     *
     * @return Iterator object, pointing to the node with given id.
     *
     * @panic EFsListPanicInvalidItemId Specified item id was not found
     *                                  in the tree.
     */
    TFsTreeIterator Iterator(const TFsTreeItemId aNodeId = KFsTreeRootID,
            const TFsTreeItemId aCurrentItemId = KFsTreeRootID,
            const TUint aFlags = KFsTreeIteratorNullFlag );

    /**
     * Checks whether the item with specified id is a node.
     *
     * @param aItemId Id of an item to check.
     *
     * @return ETrue if the item is a node, EFalse otherwise.
     *
     * @panic EFsListPanicInvalidItemId Specified item id was not found
     *                                  in the tree.
     */
    TBool IsNode( const TFsTreeItemId aItemId ) const;

    /**
     * Gets the number of children of the given node.
     *
     * @param aNodeId Id of the node.
     *
     * @return Number of node's children.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                                    node id.
     */
    TUint CountChildren( const TFsTreeItemId aNodeId ) const;

    /**
     * Gets recursively the number of all children under the given node.
     * @param aNodeId Id of the node.
     * @return Number of node's children.
     */
    TUint CountChildrenRecursively( const TFsTreeItemId aNodeId ) const;

    /**
     * Gets the id of a child of the specified node with the position given.
     *
     * @param aNodeId Id of a node.
     * @param aIndex Index of the child.
     *
     * @return Id of a child.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found in
     *                  the list.
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                  node id.
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                    number of children.
     */
    TFsTreeItemId Child( const TFsTreeItemId aNodeId,
            const TUint aIndex ) const;

    /**
     * Returns the index of a child for a given parent.
     *
     * @param aNodeId Id of a node.
     * @param aItemId Id of a child.
     *
     * @return Index of a child.
     */
    TInt ChildIndex( const TFsTreeItemId aNodeId,
            const TFsTreeItemId aItemId ) const;

    /**
     * Gets a pointer to the node visualizer.
     *
     * @param aNodeId Id of the node.
     *
     * @return Pointer to the item visualizer.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                  node id.
     */
    MFsTreeNodeVisualizer* NodeVisualizer(const TFsTreeItemId aNodeId) const;

    /**
     * Sets the reference to the node visualizer.
     *
     * @param aVisualizer Reference to the new node's visualizer
     * @param aNodeId Id of the node
     *
     * @panic EFsListLeaveInvalidItemId The specified item id was not found
     *                                  in the list.
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                          node id.
     */
    void SetNodeVisualizer( MFsTreeItemVisualizer& aVisualizer,
            const TFsTreeItemId aNodeId );

    /**
     * Gets the parent's id for the specified item. If KFsTreeRootID is passed
     * then KFsTreeNoneID is returned.
     *
     * @param aItemId Id of an item.
     *
     * @return Id of item's parent.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    TFsTreeItemId Parent( const TFsTreeItemId aItemId ) const;

    /**
     * Gets a reference to the item visualizer.
     *
     * @param aItemId Id of the item.
     *
     * @return Reference to the item visualizer.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    MFsTreeItemVisualizer* ItemVisualizer(const TFsTreeItemId aItemId) const;

    /**
     * Gets a reference to the item/node data.
     *
     * @param aItemId Id of the item/node.
     *
     * @return Reference to the item/node data.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    MFsTreeItemData& ItemData( const TFsTreeItemId aItemId ) const;

    /**
     * Sets a reference to the item visualizer.
     *
     * @param aVisualizer Reference to the new item's visualizer.
     * @param aItemId Id of the item.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    void SetItemVisualizer( MFsTreeItemVisualizer& aVisualizer,
            const TFsTreeItemId aItemId );

    /**
     * Sets the reference to the item/node data.
     *
     * @param aData Reference to the item/node data.
     * @param aItemId Id of the item/node.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    void SetItemData( MFsTreeItemData& aData, const TFsTreeItemId aItemId );

    /**
     * Returns the ID assigned for the specified tree item.
     *
     * @param aItem An item which id will be returned.
     *
     * @return Id of the specified item.
     */
    TFsTreeItemId Id( const CFsTreeItem* aItem ) const;

    /**
     * Returns the level of the item in the tree. The returned value matches
     * the number of parents in the items parent chain.
     *
     * @param aItemId Id of the item for which the level should be returned.
     *
     * @return The level of the item in the tree.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    TUint Level( const TFsTreeItemId aItemId ) const;

private: // internal methods for operating on the tree

    /**
     * Retrieves the specified item from the tree structure.
     *
     * @param aItemId ID of the item to be retrieved.
     *
     * @return The retrieved tree item. NULL if the item is not in the tree.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    CFsTreeItem* Item( const TFsTreeItemId aItemId ) const;

    /**
     * Retrieves the specified node from the tree structure.
     *
     * @param aNodeId ID of the node to be retrieved.
     *
     * @return The retrieved tree node. NULL if the node is not in the tree or
     *         it is not a node, but item.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                              node id.
     */
    CFsTreeNode* Node( const TFsTreeItemId aItemId ) const;

    /**
     * Notifies the tree observer about an event in the tree.
     *
     * @param aEvent Tree event.
     * @param aItemId    Id of an element associated with the event.
     */
    void NotifyObserverL( const MFsTreeObserver::TFsTreeEvent aEvent,
            const MFsTreeObserver::TFsTreeEventParams& aParams );

    /**
     * Inserts given item into given parent at specified positon.
     *
     * @param aItem Item to be inserted.
     * @param aParent A node into which given item should be inserted.
     * @param aIndex An index at which given item should be inserted.If omited
     *               then item is placed as the last child.
     *
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                    number of children
     */
    TFsTreeItemId InsertL( CFsTreeItem* aItem,
                          CFsTreeNode* aParent,
                          TInt aIndex = KFsTreeChildIndexLast );

private:

    /**
     * C++ constructor
     */
    CFsTree( MFsTreeObserver& aObserver,
             MFsTreeItemData& aItemD,
             MFsTreeNodeVisualizer& aItemV );

    /**
     * Second phase constructor
     */
    void ConstructL( );

private: //member data

    /**
     * A observer for the tree.
     */
    MFsTreeObserver& iTreeObserver;

    /**
     * Pointer array containin pointers to all tree items.
     * Not owned.
     */
    RFsTreeItemList iItems;
    };

#endif  // C_FSTREE_H

