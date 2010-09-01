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
* Description:  Freestyle tree item implementation
*
*/

#include "emailtrace.h"
#include <e32const.h>

#include "fstreeitem.h"
#include "fstreeitemdata.h"
#include "fstreeitemvisualizer.h"
#include "fstreenode.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeItem* CFsTreeItem::NewL( CFsTreeNode& aParent,
    MFsTreeItemData& aData, MFsTreeItemVisualizer& aVisualizer )
    {
    FUNC_LOG;
    CFsTreeItem* self = CFsTreeItem::NewLC( aParent, aData, aVisualizer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeItem* CFsTreeItem::NewLC( CFsTreeNode& aParent,
    MFsTreeItemData& aData, MFsTreeItemVisualizer& aVisualizer )
    {
    FUNC_LOG;
    CFsTreeItem* self = new( ELeave ) CFsTreeItem( &aParent, aData,
                                                   aVisualizer );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreeItem::~CFsTreeItem()
    {
    FUNC_LOG;
    delete iData;
    delete iVisualizer;
    }

// ---------------------------------------------------------------------------
// Gets reference to item visualizer.
// ---------------------------------------------------------------------------
//
MFsTreeItemVisualizer* CFsTreeItem::ItemVisualizer() const
	{
    FUNC_LOG;
	return iVisualizer;
	}

// ---------------------------------------------------------------------------
// Gets reference to item data.
// ---------------------------------------------------------------------------
//
MFsTreeItemData& CFsTreeItem::ItemData() const
	{
    FUNC_LOG;
	return *iData;
	}

// ---------------------------------------------------------------------------
// Sets item visualizer object.
// ---------------------------------------------------------------------------
//
void CFsTreeItem::SetItemVisualizer( MFsTreeItemVisualizer& aVisualizer )
	{
    FUNC_LOG;
	iVisualizer = &aVisualizer;
	}

// ---------------------------------------------------------------------------
// Sets item data object.
// ---------------------------------------------------------------------------
//
void CFsTreeItem::SetItemData( MFsTreeItemData& aData )
	{
    FUNC_LOG;
	iData = &aData;
	}


// ---------------------------------------------------------------------------
// Returns EFalse since item is not a node.
// ---------------------------------------------------------------------------
//
TBool CFsTreeItem::IsNode() const
	{
    FUNC_LOG;
	return EFalse;
	}

// ---------------------------------------------------------------------------
// Returns NULL since the item is not a node.
// ---------------------------------------------------------------------------
//
CFsTreeNode* CFsTreeItem::Node()
    {
    FUNC_LOG;
    return NULL;
    }

// ---------------------------------------------------------------------------
// Gets the pointer to parent node of the item.
// ---------------------------------------------------------------------------
//
CFsTreeNode* CFsTreeItem::Parent() const
    {
    FUNC_LOG;
    return iParent;
    }

// ---------------------------------------------------------------------------
// Sets parent node for the item.
// ---------------------------------------------------------------------------
//
void CFsTreeItem::SetParent( CFsTreeNode& aParent )
	{
    FUNC_LOG;
	iParent = &aParent;
	}

// ---------------------------------------------------------------------------
// Returns the level of the item in tree structure.
// ---------------------------------------------------------------------------
//
TUint CFsTreeItem::Level() const
    {
    FUNC_LOG;
    TUint itemLevel = 0;

    CFsTreeNode* parent = iParent;
    while ( parent )
        {
        itemLevel++;
        parent = parent->Parent();
        }

    return itemLevel;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint CFsTreeItem::CountChildrenRecursively() const
    {
    // return always 0 here.
    // NOTE: this is overridden in child class CFsTreeNode!
    return 0;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreeItem::CFsTreeItem( CFsTreeNode* aParent, MFsTreeItemData& aData,
    MFsTreeItemVisualizer& aVisualizer )
    : iParent( aParent ),
      iData( &aData ),
      iVisualizer( &aVisualizer )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreeItem::ConstructL( )
    {
    FUNC_LOG;
    }
