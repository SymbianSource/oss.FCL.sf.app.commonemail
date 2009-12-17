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
* Description:  ESMR service policy checker
*
*/


#ifndef CESMRRECURRENCEINFOHANDLER_H
#define CESMRRECURRENCEINFOHANDLER_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <caltime.h>

class CCalEntry;
class TCalRRule;

/**
 *  CESMRRecurrenceInfoHandler is responsible for handling recurrence related
 *  issues.
 *
 *  @lib esmrservices.lib
 */
NONSHARABLE_CLASS(CESMRRecurrenceInfoHandler) :
        public CBase
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRRecurrenceInfoHandler object. Ownership transferred to caller.
     *
     * @param aEntry Reference to calendar entry.
     * @return Pointer to esmr recurrence handler object.
     */
    IMPORT_C static CESMRRecurrenceInfoHandler* NewL(
            CCalEntry& aEntry );

    /**
     * Two-phased constructor. Creates and initializes
     * CESMRRecurrenceInfoHandler object. Ownership transferred to caller.
     * Created object is left to cleanup stack.
     *
     * @param aEntry Reference to calendar entry.
     * @return Pointer to esmr recurrence handler object.
     */
    IMPORT_C static CESMRRecurrenceInfoHandler* NewLC(
            CCalEntry& aEntry );

    /**
     * C++ Destructor.
     */
    IMPORT_C ~CESMRRecurrenceInfoHandler();

public: // Interface

    /**
     * Sets recurrence to ES MR entry. If until time is not set
     * (i.e, it is null time), then until time is implementation
     * dependent.
     *
     * When recurrence is set to ERecurrenceNot, then aUntil parameter
     * is ignored.
     *
     * @param aRecurrece Recurrece value.
     * @param aUntil Recurrence valid until in UTC format.
     */
    IMPORT_C void SetRecurrenceL(
            TESMRRecurrenceValue aRecurrence,
            TTime aUntil );

    /**
     * Fetches entry's recurrence value.
     *
     * @param aRecurrence On return contains entry's recurrence information
     * @param aUntil On return contains time in UTC until recurrence is valid.
     */
    IMPORT_C void GetRecurrenceL(
            TESMRRecurrenceValue& aRecurrence,
            TTime& aUntil) const;

    /**
     * Removes instance from series.
     * @param aInstanceTime Instance to be removed
     */
    IMPORT_C void RemoveInstanceL(
            TCalTime aInstanceTime );

    /**
     * Removes calendar instance from entry.
     * @param aNewInstanceTime New instance time
     * @param aOrginalInstanceTime Orginal instance time
     */
    IMPORT_C void AddExceptionL(
             TCalTime aNewInstanceTime,
             TCalTime aOrginalInstanceTime );

    /**
     * Copies recurrence information to entry given as parameter.
     * @param aDestination Reference to destination calendar entry
     */
    IMPORT_C void CopyRecurrenceInformationToL(
            CCalEntry& aDestination );

    /**
     * Copies recurrence information from entry given as parameter.
     * @param aSource Reference to source calendar entry
     */
    IMPORT_C void CopyRecurrenceInformationFromL(
            const CCalEntry& aSource );

    /**
     * Gets first instances start and end time.
     * @param aStart Reference to start time.
     * @param aEnd Reference to end time.
     */
    IMPORT_C void GetFirstInstanceTimeL(
            TCalTime& aStart,
            TCalTime& aEnd );

private: // Implementation
    CESMRRecurrenceInfoHandler(
            CCalEntry& aEntry );
    void ConstructL();

    void CalculateRecurrenceUntilDateL(
            TESMRRecurrenceValue aRecurrenceType,
            TTime& aUntil,
            TCalRRule& aRule,
            const CCalEntry& aEntry  ) const;

    void HandleDailyRecurrenceL(
            TESMRRecurrenceValue& aRecurrenceValue,
            TCalRRule& aRule ) const;

    void HandleWeeklyRecurrenceL(
            TESMRRecurrenceValue& aRecurrenceValue,
            TCalRRule& aRule ) const;

    void HandleMonthlyRecurrenceL(
            TESMRRecurrenceValue& aRecurrenceValue,
            TCalRRule& aRule ) const;

    void HandleYearlyRecurrenceL(
            TESMRRecurrenceValue& aRecurrenceValue,
            TCalRRule& aRule ) const;

    TBool NeedToSetRecurrenceL(
            TESMRRecurrenceValue aRecurrence,
            TTime aUntil ) const;


private:
    /// Ref: Reference to meeting request entry
    CCalEntry& iEntry;
    };

#endif // CESMRRECURRENCEINFOHANDLER_H

// EOF
