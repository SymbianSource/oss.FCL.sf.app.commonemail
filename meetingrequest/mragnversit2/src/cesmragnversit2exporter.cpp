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
* Description: This file implements class CESMRAgnVersit2Exporter.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmragnversit2exporter.h"

//debug
//<cmail>

// Agenda includes.
#include <calalarm.h>                   // CCalAlarm
#include <calcategory.h>                // CCalCategory
#include <calentry.h>                   // CCalEntry
#include <calrrule.h>                   // TCalRRule
#include <caluser.h>                    // CCalUser
#include <vtzrules.h>                   // CVTzActualisedRules

// Versit includes.
#include "cesmrical.h"                      // cesmrical
#include "cesmricalcomponent.h"             // cesmricalcomponent
#include "esmricalkeywords.h"               // literals used during export
#include "cesmricalproperty.h"              // cesmricalproperty
#include "cesmricalvalue.h"                 // cesmricalvalue
#include "cesmricalbase.h"                  // ticalmethod
#include <tzlocalizer.h>
#include <tzconverter.h>

// MRUI includes
#include "esmrdef.h"
//</cmail>

// AgnVersit2 includes.
#include "cesmragnversit2stringprovider.h"  // CESMRAgnVersit2StringProvider
#include "cesmragnexternalinterface.h"      // TAgnEntryExport

// Unnamed namespace for local definitions and functions
namespace {

// Constants for timezone RRULE
_LIT(KRRuleFreqYearly, "FREQ=YEARLY;BYMONTH=");
_LIT(KRRuleByMonthDay, ";BYMONTHDAY=");
_LIT(KRRuleByDayPlus, ";BYDAY=+");
_LIT(KRRuleByDayPlusOne, ";BYDAY=+1");
_LIT(KRRuleByDayMinusOne, ";BYDAY=-1");

//iCalendar version number
_LIT(KICalVersionNumber,"2.0");

// Constant integer values
const TInt KMaxOffsetMinutes = 720; //12 hours * 60 minutes
const TInt KMinOffsetMinutes = -720; //-12 hours * 60 minutes

const TInt KSecondsInOneMinute = 60;
const TInt KInitialRRuleLength = 150; //RRule line length
const TInt KMaxBufLength = 80;  //Max time zone name length


// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::ConstructMailtoPropertyLC
// ---------------------------------------------------------------------------
//
TTime StartDateUtcL( const CCalEntry& aEntry )
    {
    return aEntry.StartTimeL().TimeUtcL();
    }

TTime EndDateUtcL( const CCalEntry& aEntry )
    {
    TTime endTime = aEntry.EndTimeL().TimeUtcL();
    TCalRRule rrule;

    if ( aEntry.GetRRuleL(rrule) )
        {
        endTime = rrule.Until().TimeUtcL();
        }

    return endTime;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::ConstructMailtoPropertyLC
// ---------------------------------------------------------------------------
//
void FilterTimezoneInformationL(
        CTzRules& aTzRules,
        TTime aStartTime )
    {
    FUNC_LOG;
    TInt index(0);
    while ( index < aTzRules.Count() )
        {
        TTzRule& singleRule = aTzRules[ index ];
        if ( ETzFixedDate == singleRule.iDayRule)
            {
            aTzRules.RemoveRule( index );
            }
        else
            {
            TTime startTime = singleRule.iFrom.iTime;

            //Check whether the year is 0, if so set it to 1970
            if( !startTime.DateTime().Year() )//Year == 0000
                {
                TDateTime dateTime = startTime.DateTime();
                dateTime.SetYear( 1970 );
                singleRule.iFrom.iTime = dateTime;
                }
            ++index;
            }
        }

    if ( aTzRules.Count() >= 2 )
        {
        // Only one STANDARD / DAYLIGHT PAIR is accepted by exchange
        // We need to locate the one, which corresponds to this one

        TTzRule standard;
        TTzRule dayLight;

        TInt daylightFound( KErrNotFound );
        TInt standardFound( KErrNotFound );

        index = 0;
        while ( index < aTzRules.Count() )
            {
            TTzRule& singleRule = aTzRules[ index ];

            TDateTime meetingStart = aStartTime.DateTime();
            TDateTime ruleStart = singleRule.iFrom.iTime.DateTime();

            TBool standardTimeZone( ETrue );
            if ( singleRule.iNewLocalTimeOffset > singleRule.iOldLocalTimeOffset )
                {
                // Daylight timezone
                standardTimeZone = EFalse;
                }
            // singleRule.iFrom.iTime
            if ( standardTimeZone )
                {
                if ( KErrNotFound == standardFound )
                    {
                    standard = singleRule;
                    standardFound = KErrNone;
                    }
                else if ( standard.iFrom.iTime < singleRule.iFrom.iTime &&
                        singleRule.iFrom.iTime <= aStartTime )
                    {
                    standard = singleRule;
                    }
                aTzRules.RemoveRule( index );
                }
            else
                {
                if ( KErrNotFound == daylightFound )
                    {
                    dayLight = singleRule;
                    daylightFound = KErrNone;
                    }
                else if ( dayLight.iFrom.iTime < singleRule.iFrom.iTime &&
                        singleRule.iFrom.iTime <= aStartTime )
                    {
                    dayLight = singleRule;
                    }
                aTzRules.RemoveRule( index );
                }
            }

        if ( KErrNotFound != standardFound &&  KErrNotFound != daylightFound )
            {
            // Add STANDARD / DAYLIGHT pair into timezone information
            aTzRules.AddRuleL( standard );
            aTzRules.AddRuleL( dayLight );
            }
        }
    else
        {
        // Not enough timezone areas --> Remove all
        index = 0;
        while ( index < aTzRules.Count() )
            {
            aTzRules.RemoveRule( index );
            }
        }
    }
} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::ConstructMailtoPropertyLC
// ---------------------------------------------------------------------------
//
HBufC* CESMRAgnVersit2Exporter::ConstructMailtoPropertyLC(
        const TDesC& aAddress )
    {
    FUNC_LOG;
    // "MAILTO:" prefix
    _LIT( KMailTo, "MAILTO:" );

    // Making the value to MAILTO:<address>
    HBufC* addressBuffer = HBufC::NewLC( KMailTo().Length() + aAddress.Length() );
    TPtr ptrAddressBuffer( addressBuffer->Des() );
    if ( aAddress.Find( KMailTo ) == KErrNotFound )
        {
        ptrAddressBuffer.Copy( KMailTo );
        } // find
    ptrAddressBuffer.Append( aAddress );

    return addressBuffer;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::NewL
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Exporter* CESMRAgnVersit2Exporter::NewL(CESMRAgnVersit2StringProvider& aStringProvider)
    {
    FUNC_LOG;

    CESMRAgnVersit2Exporter* self = CESMRAgnVersit2Exporter::NewLC(aStringProvider);
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::NewLC
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Exporter* CESMRAgnVersit2Exporter::NewLC(CESMRAgnVersit2StringProvider& aStringProvider)
    {
    FUNC_LOG;

    CESMRAgnVersit2Exporter* self = new (ELeave) CESMRAgnVersit2Exporter(aStringProvider);
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::~CESMRAgnVersit2Exporter
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Exporter::~CESMRAgnVersit2Exporter()
    {
    FUNC_LOG;

    iTzNameArray.ResetAndDestroy();
    iTimeZoneArray.ResetAndDestroy();

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::ExportL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2Exporter::ExportL( RPointerArray<CCalEntry>& aEntries,
                                   RWriteStream& aWriteStream,
                                   TUint aExportFlags,
                                   MESMRAgnExportObserver& aObserver )
    {
    FUNC_LOG;

    //We are exporting more than one entry to a single iCalendar.
    //Clear the time zone arrays, otherwise if this function is called twice
    //components in this iCalendar may reference timezone components in a preceding
    //iCalendar.
    iTzNameArray.ResetAndDestroy();
    iTimeZoneArray.ResetAndDestroy();

    iExportProperties = aExportFlags;
    iObserver = &aObserver;
    ASSERT( aEntries.Count() > 0 );
    CESMRICal* cal = AddICalLC(*aEntries[0]);

    TInt entryCount = aEntries.Count();

    TRAPD(err,

        for (TInt x = 0; x < entryCount; ++x)
            {
            if (iExportProperties & KAgnExportTzRules)
                {
                AddTimezoneL(*cal, *aEntries[x]);
                }
            AddComponentL(*cal, *aEntries[x]);
            }
        ); //End TRAP

    User::LeaveIfError(err);

    cal->ExternalizeL(aWriteStream);
    CleanupStack::PopAndDestroy(cal);

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::ExportL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2Exporter::ExportL( const CCalEntry& aEntry,
                                   RWriteStream& aWriteStream,
                                   TUint aExportFlags,
                                   MESMRAgnExportObserver& aObserver )
    {
    FUNC_LOG;

    //If we are exporting a single iCalendar per entry, we want to include the timezone
    //even if we have previously encountered it.  We do this by clearing the time zone
    //name array
    iTzNameArray.ResetAndDestroy();
    iTimeZoneArray.ResetAndDestroy();

    iExportProperties = aExportFlags;
    iObserver = &aObserver;
    CESMRICal* cal = AddICalLC(aEntry);

    TRAPD(err,
        if (iExportProperties & KAgnExportTzRules)
            {
            AddTimezoneL(*cal, aEntry);
            }
            AddComponentL(*cal, aEntry);
        );


    User::LeaveIfError(err);

    cal->ExternalizeL(aWriteStream);
    CleanupStack::PopAndDestroy(cal);

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::CESMRAgnVersit2Exporter
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2Exporter::CESMRAgnVersit2Exporter(CESMRAgnVersit2StringProvider& aStringProvider) :
    iStringProvider(aStringProvider)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2Exporter::ConstructL()
    {
    FUNC_LOG;
    iTimezoneIndex = KErrNotFound;
    }

/**
 * Creates a new CESMRICal*, adds any properties and returns it. The new object is
 * left on the cleanup stack.
 * @code
 * ICalendar components can have the following properties:
 *     PRODID: This is fixed for all exports from this device.
 *     VERSION: Should only ever be 2.0 for iCalendar v1 (vCalendar 2).
 *     METHOD: Member data from aEntry.
 * @endcode
 * @param aEntry entry to export.
 * @return newly created CESMRICal*.
 * @internalTechnology
 */
CESMRICal* CESMRAgnVersit2Exporter::AddICalLC(const CCalEntry& aEntry) const
    {
    FUNC_LOG;

    CESMRICal* cal = CESMRICal::NewLC();
    cal->AddPropertyL(KICalProdId, iStringProvider.StringL(EICalProdIdValue));
    cal->AddPropertyL(KICalVersion,KICalVersionNumber);

    switch (aEntry.MethodL())
        {
        case CCalEntry::EMethodPublish :
            cal->AddPropertyL(KICalMethod, KICalPublish);
            cal->SetMethodL(CESMRICal::EMethodPublish);
            break;
        case CCalEntry::EMethodRequest :
            cal->AddPropertyL(KICalMethod, KICalRequest);
            cal->SetMethodL(CESMRICal::EMethodRequest);
            break;
        case CCalEntry::EMethodReply :
            cal->AddPropertyL(KICalMethod, KICalReply);
            cal->SetMethodL(CESMRICal::EMethodReply);
            break;
        case CCalEntry::EMethodAdd :
            cal->AddPropertyL(KICalMethod, KICalAdd);
            cal->SetMethodL(CESMRICal::EMethodAdd);
            break;
        case CCalEntry::EMethodCancel :
            cal->AddPropertyL(KICalMethod, KICalCancel);
            cal->SetMethodL(CESMRICal::EMethodCancel);
            break;
        case CCalEntry::EMethodRefresh :
            cal->AddPropertyL(KICalMethod, KICalRefresh);
            cal->SetMethodL(CESMRICal::EMethodRefresh);
            break;
        case CCalEntry::EMethodCounter :
            cal->AddPropertyL(KICalMethod, KICalCounter);
            cal->SetMethodL(CESMRICal::EMethodCounter);
            break;
        case CCalEntry::EMethodDeclineCounter :
            cal->AddPropertyL(KICalMethod, KICalDeclineCounter);
            cal->SetMethodL(CESMRICal::EMethodDeclineCounter);
            break;
        case CCalEntry::EMethodNone :
            // fall through...
        default :
            //Add MEHTOD:PUBLISH as default if no method exists
            //needed by outlook
            cal->AddPropertyL(KICalMethod, KICalPublish);
            cal->SetMethodL(CESMRICal::EMethodPublish);
            break;
        }

    return cal;
    }


//
//     ADD COMPONENT METHODS
//     =====================
//

/**
 * Exports an agenda Anniversary to an iCalendar component.
 * @param aAnniv component to add to.
 * @param aEntry Entry containing information about the anniversary.
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddAnnivL(CESMRICalComponent& aAnniv, const CCalEntry& /*aEntry*/) const
    {
    FUNC_LOG;

    //Add 'X-PARAM' identifier
    if (iExportProperties & KAgnExportXProp)
        {
        aAnniv.AddPropertyL( iStringProvider.StringL(EICalXParamType),
                             iStringProvider.StringL(EICalXParamAnniversary) );
        }

    }


/**
 * Adds an alarm to a component.
 * @code
 * 
 * RFC 2445 states that an alarm MUST have an ACTION and TRIGGER property. We can
 * retrieve the trigger property value from the alarm offset, but Agenda does not
 * support the ACTION property.
 * 
 * An alarm is defined as:
 * 
 *     "BEGIN" ":" "VALARM" CRLF
 *     (audioprop / dispprop / emailprop / procprop)
 *     "END" ":" "VALARM" CRLF
 * 
 * where audioprop / dispprop / emailprop and procprop are all values of the
 * ACTION property.
 * 
 * An ACTION property value of AUDIO only requires an additional TRIGGER property,
 * all other properties are optional.  To comply with RFC 2445, all alarms will be
 * exported with the following property:
 * 
 *     ACTION:AUDIO
 * 
 * as to set ACTION to any other value would require more properties, which we do
 * not have values for.
 * 
 * @endcode
 * @param aComponent Component to add the VALARM component to
 * @param aEntry Entry to extract ALARM information from
 * @internalTechnology
*/
void CESMRAgnVersit2Exporter::AddAlarmL(CESMRICalComponent& aComponent, const CCalEntry& aEntry) const
    {
    FUNC_LOG;

    CCalAlarm* alarm = aEntry.AlarmL();
    if (alarm)
        {
        CleanupStack::PushL(alarm);
        CESMRICalComponent& component = aComponent.AddComponentL(CESMRICalBase::EICalAlarm);
        CESMRICalValue* triggerVal = CESMRICalValue::NewLC();
        triggerVal->SetDurationL(TTimeIntervalSeconds(-alarm->TimeOffset().Int() * KSecondsInOneMinute));
        CleanupStack::Pop(triggerVal);
        component.AddPropertyL(KICalTrigger, triggerVal); //Pass ownership
        component.AddPropertyL(KICalAction, KICalAudio);
        CleanupStack::PopAndDestroy(alarm);
        }

    }

/**
 * Adds an appointment to a component.
 * @param aAppt Component to add the appointment to.
 * @param aEntry Entry to extract the appointment information from.
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddApptL(CESMRICalComponent& aAppt, const CCalEntry& aEntry) const
    {
    FUNC_LOG;

    //Add 'DTEND' property
    if (iExportProperties & KAgnExportDtEnd)
        {
        TCalTime time(aEntry.EndTimeL());
        if (time.TimeUtcL() != Time::NullTTime())
            {
            AddDateTimePropertyL(aAppt, time, KICalDtend);
            }
        }

    if (iExportProperties & KAgnExportStatus)
        {
        //Add 'STATUS' property
        switch (aEntry.StatusL())
            {
            //VEvent supports TENTATIVE, CONFIRMED, CANCELLED
            case CCalEntry::ETentative:
                aAppt.AddPropertyL(KICalStatus, KICalTentative);
                break;
            case CCalEntry::EConfirmed:
                aAppt.AddPropertyL(KICalStatus, KICalConfirmed);
                break;
            case CCalEntry::ECancelled:
                aAppt.AddPropertyL(KICalStatus, KICalCancelled);
                break;
            case CCalEntry::ETodoNeedsAction :  // not supported, fall through
            case CCalEntry::ETodoCompleted :    // not supported, fall through
            case CCalEntry::ETodoInProcess :    // not supported, fall through
            default:
                //Do not add a status property
                break;
            }
        }

    //Add 'X-PARAM' identifier
    if (iExportProperties & KAgnExportXProp)
        {
        aAppt.AddPropertyL( iStringProvider.StringL(EICalXParamType),
                            iStringProvider.StringL(EICalXParamAppointment) );
        }

    }

/**
 * Adds a component to aICal. The type of component added is determined by the
 * value of aEntry.EntryTypeL(). Generic properties (valid in both VEVENT and
 * VTODO components) are added first, followed by type specific properties.
 * @param aICal CESMRICal object to add the component to.
 * @param aEntry CCalEntry containing information about the component.
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddComponentL(CESMRICal& aICal, const CCalEntry& aEntry) const
    {
    FUNC_LOG;

    //Add the  component
    CESMRICalBase::TICalComponentType type(CESMRICalBase::EICalEvent);
    switch (aEntry.EntryTypeL())
        {
        //The agenda types all map to an iCalendar 'VEVENT' component
        case CCalEntry::EAppt :
            // fall through...
        case CCalEntry::EEvent :
            // fall through...
        case CCalEntry::EReminder :
            // fall through...
        case CCalEntry::EAnniv :
            {
            type = CESMRICalBase::EICalEvent;
            break;
            }
        case CCalEntry::ETodo :
            {
            type = CESMRICalBase::EICalTodo;
            break;
            }
        default :
            User::Leave(KErrNotSupported); //For now at least
            break;
        }
    CESMRICalComponent& comp = aICal.AddComponentL(type);

    //Add 'ATTENDEE' properties
    if (iExportProperties & KAgnExportAttendee)
        {
        AddAttendeePropertiesL(comp, aEntry);
        }

    //Add 'ORGANIZER' property
    if (iExportProperties & KAgnExportOrganizer)
        {
        AddOrganizerPropertyL(comp, aEntry);
        }

    //Add 'LOCATION' property.
    if (iExportProperties & KAgnExportLocation)
        {
        AddTextPropertyL(comp, aEntry.LocationL(), KICalLocation);
        }

    //Outlook puts TRANSP here. The default value is 'OPAQUE'.

    //Add 'SEQUENCE' property.  The default value is '0', so
    //the property is only added if there is a value.
    if (iExportProperties & KAgnExportSequence)
        {
        TInt sequence = aEntry.SequenceNumberL();
        if (sequence != 0)
            {
            AddIntegerPropertyL(comp, aEntry.SequenceNumberL(), KICalSequence);
            }
        }

    //Add 'UID' property
    if (iExportProperties & KAgnExportUID)
        {
        CESMRICalValue* uidVal = CESMRICalValue::NewLC();
        uidVal->SetBinaryL(aEntry.UidL());
        CleanupStack::Pop(uidVal);
        comp.AddPropertyL(KICalUid, uidVal); //Pass ownership
        }

    //Time for DTSTAMP, CREATED and LAST-MODIFIED
    TCalTime time;

    //Check if DTSTAMP exist
    if( aEntry.DTStampL().TimeUtcL() != Time::NullTTime() )
        {
        time = aEntry.DTStampL();
        }
    else //If not use current time
        {
        TTime currentTime;
        currentTime.UniversalTime();
        time.SetTimeUtcL( currentTime );
        }

    //Add 'DTSTAMP' property
    if (iExportProperties & KAgnExportDtStamp)
        {
        AddUtcDateTimePropertyL(comp, time.TimeUtcL(), KICalDtstamp);
        }

    //Add 'CATEGORIES' properties
    if (iExportProperties & KAgnExportCategory)
        {
        AddCategoriesPropertyL(comp, aEntry);
        }

    //Add 'DESCRIPTION' property.
    if (iExportProperties & KAgnExportDescription)
        {
        AddTextPropertyL(comp, aEntry.DescriptionL(), KICalDescription);
        }

    //Add 'SUMMARY' property
    if (iExportProperties & KAgnExportSummary)
        {
        AddTextPropertyL(comp, aEntry.SummaryL(), KICalSummary);
        }


    //Add 'PRIORITY' property
    if (iExportProperties & KAgnExportPriority)
        {
        TInt priority = aEntry.PriorityL();

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
            AddIntegerPropertyL(comp, priority, KICalPriority);
        }

    //Add 'CLASS' property
    if (iExportProperties & KAgnExportClass)
        {
        AddClassPropertyL(comp, aEntry);
        }

    //Check if last modified date exist
    if( aEntry.LastModifiedDateL().TimeUtcL() != Time::NullTTime() )
        {
        //update time with it otherwise set CREATED and LAST-MODIFIED to
        //current time
        time = aEntry.LastModifiedDateL();
        }

    //Add 'CREATED' property with current system time.
    //time should be set to 'now' in UTC if not already.
    if (iExportProperties & KAgnExportCreated)
        {
        AddUtcDateTimePropertyL(comp, time.TimeUtcL(), KICalCreated);
        }

    //Add 'LAST-MODIFIED' property
    if (iExportProperties & KAgnExportLastModified)
        {
        time = aEntry.LastModifiedDateL();
        if (time.TimeUtcL() != Time::NullTTime())
            {
            AddUtcDateTimePropertyL(comp, time.TimeUtcL(), KICalLastmodified);
            }
        }

    //Add 'RECURRENCE-ID' property
    if (iExportProperties & KAgnExportRecurrenceId)
        {
        time = aEntry.RecurrenceIdL();
        if (time.TimeUtcL() != Time::NullTTime())
            {
            CESMRICalProperty& recurProp = AddDateTimePropertyL(comp, time, KICalRecurrenceId);
            switch (aEntry.RecurrenceRangeL())
                {
                case CalCommon::EThisAndFuture:
                    {
                    recurProp.AddPropertyParamL(KICalRange,KICalThisAndFuture);
                    }
                break;
                case CalCommon::EThisAndPrior:
                    {
                    recurProp.AddPropertyParamL(KICalRange,KICalThisAndPrior);
                    }
                break;
                default:
                break;
                }

            }
        }

    //Add 'EXDATE' properties
    if (iExportProperties & KAgnExportExDate)
        {
        AddExceptionDatePropertyL(comp, aEntry);
        }

    //Add 'RDATE' property
    if (iExportProperties & KAgnExportRDate)
        {
        AddRDatePropertyL(comp, aEntry);
        }

    //Add 'RRULE' property
    if (iExportProperties & KAgnExportRRule)
        {
        AddRRulePropertyL(comp, aEntry);
        }

    //Add 'VALARM' component
    if (iExportProperties & KAgnExportAlarm)
        {
        AddAlarmL(comp, aEntry);
        }

    //Add 'DTSTART' property
    if (iExportProperties & KAgnExportDtStart)
        {
        time = aEntry.StartTimeL();
        if (time.TimeUtcL() != Time::NullTTime())
            {
            AddDateTimePropertyL(comp, time, KICalDtstart);
            }
        }

    //Add type specific properties
    switch (aEntry.EntryTypeL())
        {
        case CCalEntry::EAppt:
            AddApptL(comp, aEntry);
            break;
        case CCalEntry::EEvent:
            AddEventL(comp, aEntry);
            break;
        case CCalEntry::EReminder:
            AddReminderL(comp, aEntry);
            break;
        case CCalEntry::EAnniv:
            AddAnnivL(comp, aEntry);
            break;
        case CCalEntry::ETodo:
            AddTodoL(comp, aEntry);
            break;
        default:
            //Nothing more to add
            break;
        }

    }

/**
 * Adds the information stored in aEntry to aEvent
 * @param aEvent CESMRICalComponent& to add the agenda information to.
 * @param aEntry CCalEntry& containing the agenda information to export.
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddEventL(CESMRICalComponent& aEvent, const CCalEntry& aEntry) const
    {
    FUNC_LOG;

    //Add 'DTEND' property
    if (iExportProperties & KAgnExportDtEnd)
        {
        TCalTime time(aEntry.EndTimeL());
        if (time.TimeUtcL() != Time::NullTTime())
            {
            AddDateTimePropertyL(aEvent, time, KICalDtend);
            }
        }

    if (iExportProperties & KAgnExportStatus)
        {
        //Add 'STATUS' property
        switch (aEntry.StatusL())
            {
            //VEvent supports TENTATIVE, CONFIRMED, CANCELLED
            case CCalEntry::ETentative:
                aEvent.AddPropertyL(KICalStatus, KICalTentative);
                break;
            case CCalEntry::EConfirmed:
                aEvent.AddPropertyL(KICalStatus, KICalConfirmed);
                break;
            case CCalEntry::ECancelled:
                aEvent.AddPropertyL(KICalStatus, KICalCancelled);
                break;
            case CCalEntry::ETodoNeedsAction :  // not supported, fall through
            case CCalEntry::ETodoCompleted :    // not supported, fall through
            case CCalEntry::ETodoInProcess :    // not supported, fall through
            default:
                //Do not add a status property
                break;
            }
        }
    //Add 'X-PARAM' identifier
    if (iExportProperties & KAgnExportXProp)
        {
        aEvent.AddPropertyL( iStringProvider.StringL(EICalXParamType),
                             iStringProvider.StringL(EICalXParamEvent) );
        }

    }

/**
 * Adds an agenda-style Reminder to the specified component.
 * @param aReminder Component to add the reminder to.
 * @param aEntry CCalEntry containing details of the reminder.
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddReminderL(CESMRICalComponent& aReminder, const CCalEntry& /*aEntry*/) const
    {
    FUNC_LOG;

    //Add 'X-PARAM' identifier
    if (iExportProperties & KAgnExportXProp)
        {
        aReminder.AddPropertyL( iStringProvider.StringL(EICalXParamType),
                                iStringProvider.StringL(EICalXParamReminder) );
        }

    }

/**
 * Adds a Timezone component to the specified CCalEntry.
 * @param aICal CESMRICal object to add the component to.
 * @param aEntry CCalEntry containing information about the Timezone.
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddTimezoneL(CESMRICal& aICal, const CCalEntry& aEntry)
    {
    FUNC_LOG;

    iTimezoneIndex = KErrNotFound;

    CTzRules* tzRules = aEntry.TzRulesL();
    CleanupStack::PushL(tzRules);

    if (!tzRules || tzRules->Count() < 1)
        {
        CleanupStack::PopAndDestroy(tzRules);
        tzRules = NULL;

        TTime utcStartTime = StartDateUtcL( aEntry );
        TTime utcEndTime = EndDateUtcL( aEntry );

        RTz tzServer;
        CleanupClosePushL( tzServer );
        User::LeaveIfError( tzServer.Connect() );

        tzRules =
                tzServer.GetTimeZoneRulesL(
                            utcStartTime,
                            utcEndTime,
                            ETzUtcTimeReference );

        CleanupStack::PopAndDestroy( &tzServer );
        CleanupStack::PushL( tzRules );
        }

    FilterTimezoneInformationL( *tzRules, StartDateUtcL( aEntry ) );
    TInt count = tzRules->Count();
    if ( !count )
        {
        CleanupStack::PopAndDestroy(tzRules);
        tzRules = NULL;
        return; // --> No timezones to export
        }

    TInt minimumMinutes = KMaxOffsetMinutes;// TInts to store the maximum and minimum UTC offset
    TInt maximumMinutes = KMinOffsetMinutes;// to get a human readable name for TZID

    for (TInt i = 0; i < count; ++i)
        {
        TTzRule& singleRule = (*tzRules)[i];
        if (singleRule.iNewLocalTimeOffset < minimumMinutes)
            {
            minimumMinutes = singleRule.iNewLocalTimeOffset;
            }
        if (singleRule.iNewLocalTimeOffset > maximumMinutes)
            {
            maximumMinutes = singleRule.iNewLocalTimeOffset;
            }
        if (singleRule.iOldLocalTimeOffset < minimumMinutes)
            {
            minimumMinutes = singleRule.iOldLocalTimeOffset;
            }
        if (singleRule.iOldLocalTimeOffset > maximumMinutes)
            {
            maximumMinutes = singleRule.iOldLocalTimeOffset;
            }
        }

    // Now we've got the max and minimum values, store these in the TzId text field
    // iTimeZoneName allocates a new HBufC, but either deletes it or transfers ownership
    // to iTznamesArray
    HBufC* timeZoneName = HBufC::NewLC(KMaxBufLength);
    timeZoneName->Des().Copy(iStringProvider.StringL(EICalTzidUtc));

    //Convert minutes to hours
    const TInt KConversionMul = 10;
    const TInt KConversionDiv = 6;

    // Number of digits to store time in
    const TInt KTimeWidth = 4;

    TInt minimumHours = minimumMinutes * KConversionMul / KConversionDiv; // convert to HHMM format
    if (minimumHours < 0)
        {
        minimumHours =- minimumHours;
        timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidMinus));
        }
    else
        {
        timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidPlus));
        }
    timeZoneName->Des().AppendNumFixedWidth(minimumHours, EDecimal, KTimeWidth);
    timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidStandard));

    //Add a slash seperator
    timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidSlash));
    timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidUtc));

    TInt maximumHours = maximumMinutes * KConversionMul / KConversionDiv; // convert to HHMM format
    if (maximumHours < 0)
        {
        maximumHours =- maximumHours;
        timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidMinus));
        }
    else
        {
        timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidPlus));
        }
    timeZoneName->Des().AppendNumFixedWidth(maximumHours, EDecimal, KTimeWidth);
    timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidDaylight));

    // The time zone name should now read: "UTC +HHMM (Standard) / UTC +HHMM (Daylight)".

    // Check if we have already used this timezone in this iCal.  If we have,
    // we reuse this string but don't output the timezone again.
    TInt tzCount = iTzNameArray.Count();
    for (TInt x = 0; x < tzCount; ++x)
        {
        if ((iTzNameArray[x])->CompareC(*timeZoneName) == 0)
            {
            //Already used this timezone
            iTimezoneIndex = x;
            CleanupStack::PopAndDestroy(timeZoneName);
            CleanupStack::PopAndDestroy(tzRules);
            return;
            }
        }

    //Add the 'VTIMEZONE' component
    CESMRICalComponent& timezone = aICal.AddComponentL(CESMRICalBase::EICalTimeZone);

    timezone.AddPropertyL(KICalTzid, *timeZoneName);

    for (TInt ii = 0; ii < count; ++ii)
        {
        TTzRule& singleRule = (*tzRules)[ii];

        // Set the DAYLIGHT/STANDARD field based on whether the
        // time zone rule shifts time forwards or backwards
        CESMRICalComponent& tzProp = singleRule.iNewLocalTimeOffset < singleRule.iOldLocalTimeOffset ?
            timezone.AddComponentL(CESMRICalBase::EICalStandard) :
            timezone.AddComponentL(CESMRICalBase::EICalDaylight);

        //  -DTSTART (Required)
        CESMRICalValue* dtstart = CESMRICalValue::NewLC();


        TTime startTime = singleRule.iFrom.iTime;

        //Check whether the year is 0, if so set it to 1970
        if( !startTime.DateTime().Year() )//Year == 0000
            {
            TDateTime dateTime = startTime.DateTime();
            dateTime.SetYear( 1970 );
            startTime = dateTime;
            }

        // DTSTART in a VTIMEZONE must always be in local time (with no TZ identifier).
        dtstart->SetDateTimeL(startTime,CESMRICalValue::ESpecifiedTimeZone);

        CleanupStack::Pop(dtstart);
        tzProp.AddPropertyL(KICalDtstart, dtstart); //Pass ownership

        //  -RRULE (optional)
        CESMRICalValue* tzRrule = CESMRICalValue::NewLC();
        HBufC* ruleText = HBufC::NewLC(KMaxBufLength);
        ruleText->Des().Copy(KRRuleFreqYearly);

        TInt monthNum = singleRule.iMonth + 1;

        switch (singleRule.iDayRule)
            {
            case ETzFixedDate :
                {
                // e.g. 11th October
                // Equivalent to FREQ=YEARLY;BYMONTH=?;BYMONTHDAY=?
                // This won't get imported by Outlook, but it's the only way of exporting it.
                // It can't be converted to a week of the month.
                ruleText->Des().AppendNum(monthNum);
                ruleText->Des().Append(KRRuleByMonthDay);
                ruleText->Des().AppendNum(singleRule.iDayOfMonth);
                break;
                }
            case ETzDayAfterDate :
                {
                // e.g. first Sunday after 8th October
                // To get this to a format understood by Outlook, we have to convert the day of
                // month into a week number. If the day of the month is greater than 21, the week
                // could cover multiple months (and possibly years), so we export using BYMONTHDAY.
                const TInt KLastPossibleDayOfMonth = 21;
                if (singleRule.iDayOfMonth > KLastPossibleDayOfMonth)
                    {
                    // Equivalent to FREQ=YEARLY;BYMONTH=?;BYMONTHDAY=?;BYDAY=+1?
                    ruleText->Des().AppendNum(monthNum);
                    ruleText->Des().Append(KRRuleByMonthDay);
                    ruleText->Des().AppendNum(singleRule.iDayOfMonth);
                    ruleText->Des().Append(KRRuleByDayPlusOne);
                    ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
                    }
                else
                    {
                    // Equivalent to FREQ=YEARLY;BYMONTH=?;BYDAY=+n?
                    ruleText->Des().AppendNum(monthNum);
                    ruleText->Des().Append(KRRuleByDayPlus);
                    ruleText->Des().AppendNum(WeekNumFromDayOfMonth(singleRule.iDayOfMonth));
                    ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
                    }
                break;
                }
            case ETzDayBeforeDate :
                {
                // e.g. Sunday before 8th October
                // To get this to a format understood by Outlook, we have to convert the day of
                // month into a week number. If the day of the month is less than 8, the week
                // could cover multiple months (and possibly years), so we export using BYMONTHDAY.
                const TInt KFirstPossibleDayOfMonth = 8;
                if (singleRule.iDayOfMonth < KFirstPossibleDayOfMonth)
                    {
                    // Equivalent to FREQ=YEARLY;BYMONTH=?;BYMONTHDAY=?;BYDAY=-1?
                    ruleText->Des().AppendNum(monthNum);
                    ruleText->Des().Append(KRRuleByMonthDay);
                    ruleText->Des().AppendNum(singleRule.iDayOfMonth);
                    ruleText->Des().Append(KRRuleByDayMinusOne);
                    ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
                    }
                else
                    {
                    // Equivalent to FREQ=YEARLY;BYMONTH=?;BYDAY=+n?
                    ruleText->Des().AppendNum(monthNum);
                    ruleText->Des().Append(KRRuleByDayPlus);
                    ruleText->Des().AppendNum(WeekNumFromDayOfMonth(singleRule.iDayOfMonth));
                    ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
                    }
                break;
                }
            case ETzDayInLastWeekOfMonth :
                {
                // e.g. last Sunday in October
                // Equivalent to FREQ=YEARLY;BYMONTH=?;BYDAY=-1?
                ruleText->Des().AppendNum(monthNum);
                ruleText->Des().Append(KRRuleByDayMinusOne);
                ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
                break;
                }
            default :
                User::Leave(KErrCorrupt);
                break;
            }
        tzRrule->SetRecurrenceRuleL(*ruleText);
        CleanupStack::PopAndDestroy(ruleText);
        CleanupStack::Pop(tzRrule);
        tzProp.AddPropertyL(KICalRRule, tzRrule); //Pass ownership

        //  -TZOFFSETFROM (Required)
        CESMRICalValue* tzOffsetFrom = CESMRICalValue::NewLC();
        tzOffsetFrom->SetUtcOffsetL(
                    TTimeIntervalSeconds(singleRule.iOldLocalTimeOffset * KSecondsInOneMinute) );
        CleanupStack::Pop(tzOffsetFrom);
        tzProp.AddPropertyL(KICalTzoffsetfrom, tzOffsetFrom); //Pass ownership

        //  -TZOFFSETTO (Required)
        CESMRICalValue* tzOffsetTo = CESMRICalValue::NewLC();
        tzOffsetTo->SetUtcOffsetL(
                    TTimeIntervalSeconds(singleRule.iNewLocalTimeOffset * KSecondsInOneMinute) );
        CleanupStack::Pop(tzOffsetTo);
        tzProp.AddPropertyL(KICalTzoffsetto, tzOffsetTo); //Pass ownership
        }

    iTzNameArray.AppendL(timeZoneName); //Transfer ownership
    iTimeZoneArray.AppendL(tzRules);    //Transfer ownership

    iTimezoneIndex = iTimeZoneArray.Count() -1;
    CleanupStack::Pop(timeZoneName);
    CleanupStack::Pop(tzRules);

    }

/**
 * Adds properties specific to VTODO to the specified component.
 * @param aTodo VTODO component to add properties to.
 * @param aEntry CCalEntry containing details of the TO-DO.
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddTodoL(CESMRICalComponent& aTodo,const CCalEntry& aEntry) const
    {
    FUNC_LOG;

    TCalTime time(aEntry.EndTimeL());

    if (iExportProperties & KAgnExportDtEnd)
        {
        if (time.TimeUtcL() != Time::NullTTime())
            {
            //Add 'DUE' property
            AddDateTimePropertyL(aTodo, time, KICalDue);
            }
        }

    //Add 'COMPLETED' property
    if (iExportProperties & KAgnExportCompleted)
        {
        time = aEntry.CompletedTimeL();
        if (time.TimeUtcL() != Time::NullTTime())
            {
            //Add 'COMPLETED' property
            AddDateTimePropertyL(aTodo, time, KICalCompleted);
            }
        }

    //Add 'STATUS' property
    if (iExportProperties & KAgnExportStatus)
        {
        switch (aEntry.StatusL())
            {
            //VTODO supports NEEDS_ACTION, COMPLETED, IN-PROCESS, CANCELLED
            case CCalEntry::ECancelled :
                aTodo.AddPropertyL(KICalStatus, KICalCancelled);
                break;
            case CCalEntry::ETodoNeedsAction :
                aTodo.AddPropertyL(KICalStatus, KICalNeedsAction);
                break;
            case CCalEntry::ETodoCompleted :
                aTodo.AddPropertyL(KICalStatus, KICalCompleted);
                break;
            case CCalEntry::ETodoInProcess :
                aTodo.AddPropertyL(KICalStatus, KICalInProcess);
                break;
            case CCalEntry::ETentative :    // not supported, fall through
            case CCalEntry::EConfirmed :    // not supported, fall through
            default :
                //Do not add a status property
                break;
            }
        }
    //Add 'X-PARAM' identifier
    if (iExportProperties & KAgnExportXProp)
        {
        aTodo.AddPropertyL( iStringProvider.StringL(EICalXParamType),
                            iStringProvider.StringL(EICalXParamTodo) );
        }

    }


//
//     ADD PROPERTY FUNCTIONS
//     ======================
//

/**
 * Adds 'ATTENDEE' properties to aComponent from aEntry
 * @param aComponent component to add the ATTENDEE property to
 * @param aEntry entry to extract ATTENDEE information from
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddAttendeePropertiesL( CESMRICalComponent& aComponent,
                                                  const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    const RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
    const TInt count = attendees.Count();
    CCalAttendee* attendee = NULL;
    CESMRICalValue* val = NULL;
    for (TInt x = 0; x < count; ++x)
        {
        /** Can't add the following property parameters due to no equivalence:
        *   (";" dirparam) / (";" languageparam) / (";" cutypeparam) /
        *   (";"memberparam) / (";" deltoparam) / (";" delfromparam)
        */
        attendee = attendees[x]; //Not taking ownership
        val = CESMRICalValue::NewLC();
        HBufC* address = ConstructMailtoPropertyLC( attendee->Address() );
        val->SetTextL( *address );
        CleanupStack::PopAndDestroy( address );
        address = NULL;

        //Add a property for each ATTENDEE
        CleanupStack::Pop(val);
        CESMRICalProperty& attProp = aComponent.AddPropertyL(KICalAttendee, val); //Pass ownership

        //Add 'CN' property parameter
        TPtrC commonName(attendee->CommonName());
        if (commonName.Length() > 0)
            {
            attProp.AddPropertyParamL(KICalCn, commonName);
            }

        //Add 'ROLE' property parameter
        TPtrC roleName(RoleFromEnum(attendee->RoleL()));
        if (roleName.Length() > 0)
            {
            attProp.AddPropertyParamL(KICalRole, roleName);
            }

        //Add 'RSVP' property parameter
        // Default is FALSE so don't add a value if this is the case
        if (attendee->ResponseRequested())
            {
            CESMRICalValue* rsvpVal = CESMRICalValue::NewLC();
            rsvpVal->SetBooleanL(ETrue);
            CleanupStack::Pop(rsvpVal);
            attProp.AddPropertyParamL(KICalRsvp, rsvpVal);
            }

        //Add "SENT-BY" property parameter
        TPtrC sentBy(attendee->SentBy());
        if (sentBy.Length() > 0)
            {
            attProp.AddPropertyParamL(KICalSentBy, sentBy);
            }

        //Add PARTSTAT property parameter
        TPtrC participationStatus(StatusFromEnum(attendee->StatusL()));
        if (participationStatus.Length() > 0)
            {
            attProp.AddPropertyParamL(KICalPartStat, participationStatus);
            }
        }

    }

/**
 * Adds 'CATEGORY' properties to aComponent from aEntry
 * @param aComponent component to add the CATEGORY property to
 * @param aEntry entry to extract CATEGORY information from
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddCategoriesPropertyL( CESMRICalComponent& aComponent,
                                                  const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    //CategoryListL is not const so we need a non-const CalEntry to call it
    CCalEntry& nonConstEntry = const_cast<CCalEntry&>(aEntry);
    const RPointerArray<CCalCategory>& categories = nonConstEntry.CategoryListL();
    const TInt count = categories.Count();
    if (count)
        {
        CESMRICalProperty& catProp =
                aComponent.AddPropertyL(KICalCategories, CategoryStringL(*categories[0]));
        for (TInt x = 1; x < count; ++x)
            {
            catProp.AddValueL(CategoryStringL(*categories[x]));
            }
        }

    }

/**
 * Adds a CLASS property to the specified component
 * @param aComponent component to add the CLASS property to
 * @param aEntry entry to extract CLASS information from
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddClassPropertyL( CESMRICalComponent& aComponent,
                                             const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    CCalEntry::TReplicationStatus repStatus = aEntry.ReplicationStatusL();
    TPtrC status(ClassStringL(repStatus));
    if (status.Length() > 0)
        {
        // Only add the CLASS property if it is not the default, 'PUBLIC'.
        if (status.CompareF(KICalPublic) != 0)
            {
            aComponent.AddPropertyL(KICalClass, status);
            }
        }

    }

/**
 * Adds 'EXDATE' property to aComponent from aEntry
 * @param aComponent component to add the EXDATE property to
 * @param aEntry entry to extract EXDATE information from
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddExceptionDatePropertyL( CESMRICalComponent& aComponent,
                                                     const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    RArray<TCalTime> exDates;
    CleanupClosePushL(exDates);
    aEntry.GetExceptionDatesL(exDates);
    const TInt count = exDates.Count();
    if (count)
        {
        CESMRICalValue* val = CESMRICalValue::NewLC();
        val->SetDateTimeL(exDates[0].TimeUtcL(), CESMRICalValue::EUtcTime);
        CleanupStack::Pop(val);
        CESMRICalProperty& prop = aComponent.AddPropertyL(KICalExdate, val); //Pass ownership

        for (TInt x = 1; x < count; ++x)
            {
            val = CESMRICalValue::NewLC();
            val->SetDateTimeL(exDates[x].TimeUtcL(), CESMRICalValue::EUtcTime);
            CleanupStack::Pop(val);
            prop.AddValueL(val); //Pass ownership
            }
        }
    CleanupStack::PopAndDestroy(&exDates);

    }

/**
 * Adds an integer value to a given component
 * @param aComponent component to add the integer value to
 * @param aInt integer to be added
 * @param aProperty current property
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddIntegerPropertyL( CESMRICalComponent& aComponent,
                                               TInt aInt,
                                               const TDesC& aProperty ) const
    {
    FUNC_LOG;

    CESMRICalValue* val = CESMRICalValue::NewLC();
    val->SetIntegerL(aInt);
    CleanupStack::Pop(val);
    aComponent.AddPropertyL(aProperty, val); //Pass ownership

    }

/**
 * Adds an 'ORGANIZER' property to aComponent from aEntry
 * @param aComponent component to add the ORGANIZER property to
 * @param aEntry entry to extract ORGANIZER information from
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddOrganizerPropertyL( CESMRICalComponent& aComponent,
                                                 const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    CCalUser* organizer = aEntry.OrganizerL();

    if (organizer)
        {
        /** Can't add the following property parameters due to no equivalence:
        *   (";" dirparam) /  (";" languageparam)
        */
        CESMRICalValue* orgVal = CESMRICalValue::NewLC();

        HBufC* address = ConstructMailtoPropertyLC( organizer->Address() );
        orgVal->SetTextL( *address );
        CleanupStack::PopAndDestroy( address );
        address = NULL;

        CleanupStack::Pop(orgVal);
        CESMRICalProperty& orgProp = aComponent.AddPropertyL(KICalOrganizer, orgVal); //Pass ownership
        //Add 'CN' property parameter
        TPtrC commonName(organizer->CommonName());
        if (commonName.Length() > 0)
            {
            orgProp.AddPropertyParamL(KICalCn, commonName);
            }

        //Add "SENT-BY" property parameter
        TPtrC sentBy(organizer->SentBy());
        if (sentBy.Length() > 0)
            {
            orgProp.AddPropertyParamL(KICalSentBy, sentBy );
            }
        }

    }

/**
 * Adds a text value to a given component
 * @param aComponent component to add the text value to
 * @param aText text to be added
 * @param aProperty current property
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddTextPropertyL( CESMRICalComponent& aComponent,
                                            const TDesC& aText,
                                            const TDesC& aProperty ) const
    {
    FUNC_LOG;

    if (aText.Length() > 0)
        {
        aComponent.AddPropertyL(aProperty, aText);
        }

    }

/**
 * Adds 'RDATE' property to aComponent from aEntry
 * @param aComponent component to add the RDATE property to
 * @param aEntry entry to extract RRULE information from
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddRDatePropertyL( CESMRICalComponent& aComponent,
                                             const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    RArray<TCalTime> rDates;
    CleanupClosePushL(rDates);
    aEntry.GetRDatesL(rDates);
    const TInt count = rDates.Count();
    if (count)
        {
        //Must create a property with a value
        CESMRICalValue* val = CESMRICalValue::NewLC();
        val->SetDateTimeL(rDates[0].TimeUtcL(), CESMRICalValue::EUtcTime);
        CleanupStack::Pop(val);
        CESMRICalProperty& prop = aComponent.AddPropertyL(KICalRdate, val); //Pass ownership

        for (TInt x = 1; x < count; ++x)
            {
            val = CESMRICalValue::NewLC();
            val->SetDateTimeL(rDates[x].TimeUtcL(), CESMRICalValue::EUtcTime);
            CleanupStack::Pop(val);
            prop.AddValueL(val);
            }
        }
    CleanupStack::PopAndDestroy(&rDates);

    }

/**
 * Adds 'EXDATE' property to aComponent from aEntry
 * @param aComponent component to add the EXDATE property to
 * @param aEntry entry to extract EXDATE information from
 * @internalTechnology
 */
void CESMRAgnVersit2Exporter::AddRRulePropertyL( CESMRICalComponent& aComponent,
                                             const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    TCalRRule rule;
    if (aEntry.GetRRuleL(rule))
        {
        HBufC* ruleVal = HBufC::NewLC(KInitialRRuleLength);
        TPtr ruleValPtr(ruleVal->Des());
        //Add 'FREQ'
        ruleValPtr.Append(KICalFreq);
        ruleValPtr.Append(KICalEquals);
        switch (rule.Type())
            {
            case TCalRRule::EDaily:
                ruleValPtr.Append(KICalDaily);
                break;
            case TCalRRule::EWeekly:
                ruleValPtr.Append(KICalWeekly);
                break;
            case TCalRRule::EMonthly:
                ruleValPtr.Append(KICalMonthly);
                break;
            case TCalRRule::EYearly:
                ruleValPtr.Append(KICalYearly);
                break;
            default:
                {
                User::Leave(KErrCorrupt);
                }
            }

        // Add 'UNTIL' or 'COUNT'.  Not both.
        // Try UNTIL first as the COUNT implementation has issues.
        TCalTime untilTime = rule.Until();

        if (untilTime.TimeUtcL() == TCalTime::MaxTime())
            {
            // "UNTIL" returns the maximum date that Agenda can handle, so let's assume that it
            // repeats "forever". In this case we do not export a value for either UNTIL or COUNT.
            // Do nothing...
            }
        else if (untilTime.TimeUtcL() != Time::NullTTime())
            {
            ruleValPtr.Append(KICalSemiColon);
            // Add the 'UNTIL'
            ruleValPtr.Append(KICalUntil);
            ruleValPtr.Append(KICalEquals);
            CESMRICalValue* val = CESMRICalValue::NewLC();
            // iCalendar spec states 'UNTIL' should be in UTC
            val->SetDateTimeL(untilTime.TimeUtcL(), CESMRICalValue::EUtcTime);
            ruleValPtr.Append(val->TextL());
            CleanupStack::PopAndDestroy(val);
            }
        else
            {
            TInt countNum = rule.Count();

            if (countNum != 0)
                {
                ruleValPtr.Append(KICalSemiColon);
                ruleValPtr.Append(KICalCount);
                ruleValPtr.Append(KICalEquals);

                // APINOTE: COUNT seems to be divided by interval during aEntry.GetRRuleL call.
                // APINOTE: It also seems to be count left (from now) rather than count from DTSTART.
                // APINOTE: In either case UNTIL will always be valid with the current API, so COUNT will never be used.
                countNum *= rule.Interval();
                ruleValPtr.AppendNum(countNum);
                }

            // Else neither is present event repeats forever.
            }

        //Add 'INTERVAL' if present
        TInt interval = rule.Interval();

        if (interval > 1)   // "1" is the default, so don't bother exporting it.
            {
            ruleValPtr.Append(KICalSemiColon);
            ruleValPtr.Append(KICalInterval);
            ruleValPtr.Append(KICalEquals);
            ruleValPtr.AppendNum(rule.Interval());
            }

        //Add 'BYMONTHDAY' if present
        if(rule.Type() == TCalRRule::EMonthly)
            {
            RArray<TInt> monthDays;
            CleanupClosePushL(monthDays);
            rule.GetByMonthDayL(monthDays);
            TInt count = monthDays.Count();

            if (count > 0)
                {
                ruleValPtr.Append(KICalSemiColon);
                ruleValPtr.Append(KICalByMonthDay);
                ruleValPtr.Append(KICalEquals);

                for (TInt x = 0; x < count; ++x)
                    {
                    //Add 1 the month day ( 0 = 1).
                    ruleValPtr.AppendNum( monthDays[x]  + 1 );
                    if (x < count - 1)
                        {
                        ruleValPtr.Append(KICalComma);
                        }
                    }

                }
            CleanupStack::PopAndDestroy(&monthDays);
            }

        //Add 'WKST'.  Agenda only supports 'WKST' if 'FREQ=WEEKLY'
        if (rule.Type() == TCalRRule::EWeekly)
            {
            ruleValPtr.Append(KICalSemiColon);
            ruleValPtr.Append(KICalWkSt);
            ruleValPtr.Append(KICalEquals);
            ruleValPtr.Append(DayFromTDay(rule.WkSt()));
            }


        //Add 'BYDAY'
        if ( rule.Type() == TCalRRule::EYearly  ||
             rule.Type() == TCalRRule::EMonthly ||
             rule.Type() == TCalRRule::EWeekly )
            {
            RArray<TDay> days;
            CleanupClosePushL(days);
            RArray<TCalRRule::TDayOfMonth> daysOfMonth;
            CleanupClosePushL(daysOfMonth);
            rule.GetByDayL(days);
            rule.GetByDayL(daysOfMonth);
            TInt daysCount = days.Count();
            TInt daysOfMonthCount = daysOfMonth.Count();

            if (daysCount > 0 || daysOfMonthCount > 0)
                {
                ruleValPtr.Append(KICalSemiColon);
                ruleValPtr.Append(KICalByDay);
                ruleValPtr.Append(KICalEquals);
                }

            if (daysCount > 0)
                {
                for (TInt x = 0; x < daysCount; ++x)
                    {
                    ruleValPtr.Append(DayFromTDay(days[x]));
                    if (x < daysCount - 1)
                        {
                        ruleValPtr.Append(KICalComma);
                        }
                    }
                }
            if (daysOfMonthCount > 0)
                {
                for (TInt x = 0; x < daysOfMonthCount; ++x)
                    {
                    ruleValPtr.AppendNum(daysOfMonth[x].WeekInMonth());
                    ruleValPtr.Append(DayFromTDay(daysOfMonth[x].Day()));
                    if (x < daysOfMonthCount - 1)
                        {
                        ruleValPtr.Append(KICalComma);
                        }
                    }
                }
            CleanupStack::PopAndDestroy(&daysOfMonth);
            CleanupStack::PopAndDestroy(&days);
            }


        //Add 'BYMONTH'. Agenda only supports 'BYMONTH' if 'FREQ=YEARLY'
        if (rule.Type() == TCalRRule::EYearly)
            {
            RArray<TMonth> theMonths;
            CleanupClosePushL(theMonths);
            rule.GetByMonthL(theMonths);

            TInt monthsCount = theMonths.Count();

            if (monthsCount > 0)
                {
                ruleValPtr.Append(KICalSemiColon);
                ruleValPtr.Append(KICalByMonth);
                ruleValPtr.Append(KICalEquals);

                for (int i = 0; i < monthsCount; i++)
                    {
                    TInt monthNum = (TInt)(theMonths[i] + 1);   // TMonth is offset from 0
                    ruleValPtr.AppendNum(monthNum);
                    if (i < monthsCount - 1)
                        {
                        ruleValPtr.Append(KICalComma);
                        }
                    }
                }
            CleanupStack::PopAndDestroy(&theMonths);
            }

        CESMRICalValue* val = CESMRICalValue::NewLC();
        val->SetTextL(ruleValPtr);
        CleanupStack::Pop(val);
        CESMRICalProperty& prop = aComponent.AddPropertyL(KICalRRule,val); //Pass ownership

        CleanupStack::PopAndDestroy(ruleVal);

        }
    }

/**
 * Adds a date-time property to the passed component. Outputs in Local time if
 * possible, UTC otherwise.
 * @param aComponent component to add the property to.
 * @param aUtcTime time in UTC.
 * @param aProperty property to add.
 * @return property resulting from adding the date-time.
 * @internalTechnology
 */
CESMRICalProperty& CESMRAgnVersit2Exporter::AddDateTimePropertyL(CESMRICalComponent& aComponent,
    const TCalTime& aTime, const TDesC& aProperty) const
    {
    FUNC_LOG;

    // Check if we have a local time zone name - output in this timezone if we do
    if ((iTimezoneIndex != KErrNotFound) && (iTimezoneIndex < iTzNameArray.Count()))
        {
        CESMRICalValue* val = CESMRICalValue::NewLC();
        TTime timeCopy(aTime.TimeUtcL());
        ASSERT(iTimeZoneArray.Count() >= iTimezoneIndex + 1);
        iTimeZoneArray[iTimezoneIndex]->ConvertToLocalL(timeCopy);
        val->SetDateTimeL(timeCopy, CESMRICalValue::ESpecifiedTimeZone);
        CleanupStack::Pop(val);
        CESMRICalProperty& prop = aComponent.AddPropertyL(aProperty, val); //pass ownership of val
        prop.AddPropertyParamL(KICalTzid,*iTzNameArray[iTimezoneIndex]);

        return prop;
        }
    else if(aTime.TimeMode() == TCalTime::EFloating)
        {
        // If the CalTime was a floating time when it was created, output in floating time.
        return AddFloatingDateTimePropertyL(aComponent, aTime.TimeLocalL(), aProperty);
        }
    else
        {
        // Otherwise output in UTC
        return AddUtcDateTimePropertyL(aComponent, aTime.TimeUtcL(), aProperty);
        }
    }

/**
 * Adds a date-time property to the passed component. Forces UTC.
 * @param aComponent component to add the property to.
 * @param aUtcTime time in UTC.
 * @param aProperty property to add.
 * @return property resulting from adding the date-time.
 * @internalTechnology
 */
CESMRICalProperty& CESMRAgnVersit2Exporter::AddUtcDateTimePropertyL(CESMRICalComponent& aComponent,
    const TTime& aUtcTime, const TDesC& aProperty) const
    {
    FUNC_LOG;

    CESMRICalValue* val = CESMRICalValue::NewLC();
    val->SetDateTimeL(aUtcTime,CESMRICalValue::EUtcTime);
    CleanupStack::Pop(val);
    CESMRICalProperty& prop = aComponent.AddPropertyL(aProperty, val); //pass ownership of val

    return prop;
    }

/**
 * Adds a date-time property to the passed component. Forces floating.
 * @param aComponent component to add the property to.
 * @param aUtcTime time in floating time.
 * @param aProperty property to add.
 * @return property resulting from adding the date-time.
 * @internalTechnology
 */
CESMRICalProperty& CESMRAgnVersit2Exporter::AddFloatingDateTimePropertyL(CESMRICalComponent& aComponent,
    const TTime& aFloatingTime, const TDesC& aProperty) const
    {
    FUNC_LOG;

    CESMRICalValue* val = CESMRICalValue::NewLC();
    val->SetDateTimeL(aFloatingTime,CESMRICalValue::EFloatingTime);
    CleanupStack::Pop(val);
    CESMRICalProperty& prop = aComponent.AddPropertyL(aProperty, val); //pass ownership of val

    return prop;
    }


//
//     HELPER FUNCTIONS
//     ================
//

/**
 * Helper function to convert between a CCalEntry::TReplicationStatus stored in
 * agenda and a spec-compliant descriptor
 * @param aStatus agenda representation of CLASS
 * @return Descriptor representation of CLASS
 */
const TDesC& CESMRAgnVersit2Exporter::ClassStringL(const CCalEntry::TReplicationStatus aStatus)
    {
    FUNC_LOG;

    switch (aStatus)
        {
        case CCalEntry::EOpen :
            return KICalPublic;
        case CCalEntry::EPrivate :
            return KICalPrivate;
        case CCalEntry::ERestricted :
            return KICalConfidential;
        default :
            User::Leave(KErrCorrupt);
            break;
        }

    return KNullDesC;
    }

/**
 * Helper function to convert between a category stored in agenda and a
 * spec-compliant descriptor
 * @param aCategory category to extract data from
 * @return Descriptor representation of category
 * @internalTechnology
 */
const TDesC& CESMRAgnVersit2Exporter::CategoryStringL(const CCalCategory& aCategory) const
    {
    FUNC_LOG;

    switch (aCategory.Category())
        {
        case CCalCategory::ECalAppointment :
            return iStringProvider.StringL(EICalAppointment);
        case CCalCategory::ECalBusiness :
            return iStringProvider.StringL(EICalBusiness);
        case CCalCategory::ECalEducation :
            return iStringProvider.StringL(EICalEducation);
        case CCalCategory::ECalHoliday :
            return iStringProvider.StringL(EICalHoliday);
        case CCalCategory::ECalMeeting :
            return iStringProvider.StringL(EICalMeeting);
        case CCalCategory::ECalMiscellaneous :
            return iStringProvider.StringL(EICalMisc);
        case CCalCategory::ECalPersonal :
            return iStringProvider.StringL(EICalPersonal);
        case CCalCategory::ECalPhoneCall :
            return iStringProvider.StringL(EICalPhoneCall);
        case CCalCategory::ECalSickDay :
            return iStringProvider.StringL(EICalSick);
        case CCalCategory::ECalSpecialOccasion :
            return iStringProvider.StringL(EICalSpecial);
        case CCalCategory::ECalTravel :
            return iStringProvider.StringL(EICalTravel);
        case CCalCategory::ECalVacation :
            return iStringProvider.StringL(EICalVacation);
        case CCalCategory::ECalExtended :
            return aCategory.ExtendedCategoryName();
        default :
            User::Leave(KErrCorrupt);
            break;
        }

    return KNullDesC;
    }

/**
 * Converts from a TCalRole defined in CCalAttendee into a spec-compliant
 * descriptor.
 * @param aRole TCalRole enumeration to convert from.
 * @return Descriptor containing the equivalent text.
 * @internalTechnology
 */
const TDesC& CESMRAgnVersit2Exporter::RoleFromEnum(CCalAttendee::TCalRole aRole)
    {
    FUNC_LOG;

    switch (aRole)
        {
        case CCalAttendee::EReqParticipant :
            return KICalReqParticipant;
        case CCalAttendee::EOptParticipant :
            return KICalOptParticipant;
        case CCalAttendee::ENonParticipant :
            return KICalNonParticipant;
        case CCalAttendee::EChair :
            return KICalChair;
        default :
            break;
        }

    return KNullDesC;
    }

/**
 * Converts from a TCalStatus defined in CCalAttendee into a spec-compliant descriptor.
 * @param aStatus TCalStatus enumeration to convert from.
 * @return Descriptor containing the equivalent text.
 * @internalTechnology
 */
const TDesC& CESMRAgnVersit2Exporter::StatusFromEnum(CCalAttendee::TCalStatus aStatus)
    {
    FUNC_LOG;

    switch (aStatus)
        {
        case CCalAttendee::EAccepted :
            return KICalAccepted;
        case CCalAttendee::ETentative :
            return KICalTentative;
        case CCalAttendee::EConfirmed :
            return KICalConfirmed;
        case CCalAttendee::EDeclined :
            return KICalDeclined;
        case CCalAttendee::ECompleted :
            return KICalCompleted;
        case CCalAttendee::EDelegated :
            return KICalDelegated;
        case CCalAttendee::EInProcess :
            return KICalInProcess;
        case CCalAttendee::ENeedsAction :
        // Needs action is the default so fall through and set a null descriptor
        default :
            break;
        }

    return KNullDesC;
    }

/**
 * Helper function to convert from a TUint8 as defined in vtzrules.h into a
 * spec-compliant descriptor, such as MO, TU etc.
 * @param aDayInt Day number to be converted.
 * @return Descriptor containing the converted day.
 * @internalTechnology
 */
const TDesC& CESMRAgnVersit2Exporter::DayFromInt(TUint8 aDayInt)
    {
    FUNC_LOG;

    switch (aDayInt)
        {
        case 0 :
            return KICalMonday;
        case 1 :
            return KICalTuesday;
        case 2 :
            return KICalWednesday;
        case 3 :
            return KICalThursday;
        case 4 :
            return KICalFriday;
        case 5 :
            return KICalSaturday;
        case 6 :
            return KICalSunday;
        default :
            break;
        }

    return KNullDesC;
    }

/**
 * Helper function to convert from a TDay into a spec-compliant descriptor, such
 * as MO, TU etc.
 * @param aDay TDay to be converted.
 * @return Descriptor containing the converted day.
 * @internalTechnology
 */
const TDesC& CESMRAgnVersit2Exporter::DayFromTDay(TDay aDay)
    {
    FUNC_LOG;

    switch (aDay)
        {
        case EMonday :
            return KICalMonday;
        case ETuesday :
            return KICalTuesday;
        case EWednesday :
            return KICalWednesday;
        case EThursday :
            return KICalThursday;
        case EFriday :
            return KICalFriday;
        case ESaturday :
            return KICalSaturday;
        case ESunday :
            return KICalSunday;
        default :
            break;
        }

    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2Exporter::WeekNumFromDayOfMonth
// ---------------------------------------------------------------------------
//
TInt CESMRAgnVersit2Exporter::WeekNumFromDayOfMonth(TInt aDayOfMonth)
    {
    FUNC_LOG;

    return (aDayOfMonth / 7) + 1;
    }

//End of file.

