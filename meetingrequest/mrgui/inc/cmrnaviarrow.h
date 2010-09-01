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
* Description:  Navigation arrows used to replace the navipane.
*
*/

#ifndef CMRNAVIARROW_H_
#define CMRNAVIARROW_H_

#include <coecntrl.h>

class CGulIcon;
class CAknButton;
class MESMRNaviArrowEventObserver;


NONSHARABLE_CLASS( CMRNaviArrow ) : 
        public CCoeControl
    {
public:
	enum TArrowType {
		EMRArrowLeft,
		EMRArrowRight
		};
public:
    /**
     * Two-phased constructor.
     */
	static CMRNaviArrow* NewL( const CCoeControl* aParent, TArrowType aType,
                               MESMRNaviArrowEventObserver* aObserver,
                               TSize aIconSize );

    /**
     * Destructor.
     */
    virtual ~CMRNaviArrow();
    
private:
    /**
     * Default c++ constuctor
     */
    CMRNaviArrow( MESMRNaviArrowEventObserver* aObserver, TArrowType aType, TSize aIconSize );
    /**
     * Second phase constructor
     */
    void ConstructL( const CCoeControl* aParent );
    
public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void HandlePointerEventL(
            const TPointerEvent &aPointerEvent );
    void Draw(const TRect& aRect) const;
    
public:
    /**
     * Set the navi arrow's visibility.
     * @param aVisible  Indicate if the arrow visible
     */
    void SetNaviArrowStatus( TBool aVisible );
    
private:
    /**
     * Init the navi arrow buttons.
     */
    void InitNaviArrowsL();
    /**
     * Create a semi-transparent Icon.
     * @param aBitMap  The bitmap source
     * @param aBitMapMask The bitmap mask source
     */
    CGulIcon* CreateSemiTransparentIconL(  CFbsBitmap* aBitMap, 
                                      CFbsBitmap* aBitMapMask);
    
private:
    /// Own: Left arrow button
    CAknButton* iArrow;
    /// Ref: Navi arrow event observer
    MESMRNaviArrowEventObserver* iObserver;
    /// Own: Left arrow visibility status
    TBool iArrowVisibility;
    /// Own: Store the type of such arrow
    TArrowType iArrowType;
    /// Own: Store the size of such arrow
    TSize iIconSize;
    };

#endif /* CMRNAVIARROW_H_ */
