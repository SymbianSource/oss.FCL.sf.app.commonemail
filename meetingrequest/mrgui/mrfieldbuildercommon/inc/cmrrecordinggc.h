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
* Description:  Recording graphics context used to cache drawing commands.
*
*/


#include <w32std.h>
#include <gdi.h>

class CFbsBitmap;
class CFbsBitGc;
class CFbsBitmapDevice;

NONSHARABLE_CLASS( CMRRecordingGc ) : public CWindowGc
    {
public:

    CMRRecordingGc( CWindowGc& aRealGc );
	~CMRRecordingGc();

public:

	/**
	 * Flush recorded drawing commands from buffer to the real
	 * graphics context.
	 *
	 * @param aRect Target rectangle to draw
	 * @param aClippingRect Clipping rectangle to use
	 */
	void FlushBuffer(
	        const TRect& aRect,
	        const TRect& aClippingRect );

	/**
	 * Delete recorded drawing commands
	 */
    void PurgeBuffer();

public: // From CGraphicsContext

    void SetOrigin(const TPoint &aPoint=TPoint(0,0));

protected:

    // From CWindowGc

	void Activate( RDrawableWindow &aDevice );
	void Deactivate();

	// From CGraphicsContext
	CGraphicsDevice* Device() const;
	void SetDrawMode(TDrawMode aDrawingMode);
	void SetClippingRect(const TRect& aRect);
	void CancelClippingRect();
	void Reset();

	void UseFont(const CFont *aFont);
	void DiscardFont();
	void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	void SetStrikethroughStyle( TFontStrikethrough aStrikethroughStyle );

	void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
	void SetCharJustification(TInt aExcessWidth,TInt aNumChars);

	void SetPenColor(const TRgb &aColor);
	void SetPenStyle(TPenStyle aPenStyle);
	void SetPenSize(const TSize& aSize);

	void SetBrushColor(const TRgb &aColor);
	void SetBrushStyle(TBrushStyle aBrushStyle);
	void SetBrushOrigin(const TPoint &aOrigin);
	void UseBrushPattern(const CFbsBitmap *aDevice);
	void DiscardBrushPattern();

	void MoveTo(const TPoint &aPoint);
	void MoveBy(const TPoint &aPoint);
	void Plot(const TPoint &aPoint);

	void DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	void DrawLine(const TPoint &aPoint1,const TPoint &aPoint2);
	void DrawLineTo(const TPoint &aPoint);
	void DrawLineBy( const TPoint& aPoint );

	void DrawPolyLine(const CArrayFix<TPoint> *aPointList);
	void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);

	void DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	void DrawEllipse(const TRect &aRect);
	void DrawRect(const TRect &aRect);
	void DrawRoundRect(const TRect &aRect,const TSize &aEllipse);
	TInt DrawPolygon(const CArrayFix<TPoint> *aPointList,TFillRule aFillRule=EAlternate);
	TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule=EAlternate);

	void DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aDevice);
	void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice);
	void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice, const TRect &aSourceRect);
	void DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask);
	void DrawBitmapMasked(const TRect& aDestRect, const CWsBitmap* aBitmap, const TRect& aSourceRect, const CWsBitmap* aMaskBitmap, TBool aInvertMask);

	void DrawText(const TDesC &aBuf,const TPoint &aPos);
	void DrawText(const TDesC &aBuf,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz=ELeft,TInt aLeftMrg=0);

	// From CBitmapContext
	void Clear();
	void Clear(const TRect &aRect);
	void CopyRect(const TPoint &anOffset,const TRect &aRect);
	void BitBlt(const TPoint &aPos, const CFbsBitmap *aBitmap);
	void BitBlt(const TPoint &aDestination, const CFbsBitmap *aBitmap, const TRect &aSource);
	void BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	void BitBlt(const TPoint &aPoint, const CWsBitmap *aBitmap);
	void BitBlt(const TPoint &aDestination, const CWsBitmap *aBitmap, const TRect &aSource);
	void BitBltMasked(const TPoint& aPoint,const CWsBitmap *aBitmap,const TRect& aSourceRect,const CWsBitmap *aMaskBitmap,TBool aInvertMask);
	void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs=2,TBool aMapForwards=ETrue);

	void DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp);
	void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);

	//=================Extra functions specific to wserv GDI==============
	void SetDitherOrigin(const TPoint& aPoint);
	TInt SetClippingRegion(const TRegion &aRegion);
	void CancelClippingRegion();
	void SetOpaque(TBool aDrawOpaque=ETrue);

	// From CFbsBitGc
	void SetFaded(TBool aFaded);
	void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);
	TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt);
	TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp, const TRect& aSrcRect, const CWsBitmap* aAlphaBmp, const TPoint& aAlphaPt);

	TAny* Interface( TUid aInterfaceId );
    const TAny* Interface( TUid aInterfaceId ) const;

protected:
    TInt APIExtension( TUid aUid, TAny*& aOutput, TAny* aInput );

private:

    /**
     * Buffer item to store drawing command and parameters
     */
    NONSHARABLE_CLASS( CBufferItem ) : public CBase
        {
        public:
        enum TCommandType
            {
            EShortText,
            ELongText,
            ESetPenColor,
            EBitBlt1,
            EBitBlt2,
            EBitBltMasked1,
            EUseFont,
            EDiscardFont,
            ESetDrawMode,
            ESetClippingRect,
            ECancelClippingRect,
            ESetBrushColor,
            ESetBrushStyle,
            EDrawRect,
            EClear,
            EDrawLine,
            ESetUnderlineStyle,
            ESetStrikethroughStyle,
            EError // General error. To be omitted when flushing commands
            };

        public:
            ~CBufferItem();

            /**
             * Translated commaned with given point
             */
            void Translate( const TPoint& aPoint );

            /**
             * Resets item for reuse
             */
            void Reset();

            TInt iType;
            HBufC* iText;
            TPoint iPosition;
            TRect iBox;
            TInt iBaseLineOffset;
            TInt iLeftMargin;
            TRgb iColor;
            CFbsBitmap* iBitmap;
            CFbsBitmap* iMask;
            TRect iSource;
            TBool iInvertMask;
            const CFont* iFont;
            TRect iRect;
            TInt iValue;
        };

    CBufferItem* BufferItem( CBufferItem::TCommandType aType );

    /// Ref: Real window GC for actual drawing
    CWindowGc& iRealGc;
    /// Own: Array of recorded drawing commands
    RPointerArray<CBufferItem> iItems;
    /// Own: Recording origin relative to real context
    TPoint iOrigin;
    /// Own: current count of recorded commands
    TInt iItemCount;

    };

