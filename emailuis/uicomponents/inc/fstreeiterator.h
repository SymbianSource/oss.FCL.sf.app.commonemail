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
* Description:  Iterator class for traversing the tree structure.
*
*/


#ifndef T_FSTREEITERATOR_H
#define T_FSTREEITERATOR_H


#include <e32def.h>

#include "fstreelistconstants.h"


class CFsTree;
class CFsTreeItem;
class CFsTreeNode;


const TUint KFsTreeIteratorNullFlag = 0x00;

/**  Flag indicating that the content of collapsed nodes is not to be
     included in the iteration sequence. */
const TUint KFsTreeIteratorSkipCollapsedFlag = 0x01;

const TUint KFsTreeIteratorSkipHiddenFlag = 0x02;


NONSHARABLE_CLASS( TFsTreeIterator )
    {

    //friend class CFsTree;

public:

    /**
     * Copy constructor.
     *
     * @param aIterator Copied iterator.
     */
    
    TFsTreeIterator( const TFsTreeIterator& aIterator );
    
    /**
     * C++ constructor.
     *
     * @param aRoot Root node for the iterated tree (or subtree).
     */
    TFsTreeIterator( CFsTreeNode* aRoot, CFsTree* aTree,
                   CFsTreeItem* aCurrent,
                   const TUint aFlags = KFsTreeIteratorNullFlag);
        
    /**
     * Assignment operator.
     *
     * @param aIterator Iterator.
     */
    TFsTreeIterator& operator=( const TFsTreeIterator& aIterator );
    
    /**
     * Returns whether the iterator has next tree item.
     *
     * @return @c ETrue, if there is a next item.
     */
    TBool HasNext();

    /**
     * Returns an identifier to the next tree item. The returned value is @c 
     * KFsTreeNoneID,
     * when there is no more items. This changes also the iterator to point
     * to the returned item.
     *
     * @return Identifier of the next tree item.
     */
    TFsTreeItemId Next();
    
    /**
     * Returns whether the iterator has previous tree item.
     *
     * @return @c ETrue, if there is a previous item.
     */
    TBool HasPrevious();

    /**
     * Returns an identifier to the previous tree item. The returned value is
     * @c KFsTreeNoneID, if there is no previous tree item. This changes also 
     * the iterator to point to the returned item. 
     *
     * @return Identifier to the previous tree item.
     */
    TFsTreeItemId Previous();
    
    /**
     * Returns an identifier to the current tree item. The returned value is
     * @c KFsTreeNoneID, if there is no current tree item.
     *
     * @return Identifier to the current tree item.
     */
    TFsTreeItemId Current() const;

    /**
     * Return an identifier to the first item in the sequence.
     *
     * @return An identifier of the first item.
     */
    TFsTreeItemId First();
    
    /**
     * Returns an identifier to the last item in the sequence.
     *
     * @return An identifier of the last item.
     */ 
    TFsTreeItemId Last();

private:

    /**
     * Returns pointer to the next tree item.
     *
     * @return Pointer to the next tree item. @c NULL, if the next item is not
     *      found.
     */
    CFsTreeItem* FindNext();
    
    /**
     * Returns pointer ot the previous tree item.
     *
     * @return Pointer to the previous tree item. @c NULL, if previous item is
     *      not found.
     */
    CFsTreeItem* FindPrevious();

private: // data

    /**
     * Tree to iterate in.
     */
    CFsTree* iTree;

    /**
     * Current item.
     */
    CFsTreeItem* iCurrent;

    /**
     * Root of the tree.
     */
    CFsTreeNode* iRoot;

    /**
     * Next item. Contains a pointer to the cached next item.
     */
    CFsTreeItem* iNext;

    /**
     * Previous item. Contains a pointer to the cached previous item.
     */
    CFsTreeItem* iPrevious;

    /**
     * Flags for the iterator.
     */
    TUint iFlags;
      
    };
 
#endif // T_FSTREEITERATOR_H
