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

class MESMRCalEntry;

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
            MESMRCalEntry& aEntry ) = 0;

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
            MESMRCalEntry& aEntry ) = 0;

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
            MESMRCalEntry& aEntry ) = 0;

    /**
     * Stores Cal Event to local calendar DB.
     *
     * Command needs to be one of the following:
     *  - EESMRCmdSaveMR
     *
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */
    virtual void StoreEntryToLocalDBL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry ) = 0;
                        
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
            MESMRCalEntry& aEntry ) = 0;

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
            MESMRCalEntry& aEntry ) = 0;
     
     /**
      * Deletes calendar event from local calendar DB.
      *
      * Command needs to be one of the following:
      *  - EESMRCmdCalEntryUIDelete
      *  - EESMRCmdDeleteMR
      *  
      * @param aCommand Command to be executed
      * @param aEntry Reference to entry
      * @see TESMRCommand
      */      
     virtual void DeleteEntryFromLocalDBL(
             TESMRCommand aCommand,
             MESMRCalEntry& aEntry ) = 0;
     
     /**
      * Marks to-do event as done.
      *
      * Command needs to be one of the following:
      *  - EESMRCmdTodoMarkAsDone
      *
      * @param aCommand Command to be executed
      * @param aEntry Reference to entry
      * @see TESMRCommand
      */      
     virtual void MarkTodoAsDoneL(
             TESMRCommand aCommand,
             MESMRCalEntry& aEntry ) = 0;
     
     /**
      * Marks to-do event as not done.
      *
      * Command needs to be one of the following:
      *  - EESMRCmdTodoMarkAsNotDone
      *
      * @param aCommand Command to be executed
      * @param aEntry Reference to entry
      * @see TESMRCommand
      */      
     virtual void MarkTodoAsNotDoneL(
             TESMRCommand aCommand,
             MESMRCalEntry& aEntry ) = 0 ;
     
    /**
     * Moves entry to current database
     * 
     * Command needs to be one of following:
     *  - EESMRCmdSaveMR
     *  - EESMRCmdCalEntryUISave
     * @param aCommand Command to be executed
     * @param aEntry Reference to entry
     * @see TESMRCommand
     */      
    virtual void MoveEntryToCurrentDBL(
         TESMRCommand aCommand,
         MESMRCalEntry& aEntry ) = 0 ;
    };

#include "mesmrtaskextension.inl"

#endif  // MESMRTASKEXTENSION_H
