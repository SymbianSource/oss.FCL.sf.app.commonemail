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
* Description:  Implements the export functionality of CESMRAgnVersit2
*
*
*/

#ifndef CESMRAGNVERSIT2EXPORTER_H
#define CESMRAGNVERSIT2EXPORTER_H

// Agenda includes.
#include <calentry.h>   // for CCalEntry::TReplicationStatus
#include <caluser.h>    // for CCalAttendee::TCalRole

// Forward class declarations.
class CESMRAgnVersit2StringProvider;
class CESMRICal;
class CESMRICalComponent;
class CESMRICalProperty;
class CTzRules;
class MESMRAgnExportObserver;
class RWriteStream;

/**
 * Implementation class for CESMRAgnVersit2::ExportL.
 * @internalTechnology
 */
class CESMRAgnVersit2Exporter : public CBase
    {
public:     // Construction/destruction.
    /**
     * Constructs a new CESMRAgnVersit2Exporter and returns it.
     * @param aObserver Application interface for error reporting.
     * @param aWriteStream Stream to write output to.
     * @param aStringProvider localisable string resources
     * @return The new CESMRAgnVersit2Exporter
     * @internalTechnology
     */
    static CESMRAgnVersit2Exporter* NewL(CESMRAgnVersit2StringProvider& aStringProvider);
    
    /**
     * Constructs a new CESMRAgnVersit2Exporter and returns it.
     * A pointer to the new object is left on the cleanup stack.
     * @param aObserver Application interface for error reporting.
     * @param aWriteStream Stream to write output to.
     * @param aStringProvider localisable string resources
     * @return The new CESMRAgnVersit2Exporter
     * @internalTechnology
     */
    static CESMRAgnVersit2Exporter* NewLC(CESMRAgnVersit2StringProvider& aStringProvider);

    /**
     * C++ Destructor.
     */
    ~CESMRAgnVersit2Exporter();

public:     // Methods.
    /**
     * Exports a given calendar entry.
     * @param aEntry entry to export.
     * @param aFlags not used.
     * @internalTechnology
     */
    void ExportL(const CCalEntry& aEntry,
                 RWriteStream& aWriteStream, 
                 TUint aExportFlags,
                 MESMRAgnExportObserver& aObserver);
    
    /**
     * Exports an array of calendar entries to a single iCalendar
     * @param aEntries array of entries to export.
     * @param aFlags not used.
     * @internalTechnology
     */
    void ExportL(RPointerArray<CCalEntry>& aEntries,
                 RWriteStream& aWriteStream, 
                 TUint aExportFlags,
                 MESMRAgnExportObserver& aObserver);
    
public:
    /**
     * Constructs mail address buffer,
     * inserts MAILTO: prefix if it's not already available
     * @param aAddress Mail address
     * @internalTechnology
     */
    static HBufC* ConstructMailtoPropertyLC(const TDesC& aAddress);

private:    // Construction.
    CESMRAgnVersit2Exporter(CESMRAgnVersit2StringProvider& aStringProvider);

    void ConstructL();

private:    // Methods.
    CESMRICal* AddICalLC(const CCalEntry& aEntry) const;

    // Add Component methods.
    void AddAnnivL(CESMRICalComponent& aAnniv, const CCalEntry& aEntry) const;
    void AddAlarmL(CESMRICalComponent& aComponent, const CCalEntry& aEntry) const;
    void AddApptL(CESMRICalComponent& aAppt, const CCalEntry& aEntry) const;
    void AddComponentL(CESMRICal& aICal, const CCalEntry& aEntry) const;
    void AddEventL(CESMRICalComponent& aEvent, const CCalEntry& aEntry) const;
    void AddReminderL(CESMRICalComponent& aReminder, const CCalEntry& aEntry) const;
    void AddTimezoneL(CESMRICal& aICal, const CCalEntry& aEntry);
    void AddTodoL(CESMRICalComponent& aTodo, const CCalEntry& aEntry) const;

    // Add Property methods.
    void AddAttendeePropertiesL(CESMRICalComponent& aComponent, const CCalEntry& aEntry) const;
    void AddCategoriesPropertyL(CESMRICalComponent& aComponent, const CCalEntry& aEntry)const ;
    void AddClassPropertyL(CESMRICalComponent& aComponent, const CCalEntry& aEntry) const;
    void AddExceptionDatePropertyL(CESMRICalComponent& aComponent, const CCalEntry& aEntry) const;
    void AddIntegerPropertyL(CESMRICalComponent& aComponent, TInt aInt, const TDesC& aProperty) const;
    void AddOrganizerPropertyL(CESMRICalComponent& aComponent, const CCalEntry& aEntry) const;
    void AddTextPropertyL(CESMRICalComponent& aComponent, const TDesC& aText, const TDesC& aProperty) const;
    void AddRDatePropertyL(CESMRICalComponent& aComponent, const CCalEntry& aEntry) const;
    void AddRRulePropertyL(CESMRICalComponent& aComponent, const CCalEntry& aEntry)const;
    CESMRICalProperty& AddDateTimePropertyL(CESMRICalComponent& aComponent, const TCalTime& aTime, const TDesC& aProperty) const;
    CESMRICalProperty& AddFloatingDateTimePropertyL(CESMRICalComponent& aComponent, const TTime& aFloatingTime, const TDesC& aProperty) const;
    CESMRICalProperty& AddUtcDateTimePropertyL(CESMRICalComponent& aComponent, const TTime& aUtcTime, const TDesC& aProperty) const;

    // Helper methods.
    static const TDesC& ClassStringL(const CCalEntry::TReplicationStatus aStatus);
    const TDesC& CategoryStringL(const CCalCategory& aCategory) const;
    static const TDesC& RoleFromEnum(CCalAttendee::TCalRole aRole);
    static const TDesC& StatusFromEnum(CCalAttendee::TCalStatus aStatus);
    static const TDesC& DayFromInt(TUint8 aDayInt);
    static const TDesC&  DayFromTDay(TDay aDay);
    static TInt WeekNumFromDayOfMonth(TInt aDayOfMonth);

private:    // Members.
    MESMRAgnExportObserver* iObserver;
    CESMRAgnVersit2StringProvider& iStringProvider;
    RPointerArray<HBufC> iTzNameArray;
    RPointerArray<CTzRules> iTimeZoneArray;
    TUint iExportProperties;
    TInt iTimezoneIndex;
    };

#endif // CESMRAGNVERSIT2EXPORTER_H

// End of file.
