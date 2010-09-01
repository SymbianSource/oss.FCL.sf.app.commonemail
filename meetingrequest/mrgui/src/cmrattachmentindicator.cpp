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
* Description: Attachment indicator implementation
*
*/

#include "cmrattachmentindicator.h"

#include <biditext.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <AknBidiTextUtils.h>

namespace
{
const TInt KImageMargin = 8;
const TInt KTextMargin = 4;
const TInt KRoundBoundaryEllipse = 5;
const TInt KTruncationCharsWidth = 20;
}


// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::CMRAttachmentIndicator
// ---------------------------------------------------------------------------
//
CMRAttachmentIndicator::CMRAttachmentIndicator()
    : iBitmap( NULL ),
    iMaskBitmap( NULL ),
    iText( NULL ),
    iVisualText( NULL ),
    iTextFont( NULL )
    {
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::~CMRAttachmentIndicator
// ---------------------------------------------------------------------------
//
CMRAttachmentIndicator::~CMRAttachmentIndicator()
    {
    ClearImage();
    ClearText();
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::CMRAttachmentIndicator
// ---------------------------------------------------------------------------
//
CMRAttachmentIndicator* CMRAttachmentIndicator::NewL(
        const TRect& aRect,
        const CCoeControl* aParent )
    {
    CMRAttachmentIndicator* self = new (ELeave) CMRAttachmentIndicator();
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::ConstructL
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::ConstructL(
        const TRect& aRect,
        const CCoeControl* aParent )
    {
    CreateWindowL( aParent );

    SetRect( aRect );
    ActivateL();

    MakeVisible( EFalse );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::ShowIndicatorL
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::ShowIndicatorL(
        CFbsBitmap* aBitmap,
        CFbsBitmap* aMaskBitmap,
        const TDesC& aText,
        TInt /*aDuration*/ )
    {
    SetImage( aBitmap, aMaskBitmap );
    SetTextL( aText );

    MakeVisible( ETrue );
    DrawNow();
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::SetTextL
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::SetTextL( const TDesC& aText )
    {
    SetTextFont();

    ClearText();

    iText = aText.AllocL();
    if ( aText.Length() )
        {
        iVisualText = HBufC::NewL( aText.Length() + KAknBidiExtraSpacePerLine );
        CalculateVisualText();
        }

    if ( IsVisible() )
        {
        DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::SetImage
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::SetImage(
        CFbsBitmap* aBitmap,
        CFbsBitmap* aMaskBitmap )
    {
    ClearImage();
    iBitmap = aBitmap;
    iMaskBitmap = aMaskBitmap;
    ScaleImage();
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::HideIndicator
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::HideIndicator( TInt aDelayBeforeHidingInMs )
    {
    if ( aDelayBeforeHidingInMs == 0 )
        {
        MakeVisible( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRAttachmentIndicator::CMRAttachmentIndicator::ComponentControl( TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentIndicator::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::Draw
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );

    DrawBoundary( aRect );
    DrawImage( iImageRect );
    DrawText( iTextRect );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::SizeChanged()
    {
    CalculateLayout( Rect() );
    ScaleImage();
    CalculateVisualText();
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::Image
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CMRAttachmentIndicator::Image() const
    {
    return iBitmap;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::ImageMask
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CMRAttachmentIndicator::ImageMask() const
    {
    return iMaskBitmap;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::Text
// ---------------------------------------------------------------------------
//
const TDesC& CMRAttachmentIndicator::Text() const
    {
    return *iText;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::CalculateLayout
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::CalculateLayout( const TRect& aRect ) const
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

    TLanguage language = User::Language();
    TBidiText::TDirectionality  direction = TBidiText::ScriptDirectionality( language );
    if ( direction == TBidiText::ELeftToRight )
        {
        TPoint imageLeftTop ( aRect.iTl.iX + KImageMargin, aRect.iTl.iY + KImageMargin );
        iImageRect.SetRect( imageLeftTop, imageSize );

        TPoint textLeftTop ( aRect.iTl.iX + 2*KImageMargin + imageSize.iWidth + KTextMargin, aRect.iTl.iY + KTextMargin );
        iTextRect.SetRect( textLeftTop, textSize );
        }
    else
        {
        TPoint imageLeftTop ( aRect.iBr.iX - KImageMargin - imageSize.iWidth, aRect.iTl.iY + KImageMargin );
        iImageRect.SetRect( imageLeftTop, imageSize );

        TPoint textLeftTop ( aRect.iTl.iX + KTextMargin, aRect.iTl.iY + KTextMargin );
        iTextRect.SetRect( textLeftTop, textSize );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::SetTextFont
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::SetTextFont() const
    {
    if ( iTextFont )
        return;

    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont );
    iTextFont = CONST_CAST( CFont*, font );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::CalculateVisualText
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::CalculateVisualText() const
    {
    if ( iVisualText )
        {
        TPtr visualText = iVisualText->Des();
        visualText.Zero();
        TInt MaxWidthInPixels = iTextRect.Size().iWidth - KTruncationCharsWidth;
        TInt MaxClippedWidthInPixels = iTextRect.Size().iWidth;
        AknBidiTextUtils::ConvertToVisualAndClip( *iText,
                                                  visualText,
                                                  *iTextFont,
                                                  MaxWidthInPixels,
                                                  MaxClippedWidthInPixels );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::DrawBoundary
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::DrawBoundary( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenColor( KRgbCyan );
    TSize ellipse( KRoundBoundaryEllipse, KRoundBoundaryEllipse );
    gc.DrawRoundRect( aRect, ellipse);
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::DrawImage
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::DrawImage( const TRect& aRect ) const
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

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::DrawText
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::DrawText( const TRect& aRect ) const
    {
    if ( !iVisualText )
        return;
    if ( iVisualText->Length() == 0 )
        return;

    TLanguage language = User::Language();
    TBidiText::TDirectionality  direction = TBidiText::ScriptDirectionality( language );

    CGraphicsContext::TTextAlign alignment = CGraphicsContext::ELeft;
    if ( direction == TBidiText::ERightToLeft )
        {
        alignment = CGraphicsContext::ERight;
        }

    CWindowGc& gc = SystemGc();
    gc.UseFont( iTextFont );

    TInt baseline = aRect.Height() / 2 + iTextFont->FontMaxAscent() / 2;

    gc.SetPenColor(KRgbBlack);

    gc.DrawText( *iVisualText, aRect, baseline, alignment, 0);
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::ScaleImage
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::ScaleImage() const
    {
    if ( iBitmap )
        {
        AknIconUtils::DisableCompression( iBitmap );
        AknIconUtils::SetSize( iBitmap, iImageRect.Size(), EAspectRatioPreserved );
        }
    if ( iMaskBitmap )
        {
        AknIconUtils::DisableCompression( iMaskBitmap );
        AknIconUtils::SetSize( iMaskBitmap, iImageRect.Size(), EAspectRatioPreserved );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::ClearImage
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::ClearImage()
    {
    delete iBitmap;
    iBitmap = NULL;
    delete iMaskBitmap;
    iMaskBitmap = NULL;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentIndicator::ClearText
// ---------------------------------------------------------------------------
//
void CMRAttachmentIndicator::ClearText()
    {
    delete iText;
    iText = NULL;

    delete iVisualText;
    iVisualText = NULL;
    }

// EOF
