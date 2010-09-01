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
* Description:  Interface definition for ESMR calendar entry
*
*/


#ifndef MESMRICALVIEWER_H
#define MESMRICALVIEWER_H

// INCLUDES
//<cmail>
#include "esmricalviewerdefs.hrh"  // tesmrattendeestatus
//</cmail>

// FORWARD DECLARATIONS
class MESMRIcalViewerObserver;
class CFSMailMessage;

// CLASS DECLARATIONS
/**
 * MESMRIcalViewer defines interface for processing
 * FS email messages possibly containing meeting request.
 * ical viewer extension plugin implements this interface and
 * handles the involved command processing
 */
class MESMRIcalViewer
    {
    public: // Destruction
        /**
        * C++ destructror
        */
        virtual ~MESMRIcalViewer() { }

    public: // Interface

        /**
        * Tests, if message can be viewed.
        * @param aMessage Referece to FS email message.
        * @return ETrue, if message can be viewed
        */
        virtual TBool CanViewMessage(
              CFSMailMessage& aMessage ) = 0;

        /**
        * Launches viewer for the message asynchronously.
        *
        * @param aMessage Reference to FS email message.
        * @param aObserver Observer for this operation.
        * @exception KErrInUse If another asynchronous operation
        *            is already in progress.
        */
        virtual void ExecuteViewL(
              CFSMailMessage& aMessage,
              MESMRIcalViewerObserver& aObserver ) = 0;

        /**
        * Responses to meeting request asynchronously.
        * @param aAttendeeStatus Attendee's response
        * @param aMessage Reference to FS email message
        * @param aObserver Observer for this operation.
        * @exception KErrNotSupported if Meeting Requests are not enabled
        *            KErrInUse if request is already ongoing
        */
        virtual void ResponseToMeetingRequestL(
              TESMRAttendeeStatus aAttendeeStatus,
              CFSMailMessage& aMessage,
              MESMRIcalViewerObserver& aObserver ) = 0;

        /**
        * Removes meeting request from calendar asynchronously. Method resolves
        * meeting request information from the email message and
        * removes it from calendar.
        *
        * Meeting request needs to be cancellation meeting request.
        *
        * @param aMessage Reference to FS Email message
        * @param aObserver Observer for this operation.
        * @exception KErrInUse If another asynchronous operation
        *            is already in progress.
        */
        virtual void RemoveMeetingRequestFromCalendarL(
              CFSMailMessage& aMessage,
              MESMRIcalViewerObserver& aObserver ) = 0;

        /**
        * Resolves meeting request method asynchronously. Method extracts
        * meeting request information from the email message and fetches the MR
        * method information.
        *
        * @param aMessage Reference to mail message
        * @param aObserver Observer for this operation.
        *                  TESMRMeetingRequestMethod MR method information will
        *                  be returned through the observer.
        * @exception KErrInUse If another asynchronous operation
        *            is already in progress.
        */
        virtual void ResolveMeetingRequestMethodL(
              CFSMailMessage& aMessage,
              MESMRIcalViewerObserver& aObserver ) = 0;

        /**
        * Resolves meeting request method synchronously. Method locates the MRINFO
        * data from the mail message and tries to find out current meeting request
        * method. If message does not contain MRINFO data, MR method cannot be
        * solved out.
        *
        * @param aMessage Reference to mail message.
        * @return TESMRMeetingRequestMethod type
        */
        virtual TESMRMeetingRequestMethod ResolveMeetingRequestMethodL(
              CFSMailMessage& aMessage ) = 0;

        /**
        * Cancels any outstanding asynchronous request.
        */
        virtual void CancelOperation() = 0;

        /**
        * Returns an extension point for this interface or NULL.
        * @param aExtensionUid Uid of extension.
        * @return Extension point or NULL.
        */
        virtual TAny* MRIcalViewerExtension(
              TUid /*aExtensionUid*/) { return NULL; }
    };

#endif // MESMRICALVIEWER_H

// EOF
