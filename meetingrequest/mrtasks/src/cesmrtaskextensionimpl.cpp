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
* Description:  ESMR task extension implementation
*
*/

#include "emailtrace.h"
#include "cesmrtaskextensionimpl.h"

#include <caluser.h>
#include "esmrinternaluid.h"
//<cmail>
#include "cesmrpolicymanager.h"
//</cmail>
#include "cesmrentryprocessor.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrtaskfactory.h"
#include "mesmrtask.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::CESMRTaskExtenstionImpl
// ---------------------------------------------------------------------------
//
CESMRTaskExtenstionImpl::CESMRTaskExtenstionImpl(
        MESMRCalDbMgr& aCalDBMgr,
        CMRMailboxUtils& aMRMailboxUtils,
        CESMRPolicyManager& aPolicyManager,
        CESMREntryProcessor& aEntryProcessor,
        MESMRTaskFactory& aTaskFactory )
:       iCalDBMgr( aCalDBMgr ),
        iMRMailboxUtils( aMRMailboxUtils),
        iPolicyManager( aPolicyManager ),
        iEntryProcessor( aEntryProcessor),
        iTaskFactory( aTaskFactory )
    {
    FUNC_LOG;
    //do nothing
    }


// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::~CESMRTaskExtenstionImpl
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRTaskExtenstionImpl::~CESMRTaskExtenstionImpl()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRTaskExtenstionImpl* CESMRTaskExtenstionImpl::NewL(
        MESMRCalDbMgr& aCalDBMgr,
        CMRMailboxUtils& aMRMailboxUtils,
        CESMRPolicyManager& aPolicyManager,
        CESMREntryProcessor& aEntryProcessor,
        MESMRTaskFactory& aTaskFactory )
    {
    FUNC_LOG;
    CESMRTaskExtenstionImpl* self =
        new (ELeave) CESMRTaskExtenstionImpl(
                        aCalDBMgr,
                        aMRMailboxUtils,
                        aPolicyManager,
                        aEntryProcessor,
                        aTaskFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::SendAndStoreResponseL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::SendAndStoreResponseL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    // First do possible command conversion
    if ( EESMRAcceptWithoutAttachmentCheck == aCommand  )
        {
        aCommand = EESMRCmdAcceptMR;
        }
    else if ( EESMRTentativeWithoutAttachmentCheck == aCommand )
        {
        aCommand = EESMRCmdTentativeMR;
        }
    else if ( EESMRDeclineWithoutAttachmentCheck == aCommand )
        {
        aCommand = EESMRCmdDeclineMR;
        }

    // Check that command is valid
    if ( !( EESMRCmdAcceptMR == aCommand ||
            EESMRCmdTentativeMR == aCommand ||
            EESMRCmdDeclineMR == aCommand ) )
        {
        // Invalid command --> Leave
        User::Leave( KErrArgument );
        }

    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::SendAndStoreMRL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::SendAndStoreMRL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    // Check that command is valid
    if ( !( EESMRCmdSendMR == aCommand ||
            EESMRCmdSendMRUpdate == aCommand) )
        {
        // Invalid command --> Leave
        User::Leave( KErrArgument );
        }

    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::SendAndStoreMRL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::DeleteMRFromLocalDBL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    // Check that command is valid
    if ( EESMRCmdDeleteMR != aCommand )
        {
        // Invalid command --> Leave
        User::Leave( KErrArgument );
        }

    // Create and execute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::SendAndStoreMRL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::DeleteAndSendMRL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    // Check that command is valid
    if ( EESMRCmdDeleteMR == aCommand ||
         EESMRCmdRemoveFromCalendar == aCommand ||
         EESMRCmdMailDelete == aCommand )
        {
        // Create And execute task
        CreateAndExecuteTaskL( aCommand, aEntry );
        }
    else
        {
        // Invalid command --> Leave
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::StoreMRToLocalDBL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::StoreMRToLocalDBL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    if ( EESMRCmdSaveMR != aCommand )
        {
        // Invalid command --> Leave
        User::Leave( KErrArgument );
        }
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::ForwardMRAsEmailL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::ForwardMRAsEmailL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    // Check that command is valid
    if ( EESMRCmdForwardAsMail != aCommand )
        {
        // Invalid command --> Leave
        User::Leave( KErrArgument );
        }

    // Create And exexute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::ReplyAsEmailL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::ReplyAsEmailL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    // Check that command is valid
    if ( EESMRCmdReply != aCommand  && EESMRCmdReplyAll != aCommand)
        {
        // Invalid command --> Leave
        User::Leave( KErrArgument );
        }

    // Create And exexute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::CreateAndExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::CreateAndExecuteTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    MESMRTask* task = iTaskFactory.CreateTaskL( aCommand, aEntry );

    if (task )
        {
        CleanupDeletePushL( task );
        task->ExecuteTaskL();
        CleanupStack::PopAndDestroy(); // task;
        }
    else
        {
        // Task cannot be constructed --> Leave

        User::Leave( KErrNotSupported );
        }
    }

//EOF

