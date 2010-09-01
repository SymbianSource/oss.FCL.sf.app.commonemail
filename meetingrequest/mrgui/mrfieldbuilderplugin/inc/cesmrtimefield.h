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
* Description:  ESMR time (start-end) field implementation
 *
*/


#ifndef CESMRTIMEFIELD_H
#define CESMRTIMEFIELD_H

// INCLUDES
#include "cesmrfield.h"

//Forward declarations
class CMRImage;
class CMRTimeContainer;
class MESMRFieldValidator;

/**
 * Time field for ESMR editor. Time field has start and end time
 * separated by a '-'. Focus traverses from left to right and right
 * to left.
 */
NONSHARABLE_CLASS( CESMRTimeField ): public CESMRField
    {
public:
    /**
     * Creates new CESMRTimeField object. Ownership
     * is transferred to caller.
     * @param aValidator validator object
     * @return Pointer to created object,
     */
    static CESMRTimeField* NewL( MESMRFieldValidator* aValidator );

    /**
     * Destructor.
     */
    ~CESMRTimeField();

public: // From CCoeControl
    void SetContainerWindowL( const CCoeControl& aControl );
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void SizeChanged();

public: // From ESMRField
    TBool OkToLoseFocusL( TESMREntryFieldId aNext );
    void SetOutlineFocusL( TBool aFocus );
    void SetValidatorL( MESMRFieldValidator* aValidator );
    TBool ExecuteGenericCommandL( TInt aCommand );
    
private:
    /**
     * Constructor.
     */
    CESMRTimeField();

    /**
     * Second phase constructor.
     *
     * @param aValidator validator object
     */
    void ConstructL( MESMRFieldValidator* aValidator );

private:
    /**
     * Ref: Container for the time fields. 
     *  Created by this class but deleted by base class.  
     */
    CMRTimeContainer* iContainer;
    
    /// Own: Field icon
    CMRImage* iFieldIcon;
    };

#endif  // CESMRTIMEFIELD_H

