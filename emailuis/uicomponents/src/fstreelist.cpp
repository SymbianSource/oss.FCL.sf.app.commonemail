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
* Description:  Freestyle list component implementation
*
*/



//////SYSTEM INCLUDES

//////TOOLKIT INCLUDES
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfenv.h>
// </cmail>

//////PROJECT INCLUDES
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include "fstreelist.h"
#include "fstreecontrol.h"
#include "fstree.h"
#include "fstreeitem.h"
#include "fstreenode.h"
#include "fsgenericpanic.h"
#include "fstreeitemdata.h"
#include "fstreeitemvisualizer.h"
#include "fstreenodevisualizer.h"
#include "fstreelistobserver.h"
#include "fstreevisualizerbase.h"
#include "fstextstylemanager.h"
#include "fstreeobserver.h"
#include "fsalftextstylemanager.h"
#include "fsscrollbarset.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeList* CFsTreeList::NewL( CFsTreeVisualizerBase& aTreeV,
                                         CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CFsTreeList* self = new( ELeave ) CFsTreeList( aEnv, aTreeV );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeList::~CFsTreeList()
    {
    FUNC_LOG;
    delete iTree;
    delete iTreeVisualizer;
    delete iTextStyleManager;
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
//  Adds observer to the list of observers
// ---------------------------------------------------------------------------
EXPORT_C void CFsTreeList::AddObserverL(
    MFsTreeListObserver& aTreeListObserver )
    {
    FUNC_LOG;
    iObservers.AppendL(&aTreeListObserver);
    }

// ---------------------------------------------------------------------------
//  Removes observer from the list of observers
// ---------------------------------------------------------------------------

EXPORT_C void CFsTreeList::RemoveObserver(
    MFsTreeListObserver& aTreeListObserver )
    {
    FUNC_LOG;
    TInt ind = iObservers.Find( &aTreeListObserver );
    if ( ind >= 0 )
        {
        iObservers.Remove( ind );
        }
    }

// ---------------------------------------------------------------------------
// Returns a control associated with the component.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControl* CFsTreeList::TreeControl( ) const
    {
    FUNC_LOG;
    return iTreeControl;
    }

// ---------------------------------------------------------------------------
// Inserts new item as a child of parent given by Id with the given position.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsTreeItemId CFsTreeList::InsertItemL( MFsTreeItemData& aItemD,
                                                 MFsTreeItemVisualizer& aItemV,
                                                 const TFsTreeItemId aParent,
                                                 const TInt aIndex,
                                                 const TBool aAllowRefresh )
    {
    FUNC_LOG;
    iTreeVisualizer->SetAutoRefreshAtInsert( aAllowRefresh );
    return iTree->InsertItemL(aItemD,aItemV,aParent,aIndex);
    }

// ---------------------------------------------------------------------------
// Inserts new node as a child of parent given by Id with the given position.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsTreeItemId CFsTreeList::InsertNodeL( MFsTreeItemData& aItemD,
                                                 MFsTreeNodeVisualizer& aNodeV,
                                                 const TFsTreeItemId aParent,
                                                 const TInt aIndex,
                                                 const TBool aAllowRefresh )
    {
    FUNC_LOG;
    iTreeVisualizer->SetAutoRefreshAtInsert( aAllowRefresh );
    return iTree->InsertNodeL(aItemD, aNodeV, aParent, aIndex);
    }

// ---------------------------------------------------------------------------
//  Remove all items from the list.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::RemoveL( const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    iTree->RemoveL(aItemId);
    }

// ---------------------------------------------------------------------------
//  Remove all items from the list.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::RemoveAllL()
    {
    FUNC_LOG;
    iTree->RemoveL(KFsTreeRootID);
    }

// ---------------------------------------------------------------------------
//  Remove all children of the selected node.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::RemoveChildrenL( const TFsTreeItemId aNodeId )
    {
    FUNC_LOG;
    for ( TInt childIndex = 0; childIndex < iTree->CountChildren(aNodeId);
                                                    childIndex++ )
        {
        iTree->RemoveL(iTree->Child(aNodeId, childIndex));
        }
    }


// ---------------------------------------------------------------------------
//  Moves the item with given Id to the list of children of parent with
//  given id
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::MoveItemL( const TFsTreeItemId aItemId,
        const TFsTreeItemId aTargetNode, const TInt aIndex )
    {
    FUNC_LOG;
    TBool subtreeItem = EFalse;
    TFsTreeItemId treeItem = aTargetNode;
    while (treeItem!=KFsTreeRootID)
        {
        treeItem = iTree->Parent(treeItem);
        if (treeItem==aItemId)
            {
            subtreeItem = ETrue;
            break;
            }
        }

    if (subtreeItem)
        {
        User::Leave(EFsListPanicMovingNodeToItsChild);
        }
    else
        {
        iTree->MoveL( aItemId, aTargetNode, aIndex );
        }
    }

// ---------------------------------------------------------------------------
//  Checks if a node has children, if parameter is omitted, root of the list
//  is checked to indicate if the list is empty.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsTreeList::IsEmpty( const TFsTreeItemId aNodeId ) const
    {
    FUNC_LOG;
    TBool result = ETrue;

    if (aNodeId == KFsTreeRootID)
        {
        if (iTree->Count() > 0)
            {
            result = EFalse;
            }
        }
    else
        {
        if (iTree->CountChildren(aNodeId) > 0)
            {
            result = EFalse;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
//  Returns the level of an item in tree.
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CFsTreeList::Level( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return iTree->Level(aItemId);
    }

// ---------------------------------------------------------------------------
//  Remove all children of the selected node.
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CFsTreeList::Count() const
    {
    FUNC_LOG;
    return iTree->Count();
    }

// ---------------------------------------------------------------------------
//  Gets the number of children of the given node
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CFsTreeList::CountChildren( const TFsTreeItemId aNodeId ) const
    {
    FUNC_LOG;
    return iTree->CountChildren( aNodeId );
    }

// ---------------------------------------------------------------------------
//  Gets the id of a child of the specified node with the position given
// ---------------------------------------------------------------------------
//
EXPORT_C TFsTreeItemId CFsTreeList::Child( const TFsTreeItemId aNodeId,
                                            const TUint aIndex ) const
    {
    FUNC_LOG;
    return iTree->Child( aNodeId,  aIndex );
    }

// ---------------------------------------------------------------------------
//  Gets the index of a child with the specified id.
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CFsTreeList::ChildIndex(const TFsTreeItemId aNodeId,
        const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return iTree->ChildIndex(aNodeId, aItemId);
    }

// ---------------------------------------------------------------------------
//  Gets the parent's id for the specified item.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsTreeItemId CFsTreeList::Parent(const TFsTreeItemId aItemId) const
    {
    FUNC_LOG;
    return iTree->Parent(aItemId);
    }

// ---------------------------------------------------------------------------
//  Checks whether the item with specified id belongs to the list.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsTreeList::Contains( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return iTree->Contains(aItemId);
    }

// ---------------------------------------------------------------------------
//  Checks if item is node or not.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsTreeList::IsNode( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return iTree->IsNode(aItemId);
    }

//---------------------------------------------------------------------------
// Sets focus state of list component
//---------------------------------------------------------------------------
EXPORT_C void CFsTreeList::SetFocusedL( const TBool aFocused )
    {
    FUNC_LOG;
    iListFocused = aFocused;
    iTreeControl->SetFocused( iListFocused );
    iTreeVisualizer->SetFocusedL( iListFocused );
    }

//---------------------------------------------------------------------------
// Returns state of the focus
//---------------------------------------------------------------------------
EXPORT_C TBool CFsTreeList::IsFocused() const
    {
    FUNC_LOG;
    return iListFocused;
    }

// ---------------------------------------------------------------------------
//  The function sets duration of the slide-in effect.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetSlideInDuration ( TInt aTimeMiliseconds )
    {
    FUNC_LOG;
    iTreeVisualizer->SetSlideInDuration ( aTimeMiliseconds );
    }

// ---------------------------------------------------------------------------
//  The function returns slide in effect's duration.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::SlideInDuration ( ) const
    {
    FUNC_LOG;
    return iTreeVisualizer->SlideInDuration ( );
    }

// ---------------------------------------------------------------------------
//  The function sets direction of the slide in effect.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetSlideInDirection (
                                MFsTreeVisualizer::TFsSlideEffect aDirection )
    {
    FUNC_LOG;
    iTreeVisualizer->SetSlideInDirection ( aDirection );
    }

// ---------------------------------------------------------------------------
//  The function returns direction of the slide in effect.
// ---------------------------------------------------------------------------
//
EXPORT_C MFsTreeVisualizer::TFsSlideEffect
                                       CFsTreeList::SlideInDirection ( ) const
    {
    FUNC_LOG;
    return iTreeVisualizer->SlideInDirection ( );
    }

// ---------------------------------------------------------------------------
//  The function sets duration of the slide-out effect.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetSlideOutDuration ( TInt aTimeMiliseconds )
    {
    FUNC_LOG;
    iTreeVisualizer->SetSlideOutDuration ( aTimeMiliseconds );
    }

// ---------------------------------------------------------------------------
//  The function returns slide out effect's duration.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::SlideOutDuration ( ) const
    {
    FUNC_LOG;
    return iTreeVisualizer->SlideOutDuration ( );
    }

// ---------------------------------------------------------------------------
//  The function sets direction of the slide out effect.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetSlideOutDirection (
                                MFsTreeVisualizer::TFsSlideEffect aDirection )
    {
    FUNC_LOG;
    iTreeVisualizer->SetSlideOutDirection ( aDirection );
    }

// ---------------------------------------------------------------------------
//  The function returns direction of the slide out effect.
// ---------------------------------------------------------------------------
//
EXPORT_C MFsTreeVisualizer::TFsSlideEffect
                                      CFsTreeList::SlideOutDirection ( ) const
    {
    FUNC_LOG;
    return iTreeVisualizer->SlideOutDirection ( );
    }

// ---------------------------------------------------------------------------
//  Returns information about looping type of the list.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsTreeListLoopingType CFsTreeList::LoopingType() const
    {
    FUNC_LOG;
    TFsTreeListLoopingType loopingType = EFsTreeListLoopingDisabled;

    if ( iTreeVisualizer )
        {
        loopingType = iTreeVisualizer->LoopingType();
        }

    return loopingType;
    }

// ---------------------------------------------------------------------------
// Returns information whether list is markable, non-markable or
// multi-markable
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeList::TFsTreeListMarkType CFsTreeList::MarkType() const
    {
    FUNC_LOG;
    return iMarkType;
    }

// ---------------------------------------------------------------------------
// OLD API - to be removed.
// Returns information about scrollbars used by list visual
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeList::TFsTreeListScrollbars CFsTreeList::HasScrollBars() const
    {
    FUNC_LOG;
    return iHasScrollBars;
    }

// ---------------------------------------------------------------------------
// Old API - to be removed
// Enables or disables scrollbars
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetScrollBarsL( const TFsTreeListScrollbars aFlag )
    {
    FUNC_LOG;
    iHasScrollBars = aFlag;
    if ( aFlag == EFsTreeListNoScrollbars )
        {
        iTreeVisualizer->SetVScrollBarL( EFalse );
        }
    else if ( aFlag == EFsTreeListVerticalScrollbar )
        {
        iTreeVisualizer->SetVScrollBarL( ETrue );
        }
    }

// ---------------------------------------------------------------------------
// Returns information about scrollbars used by list visual
// ---------------------------------------------------------------------------
//
EXPORT_C TFsScrollbarVisibility CFsTreeList::ScrollbarVisibility() const
    {
    FUNC_LOG;
    return iTreeVisualizer->ScrollbarVisibility();
    }

// ---------------------------------------------------------------------------
// Enables or disables scrollbars
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetScrollbarVisibilityL(const TFsScrollbarVisibility aVisibility)
    {
    FUNC_LOG;
    iTreeVisualizer->SetScrollBarVisibilityL(aVisibility);
    }

// ---------------------------------------------------------------------------
//  Sets type of list's looping.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetLoopingType(
                                   const TFsTreeListLoopingType aLoopingType )
    {
    FUNC_LOG;
    if ( iTreeVisualizer )
        {
        iTreeVisualizer->SetLoopingType( aLoopingType );
        }
    }

// ---------------------------------------------------------------------------
// The functions sets fade-in's effect duration.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetFadeInEffectTime( TInt aFadeTime )
    {
    FUNC_LOG;
    iTreeVisualizer->SetFadeInEffectTime( aFadeTime );
    }

// ---------------------------------------------------------------------------
// The functions returns fade-in's effect duration.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::FadeInEffectTime( )
    {
    FUNC_LOG;
    return iTreeVisualizer->FadeInEffectTime( );
    }

// ---------------------------------------------------------------------------
// The functions sets fade-out's effect duration.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetFadeOutEffectTime( TInt aFadeTime )
    {
    FUNC_LOG;
    iTreeVisualizer->SetFadeOutEffectTime ( aFadeTime );
    }

// ---------------------------------------------------------------------------
// The functions returns fade-out's effect duration.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::FadeOutEffectTime( )
    {
    FUNC_LOG;
    return iTreeVisualizer->FadeOutEffectTime();
    }

// ---------------------------------------------------------------------------
// Function sets the time of list scroll per item and the scroll
// acceleration rate.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetScrollTime ( TInt aScrollTime,
                                           TReal aScrollAcceleration )
    {
    FUNC_LOG;
    iTreeVisualizer->SetScrollTime( aScrollTime, aScrollAcceleration );
    }

// ---------------------------------------------------------------------------
// Function gets the values of scroll speed and its acceleration rate.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::GetScrollTime( TInt& aScrollTime,
                                          TReal& aScrollAcceleration )
    {
    FUNC_LOG;
    iTreeVisualizer->GetScrollTime( aScrollTime, aScrollAcceleration );
    }

// ---------------------------------------------------------------------------
// Gets the currently focused (highlighted) item id.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsTreeItemId CFsTreeList::FocusedItem() const
    {
    FUNC_LOG;
    return iTreeVisualizer->FocusedItem();
    }

// ---------------------------------------------------------------------------
// Sets the focus (highlight) upon the item with given id.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetFocusedItemL( const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    iTreeVisualizer->SetFocusedItemL(aItemId);
    }

// ---------------------------------------------------------------------------
//  Checks whether the item with specified id is marked.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsTreeList::IsMarked( const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return iTreeVisualizer->IsMarked( aItemId );
    }

// ---------------------------------------------------------------------------
//  Marks the item.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::MarkItemL( const TFsTreeItemId aItemId, TBool aMarked )
    {
    FUNC_LOG;
    iTreeVisualizer-> MarkItemL( aItemId, aMarked );
    }

// ---------------------------------------------------------------------------
// Enables or disables marking of list items.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetMarkTypeL( const TFsTreeListMarkType aFlag )
    {
    FUNC_LOG;
    iMarkType = aFlag;

    switch ( iMarkType )
    {
    case EFsTreeListNonMarkable:
        {
        iTreeVisualizer->MarkAllItemsL( EFalse );
        break;
        }
    case EFsTreeListMarkable:
        {
        RArray<TFsTreeItemId> markedItems;
        iTreeVisualizer->GetMarkedItemsL( markedItems );
        iTreeVisualizer->MarkAllItemsL( EFalse );
        if ( markedItems.Count() > 0 )
            {
            iTreeVisualizer->MarkItemL( markedItems[0], ETrue );
            }
        break;
        }
    case EFsTreeListMultiMarkable:
        {
        break;
        }
    }
    }

// ---------------------------------------------------------------------------
//  Get items indentation in pixels.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::Indentation() const
    {
    FUNC_LOG;
    return iTreeVisualizer->Indentation();
    }

// ---------------------------------------------------------------------------
//  Set items indentation in pixels.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetIndentationL( const TInt aIndentation )
    {
    FUNC_LOG;
    iTreeVisualizer->SetIndentationL( aIndentation );
    }

// ---------------------------------------------------------------------------
//  The functions sets wether all item in the list should be always in
//  extended state or in normal state.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetItemsAlwaysExtendedL ( TBool aAlwaysExtended )
    {
    FUNC_LOG;
    iTreeVisualizer->SetItemsAlwaysExtendedL ( aAlwaysExtended );
    }

// ---------------------------------------------------------------------------
//  The function returns if items are always in extended state or in normal.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsTreeList::IsItemsAlwaysExtended ( )
    {
    FUNC_LOG;
    TBool alwaysExtended = EFalse;
    alwaysExtended = iTreeVisualizer->IsItemsAlwaysExtended();
    return alwaysExtended;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeList::CFsTreeList( CAlfEnv& aEnv, CFsTreeVisualizerBase& aTreeV )
    : iEnv(&aEnv),
      iListFocused (ETrue)
    {
    FUNC_LOG;
    iTreeVisualizer = &aTreeV;
    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::ConstructL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    iTextStyleManager = CFsAlfTextStyleManager::NewL( aEnv.TextStyleManager() );
    iTree = CFsTree::NewL(*iTreeVisualizer, *(iTreeVisualizer->RootItemData()),
                                      *(iTreeVisualizer->RootNodeVisualizer()));
    iTreeVisualizer->SetTreeData(*iTree);
    iTreeControl = CFsTreeControl::NewL(aEnv, *iTree, *iTreeVisualizer);
    iTreeVisualizer->ShowListL( EFalse );
    iTreeVisualizer->SetVisualizerObserver( this );
    }

// ---------------------------------------------------------------------------
//  Expand the node.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::ExpandNodeL( const TFsTreeItemId aNodeId )
    {
    FUNC_LOG;
    iTreeVisualizer->ExpandNodeL(aNodeId);
    }

// ---------------------------------------------------------------------------
//  Collapse the node.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::CollapseNodeL( const TFsTreeItemId aNodeId )
    {
    FUNC_LOG;
    iTreeVisualizer->CollapseNodeL(aNodeId);
    }

// ---------------------------------------------------------------------------
//  Copy the list of marked items.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::GetMarkedItemsL(
                                       RFsTreeItemIdList& aMarkedItems ) const
    {
    FUNC_LOG;
    iTreeVisualizer->GetMarkedItemsL(aMarkedItems);
    }

// ---------------------------------------------------------------------------
//  Gets the reference to the item/node data (const).
// ---------------------------------------------------------------------------
//
EXPORT_C MFsTreeItemData& CFsTreeList::ItemData(
        const TFsTreeItemId aItemId) const
    {
    FUNC_LOG;
    return iTree->ItemData(aItemId);
    }

// ---------------------------------------------------------------------------
//  Sets the reference to the item/node data.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetItemDataL( MFsTreeItemData& aData,
        const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    MFsTreeItemData* data = &iTree->ItemData(aItemId);
    delete data;
    iTree->SetItemData(aData, aItemId);
    iTreeVisualizer->TreeEventL(
            MFsTreeObserver::EFsTreeItemDataChanged,
            MFsTreeObserver::TFsTreeEventParams(aItemId, KFsTreeNoneID, -1) );
    }

// ---------------------------------------------------------------------------
//  Gets the reference to the item visualizer (const).
// ---------------------------------------------------------------------------
//
EXPORT_C MFsTreeItemVisualizer& CFsTreeList::ItemVisualizer(
        const TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
    return *iTree->ItemVisualizer(aItemId);
    }

// ---------------------------------------------------------------------------
//  Sets the pointer to the item visualizer and refreshes item.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetItemVisualizerL(
        MFsTreeItemVisualizer& aVisualizer,
        const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    MFsTreeItemVisualizer* visualizer = iTree->ItemVisualizer(aItemId);
    visualizer->Hide(iTreeVisualizer->ItemExpansionDelay());
    delete visualizer;
    iTree->SetItemVisualizer(aVisualizer, aItemId);
    iTreeVisualizer->TreeEventL(
            MFsTreeObserver::EFsTreeItemVisualizerChanged,
            MFsTreeObserver::TFsTreeEventParams(aItemId, KFsTreeNoneID, -1));
    }

// ---------------------------------------------------------------------------
//  Check whether the node is expanded or collapsed.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsTreeList::IsExpanded( const TFsTreeItemId aNodeId )
    {
    FUNC_LOG;
    return iTree->NodeVisualizer(aNodeId)->IsExpanded();
    }

// ---------------------------------------------------------------------------
//  Gets the reference to the tree visualizer
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeVisualizerBase& CFsTreeList::TreeVisualizer() const
    {
    FUNC_LOG;
    return *iTreeVisualizer;
    }

// ---------------------------------------------------------------------------
//  Sets a tree visualizer for the list
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetTreeVisualizer( CFsTreeVisualizerBase& aTreeV )
    {
    FUNC_LOG;
    iTreeVisualizer = &aTreeV;

    if (iTree)
        {
        iTreeVisualizer->SetTreeData(*iTree);
        }

    }

// ---------------------------------------------------------------------------
//  Shows the list visual
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::ShowListL( const TBool aFadeIn,
                                      const TBool aSlideIn )
    {
    FUNC_LOG;
    //SetFocusedL(ETrue);
    iTreeVisualizer->ShowListL( aFadeIn, aSlideIn );
    }

// ---------------------------------------------------------------------------
//  Shows the list visual
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetDirectTouchMode( const TBool aDirectTouchMode )
    {
    FUNC_LOG;
    iTreeVisualizer->SetDirectTouchMode( aDirectTouchMode );
    }


// ---------------------------------------------------------------------------
//  Hides the list visual
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::HideListL( const TBool aFadeOut,
                                     const TBool aSlideOut )
    {
    FUNC_LOG;
    //SetFocusedL(EFalse);
    iTreeVisualizer->HideList( aFadeOut, aSlideOut );
    }

// ---------------------------------------------------------------------------
//  Sets visiblity of the border
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetBorderL( const TBool aVisible,
    CAlfTexture* aBorderTexture )
    {
    FUNC_LOG;
    iTreeVisualizer->SetBorderL( aVisible, aBorderTexture );
    }

// ---------------------------------------------------------------------------
//  Sets visiblity of the border
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetShadowL( const TBool aVisible )
    {
    FUNC_LOG;
    iTreeVisualizer->SetShadowL( aVisible );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::GetItemDisplayRectTarget ( const TFsTreeItemId aItemId, TAlfRealRect& aRect )
    {
    FUNC_LOG;
    TInt retVal(KErrNotFound);
    if (iTree->Contains( aItemId ))
        {
        retVal = iTreeVisualizer->GetItemDisplayRectTarget( aItemId, aRect );
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
//  Returns pointer to the menu list assigned to the item
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeList* CFsTreeList::ItemMenu(
    TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    return iTree->ItemVisualizer(aItemId)->Menu();
    }

// ---------------------------------------------------------------------------
//  Retrieves events from tree visualizer
// ---------------------------------------------------------------------------
//
// <cmail> Touch
EXPORT_C void CFsTreeList::TreeVisualizerEventL(
        const TFsTreeVisualizerEvent aEvent,
        const TFsTreeItemId aId ) const
// </cmail>
    {
    FUNC_LOG;
    MFsTreeListObserver::TFsTreeListEvent listEvent;

    switch ( aEvent )
    {
    // <cmail> "Base class modifications for using touch"
    case MFsTreeVisualizerObserver::EFsTreeItemTouchAction:
        {
        listEvent = MFsTreeListObserver::EFsTreeListItemTouchAction;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeItemTouchLongTap:
        {
        listEvent = MFsTreeListObserver::EFsTreeListItemTouchLongTap;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeItemTouchFocused:
        {
        listEvent = MFsTreeListObserver::EFsTreeListItemTouchFocused;
        break;
        }

    // </cmail>

    case MFsTreeVisualizerObserver::EFsTreeItemSelected:
        {
        listEvent = MFsTreeListObserver::EFsTreeItemSelected;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeItemWithMenuSelected:
        {
        listEvent = MFsTreeListObserver::EFsTreeItemWithMenuSelected;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeKeyRightArrow:
        {
        listEvent = MFsTreeListObserver::EFsTreeListKeyRightArrow;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeKeyLeftArrow:
        {
        listEvent = MFsTreeListObserver::EFsTreeListKeyLeftArrow;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeListVisualizerShown:
        {
        listEvent = MFsTreeListObserver::EFsTreeListShown;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeListVisualizerHidden:
        {
        listEvent = MFsTreeListObserver::EFsTreeListHidden;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeItemWillGetFocused:
        {
        listEvent = MFsTreeListObserver::EFsTreeListItemWillGetFocused;
        break;
        }

    // <cmail>
    // Pass on the collapse/expand events.
    case MFsTreeVisualizerObserver::EFsTreeListItemCollapsed:
        {
        listEvent = MFsTreeListObserver::EFsTreeListItemCollapsed;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeListItemExpanded:
        {
        listEvent = MFsTreeListObserver::EFsTreeListItemExpanded;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeListCollapsedAll:
        {
        listEvent = MFsTreeListObserver::EFsTreeListCollapsedAll;
        break;
        }
    case MFsTreeVisualizerObserver::EFsTreeListExpandedAll:
        {
        listEvent = MFsTreeListObserver::EFsTreeListExpandedAll;
        break;
        }
    // </cmail>
    case MFsTreeVisualizerObserver::EFsChangeFocusVisibility:
        {
        listEvent = MFsTreeListObserver::EFsFocusVisibilityChange;
        break;
        }
    default:
        {
        return;
        }
    }

    for ( TInt c = 0; c < iObservers.Count(); c++ )
        {
        // <cmail> Touch
        iObservers[c]->TreeListEventL(
            listEvent,
            aId);
        // </cmail>
        }
    }

// ---------------------------------------------------------------------------
// Gets the reference to the text style manager.
// ---------------------------------------------------------------------------
EXPORT_C CFsAlfTextStyleManager& CFsTreeList::TextStyleManager() const
    {
    FUNC_LOG;
    return *iTextStyleManager;
    }


// ---------------------------------------------------------------------------
//  Sets the type of text marquee.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetTextMarqueeType(const TFsTextMarqueeType aMarquee)
    {
    FUNC_LOG;
    iTreeVisualizer->SetTextMarqueeType( aMarquee );
    }


// ---------------------------------------------------------------------------
//  Returns the type of text marquee.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsTextMarqueeType CFsTreeList::TextMarqueeType() const
    {
    FUNC_LOG;
    return iTreeVisualizer->TextMarqueeType();
    }


// ---------------------------------------------------------------------------
//  Sets the speed of marquee.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetTextMarqueeSpeed ( const TInt aPixelsPerSec )
    {
    FUNC_LOG;
    iTreeVisualizer->SetTextMarqueeSpeed ( aPixelsPerSec );
    }


// ---------------------------------------------------------------------------
//  Returns the speed of marquee.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::TextMarqueeSpeed ( ) const
    {
    FUNC_LOG;
    return iTreeVisualizer->TextMarqueeSpeed( );
    }


// ---------------------------------------------------------------------------
//  Sets delay for text marquee start.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetTextMarqueeStartDelay ( const TInt aStartDelay )
    {
    FUNC_LOG;
    iTreeVisualizer->SetTextMarqueeStartDelay ( aStartDelay );
    }


// ---------------------------------------------------------------------------
//  Returns delay for text marquee.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::TextMarqueeStartDelay () const
    {
    FUNC_LOG;
    return iTreeVisualizer->TextMarqueeStartDelay( );
    }


// ---------------------------------------------------------------------------
//  Sets a delay for each cycle start.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetTextMarqueeCycleStartDelay ( const TInt aCycleStartDelay )
    {
    FUNC_LOG;
    iTreeVisualizer->SetTextMarqueeCycleStartDelay ( aCycleStartDelay );
    }


// ---------------------------------------------------------------------------
//  Returns a delay for each cycle start.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::TextMarqueeCycleStartDelay () const
    {
    FUNC_LOG;
    return iTreeVisualizer->TextMarqueeCycleStartDelay( );
    }


// ---------------------------------------------------------------------------
//  Sets number of marquee cycles.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::SetTextMarqueeRepetitions ( const TInt aMarqueeRepetitions )
    {
    FUNC_LOG;
    iTreeVisualizer->SetTextMarqueeRepetitions ( aMarqueeRepetitions );
    }


// ---------------------------------------------------------------------------
//  Returns number of marquee cycles.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsTreeList::TextMarqueeRepetitions ( ) const
    {
    FUNC_LOG;
    return iTreeVisualizer->TextMarqueeRepetitions( );
    }

// ---------------------------------------------------------------------------
//  Returns number of marquee cycles.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeList::DisableKineticScrolling( TBool aDisable )
    {
    iTreeVisualizer->DisableKineticScrolling( aDisable );
    }

//EOF

