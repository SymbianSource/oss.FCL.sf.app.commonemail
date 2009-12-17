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
* Description:  Freestyle tree node implementation.
*
*/

#include "emailtrace.h"
#include "fstree.h"
#include "fstreeiterator.h"
#include "fstreeobserver.h"
#include "fsgenericpanic.h"
#include "fstreenodevisualizer.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTree* CFsTree::NewL(  MFsTreeObserver& aObserver,
                                  MFsTreeItemData& aItemD,
                                  MFsTreeNodeVisualizer& aItemV )
    {
    FUNC_LOG;
    CFsTree* self = new (ELeave) CFsTree( aObserver, aItemD, aItemV );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTree::~CFsTree()
    {
    FUNC_LOG;
    iItems.ResetAndDestroy();
    iItems.Close();
    }

// ---------------------------------------------------------------------------
// Inserts new item as a child of parent given by Id at the given position.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTree::InsertItemL( MFsTreeItemData& aItemD,
                                    MFsTreeItemVisualizer& aItemV,
                                    const TFsTreeItemId aParentId,
                                    const TInt aIndex )
    {
    FUNC_LOG;
    CFsTreeNode* parentNode = Node( aParentId );

    CFsTreeItem* item = CFsTreeItem::NewLC( *parentNode, aItemD, aItemV );

    TFsTreeItemId itemId = InsertL( item, parentNode, aIndex );

    NotifyObserverL( MFsTreeObserver::EFsTreeItemAdded, MFsTreeObserver::TFsTreeEventParams(itemId, aParentId, aIndex) );

    CleanupStack::Pop( item );

    return itemId;
    }

// ---------------------------------------------------------------------------
// Inserts new node as a child of parent given by Id at the given position.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTree::InsertNodeL( MFsTreeItemData& aItemD,
		                            MFsTreeNodeVisualizer& aItemV,
                                    const TFsTreeItemId aParentId,
                                    const TInt aIndex )
    {
    FUNC_LOG;
    CFsTreeNode* parentNode = Node( aParentId );

    CFsTreeNode* node = CFsTreeNode::NewL( *parentNode, aItemD, aItemV );

    CleanupStack::PushL( node );

    TFsTreeItemId itemId = InsertL( node, parentNode, aIndex );

    NotifyObserverL( MFsTreeObserver::EFsTreeNodeAdded, MFsTreeObserver::TFsTreeEventParams(itemId, aParentId, aIndex) );

    CleanupStack::Pop( node );

    return itemId;
    }

// ---------------------------------------------------------------------------
// Removes the item with given Id from the list. If aItemId is a node all
// its children will be removed as well.
// ---------------------------------------------------------------------------
//
TBool CFsTree::RemoveL( const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    if ( aItemId != KFsTreeRootID )
    //do not try to delete the tree itself
        {
        CFsTreeItem* removedItem = Item( aItemId );

        CFsTreeNode* parentNode = removedItem->Parent();

        //check if last item is removed
        if ((parentNode==this) && (parentNode->CountChildren()==1))
            {
            iTreeObserver.TreeEventL( MFsTreeObserver::EFsTreeRemovedAll,
                    MFsTreeObserver::TFsTreeEventParams(KFsTreeNoneID, KFsTreeNoneID, -1));
            }
        else
            {
            iTreeObserver.TreeEventL( MFsTreeObserver::EFsTreeItemRemoved,
                    MFsTreeObserver::TFsTreeEventParams(aItemId, Id(parentNode), -1));
            }


        if (removedItem->IsNode())
            {
            // <cmail>
            RFsTreeItemList itemsToBeDeleted;
            TFsTreeIterator iter = Iterator( aItemId, aItemId );
            CFsTreeNode* removedNode = static_cast<CFsTreeNode*>(removedItem);
            // </cmail>

            while (iter.HasNext())
                {
                CFsTreeItem* iterItem = Item(iter.Next());
                itemsToBeDeleted.Append( iterItem );
                iItems.Remove( iItems.FindInAddressOrder(iterItem) );
                }
            itemsToBeDeleted.ResetAndDestroy();
            //remove children
            for (TInt childIndex=removedNode->CountChildren()-1;
                    childIndex>=0;
                    childIndex--)
                {
                removedNode->RemoveChild(childIndex);
                }
            }

        parentNode->RemoveChild( parentNode->Index(removedItem) );
        iItems.Remove( iItems.FindInAddressOrder(removedItem) );
        delete removedItem;
        }
    else
        {
        if (iItems.Count() > 0)
            {
            iTreeObserver.TreeEventL( MFsTreeObserver::EFsTreeRemovedAll,
                    MFsTreeObserver::TFsTreeEventParams(KFsTreeNoneID, KFsTreeNoneID, -1));
            iItems.ResetAndDestroy();
            iChildren.Reset();
            }
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// Moves the specified item to the specified target node.
// ---------------------------------------------------------------------------
//
TBool CFsTree::MoveL( const TFsTreeItemId aItemId,
                     const TFsTreeItemId aTargetNodeId,
                     const TInt aIndex )
    {
    FUNC_LOG;
    CFsTreeItem* movedItem = Item( aItemId );
    CFsTreeNode* parentNode = movedItem->Parent();
    CFsTreeNode* targetParent = Node( aTargetNodeId );

    iTreeObserver.TreeEventL(MFsTreeObserver::EFsTreeItemRemoved, MFsTreeObserver::TFsTreeEventParams(aItemId, aTargetNodeId, aIndex));
    parentNode->RemoveChild(parentNode->Index(movedItem));

    targetParent->InsertChild(movedItem, aIndex);
    movedItem->SetParent(*targetParent);
    iTreeObserver.TreeEventL(MFsTreeObserver::EFsTreeItemAdded, MFsTreeObserver::TFsTreeEventParams(aItemId, aTargetNodeId, aIndex));

    if (movedItem->IsNode())
        {
        CFsTreeNode* node = Node(aItemId);
        for (TUint childNo=0; childNo<node->CountChildren(); childNo++)
            {
            iTreeObserver.TreeEventL(MFsTreeObserver::EFsTreeItemAdded,
                    MFsTreeObserver::TFsTreeEventParams(Id(node->Child(childNo)), aTargetNodeId, aIndex));
            }
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// Function returns number of elements in the tree.
// ---------------------------------------------------------------------------
//
TUint CFsTree::Count() const
    {
    FUNC_LOG;
    return iItems.Count();
    }

// ---------------------------------------------------------------------------
// Function checks whether an item with a given id exists in the tree.
// ---------------------------------------------------------------------------
//
TBool CFsTree::Contains( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    TBool itemFound = EFalse;

    TInt index = iItems.FindInAddressOrder(
                                    reinterpret_cast<CFsTreeItem*>( aItemId ) );
    if ( index != KErrNotFound )
        {
        itemFound = ETrue;
        }

    return itemFound;
    }

// ---------------------------------------------------------------------------
// Creates an iterator object set to point at the specified item in the tree.
// ---------------------------------------------------------------------------
//
TFsTreeIterator CFsTree::Iterator( const TFsTreeItemId aNodeId,
                                   const TFsTreeItemId aCurrentItemId,
                                   const TUint aFlags )
    {
    FUNC_LOG;
    CFsTreeNode* node = Node( aNodeId );
    CFsTreeItem* currentItem = Item( aCurrentItemId );

    return TFsTreeIterator( node, this, currentItem, aFlags );
    }

// ---------------------------------------------------------------------------
// Checks whether the item with specified id is a node.
// ---------------------------------------------------------------------------
//
TBool CFsTree::IsNode( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;

    CFsTreeItem* item = Item(aItemId);

    return ( item ? Item(aItemId)->IsNode() : EFalse );
    }

// ---------------------------------------------------------------------------
// Gets the number of children of the given node.
// ---------------------------------------------------------------------------
//
TUint CFsTree::CountChildren( const TFsTreeItemId aNodeId ) const
    {
    FUNC_LOG;
    return Node( aNodeId )->CountChildren();
    }

// ---------------------------------------------------------------------------
// Gets recursively the number of all children under the given node
// ---------------------------------------------------------------------------
//
TUint CFsTree::CountChildrenRecursively( const TFsTreeItemId aNodeId ) const
    {
    FUNC_LOG;
    return Node( aNodeId )->CountChildrenRecursively();
    }

// ---------------------------------------------------------------------------
// Gets the id of a child of the specified node with the position given.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTree::Child( const TFsTreeItemId aNodeId,
        const TUint aIndex ) const
    {
    FUNC_LOG;
    return Id( Node(aNodeId)->Child(aIndex) );
    }

// ---------------------------------------------------------------------------
// Returns the index of a child for a given parent.
// ---------------------------------------------------------------------------
//
TInt CFsTree::ChildIndex( const TFsTreeItemId aNodeId,
                           const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return Node(aNodeId)->Index(Item(aItemId));
    }


// ---------------------------------------------------------------------------
// Gets a pointer to the node visualizer (const).
// ---------------------------------------------------------------------------
//
MFsTreeNodeVisualizer* CFsTree::NodeVisualizer(
                                           const TFsTreeItemId aNodeId ) const
    {
    FUNC_LOG;
    return Node( aNodeId )->NodeVisualizer();
    }


// ---------------------------------------------------------------------------
// Sets the reference to the node visualizer.
// ---------------------------------------------------------------------------
//
void CFsTree::SetNodeVisualizer( MFsTreeItemVisualizer& aVisualizer,
                                 const TFsTreeItemId aNodeId )
    {
    FUNC_LOG;
    Node(aNodeId)->SetItemVisualizer(aVisualizer);
    }

// ---------------------------------------------------------------------------
// Gets the parent's id for the specified item. If KFsTreeRootID is passed
// then KFsTreeNoneID is returned.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTree::Parent( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    TFsTreeItemId parentId;

    if ( aItemId == KFsTreeRootID )
        parentId = KFsTreeNoneID;
    else
        parentId = Id(Item(aItemId)->Parent());

    return parentId;
    }

// ---------------------------------------------------------------------------
// Gets a reference to the item visualizer (const).
// ---------------------------------------------------------------------------
//
MFsTreeItemVisualizer* CFsTree::ItemVisualizer( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    CFsTreeItem* item = Item( aItemId );
    MFsTreeItemVisualizer* visualizer = NULL;

    if (item)
        {
        visualizer = item->ItemVisualizer();
        }

    return visualizer;
    }

// ---------------------------------------------------------------------------
// Gets a reference to the item/node data (const).
// ---------------------------------------------------------------------------
//
MFsTreeItemData& CFsTree::ItemData( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return Item( aItemId )->ItemData();
    }

// ---------------------------------------------------------------------------
// Sets a reference to the item visualizer.
// ---------------------------------------------------------------------------
//
void CFsTree::SetItemVisualizer( MFsTreeItemVisualizer& aVisualizer,
                                 const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    Item(aItemId)->SetItemVisualizer(aVisualizer);
    }

// ---------------------------------------------------------------------------
// Sets the reference to the item/node data.
// ---------------------------------------------------------------------------
//
void CFsTree::SetItemData( MFsTreeItemData& aData,
                           const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    Item(aItemId)->SetItemData(aData);
    }

// ---------------------------------------------------------------------------
// Returns the ID assigned for the specified tree item.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTree::Id( const CFsTreeItem* aItem ) const
    {
    FUNC_LOG;
    TFsTreeItemId id = KFsTreeNoneID;

    if (aItem == this)
        {
        id = KFsTreeRootID;
        }
    else
        if (aItem == NULL)
            {
            id = KFsTreeNoneID;
            }
        else
            {
            id = reinterpret_cast<TInt>( aItem );
            }

    return id;
    }

// ---------------------------------------------------------------------------
// Returns the level of the item in the tree.
// ---------------------------------------------------------------------------
//
TUint CFsTree::Level( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    CFsTreeItem* item = Item( aItemId );
    return item->Level();
    }

// ---------------------------------------------------------------------------
// Internal use.Retrieves the specified item from the tree structure.
// ---------------------------------------------------------------------------
//
CFsTreeItem* CFsTree::Item( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    CFsTreeItem* item = NULL;

    switch (aItemId)
        {
        case KFsTreeRootID :
            item = const_cast<CFsTree*>( this );
            break;

        case KFsTreeNoneID :
            //FsGenericPanic(EFsListPanicInvalidItemId);
            break;

        default :
            TInt index = iItems.FindInAddressOrder(
                                    reinterpret_cast<CFsTreeItem*>( aItemId ) );
            if ( index != KErrNotFound )
                {
                item = iItems[index];
                }
            else
                {
                //FsGenericPanic(EFsListPanicInvalidItemId);
                }
            break;
        }

    return item;
    }

// ---------------------------------------------------------------------------
// Internal use.Retrieves the specified node from the tree structure.
// ---------------------------------------------------------------------------
//
CFsTreeNode* CFsTree::Node( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    CFsTreeItem* item = Item(aItemId);
    CFsTreeNode* node = NULL;

    if (item->IsNode())
        {
        node = item->Node();
        }
    else
        {
        FsGenericPanic(EFsListPanicInvalidItemType);
        }

    return node;
    }

// ---------------------------------------------------------------------------
// Notifies the tree observer about an event in the tree.
// ---------------------------------------------------------------------------
//
void CFsTree::NotifyObserverL( const MFsTreeObserver::TFsTreeEvent aEvent,
        const MFsTreeObserver::TFsTreeEventParams& aParams )
    {
    FUNC_LOG;
    iTreeObserver.TreeEventL( aEvent, aParams );
    }

// ---------------------------------------------------------------------------
// Internal use.Inserts given item into given parent at specified positon.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTree::InsertL( CFsTreeItem* aItem,
                               CFsTreeNode* aParent,
                               TInt aIndex )
    {
    FUNC_LOG;
    TInt error = 0;

    error = aParent->InsertChild( aItem, aIndex );
    User::LeaveIfError( error );
    //the if statement to avoid multiple entries
    if (iItems.Find(aItem) == KErrNotFound)
        {
        //store item's pointer in the tree's lookup array
        error = iItems.InsertInAddressOrder( aItem );
        User::LeaveIfError( error );
        }

    return Id( aItem );
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTree::CFsTree( MFsTreeObserver& aObserver,
                  MFsTreeItemData& aItemD,
                  MFsTreeNodeVisualizer& aItemV )
    :   CFsTreeNode(NULL, aItemD, aItemV),
        iTreeObserver( aObserver )
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTree::ConstructL(  )
    {
    FUNC_LOG;

    }

