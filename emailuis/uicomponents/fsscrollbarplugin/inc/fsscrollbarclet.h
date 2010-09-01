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
* Description:  Thumb bar size and position adjustment.
*
*/



#ifndef C_FSSCROLLBARCLET_H
#define C_FSSCROLLBARCLET_H


#include <e32base.h>

#include "fsscrollbarset.h"

class CHuiDeckLayout;

/**
 * Scrollbar controlet.
 *
 * This class handles the logic and drawing of a scrollbar.
 * You pass in a series of visuals, and then the class will
 * resize them to reflect the state of the scrollbar.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFsScrollbarClet : public CBase
    {
public:

    /**
     * Constructor.
     */
    CFsScrollbarClet();

    /**
    * Destructor.
    */
    virtual ~CFsScrollbarClet();

    /**
     * Set the scrollbar visuals.
     * The scrollbar is build from these images by changing their places.
     *
     * @param aLayout The parent layout, should be a layout containing the
     * entire scrollbar.
     * @param aThumb The thumb scrollbar of the controlet.
     */
    void SetVisuals( CHuiDeckLayout* aLayout, CHuiDeckLayout* aThumb );

    /**
     * Define the layout to use.
     *
     * @param aHorizontal <code>ETrue</code>, if this scrollbar use
     * horizontal layout. Otherwise, <code>EFalse</code> to use vertical
     * layout.
     */
    void SetHorizontal( TBool aHorizontal );

    /**
     * Set the visibility mode of this scrollbar.
     *
     * @param aMode New visibility mode.
     */
    void SetVisibleMode( TFsScrollbarVisibility aMode );

    /**
     * Resolve the current visibility mode.
     *
     * @return The current visibility mode.
     */
    TFsScrollbarVisibility VisibilityMode();

    /**
     * Resolve if this scrollbar is visible.
     *
     * @return <code>ETrue</code>, if this scrollbar is visible.
     * Otherwise, <code>EFalse</code> is returned and the scrollbar is not
     * visible.
     */
    TBool IsVisible();

    /**
     * Set the range of the scrollbar. Will not actually move the scrollbar
     * until Update() is called.
     *
     * @param aStart Smallest value the scrollbar position can be set to.
     * @param aEnd Largest value the scrollbar position can be set to.
     * @panic KErrArgument if  the minimum range value is bigger than the
     * maximum range value.
     */
    void SetRange( TInt aStart, TInt aEnd );

    /**
     * Resolve the minimum range value.
     *
     * @return Minimum range value.
     */
    TInt RangeStart();

    /**
     * Resolve the maximum range value.
     *
     * @return Maximum range value.
     */
    TInt RangeEnd();

    /**
     * Set the count of visible items.
     *
     * @param aThumbSpan Count of items visible at the same time.
     */
    void SetThumbSpan( TInt aThumbSpan );

    /**
     * Resolve the count of total items on the list.
     *
     * @return Count of the total visible items at a time.
     */
    TInt ThumbSpan();

    /**
     * Set the position of the scrollbar. Will not actually move the scrollbar
     * until Update() is called.
     *
     * @param aPos Index of the first item visible, i.e. the index of the
     * topmost item in a list box.
     */
    void SetPos( const TInt aPos );

    /**
     * Get the index of the first position.
     *
     * @return Index of the first item.
     */
    TInt Pos();

    /**
     * Set the height and width of the thumb item.
     *
     * @param aSize Height and the width of the thumb image.
     */
    void SetThumbSize( const TSize& aSize );

    /**
     * Resolve the thumb size.
     *
     * @return The thumb image size.
     */
    TSize ThumbSize();

    /**
     * Update scrollbar thumb positions.
     *
     * @param aLayoutTransitionTime How long the transition should take
     * visually, in milliseconds.
     */
    void Update( TInt aLayoutTransitionTime = 0 );

    /**
     * Inform that the custom bar image is used.
     */
    void EnableCustomBarImage();

    /**
     * Inform that the custom thumb image is used.
     */
    void EnableCustomThumbImage();

    /**
     * Adjust the scrollbar visual size. The visual constructs from three
     * visual which size relations need to be calculated when the parent
     * visual's size changes.
     *
     * @param aLayout Layout containing the three visuals.
     * @param aTarget Target size.
     * @param aLayoutTransitionTime How long the transition should take
     * visually, in milliseconds.
     */
    void AdjustSize( CHuiGridLayout* aLayout, TSize& aTarget, TInt aLayoutTransitionTime = 0 );

private:

    /**
     * Set to show or hide this component.
     *
     * @param aIsVisible <code>ETrue</code>, if this scrollbar is visible.
     * Otherwise, <code>EFalse</code> to hide this scrollbar.
     */
    void SetVisible( TBool aIsVisible );

private: // data

    /**
     * Main visual of the part.
     * Not own.
     */
    CHuiDeckLayout* iScrollbarLayout;

    /**
     * Thumb of the item.
     * Not own.
     */
    CHuiDeckLayout* iThumbLayout;

    /**
     * The current scrollbar position.
     */
    TInt iPos;

    /**
     * The minimum value of the scrollbar position.
      */
    TInt iStart;

    /**
     * The maximum value of the scrollbar position.
     */
    TInt iEnd;

    /**
     * The size of the visible area in scrollbar values.
     */
    TInt iThumbSpan;

    /**
     * The size of the thumb.
     */
    TSize iThumbSize;

    /**
     * The minimum size of the thumb.
     */
    TSize iMinThumbSize;

    /**
     * The scrollbar is vertical by default.
     */
    TBool iHorizontal;

    /**
     * Defines if the scrollbar is visible or hidden.
     */
    TBool iVisible;

    /**
     * The visibility mode of the scrollbar.
     */
    TFsScrollbarVisibility iVisibility;

    /**
     * Defines if the default or custom bar image is used.
     */
    TBool iUseDefaultBarImages;

    /**
     * Defines if the default or custom thumb image is used.
     */
    TBool iUseDefaultThumbImages;

    };


#endif // C_FSSCROLLBARCLET_H
