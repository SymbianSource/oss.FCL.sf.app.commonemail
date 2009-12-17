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
* Description:  ESMRIcalViewer utility methods
*
*/


#ifndef ESMRICALVIEWERUTILS_H
#define ESMRICALVIEWERUTILS_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "tesmrinputparams.h"

class CFSMailMessagePart;
class CCalEntry;

/**
 * ESMRIcalViewerUtils contains static utilies for use of the
 * ical viewer ecom plugin. Not for use outside this ecom implementation.
 */
NONSHARABLE_CLASS( ESMRIcalViewerUtils ) :
        public CBase
    {
public: // Construction and destruction

    /**
     * Converts MRUI callback command to
     * correct email operation.
     *
     * @param aCommandId MRUI command id.
     * @return Email command id.
     */
    static TESMRIcalViewerOperationType EmailOperation(
            TInt aCommandId );

    /**
     * Tests if entry contains attachments or not.
     * @param aEntry Reference to meeting request entry.
     * @return ETrue if entry contains attachments
     */
    static TBool ContainsAttachments(
            TESMRInputParams& aParams );

    /**
     * Tests if command is asynchronous or not.
     * @param aCommand Command to be evaluated.
     * @return ETrue if command is asynchronous
     */
    static TBool IsAsyncEmailCommand(
            TESMRIcalViewerOperationType aCommand );

    /**
     * Resolves correct MRUI MR response command without performing
     * attachment check.
     * @param aCommand MRUI command.
     * @return TESMRCommand for correct attachement command ordering
     */
    static TESMRCommand MRResponseCommandWithoutAttachmentCheck(
            TInt aCommand );

    /**
     * Locates child message part. Ownership is transferred to caller.
     * If child part cannot be located, NULL pointer is returned.
     * After return, cleanupstack always contains a pointer.
     *
     * @param aParentPart Reference to parent mail part.
     * @param aMimeType Defines the mime type to be located
     * @return part of mail message based on the mime type and parentpart
     */
    static CFSMailMessagePart* LocateChildPartLC(
            CFSMailMessagePart& aParentPart,
            const TDesC& aMimeType );

    /**
     * Locates calendar (content type = TEXT/CALENDAR) from message
     * If not found, NULL pointer is returned.
     * Function tries first to find a "multipart/alternative" section and a "text/calendar"
     * item from there. If not found it is looked from root of the message.
     * @param aMessage contains the message
     * @return message part being of correct content type
     */
    static CFSMailMessagePart* LocateCalendarPartL( CFSMailMessage& aMessage );

    /**
     * Cleanup operations for RPointerArray.
     *
     * @param aArray Pointer to RPointerArray.
     */
    static void MessagePartPointerArrayCleanup( TAny* aArray );

    /**
     * Tests if user responsed to MR and returns the correct attendee value.
     * @param aEntry calendar entry of the meeting request to check attendee status
     * @return Attendee response status
     */
    static TESMRAttendeeStatus UserResponsedToMRL(
            CCalEntry& aEntry );

    /**
     * Resolves the mailbox owner's role in meeting request
     * @param aEntry Meeting request entry
     * @param aMailClient Reference to mail client
     * @param aMessage Reference to mail message
     * @return role of the meeting request
     */
    static TESMRRole MailboxOwnerRoleL(
            CCalEntry& aEntry,
            CFSMailClient& aMailClient,
            CFSMailMessage& aMessage );

    /**
     * Resolves the mailbox owner's role is attendee in meeting request
     * @param aEntry Meeting request entry
     * @param aMailClient Reference to mail client
     * @param aMessage Reference to mail message
     * @return True if mailbox owner is an attendee in the meeting request
     */
    static TBool IsMailboxOwnerAttendeeL(
            CCalEntry& aEntry,
            CFSMailClient& aMailClient,
            CFSMailMessage& aMessage );
    };

#endif // ESMRICALVIEWERUTILS_H

// EOF
