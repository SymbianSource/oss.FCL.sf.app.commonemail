/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR boolean field implementation
 *
*/


#ifndef CESMRBOOLEANFIELD_H
#define CESMRBOOLEANFIELD_H

#include "cesmriconfield.h"

class CEikLabel;
class MESMRFieldValidator;

/**
 * Creates a boolean checkbox type field for misc calendar event related info
 */
NONSHARABLE_CLASS( CESMRBooleanField ): public CESMRIconField
    {
public:
    /**
     * Creates new CESMRBooleanField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRBooleanField* NewL( MESMRFieldValidator* aValidator );

    /**
     * C++ Destructor.
     */
    ~CESMRBooleanField();

public: // From CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    void ExecuteGenericCommandL( TInt aCommand );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void MakeVisible(TBool aVisible);

private:
    /**
     * Constructor.
     *
     * @param aValidator validator object
     */
    CESMRBooleanField( MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Reset field value.
     */
    void ResetFieldL();

    /**
     * Switch middle softkey: on/off
     */
    void SwitchMiddleSoftKeyL();

private:
    /**
     * Not owned. Field label.
     */
    CEikLabel* iLabel;

    /**
     * Current value.
     */
    TBool iStatus;

    /**
     * Not owned. Validator object.
     */
    MESMRFieldValidator* iValidator;
    };

#endif  // CESMRBOOLEANFIELD_H

