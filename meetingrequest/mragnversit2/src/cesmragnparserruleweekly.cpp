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
* Description: This file implements class CESMRAgnParseRRuleWeekly.
*
*/


#include "emailtrace.h"
#include "cesmragnparserruleweekly.h"

//<cmail>
#include "cesmricalvalue.h"
//</cmail>
#include <calrrule.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleWeekly::~CESMRAgnParseRRuleWeekly
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleWeekly::CESMRAgnParseRRuleWeekly( MESMRAgnImpUtil& aAgnImpUtil ):
                        CESMRAgnParseRRule( aAgnImpUtil )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleWeekly::CESMRAgnParseRRuleWeekly
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleWeekly::~CESMRAgnParseRRuleWeekly()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleWeekly::NewLC
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRule* CESMRAgnParseRRuleWeekly::NewLC( MESMRAgnImpUtil& aAgnImpUtil )
    {
    FUNC_LOG;

    CESMRAgnParseRRule *self = new (ELeave) CESMRAgnParseRRuleWeekly( aAgnImpUtil );
    CleanupStack::PushL( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleWeekly::DoParseL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleWeekly::DoParseL( CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    TBool parsed = ETrue;

    switch( aSegment.Type() )
        {
        //ESegWkSt
        case CESMRICalRuleSegment::ESegWkSt :
            {
            parsed = SetRRuleWkStL( aSegment, aRule );
            break;
            }

        //ESegByDay
        case CESMRICalRuleSegment::ESegByDay :
            {
            parsed = SetRRuleByDayL( aSegment, aRule );
            break;
            }
        default:
            {
            parsed = EFalse;
            }
        }

    return parsed;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleWeekly::FinalizeParsingL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleWeekly::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    FUNC_LOG;

    //Call the parent, to set some common values
    TBool finalized = CESMRAgnParseRRule::FinalizeParsingL( aRule, aStartTime );

    // If we are yearly or weekly we can only handle ByDay, and need to handle it
    // differently for each.
    if ( !( iSetSegs & ESegFlagByDay ) )
        {
        RArray<TDay> days;
        CleanupClosePushL(days);
        User::LeaveIfError(days.Append(aStartTime.TimeUtcL().DayNoInWeek()));
        aRule.SetByDay(days);
        CleanupStack::PopAndDestroy(&days);
        }

    return finalized;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleWeekly::SetRRuleByDayL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleWeekly::SetRRuleByDayL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    TBool parsed = ETrue;

    RArray<TDay> days;
    CleanupClosePushL(days);

    const RPointerArray<CESMRICalValue>& values = aSegment.Values();
    TInt valCount = values.Count();
    TDay dayval = EMonday;
    TInt daypos = 0;

    for (TInt day = 0; day < valCount; ++day)
        {
        values[day]->GetDayL(dayval, daypos);

        if (daypos == 0)
            {
            User::LeaveIfError(days.Append(dayval));
            }
        }

    if( parsed )
        {
        aRule.SetByDay(days);
        iSetSegs |= ESegFlagByDay;
        }

    CleanupStack::PopAndDestroy(&days);

    return parsed;
    }

