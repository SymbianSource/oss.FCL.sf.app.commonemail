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
* Description: This file implements class CESMRAgnParseRRule.
*
*/



#include "emailtrace.h"
#include "cesmragnparserrule.h"

//<cmail>

#include <calrrule.h>
#include "cesmricalvalue.h"         // for cesmricalvalue
//</cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::CESMRAgnParseRRule
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRule::CESMRAgnParseRRule( MESMRAgnImpUtil& aAgnImpUtil ) :
                            iAgnImpUtil( aAgnImpUtil ),
                            iSetSegs( 0 )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::~CESMRAgnParseRRule
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRule::~CESMRAgnParseRRule()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::ParseL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::ParseL( TCalRRule& aRule, RPointerArray<CESMRICalRuleSegment>& aRecRules,
                              const TCalTime& aStartTime, const TDesC8& aUid,
                              const RPointerArray<CESMRICalValue>& aRulevalues )
    {
    FUNC_LOG;

    aRule.SetDtStart( aStartTime );

    TInt recCount = aRecRules.Count();
    TBool parsed = EFalse;

    //Loop through the CESMRICalRuleSegments
    for (TInt rnum = 0; rnum < aRecRules.Count(); ++rnum)
        {
        parsed = EFalse;

        CESMRICalRuleSegment* segment = aRecRules[rnum];

        if (segment->Values().Count() < 1  && aRulevalues.Count() > 0 )
            {
            iAgnImpUtil.ReportErrorL( MESMRAgnImportObserver::EImpErrorMissingData, aUid,
                                      aRulevalues[0]->TextL() );
            // On continue, all we have is FREQ=DAILY.
            }

        //Try to parse it with specific parser
        TRAPD( error, parsed = DoParseL( *segment, aRule ) );

        //if the keyword was not parsed, try to parse it
        if( error == KErrNone && !parsed )
            {
            //Update aRule with CESMRICalRuleSegment
            switch (segment->Type())
                {
                case CESMRICalRuleSegment::ESegFreq :
                    break;

                //Set Until
                case CESMRICalRuleSegment::ESegUntil :
                    {
                    SetRRuleUntilL( *segment, aRule );
                    break;
                    }

                //Set Count
                case CESMRICalRuleSegment::ESegCount :
                    {
                    aRule.SetCount(segment->Values()[0]->IntegerL());
                    break;
                    }

                //Set Interval
                case CESMRICalRuleSegment::ESegInterval :
                    {
                    aRule.SetInterval(segment->Values()[0]->IntegerL());
                    break;
                    }

                //Not supported keywords by freq, report error
                case CESMRICalRuleSegment::ESegByPos :      // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegBySecond :   // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegByMinute :   // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegByHour :     // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegByYearDay :  // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegByWeekNo :   // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegExtension :  // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegWkSt :       // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegByDay :      // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegByMonthDay : // not supported by freq, fall through
                case CESMRICalRuleSegment::ESegByMonth :    // not supported by freq, fall through
                default:
                    {
                    if(aRulevalues.Count() > 0)
                        {
                        iAgnImpUtil.ReportErrorL( MESMRAgnImportObserver::EImpErrorNotSupported, aUid,
                                              aRulevalues[0]->TextL() );
                        }
                    }
                }
            }

        //TRAP returned error
        else if( error != KErrNone )
            {
            if( error == KErrAbort )//KErrAbort, report error and stop parsing
                {
                
                if(aRulevalues.Count() > 0)
                	{
                    iAgnImpUtil.ReportErrorL( MESMRAgnImportObserver::EImpErrorNotSupported, aUid,
                                          aRulevalues[0]->TextL(), EFalse );
                	}
                }
            else //Otherwise leave with error
                {
                User::Leave( error );
                }
            }

        }//for end


    //Finalize parsing
    parsed = FinalizeParsingL( aRule, aStartTime );

    return parsed;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::DoParseL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::DoParseL( CESMRICalRuleSegment& /*aSegment*/, TCalRRule& /*aRule*/ )
    {
    FUNC_LOG;

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::FinalizeParsingL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::FinalizeParsingL( TCalRRule& aRule, const TCalTime& /*aStartTime*/ )
    {
    FUNC_LOG;

    TBool finalized = ETrue;

    // Agenda requires that all RRULES have an interval of some sort.
    // An interval of 1 is the default.
    if (aRule.Interval() < 1)
        {
        aRule.SetInterval(1);
        }

    return finalized;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::SetRRuleUntilL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::SetRRuleUntilL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    TTime time;
    TCalTime calTime;

     // adding time and date information to the Repeat Until field
  CESMRICalValue::TTimeZoneType type = CESMRICalValue::EUtcTime;
    aSegment.Values()[0]->GetDateTimeL(time, type);

    calTime.SetTimeUtcL(time);
    aRule.SetUntil(calTime);

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::SetRRuleWkStL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::SetRRuleWkStL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    TDay dayval;
    TInt daypos;
    aSegment.Values()[0]->GetDayL(dayval, daypos);
    aRule.SetWkSt(dayval);

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::SetRRuleByDayL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::SetRRuleByDayL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    RArray<TCalRRule::TDayOfMonth> daysOfMonth;
    CleanupClosePushL(daysOfMonth);

    const RPointerArray<CESMRICalValue>& values = aSegment.Values();

    TInt valCount = values.Count();
    TDay dayval = EMonday;
    TInt daypos = 0;

    for (TInt day = 0; day < valCount; ++day)
        {
        values[day]->GetDayL(dayval, daypos);
        if ((daypos == -1) || ((daypos >= 1) && (daypos <= KMaxWeekDayNum)))
            {
            User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, daypos)));
            }
        else if (daypos == 0)
            //BYDAY applies to every day in month
            {
            User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 1)));
            User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 2)));
            User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 3)));
            User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 4)));
            User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, -1)));
            }
        else
            {
            //Invalid day - unrecoverable
            User::Leave( KErrAbort );
            }
        }

    aRule.SetByDay(daysOfMonth);
    CleanupStack::PopAndDestroy(&daysOfMonth);
    iSetSegs |= ESegFlagByDay;

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::SetRRuleByMonthL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::SetRRuleByMonthL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    RArray<TMonth> theMonths;
    CleanupClosePushL(theMonths);
    const RPointerArray<CESMRICalValue>& values = aSegment.Values();
    TInt valCount = values.Count();

    for (TInt month = 0; month < valCount; ++month)
        {
        User::LeaveIfError(theMonths.Append(values[month]->MonthL()));
        }

    iSetSegs |= ESegFlagByMonth;
    aRule.SetByMonth(theMonths);
    CleanupStack::PopAndDestroy(&theMonths);

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRule::SetRRuleByMonthDayL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRule::SetRRuleByMonthDayL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    RArray<TInt> days;
    CleanupClosePushL(days);
    const RPointerArray<CESMRICalValue>& values = aSegment.Values();
    TInt valCount = values.Count();

    for (TInt day = 0; day < valCount; ++day)
        {
        //dates are represented as day - 1 (0 = 1) in TDateTime
        User::LeaveIfError(days.Append(values[day]->IntegerL() - 1 ));
        }

    aRule.SetByMonthDay(days);
    CleanupStack::PopAndDestroy(&days);
    iSetSegs |= ESegFlagByMonthDay;

    return ETrue;
    }

