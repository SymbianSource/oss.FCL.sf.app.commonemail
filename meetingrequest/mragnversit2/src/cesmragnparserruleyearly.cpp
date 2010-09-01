/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMRAgnParseRRuleYearly.
*
*/


#include "emailtrace.h"
#include "cesmragnparserruleyearly.h"

//<cmail>

#include "cesmricalvalue.h"
//</cmail>
#include <calrrule.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleYearly::CESMRAgnParseRRuleYearly
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleYearly::CESMRAgnParseRRuleYearly( MESMRAgnImpUtil& aAgnImpUtil ):
                        CESMRAgnParseRRule( aAgnImpUtil )
    {
    FUNC_LOG;
    //do nothing
    }


// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleYearly::~CESMRAgnParseRRuleYearly
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleYearly::~CESMRAgnParseRRuleYearly()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleYearly::NewLC
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRule* CESMRAgnParseRRuleYearly::NewLC( MESMRAgnImpUtil& aAgnImpUtil )
    {
    FUNC_LOG;

    CESMRAgnParseRRule *self = new (ELeave) CESMRAgnParseRRuleYearly( aAgnImpUtil );
    CleanupStack::PushL( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleYearly::DoParseL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleYearly::DoParseL( CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    TBool parsed;

    switch( aSegment.Type() )
        {
        //ESegByDay
        case CESMRICalRuleSegment::ESegByDay :
            {
            parsed = SetRRuleByDayL( aSegment, aRule );
            break;
            }

        //ESegByMonth
        case CESMRICalRuleSegment::ESegByMonth :
            {
            parsed = SetRRuleByMonthL( aSegment, aRule );
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
// CESMRAgnParseRRuleYearly::FinalizeParsingL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleYearly::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    FUNC_LOG;

    //Call the parent, to set some common values
    TBool finalized = CESMRAgnParseRRule::FinalizeParsingL( aRule, aStartTime );

    // ByMonth is easy.
    if ( !(iSetSegs & ESegFlagByMonth) )
        {
        // The specification requires certain fields to be taken from the DTSTART
        // if they have not already been set.
        TDateTime startDate = aStartTime.TimeUtcL().DateTime();

        RArray<TMonth> theMonths;
        CleanupClosePushL(theMonths);
        User::LeaveIfError(theMonths.Append(startDate.Month()));
        aRule.SetByMonth(theMonths);
        CleanupStack::PopAndDestroy(&theMonths);
        }

    #if 0
    //this will cause yearly repeat not to work
    //further investigation needed
        else if ( !( iSetSegs & ESegFlagByDay ) )
            {
            /*
            // We are in trouble here, because we are trying to work out a rule along the
            // lines of "last saturday" from a date/time, which simply can't be done.
            // This arrises because agenda doesn't support a yearly repeating rule with
            // a numbered day of the month.
            // We can't import this RRule, but we can continue with the rest of the entry.
            */
            finalized = EFalse;
            }
    #endif

    return finalized;
    }

