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
* Description:  Response dialog's custom control implementation
*
*/


#ifndef CESMRRESPONSEDIALOGVIEW_H
#define CESMRRESPONSEDIALOGVIEW_H

// INCLUDES
#include <coecntrl.h>
#include <eiksbfrm.h>

// FORWARD DECLARATIONS
class CAknsBasicBackgroundControlContext;
class CEikRichTextEditor;

/**
* View for response dialog.
*
* @lib esmrgui.lib
*/
NONSHARABLE_CLASS( CESMRResponseDialogView ) : public CCoeControl
    {
public:
    /**
     * Two-phased constructor.
     */
    static CESMRResponseDialogView* NewL();

    /**
     * Destructor.
     */
    ~CESMRResponseDialogView();

    /**
     * Fetches the text written to dialog, Ownership
     * is trasferred
     * @return HBufC Text in buffer
     */
    HBufC* GetTextL();

public:// From CCoeControl
    void Draw(const TRect& aRect) const;
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SetContainerWindowL(const CCoeControl& aContainer);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void HandleResourceChange( TInt aType );
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    void SizeChanged();
    TSize MinimumSize();
    void SetFontColorL();
    
private:    

    CESMRResponseDialogView();
    void ConstructL();
private: // Data
    /// Own:
    CEikRichTextEditor* iEditor;
    /// Own: bg context
    CAknsBasicBackgroundControlContext* iBgContext;     
    
    CEikScrollBarFrame* iScrollBarFrame;
    };

#endif // CESMRRESPONSEDIALOGVIEW_H
