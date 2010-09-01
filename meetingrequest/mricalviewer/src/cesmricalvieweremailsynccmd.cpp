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
* Description:  Resolve meeting request method sync command
*
*/

#include "emailtrace.h"
#include "cesmricalvieweremailsynccmd.h"
//<cmail>
#include "esmricalvieweropcodes.hrh"
//</cmail>
#include <calentry.h>

/// Unnamed namespace for local definitions
namespace {
#ifdef _DEBUG

// Panic literal
_LIT( KSMRIcalViewerEmailSyncCmd, "SMRIcalViewerEmailSyncCmd" );

/** Panic codes */
enum TSMRIcalViewerEmailSyncCmdPanic
    {
    EInvalidCommandId = 0 //Invalid command id
    };

void Panic( TSMRIcalViewerEmailSyncCmdPanic aPanic )
    {
    User::Panic( KSMRIcalViewerEmailSyncCmd, aPanic );
    }

#endif // _DEBUG

}  // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailSyncCmd::CESMRIcalViewerEmailSyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerEmailSyncCmd::CESMRIcalViewerEmailSyncCmd(
        TESMRIcalViewerOperationType aOperation,
        CCalSession& aCalSession,
        MESMRIcalViewerCallback& aCallback )
:   CESMRIcalViewerCommandBase(
        EESMREmailCommand,
        aCalSession ),
        iCallback( aCallback),
        iOperation( aOperation )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailSyncCmd::~CESMRIcalViewerEmailSyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerEmailSyncCmd::~CESMRIcalViewerEmailSyncCmd()
    {
    FUNC_LOG;
    CancelCommand();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerEmailSyncCmd* CESMRIcalViewerEmailSyncCmd::NewL(
        TESMRIcalViewerOperationType aOperation,
        CCalSession& aCalSession,
        MESMRIcalViewerCallback& aCallback )
    {
    FUNC_LOG;
    CESMRIcalViewerEmailSyncCmd* self =
            new (ELeave) CESMRIcalViewerEmailSyncCmd(
                    aOperation,
                    aCalSession,
                    aCallback );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailSyncCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailSyncCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailSyncCmd::ExecuteCommandL(
        CFSMailMessage& aMessage,
        MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    SetObserver( &aObserver );
    SetMessage( &aMessage );

    switch ( iOperation )
        {
        case EESMRCmdMailFlagMessage://fallthrough
        case EESMRCmdMailMarkUnread://fallthrough
        case EESMRCmdMailMarkRead:
            {
            iCallback.ProcessSyncCommandL(
                    iOperation,
                    *Message() );
            }
            break;

        default:
            __ASSERT_DEBUG( EFalse, Panic(EInvalidCommandId) );
            break;
        }

    iResult.iOpType = iOperation;
    iResult.iMessage = Message();
    iResult.iResultCode = KErrNone;

    Observer()->OperationCompleted( iResult );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::CancelCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailSyncCmd::CancelCommand()
    {
    FUNC_LOG;
    // No cancellation needed because command was forced synchronised
    }

// EOF

