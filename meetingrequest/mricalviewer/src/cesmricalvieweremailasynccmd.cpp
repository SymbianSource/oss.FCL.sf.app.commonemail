/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resolving meeting request method asynchronoysly 
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmricalvieweremailasynccmd.h"

//<cmail>
#include "esmricalvieweropcodes.hrh"
#include <calsession.h>
#include "CFSMailClient.h"
//</cmail>
#include <calentry.h>

#include "cesmrmrinfoicalretriever.h"
#include "cesmrimapicalretriever.h"

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// Panic literal
_LIT( KESMRIcalViewerEmailAsyncCmd, "ESMRIcalViewerEmailAsyncCmd" );

/** Panic codes */
enum TESMRIcalViewerEmailAsyncCmdPanic
    {
    EInvalidCommandId = 0 //Invalid command id
    };

void Panic( TESMRIcalViewerEmailAsyncCmdPanic aPanic )
    {
    User::Panic( KESMRIcalViewerEmailAsyncCmd, aPanic );
    }

#endif // _DEBUG

}  // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailAsyncCmd::CESMRIcalViewerEmailAsyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerEmailAsyncCmd::CESMRIcalViewerEmailAsyncCmd(
        TESMRIcalViewerOperationType aOperation,
        CCalSession& aCalSession,
        MESMRIcalViewerCallback& aCallback  )
:   CESMRIcalViewerAsyncCommand( EESMREmailCommand, aCalSession ),
    iOperation( aOperation),
    iCallback( aCallback )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::~CESMRIcalViewerEmailAsyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerEmailAsyncCmd::~CESMRIcalViewerEmailAsyncCmd()
    {
    FUNC_LOG;
    CancelCommand();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailAsyncCmd::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerEmailAsyncCmd* CESMRIcalViewerEmailAsyncCmd::NewL(
        TESMRIcalViewerOperationType aOperation,
        CCalSession& aCalSession,
        MESMRIcalViewerCallback& aCallback )
    {
    FUNC_LOG;
    CESMRIcalViewerEmailAsyncCmd* self =
            new (ELeave) CESMRIcalViewerEmailAsyncCmd(
                    aOperation,
                    aCalSession,
                    aCallback );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailAsyncCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailAsyncCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailAsyncCmd::OperationCompleted
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailAsyncCmd::OperationCompleted(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( aResult.iOpType == iOperation, Panic( EInvalidCommandId)  );

    // Dispatch completion message to observer
    Observer()->OperationCompleted( aResult );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailAsyncCmd::OperationError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailAsyncCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( aResult.iOpType == iOperation, Panic( EInvalidCommandId)  );

    // Dispatch completion message to observer
    Observer()->OperationError( aResult );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailAsyncCmd::ExecuteAsyncCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailAsyncCmd::ExecuteAsyncCommandL()
    {
    FUNC_LOG;
    switch ( iOperation )
        {
        case EESMRCmdOpenAttachment://fallthrough
        case EESMRCmdOpenAttachmentView://fallthrough
// <cmail>
        case EESMRCmdDownloadAttachment://fallthrough
        case EESMRCmdDownloadAllAttachments://fallthrough
        case EESMRCmdSaveAttachment://fallthrough
        case EESMRCmdSaveAllAttachments://fallthrough
// </cmail>
        case EESMRCmdMailDelete://fallthrough
        case EESMRCmdMailForwardAsMessage://fallthrough
        case EESMRCmdMailMoveMessage://fallthrough
        case EESMRCmdMailCopyMessage://fallthrough
        case EESMRCmdMailComposeMessage://fallthrough
        case EESMRCmdMailMessageDetails://fallthrough
        case EESMRCmdMailPreviousMessage://fallthrough
        case EESMRCmdMailNextMessage://fallthrough
        case EESMRCmdMailMoveMessageToDrafts://fallthrough
        case EESMRCmdDownloadManager://fallthrough
        case EESMRCmdMailReply://fallthrough
        case EESMRCmdMailReplyAll://fallthrough
            {
            iCallback.ProcessAsyncCommandL(
                    iOperation,
                    *Message(),
                    this );
            }
            break;

        default:
            __ASSERT_DEBUG( EFalse, Panic(EInvalidCommandId) );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerEmailAsyncCmd::CancelAsyncCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerEmailAsyncCmd::CancelAsyncCommand()
    {
    FUNC_LOG;
    // No cancellation necessary
    }

// EOF

