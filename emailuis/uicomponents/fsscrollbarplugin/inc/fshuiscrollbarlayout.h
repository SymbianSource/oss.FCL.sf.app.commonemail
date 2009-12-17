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
* Description:  Implementation on scrollbar layout.
*
*/



#ifndef C_FSHUISCROLLBARLAYOUT_H
#define C_FSHUISCROLLBARLAYOUT_H


#include <e32base.h>
//<cmail> SF
#include <uiacceltk/hitchcock.h>
//</cmail>
#include <uiacceltk/HuiImageLoaderUtil.h>
#include <uiacceltk/HuiBitmapProvider.h>
#include "fsscrollbarclet.h"

#include <uiacceltk/HuiBorderBrush.h>

class CFbsBitmap;

/**
 * Scrollbar layout.
 * This class creates a layout visual with scrollbars in it.
 * Both horizontal and vertical scrollbars are supported. Scrollbars have
 * three modes: show, hide and auto.
 * When set to show the scrollbar is always visible.
 * Hide setting keeps the scrollbar always hidden.
 * Auto setting determines if the scrollbar is needed or not.
 * Each scrollbar can have different visibility setting.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFsScrollbarBitmapProvider :     
    public CBase,
    public MHuiBitmapProvider

	{
	public:
		CFsScrollbarBitmapProvider( TAknsItemID aID, TFsScrollbar aOrientation );
		~CFsScrollbarBitmapProvider();
		
		virtual void ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap);
		
	private: 
		TAknsItemID iID;
		TFsScrollbar iOrientation;
	};

class CFsHuiScrollbarLayout : public CHuiLayout
    {
public:

    /**
     * Construct a new scrollbar layout and give its ownership to a control.
     *
     * @param aOwnerControl Owner control.
     * @param aParentLayout Parent layout for the new layout.
     * @return The new scrollbar layout.
     */
    IMPORT_C static CFsHuiScrollbarLayout* AddNewL(
        CHuiControl& aOwnerControl, CHuiLayout* aParentLayout = 0 );

    /**
    * Destructor.
    */
    virtual ~CFsHuiScrollbarLayout();

    /**
     * Set the range of the scrollbar. Will not actually move the scrollbar
     * until Update() is called.
     *
     * @param aStart Smallest value the scrollbar position can be set to.
     * @param aEnd Largest value the scrollbar position can be set to.
     * @param aScrollbar The scrollbar at issue.
     */
    IMPORT_C void SetRange( TInt aStart, TInt aEnd,
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Get scrollbar's minimum range value.
     *
     * @param aScrollbar The scrollbar at issue.
     * @return Scrollbar's start range value.
     */
    IMPORT_C TInt RangeStart(
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Get scrollbar's maximum range value.
     *
     * @param aScrollbar The scrollbar at issue.
     * @return Scrollbar's end range value.
     */
    IMPORT_C TInt RangeEnd( TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Set the count of the visible unit.
     *
     * @param aThumbSpan Count of the units that is visible at a time.
     * @param aScrollbar The scrollbar at issue.
     */
    IMPORT_C void SetThumbSpan( TInt aThumbSpan,
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Resolve the count of the units that is visible at a time.
     *
     * @param aScrollbar The scrollbar at issue.
     * @return Count of the units that is visible at a time.
     */
    IMPORT_C TInt ThumbSpan(
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Adjust the vertical scrollbar's width.
     *
     * @param aWidth New width.
     * @param aTransitionTime How long the transition should take visually,
     * in milliseconds.
     */
    IMPORT_C void SetVerticalScrollbarWidth(
        TInt aWidth, TInt aTransitionTime = 0 );

    /**
     * Adjust the horizontal scrollbar's height.
     *
     * @param aHeight New height.
     * @param aTransitionTime How long the transition should take visually,
     * in milliseconds.
     */
    IMPORT_C void SetHorizontalScrollbarHeight(
        TInt aHeight, TInt aTransitionTime = 0 );

    /**
     * Set the position of the scrollbar. Will not actually move the scrollbar
     * until Update() is called.
     *
     * @param aPos Index of the first item visible, i.e. the index of the
     * topmost item in a list box.
     * @param aScrollbar The scrollbar at issue.
     */
    IMPORT_C void SetScrollbarPos( const TInt aPos,
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Resolve the current scrollbar position.
     *
     * @param aScrollbar The scrollbar at issue.
     * @return Index of the first item.
     */
    IMPORT_C TInt ScrollbarPos(
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Set the scrollbar visibility.
     *
     * @param aVisibility New visibility mode for the scrollbar.
     * @param aScrollbar The scrollbar at issue.
     */
    IMPORT_C void SetVisibilityMode( TFsScrollbarVisibility aVisibility,
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Resolve the scrollbar visibility.
     *
     * @param aVisibility New visibility mode for the scrollbar.
     * @return Current visibility mode of the selected scrollbar.
     */
    IMPORT_C TFsScrollbarVisibility VisibilityMode(
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Resolve if the specified scrollbar is visible.
     *
     * @param aScrollbar The scrollbar at issue.
     * @return <code>ETrue</code>, if the specified scrollbar is visible.
     * Otherwise, <code>EFalse</code> is returned if the scrollbar is not
     * visible.
     */
    IMPORT_C TBool IsScrollbarVisible(
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Change the bar image to custom image. Ownership of the brushes are
     * changed.
     *
     * @param aBarBrush Brush with the new bar image.
     * @param aThumbBrush Brush with the new thumb image.
     * @param aScrollbar The scrollbar at issue.
     */
    IMPORT_C void SetScrollbarImagesL(
        CHuiImageBrush* aBarBrush, CHuiImageBrush* aThumbBrush,
        TFsScrollbar aScrollbar );

    /**
     * Update the visibility of scrollbars.
     *
     * @param aLayoutTransitionTime How long the transition should take
     * visually, in milliseconds.
     */
    IMPORT_C void Update( TInt aLayoutTransitionTime = 0 );

    /**
     * Remove reference to custom image brushes without releasing those.
     * This is for cases where ownership of image brushes couldn't be
     * tranfered to scrollbar layout.
     *
     * @param aScrollbar The scrollbar at issue.
     */
    void RemoveCustomImages( TFsScrollbar aScrollbar );

    /**
     * Resolve the layout's size without the scrollbars.
     *
     * @return Size of the layout without the scrollbars.
     */
    IMPORT_C THuiRealPoint LayoutSize();

// from base class CHuiLayout

    /**
     * From CHuiLayout.
     * Change the size of the layout. Children's positions and sizes are
     * updated accordingly. Each layout class is responsible for determining
     * how to update children's layout.
     *
     * @param aSize Size to set the layout to.
     * @param aTransitionTime How long the transition should take visually,
     * in milliseconds.
     */
    IMPORT_C virtual void SetSize( const THuiRealSize& aSize,
        TInt aTransitionTime = 0 );

    /**
     * From CHuiLayout.
     * Update the layout of one child visual.
     *
     * @param aIndex Child visual index.
     * @param aTransitionTime  Time for layout transition.
     */
    IMPORT_C virtual void UpdateChildLayout(
        TInt aIndex, TInt aTransitionTime = 0 );

    /**
     * From CHuiLayout.
     * Determines the size of a child visual according to the layout.
     *
     * @param aOrdinal Layout ordinal of the child visual.
     * @param aSize New target size for the child.
     * @return  <code>ETrue</code>, if a new size was determined. Otherwise,
     *          <code>EFalse</code> is returned and no size changes are
     *          needed.
     */
    TBool ChildSize( TInt aOrdinal, TSize& aSize );

    /**
     * From CHuiLayout.
     * Determines the position of a child visual according to the layout.
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aPos      New position for the child.
     *
     * @return  <code>ETrue</code>, if a new position was determined.
     *          Otherwise, <code>EFalse</code> is returned and no position
     *          changes are needed.
     */
    TBool ChildPos( TInt aOrdinal, TPoint& aPos );

    /**
     * From CHuiLayout.
     * Update the layout of all children.
     *
     * @param aLayoutTransitionTime How long the transition should take
     * visually, in milliseconds.
     */
    virtual void UpdateChildrenLayout( TInt aTransitionTime = 0 );

    /**
     * From CHuiLayout.
     * Determines the position and size of a child visual according to the
     * layout.
     *
     * @param aOrdinal  Layout ordinal of the child visual.
     * @param aRect      New position for the child. This is used to set
     *                          the child size and position with no conversions.
     */
    virtual TInt ChildRect( TInt aOrdinal, THuiRealRect& aPos );

private:

    CFsHuiScrollbarLayout( MHuiVisualOwner& aOwner );

    void ConstructL( CHuiControl& aOwner );

    /**
     * Create a scrollbar visual.
     * The scrollbar is only created, but not added to this layout.
     *
     * @param aScrollbarLayout The parent visual, should be a layout
     * containing the entire scrollbar.
     * @param aScrollbarVisual The scrollbar visual - the background of the
     * controlet.
     * @param aThumbVisual The thumb scrollbar of the controlet.
     */
    void CreateScrollbarL( CHuiDeckLayout*& aScrollbarLayout,
        CHuiLayout*& aThumbLayout,
        TFsScrollbar aOrientation = EFsScrollbarVertical );

    /**
     * Create a scrollbar visual.
     * The scrollbar is only created, but not added to this layout.
     *
     * @param aVisible <code>ETrue</code>, if the scrollbar is made visible.
     * Otherwise,<code>EFalse</code> to hide the scrollbar.
     * @param aScrollbar The scrollbar at issue.
     */
    void MakeScrollbarVisible( TBool aVisible,
        TFsScrollbar aScrollbar = EFsScrollbarVertical );

    /**
     * Take the custom set image in use.
     * Default scrollbar is build with grid layout and it must be released
     * first.
     *
     * @param aLayout Layout where the image is set.
     * @param aImage Brush with custom image.
     * @param aChildLayout Child layout that is not touched.
     * @return Error code.
     */
    TInt UseCustomImage( CHuiDeckLayout* aLayout, CHuiImageBrush* aImage,
        CHuiDeckLayout* aChildLayout );

    /**
     * Update scrollbar thumb positions.
     *
     * @param aLayoutTransitionTime How long the transition should take
     * visually, in milliseconds.
     */
    void UpdateScrollbars( TInt aLayoutTransitionTime = 0 );

    /**
     * Create a gridlayout with three skin images. Layout is used to construct
     * the scrollbar image.
     *
     * @param aLayout Parent layout.
     * @param aIdTop Skin item id.
     * @param aIdMiddle Skin item id.
     * @param aIdBottom Skin item id.
     * @param aOrientation Orientation of the grid.
     */
    CHuiGridLayout* CreateGridLayoutL(
        CHuiLayout* aLayout,
        const TAknsItemID& aIdTop,
        const TAknsItemID& aIdMiddle,
        const TAknsItemID& aIdBottom,
        TFsScrollbar aOrientation );

    /**
     * Start using a custom set scrollbar image.
     * Ownership of the new brush is gained.
     *
     * @param aBrush Brush with custom image.
     * @param aScrollbar The scrollbar at issue.
     * @param aSetThumbImage <code>ETrue</code>, if the thumb image is set.
     * <code>EFalse</code>, if the bar image is changed.
     * @return Error code.
     */
    void SetCustomImageL( CHuiImageBrush* aBrush,
        TFsScrollbar aScrollbar, TBool aSetThumbImage );
    
    void SetImageL(
        CHuiImageVisual* aVisual,
        const TAknsItemID& aID,
        TFsScrollbar aOrientation );

private: // data

    /**
     * The layout that contains the entire vertical scrollbar.
     * Not own.
     */
    CHuiDeckLayout* iVScrollbarLayout;

    /**
     * The layout that contains the entire horizontal scrollbar.
     * Not own.
     */
    CHuiDeckLayout* iHScrollbarLayout;

    /**
     * Place holder for scrollbar size. Width is the width of vertical
     * scrollbar and height is the height of horizontal scrollbar.
     */
    TSize iScrollbarSize;

    /**
     * Visual for the vertical scrollbar thumb (part that scrolls around).
     * Not own.
     */
    CHuiDeckLayout* iVThumb;

    /**
     * Visual for the horizontal scrollbar thumb (part that scrolls around).
     * Not own.
     */
    CHuiDeckLayout* iHThumb;

    /**
     * Owner control.
     * Not own.
     */
    CHuiControl* iOwnerControl;

    /**
     * Handles manipulating the vertical scrollbar.
     */
    CFsScrollbarClet iVertScroll;

    /**
     * Handles manipulating the horizontal scrollbar.
     */
    CFsScrollbarClet iHorScroll;

    /**
     * Custom image of vertical bar.
     * Owned.
     */
    CHuiImageBrush* iVBarBrush;

    /**
     * Custom image of horizontal bar.
     * Owned.
     */
    CHuiImageBrush* iHBarBrush;

    /**
     * Custom image of vertical thumb.
     * Owned.
     */
    CHuiImageBrush* iVThumbBrush;

    /**
     * Custom image of horizontal thumb.
     * Owned.
     */
    CHuiImageBrush* iHThumbBrush;

    RPointerArray<CHuiTexture> iTextureStack;
    RPointerArray<CFsScrollbarBitmapProvider> iBitmapProviderStack;

    TInt iTextureCounter;
    };


#endif // C_FSHUISCROLLBARLAYOUT_H
