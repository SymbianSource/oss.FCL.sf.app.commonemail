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
* Description:  ESMR esmricalviewer implementation
*
*/


// INCLUDE FILES

#include "emailtrace.h"
#include "cesmricalviewercmdhandler.h"
#include "cesmricalvieweropenmrcmd.h"
#include "cesmricalviewercanviewmr.h"
#include "cesmricalviewerresolvemrmethodsync.h"
#include "cesmricalviewerresolvemrmethodasync.h"
#include "cesmricalviewerresponsetomrcmd.h"
#include "cesmricalviewerremovemrfromcalcmd.h"

#include <calsession.h>
//<cmail>
#include "cfsmailclient.h"
#include "esmricalvieweropcodes.hrh"
#include "esmricalviewerdefs.hrh"
//</cmail>
#include <eikenv.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// Panic literal
_LIT( KESMRIcalViewerCmdHandler, "ESMRIcalViewerCmdHandler" );

enum TESMRIcalViewerCmdHandlerPanic
    {
    EInvalidCommand = 0, // Trying to execute invalid command
    EInvalidObserverCall
    };

void Panic( TESMRIcalViewerCmdHandlerPanic aPanic )
    {
    User::Panic( KESMRIcalViewerCmdHandler, aPanic );
    }

#endif // _DEBUG

}  // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::CESMRIcalViewerCmdHandler
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCmdHandler::CESMRIcalViewerCmdHandler(
        MESMRIcalViewerCallback& aCallback )
:   iCallback( aCallback )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::~CESMRIcalViewerCmdHandler
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCmdHandler::~CESMRIcalViewerCmdHandler()
    {
    FUNC_LOG;
    CancelOperation();

    delete iCommand;
    delete iMessage;
    if ( iMailClient )
        {
        iMailClient->Close();
        }
    delete iCalSession;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCmdHandler* CESMRIcalViewerCmdHandler::NewL(
        MESMRIcalViewerCallback& aCallback )
    {
    FUNC_LOG;
    CESMRIcalViewerCmdHandler* self =
            new (ELeave) CESMRIcalViewerCmdHandler( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCmdHandler::ConstructL()
    {
    FUNC_LOG;
    iCalSession = CCalSession::NewL();
    iCalSession->OpenL( KNullDesC );
    iMailClient = CFSMailClient::NewL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCmdHandler::ExecuteCommandL(
        TESMRIcalViewerOperationType aOpType,
        MESMRIcalViewerObserver* aObserver,
        CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    ExecuteCommandL( aOpType, aObserver, aMessage, NULL );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCmdHandler::ExecuteCommandL(
        TESMRIcalViewerOperationType aOpType,
        MESMRIcalViewerObserver* aObserver,
        CFSMailMessage& aMessage,
        TAny* aParam )
    {
    FUNC_LOG;
    TRAPD( err, ExecuteCommandInternalL( aOpType,
                                        aObserver,
                                        aMessage,
                                        aParam ) );
    
    if ( KErrNone != err && EIdle != iState )
        {
        // Make sure that error is notified to caller in all cases
        if ( iObserver)
            {
            iResult.iMessage = &aMessage;
            iResult.iResultCode = err;
            iResult.iOpType = aOpType;        
            iObserver->OperationError( iResult );
            }
        iState = EIdle;
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::CancelOperation
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCmdHandler::CancelOperation()
    {
    FUNC_LOG;
    if ( iCommand )
        {
        iCommand->CancelCommand();
        }

    iState = EIdle;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::OperationCompleted
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCmdHandler::OperationCompleted(
        TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iCommand, Panic(EInvalidObserverCall) );
    __ASSERT_DEBUG(
            iCommand->OperationType() == aResult.iOpType,
            Panic(EInvalidObserverCall) );

    if ( iObserver )
        {
        aResult.iOpType = iCommand->OperationType();
        iObserver->OperationCompleted( aResult );
        }

    iState = EIdle;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::OperationError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCmdHandler::OperationError(
                TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iCommand, Panic(EInvalidObserverCall) );
    __ASSERT_DEBUG(
            iCommand->OperationType() == aResult.iOpType,
            Panic(EInvalidObserverCall) );

    if ( iObserver )
        {
        aResult.iOpType = iCommand->OperationType();
        iObserver->OperationError( aResult );
        }

    iState = EIdle;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCmdHandler::ExecuteCommandInternalL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCmdHandler::ExecuteCommandInternalL( 
        TESMRIcalViewerOperationType aOpType,
        MESMRIcalViewerObserver* aObserver,
        CFSMailMessage& aMessage,
        TAny* aParam )
    {
    FUNC_LOG;
    if ( EExecuting == iState )
        {
        User::Leave( KErrInUse );
        }

    delete iCommand; iCommand = NULL;

    iState = EExecuting;
    iObserver = aObserver;

    // Get own copy of the message before executing commands
    delete iMessage; iMessage = NULL;
    iMessage = iMailClient->GetMessageByUidL(
            aMessage.GetMailBoxId(),
            aMessage.GetFolderId(),
            aMessage.GetMessageId(),
            EFSMsgDataStructure );

    switch ( aOpType )
        {
        case EESMRViewLaunch:
            {
            iCommand = CESMRIcalViewerOpenMRCmd:: NewL(
                    iCallback,
                    *iMailClient,
                    *iCalSession );
            }
            break;

        case EESMRCanViewMR:
            {
            TBool* flag = static_cast<TBool*>( aParam );
            iCommand = CESMRIcalViewerCanViewMsgCmd:: NewL(
                    *iCalSession,
                    *flag );
            }
            break;

        case EESMRResolveMRMethod:
            {
            iCommand = CESMRIcalViewerResolveMRMethodASyncCmd::NewL(
                                *iCalSession,
                                *iMailClient );
            }
            break;

        case EESMRResolveMRMethodSync:
            {
            TESMRMeetingRequestMethod* mrMethod =
                    static_cast<TESMRMeetingRequestMethod*>( aParam );

            iCommand = CESMRIcalViewerResolveMRMethodSyncCmd::NewL(
                                    *iCalSession,
                                    *iMailClient,
                                    *mrMethod );
            }
            break;

        case EESMRResponseToMR:
            {
            TESMRAttendeeStatus* attendeeStatus =
                    static_cast<TESMRAttendeeStatus*>( aParam );

            iCommand = CESMRIcalViewerResponseToMRCmd::NewL(
                            iCallback,
                            *iCalSession,
                            *iMailClient,
                            *attendeeStatus);

            }
            break;

        case EESMRRemoveMR:
            {
            iCommand = CESMRIcalViewerRemoveMRFromCalCmd::NewL(
                            *iCalSession,
                            *iMailClient );
            }
            break;

        default:
            break;
        }

    __ASSERT_DEBUG( iCommand, Panic( EInvalidCommand) );
    iCommand->ExecuteCommandL( *iMessage, *this );
    }

// EOF

