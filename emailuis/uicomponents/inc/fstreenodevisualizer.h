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
* Description:  MFsNodeVisualizer is an interface for the node visualizer.
*
*/


#ifndef M_FSTREENODEVISUALIZER_H
#define M_FSTREENODEVISUALIZER_H


#include "fstreeitemvisualizer.h"


/**
 * MFsNodeVisualizer is an interface for the node visualizer.
 *
 * This is an interface for node visualization in the tree. This interface is 
 * based on item visualization interface. This interface adds functions to 
 * change node's visualization state between collapsed and unrolled.
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS ( MFsTreeNodeVisualizer ): public MFsTreeItemVisualizer
    {
public:

    /**
     * C++ destructor
     */
    virtual ~MFsTreeNodeVisualizer() {};    
    
public:

    /**
     * Sets visualized node's state to rolled/unrolled.
     * 
     * @param aFlag ETrue to visualize node as rolled (collapsed).
     */
    virtual void SetExpanded( TBool aFlag, const MFsTreeItemData* aData = 0 ) = 0;
    
    /**
     * Returns rolled/unrolled state of the node's visualization.
     * 
     * @return ETrue if node is visualized as rolled (collapsed).
     */
    virtual TBool IsExpanded() const = 0;
    
    };
        
#endif // M_FSTREENODEVISUALIZER_H
