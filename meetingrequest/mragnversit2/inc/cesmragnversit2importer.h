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
* Description:  Implements the import functionality of CESMRAgnVersit2
*
*
*/

#ifndef CESMRAGNVERSIT2IMPORTER_H
#define CESMRAGNVERSIT2IMPORTER_H

// System includes.
#include <calentry.h>       // for CCalEntry::TType
//<cmail>
#include "cesmricalrulesegment.h"
//</cmail>

// User includes.
#include "mesmragnimputil.h"

// Forward class declarations.
class CESMRAgnVersit2StringProvider;
class CESMRICal;
class CESMRICalComponent;
class CESMRICalProperty;
class CTzRules;
class RReadStream;

/**
 * Implementation class for CESMRAgnVersit2::ImportL.
 * @internalTechnology
 */
class CESMRAgnVersit2Importer : public CBase, public MESMRAgnImpUtil
    {
private:    // Types
    /**
     * Nested class used internally to maintain a set of rules searchable by TZID.
     * @internalTechnology
     */
    class CTzNamedRules : public CBase
        {
    public:
        /**
         * C++ Constructor.
         */
        CTzNamedRules();
        /**
         * C++ Destructor.
         */
        ~CTzNamedRules();

    public: // Members
        CTzRules* iRules;
        HBufC* iName;
        };

public:     // Construction/destruction.
    /**
     * Constructs a new CESMRAgnVersit2Importer and returns it.
     * @param aStringProvider A string provider for obtaining resource strings from.
     * @return a new CESMRAgnVersit2Importer.
     * @internalTechnology
     */
    static CESMRAgnVersit2Importer* NewL(CESMRAgnVersit2StringProvider& aStringProvider);
    
    /**
     * Constructs a new CESMRAgnVersit2Importer and returns it. The object is left on the
     * cleanup stack.
     * @param aStringProvider A string provider for obtaining resource strings from.
     * @return a new CESMRAgnVersit2Importer.
     * @internalTechnology
     */
    static CESMRAgnVersit2Importer* NewLC(CESMRAgnVersit2StringProvider& aStringProvider);
    ~CESMRAgnVersit2Importer();

public:     // Methods.
    /**
     * Uses Versit2 to parse an input stream, and then converts the result into
     * CCalEntry objects.
     * @param aEntries The array to append entries to.
     * @param aReadStream The stream to read iCalendar data from.
     * @param aObserver Application interface for error reporting.
     * @internalTechnology
     */
    void ImportL(RPointerArray<CCalEntry>& aEntries, RReadStream& aReadStream, MESMRAgnImportObserver& aObserver);

private:    // Construction.
    CESMRAgnVersit2Importer(CESMRAgnVersit2StringProvider& aStringProvider);
    void ConstructL();

private:    // Methods.
    void ImportICalL(RPointerArray<CCalEntry>& aEntries, CESMRICal& aCal);

    // Import component methods.
    void ImportComponentL(const CESMRICal& aCal, const CESMRICalComponent& aComponent, RPointerArray<CCalEntry>& aEntries);
    void ImportAlarmL(const CESMRICalComponent& aComponent, CCalEntry& aEntry, const HBufC8& aUid, const TCalTime* aStartTime);
    void ImportEntryL(const CESMRICal& aCal, const CESMRICalComponent& aComponent, RPointerArray<CCalEntry>& aEntries, HBufC8* aUid, CCalEntry::TType aType);
    CCalEntry* CreateEntryLC(const CESMRICal& aCal, const CESMRICalComponent& aComponent, HBufC8* aUid, CCalEntry::TType aType);
    void ImportPropertyL(CCalEntry& aEntry, const CESMRICalProperty& aProperty);
    void ImportTimezoneL(const CESMRICalComponent& aTimezone);
    void ImportTimezoneIntervalL(const CESMRICalComponent& aInterval, CTzRules& aRules);

    // Import property methods.
    void ImportCategoriesL(const CESMRICalProperty& aProperty, CCalEntry& aEntry) const;
    void ImportClassL(const CESMRICalProperty& aProperty, CCalEntry& aEntry);
    TBool ImportRRuleL(const CESMRICalProperty& aProperty, CCalEntry& aEntry, const TCalTime& aStartTime);

    // Helper methods.
    void GetCalTimeL(const CESMRICalProperty& aProperty, TCalTime& aTime, TValueType aValueType, TInt aValue = 0);
    TInt FindRuleSegment(const RPointerArray<CESMRICalRuleSegment>& aRules, CESMRICalRuleSegment::TSegmentType aType) const;
    const CTzRules* FindTzRule(const TDesC& aName) const;
    void ReportErrorL(MESMRAgnImportObserver::TImpError aType, const TDesC8& aUid, const TDesC& aContext, TBool aCanContinue = ETrue);

private:    // Members.
    CESMRAgnVersit2StringProvider& iStringProvider;
    MESMRAgnImportObserver* iImportObserver;
    MESMRAgnImportObserver::TImpResponse iResponse;
    RPointerArray<CTzNamedRules> iTzRules;
    const CTzRules* iCurrentTzRules;    // This is not owned by the class and should not be deleted by the class.
    TBool iNeedsTzRules;
    };

#endif

// End of file.
