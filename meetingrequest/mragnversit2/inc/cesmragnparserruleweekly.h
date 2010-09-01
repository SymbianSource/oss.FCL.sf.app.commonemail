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

#ifndef CESMRAGNPARSERRULEWEEKLY_H
#define CESMRAGNPARSERRULEWEEKLY_H

#include "cesmragnparserrule.h"

/**
 * An encapsulated CESMRAgnParseRRule for weeks
 */
class CESMRAgnParseRRuleWeekly : CESMRAgnParseRRule
    {
public:
    /**
     * C++ Constructor.
     */
    CESMRAgnParseRRuleWeekly();
    
    /**
     * C++ Destructor.
     */
    virtual ~CESMRAgnParseRRuleWeekly();

public:
    /**
     * Constructs a new CESMRAgnParseRRuleWeekly and returns it. The object is left on the
     * cleanup stack.
     * @param aAgnImpUtil AgnVersit2Importer utility functions
     * @return a new CESMRAgnParseRRuleWeekly.
     */
    static CESMRAgnParseRRule* NewLC( MESMRAgnImpUtil& aAgnImpUtil );

private://Constructor
    CESMRAgnParseRRuleWeekly( MESMRAgnImpUtil& aAgnImpUtil );

protected://From CESMRAgnParseRRule
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

protected:
    /**
     * Update TCalRRule with by days
     * @param aSegment recurrence rule value segment.
     * @param aRule recurrence rules
     * @return true if aRule updated successfully
     */
    virtual TBool SetRRuleByDayL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
    };

#endif  // CESMRAgnParseRRuleWeekly_h
