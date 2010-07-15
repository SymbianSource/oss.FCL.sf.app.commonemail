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
 *  Description : Freestyle tree list visualizer implementation
 *
 *
 */

//////SYSTEM INCLUDES
#include "emailtrace.h"
#include <AknUtils.h>
#include <StringLoader.h>
#include <avkon.rsg> // R_AVKON_EMPTY_POPUP_LIST_TEXT
#include <centralrepository.h>

//////TOOLKIT INCLUDES
// <cmail> SF
#include <alf/alfstatic.h>
#include <alf/alfownership.h>
#include <alf/alfevent.h>
#include <alf/alfimage.h>
#include <alf/alfenv.h>
#include <alf/alfcontrolgroup.h>
//layouts
#include <alf/alfflowlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfgridlayout.h>
#include <alf/alfanchorlayout.h>
//brushes
#include <alf/alfbrusharray.h>
#include <alf/alfbrush.h>
#include <alf/alfborderbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfframebrush.h>
#include <alf/alfshadowborderbrush.h>
//metrics
#include <alf/alfmetric.h>
#include <alf/alftimedvalue.h>
//visuals
#include <alf/alfvisual.h>
#include <alf/alfimagevisual.h>
// </cmail>


//////PROJECT INCLUDES
#include "fstreevisualizerbase.h"
#include "fstreeplainrootvisualizer.h"
#include "fstree.h"
#include "fstreeiterator.h"
#include "fsgenericpanic.h"
#include "fsfadeeffect.h"
#include "fstreeplainonelineitemdata.h"
#include "fsslideeffect.h"
#include "fstreeplainonelineitemvisualizer.h"
#include "fstreelist.h"
#include "fstreevisualizerobserver.h"
#include "fscenrepkeys.h"
#include "fswatermark.h"
#include "fslayoutmanager.h"
#include "fsseparatordata.h"
////alf specific
#include "fsalfscrollbarlayout.h"
#include "fsalftextstylemanager.h"
#include "fsinteractioninterval.h"
#include "muicscrollbar.h"
#include "cuicaknscrollbaradapter.h"
#include "cuicscrollbar.h"

#include <aknphysics.h>
#include <touchfeedback.h> // for MTouchFeedback


//CONSTANTS
const TInt KDefaultShadowWidth = 4;
const TInt KFSListDefaultSelectorBorderWidth = 0;
const TInt KZero = 0;
_LIT8( KPropertyItemId, "PropItemID" );

// check from aknphysicsconstants.h
const TInt KFlickMaxDuration( 500000 );
//const TInt KDefaultFriction( 25 ); // avkon default is 10
const TInt KDefaultFriction( 5 ); // avkon default is 10

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Deletes and sets pointer to NULL.
// ---------------------------------------------------------------------------
template<class T> void SafeDelete(T*& aPtr)
    {
    delete aPtr;
    aPtr = NULL;
    }

// ======== MEMBER FUNCTIONS ========

// TWorld::TItem

// ---------------------------------------------------------------------------
// TWorld::TItem::TItem
// ---------------------------------------------------------------------------
TWorld::TItem::TItem(const TFsTreeItemId aId, const TRect& aRect) :
    iId(aId)
    {
    SetRect(aRect);
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::Rect
// ---------------------------------------------------------------------------
TRect TWorld::TItem::Rect() const
    {
    return TRect(TPoint(0, iY), TPoint(0, iY + iHeight));
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::SetRect
// ---------------------------------------------------------------------------
void TWorld::TItem::SetRect(const TRect& aRect)
    {
    iY = aRect.iTl.iY;
    iHeight = aRect.Height();
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::Move
// ---------------------------------------------------------------------------
void TWorld::TItem::Move(TInt aDy)
    {
    iY += aDy;
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::Top
// ---------------------------------------------------------------------------
TInt TWorld::TItem::Top() const
    {
    return iY;
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::TopLeft
// ---------------------------------------------------------------------------
TPoint TWorld::TItem::TopLeft() const
    {
    return TPoint(0, iY);
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::Height
// ---------------------------------------------------------------------------
TInt TWorld::TItem::Height() const
    {
    return iHeight;
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::Bottom
// ---------------------------------------------------------------------------
TInt TWorld::TItem::Bottom() const
    {
    return iY + iHeight;
    }

// ---------------------------------------------------------------------------
// TWorld::TItem::Bottom
// ---------------------------------------------------------------------------
TPoint TWorld::TItem::BottomRight() const
    {
    return TPoint(0, iY + iHeight);
    }

// TWorld

// ---------------------------------------------------------------------------
// TWorld::TWorld
// ---------------------------------------------------------------------------
TWorld::TWorld() :
    iBatchUpdateOngoing(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// TWorld::~TWorld
// ---------------------------------------------------------------------------
TWorld::~TWorld()
    {
    iItems.Reset();
    iObservers.Reset();
    }

// ---------------------------------------------------------------------------
// TWorld::AddObserverL
// ---------------------------------------------------------------------------
void TWorld::AddObserverL(MObserver& aObserver)
    {
    iObservers.AppendL(&aObserver);
    }

// ---------------------------------------------------------------------------
// TWorld::RemoveObserver
// ---------------------------------------------------------------------------
void TWorld::RemoveObserver(MObserver& aObserver)
    {
    const TInt index(iObservers.Find(&aObserver));
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// TWorld::AppendL
// ---------------------------------------------------------------------------
void TWorld::AppendL(const TFsTreeItemId aId, const TSize& aSize)
    {
    const TInt count(iItems.Count());
    TPoint tl;
    if (count > 0)
        {
        tl.iY = iItems[count - 1].Bottom();
        }
    const TRect modifiedRect(TRect(tl, aSize));
    iItems.AppendL(TItem(aId, modifiedRect));
    iHeight += aSize.iHeight;
    UpdatedL();
    }

// ---------------------------------------------------------------------------
// TWorld::InsertL
// ---------------------------------------------------------------------------
void TWorld::InsertL(const TFsTreeItemId aId, const TSize& aSize,
        const TInt aSlot)
    {
    if (aSlot >= iItems.Count())
        {
        AppendL(aId, aSize);
        }
    else
        {
        const TPoint topLeft(iItems[aSlot].TopLeft());
        iItems.InsertL(TItem(aId, TRect(topLeft, aSize)), aSlot);
        MoveBy(aSlot + 1, aSize.iHeight);
        iHeight += aSize.iHeight;
        UpdatedL();
        }
    }

// ---------------------------------------------------------------------------
// TWorld::RemoveL
// ---------------------------------------------------------------------------
void TWorld::RemoveL(const TFsTreeItemId aId)
    {
    for (TInt i = iItems.Count() - 1; i >= 0; i--)
        {
        const TFsTreeItemId itemId(iItems[i].iId);
        if (itemId == aId)
            {
            const TItem item(iItems[i]);
            iItems.Remove(i);
            const TInt delta(-item.Height());
            MoveBy(i, delta);
            iHeight += delta;
            UpdatedL();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// TWorld::RemoveAllL
// ---------------------------------------------------------------------------
void TWorld::RemoveAllL()
    {
    iHeight = 0;
    iItems.Reset();
    UpdatedL();
    }

// ---------------------------------------------------------------------------
// TWorld::Height
// ---------------------------------------------------------------------------
TInt TWorld::Height() const
    {
    return iHeight;
    }

// ---------------------------------------------------------------------------
// TWorld::ItemCount
// ---------------------------------------------------------------------------
TInt TWorld::ItemCount() const
    {
    return iItems.Count();
    }

// ---------------------------------------------------------------------------
// TWorld::Item
// ---------------------------------------------------------------------------
TFsTreeItemId TWorld::Item(TInt aIndex) const
    {
    return iItems[aIndex].iId;
    }

// ---------------------------------------------------------------------------
// TWorld::IndexOfItem
// ---------------------------------------------------------------------------
TInt TWorld::IndexOfItem(TFsTreeItemId aItem) const
    {
    for (TInt i = 0; i < iItems.Count(); i++)
        {
        if (iItems[i].iId == aItem)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// TWorld::MoveBy
// ---------------------------------------------------------------------------
void TWorld::MoveBy(const TInt aSlot, const TInt aDelta)
    {
    const TInt count(iItems.Count());
    for (TInt i = aSlot; i < count; i++)
        {
        iItems[i].Move(aDelta);
        }
    }

// ---------------------------------------------------------------------------
// TWorld::UpdatedL
// ---------------------------------------------------------------------------
void TWorld::UpdatedL()
    {
    if (!iBatchUpdateOngoing)
        {
        for (TInt i = iObservers.Count() - 1; i >= 0; i--)
            {
            iObservers[i]->WorldUpdatedL(*this);
            }
        }
    }

// ---------------------------------------------------------------------------
// TWorld::GetItemsL
// Uses binary search.
// ---------------------------------------------------------------------------
void TWorld::GetItemsL(RArray<TFsTreeItemId>& aItems, TInt aY, TInt aHeight,
        TInt& aOffset, TInt& aBeginIndex)
    {
    // First find first visible item using binary search algorithm
    aItems.Reset();
    TInt i(0);
    aOffset = 0;
    if (aY > Height())
        {
        aHeight = -1;
        }
    else if (aY > 0)
        {
        TInt min(0);
        TInt max(iItems.Count() - 1);
        while (min <= max)
            {
            const TInt middle(min + (max - min) / 2);
            if (aY < iItems[middle].Top())
                {
                max = middle - 1;
                }
            else if (aY > iItems[middle].Bottom())
                {
                min = middle + 1;
                }
            else
                {
                aOffset = aY - iItems[middle].Top();
                aHeight += aOffset;
                aBeginIndex = i = middle;
                break;
                }
            }
        }
    else
        {
        aOffset = aY;
        aHeight += aY;
        }

    // If item was found, collect enough items to fill the view
    while (aHeight >= 0 && i < iItems.Count())
        {
        aItems.AppendL(iItems[i].iId);
        aHeight -= iItems[i++].Height();
        }
    }

// ---------------------------------------------------------------------------
// TWorld::BeginUpdate
// ---------------------------------------------------------------------------
void TWorld::BeginUpdate()
    {
    iBatchUpdateOngoing = ETrue;
    }

// ---------------------------------------------------------------------------
// TWorld::EndUpdateL
// ---------------------------------------------------------------------------
void TWorld::EndUpdateL()
    {
    if (iBatchUpdateOngoing)
        {
        iBatchUpdateOngoing = EFalse;
        UpdatedL();
        }
    }

// ---------------------------------------------------------------------------
// TWorld::IsUpdating
// ---------------------------------------------------------------------------
TBool TWorld::IsUpdating() const
    {
    return iBatchUpdateOngoing;
    }

// ---------------------------------------------------------------------------
// TWorld::Recalculate
// ---------------------------------------------------------------------------
void TWorld::Recalculate(const CFsTree& aTree)
    {
    TPoint tl;
    iHeight = 0;
    for (TInt i = 0; i < iItems.Count(); i++)
        {
        TItem& item(iItems[i]);
        MFsTreeItemVisualizer* visualizer(aTree.ItemVisualizer(item.iId));
        TSize size(visualizer->Size());
        item.SetRect(TRect(tl, size));
        tl.iY += size.iHeight;
        iHeight += size.iHeight;
        }
    }

// ---------------------------------------------------------------------------
// TWorld::GetItemRect
// ---------------------------------------------------------------------------
TInt TWorld::GetItemRect(const TFsTreeItemId aItemId, TRect& aRect) const
    {
    return GetItemRect(aItemId, aRect, 0);
    }

// ---------------------------------------------------------------------------
// TWorld::GetItemRect
// ---------------------------------------------------------------------------
TInt TWorld::GetItemRect(const TFsTreeItemId aItemId, TRect& aRect,
        const TInt aStartIndex ) const
    {
    TInt result(KErrNotFound);
    const TInt itemCount(iItems.Count());
    for (TInt i = aStartIndex; i < itemCount; i++)
        {
        if (iItems[i].iId == aItemId)
            {
            result = i;
            aRect = iItems[i].Rect();
            break;
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TWorld::GetItemRectByIndex
// ---------------------------------------------------------------------------
TInt TWorld::GetItemRectByIndex(const TInt aIndex, TRect& aRect) const
    {
    TInt result(KErrNotFound);
    if (aIndex < iItems.Count() && aIndex >= 0)
        {
        aRect = iItems[aIndex].Rect();
        result = KErrNone;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TWorld::CheckIntegrity
// ---------------------------------------------------------------------------
TBool TWorld::CheckIntegrity() const
    {
    TBool result(ETrue);
    if (iItems.Count() > 1)
        {
        TRect prev(iItems[0].Rect());
        for (TInt i = 1; i < iItems.Count() - 1; i++)
            {
            const TRect& curr(iItems[i].Rect());
            if (prev.Intersects(curr))
                {
                result = EFalse;
                break;
                }
            prev.iBr.iY += curr.Height();
            }
        }
    return result;
    }

// TViewPort

// ---------------------------------------------------------------------------
// TViewPort::TViewPort
// ---------------------------------------------------------------------------
TViewPort::TViewPort(MObserver& aObserver, TWorld& aWorld) :
    iObserver(aObserver), iWorld(aWorld), iWorldIndex(KErrNotFound)
    {
    }

// ---------------------------------------------------------------------------
// TViewPort::SetPositionL
// ---------------------------------------------------------------------------
void TViewPort::SetPositionL(const TPoint& aPosition, TBool aInformUpdate)
    {
    iPosition = aPosition;
    // Do validation!
    if (aInformUpdate)
        {
        UpdatedL();
        }
    }

// ---------------------------------------------------------------------------
// TViewPort::SetCenterPositionL
// ---------------------------------------------------------------------------
void TViewPort::SetCenterPositionL(const TPoint& aPosition,
        TBool aInformUpdate, TUpdatedByPhysic aUpdateByPhysic)
    {
    iPosition = TPoint(aPosition.iX - iSize.iWidth / 2, aPosition.iY
            - iSize.iHeight / 2);
    // Do validation!
    if (aInformUpdate)
        {
        UpdatedL(aUpdateByPhysic);
        }
    }

// ---------------------------------------------------------------------------
// TViewPort::Position
// ---------------------------------------------------------------------------
const TPoint& TViewPort::Position() const
    {
    return iPosition;
    }

// ---------------------------------------------------------------------------
// TViewPort::CenterPosition
// ---------------------------------------------------------------------------
TPoint TViewPort::CenterPosition() const
    {
    TPoint result(iPosition.iX + iSize.iWidth / 2, iPosition.iY
            + iSize.iHeight / 2);
    return result;
    }

// ---------------------------------------------------------------------------
// TViewPort::SetSizeL
// ---------------------------------------------------------------------------
void TViewPort::SetSizeL(const TSize& aSize)
    {
    iSize = aSize;
    // Do validation!
    if (!iWorld.IsUpdating())
        {
        UpdatedL();
        }
    }

// ---------------------------------------------------------------------------
// TViewPort::Size
// ---------------------------------------------------------------------------
const TSize& TViewPort::Size() const
    {
    return iSize;
    }

// ---------------------------------------------------------------------------
// TViewPort::GetVisibleItemsL
// ---------------------------------------------------------------------------
void TViewPort::GetVisibleItemsL(RArray<TFsTreeItemId>& aItemsToBeRemoved,
        RArray<TFsTreeItemId>& aItemsToBeAdded, TInt& aOffset,
        TBool& aFullUpdate)
    {
    RArray<TFsTreeItemId> tempItems;
    CleanupClosePushL(tempItems);
    iWorld.GetItemsL(tempItems, iPosition.iY, iSize.iHeight, aOffset, iWorldIndex);
    iCache.UpdateL(tempItems, aItemsToBeRemoved, aItemsToBeAdded, aFullUpdate);
    CleanupStack::PopAndDestroy(); // tempItems
    }

// ---------------------------------------------------------------------------
// TViewPort::GetVisibleItemsL
// ---------------------------------------------------------------------------
void TViewPort::GetVisibleItemsL(RArray<TFsTreeItemId>& aVisible, TInt& aOffset)
	{
	iWorld.GetItemsL(aVisible, iPosition.iY, iSize.iHeight, aOffset, iWorldIndex);
	}

// ---------------------------------------------------------------------------
// TViewPort::IsScrollBarNeeded
// ---------------------------------------------------------------------------
TBool TViewPort::IsScrollBarNeeded() const
    {
    return iWorld.Height() > iSize.iHeight;
    }

// ---------------------------------------------------------------------------
// TViewPort::UpdatedL
// ---------------------------------------------------------------------------
void TViewPort::UpdatedL(TUpdatedByPhysic aUpdateByPhysic)
    {
    iObserver.ViewPortUpdatedL(*this, aUpdateByPhysic);
    }

// ---------------------------------------------------------------------------
// TViewPort::WorldUpdatedL
// ---------------------------------------------------------------------------
void TViewPort::WorldUpdatedL(const TWorld& aWorld)
    {
    // recreate viewable set if needed
    if (iPosition.iY > aWorld.Height() || (iPosition.iY + iSize.iHeight
            > aWorld.Height() && iSize.iHeight <= aWorld.Height()))
        {
        iPosition.iY = Max(0, aWorld.Height() - iSize.iHeight);
        }
    else if (iSize.iHeight > aWorld.Height() && iPosition.iY > 0)
        {
        iPosition.iY = 0;
        }
    iObserver.ViewPortUpdatedL(*this);
    }

// ---------------------------------------------------------------------------
// TViewPort::ScrollToItemL
// ---------------------------------------------------------------------------
void TViewPort::ScrollToItemL(const TFsTreeItemId aItemId,
        TBool aInformUpdate)
    {
    TRect itemRect;
    if (iWorld.GetItemRect(aItemId, itemRect) != KErrNotFound)
        {
        SetPositionL(itemRect.iTl, aInformUpdate);
        }
    }

// ---------------------------------------------------------------------------
// TViewPort::CenterToItemL
// ---------------------------------------------------------------------------
void TViewPort::CenterToItemL(const TFsTreeItemId aItemId,
        TBool aInformUpdate)
    {
    TRect itemRect;
    if (iWorld.GetItemRect(aItemId, itemRect) != KErrNotFound)
        {
        TInt top(itemRect.iTl.iY);
        top -= (iSize.iHeight - itemRect.Height()) / 2;
        LimitTop(top);
        SetPositionL(TPoint(iPosition.iX, top), aInformUpdate);
        }
    }

// ---------------------------------------------------------------------------
// TViewPort::ItemToPositionL
// Tries to scroll view so that given item is in given position aPosition.
// ---------------------------------------------------------------------------
void TViewPort::ItemToPositionL(const TFsTreeItemId aItemId,
        const TPoint& aPosition, const TBool aInformUpdate,
        const TPositionHint aHint)
    {
    TRect itemRect;
    if (iWorld.GetItemRect(aItemId, itemRect) != KErrNotFound)
        {
        TInt top(itemRect.iTl.iY);
        top -= aPosition.iY;
        LimitTop(top);
        if (aHint == EPositionAfterExpand)
            {
            const TInt error( iWorld.GetItemRectByIndex(
                    iWorld.IndexOfItem(aItemId) + 1, itemRect) );
            if (!error)
                {
                if (itemRect.iBr.iY - top > iSize.iHeight)
                    {
                    top += itemRect.iBr.iY - top - iSize.iHeight;
                    LimitTop(top);
                    }
                }
            }
        SetPositionL(TPoint(iPosition.iX, top), aInformUpdate);
        }
    }

// ---------------------------------------------------------------------------
// TViewPort::ScrollItemToViewL
// Scrolls item to view. Item is set as top most item, if it is above the current
// display area or bottom most item if it is below the current display area.
// ---------------------------------------------------------------------------
void TViewPort::ScrollItemToViewL(const TFsTreeItemId aItemId,
        TBool aInformUpdate)
    {
    TRect itemRect;
    if (iWorld.GetItemRect(aItemId, itemRect) != KErrNotFound)
        {
        if (itemRect.iTl.iY < iPosition.iY)
            {
            SetPositionL(itemRect.iTl, aInformUpdate);
            }
        if (itemRect.iBr.iY > iPosition.iY + iSize.iHeight)
            {
            SetPositionL(
                    TPoint(iPosition.iX, itemRect.iBr.iY - iSize.iHeight),
                    aInformUpdate);
            }
        }
    }

// ---------------------------------------------------------------------------
// TViewPort::MakeViewFullyOccupiedL
// ---------------------------------------------------------------------------
void TViewPort::MakeViewFullyOccupiedL(TBool aInformUpdate)
    {
    TInt top(iPosition.iY);
    LimitTop(top);
    SetPositionL(TPoint(iPosition.iX, top), aInformUpdate);
    }

// ---------------------------------------------------------------------------
// TViewPort::TopItem
// ---------------------------------------------------------------------------
TFsTreeItemId TViewPort::TopItem() const
    {
    return iCache.TopItem();
    }

// ---------------------------------------------------------------------------
// TViewPort::ClearCache
// ---------------------------------------------------------------------------
void TViewPort::ClearCache()
    {
    iWorldIndex = KErrNotFound;
    iCache.Clear();
    }

// ---------------------------------------------------------------------------
// TViewPort::ItemRect
// ---------------------------------------------------------------------------
TRect TViewPort::ItemRect(const TFsTreeItemId aItemId) const
    {
    TRect itemRect;
    if (!iCache.IsClear() && iCache.ContainsItem(aItemId))
        {
        const TInt beginIndex(iWorldIndex == KErrNotFound ? 0 : iWorldIndex);
        if (iWorld.GetItemRect(aItemId, itemRect, beginIndex) != KErrNotFound)
            {
            itemRect.Move(0, -iPosition.iY);
            }
        }
    return itemRect;
    }

// ---------------------------------------------------------------------------
// TViewPort::ItemFullyVisible
// ---------------------------------------------------------------------------
TBool TViewPort::ItemFullyVisible(const TFsTreeItemId aItemId) const
    {
    TBool result(EFalse);
    if (!iCache.IsClear() && iCache.ContainsItem(aItemId))
        {
        TRect itemRect;
        if (iWorld.GetItemRect(aItemId, itemRect) != KErrNotFound)
            {
            if (itemRect.Height())
                {
                const TRect viewRect(iPosition, iSize);
                result = viewRect.Contains(itemRect.iTl)
                        && viewRect.Contains(itemRect.iBr);
                }
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TViewPort::ItemRect
// ---------------------------------------------------------------------------
void TViewPort::LimitTop(TInt& aTop) const
    {
    aTop = Min(iWorld.Height() - iSize.iHeight, aTop);
    aTop = Max(0, aTop);
    }

// ---------------------------------------------------------------------------
// TViewPort::TCache::TCache
// ---------------------------------------------------------------------------
TViewPort::TCache::TCache() :
    iClear(ETrue)
    {
    }

// ---------------------------------------------------------------------------
// TViewPort::TCache::~TCache
// ---------------------------------------------------------------------------
TViewPort::TCache::~TCache()
    {
    iCachedItems.Reset();
    }

// ---------------------------------------------------------------------------
// TViewPort::TCache::Clear
// ---------------------------------------------------------------------------
void TViewPort::TCache::Clear()
    {
    iClear = ETrue;
    iCachedItems.Reset();
    }

// ---------------------------------------------------------------------------
// TViewPort::TCache::IsClear
// ---------------------------------------------------------------------------
TBool TViewPort::TCache::IsClear() const
    {
    return iClear;
    }

// ---------------------------------------------------------------------------
// TViewPort::TCache::ContainsItem
// ---------------------------------------------------------------------------
TBool TViewPort::TCache::ContainsItem(const TFsTreeItemId aItemId) const
    {
    return iCachedItems.Find(aItemId) != KErrNotFound;
    }

// ---------------------------------------------------------------------------
// TViewPort::TCache::UpdateL
// ---------------------------------------------------------------------------
void TViewPort::TCache::UpdateL(const RArray<TFsTreeItemId>& aItems, RArray<
        TFsTreeItemId>& aItemsToBeRemoved,
        RArray<TFsTreeItemId>& aItemsToBeAdded, TBool& aFullUpdate)
    {
    aFullUpdate = iClear;
    if (!iClear)
        {
        // find items in cache, missing in new set => items to be removed
        aItemsToBeRemoved.Reset();
        for (TInt i = 0; i < iCachedItems.Count(); i++)
            {
            if (aItems.Find(iCachedItems[i]) == KErrNotFound)
                {
                aItemsToBeRemoved.AppendL(iCachedItems[i]);
                }
            }

        // find items in new set, missing in cache => items to be added
        aItemsToBeAdded.Reset();
        for (TInt i = 0; i < aItems.Count(); i++)
            {
            if (iCachedItems.Find(aItems[i]) == KErrNotFound)
                {
                aItemsToBeAdded.AppendL(aItems[i]);
                }
            else
                {
                aItemsToBeAdded.AppendL(KFsTreeNoneID);
                }
            }
        }
    else
        {
        // find items in new set, missing in cache => items to be added
        aItemsToBeAdded.Reset();
        for (TInt i = 0; i < aItems.Count(); i++)
            {
            aItemsToBeAdded.AppendL(aItems[i]);
            }
        }
    // finally update cache
    iCachedItems.Reset();
    for (TInt i = 0; i < aItems.Count(); i++)
        {
        iCachedItems.AppendL(aItems[i]);
        }
    iClear = EFalse;
    }

// ---------------------------------------------------------------------------
// TViewPort::TCache::TopItem
// ---------------------------------------------------------------------------
TFsTreeItemId TViewPort::TCache::TopItem() const
    {
    if (iClear || !iCachedItems.Count())
        {
        return KFsTreeNoneID;
        }
    return iCachedItems[0];
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeVisualizerBase* CFsTreeVisualizerBase::NewL(
        CAlfControl* aOwnerControl, CAlfLayout& aParent,
        const TBool aPopUpMode)
    {
    FUNC_LOG;
    CFsTreeVisualizerBase* self = new (ELeave) CFsTreeVisualizerBase(
            aOwnerControl, aParent, aPopUpMode);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreeVisualizerBase::~CFsTreeVisualizerBase()
    {
    FUNC_LOG;
    delete iDragHandler;
    delete iPhysics;
    if (iOwnerControl)
        {
        iOwnerControl->Env().RemoveActionObserver(this);
        }
    delete iBorderBrush;
    iVisibleItems.Close();
    iVisibleItemsBackground.Close();
    iCustomPageUpKeys.Close();
    iCustomPageDownKeys.Close();
    delete iFadeEffect;
    delete iSlideEffect;
    delete iWatermark;
    delete iTextStyleManager;
    delete iIntx;
    if ( iScrollBar )
        {
        iScrollBar->Destroy();
        }
    }

// ---------------------------------------------------------------------------
//Sets data model for the visualizer.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetTreeData(CFsTree& aTreeData)
    {
    FUNC_LOG;
    iTreeData = &aTreeData;
    }

// ---------------------------------------------------------------------------
// Adds custom key which triggers page-up event.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::AddCustomPageUpKey(TInt aKeyCode)
    {
    FUNC_LOG;
    iCustomPageUpKeys.InsertInOrder(aKeyCode);
    }

// ---------------------------------------------------------------------------
// Removes given key code from custom page-up event key codes.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::RemoveCustomPageUpKey(TInt aKeyCode)
    {
    FUNC_LOG;
    TBool keyRemoved = EFalse;

    TInt index = iCustomPageUpKeys.FindInOrder(aKeyCode);
    if (index != KErrNotFound)
        {
        iCustomPageUpKeys.Remove(index);
        keyRemoved = ETrue;
        }

    return keyRemoved;
    }

// ---------------------------------------------------------------------------
// Checks whether given key is a custom key for page-up event.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsCustomPageUpKey(TInt aKeyCode)
    {
    FUNC_LOG;
    TBool keyExists = EFalse;
    TInt index = iCustomPageUpKeys.FindInOrder(aKeyCode);
    if (index != KErrNotFound)
        {
        keyExists = ETrue;
        }
    return keyExists;
    }

// ---------------------------------------------------------------------------
// Adds custom key which triggers page-down event.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::AddCustomPageDownKey(TInt aKeyCode)
    {
    FUNC_LOG;
    iCustomPageDownKeys.InsertInOrder(aKeyCode);
    }

// ---------------------------------------------------------------------------
// Removes given key code from custom page-down event key codes.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::RemoveCustomPageDownKey(TInt aKeyCode)
    {
    FUNC_LOG;
    TBool keyRemoved = EFalse;

    TInt index = iCustomPageDownKeys.FindInOrder(aKeyCode);
    if (index != KErrNotFound)
        {
        iCustomPageDownKeys.Remove(index);
        keyRemoved = ETrue;
        }

    return keyRemoved;
    }

// ---------------------------------------------------------------------------
//  Checks whether given key is a custom key for page-down event.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsCustomPageDownKey(TInt aKeyCode)
    {
    FUNC_LOG;
    TBool keyExists = EFalse;
    TInt index = iCustomPageDownKeys.FindInOrder(aKeyCode);
    if (index != KErrNotFound)
        {
        keyExists = ETrue;
        }
    return keyExists;
    }

// ---------------------------------------------------------------------------
//  Sets icon for a mark sign.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetMarkIcon(CAlfTexture& aMarkIcon)
    {
    FUNC_LOG;
    iMarkIcon = &aMarkIcon;
    }

// ---------------------------------------------------------------------------
//  Returns icon used to mark items.
// ---------------------------------------------------------------------------
//
CAlfTexture* CFsTreeVisualizerBase::MarkIcon()
    {
    FUNC_LOG;
    return iMarkIcon;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsMarked(const TFsTreeItemId aItemId) const
    {
    FUNC_LOG;
    MFsTreeItemVisualizer* vis = NULL;
    vis = iTreeData->ItemVisualizer(aItemId);
    return vis->IsMarked();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::MarkItemL(const TFsTreeItemId aItemId,
        TBool aMarked)
    {
    FUNC_LOG;
    MFsTreeItemVisualizer* vis = NULL;
    vis = iTreeData->ItemVisualizer(aItemId);
    vis->SetMarked(aMarked);
    if (iVisibleItems.Find(aItemId) != KErrNotFound)
        {
        MFsTreeItemData* data = NULL;
        data = &(iTreeData->ItemData(aItemId));

        vis->UpdateL(*data, IsItemFocused(aItemId) && IsFocusShown(),
                iTreeData->Level(aItemId), iMarkIcon, iMenuIcon);
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::GetMarkedItemsL(RFsTreeItemIdList& aMarkedItems) const
    {
    FUNC_LOG;
    TFsTreeIterator treeIter = iTreeData->Iterator(KFsTreeRootID,
            KFsTreeRootID);
    TFsTreeItemId itemId;
    MFsTreeItemVisualizer* vis = NULL;
    while (treeIter.HasNext())
        {
        itemId = treeIter.Next();
        vis = iTreeData->ItemVisualizer(itemId);
        if (vis->IsMarked())
            {
            aMarkedItems.AppendL(itemId);
            }
        }
    }

// ---------------------------------------------------------------------------
//  Function marks/unmarks all items in the list.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::MarkAllItemsL(TBool aMarkAll)
    {
    FUNC_LOG;
    TFsTreeIterator treeIter = iTreeData->Iterator(KFsTreeRootID,
            KFsTreeRootID);
    TFsTreeItemId itemId;
    MFsTreeItemVisualizer* vis = NULL;
    while (treeIter.HasNext())
        {
        itemId = treeIter.Next();
        vis = iTreeData->ItemVisualizer(itemId);
        vis->SetMarked(aMarkAll);
        }
    RefreshListViewL();
    }

// ---------------------------------------------------------------------------
//  Sets item indentation per hierarchy level.
// ---------------------------------------------------------------------------
void CFsTreeVisualizerBase::SetIndentationL(TInt aIndentation)
    {
    FUNC_LOG;
    iLevelIndentation = aIndentation;

    MFsTreeItemVisualizer* vis = NULL;
    TFsTreeItemId itemId = KFsTreeNoneID;
    //loop through the list and set items to always-extended state.
    TFsTreeIterator iter = iTreeData->Iterator(KFsTreeRootID, KFsTreeRootID);
    while (iter.HasNext())
        {
        itemId = iter.Next();
        //get item's visualizer
        vis = iTreeData->ItemVisualizer(itemId);
        //set indentation in pixels
        vis->SetIndentation(iLevelIndentation);
        }

    //refresh the list view
    RefreshListViewL();
    }

// ---------------------------------------------------------------------------
//  Returns indentation per hierarchy level in pixels.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::Indentation() const
    {
    FUNC_LOG;
    return iLevelIndentation;
    }

// ---------------------------------------------------------------------------
//  Sets icon for a menu sign.
// ---------------------------------------------------------------------------
void CFsTreeVisualizerBase::SetMenuIcon(CAlfTexture& aMenuIcon)
    {
    FUNC_LOG;
    iMenuIcon = &aMenuIcon;
    }

// ---------------------------------------------------------------------------
//  Returns menu icon.
// ---------------------------------------------------------------------------
CAlfTexture* CFsTreeVisualizerBase::MenuIcon() const
    {
    FUNC_LOG;
    return iMenuIcon;
    }

// ---------------------------------------------------------------------------
//  Used internally by the list. To make the list focused/not focused use
//  CFsTreeList::SetFocusedL() instead.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetFocusedL(const TBool aFocused)
    {
    FUNC_LOG;
    if (iDragHandler && !aFocused)
        {
        iDragHandler->Reset();
        }
    if (iFlags.IsSet(EListFocused) != aFocused)
        {
        iFlags.Assign(EListFocused, aFocused);
        if (iTreeData->Count())
            {
            if (iFlags.IsSet(EListFocused))
                {
                //list will receive focus
                if (iFocusedItem == KFsTreeNoneID)
                    {
                    TFsTreeIterator treeIter = iTreeData->Iterator(
                            KFsTreeRootID, KFsTreeRootID,
                            KFsTreeIteratorSkipCollapsedFlag
                                    | KFsTreeIteratorSkipHiddenFlag);

                    TFsTreeItemId id(KFsTreeNoneID);
                    MFsTreeItemVisualizer* visualizer(NULL);
                    id = treeIter.First();
                    visualizer = iTreeData->ItemVisualizer(id);
                    if (visualizer)
                        {
                        if (visualizer->IsFocusable())
                            {
                            iFocusedItem = id;
                            }
                        }
                    }
                // a workaround to make SetFocusedItemL work with timeout
                SetFocusedItemL(iFocusedItem);
                MakeSelectorVisibleL(ETrue);
                }
            else
                {
                if (iFocusedItem != KFsTreeNoneID)
                    {
                    //list will loose focus
                    MFsTreeItemVisualizer* visualizer =
                            iTreeData->ItemVisualizer(iFocusedItem);
                    if (visualizer)
                        {
                        TInt index(iListLayout->FindVisual(
                                &visualizer->Layout()));
                        if (index != KErrNotFound)
                            {
                            MFsTreeItemData* data = &iTreeData->ItemData(
                                    iFocusedItem);
                            MakeSelectorVisibleL(EFalse);
                            visualizer->UpdateL(*data, EFalse,
                                    iTreeData->Level(iFocusedItem),
                                    iMarkIcon, iMenuIcon, iCurrentScrollSpeed);
                            }
                        }
                    }
                }
            iListLayout->UpdateChildrenLayout();
            }
        }
    }

// ---------------------------------------------------------------------------
// The function sets some properties of the selector like bitmap, type
// of movement.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetSelectorPropertiesL(CAlfTexture* aTexture,
        TFsSelectorMovement aMovementType, TInt aLeftBorderWidth,
        TInt aRightBorderWidth, TInt aTopBorderHeight,
        TInt aBottomBorderHeight)
    {
    FUNC_LOG;
    iSelectorTexture = aTexture;
    iSelectorBrush = NULL;
    iSelectorMovementType = aMovementType;
    iSelectorOpacity = 1.0;

    iLeftSelectorBorderWidth = aLeftBorderWidth;
    iRightSelectorBorderWidth = aRightBorderWidth;
    iTopSelectorBorderHeight = aTopBorderHeight;
    iBottomSelectorBorderHeight = aBottomBorderHeight;

    DestroySelectorVisualL();
    MakeSelectorVisibleL(ETrue);
    }

// ---------------------------------------------------------------------------
// The function sets some properties of the selector.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetSelectorPropertiesL(CAlfBrush* aSelectorBrush,
        TReal32 aOpacity, TFsSelectorMovement aMovementType)
    {
    FUNC_LOG;
    iSelectorTexture = NULL;
    iSelectorBrush = aSelectorBrush;
    iSelectorMovementType = aMovementType;
    //check value, selector cannot be fully transparent ( <= 0.0 )
    if (aOpacity <= 0.0 || aOpacity > 1.0)
        {
        iSelectorOpacity = 1.0;
        }
    else
        {
        iSelectorOpacity = aOpacity;
        }

    iLeftSelectorBorderWidth = 0;
    iRightSelectorBorderWidth = 0;
    iTopSelectorBorderHeight = 0;
    iBottomSelectorBorderHeight = 0;

    DestroySelectorVisualL();
    MakeSelectorVisibleL(ETrue);
    }

// ---------------------------------------------------------------------------
// The function returnes selector's properties.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::GetSelectorProperties(CAlfTexture*& aTexture,
        TFsSelectorMovement& aMovementType, TInt& aLeftBorderWidth,
        TInt& aRightBorderWidth, TInt& aTopBorderHeight,
        TInt& aBottomBorderHeight)
    {
    FUNC_LOG;
    aTexture = iSelectorTexture;
    aMovementType = iSelectorMovementType;

    aLeftBorderWidth = iLeftSelectorBorderWidth;
    aRightBorderWidth = iRightSelectorBorderWidth;
    aTopBorderHeight = iTopSelectorBorderHeight;
    aBottomBorderHeight = iBottomSelectorBorderHeight;
    }

// ---------------------------------------------------------------------------
// The function sets a text for empty list's view.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetEmptyListTextL(const TDesC& aText)
    {
    FUNC_LOG;
    if (iRootData)
        {
        iRootData->SetDataL(aText);
        }
    }

// ---------------------------------------------------------------------------
// The function returns a text for empty list's view.
// ---------------------------------------------------------------------------
//
TDesC& CFsTreeVisualizerBase::GetEmptyListText() const
    {
    FUNC_LOG;
    return iRootData->Data();
    }

// ---------------------------------------------------------------------------
//  Handles key event forwarded by the controller.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::HandleKeyEventL(const TAlfEvent& aEvent)
    {
    //ignore  KeyUp events
    TBool eventHandled = aEvent.Code() == EEventKeyUp;

    if (iTreeData->Count() && (!eventHandled))
        {
        TInt nowOffset = iListLayout->ScrollOffset().iY.ValueNow();
        TInt targetOffset = iListLayout->ScrollOffset().iY.Target();

        if (!(iFlags.IsSet(EIsPageUpDownActive)
                && (nowOffset != targetOffset)))
            {
            iFlags.Clear(EIsPageUpDownActive);

            switch (aEvent.KeyEvent().iScanCode)
                {
                case EStdKeyUpArrow:
                    {
                    if (iFocusedItem != KFsTreeNoneID)
                        {
                        SetCurrentScrollSpeed(aEvent.KeyEvent().iRepeats);
                        MoveSelectionL(EFsTreeVisualizerMoveLineUp);
                        //iCurrentScrollSpeed = iScrollSpeed;
                        }
                    eventHandled = ETrue;
                    break;
                    }
                case EStdKeyDownArrow:
                    {
                    if (iFocusedItem != KFsTreeNoneID)
                        {
                        SetCurrentScrollSpeed(aEvent.KeyEvent().iRepeats);
                        MoveSelectionL(EFsTreeVisualizerMoveLineDown);
                        //iCurrentScrollSpeed = iScrollSpeed;
                        }
                    eventHandled = ETrue;
                    break;
                    }
                case EStdKeyLeftArrow:
                    {
                    if (iVisualizerObserver)
                        {
                        iVisualizerObserver->TreeVisualizerEventL(
                                MFsTreeVisualizerObserver::EFsTreeKeyLeftArrow,
                                iFocusedItem);
                        }
                    eventHandled = ETrue;
                    break;
                    }
                case EStdKeyRightArrow:
                    {
                    if (iVisualizerObserver)
                        {
                        iVisualizerObserver->TreeVisualizerEventL(
                                MFsTreeVisualizerObserver::EFsTreeKeyRightArrow,
                                iFocusedItem);
                        }
                    eventHandled = ETrue;
                    break;
                    }
                case EStdKeyDevice3:
                    {
                    if (iTreeData->IsNode(iFocusedItem))
                        {
                        if (iTreeData->NodeVisualizer(iFocusedItem)->IsExpanded())
                            {
                            CollapseNodeL(iFocusedItem);
                            }
                        else
                            {
                            ExpandNodeL(iFocusedItem);
                            }
                        }
                    else
                        {
                        if (iVisualizerObserver)
                            {
                            if (iTreeData->ItemVisualizer(iFocusedItem)->Menu())
                                {
                                iVisualizerObserver->TreeVisualizerEventL(
                                        MFsTreeVisualizerObserver::EFsTreeItemWithMenuSelected,
                                        iFocusedItem);
                                }
                            else
                                {
                                iVisualizerObserver->TreeVisualizerEventL(
                                        MFsTreeVisualizerObserver::EFsTreeItemSelected,
                                        iFocusedItem);
                                }
                            }
                        }
                    eventHandled = ETrue;
                    break;
                    }
                default:
                    {
                    // the visualizer ignores all other events
                    break;
                    }
                }
            //page up/down
            if (!eventHandled)
                {
                if (IsCustomPageUpKey(aEvent.KeyEvent().iScanCode))
                    {
                    SetCurrentScrollSpeed(aEvent.KeyEvent().iRepeats);
                    MoveSelectionL(EFsTreeVisualizerMovePageUp);
                    iFlags.Set(EIsPageUpDownActive);
                    eventHandled = ETrue;
                    //iCurrentScrollSpeed = iScrollSpeed;
                    }
                else if (IsCustomPageDownKey(aEvent.KeyEvent().iScanCode))
                    {
                    SetCurrentScrollSpeed(aEvent.KeyEvent().iRepeats);
                    MoveSelectionL(EFsTreeVisualizerMovePageDown);
                    iFlags.Set(EIsPageUpDownActive);
                    eventHandled = ETrue;
                    //iCurrentScrollSpeed = iScrollSpeed;
                    }
                }
            }
        else
            {
            eventHandled = EFalse;
            }
        }
    else
        {
        eventHandled = EFalse;
        }

    return eventHandled;
    }

// ---------------------------------------------------------------------------
// CFsTreeVisualizerBase::SetFlipState
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetFlipState( TBool aOpen )
    {
    iFlipOpen = aOpen;
    }

// ---------------------------------------------------------------------------
// CFsTreeVisualizerBase::SetFocusVisibility
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetFocusVisibility( TBool aShow )
    {
    if ( ( iFocusVisible && !aShow ) || ( !iFocusVisible && aShow ) )
        {
        iFocusVisible = aShow;
        TRAP_IGNORE(
            MakeSelectorVisibleL( aShow );
            UpdateItemL( iFocusedItem ); );
        }
    }

// ---------------------------------------------------------------------------
// CFsTreeVisualizerBase::HandlePointerEventL
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::HandlePointerEventL(const TAlfEvent& aEvent)
    {
    TBool eventHandled(EFalse);
    if (iDragHandler && iOwnerControl->ControlGroup()->AcceptInput())
        {
        TPointerEvent::TType type = aEvent.PointerEvent().iType;
        const TFsTreeItemId id(EventItemId(aEvent));
        INFO_1("visual: $%x", aEvent.Visual());
        if ( KFsTreeNoneID != id || type == TPointerEvent::EDrag ||
                type  == TPointerEvent::EButtonRepeat ||
                type  == TPointerEvent::EButton1Up ||
                type  == TPointerEvent::EButton1Down)
            {
            switch (type)
                {
                case TPointerEvent::EButton1Down:
                    {
                    eventHandled = ETrue;
                    iTouchPressed = ETrue;
                    const TPoint& pos(aEvent.PointerEvent().iParentPosition);
                    INFO_2("EButton1Down (%d, %d)", pos.iX, pos.iY);
                    //if (!iFlags.IsSet(EPhysicsOn))
                    // Keep this last!
                    if (iFlags.IsSet(EPhysicsOn))
                        {
                        iPhysics->StopPhysics();
                        //iPhysics->ResetFriction();
                        UpdatePhysicsL();
                        iFlags.Set(EIgnorePointerUpAction);
                        }
                    else
                        {
                        iFlags.Clear(EIgnorePointerUpAction);
                        }
                    iDragHandler->PointerDown(aEvent.PointerEvent(), id);
                    break;
                    }
                case TPointerEvent::EButton1Up:
                    {
                    const TPoint& pos(aEvent.PointerEvent().iParentPosition);
                    if( iTouchPressed )
                        {
                        eventHandled = ETrue;
                        iTouchPressed = EFalse;
                        iVisualizerObserver->TreeVisualizerEventL(
                            MFsTreeVisualizerObserver::EFsChangeFocusVisibility );
                        INFO_2( "EButton1Up (%d, %d)", pos.iX, pos.iY );
                        }
                    /*if( !iDragHandler->IsFlicking() )
                        {
                        iPhysics->StopPhysics();
                        }*/
                    iDragHandler->PointerUp( aEvent.PointerEvent(), id );
                    iFlags.Clear(EIgnorePointerUpAction);
                    iFlags.Clear( EListPanning );
                    break;
                    }
                case TPointerEvent::EDrag:
                    {
                    eventHandled = ETrue;
                    const TPoint& pos(aEvent.PointerEvent().iPosition);
                    INFO_2("EDrag (%d, %d)", pos.iX, pos.iY);
                    if (iDragHandler->IsFlicking()
                            || iDragHandler->DragDelta(aEvent.PointerEvent())
                                    >= iPhysics->DragThreshold())
                        {
                        iFlags.Set(EIgnorePointerUpAction);
                        iDragHandler->PointerDrag(aEvent.PointerEvent(), id);
                        iDragHandler->EnableFlicking();
                        }
                    break;
                    }

                case TPointerEvent::EButtonRepeat:
                    {
                    eventHandled = ETrue;
                    const TPoint& pos(aEvent.PointerEvent().iParentPosition);
                    INFO_2("EButtonRepeat (%d, %d)", pos.iX, pos.iY);
                    iDragHandler->PointerRepeat(aEvent.PointerEvent());
                    iFlags.Clear(EIgnorePointerUpAction);
                    break;
                    }

                default:
                    {
                    //Unknown event, ignore.
                    break;
                    }
                }
            }
        else
        	{
        	iVisualizerObserver->TreeVisualizerEventL(
					MFsTreeVisualizerObserver::EFsChangeFocusVisibility );
        	}
        }
    return eventHandled;
    }
// ---------------------------------------------------------------------------
//  CFsTreeVisualizerBase::EventItemId
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTreeVisualizerBase::EventItemId(const TAlfEvent& aEvent) const
    {
    FUNC_LOG;
    if (aEvent.Type() == TPointerEvent::EDrag)
        {
        return VisualItemId(FindVisualUnderDrag(
                aEvent.PointerEvent().iPosition));
        }
    else
        {
        return VisualItemId(aEvent.Visual());
        }
    }

// ---------------------------------------------------------------------------
//  CFsTreeVisualizerBase::FindVisualUnderDrag
// ---------------------------------------------------------------------------
//
const CAlfVisual* CFsTreeVisualizerBase::FindVisualUnderDrag(
        const TPoint& aPosition) const
    {
    FUNC_LOG;
    for (TInt i = 0; i < iListLayout->Count(); i++)
        {
        const TRect displayRect(iListLayout->Visual(i).DisplayRectTarget());
        if (displayRect.Contains(aPosition))
            {
            return &iListLayout->Visual(i);
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
//  CFsTreeVisualizerBase::VisualItemId
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTreeVisualizerBase::VisualItemId(const CAlfVisual* aVisual) const
    {
    FUNC_LOG;
    TFsTreeItemId itemId( KFsTreeNoneID );
    if (aVisual)
        {
        aVisual->PropertyFindInteger(KPropertyItemId(), &itemId);
        }
    return itemId;
    }

// ---------------------------------------------------------------------------
// Sets the specified item as focused.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetFocusedItemL( const TFsTreeItemId aItemId,
		TBool /*aCheckFocus*/ )
    {
    FUNC_LOG;
    MFsTreeItemVisualizer* visualizer = NULL;

    if ( aItemId != iFocusedItem )
        {
        visualizer = iTreeData->ItemVisualizer( iFocusedItem );

        if ( visualizer )
            {
            visualizer->UpdateL( iTreeData->ItemData( iFocusedItem ), EFalse,
            		iTreeData->Level( iFocusedItem ), iMarkIcon, iMenuIcon, 0 );
            }

        iFocusedItem = aItemId;
        }

    visualizer = iTreeData->ItemVisualizer( iFocusedItem );

    if ( visualizer )
        {
        visualizer->UpdateL( iTreeData->ItemData( iFocusedItem ), IsFocusShown(),
                iTreeData->Level( iFocusedItem ), iMarkIcon, iMenuIcon, 0 );
        }

    if ( iFocusedItem != KFsTreeNoneID )
        {
        UpdateSelectorVisualL();

        if ( !iViewPort.ItemFullyVisible( iFocusedItem ) )
            {
            iViewPort.ScrollItemToViewL( iFocusedItem );
            }
        }
    }

// ---------------------------------------------------------------------------
// Gets item vertical position in the list
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::GetItemWorldPosition( const TInt aIdx )
    {
    TRect rect;
    iWorld.GetItemRectByIndex( aIdx, rect);
    return rect.iTl.iY;
    }

// ---------------------------------------------------------------------------
// Checks if the specified item is focused.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTreeVisualizerBase::FocusedItem() const
    {
    FUNC_LOG;
    return iFocusedItem;
    }

// ---------------------------------------------------------------------------
// Checks if the specified item is focused.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsFocused(const TFsTreeItemId aItemId) const
    {
    FUNC_LOG;
    return iFocusedItem == aItemId ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// Sets an item as first visible one in the list.
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsTreeVisualizerBase::SetFirstVisibleItemL(const TFsTreeItemId /*aItemId*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Get an id of the first visible item.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTreeVisualizerBase::FirstVisibleItem()
    {
    FUNC_LOG;
    TFsTreeItemId retId = KFsTreeNoneID;
    if (iVisibleItems.Count())
        {
        // <cmail>
        //        TRAP_IGNORE( ClearVisibleItemsListL(EFalse) );
        // </cmail>
        retId = iVisibleItems[0];
        }
    else
        {
        retId = KFsTreeNoneID;
        }
    return retId;
    }

// ---------------------------------------------------------------------------
// Gets a pointer to the root's item data object.
// ---------------------------------------------------------------------------
//
MFsTreeItemData* CFsTreeVisualizerBase::RootItemData()
    {
    FUNC_LOG;
    return iRootData;
    }

// ---------------------------------------------------------------------------
// Gets a pointer to the root's item visualizer object.
// ---------------------------------------------------------------------------
//
MFsTreeNodeVisualizer* CFsTreeVisualizerBase::RootNodeVisualizer()
    {
    FUNC_LOG;
    return iRootVisualizer;
    }

// ---------------------------------------------------------------------------
// Displays the list.
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsTreeVisualizerBase::ShowListL(const TBool aFadeIn,
        const TBool aSlideIn)
    {
    FUNC_LOG;
    iVisualizationState = EFsTreeVisible;
    if (iTreeData->Count() == 0)
        {
        iRootVisualizer->ShowL(*iRootLayout);
        iRootVisualizer->UpdateL(*iRootData, EFalse, 0, iMarkIcon, iMenuIcon);
        }
    else
        {
        const TBool showFocus = IsFocusShown();
        for (TUint i = 0; i < iVisibleItems.Count(); ++i)
            {
            TFsTreeItemId itemId(iVisibleItems[i]);
            MFsTreeItemVisualizer* visualizer = iTreeData->ItemVisualizer(
                    itemId);

            TBool itemFocused( showFocus && IsItemFocused(itemId) );
			if ( visualizer )
				{
	        	visualizer->UpdateL(iTreeData->ItemData(itemId), itemFocused,
	                iTreeData->Level(itemId), iMarkIcon, iMenuIcon,
	                0);
				}
            }
        }
    UpdateViewPortL();

    TInt fadeInTime(KZero), slideInTime(KZero);
    CFsSlideEffect::TSlideEffectDirection slideInDir(
            CFsSlideEffect::ESlideNone);


    if (aFadeIn)
        {
        fadeInTime = iFadeInEffectTime;
        if (aSlideIn)
            {
            iVisualizationState = EFsTreeFadingInSlidingIn;
            }
        else
            {
            iVisualizationState = EFsTreeFadingIn;
            }
        }
    else
        {
        fadeInTime = KZero;
        }

    if (aSlideIn)
        {
        slideInTime = iSlideInDuration;
        switch (iSlideInDirection)
            {
            case MFsTreeVisualizer::ESlideFromTop:
                {
                slideInDir = CFsSlideEffect::ESlideFromTop;
                break;
                }
            case MFsTreeVisualizer::ESlideFromBottom:
                {
                slideInDir = CFsSlideEffect::ESlideFromBottom;
                break;
                }
            case MFsTreeVisualizer::ESlideFromLeft:
                {
                slideInDir = CFsSlideEffect::ESlideFromLeft;
                break;
                }
            case MFsTreeVisualizer::ESlideFromRight:
                {
                slideInDir = CFsSlideEffect::ESlideFromRight;
                break;
                }
            case MFsTreeVisualizer::ESlideNone:
            default:
                {
                slideInDir = CFsSlideEffect::ESlideNone;
                break;
                }
            }
        iVisualizationState = EFsTreeFadingInSlidingIn;
        }
    else
        {
        slideInTime = KZero;
        slideInDir = CFsSlideEffect::ESlideNone;
        }

    iFadeEffect->SetTime(fadeInTime);
    iSlideEffect->SetTime(slideInTime);

    iFadeEffect->FadeIn();
    iSlideEffect->SlideIn(slideInDir);

    UpdateScrollBarL();
    }
// </cmail>

// ---------------------------------------------------------------------------
// The function assures that the list view is correct.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ValidateListLayoutL(TInt /*aTime*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Hides the list.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::HideList(const TBool aFadeOut,
        const TBool aSlideOut)
    {
    FUNC_LOG;
    iScrollBar->Show( EFalse );
    if (iDragHandler)
        {
        iDragHandler->Reset();
        }

    if (aFadeOut)
        {
        iFadeEffect->SetTime(iFadeOutEffectTime);
        iVisualizationState = EFsTreeFadingOut;
        iFadeEffect->FadeOut();
        }

    if (aSlideOut)
        {
        iSlideEffect->SetTime(iSlideOutDuration);
        if (iVisualizationState == EFsTreeFadingOut)
            {
            iVisualizationState = EFsTreeFadingOutSlidingOut;
            }
        else
            {
            iVisualizationState = EFsTreeSlidingOut;
            }
        switch (iSlideOutDirection)
            {
            case MFsTreeVisualizer::ESlideFromTop:
                {
                iSlideEffect->SlideOut(CFsSlideEffect::ESlideFromTop);
                break;
                }
            case MFsTreeVisualizer::ESlideFromBottom:
                {
                iSlideEffect->SlideOut(CFsSlideEffect::ESlideFromBottom);
                break;
                }
            case MFsTreeVisualizer::ESlideFromLeft:
                {
                iSlideEffect->SlideOut(CFsSlideEffect::ESlideFromLeft);
                break;
                }
            case MFsTreeVisualizer::ESlideFromRight:
                {
                iSlideEffect->SlideOut(CFsSlideEffect::ESlideFromRight);
                break;
                }
            case MFsTreeVisualizer::ESlideNone:
            default:
                {
                iSlideEffect->SlideOut(CFsSlideEffect::ESlideNone);
                break;
                }
            }
        }

    if (!aFadeOut && !aSlideOut)
        {
        iVisualizationState = EFsTreeFadingOut;
        iFadeEffect->SetTime(KZero);
        iFadeEffect->FadeOut();
        }
    }

// ---------------------------------------------------------------------------
//  The function sets duration of the slide-in effect.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetSlideInDuration(TInt aTimeMiliseconds)
    {
    FUNC_LOG;
    iSlideInDuration = aTimeMiliseconds;
    if (iSlideInDuration < KZero)
        {
        iSlideInDuration = KZero;
        }
    }

// ---------------------------------------------------------------------------
//  The function returns slide in effect's duration.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::SlideInDuration() const
    {
    FUNC_LOG;
    return iSlideInDuration;
    }

// ---------------------------------------------------------------------------
//  The function sets direction of the slide in effect.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetSlideInDirection(TFsSlideEffect aDirection)
    {
    FUNC_LOG;
    iSlideInDirection = aDirection;
    }

// ---------------------------------------------------------------------------
//  The function returns direction of the slide in effect.
// ---------------------------------------------------------------------------
//
MFsTreeVisualizer::TFsSlideEffect CFsTreeVisualizerBase::SlideInDirection() const
    {
    FUNC_LOG;
    return iSlideInDirection;
    }

// ---------------------------------------------------------------------------
//  The function sets duration of the slide-out effect.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetSlideOutDuration(TInt aTimeMiliseconds)
    {
    FUNC_LOG;
    iSlideOutDuration = aTimeMiliseconds;
    if (iSlideOutDuration < 0)
        {
        iSlideOutDuration = 0;
        }
    }

// ---------------------------------------------------------------------------
//  The function returns slide out effect's duration.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::SlideOutDuration() const
    {
    FUNC_LOG;
    return iSlideOutDuration;
    }

// ---------------------------------------------------------------------------
//  The function sets direction of the slide out effect.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetSlideOutDirection(TFsSlideEffect aDirection)
    {
    FUNC_LOG;
    iSlideOutDirection = aDirection;
    }

// ---------------------------------------------------------------------------
//  The function returns direction of the slide out effect.
// ---------------------------------------------------------------------------
//
MFsTreeVisualizer::TFsSlideEffect CFsTreeVisualizerBase::SlideOutDirection() const
    {
    return iSlideOutDirection;
    }

// ---------------------------------------------------------------------------
// Returns information about looping type of the list.
// ---------------------------------------------------------------------------
//
TFsTreeListLoopingType CFsTreeVisualizerBase::LoopingType() const
    {
    FUNC_LOG;
    return iLooping;
    }

// ---------------------------------------------------------------------------
// Sets type of list's looping.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetLoopingType(
        const TFsTreeListLoopingType aLoopingType)
    {
    FUNC_LOG;
    iLooping = aLoopingType;
    }

// ---------------------------------------------------------------------------
// Sets item expansion delay.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetItemExpansionDelay(const TInt aDelay)
    {
    FUNC_LOG;
    iScrollSpeed = aDelay;
    if (iScrollSpeed < 0)
        {
        iScrollSpeed = 0;
        }

    iCurrentScrollSpeed = iScrollSpeed;
    iScrollAccelerationRate = 0.5;
    }

// ---------------------------------------------------------------------------
// Sets an item separator color.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetItemSeparatorColor(const TRgb& aColor)
    {
    FUNC_LOG;
    static_cast<CAlfBorderBrush*> (iBorderBrush)->SetColor(aColor);
    }

// ---------------------------------------------------------------------------
// Sets an item separator size.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetItemSeparatorSize(const TSize aSize)
    {
    FUNC_LOG;
    static_cast<CAlfBorderBrush*> (iBorderBrush)->SetThickness(
            TAlfTimedPoint(aSize.iWidth, aSize.iHeight));
    }

// ---------------------------------------------------------------------------
// Sets an item separator size.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetItemSeparatorSize(
        const TAlfTimedPoint& aThickness)
    {
    FUNC_LOG;
    static_cast<CAlfBorderBrush*> (iBorderBrush)->SetThickness(aThickness);
    }

// ---------------------------------------------------------------------------
// The functions sets fade-in's effect duration.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetFadeInEffectTime(TInt aFadeTime)
    {
    FUNC_LOG;
    if (aFadeTime < 0)
        {
        iFadeInEffectTime = 0;
        }
    else
        {
        iFadeInEffectTime = aFadeTime;
        }
    }

// ---------------------------------------------------------------------------
// The functions returns fade-in's effect duration.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::FadeInEffectTime()
    {
    FUNC_LOG;
    return iFadeInEffectTime;
    }

// ---------------------------------------------------------------------------
// The functions sets fade-out's effect duration.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetFadeOutEffectTime(TInt aFadeTime)
    {
    FUNC_LOG;
    if (aFadeTime < 0)
        {
        iFadeOutEffectTime = 0;
        }
    else
        {
        iFadeOutEffectTime = aFadeTime;
        }
    }

// ---------------------------------------------------------------------------
// The functions returns fade-out's effect duration.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::FadeOutEffectTime()
    {
    FUNC_LOG;
    return iFadeOutEffectTime;
    }

// ---------------------------------------------------------------------------
// Function sets the time of list scroll per item and the scroll
// acceleration rate.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetScrollTime(TInt aScrollTime,
        TReal aScrollAcceleration)
    {
    FUNC_LOG;
    iScrollSpeed = aScrollTime;
    if (iScrollSpeed < 0)
        {
        iScrollSpeed = 0;
        }

    iCurrentScrollSpeed = iScrollSpeed;
    iScrollAccelerationRate = aScrollAcceleration;
    if (iScrollAccelerationRate < 0.0)
        {
        iScrollAccelerationRate = 0.0;
        }
    else if (iScrollAccelerationRate > 1.0)
        {
        iScrollAccelerationRate = 1.0;
        }
    }

// ---------------------------------------------------------------------------
// Gets the item expansion delay.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::ItemExpansionDelay() const
    {
    FUNC_LOG;
    return iScrollSpeed;
    }

// ---------------------------------------------------------------------------
// Function gets the values of scroll speed and its acceleration rate.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::GetScrollTime(TInt& aScrollTime,
        TReal& aScrollAcceleration)
    {
    FUNC_LOG;
    aScrollTime = iScrollSpeed;
    aScrollAcceleration = iScrollAccelerationRate;
    }

// ---------------------------------------------------------------------------
// Expand the node.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ExpandNodeL(const TFsTreeItemId aNodeId)
    {
    FUNC_LOG;
    MFsTreeNodeVisualizer* nodeVisualizer =
            iTreeData->NodeVisualizer(aNodeId);
    TRect rc;
    TInt index = iWorld.GetItemRect(aNodeId, rc);
    if (!nodeVisualizer->IsExpanded() && index != KErrNotFound )
        {
        nodeVisualizer->SetExpanded(ETrue, &iTreeData->ItemData(aNodeId));

        TFsTreeIterator treeIter = iTreeData->Iterator(aNodeId, aNodeId,
                KFsTreeIteratorSkipHiddenFlag);
        if (treeIter.HasNext())
            {
            TPoint position;
            if (iFocusedItem != KFsTreeNoneID)
                {
                position = iViewPort.ItemRect(iFocusedItem).iTl;
                }
            iWorld.BeginUpdate();
            TFsTreeItemId itemId;
            TSize size;
            do
                {
                itemId = treeIter.Next();
                ++index;
                MFsTreeItemVisualizer* itemviz =
                        iTreeData->ItemVisualizer(itemId);
                ApplyListSpecificValuesToItem(itemviz);
                size = itemviz->Size();
                iWorld.InsertL(itemId, size, index);
                } while (treeIter.HasNext());
            if (iFocusedItem != KFsTreeNoneID)
                {
                iViewPort.ItemToPositionL(iFocusedItem, position, EFalse,
                        TViewPort::EPositionAfterExpand);
                }
            iWorld.EndUpdateL();

            if ( iVisualizerObserver )
                {
                iVisualizerObserver->TreeVisualizerEventL(
                        MFsTreeVisualizerObserver::EFsTreeListItemExpanded,
                        aNodeId );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Collapse the node.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CollapseNodeL(const TFsTreeItemId aNodeId)
    {
    FUNC_LOG;
    MFsTreeNodeVisualizer* nodeVisualizer =
            iTreeData->NodeVisualizer(aNodeId);
    if (nodeVisualizer->IsExpanded())
        {
        TFsTreeIterator treeIter = iTreeData->Iterator(aNodeId, aNodeId,
                KFsTreeIteratorSkipHiddenFlag);
        nodeVisualizer->SetExpanded(EFalse, &iTreeData->ItemData(aNodeId));
        if (treeIter.HasNext())
            {
            iWorld.BeginUpdate();
            TFsTreeItemId itemId;
            TSize size;
            do
                {
                itemId = treeIter.Next();
                if (itemId != KFsTreeNoneID)
                    {
                    iWorld.RemoveL(itemId);
                    }
                } while (treeIter.HasNext());
            iWorld.EndUpdateL();

            if ( iVisualizerObserver )
                {
                iVisualizerObserver->TreeVisualizerEventL(
                        MFsTreeVisualizerObserver::EFsTreeListItemCollapsed,
                        aNodeId );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Expand all nodes in the tree.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ExpandAllL()
    {
    FUNC_LOG;
    iWorld.BeginUpdate();
    TPoint position;

    if( iFocusedItem != KFsTreeNoneID )
        {
        position = iViewPort.ItemRect(iFocusedItem).iTl;
        }
    iWorld.RemoveAllL();
    TFsTreeIterator treeIter = iTreeData->Iterator(
        KFsTreeRootID, KFsTreeRootID );
    TFsTreeItemId itemId = KFsTreeNoneID;
    TSize size;
    while( treeIter.HasNext() )
        {
        itemId = treeIter.Next();
        if( itemId != KFsTreeNoneID )
            {
            if( iTreeData->IsNode( itemId ) )
                {
                MFsTreeNodeVisualizer* nodeviz = iTreeData->NodeVisualizer(
                        itemId);
                nodeviz->SetExpanded( ETrue, &iTreeData->ItemData( itemId ) );
                size = nodeviz->Size();
                }
            else
                {
                MFsTreeItemVisualizer* itemviz =
                    iTreeData->ItemVisualizer( itemId );
                size = itemviz->Size();
                }
            iWorld.AppendL( itemId, size );
            }
        }

    if( iFocusedItem != KFsTreeNoneID )
        {
        iViewPort.ItemToPositionL(iFocusedItem, position, EFalse,
                TViewPort::EPositionAfterExpand );
        }
    iWorld.EndUpdateL();

    if ( iVisualizerObserver )
        {
        iVisualizerObserver->TreeVisualizerEventL(
                MFsTreeVisualizerObserver::EFsTreeListExpandedAll );
        }
    }

// ---------------------------------------------------------------------------
// Collapse all nodes in the tree.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CollapseAllL()
    {
    FUNC_LOG;
    iWorld.BeginUpdate();
    TPoint position;

    if( iFocusedItem != KFsTreeNoneID )
        {
        position = iViewPort.ItemRect( iFocusedItem ).iTl;
        }
    iWorld.RemoveAllL();
    if( iTreeData->Count() > 0 )
        {
        const TUint childcount = iTreeData->CountChildren( KFsTreeRootID );
        for( TUint i( 0 ); i < childcount; ++i )
            {
            TFsTreeItemId itemId = iTreeData->Child( KFsTreeRootID, i );
            TSize itemSize;
            if( iTreeData->IsNode( itemId ) )
                {
                MFsTreeNodeVisualizer* nodeVis =
                    iTreeData->NodeVisualizer( itemId );
                nodeVis->SetExpanded( EFalse, &iTreeData->ItemData( itemId ) );
                itemSize = nodeVis->Size();
                }
            else
                {
                itemSize = iTreeData->ItemVisualizer( itemId )->Size();
                }
            iWorld.AppendL(itemId, itemSize);
            }
        }

    if( iFocusedItem != KFsTreeNoneID )
        {
        // If item, which was selected, wasn't node, then make its parent node
        // focused, if it doesn't have parent node or its parent is not a node
        // then leave focus as it was.
        if( !iTreeData->IsNode( iFocusedItem ) )
            {
            TFsTreeItemId parentId = iTreeData->Parent( iFocusedItem );
            if( iTreeData->IsNode( parentId ) )
                {
                SetFocusedItemL( parentId );
                position = iViewPort.ItemRect( iFocusedItem ).iTl;
                }
            }
        iViewPort.ItemToPositionL( iFocusedItem, position, EFalse );
        }

    iWorld.EndUpdateL();

    if ( iVisualizerObserver )
        {
        iVisualizerObserver->TreeVisualizerEventL(
                MFsTreeVisualizerObserver::EFsTreeListCollapsedAll );
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::AllNodesCollapsed() const
    {
    FUNC_LOG;
    TFsTreeIterator treeIter = iTreeData->Iterator(KFsTreeRootID,
            KFsTreeRootID, KFsTreeIteratorSkipCollapsedFlag
                    | KFsTreeIteratorSkipHiddenFlag);
    TFsTreeItemId itemId = KFsTreeNoneID;
    while (treeIter.HasNext())
        {
        itemId = treeIter.Next();
        if (!iTreeData->IsNode(itemId))
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::AllNodesExpanded() const
    {
    FUNC_LOG;
    return iWorld.ItemCount() == iTreeData->Count();
    }

// ---------------------------------------------------------------------------
// Update visual of the item given by id.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdateItemL(const TFsTreeItemId aItemId)
    {
    FUNC_LOG;
    TInt i = iVisibleItems.Find(aItemId);
    if (i != KErrNotFound)
        {
        MFsTreeItemVisualizer* vis = iTreeData->ItemVisualizer(aItemId);
        if (vis)
            {
            vis->UpdateL(iTreeData->ItemData(aItemId),
                    IsItemFocused(aItemId) && IsFocusShown(),
                    iTreeData->Level(aItemId),
                    iMarkIcon, iMenuIcon, 0);
            }
        }
    }

// ---------------------------------------------------------------------------
//  The functions sets wether all item in the list should be always in
//  extended state or in normal state.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetItemsAlwaysExtendedL(TBool aAlwaysExtended)
    {
    FUNC_LOG;
    // Do not change this to: IsItemsAlwaysExtended() != aAlwaysExtended, because
    // it will not work. TBool is defined as TInt and thus this comparison
    // comes out as TInt != TInt, which always evaluates true in this case.
    if ( ( IsItemsAlwaysExtended() && !aAlwaysExtended ) || 
         ( !IsItemsAlwaysExtended() && aAlwaysExtended ) )
        {
        iFlags.Assign(EItemsAlwaysExtended, aAlwaysExtended);
        TFsTreeIterator treeIter(
                iTreeData->Iterator(KFsTreeRootID, KFsTreeRootID));
        while (treeIter.HasNext())
            {
            TFsTreeItemId itemId(treeIter.Next());
            if (itemId != KFsTreeNoneID && !iTreeData->IsNode(itemId))
                {
                MFsTreeItemVisualizer* itemviz(iTreeData->ItemVisualizer(itemId));
                ApplyListSpecificValuesToItem(itemviz);
                }
            }
            const TBool isUpdating(iWorld.IsUpdating());
        if (!isUpdating)
            {
            iWorld.BeginUpdate();
            }
        iWorld.RemoveAllL();
        treeIter = iTreeData->Iterator(KFsTreeRootID, KFsTreeRootID,
                KFsTreeIteratorSkipCollapsedFlag);
        while (treeIter.HasNext())
            {
            TFsTreeItemId itemId(treeIter.Next());
            if (itemId != KFsTreeNoneID)
                {
                MFsTreeItemVisualizer* itemviz(iTreeData->ItemVisualizer(itemId));
                iWorld.AppendL(itemId, itemviz->Size());
                }
            }
            // Below line commetned out. ViewPort is now not moved to the top of mail list
            //iViewPort.SetPositionL(TPoint(), EFalse);
        iViewPort.ClearCache();
        if (!isUpdating)
            {
            iWorld.EndUpdateL();
            }
        }
    }

// ---------------------------------------------------------------------------
//  The function returns if items are always in extended state or in normal.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsItemsAlwaysExtended()
    {
    FUNC_LOG;
    return iFlags.IsSet(EItemsAlwaysExtended);
    }

// ---------------------------------------------------------------------------
// Set the background texture, clears the background color.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::SetBackgroundTextureL(CAlfTexture& /*aBgTexture*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Sets the background color, clears the background texture.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::SetBackgroundColorL(TRgb aColor)
    {
    FUNC_LOG;
    if (iWatermarkLayout && iWatermarkLayout->Brushes())
        {
        TInt count = iWatermarkLayout->Brushes()->Count();
        if (count > 0)
            {//there is only background brush attached to this layout
            iWatermarkLayout->Brushes()->Remove(KZero);
            iWatermarkLayout->Brushes()->Reset();
            iWatermarkLayout->SetFlag(EAlfVisualChanged);
            }

    CAlfGradientBrush* backgroundBrush = CAlfGradientBrush::NewL(
            iOwnerControl->Env());
    backgroundBrush->SetLayer(EAlfBrushLayerBackground);
    backgroundBrush->SetColor(aColor);

    iWatermarkLayout->Brushes()->AppendL(backgroundBrush, EAlfHasOwnership);
        }
    }

// ---------------------------------------------------------------------------
// Sets a given brush as a list background.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::SetBackgroundBrushL(CAlfBrush* aBrush)
    {
    FUNC_LOG;

    if (iWatermarkLayout && iWatermarkLayout->Brushes())
        {
        TInt count = iWatermarkLayout->Brushes()->Count();
        if (count > 0)
            {//there is only background brush attached to this layout
            iWatermarkLayout->Brushes()->Remove(KZero);
            iWatermarkLayout->Brushes()->Reset();
            iWatermarkLayout->SetFlag(EAlfVisualChanged);
            }
        }

    aBrush->SetLayer(EAlfBrushLayerBackground);
    iWatermarkLayout->Brushes()->AppendL(aBrush, EAlfHasOwnership);
    }

// ---------------------------------------------------------------------------
// The function clears list's background.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::ClearBackground()
    {
    FUNC_LOG;
    if (iWatermarkLayout && iWatermarkLayout->Brushes())
        {
        TInt count = iWatermarkLayout->Brushes()->Count();
        if (count > 0)
            {//there is only background brush attached to this layout
            iWatermarkLayout->Brushes()->Remove(KZero);
            iWatermarkLayout->Brushes()->Reset();
            iWatermarkLayout->SetFlag(EAlfVisualChanged);
            }
        }
    }

// ---------------------------------------------------------------------------
// Sets watermark position.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::SetWatermarkPos(const TPoint& aPosition)
    {
    FUNC_LOG;
    if (iWatermark)
        {
        TAlfTimedPoint position;
        position.SetTarget(TAlfRealPoint(aPosition));
        iWatermark->SetPos(position);
        }
    }

// ---------------------------------------------------------------------------
// Sets watermark's size.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::SetWatermarkSize(const TSize& aSize)
    {
    FUNC_LOG;
    if (iWatermark)
        {
        TAlfTimedPoint size;
        size.SetTarget(TAlfRealPoint(aSize.AsPoint()));
        iWatermark->SetSize(size);
        }
    }

// ---------------------------------------------------------------------------
// Sets watermark opacity.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::SetWatermarkOpacity(const float aOpacity)
    {
    FUNC_LOG;
    if (iWatermark)
        {
        TAlfTimedValue opacity;
        opacity.SetTarget(aOpacity, 0);
        iWatermark->SetOpacity(opacity);
        }
    }

// ---------------------------------------------------------------------------
// Sets watermark texture.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::SetWatermarkL(CAlfTexture* aTexture)
    {
    FUNC_LOG;
    if (aTexture)
        {
        if (!iWatermark)
            {
            iWatermark
                    = CFsWatermark::NewL(*iOwnerControl, *iWatermarkLayout);
            }
        // <cmail>
        iWatermark->SetWatermarkTextureL(*aTexture);
        // </cmail>
        }
    else
        {
        delete iWatermark;
        iWatermark = NULL;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::TreeEventL(
    const TFsTreeEvent aEvent,
    const MFsTreeObserver::TFsTreeEventParams& aParams)
    {
    FUNC_LOG;
    switch (aEvent)
        {
        case EFsTreeItemAdded:
        case EFsTreeNodeAdded:
            {
            MFsTreeItemVisualizer* itemVisualizer(iTreeData->ItemVisualizer(
                    aParams.iItemId));
            const TBool isHidden(itemVisualizer->IsHidden());
            if (!isHidden)
                {
                if (!iListLayout->Count())
                    {
                    // Hide empty list marker
                    iRootVisualizer->Hide();
                    iComponentLayout->UpdateChildrenLayout();
                    iListLayout->UpdateChildrenLayout();
                    }
                if (aParams.iTargetNodeId != KFsTreeRootID)
                    {
                    MFsTreeNodeVisualizer* nodeVisualizer =
                            iTreeData->NodeVisualizer(aParams.iTargetNodeId);
                    if (nodeVisualizer->IsExpanded())
                        {
                        // look for parent's index
                        TInt indexAdd = 0;
                        while (iWorld.Item(indexAdd) != aParams.iTargetNodeId)
                            {
                            indexAdd++;
                            }

                        // look for how many children parent already has in
                        // the tree model
                        TInt childCount( 0 );
                        if ( aParams.iIndex >= 0 )
                            {
                            childCount = aParams.iIndex;
                            }
                        else
                            {
                            // -1 is for excluding the item that is now being
                            // inserted to visualiser
                            // (it has already been inserted to the tree model)
                            childCount = iTreeData->CountChildrenRecursively(
                                aParams.iTargetNodeId ) - 1;
                            }

                        InsertItemL( aParams.iItemId, indexAdd + 1 + childCount );
                        }
                    }
                else if (iWorld.ItemCount() > 0)
                    {
                    if (aParams.iIndex >= 0)
                        {
                        TInt indexAdd = 0;
                        TInt nodeCount = 0;
                        while (indexAdd < iWorld.ItemCount() - 1 && nodeCount
                                != aParams.iIndex)
                            {
                            indexAdd++;
                            if (iTreeData->Parent(iWorld.Item(indexAdd))
                                    == KFsTreeRootID)
                                {
                                nodeCount++;
                                }
                            }
                        InsertItemL(aParams.iItemId, indexAdd);
                        }
                    else
                        {
                        InsertItemL(aParams.iItemId, KErrNotFound);
                        }
                    }
                else
                    {
                    InsertItemL(aParams.iItemId, aParams.iIndex);
                    }
                }
            }
            break;
        case EFsTreeItemRemoved:
        case EFsTreeNodeRemoved:
            {
            RemoveItemL(aParams.iItemId);
            }
            break;
        case EFsTreeRemovedAll:
            {
            if ( iPhysics )
                {
                iPhysics->StopPhysics();
                }
            if (!iWorld.IsUpdating())
                {
                iRootVisualizer->ShowL(*iRootLayout);
                iRootVisualizer->UpdateL(*iRootData, EFalse, 0, iMarkIcon,
                        iMenuIcon);
                }
            iVisibleItems.Reset();
            iWorld.RemoveAllL();
            iFocusedItem = KFsTreeNoneID;
            }
            break;
        case EFsTreeItemVisualizerChanged:
            //the selector visual will be destroyed only when changing visualizer
            if (aParams.iItemId == iFocusedItem)
                {
                iSelectorVisual = NULL;
                }
        case EFsTreeItemDataChanged:
            {
            const TFsTreeItemId itemId(aParams.iItemId);
            MFsTreeItemVisualizer* visualizer = iTreeData->ItemVisualizer(
                    itemId);
            if (visualizer)
                {
                visualizer->UpdateL(iTreeData->ItemData(itemId),
                        IsItemFocused(itemId), iTreeData->Level(itemId),
                        iMarkIcon, iMenuIcon, 0);
                }
            }
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// The function implements tree observer's reaction to removing a list item
// from the model. If the item was visible then it's removed from the visible
// area. If the focused item was removed then the focus is moved to the next
// item in the tree hierarchy (if the next item is not present, then the focus
// is moved to the previous one).
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::RemoveItemL(TFsTreeItemId aItemId)
    {
    FUNC_LOG;
    const TInt removedindex(iWorld.IndexOfItem(aItemId));
    iWorld.RemoveL(aItemId);
    if (iFocusedItem == aItemId)
        {
        if (iWorld.ItemCount() > removedindex)
            {
            SetFocusedItemL(iWorld.Item(removedindex), EFalse);
            }
        else if (iWorld.ItemCount() > 0)
            {
            SetFocusedItemL(iWorld.Item(removedindex - 1), EFalse);
            }
        else
            {
            iFocusedItem = KFsTreeNoneID;
            }
        }
    }

// ---------------------------------------------------------------------------
//  The function checks whether the focused item is not outside the visible
//  area and if needed scrolls the list so that selected item is fully visible.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::MakeFocusedItemFullyVisible()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::RefreshListViewL()
    {
    FUNC_LOG;
    if (iTreeData->Count() == 0)
        {
        iRootVisualizer->ShowL(*iRootLayout);
        iRootVisualizer->UpdateL(*iRootData, EFalse, 0, iMarkIcon, iMenuIcon);
        }
    else
        {
        }
    }

// ---------------------------------------------------------------------------
// Hides item.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::HideItemL(const TFsTreeItemId /*aItemId*/)

    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Unhides item.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UnhideItemL(const TFsTreeItemId /*aItemId*/)
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetPadding(const TAlfBoxMetric& aPadding)
    {
    FUNC_LOG;

    iWatermarkLayout->SetPadding(aPadding);

    }

// ---------------------------------------------------------------------------
// List pane's padding taken from Layout Manager or switched off.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::EnableListPanePadding(TBool aEnable)
    {
    FUNC_LOG;
    if (aEnable)
        {
        TRect listPane;
        CFsLayoutManager::LayoutMetricsRect(TRect(
                iComponentLayout->Size().Target().AsSize()),
                CFsLayoutManager::EFsLmMainSpFsListPane, listPane);

        if (CFsLayoutManager::IsMirrored())
            {
            iListLayout->SetPadding(TPoint(
                    iComponentLayout->Size().Target().AsSize().iWidth
                            - listPane.iBr.iX, 0));
            iListItemBackgroundLayout->SetPadding(TPoint(
                    iComponentLayout->Size().Target().AsSize().iWidth
                            - listPane.iBr.iX, 0));
            }
        else
            {
            TInt xPadd;
            xPadd = listPane.iTl.iX;
            iListLayout->SetPadding(TPoint(xPadd, 0));
            iListItemBackgroundLayout->SetPadding(TPoint(xPadd, 0));
            }
        }
    else
        {
        //no padding
        TInt padd(0);
        iListLayout->SetPadding(TPoint(padd, padd));
        iListItemBackgroundLayout->SetPadding(TPoint(padd, padd));
        }
    }

// ---------------------------------------------------------------------------
//  Sets the type of text marquee.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetTextMarqueeType(
        const TFsTextMarqueeType aMarqueeType)
    {
    FUNC_LOG;
    iMarqueeType = aMarqueeType;
    }

// ---------------------------------------------------------------------------
//  Gets the type of text marquee.
// ---------------------------------------------------------------------------
//
TFsTextMarqueeType CFsTreeVisualizerBase::TextMarqueeType() const
    {
    FUNC_LOG;
    return iMarqueeType;
    }

// ---------------------------------------------------------------------------
//  Sets the speed of marquee.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetTextMarqueeSpeed(const TInt aPixelsPerSec)
    {
    FUNC_LOG;
    iMarqueeSpeed = aPixelsPerSec;
    }

// ---------------------------------------------------------------------------
//  Gets the speed of marquee.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::TextMarqueeSpeed() const
    {
    FUNC_LOG;
    return iMarqueeSpeed;
    }

// ---------------------------------------------------------------------------
//  Sets delay for text marquee start.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetTextMarqueeStartDelay(const TInt aStartDelay)
    {
    FUNC_LOG;
    iMarqueStartDelay = aStartDelay;
    }

// ---------------------------------------------------------------------------
//  Gets delay for text marquee.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::TextMarqueeStartDelay() const
    {
    FUNC_LOG;
    return iMarqueStartDelay;
    }

// ---------------------------------------------------------------------------
//  Sets a delay for each cycle start.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetTextMarqueeCycleStartDelay(
        const TInt aCycleStartDelay)
    {
    FUNC_LOG;
    iMarqueCycleStartDelay = aCycleStartDelay;
    }

// ---------------------------------------------------------------------------
//  Returns a delay for each cycle start.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::TextMarqueeCycleStartDelay() const
    {
    FUNC_LOG;
    return iMarqueCycleStartDelay;
    }

// ---------------------------------------------------------------------------
//  Sets number of marquee cycles.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetTextMarqueeRepetitions(
        const TInt aMarqueeRepetitions)
    {
    FUNC_LOG;
    iMarqueeRepetitions = aMarqueeRepetitions;
    }

// ---------------------------------------------------------------------------
//  Gets number of marquee cycles.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::TextMarqueeRepetitions() const
    {
    FUNC_LOG;
    return iMarqueeRepetitions;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::GetItemDisplayRectTarget(
        const TFsTreeItemId aItemId, TAlfRealRect& aRect)
    {
    FUNC_LOG;
    TInt retVal(KErrNotFound);
    MFsTreeItemVisualizer* itemVis(NULL);
    CAlfLayout* lay(NULL);

    aRect.iTl = TAlfRealPoint(0.0, 0.0);
    aRect.iBr = TAlfRealPoint(0.0, 0.0);

    retVal = iVisibleItems.Find(aItemId);
    if (retVal != KErrNotFound)
        {//item visible
        itemVis = iTreeData->ItemVisualizer(aItemId);
        if (itemVis)
            {
            lay = &itemVis->Layout();
            if (lay)
                {
                aRect = lay->DisplayRectTarget();
                retVal = KErrNone;
                }
            else
                {
                retVal = KErrNotFound;
                }
            }
        else
            {
            retVal = KErrNotFound;
            }
        }
    else
        {
        retVal = KErrNotFound;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
//  Internall to TreeList. Do not use directly.
//  Used to block update during addition of many items/nodes. Currently only
//  scrollbar is blocked from beign updated.Gets number of marquee cycles.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetAutoRefreshAtInsert(TBool aAllowRefresh)
    {
    FUNC_LOG;
    if (!aAllowRefresh && iFlags.IsSet(EAutoRefresh))
        {
        iWorld.BeginUpdate();
        }
    else if (aAllowRefresh && !iFlags.IsSet(EAutoRefresh))
        {
        TInt error( KErrNone );
        TRAP( error, iWorld.EndUpdateL() );
        ERROR_1( error, "iWorld.EndUpdateL failed with error: %d", error );
        }
    iFlags.Assign(EAutoRefresh, aAllowRefresh);
    }

// ---------------------------------------------------------------------------
//  Internall to TreeList. Do not use directly.
//  Used to block update during addition of many items/nodes. Currently only
//  scrollbar is blocked from beign updated.Gets number of marquee cycles.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsAutoRefreshAtInsert()
    {
    FUNC_LOG;
    return iFlags.IsSet(EAutoRefresh);
    }

// ---------------------------------------------------------------------------
//  CFsTreeVisualizerBase::SetDirectTouchMode
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetDirectTouchMode(const TBool aDirectTouchMode)
    {
    FUNC_LOG;
    iFlags.Assign(EDirectTouchMode, aDirectTouchMode);
    }

// ---------------------------------------------------------------------------
//  CFsTreeVisualizerBase::SetExpandCollapseAllOnLongTap
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetExpandCollapseAllOnLongTap( TBool aExpandCollapse )
    {
    FUNC_LOG;
    iFlags.Assign( EExpandCollapseOnLongTap, aExpandCollapse );
    }

// ---------------------------------------------------------------------------
//  CFsTreeVisualizerBase::IsExpandCollapseAllOnLongTap
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsExpandCollapseAllOnLongTap()
    {
    FUNC_LOG;
    return iFlags.IsSet( EExpandCollapseOnLongTap );
    }

// ---------------------------------------------------------------------------
// From MFsFadeEffectObserver
// Function which will receive notifications about fade effect state changes.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::FadeEffectEvent(
        MFsFadeEffectObserver::TFadeEffectState aState)
    {
    FUNC_LOG;
    switch (aState)
        {
        case MFsFadeEffectObserver::EFadeInFinished:
            {
            if (iVisualizationState == EFsTreeFadingInSlidingIn)
                {//slide in effect still on
                iVisualizationState = EFsTreeSlidingIn;
                }
            else if (iVisualizationState == EFsTreeFadingIn)
                {
                iVisualizationState = EFsTreeVisible;
                UpdateScrollBar();
                }
            break;
            }
        case MFsFadeEffectObserver::EFadeOutFinished:
            {
            if (iVisualizationState == EFsTreeFadingOutSlidingOut)
                {//slide out effect still on
                iVisualizationState = EFsTreeSlidingOut;
                }
            else if (iVisualizationState == EFsTreeFadingOut)
                {
                iScrollBar->Show( EFalse );
                iVisualizationState = EFsTreeHidden;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if (iVisualizationState == EFsTreeVisible && iVisualizerObserver)
        {
            // <cmail> Touch
            TRAP_IGNORE(iVisualizerObserver->TreeVisualizerEventL(
                            MFsTreeVisualizerObserver::EFsTreeListVisualizerShown,
                            KFsTreeNoneID ));
        // </cmail>
        }

    if (iVisualizationState == EFsTreeHidden && iVisualizerObserver)
        {
            // <cmail> Touch
            TRAP_IGNORE(iVisualizerObserver->TreeVisualizerEventL(
                            MFsTreeVisualizerObserver::EFsTreeListVisualizerHidden,
                            KFsTreeNoneID ));
        // </cmail>
        }
    }

// ---------------------------------------------------------------------------
// From MFsSlideEffectObserver
// Function which will receive notifications about slide effect state changes.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SlideEffectEvent(
        MFsSlideEffectObserver::TSlideEffectState aState)
    {
    FUNC_LOG;
    switch (aState)
        {
        case MFsSlideEffectObserver::ESlideInFinished:
            {
            if (iVisualizationState == EFsTreeFadingInSlidingIn)
                {//fade in effect still on
                iVisualizationState = EFsTreeFadingIn;
                }
            else if (iVisualizationState == EFsTreeSlidingIn)
                {
                iVisualizationState = EFsTreeVisible;
                UpdateScrollBar();
                }
            break;
            }
        case MFsSlideEffectObserver::ESlideOutFinished:
            {
            if (iVisualizationState == EFsTreeFadingOutSlidingOut)
                {//fade out effect still on
                iVisualizationState = EFsTreeFadingOut;
                }
            else if (iVisualizationState == EFsTreeSlidingOut)
                {
                iScrollBar->Show( EFalse );
                iVisualizationState = EFsTreeHidden;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if (iVisualizationState == EFsTreeVisible && iVisualizerObserver)
        {
            // <cmail> Touch
            TRAP_IGNORE(iVisualizerObserver->TreeVisualizerEventL(
                            MFsTreeVisualizerObserver::EFsTreeListVisualizerShown,
                            KFsTreeNoneID ));

        }

    if (iVisualizationState == EFsTreeHidden && iVisualizerObserver)
        {
            TRAP_IGNORE(iVisualizerObserver->TreeVisualizerEventL(
                            MFsTreeVisualizerObserver::EFsTreeListVisualizerHidden,
                            KFsTreeNoneID ));
        // </cmail>
        }
    }

// ---------------------------------------------------------------------------
// From MAlfActionObserver
// Called by the server when an action command is executed.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::HandleActionL(
        const TAlfActionCommand& aActionCommand)
    {
    if (KAknsMessageSkinChange == aActionCommand.Id())
        {
        iScrollBar->NotifyThemeChanged();
        }
    }

// ---------------------------------------------------------------------------
// Hides visible list items. Clears the visible items table.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ClearVisibleItemsListL(TBool /*aRemoveAll*/,
        TBool /*aScrollList*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Applies focus brush to the selected item.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ChangeFocusL(const TFsTreeItemId /*aPrevious*/)
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CFsTreeVisualizerBase::AdjustVisibleItemsL
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::AdjustVisibleItemsL()
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CFsTreeVisualizerBase::InsertItemL
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::InsertItemL(TFsTreeItemId aItemId,
        const TInt aSlot)
    {
    MFsTreeItemVisualizer* visualizer = iTreeData->ItemVisualizer(aItemId);
    ApplyListSpecificValuesToItem(visualizer);
    //TUint32 oldflags = visualizer->Flags();
    //visualizer->SetFlags(oldflags | KFsTreeListItemManagedLayout);
    const TSize size(visualizer->Size());
    //visualizer->SetFlags(oldflags);
    if (aSlot != KErrNotFound)
        {
        iWorld.InsertL(aItemId, size, aSlot);
        }
    else
        {
        iWorld.AppendL(aItemId, size);
        }
    }

// ---------------------------------------------------------------------------
// Move selection in the list.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::MoveSelectionL(
        const TFsTreeVisualizerMove aMoveType)
    {
    FUNC_LOG;
    TInt focusedIndex = iWorld.IndexOfItem(iFocusedItem);
    switch (aMoveType)
        {
        case EFsTreeVisualizerMoveLineUp:
            {
            while (focusedIndex > 0)
                {
                focusedIndex -= 1;
                if (iTreeData->ItemData(iWorld.Item(focusedIndex)).Type()
                        != KFsSeparatorDataType)
                    {
                    SetFocusedItemL(iWorld.Item(focusedIndex));
                    break;
                    }
                }
            break;
            }
        case EFsTreeVisualizerMoveLineDown:
            {
            while (focusedIndex < iWorld.ItemCount() - 1)
                {
                focusedIndex += 1;
                if (iTreeData->ItemData(iWorld.Item(focusedIndex)).Type()
                        != KFsSeparatorDataType)
                    {
                    SetFocusedItemL(iWorld.Item(focusedIndex));
                    break;
                    }
                }
            break;
            }
        case EFsTreeVisualizerMovePageUp:
            {
            focusedIndex -= iVisibleItems.Count() - 2;
            while (focusedIndex > 0)
                {
                if (iTreeData->ItemData(iWorld.Item(focusedIndex)).Type()
                        != KFsSeparatorDataType)
                    {
                    break;
                    }
                else
                    {
                    focusedIndex--;
                    }
                }
            if (focusedIndex < 0)
                {
                focusedIndex = 0;
                }
            if (iWorld.ItemCount() > 0)
                {
                SetFocusedItemL(iWorld.Item(focusedIndex));
                }
            break;
            }
        case EFsTreeVisualizerMovePageDown:
            {
            focusedIndex += iVisibleItems.Count() - 2;
            while (focusedIndex < iWorld.ItemCount() - 1)
                {
                if (iTreeData->ItemData(iWorld.Item(focusedIndex)).Type()
                        != KFsSeparatorDataType)
                    {
                    break;
                    }
                else
                    {
                    focusedIndex++;
                    }
                }
            if (focusedIndex > iWorld.ItemCount() - 1)
                {
                focusedIndex = iWorld.ItemCount() - 1;
                }
            if (iWorld.ItemCount() > 0)
                {
                SetFocusedItemL(iWorld.Item(focusedIndex));
                }
            break;
            }
        }
    iViewPort.ScrollItemToViewL(iFocusedItem, ETrue);
    }

// ---------------------------------------------------------------------------
// Moves selection in the list by one item upwards.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::MoveSelectionByLineUpL(const TBool /*aChangeFocus*/)
    {
    FUNC_LOG;
    return 0;
    }

// ---------------------------------------------------------------------------
// Moves selection in the list by one item downwards.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::MoveSelectionByLineDownL(const TBool /*aChangeFocus*/)
    {
    FUNC_LOG;
    return 0;
    }

// ---------------------------------------------------------------------------
// Scrolling
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::Scroll(const TFsTreeItemId aPrevious,
        TBool aAlignTop)
    {
    FUNC_LOG;
    TInt currScrollOffset = iListLayout->ScrollOffset().iY.Target();
    TInt listItemsHeight = 0;
    TInt visibleHeight = iComponentLayout->Size().iY.Target();
    TInt scrollOffset = 0;

    listItemsHeight = VisibleItemsHeight(aPrevious);

    if (aAlignTop)
        {
        scrollOffset = 0;
        }
    else
        {
        if (listItemsHeight > visibleHeight)
            {
            scrollOffset = listItemsHeight - visibleHeight;
            }
        else
            {
            scrollOffset = currScrollOffset;
            }
        }

    ScrollListLayouts(scrollOffset, iCurrentScrollSpeed);
    }

// ---------------------------------------------------------------------------
// Counts height items from iVisibleItems array which will
// be drawn on the screen
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::VisibleItemsHeight(const TFsTreeItemId aPrevious)
    {
    FUNC_LOG;
    TInt listItemsHeight = 0;
    MFsTreeItemVisualizer* visualizer = 0;
    TFsTreeItemId itemId;
    TInt itemCount = iVisibleItems.Count();

    for (TInt index = 0; index < itemCount; ++index)
        {
        itemId = iVisibleItems[index];
        visualizer = iTreeData->ItemVisualizer(itemId);


        if (visualizer)
            {
            if (itemId == iFocusedItem)
                {
                if (visualizer->IsExtendable())
                    {
                    listItemsHeight += visualizer->ExtendedSize().iHeight;
                    }
                else
                    {
                    listItemsHeight += visualizer->Size().iHeight;
                    }
                }
            else if (itemId == aPrevious && iFocusedItem != aPrevious)
                {
                listItemsHeight += visualizer->Size().iHeight;
                }
            else if (visualizer->IsExtended() && visualizer->IsExtendable())
                {
                listItemsHeight += visualizer->ExtendedSize().iHeight;
                }
            else
                {
                listItemsHeight += visualizer->Size().iHeight;
                }
            }
        }

    return listItemsHeight;
    }

// ---------------------------------------------------------------------------
// Check if item is in expanded node
// ---------------------------------------------------------------------------
TBool CFsTreeVisualizerBase::IsInExpanded(TFsTreeItemId aItemId)
    {
    FUNC_LOG;
    TBool result(ETrue);
    TFsTreeItemId parentId = iTreeData->Parent(aItemId);
    while (parentId != KFsTreeRootID && parentId != KFsTreeNoneID)
        {
        if (iTreeData->IsNode(parentId))
            {
            if (iTreeData->NodeVisualizer(parentId)->IsExpanded())
                {
                result = ETrue;
                parentId = iTreeData->Parent(parentId);
                }
            else
                {
                result = EFalse;
                break;
                }
            }
        else
            {
            parentId = iTreeData->Parent(parentId);
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
//  CFsTreeVisualizerBase::UpdateViewPortL
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdateViewPortL()
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    TRect listRect(iRootLayout->DisplayRectTarget());
    listRect.Move(mainPaneRect.iTl);
    TRect listPane;
    CFsLayoutManager::LayoutMetricsRect(listRect,
            CFsLayoutManager::EFsLmMainSpFsListPane, listPane);
    iWorld.BeginUpdate();
    TBool keepFocusedItemInView(EFalse);
    if (iFocusedItem != KFsTreeNoneID)
        {
        keepFocusedItemInView = iViewPort.ItemRect(iFocusedItem).Height() > 0;
        }
    iViewPort.SetSizeL(listPane.Size());
    iWorld.Recalculate(*iTreeData);
    if (keepFocusedItemInView)
        {
        iViewPort.ScrollItemToViewL(iFocusedItem, EFalse);
        }
    else
        {
        iViewPort.MakeViewFullyOccupiedL();
        }
    iViewPort.ClearCache();
    iWorld.EndUpdateL();
    }

// ---------------------------------------------------------------------------
//  Updates scrollbar, only if update is needed.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdateScrollBarIfNeededL()
    {
    FUNC_LOG;
    if (iScrollBar->ThumbPosition() != iViewPort.Position().iY)
        {
        SetScrollBarModelValues();
        iScrollBar->UpdateModelL(iScrollBarModel);
        iScrollBar->Redraw();
        }
    }

// ---------------------------------------------------------------------------
//  Sets scrollbar model values.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetScrollBarModelValues()
    {
    const TInt pageSize(iViewPort.Size().iHeight);
    iScrollBarModel.SetHeight(Max(iWorld.Height(), pageSize));
    iScrollBarModel.SetWindowHeight(pageSize);
    iScrollBarModel.SetWindowPosition(iViewPort.Position().iY);    
    }

// ---------------------------------------------------------------------------
//  Updates scrollbar.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdateScrollBarL(const TInt /*aTimeout*/)
    {
    FUNC_LOG;

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    TRect listRect(iRootLayout->DisplayRectTarget());
    if (!(iScrollBar->LayoutHints() & MUiCScrollBar::ELayoutRelativeToList))
        {
        listRect.Move(mainPaneRect.iTl);
        }
    else
        {
        const TPoint& tl( listRect.iTl );
        listRect.Move( -tl.iX, -tl.iY );
        }
    TRect scrollPane, listPane;
    CFsLayoutManager::LayoutMetricsRect(listRect,
            CFsLayoutManager::EFsLmSpFsScrollPane, scrollPane);
    CFsLayoutManager::LayoutMetricsRect(listRect,
            CFsLayoutManager::EFsLmMainSpFsListPane, listPane);
    SetScrollBarModelValues();
    iScrollBar->UpdateModelL(iScrollBarModel);
    iScrollBar->Redraw();

    RArray<TInt> columns;
    CleanupClosePushL(columns);
    TBool updateLayouts(EFalse);
    if (iViewPort.IsScrollBarNeeded() && iVisualizationState
            == EFsTreeVisible)
        {
        iScrollBar->SetRect(scrollPane);
        if (AknLayoutUtils::LayoutMirrored())
            {
            if (iScrollBar->LayoutHints() & MUiCScrollBar::ELayoutOnTopOfList)
                {
                columns.AppendL(0);
                columns.AppendL(listRect.Width());
                }
            else
                {
                columns.AppendL(scrollPane.Width());
                columns.AppendL(listPane.Width());
                }
            }
        else
            {
            if (iScrollBar->LayoutHints() & MUiCScrollBar::ELayoutOnTopOfList)
                {
                columns.AppendL(listRect.Width());
                columns.AppendL(0);
                }
            else
                {
                columns.AppendL(listPane.Width());
                columns.AppendL(scrollPane.Width());
                }
            }
        iComponentLayout->SetColumnsL(columns);
        iScrollBar->Show(ETrue);
        updateLayouts = ETrue;
        }
    else if (!iViewPort.IsScrollBarNeeded())
        {
        if (AknLayoutUtils::LayoutMirrored())
            {
            columns.AppendL(0);
            columns.AppendL(listRect.Width());
            }
        else
            {
            columns.AppendL(listRect.Width());
            columns.AppendL(0);
            }
        iComponentLayout->SetColumnsL(columns);
        iScrollBar->Show(EFalse);
        updateLayouts = ETrue;
        }
    CleanupStack::PopAndDestroy(); // columns.Close()
    if (updateLayouts)
        {
        iComponentLayout->UpdateChildrenLayout();

        for (TUint i = 0; i < iVisibleItems.Count(); ++i)
            {
            TFsTreeItemId itemId(iVisibleItems[i]);
            MFsTreeItemVisualizer* visualizer = iTreeData->ItemVisualizer(itemId);
            if (visualizer)
                {
                visualizer->UpdateL(iTreeData->ItemData(itemId),
					IsItemFocused(itemId) && IsFocusShown(),
                    iTreeData->Level(itemId), iMarkIcon, iMenuIcon, 0, EFalse);
                }
            }
        UpdateSelectorVisualL();
        }

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::UpdateScrollBar(const TInt aTimeout)
    {
    FUNC_LOG;
    TRAPD( error, UpdateScrollBarL( aTimeout ) );
    return error;
    }

// ---------------------------------------------------------------------------
//  Old API to be removed. Turns on or off vertical scrollbar.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetVScrollBarL(TBool /*aSetOn*/)
    {
    FUNC_LOG;
    UpdateScrollBarL(iCurrentScrollSpeed);
    }

// ---------------------------------------------------------------------------
//  Turns on or off vertical scrollbar.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetScrollBarVisibilityL(
        const TFsScrollbarVisibility aVisibility)
    {
    iScrollbarVisibility = aVisibility;
    UpdateScrollBarL(iCurrentScrollSpeed);
    }

// ---------------------------------------------------------------------------
//  Gets scrollbar visibility.
// ---------------------------------------------------------------------------
//
TFsScrollbarVisibility CFsTreeVisualizerBase::ScrollbarVisibility() const
    {
    FUNC_LOG;
    return iScrollbarVisibility;
    }

// ---------------------------------------------------------------------------
// Sets border status
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetBorderL(const TBool aVisible,
        CAlfTexture* aBorderTexture)
    {
    FUNC_LOG;
    if (!aBorderTexture)
        {
        FsGenericPanic(EFsListPanicBorderNotSet);
        }

    if (aVisible)
        {
        CAlfImageBrush* border = CAlfImageBrush::NewL(iOwnerControl->Env(),
                TAlfImage(*aBorderTexture));
        border->SetLayer(EAlfBrushLayerBackground);

        border->SetBorders(-8, -8, -8, -8);
        //<cmail>
        // layout values needed here (not: only used from FS action menu)
        //</cmail>
        iBorderLayout->SetPadding(TPoint(5, 5));

        if (iBorderLayout->Brushes()->Count() > 0)
            {
            iBorderLayout->Brushes()->Remove(0);
            iBorderLayout->Brushes()->Reset();
            }

        iBorderLayout->Brushes()->AppendL(border, EAlfHasOwnership);
        }
    else
        {
        if (iBorderLayout->Brushes()->Count() > 0)
            {
            iBorderLayout->Brushes()->Remove(0);
            iBorderLayout->Brushes()->Reset();
            iBorderLayout->SetPadding(0);
            }
        }

    iBorderLayout->SetFlag(EAlfVisualChanged);
    iBorderLayout->UpdateChildrenLayout();
    RefreshListViewL();
    }

// ---------------------------------------------------------------------------
// Sets shadow status
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetShadowL(const TBool aVisible)
    {
    FUNC_LOG;
    if (aVisible)
        {
        //remove shadow if it's already set
        if (iShadowLayout && iShadowLayout->Brushes())
            {
            TInt count = iShadowLayout->Brushes()->Count();
            if (count > 0)
                {//there is only a shadow brush attached to this layout
                iShadowLayout->Brushes()->Remove(KZero);
                iShadowLayout->Brushes()->Reset();
                iShadowLayout->SetFlag(EAlfVisualChanged);
                }
            }
        //attach shadow brush to layout
        CAlfShadowBorderBrush* shadow = CAlfShadowBorderBrush::NewLC(
                iOwnerControl->Env(), KDefaultShadowWidth);
        shadow->SetOpacity(.25f);

        //set paddind to root layout so the shadow can be visible
        TAlfBoxMetric m;
        m.iBottom.iMagnitude = KDefaultShadowWidth;
        m.iLeft.iMagnitude = KZero;
        m.iRight.iMagnitude = KDefaultShadowWidth;
        m.iTop.iMagnitude = KZero;
        iRootLayout->SetPadding(m);

        iShadowLayout->EnableBrushesL(ETrue);
        iShadowLayout->Brushes()->AppendL(shadow, EAlfHasOwnership);
        CleanupStack::Pop(shadow);

        iShadowLayout->SetFlag(EAlfVisualChanged);
        iRootLayout->UpdateChildrenLayout(KZero);
        }
    else
        {
        //remove shadow if it's already set
        if (iShadowLayout && iShadowLayout->Brushes())
            {
            TInt count = iShadowLayout->Brushes()->Count();
            if (count > 0)
                {//there is only a shadow brush attached to this layout
                iShadowLayout->Brushes()->Remove(KZero);
                iShadowLayout->Brushes()->Reset();
                iShadowLayout->SetFlag(EAlfVisualChanged);

                //remove paddind from root layout
                TAlfBoxMetric m;
                m.iBottom.iMagnitude = KZero;
                m.iLeft.iMagnitude = KZero;
                m.iRight.iMagnitude = KZero;
                m.iTop.iMagnitude = KZero;
                iRootLayout->SetPadding(m);
                iRootLayout->UpdateChildrenLayout(KZero);
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//  Sets visualizer observer
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetVisualizerObserver(
        MFsTreeVisualizerObserver* aObserver)
    {
    FUNC_LOG;
    iVisualizerObserver = aObserver;
    }

// ---------------------------------------------------------------------------
//  Returns parent layout.
// ---------------------------------------------------------------------------
//
CAlfLayout& CFsTreeVisualizerBase::ParentLayout() const
    {
    FUNC_LOG;
    return iParentLayout;
    }

// ---------------------------------------------------------------------------
//  Function shows selector or hides it.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreeVisualizerBase::MakeSelectorVisibleL(TBool aVisible)
    {
    MakeSelectorVisibleL( aVisible, 0 );
    }

// ---------------------------------------------------------------------------
//  Function shows selector or hides it.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::MakeSelectorVisibleL(TBool aVisible, TInt aDelay)
    {
    FUNC_LOG;
    TAlfTimedValue opacity;

    if (aVisible)
        {
        if (!iSelectorVisual)
            {
            CreateSelectorVisualL();
            }

        TBool isFocused = IsItemFocused(iFocusedItem);

        if (iSelectorVisual)
            {
            if (isFocused && IsFocusShown())
                {
                if( aDelay )
                    opacity.SetTarget(iSelectorOpacity, aDelay);
                else
                    opacity.SetValueNow(iSelectorOpacity);
                }
            else
                {
                if( aDelay )
                    opacity.SetTarget(0.0, aDelay);
                else
                    opacity.SetValueNow(0.0);
                }

            iSelectorVisual->SetOpacity(opacity);
            }
        UpdateSelectorVisualL();
        }
    else
        {
        if (iSelectorVisual)
            {
            opacity.SetValueNow(0);
            iSelectorVisual->SetOpacity(opacity);
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdateSelectorVisualL(TInt /*aTime*/)
    {
    FUNC_LOG;

    if ( iSelectorVisual )
        {
        if ( iFocusedItem == KFsTreeNoneID ||
        	 !iFlags.IsSet( EListFocused ) || !IsFocusShown() )
            {
            TAlfTimedValue opacity;
            opacity.SetValueNow( 0.0f );
            iSelectorVisual->SetOpacity( opacity );
            }
        else
            {
            //check if item is visible
            MFsTreeItemVisualizer* vis( NULL );

            iListLayout->SetFlag( EAlfVisualFlagFreezeLayout );
            iListItemBackgroundLayout->SetFlag( EAlfVisualFlagFreezeLayout );

            TRect itemRect( iViewPort.ItemRect( iFocusedItem ) );

            if ( itemRect.Height() )
                {
                TSize currSize;
                currSize.iWidth = iListLayout->Size().iX.Target() - 2
                        * iListLayout->PaddingInPixels().iTl.iX;
                currSize.iHeight = itemRect.Height();
                iSelectorVisual->SetSize( currSize, 0 );
                TAlfRealPoint pNow;
                pNow.iY = itemRect.iTl.iY;
                pNow.iX = iListLayout->PaddingInPixels().iTl.iX;
                iSelectorVisual->SetPos( pNow, 0 );

                TAlfTimedValue opacity( iSelectorOpacity, 0 );
                iSelectorVisual->SetOpacity( opacity );
                iFocusVisible = ETrue;

                if ( iMarqueeType != EFsTextMarqueeNone )
                    {
                    vis = iTreeData->ItemVisualizer( iFocusedItem );
                    if (vis)
                        {
                        vis->MarqueeL( iMarqueeType, iMarqueeSpeed,
                    	        iMarqueStartDelay, iMarqueCycleStartDelay,
                                iMarqueeRepetitions );
                        }
                    }
                }
            else
                {
                TAlfTimedValue opacity;
                opacity.SetValueNow( 0.0f );
                iSelectorVisual->SetOpacity( opacity );
                }

            iListLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
            iListItemBackgroundLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
            }
        }
    else if ( iFocusedItem != KFsTreeNoneID && iFlags.IsSet( EListFocused ) )
        {
        MakeSelectorVisibleL( ETrue );
        }
    }

// ---------------------------------------------------------------------------
//  The function deletes selector visual.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::DestroySelectorVisualL()
    {
    FUNC_LOG;

    if (iSelectorVisual && iSelectorLayout)
        {
        iSelectorLayout->Remove(iSelectorVisual);
        //destroy
        iSelectorVisual->RemoveAndDestroyAllD();
        iSelectorVisual = NULL;
        }
    }

// ---------------------------------------------------------------------------
//  The function creates selector visual.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CreateSelectorVisualL()
    {
    FUNC_LOG;
    TAlfTimedValue opacity;

    if (!iSelectorVisual)
        {
        if (iFocusedItem != KFsTreeNoneID)
            {
            MFsTreeItemVisualizer* vis =
                    iTreeData->ItemVisualizer( iFocusedItem );
            MFsTreeItemData* data = &iTreeData->ItemData( iFocusedItem );

            iListLayout->SetFlag( EAlfVisualFlagFreezeLayout );
            iListItemBackgroundLayout->SetFlag( EAlfVisualFlagFreezeLayout );

            iSelectorVisual = CAlfImageVisual::AddNewL(*iOwnerControl, iSelectorLayout);
            opacity.SetValueNow(0.0f);
            iSelectorVisual->SetOpacity(opacity);
            iSelectorVisual->SetScaleMode( CAlfImageVisual::EScaleFit);
            iSelectorVisual->EnableBrushesL();

            // Set the selector visual size and pos.
            iSelectorVisual->SetFlags(EAlfVisualFlagManualLayout |
                    EAlfVisualFlagIgnorePointer);

            TRect itemRect(iViewPort.ItemRect(iFocusedItem));
            itemRect.SetWidth(iListLayout->Size().iX.Target() - 2 * iListLayout->PaddingInPixels().iTl.iX);
            if ( itemRect.Height() )
                {
                iSelectorVisual->SetPos(itemRect.iTl);
                iSelectorVisual->SetSize(itemRect.Size());
                }

            //create a selector from skin or from a supplied bitmap
            if ( iSelectorTexture )
                {
                CAlfImageBrush* brush = CAlfImageBrush::NewLC(
                        iOwnerControl->Env(),TAlfImage(*iSelectorTexture));

                brush->SetBorders(iLeftSelectorBorderWidth,
                                  iRightSelectorBorderWidth,
                                  iTopSelectorBorderHeight,
                                  iBottomSelectorBorderHeight);

                iSelectorVisual->Brushes()->AppendL(brush, EAlfHasOwnership);
                CleanupStack::Pop(brush);
                }
            else if ( iSelectorBrush )
                {
                //append brush supplied by the user to the selector visual
                iSelectorVisual->Brushes()->AppendL(
                        iSelectorBrush, EAlfDoesNotHaveOwnership);

                if ( iSelectorOpacity <= 0.0 || iSelectorOpacity > 1.0 )
                        {
                        iSelectorOpacity = 1.0;
                        }

                opacity.SetValueNow(iSelectorOpacity);
                iSelectorVisual->SetOpacity(opacity);
                }
            else //no bitmap/brush given - create selector from skin
                {
                // apply a image brush to the visual
                CAlfFrameBrush* brush = CAlfFrameBrush::NewLC(iOwnerControl->Env(),
                                        KAknsIIDQsnFrList);

                iSelectorVisual->Brushes()->AppendL(brush, EAlfHasOwnership);
                CleanupStack::Pop(brush);

                CAlfBrush& skinbrush( iSelectorVisual->Brushes()->At(
                        iSelectorVisual->Brushes()->Count() - 1 ) );
                CAlfFrameBrush*
                    selectorBrush( static_cast<CAlfFrameBrush*>( &skinbrush ) );

                const TRect outerRect( itemRect.Size() ); //outer rect has the size of item
                TRect innerRect( outerRect );

                innerRect.Shrink( 5, 5 );
                if ( outerRect.Width() > 0 && outerRect.Height() > 0 &&
                        innerRect.Width() > 0 && innerRect.Height() > 0   )
                    {
                    // aInnerRect, const TRect& aOuterRect
                    selectorBrush->SetFrameRectsL( innerRect, outerRect );
                    }
                }

            iListLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
            iListItemBackgroundLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdateListItemSizes()
    {
    FUNC_LOG;
    TInt focused = iVisibleItems.Find(iFocusedItem);
    if (focused == KErrNotFound)
        {
        return;
        }
    TInt count = iVisibleItemsBackground.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (i == focused)
            {//focused item already updated
            continue;
            }
        else
            {
            UpdateListItemSize(i);
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdateListItemSize(TInt aIndex)
    {
    FUNC_LOG;
    TSize size;
    TReal32 diff(0);
    TReal32 speed(0);
    TAlfTimedPoint sizeTarg;
    TAlfTimedPoint sizeNow;
    CAlfDeckLayout* lay;

    lay = iVisibleItemsBackground[aIndex];

    sizeNow.iY.SetTarget(lay->Size().iY.ValueNow(), 0);
    sizeNow.iX.SetTarget(lay->Size().iX.ValueNow(), 0);
    sizeTarg.iY.SetTarget(lay->Size().iY.Target(), 0);
    sizeTarg.iX.SetTarget(lay->Size().iX.Target(), 0);

    if (sizeNow.iY.Target() != sizeTarg.iY.Target())
        {
        diff = sizeNow.iY.Target() - sizeTarg.iY.Target();
        if (diff < 0)
            {
            diff *= -1;
            }

        if (iCurrentScrollSpeed != 0)
            {
            speed = diff / iCurrentScrollSpeed * 1000;
            }

        TAlfTimedPoint p;
        p.iY.SetTargetWithSpeed(sizeTarg.iY.Target(), speed);
        p.iX.SetTarget(sizeTarg.iX.Target(), 0);
        p.SetStyle(EAlfTimedValueStyleLinear);

        lay->SetSize(p);
        iListLayout->Visual(aIndex).SetSize(p);

        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ScrollListLayoutsL(TInt aScrollOffset, TInt /*aScrollTime*/)
    {
    FUNC_LOG;
    TPoint position(iViewPort.Position());
    if (position.iY != aScrollOffset)
        {
        position.iY = aScrollOffset;
        iViewPort.SetPositionL(position);
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::ScrollListLayouts(TInt aScrollOffset,
        TInt aScrollTime)
    {
    TRAPD( error, ScrollListLayoutsL(aScrollOffset, aScrollTime) );
    return error;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTreeVisualizerBase::NextFocusableItem(
        const TFsTreeItemId aItemId)
    {
    FUNC_LOG;
    TFsTreeItemId treeItem = KFsTreeNoneID;
    TFsTreeItemId foundItem = KFsTreeNoneID;

    TFsTreeIterator treeIter = iTreeData->Iterator(KFsTreeRootID, aItemId,
            KFsTreeIteratorSkipCollapsedFlag | KFsTreeIteratorSkipHiddenFlag);

    while (treeIter.HasNext())
        {
        treeItem = treeIter.Next();
        if (iTreeData->ItemVisualizer(treeItem)->IsFocusable())
            {
            foundItem = treeItem;
            break;
            }
        }

    return foundItem;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFsTreeVisualizerBase::PrevFocusableItem(
        const TFsTreeItemId aItemId)
    {
    FUNC_LOG;
    TFsTreeItemId treeItem = KFsTreeNoneID;
    TFsTreeItemId foundItem = KFsTreeNoneID;

    TFsTreeIterator treeIter = iTreeData->Iterator(KFsTreeRootID, aItemId,
            KFsTreeIteratorSkipCollapsedFlag | KFsTreeIteratorSkipHiddenFlag);

    while (treeIter.HasPrevious())
        {
        treeItem = treeIter.Previous();
        if (iTreeData->ItemVisualizer(treeItem)->IsFocusable())
            {
            foundItem = treeItem;
            break;
            }
        }

    return foundItem;
    }

// ---------------------------------------------------------------------------
//  Function sets current scroll speed based on the up/down key repeats.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetCurrentScrollSpeed(TInt aKeyRepeats)
    {
    FUNC_LOG;
    TInt time(0);
    if (iScrollSpeed > 0)
        {
        time = iIntx->Interval(iScrollSpeed);
        if (time != iScrollSpeed)
            {
            iCurrentScrollSpeed = time;
            if (aKeyRepeats > 0)
                {
                iCurrentScrollSpeed = iCurrentScrollSpeed * (1 - aKeyRepeats
                        * iScrollAccelerationRate);
                if (iCurrentScrollSpeed < 0)
                    {
                    iCurrentScrollSpeed = 1;
                    }
                }
            }
        else
            {
            iCurrentScrollSpeed = iScrollSpeed;
            }

        if (iCurrentScrollSpeed < iScrollSpeed)
            {
            iCurrentScrollSpeed = KZero;
            }
        //        TInt part(0);
        //        part = iScrollSpeed * 0.9;

        //        if ( iCurrentScrollSpeed <= part )
        //            {
        //            iCurrentScrollSpeed = 0;
        //            }
        //        if (iCurrentScrollSpeed < 200)
        //            {
        //            iCurrentScrollSpeed = 200;
        //            }
        }
    }

// ---------------------------------------------------------------------------
//  Gets default settings from Central Repository.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ReadDefaultValuesFromCenRep()
    {
    FUNC_LOG;
    CRepository *crep(NULL);
        TRAP_IGNORE( crep = CRepository::NewL( KCRUidFSGenericUI ) );

    //scroll time
    if (!crep || crep->Get(KFSGenericUIDefScrollSpeed, iScrollSpeed)
            != KErrNone)
        {
        iScrollSpeed = KFsListDefaultScrollSpeed;
        }
    if (iScrollSpeed < 0)
        {
        iScrollSpeed = 0;
        }

    //duration of the fade-in effect
    if (!crep || crep->Get(KFSGenericUIFadeIn, iFadeInEffectTime) != KErrNone)
        {
        iFadeInEffectTime = KFsListDefaultFadeInTime;
        }
    if (iFadeInEffectTime < 0)
        {
        iFadeInEffectTime = 0;
        }

    //duration of the fade-out effect
    if (!crep || crep->Get(KFSGenericUIFadeOut, iFadeOutEffectTime)
            != KErrNone)
        {
        iFadeOutEffectTime = KFsListDefaultFadeOutTime;
        }
    if (iFadeOutEffectTime < 0)
        {
        iFadeOutEffectTime = 0;
        }

    //duration of the slide-in effect
    if (!crep || crep->Get(KFSGenericUISlideInTime, iSlideInDuration)
            != KErrNone)
        {
        iSlideInDuration = KFsListDefaultSlideInTime;
        }
    if (iSlideInDuration < 0)
        {
        iSlideInDuration = KFsListDefaultSlideInTime;
        }

    //duration of the slide-out effect
    if (!crep || crep->Get(KFSGenericUISlideOutTime, iSlideOutDuration)
            != KErrNone)
        {
        iSlideOutDuration = KFsListDefaultSlideOutTime;
        }
    if (iSlideOutDuration < 0)
        {
        iSlideOutDuration = 0;
        }

    //direction of the slide-in effect
    TInt slideInDir;
    if (!crep || crep->Get(KFSGenericUISlideInDir, slideInDir) != KErrNone)
        {
        slideInDir = KFsListDefaultSlideInDir;
        }
    if (slideInDir < 0 || slideInDir > 4)
        {
        slideInDir = 0;
        }
    iSlideInDirection
            = static_cast<MFsTreeVisualizer::TFsSlideEffect> (slideInDir);

    //direction of the slide-out effect
    TInt slideOutDir;
    if (!crep || crep->Get(KFSGenericUISlideOutDir, slideOutDir) != KErrNone)
        {
        slideOutDir = KFsListDefaultSlideOutDir;
        }
    if (slideOutDir < 0 || slideOutDir > 4)
        {
        slideOutDir = 0;
        }
    iSlideOutDirection =
                   static_cast<MFsTreeVisualizer::TFsSlideEffect>(slideOutDir);

    //item expansion time
    if ( !crep || crep->Get( KFSGenericUIListExpTimeOut, iCurrentScrollSpeed ) !=
        KErrNone  )
        {
        iCurrentScrollSpeed = KFsListDefaultListExpTimeOut;
        }
    if (iCurrentScrollSpeed < 1)
        {
        iCurrentScrollSpeed = 1;
        }

    //list looping type
    TInt listLooping;
    if ( !crep || crep->Get( KFSGenericUIListLooping, listLooping) !=
        KErrNone )
        {
        listLooping = KFsListDefaultListLooping;
        }
    if (listLooping < 0 || listLooping > 2)
        {
        listLooping = 0;
        }
    iLooping = static_cast<TFsTreeListLoopingType> (listLooping);

    delete crep;
    }

// ---------------------------------------------------------------------------
//  Sets values to an item which are default for the whole list (e.g. if
//  items are always extended.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ApplyListSpecificValuesToItem(
        MFsTreeItemVisualizer *aItemVis)
    {
    FUNC_LOG;
    //list can have all items in extended or in a normal state
    aItemVis->SetAlwaysExtended(IsItemsAlwaysExtended());

    //global indentation in pixels for a list component
    aItemVis->SetIndentation(iLevelIndentation);

    //global indentation in pixels for a list component
    aItemVis->SetTextStyleManager(*iTextStyleManager);
    }

// ---------------------------------------------------------------------------
// The function calculates tree height up to the specified item.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::CalculateTreeHeightUpToItem(
        const TFsTreeItemId aItemId, TInt& aHeight) const
    {
    FUNC_LOG;
    TFsTreeItemId tmpItem(KFsTreeNoneID);
    TFsTreeItemId itemToFind(KFsTreeNoneID);
    itemToFind = aItemId;
    TInt retVal(KErrNone);
    //validate given item id
    if (!iTreeData->Contains(itemToFind))
        {
        retVal = KErrNotFound;
        }
    if (retVal == KErrNone)
        {
        //create an iterator
        TFsTreeIterator iter = iTreeData->Iterator(KFsTreeRootID,
                KFsTreeRootID, KFsTreeIteratorSkipCollapsedFlag
                        | KFsTreeIteratorSkipHiddenFlag);

        TBool notFound(ETrue);
        TInt height(0);
        MFsTreeItemVisualizer* vis = NULL;

        //iterate up to the given item id and calculate the tree height
        while (notFound && iter.HasNext())
            {
            tmpItem = iter.Next();
            if (tmpItem == itemToFind)
                {
                notFound = EFalse;
                continue;
                }
            else
                {//add item's height
                vis = iTreeData->ItemVisualizer(tmpItem);
                if (vis->IsExtendable() && vis->IsExtended())
                    {
                    height += vis->ExtendedSize().iHeight;
                    }
                else
                    {
                    height += vis->Size().iHeight;
                    }
                }
            }

        if (notFound)
            {
            retVal = KErrNotFound;
            }
        else
            {
            retVal = KErrNone;
            aHeight = height;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
//  Update list's visual content by appending items into empty gap in list.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::FillGapWithItemsL(const TInt /*aHeight*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
//  The function checks if the given item id matches the focused one.
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsItemFocused(TFsTreeItemId aItemId) const
    {
    FUNC_LOG;
    TBool isFocused(EFalse);

    if (iFocusedItem != KFsTreeNoneID)
        {
        if (iFlags.IsSet(EListFocused) && (aItemId == iFocusedItem))
            {
            isFocused = ETrue;
            }
        }
    return isFocused;
    }

// <cmail> "Base class modifications for using touch"

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreeVisualizerBase::CFsTreeVisualizerBase( CAlfControl* aOwnerControl,
        CAlfLayout& aParent, const TBool aPopUpMode )
    : iVisualizationState(EFsTreeHidden),
      iOwnerControl ( aOwnerControl ),
      iParentLayout ( aParent ),
      iTreeData ( NULL ),
      iFocusedItem ( KFsTreeNoneID ),
      iListLayout ( NULL ),
      iLooping ( EFsTreeListLoopingDisabled ),
      iSelectorTexture(NULL),
      iSelectorBrush(NULL),
      iSelectorOpacity(1.0),
      iLeftSelectorBorderWidth(KFSListDefaultSelectorBorderWidth),
      iRightSelectorBorderWidth(KFSListDefaultSelectorBorderWidth),
      iTopSelectorBorderHeight(KFSListDefaultSelectorBorderWidth),
      iBottomSelectorBorderHeight(KFSListDefaultSelectorBorderWidth),
      iSelectorMovementType(EFsSelectorMoveImmediately),
      iMarqueeSpeed(30),
      iScrollSpeed( 0 ),
      iCurrentScrollSpeed(0),
      iScrollAccelerationRate(0.4),
      iFadeInEffectTime(0),
      iFadeOutEffectTime(0),
      iWatermark(NULL),
      iLevelIndentation ( KFsDefaultIndentation ),
      iSlideInDuration ( 0 ),
      iSlideInDirection ( ESlideFromTop ),
      iSlideOutDuration ( 0 ),
      iSlideOutDirection ( ESlideFromTop ),
      iScrollOffset(0),
      iMarqueeType(EFsTextMarqueeForth),
      iMarqueStartDelay( 1000 ), //one sec of delay
      iMarqueCycleStartDelay ( 500 ), //half a sec cycle delay
      iMarqueeRepetitions (-1), //infinite loop
      iViewPort( *this, iWorld )
    {
    FUNC_LOG;
    iFlags.Set( EAutoRefresh );
    iFlags.Assign( EPopupMode, aPopUpMode );
    iFlags.Set( EExpandCollapseOnLongTap );
    }
// </cmail>

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ConstructL()
    {
    FUNC_LOG;
    ReadDefaultValuesFromCenRep();

    if (CAknPhysics::FeatureEnabled())
        {
        iPhysics = CAknPhysics::NewL(*this, NULL );
        iDragHandler = CDragHandler::NewL(*this,
                iPhysics->HighlightTimeout(), iFlags);
        iPhysics->SetFriction(KDefaultFriction);
        }

    iRootData = CFsTreePlainOneLineItemData::NewL();
    HBufC* empty = StringLoader::LoadL(R_AVKON_EMPTY_POPUP_LIST_TEXT);
    CleanupStack::PushL(empty);
    iRootData->SetDataL(*empty);
    CleanupStack::PopAndDestroy(empty);

    iParentLayout.UpdateChildrenLayout();

    iRootVisualizer = CFsTreePlainRootVisualizer::NewL(*iOwnerControl);

    iRootLayout = CAlfDeckLayout::AddNewL(*iOwnerControl, &iParentLayout);
    iRootLayout->EnableBrushesL();
    iRootLayout->SetClipping(ETrue);
    iRootLayout->UpdateChildrenLayout();

    iTextStyleManager = CFsAlfTextStyleManager::NewL(
            iRootLayout->Env().TextStyleManager());

    iRootVisualizer->SetTextStyleManager(*iTextStyleManager);

    iShadowLayout = CAlfDeckLayout::AddNewL(*iOwnerControl, iRootLayout);
    iShadowLayout->EnableBrushesL();
    iShadowLayout->UpdateChildrenLayout();

    iBorderLayout = CAlfDeckLayout::AddNewL(*iOwnerControl, iShadowLayout);
    iBorderLayout->EnableBrushesL();
    iBorderLayout->UpdateChildrenLayout();

    iWatermarkLayout = CAlfDeckLayout::AddNewL(*iOwnerControl, iBorderLayout);
    iWatermarkLayout->EnableBrushesL();
    iWatermarkLayout->UpdateChildrenLayout();

    iComponentLayout = CAlfGridLayout::AddNewL(*iOwnerControl, 1, 1,
            iWatermarkLayout);
    iComponentLayout->SetClipping(ETrue);
    iComponentLayout->EnableBrushesL();
    iComponentLayout->UpdateChildrenLayout();

    if (AknLayoutUtils::LayoutMirrored())
        {
        iDummyScrollbar = CAlfImageVisual::AddNewL(*iOwnerControl);
        iComponentLayout->Append(iDummyScrollbar);
        iListDeck = CAlfDeckLayout::AddNewL(*iOwnerControl, iComponentLayout);
        }
    else
        {
        iListDeck = CAlfDeckLayout::AddNewL(*iOwnerControl, iComponentLayout);
        iDummyScrollbar = CAlfImageVisual::AddNewL(*iOwnerControl);
        iComponentLayout->Append(iDummyScrollbar);
        }
    iListItemBackgroundLayout = CAlfFlowLayout::AddNewL(*iOwnerControl,
            iListDeck);
    iListItemBackgroundLayout->SetFlowDirection(CAlfFlowLayout::EFlowVertical);
    iListItemBackgroundLayout->SetClipping(ETrue);
    iListItemBackgroundLayout->EnableScrollingL(ETrue);
    TAlfTimedPoint tpListScrollOffset;
    tpListScrollOffset.SetStyle(EAlfTimedValueStyleLinear);
    iListItemBackgroundLayout->SetScrollOffset(tpListScrollOffset);

    iSelectorLayout = CAlfDeckLayout::AddNewL(*iOwnerControl, iListDeck);
    iSelectorLayout->SetClipping(ETrue);

    iListLayout = CAlfFlowLayout::AddNewL(*iOwnerControl, iListDeck);
    iListLayout->SetFlowDirection(CAlfFlowLayout::EFlowVertical);

    iListLayout->EnableBrushesL();
    iListLayout->EnableScrollingL(ETrue);
    iListLayout->SetScrollOffset(tpListScrollOffset);

    //list pane's padding taken from layout manager
    EnableListPanePadding(ETrue);

    iListLayout->UpdateChildrenLayout();
    iListItemBackgroundLayout->UpdateChildrenLayout();

    iBorderBrush = CAlfBorderBrush::NewL(iOwnerControl->Env(), 0, 1, 0, 0);
    static_cast<CAlfBorderBrush*> (iBorderBrush)->SetColor(KRgbGray);
    static_cast<CAlfBorderBrush*> (iBorderBrush)->SetThickness(
            TAlfTimedPoint(0.0f, 0.5f));
    iBorderBrush->SetLayer(EAlfBrushLayerForeground);

    iScrollbarVisibility = EFsScrollbarShowAlways;

    iScrollBarLayout = CAlfAnchorLayout::AddNewL( *iOwnerControl, iListDeck );
    iScrollBarLayout->SetFlags( EAlfVisualFlagIgnorePointer );

    if ( iFlags.IsSet( EPopupMode ) )
        {
        iScrollBar = CUiCAknScrollBarAdapter::NewL( *this );
        }
    else
        {
        iScrollBar = CUiCScrollBar::NewL( iOwnerControl->Env(), iScrollBarLayout );
        }

    iScrollBarModel.SetHeight(iComponentLayout->Size().Target().iY);
    iScrollBarModel.SetWindowPosition(iViewPort.Position().iY);
    iScrollBarModel.SetWindowHeight(iListLayout->Size().iY.Target());

    iScrollBar->Show( EFalse );
    iScrollBar->UpdateModelL( iScrollBarModel );

    iFadeEffect = CFsFadeEffect::NewL(iOwnerControl, this, iRootLayout, 0);
    iSlideEffect = CFsSlideEffect::NewL(iOwnerControl, this, iRootLayout, 0);

    iFadeInEffectTime = KFsListDefaultFadeInTime;
    iFadeOutEffectTime = KFsListDefaultFadeOutTime;

    iIntx = new (ELeave) CFsInteractionInterval;

    iOwnerControl->Env().AddActionObserverL(this);

    iWorld.AddObserverL(iViewPort);
    iWorld.AddObserverL(*this);
    UpdateViewPortL();
    }

// ---------------------------------------------------------------------------
//  Handle scrollbar events
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::HandleScrollEventL( const MUiCScrollBar& aScrollBar,
        MUiCScrollBar::TEvent aEvent )
    {
    FUNC_LOG;
    switch (aEvent)
        {
        case MUiCScrollBar::EScrollHome:
            // Home
            break;
        case MUiCScrollBar::EScrollEnd:
            // End
            break;
        default:
            TPoint position(iViewPort.Position());
            position.iY = aScrollBar.ThumbPosition();
            iViewPort.SetPositionL(position);
            break;
        }
    }

// ---------------------------------------------------------------------------
//  Handle visibility change
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::NotifyControlVisibilityChange(TBool aIsVisible)
    {
    FUNC_LOG;

    if (aIsVisible && iVisualizationState == EFsTreeVisible)
        {
        UpdateScrollBar();
        }
    else
        {
        iScrollBar->Show( EFalse );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::DisableKineticScrolling( TBool aDisable )
    {
    FUNC_LOG;
    iKineticScrollingDisabled = aDisable;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsKineticScrollingDisabled() const
    {
    FUNC_LOG;
    return iKineticScrollingDisabled;
    }

// ---------------------------------------------------------------------------
// World updated
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::WorldUpdatedL(const TWorld& /*aWorld*/)
    {
    FUNC_LOG;
    if (iFlags.IsSet(EPhysicsOn))
        {
        iFlags.Set(EUpdatePhysicsAfterSimulationFinished);
        }
    else
        {
        UpdatePhysicsL();
        }
    UpdateScrollBarL();
    }

// ---------------------------------------------------------------------------
// ViewPort updated
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ViewPortUpdatedL(TViewPort& aViewPort, TUpdatedByPhysic aUpdateByPhysic)
    {
    FUNC_LOG;

    // Only upadte view if the list is visible
    if (iVisualizationState != EFsTreeVisible)
        {
        return;
        }

    RArray<TFsTreeItemId> visibleItems;
    RArray<TFsTreeItemId> removableItems;
    TInt scrollOffset;
    TBool fullUpdate;
    CleanupClosePushL(visibleItems);
    CleanupClosePushL(removableItems);
    aViewPort.GetVisibleItemsL(removableItems, visibleItems, scrollOffset,
            fullUpdate);
    TAlfRefreshMode rm(EAlfRefreshModeAutomatic);
    rm = CAlfStatic::Env().RefreshMode();
    CAlfStatic::Env().SetRefreshMode(EAlfRefreshModeManual);
    iListLayout->SetFlag(EAlfVisualFlagFreezeLayout);

	if(aUpdateByPhysic == EUpdatedByPhisicEnd)
		{
    	RArray<TFsTreeItemId> myVisibleItems;
    	TInt myOffset;
    	CleanupClosePushL(myVisibleItems);
    	aViewPort.GetVisibleItemsL(myVisibleItems, myOffset);

    	for (TInt i = 0; i < myVisibleItems.Count(); i++)
    		{
    		const TFsTreeItemId itemId(myVisibleItems[i]);
    		if (itemId != KFsTreeNoneID)
    			{
    			MFsTreeItemVisualizer* visualizer = iTreeData->ItemVisualizer(
    											itemId);
    			visualizer->UpdateL(iTreeData->ItemData(itemId),
    							IsItemFocused(itemId) && IsFocusShown(),
    							iTreeData->Level(itemId), iMarkIcon, iMenuIcon,
    							0);
    			}
    		}
    	CleanupStack::PopAndDestroy();
    	}

    // clear list
    TInt removed(0);
    if (fullUpdate)
        {
        removed += iListLayout->Count();
        for (TInt i = removed - 1; i >= 0; i--)
            {
            iTreeData->ItemVisualizer(VisualItemId(&iListLayout->Visual(i)))->Hide();
            }
        iVisibleItems.Reset();
        }
    else
        {
        for (TInt i = 0; i < removableItems.Count(); i++)
            {
            MFsTreeItemVisualizer* itemVisualizer(iTreeData->ItemVisualizer(removableItems[i]));
            if (itemVisualizer)
                {
                CAlfVisual* visual(&itemVisualizer->Layout());
                if (iListLayout->FindVisual(visual) != KErrNotFound)
                    {
                    removed++;
                    }
                itemVisualizer->Hide();
                const TInt index(iVisibleItems.Find(removableItems[i]));
                if (index != KErrNotFound)
                    {
                    iVisibleItems.Remove(index);
                    }
                }
            }
        }

    // update items to list
    TInt added(0);
    for (TInt i = 0; i < visibleItems.Count(); i++)
        {
        const TFsTreeItemId itemId(visibleItems[i]);
        if (itemId != KFsTreeNoneID)
            {
            MFsTreeItemVisualizer* visualizer = iTreeData->ItemVisualizer(
                    itemId);
            visualizer->ShowL(*iListLayout);
            TSize size(visualizer->Size());
            TInt listInnerWidth(iListLayout->Size().IntTarget().iX);
            TAlfTimedPoint tpItemSize( listInnerWidth, size.iHeight );
            CAlfLayout& visualizerLayout(visualizer->Layout());
            visualizerLayout.SetSize(tpItemSize);
        	visualizer->UpdateL(iTreeData->ItemData(itemId),
                IsItemFocused(itemId) && IsFocusShown(),
                iTreeData->Level(itemId), iMarkIcon, iMenuIcon,
                0);
            visualizerLayout.PropertySetIntegerL(KPropertyItemId(), itemId);
            visualizerLayout.Brushes()->AppendL(iBorderBrush,
                    EAlfDoesNotHaveOwnership);
            CAlfBrush* bgBrush(NULL);
            if (visualizer->GetBackgroundBrush(bgBrush))
                {
                bgBrush->SetLayer(EAlfBrushLayerBackground);
                visualizerLayout.Brushes()->AppendL(bgBrush,
                        EAlfDoesNotHaveOwnership);
                }
            const TInt orderPosition(
                    i < iListLayout->Count() ? i : iListLayout->Count() - 1);
            iListLayout->Reorder(visualizerLayout, orderPosition);
            iVisibleItems.AppendL(itemId);
            added++;
            }
        }

    INFO_2("totalItems: %d, visibleItems: %d", iWorld.ItemCount(), visibleItems.Count());
    CleanupStack::PopAndDestroy(2); // visibleItems.Close(), removableItems.Close()
    // scroll list layout by offset
    TAlfTimedPoint alfOffset;
    alfOffset.iY.SetTarget(scrollOffset, 0);
    iListLayout->SetScrollOffset(alfOffset);
    iListLayout->ClearFlag(EAlfVisualFlagFreezeLayout);
    iListLayout->UpdateChildrenLayout();
    UpdateSelectorVisualL();
    UpdateScrollBarIfNeededL();
    CAlfStatic::Env().RefreshCallBack(&CAlfStatic::Env());
    CAlfStatic::Env().SetRefreshMode(rm);
    }

// ---------------------------------------------------------------------------
// Set panning position
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetPanningPosition(const TPoint& aDelta)
    {
    FUNC_LOG;
    iPhysics->RegisterPanningPosition(aDelta);    
    iFlags.Set( EListPanning );
    SetFocusVisibility( EFalse );
    }

// ---------------------------------------------------------------------------
// Starts flicking.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::StartPhysics(TPoint& aDrag,
        const TTime& aStartTime)
    {
    FUNC_LOG;
    TTime startTime( aStartTime );
    TTime now;
    now.HomeTime();
    TInt moveTime( now.MicroSecondsFrom( aStartTime ).Int64() );
    if (moveTime > KFlickMaxDuration)
        {
        startTime = now - TTimeIntervalMicroSeconds( KFlickMaxDuration - 1 );
        aDrag.iY = aDrag.iY * KFlickMaxDuration / moveTime;
        }
    if (iPhysics->StartPhysics(aDrag, startTime))
        {
        iFlags.Set(EPhysicsOn);
        }
    }

// ---------------------------------------------------------------------------
// Update physics
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::UpdatePhysicsL()
    {
    FUNC_LOG;
    if (iPhysics)
        {
        SafeDelete(iPhysics);
        iPhysics = CAknPhysics::NewL(*this, NULL );
        const TSize viewSize(iViewPort.Size());
        const TSize worldSize(viewSize.iWidth, Max(iWorld.Height(),
                viewSize.iHeight));
        iPhysics->InitPhysicsL(worldSize, viewSize, EFalse);
        iPhysics->SetFriction(KDefaultFriction);
        }
    }

// ---------------------------------------------------------------------------
// Returns viewPort top-left position
// ---------------------------------------------------------------------------
//
TPoint CFsTreeVisualizerBase::ViewPortTopPosition() const
{
    FUNC_LOG;
    return iViewPort.Position();
}

// ---------------------------------------------------------------------------
// Physic updated view position
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::ViewPositionChanged(const TPoint& aNewPosition,
        TBool /*aDrawNow*/, TUint aFlags )
    {
    FUNC_LOG;
    TInt error(KErrNone);
    TRAP( error, iViewPort.SetCenterPositionL(aNewPosition, ETrue,
                aFlags == 0 ? EUpdatedByPhisic : ENotUpdatedByPhisic ) );
    ERROR_1( error, "iViewPort.SetCenterPositionL failed with error: %d", error );
    }

// ---------------------------------------------------------------------------
// Physic emulation has finished
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::PhysicEmulationEnded()
    {
    FUNC_LOG;

    TRAP_IGNORE( iViewPort.SetCenterPositionL(iViewPort.CenterPosition(), ETrue, EUpdatedByPhisicEnd));

    iFlags.Clear(EPhysicsOn);
    if (iFlags.IsSet(EUpdatePhysicsAfterSimulationFinished))
        {
        iFlags.Clear(EUpdatePhysicsAfterSimulationFinished);
            TRAP_IGNORE( UpdatePhysicsL() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TPoint CFsTreeVisualizerBase::ViewPosition() const
    {
    FUNC_LOG;
    return iViewPort.CenterPosition();
    }

// ---------------------------------------------------------------------------
// Wrapper for SetFocusedItemAndSendEventL to trap the error and return it
// instead of leaving.
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::SetFocusedItemAndSendEvent(
    const TFsTreeItemId aItemId, TPointerEventType aEventType, const TPoint& aPoint )
    {
    FUNC_LOG;
    TRAPD(error, SetFocusedItemAndSendEventL( aItemId, aEventType, aPoint ) );
    return error;
    }

// ---------------------------------------------------------------------------
// Sets the specified item as focused and sends event about touch focusing if
// the same item is already focused, action event will be sent instead.
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::SetFocusedItemAndSendEventL(
        const TFsTreeItemId aItemId, TPointerEventType aEventType, const TPoint& aPoint )
    {
    FUNC_LOG;

    // Handles all other than pointer up events.
    // Focus is drawn when:
    //  - new item is selected (FocusedItem() != aItemId)
    //  - pointer is pressed down and focus is not visible by default
    //    (aEventType == EPointerDown ) && IsFocusShown())
    if( ( aEventType < EPointerUp ) &&
        ( !iFlags.IsSet( EListFocused ) || FocusedItem() != aItemId ||
            ( ( aEventType == EPointerDown ) && IsFocusShown() ) ) )
        {
        SetFocusedItemL( aItemId );
        iVisualizerObserver->TreeVisualizerEventL(
            MFsTreeVisualizerObserver::EFsTreeItemTouchFocused,
            FocusedItem(), aPoint );

        // If DirectTouchMode (actions happens in pointer down events)
        if( iFlags.IsSet( EDirectTouchMode ) && aEventType == EPointerDown )
            {
            iTouchPressed = EFalse;
            iVisualizerObserver->TreeVisualizerEventL(
                MFsTreeVisualizerObserver::EFsTreeItemTouchAction,
                aItemId, aPoint );
            }
        }
    else if( aEventType == ELongTap )
        {
        // HandlePointerEventL do not get pointer up event after long tap event
        // so <code>iTouchPressed</code> flag needs to be reset here.
        iTouchPressed = EFalse;
		iVisualizerObserver->TreeVisualizerEventL(
				MFsTreeVisualizerObserver::EFsTreeItemTouchLongTap,
  			    aItemId, aPoint );
        }
    else if( aEventType == EPointerUp )
        {
        // Ignore pointer up action IF physic emulation was stopped on pointer down
        if ( !iFlags.IsSet(EIgnorePointerUpAction) )
            {
            iVisualizerObserver->TreeVisualizerEventL(
                MFsTreeVisualizerObserver::EFsTreeItemTouchAction, aItemId, aPoint );
            }
        }
    // Added for tactile feedback
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if( feedback )
        {
        feedback->InstantFeedback( ETouchFeedbackBasic );
        }
    }

// CFsTreeVisualizerBase::CDragHandler

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFsTreeVisualizerBase::CDragHandler* CFsTreeVisualizerBase::CDragHandler::NewL(
        CFsTreeVisualizerBase& aTree, const TInt aHighlightTimeout,
        const TBitFlags& aFlags)
    {
    FUNC_LOG;
    CDragHandler* self = new (ELeave) CDragHandler(aTree, aHighlightTimeout,
            aFlags);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFsTreeVisualizerBase::CDragHandler::~CDragHandler()
    {
    FUNC_LOG;
    delete iHighlightTimer;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CDragHandler::Reset()
    {
    FUNC_LOG;
    iFlags.ClearAll();
    iHighlightTimer->Cancel();
    iItemId = KFsTreeNoneID;
    iDragDirection = EDraggingNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFsTreeVisualizerBase::CDragHandler::TDragDirection
    CFsTreeVisualizerBase::CDragHandler::DragDirection(
        const TPoint& aCurrent, const TPoint aPrevious ) const
    {
    FUNC_LOG;
    if (aCurrent.iY > aPrevious.iY)
        {
        return EDraggingDown;
        }
    else if (aCurrent.iY < aPrevious.iY)
        {
        return EDraggingUp;
        }
    else
        {
        return iDragDirection;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CDragHandler::PointerDown(
        const TPointerEvent& aEvent, const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    Reset();
    iItemId = aItemId;
    iPosition = aEvent.iParentPosition;
    iLastPointerPosition = aEvent.iParentPosition;
    iStartTime.HomeTime();
    iFlags.Set( EPointerDownReceived );
    iTree.SetFocusedItemAndSendEvent( iItemId, EPointerDown, iPosition );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CDragHandler::PointerRepeat(
        const TPointerEvent& aEvent )
    {
    if( iItemId != KFsTreeNoneID && !IsFlicking() )
        {
        iFlags.Set( EWasRepeat );
        iTree.SetFocusedItemAndSendEvent( iItemId, ELongTap, aEvent.iParentPosition );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CDragHandler::PointerUp(
        const TPointerEvent& aEvent, const TFsTreeItemId aItemId )
    {
    FUNC_LOG;
    // If kinetic scrolling is disabled
    if( iTree.IsKineticScrollingDisabled() )
        {
        if( ( aItemId != KFsTreeNoneID ) &&
            ( iTree.FocusedItem() == aItemId  ) )
            {
            iTree.SetFocusedItemAndSendEvent( iItemId, EPointerUp, aEvent.iParentPosition );
            }
        }
    else if ( IsFlicking() )
        {
        TPoint drag(iPosition - aEvent.iParentPosition);
        iTree.StartPhysics( drag, iStartTime );
        }
    else if( !iFlags.IsSet( EWasRepeat ) && iItemId != KFsTreeNoneID )
        {
        iTree.SetFocusedItemAndSendEvent( iItemId, EPointerUp, aEvent.iParentPosition );
        }
    iLastPointerPosition = iPosition = TPoint();
    iFlags.Clear( EPointerDownReceived );
    iDragDirection = EDraggingNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::CDragHandler::DragDelta(
        const TPointerEvent& aEvent) const
    {
    FUNC_LOG;
    return Abs(iPosition.iY - aEvent.iPosition.iY);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TPoint CFsTreeVisualizerBase::CDragHandler::PointerDrag(
        const TPointerEvent& aEvent, const TFsTreeItemId /*aItemId*/)
    {
    FUNC_LOG;
    // If kinetic scrolling is disabled
    if( iTree.IsKineticScrollingDisabled() )
        {
        iLastPointerPosition = iPosition;
        return aEvent.iPosition;
        }

    if (!iFlags.IsSet(EPointerDownReceived))
        {
        iLastPointerPosition = iPosition = aEvent.iPosition;
        iFlags.Set(EPointerDownReceived);
        }
    else
        {
        iHighlightTimer->Cancel();
        if (!iTreeVisualizerFlags.IsSet(EPhysicsOn))
            {
            iTree.SetPanningPosition(TPoint(0, iLastPointerPosition.iY
                    - aEvent.iPosition.iY));
            }
        TDragDirection dragDirection(DragDirection(aEvent.iPosition, iLastPointerPosition));
        const TBool dragDirectionChanged(
                (dragDirection == EDraggingUp && iDragDirection == EDraggingDown ) ||
                (dragDirection == EDraggingDown && iDragDirection == EDraggingUp ) );
        if (dragDirectionChanged)
            {
            iPosition = aEvent.iPosition;
            iStartTime.HomeTime();
            }
        iDragDirection = dragDirection;
        iLastPointerPosition = aEvent.iPosition;
        }
    return iPosition - aEvent.iPosition;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CDragHandler::EnableFlicking()
    {
    FUNC_LOG;
    iFlags.Set(EFlicking);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::CDragHandler::IsFlicking() const
    {
    FUNC_LOG;
    return iFlags.IsSet(EFlicking);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFsTreeVisualizerBase::CDragHandler::CDragHandler(
        CFsTreeVisualizerBase& aTree, const TInt aHighlightTimeout,
        const TBitFlags& aFlags) :
    iTree(aTree), iHighlightTimeout(aHighlightTimeout * 1000),
            iTreeVisualizerFlags(aFlags)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFsTreeVisualizerBase::CDragHandler::ConstructL()
    {
    FUNC_LOG;
    iHighlightTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CFsTreeVisualizerBase::CDragHandler::HighlightTimerCallback( TAny* aPtr )
    {
    FUNC_LOG;
    CDragHandler* self = reinterpret_cast<CDragHandler*>( aPtr );
    self->iHighlightTimer->Cancel();
    return self->iTree.SetFocusedItemAndSendEvent( self->iItemId, ELongTap, self->iPosition );
    }

// ---------------------------------------------------------------------------
// CFsTreeVisualizerBase::IsFocusShown
// ---------------------------------------------------------------------------
//
TBool CFsTreeVisualizerBase::IsFocusShown()
    {
    return ( iTouchPressed || iFocusVisible ) && iFlags.IsClear( EListPanning );
    }


// </cmail>
