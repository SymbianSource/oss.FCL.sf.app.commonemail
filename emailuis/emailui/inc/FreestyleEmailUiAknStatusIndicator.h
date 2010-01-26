/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*/ 


#ifndef __FREESTYLEEMAILUI_AKN_STATUS_INDICATOR_H__
#define __FREESTYLEEMAILUI_AKN_STATUS_INDICATOR_H__

#include <coecntrl.h>

#include "FreestyleEmailUiUtilities.h"  // FSEmailUiGenericTimer

const TInt KStatusIndicatorAutomaticHidingDuration = 3000;
const TInt KStatusIndicatorDefaultDuration = -1;

class CFreestyleEmailUiAknStatusIndicator : public CCoeControl,
                                            protected MFSEmailUiGenericTimerCallback
    {
public:
    IMPORT_C static CFreestyleEmailUiAknStatusIndicator* NewL( const TRect& aRect, const CCoeControl* aParent = NULL );
    
    IMPORT_C static CFreestyleEmailUiAknStatusIndicator* NewLC( const TRect& aRect, const CCoeControl* aParent = NULL );
    
    ~CFreestyleEmailUiAknStatusIndicator();
    
    // The ownships of aBitmap, aMaskBitmap and aText are transferred in.
    // The status indicator is display for aDuration millisecond if aDuration is not negative;
    // otherwise, it is diplayed forever until it is manually hidden.
    void ShowIndicatorL( CFbsBitmap* aBitmap, 
                         CFbsBitmap* aMaskBitmap,
                         TDesC* aText,
                         TInt aDuration );

    void HideIndicator( TInt aDelayBeforeHidingInMs = 0 );
    
    // The ownship of aText is transferred in.
    void SetTextL( TDesC* aText );
    
    // CCoeControl
    CCoeControl* ComponentControl( TInt aIndex ) const;
    TInt CountComponentControls() const;
    void Draw( const TRect& aRect ) const;
    void SizeChanged();
    
    inline const CFbsBitmap* Image() const 
        {
        return iBitmap;
        }
    
    inline const CFbsBitmap* ImageMask() const 
        {
        return iMaskBitmap;
        }
    
    inline const TDesC* Text() const
        {
        return iText;
        }
    
protected:    
    CFreestyleEmailUiAknStatusIndicator();
    void ConstructL( const TRect& aRect, const CCoeControl* aParent );

    void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );

private:
    void CalculateLayout( const TRect& aRect ) const;
    
    void SetTextFont() const;
    void CalculateVisualText() const;

    void DrawBoundary( const TRect& aRect ) const;
    void DrawImage( const TRect& aRect ) const;
    void DrawText( const TRect& aRect ) const;
    
    void SetImage( CFbsBitmap* aBitmap, CFbsBitmap* aMaskBitmap );
    void ScaleImage() const;
    
    void ClearImage();
    void ClearText();
    
private:
    CFbsBitmap*                 iBitmap; 
    CFbsBitmap*                 iMaskBitmap;
    TDesC*                      iText;

    CFSEmailUiGenericTimer*     iTimer;
    
    mutable TRect               iImageRect;
    mutable TRect               iTextRect;
    
    mutable HBufC*              iVisualText;
    mutable CFont*              iTextFont;
    };

#endif // __FREESTYLEEMAILUI_AKN_STATUS_INDICATOR_H__
