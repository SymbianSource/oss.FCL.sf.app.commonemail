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


#ifndef C_FSTREELISTCONSTANTS_H
#define C_FSTREELISTCONSTANTS_H

#include <e32def.h>
#include <e32cmn.h>
#include <e32math.h>

/** Item ID for hierarchical list items. */
typedef TInt TFsTreeItemId;
//TFsItemID

/** Constant Item ID for non-existent hierarchical list item. */
const TFsTreeItemId KFsTreeNoneID = 0;

/** Constant Item ID for the hierarchical list root node. */
const TFsTreeItemId KFsTreeRootID = 1;
 
/** Array of tree item ids */
typedef RArray<TFsTreeItemId> RFsTreeItemIdList;

enum TFsTreeListLoopingType
    {
    /**
     * Looping turned off.
     */
    EFsTreeListLoopingDisabled,
    /**
     * List is effectively a loop so that the first item appears in the list 
     * after the last item - moving in the list is smooth, there is no need 
     * to jump when moving past end or start.
     */
    EFsTreeListLoopingSmooth,
    /**
     * When scrolling down from the last item, focus jumps to the first item 
     * (normal behaviour in e.g. S60 General Settings)
     */
    EFsTreeListLoopingJumpToFirstLast
    };


enum TFsTextMarqueeType
    {
    /**
     * The text does not scroll.
     */
    EFsTextMarqueeNone,
    /**
     * The text scrolls until its end.
     */
    EFsTextMarqueeForth,
    /**
     * The text scrolls forwards and backwards.
     */
    EFsTextMarqueeForthBack,
    /**
     * The text scrolls in a loop.
     */
    EFsTextMarqueeLoop
    };



#endif  // C_FSTREELISTCONSTANTS_H
 
