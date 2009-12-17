/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  esmrui commands definitions
*
*/

#ifndef ESMRCOMMANDS_H
#define ESMRCOMMANDS_H

#include <avkon.hrh>

/**
 * Enumeration for ES MR commands.
 * Define ES email command ids after enumaration value
 * EESMRFirstFSMailCommand.
 */
enum TESMRCommand
    {
    EESMRCmdUndefined = 0,               // Undefined command
    EESMRCmdAcceptMR = 1,                // Accept received MR
    EESMRCmdTentativeMR,                 // Tentatively accept MR
    EESMRCmdDeclineMR,                   // Decline MR
    EESMRCmdDeleteMR,                    // Deletes MR
    EESMRCmdRemoveFromCalendar,          // Removes cancelled MR from calendar
    EESMRCmdSendMR,                      // Send Meeting request
    EESMRCmdSendMRUpdate,                // Send MR update
    EESMRCmdSaveMR,                      // Save MR
    EESMRCmdEdit,                        // Edit MR
    EESMRCmdEditLocal,                   // Locally edit meeting request
    EESMRCmdReply,                       // Reply to one recipient
    EESMRCmdReplyAll,                    // Reply to all recipients
    EESMRCmdForwardAsMeeting,            // Forward MR as meeting
    EESMRCmdForwardAsMail,               // Forward MR as mail
    EESMRCmdViewCalendar,                // View calendar
    EESMRCmdAddOptAttendee,              // Adds optional attendee
    EESMRCmdPriorityHigh,                // Mark MR with 'high' priority
    EESMRCmdPriorityNormal,              // Mark MR with 'normal' priority
    EESMRCmdPriorityLow,                 // Mark MR with 'low' priority
    EESMRCmdPriorityOptions,             // Cascade menu command for priority options
    EESMRCmdAddAttendees,                // add attendees to meeting
    EESMRCmdShowAllAttendees,            // show all attendees of attendee viever field
    EESMRCmdForward,                     // Forward MR
    EESMRCmdForceExit,                   // Exit without saving
    EESMRCmdAlarmOn,
    EESMRCmdAlarmOff,
    EESMRCmdOpenPriorityQuery,
    EESMRCmdOpenSyncQuery,
    EESMRCmdOpenRecurrenceQuery,
    EESMRCmdOpenAlarmQuery,
    EESMRCmdOpenLocationContextMenu,
    EESMRCmdCheckEvent,
    EESMRCmdClipboardCopy,				// Copy to clipboard in viewer options menu
    EESMRCmdTodoMarkAsDone,			    // todoviewer options menu
    EESMRCmdTodoMarkAsNotDone,			// todoviewer options menu

    /** 
     * Tracking view commands
     */
    EESMRCmdViewTrack,					// Show tracking view
    EESMRCmdCall,						// Call the selected participant
    EESMRCmdBack,						// Back to previos dialog

    /**
     * Calendar Entry UI Specific commands
     */
    EESMRCmdCalEntryUIDelete,           // Delete calendar entry item
    EESMRCmdCalEntryUISend,             // Send calendar entry item
    EESMRCmdCalEntryUIEdit,             // Edit calendar entry item
    EESMRCmdCalEntryUIAddParticipants,  // Add participants to meeting item
    EESMRCmdCalEntryUISave,             // Sace calendar entry item

    /**
     * MRUI Attachment specific commands
     */
    EESMRAcceptWithoutAttachmentCheck,          // Accept MR without attachment check
    EESMRTentativeWithoutAttachmentCheck,       // Tentative MR without attachment check
    EESMRDeclineWithoutAttachmentCheck,         // Decline MR without attachment check
    EESMRRemoveFromDbWithoutAttachmentCheck,    // Remove from calendar without attachment check

    /**
     * Attendee field contact handling related
     */
    EESMRCmdAttendeeSoftkeySelect,
    EESMRCmdAttendeeSoftkeyCancel,
    EESMRCmdAttendeeInsertContact,

    EESMRCmdAttendeeRequiredEnabled,
    EESMRCmdAttendeeRequiredDisabled,
    EESMRCmdAttendeeOptionalEnabled,
    EESMRCmdAttendeeOptionalDisabled,

    /** Attachment field specific command */
    EESMRCmdMskOpenEmailAttachment,
    
    /**
     * MR Contact Action Plugin specific command
     * to notify that editor is inialized and ready
     * to be shown on screen
     * */
    EESMRCmdEditorInitializationComplete,    

    /**
     * Location integration
     */
    EESMRCmdAddLocation,
    EESMRCmdAssignFromMap,
    EESMRCmdUpdateFromMap,
    EESMRCmdSearchFromContacts,
    EESMRCmdSearchFromMap,
    EESMRCmdShowOnMap,
    EESMRCmdPreviousLocations,
    EESMRCmdLandmarks,
    EESMRCmdDisableWaypointIcon,
    EESMRCmdEnableWaypointIcon,
    
    /**
     * Middle softkey restoration 
     */
    EESMRCmdRestoreMiddleSoftKey,
    
    /**
     * Text size of field exceeded
     */
    EESMRCmdSizeExceeded,
    
    /**
     * FS Email specific command range
     */
    //<cmail> Moved bit forward to avoid conflict with avkon.hrh/ProcessCommandL constant range
    EESMRFirstFSMailCommand = 0x1388,
    //</cmail>
    EESMRCCmdMailLast =       0x2000,

    /**
     * SendUI
     * this value comes from and is shared with CalenGlobalData component
     */
    EESMRCmdSendAs = 30000,

    /**
     * Printing Framework
     */
    EESMRCmdPrint = 100155,
    EESMRCmdPrint_Reserved1,
    EESMRCmdPrint_Reserved2,
    EESMRCmdPrint_Reserved3,
    EESMRCmdPrint_Reserved4,


    /**
     * Action menu command items
     */
    EESMRCmdActionMenu = 999500,        // Action menu command
    EESMRCmdActionMenuFirst = 999501,   // First dynamic submenu command
    EESMRCmdActionMenuLast = 999600     // Last dynamic submenu command
    };

#endif // ESMRCOMMANDS_H

// EOF
