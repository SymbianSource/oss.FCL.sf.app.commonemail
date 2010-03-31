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
* Description: tuicscrollbarmodel.h
*
*/

#ifndef T_UICSCROLLBARMODEL_H
#define T_UICSCROLLBARMODEL_H

#include <e32base.h>

/**
 * TUiCScrollBarModel
 * 
 */
class TUiCScrollBarModel
    {

public:
    
    /**
     * Constructor
     */
    TUiCScrollBarModel();

public: // model

    /**
     * Set total height
     */
    void SetHeight( TInt aHeight );
    
    /**
     * Total height
     */
    TInt Height() const;
    
    /**
     * Set window height
     */
    void SetWindowHeight( TInt aHeight );
    
    /**
     * Window height
     */
    TInt WindowHeight() const;
    
    /**
     * Sets top position of the window
     */
    void SetWindowPosition( TInt aPosition );

    /**
     * Top position of the window
     */
    TInt WindowPosition() const;

    /**
     * Height of the thumb
     */
    TInt ThumbHeight() const;
    
    /**
     * Position of thumb inside the window
     */
    TInt ThumbPosition() const;
    
    /**
     * Returns ETrue if total height is greater than window height
     */
    TBool ScrollBarNeeded() const;

private:
    
    /**
     * Returns minimum thumb height (from layout data)
     */
    TInt MinimumThumbHeight() const;
    
    /**
     * Returns calculated thumb height. Thumb height is calculated,
     * unless this result is smaller than MinimumThumbHeight().
     */
    TInt CalculatedThumbHeight() const;

private:

    TInt iHeight;
    TInt iWindowHeight;
    TInt iWindowPosition;

    };

#endif /* T_UICSCROLLBARMODEL_H */
