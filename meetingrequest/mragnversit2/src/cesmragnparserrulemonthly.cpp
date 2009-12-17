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
* Description: This file implements class CESMRAgnParseRRuleMonthly.
*
*/


#include "emailtrace.h"
#include "cesmragnparserrulemonthly.h"

//<cmail>
#include "cesmricalvalue.h"
//</cmail>
#include <calrrule.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleMonthly::CESMRAgnParseRRuleMonthly
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleMonthly::CESMRAgnParseRRuleMonthly( MESMRAgnImpUtil& aAgnImpUtil ):
                        CESMRAgnParseRRule( aAgnImpUtil )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleMonthly::~CESMRAgnParseRRuleMonthly
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleMonthly::~CESMRAgnParseRRuleMonthly()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleMonthly::NewLC
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRule* CESMRAgnParseRRuleMonthly::NewLC( MESMRAgnImpUtil& aAgnImpUtil )
    {
    FUNC_LOG;

    CESMRAgnParseRRule *self = new (ELeave) CESMRAgnParseRRuleMonthly( aAgnImpUtil );
    CleanupStack::PushL( self );


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleMonthly::DoParseL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleMonthly::DoParseL( CESMRICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    FUNC_LOG;

    TBool parsed;

    switch( aSegment.Type() )
        {
        //ByDay
        case CESMRICalRuleSegment::ESegByDay :
            {
            parsed = SetRRuleByDayL( aSegment, aRule );
            break;
            }
        //ByMonthDay
        case CESMRICalRuleSegment::ESegByMonthDay :
            {
            parsed = SetRRuleByMonthDayL( aSegment, aRule );
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
// CESMRAgnParseRRuleMonthly::FinalizeParsingL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleMonthly::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    FUNC_LOG;

    //Call the parent, to set some common values
    TBool finalized = CESMRAgnParseRRule::FinalizeParsingL( aRule, aStartTime );

    // If we are monthly, we would rather have a ByMonthDay than a ByDay:
    if ( !( iSetSegs & ( ESegFlagByDay | ESegFlagByMonthDay ) ) )
        {
        // The specification requires certain fields to be taken from the DTSTART
        // if they have not already been set.
        TDateTime startDate = aStartTime.TimeUtcL().DateTime();

        RArray<TInt> days;
        CleanupClosePushL(days);
        User::LeaveIfError(days.Append(startDate.Day() + 1));
        aRule.SetByMonthDay(days);
        CleanupStack::PopAndDestroy(&days);
        }

    return finalized;
    }

