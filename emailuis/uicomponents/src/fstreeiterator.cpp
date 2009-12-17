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
* Description:  Freestyle tree iterator implementation
*
*/



#include "emailtrace.h"
#include <e32const.h>

#include "fstreeiterator.h"
#include "fstreeitem.h"
#include "fstreenode.h"
#include "fstreenodevisualizer.h"
#include "fstree.h"
#include "fsgenericpanic.h"


/** Bitmask specifying which flags can be given as parameters. */
const TUint KAllowedParameters = KFsTreeIteratorSkipCollapsedFlag |
                                 KFsTreeIteratorSkipHiddenFlag;

/** Flag indicating that the next item is stored in the iterator. */ 
const TUint KNextItemCached = 0x8000;

/** Flag indicating that the previous item is stored in the iterator. */
const TUint KPreviousItemCached = 0x4000;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Copy constructor.
// ---------------------------------------------------------------------------
//
TFsTreeIterator::TFsTreeIterator( const TFsTreeIterator& aIterator )
    : iTree(aIterator.iTree),
      iCurrent( aIterator.iCurrent ),
      iRoot( aIterator.iRoot ),
      iNext( aIterator.iNext ),
      iPrevious( aIterator.iPrevious ),
      iFlags( aIterator.iFlags ) 
    {
    FUNC_LOG;
    
    }


// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
TFsTreeIterator::TFsTreeIterator( CFsTreeNode* aRoot, CFsTree* aTree,
                                  CFsTreeItem* aCurrent, const TUint aFlags )
    : iTree(aTree),
      iCurrent( aCurrent ),
      iRoot( aRoot ),
      iNext( NULL ),
      iPrevious( NULL ),
      iFlags( aFlags & KAllowedParameters )     
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// Returns whether the iterator has next tree item.
// ---------------------------------------------------------------------------
//
TBool TFsTreeIterator::HasNext()
    {
    FUNC_LOG;
    return FindNext() ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// Returns a pointer to the next tree item.
// ---------------------------------------------------------------------------
//
TFsTreeItemId TFsTreeIterator::Next()
    {
    FUNC_LOG;
    CFsTreeItem* next = FindNext();
    
    iPrevious = iCurrent;
    iCurrent = next;
    iFlags = ( iFlags & KAllowedParameters ) | KPreviousItemCached;

    return iTree->Id(next);  
    }

// ---------------------------------------------------------------------------
// Returns whether the iterator has previous tree item.
// ---------------------------------------------------------------------------
//
TBool TFsTreeIterator::HasPrevious()
    {
    FUNC_LOG;
    return FindPrevious() ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// Returns an id of the previous tree item.
// ---------------------------------------------------------------------------
//
TFsTreeItemId TFsTreeIterator::Previous()
    {
    FUNC_LOG;
    CFsTreeItem* previous = FindPrevious();

    iNext = iCurrent;
    iCurrent = previous;
    iFlags = ( iFlags & KAllowedParameters ) | KNextItemCached;

    return iTree->Id( previous );
    }

// ---------------------------------------------------------------------------
// Returns an id of the current tree item.
// ---------------------------------------------------------------------------
//
TFsTreeItemId TFsTreeIterator::Current() const
    {
    FUNC_LOG;
    return iTree->Id(iCurrent);
    }

// ---------------------------------------------------------------------------
// Returns a pointer to the first item in the sequence.
// ---------------------------------------------------------------------------
//
TFsTreeItemId TFsTreeIterator::First()
    {
    FUNC_LOG;
    CFsTreeItem* first = NULL;
    
    if ( iRoot->CountChildren() > 0 )
        {
        first = iRoot->Child( 0 );
        }
    else
        {
        first = iRoot;
        }
    
    return iTree->Id( first );
    }

// ---------------------------------------------------------------------------
// Returns pointer to the las item in the sequence.
// ---------------------------------------------------------------------------
//
TFsTreeItemId TFsTreeIterator::Last()
    {
    FUNC_LOG;
    CFsTreeItem* last = NULL;
    CFsTreeNode* node = iRoot->Node();
    CFsTreeItem* item = iRoot; 
    TInt index = 0;
    TBool found = EFalse;
    
    while (!found && (node!=iRoot->Parent()))
        {
        node = item->Node();
        
        while (node && node->CountChildren() &&
                    
              (node->NodeVisualizer()->IsExpanded() || !(iFlags & KFsTreeIteratorSkipCollapsedFlag)) &&  
                   
             !(node->ItemVisualizer()->IsHidden() && (iFlags & KFsTreeIteratorSkipHiddenFlag)))
            {
            index = node->CountChildren()-1;
            item = node->Child(index);
            node = item->Node();
            }
        
        if (!(item->ItemVisualizer()->IsHidden() && (iFlags & KFsTreeIteratorSkipHiddenFlag))
                && (iTree->Id(item)!=KFsTreeRootID) )
            
            {
            last = item;
            found = ETrue;
            }
        else
            {
            node = item->Parent();
            if (node)
                {
                index = node->Index(item);
                if (index>0)
                    {
                    item = node->Child(--index);
                    }
                else
                    {
                    item = node;
                    node = item->Parent();
                    }
                }
            }
        }
    
    return iTree->Id(last);
    }

// ---------------------------------------------------------------------------
// Assignment operator.
// ---------------------------------------------------------------------------
//
TFsTreeIterator& TFsTreeIterator::operator=(const TFsTreeIterator& aIterator)
    {
    FUNC_LOG;
    iTree = aIterator.iTree;
    iCurrent = aIterator.iCurrent;
    iRoot = aIterator.iRoot;
    iNext = aIterator.iNext;
    iPrevious = aIterator.iPrevious;
    iFlags = aIterator.iFlags;  
    
    return *this;
    }

// ---------------------------------------------------------------------------
// Returns pointer to the next tree item.
// ---------------------------------------------------------------------------
//
CFsTreeItem* TFsTreeIterator::FindNext()
    {
    FUNC_LOG;
    CFsTreeItem* next = NULL;
    if ( iFlags & KNextItemCached )
        {
        next = iNext;
        }
    else
        {
        CFsTreeItem* item = iCurrent;
        CFsTreeNode* node = item->Node();
        TBool found = EFalse;
        TInt index = 0;
        
        if  (node && node->CountChildren() &&
                            
            (node->NodeVisualizer()->IsExpanded() || !(iFlags & KFsTreeIteratorSkipCollapsedFlag)) &&  
               
            !(node->ItemVisualizer()->IsHidden() && (iFlags & KFsTreeIteratorSkipHiddenFlag)))
            {
            index = 0;
            item = node->Child(index);
            }
        else
            {
            if (item!=iRoot)
                {
                node = item->Parent();
                index = node->Index(item) + 1;
                }
            else
                {
                found = ETrue;
                }
            }
        
        while (!found && (node!=iRoot->Parent()))
            {
            if (index < node->CountChildren())
                {
                item = node->Child(index);
                if (!(item->ItemVisualizer()->IsHidden() && (iFlags & KFsTreeIteratorSkipHiddenFlag)))
                    {
                    found = ETrue;
                    next = item;
                    }
                }
            else
                {
                item = node;
                node = item->Parent();
                }
            
            if (node)
                {
                index = node->Index(item) + 1;
                }
            }
        
        iNext = next;
        iFlags |= KNextItemCached;
        }
    return next;
    }
    
// ---------------------------------------------------------------------------
// Returns pointer ot the previous tree item.
// ---------------------------------------------------------------------------
//
CFsTreeItem* TFsTreeIterator::FindPrevious()
    {
    FUNC_LOG;
    CFsTreeItem* previous = NULL;
    if ( iFlags & KPreviousItemCached )
        {
        previous = iPrevious;
        }
    else if ( iCurrent )
        {
        CFsTreeItem* item = iCurrent;
        CFsTreeNode* node = item->Parent();
        TInt index = 0;
        
        TBool found = EFalse;
        
        while (!found && (node!=iRoot->Parent()))
            {
            index = node->Index(item);
            if (index>0)
                {
                item = node->Child(--index);
                node = item->Node();
                
                //previous is a node so dig in deeply
                while (node && node->CountChildren() &&
                        
                     (node->NodeVisualizer()->IsExpanded() || !(iFlags & KFsTreeIteratorSkipCollapsedFlag)) &&  
                        
                     !(node->ItemVisualizer()->IsHidden() && (iFlags & KFsTreeIteratorSkipHiddenFlag)))
                    {
                    index = node->CountChildren()-1;
                    item = node->Child(index);
                    node = item->Node();
                    }
                }
            else
                {
                //index == 0
                item = node;
                node = item->Parent();
                }

            if (!(item->ItemVisualizer()->IsHidden() && (iFlags & KFsTreeIteratorSkipHiddenFlag)) &&
                    (iTree->Id(item) != KFsTreeRootID))
                {
                found = ETrue;
                previous = item;
                }
            else
                {
                node = item->Parent();
                }
            }
        
        iPrevious = previous;
        iFlags |= KPreviousItemCached;
        }
        
    return previous;
    }

