/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file implements overlay controls
*
*/

#ifndef CTESTOVERLAYCONTROL_H
#define CTESTOVERLAYCONTROL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>
#include <eikenv.h>
#include <AknIconUtils.h>
#include <freestyleemailui.mbg>
#include "FreestyleEmailUiUtilities.h"

// forward declarations.	
class RWsSession;
class TFsEmailUiUtility;

// CLASS DECLARATION
class COverlayControl;

/**
 * Observer for overlay control touch events
 */
class MOverlayControlObserver
    {
public:
    virtual void HandleOverlayPointerEventL( COverlayControl* aControl, const TPointerEvent& aEvent ) = 0;
    };

/**
 *  COverlayControl
 */
class COverlayControl : public CCoeControl
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~COverlayControl();

	/**
	 * Two-phased constructor.
	 * @param aParent if not NULL, parent control
	 * @param aObserver observer for touch events
	 * @param aRect initial size and position
	 * @param aBitmapId bitmap to be used
	 * @param aMaskId mask to be used
	 */
	static COverlayControl* NewL( CCoeControl* aParent, MOverlayControlObserver* aObserver, 
	        const TRect& aRect, TInt aBitmapId, TInt aMaskId );

	void Draw( const TRect& ) const;

	void SetRect( const TRect& aRect );

	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	
	void MakeVisible( TBool aVisible );

	   /**
	     * Update visual state
	     * @param aButtonDown ETrue if button is in down state
	     */
	void UpdateButtonState( TBool aButtonDown );
	
private:

	/**
	 * Constructor for performing 1st stage construction
	 * @param aObserver observer for touch events
	 */
	COverlayControl( MOverlayControlObserver* aObserver );

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL( CCoeControl* aParent, const TRect& aRect, TInt aBitmapId, TInt aMaskId );

	/**
	 * Make a copy of bitmap
	 */
    void CopyBitmapL( const CFbsBitmap& aSource, CFbsBitmap& aDest );
    
    /**
     * Adjust alpha for the bitmap
     * @param aBitmap bitmap to adjust
     * @param aFactor factor to apply
     */
    void AdjustAlpha( CFbsBitmap& aBitmap, TReal aFactor );
    
    /**
     * Update used mask according to current button state
     */
    void UpdateMask();
	
private:
    
	CFbsBitmap* iBitmap;
	CFbsBitmap* iMask;
	CFbsBitmap* iSolidMask;
	TBool iDownState;
	
	MOverlayControlObserver* iObserver;
	};

#endif // CTESTOVERLAYCONTROL_H
