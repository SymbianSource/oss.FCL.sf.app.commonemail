/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class MESMRAgnImpUtil.
*
*
*/


#ifndef MESMRAGNIMPUTIL_H
#define MESMRAGNIMPUTIL_H

//System includes
#include <caltime.h>

//User includes
#include "mesmragnimportobserver.h" // for MESMRAgnImportObserver::TImpResponse

//<cmail>
#include "cesmricalrulesegment.h"   // for cesmricalrulesegment
//</cmail>

//Forward declarations
class CESMRICalProperty;
class CTzRules;
class CTzNamedRules;

// Constants.
const TInt KSecondsInOneMinute = 60;
const TInt KMinutesInOneHour = 60;
const TInt KMaxWeekDayNum = 4;
const TInt KDaysPerWeek = 7;

//Class declarations
class MESMRAgnImpUtil
    {
    public: // Enumerations
        enum TValueType
            {
            EDefault,
            EDate,
            EDateTime,
            ETime,
            EDuration,
            EPeriod
            };

    public: //Helper functions
        virtual void GetCalTimeL(const CESMRICalProperty& aProperty, TCalTime& aTime, TValueType aValueType, TInt aValue = 0) = 0;
        virtual TInt FindRuleSegment(const RPointerArray<CESMRICalRuleSegment>& aRules, CESMRICalRuleSegment::TSegmentType aType) const = 0;
        virtual const CTzRules* FindTzRule(const TDesC& aName) const = 0;

    public: //Report errors
        virtual void ReportErrorL(MESMRAgnImportObserver::TImpError aType, const TDesC8& aUid, const TDesC& aContext, TBool aCanContinue = ETrue) = 0;
    };

#endif
