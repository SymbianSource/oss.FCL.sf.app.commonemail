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
#include "fstreenode.h"
#include "fstreenodevisualizer.h"
#include "fsgenericpanic.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeNode* CFsTreeNode::NewL( CFsTreeNode& aParent,
    MFsTreeItemData& aData, MFsTreeNodeVisualizer& aVisualizer )
    {
    FUNC_LOG;
    CFsTreeNode* self = new( ELeave ) CFsTreeNode( &aParent, aData,
            aVisualizer );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreeNode::~CFsTreeNode()
    {
    FUNC_LOG;
    iChildren.Close();
    }

// ---------------------------------------------------------------------------
// Gets node visualizer.
// ---------------------------------------------------------------------------
//
MFsTreeNodeVisualizer* CFsTreeNode::NodeVisualizer() const
    {
    FUNC_LOG;
    return ( static_cast<MFsTreeNodeVisualizer*>(iVisualizer) );
    }

// ---------------------------------------------------------------------------
// Inserts child with given index.
// ---------------------------------------------------------------------------
//
TInt CFsTreeNode::InsertChild( CFsTreeItem* aItem, const TInt aIndex )
    {
    FUNC_LOG;
    TInt childIndex = 0;

    if ( aIndex == KFsTreeChildIndexLast )
        {
        childIndex = iChildren.Append( aItem );
        }
    else
        {
        childIndex = iChildren.Insert( aItem, aIndex );
        }

    return childIndex;
    }

// ---------------------------------------------------------------------------
// Removes child with given index.
// ---------------------------------------------------------------------------
//
CFsTreeItem* CFsTreeNode::RemoveChild( const TInt aIndex )
    {
    FUNC_LOG;
    CFsTreeItem* childItem = NULL;

    childItem = Child( aIndex );
    if (childItem)
        {
        iChildren.Remove( aIndex );
        }

    return childItem;
    }

// ---------------------------------------------------------------------------
// Gets child with given index (position).
// ---------------------------------------------------------------------------
//
CFsTreeItem* CFsTreeNode::Child( const TInt aIndex ) const
    {
    FUNC_LOG;
    CFsTreeItem* treeItem = NULL;

    if (( aIndex < iChildren.Count() ) && ( aIndex >= 0 ))
        {
        treeItem = iChildren[aIndex];
        }
    else
        {
        FsGenericPanic(EFsListPanicIndexOutOfRange);
        }

    return treeItem;
    }

// ---------------------------------------------------------------------------
// Gets an index of desired child tree item.
// ---------------------------------------------------------------------------
//
TInt CFsTreeNode::Index( const CFsTreeItem* aItem ) const
    {
    FUNC_LOG;
    return iChildren.Find(aItem);
    }

// ---------------------------------------------------------------------------
// Returns the number of children for the node.
// ---------------------------------------------------------------------------
//
TUint CFsTreeNode::CountChildren() const
    {
    FUNC_LOG;
    return iChildren.Count();
    }

// ---------------------------------------------------------------------------
// Returns recursively the number of all children under the node
// ---------------------------------------------------------------------------
//
TUint CFsTreeNode::CountChildrenRecursively() const
    {
    FUNC_LOG;
    TInt count ( 0 );
    for ( TInt i = 0; i < iChildren.Count(); i++ )
        {
        count++;
        count += iChildren[i]->CountChildrenRecursively();
        }
    return count;
    }

// ---------------------------------------------------------------------------
// Checks if tree item is a node.
// ---------------------------------------------------------------------------
TBool CFsTreeNode::IsNode() const
    {
    FUNC_LOG;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Return pointer to node object. Returned value is @c NULL, if the item
// is not derived from @c CFsTreeNode.
// ---------------------------------------------------------------------------
//
CFsTreeNode* CFsTreeNode::Node()
    {
    FUNC_LOG;
    return this;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreeNode::CFsTreeNode( CFsTreeNode* aParent, MFsTreeItemData& aData,
    MFsTreeNodeVisualizer& aVisualizer )
    : CFsTreeItem( aParent, aData, aVisualizer )
    {
    FUNC_LOG;

    }


// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreeNode::ConstructL( )
    {
    FUNC_LOG;

    }

