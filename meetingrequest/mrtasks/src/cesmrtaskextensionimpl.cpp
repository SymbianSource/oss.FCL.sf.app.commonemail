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
#include "mesmrmeetingrequestentry.h"
#include "cesmrtaskfactory.h"
#include "mesmrtask.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::CESMRTaskExtenstionImpl
// ---------------------------------------------------------------------------
//
CESMRTaskExtenstionImpl::CESMRTaskExtenstionImpl( MESMRTaskFactory& aTaskFactory )
    : iTaskFactory( aTaskFactory )
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
        MESMRTaskFactory& aTaskFactory )
    {
    FUNC_LOG;
    CESMRTaskExtenstionImpl* self =
        new (ELeave) CESMRTaskExtenstionImpl( aTaskFactory );
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
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // First do possible command conversion

    switch ( aCommand )
        {
        case EESMRAcceptWithoutAttachmentCheck:
            {
            aCommand = EESMRCmdAcceptMR;
            break;
            }
        case EESMRTentativeWithoutAttachmentCheck:
            {
            aCommand = EESMRCmdTentativeMR;
            break;
            }
        case EESMRDeclineWithoutAttachmentCheck:
            {
            aCommand = EESMRCmdDeclineMR;
            break;
            }
        default:
            {
            break;
            }
        }
    
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::SendAndStoreMRL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::SendAndStoreMRL(
        TESMRCommand aCommand,
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::SendAndStoreMRL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::DeleteAndSendMRL(
        TESMRCommand aCommand,
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // Create And execute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::StoreEntryToLocalDBL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::StoreEntryToLocalDBL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // Create And exexute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }


// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::ForwardMRAsEmailL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::ForwardMRAsEmailL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // Create And exexute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::ReplyAsEmailL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::ReplyAsEmailL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // Create And exexute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::DeleteEntryFromLocalDBL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::DeleteEntryFromLocalDBL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // Create And exexute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::MarkTodoAsDoneL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::MarkTodoAsDoneL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // Create And execute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::MarkTodoAsNotDoneL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::MarkTodoAsNotDoneL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    
    // Create And exexute task
    CreateAndExecuteTaskL( aCommand, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::MoveEntryToCurrentDBL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::MoveEntryToCurrentDBL(
            TESMRCommand /*aCommand*/,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
        
    // Create And exexute task
    CreateAndExecuteTaskL( EESMRCmdCalendarChange, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRTaskExtenstionImpl::CreateAndExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRTaskExtenstionImpl::CreateAndExecuteTaskL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRTask* task = iTaskFactory.CreateTaskL( aCommand, aEntry );

    if (task )
        {
        CleanupDeletePushL( task );
        task->ExecuteTaskL();
        CleanupStack::PopAndDestroy( task ); // task;
        }
    else
        {
        // Task cannot be constructed --> Leave

        User::Leave( KErrNotSupported );
        }
    }

//EOF

