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
* Description:  ESMR Recurence field.
 *
*/


#ifndef CESMRRECURENCEFIELD_H
#define CESMRRECURENCEFIELD_H

#include "cesmrfield.h"

class CMRLabel;
class CMRImage;
class CESMRRecurrence;
class MESMRFieldValidator;

NONSHARABLE_CLASS( CESMRRecurenceField ): public CESMRField
    {
public:
    /**
     * Creates new CESMRRecurenceField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRRecurenceField* NewL( MESMRFieldValidator* aValidator );

    /**
     * Destructor.
     */
    ~CESMRRecurenceField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );

private: // implementation
    /**
     * Constructor.
     *
     * @param aValidator validator object
     */
    CESMRRecurenceField( MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Selects new recurrence value.
     *
     * @param aIndex index of new recurrence
     */
    void SetRecurrenceL( TInt aIndex );

    /**
    * Shows query dialog for selecting
    * priority
    *
    */
    void ExecuteRecurrenceQueryL();

private: // data
   
    // Not owned. Label for current recurrence.
    CMRLabel* iRecurrence;
    
    // Own. Field icon.
    CMRImage* iFieldIcon;
    
    // Own. Array of recurrence objects.
    RPointerArray< CESMRRecurrence > iArray;

    // Index for selected recurrence (index to iArray)
    TInt iIndex;
    };

#endif  // CESMRRECURENCEFIELD_H

