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
* Description:  An observer interface for the tree data model.
*
*/


#ifndef M_FSTREEOBSERVER_H
#define M_FSTREEOBSERVER_H


/**
 * MFsTreeObserver defines an interface which is used to observe tree events.
 * 
 * Observer of the tree is notified of changes in the tree structure
 * through this interface with appropriate events.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( MFsTreeObserver )
    {
public:

    enum TFsTreeEvent
        {
        /** 
         * Item was added to the tree. 
         * The visible items list should be updated if needed.
         */
        EFsTreeItemAdded,
        
        /** 
         * Node was added to the tree. 
         * The visible items list should be updated if needed.
         */
        EFsTreeNodeAdded,
        
        /**
         * Item was removed from the tree.
         * The visible items list should be updated if needed.
         */
        EFsTreeItemRemoved,
        
        /**
         * Node was removed from the tree.
         * The visible items list should be updated if needed.
         */
        EFsTreeNodeRemoved,
        
        /**
         * All items will be removed from the tree.
         * The visible items list should be updated..
         */
        EFsTreeRemovedAll,
        
        /**
         * Item's/Node's data has changed.
         * The visible items list should be updated if needed.
         */
        EFsTreeItemDataChanged,
        
        /**
         * Item data's visualizer has changed.
         * The visible items list should be updated if needed.
         */
        EFsTreeItemVisualizerChanged
        };
    
public:
    
    /**
     * Observer of the tree is notified of changes in the tree structure
     * through this function.
     * 
     * @param aEvent Event which happend in the tree structure.
     * @param aId Id of the node/item associated with the event.
     */
    struct TFsTreeEventParams
        {
        TFsTreeEventParams(TFsTreeItemId aItemId, TFsTreeItemId aTargetNodeId, TInt aIndex ) :
        iItemId(aItemId), iTargetNodeId(aTargetNodeId), iIndex(aIndex) {};
        
        TFsTreeItemId iItemId;
        TFsTreeItemId iTargetNodeId; 
        TInt iIndex;
        };
    
    virtual void TreeEventL( const TFsTreeEvent aEvent, 
            const TFsTreeEventParams& aParams ) = 0;
    
    };
    
     
#endif // M_FSTREEOBSERVER_H
