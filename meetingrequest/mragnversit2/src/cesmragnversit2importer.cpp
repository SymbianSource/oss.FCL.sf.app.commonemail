/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements classes CTzNamedRules, CESMRAgnVersit2Importer. 
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmragnversit2importer.h"
#include "cesmragnversit2exporter.h"

//debug
//<cmail>

// Agenda includes.
#include <calalarm.h>           // For CCalAlarm
#include <calcategory.h>        // For CCalCategory
#include <calentry.h>           // For CCalEntry
#include <calrrule.h>           // For CCalRRule
#include <caltime.h>            // For CCalTime
#include <caluser.h>            // For CCalUser
#include <tzdefines.h>          // For TTzRuleDay
#include <vtzrules.h>           // For CTzRules and TTzRule

// Versit includes.
#include "cesmrical.h"              // for cical
#include "cesmricalcomponent.h"     // for cicalcomponent
#include "esmricalkeywords.h"       // for cicalkeywords
#include "cesmricalparser.h"            // for cicalparser
#include "cesmricalproperty.h"      // for cicalproperty
#include "cesmricalpropertyparam.h" // for cicalpropertyparam
#include "cesmricalrulesegment.h"   // for cicalrulesegment
#include "cesmricalvalue.h"         // for cicalvalue

// ESMRAgnVersit2 includes.
#include "mesmragnimportobserver.h"         // For MESMRAgnImportObserver
#include "cesmragnversit2stringprovider.h"  // For CESMRAgnVersit2StringProvider
#include "esmrcleanuppointerarray.h"        // For TCleanupPointerArray

#include "cesmragnrruleimporter.h"           // For CESMRAgnRRuleImporter
#include "esmrhelper.h"

// MRUI includes
#include "esmrdef.h"
//</cmail>

// Constants.

//
//    CTzNamedRules
//    =============
//

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::CTzNamedRules::CTzNamedRules
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Importer::CTzNamedRules::CTzNamedRules()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::CTzNamedRules::~CTzNamedRules
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Importer::CTzNamedRules::~CTzNamedRules()
    {
    FUNC_LOG;

    delete iRules;
    delete iName;

    }

//
//     CESMRAgnVersit2Importer
//     ===================
//

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::NewL
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Importer* CESMRAgnVersit2Importer::NewL(CESMRAgnVersit2StringProvider& aStringProvider)
    {
    FUNC_LOG;

    CESMRAgnVersit2Importer* self = CESMRAgnVersit2Importer::NewLC(aStringProvider);
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::NewLC
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Importer* CESMRAgnVersit2Importer::NewLC(CESMRAgnVersit2StringProvider& aStringProvider)
    {
    FUNC_LOG;

    CESMRAgnVersit2Importer* self = new (ELeave) CESMRAgnVersit2Importer(aStringProvider);
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::NewL
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Importer::~CESMRAgnVersit2Importer()
    {
    FUNC_LOG;

    iTzRules.ResetAndDestroy();

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::ImportL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2Importer::ImportL( RPointerArray<CCalEntry>& aEntries,
                                   RReadStream& aReadStream,
                                   MESMRAgnImportObserver& aObserver )
    {
    FUNC_LOG;

    iImportObserver = &aObserver;

    CESMRICalParser* parser = CESMRICalParser::NewLC();
    parser->InternalizeL(aReadStream);

    const TInt count = parser->CalCount();

    for (TInt cal = 0; cal < count; ++cal)
        {
        CESMRICal& calendar = parser->Cal(cal);
        TRAPD(err, ImportICalL(aEntries, calendar));
        iTzRules.ResetAndDestroy();
        User::LeaveIfError(err);
        }

    CleanupStack::PopAndDestroy(parser);
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::CESMRAgnVersit2Importer
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Importer::CESMRAgnVersit2Importer(CESMRAgnVersit2StringProvider& aStringProvider)
    : iStringProvider(aStringProvider)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Importer::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2Importer::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

/**
 * Takes a calendar object from Versit2 and converts it into a set of CCalEntry
 * objects.
 * @param aEntries The array to append entries to.
 * @param aCal An object of type CESMRICal from which to obtain data.
 * @leave KErrAbort, or any other system wide error code.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportICalL(RPointerArray<CCalEntry>& aEntries, CESMRICal& aCal)
    {
    FUNC_LOG;

    const RPointerArray<CESMRICalComponent>& components = aCal.Components();

    // Run through the timezones so that we know what other components are referring to:
    iTzRules.ResetAndDestroy();
    TInt com; // current component number
    TInt comCount = components.Count();
    for (com = 0; com < comCount && iResponse == MESMRAgnImportObserver::EImpResponseContinue; ++com)
        {
        CESMRICalComponent* component = components[com];

        // We are only interested in timezones:
        if (component->Type() == CESMRICalBase::EICalTimeZone)
            {
            TRAPD(err, ImportTimezoneL(*component));
            if (err)
                {
                // If there isn't a response, then it isn't a recoverable leave:
                if (iResponse == MESMRAgnImportObserver::EImpResponseContinue)
                    {
                    User::Leave(err);
                    }
                // If there is a response, then we threw this after reporting an
                // error to the observer, and we may be able to continue:
                else if(iResponse == MESMRAgnImportObserver::EImpResponseSkip)
                    {
                    iResponse = MESMRAgnImportObserver::EImpResponseContinue;
                    }
                }
            }
        }

    // Run through the other components:
    for (com = 0; com < comCount && iResponse == MESMRAgnImportObserver::EImpResponseContinue; ++com)
        {
        CESMRICalComponent* component = components[com];

        // We aren't interested in timezones:
        if (component->Type() != CESMRICalBase::EICalTimeZone)
            {
            TRAPD(err, ImportComponentL(aCal, *component, aEntries));
            if (err)
                {
                // If there isn't a response, then it isn't a recoverable leave:
                if (iResponse == MESMRAgnImportObserver::EImpResponseContinue)
                    {
                    User::Leave(err);
                    }
                // If there is a response, then we threw this after reporting an
                // error to the observer, and we may be able to continue:
                else if(iResponse == MESMRAgnImportObserver::EImpResponseSkip)
                    {
                    iResponse = MESMRAgnImportObserver::EImpResponseContinue;
                    }
                }
            }
        }

    // If we have been asked to leave after an error, do so:
    if (iResponse == MESMRAgnImportObserver::EImpResponseLeave)
        {
        User::Leave(KErrAbort);
        }

    }

/**
*     IMPORT COMPONENT FUNCTIONS
*     ==========================
*/

/**
 * Handles the importing of a single component, other than a timezone.
 * @param aCal The calendar object from which we are importing
 * @param aComponent The specific component we are importing
 * @param aEntries The agenda entries onto which this component is to be appended.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportComponentL( const CESMRICal& aCal,
                                            const CESMRICalComponent& aComponent,
                                            RPointerArray<CCalEntry>& aEntries )
    {
    FUNC_LOG;

    // Get the UID:
    HBufC8* entryUid = NULL;
    const CESMRICalProperty* propUid = aComponent.FindProperty(KICalUid());


    if (!propUid)//(propUid && propUid->Values().Count() < 1)
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorInvalidData, KNullDesC8, KICalUid, EFalse);
        // We do not continue without a UID.
        }

    //if (propUid)
    else
        {
        entryUid = propUid->Values()[0]->BinaryLC();

        // Import entry-type specific data:
        switch (aComponent.Type())
            {
            case CESMRICalBase::EICalEvent :
                {
                CCalEntry::TType eventType = CCalEntry::EEvent;

                //Check if we have a DTEND or DURATION
                if (aComponent.FindProperty(KICalDtend()) || aComponent.FindProperty(KICalDuration()))
                    {
                    eventType = CCalEntry::EAppt;
                    }

                else
                    {
                    //Check if we have a DTSTART
                    if (aComponent.FindProperty(KICalDtstart()))
                        {
                        eventType = CCalEntry::EReminder;
                        }

                    //Check if this is a yearly repeat
                    if (aComponent.FindProperty(KICalRRule()))
                        {
                        const CESMRICalProperty* rrule = aComponent.FindProperty(KICalRRule());
                        const RPointerArray<CESMRICalValue>& rulevalues = rrule->Values();

                        if (rulevalues.Count() >= 1)
                            {
                            CESMRICalRuleSegment::TFreq freq = CESMRICalRuleSegment::EFreqDaily;
                            RPointerArray<CESMRICalRuleSegment> recRules;
                            CleanupPointerArrayPushL(recRules);

                            // At this point we take ownership of the things which rules contains:
                            rulevalues[0]->GetRecurrenceRuleL(recRules);

                            // An RRule must have a frequency.
                            TInt pos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegFreq);
                            if (pos != KErrNotFound)
                                {
                                ASSERT(recRules.Count() >= pos + 1);
                                freq = recRules[pos]->FreqL();
                                }

                            // Find the interval.
                            pos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegInterval);
                            TInt interval = 1; //default value if no INTERVAL specified
                            if (pos != KErrNotFound)
                                {
                                ASSERT(recRules[pos]->Values().Count() >= 0);
                                interval = recRules[pos]->Values()[0]->IntegerL();
                                }

                            if ((freq == CESMRICalRuleSegment::EFreqYearly) && (interval == 1))
                                {
                                //Anniversary repeats once a year
                                eventType = CCalEntry::EAnniv;
                                }

                            CleanupStack::PopAndDestroy(&recRules);
                            }
                        }
                    }
                CleanupStack::Pop(entryUid);
                // Passing ownership of entryUid
                ImportEntryL(aCal, aComponent, aEntries, entryUid, eventType);

                break;
                }
            case CESMRICalBase::EICalTodo :
                CleanupStack::Pop(entryUid);
                // Passing ownership of entryUid
                ImportEntryL(aCal, aComponent, aEntries, entryUid, CCalEntry::ETodo);
                break;
            case CESMRICalBase::EICalJournal :
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, *entryUid, KICalJournal, EFalse);
                // Continue equates to skip in this instance.
                break;
            case CESMRICalBase::EICalAlarm :
                ReportErrorL(MESMRAgnImportObserver::EImpErrorInvalidData, *entryUid, KICalAlarm, EFalse);
                // Continue equates to skip in this instance.
                break;
            case CESMRICalBase::EICalFreeBusy :
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, *entryUid, KICalFreeBusy, EFalse);
                // Continue equates to skip in this instance.
                break;
            case CESMRICalBase::EICalInvalid :  // not valid here, fall through
            case CESMRICalBase::EICalCalendar : // not valid here, fall through
            case CESMRICalBase::EICalTimeZone : // not valid here, fall through
            case CESMRICalBase::EICalStandard : // not valid here, fall through
            case CESMRICalBase::EICalDaylight : // not valid here, fall through
            default :
                ReportErrorL( MESMRAgnImportObserver::EImpErrorInvalidData,
                              *entryUid,
                              aComponent.TypeStringL(),
                              EFalse );
                // Continue equates to skip in this instance.
                break;
            }
        }
    }

/**
 * Checks for a VALARM in a component. If a valid one is found, it is added to the
 * specified entry.
 * @param aComponent Component to search for a VALARM in.
 * @param aEntry Alarm is added to this calendar entry.
 * @param aUid UID of the component.
 * @param aStartTime A pointer to the DTSTART of the parent component, if it has
 * one, or NULL if it does not. This does not take ownership.
 * @return MESMRAgnImportObserver::TImpResponse indicating application response to errors, if any.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportAlarmL( const CESMRICalComponent& aComponent,
                                        CCalEntry& aEntry,
                                        const HBufC8& aUid,
                                        const TCalTime* aStartTime )
    {
    FUNC_LOG;

    if (aComponent.ComponentExists(CESMRICalBase::EICalAlarm))
        {
        // Check the outer component is a VEVENT or a VTODO
        if (aComponent.Type() == CESMRICalBase::EICalEvent || aComponent.Type() == CESMRICalBase::EICalTodo)
            {
            const RPointerArray<CESMRICalComponent>& nestedComponents = aComponent.Components();
            // Iterate through all the nested components and create a new alarm for each VALARM component
            TInt nestedComCount = nestedComponents.Count();
            for (TInt i = 0; i < nestedComCount; i++)
                {
                if (nestedComponents[i]->Type() == CESMRICalBase::EICalAlarm)
                    {
                    CCalAlarm* alarm = CCalAlarm::NewL();
                    CleanupStack::PushL(alarm);
                    const RPointerArray<CESMRICalProperty>& nestedProperties = nestedComponents[i]->Properties();
                    // Go through all the nested properties until we find the TRIGGER
                    // Ignore REPEAT and DURATION + any others
                    TBool foundTrigger=EFalse;
                    TInt nestedPropCount = nestedProperties.Count();
                    for (TInt j = 0; j < nestedPropCount; j++)
                        {
                        if (nestedProperties[j]->Type() == KICalTrigger)
                            {
                            const RPointerArray<CESMRICalValue>& values = nestedProperties[j]->Values();
                            // There should only be one value, don't try to add the alarm if there are more
                            if (values.Count() == 1)
                                {
                                const CESMRICalPropertyParam* valueType = nestedProperties[j]->FindParam(KICalValue);
                                TTimeIntervalMinutes duration;
                                if (valueType) // Only DATE-TIME is legal here - assume it's that one.
                                    {
                                    if (!aStartTime)
                                        {
                                        ReportErrorL( MESMRAgnImportObserver::EImpErrorMissingData,
                                                      aUid,
                                                      iStringProvider.StringL(EICalErrValarmNoStart),
                                                      EFalse );
                                        // On continue, we skip the alarm.
                                        }
                                    else
                                        {
                                        TCalTime absTime;
                                        GetCalTimeL(*nestedProperties[j], absTime, EDateTime);
                                        aStartTime->TimeUtcL().MinutesFrom(absTime.TimeUtcL(), duration);
                                        }
                                    }
                                else
                                    {
                                    duration = values[0]->DurationL().Int() / KSecondsInOneMinute;
                                    }

                                // Only negative durations are allowed (the alarm must be *before* the event).
                                if (duration.Int() > 0)
                                    {
                                    ReportErrorL( MESMRAgnImportObserver::EImpErrorNotSupported,
                                                  aUid,
                                                  iStringProvider.StringL(EICalErrAlarmAfterEvent) );
                                    // On continue, the duration will be made "negative".
                                    }

                                // Agenda expects a positive duration - convert the sign.
                                if (duration.Int() < 0)
                                    {
                                    duration = -(duration.Int());
                                    }

                                alarm->SetTimeOffset(duration);
                                foundTrigger=ETrue;
                                }
                            }
                        }
                    if (foundTrigger)
                        {
                        aEntry.SetAlarmL(alarm);// Doesn't take ownership
                        }
                    else
                        {
                        ReportErrorL( MESMRAgnImportObserver::EImpErrorMissingData,
                                      aUid,
                                      iStringProvider.StringL(EICalErrValarmNoTrigger),
                                      EFalse );
                        // On continue, the alarm will be skipped.
                        }
                    CleanupStack::PopAndDestroy(alarm);
                    }
                }
            }
        else
            {
            ReportErrorL( MESMRAgnImportObserver::EImpErrorInvalidData,
                          aUid,
                          iStringProvider.StringL(EICalErrValarmNotAllowed),
                          EFalse );
            // On continue, the alarm will be skipped.
            }
        }

    }


/**
 * Performs most of the work of importing a component from an iCalendar, provided
 * that the component is one which can be handled as a CCalEntry. Timezone and
 * Alarm components are not imported in this manner.
 * @param aCal The calendar object containing the component.
 * @param aComponent The component to be imported.
 * @param aEntries The array onto which to append the new entry.
 * @param aUid The UID of the component (takes ownership).
 * @param aType The type of entry being imoported
 * @return MESMRAgnImportObserver::TImpResponse indicating application response to errors, if any.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportEntryL( const CESMRICal& aCal,
                                        const CESMRICalComponent& aComponent,
                                        RPointerArray<CCalEntry>& aEntries,
                                        HBufC8* aUid,
                                        CCalEntry::TType aType )
    {
    FUNC_LOG;

    // Create a basic entry to add properties to.
    CCalEntry* entry = CreateEntryLC(aCal, aComponent, aUid, aType); // takes ownership of aUid

    iNeedsTzRules = (EFalse);
    iCurrentTzRules = NULL;

    // set the entry properties:
    CESMRICalProperty* rruleproperty = NULL;
    CESMRICalProperty* startproperty = NULL;
    CESMRICalProperty* endproperty = NULL;
    CESMRICalProperty* durationproperty = NULL;
    RPointerArray<CESMRICalProperty> exDateProperties;
    CleanupClosePushL(exDateProperties);
    RPointerArray<CESMRICalProperty> rDateProperties;
    CleanupClosePushL(rDateProperties);

    const RPointerArray<CESMRICalProperty>& properties = aComponent.Properties();
    TInt propCount = properties.Count();
    for (TInt pnum = 0; pnum < propCount; ++pnum)
        {
        CESMRICalProperty* property = properties[pnum];

        // DTSTART, DTEND, DURATION, EXDATE, RDATE and RRULE need special handling,
        //as they affect one another.
        if (property->Type().CompareF(KICalRRule) == 0)
            {
            rruleproperty = property;
            }
        else if (property->Type().CompareF(KICalDtstart) == 0)
            {
            startproperty = property;
            }
        else if (property->Type().CompareF(KICalDtend) == 0)
            {
            endproperty = property;
            }
        else if (property->Type().CompareF(KICalDuration) == 0)
            {
            durationproperty = property;
            }
        else if (property->Type().CompareF(KICalExdate) == 0)
            {
            // It is valid to have more than one EXDATE property. We store them all in an array.
            User::LeaveIfError(exDateProperties.Append(property));
            }
        else if (property->Type().CompareF(KICalRdate) == 0)
            {
            // It is valid to have more than one RDATE property. We store them all in an array.
            User::LeaveIfError(rDateProperties.Append(property));
            }
        else
            {
            ImportPropertyL(*entry, *property);
            }
        }

    // We cannot handle a DTSTART, DTEND or DURATION field on its own, so we have saved them up
    // and will sort them out here.
    // In theory, some methods allow an event to specify an end time but no start time.
    // We have no way of dealing with this so we discard it.  When we get a start time
    // with no end time, we assume a duration of 0 seconds.
    TCalTime start;
    TCalTime end;
    if (startproperty)
        {
        GetCalTimeL(*startproperty, start, EDateTime);
        end = start;
        if (endproperty)
            {
            GetCalTimeL(*endproperty, end, EDateTime);
            }
        else if (durationproperty && startproperty)
            {
            if (durationproperty->Values().Count() < 1)
                {
                ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, *aUid, KICalDuration);
                // On continue use the start time as the end time (a duration of 0 seconds)
                }
            else
                {
                end.SetTimeUtcL(start.TimeUtcL() + durationproperty->Values()[0]->DurationL());
                }
            }
        entry->SetStartAndEndTimeL(start, end);
        }

    // We must have a start time before we can interpret some alarms:
    TRAPD(alarmerr, ImportAlarmL(aComponent, *entry, *aUid, startproperty?&start:0));
    // If we fail and elect to skip whilst importing the alarm, continue
    // with the current component.
    if (alarmerr && iResponse == MESMRAgnImportObserver::EImpResponseSkip)
        {
        iResponse = MESMRAgnImportObserver::EImpResponseContinue;
        }
    else
        {
        User::LeaveIfError(alarmerr);
        }

    // We cannot put an rrule property into the agenda model format until we
    // have properties such as dtstart already stored, so we do this last:
    if (rruleproperty)
        {
        if (startproperty)
            {
            if (ImportRRuleL(*rruleproperty, *entry, start))
                {
                // APINOTE: In the Cal Interim API EXDATE is only valid with an RRULE.
                // This is not necessarily so for iTIP updates.
                for (TInt i = 0; i < exDateProperties.Count(); i++)
                    {
                    ImportPropertyL(*entry, *(exDateProperties[i]));
                    }
                }
            else if (exDateProperties.Count() > 0)
                {
                // Cannot process an EXDATE without an RRULE.
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, *aUid, KICalExdate);
                }
            }
        else
            {
            // Cannot process an RRULE without a DTSTART
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, *aUid, KICalDtstart);
            }
        }


    // RDATE does not neccessary need an RRULE.
    for (TInt i = 0; i < rDateProperties.Count(); i++)
        {
        ImportPropertyL(*entry, *(rDateProperties[i]));
        }

    CleanupStack::PopAndDestroy(&rDateProperties);
    CleanupStack::PopAndDestroy(&exDateProperties);

    // APINOTE: When setting the RRULE, the API overwrites the DTEND field.
    // This block sets it back to what it should be.
    if (startproperty)
        {
        // We don't need to check for an end time as it gets initialised when the
        // start time gets set.
        entry->SetStartAndEndTimeL(start, end);
        }

    // If we have set up a timezone, add this to the entry.
    // Only entries with RDATES, EXDATES, RRULES or EXRULES will have this.
    if (iNeedsTzRules)
        {
        if (iCurrentTzRules)
            {
            entry->SetTzRulesL(*iCurrentTzRules);
            }
        }

    //Set the last modified date to current time
    entry->SetLastModifiedDateL();
    // Store the entry:
    User::LeaveIfError(aEntries.Append(entry)); // Takes ownership if successful
    CleanupStack::Pop(entry);

    }

/**
 * Creates a new entry and pushes it onto the cleanup stack.
 * @param aCal The calendar object containing the component.
 * @param aComponent The component to be imported.
 * @param aUid The UID of the component (takes ownership).
 * @param aType The type of entry being imoported
 * @return A new CCalEntry, or zero if a recoverable error occurs and the observer
 * elects not to continue from it.
 * @internalTechnology
 */
CCalEntry * CESMRAgnVersit2Importer::CreateEntryLC( const CESMRICal& aCal,
                                                const CESMRICalComponent& aComponent,
                                                HBufC8* aUid,
                                                CCalEntry::TType aType )
    {

    CleanupStack::PushL(aUid);

    const CESMRICalProperty* propSeq = aComponent.FindProperty(KICalSequence);

    // Find the sequence number, but accept a default of 0 if there isn't one:
    TUint entrySeq = 0;
    if (propSeq && propSeq->Values().Count() > 0)
        {
        entrySeq = propSeq->Values()[0]->IntegerL();
        }

    // Find the method, but accept a default of EMethodNone if there isn't one:
    CCalEntry::TMethod method = CCalEntry::EMethodNone;
    const CESMRICalProperty* prop = aCal.FindProperty(KICalMethod);
    if (prop && prop->Values().Count() > 0)
        {
        TPtrC val;
        val.Set(prop->Values()[0]->TextL());
        if (val.CompareF(KICalPublish) == 0)
            {
            method = CCalEntry::EMethodPublish;
            }
        else if (val.CompareF(KICalRequest) == 0)
            {
            method = CCalEntry::EMethodRequest;
            }
        else if (val.CompareF(KICalReply) == 0)
            {
            method = CCalEntry::EMethodReply;
            }
        else if (val.CompareF(KICalAdd) == 0)
            {
            method = CCalEntry::EMethodAdd;
            }
        else if (val.CompareF(KICalCancel) == 0)
            {
            method = CCalEntry::EMethodCancel;
            }
        else if (val.CompareF(KICalRefresh) == 0)
            {
            method = CCalEntry::EMethodRefresh;
            }
        else if (val.CompareF(KICalCounter) == 0)
            {
            method = CCalEntry::EMethodCounter;
            }
        else if (val.CompareF(KICalDeclineCounter) == 0)
            {
            method = CCalEntry::EMethodDeclineCounter;
            }
        else
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorInvalidData, *aUid, val);
            // On continue, use EMethodNone.
            }
        }

    CCalEntry* entry = NULL;
    // Find the recurrence information, if there is any:
    const CESMRICalProperty* propRecurrenceId = aComponent.FindProperty(KICalRecurrenceId);
    if (propRecurrenceId)
        {
        TCalTime entryRecId;
        GetCalTimeL(*propRecurrenceId, entryRecId, EDateTime);

        const CESMRICalPropertyParam* range = propRecurrenceId->FindParam(KICalRange);
        CalCommon::TRecurrenceRange entryRange = CalCommon::EThisOnly;
        if (range && range->Values().Count() > 0)
            {
            if (range->Values()[0]->TextL().CompareF(KICalThisAndPrior) == 0)
                {
                entryRange = CalCommon::EThisAndPrior;
                }
            else if (range->Values()[0]->TextL().CompareF(KICalThisAndFuture) == 0)
                {
                entryRange = CalCommon::EThisAndFuture;
                }
            else
                {
                ReportErrorL(MESMRAgnImportObserver::EImpErrorInvalidData, *aUid, range->Values()[0]->TextL());
                // On continue, assume this instance only.
                }
            }
        // Create an entry with recurrence data:
        // Passing ownership of aUid
        CleanupStack::Pop(aUid);
        entry = CCalEntry::NewL(aType, aUid, method, entrySeq, entryRecId, entryRange);
        CleanupStack::PushL(entry);
        }
    else
        {
        // Create an entry without recurrence data:
        // Passing ownership of aUid
        CleanupStack::Pop(aUid);
        entry = CCalEntry::NewL(aType, aUid, method, entrySeq);
        CleanupStack::PushL(entry);
        }

    return entry;
    }

/**
 * Imports an individual property and adds it to an entry.
 * @param aEntry The entry to which the property is to be added.
 * @param aProperty The property to add.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportPropertyL(CCalEntry& aEntry, const CESMRICalProperty& aProperty)
    {
    FUNC_LOG;

    if (aProperty.Type().CompareF(KICalAttendee) == 0)
        {
        // Find the paramaters we need:
        CESMRICalValue* address = aProperty.Values()[0];
        const CESMRICalPropertyParam* sentBy = aProperty.FindParam(KICalSentBy);
        const CESMRICalPropertyParam* role = aProperty.FindParam(KICalRole);
        const CESMRICalPropertyParam* status = aProperty.FindParam(KICalPartStat);
        const CESMRICalPropertyParam* rsvp = aProperty.FindParam(KICalRsvp);
        const CESMRICalPropertyParam* cn = aProperty.FindParam(KICalCn);
        // The role:
        CCalAttendee::TCalRole roleVal = CCalAttendee::EReqParticipant;
        if (role && role->Values().Count() > 0)
            {
            CESMRICalValue* theVal = role->Values()[0];
            if (theVal->TextL().CompareF(KICalChair) == 0)
                {
                roleVal = CCalAttendee::EChair;
                }
            else if (theVal->TextL().CompareF(KICalOptParticipant) == 0)
                {
                roleVal = CCalAttendee::EOptParticipant;
                }
            else if (theVal->TextL().CompareF(KICalNonParticipant) == 0)
                {
                roleVal = CCalAttendee::ENonParticipant;
                }

            }
        // The status:
        CCalAttendee::TCalStatus statusVal = CCalAttendee::ENeedsAction;
        if (status && status->Values().Count() > 0)
            {
            CESMRICalValue* theVal = status->Values()[0];
            if (theVal->TextL().CompareF(KICalAccepted) == 0)
                {
                statusVal = CCalAttendee::EAccepted;
                }
            else if (theVal->TextL().CompareF(KICalDeclined) == 0)
                {
                statusVal = CCalAttendee::EDeclined;
                }
            else if (theVal->TextL().CompareF(KICalTentative) == 0)
                {
                statusVal = CCalAttendee::ETentative;
                }
            else if (theVal->TextL().CompareF(KICalDelegated) == 0)
                {
                statusVal = CCalAttendee::EDelegated;
                }
            else if (theVal->TextL().CompareF(KICalCompleted) == 0)
                {
                statusVal = CCalAttendee::ECompleted;
                }
            else if (theVal->TextL().CompareF(KICalInProcess) == 0)
                {
                statusVal = CCalAttendee::EInProcess;
                }
            }
        // The RSVP:
        TBool rsvpVal = EFalse;
        if (rsvp && rsvp->Values().Count() > 0)
            {
            rsvpVal = rsvp->Values()[0]->BooleanL();
            }

        // The complete attendee:
        CCalAttendee* attendee;
        if (sentBy && sentBy->Values().Count() > 0)
            {
            attendee = CCalAttendee::NewL(
                            address->TextL(),
                            sentBy->Values()[0]->TextL());
            }
        else
            {
            HBufC* buf =
                    ESMRHelper::AddressWithoutMailtoPrefix(
                            address->TextL() ).AllocLC();

            attendee = CCalAttendee::NewL( buf->Des() );
            CleanupStack::PopAndDestroy( buf );
            buf = NULL;
            }
        CleanupStack::PushL(attendee);
        if (cn && cn->Values().Count() > 0)
            {
            attendee->SetCommonNameL(cn->Values()[0]->TextL());
            }
        attendee->SetRoleL(roleVal);
        attendee->SetStatusL(statusVal);
        attendee->SetResponseRequested(rsvpVal);
        // Add the attendee:
        CleanupStack::Pop(attendee);
        // Passing ownership of attendee
        aEntry.AddAttendeeL(attendee);
        }
    else if (aProperty.Type().CompareF(KICalCategories) == 0)
        {
        ImportCategoriesL(aProperty, aEntry);
        }
    else if (aProperty.Type().CompareF(KICalClass) == 0)
        {
        ImportClassL(aProperty, aEntry);
        }
    else if (aProperty.Type().CompareF(KICalDescription) == 0)
        {
        if (aProperty.Values().Count() < 1)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalDescription);
            // On continue ignore the description.
            }
        else
            {
            aEntry.SetDescriptionL(aProperty.Values()[0]->TextL());
            }
        }
    else if (aProperty.Type().CompareF(KICalLocation) == 0)
        {
        // CCalEntry only supports one location, so bundle them all
        // together into one descriptor.
        TInt numLocations = aProperty.Values().Count();

        if (numLocations > 0)
            {
            HBufC* loc (aProperty.Values()[0]->TextL().AllocLC());

            for (TInt i = 1; i < numLocations; i++)
                {
                HBufC* oldLoc = loc;
                loc = loc->ReAllocL(loc->Length() + aProperty.Values()[i]->TextL().Length() + 1);
                CleanupStack::Pop(oldLoc); // location of loc has changed, cleanupstack needs updating
                CleanupStack::PushL(loc);
                loc->Des().Append(KICalComma);
                loc->Des().Append(aProperty.Values()[i]->TextL());
                }

            aEntry.SetLocationL(*loc);
            CleanupStack::PopAndDestroy(loc);
            }
        }
    else if (aProperty.Type().CompareF(KICalOrganizer) == 0)
        {
        if (aProperty.Values().Count() < 1)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalOrganizer);
            // On continue ignore the organizer.
            }
        else
            {
            CESMRICalValue* address = aProperty.Values()[0];
            const CESMRICalPropertyParam* sentBy = aProperty.FindParam(KICalSentBy);
            const CESMRICalPropertyParam* cn = aProperty.FindParam(KICalCn);
            CCalUser* user = NULL;
            if (sentBy && sentBy->Values().Count() > 0)
                {
                user = CCalAttendee::NewL(address->TextL(), sentBy->Values()[0]->TextL());
                }
            else
                {
                HBufC* buf = ESMRHelper::AddressWithoutMailtoPrefix(
                                address->TextL() ).AllocLC();
                
                user = CCalAttendee::NewL( buf->Des() );
                CleanupStack::PopAndDestroy( buf );
                buf = NULL;
                }
            CleanupStack::PushL(user);
            if (cn && cn->Values().Count() > 0)
                {
                user->SetCommonNameL(cn->Values()[0]->TextL());
                }
            CleanupStack::Pop(user);
            // Passing ownership of user
            aEntry.SetOrganizerL(user);
            }
        }
    else if (aProperty.Type().CompareF(KICalRdate) == 0)
        {
        iNeedsTzRules = ETrue;
        RArray<TCalTime> dates;
        CleanupClosePushL(dates);
        aEntry.GetRDatesL(dates);
        TInt valCount = aProperty.Values().Count();
        for (TInt dnum = 0; dnum < valCount; ++dnum)
            {
            TCalTime time;
            GetCalTimeL(aProperty, time, EDateTime, dnum);
            User::LeaveIfError(dates.Append(time));
            }
        aEntry.SetRDatesL(dates);
        CleanupStack::PopAndDestroy(&dates);
        }
    else if (aProperty.Type().CompareF(KICalExdate) == 0)
        {
        iNeedsTzRules = ETrue;
        RArray<TCalTime> dates;
        CleanupClosePushL(dates);
        aEntry.GetExceptionDatesL(dates);
        TInt valCount = aProperty.Values().Count();
        for (TInt dnum = 0; dnum < valCount; ++dnum)
            {
            TCalTime time;
            GetCalTimeL(aProperty, time, EDateTime, dnum);
            User::LeaveIfError(dates.Append(time));
            }
        aEntry.SetExceptionDatesL(dates);
        CleanupStack::PopAndDestroy(&dates);
        }
    else if (aProperty.Type().CompareF(KICalStatus) == 0)
        {
        if (aProperty.Values().Count() < 1)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalDescription);
            // On continue ignore the status.
            }
        else
            {
            TPtrC statstr(aProperty.Values()[0]->TextL());
            if (statstr.CompareF(KICalTentative) == 0)
                {
                aEntry.SetStatusL(CCalEntry::ETentative);
                }
            else if (statstr.CompareF(KICalConfirmed) == 0)
                {
                aEntry.SetStatusL(CCalEntry::EConfirmed);
                }
            else if (statstr.CompareF(KICalCancelled) == 0)
                {
                aEntry.SetStatusL(CCalEntry::ECancelled);
                }
            else if (statstr.CompareF(KICalNeedsAction) == 0)
                {
                aEntry.SetStatusL(CCalEntry::ETodoNeedsAction);
                }
            else if (statstr.CompareF(KICalCompleted) == 0)
                {
                aEntry.SetStatusL(CCalEntry::ETodoCompleted);
                }
            else if (statstr.CompareF(KICalInProcess) == 0)
                {
                aEntry.SetStatusL(CCalEntry::ETodoInProcess);
                }
            else
                {
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, aEntry.UidL(), statstr);
                // On continue ignore the status.
                }
            }
        }
    else if (aProperty.Type().CompareF(KICalSummary) == 0)
        {
        if (aProperty.Values().Count() < 1)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalDescription);
            // On continue ignore the summary.
            }
        else
            {
            aEntry.SetSummaryL(aProperty.Values()[0]->TextL());
            }
        }
    else if (aProperty.Type().CompareF(KICalPriority) == 0)
        {
        if (aProperty.Values().Count() < 1)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalDescription);
            // On continue ignore the priority.
            }
        else
            {
            TInt priority = aProperty.Values()[0]->IntegerL();

            //Pirorites are saved always in vcal format
            //so they are mapped the following way
            //iCal 1 - 4 = vCal 1
            //iCal 5     = vCal 2
            //iCal 6 - 9 = vCal 3

            // MRUI always uses iCal format
            //Pirorites are saved always in vcal format
            //so they are mapped the following way
            //vCal 1 = iCal 1
            //vCal 2 = iCal 5
            //vCal 3 = iCal 9

            if ( priority != EFSCalenMRPriorityLow &&
                 priority != EFSCalenMRPriorityNormal &&
                 priority != EFSCalenMRPriorityHigh )
                {
                priority = EFSCalenMRPriorityNormal;
                }
            /*if( priority < 5 )
                {
                priority = 1;
                }

            else if( priority == 5 )
                {
                priority = 2;
                }

            else
                {
                priority = 3;
                }*/

            aEntry.SetPriorityL( priority );
            }
        }

    else if (aProperty.Type().CompareF(KICalDtstamp) == 0)
        {
        TCalTime timeStamp;
        GetCalTimeL(aProperty, timeStamp, EDateTime);
        aEntry.SetDTStampL(timeStamp);
        }
    else if ((aProperty.Type().CompareF(KICalUid) == 0) ||
             (aProperty.Type().CompareF(KICalSequence) == 0) ||
             (aProperty.Type().CompareF(KICalRecurrenceId) == 0) ||
             (aProperty.Type().CompareF(KICalLastmodified) == 0))
        {
        //Already been handled when constructing the entry :- do nothing
        }
    else if (aProperty.Type().Find(KICalXProperty) == 0)
        {
        // Quietly swallow X- properties, instead of reporting "NotSupported"
        }
    else
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, aEntry.UidL(), aProperty.Type());
        }

    }

/**
 * Performs most of the work of importing a timezone component. Timezones are not
 * stored as separate entities within Agenda, but as CTzRules objects within
 * individual entries, and so this maintains an array of all timezone components
 * for any given calendar, to be accessed later as required.
 * @param aTimezone The timezone to convert into CTzRules format.
 * @return MESMRAgnImportObserver::TImpResponse indicating application response to errors, if any.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportTimezoneL(const CESMRICalComponent& aTimezone)
    {
    FUNC_LOG;

    CTzRules* rules = NULL;
    // Get the TZID:
    TPtrC tzid;
    const CESMRICalProperty* propUid = aTimezone.FindProperty(KICalTzid());
    if (propUid && propUid->Values().Count() > 0)
        {
        tzid.Set(propUid->Values()[0]->TextL());
        }
    else
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorInvalidData, KNullDesC8, KICalUid, EFalse);
        // A Timezone with no ID is completely pointless, so on continue skip.
        }

    // Create somewhere to store the data we are parsing:
    rules = CTzRules::NewL();
    CleanupStack::PushL(rules);

    // We don't care about any of the timezones properties except the tzid - we go straight
    // onto the standard and daylight values.
    const RPointerArray<CESMRICalComponent>& tzComponents = aTimezone.Components();
    if (tzComponents.Count() < 1)
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, KNullDesC8, KICalTimeZone, EFalse);
        // A Timezone without atleast 1 value is meaningless so on continue skip.
        }
    else
        {
        // Agenda expects a set of month+day definitions, which repeat every year between start and end years.
        // iCalendar provides a set of year+month+day definitions, which cannot always be represented.
        // We will try, and hope for the best, using the first rules start and end to define the overall duration,
        // effectively overwriting all the other rules.
        rules->SetInitialStdTimeOffset(0);

        TInt tzCount = tzComponents.Count();
        for (TInt tznum = 0; tznum < tzCount; ++tznum)
            {
            // Find the properties for this component:
            CESMRICalComponent* tzinterval = tzComponents[tznum];
            ImportTimezoneIntervalL(*tzinterval, *rules);
            }
        }

    CTzNamedRules* namedRules = new (ELeave) CTzNamedRules;
    CleanupStack::Pop(rules);
    namedRules->iRules = rules;
    CleanupStack::PushL(namedRules);
    namedRules->iName = tzid.AllocL();
    User::LeaveIfError(iTzRules.Append(namedRules));
    CleanupStack::Pop(namedRules);

    }

/**
 * Performs most of the work of translating a DAYLIGHT or STANDARD section of a
 * VTIMEZONE into the equivalent TTzRule for Agenda.
 * @param aInterval The DAYLIGHT or STANDARD component.
 * @param aRules The CTzRules into which the TTzRule will be placed.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportTimezoneIntervalL( const CESMRICalComponent& aInterval,
                                                   CTzRules& aRules )
    {
    FUNC_LOG;

    // Find the properties:
    const CESMRICalProperty* from = aInterval.FindProperty(KICalTzoffsetfrom);
    const CESMRICalProperty* to = aInterval.FindProperty(KICalTzoffsetto);
    const CESMRICalProperty* start = aInterval.FindProperty(KICalDtstart);
    const CESMRICalProperty* rdate = aInterval.FindProperty(KICalRdate);
    const CESMRICalProperty* rrule = aInterval.FindProperty(KICalRRule);

    // Set the time offsets it implies:
    TTzRule tzrule;
    tzrule.iOldLocalTimeOffset = from->Values()[0]->UtcOffsetL().Int() / KSecondsInOneMinute;
    tzrule.iNewLocalTimeOffset = to->Values()[0]->UtcOffsetL().Int() / KSecondsInOneMinute;
    tzrule.iTimeReference = ETzUtcTimeReference;

    RPointerArray<CESMRICalRuleSegment> recRules;
    CleanupPointerArrayPushL(recRules);

    // If this has an rdate with more than one value we can only represent it by
    // creating a seperate tzrule for each date.  Not only that, but because the rdate
    // specifies years, we need separate CTzRules for each year, which we can't actually
    // do in Agenda. This is unsupported.
    if (rdate)
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalRdate);
        // On continue we ignore the rdate.
        }

    // Start date
    // Agenda has a start year and end year for the entire timezone.
    // iCalendar has a start date for each interval and an end date for each rrule within the interval.
    // This means we are trying to map N starts and M ends to 1 start and 1 end.
    // We will just use whatever we find - it's not going to work and it can't be made to work.
    TDateTime dateTime;
    if (start)
        {
        TTime startTime;
        CESMRICalValue::TTimeZoneType type;
        start->Values()[0]->GetDateTimeL(startTime, type);
        dateTime = startTime.DateTime();
        aRules.SetStartYear(dateTime.Year());
        tzrule.iFrom = TTimeWithReference( dateTime );
        // Put the max TTimeWithReference value to iTo as default.
        // Normally it is not set in the VCALENDAR object,
        // but it is required for CTzRules API.
        tzrule.iTo = TTimeWithReference::Max();
        // We cannot set the day of the month on which this timezone rule starts being applied
        // because Agenda does not support it. We just have to make do without it; the field
        // gets set to the first day of the month by default.
        }
    else
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, KNullDesC8, KICalDtstart, EFalse);
        // We cannot continue without a start, so skip.
        }

    // If this has an rrule we may be able to represent it.
    if (rrule && rrule->Values().Count() > 0)
        {
        // One thing we cannot do is handle multiple RRules.
        if (rrule->Values().Count() > 1)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8,
                        iStringProvider.StringL(EICalErrRruleNumRules));
            // On continue ignore all but the first RRule.
            }
        CESMRICalRuleSegment::TFreq freq;
        rrule->Values()[0]->GetRecurrenceRuleL(recRules);

        // Find the frequency first - if there is no frequency it is an error, and if the
        // frequency is anything other than yearly we can't handle it anyway.
        TInt pos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegFreq);
        if (pos != KErrNotFound)
            {
            ASSERT(recRules.Count() >= pos + 1);
            freq = recRules[pos]->FreqL();
            if (freq != CESMRICalRuleSegment::EFreqYearly)
                {
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8,
                        iStringProvider.StringL(EICalErrRruleNotYearly));
                // On continue assume yearly.
                freq = CESMRICalRuleSegment::EFreqYearly;
                }
            }
        else
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, KNullDesC8,
                        rrule->Values()[0]->TextL());
            // On continue assume yearly.
            freq = CESMRICalRuleSegment::EFreqYearly;
            }

        // Find the interval.  We can only handle an interval of 1.
        TInt thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegInterval);
        if (thePos != KErrNotFound)
            {
            ASSERT(recRules.Count() >= thePos + 1);
            ASSERT(recRules[thePos]->Values().Count() >= 0);
            if (recRules[thePos]->Values()[0]->IntegerL() != 1)
                {
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalInterval);
                // On continue assume the interval is 1.
                }
            }

        // Find the count, and interpret it as an end year.
        // See the comment above about start date for the problems with this.
        thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegCount);
        if (thePos != KErrNotFound)
            {
            ASSERT(recRules.Count() >= thePos + 1);
            ASSERT(recRules[thePos]->Values().Count() >= 0);
            TInt count = recRules[thePos]->Values()[0]->IntegerL();
            aRules.SetEndYear(aRules.StartYear() + count);
            }

        // Find the until, and interpret it as an end year.
        // See the comment above about start date for the problems with this.
        thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegUntil);
        if (thePos != KErrNotFound)
            {
            ASSERT(recRules.Count() >= thePos + 1);
            ASSERT(recRules[thePos]->Values().Count() >= 0);
            TTime endTime;
            recRules[thePos]->Values()[0]->GetDateL(endTime);
            aRules.SetEndYear(endTime.DateTime().Year());
            }

        // WKST (which defines the day on which a week starts) is not
        // supported by the agenda timezones.
        thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegUntil);
        if (thePos != KErrNotFound)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalWkSt);
            // On continue ignore the WKST.
            }

        // BySecond, ByMinute, ByHour, ByYearDay, ByWeekNo are not
        // supported by the agenda timezones.
        thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegBySecond);
        if (thePos == KErrNotFound)
            {
            thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByMinute);
            }
        if (thePos == KErrNotFound)
            {
            thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByHour);
            }
        if (thePos == KErrNotFound)
            {
            thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByYearDay);
            }
        if (thePos == KErrNotFound)
            {
            thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByWeekNo);
            }
        if (thePos != KErrNotFound)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalRRule);
            // On continue ignore the unsupported field.
            }

        // ByMonth
        TBool haveMonth = EFalse;
        thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByMonth);
        if (thePos != KErrNotFound)
            {
            if (recRules[thePos]->Values().Count() > 0)
                {
                TMonth month = recRules[thePos]->Values()[0]->MonthL();
                haveMonth = ETrue;
                tzrule.iMonth = month;
                if (recRules[thePos]->Values().Count() > 1)
                    {
                    ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalByMonth);
                    // On continue ignore the additional months.
                    }
                }
            }

        if (!haveMonth)
            {
            tzrule.iMonth = dateTime.Month();
            }

        // ByDay
        TBool haveDay = EFalse;
        TInt daypos;
        thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByDay);
        if (thePos != KErrNotFound)
            {
            if (recRules[thePos]->Values().Count() > 0)
                {
                TDay day;
                recRules[thePos]->Values()[0]->GetDayL(day, daypos);
                haveDay = ETrue;
                tzrule.iDayOfWeek = day;
                if (recRules[thePos]->Values().Count() > 1)
                    {
                    ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalByDay);
                    // On continue ignore extra days.
                    }
                }
            }

        if (haveDay)
            {
            if (daypos == 0)
                {
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalByDay, EFalse);
                // We cannot handle this - on continue skip.
                }
            else if (daypos < 0)
                {
                // We cant handle -2,-3 etc, so we assume -1
                tzrule.iDayRule = ETzDayInLastWeekOfMonth;
                tzrule.iDayOfMonth = 0;
                }
            else
                {
                if (daypos > KMaxWeekDayNum)
                    {
                    ReportErrorL(MESMRAgnImportObserver::EImpErrorInvalidData, KNullDesC8, KICalByDay);
                    // On continue, assume the KMaxWeekDayNum.
                    daypos = KMaxWeekDayNum;
                    }
                tzrule.iDayRule = ETzDayAfterDate;
                tzrule.iDayOfMonth = (daypos - 1) * KDaysPerWeek;
                }
            }
        else
            {
            tzrule.iDayRule = ETzFixedDate;
            tzrule.iDayOfMonth = dateTime.Day();
            }

        // ByMonthDay and BySetPos can in theory be supported under
        // some circumstances, but right now they aren't.
        thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByMonthDay);
        if (thePos == KErrNotFound)
            {
            thePos = FindRuleSegment(recRules, CESMRICalRuleSegment::ESegByPos);
            }
        if (thePos != KErrNotFound)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalRRule);
            // On continue ignore both fields.
            }

        tzrule.iTimeOfChange = dateTime.Hour() * KMinutesInOneHour + dateTime.Minute();
        aRules.AddRuleL(tzrule);
        }
    else // no rrule
        {
        tzrule.iDayRule = ETzFixedDate;
        tzrule.iMonth = dateTime.Month();
        tzrule.iDayOfMonth = dateTime.Day();
        tzrule.iTimeOfChange = dateTime.Hour() * KMinutesInOneHour + dateTime.Minute();
        aRules.AddRuleL(tzrule);
        }
    CleanupStack::PopAndDestroy(&recRules);

    }

/**
*     IMPORT PROPERTY FUNCTIONS
*     =========================
*/

/**
 * Helper function for translating categories from the pure text format of
 * iCalendar to the enumerated type of Agenda.
 * @param aProperty A category property.
 * @param aEntry An entry into which to put the categories.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportCategoriesL( const CESMRICalProperty& aProperty,
                                             CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    TInt valCount = aProperty.Values().Count();
    for (TInt pnum = 0; pnum < valCount; ++pnum)
        {
        TPtrC name(aProperty.Values()[pnum]->TextL());
        CCalCategory* cat = NULL;
        if (name.CompareF(iStringProvider.StringL(EICalAppointment)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalAppointment);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalBusiness)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalBusiness);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalEducation)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalEducation);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalHoliday)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalHoliday);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalMeeting)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalMeeting);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalMisc)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalMiscellaneous);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalPersonal)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalPersonal);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalPhoneCall)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalPhoneCall);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalSick)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalSickDay);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalSpecial)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalSpecialOccasion);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalTravel)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalTravel);
            }
        else if (name.CompareF(iStringProvider.StringL(EICalVacation)) == 0)
            {
            cat = CCalCategory::NewL(CCalCategory::ECalVacation);
            }
        else
            {
            cat = CCalCategory::NewL(name);
            }
        aEntry.AddCategoryL(cat);
        }
    }

/**
 * A function to import a CLASS property into a format suitable for Agenda.
 * @param aProperty The CLASS property to translate.
 * @param aEntry The entry to add the rules to.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ImportClassL(const CESMRICalProperty& aProperty, CCalEntry& aEntry)
    {
    FUNC_LOG;

    if (aProperty.Values().Count() > 0)
        {
        // Ignore any values after the first one
        TPtrC name(aProperty.Values()[0]->TextL());
        if (name.CompareF(KICalPublic) == 0)
            {
            aEntry.SetReplicationStatusL(CCalEntry::EOpen);
            }
        else if (name.CompareF(KICalPrivate) == 0)
            {
            aEntry.SetReplicationStatusL(CCalEntry::EPrivate);
            }
        else if (name.CompareF(KICalConfidential) == 0)
            {
            aEntry.SetReplicationStatusL(CCalEntry::ERestricted);
            }
        else
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, aEntry.UidL(), name);
            // On continue ignore the class.
            }
        }
    else
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalRRule);
        // On continue ignore the class.
        }
    }

/**
 * A function to translate an RRULE property into a TCalRRule for Agenda.
 * @param aProperty The RRULE property to translate.
 * @param aEntry The entry to add the rules to.
 * @param aStartTime The DTSTART of the parent component.
 * @return ETrue if the rule is imported, else EFalse.
 * @internalTechnology
 */
TBool CESMRAgnVersit2Importer::ImportRRuleL( const CESMRICalProperty& aProperty,
                                         CCalEntry& aEntry,
                                         const TCalTime& aStartTime )
    {
    FUNC_LOG;

    //Create a new recurrence rule importer
    CESMRAgnRRuleImporter *importer = CESMRAgnRRuleImporter::NewLC( *this );

    //import the recurrence rule
    TBool importRRule = importer->ImportL( aProperty, aEntry, aStartTime );

    if( importRRule )
        {
        iNeedsTzRules = ETrue;
        }

    CleanupStack::PopAndDestroy( importer );


    //return true if the rule is imported
    return importRRule;
    }



//
//     HELPER FUNCTIONS
//     ================
//


/**
 * Takes a property, a TCalTime, and an index into the set of values of the
 * property, and populates the TCalTime from the value, performing any required
 * timezone conversions first.
 * @param aProperty A property which has a date or date-time type.
 * @param aTime The time to put the resulting UTC time into.
 * @param aDefaultType Enumeration of the type of value being passed.
 * @param aValue The index of the property value to use, for properties which have
 * more than one.
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::GetCalTimeL( const CESMRICalProperty& aProperty,
                                       TCalTime& aTime,
                                       TValueType aValueType,
                                       TInt aValue )
    {
    FUNC_LOG;

    // Find the property value, value parameter and tzid parameter:
    if (aProperty.Values().Count() < aValue)
        {
        ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, KNullDesC8, aProperty.Type(), EFalse);
        // We cannot continue - we interpret continue as skip.
        }

    const CESMRICalPropertyParam* valuetype = aProperty.FindParam(KICalValue);
    if (valuetype)
        {
        if (valuetype->Values().Count() < 1)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData, KNullDesC8, KICalValue);
            // On continue, use the default type passed in the function arguments.
            }
        else
            {
            TPtrC vt(valuetype->Values()[0]->TextL());
            if (vt.CompareF(KICalDate) == 0)
                {
                aValueType = EDate;
                }
            else if (vt.CompareF(KICalDateTime) == 0)
                {
                aValueType = EDateTime;
                }
            else
                {
                ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, vt, EFalse);
                // We can't continue - skip instead.
                }
            }
        }

    TTime dateAndTime;
    CESMRICalValue::TTimeZoneType tzType = CESMRICalValue::EUtcTime;

    CESMRICalValue* value = aProperty.Values()[aValue];

    switch (aValueType)
        {
        case EDate :
            value->GetDateL(dateAndTime);
            break;
        case EDateTime :
            value->GetDateTimeL(dateAndTime, tzType);
            break;
        case ETime :
            value->GetTimeL(dateAndTime, tzType);
            break;
        case EDefault :     // fall through..
        case EDuration :    // not supported, fall through
        case EPeriod :      // not supported, fall through
        default :
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorNotSupported, KNullDesC8, KICalPeriod, EFalse);
            // We can't continue - skip instead.
            break;
            }
        }

    const CESMRICalPropertyParam* tzid = aProperty.FindParam(KICalTzid);

    // Handle timezone information:
    if (tzType == CESMRICalValue::EUtcTime)
        {
        // A trailing Z means UTC:
        aTime.SetTimeUtcL(dateAndTime);
        }
    else if (!tzid || tzid->Values().Count() < 1)
        {
        // No trailing Z and no timezone ID.
        // This is a floating time.
        aTime.SetTimeLocalFloatingL(dateAndTime);
        }
    else
        {
        // Find the name of the timezone, and the timezone with that name:
        const CTzRules* rules = FindTzRule(tzid->Values()[0]->TextL());

        if (!rules)
            {
            ReportErrorL(MESMRAgnImportObserver::EImpErrorMissingData,KNullDesC8, aProperty.Type());
            // On continue, assume UTC.
            }
        else
            {
            // Remember the timezone:
            iCurrentTzRules = rules;
            // All times are stored in Agenda in UTC, and the timezone
            // applied by Agenda where appropriate:
            rules->ConvertToUtcL(dateAndTime);
            }

        // Set the time:
        aTime.SetTimeUtcL(dateAndTime);
        }
    }

/**
 * Searches through a set of rule segments and finds the segment corresponding to
 * a particular type.
 * @param aRules The array of rules to search.
 * @param aType The type of segment to search for from CESMRICalRuleSegment::TSegmentType.
 * @see CESMRICalRuleSegment::TSegmentType
 * @return The index of the rule within the array, or KErrNotFound
 * @internalTechnology
 */
TInt CESMRAgnVersit2Importer::FindRuleSegment( const RPointerArray<CESMRICalRuleSegment>& aRules,
                                           CESMRICalRuleSegment::TSegmentType aType ) const
    {
    FUNC_LOG;

    for (TInt r = aRules.Count() - 1; r > -1; --r)
        {
        if (aRules[r]->Type() == aType)
            {
            return r;
            }
        }
    return KErrNotFound;
    }

/**
 * Searches through the internally stored array of time zone rules looking for one
 * with a particular name, and returns its address. This does not transfer
 * ownership of the rule.
 * @param aName The name of the timezone rule to find (corresponding to TZID)
 * @return A pointer to the rule, or zero if no matching rule is found.
 * @internalTechnology
 */
const CTzRules* CESMRAgnVersit2Importer::FindTzRule(const TDesC& aName) const
    {
    FUNC_LOG;

    TInt tzCount = iTzRules.Count();
    for (TInt r = 0; r < tzCount; ++r)
        {
        if (iTzRules[r]->iName->CompareF(aName) == 0)
            {
            return iTzRules[r]->iRules;
            }
        }

    return NULL;
    }

/**
 * Reports an error through the MESMRAgnImportObserver interface, and then leaves if
 * the return value is anything other than
 * MESMRAgnImportObserver::EImpResponseContinue. The return value is stored in
 * iResponse.
 * @param aType The type of error which occurred
 * @param aUid The Uid of the entry in which it occurred, if known, or KNullDesC
 * if not.
 * @param aContext A textual indication of where the error occurred.
 * @param aCanContinue If this is not ETrue (default = ETrue) then a response of
 * EImpResponseContinue will be handled as an EImpResponseSkip
 * @leave KErrAbort
 * @internalTechnology
 */
void CESMRAgnVersit2Importer::ReportErrorL( MESMRAgnImportObserver::TImpError aType,
                                        const TDesC8& aUid,
                                        const TDesC& aContext,
                                        TBool aCanContinue )
    {
    FUNC_LOG;

    iResponse = iImportObserver->AgnImportErrorL(aType, aUid, aContext);
    if (iResponse != MESMRAgnImportObserver::EImpResponseContinue)
        {
        User::Leave(KErrAbort);
        }
    else if (!aCanContinue)
        {
        iResponse = MESMRAgnImportObserver::EImpResponseSkip;

        User::Leave(KErrAbort);
        }

    }

// End of file.

