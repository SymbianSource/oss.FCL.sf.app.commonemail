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
* Description:  ESMR mrinfo recurrence handler definition
 *
*/


#ifndef CESMRINFORECURRENCEHANDLER_H
#define CESMRINFORECURRENCEHANDLER_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

class MMRInfoObject;
class CCalEntry;
class MRRecurrenceRule;

/**
 * CESMRInfoRecurrenceHandler parses recurrence information from MRINFO
 * object and stores it to calendar entry object.
 */
NONSHARABLE_CLASS( CESMRInfoRecurrenceHandler ) :
        public CBase
    {
public: // Construction and destruction

    /**
     * Creates and initialzes new CESMRInfoRecurrenceHandler
     * object. Ownership is transferred to caller.
     *
     * @return Pointer to CESMRInfoRecurrenceHandler object.
     */
    static CESMRInfoRecurrenceHandler* NewL();

    /**
     * Creates and initialzes new CESMRInfoRecurrenceHandler
     * object. Ownership is transferred to caller. Created object is
     * left to cleaup stack.
     *
     * @return Pointer to CESMRInfoRecurrenceHandler object.
     */
    static CESMRInfoRecurrenceHandler* NewLC();

    /**
     * C++ destructor.
     */
    ~CESMRInfoRecurrenceHandler();

public: // Interface

    /**
     * Parser recurrence information MRINFO object and stores it to
     * calendar entry object.
     *
     * @param aCalEntry Reference to calendar entry.
     * @param aInfoObject Reference to mrinfo object
     * @see TESMRInputParams
     */
    void ParseRecurrenceInforationL(
            CCalEntry& aCalEntry,
            const MMRInfoObject& aInfoObject );

private: // Implemenation
    CESMRInfoRecurrenceHandler();
    void ConstructL();

    void GetRecurrenceL(
            CCalEntry& aCalEntry,
            const MMRInfoObject& aInfoObject );

    void HandleDailyRecurrenceL(
            CCalEntry& aCalEntry,
            const MMRInfoObject& aInfoObject );

    void HandleWeeklyRecurrenceL(
            CCalEntry& aCalEntry,
            const MMRInfoObject& aInfoObject );

    void HandleMonthlyRecurrenceL(
            CCalEntry& aCalEntry,
            const MMRInfoObject& aInfoObject );

    void HandleYearlyRecurrenceL(
            CCalEntry& aCalEntry,
            const MMRInfoObject& aInfoObject );

private: // Data

    };

#endif 

// EOF
