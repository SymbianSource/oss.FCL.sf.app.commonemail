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


#ifndef M_FSTREEITEMVISUALIZER_H
#define M_FSTREEITEMVISUALIZER_H

//////PROJECT INCLUDES
#include "fstreelistconstants.h"
#include <alf/alfgc.h>
class TRgb;
//////TOOLKIT CLASSES
class CAlfTexture;
//class TAlfAlignHorizontal;
class CAlfLayout;
class CAlfTexture;
class CAlfBrush;

//////PROJECT CLASSES
class MFsTreeItemData;
class CFsTreeList;
class CFsLayoutManager;
class CFsAlfTextStyleManager;


enum TFsTreeItemVisualizerType
    {
    EFsTreeItemVisualizerBase,
    EFsTreeNodeVisualizerBase,
    EFsTreePlainOneLineItemVisualizer,
    EFsTreePlainTwoLineItemVisualizer,
    EFsTreePlainOneLineNodeVisualizer,
    EFsTreePlainRootVisualizer,
    EFsTreeRichItemVisualizer,
    EFsMultiLineItemVisualizer,
    EFsSeparatorVisualizer,
    EFsVisualizerCount    //number of visualizers
    };


const TUint32 KFsTreeListItemMarked =           0x0001;
const TUint32 KFsTreeListItemFocused =          0x0002;
const TUint32 KFsTreeListItemDimmed =           0x0004;
const TUint32 KFsTreeListItemExtended =         0x0008;
const TUint32 KFsTreeListItemExtendable =       0x0010;
const TUint32 KFsTreeListItemHidden =           0x0020;
const TUint32 KFsTreeListNodeExpanded =         0x0040;
const TUint32 KFsTreeListItemDisplayIcons =     0x0080;
const TUint32 KFsTreeListItemFocusable =        0x0100;
const TUint32 KFsTreeListItemTextShadow =       0x0200;
const TUint32 KFsTreeListItemAlwaysExtended =   0x0400;
const TUint32 KFsTreeListItemManagedLayout =    0x0800;
const TUint32 KFsTreeListItemHasMenu =          0x1000;
const TUint32 KFsTreeListItemTextBold =         0x2000;


const TInt KFsDefaultIndentation = 20;


/**
 * MFsTreeItemVisualizer is an interface for the item visualizer.
 *
 * This is an interface for item visualization in the tree. This interface is 
 * used to change the visual appearance of the items in the list. This 
 * interface is also used to request from the item visualizer that it should 
 * draw data passed as an argument within given parent layout.
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( MFsTreeItemVisualizer )
    {
public:

    /**
     * C++ destructor
     */
    virtual ~MFsTreeItemVisualizer() {};
    
public:

    /**
     * Type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const = 0;

    /**
     * Sets multiple flags for an item at one time.
     * 
     * @param aFlags Flags for item visualizer.
     */
    virtual void SetFlags( TUint32 aFlags ) = 0;
    
    /**
     * Returns flags of the item visualizer.
     * 
     * @return Item visualizer's flags value.
     */
    virtual TUint32 Flags() = 0;

    /**
     * Sets visualized item's state to marked/unmarked.
     * 
     * @param aFlag ETrue to visualize item as marked.
     */
    virtual void SetMarked( TBool aFlag ) = 0;
    
    /**
     * Returns mark/unmark state of the item's visualization.
     * 
     * @return ETrue if item is visualized as marked.
     */
    virtual TBool IsMarked() const = 0;
    
    /**
     * Sets visualized item's state to dimmed/not dimmed.
     * 
     * @param aFlag ETrue to visualize item as dimmed.
     */
    virtual void SetDimmed( TBool aFlag ) = 0;
    
    /**
     * Returns dimmed/not dimmed state of the item's visualization.
     * 
     * @return ETrue if item is visualized as dimmed.
     */
    virtual TBool IsDimmed() const = 0;
    
    /**
     * Sets the size of the item visualization when item is not in expanded 
     * state. 
     * 
     * @param aSize Size in pixels of the an when it is not expanded.
     */
    virtual void SetSize( TSize aSize ) = 0;
    
    /**
     * Returns the size of an item in a normal (not expanded) state.
     * 
     * @return Size in pixels of an item when it is not expanded.
     */
    virtual TSize Size() const = 0;     
    
    /**
     * Sets the item's visualization state to expanded. This state is used to
     * preview more data of an item.
     * 
     * @param aFlag ETrue to visualize item as expanded.
     */
    virtual void SetExtended( TBool aFlag ) = 0;
    
    /**
     * Returns expanded/not expanded state of the item visualization.
     * 
     * @return ETrue if item is visualized as expanded.
     */
    virtual TBool IsExtended() const = 0;    
    
    /**
     * Sets if an item can be in expanded state or not.
     * 
     * @param aFlag ETrue if item can be in expanded state.
     */
    virtual void SetExtendable( TBool aFlag ) = 0;
    
    /**
     * Returns an information if item can be in expanded state.
     * 
     * @return ETrue if item can be in expanded state.
     */
    virtual TBool IsExtendable( ) = 0;
    
    /**
     * Returns an information if item can be focused.
     * 
     * @return ETrue if item can be focused.
     */
    virtual TBool IsFocusable( ) const = 0;
    
    /**
     * Used to set/unset item's ability to receive focus.
     * 
     * @param aFlag ETrue if item can be focused.
     */
    virtual void SetFocusable( const TBool aFlag ) = 0;
    
    /**
     * Sets the size of the item visualization area when item is in expanded 
     * state. 
     * 
     * @param aSize Size in pixels of the item when it is expanded.
     */
    virtual void SetExtendedSize( TSize aSize ) = 0;
    
    /**
     * Returns the size of an item in expanded state.
     * 
     * @return Size in pixels of the item when it is in expanded state.
     */
    virtual TSize ExtendedSize() const = 0;        
        
    /**
     * Sets visualized item's state to hidden/visible.
     * 
     * @param ETrue if item should be visualized as hidden.
     */
    virtual void SetHidden( TBool aFlag ) = 0;
    
    /**
     * Returns an information if the item is hidden or not.
     * 
     * @return ETrue if item is hiddden, EFalse if item is visible.
     */
    virtual TBool IsHidden() = 0;
    
    /**
     * Enables or disables text shadow.
     * 
     * @param aShadowOn Pass ETrue if shadow should be enabled 
     * EFalse otherwise.
     */
    virtual void EnableTextShadow( const TBool aShadowOn = ETrue ) = 0;
    
    /**
     * Sets text indentation.
     * 
     * @param aIndentation Indentation in pixels.
     */
    virtual void SetIndentation( const TInt aIndentation ) = 0;
    
    /**
     * Sets item background color.
     * 
     * @param aColor New background color.
     */
    virtual void SetBackgroundColor( const TRgb aColor ) = 0;
    
    /**
     * Returns background color if it's set.
     * 
     * @param aColor Out parameter. Contains background color.
     * 
     * @return ETrue if background color is set, EFalse otherwise.
     */
    virtual TBool GetBackgroundColor( TRgb& aColor ) = 0;
    
    /**
     * Sets item background texture.
     * 
     * @param aTexture New background texture.
     */
    virtual void SetBackgroundTexture( const CAlfTexture& aTexture ) = 0;

    // <cmail>
    /**
     * Sets item background brush. Ownership not transfered.
     * 
     * @param aBrush New background brush.
     */
    virtual void SetBackgroundBrush( CAlfBrush* aBrush ) = 0;
    
    /**
     * Returns item background brush.
     * 
     * @param aBrush Out parameter. Contains pointer to a background brush.
     * Ownerhship is transferred to the caller.
     *
     * @return ETrue if background brush is set, EFalse otherwise.
     */
    virtual TBool GetBackgroundBrush( CAlfBrush*& aBrush ) = 0;
    // </cmail>
    
    /**
     * Returns background texture if it's set.
     * 
     * @param aTexture Out parameter. Contains pointer to a texture.
     * 
     * @return ETrue if background texture is set, EFalse otherwise.
     */
    virtual TBool GetBackgroundTexture( const CAlfTexture*& aTexture ) = 0;
    
    /**
     * Clears item background.
     */
    virtual void ClearBackground( ) = 0;
    
    /**
     * Sets main text style for an item.
     * 
     * @param aTextStyleId Text style to be set.
     */
    virtual void SetTextStyleId( const TInt aTextStyleId ) = 0;
    
    /**
     * Returns the style of item's main text.
     */
    virtual TInt TextStyleId() const = 0;
    
    /**
     * Sets text style alignment.
     * For AH layout EHuiAlignHLeft is internally converted to EHuiAlignHRight
     * and EHuiAlignHRight is converted to EHuiAlignHLeft.
     * 
     * EAlfAlignHLocale (default setting) - alignment set according to text directionality
     * 
     * @param aAlignment Text alignment to be set.
     */
    virtual void SetTextAlign(const TAlfAlignHorizontal aAlignment) = 0;
    
    /**
     * The function sets item's text color for the focused state.
     *
     * @param aColor New color to be set.
     */
    virtual void SetFocusedStateTextColor( TRgb& aColor ) = 0;
    
    /**
     * The function returns item's text color for the focused state.
     */
    virtual TRgb FocusedStateTextColor() const = 0;
    
    /**
     * The function sets item's text color for the normal (unfocused) state.
     *
     * @param aColor New color to be set.
     */
    virtual void SetNormalStateTextColor( TRgb& aColor ) = 0;
    
    /**
     * The function returns item's text color for the normal (unfocused) state
     */
    virtual TRgb NormalStateTextColor() const = 0;
    
    /**
     * Method to set text font height in twips.
     * 
     * @param aHeightInTwips Text font height to be set.
     */
    virtual void SetFontHeight(const TInt aHeightInTwips) = 0;
    
    /**
     * Gets height of a text font in twips.
     * 
     * @return Height of the font in twips.
     */
    virtual TInt FontHeight() const = 0;
    
    /**
     * Method to set text font height in pixels.
     * 
     * @param aHeightInPixels Text font height to be set.
     */
    virtual void SetFontHeightInPixels(const TInt aHeightInPixels) = 0;
    
    /**
     * Gets height of a text font in pixels.
     * 
     * @return Height of the font in pixels.
     */
    virtual TInt FontHeightInPixels() const = 0;    

    /**
     * A call to this function means that the item is requested to draw itself
     * within specified parent layout. Data to be visualized is passed as an
     * argument. 
     * 
     * Parameter descriptions will be added when the final function prototype
     * is ready.
     */
    virtual void ShowL( CAlfLayout& aParentLayout,
                        const TUint aTimeout = 0 ) = 0;
    
    /**
     * A call to this function means that the item is requested to update its
     * visual content.
     */
    virtual void UpdateL( const MFsTreeItemData& aData,
                        TBool aFocused,
                        const TUint aLevel,
                        CAlfTexture*& aMarkIcon,
                        CAlfTexture*& aMenuIcon,
                        const TUint aTimeout = 0,
                        TBool aUpdateData = ETrue) = 0;

    /**
    * This method sets flag disable wrapping the text item.
    */
    virtual void OffWrapping() = 0;
    
    /**
     * A call to this function means that the item goes out of the visible 
     * items scope.The visualizer should destroy all its visuals to save 
     * memory.
     */
    virtual void Hide( const TInt aTimeout = 0 ) = 0;
    
    /**
     * 
     */
    virtual CAlfLayout& Layout() const = 0;

    /**
     * This method marquees the text in tree item if it is too long.
     * 
     * @param aMarqueeType            Type of marquee.
     * @param aMarqueeSpeed           Speed of marquee in pixels per second.
     * @param aMarqueeStartDelay      Marquee's start delay in miliseconds.
     * @param aMarqueeCycleStartDelay Marquee cycle's start delay.
     * @param aMarqueeRepetitions     Number of marquee's cycles.
     */
    virtual void MarqueeL( const TFsTextMarqueeType aMarqueeType,
                           const TUint aMarqueeSpeed,
                           const TInt aMarqueeStartDelay,
                           const TInt aMarqueeCycleStartDelay,
                           const TInt aMarqueeRepetitions
                           ) = 0;
    
    /**
     * Sets menu for item
     * @param aMenu CTreeList object containing menu items
     */
    virtual void SetMenu( CFsTreeList* aMenu ) = 0;
     
    /**
     * Returns CFsTreeList object contaning menu items
     * @return Menu for item.
     */
    virtual CFsTreeList* Menu() const = 0;
    
    /**
     * This functions sets wether an item should always be in extended state.
     * 
     * @param ETrue if item should always be in extended state.
     */
    virtual void SetAlwaysExtended( TBool aAlwaysExtended ) = 0;
    
    /**
     * This function returns wether an item is always in extended state.
     * 
     * @return ETrue if item is always in extended state.
     */
    virtual TBool IsAlwaysExtended( ) const = 0;
    
    /**
     * Sets the text style manager for the item visualizer.
     * 
     * @param aManager Reference to the text style manager.
     */
    virtual void SetTextStyleManager( CFsAlfTextStyleManager& aManager ) = 0;
    
    /**
     * Sets the text stroke weight for the item visualizer.
     * 
     * @param aIsBold ETrue if the text shoul be bold.
     */
    virtual void SetTextBold( const TBool aIsBold ) = 0;
    
    /**
     * Gets the text stroke weight for the item visualizer.
     * 
     * @return ETrue if the text is be bold.
     */
    virtual TBool IsTextBold( ) const = 0;
    
    };
    
    
#endif // M_FSTREEITEMVISUALIZER_H
