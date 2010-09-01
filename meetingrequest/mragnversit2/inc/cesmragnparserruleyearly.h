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


#ifndef CESMRAGNPARSERRULEYEARLY_H
#define CESMRAGNPARSERRULEYEARLY_H

#include "cesmragnparserrule.h"

class CESMRAgnParseRRuleYearly : CESMRAgnParseRRule
    {
    public:
    	CESMRAgnParseRRuleYearly();
    	virtual ~CESMRAgnParseRRuleYearly();
    
    public:
        static CESMRAgnParseRRule* NewLC( MESMRAgnImpUtil& aAgnImpUtil );
    
    private://Constructor
        CESMRAgnParseRRuleYearly( MESMRAgnImpUtil& aAgnImpUtil );
    
    protected:
        virtual TBool DoParseL( CESMRICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime );
    };

#endif	// CESMRAGNPARSERRULEYEARLY_H
