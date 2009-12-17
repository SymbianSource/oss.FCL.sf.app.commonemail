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
* Description: This file implements class CESMRAgnParseRRuleDaily.
*
*/


#include "emailtrace.h"
#include "cesmragnparserruledaily.h"

//<cmail>
//</cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleDaily::CESMRAgnParseRRuleDaily
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleDaily::CESMRAgnParseRRuleDaily( MESMRAgnImpUtil& aAgnImpUtil ):
                        CESMRAgnParseRRule( aAgnImpUtil )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleDaily::~CESMRAgnParseRRuleDaily
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRuleDaily::~CESMRAgnParseRRuleDaily()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleDaily::NewLC
// ---------------------------------------------------------------------------
//
CESMRAgnParseRRule* CESMRAgnParseRRuleDaily::NewLC( MESMRAgnImpUtil& aAgnImpUtil )
    {
    FUNC_LOG;

    CESMRAgnParseRRule *self = new (ELeave) CESMRAgnParseRRuleDaily( aAgnImpUtil );
    CleanupStack::PushL( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleDaily::DoParseL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleDaily::DoParseL( CESMRICalRuleSegment& aSegment, TCalRRule& /*aRule*/ )
    {
    FUNC_LOG;

    TBool parsed;

    switch( aSegment.Type() )
        {
        default:
            {
            parsed = EFalse;
            }
        }

    return parsed;
    }

// ---------------------------------------------------------------------------
// CESMRAgnParseRRuleDaily::FinalizeParsingL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnParseRRuleDaily::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    FUNC_LOG;

    //Call the parent, to set some common values
    TBool finalized = CESMRAgnParseRRule::FinalizeParsingL( aRule, aStartTime );

    return finalized;
    }

