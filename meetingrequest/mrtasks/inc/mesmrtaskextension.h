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
* Description:  Interface definition for ESMR task factory
*
*/


#ifndef MESMRTASKEXTENSION_H
#define MESMRTASKEXTENSION_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

class MESMRMeetingRequestEntry;

/**
 * MESMRTaskExtension is extension API for CMRUtils.
 * MESMRTaskExtension defines methods for executing ES MR MR related
 * functionality.
 *
 * Usage (assuming you have CMRUtils* iUtils):
 * MESMRTaskExtension* ext = ExtensionApiL<MESMRTaskExtension>( iUtils );
 *
 * @lib esmrtasks.lib
 */
class MESMRTaskExtension
    {
public: // Inline implementation
    /**
     * Fetches task extension uid.
     * @return task extension uid
     */
    inline static TUid ExtensionUid();

public: // Destruction
    /**
     * Virtual C++ destructor.
     */
    virtual ~MESMRTaskExtension() { }

public: // Interface
    /**
     * Sends and stores MR response. Command needs to be one of the
     * following:
     *  - EESMRCmdAcceptMR
     *  - EESMRCmdTentativeMR
     *  - EESMRCmdDeclineMR
     *
     * Phone owner's attendee status needs to match MR response command.
     *
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */
    virtual void SendAndStoreResponseL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;

    /**
     * Sends and stores MR . Command needs to be one of the
     * following:
     *  - EESMRCmdSendMR
     *  - EESMRCmdSendMRUpdate
     *
     * Phone owner needs to be meeting organizer.
     *
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */
    virtual void SendAndStoreMRL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;

    /**
     * Delete MR from local calendar DB. Command needs to be one of the
     * following:
     *  - EESMRCmdDeleteMR
     *
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */
    virtual void DeleteMRFromLocalDBL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;

    /**
     * Deletes MR from local calendar DB. Method also sends either cancellation
     * or decline message.
     *
     * Command needs to be one of the following:
     *  - EESMRCmdDeleteMR
     *
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */
    virtual void DeleteAndSendMRL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;

    /**
     * Stores MR to local calendar DB.
     *
     * Command needs to be one of the following:
     *  - EESMRCmdSaveMR
     *
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */
    virtual void StoreMRToLocalDBL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;

    /**
     * Forwards MR as email. Email editor is launched with pre-filled
     * email message.
     *
     * Command needs to be one of the following:
     *  - EESMRCmdForwardAsMail
     *
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */
     virtual void ForwardMRAsEmailL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;

     /**
      * Creates email reply for meeting request.
      * Email editor is launched with pre-filled
      * email message.
      *
      * Command needs to be one of the following:
      *  - EESMRCmdReply or EESMRCmdReplyAll
      *
      * @param aCommand Command to be executed
      * @param aEntry Reference to entry
      * @see TESMRCommand
      */
     virtual void ReplyAsEmailL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;
    };

#include "mesmrtaskextension.inl"

#endif  // MESMRTASKEXTENSION_H
