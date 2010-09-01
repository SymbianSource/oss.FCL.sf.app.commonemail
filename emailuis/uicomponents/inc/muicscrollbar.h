/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: muicscrollbar.h
*
*/

#ifndef M_UICSCROLLBAR_H
#define M_UICSCROLLBAR_H

#include <e32base.h>
#include "tuicscrollbarmodel.h"

/**
 * MUiCScrollBar
 *
 */
class MUiCScrollBar
    {

public:

    /**
     * Scroll events
     */
    enum TEvent
        {
        EScrollHome,
        EScrollEnd,
        EScrollPosition
        };

    /**
     * Layouting hints
     */
    enum TLayoutHint
        {
        // Scroll bar is shown on top of the list, not on side
        ELayoutOnTopOfList = (1 << 0),
                
        // Coordinates must be given relative to the list, not screen
        ELayoutRelativeToList = (1 << 1),
                
        // Scroll bar will do automatic mirroring
        ELayoutAutomaticMirroring = (1 << 2)      
        };
    
public:

    /**
     * Scrollbar observer
     */
    class MObserver
        {
    public:
        virtual void HandleScrollEventL( const MUiCScrollBar& aScrollBar,
            TEvent aEvent ) = 0;
        };

public:

    /**
     * Destroys instance and frees resources
     */
    virtual void Destroy() = 0;
    
    /**
     * Shows scrollbar
     */
    virtual void Show( const TBool aShow ) = 0;
    
    /**
     * Updates scrollbar model
     */
    virtual void UpdateModelL( const TUiCScrollBarModel& aModel ) = 0;
    
    /**
     * Returns thumb position
     */
    virtual TInt ThumbPosition() const = 0;
    
    /**
     * Sets scrollbar rect
     */
    virtual void SetRect( const TRect& aRect ) = 0;

    /**
     * Force scrollbar redraw
     */
    virtual void Redraw() = 0;

    /**
     * Called when theme is changed
     */
    virtual void NotifyThemeChanged() = 0;

    /**
     * Returns layouting hints for the scrollbar
     */
    virtual TInt LayoutHints() const = 0;

    };

#endif /* M_UICSCROLLBAR_H */
