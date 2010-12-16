/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Recording graphics context used to cache drawing commands
*
*/

#include "cmrrecordinggc.h"

#include <bitdev.h>
#include <graphics/gdi/gdiconsts.h>
#include <graphics/gdi/gdistructs.h>
#include <e32err.h>

#include "emailtrace.h"

namespace
{

#ifdef _DEBUG
_LIT( KPanicCategory, "CMRRecordingGc" );

enum TPanic
    {
    EBadArgument = 0,
    ENotSupported
    };

void Panic( TPanic aPanic )
    {
    User::Panic( KPanicCategory, aPanic );
    }

#endif // _DEBUG

}

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CMRRecordingGc::CMRRecordingGc
// -----------------------------------------------------------------------------
//
CMRRecordingGc::CMRRecordingGc( CWindowGc& aRealGc )
    : CWindowGc( static_cast<CWsScreenDevice*>( aRealGc.Device() ) ),
      iRealGc( aRealGc )
    {
    FUNC_LOG;
    }


// -----------------------------------------------------------------------------
// CMRRecordingGc::~CMRRecordingGc
// -----------------------------------------------------------------------------
//
CMRRecordingGc::~CMRRecordingGc()
    {
    FUNC_LOG;

    iItems.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CMRRecordingGc::FlushBuffer
// -----------------------------------------------------------------------------
//
void CMRRecordingGc::FlushBuffer(
        const TRect& aRect,
        const TRect& aClippingRect )
    {
    FUNC_LOG;

    if ( iItemCount == 0 )
        {
        return;
        }

    CBufferItem* item = NULL;

    // Use real  graphics context
    CBitmapContext* gc = &iRealGc;

    // Set graphics context origin relative to the drawing position
    gc->SetOrigin( aRect.iTl );

    // Set clipping rect relative to origin
    TRect clippingRect( aClippingRect );
    clippingRect.Move( -aRect.iTl );

    // Adjust clipping rect upper left corner to the visible area
    clippingRect.iTl.iY = Max( 0, clippingRect.iTl.iY );
    gc->SetClippingRect( clippingRect );
    TRect currentClippingRect( clippingRect );
    const CFont* currentFont = NULL;

    for ( TInt i = 0; i < iItemCount; ++i )
        {
        item = iItems[i];

        switch ( item->iType )
            {
            case CBufferItem::EShortText:
                {
                TBool draw( ETrue );

                if ( currentFont )
                    {
                    TPoint upper(
                            item->iPosition.iX,
                            item->iPosition.iY - currentFont->AscentInPixels() );
                    TPoint lower(
                            item->iPosition.iX,
                            item->iPosition.iY + currentFont->DescentInPixels() );

                    if ( !currentClippingRect.Contains( upper )
                         && !currentClippingRect.Contains( lower ) )
                        {
                        draw = EFalse;
                        }
                    }

                if ( draw )
                    {
                    gc->DrawText( *item->iText, item->iPosition );
                    }
                }
                break;

            case CBufferItem::ELongText:
                if ( currentClippingRect.Intersects( item->iBox ) )
                    {
                    gc->DrawText(
                            *item->iText,
                            item->iBox,
                            item->iBaseLineOffset,
                            TTextAlign( item->iValue ), //iHorizontal,
                            item->iLeftMargin );
                    }
                break;

            case CBufferItem::ESetPenColor:
                gc->SetPenColor( item->iColor );
                break;

            case CBufferItem::EBitBltMasked1:
                {
                // Bitmap area to blit
                TRect area( item->iPosition, item->iBox.Size() );
                if ( currentClippingRect.Intersects( area ) )
                    {
                    // Bitmap area intersects with current clipping area
                    // Gc will clip the actual bitmap to blit if needed.
                    gc->BitBltMasked(
                            item->iPosition,
                            item->iBitmap,
                            item->iBox,
                            item->iMask,
                            item->iInvertMask );
                    }
                }
                break;

            case CBufferItem::EUseFont:
                gc->UseFont( item->iFont );
                currentFont = item->iFont;
                break;

            case CBufferItem::EDiscardFont:
                gc->DiscardFont();
                currentFont = NULL;
                break;

            case CBufferItem::ESetDrawMode:
                gc->SetDrawMode( TDrawMode( item->iValue ) );
                break;

            case CBufferItem::ESetClippingRect:
                if ( currentClippingRect.Intersects( item->iBox ) )
                    {
                    // Set clipping rect to only visible area
                    TRect box( item->iBox );
                    box.Intersection( currentClippingRect );
                    gc->SetClippingRect( box );
                    currentClippingRect = box;
                    }
                break;

            case CBufferItem::ECancelClippingRect:
                gc->CancelClippingRect();
                gc->SetClippingRect( clippingRect );
                currentClippingRect = clippingRect;
                break;

            case CBufferItem::ESetBrushColor:
                gc->SetBrushColor( item->iColor );
                break;

            case CBufferItem::ESetBrushStyle:
                gc->SetBrushStyle( TBrushStyle( item->iValue ) );//iBrushStyle );
                break;

            case CBufferItem::EDrawRect:
                if ( currentClippingRect.Intersects( item->iRect ) )
                    {
                    TRect rect( item->iRect );
                    rect.Intersection( currentClippingRect );
                    gc->DrawRect( rect );
                    }
                break;

            case CBufferItem::EClear:
                if ( currentClippingRect.Intersects( item->iRect ) )
                    {
                    TRect rect( item->iRect );
                    rect.Intersection( currentClippingRect );
                    gc->Clear( rect );
                    }
                break;

            case CBufferItem::EDrawLine:
                if ( currentClippingRect.Contains( item->iRect.iTl )
                     || currentClippingRect.Contains( item->iRect.iBr ) )
                    {
                    gc->DrawLine( item->iRect.iTl, item->iRect.iBr );
                    }
                break;
            case CBufferItem::ESetUnderlineStyle:
                gc->SetUnderlineStyle( TFontUnderline( item->iValue ) );
                break;
            case CBufferItem::ESetStrikethroughStyle:
                gc->SetStrikethroughStyle( TFontStrikethrough( item->iValue ) );
                break;
            case CBufferItem::EError:
            default:
                break;
            }
        }

    // Reset gc
    gc->Reset();
    }


// -----------------------------------------------------------------------------
// CMRRecordingGc::PurgeBuffer
// -----------------------------------------------------------------------------
//
void CMRRecordingGc::PurgeBuffer()
    {
    FUNC_LOG;

    // Reset actual item count
    iItemCount = 0;
    }

// -----------------------------------------------------------------------------
// CMRRecordingGc::CBufferItem::~CBufferItem
// -----------------------------------------------------------------------------
//
CMRRecordingGc::CBufferItem::~CBufferItem()
    {
    FUNC_LOG;

    delete iText;
    delete iBitmap;
    delete iMask;
    }

// -----------------------------------------------------------------------------
// CMRRecordingGc::CBufferItem::Translate
// -----------------------------------------------------------------------------
//
void CMRRecordingGc::CBufferItem::Translate( const TPoint& aPoint )
    {
    FUNC_LOG;

    iBox.Move( aPoint );
    iPosition += aPoint;
    iRect.Move( aPoint );
    }

// -----------------------------------------------------------------------------
// CMRRecordingGc::CBufferItem::Reset
// -----------------------------------------------------------------------------
//
void CMRRecordingGc::CBufferItem::Reset()
    {
    FUNC_LOG;

    switch ( iType )
        {
        case EShortText:
        case ELongText:
            {
            delete iText;
            iText = NULL;
            break;
            }

        case EBitBltMasked1:
            {
            if ( iMask )
                {
                iMask->Reset();
                }
            // Fall through. Reset bitmap also.
            }

        case EBitBlt1:
        case EBitBlt2:
            {
            if ( iBitmap )
                {
                iBitmap->Reset();
                }
            break;
            }
        case EError:
            {
            if ( iText )
                {
                delete iText;
                iText = NULL;
                }
            if ( iBitmap )
                {
                iBitmap->Reset();
                }
            if ( iMask )
                {
                iMask->Reset();
                }
            break;
            }

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMRRecordingGc::BufferItem
// -----------------------------------------------------------------------------
//
CMRRecordingGc::CBufferItem* CMRRecordingGc::BufferItem(
        CBufferItem::TCommandType aType )
    {
    FUNC_LOG;

    CBufferItem* buffer = NULL;

    if ( iItemCount < iItems.Count() )
        {
        // Items have been allocated in buffer
        // Reset next one if its type is changed
        buffer = iItems[ iItemCount++ ];
        if ( buffer->iType != aType )
            {
            buffer->Reset();
            buffer->iType = aType;
            }
        }
    else
        {
        // Allocate new item
        TRAP_IGNORE(
                {
                buffer = new ( ELeave ) CBufferItem ;
                CleanupStack::PushL( buffer );
                iItems.AppendL( buffer );
                CleanupStack::Pop( buffer );
                buffer->iPosition.SetXY( -1, -1 );
                buffer->iType = aType;
                ++iItemCount;
                } );
        }

    return buffer;
    }

// ---------------------------------------------------------------------------
// CMRRecordingGc::Activate
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::Activate( RDrawableWindow& /*aDevice*/ )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::Deactivate
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::Deactivate()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::Device
// From class CWindowGc
// ---------------------------------------------------------------------------
//
CGraphicsDevice* CMRRecordingGc::Device() const
    {
    FUNC_LOG;

    return NULL;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetOrigin
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetOrigin( const TPoint& aPoint )
    {
    FUNC_LOG;

    iOrigin = aPoint;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetDrawMode
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetDrawMode( TDrawMode aDrawingMode )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ESetDrawMode );

    if ( buffer )
        {
        buffer->iValue = aDrawingMode;
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetClippingRect
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetClippingRect( const TRect& aRect )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ESetClippingRect );

    if ( buffer )
        {
        buffer->iBox = aRect;
        buffer->iPosition = aRect.iTl;
        buffer->Translate( iOrigin );
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::CancelClippingRect
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::CancelClippingRect()
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ECancelClippingRect );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetClippingRegion
// From class CWindowGc
// ---------------------------------------------------------------------------
//
TInt CMRRecordingGc::SetClippingRegion( const TRegion& /*aRegion*/ )
    {
    FUNC_LOG;

    return 0;
    }


// ---------------------------------------------------------------------------
// void CMRRecordingGc::Reset
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::Reset()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::UseFont
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::UseFont( const CFont* aFont )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EUseFont );

    if ( buffer )
        {
        buffer->iFont = aFont;
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DiscardFont
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DiscardFont()
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EDiscardFont );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetUnderlineStyle
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetUnderlineStyle( TFontUnderline aUnderlineStyle )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ESetUnderlineStyle );

    if ( buffer )
        {
        buffer->iValue = aUnderlineStyle;
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetStrikethroughStyle
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetStrikethroughStyle(
        TFontStrikethrough aStrikethroughStyle )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ESetStrikethroughStyle );

    if ( buffer )
        {
        buffer->iValue = aStrikethroughStyle;
        }
    }



// ---------------------------------------------------------------------------
// CMRRecordingGc::SetWordJustification
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetWordJustification(
        TInt /*aExcessWidth*/,
        TInt /*aNumGaps*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetCharJustification
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetCharJustification(
        TInt /*aExcessWidth*/,
        TInt /*aNumChars*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetPenColor
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetPenColor( const TRgb& aColor )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ESetPenColor );

    if ( buffer )
        {
        buffer->iColor = aColor;
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetPenStyle
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetPenStyle( TPenStyle /*aPenStyle*/ )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetPenSize
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetPenSize( const TSize& /*aSize*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetBrushColor
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetBrushColor( const TRgb& aColor )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ESetBrushColor );

    if ( buffer )
        {
        buffer->iColor = aColor;
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetBrushStyle
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetBrushStyle( TBrushStyle aBrushStyle )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ESetBrushStyle );

    if ( buffer )
        {
        buffer->iValue = aBrushStyle;
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetBrushOrigin
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetBrushOrigin( const TPoint& /*aOrigin*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::UseBrushPattern
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::UseBrushPattern( const CFbsBitmap* /*aDevice*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DiscardBrushPattern
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DiscardBrushPattern()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::MoveTo
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::MoveTo( const TPoint& /*aPoint*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::MoveBy
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::MoveBy( const TPoint& /*aPoint*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::Plot
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::Plot( const TPoint& /*aPoint*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawArc
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawArc(
        const TRect& /*aRect*/,
        const TPoint& /*aStart*/,
        const TPoint& /*aEnd*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawLine
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawLine(
        const TPoint& aPoint1,
        const TPoint& aPoint2 )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EDrawLine );

    if ( buffer )
        {
        buffer->iRect.iTl = aPoint1;
        buffer->iRect.iBr = aPoint2;
        buffer->Translate( iOrigin );
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawLineTo
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawLineTo( const TPoint& /*aPoint*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawLineBy
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawLineBy( const TPoint& /*aPoint*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawPolyLine
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawPolyLine( const CArrayFix<TPoint>* /*aPointList*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawPolyLine
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawPolyLine(
        const TPoint* /*aPointList*/,
        TInt /*aNumPoints*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawPie
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawPie(
        const TRect& /*aRect*/,
        const TPoint& /*aStart*/,
        const TPoint& /*aEnd*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawEllipse
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawEllipse( const TRect& /*aRect*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawRect
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawRect( const TRect& aRect )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EDrawRect );

    if ( buffer )
        {
        buffer->iRect = aRect;
        buffer->Translate( iOrigin );
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawRoundRect
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawRoundRect(
        const TRect& /*aRect*/,
        const TSize& /*aEllipse*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawPolygon
// From class CWindowGc
// ---------------------------------------------------------------------------
//
TInt CMRRecordingGc::DrawPolygon(
        const CArrayFix<TPoint>* /*aPointList*/,
        TFillRule /*aFillRule*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );

    return KErrNotSupported;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawPolygon
// From class CWindowGc
// ---------------------------------------------------------------------------
//
TInt CMRRecordingGc::DrawPolygon(
        const TPoint* /*aPointList*/,
        TInt /*aNumPoints*/,
        TFillRule /*aFillRule*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );

    return KErrNotSupported;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawBitmap
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawBitmap(
        const TPoint& /*aTopLeft*/,
        const CFbsBitmap* /*aDevice*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawBitmap
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawBitmap(
        const TRect& /*aDestRect*/,
        const CFbsBitmap* /*aDevice*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawBitmap
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawBitmap(
        const TRect& /*aDestRect*/,
        const CFbsBitmap* /*aDevice*/,
        const TRect& /*aSourceRect*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawBitmapMasked
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawBitmapMasked(
        const TRect& /*aDestRect*/,
        const CFbsBitmap* /*aBitmap*/,
        const TRect& /*aSourceRect*/,
        const CFbsBitmap* /*aMaskBitmap*/,
        TBool /*aInvertMask*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawBitmapMasked
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawBitmapMasked(
        const TRect& /*aDestRect*/,
        const CWsBitmap* /*aBitmap*/,
        const TRect& /*aSourceRect*/,
        const CWsBitmap* /*aMaskBitmap*/,
        TBool /*aInvertMask*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawText
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawText( const TDesC& aBuf, const TPoint& aPos )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EShortText );

    if ( buffer )
        {
        // Reuse existing buffer if possible
        if ( buffer->iText
             && aBuf.Length() <= buffer->iText->Des().MaxLength() )
            {
            buffer->iText->Des().Copy( aBuf );
            }
        else
            {
            // Realloc new buffer
            delete buffer->iText;
            buffer->iText = NULL;
            buffer->iText = aBuf.Alloc();
            if ( !buffer->iText )
                {
                // Tag error
                buffer->iType = CBufferItem::EError;
                return;
                }
            }
        buffer->iPosition = aPos;
        buffer->Translate( iOrigin );
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawText
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawText(
        const TDesC& aBuf,
        const TRect& aBox,
        TInt aBaselineOffset,
        TTextAlign aHoriz,
        TInt aLeftMrg )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::ELongText );

    if ( buffer )
        {
        // Reuse existing buffer if possible
        if ( buffer->iText
             && aBuf.Length() <= buffer->iText->Des().MaxLength() )
            {
            buffer->iText->Des().Copy( aBuf );
            }
        else
            {
            // Realloc new buffer
            delete buffer->iText;
            buffer->iText = NULL;
            buffer->iText = aBuf.Alloc();
            if ( !buffer->iText )
                {
                // Tag error
                buffer->iType = CBufferItem::EError;
                return;
                }
            }
        buffer->iBox = aBox;
        buffer->iBaseLineOffset = aBaselineOffset;
        buffer->iValue = aHoriz;
        buffer->iLeftMargin = aLeftMrg;
        buffer->iPosition = aBox.iTl;
        buffer->Translate( iOrigin );
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::Clear
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::Clear()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::Clear
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::Clear( const TRect& aRect )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EClear );

    if ( buffer )
        {
        buffer->iRect = aRect;
        buffer->Translate( iOrigin );
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::CopyRect
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::CopyRect(
        const TPoint& /*aOffset*/,
        const TRect& /*aRect */ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::BitBlt
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::BitBlt(
        const TPoint& /*aPos*/,
        const CFbsBitmap* /*aBitmap*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::BitBlt
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::BitBlt(
        const TPoint& aDestination,
        const CFbsBitmap* aBitmap,
        const TRect& aSource )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EBitBlt2 );

    if ( buffer )
        {
        buffer->iPosition = aDestination + iOrigin;

        // Duplicate bitmap
        if ( buffer->iBitmap )
            {
            // Check if bitmap handles are different
            if ( buffer->iBitmap->Handle() != aBitmap->Handle() )
                {
                buffer->iBitmap->Reset();
                if ( buffer->iBitmap->Duplicate( aBitmap->Handle() ) )
                    {
                    buffer->iType = CBufferItem::EError;
                    return;
                    }
                }
            }
        else
            {
            CFbsBitmap* bitmap = NULL;
            TRAPD( error,
                    {
                    bitmap = new( ELeave ) CFbsBitmap;
                    User::LeaveIfError(
                            bitmap->Duplicate( aBitmap->Handle() ) );
                    } )
            if ( error )
                {
                buffer->iType = CBufferItem::EError;
                return;
                }

            buffer->iBitmap = bitmap;
            }
        buffer->iBox = aSource;
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::BitBltMasked
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::BitBltMasked(
        const TPoint& aPoint,
        const CFbsBitmap* aBitmap,
        const TRect& aSourceRect,
        const CFbsBitmap* aMaskBitmap,
        TBool aInvertMask )
    {
    FUNC_LOG;

    CBufferItem* buffer = BufferItem( CBufferItem::EBitBltMasked1 );

    if ( buffer )
        {
        buffer->iPosition = aPoint + iOrigin;
        buffer->iBox = aSourceRect;
        buffer->iInvertMask = aInvertMask;

        // Make a duplicate of passed in bitmap, so that to make sure
        // the bitmap won't be freed when FlushBuffer(). (Duplicate
        // only increases the access count for bitmap)
        if ( buffer->iBitmap )
            {
            if ( buffer->iBitmap->Handle() != aBitmap->Handle() )
                {
                buffer->iBitmap->Reset();
                if ( buffer->iBitmap->Duplicate( aBitmap->Handle() ) )
                    {
                    buffer->iType = CBufferItem::EError;
                    return;
                    }
                }
            }
        else
            {
            CFbsBitmap* bitmap = NULL;
            TRAPD( error,
                    {
                    bitmap = new (ELeave) CFbsBitmap;
                    User::LeaveIfError(
                            bitmap->Duplicate(aBitmap->Handle() ) );
                    } )

            buffer->iBitmap = bitmap;
            if ( error )
                {
                buffer->iType = CBufferItem::EError;
                return;
                }
            }

        if ( buffer->iMask )
            {
            if ( buffer->iMask->Handle() != aMaskBitmap->Handle() )
                {
                buffer->iMask->Reset();
                if ( buffer->iMask->Duplicate( aMaskBitmap->Handle() ) )
                    {
                    buffer->iType = CBufferItem::EError;
                    return;
                    }
                }
            }
        else
            {
            CFbsBitmap* maskBitmap = NULL;
            TRAPD( error,
                    {
                    maskBitmap = new (ELeave) CFbsBitmap;
                    User::LeaveIfError(
                            maskBitmap->Duplicate(aMaskBitmap->Handle() ) );
                    } )

            buffer->iMask = maskBitmap;

            if ( error )
                {
                buffer->iType = CBufferItem::EError;
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::BitBlt
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::BitBlt(
        const TPoint& /*aPoint*/,
        const CWsBitmap* /*aBitmap*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::BitBlt
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::BitBlt(
        const TPoint& /*aDestination*/,
        const CWsBitmap* /*aBitmap*/,
        const TRect& /*aSource*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::BitBltMasked
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::BitBltMasked(
        const TPoint& /*aPoint*/,
        const CWsBitmap* /*aBitmap*/,
        const TRect& /*aSourceRect*/,
        const CWsBitmap* /*aMaskBitmap*/,
        TBool /*aInvertMask*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::MapColors
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::MapColors(
        const TRect& /*aRect*/,
        const TRgb* /*aColors*/,
        TInt /*aNumPairs*/,
        TBool /*aMapForwards*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawTextVertical
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawTextVertical(
        const TDesC& /*aText*/,
        const TPoint& /*aPos*/,
        TBool /*aUp*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::DrawTextVertical
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::DrawTextVertical(
        const TDesC& /*aText*/,
        const TRect& /*aBox*/,
        TInt /*aBaselineOffset*/,
        TBool /*aUp*/,
        TTextAlign /*aVert*/,
        TInt /*aMargin*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetDitherOrigin
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetDitherOrigin( const TPoint& /*aPoint*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::CancelClippingRegion
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::CancelClippingRegion()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetOpaque
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetOpaque( TBool /*aDrawOpaque*/ )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetFaded
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetFaded( TBool /*aFaded*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::SetFadingParameters
// From class CWindowGc
// ---------------------------------------------------------------------------
//
void CMRRecordingGc::SetFadingParameters(
        TUint8 /*aBlackMap*/,
        TUint8 /*aWhiteMap*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::AlphaBlendBitmaps
// From class CWindowGc
// ---------------------------------------------------------------------------
//
TInt CMRRecordingGc::AlphaBlendBitmaps(
        const TPoint& /*aDestPt*/,
        const CFbsBitmap* /*aSrcBmp*/,
        const TRect& /*aSrcRect*/,
        const CFbsBitmap* /*aAlphaBmp*/,
        const TPoint& /*aAlphaPt*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );

    return KErrNotSupported;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::AlphaBlendBitmaps
// From class CWindowGc
// ---------------------------------------------------------------------------
//
TInt CMRRecordingGc::AlphaBlendBitmaps(
        const TPoint& /*aDestPt*/,
        const CWsBitmap* /*aSrcBmp*/,
        const TRect& /*aSrcRect*/,
        const CWsBitmap* /*aAlphaBmp*/,
        const TPoint& /*aAlphaPt*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( EFalse, Panic( ENotSupported ) );

    return KErrNotSupported;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::Interface
// From class CWindowGc
// ---------------------------------------------------------------------------
//
TAny* CMRRecordingGc::Interface( TUid /*aInterfaceId*/ )
    {
    FUNC_LOG;

    return NULL;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::Interface
// From class CWindowGc
// ---------------------------------------------------------------------------
//
const TAny* CMRRecordingGc::Interface( TUid /*aInterfaceId*/ ) const
    {
    FUNC_LOG;

    return NULL;
    }


// ---------------------------------------------------------------------------
// CMRRecordingGc::APIExtension
// From class CWindowGc
// ---------------------------------------------------------------------------
//
TInt CMRRecordingGc::APIExtension(
        TUid aUid,
        TAny*& /*aOutput*/,
        TAny* aInput )
    {
    FUNC_LOG;

    if (aUid == KDrawTextInContextUid)
        {
        __ASSERT_DEBUG( aInput, Panic( EBadArgument ) );
        TDrawTextInContextInternal* contextParam =
            static_cast<TDrawTextInContextInternal*>(aInput);
        const TTextParameters* params = &contextParam->iParam;
        __ASSERT_DEBUG( params, Panic( EBadArgument ) );
        TPtrC textToDraw = contextParam->iText.Mid(
                params->iStart,
                params->iEnd - params->iStart );
        DrawText( textToDraw, contextParam->iPosition );
		return KErrNone;
        }
    else
        {
		return KErrNotSupported;
        }
    }
