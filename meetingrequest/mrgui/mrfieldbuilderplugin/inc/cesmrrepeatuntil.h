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
* Description:  ESMR repeat until field implementation
 *
*/


#ifndef CESMRREPEATUNTIL_H
#define CESMRREPEATUNTIL_H

#include "cesmrfield.h"

#include <AknsControlContext.h>

class CEikLabel;
class CEikDateEditor;
class MESMRFieldValidator;
class CAknsFrameBackgroundControlContext;

NONSHARABLE_CLASS( CESMRRepeatUntilField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRRepeatUntilField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRRepeatUntilField* NewL( MESMRFieldValidator* aValidator );

    /**
     * Destructor.
     */
    ~CESMRRepeatUntilField();

public: // From CESMRField
    void InitializeL();
    TBool OkToLoseFocusL( TESMREntryFieldId aNextItem );
    void SetOutlineFocusL( TBool aFocus );
    
public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void ActivateL();
    void PositionChanged();
    TKeyResponse OfferKeyEventL(
            const TKeyEvent& aEvent,
            TEventCode aType );
    void CheckIfValidatingNeededL(
            TInt aStartFieldIndex );

private:
    /**
     * Constructor.
     * @param aValidator validator object
     */
    CESMRRepeatUntilField(MESMRFieldValidator* aValidator);

    /**
     * Second phase constructor.
     */
    void ConstructL();

private:
    /**
     * Own. Repeat until field label.
     */
    CEikLabel* iLabel;

    /**
     * Not owned. Repeat until field editor.
     */
    CEikDateEditor* iDate;

    /**
     * Not owned. Validator object.
     */
    MESMRFieldValidator* iValidator;

    /**
     * Background control context
     */
    MAknsControlContext* iBackground;

    /**
     * Actual background for the editor.
     */
    CAknsFrameBackgroundControlContext* iFrameBgContext;
    };

#endif  // CESMRREPEATUNTIL_H
