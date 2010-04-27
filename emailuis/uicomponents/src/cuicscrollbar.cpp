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
* Description: cuicscrollbar.cpp
*
*/

#include "cuicscrollbar.h"
#include "emailtrace.h"

#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexture.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfdecklayout.h>
#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>

// Constants

// Scroll bar opacity (visible)
const TReal32 KVisibleOpacity = 1.0f;
// Thumb opacity (visible)
const TReal32 KVisibleOpacityThumb = 0.85f;
// Background opacity (visible)
const TReal32 KVisibleOpacityBackground = 0.65f;
// How long scrollbar will be visible after change
const TReal32 KVisibleTimeout = 250;
// How long fading will take
const TReal32 KFadeTimeout = 500;

// Local methods

// ---------------------------------------------------------------------------
// ScrollBarWidth
// ---------------------------------------------------------------------------
TInt ScrollBarWidth()
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_email_pane() );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::listscroll_cmail_pane() );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::list_cmail_pane() );    
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::sp_fs_scroll_pane_cp02() );
    return layoutRect.Rect().Width();
    }

// CUiCCompositeImage

// ---------------------------------------------------------------------------
// CUiCCompositeImage::NewL
// ---------------------------------------------------------------------------
//
CUiCCompositeImage* CUiCCompositeImage::NewL( CAlfControl& aControl,
        CAlfLayout* aParentLayout, CAlfTexture& aTop,
        CAlfTexture& aMiddle, CAlfTexture& aBottom )
    {
    FUNC_LOG;
    CUiCCompositeImage* self = new (ELeave) CUiCCompositeImage();
    CleanupStack::PushL( self );
    self->ConstructL( aControl, aParentLayout, aTop, aMiddle, aBottom );
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// CUiCCompositeImage::~CUiCCompositeImage
// ---------------------------------------------------------------------------
//
CUiCCompositeImage::~CUiCCompositeImage()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CUiCCompositeImage::Layout
// ---------------------------------------------------------------------------
//
CAlfLayout& CUiCCompositeImage::Layout()
    {
    FUNC_LOG;
    return *iLayout;
    }

// ---------------------------------------------------------------------------
// CUiCCompositeImage::CUiCCompositeImage
// ---------------------------------------------------------------------------
//
CUiCCompositeImage::CUiCCompositeImage()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CUiCCompositeImage::ConstructL
// ---------------------------------------------------------------------------
//
void CUiCCompositeImage::ConstructL( CAlfControl& aControl, CAlfLayout* aParentLayout,
        CAlfTexture& aTop, CAlfTexture& aMiddle, CAlfTexture& aBottom )
    {
    FUNC_LOG;
    iLayout = CAlfAnchorLayout::AddNewL( aControl, aParentLayout );
    iTop = CAlfImageVisual::AddNewL( aControl, iLayout );
    iTop->SetImage( TAlfImage( aTop ) );
    iMiddle = CAlfImageVisual::AddNewL( aControl, iLayout );
    iMiddle->SetImage( TAlfImage( aMiddle ) );
    iBottom = CAlfImageVisual::AddNewL( aControl, iLayout );
    iBottom->SetImage( TAlfImage( aBottom ) );
    SetAnchors();
    }

// ---------------------------------------------------------------------------
// CUiCCompositeImage::SetAnchors
// ---------------------------------------------------------------------------
//
void CUiCCompositeImage::SetAnchors()
    {
    FUNC_LOG;
    const TInt width( ScrollBarWidth() );
    
    // iTop
    iLayout->SetAnchor(
            EAlfAnchorTopLeft, 0,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, 0 ) );
    iLayout->SetAnchor(
            EAlfAnchorBottomRight, 0,
            EAlfAnchorOriginRight, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, width  ) );

    // iMiddle
    iLayout->SetAnchor(
            EAlfAnchorTopLeft, 1,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, width ) );
    iLayout->SetAnchor(
            EAlfAnchorBottomRight, 1,
            EAlfAnchorOriginRight, EAlfAnchorOriginBottom,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, -width  ) );

    // iBottom
    iLayout->SetAnchor(
            EAlfAnchorTopLeft, 2,
            EAlfAnchorOriginLeft, EAlfAnchorOriginBottom,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, -width ) );
    iLayout->SetAnchor(
            EAlfAnchorBottomRight, 2,
            EAlfAnchorOriginRight, EAlfAnchorOriginBottom,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, 0  ) );

    iLayout->UpdateChildrenLayout();
    }

// CUiCScrollBar

// ---------------------------------------------------------------------------
// CUiCScrollBar::NewL
// ---------------------------------------------------------------------------
//
CUiCScrollBar* CUiCScrollBar::NewL( CAlfEnv& aEnv, CAlfLayout* aParentLayout )
    {
    FUNC_LOG;
    CUiCScrollBar* self = new (ELeave) CUiCScrollBar();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aParentLayout );
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::~CUiCScrollBar
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::Destroy()
    {
    FUNC_LOG;
    delete this;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::Show
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::Show( const TBool aShow )
    {
    FUNC_LOG;
    Env().CancelCustomCommands( this );
    TAlfTimedValue opacity( aShow ? KVisibleOpacity : 0 );
    iDeck->SetOpacity( opacity );
    if ( aShow )
        {
        iThumbVisual->Layout().SetOpacity( KVisibleOpacityThumb );
        iBackGroundVisual->Layout().SetOpacity( KVisibleOpacityBackground );
        TAlfCustomEventCommand command( EFadeOut, this );
        Env().Send( command, KVisibleTimeout );
        }
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::UpdateModelL
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::UpdateModelL( const TUiCScrollBarModel& aModel )
    {
    FUNC_LOG;
    iModel = aModel;
    if ( iModel.ScrollBarNeeded() )
        {
        SetThumbAnchors();
        Show( ETrue );
        }
    else
        {
        Show( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::ThumbPosition
// ---------------------------------------------------------------------------
//
TInt CUiCScrollBar::ThumbPosition() const
    {
    FUNC_LOG;
    return iModel.ThumbPosition();
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::SetRect
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::SetRect( const TRect& aRect )
    {
    FUNC_LOG;
    TAlfRealRect realRect( aRect );
    iDeck->SetRect( realRect );
    iDeck->UpdateChildrenLayout();
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::Redraw
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::Redraw()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::NotifyThemeChanged
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::NotifyThemeChanged()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::LayoutHints
// ---------------------------------------------------------------------------
//
TInt CUiCScrollBar::LayoutHints() const
    {
    FUNC_LOG;
    return ELayoutOnTopOfList | ELayoutRelativeToList |
        ELayoutAutomaticMirroring;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::OfferEventL
// ---------------------------------------------------------------------------
//
TBool CUiCScrollBar::OfferEventL( const TAlfEvent& aEvent )
    {
    FUNC_LOG;
    TBool handled( EFalse );
    if ( aEvent.Type() == TAlfEvent::ETypeCustom )
        {
        if ( aEvent.CustomParameter() == EFadeOut )
            {
            handled = ETrue;
            TAlfTimedValue opacity( 0, KFadeTimeout );
            iDeck->SetOpacity( opacity );
            }
        }
    else
        {
        handled = CAlfControl::OfferEventL( aEvent );
        }
    return handled;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::ProvideBitmapL
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::ProvideBitmapL( TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap )
    {
    FUNC_LOG;
    TAknsItemID itemId;

    switch ( aId )
        {
        case EThumbTop:
            itemId = KAknsIIDQsnCpScrollHandleTop;
            break;

        case EThumbMiddle:
            itemId = KAknsIIDQsnCpScrollHandleMiddle;
            break;

        case EThumbBottom:
            itemId = KAknsIIDQsnCpScrollHandleBottom;
            break;

        case EBackgroundTop:
            itemId = KAknsIIDQsnCpScrollBgTop;
            break;

        case EBackgroundMiddle:
            itemId = KAknsIIDQsnCpScrollBgMiddle;
            break;

        case EBackgroundBottom:
            itemId = KAknsIIDQsnCpScrollBgBottom;
            break;

        default:
            User::Leave( KErrNotFound );
        }

    AknsUtils::CreateIconL( AknsUtils::SkinInstance(), itemId, aBitmap, aMaskBitmap, KNullDesC, -1, -1 );

    if ( aBitmap )
        {
        const TInt width( ScrollBarWidth() );

        TAknContentDimensions origDim;
        AknIconUtils::GetContentDimensions( aBitmap, origDim );
        TSize iconSize( origDim.iWidth, origDim.iHeight );
        if (iconSize.iWidth != width && iconSize.iWidth > 0)
            {
            iconSize.iHeight = iconSize.iHeight * width / iconSize.iWidth;
            iconSize.iWidth = width;
            }
        AknIconUtils::DisableCompression( aBitmap );
        AknIconUtils::SetSize( aBitmap, iconSize, EAspectRatioNotPreserved );
        if ( aMaskBitmap )
            {
            AknIconUtils::DisableCompression( aMaskBitmap );
            AknIconUtils::SetSize( aMaskBitmap, iconSize, EAspectRatioNotPreserved );
            }
        }
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::~CUiCScrollBar
// ---------------------------------------------------------------------------
//
CUiCScrollBar::~CUiCScrollBar()
    {
    FUNC_LOG;
    Env().CancelCommands( this );
    if ( iDeck )
        {
        iDeck->RemoveAndDestroyAllD();
        }
    delete iBackGroundVisual;
    delete iThumbVisual;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::CUiCScrollBar
// ---------------------------------------------------------------------------
//
CUiCScrollBar::CUiCScrollBar()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::ConstructL
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::ConstructL( CAlfEnv& aEnv, CAlfLayout* aParentLayout )
    {
    FUNC_LOG;
    CAlfControl::ConstructL( aEnv );
    iDeck = CAlfDeckLayout::AddNewL( *this, aParentLayout );
    iDeck->SetFlags( EAlfVisualFlagManualLayout |
            EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iBgLayout = CAlfAnchorLayout::AddNewL( *this, iDeck );
    iBgLayout->SetFlags( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iBackGroundVisual = CUiCCompositeImage::NewL( *this, iBgLayout,
            Env().TextureManager().CreateTextureL( EBackgroundTop, this, EAlfTextureFlagSkinContent ),
            Env().TextureManager().CreateTextureL( EBackgroundMiddle, this, EAlfTextureFlagSkinContent ),
            Env().TextureManager().CreateTextureL( EBackgroundBottom, this, EAlfTextureFlagSkinContent ) );
    iThumbLayout = CAlfAnchorLayout::AddNewL( *this, iDeck );
    iThumbLayout->SetFlags( EAlfVisualFlagAutomaticLocaleMirroringEnabled );
    iThumbLayout->SetClipping( ETrue );
    iThumbVisual = CUiCCompositeImage::NewL( *this, iThumbLayout,
            Env().TextureManager().CreateTextureL( EThumbTop, this, EAlfTextureFlagSkinContent ),
            Env().TextureManager().CreateTextureL( EThumbMiddle, this, EAlfTextureFlagSkinContent ),
            Env().TextureManager().CreateTextureL( EThumbBottom, this, EAlfTextureFlagSkinContent ) );
    iThumbVisual->Layout().SetFlags( EAlfVisualFlagDrawAfterOthers );
    SetAnchors();    
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::ConstructL
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::SetAnchors()
    {
    FUNC_LOG;
    SetBgAnchors();
    SetThumbAnchors();
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::SetBgAnchors
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::SetBgAnchors()
    {
    FUNC_LOG;
    iBgLayout->SetAnchor(
            EAlfAnchorTopLeft, 0,
            EAlfAnchorOriginRight, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( -ScrollBarWidth(), 0 ) );
    iBgLayout->SetAnchor(
            EAlfAnchorBottomRight, 0,
            EAlfAnchorOriginRight, EAlfAnchorOriginBottom,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, 0 ) );
    iBgLayout->UpdateChildrenLayout();
    }

// ---------------------------------------------------------------------------
// CUiCScrollBar::SetThumbAnchors
// ---------------------------------------------------------------------------
//
void CUiCScrollBar::SetThumbAnchors()
    {
    FUNC_LOG;
    iThumbLayout->SetAnchor(
            EAlfAnchorTopLeft, 0,
            EAlfAnchorOriginRight, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( -ScrollBarWidth(), iModel.ThumbPosition() ) );
    iThumbLayout->SetAnchor(
            EAlfAnchorBottomRight, 0,
            EAlfAnchorOriginRight, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( 0, iModel.ThumbPosition() + iModel.ThumbHeight() ) );
    iThumbLayout->UpdateChildrenLayout();
    }

// end of file
