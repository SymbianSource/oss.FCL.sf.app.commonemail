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
* Description: Attachment indicator class definition
* 
*/ 

#ifndef CMRATTACHMENTINDICATOR_H
#define CMRATTACHMENTINDICATOR_H

#include <coecntrl.h>

/**
 * Attachment indicator
 */
NONSHARABLE_CLASS( CMRAttachmentIndicator ) : 
        public CCoeControl                                            
    {
public: // Construction and destruction
    
	/**
	 * Two-phased constructor.
	 * Creates a new instance of class
	 * 
	 * @param aRect   rect for this control
	 * @param aParent parent control
	 * 
	 * @return CMRAttachmentIndicator instance
	 */
    static CMRAttachmentIndicator* NewL( 
            const TRect& aRect, 
            const CCoeControl* aParent );
    
    /**
     * C++ Destructor.
     */    
    ~CMRAttachmentIndicator();

public: // new methods
    
    /**
     * Shows the indicator in UI. The ownships of aBitmap, aMaskBitmap and aText 
     * are transferred in. The status indicator is displayed for aDuration 
     * millisecond if aDuration is not negative; otherwise, it is diplayed forever 
     * until it is manually hidden.
     * 
     * @param aBitmap     Indicator image bitmap
     * @param aMaskBitmap Indicator image bitmap mask
     * @param aText       Indicator text
     * @param aDuration   Duration for the indicator to be shown
     */ 
    void ShowIndicatorL( 
            CFbsBitmap* aBitmap, 
            CFbsBitmap* aMaskBitmap,
            const TDesC& aText,
            TInt aDuration );

    /**
     * Hides the indicator from UI
     * 
     * @param aDelayBeforeHidingInMs Delay before the indicator is hidden
     *                               in milliseconds
     */
    void HideIndicator( 
            TInt aDelayBeforeHidingInMs );
    
    /**
     * Setter for indicator text. The ownship of aText is transferred in.
     * 
     * @param aText text for indicator
     */
    void SetTextL( 
            const TDesC& aText );

    /**
     * Setter for indicator image.
     * 
     * @param aBitmap     image bitmap for indicator
     * @param aMaskBitmap image bitmap mask for indicator
     */
    void SetImage( 
            CFbsBitmap* aBitmap, 
            CFbsBitmap* aMaskBitmap );
    
    /**
     * Getter for indicator image bitmap
     * 
     * @return the indicator image bitmap
     */
    const CFbsBitmap* Image() const;
    
    /**
     * Getter for indicator image bitmap mask
     * 
     * @return the indicator image bitmap mask
     */    
    const CFbsBitmap* ImageMask() const;    
    
    /**
     * Getter for indicator text
     * 
     * @return the indicator text
     */    
    const TDesC& Text() const;
    
public:    // From CCoeControl
    CCoeControl* ComponentControl( 
            TInt aIndex ) const;
    TInt CountComponentControls() const;
    void Draw( 
            const TRect& aRect ) const;
    void SizeChanged();
    
private:    
    
    CMRAttachmentIndicator();
    void ConstructL( 
            const TRect& aRect, 
            const CCoeControl* aParent );
    void CalculateLayout( 
            const TRect& aRect ) const;
    
    void SetTextFont() const;
    void CalculateVisualText() const;

    void DrawBoundary( const TRect& aRect ) const;
    void DrawImage( const TRect& aRect ) const;
    void DrawText( const TRect& aRect ) const;
    void ScaleImage() const;
    
    void ClearImage();
    void ClearText();
    
private:
    /// own: Indicator image bitmap
    CFbsBitmap*                 iBitmap; 
    /// own: Indicator image bitmap mask
    CFbsBitmap*                 iMaskBitmap;
    /// own: Indicator text
    HBufC*                      iText;   
    // Rect for indicator image
    mutable TRect               iImageRect;
    // Rect for indicator text
    mutable TRect               iTextRect;
    /// own: Indicator visual text (final text shown in UI)
    mutable HBufC*              iVisualText;
    /// not owned: Indicator text font
    mutable CFont*              iTextFont;
    };

#endif // CMRATTACHMENTINDICATOR_H
