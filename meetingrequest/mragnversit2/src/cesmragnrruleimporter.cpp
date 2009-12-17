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
* Description: This file implements class CESMRAgnRRuleImporter.
*
*/


#include "emailtrace.h"
#include "cesmragnrruleimporter.h"

//<cmail>

// Agenda includes.
#include <calrrule.h>              // For CCalRRule

// Versit includes.
#include "esmricalkeywords.h"          // for cesmricalkeywords
#include "cesmricalproperty.h"         // for cesmricalproperty
#include "cesmricalvalue.h"            // for cesmricalvalue
//</cmail>


#include "esmrcleanuppointerarray.h"   // For TCleanupPointerArray

// RRule Parser includes
#include "cesmragnparserruleyearly.h"  // For CESMRAgnParseRRuleYearly
#include "cesmragnparserrulemonthly.h" // For CESMRAgnParseRRuleMonthly
#include "cesmragnparserruleweekly.h"  // For CESMRAgnParseRRuleWeekly
#include "cesmragnparserruledaily.h"   // For CESMRAgnParseRRuleDaily

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnRRuleImporter::CESMRAgnRRuleImporter
// ---------------------------------------------------------------------------
//
CESMRAgnRRuleImporter::CESMRAgnRRuleImporter( MESMRAgnImpUtil& aAgnImpUtil ): iAgnImpUtil( aAgnImpUtil )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnRRuleImporter::~CESMRAgnRRuleImporter
// ---------------------------------------------------------------------------
//
CESMRAgnRRuleImporter::~CESMRAgnRRuleImporter()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnRRuleImporter::NewL
// ---------------------------------------------------------------------------
//
CESMRAgnRRuleImporter* CESMRAgnRRuleImporter::NewL( MESMRAgnImpUtil& aAgnImpUtil )
    {
    FUNC_LOG;
    CESMRAgnRRuleImporter *self = CESMRAgnRRuleImporter::NewLC( aAgnImpUtil );
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnRRuleImporter::NewLC
// ---------------------------------------------------------------------------
//
CESMRAgnRRuleImporter* CESMRAgnRRuleImporter::NewLC( MESMRAgnImpUtil& aAgnImpUtil )
    {
    FUNC_LOG;

    CESMRAgnRRuleImporter *self = new (ELeave)CESMRAgnRRuleImporter( aAgnImpUtil );
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnRRuleImporter::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAgnRRuleImporter::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnRRuleImporter::ImportL
// ---------------------------------------------------------------------------
//
TBool CESMRAgnRRuleImporter::ImportL( const CESMRICalProperty& aProperty, CCalEntry& aEntry, const TCalTime& aStartTime )
    {
    FUNC_LOG;

    //===============================
    //Frequency and recurrency rules
    //===============================
    const RPointerArray<CESMRICalValue>& rulevalues = aProperty.Values();

    // There should never be less than 1 value.
    if (rulevalues.Count() < 1)
        {
        // We can't continue so abort.
        iAgnImpUtil.ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalRRule, EFalse);
        }

    CESMRICalRuleSegment::TFreq freq = CESMRICalRuleSegment::EFreqDaily;
    RPointerArray<CESMRICalRuleSegment> recRules;
    CleanupPointerArrayPushL(recRules);

    // At this point we take ownership of the things which rules contains:
    rulevalues[0]->GetRecurrenceRuleL(recRules);

    // An RRule must have a frequency.
    TInt pos = iAgnImpUtil.FindRuleSegment(recRules, CESMRICalRuleSegment::ESegFreq);

    if (pos != KErrNotFound)
        {
        ASSERT(recRules.Count() >= pos + 1);
        freq = recRules[pos]->FreqL();
        }
    else
        {
        iAgnImpUtil.ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), rulevalues[0]->TextL());
        // On continue, EFreqDaily will be used.
        }

    //===================================
    // Create Parser and parse the rrule
    //===================================
    TBool parsed = EFalse;
    TCalRRule rule;

    //Create parser and parse the recurrency rule
    CESMRAgnParseRRule* parser = CreateParserLC( freq, rule );

    if( parser )
        {
        //if parser returns true the RRule was parsed successfully
        parsed = parser->ParseL( rule, recRules, aStartTime, aEntry.UidL(), rulevalues );
        CleanupStack::PopAndDestroy( parser );
        }
    else
        {
        parsed = EFalse;
        iAgnImpUtil.ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, aEntry.UidL(), rulevalues[0]->TextL());
        }

    //=================================
    // Update aEntry if parsed is true
    //=================================
    if ( parsed )
        {
        aEntry.SetRRuleL(rule);
        }

    CleanupStack::PopAndDestroy(&recRules);

    return parsed;
    }

// ---------------------------------------------------------------------------
// CESMRAgnRRuleImporter::CreateParserLC
// ---------------------------------------------------------------------------
//
/**
Factory method to create the RRule parser
@param aFreq The RRULE frequency
@param aRule rule to add the frequency to
@return returns NULL if parser for CESMRICalRuleSegment::TFreq type not found, otherwise parser
*/
CESMRAgnParseRRule* CESMRAgnRRuleImporter::CreateParserLC( const CESMRICalRuleSegment::TFreq aFreq, TCalRRule& aRule )
    {
    FUNC_LOG;

    CESMRAgnParseRRule* parser;

    switch ( aFreq )
        {
        //Daily
        case CESMRICalRuleSegment::EFreqDaily :
            {
            aRule.SetType(TCalRRule::EDaily);
            parser = CESMRAgnParseRRuleDaily::NewLC( iAgnImpUtil );
            break;
            }
        //Weekly
        case CESMRICalRuleSegment::EFreqWeekly :
            {
            aRule.SetType(TCalRRule::EWeekly);
            parser = CESMRAgnParseRRuleWeekly::NewLC( iAgnImpUtil );
            break;
            }
        //Monthly
        case CESMRICalRuleSegment::EFreqMonthly :
            {
            aRule.SetType(TCalRRule::EMonthly);
            parser = CESMRAgnParseRRuleMonthly::NewLC( iAgnImpUtil );
            break;
            }
        //Yearly
        case CESMRICalRuleSegment::EFreqYearly :
            {
            aRule.SetType(TCalRRule::EYearly);
            parser = CESMRAgnParseRRuleYearly::NewLC( iAgnImpUtil );
            break;
            }
        //Not supported
        case CESMRICalRuleSegment::EFreqSecondly :  // Not supported, fall through...
        case CESMRICalRuleSegment::EFreqMinutely :  // Not supported, fall through...
        case CESMRICalRuleSegment::EFreqHourly :    // Not supported, fall through...
        default :
            {
            parser = NULL;
            }

        }

    return parser;
    }

