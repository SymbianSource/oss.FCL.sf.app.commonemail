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
* Description:  Freestyle tree list component
 *
*/


#ifndef C_FSTREELIST_H
#define C_FSTREELIST_H


/////SYSTEM INCLUDES

//////TOOLKIT INCLUDES
// <cmail> SF
#include <alf/alfcontrol.h>
// </cmail>

//////PROJECT INCLUDES
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstreelistconstants.h"
#include "fstreevisualizerbase.h"
#include "fstreevisualizerobserver.h"

//////TOOLKIT CLASSES
class CAlfEnv;
class CAlfTexture;

//////PROJECT CLASSES
class CFsTree;
class CFsTreeControl;
class MFsTreeListObserver;
class MFsTreeItemData;
class MFsTreeItemVisualizer;
class MFsTreeNodeVisualizer;
class CFsAlfTextStyleManager;

/** An array of tree list observers */
typedef RPointerArray<MFsTreeListObserver> RFsTreeListObservers;

/**
 * CFsTreeList is a component for creating and displaying hierarchical list
 * using tree data structure.
 *
 * @code
 *
 * @endcode
 *
 * @lib
 */
class CFsTreeList  : public CBase, MFsTreeVisualizerObserver
    {
public: // Construction


    /** Pre-defined types of markability in list */
    enum TFsTreeListMarkType
        {
        EFsTreeListNonMarkable,
        EFsTreeListMarkable,
        EFsTreeListMultiMarkable
        };

    /** Pre-defined types of scrolling in list
     *  OLD API - to be removed.
     * */
    enum TFsTreeListScrollbars
        {
        EFsTreeListNoScrollbars,
        EFsTreeListVerticalScrollbar
        };

    /**
     * Two-phased constructor
     *
     * @param aTreeV Pointer to the class responsible for visualizing tree
     *        structure
     * @param aEnv Reference to the UI toolkit environment
     */
    IMPORT_C static CFsTreeList* NewL( CFsTreeVisualizerBase& aTreeV,
                                       CAlfEnv& aEnv );

    /**
     * C++ destructor
     */
    IMPORT_C virtual ~CFsTreeList();

    /**
     * Adds observer to the list of observers
     *
     * @param aTreeListObserver Reference to the list observer
     */
    IMPORT_C void AddObserverL( MFsTreeListObserver& aTreeListObserver );

    /**
     * Removes observer from the list of observers
     *
     * @param aTreeListObserver         Reference to the list observer
     *
     * @panic ETreeListPanicObserverNotFound The given observer was not
     *                                  registered previously
     */
    IMPORT_C void RemoveObserver( MFsTreeListObserver& aTreeListObserver );

public: // Tree list configuration

    /**
     * Returns a hitckcock control associated with the component.
     */
    IMPORT_C CAlfControl* TreeControl( ) const;

    /**
     * The function sets duration of the slide-in effect.
     *
     * @param aTimeMiliseconds Slide in time in milliseconds.
     */
    IMPORT_C void SetSlideInDuration ( TInt aTimeMiliseconds );

    /**
     * The function returns slide in effect's duration.
     *
     * @return Slide in duration in miliseconds.
     */
    IMPORT_C TInt SlideInDuration ( ) const;

    /**
     * The function sets direction of the slide in effect.
     *
     * @param aDirection A direction of the slide in effect.
     */
    IMPORT_C void SetSlideInDirection (
            MFsTreeVisualizer::TFsSlideEffect aDirection );

    /**
     * The function returns direction of the slide in effect.
     *
     * @return A direction of the slide in effect.
     */
    IMPORT_C MFsTreeVisualizer::TFsSlideEffect SlideInDirection ( ) const;

    /**
     * The function sets duration of the slide-out effect.
     *
     * @param aTimeMiliseconds Slide out time in milliseconds.
     */
    IMPORT_C void SetSlideOutDuration ( TInt aTimeMiliseconds );

    /**
     * The function returns slide out effect's duration.
     *
     * @return Slide out duration in miliseconds.
     */
    IMPORT_C TInt SlideOutDuration ( ) const;

    /**
     * The function sets direction of the slide out effect.
     *
     * @param aDirection A direction of the slide out effect.
     */
    IMPORT_C void SetSlideOutDirection (
            MFsTreeVisualizer::TFsSlideEffect aDirection );

    /**
     * The function returns direction of the slide out effect.
     *
     * @return A direction of the slide out effect.
     */
    IMPORT_C MFsTreeVisualizer::TFsSlideEffect SlideOutDirection ( ) const;

    /**
     * Returns information about looping type of the list.
     *
     * @return Type of list looping. EFsTreeListLoopingDisabled when looping
     *         is turned off.
     */
    IMPORT_C TFsTreeListLoopingType LoopingType() const;

    /**
     * Returns information whether list is markable, non-markable or
     * multi-markable
     *
     * @return Returns markability of the list
     */
    IMPORT_C TFsTreeListMarkType MarkType() const;

    /**
     * OLD API - to be removed.
     * Returns information about scrollbars used by list visual
     *
     * @return Returns a type of scrollbars that the list uses
     */
    IMPORT_C TFsTreeListScrollbars HasScrollBars() const;

    /**
     * Old API - to be removed
     *
     * @param aFlag Scrollbar type for the list
     */
    IMPORT_C void SetScrollBarsL( const TFsTreeListScrollbars aFlag );


    /**
     * Returns information about scrollbars used by list visual
     *
     * @return Returns a type of scrollbars that the list uses
     */
    IMPORT_C TFsScrollbarVisibility ScrollbarVisibility() const;

    /**
     * Enables or disables scrollbars
     *
     * @param aFlag Scrollbar type for the list
     */
    IMPORT_C void SetScrollbarVisibilityL(const TFsScrollbarVisibility aVisibility);

    /**
     * Sets type of list's looping.
     *
     * @param aLoopingType type of list looping.
     */
    IMPORT_C void SetLoopingType( const TFsTreeListLoopingType aLoopingType );


    /**
     * The functions sets fade-in's effect duration.
     * @param aFadeTime A time in miliseconds for fade in effect.
     */
    IMPORT_C void SetFadeInEffectTime( TInt aFadeTime );

    /**
     * The functions returns fade-in's effect duration.
     * @return A time in miliseconds for fade in effect.
     */
    IMPORT_C TInt FadeInEffectTime( );

    /**
     * The functions sets fade-out's effect duration.
     * @param aFadeTime A time in miliseconds for fade-out effect.
     */
    IMPORT_C void SetFadeOutEffectTime( TInt aFadeTime );

    /**
     * The functions returns fade-out's effect duration.
     * @return A time in miliseconds for fade-out effect.
     */
    IMPORT_C TInt FadeOutEffectTime( );

    /**
     * Function sets the time of list scroll per item and the scroll
     * acceleration rate.
     * @param aScrollTime Time (in miliseconds) at which the list items are
     *                    scrolled.
     * @param aScrollAcceleration Rate of scroll acceleration when the up/down
     *                            key is pressed for a long time. This is
     *                            multiplied by the number of up/down key
     *                            repeats. Acceleration rate should be in
     *                            0.0-1.0 range.
     */
    IMPORT_C void SetScrollTime ( TInt aScrollTime,
                                  TReal aScrollAcceleration = 0.0 );

    /**
     * Function gets the values of scroll speed and its acceleration rate.
     *
     * @param aScrollTime Time (in miliseconds) at which the list items are
     *                    scrolled.
     *
     * @param aScrollAcceleration Rate of scroll acceleration when the up/down
     *                            key is pressed for a long time.
     */
    IMPORT_C void GetScrollTime( TInt& aScrollTime,
                                 TReal& aScrollAcceleration );

    /**
     * Enables or disables marking of list items
     *
     * @param aFlag Type of markability for the list
     */
    IMPORT_C void SetMarkTypeL( const TFsTreeListMarkType aFlag );

    /**
     * Returns information whether list enables indention or not
     *
     * @return ETrue if indention is enabled, EFalse otherwise
     */
    IMPORT_C TInt Indentation() const;

    /**
     * Enables or disables indention for list
     *
     * @param aIndentation Indentation in pixels.
     */
    IMPORT_C void SetIndentationL( const TInt aIndentation );

    /**
     * The functions sets wether all item in the list should be always in
     * extended state or in normal state.
     * The list view is refreshed.
     *
     * @param aAlwaysExtended ETrue if items should be always extended. EFalse
     *                        if items should be in normal state.
     */
    IMPORT_C void SetItemsAlwaysExtendedL ( TBool aAlwaysExtended );

    /*
     * The function returns if items are always in extended state or in
     * normal.
     *
     * @return ETrue if items are always in extended state, EFalse if items
     *         are in normal state.
     */
    IMPORT_C TBool IsItemsAlwaysExtended ( );

public: // Tree management

    /**
     * Inserts new item as a child of parent given by Id with the given
     * position
     *
     * @param aItemD Pointer to item's data
     * @param aItemV Pointer to item's visualizer
     * @param aParentId Id of parent node
     * @param aIndex Position of new item in the list of parent's children (if
     *               omitted, the item is inserted as last)
     * @param aAllowRefresh Blocks/Allows view update after an item has been added
     *               to the tree list. Currently works only with scrollbar update.
     *
     * @return Id of the inserted item
     *
     * @panic EFsListPanicParentIdNotFound The specified parent id was not
     *                          found
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                    number of children
     */
    IMPORT_C TFsTreeItemId InsertItemL( MFsTreeItemData& aItemD,
                                        MFsTreeItemVisualizer& aItemV,
                                        const TFsTreeItemId aParentId,
                                        const TInt aIndex = -1,
                                        const TBool aAllowRefresh = ETrue);

    /**
     * Inserts new node as a child of parent given by Id with the given
     * position
     *
     * @param aItemD Pointer to node's data
     * @param aItemV Pointer to node's visualizer
     * @param aParentId Id of parent node
     * @param aIndex Position of new node in the list of parent's children (if
     *               omitted, the node is inserted as last)
     *
     * @return Id of the inserted node
     *
     * @panic EFsListPanicParentIdNotFound The specified parent id was not
     *                          found
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                    number of children
     */
    IMPORT_C TFsTreeItemId InsertNodeL( MFsTreeItemData& aItemD,
                                        MFsTreeNodeVisualizer& aNodeV,
                                        const TFsTreeItemId aParentId,
                                        const TInt aIndex = -1,
                                        const TBool aAllowRefresh = ETrue );

    /**
     * Removes the item with given Id from the list. If aItemId is a node all
     * its children will be removed as well.
     *
     * @param aItemId Id of the item to be removed
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found in
     *                                  the list
     */
    IMPORT_C void RemoveL( const TFsTreeItemId aItemId );

    /**
     * Removes all items from the list.
     */
    IMPORT_C void RemoveAllL( );

    /**
     * Removes all children of the given node.
     *
     * @param aNodeId Id of the node
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found in
     *                                  the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                                  node id
     */
    IMPORT_C void RemoveChildrenL( const TFsTreeItemId aNodeId );

    /**
     * Moves the item with given Id to the list of children of parent with
     * given id
     *
     * @param aItemId Id of the item to be moved
     * @param aTargetNode Id of the new parent node
     * @param aIndex Position of the item in the list of new parent's children
     *               (if omitted, the item/node is inserted as last)
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found in
     *                                  the list
     * @panic EFsListPanicParentIdNotFound The specified parent id was not
     *                                  found
     * @panic EFsListPanicInvalidItemType Item id was passed as a second param
     *                                    instead of the node id
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                    number of children
     */
    IMPORT_C void MoveItemL( const TFsTreeItemId aItemId,
            const TFsTreeItemId aTargetNode, const TInt aIndex = -1 );

    /**
     * Checks if a node has children, if parameter is omitted, root of the
     * list is checked to indicate if the list is empty
     *
     * @param aNodeId Id of a node
     *
     * @return Etrue if a node has children, EFalse if it does not
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found in
     *                                  the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                          node id
     */
    IMPORT_C TBool IsEmpty(const TFsTreeItemId aNodeId = KFsTreeRootID) const;

    /**
     * Returns the level of an item in tree.
     *
     * @param aItemId Id of an item.
     *
     * @return Level of an item.
     */
    IMPORT_C TUint Level( const TFsTreeItemId aItemId ) const;

    /**
     * Counts elements in the list
     *
     * @return Number of list items
     */
    IMPORT_C TUint Count() const;

    /**
     * Gets the number of children of the given node
     *
     * @param aNodeId Id of the node
     *
     * @return Number of node's children
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found in
     *                                  the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                      node id
     */
    IMPORT_C TUint CountChildren( const TFsTreeItemId aNodeId ) const;

    /**
     * Gets the id of a child of the specified node with the position given
     *
     * @param aNodeId Id of a node
     * @param aIndex Index of the child
     *
     * @return Id of a child
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found in
     *                                  the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of the
     *                      node id
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                    number of children
     */
    IMPORT_C TFsTreeItemId Child( const TFsTreeItemId aNodeId,
            const TUint aIndex ) const;

    /**
     * Returns the index of a child for a given parent
     *
     * @param aNodeId Id of a node
     * @param aItemId Id of a child
     *
     * @return Index of a child
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of
     *                                  the node id
     * @panic EFsListPanicIndexOutOfRange Specified index exceeds node's
     *                                  number of children
     */
    IMPORT_C TUint ChildIndex( const TFsTreeItemId aNodeId,
            const TFsTreeItemId aItemId ) const;

    /**
     * Gets the parent's id for the specified item. If KFsTreeRootID is passed
     * then KFsTreeNoneID  is returned.
     *
     * @param aItemId Id of an item
     *
     * @return Id of item's parent
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     */
    IMPORT_C TFsTreeItemId Parent( const TFsTreeItemId aItemId ) const;

    /**
     * Checks whether the item with specified id belongs to the list
     *
     * @param aItemId Id of an item
     *
     * @return ETrue if the item is in the list, EFalse otherwise
     */
    IMPORT_C TBool Contains( const TFsTreeItemId aItemId ) const;

    /**
     * Checks whether the item with specified id is a node
     *
     * @param aItemId Id of an item
     *
     * @return ETrue if the item is a node, EFalse otherwise
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     */
    IMPORT_C TBool IsNode( const TFsTreeItemId aItemId ) const;

    /**
    * Sets focus state of list component
    * @param aFocused status of the focus to be set
    */
    IMPORT_C void SetFocusedL( const TBool aFocused );

    /**
    * Returns state of the focus
    * @return State of the focus
    */
    IMPORT_C TBool IsFocused() const;

    /**
    * Returns item visualizer for given item id.
    */
    IMPORT_C CFsTreeList* ItemMenu( TFsTreeItemId aItemId );

public: // Visualization management

    /**
     * Displays the list.
     *
     * @param aFadeIn Defines if list visual should or shouldn't fade in to
     *                the screen.
     * @param aSlideIn ETrue if slide in effect should be used, EFalse if not.
     */
    IMPORT_C void ShowListL( const TBool aFadeIn = EFalse,
                             const TBool aSlideIn = EFalse );


    /**
     * Sets direct touch mode.
     *
     * @param aDirectTouchMode  ETrue if list should work as in direct touch, EFalse
     *                          otherwise.
     */
    IMPORT_C void SetDirectTouchMode( const TBool aDirectTouchMode );

    /**
     * Hides the list.
     *
     * @param aFadeOut Defines if list visual should or shouldn't fade out
     *                 from the screen.
     * @param aSlideOut ETrue if slide out effect should be used,EFalse if not
     */
    IMPORT_C void HideListL( const TBool aFadeOut = EFalse,
                            const TBool aSlideOut = EFalse );

    /**
     * Gets the currently focused (highlighted) item id
     *
     * @return Id of the item being focused (highlighted)
     */
    IMPORT_C TFsTreeItemId FocusedItem() const;

    /**
     * Sets the focus (highlight) upon the item with given id
     *
     * @param aItemId Id of the item that is to be focused (highlighted)
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     */
    IMPORT_C void SetFocusedItemL( const TFsTreeItemId aItemId );

    /**
     * Checks whether the item with specified id is marked.
     *
     * @param aItemId Id of an item.
     *
     * @return ETrue if the item is marked, EFalse otherwise.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list.
     */
    IMPORT_C TBool IsMarked( const TFsTreeItemId aItemId ) const;

    /**
     * Checks whether the node with specified id is expanded.
     *
     * @param aNodeId Id of a node
     *
     * @return ETrue if the node is expanded, EFalse otherwise
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of
     *                                  the node id
     */
    IMPORT_C TBool IsExpanded( const TFsTreeItemId aNodeId );

    /**
     * Marks the item.
     *
     * @param aItemId Id of an item
     *
     * @param aMarked ETrue to set item marked, EFalse to unmarked.
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     * @panic EFsListPanicIsNonMarkable The list is non-markable
     */
    IMPORT_C void MarkItemL( const TFsTreeItemId aItemId, TBool aMarked );

    /**
     * Expands the node. Does nothing when the node is already expanded.
     *
     * @param aNodeId Id of a node
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of
     *                                  the node id
     */
    IMPORT_C void ExpandNodeL( const TFsTreeItemId aNodeId );

    /**
     * Collapses the node. Does nothing when the node is already collapsed.
     *
     * @param aNodeId Id of a node
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     * @panic EFsListPanicInvalidItemType Item id was passed instead of
     *                                  the node id
     */
    IMPORT_C void CollapseNodeL( const TFsTreeItemId aNodeId );

    /**
     * Gets the list of marked items
     *
     * @param aMarkedItems Reference to the array that the list will copy
     *                     array of marked items' ids to
     */
    IMPORT_C void GetMarkedItemsL( RFsTreeItemIdList& aMarkedItems ) const;

    /**
     * Gets the reference to the item/node data (const)
     *
     * @param aItemId Id of the item/node
     *
     * @return Reference to the item/node data (const)
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     */
    IMPORT_C MFsTreeItemData& ItemData(const TFsTreeItemId aItemId) const;

    /**
     * Sets the reference to the item/node data
     *
     * @param aData Reference to the item/node data
     * @param aItemId Id of the item/node
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     */
    IMPORT_C void SetItemDataL( MFsTreeItemData& aData,
            const TFsTreeItemId aItemId );

    /**
     * Gets the reference to the item visualizer (const)
     *
     * @param aItemId Id of the item
     *
     * @return Reference to the item visualizer (const)
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     */
    IMPORT_C MFsTreeItemVisualizer& ItemVisualizer(
            const TFsTreeItemId aItemId ) const;

    /**
     * Sets the reference to the item visualizer
     *
     * @param aVisualizer Reference to the new item's visualizer
     * @param aItemId Id of the item
     *
     * @panic EFsListPanicInvalidItemId The specified item id was not found
     *                                  in the list
     */
    IMPORT_C void SetItemVisualizerL( MFsTreeItemVisualizer& aVisualizer,
            const TFsTreeItemId aItemId );

    /**
     * Gets the reference to the tree visualizer
     *
     * @return Reference to the tree visualizer currently used by the tree
     *         list.
     */
    IMPORT_C CFsTreeVisualizerBase& TreeVisualizer() const;

    /**
     * Sets a tree visualizer for the list
     *
     * @param aTreeV Reference to the tree visualizer that the tree list will
     *               be using
     */
    IMPORT_C void SetTreeVisualizer( CFsTreeVisualizerBase& aTreeV );

    /**
     * Sets visiblity of the border
     */
     IMPORT_C virtual void SetBorderL( const TBool aVisible,
         CAlfTexture* aBorderTexture = 0 );

     /**
     * Sets visiblity of the shadow
     */
     IMPORT_C void SetShadowL( const TBool aVisible );

     /**
      * Returns the items's target bounding rectangle in display coordinates if the item is visible.
      * Display coordinates are defined relative to the display's visible area.
      *
      * @param aItemId Item's ID.
      * @param aRect Out parameter. If successfull, contains item's target rectangle.
      * @return KErrNone if successfull,
      *         KErrNotFound 1. the list does not contain item with given ID.
      *                      2. item is not visible
      */
     IMPORT_C TInt GetItemDisplayRectTarget ( const TFsTreeItemId aItemId, TAlfRealRect& aRect );

     //From MFsTreeVisualizerObserver

     /**
      *
      */
     // <cmail> Touch
     IMPORT_C virtual void TreeVisualizerEventL(
             const TFsTreeVisualizerEvent aEvent,
             const TFsTreeItemId aId = KFsTreeNoneID ) const;
     // </cmail>

    /**
     * Gets the reference to the text style manager.
     *
     * @return Reference to the text style manager.
     */
     IMPORT_C CFsAlfTextStyleManager& TextStyleManager() const;

    /**
      * Sets the type of text marquee.
      *
      * @param aMarquee Type of marquee.
      */
    IMPORT_C void SetTextMarqueeType(const TFsTextMarqueeType aMarquee);

    /**
     * Returns the type of text marquee.
     *
     * @return Type of marquee.
     */
    IMPORT_C TFsTextMarqueeType TextMarqueeType() const;

    /**
     * Sets the speed of marquee.
     *
     * @param aPixelsPerSec How many pixels per second.
     */
    IMPORT_C void SetTextMarqueeSpeed ( const TInt aPixelsPerSec );

    /**
      * Returns the speed of marquee.
      *
      * @return How many pixels per second.
      */
    IMPORT_C TInt TextMarqueeSpeed ( ) const;

    /**
      * Sets delay for text marquee start.
      *
      * @param aStartDelay Time after which the text marquee starts (in miliseconds).
      */
    IMPORT_C void SetTextMarqueeStartDelay ( const TInt aStartDelay );

     /**
      * Returns delay for text marquee.
      *
      * @return Time after which the text marquee starts (in miliseconds).
      */
    IMPORT_C TInt TextMarqueeStartDelay () const;

    /**
     * Sets a delay for each cycle start.
     *
     * @param aCycleStartDelay Time after which the text marquee cycle starts (in miliseconds).
     */
    IMPORT_C void SetTextMarqueeCycleStartDelay ( const TInt aCycleStartDelay );

    /**
     * Returns a delay for each cycle start.
     *
     * @return Time after which the text marquee cycle starts (in miliseconds).
     */
    IMPORT_C TInt TextMarqueeCycleStartDelay () const;

     /**
      * Sets repetitions number of marquee cycle.
      *
      * @param aRepetitions Number of marquee cycles. Negetive values - infinite loop.
      */
    IMPORT_C void SetTextMarqueeRepetitions ( const TInt aMarqueeRepetitions );

    /**
      * Returns repetitions number of marquee cycle.
      *
      * @return Number of marquee cycles. Negetive values - infinite loop.
      */
    IMPORT_C TInt TextMarqueeRepetitions ( ) const;

    /**
     * Disables kinetic scrolling (Drag events will not scroll the list).
     *
     * @param aDisable ETrue if kinetic scrolling should be disabled.
     */
    IMPORT_C void DisableKineticScrolling( TBool aDisable );

protected: //Construction

    /**
     * C++ constructor
     */
    IMPORT_C CFsTreeList( CAlfEnv& aEnv, CFsTreeVisualizerBase& aTreeV );

    /**
     * Second phase constructor
     *
     * @param aEnv Reference to the UI toolkit environment
     */
    IMPORT_C void ConstructL( CAlfEnv& aEnv );

protected: //Data members

    /**
     * Alf env
     */
    CAlfEnv* iEnv;

    /**
     * Visualizer class
     * Not own
     */
    CFsTreeVisualizerBase* iTreeVisualizer;

    /**
     * Tree data structure clas
     * Own
     */
    CFsTree* iTree;

    /**
     * Controller class for the list
     * Own
     */
    CFsTreeControl* iTreeControl;

    /**
     * Loop flag
     */
    TBool iIsLooping;

    /**
     * part of OLD API - to be removed.
     * SScrollbar flag
     */
    TFsTreeListScrollbars iHasScrollBars;


    /**
     * Id of the focused item
     */
    TFsTreeItemId iFocusedItem;

    /**
     * Type of markability
     */
    TFsTreeListMarkType iMarkType;

    /**
     * Array of observers
     */
    RFsTreeListObservers iObservers;

    /**
     * Text style manager.
     * Own
     */
    CFsAlfTextStyleManager* iTextStyleManager;

    /**
     * The list has focus or not.
     */
    TBool iListFocused;

    };

#endif // C_FSTREELIST_H

