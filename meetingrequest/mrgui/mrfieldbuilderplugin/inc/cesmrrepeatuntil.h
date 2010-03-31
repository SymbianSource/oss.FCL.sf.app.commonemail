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
* Description:  ESMR repeat until field implementation
 *
*/


#ifndef CESMRREPEATUNTIL_H
#define CESMRREPEATUNTIL_H

#include "cesmrfield.h"

class CMRLabel;
class CEikDateEditor;
class MESMRFieldValidator;
class CAknsBasicBackgroundControlContext;


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
    TBool OkToLoseFocusL( TESMREntryFieldId aNextItem );
    void SetOutlineFocusL( TBool aFocus );
    void SetValidatorL( MESMRFieldValidator* aValidator );
    TBool ExecuteGenericCommandL( TInt aCommand );
    
public: // From CCoeControl
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL(
            const TKeyEvent& aEvent,
            TEventCode aType );
    void CheckIfValidatingNeededL(
            TInt aStartFieldIndex );
    void SetContainerWindowL( const CCoeControl& aContainer );

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

    void DoEnvChangeL();
    
private:

    // Own. Repeat until field label.
    CMRLabel* iLabel;

    // Not owned. Repeat until field editor.
    CEikDateEditor* iDate;

    // Own. Background control context.
    CAknsBasicBackgroundControlContext* iBgCtrlContext;
    
    /// Ref: Pointer to container window
    const CCoeControl* iContainerWindow;    
    };

#endif  // CESMRREPEATUNTIL_H
