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
* Description:  Visualizer class for the hierarchical tree list.
*
*/


#ifndef C_FSTREEVISUALIZER_H
#define C_FSTREEVISUALIZER_H


//////SYSTEM INCLUDES
#include <e32base.h>
#include <babitflags.h>

//////TOOLKIT INCLUDES
// <cmail> SF
#include <alf/alfenv.h> //MAlfActionObserver
// </cmail>

//////PROJECT INCLUDES
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <aknphysicsobserveriface.h>

#include "fstreevisualizer.h"
#include "fstreeobserver.h"
#include "fsfadeeffectobserver.h"
#include "fsslideeffectobserver.h"
#include "fsscrollbarset.h"

//////TOOLKIT CLASSES
class CAlfLayout;
class CAlfTexture;
class CAlfBrush;
class TAlfEvent;
class CAlfGridLayout;
class CAlfDeckLayout;
class CAlfFlowLayout;
class CAlfImageVisual;
class CAlfImageBrush;

//////PROJECT CLASSES
class MFsTreeItemData;
class MFsTreeNodeVisualizer;
class MFsTreeVisualizerObserver;
class CFsTree;
class CFsTreeListLayoutData;
class CFsFadeEffect;
class CFsSlideEffect;
class CFsTreePlainOneLineItemData;
class CFsWatermark;
class MFsTreeItemVisualizer;
class CFsInteractionInterval;
class CFsAlfTextStyleManager;
class CAknDoubleSpanScrollBar;

class CAknPhysics;

/**
 * Contains the visual structure of the list.
 *
 */
NONSHARABLE_CLASS( TWorld )
    {

public:

    /**
     * World observer interface
     */
    class MObserver
        {
    public:

        /**
         * This method is called when world is changed.
         */
        virtual void WorldUpdatedL( const TWorld& aWorld ) = 0;

        };

public:

    /**
     * Constructor
     */
    TWorld();

    /**
     * Destructor
     */
    ~TWorld();

    /**
     * Add new observer to the world
     */
    void AddObserverL( MObserver& aObserver );

    /**
     * Remove observer from the world, if the observer is not found, does nothing.
     */
    void RemoveObserver( MObserver& aObserver );

    /**
     * Append new item to the bottom of world.
     */
    void AppendL( const TFsTreeItemId aId, const TSize& aSize );

    /**
     * Insert new item to the world. Items after inserted item will be shifted down.
     */
    void InsertL( const TFsTreeItemId aId, const TSize& aSize, const TInt aSlot );

    /**
     * Remove item from the world. Items after removed item will be shifted up.
     */
    void RemoveL( const TFsTreeItemId aId );

    /**
     * Clear the world.
     */
    void RemoveAllL();

    /**
     * Get items in range aY to aY + aHeight. Also returns offset by which the listlayout
     * needs to be scrolled (e.g. the first item in list might be only partly visible)
     */
    void GetItemsL( RArray<TFsTreeItemId>& aItems, TInt aY, TInt aHeight, TInt& aOffset );

    /**
     * Returns full height of the world (in pixels).
     */
    TInt Height() const;

    /**
     * Begin batch update. When batch update is going on, single modifications to the world
     * will not be signalled to the observers.
     */
    void BeginUpdate();

    /**
     * End batch update. Observers will be signalled.
     */
    void EndUpdateL();

    /**
     * Returns item's rect.
     */
    TInt GetItemRect( const TFsTreeItemId, TRect& aRect ) const;

    /**
     * Returns item's rect by index in world.
     */
    TInt GetItemRectByIndex(const TInt aIndex, TRect& aRect) const;

    /**
     * Returns ETrue if batch update is going on.
     */
    TBool IsUpdating() const;

    /**
     * Reconstructs the whole world from given tree.
     */
    void Recalculate( const CFsTree& aTree );

    // for debug print

    /**
     * Checks world's integrity (e.g. there cannot be any overlapping items in the world)
     */
    TBool CheckIntegrity() const;

    /**
     * Returns number of items in the world.
     */
    TInt ItemCount() const;

    /**
     * Returns item id for the item at index aIndex.
     */
    TFsTreeItemId Item( TInt aIndex ) const;

    /**
     * Returns index of item with id aItem.
     */
    TInt IndexOfItem( TFsTreeItemId aItem ) const;

private:

    /**
     * Move items from aSlot onwards down by aDelta amount.
     */
    void MoveBy( const TInt aSlot, const TInt aDelta);

    /**
     * Called when world has been updated.
     */
    void UpdatedL();

private:

    /**
     * World item. Contains tree Id and visual rect.
     */
    class TItem
        {

    public:

        /**
         * Constructor
         */
        TItem( const TFsTreeItemId aId, const TRect& aRect );

        /**
         * Item rect
         */
        TRect Rect() const;

        /**
         * Sets item's rect
         */
        void SetRect( const TRect& aRect );

        /**
         * Move item by given amount.
         */
        void Move( TInt aDy );

        /**
         * Item's top Y coordinate
         */
        TInt Top() const;

        /**
         * Item's top left coordinate
         */
        TPoint TopLeft() const;

        /**
         * Item's height
         */
        TInt Height() const;

        /**
         * Item's bottom Y coordinate
         */
        TInt Bottom() const;

        /**
         * Item's bottom right coordinate
         */
        TPoint BottomRight() const;

    public:

        // Tree item id
        TFsTreeItemId iId;

    private:

        // Top left Y coordinate of the item
        TInt iY;

        // Height of the item
        TInt iHeight;

        };

private:

    // World observers
    RPointerArray<MObserver> iObservers;

    // World items
    RArray<TItem> iItems;

    // Total height of the world
    TInt iHeight;

    // True if batch update is going on
    TBool iBatchUpdateOngoing;

    };

/**
 * ViewPort to the list World. Is used to access the visible set of
 * the list.
 *
 */
NONSHARABLE_CLASS( TViewPort ) : public TWorld::MObserver
    {

public:

    /**
     * Hints for item repositioning inside the viewport.
     */
    enum TPositionHint
        {
        /** Keep the item position exactly same. */
        EPositionExact,

        /** Allow repositioning method to modify original position,
         * because item was expanded
         */
        EPositionAfterExpand
        };

public:

    /**
     * Viewport observer interface.
     */
    class MObserver
        {

    public:

        /**
         * This method is called when the viewport changes.
         */
        virtual void ViewPortUpdatedL( TViewPort& aViewPort ) = 0;

        };

public:

    /**
     * Constructor
     */
    TViewPort( MObserver& aObserver, TWorld& aWorld );

    /**
     * Set viewport position to aPosition. If aInformUpdate is set to ETrue, observer
     * will be signalled.
     */
    void SetPositionL( const TPoint& aPosition, TBool aInformUpdate = ETrue );

    /**
     * Center viewport around position aPosition. If aInformUpdate is set to ETrue, observer
     * will be signalled.
     */
    void SetCenterPositionL( const TPoint& aPosition, TBool aInformUpdate = ETrue );

    /**
     * Returns viewport position (i.e. topleft corner).
     */
    const TPoint& Position() const;

    /**
     * Returns viewport center position.
     */
    TPoint CenterPosition() const;

    /**
     * Sets viewport size.
     */
    void SetSizeL( const TSize& aSize );

    /**
     * Returns viewport's size.
     */
    const TSize& Size() const;

    /**
     * Get items visible in viewport's area.
     */
    void GetVisibleItemsL( RArray<TFsTreeItemId>& aItemsToBeRemoved, RArray<TFsTreeItemId>& aItemsToBeAdded, TInt& aOffset, TBool& aFullUpdate  );

    /**
     * Returns ETrue if the world is higher than viewport.
     */
    TBool IsScrollBarNeeded() const;

    /**
     * Scroll viewport to item aItemId. If aInformUpdate is set to ETrue, observer
     * will be signalled.
     */
    void ScrollToItemL( const TFsTreeItemId aItemId, TBool aInformUpdate = ETrue );

    /**
     * Center viewport to item aItemId. If aInformUpdate is set to ETrue, observer
     * will be signalled.
     */
    void CenterToItemL( const TFsTreeItemId aItemId, TBool aInformUpdate = ETrue );

    /**
     * Scroll viewport so that aItemId becomes visible. If aInformUpdate is set to
     * ETrue, observer will be signalled.
     */
    void ScrollItemToViewL( const TFsTreeItemId aItemId, TBool aInformUpdate = ETrue );

    /**
     * Scroll viewport so that whole view is fully occupied by the list (if possible).
     */
    void MakeViewFullyOccupiedL( TBool aInformUpdate = ETrue );

    /**
     * Scoll viewport so that given item is shown in given position in view.
     * If aInformUpdate is set to ETrue, observer will be signalled.
     */
    void ItemToPositionL( const TFsTreeItemId aItemId, const TPoint& aPosition,
            TBool aInformUpdate = ETrue,
            const TPositionHint aHint = EPositionExact );

    /**
     * Returns viewport top item.
     */
    TFsTreeItemId TopItem() const;

    /**
     * Returns viewport bottom item.
     */
    TFsTreeItemId BottomItem() const;

    /**
     * Clears cache to do full list update.
     */
    void ClearCache();

    /**
     * Fetches item rect in viewport. If TRect::Height() is non zero, then item
     * is visible in viewport and TRect contains item coordinates.
     */
    TRect ItemRect( const TFsTreeItemId aItemId ) const;

    /**
     * Returns ETrue if the item is fully visible in viewport.
     */
    TBool ItemFullyVisible( const TFsTreeItemId aItemId ) const;

private:

    /**
     * Limits given top value so that whole list is correctly layouted.
     */
    void LimitTop( TInt& aTop ) const;

private:

    /**
     * Called when viewport is updated.
     */
    void UpdatedL();

private: // from TWorld::MObserver

    /**
     * @see TWorld::MObserver::WorldUpdatedL
     */
    virtual void WorldUpdatedL( const TWorld& aWorld );

private:

    /**
     * Viewport cache class. Contains items from previous visible item query. On following
     * queries only changed items will be returned. For performance reasons array will contain
     * NULL items in slots that have not changed.
     */
    NONSHARABLE_CLASS( TCache )
        {

    public:

        /**
         * Constructor
         */
        TCache();

        /**
         * Destructor
         */
        ~TCache();

        /**
         * Clears cache
         */
        void Clear();

        /**
         * Check if the cache is clear.
         */
        TBool IsClear() const;

        /**
         * Check if the given item is in cache.
         */
        TBool ContainsItem( const TFsTreeItemId aItemId ) const;

        /**
         * Updates cache.
         */
        void UpdateL( const RArray<TFsTreeItemId>& aItems, RArray<TFsTreeItemId>& aItemsToBeRemoved, RArray<TFsTreeItemId>& aItemsToBeAdded, TBool& aFullUpdate );

        /**
         * Returns top most item from the cache.
         */
        TFsTreeItemId TopItem() const;

        /**
         * Returns bottom most item from the cache.
         */
        TFsTreeItemId BottomItem() const;

    private:

        // Cached items
        RArray<TFsTreeItemId> iCachedItems;

        // True if the cache is clear
        TBool iClear;
        };

private:

    // Viewport observer
    MObserver& iObserver;

    // World to which this viewport is related to
    TWorld& iWorld;

    // Viewport position (topleft corner)
    TPoint iPosition;

    // Size of the viewport
    TSize iSize;

    // Viewport cache
    TCache iCache;

    };

/**
 * Visualizer class for the hierarchical tree list.
 *
 * The tree visualizer is responsible for visualizing tree structure in a
 * hierarchical list form. It is responsible for updating the list view based
 * on events that come from the tree structure (through MFsTreeObserver
 * interface). For example refreshing the list view after an item was added to
 * the tree.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib
 */
NONSHARABLE_CLASS( CFsTreeVisualizerBase ) : public CBase,
                                             public MFsTreeVisualizer,
                                             public MFsTreeObserver,
                                             public MFsFadeEffectObserver,
                                             public MFsSlideEffectObserver,
                                             public MAlfActionObserver,
                                             public MEikScrollBarObserver,
                                             public TWorld::MObserver,
                                             public TViewPort::MObserver,
                                             public MAknPhysicsObserver
    {

private:

    /**
     * Event types
     */
    enum TPointerEventType
        {
        EPointerDown = 0,
        ELongTap,
        EPointerUp
        };

private:

    /**
     * Drag handler class for handling pointer event and drag.
     */
    NONSHARABLE_CLASS( CDragHandler ) : public CBase
        {
    private:

        /**
         * Drag handler's internal flags
         */
        enum TFlag {

            /**
             * Flicking mode on
             */
            EFlicking,

            /**
             * Pointer repeat was received
             */
            EWasRepeat,

            /**
             * Pointer down was received
             */
            EPointerDownReceived,

        };

        /**
         * Drag direction enumeration
         */
        enum TDragDirection {
            EDraggingNone,

            /**
             * Drag direction up
             */
            EDraggingUp,

            /**
             * Drag direction down
             */
            EDraggingDown
        };

    public:
        /**
         * Leave-safe constructor
         */
        static CDragHandler* NewL(CFsTreeVisualizerBase& aTree, const TInt aHighlightTimeout,
                const TBitFlagsT<TUint>& aFlags );

        /**
         * Destructor
         */
        ~CDragHandler();

        /**
         * Pointer down event handling
         */
        void PointerDown(const TPointerEvent& aEvent, const TFsTreeItemId aItemId);

        /**
         * Pointer repeat event handling
         */
        void PointerRepeat(const TPointerEvent& aEvent);

        /**
         * Pointer up event handling
         */
        void PointerUp(const TPointerEvent& aEvent, const TFsTreeItemId aItemId);

        /**
         * Pointer drag event handling
         */
        TPoint PointerDrag(const TPointerEvent& aEvent, const TFsTreeItemId aItemId);

        /**
         * Returns currect drag delta
         */
        TInt DragDelta(const TPointerEvent& aEvent) const;

        /**
         * Set flicking mode on
         */
        void EnableFlicking();

        /**
         * Test if flicking
         */
        TBool IsFlicking() const;

        /**
         * Reset state
         */
        void Reset();

        /**
         * Calculate drag direction from given arguments
         */
        TDragDirection DragDirection( const TPoint& aCurrent, const TPoint aPrevious ) const;

    private:

        /**
         * Private constructor
         */
        CDragHandler(CFsTreeVisualizerBase& aTree, const TInt aHighlightTimeout, const TBitFlagsT<TUint>& aFlags);

        /**
         * Leave safe construct.
         */
        void ConstructL();

        /**
         * Timer callback.
         */
        static TInt HighlightTimerCallback(TAny* aPtr);

    private:

        CFsTreeVisualizerBase& iTree;
        TBitFlagsT<TUint> iFlags;
        TTimeIntervalMicroSeconds32 iHighlightTimeout;
        CPeriodic* iHighlightTimer;
        TTime iStartTime;
        TPoint iPosition;
        TPoint iLastPointerPosition;
        TPoint iDrag;
        TFsTreeItemId iItemId;
        const TBitFlagsT<TUint>& iTreeVisualizerFlags;
        TDragDirection iDragDirection;
        };


public:

    enum TFsTreeVisualizerMove
        {
        EFsTreeVisualizerMoveLineUp,
        EFsTreeVisualizerMoveLineDown,
        EFsTreeVisualizerMovePageUp,
        EFsTreeVisualizerMovePageDown
        };

    enum TFsSelectorMovement
        {
        /**
         * The selector moves immediately between list's items.
         */
        EFsSelectorMoveImmediately,

        /**
         * The selector moves smoothly between list's items.
         */
        EFsSelectorMoveSmoothly
        };

public: // Construction

    /**
     * Two-phased constructor
     *
     * @param aOwnerControl Owner control.
     * @param aParent Parent layout for all the new layouts constructed by
     *                this class.
     *
     * @return Pointer to the newly constructed object.
     */
    IMPORT_C static CFsTreeVisualizerBase* NewL( CAlfControl* aOwnerControl,
            CAlfLayout& aParent, const TBool aDirectTouchMode = EFalse );

    /**
     * C++ destructor
     */
    virtual ~CFsTreeVisualizerBase();

public:

    /**
     * Sets data model for the visualizer.
     *
     * @param aTreeData Data model visualized by the tree visualizer.
     */
    void SetTreeData( CFsTree& aTreeData );

    /**
     * Adds custom key which triggers page-up event.
     *
     * @param aKey Key code.
     */
    virtual void AddCustomPageUpKey(TInt aKeyCode);

    /**
     * Removes given key code from custom page-up event key codes.
     *
     * @param aKey Key code.
     *
     * @return ETrue if the key code was removed.
     */
    virtual TBool RemoveCustomPageUpKey(TInt aKeyCode);

    /**
     * Checks whether given key is a custom key for page-up event.
     *
     * @param aKey Key code.
     *
     * @return ETrue if the given key code triggers page-up event.
     */
    virtual TBool IsCustomPageUpKey(TInt aKeyCode);

    /**
     * Adds custom key which triggers page-down event.
     *
     * @param aKey Key code.
     */
    virtual void AddCustomPageDownKey(TInt aKeyCode);

    /**
     * Removes given key code from custom page-down event key codes.
     *
     * @param aKey Key code.
     *
     * @return ETrue if the key code was removed.
     */
    virtual TBool RemoveCustomPageDownKey(TInt aKeyCode);

    /**
     * Checks whether given key is a custom key for page-down event.
     *
     * @param aKey Key code.
     *
     * @return ETrue if the given key code triggers page-down event.
     */
    virtual TBool IsCustomPageDownKey(TInt aKeyCode);

    /**
     * Sets icon for a mark sign.
     *
     * @param aMarkIcon Icon to mark items.
     */
    virtual void SetMarkIcon( CAlfTexture& aMarkIcon );

    /**
     * Returns icon used to mark items.
     */
    virtual CAlfTexture* MarkIcon( );

    /**
     * Checks if item is marked and return status
     * @param aItemId item id to be checked
     */
    virtual TBool IsMarked( const TFsTreeItemId aItemId ) const;

    /**
     * Sets status of the item to be marked/unmarked
     * @param aItemId item id which of the mark/unmark status will be set
     * @param aMarked Marked/Unmarked status
     */
    virtual void MarkItemL( const TFsTreeItemId aItemId, TBool aMarked );

    /**
     * Returns list of marked items
     * @param aMarkedItems list to which the id's of marked items will be
     *                     added
     */
    virtual void GetMarkedItemsL( RFsTreeItemIdList& aMarkedItems ) const;

    /**
     * Function marks/unmarks all items in the list.
     *
     * @param aMarkAll ETrue to mark all items, EFalse to unmark.
     */
    virtual void MarkAllItemsL( TBool aMarkAll );

    /**
     * Sets item indentation per hierarchy level.
     *
     * @param aIndentation Indentation in pixels
     */
    virtual void SetIndentationL( TInt aIndentation );

    /**
     * Returns indentation per hierarchy level in pixels.
     *
     * @return Indentation per hierarchy level in pixels.
     */
    virtual TInt Indentation() const;

    /**
     * Sets icon for a menu sign.
     *
     * @param aMenuIcon Menu icon.
     */
    virtual void SetMenuIcon( CAlfTexture& aMenuIcon );

    /**
     * Returns menu icon.
     */
    virtual CAlfTexture* MenuIcon( ) const;

    /**
     * Update flip state
     * @param aOpen ETrue if flip is open
     */
    virtual void SetFlipState( TBool aOpen );

    /**
     * Update focus visibility state
     * @param aShow ETrue if the focus should be shown
     */
    virtual void SetFocusVisibility( TBool aShow );

    /**
     * Used internally by the list. To make the list focused/not focused use
     * CFsTreeList::SetFocusedL() instead.
     */
    void SetFocusedL( const TBool aFocused );

    /**
     *
     * @param aTexture A custom texture for the selector.
     *                 The texture is used to create an image brush.
     *                 Ownership is not transferred.
     *                 If the parameter is NULL, the selector is taken from
     *                 the s60 skin.
     *
     * @param aMovementType Defines the way the selector moves between items.
     *                      Currently only EFsSelectorMoveImmediately is
     *                      supported.
     * @param aLeftBorderWidth Left border value.
     *                         Positive values extend the image border and
     *                         negative values decrease the visual area.
     *                         Zero values - draw without border.
     * @param aRightBorderWidth Right border value.
     *                          Positive values extend the image border and
     *                          negative values decrease the visual area.
     *                          Zero values - draw without border.
     * @param aTopBorderHeight Top border value.
     *                         Positive values extend the image border and
     *                         negative values decrease the visual area.
     *                         Zero values - draw without border.
     * @param aBottomBorderHeight Bottom border value.
     *                            Positive values extend the image border and
     *                            negative values decrease the visual area.
     *                            Zero values - draw without border.
     */
    virtual void SetSelectorPropertiesL ( CAlfTexture* aTexture = NULL,
                                          TFsSelectorMovement aMovementType =
                                                  EFsSelectorMoveImmediately,
                                          TInt aLeftBorderWidth = 0,
                                          TInt aRightBorderWidth = 0,
                                          TInt aTopBorderHeight = 0,
                                          TInt aBottomBorderHeight = 0 );

    /**
     * Set some of the selector's properties.
     *
     * @param aSelectorBrush A custom brush for selector's visual.
     *                       Ownership is not transferred.
     *                       If the parameter is NULL, the selector is taken
     *                       from the s60 skin.
     *
     * @param aMovementType Defines the way the selector moves between items.
     *                      Currently only EFsSelectorMoveImmediately is
     *                      supported.
     *
     * @param aOpacity Opacity for the selector visual.
     *
     */
    virtual void SetSelectorPropertiesL ( CAlfBrush* aSelectorBrush,
                TReal32 aOpacity = 1.0,
                TFsSelectorMovement aMovementType = EFsSelectorMoveImmediately );


    /**
     * The function returns selector's properties.
     */
    virtual void GetSelectorProperties ( CAlfTexture*& aTexture,
                                         TFsSelectorMovement& aMovementType,
                                         TInt& aLeftBorderWidth,
                                         TInt& aRightBorderWidth,
                                         TInt& aTopBorderHeight,
                                         TInt& aBottomBorderHeight );

    // from base class MFsTreeVisualizer

    /**
     * @see MFsTreeVisualizer::HandleKeyEventL
     */
    virtual TBool HandleKeyEventL(const TAlfEvent& aEvent);

    /**
     * @see MFsTreeVisualizer::HandlePointerEventL
     */
    virtual TBool  HandlePointerEventL( const TAlfEvent& aEvent );


    /**
     * Sets the specified item as focused.
     *
     * @param aItem indicates item to be focused.
     *
     * @panic EFsListPanicInvalidItemID Item with specified ID is not found.
     */
    // <cmail>
    virtual void SetFocusedItemL( const TFsTreeItemId aItem, TBool aCheckFocus = ETrue );
    // </cmail>

    /**
     * Returns item id of the currently focused item.
     *
     * @return Id of the focused item.
     */
    virtual TFsTreeItemId FocusedItem() const;

    /**
     * Checks if the specified item is focused.
     *
     * @param aItem Item to be checked.
     *
     * @return ETrue If item is focused. EFalse if item is not focused or
     *         item does not exist in the tree.
     */
    virtual TBool IsFocused( const TFsTreeItemId aItemId ) const;

    /**
     * Sets an item as first visible one in the list.
     *
     * @param aItemId Id of an item that will be displayed as first.
     */
    // <cmail>
    virtual void SetFirstVisibleItemL( const TFsTreeItemId aItemId );
    // </cmail>

    /**
     * Get an id of the first visible item.
     *
     * @return Id of the first visible item.
     */
    virtual TFsTreeItemId FirstVisibleItem();

    /**
     * Gets a pointer to the root's item data object.
     *
     * @return Pointer to the root's item data object.
     */
    MFsTreeItemData* RootItemData();

    /**
     * Gets a pointer to the root's item visualizer object.
     *
     * @return Pointer to the root's item visualizer object.
     */
    MFsTreeNodeVisualizer* RootNodeVisualizer();

    /**
     * Displays the list.
     *
     * @param aFadeIn Defines if list visual should or shouldn't fade in to
     *                the screen.
     * @param aSlideIn ETrue if slide in effect should be used, EFalse if not.
     */
    virtual void ShowListL( const TBool aFadeIn = EFalse,
                            const TBool aSlideIn = EFalse );

    /**
     * Hides the list.
     *
     * @param aFadeOut Defines if list visual should or shouldn't fade out
     *                 from the screen.
     * @param aSlideOut ETrue if slide out effect should be used,EFalse if not
     */
    virtual void HideList( const TBool aFadeOut = EFalse,
                           const TBool aSlideOut = EFalse );

    /**
     * The function sets duration of the slide-in effect.
     *
     * @param aTimeMiliseconds Slide in time in milliseconds.
     */
    virtual void SetSlideInDuration ( TInt aTimeMiliseconds );

    /**
     * The function returns slide in effect's duration.
     *
     * @return Slide in duration in miliseconds.
     */
    virtual TInt SlideInDuration ( ) const;

    /**
     * The function sets direction of the slide in effect.
     *
     * @param aDirection A direction of the slide in effect.
     */
    virtual void SetSlideInDirection ( TFsSlideEffect aDirection );

    /**
     * The function returns direction of the slide in effect.
     *
     * @return A direction of the slide in effect.
     */
    virtual MFsTreeVisualizer::TFsSlideEffect SlideInDirection ( ) const;

    /**
     * The function sets duration of the slide-out effect.
     *
     * @param aTimeMiliseconds Slide out time in milliseconds.
     */
    virtual void SetSlideOutDuration ( TInt aTimeMiliseconds );

    /**
     * The function returns slide out effect's duration.
     *
     * @return Slide out duration in miliseconds.
     */
    virtual TInt SlideOutDuration ( ) const;

    /**
     * The function sets direction of the slide out effect.
     *
     * @param aDirection A direction of the slide out effect.
     */
    virtual void SetSlideOutDirection ( TFsSlideEffect aDirection );

    /**
     * The function returns direction of the slide out effect.
     *
     * @return A direction of the slide out effect.
     */
    virtual MFsTreeVisualizer::TFsSlideEffect SlideOutDirection ( ) const;

    /**
     * Returns information about looping type of the list.
     *
     * @return Type of list looping. EFsTreeListLoopingDisabled when looping
     *         is turned off.
     */
    virtual TFsTreeListLoopingType LoopingType() const;

    /**
     * Sets type of list's looping.
     *
     * @param aLoopingType type of list looping.
     */
    virtual void SetLoopingType( const TFsTreeListLoopingType aLoopingType );

    /**
     * Sets the delay for the item expansion animation.
     *
     * @param aDelay Delay in miliseconds.
     */
    virtual void SetItemExpansionDelay( const TInt aDelay );

    /**
     *
     */
    virtual void SetItemSeparatorColor( const TRgb& aColor );

    /**
     * The functions sets fade-in's effect duration.
     * @param aFadeTime A time in miliseconds for fade in effect.
     */
    virtual void SetFadeInEffectTime( TInt aFadeTime );

    /**
     * Get the current parent layout of tree visualizer.
     *
     * @return Tree visualizer's current parent layout.
     */
    CAlfLayout& ParentLayout() const;

    /**
     * The functions sets fade's effect duration.
     *
     * @param aFadeTime A time in miliseconds for fade in/out effect.
     */
    virtual TInt FadeInEffectTime( );

    /**
     * The functions sets fade-out's effect duration.
     * @param aFadeTime A time in miliseconds for fade-out effect.
     */
    virtual void SetFadeOutEffectTime( TInt aFadeTime );

    /**
     * The functions returns fade's effect duration.
     *
     * @return A time in miliseconds for fade in/out effect.
     */
    virtual TInt FadeOutEffectTime( );

    /**
     * Set the item separator size (item border).
     *
     * @param aSize
     */
    virtual void SetItemSeparatorSize( const TSize aSize );

    /**
     * Sets item border's thickness. To have one pixel border between items in
     * the list pass TAlfTimedPoint(0.0f, 0.5f) as a parameter.
     *
     * @param aThickness Thickness of the border.
     */
    virtual void SetItemSeparatorSize( const TAlfTimedPoint& aThickness );

    /**
     * Function sets the time of list scroll per item and the scroll
     * acceleration rate.
     *
     * @param aScrollTime Time (in miliseconds) at which the list items are
     *                    scrolled.
     * @param aScrollAcceleration Rate of scroll acceleration when the up/down
     *                            key is pressed for a long time. This is
     *                            multiplied by the number of up/down key
     *                            repeats. Acceleration rate should be in
     *                            0.0-1.0 range.
     */
    virtual void SetScrollTime ( TInt aScrollTime,
                                 TReal aScrollAcceleration = 0.0 );

    /**
     * Gets the delay for the item expansion animation.
     *
     * @return Delay in miliseconds.
     */
    virtual TInt ItemExpansionDelay( ) const;

    /**
     * Function gets the values of scroll speed and its acceleration rate.
     *
     * @param aScrollTime Time (in miliseconds) at which the list items are
     *                    scrolled.
     *
     * @param aScrollAcceleration Rate of scroll acceleration when the up/down
     *                            key is pressed for a long time.
     */
    virtual void GetScrollTime(TInt& aScrollTime, TReal& aScrollAcceleration);

    /**
     * Expands node, causes list refresh.
     *
     * @param aNodeId Id of the node to be expanded.
     */
    virtual void ExpandNodeL( const TFsTreeItemId aNodeId );

    /**
     * Collapses node, causes list refresh.
     *
     * @param aNodeId Id of the node to be collapseded.
     */
    virtual void CollapseNodeL( const TFsTreeItemId aNodeId );

    /**
     * Expands all nodes in the list, causes list refresh.
     */
    virtual void ExpandAllL( );

    /**
     * Collapses all nodes in the list, causes list refresh.
     */
    virtual void CollapseAllL( );

    /**
     * Updates item, if visible.
     *
     * @param aItemId Id of the item to be refreshed.
     */
    virtual void UpdateItemL( const TFsTreeItemId aItemId );

    /**
     * The functions sets wether all item in the list should be always in
     * extended state or in normal state.
     * The function refreshes the list view.
     *
     * @param aAlwaysExtended ETrue if items should be always extended. EFalse
     *                        if items should be in normal state.
     */
    virtual void SetItemsAlwaysExtendedL ( TBool aAlwaysExtended );

    /*
     * The function returns if items are always in extended state or in normal
     *
     * @return ETrue if items are always in extended state, EFalse if items
     *         are in normal state.
     */
    virtual TBool IsItemsAlwaysExtended ( );

    /**
     * The function sets a text for empty list's view.
     * The function does not refresh the view automatically.
     *
     * @param aText Text to be displayed.
     */
    virtual void SetEmptyListTextL( const TDesC& aText );

    /**
     * The function returns a text for empty list's view.
     *
     * @return Text displayed when a list is empty.
     */
    virtual TDesC& GetEmptyListText( ) const;

    /**
     * Sets the background texture, clears the background color.
     *
     * @param aBgTexture Texture to be set as list background.
     */
    IMPORT_C void SetBackgroundTextureL( CAlfTexture& aBgTexture );

    /**
     * Sets the background color, clears the background texture.
     *
     * @param aBgTexture Texture to be set as list background.
     */
    IMPORT_C void SetBackgroundColorL( TRgb aColor );


    /**
     * Sets a given brush as a list background.
     *
     * @param aBrush Brush to be used as background.
     */
    IMPORT_C void SetBackgroundBrushL( CAlfBrush* aBrush );

    /**
     * The function clears list's background.
     */
    IMPORT_C void ClearBackground();

    /**
     * Sets watermark position.
     *
     * @param aPosition New position for the watermark.
     */
    IMPORT_C void SetWatermarkPos( const TPoint& aPosition );

    /**
     * Sets watermark size.
     *
     * @param aSize New size for the watermark.
     */
    IMPORT_C void SetWatermarkSize( const TSize& aSize );

    /**
     * Sets watermark opacity.
     * If new opacity is equal to 0 then watermark is fully transparent,
     * if it is equal to 1 then watermark is not transparent at all.
     *
     * @param aOpacity New opacity for the watermark.
     */
    IMPORT_C void SetWatermarkOpacity( const float aOpacity );

    /**
     * Sets watermark texture.
     * Pass NULL as a parameter to disable watermark.
     *
     * @param aTexture New texture for the watermark.
     */
    IMPORT_C void SetWatermarkL( CAlfTexture* aTexture );

    // from base class MFsTreeObserver

    /**
     *
     */
    virtual void TreeEventL( const TFsTreeEvent aEvent,
            const MFsTreeObserver::TFsTreeEventParams& aParams );

    /**
     * Sets vertical scrollbar visibility status.
     *
     * @param aSetOn Visibility status for the scrollbar
     */
    virtual void SetScrollBarVisibilityL(const TFsScrollbarVisibility aVisibility);

    /**
     * Gets vertical scrollbar visibility status.
     *
     * @return Visibility status for the scrollbar
     */
    virtual TFsScrollbarVisibility ScrollbarVisibility() const;

    /**
     * Old API to be removed. Sets vertical scrollbar visibility status.
     *
     * @param aSetOn Visibility status for the scrollbar
     */
    virtual void SetVScrollBarL( const TBool aSetOn );


    /**
     * Sets border status.
     *
     * @param aVisible Visibility status of border
     * @param aBorderTexture pointer to texture to be set for border
     */
    virtual void SetBorderL( const TBool aVisible,
        CAlfTexture* aBorderTexture = 0 );

    /**
     * Sets shadow status.
     *
     * @param aVisible Visibility status of shadow
     */
    virtual void SetShadowL( const TBool aVisible );

    /**
     * Sets visualizer observer.
     *
     * @param aObserver Sets new tree visualizer observer.
     */
    void SetVisualizerObserver( MFsTreeVisualizerObserver* aObserver );

    /**
     * Function which refreshes list display.
     */
    virtual void RefreshListViewL();

    /**
     * Hide item.
     *
     * @param aItemId Id of the item to be hidden.
     */
    virtual void HideItemL(const TFsTreeItemId aItemId);

    /**
     * Unhide item.
     *
     * @param aItemId Id of the item to be shown.
     */
    virtual void UnhideItemL(const TFsTreeItemId aItemId);

    /**
     *
     */
    virtual void SetPadding( const TAlfBoxMetric& aPadding );

    /**
     * List pane's padding taken from Layout Manager or switched off.
     */
    void EnableListPanePadding( TBool aEnable );

    /**
     * Sets the type of text marquee.
     *
     * @param aMarquee Type of marquee.
     */
    virtual void SetTextMarqueeType(const TFsTextMarqueeType aMarquee);

    /**
     * Returns the type of text marquee.
     *
     * @return Type of marquee.
     */
    virtual TFsTextMarqueeType TextMarqueeType() const;

    /**
     * Sets the speed of marquee.
     *
     * @param aPixelsPerSec How many pixels per second.
     */
    virtual void SetTextMarqueeSpeed ( const TInt aPixelsPerSec );

    /**
     * Returns the speed of marquee.
     *
     * @return How many pixels per second.
     */
    virtual TInt TextMarqueeSpeed ( ) const;

    /**
     * Sets delay for text marquee start.
     *
     * @param aStartDelay Time after which the text marquee starts (in miliseconds).
     */
    virtual void SetTextMarqueeStartDelay ( const TInt aStartDelay );

    /**
     * Returns delay for text marquee.
     *
     * @return Time after which the text marquee starts (in miliseconds).
     */
    virtual TInt TextMarqueeStartDelay () const;

    /**
     * Sets a delay for each cycle start.
     *
     * @param aCycleStartDelay Time after which the text marquee cycle starts (in miliseconds).
     */
    void SetTextMarqueeCycleStartDelay ( const TInt aCycleStartDelay );

    /**
     * Returns a delay for each cycle start.
     *
     * @return Time after which the text marquee cycle starts (in miliseconds).
     */
    TInt TextMarqueeCycleStartDelay () const;

    /**
     * Sets number of marquee cycles.
     *
     * @param aRepetitions Number of marquee cycles. Negetive values - infinite loop.
     */
    virtual void SetTextMarqueeRepetitions ( const TInt aMarqueeRepetitions );

    /**
     * Returns number of marquee cycles.
     *
     * @return Number of marquee cycles. Negetive values - infinite loop.
     */
    virtual TInt TextMarqueeRepetitions ( ) const;

    /**
      * Returns the items's target bounding rectangle in display coordinates if the item is visible.
      * Display coordinates are defined relative to the display's visible area.
      *
      * @param aItemId Item's ID.
      * @param aRect Out parameter. If successfull, contains item's rectangle.
      * @return KErrNone if successfull,
      *         KErrNotFound 1. the list does not contain item with given ID.
      *                      2. item is not visible
      */
     virtual TInt GetItemDisplayRectTarget ( const TFsTreeItemId aItemId, TAlfRealRect& aRect );

     /**
      * Internall to TreeList. Do not use directly.
      *
      * Used to block update during addition of many items/nodes. Currently only
      * scrollbar is blocked from beign updated.
      * @param aDoRefresh ETrue
      */
     virtual void SetAutoRefreshAtInsert(  TBool aAllowRefresh );

     /**
       * Internall to TreeList. Do not use directly.
       *
       * Used to block update during addition of many items/nodes. Currently only
       * scrollbar is blocked from beign updated.
       * @return Auto update of scrollbar On/off
       */
     virtual TBool IsAutoRefreshAtInsert();

     /**
       * Internall to TreeList. Do not use directly.
       *
       * Used to enable direct touch mode for the list.
       */
     void SetDirectTouchMode( const TBool aDirectTouchMode );

    /**
     * Sets whether a long tap on a node should invoke expand/collapse all
     * functionality.
     *
     * @param aExpandCollapse ETrue if long tap should expand/collapse all,
     *                        EFalse if no action should be taken.
     */
    virtual void SetExpandCollapseAllOnLongTap ( TBool aExpandCollapse );

    /*
     * The function returns if long tapping on a node results in
     * expand/collapse all.
     *
     * @return ETrue if items are always in extended state, EFalse if items
     *         are in normal state.
     */
    virtual TBool IsExpandCollapseAllOnLongTap ( );

    /**
     * Function shows selector or hides it.
     *
     * @param aVisible ETrue to make selector visible, EFalse to hide
     *                 selector.
     */
    IMPORT_C void MakeSelectorVisibleL( TBool aVisible );

    /**
     * Function shows selector or hides it.
     *
     * @aDelay for changing the opacity
     * @param aVisible ETrue to make selector visible, EFalse to hide
     *                 selector.
     */
    void MakeSelectorVisibleL( TBool aVisible, TInt aDelay );

public: //From MFsFadeEffectObserver

    /**
     * From MFsFadeEffectObserver
     * Function which will receive notifications about fade effect state changes.
     */
    virtual void FadeEffectEvent(MFsFadeEffectObserver::TFadeEffectState aState);

public: //From MFsSlideEffectObserver

    /**
     * From MFsSlideEffectObserver
     * Function which will receive notifications about slide effect state changes.
     * @param aState Current state of slide affect
     */
    virtual void SlideEffectEvent(MFsSlideEffectObserver::TSlideEffectState aState);

public: //From MAlfActionObserver

    /**
     * Called by the server when an action command is executed.
     *
     * @param aActionCommand  The command that is being executed.
     */
    virtual void HandleActionL(const TAlfActionCommand& aActionCommand);

// <cmail> Change scrollbar to avkon (to support skinning & touch)
public:

    /**
     * From MEikScrollBarObserver
     *
     * Callback method for scroll bar events
     *
     * Scroll bar observer should implement this method to get scroll bar events.
     *
     * @since S60 0.9
     * @param aScrollBar A pointer to scrollbar which created the event
     * @param aEventType The event occured on the scroll bar
     */
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

    /**
      *
      */
    void NotifyControlVisibilityChange( TBool aIsVisible );

    /**
     * Disables kinetic scrolling (Drag events will not scroll the list).
     *
     * @param aDisable ETrue if kinetic scrolling should be disabled.
     */
    void DisableKineticScrolling( TBool aDisable );

    /**
     * Returns status of kinetic scrolling.
     *
     * @return ETrue if kinetic scrolling is disabled.
     */
    TBool IsKineticScrollingDisabled() const;

private:

    /**
     * Hides off-screen list items, or clears the whole list.
     *
     * @param aRemoveAll ETrue if destroy all item visualizers, EFalse if
     *          destroy only off-screen visualizers
     */
    void ClearVisibleItemsListL( TBool aRemoveAll = ETrue,
            TBool aScrollList = ETrue );

    /**
     * Returns id of tree item if pointer event has hit the area.
     * If item cannot be located KErrNotFound will be returned.
     *
     * @param aEvent Pointer event to investigate.
     * @return TFsTreeItemId of item.
     */
    TFsTreeItemId EventItemId( const TAlfEvent& aEvent ) const;

    TFsTreeItemId VisualItemId( const CAlfVisual* aVisual ) const;

    const CAlfVisual* FindVisualUnderDrag( const TPoint& aPosition ) const;

    /**
     * Moves focus visual from one item to another.
     *
     * @param aPrevious Id of the previously focused item.
     */
    void ChangeFocusL( const TFsTreeItemId aPrevious = KFsTreeNoneID );

    /**
     * Move focus in the list.
     *
     * @param aMoveType Type of a move, down, up, page down, page up.
     */
    void MoveSelectionL(const TFsTreeVisualizerMove aMoveType);

    /**
     * Moves focus by one item in the upper direction.
     *
     * @param aChangeFocus tells whether the selector should be updated or not
     *
     * @return Height of items that were traversed
     */
    TInt MoveSelectionByLineUpL(const TBool aChangeFocus = ETrue);

    /**
     * Moves focus by one item in the lower direction.
     *
     * @param aChangeFocus tells whether the selector should be updated or not
     *
     * @return Height of items that were traversed
     */
    TInt MoveSelectionByLineDownL(const TBool aChangeFocus = ETrue);

    /**
     * Scrolls list layout.
     *
     * @param aPrevious
     * @param aAlignTop
     */
    void Scroll( const TFsTreeItemId aPrevious = KFsTreeNoneID,
                TBool aAlignTop=ETrue );

    /**
     * Returns the height of all visible items.
     *
     * @param aPrevious
     *
     * @return Height of all visible items in pixels.
     */
    TInt VisibleItemsHeight( const TFsTreeItemId aPrevious );

    /**
     * Check if item is in expanded node.
     *
     * @param aItemId
     *
     * @return Etrue if item is in expanded node, EFalse otherwise
     */
     TBool IsInExpanded( TFsTreeItemId aItemId );

     void UpdateViewPortL();

    /**
     * Update scrollbar and list layout respectively.
     *
     * @param aTimeout Timeout needed for the update.
     */
     void UpdateScrollBarL( const TInt aTimeout = 0 );
     TInt UpdateScrollBar( const TInt aTimeout = 0 );
     void UpdateScrollBarIfNeededL();

     /**
      * Finds next focusable item.
      *
      * @param aItemId Focusable item for whom next focusable should be
      * found.
      *
      * @return Focusable item id.
      */
     TFsTreeItemId NextFocusableItem( const TFsTreeItemId aItemId );

     /**
      * Finds previous focusable item.
      *
      * @param aItemId Focusable item for whom previous focusable should be
      * found.
      *
      * @return Focusable item id.
      */
     TFsTreeItemId PrevFocusableItem( const TFsTreeItemId aItemId );

     /**
      * Function sets current scroll speed based on the up/down key repeats.
      *
      * @param aKeyRepeats Number of up/down key repeats.
      */
     void SetCurrentScrollSpeed( TInt aKeyRepeats );

     /**
      * Gets default settings from Central Repository.
      */
     void ReadDefaultValuesFromCenRep();

     /**
      * Sets values to an item which are default for the whole list (e.g. if
      * items are always extended.
      *
      * @param aItemVis Item's visualizer.
      */
     void ApplyListSpecificValuesToItem (MFsTreeItemVisualizer *aItemVis);


     /**
      * The function calculates tree height up to the specified item.
      *
      * @param aItemId Id on an item up to which the tree height should be
      *                calculated.
      * @param aHeight Calculated height.
      *
      * @return KErrNone if successful. KErrNotFound if item id was invalid or
      *         item is in collapsed node.
      */
     TInt CalculateTreeHeightUpToItem( const TFsTreeItemId aItemId,
                                       TInt& aHeight ) const;

     /**
      * Update list's visual content by appending items into empty gap in list
      *
      * @param aHeight Height of the empty gap to be filled.
      */
     void FillGapWithItemsL( const TInt aHeight );

     /**
      */
     void UpdateListItemSizes( );

     /**
      */
     void UpdateListItemSize( TInt aIndex );

     /**
      * The function scrolls list layouts to the given offset in a given time.
      */
     void ScrollListLayoutsL( TInt aScrollOffset, TInt aScrollTime );
     TInt ScrollListLayouts( TInt aScrollOffset, TInt aScrollTime );

     void AdjustVisibleItemsL();

     /*
      * Appends item to the top of the list layout.
      */
     void InsertItemL( TFsTreeItemId aItemId, const TInt aSlot = 0);

     /**
      * The function deletes selector visual.
      */
     void DestroySelectorVisualL( );

     /**
      * The function creates selector visual.
      */
     void CreateSelectorVisualL( );

     /**
      */
     void UpdateSelectorVisualL(TInt aTime = 0);

     /**
     The function implements tree observer's reaction to removing a list item
     from the model. If the item was visible then it's removed from the visible
     area. If the focused item was removed then the focus is moved to the next
     item in the tree hierarchy (if the next item is not present, then the focus
     is moved to the previous one).
      */
     void RemoveItemL( TFsTreeItemId aItemId );

     /**
      The function checks whether the focused item is not outside the visible
      area and if needed scrolls the list so that selected item is fully visible.
      */
     void MakeFocusedItemFullyVisible();

     /**
      * The function assures that the list view is correct.
      * It sets focused item as fully visible.Neighbouring items are also made, if possible,
      * fully visible to fullfil the scrolling rules.
      */
     void ValidateListLayoutL( TInt aTime = 0 );

     /**
      * The function checks if the given item id matches the focused one.
      *
      * @param aItemId Item id to be checked.
      *
      * @return ETrue if the id matches the focused id && list has focus &&
      *               there is a defined focused item.
      */
     TBool IsItemFocused( TFsTreeItemId aItemId ) const;

private:
    /**
     * Tree visualization state
     */
    enum TFsTreeVisualState
        {
        /**
         *
         */
        EFsTreeVisible,
        EFsTreeHidden,
        EFsTreeFadingIn,
        EFsTreeFadingOut,
        EFsTreeSlidingIn,
        EFsTreeSlidingOut,
        EFsTreeFadingInSlidingIn,
        EFsTreeFadingOutSlidingOut
        };
    /**
     * C++ constructor
     *
     * @param aOwnerControl Owner control.
     * @param aParent Parent layout for all the new layouts constructed by
     *                this class.
     */
    CFsTreeVisualizerBase( CAlfControl* aOwnerControl, CAlfLayout& aParent, const TBool aDirectTouchMode );

    /**
     * Second phase constructor
     *
     */
    void ConstructL( );


// TREE OPTIMIZATIONS

    TInt TreeHeight() const;

    //void SetTreeHeightL( const TInt aValue );

    TBool AllNodesCollapsed() const;
    TBool AllNodesExpanded() const;

private: // from TWorld::MObserver

    virtual void WorldUpdatedL( const TWorld& aWorld );


private: // from TViewPort::MObserver

    virtual void ViewPortUpdatedL( TViewPort& aViewPort );

// TREE OPTIMIZATIONS

    /**
     * Updates world size and view position.
     */
    void UpdatePhysicsL();

    /**
     *
     */
    void SetPanningPosition(const TPoint& aDelta);

    /**
     *
     */
    void StartPhysics(TPoint& aDrag, const TTime& aStartTime);

    /**
     * Sets focused item and send event of touch focusing.
     */
    void SetFocusedItemAndSendEventL( const TFsTreeItemId aItem,
        TPointerEventType aEventType );

    /**
     * Just a wrapper for SetFocusedItemAndSendEventL which returns the
     * error code instead of leaving.
     */
    TInt SetFocusedItemAndSendEvent( const TFsTreeItemId aItem,
        TPointerEventType aEventType );

private: // from MAknPhysicsObserver

   /**
    * @see MAknPhysicsObserver::ViewPositionChanged
    */
   virtual void ViewPositionChanged( const TPoint& aNewPosition, TBool aDrawNow = ETrue, TUint aFlags = 0 );

   /**
    * @see MAknPhysicsObserver::PhysicEmulationEnded
    */
   virtual void PhysicEmulationEnded();

   /**
    * @see MAknPhysicsObserver::ViewPosition
    */
   virtual TPoint ViewPosition() const;

private: // Single click changes

   virtual TBool IsFocusShown();

private: //Data members
    /*
    * Visualization state
    */
    TFsTreeVisualState iVisualizationState;

    /*
     * A parent control for the tree visualizer.
     */
    CAlfControl* iOwnerControl;

    /*
     * A parent layout for the tree visualizer.
     */
    CAlfLayout& iParentLayout;

    /**
     * Data model.
     * Not owned.
     */
    CFsTree* iTreeData;

    /**
     *  Id of the currently selected item..
     */
    TFsTreeItemId iFocusedItem;

    /**
     * Visualizer for the empty list component. Visualizes the list when only
     * root "is visible".
     * Own.
     */
    MFsTreeNodeVisualizer* iRootVisualizer;

    /**
     * Data for the root visualizer. Used for example to store "empty" text.
     * Own.
     */
    CFsTreePlainOneLineItemData* iRootData;

    /**
     * Main layout of the component.
     */
    CAlfGridLayout* iComponentLayout;

    /**
     *
     */
    CAlfDeckLayout* iListDeck;

    /**
     *
     */
    CAlfFlowLayout* iListItemBackgroundLayout;

    /**
     *
     */
    CAlfDeckLayout* iSelectorLayout;

    /**
     * Layout in which items are placed.
     */
    CAlfFlowLayout* iListLayout;

    /**
     * Layout in which watermark is placed.
     */
    CAlfDeckLayout* iWatermarkLayout;

    /**
     * A list of currently visible items.
     */
    RArray<TFsTreeItemId> iVisibleItems;

    /**
     *
     */
    RArray<CAlfDeckLayout*> iVisibleItemsBackground;

    /**
     * Type of List's looping.
     */
    TFsTreeListLoopingType iLooping;

    /**
     * A brush holding item separator color.
     */
    CAlfBrush* iBorderBrush;

    /**
     * A table of keys which trigger page-up event.
     */
    RArray<TInt> iCustomPageUpKeys;

    /**
     * A table of keys which trigger page-down event.
     */
    RArray<TInt> iCustomPageDownKeys;

    /**
     * Scrollbar
     */
    CAlfImageVisual* iDummyScrollbar;
    CAknDoubleSpanScrollBar* iScrollBar;
    TAknDoubleSpanScrollBarModel iScrollbarModel;
    TFsScrollbarVisibility iScrollbarVisibility;

    /**
     * Image visual for the selector.
     * Own.
     */
    CAlfImageVisual* iSelectorVisual;

    /**
     * Texture for the selector.
     * Not own.
     */
    CAlfTexture* iSelectorTexture;

    /**
     * Selector's brush.
     * Not own.
     */
    CAlfBrush* iSelectorBrush;

    /**
     Selector's opacity.
     */
    TReal32 iSelectorOpacity;

    /**
     * Selector's left border width.
     */
    TInt iLeftSelectorBorderWidth;

    /**
     * Selector's right border width.
     */
    TInt iRightSelectorBorderWidth;

    /**
     * Selector's top border height.
     */
    TInt iTopSelectorBorderHeight;

    /**
     * Selector's bottom border height.
     */
    TInt iBottomSelectorBorderHeight;

    /**
     * Type of the selector's  movement.
     */
    TFsSelectorMovement iSelectorMovementType;

    /**
     * Class for controlling the fade effects
     */
    CFsFadeEffect* iFadeEffect;

    /**
     * Class for controlling the slide effects
     */
    CFsSlideEffect* iSlideEffect;

    /**
     * Root layout - for shadow
     */
    CAlfDeckLayout* iRootLayout;

    /**
     * Shadow layout, holds shadow brush when shadow is enabled.
     */
    CAlfDeckLayout* iShadowLayout;

    /**
     * Border layout, holds border brush if it is enabled..
     */
    CAlfDeckLayout* iBorderLayout;

    /**
     * Icon used to mark items.
     */
    CAlfTexture* iMarkIcon;

    /**
     * Icon indicating that item has menu.
     */
    CAlfTexture* iMenuIcon;

    /**
     * Item text marquee speed. In pixels per second.
     * Own.
     */
    TUint iMarqueeSpeed;

    /**
     * Visualizer observer
     * Not own.
     */
    MFsTreeVisualizerObserver* iVisualizerObserver;

    /**
     * Speed of scrolling.
     */
    TInt iScrollSpeed;

    /**
     * Current speed of scrolling. Can vary from iScrollSpeed by rate of
     * scroll acceleration.
     */
    TInt iCurrentScrollSpeed;

    /**
     * The rate at which scroll speed accelerates when up/down key is pressed
     * for a long time.
     */
    TReal iScrollAccelerationRate;

    /**
     * Duration of fade-in effect.
     */
    TInt iFadeInEffectTime;

    /**
     * Duration of fade-out effect.
     */
    TInt iFadeOutEffectTime;

    /**
     * Watermark used by the list.
     */
    CFsWatermark* iWatermark;

    /**
     * Hierarchy level indentation in pixel.
     */
    TInt iLevelIndentation;

    /**
     * Duration of a slide in effect.
     */
    TInt iSlideInDuration;

    /**
     * Direction of a slide in effect.
     */
    MFsTreeVisualizer::TFsSlideEffect iSlideInDirection;

    /**
     * Duration of a slide out effect.
     */
    TInt iSlideOutDuration;

    /**
     * Direction of a slide out effect.
     */
    MFsTreeVisualizer::TFsSlideEffect iSlideOutDirection;

    /**
     * Instance of text style manager.
     * Own.
     */
    CFsAlfTextStyleManager* iTextStyleManager;

    /**
     * List layout scroll offset.
     */
    TInt iScrollOffset;

    /**
     * Time interval.
     * Own.
     */
    CFsInteractionInterval* iIntx;

    /**
     * List item text marquee setting.
     */
    TFsTextMarqueeType iMarqueeType;

    /**
     * Marquee start delay in miliseconds.
     */
    TInt iMarqueStartDelay;

    /**
     * Marquee cycle start delay in miliseconds.
     */
    TInt iMarqueCycleStartDelay;

    /**
     * Number of marquee cycles.
     * Negative value - infinite loop.
     */
    TInt iMarqueeRepetitions;

    /**
     * Pointer drag handler instance (owned)
     */
    CDragHandler* iDragHandler;

    /**
     * AknPhysics instance (owned)
     */
    CAknPhysics* iPhysics;

    /**
     * Visual structure of the whole list.
     */
    TWorld iWorld;

    /*
     * List viewport.
     */
    TViewPort iViewPort;

    /**
     * Boolean flags
     */
    enum TFlag {
        /**
         * Set if the physics action is going on
         */
        EPhysicsOn,

        /**
         * Do auto refresh during addition of items.
         * Currently scrollbar refresh can be blocked.
         */
        EAutoRefresh,

        /**
         * Holds information wether items are always extended.
         */
        EItemsAlwaysExtended,

        /**
         * Page down event is being handled.
         */
        EIsPageUpDownActive,

        /**
         * If this flag is set, tree will work in direct touch mode
         */
        EDirectTouchMode,

        /**
         * The list is focused or not.
         */
        EListFocused,

        /**
         * Long tap on node invokes expand/collapse all.
         */
        EExpandCollapseOnLongTap,

        /**
         * Do physics update when simulation has finished.
         */
        EUpdatePhysicsAfterSimulationFinished
    };

    // Flags
    TBitFlagsT<TUint> iFlags;

    // ETrue when the stylus/finger is currently pressed down
    TBool iTouchPressed;

    // Keyboard flip state
    TBool iFlipOpen;

    // Focus visibility state (e.g. will be hidden after a delay)
    TBool iFocusVisible;

    /**
     * States status of kinetic scrolling.
     */
    TBool iKineticScrollingDisabled;
    };

#endif  // C_FSTREEVISUALIZER_H

