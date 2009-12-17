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


#ifndef M_FSTREEVISUALIZEROBSERVER_H
#define M_FSTREEVISUALIZEROBSERVER_H

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
NONSHARABLE_CLASS( MFsTreeVisualizerObserver )
    {
public:

    enum TFsTreeVisualizerEvent
        {
        
        // <cmail> "Base class modifications for using touch"
        /**
         * Action started for selected tree item.
         */
        EFsTreeItemTouchAction,
        
        /**
         * Long tap action started for selected tree item. 
         */
        EFsTreeItemTouchLongTap,
                
        /**
         * Touch focus for list gained. 
         */
        EFsTreeItemTouchFocused,    
        
        // </cmail>

        
        /**
         * An item received focus.
         */
        EFsTreeItemFocused,
        
        /**
         * Focus will be moved to the specified item/node.
         * Event triggered just before an item gets focus.
         * No action required.
         */
        EFsTreeItemWillGetFocused,
        
        /**
         * An item has been selected.
         */
        EFsTreeItemSelected,
        /**
         * An item with menu has been selected.
         */
        EFsTreeItemWithMenuSelected,
        /**
         * Right arrow pressed.
         */
        EFsTreeKeyRightArrow,
        /**
         * Left arrow pressed.
         */
        EFsTreeKeyLeftArrow,
        
        /**
         * Event generated when the list is fully hidden.
         */
        EFsTreeListVisualizerHidden,
        
        /**
         * Event generated when the list is fully shown.
         */
        EFsTreeListVisualizerShown,

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
        EFsTreeListExpandedAll
        // </cmail>
        };    

public:

    /**
     * Observer of the tree list is notified of changes in the tree list
     * through this function.
     * 
     * @param aEvent Event which happend in the tree list.
     * @param aId Id of the node/item associated with the event. KFsTreeNoneID
     *            means that the event is not associated with a node/item.
     */
    // <cmail> Touch
    virtual void TreeVisualizerEventL( 
            const TFsTreeVisualizerEvent aEvent, 
            const TFsTreeItemId aId = KFsTreeNoneID ) const = 0;
    // </cmail>
    
    };
    
     
#endif // M_FSTREEVISUALIZEROBSERVER_H
