/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class TScrollBarClet.
*
*/

#ifndef __HUISCROLLBARPART_H__
#define __HUISCROLLBARPART_H__

#include <e32base.h>

/* Forward declarations. */
class TScrollBarClet;
class TAlfEvent;
class CAlfVisual;

/**
 * Scroll bar controlet.
 * 
 * This class handles the logic and drawing of a scrollbar.
 * You pass in a series of visuals, and then the class will
 * resize them to reflect the state of the scrollbar.
 */
class TScrollBarClet
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructor.
     */
    IMPORT_C TScrollBarClet();

    /**
     * Constructor with more visuals.
     * 
     * @param aVisual The parent visual, should be a layout containing the entire scrollbar.
     * @param aBar The scrollbar visual - the background of the controlet.
     * @param aThumb The thumb scroll bar of the controlet.
     */
    IMPORT_C TScrollBarClet(CAlfVisual* aVisual, CAlfVisual* aBar, CAlfVisual* aThumb);

    /* Methods. */

    /**
     * Set the position of the scrollbar. Will not actually move
     * the scrollbar until Update() is called.
     *
     * @param aPos Index of the first item visible, i.e. the index of the topmost item in a list box.
     */
    IMPORT_C void SetPos(TInt aPos);

    /**
     * Set the range of the scrollbar. Will not actually move
     * the scrollbar until Update() is called.
     *
     * @param aMin Smallest value the scrollbar position can be set to.
     * @param aMax Largest value the scrollbar position can be set to.     
     */
    IMPORT_C void SetRange(TInt aMin, TInt aMax);

    IMPORT_C void SetHorizontal(TBool aHorizontal);

    IMPORT_C void SetVisibleCount(TInt aVisibleCount);

    IMPORT_C void SetThumbSize(TInt aSize);

    IMPORT_C void Update(TInt aDelayMilliSeconds = 0);
    
private:

    /* Private methods */


public:
    /** Main visual of the part. */
    CAlfVisual* iVisual;

    /** Thumbnail of the item. */
    CAlfVisual* iScrollBar;

    /** Thumb of the item. */
    CAlfVisual* iThumb;

    /** The current scrollbar position */
    TInt iPos;

    /** The minimum value of the scrollbar position */
    TInt iMin;

    /** The maximum value of the scrollbar position */
    TInt iMax;

    /** The size of the visible area in scrollbar values */
    TInt iVisibleCount;

    /** The size of the thumb */
    TInt iThumbSize;
    
    /** The minimum thumb size */
    TInt iMinThumbSize;

    /** The scroll bar is vertical by default. */
    TBool iHorizontal;

    };

#endif  // __HUISCROLLBARPART_H__
