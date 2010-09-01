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
* Description:  Definitions for project mrui
*
*/


#ifndef ESMRDEF_H
#define ESMRDEF_H

//<cmail>
#include "esmrcommands.h"

/**
 * Enumeration for calendar entry types
 */

enum TESMRCalendarEventType
    {
    EESMREventTypeNone = -1,
    EESMREventTypeMeetingRequest = 0,   // Meeting request item
    EESMREventTypeAppt,                 // Appointment calendar item
    EESMREventTypeEEvent,               // Event calendar item
    EESMREventTypeEAnniv,               // Anniversary calendar item
    EESMREventTypeETodo,                // TO-DO calendar item
    EESMREventTypeEReminder             // Reminder calendar item   
    };

/**
 * Enumeration for response message type
 */
enum TESMRResponseType
    {
    EESMRResponseDontSend = 0,  // Don't send response message
    EESMRResponsePlain,         // Response sent without message
    EESMRResponseMessage        // Response sent with message
    };

/**
 * Enumeration for MRUI  query mode
 */
enum TESMRQueryMode
    {
    EESMRQueryModeNo,           // Do not perform queries
    EESMRQueryModeYes           // Perform queries
    };

/**
 * Definitions for ES MR policies
 */
enum TESMRPolicyID
    {
    // PolicyId unspecified
    EESMRPolicyUnspecified = 0,
    // Receive MR from mail
    EESMRPolicyReceiveMRFromEmail,
    // Organizer edit MR from calendar
    EESMRPolicyOrganizerEditMRFromCal,
    // Organizer view MR from calendar
    EESMRPolicyOrganizerViewMRFromCal,
    // Attendee  view MR from calendar
    EESMRPolicyAttendeeViewMRFromCal,
    // Forward MR from calendar as MR
    EESMRPolicyForwardAsMeetingRequestFromCal,
    // Forward MR from mail as MR
    EESMRPolicyForwardAsMeetingRequestFromMail,
    // Edit locally from calendar
    EESMRPolicyAttendeeEditMRFromCal,
    // Meeting editor policy
    EESMRPolicyMeetingEditor,
    // Meeting viewer policy
    EESMRPolicyMeetingViewer,
    // TO-DO editor policy
    EESMRPolicyTodoEditor,
    // TO-DO viewer policy
    EESMRPolicyTodoViewer,
    // Memo editor policy
    EESMRPolicyMemoEditor,
    // Memo viewer policy
    EESMRPolicyMemoViewer,
    // Anniversary editor policy
    EESMRPolicyAnniversaryEditor,
    // Anniversary viewer policy
    EESMRPolicyAnniversaryViewer,
    EESMRPolicyAttendeeViewAllDayEventMRFromCal,
    // Attendee - view allday MR from email app
    EESMRPolicyAttendeeViewAllDayEventMRFromEmail,
    // Attendee - view allday MR from calendar app
    EESMRPolicyOrganizerViewAllDayEventMRFromCal,
    // Organzier  - view allday MR from email app
    EESMRPolicyOrganizerViewAllDayEventMRFromEmail,
    // Tracking view policy
    EESMRPolicyTrackingView,
    // Organizer view MR from email
    EESMRPolicyOrganizerViewMREmail
    // Add new policies here
    };

/**
 * Definition for MRUI MR view mode
 */
enum TESMRViewMode
    {
    EESMRViewUndef = 1,         // MR view mode not specified
    EESMRViewMR,                // MR view mode (read-only)
    EESMREditMR,                // MR edit mode
    EESMRForwardMR,             // MR forward mode
    EESMRViewAllDayMR,           // MR view mode for all day events (read-only)
    EESMRTrackingViewMR         // MR Tracking view mode
    };

/**
 * Definition for ES MR fields
 */
enum TESMREntryFieldId
    {
    // Response area
    EESMRFieldResponseArea = 1,
    // Response ready area, this is created always
    // when EESMRFieldResponseArea is given by policy
    EESMRFieldResponseAreaWithOneItem,
    // Organizer field
    EESMRFieldOrganizer,
    // Attendee & opt attendee label
    EESMRFieldAttendeeLabel,
    // Attendee field
    EESMRFieldAttendee,
    // Optional attendee field
    EESMRFieldOptAttendee,
    // Meeting time
    EESMRFieldMeetingTime,
    // Start date
    EESMRFieldStartDate,
    // End date
    EESMRFieldStopDate,
    // Location
    EESMRFieldLocation,
    // Alarm
    EESMRFieldAlarm,
    // Alarm date field
    EESMRFieldAlarmDate,
    // Subject field
    EESMRFieldSubject,
    // Description
    EESMRFieldDescription,
    // Recurrence
    EESMRFieldRecurrence,
    // Recurrence until-date field
    EESMRFieldRecurrenceDate,
    // Priority
    EESMRFieldPriority,
    // All day event
    EESMRFieldAllDayEvent,
    // Alarm (on/off) for all day event
    EESMRFieldAlarmOnOff,
    // Time for all day event alarm
    EESMRFieldAlarmTime,
    // Synchronization
    EESMRFieldSync,
    // Occasion
    EESMRFieldOccasion,
    // Date field
    EESMRFieldDate,
    // Basic viewer field
    EESMRFieldViewerBasic,
    // Additional Subject field for viewer
    EESMRFieldDetailedSubject,
    // Tracking required attendeee field
    EESMRFieldReqTrack,
    // Tracking Optional Attendee field
    EESMRFieldOptTrack,
    // Tracking status
    EESMRTrackStatus,
    // Attachment field
    EESMRFieldAttachments,
    // Event type field
    EESMRFieldUnifiedEditor,
    // Calendar name field
    EESMRFieldCalendarName,
    // Viewer attachment field
    EESMRFieldViewerAttachments
    };

/**
 * Definition for MRUI GUI field type
 */
enum TESMRFieldType
    {
    EESMRFieldTypeDefault,      // Default field
    EESMRFieldTypeAdditional,    // Additional field
    EESMRFieldTypeDisabled      // Disabled field
    };

/**
 * Definition for MRUI GUI field mode
 */
enum TESMRFieldMode
    {
    EESMRFieldModeView,     // Viewable (read only) field
    EESMRFieldModeEdit      // Editable field
    };

/**
 * Definition for meeting request user role
 */
enum TESMRRole
    {
    EESMRRoleUndef            = 0,     // Undefined
    EESMRRoleOrganizer        = 0x01,  // Organizer
    EESMRRoleRequiredAttendee = 0x02,  // Participant
    EESMRRoleOptionalAttendee = 0x04,  // Participant
    EESMRRoleNonParticipant   = 0x08   // Non-Participant
    };

// Flag definitions for for meeting request user role
// These should be used in resource files
#define KEESMRFlagOrganizer 0x01
#define KEESMRFlagRequiredAttendee 0x02
#define KEESMRFlagOptionalAttendee 0x04
#define KEESMRFlagNonParticipant 0x08

/**
 * Definition for applications using MRUI subsystem
 */
enum TESMRUsingApp
    {
    EESMRAppUndef    = 0x00, // undefined
    EESMRAppCalendar = 0x01, // ES Calendar
    EESMRAppESEmail  = 0x02  // ES Email
    };

// Resource definitions for MRUI client application
// These should be used in resource files
#define KEESMRFlagAppUndef 0x00
#define KEESMRFlagAppEsCalendar 0x01
#define KEESMRFlagAppEsEmail 0x02

/**
 * Meeting reqeust mode definition
 */
enum TESMREntryMode
    {
    EESMREntryNoRecurrent,          // No recurrent event
    EESMREntryRecurrent             // Recurrent event
    };

/**
 * Recurrence definition
 */
enum TESMRRecurrenceValue
    {
    ERecurrenceNot = 0,             // No recurrence
    ERecurrenceDaily,               // Daily recurrence
    ERecurrenceWeekly,              // Weekly recurrence
    ERecurrenceEverySecondWeek,     // Bi-weekly recurrence
    ERecurrenceMonthly,             // Monthly recurrence
    ERecurrenceYearly,              // Yearly recurrence
    ERecurrenceUnknown              // Unknown recurrence
    };

/**
 * Definition for relative alarm values.
 */
enum TESMRAlarmValue
    {
    EAlarmOff = 0,          // No alarm
    EAlarmZeroMinutes,      // 0 minutes before
    EAlarmFiveMinutes,      // 5 minutes before
    EAlarmFifteenMinutes,   // 15 minutes before
    EAlarmThirtyMinutes,    // 30 minutes before
    EAlarmOneHour,          // 1 hour before
    EAlarmTwoHours,         // 2 hours before
    EAlarmThreeHours,       // 3 hours before
    EAlarmFourHours,        // 4 hours before
    EAlarmFiveHours,        // 5 hours before
    EAlarmSixHours,         // 6 hours before
    EAlarmSevenHours,       // 7 hours before
    EAlarmEightHours,       // 8 hours before
    EAlarmTwelveHours,      // 12 hours before
    EAlarmOneDay,           // 1 day before
    EAlarmTwoDays,          // 2 days before
    EAlarmOneWeek           // 1 week before
    };

/**
 * Definition for synchronization values
 */
enum TESMRSyncValue
   {
   ESyncNone,           // No synchronization
   ESyncPrivate,        // Private synchronization
   ESyncPublic          // Public synchronization
   };

/** 
 * Enumeration for 'Send update' query results
 */
enum TESMRSendUpdateQueryResponeStatus
    {
    EESMRSendToAllParticipants = 0,         // Send to all participants
    EESMRSendToAddedOrRemovedParticipants   // Send to added / removed
    };

/**
 * Enumeration for 'Edit recurrent MR' query results
 */
enum TESMRThisOccurenceOrSeriesQuery
    {
    EESMRThisOccurence = 0,             // Only this MR entry
    EESMRSeries                         // All MR entries
    };

#ifndef FSCALENCOMMON_HRH
/** 
 * Priorities for meeting request entries.
 */
enum TFSCalenMRPriority
    {
    EFSCalenMRPriorityUnknown = 0,      // Unknown priority
    EFSCalenMRPriorityHigh = 1,         // High priority
    EFSCalenMRPriorityNormal = 5,       // Normal priority
    EFSCalenMRPriorityLow = 9           // Low priority
    };

/** 
 * Priorities for to-do items
 */
enum TFSCalenTodoPriority
    {
    EFSCalenTodoPriorityHigh = 1,
    EFSCalenTodoPriorityNormal,
    EFSCalenTodoPriorityLow
    };

#endif // FSCALENCOMMON_HRH

/**
 * Enumeration for email plug-in codes
 */
enum TESMRMailPlugin
    {
    // Unknow plug-in
    EESMRUnknownPlugin,
    // ActiceSync plug-in
    EESMRActiveSync,
    // IntelliSync plug-in
    EESMRIntelliSync,
    // IMAP-POP plug-in
    EESMRImapPop
    };

/**
 * BC Plugin event types for resource file
 */
#define KMRTypeMeetingRequest 0x01
#define KMRTypeMeeting        0x02
#define KMRTypeMemo           0x04
#define KMRTypeAnniversary    0x08
#define KMRTypeToDo           0x10

/**
 * BC Plugin event types
 */
enum TBCPluginEventType
    {
    EMRTypeMeetingRequest = 0,
    EMRTypeMeeting        = 1,
    EMRTypeMemo           = 2,
    EMRTypeAnniversary    = 3,
    EMRTypeToDo           = 4,
    EMRTypeNone           = 1024
    };

/**
 * Defines processor mode
 */
enum TMRProcessorMode
    {
    EMRProcessorModeNone = 0,
    EMRProcessorModeView = 1,
    EMRProcessorModeEdit = 2,
    EMRProcessorModeForward = 3,
    EMRProcessorModeTrack = 4
    };

// ESMRICALVIEWER definitions
#include "esmricalviewerdefs.hrh"
//</cmail>

#endif // ESMRDEF_H

// EOF
