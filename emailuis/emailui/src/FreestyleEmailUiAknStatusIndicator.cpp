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
* Description:  Implementation for status indicator
*
*/

#include "FreestyleEmailUiAknStatusIndicator.h"

#include <biditext.h>
#include <AknIconUtils.h>
#include <AknBidiTextUtils.h>
#include <AknsUtils.h>
#include <layoutmetadata.cdl.h>

const TInt KImageMargin = 8;
const TInt KTextMargin = 4;
const TInt KRoundBoundaryEllipse = 7;
const TInt KTuncationCharsWidth = 20;

const TInt KAnimationFps = 15;
const TInt KAnimationDelayMs = 1000 / KAnimationFps;
const TInt KSecondsPerFullRotation = 2;
const TInt KPenSize = 2;

CFreestyleEmailUiAknStatusIndicator* CFreestyleEmailUiAknStatusIndicator::NewL( const TRect& aRect, const CCoeControl* aParent )
    {
    CFreestyleEmailUiAknStatusIndicator* self = CFreestyleEmailUiAknStatusIndicator::NewLC( aRect, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CFreestyleEmailUiAknStatusIndicator* CFreestyleEmailUiAknStatusIndicator::NewLC( const TRect& aRect, const CCoeControl* aParent )
    {
    CFreestyleEmailUiAknStatusIndicator* self = new (ELeave) CFreestyleEmailUiAknStatusIndicator();
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent );
    return self;
    }

CFreestyleEmailUiAknStatusIndicator::~CFreestyleEmailUiAknStatusIndicator()
    {
    ClearImage();
    ClearText();

    delete iTimer;
    }

void CFreestyleEmailUiAknStatusIndicator::ShowIndicatorL( CFbsBitmap* aBitmap,
                                                          CFbsBitmap* aMaskBitmap,
                                                          TDesC* aText,
                                                          TInt aDuration,
                                                          const TBool aAnimate )
    {
    iTimer->Stop();

    iAnimate = aAnimate;
    SetImage( aBitmap, aMaskBitmap );
    SetTextL( aText );

    MakeVisible( ETrue );
    DrawNow();

    if ( aDuration >= 0 )
        {
        HideIndicator( aDuration );
        }
    else if ( iAnimate )
        {
        iHideTimeout = KMaxTInt;
        StartTimer( KAnimationDelayMs );
        }
    }

void CFreestyleEmailUiAknStatusIndicator::SetTextL( TDesC* aText )
    {
    SetTextFont();

    ClearText();
    iText = aText;
    if ( iText )
        {
        iVisualText = HBufC::NewL( iText->Length() + KAknBidiExtraSpacePerLine );
        CalculateVisualText();
        }

    if ( IsVisible() )
        {
        DrawNow();
        }
    }

void CFreestyleEmailUiAknStatusIndicator::HideIndicator( TInt aDelayBeforeHidingInMs )
    {
    if ( !aDelayBeforeHidingInMs )
        {
        iTimer->Stop();
        MakeVisible( EFalse );
        iAngle = 0;
        }
    else
        {
        if (iAnimate)
            {
            iHideTimeout = aDelayBeforeHidingInMs / KAnimationDelayMs;
            StartTimer( KAnimationDelayMs );
            }
        else
            {
            iHideTimeout = 0;
            iTimer->Start( aDelayBeforeHidingInMs );
            }
        }
    }

void CFreestyleEmailUiAknStatusIndicator::StartTimer( const TInt aTimeOut )
    {
    iTimer->Stop();
    iTimer->Start( aTimeOut );
    }


CCoeControl* CFreestyleEmailUiAknStatusIndicator::CFreestyleEmailUiAknStatusIndicator::ComponentControl( TInt /*aIndex*/ ) const
    {
    return NULL;
    }

TInt CFreestyleEmailUiAknStatusIndicator::CountComponentControls() const
    {
    return 0;
    }

void CFreestyleEmailUiAknStatusIndicator::Draw( const TRect& aRect ) const
    {
    DrawBoundary( aRect );
    DrawImage( iImageRect );
    DrawText( iTextRect );
    }

void CFreestyleEmailUiAknStatusIndicator::SizeChanged()
    {
    CalculateLayout( Rect() );
    ScaleImage();
    CalculateVisualText();
    }

CFreestyleEmailUiAknStatusIndicator::CFreestyleEmailUiAknStatusIndicator()
    : iBitmap( NULL ),
    iMaskBitmap( NULL ),
    iText( NULL ),
    iVisualText( NULL ),
    iTextFont( NULL )
    {
    }

void CFreestyleEmailUiAknStatusIndicator::ConstructL( const TRect& aRect, const CCoeControl* aParent )
    {
    iTimer = CFSEmailUiGenericTimer::NewL( this );
   
    if ( aParent )
        {
        SetContainerWindowL( *aParent );
        }
    else
        {
        CreateWindowL();
        }
    SetRect( aRect );
    ActivateL();
    MakeVisible( EFalse );
    }

void CFreestyleEmailUiAknStatusIndicator::TimerEventL( CFSEmailUiGenericTimer* /*aTriggeredTimer*/ )
    {
    if ( iAnimate && ( iHideTimeout == KMaxTInt || iHideTimeout-- > 0 ) )
        {
        StartTimer( KAnimationDelayMs );
        iAngle += 360 / (KAnimationFps * KSecondsPerFullRotation);
        ScaleImage();
        DrawDeferred();
        }
    else
        {
        HideIndicator();
        }
    }

TRect CFreestyleEmailUiAknStatusIndicator::ImageRect( const TRect& aRect ) const
    {
    TSize imageSize = aRect.Size();
    imageSize.iHeight = imageSize.iHeight - 2 * KImageMargin;
    imageSize.iWidth = imageSize.iWidth - 2 * KImageMargin;
    if ( imageSize.iWidth > imageSize.iHeight )
        {
        imageSize.iWidth = imageSize.iHeight;
        }
    else
        {
        imageSize.iHeight = imageSize.iWidth;
        }

    TPoint imageLeftTop;
    if ( !Layout_Meta_Data::IsMirrored() )
        {
        imageLeftTop = TPoint( aRect.iTl.iX + KImageMargin, aRect.iTl.iY + KImageMargin );
        }
    else
        {
        imageLeftTop = TPoint( aRect.iBr.iX - KImageMargin - imageSize.iWidth, aRect.iTl.iY + KImageMargin );
        }

    return TRect( imageLeftTop, imageSize );
    }

TRect CFreestyleEmailUiAknStatusIndicator::TextRect( const TRect& aRect ) const
    {
    TSize imageSize = aRect.Size();
    imageSize.iHeight = imageSize.iHeight - 2 * KImageMargin;
    imageSize.iWidth = imageSize.iWidth - 2 * KImageMargin;
    if ( imageSize.iWidth > imageSize.iHeight )
        {
        imageSize.iWidth = imageSize.iHeight;
        }
    else
        {
        imageSize.iHeight = imageSize.iWidth;
        }

    TSize textSize = aRect.Size();
    textSize.iHeight = textSize.iHeight - 2 * KTextMargin;
    textSize.iWidth = textSize.iWidth - 2 * KTextMargin - 2 * KImageMargin - imageSize.iWidth;

    TPoint textLeftTop;
    if ( !Layout_Meta_Data::IsMirrored() )
        {
        textLeftTop = TPoint( aRect.iTl.iX + KImageMargin + imageSize.iWidth + KTextMargin, aRect.iTl.iY + KTextMargin );
        }
    else
        {
        textLeftTop = TPoint( aRect.iTl.iX + KTextMargin, aRect.iTl.iY + KTextMargin );
        }

    return TRect( textLeftTop, textSize );
    }

void CFreestyleEmailUiAknStatusIndicator::CalculateLayout( const TRect& aRect ) const
    {
    iImageRect = ImageRect( aRect );
    iTextRect = TextRect( aRect );
    }

void CFreestyleEmailUiAknStatusIndicator::SetTextFont() const
    {
    if ( iTextFont )
        return;

    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont );
    iTextFont = CONST_CAST( CFont*, font );
    }

void CFreestyleEmailUiAknStatusIndicator::CalculateVisualText() const
    {
    if ( iVisualText )
        {
        TPtr visualText = iVisualText->Des();
        visualText.Zero();
        TInt MaxWidthInPixels = iTextRect.Size().iWidth - KTuncationCharsWidth;
        TInt MaxClippedWidthInPixels = iTextRect.Size().iWidth;
        AknBidiTextUtils::ConvertToVisualAndClip( *iText,
                                                  visualText,
                                                  *iTextFont,
                                                  MaxWidthInPixels,
                                                  MaxClippedWidthInPixels );
        iVisualTextWidth = AknBidiTextUtils::MeasureTextBoundsWidth(
            *iTextFont, *iVisualText, CFont::TMeasureTextInput::EFVisualOrder );
        }
    }

TRgb CFreestyleEmailUiAknStatusIndicator::BackgroundColor() const
    {
    TRgb color;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG22 ) )
        {
        // fallback to default value
        color = TRgb(197, 197, 197);
        }
    return color;
    }

TRgb CFreestyleEmailUiAknStatusIndicator::BorderColor() const
    {
    return KRgbBlack;
    }

TRgb CFreestyleEmailUiAknStatusIndicator::FontColor() const
    {
    TRgb color;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG55 /*EAknsCIQsnTextColorsCG81*/ ) )
        {
        // fallback to default value
        color = KRgbBlack;
        }
    return color;
    }

void CFreestyleEmailUiAknStatusIndicator::DrawBoundary( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );
    gc.SetBrushColor( BackgroundColor() );
    gc.SetPenColor( BorderColor() );
    gc.SetPenSize( TSize(KPenSize, KPenSize) );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    TSize ellipse( KRoundBoundaryEllipse, KRoundBoundaryEllipse );
    TRect roundRect( aRect );
    roundRect.Shrink( 2, 2 );
    gc.DrawRoundRect( roundRect, ellipse);
    }

void CFreestyleEmailUiAknStatusIndicator::DrawImage( const TRect& aRect ) const
    {
    if ( !iBitmap )
        return;

    CWindowGc& gc = SystemGc();
    if ( iMaskBitmap )
        {
        TRect bmpPieceRect(TPoint(0,0), aRect.Size() );
        gc.BitBltMasked( aRect.iTl, iBitmap, bmpPieceRect, iMaskBitmap, EFalse );
        }
    else
        {
        gc.DrawBitmap( aRect, iBitmap );
        }
    }

void CFreestyleEmailUiAknStatusIndicator::DrawText( const TRect& aRect ) const
    {
    if ( !iVisualText )
        return;
    if ( iVisualText->Length() == 0 )
        return;

    CGraphicsContext::TTextAlign alignment = Layout_Meta_Data::IsMirrored() ?
        CGraphicsContext::ERight : CGraphicsContext::ELeft;

    CWindowGc& gc = SystemGc();
    gc.UseFont( iTextFont );
    TInt baseline = aRect.Height() / 2 + iTextFont->FontMaxAscent() / 2;
    gc.SetPenColor(FontColor());

    TRect drawRect( aRect );
    drawRect.Shrink( (drawRect.Width() - iVisualTextWidth) / 2, 0 );
    gc.DrawText( *iVisualText, drawRect, baseline, alignment, 0);
    }

void CFreestyleEmailUiAknStatusIndicator::SetImage( CFbsBitmap* aBitmap, CFbsBitmap* aMaskBitmap )
    {
    ClearImage();
    iAngle = 0;
    iBitmap = aBitmap;
    iMaskBitmap = aMaskBitmap;
    ScaleImage();
    }

void CFreestyleEmailUiAknStatusIndicator::ScaleImage() const
    {
    if ( iBitmap )
        {
        AknIconUtils::DisableCompression( iBitmap );
        AknIconUtils::SetSizeAndRotation( iBitmap, iImageRect.Size(), EAspectRatioPreserved, iAngle );
        }
    }

void CFreestyleEmailUiAknStatusIndicator::ClearImage()
    {
    delete iBitmap;
    iBitmap = NULL;
    delete iMaskBitmap;
    iMaskBitmap = NULL;
    }

void CFreestyleEmailUiAknStatusIndicator::ClearText()
    {
    delete iText;
    iText = NULL;

    delete iVisualText;
    iVisualText = NULL;
    }

