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
* Description:  Holds the definition of CESMRICalBase.
*
*
*/

#ifndef CESMRICALBASE_H
#define CESMRICALBASE_H

// System includes
#include <e32base.h>    // CBase

// Forward declarations.
class CESMRICalContentLineReader;
class CESMRICalContentLineWriter;
class CESMRICalProperty;
class CESMRICalComponent;
class CESMRICalValue;

/**
 *Abstract base class of all classes representing iCalendar components.
 *@publishedPartner
 */
class CESMRICalBase : public CBase
    {
public: // Enumeration.
    enum TICalComponentType
        {
        /** An invalid or X-custom component. */
        EICalInvalid,
        /** A VCALENDAR component.*/
        EICalCalendar,
        /** A VEVENT component.*/
        EICalEvent,
        /** A VTODO component.*/
        EICalTodo,
        /** A VJOURNAL component.*/
        EICalJournal,
        /** A VALARM component.*/
        EICalAlarm,
        /** A VFREEBUSY component.*/
        EICalFreeBusy,
        /** A VTIMEZONE component.*/
        EICalTimeZone,
        /** A STANDARD component.*/
        EICalStandard,
        /** A DAYLIGHT component.*/
        EICalDaylight
        };
public: // Enumeration.
    enum TICalMethod
        {
        /** No method specified. */
        EMethodNone,
        /** The RFC2445-defined method value 'Publish'. */
        EMethodPublish,
        /** The RFC2445-defined method value 'Request'. */
        EMethodRequest,
        /** The RFC2445-defined method value 'Reply'. */
        EMethodReply,
        /** The RFC2445-defined method value 'Add'. */
        EMethodAdd,
        /** The RFC2445-defined method value 'Cancel'. */
        EMethodCancel,
        /** The RFC2445-defined method value 'Refresh'. */
        EMethodRefresh,
        /** The RFC2445-defined method value 'Counter'. */
        EMethodCounter,
        /** The RFC2445-defined method value 'DeclineCounter'. */
        EMethodDeclineCounter
        };
protected:
    enum TICalComponentMethodBitMasks
        {
        EMaskEventNone              = 1 << 0,
        EMaskEventPublish           = 1 << 1,
        EMaskEventRequest           = 1 << 2,
        EMaskEventReply             = 1 << 3,
        EMaskEventAdd               = 1 << 4,
        EMaskEventCancel            = 1 << 5,
        EMaskEventRefresh           = 1 << 6,
        EMaskEventCounter           = 1 << 7,
        EMaskEventDeclineCounter    = 1 << 8,
        EMaskTodoNone               = 1 << 9,
        EMaskTodoPublish            = 1 << 10,
        EMaskTodoRequest            = 1 << 11,
        EMaskTodoReply              = 1 << 12,
        EMaskTodoAdd                = 1 << 13,
        EMaskTodoCancel             = 1 << 14,
        EMaskTodoRefresh            = 1 << 15,
        EMaskTodoCounter            = 1 << 16,
        EMaskTodoDeclineCounter     = 1 << 17,
        EMaskJournalNone            = 1 << 18,
        EMaskJournalPublish         = 1 << 19,
        EMaskJournalAdd             = 1 << 20,
        EMaskJournalCancel          = 1 << 21,
        EMaskFreeBusyNone           = 1 << 22,
        EMaskFreeBusyPublish        = 1 << 23,
        EMaskFreeBusyRequest        = 1 << 24,
        EMaskFreeBusyReply          = 1 << 25,
        EMaskAlarmAny               = 1 << 26,
        EMaskTimezoneAny            = 1 << 27,
        EMaskTimezoneIntervalAny    = 1 << 28
        };
    enum TICalComponentMethodFlags
        {
        EICalAttendeeFlags =        EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventRefresh       |
                                    EMaskEventCounter       |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoRefresh        |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalDtStampFlags =         EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventRefresh       |
                                    EMaskEventCounter       |
                                    EMaskEventDeclineCounter|
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoRefresh        |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyPublish    |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalDtStartFlags =         EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyPublish    |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalOrganizerFlags =       EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventRefresh       |
                                    EMaskEventCounter       |
                                    EMaskEventDeclineCounter|
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyPublish    |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalSummaryFlags =         EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCounter        |
                                    EMaskTodoCounter        |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalUIDFlags =             EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventRefresh       |
                                    EMaskEventCounter       |
                                    EMaskEventDeclineCounter|
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoRefresh        |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalRecurrenceIdFlags =    EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventCancel        |
                                    EMaskEventRefresh       |
                                    EMaskEventCounter       |
                                    EMaskEventDeclineCounter|
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoCancel         |
                                    EMaskTodoRefresh        |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalCancel,

        EICalSequenceFlags =        EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskEventDeclineCounter|
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalAttachFlags =          EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalCategoriesFlags =      EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalClassFlags =           EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalCommentFlags =         EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventRefresh       |
                                    EMaskEventCounter       |
                                    EMaskEventDeclineCounter|
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyPublish    |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalContactFlags =         EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyPublish    |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalCreatedFlags =         EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalDescriptionFlags =     EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalDtEndFlags =           EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskFreeBusyPublish    |
                                    EMaskFreeBusyRequest    |
                                    EMaskFreeBusyReply,

        EICalFreeBusyFlags =        EMaskFreeBusyPublish    |
                                    EMaskFreeBusyReply,

        EICalDueFlags =             EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter,

        EICalDurationFlags =        EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter,

        EICalExDateFlags =          EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalExRuleFlags =          EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalGeoFlags =             EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter,

        EICalLastModifiedFlags =    EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalLocationFlags =        EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter,

        EICalPercentCompleteFlags = EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter,

        EICalPriorityFlags =        EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel     |
                                    EMaskTodoDeclineCounter,

        EICalRDateFlags =           EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalRelatedToFlags =       EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalRequestStatusFlags =   EMaskEventRequest       |
                                    EMaskEventCounter       |
                                    EMaskEventDeclineCounter|
                                    EMaskTodoReply          |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskFreeBusyReply,

        EICalResourcesFlags =       EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter,

        EICalRRuleFlags =           EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalStatusFlags =          EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel,

        EICalTranspFlags =          EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter,

        EICalUrlFlags =             EMaskEventPublish       |
                                    EMaskEventRequest       |
                                    EMaskEventReply         |
                                    EMaskEventAdd           |
                                    EMaskEventCancel        |
                                    EMaskEventCounter       |
                                    EMaskTodoPublish        |
                                    EMaskTodoRequest        |
                                    EMaskTodoReply          |
                                    EMaskTodoAdd            |
                                    EMaskTodoCancel         |
                                    EMaskTodoCounter        |
                                    EMaskTodoDeclineCounter |
                                    EMaskJournalPublish     |
                                    EMaskJournalAdd         |
                                    EMaskJournalCancel      |
                                    EMaskFreeBusyPublish    |
                                    EMaskFreeBusyReply,




    //  EICalClassFlags         = 0b00000001111010111111010111111,

        };
public: // Destruction.
    ~CESMRICalBase();

public: // Methods.
    /**
     * Creates a new property with a value from the given parameters, adds it to
     * this object and returns a modifiable reference to it.
     * @param aName Name of the property to add.
     * @param aValue Value of the property to add.
     * @return A reference to a new property owned by this object.
     * @leave Leaves with KErrUnsupportedProperty if the given property is not valid.
     * for this component.
     * @publishedPartner
     */
    IMPORT_C CESMRICalProperty& AddPropertyL(const TDesC& aName, const TDesC& aValue);
    
    /**
     * Creates a new property with a value and adds it to this object, returning a
     * reference to it. Ownership of aValue is transferred and it will be deleted if
     * this function leaves.
     * @leave Leaves with KErrPropertyHasNoValue if aValue is Null.
     * @leave Leaves with KErrUnsupportedProperty if the given property is not valid.
     * @return A new property
     * @publishedPartner
     */
    IMPORT_C CESMRICalProperty& AddPropertyL(const TDesC& aName, CESMRICalValue* aValue);
    
    /**
     * Creates a new component, adds it to this object, and returns a modifiable
     * reference to it.
     * @param aType The type of component to be created.
     * @return A new component
     * @leave Leaves with KErrUnsupportedComponent if the given component is not a
     * valid subcomponent for this object.
     * @publishedPartner
     */
    IMPORT_C CESMRICalComponent& AddComponentL(TICalComponentType aType);

    /**
     * Access function for the component array.
     * @return The array of components as a constant reference.
     * @publishedPartner
     */
    IMPORT_C const RPointerArray<CESMRICalComponent>& Components() const;
    
    /**
     * Access function for the property array.
     * @return The array of properties as a constant reference.
     * @publishedPartner
     */
    IMPORT_C const RPointerArray<CESMRICalProperty>& Properties() const;

    /**
     * Returns the descriptor form of this component type.
     * @return The descriptor form of this component type.
     * @publishedPartner
     */
    IMPORT_C const TDesC& TypeStringL() const;
    
    /**
     * Access method returning the concrete type as an enumeration.
     * @return The type of the concrete derived class.
     * @publishedPartner
     */
    IMPORT_C TICalComponentType Type() const;

    /**
     * Checks for a component already existing in current object's sub-components
     * @param aType The type of the component to check.
     * @return ETrue if the property does exist, EFalse otherwise
     * @publishedPartner
     */
    IMPORT_C TBool ComponentExists(TICalComponentType aType) const;
    
    /**
     * Finds the first property with a particular name and returns a pointer to it.
     * Ownership is not passed out.
     * @param aName The name of the property to search for.
     * @return A pointer to the property, or NULL.
     * @publishedPartner
     */
    IMPORT_C const CESMRICalProperty* FindProperty(const TDesC& aName) const;

    /**
     * Takes a line reader and reads lines from it until the end of the component is
     * located. Any other END:, or an end of file, are treated as errors.
     * @param aReader The line reader to read from.
     * @leave Leaves with KErrCorrupt if the component is corrupt.
     * @internalTechnology
     */
    void InternalizeL(CESMRICalContentLineReader& aReader);
    
    /**
     * Takes a line writer and exports this component, including all owned properties
     * and sub components, to it.
     * @param aWriter the writer to export to.
     * @internalTechnology
     */
    void ExternalizeL(CESMRICalContentLineWriter& aWriter) const;

    /**
     * Converts between a TICalComponentType and the type as a descriptor.
     * @param aName The type as a descriptor.
     * @return The type as an enumeration.
     * @leave KErrCorrupt if this is not a valid type.
     * @internalTechnology
     */
    static TICalComponentType TypeFromNameL(const TDesC& aName);

protected:  // Construction.
    CESMRICalBase();

protected:  // Methods.
    /**
     * Converts between a TICalComponentType and the type as a descriptor.
     * @param aType The type as an enumeration.
     * @return The type as a descriptor.
     * @leave KErrCorrupt if this is not a valid type.
     * @internalTechnology
     */
    const TDesC& TypeStringL(TICalComponentType aType) const;
    
    TBool ValidateProperty(const TDesC& aName) const;

protected:  // must be implemented by subclasses 
    virtual TBool ValidatePropertyImpl(const TDesC& aName) const = 0;
    virtual TBool ValidateComponent(TICalComponentType aType) const = 0;

private:    // Implementation.
    CESMRICalComponent* CreateComponentL(TICalComponentType aType);
    CESMRICalProperty* CreatePropertyL(const TDesC& aName);

protected:  // Data.
    RPointerArray<CESMRICalComponent> iComponents;//own
    RPointerArray<CESMRICalProperty> iProperties;//own

    TICalComponentType iComponentType;//own
    TICalMethod iMethod;
    TUint32 iComponentMethodBitMask;
    };

#endif  // CESMRICALBASE_H

// End of File
