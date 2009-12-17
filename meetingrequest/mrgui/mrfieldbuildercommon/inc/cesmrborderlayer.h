/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR service policy
*
*/

#ifndef CESMRBORDERLAYER_H
#define CESMRBORDERLAYER_H

#include <coecntrl.h>
#include "cesmrlayoutmgr.h"

#include "cesmrfield.h"

/**
 * CESMRBorderLayer draws border around the focused control.
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRBorderLayer ): public CCoeControl, 
									   public MCoeControlBackground
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aExt Control which has border around it.
     * @param aFocusType
     * @return Pointer to created and initialized esmr borderlayer object.
     */
     IMPORT_C static CESMRBorderLayer* NewL( 
    		 CCoeControl* aExt, 
    		 TESMRFieldFocusType aFocusType = EESMRBorderFocus );

     /**
      * Destructor.
      */
    IMPORT_C ~CESMRBorderLayer();

    /**
     * Set outline focus (rectangle) around the fields control.
     * @param aFocus
     */
    IMPORT_C void SetOutlineFocusL( TBool aFocus );

    /**
     * Test if this border layer has focus.
     * @return TBool
     */
    IMPORT_C TBool HasFocus() const;

    /**
     * Gives pointer to layoutmanager for this class
     * @param aLayout
     */
    IMPORT_C void SetLayoutManager( CESMRLayoutManager* aLayout );

private:
    CESMRBorderLayer( CCoeControl* aExtControl, 
					  TESMRFieldFocusType aFocusType );
    void ConstructL();

public: // From MCoeControlBackground
    IMPORT_C void Draw( CWindowGc &aGc, const 
						CCoeControl &aControl, 
						const TRect &aRect) const;
    IMPORT_C void GetTextDrawer(
    		CCoeTextDrawerBase *&aTextDrawer, 
    		const CCoeControl *aDrawingControl) const;

public: // From CCoeControl
    IMPORT_C TKeyResponse OfferKeyEventL( 
    		const TKeyEvent& aEvent, 
    		TEventCode aType );
    IMPORT_C void SetContainerWindowL(const CCoeControl& aContainer);
    IMPORT_C TInt CountComponentControls() const;
    IMPORT_C CCoeControl* ComponentControl( TInt aInd ) const;
    IMPORT_C void SizeChanged();

protected:
    /// Own: control which is surrounded by border
    CCoeControl* iExtControl;
    /// Ref: Wheater this layer has focus or not
    TBool iFocus;
    /// Own: Inofmation about focus type, needed when border/highlight is drawn
    TESMRFieldFocusType iFocusType;
    /// Ref: Pointer to layputmanager
    CESMRLayoutManager* iLayout;
    };

#endif // CESMRBORDERLAYER_H
