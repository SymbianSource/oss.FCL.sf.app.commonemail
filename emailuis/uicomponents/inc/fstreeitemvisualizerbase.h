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
* Description:  Base class of item visualizer.
*
*/


#ifndef FSTREEITEMVISUALIZERBASE_H_
#define FSTREEITEMVISUALIZERBASE_H_

#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include <alf/alfeventhandler.h>
// </cmail>
#include "fstreeitemvisualizer.h"
#include "fslayoutmanager.h"

class TRgb;

class CAlfLayout;
class CAlfTexture;
class CAlfBrush;
class CAlfAnchorLayout;
class CAlfControl;
class CFsAlfTextStyleManager;
class CAlfTextVisual;


/**
 * CFsTreeItemVisualizerBase is a base class implementing methods common for
 * all tree items.
 *
 * Thic class is a concrete implementation for the MFsTreeItemVisualizer 
 * interface.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS(CFsTreeItemVisualizerBase) : public CBase,
                                               public MFsTreeItemVisualizer,
                                               public MAlfEventHandler
    {
    
public:
    
    /** 
     * C++ destructor
     */
     virtual ~CFsTreeItemVisualizerBase();


public:

    /**
     * Returns the type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const;

    /**
     * Sets horizontal alignment of the displayed plain text.
     * 
     * For AH layout EAlfAlignHLeft is internally converted to EAlfAlignHRight
     * and EAlfAlignHRight is converted to EAlfAlignHLeft.
     * 
     * EAlfAlignHLocale (default setting) - alignment set according to text directionality
     * 
     * @param aAlign Horizontal text alignment type.
     */
    virtual void SetTextAlign( const TAlfAlignHorizontal aAlign );
        
    /**
     * The function sets item's text color for the focused state.
     *
     * @param aColor New color to be set.
     */
    virtual void SetFocusedStateTextColor( const TRgb& aColor );
    
    /**
     * The function returns item's text color for the focused state.
     */
    virtual TRgb FocusedStateTextColor() const;
    
    /**
     * The function sets item's text color for the normal (unfocused) state.
     *
     * @param aColor New color to be set.
     */
    virtual void SetNormalStateTextColor( const TRgb& aColor );
    
    /**
     * The function returns item's text color for the normal (unfocused) state
     */
    virtual TRgb NormalStateTextColor() const;
        
    /**
     * The function sets the style of item's text.
     * 
     * @param aTextStyleId Text style ID provided by the text style manager.
     */
    virtual void SetTextStyleId ( const TInt aTextStyleId );
    
    /**
     * The function returns the style of item's text.
     * 
     * @return Text style ID provided by the text style manager.
     */
    virtual TInt TextStyleId () const;
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets multiple flags for an item at one time.
     * 
     * @param aFlags Flags for item visualizer.
     */
    virtual void SetFlags( TUint32 aFlags );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns flags of the item visualizer.
     * 
     * @return Item visualizer's flags value.
     */
    virtual TUint32 Flags();
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets visualized item's state to marked/unmarked.
     * 
     * @param aFlag ETrue to visualize item as marked.
     */
    virtual void SetMarked( TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns mark/unmark state of the item's visualization.
     * 
     * @return ETrue if item is visualized as marked.
     */
    virtual TBool IsMarked() const;
     
    /**
     * From MFsTreeItemVisualizer.
     * Sets visualized item's state to dimmed/not dimmed.
     * 
     * @param aFlag ETrue to visualize item as dimmed.
     */
    virtual void SetDimmed( TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns dimmed/not dimmed state of the item's visualization.
     * 
     * @return ETrue if item is visualized as dimmed.
     */
    virtual TBool IsDimmed() const;
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets the size of the item visualization when item is not in expanded 
     * state. 
     * 
     * @param aSize Size in pixels of the an when it is not expanded.
     */
    virtual void SetSize( const TSize aSize );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns the size of an item in a normal (not extended) state.
     * 
     * @return Size in pixels of an item when it is not extended.
     */
    virtual TSize Size() const;     
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets the item's visualization state to expanded. This state is used to
     * preview more data of an item.
     * 
     * @param aFlag ETrue to visualize item as expanded.
     */
    virtual void SetExtended( const TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns expanded/not expanded state of the item visualization.
     * 
     * @return ETrue if item is visualized as expanded.
     */
    virtual TBool IsExtended() const;    
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets if an item can be in expanded state or not.
     * 
     * @param aFlag ETrue if item can be in expanded state.
     */
    virtual void SetExtendable( TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns an information if item can be in expanded state.
     * 
     * @return ETrue if item can be in expanded state.
     */
    virtual TBool IsExtendable( );
    
    /**
     * From MFsTreeItemVisualizer
     * Returns an information if item can be focused.
     * 
     * @return ETrue if item can be focused.
     */
    virtual TBool IsFocusable( ) const;
    
    /**
     * From MFsTreeItemVisualizer
     * Used to set/unset item's ability to receive focus.
     * 
     * @param aFlag ETrue if item can be focused.
     */
    virtual void SetFocusable( const TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets the size of the item visualization area when item is in expanded 
     * state. 
     * 
     * @param aSize Size in pixels of the item when it is expanded.
     */
    virtual void SetExtendedSize( const TSize aSize );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns the size of an item in expanded state.
     * 
     * @return Size in pixels of the item when it is in expanded state.
     */
    virtual TSize ExtendedSize() const;
        
    /**
     * From MFsTreeItemVisualizer.
     * Sets visualized item's state to hidden/visible.
     * 
     * @param ETrue if item should be visualized as hidden.
     */
    virtual void SetHidden( TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns an information if the item is hidden or not.
     * 
     * @return ETrue if item is hiddden, EFalse if item is visible.
     */
    virtual TBool IsHidden() ;
    
    /**
     * From MFsTreeItemVisualizer.
     * Enables or disables text shadow.
     * 
     * @param aShadowOn Pass ETrue if shadow should be enabled 
     * EFalse otherwise.
     */
    virtual void EnableTextShadow( const TBool aShadowOn = ETrue );
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets text indentation.
     *
     * @param aIndentation Indentation in pixels.
     */
    virtual void SetIndentation( const TInt aIndentation = 0 );

    /**
     * From MFsTreeItemVisualizer.
     * Sets item background color.
     * 
     * @param aColor New background color.
     */
    virtual void SetBackgroundColor( const TRgb aColor );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns background color if it's set.
     * 
     * @param aColor Out parameter. Contains background color.
     * 
     * @return ETrue if background color is set, EFalse otherwise.
     */
    virtual TBool GetBackgroundColor( TRgb& aColor );
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets item background texture.
     * 
     * @param aTexture New background texture.
     */
    virtual void SetBackgroundTexture( const CAlfTexture& aTexture );

    // <cmail>
    /**
     * From MFsTreeItemVisualizer.
     * Sets item background brush. Ownership not transfered.
     * 
     * @param aBrush New background brush.
     */
    virtual void SetBackgroundBrush( CAlfBrush* aBrush );

    /**
     * From MFsTreeItemVisualizer.
     * Returns item background brush.
     * 
     * @param aBrush Out parameter. Contains pointer to a background brush.
     *
     * @return ETrue if background brush is set, EFalse otherwise.
     */
    virtual TBool GetBackgroundBrush( CAlfBrush*& aBrush );
    // </cmail>

    /**
     * From MFsTreeItemVisualizer.
     * Returns background texture if it's set.
     * 
     * @param aTexture Out parameter. Contains pointer to a texture.
     * 
     * @return ETrue if background texture is set, EFalse otherwise.
     */
    virtual TBool GetBackgroundTexture( const CAlfTexture*& aTexture );
    
    /**
     * From MFsTreeItemVisualizer.
     * Clears item background.
     */
    virtual void ClearBackground( );
    
    /**
     * Sets menu for item
     * @param aMenu CTreeList object containing menu items
     */
    virtual void SetMenu( CFsTreeList* aMenu );
     
    /**
     * Returns CFsTreeList object contaning menu items
     * @return Menu for item.
     */
    virtual CFsTreeList* Menu() const;
    
    /**
     * This functions sets wether an item should always be in extended state.
     * 
     * @param ETrue if item should always be in extended state.
     */
    virtual void SetAlwaysExtended( TBool aAlwaysExtended );
    
    /**
     * This function returns wether an item is always in extended state.
     * 
     * @return ETrue if item is always in extended state.
     */
    virtual TBool IsAlwaysExtended( ) const;
    
    /**
     * Sets the text style manager for the item visualizer.
     * 
     * @param aManager Reference to the text style manager.
     */
    virtual void SetTextStyleManager( CFsAlfTextStyleManager& aManager );
    
    /**
     * Sets the text stroke weight for the item visualizer.
     * 
     * @param aIsBold ETrue if the text shoul be bold.
     */
    virtual void SetTextBold( const TBool aIsBold );
    
    /**
     * Method to set text font height in twips.
     * 
     * @param aHeightInTwips Text font height to be set.
     */
    virtual void SetFontHeight(const TInt aHeightInTwips);
    
    /**
     * Gets height of a text font in twips.
     * 
     * @return Height of the font in twips
     */
    virtual TInt FontHeight() const;
    
    /**
     * Method to set text font height in pixels.
     * 
     * @param aHeightInPixels Text font height to be set.
     */
    virtual void SetFontHeightInPixels(const TInt aHeightInPixels);
    
    /**
     * Gets height of a text font in pixels.
     * 
     * @return Height of the font in pixels.
     */
    virtual TInt FontHeightInPixels() const;
    
    /**
     * Gets the text stroke weight for the item visualizer.
     * 
     * @return ETrue if the text is be bold.
     */
    virtual TBool IsTextBold( ) const;
    
    /**
     * 
     */
    virtual CAlfLayout& Layout() const;

    /**
     * 
     */
    virtual void ShowL( CAlfLayout& aParentLayout,
            const TUint aTimeout = 0 );

    /**
     * 
     */
    virtual void UpdateL( const MFsTreeItemData& aData,
            TBool aFocused,
            const TUint aLevel,
            CAlfTexture*& aMarkIcon,
            CAlfTexture*& aMenuIcon,
            const TUint aTimeout = 0,
            TBool aUpdateData = ETrue);

    /**
     * 
     */
    virtual void Hide( const TInt aTimeout = 0 );

    /**
     * From MFsTreeItemVisualizer.
     * This method marquees the text in tree item if it is too long.
     * 
     * @param aMarqueeType Type of marquee.
     * @param aMarqueeSpeed Speed of marquee in pixels per second.
     * @param aMarqueeStartDelay Marquee's start delay in miliseconds.
     * @param aMarqueeCycleStartDelay Marquee cycle's start delay.
     * @param aMarqueeRepetitions Number of marquee's cycles.
     */
    virtual void MarqueeL(const TFsTextMarqueeType aMarqueeType,
                          const TUint aMarqueeSpeed,
                          const TInt aMarqueeStartDelay,
                          const TInt aMarqueeCycleStartDelay,
                          const TInt aMarqueeRepetitions);
    
    /**
     * From MAlfEventHandler.
     * Called when an input event is being offered to this object.
     */
    virtual TBool OfferEventL(const TAlfEvent& aEvent);
    
public:
    
    /*
     * Layouting hints for the visualizers. Hints are binary flags so they can be mixed.
     */
    enum TLayoutHint
        {
        ENormalLayout = 0,
        // Use folder layout for item visualizers
        EFolderLayout = (1<<0),
        // Item visualizers are in popup
        EPopupLayout = (1<<1)
        };    
    
public:

    /**
     * Set layout hint(s). Given hints are ORed so previously set flags will remain.
     */
    virtual void SetLayoutHints( const TUint aHints );
    
    /**
     * Check if given layout hint is set.
     */
    virtual TBool IsLayoutHintSet( const TLayoutHint aHint ) const;
       
protected:

    /**
     * C++ constructor
     */
    CFsTreeItemVisualizerBase(CAlfControl& aOwnerControl);
    
    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
protected:

    /**
     * Function gets text color for a focused item. The color is taken from 
     * the skin if skin support is on, if not then a default value is used.
     * 
     * @return aTextColor Text's color.
     */
    virtual TRgb FocusedStateTextSkinColor( );
    
    /**
     * Function gets text color for a non focused item. The color is taken 
     * from the skin if skin support is on, if not then a default value is 
     * used.
     * 
     * @return aTextColor Text's color.
     */
    virtual TRgb NormalStateTextSkinColor( );
    
    /**
     * Returns horizontal alignment of the displayed text in respect to
     * current layout.
     * 
     * For AH layout EAlfAlignHLeft is internally converted to EAlfAlignHRight
     * and EAlfAlignHRight is converted to EAlfAlignHLeft.
     * 
     * @param aAlign Horizontal text alignment type.
     */
    virtual TAlfAlignHorizontal CurrentTextAlignmentL( TAlfAlignHorizontal aAlignment,
                                                       TDesC* aText,
                                                       CAlfTextVisual* aVisual);

    
    /**
     * Returns text style id used in the item.
     */
    virtual TInt ModifiedStyleIdL( ) const;
    
protected:

    /**
     * State of the background: clear, color, texture.
     */
    enum TFsItemBackgroundState
        {
        EFsItemBackgroundNone,
        EFsItemBackgroundColor,
        EFsItemBackgroundTexture,
    // <cmail> this feature is waiting for title divider bg graphics
        EFsItemBackgroundBrush
    // </cmail>
        };
    
protected:

    /*
     * A parent control for the tree visualizer.
     */
    CAlfControl& iOwnerControl;
    
    /*
     * Parent layout of the item layout, typically list layout.
     * Not own.
     */
    CAlfLayout* iParentLayout;
    
    /**
     * Main layout for the plain text item.
     * Own.
     */
    CAlfAnchorLayout* iLayout;
    
    /*
     * Visualizer's state flag.
     */
    TUint32 iFlags;
    
    /*
     * Horizontal text alignment in visual.
     */
    TAlfAlignHorizontal iTextAlign;

    /*
     * Item's size.
     */
    TSize iSize;
    
    /*
     * Item's extended size.
     */
    TSize iExtendedSize;

    /**
     * CFsTreeList containing menu for item
     */
    CFsTreeList* iMenu;
     
    /**
     * Text indetation in pixels.
     */
    TInt iTextIndentation;

    /**
     * Custom color set for a text in normal state. 
     * ETrue custom color set, EFalse use color from the skin. 
     */
    TBool iIsCustomTextNormalColor;
    
    /**
     * Text's color in normal (unfocused) state.
     */
    TRgb iNormalTextColor;
    
    /**
     * Custom color set for a text in focused state. 
     * ETrue custom color set, EFalse use color from the skin.
     */    
    TBool iIsCustomTextFocusedColor;
    
    /**
     * Text's color in focused state.
     */
    TRgb iFocusedTextColor;
    
    /**
     * Text's style. The id comes from CAlfTextStyleManager.
     */
    TInt iTextStyleId;
    
    /**
     * ETrue if custom text style is used. EFalse text style taken from layout manager.
     */
    TBool iIsCustomTextStyle;
    
    /**
     * Item's text style manager.
     */
    CFsAlfTextStyleManager* iTextStyleManager;
    
    /**
     * Font height
     */
    TInt iFontHeight;
   
    /**
     * State of the background: clear, color, texture.
     */
    TFsItemBackgroundState iBackgroundState;
    
    /*
     * Background color.
     */
    TRgb iBackgroundColor;
    
    /**
     * Background texture.
     * Not own.
     */
    const CAlfTexture* iBackgroundTexture;

    /**
     * Background brush.
     * Not own.
     */
    CAlfBrush* iBackgroundBrush;
    
    /**
     * Layouting hints. @see CFsTreeItemVisualizerBase::TLayoutHint
     */
    TUint iLayoutHints;
    };

#endif /*FSTREEITEMVISUALIZERBASE_H_*/
