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
* Description:  Implements the import functionality of RRule
*
*
*/

#ifndef CESMRAGNPARSERRULEDAILY_H
#define CESMRAGNPARSERRULEDAILY_H

#include "cesmragnparserrule.h"

/**
 * Encapsulates a single CESMRAgnParseRRule handling
 */
class CESMRAgnParseRRuleDaily : CESMRAgnParseRRule
    {
public:
    CESMRAgnParseRRuleDaily();
    virtual ~CESMRAgnParseRRuleDaily();

public:
    /**
     * Constructs a new CESMRAgnParseRRuleDaily and returns it. The object is left on the
     * cleanup stack.
     * @param aAgnImpUtil AgnVersit2Importer utility functions
     * @return a new CESMRAgnParseRRuleDaily.
     */
    static CESMRAgnParseRRule* NewLC( MESMRAgnImpUtil& aAgnImpUtil );

private://Constructor
    CESMRAgnParseRRuleDaily( MESMRAgnImpUtil& aAgnImpUtil );

protected:
    /**
     * Parse the keywords and update RRule
     * @param aSegment recurrence rule value segment.
     * @param aRule recurrence rules
     * @return True if parsed the keyword successfully
     */
    virtual TBool DoParseL( CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
    
    /**
     * Finalize the parsing
     * @param aRule recurrence rules
     * @param aStartTime The DTSTART of the parent component.
     * @return True if finalized parsing successfully
     */
    virtual TBool FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime );
    };

#endif  // CAgnParseRRuleDaily_H
