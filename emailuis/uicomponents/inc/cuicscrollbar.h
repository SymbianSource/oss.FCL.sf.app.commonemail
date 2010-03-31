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
* Description: cuicscrollbar.h
*
*/

#ifndef C_UICSCROLLBAR_H
#define C_UICSCROLLBAR_H

#include <alf/alfcontrol.h>
#include <alf/alfbitmapprovider.h>

#include "muicscrollbar.h"

// Forward declarations

class CAlfAnchorLayout;
class CAlfDeckLayout;
class CAlfImageVisual;
class CAlfLayout;
class CAlfTexture;

/**
 * Composite image
 */
NONSHARABLE_CLASS( CUiCCompositeImage ) : public CBase
    {
public:

    /**
     * Leave safe constructor
     */
    static CUiCCompositeImage* NewL( CAlfControl& aControl, CAlfLayout* aParentLayout,
            CAlfTexture& aTop, CAlfTexture& aMiddle, CAlfTexture& aBottom );

    /**
     * Destructor
     */
    ~CUiCCompositeImage();

    /**
     * Returns layout for this component
     */
    CAlfLayout& Layout();

private:

    CUiCCompositeImage();
    void ConstructL( CAlfControl& aControl, CAlfLayout* aParentLayout,
            CAlfTexture& aTop, CAlfTexture& aMiddle, CAlfTexture& aBottom );
    void SetAnchors();

private:

    CAlfAnchorLayout* iLayout;
    CAlfImageVisual* iTop;
    CAlfImageVisual* iMiddle;
    CAlfImageVisual* iBottom;

    };

/**
 * CUiCScrollBar
 *
 */
NONSHARABLE_CLASS( CUiCScrollBar ) : public CAlfControl, public MUiCScrollBar,
        public MAlfBitmapProvider
    {

public:

    /**
     * Leave safe constructor
     */
    static CUiCScrollBar* NewL( CAlfEnv& aEnv, CAlfLayout* aParentLayout );

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

private: // from MAlfEventHandler

    /**
     * @see MAlfEventHandler::OfferEventL
     */
    virtual TBool OfferEventL( const TAlfEvent& aEvent );

private: // from MAlfBitmapProvider

    /**
     * @see MAlfBitmapProvider::ProvideBitmapL
     */
    virtual void ProvideBitmapL( TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap );

private:

    /**
     * Destructor
     */
    ~CUiCScrollBar();

    /**
     * Constructor
     */
    CUiCScrollBar();

    /**
     * 2nd constructor
     */
    void ConstructL( CAlfEnv& aEnv, CAlfLayout* aParentLayout );

    /**
     * Sets anchors for background image and thumb
     */
    void SetAnchors();

    /**
     * Sets anchors for background
     */
    void SetBgAnchors();

    /**
     * Sets anchors for thumb
     */
    void SetThumbAnchors();

private:

    /**
     * Custom events
     */
    enum TEvent
        {
        EFadeOut
        };

    /**
     * Unique bitmaps IDs, make sure these do no overlap with the ones defined
     * in UI
     */
    enum TBitmapId
        {
        EThumbTop = 0x10000,
        EThumbMiddle,
        EThumbBottom,
        EBackgroundTop,
        EBackgroundMiddle,
        EBackgroundBottom
        };

private:

    // Container layout
    CAlfDeckLayout* iDeck;

    // Scrollbar background layout
    CAlfAnchorLayout* iBgLayout;

    // Scrollbar layout
    CAlfAnchorLayout* iThumbLayout;

    // Scrollbar background visual
    CUiCCompositeImage* iBackGroundVisual;

    // Scrollbar thumb visual
    CUiCCompositeImage* iThumbVisual;

    // Scrollbar model
    TUiCScrollBarModel iModel;

    };

#endif /* C_UICSCROLLBAR_H */
