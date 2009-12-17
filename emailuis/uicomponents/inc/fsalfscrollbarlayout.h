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
* Description: This file defines class CFsAlfScrollbarLayout.
*
*/



#ifndef C_FSALFSCROLLBARLAYOUT_H
#define C_FSALFSCROLLBARLAYOUT_H


//<cmail> SF
#include <alf/alflayout.h>
//</cmail>
#include "fsscrollbarset.h"
#include "fsscrollbarpluginconstants.h"

class CAlfImageBrush;
class MAlfVisualOwner;
class CAlfDeckLayout;


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
class CFsAlfScrollbarLayout : public CAlfLayout
    {
public:

    /**
     * Construct a new scrollbar layout and give its ownership to a control.
     *
     * @param aOwnerControl Owner control.
     * @param aParentLayout Parent layout for the new layout.
     * @return The new scrollbar layout.
     */
    IMPORT_C static CFsAlfScrollbarLayout* AddNewL(
        CAlfControl& aOwnerControl, CAlfLayout* aParentLayout = NULL );

    /**
    * Destructor.
    */
    virtual ~CFsAlfScrollbarLayout();

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
        CAlfImageBrush* aBarBrush, CAlfImageBrush* aThumbBrush,
        TFsScrollbar aScrollbar );

    /**
     * Update the visibility of scrollbars.
     *
     * @param aLayoutTransitionTime How long the transition should take
     * visually, in milliseconds.
     */
    IMPORT_C void Update( TInt aLayoutTransitionTime = 0 );

/*IMPORT_C virtual void SetSize( const TAlfRealSize& aSize,
    TInt aTransitionTime = 0 );

IMPORT_C virtual void UpdateChildLayout(
    TInt aIndex, TInt aTransitionTime = 0 );*/

    /**
     * Get the target size of the scrolbar layout without the scrollbars.
     *
     * @return Size of the scrollbar layout without the scrollbars.
     */
    IMPORT_C TAlfRealPoint LayoutSize();

private:

    CFsAlfScrollbarLayout();

    void ConstructL( CAlfControl& aOwner );

    /**
     * Send a command to server.
     *
     * @param aCommandId Id of the command to be sent.
     * @param aInScrollbarSet Data for the used command.
     */
    void SendCommand(
        TFsScrollbarCommand aCommandId, TFsScrollbarSet& aInScrollbarSet );

    /**
     * Send a command to server.
     *
     * @param aCommandId Id of the command to be sent.
     * @param aInScrollbarSet Data for the used command.
     * @param aInScrollbarSet Placeholder for received data.
     */
    void SendCommand( TFsScrollbarCommand aCommandId,
        TFsScrollbarSet& aInScrollbarSet, TFsScrollbarSet& aOutScrollbarSet );

    /**
     * Release custom image reserved memory.
     *
     * @param aScrollbar The scrollbar at issue.
     */
    void ReleaseCustomImage( TFsScrollbar aScrollbar );

private: // data

    /**
     * The layout that contains the entire vertical scrollbar.
     * Not own.
     */
    CAlfDeckLayout* iVScrollbarLayout;

    /**
     * The layout that contains the entire horizontal scrollbar.
     * Not own.
     */
    CAlfDeckLayout* iHScrollbarLayout;

    /**
     * Background image for the vertical scrollbar.
     * Not own.
     */
    CAlfVisual* iVScrollbar;

    /**
     * Background image for the horizontal scrollbar.
     * Not own.
     */
    CAlfVisual* iHScrollbar;

    /**
     * Visual for the vertical scrollbar thumb (part that scrolls around).
     * Not own.
     */
    CAlfDeckLayout* iVThumb;

    /**
     * Visual for the horizontal scrollbar thumb (part that scrolls around).
     * Not own.
     */
    CAlfDeckLayout* iHThumb;

    /**
     * ?description_of_pointer_member
     * Not own.
     */
    CAlfControl* iOwnerControl;

CAlfImageBrush* iVBarBrush;
CAlfImageBrush* iVThumbBrush;
CAlfImageBrush* iHBarBrush;
CAlfImageBrush* iHThumbBrush;
    };


#endif // C_FSALFSCROLLBARLAYOUT_H
