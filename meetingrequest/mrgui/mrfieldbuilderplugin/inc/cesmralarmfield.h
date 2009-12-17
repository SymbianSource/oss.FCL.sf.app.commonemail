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
* Description:  ESMR Alarm field implementation
 *
*/


#ifndef CESMRALARMFIELD_H
#define CESMRALARMFIELD_H

#include "cesmriconfield.h"

class CEikLabel;
class CESMRAlarm;
class MESMRFieldValidator;

/**
 * Alarm field defines the time before the meeting when
 * alarm should be showed. User can traverse between the
 * different alarm values by pressing left/right keys or
 * open a dialog by pressing ok button.
 *
 * @see cesmralarm.h
 */
NONSHARABLE_CLASS( CESMRAlarmField ) : public CESMRIconField
    {
public:
    /**
     * Two phase constructor.
     */
    static CESMRAlarmField* NewL( MESMRFieldValidator* aValidator );

    /**
     * Destructor.
     */
    ~CESMRAlarmField();

public: // From CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool OkToLoseFocusL( TESMREntryFieldId aId );
    void ExecuteGenericCommandL( TInt aCommand );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);

private:
    /**
     * Constructor
     */
    CESMRAlarmField( MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Updates new alarm time
     *
     * @param aIndex new alarm time index
     */
    void UpdateAlarmLabelL( TInt aIndex );

    /**
     * Updates alarm to first valid value, when
     * alarm has been set to past by user
     */
    void UpdateAlarmToFirstValidValueL();

    /**
     * Handles alarm time settings query
     */
    void DoAlarmTimeQueryL();

    /**
     * Triggers validator to validate alarm
     */
    TBool TriggerValidatorL();

    /**
      * Handles MSK command execution
      */
    void ExecuteMSKCommandL();

private:
    /**
     *  Not own: Label for current alarm value
     */
    CEikLabel* iAlarm;

    /**
     * Own: List of CESMRAlarm objects
     */
    RPointerArray< CESMRAlarm > iArray;

    /**
     * Index pointing to iArray.
     */
    TInt iOptIndex;

    /**
     * Not owned. Validator class for time and sanity checks.
     */
    MESMRFieldValidator* iValidator;

    /**
     * Own: Relative alarm validity
     */
    TBool iRelativeAlarmValid;
    };

#endif  //CESMRALARMFIELD_H

