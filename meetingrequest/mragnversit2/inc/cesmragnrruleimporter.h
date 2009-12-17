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

#ifndef CESMRAGNRRULEIMPORTER_H
#define CESMRAGNRRULEIMPORTER_H

#include <e32base.h>

//System includes.
#include <calentry.h>       // for CCalEntry

//User includes
#include "mesmragnimputil.h"    // for MESMRAgnImpUtil

//Forward class declarations.
class CESMRICal;
class CESMRICalComponent;
class CESMRICalProperty;
class CCalEntry;
class CESMRAgnParseRRule;

//Class declaration
/**
 *  A base class for all CESMRAgnRRuleImporter rules
 */
class CESMRAgnRRuleImporter : public CBase
    {
public:
    /**
     * Constructs a new CESMRAgnRRuleImporter and returns it.
     * The object is left on the cleanup stack.
     * @param aAgnImpUtil AgnVersit2Importer utility functions
     * @return a new CESMRAgnParseRRuleYearly.
     */
    static CESMRAgnRRuleImporter* NewLC( MESMRAgnImpUtil& aAgnImpUtil );
    /**
     * Constructs a new CESMRAgnRRuleImporter and returns it.
     * @param aAgnImpUtil AgnVersit2Importer utility functions
     * @return a new CESMRAgnParseRRuleYearly.
     */
    static CESMRAgnRRuleImporter* NewL( MESMRAgnImpUtil& aAgnImpUtil );

    /**
     * C++ Destructor.
     */
    virtual ~CESMRAgnRRuleImporter();

private:
    CESMRAgnRRuleImporter( MESMRAgnImpUtil& aAgnErrorReporter );
    void ConstructL();

private:
    CESMRAgnParseRRule* CreateParserLC( const CESMRICalRuleSegment::TFreq aFreq, TCalRRule& rule );

public:
    /**
     * A function to translate an RRULE property into a TCalRRule for Agenda.
     * @param aProperty The RRULE property to translate.
     * @param aEntry The entry to add the rules to.
     * @param aStartTime The DTSTART of the parent component.
     * @return ETrue if the rule is imported, else EFalse.
     */
    TBool ImportL( const CESMRICalProperty& aProperty, CCalEntry& aEntry, const TCalTime& aStartTime );
    
    MESMRAgnImpUtil& iAgnImpUtil;
    };

#endif 
