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


#ifndef M_FSTREEVISUALIZER_H
#define M_FSTREEVISUALIZER_H

#include "fstreelistconstants.h"

//////SYSTEM CLASSES
class TRgb;

//////TOOLKIT CLASSES
class TAlfEvent;

//////PROJECT CLASSES
class MFsTreeItemData;
class MFsTreeNodeVisualizer;

/**
 * MFsTreeVisualizer is an interface for the tree visualizers.
 *
 * @code
 *
 * @endcode
 *
 * @lib 
 */
class MFsTreeVisualizer
    {
public:

    /** Pre-defined slide effects directions */
    enum TFsSlideEffect
        {
        ESlideNone = 0,
        ESlideFromTop,
        ESlideFromBottom,
        ESlideFromRight,
        ESlideFromLeft
        };

public:
    
    /**
     * C++ destructor
     */
    virtual ~MFsTreeVisualizer() { };
    
public: // visualizer's interface  

    /**
     * Handles key event forwarded by the controller.
     * 
     * @param aEvent Key event to handle.
     * 
     * @return ETrue if the event was handled by the tree visualizer.
     */
    virtual TBool HandleKeyEventL(const TAlfEvent& aEvent) = 0;

    // <cmail> "Base class modifications for using touch"
    /**
     * Handles key event forwarded by the controller.
     * 
     * @param aEvent Pointer event to investigate. 
     * @return TBool for indicating if event is used.
     */
    virtual TBool HandlePointerEventL( const TAlfEvent& aEvent ) = 0;
    // </cmail>
            
    /**
     * Sets the specified item as focused.
     * 
     * @param aItem indicates item to be focused.
     * 
     * @panic EFsListPanicInvalidItemID Item with specified ID is not found.
     */
    // <cmail>
    virtual void SetFocusedItemL( TFsTreeItemId aItem, TBool aCheckFocus = ETrue ) = 0;
    // </cmail>
    
    /**
     * Returns item id of the currently focused item.
     * 
     * @return Id of the focused item.
     */
    virtual TFsTreeItemId FocusedItem () const = 0;
    
    /**
     * Checks if the specified item is focused.
     * 
     * @param aItem Item to be checked.
     * 
     * @return ETrue If item is focused. EFalse if item is not focused or 
     *         item does not exist in the tree.
     */
    virtual TBool IsFocused( const TFsTreeItemId aItem ) const = 0;        

    /**
     * Gets a pointer to the root's item data object.
     * 
     * @return Pointer to the root's item data object.
     */
    virtual MFsTreeItemData* RootItemData() = 0;
    
    /**
     * Gets a pointer to the root's item visualizer object.
     * 
     * @return Pointer to the root's item visualizer object.
     */
    virtual MFsTreeNodeVisualizer* RootNodeVisualizer() = 0;
    
    /**
     * Displays the list.
     * 
     * @param aFadeIn Defines if list visual should or shouldn't fade in to 
     *                the screen.
     * @param aSlideIn ETrue if slide in effect should be used, EFalse if not.
     */
    virtual void ShowListL( const TBool aFadeIn = EFalse, 
                            const TBool aSlideIn = EFalse ) = 0;        
    
    /**
     * Hides the list.
     *
     * @param aFadeOut Defines if list visual should or shouldn't fade out
     *                 from the screen.
     * @param aSlideOut ETrue if slide out effect should be used,EFalse if not
     */
    virtual void HideList( const TBool aFadeOut = EFalse,
                           const TBool aSlideOut = EFalse ) = 0;
    
    /**
     * The function sets duration of the slide-in effect.
     * 
     * @param aTimeMiliseconds Slide in time in milliseconds.
     */
    virtual void SetSlideInDuration ( TInt aTimeMiliseconds ) = 0;
    
    /**
     * The function returns slide in effect's duration.
     * 
     * @return Slide in duration in miliseconds.
     */
    virtual TInt SlideInDuration ( ) const = 0;
    
    /**
     * The function sets direction of the slide in effect.
     * 
     * @param aDirection A direction of the slide in effect.
     */
    virtual void SetSlideInDirection ( TFsSlideEffect aDirection ) = 0;

    /**
     * The function returns direction of the slide in effect.
     * 
     * @return A direction of the slide in effect.
     */
    virtual TFsSlideEffect SlideInDirection ( ) const = 0;

    /**
     * The function sets duration of the slide-out effect.
     * 
     * @param aTimeMiliseconds Slide out time in milliseconds.
     */
    virtual void SetSlideOutDuration ( TInt aTimeMiliseconds ) = 0;
    
    /**
     * The function returns slide out effect's duration.
     * 
     * @return Slide out duration in miliseconds.
     */
    virtual TInt SlideOutDuration ( ) const = 0;
    
    /**
     * The function sets direction of the slide out effect.
     * 
     * @param aDirection A direction of the slide out effect.
     */
    virtual void SetSlideOutDirection ( TFsSlideEffect aDirection ) = 0;

    /**
     * The function returns direction of the slide out effect.
     * 
     * @return A direction of the slide out effect.
     */
    virtual TFsSlideEffect SlideOutDirection ( ) const = 0;
    
    /**
     * Returns information about looping type of the list.
     *
     * @return Type of list looping. EFsTreeListLoopingDisabled when looping
     *         is turned off.
     */
    virtual TFsTreeListLoopingType LoopingType() const = 0;

    /**
     * Sets type of list's looping.
     *
     * @param aLoopingType type of list looping.
     */
    virtual void SetLoopingType(const TFsTreeListLoopingType aLoopingType) =0;

    /**
     * Sets the delay for the item expansion animation.
     *
     * @param aDelay Delay in miliseconds.
     */
    virtual void SetItemExpansionDelay( const TInt aDelay ) = 0;
    
    /**
     * Gets the delay for the item expansion animation.
     *
     * @return Delay in miliseconds.
     */
    virtual TInt ItemExpansionDelay( ) const = 0;
    
    /**
     * 
     */
    virtual void SetItemSeparatorColor( const TRgb& aColor ) = 0;
    
    /**
     * 
     */
    virtual void SetItemSeparatorSize( const TSize aSize ) = 0;
    
    /**
     * 
     */
    virtual void CollapseAllL() = 0;
    
    /**
     * 
     */
    virtual void ExpandAllL() = 0;
    
    /**
     * Refreshes all visible items in the list.
     */
    virtual void UpdateItemL( const TFsTreeItemId aItemId ) = 0;
    
    /**
     * Refreshes all visible items in the list.
     */
    virtual void RefreshListViewL() = 0;

    /**
     * The functions sets wether all item in the list should be always in
     * extended state or in normal state.
     * The list view is refreshed.
     * 
     * @param aAlwaysExtended ETrue if items should be always extended. EFalse
     *                        if items should be in normal state.
     */
    virtual void SetItemsAlwaysExtendedL ( TBool aAlwaysExtended ) = 0;

    /*
     * The function returns if items are always in extended state or in normal
     * 
     * @return ETrue if items are always in extended state, EFalse if items
     *         are in normal state.
     */
    virtual TBool IsItemsAlwaysExtended ( ) = 0;
    
    /**
     * The function sets a text for empty list's view.
     * 
     * @param aText Text to be displayed.
     */
    virtual void SetEmptyListTextL( const TDesC& aText ) = 0; 
    
    /**
      * Sets the type of text marquee.
      * 
      * @param aMarquee Type of marquee.
      */
    virtual void SetTextMarqueeType(const TFsTextMarqueeType aMarquee) = 0;
         
    /**
     * Gets the type of text marquee.
     * 
     * @return Type of marquee.
     */
    virtual TFsTextMarqueeType TextMarqueeType() const = 0;
 
    /**
     * Sets the speed of marquee.
     * 
     * @param aPixelsPerSec How many pixels per second.
     */
    virtual void SetTextMarqueeSpeed ( const TInt aPixelsPerSec ) = 0;
    
    /**
     * Gets the speed of marquee.
     * 
     * @return How many pixels per second.
     */
    virtual TInt TextMarqueeSpeed ( ) const = 0;
     
    /**
     * Sets delay for text marquee start.
     * 
     * @param aStartDelay Time after which the text marquee starts (in miliseconds).
     */
    virtual void SetTextMarqueeStartDelay ( const TInt aStartDelay ) = 0;
     
    /**
     * Gets delay for text marquee.
     * 
     * @return Time after which the text marquee starts (in miliseconds).
     */
    virtual TInt TextMarqueeStartDelay () const = 0;
     
    /**
     * Sets repetitions number of marquee cycle.
     * 
     * @param aRepetitions Number of marquee cycles. Negetive values - infinite loop.
     */
    virtual void SetTextMarqueeRepetitions ( const TInt aMarqueeRepetitions ) = 0;
     
    /**
     * Gets repetitions number of marquee cycle.
     * 
     * @return Number of marquee cycles. Negetive values - infinite loop.
     */
    virtual TInt TextMarqueeRepetitions ( ) const = 0;
    
    // <cmail> Change scrollbar to avkon (to support skinning & touch)
    virtual void NotifyControlVisibilityChange( TBool aIsVisible ) = 0;
    // </cmail>
    
    };
    
     
#endif // M_FSTREEVISUALIZER_H
