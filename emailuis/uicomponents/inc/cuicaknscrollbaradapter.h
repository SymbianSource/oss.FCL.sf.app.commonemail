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
* Description: cuicaknscrollbaradapter.h
*
*/

#ifndef C_UICAKNSCROLLBARADAPTER_H
#define C_UICAKNSCROLLBARADAPTER_H

#include "muicscrollbar.h"
#include <eikscrlb.h>

/**
 * CUiCAknScrollBarAdapter
 *
 */
NONSHARABLE_CLASS( CUiCAknScrollBarAdapter ) :
        private CBase,
        public MUiCScrollBar,
        public MEikScrollBarObserver
    {

public:

    /**
     * Leave safe constructor
     */
    static MUiCScrollBar* NewL( MUiCScrollBar::MObserver& aObserver );

public: // from MUiCScrollBar

    /**
     * @see MUiCScrollBar::Destroy
     */
    virtual void Destroy();

    /**
     * @see MUiCScrollBar::Show
     */
    virtual void Show( const TBool aShow );

    /**
     * @see MUiCScrollBar::UpdateModelL
     */
    virtual void UpdateModelL( const TUiCScrollBarModel& aModel );

    /**
     * @see MUiCScrollBar::ThumbPosition
     */
    virtual TInt ThumbPosition() const;

    /**
     * @see MUiCScrollBar::SetRect
     */
    virtual void SetRect( const TRect& aRect );

    /**
     * @see MUiCScrollBar::Redraw
     */
    virtual void Redraw();

    /**
     * @see MUiCScrollBar::NotifyThemeChanged
     */
    virtual void NotifyThemeChanged();

    /**
     * @see MUiCScrollBar::LayoutHints
     */
    virtual TInt LayoutHints() const;

public: // from MEikScrollBarObserver

    virtual void HandleScrollEventL( CEikScrollBar* aScrollBar,
            TEikScrollEvent aEventType );

private:

    /**
     * Constructor
     */
    CUiCAknScrollBarAdapter( MUiCScrollBar::MObserver& aObserver );

    /**
     * Destructor
     */
    ~CUiCAknScrollBarAdapter();

    /**
     * 2nd constructor
     */
    void ConstructL();

private:

    // Scrollbar observer
    MUiCScrollBar::MObserver& iObserver;

    // Scrollbar instance
    CAknDoubleSpanScrollBar* iScrollBar;

    // Model for aknscrollbar
    TAknDoubleSpanScrollBarModel iModel;

    };

#endif /* C_UICAKNSCROLLBARADAPTER_H */
