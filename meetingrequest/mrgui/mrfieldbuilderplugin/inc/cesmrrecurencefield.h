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
* Description:  ESMR Recurence field for CESMRListComponent
 *
*/


#ifndef CESMRRECURENCEFIELD_H
#define CESMRRECURENCEFIELD_H

#include "cesmriconfield.h"

class CEikLabel;
class CESMRRecurrence;
class MESMRFieldValidator;

NONSHARABLE_CLASS( CESMRRecurenceField ): public CESMRIconField
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
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    void ExecuteGenericCommandL( TInt aCommand );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

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
    /**
     * Not owned. Label for current recurrence.
     */
    CEikLabel* iRecurence;

    /**
     * Own. Array of recurrence objects.
     */
    RPointerArray< CESMRRecurrence > iArray;

    /**
     * Not owned. Helper class for sanity and time checks.
     */
    MESMRFieldValidator* iValidator;

    /**
     * Index for selected recurrence (index to iArray)
     */
    TInt iIndex;
    };

#endif  // CESMRRECURENCEFIELD_H

