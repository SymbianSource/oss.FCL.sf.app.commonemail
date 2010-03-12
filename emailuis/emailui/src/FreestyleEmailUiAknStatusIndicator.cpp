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

const TInt KImageMargin = 8;
const TInt KTextMargin = 4;
const TInt KRoundBoundaryEllipse = 5;
const TInt KTuncationCharsWidth = 20;    


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
                                                          TInt aDuration )
    {
    iTimer->Stop();

    SetImage( aBitmap, aMaskBitmap );
    SetTextL( aText );

    MakeVisible( ETrue );
    DrawNow();
    
    if ( aDuration >= 0 )
        {
        HideIndicator( aDuration );
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
    iTimer->Stop();
    
    if ( aDelayBeforeHidingInMs == 0 )
        {
        MakeVisible( EFalse );
        }
    else
        {
        iTimer->Start( aDelayBeforeHidingInMs );
        }
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
    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );
    
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
    
    if ( !aParent )
        {
        CreateWindowL();
        }
    else
        {
        SetContainerWindowL( *aParent );
        }
    
    SetRect( aRect );
    ActivateL();
    
    MakeVisible( EFalse );
    }

void CFreestyleEmailUiAknStatusIndicator::TimerEventL( CFSEmailUiGenericTimer* /*aTriggeredTimer*/ )
    {
    HideIndicator();
    }

void CFreestyleEmailUiAknStatusIndicator::CalculateLayout( const TRect& aRect ) const
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
        }
    }

void CFreestyleEmailUiAknStatusIndicator::DrawBoundary( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenColor( KRgbCyan );
    TSize ellipse( KRoundBoundaryEllipse, KRoundBoundaryEllipse );
    gc.DrawRoundRect( aRect, ellipse);
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

void CFreestyleEmailUiAknStatusIndicator::SetImage( CFbsBitmap* aBitmap, CFbsBitmap* aMaskBitmap )
    {
    ClearImage();
    iBitmap = aBitmap;
    iMaskBitmap = aMaskBitmap;
    ScaleImage();
    }

void CFreestyleEmailUiAknStatusIndicator::ScaleImage() const
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

