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
* Description:  An observer interface for the tree list component.
*
*/


#ifndef M_FSTREELISTOBSERVER_H
#define M_FSTREELISTOBSERVER_H

#include "fstreelistconstants.h"

/**
 * MFsTreeObserver defines an interface which is used to observe tree list 
 * events.
 *
 * Observer of the tree list is notified of changes in the tree list through 
 * this interface with appropriate events.
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( MFsTreeListObserver )
    {
public:

    enum TFsTreeListEvent
        {
        /** 
         * An item was added to the tree list. 
         * No action required.
         */
        EFsTreeListItemAdded,
        
        /** 
         * A node was added to the tree list. 
         * No action required.
         */
        EFsTreeListNodeAdded,
        
        /** 
         * An item was removed from the tree list. 
         * No action required.
         */
        EFsTreeListItemRemoved,
        
        /** 
         * A node was removed from the tree list.
         * No action required.
         */
        EFsTreeListNodeRemoved,
        
        /**
         * All items and nodes were removed from the tree list.
         * No action required.
         */
        EFsTreeListRemovedAll,
        
        /**
         * Item/Node was moved from one parent to the other in the tree list.
         * No action required.
         */
        EFsTreeListItemMoved,
        
        /**
         * Item's/Node's data has changed.
         * No action required.
         */
        EFsTreeListItemDataChanged,
        
        /**
         * Item data's visualizer has changed.
         * No action required.
         */
        EFsTreeListItemVisualizerChanged,
        
        /**
         * Node data's visualizer has changed.
         * No action required.
         */
        EFsTreeListNodeVisualizerChanged,
        
        // <cmail> "Base class modifications for using touch"
        /**
         * Action started for selected tree item.
         */
        EFsTreeListItemTouchAction,
        
        /**
         * Long tap action started for selected tree item. 
         */
        EFsTreeListItemTouchLongTap,
                
        /**
         * Touch focus for list gained. 
         */
        EFsTreeListItemTouchFocused,    
                
        // </cmail>
                
        /**
         * Item/Node was selected in the tree list. It means that the user
         * pressed a selection (or other, depending on the implementation of
         * the tree list) key on item/node.
         * 
         * No action required.
         */
        EFsTreeListItemSelected,
        
        /**
         * Focus will be moved to the specified item/node.
         * Event triggered just before an item gets focus.
         * No action required.
         */
        EFsTreeListItemWillGetFocused,
        
        /**
         * Focus was moved to the specified item/node.
         * No action required.
         */
        EFsTreeListItemFocused,
        
        /**
         * Item/node was marked.
         * No action required.
         */
        EFsTreeListItemMarked,
        
        /**
         * Item/node was unmarked.
         * No action required.
         */
        EFsTreeListItemUnmarked,
        
        /**
         * Item/node was dimmed.
         * No action required.
         */
        EFsTreeListItemDimmed,
        
        /**
         * Loop event. Focus has moved from the last item item in the list to 
         * the first or form the first to the last.
         * 
         * No action required.
         */
        EFsTreeListLoop,
        
        /**
         * List became visible.
         * No action required.
         */
        EFsTreeListShown,
        
        /**
         * List has been hidden.
         * No action required.
         */
        EFsTreeListHidden,
        
        /**
         * Text size in all the items/nodes in the list has increased.
         * No action required.
         */
        EFsTreeListTextSizeIncreased,
        
        /**
         * Text size in all the items/nodes in the list has decreased.
         * No action required.
         */
        EFsTreeListTextSizeDecreased,
        
        /**
         * Node change state to unrolled.
         * No action required.
         */
        EFsTreeListNodeUnrolled,
        
        /**
         * Node change state to rolled.
         * No action required.
         */
        EFsTreeListNodeRolled,
        
        /**
         * List received focus
         */
        EFsTreeListFocusReceived,

        /**
         * List lost focus
         */          
        EFsTreeListFocusLost,
        
        /**
         * Item has been selected
         */
        EFsTreeItemSelected,
        
        /**
         * Item with menu has been selected
         */ 
        EFsTreeItemWithMenuSelected,
        
        /**
         * Right click on list.
         */
        EFsTreeListKeyRightArrow,
        
        /**
         * Left click on list.
         */
        EFsTreeListKeyLeftArrow,

        // <cmail>

        /**
         * Event generated when a single node is collapsed.
         */
        EFsTreeListItemCollapsed,

        /**
         * Event generated when a single node is expanded.
         */
        EFsTreeListItemExpanded,

        /**
         * Event generated when all nodes are collapsed.
         */
        EFsTreeListCollapsedAll,

        /**
         * Event generated when all nodes are expanded.
         */
        EFsTreeListExpandedAll,
        // </cmail>
        /**
         * Handle focus visibility changes
         */
        EFsFocusVisibilityChange
        };    

public:

    /**
     * Observer of the tree list is notified of changes in the tree list
     * through this function.
     * 
     * @param aEvent Event which happend in the tree list.
     * @param aId Id of the node/item associated with the event. KFsTreeNoneID
     *            means that the event is not associated with a node/item.
     * @param aPoint Event coordinates. Valid only for touch events.
     */
    // <cmail> Touch
    virtual void TreeListEventL( const TFsTreeListEvent aEvent, 
                                 const TFsTreeItemId aId,
                                 const TPoint& aPoint ) = 0;
    // </cmail>
    
    };
    
     
#endif // M_FSTREELISTOBSERVER_H
