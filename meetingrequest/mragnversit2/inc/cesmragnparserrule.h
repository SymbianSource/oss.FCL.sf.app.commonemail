/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CESMRAGNPARSERRULE_H
#define CESMRAGNPARSERRULE_H


// System includes.
#include <e32base.h>

//User includes
#include "mesmragnimputil.h"

// Forward class declarations.
class TCalRRule;
class CESMRICalRuleSegment;

class CESMRAgnParseRRule : public CBase
    {
    public:
        virtual ~CESMRAgnParseRRule();
    
    protected://Constructor
        CESMRAgnParseRRule( MESMRAgnImpUtil& aAgnImpUtil );
    
    public:
        //template method calls the real parser
        TBool ParseL( TCalRRule& aRule,
        			  RPointerArray<CESMRICalRuleSegment>& aRecRules,
        			  const TCalTime& aStartTime,
        			  const TDesC8& aUid,
        			  const RPointerArray<CESMRICalValue>& aRulevalues );
        
    protected://parsing functions
        virtual TBool DoParseL( CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime );
    
    protected://update TCalRRule
        virtual TBool SetRRuleUntilL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleWkStL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleByDayL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleByMonthL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleByMonthDayL( const CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
    
    protected:
        enum TSegFlags
		    {
    		ESegFlagByDay		= 1 << CESMRICalRuleSegment::ESegByDay,
    		ESegFlagByMonthDay	= 1 << CESMRICalRuleSegment::ESegByMonthDay,
    		ESegFlagByMonth		= 1 << CESMRICalRuleSegment::ESegByMonth
    		};
	
    protected:
        MESMRAgnImpUtil& iAgnImpUtil;
        TInt iSetSegs;
    };

#endif	// CESMRAGNPARSERRULE_H
