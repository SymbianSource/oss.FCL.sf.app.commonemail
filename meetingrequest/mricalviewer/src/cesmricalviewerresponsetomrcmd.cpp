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
* Description:  Resolving meeting request method asynchronoysly
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmricalviewerresponsetomrcmd.h"
#include "cesmruilauncher.h"
#include "cesmricalvieweremailasynccmd.h"
#include "cesmricalvieweremailsynccmd.h"
#include "esmricalviewerutils.h"

//<cmail>
#include "esmricalvieweropcodes.hrh"
#include <calsession.h>
#include "cfsmailclient.h"
//</cmail>
#include <calentry.h>

#include "cesmrmrinfoicalretriever.h"
#include "cesmrimapicalretriever.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::CESMRIcalViewerResponseToMRCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResponseToMRCmd::CESMRIcalViewerResponseToMRCmd(
        MESMRIcalViewerCallback& aCallback,
        CCalSession& aCalSession,
        CFSMailClient& aMailClient,
        TESMRAttendeeStatus aAttendeeStatus )
:   CESMRIcalViewerAsyncCommand( EESMRResponseToMR, aCalSession ),
    iCallback( aCallback ),
    iMailClient( aMailClient ),
    iAttendeeStatus( aAttendeeStatus ),
    iCurrentCbCommand( EESMREmailCommand )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::~CESMRIcalViewerResponseToMRCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResponseToMRCmd::~CESMRIcalViewerResponseToMRCmd()
    {
    FUNC_LOG;
    CancelCommand();
    delete iUILauncher;

    iMRRetrieverCommands.ResetAndDestroy();
    iMRRetrieverCommands.Close();
    delete iEmailCommand;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResponseToMRCmd* CESMRIcalViewerResponseToMRCmd::NewL(
        MESMRIcalViewerCallback& aCallback,
        CCalSession& aCalSession,
        CFSMailClient& aMailClient,
        TESMRAttendeeStatus aAttendeeStatus )
    {
    FUNC_LOG;
    CESMRIcalViewerResponseToMRCmd* self =
            new (ELeave) CESMRIcalViewerResponseToMRCmd(
                    aCallback,
                    aCalSession,
                    aMailClient,
                    aAttendeeStatus );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::OperationCompleted
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::OperationCompleted(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    TRAPD( err, HandleOperationCompletedL(aResult) );
    if (KErrNone != err )
        {
        aResult.iOpType = OperationType();
        aResult.iResultCode = err;
        Observer()->OperationError( aResult );
        }
    else if ( !iUILauncher && ((EESMREmailCommand == iCurrentCbCommand) ||
    		  (iCurrentCbCommand == EESMRCmdMailDelete)) )
        {
        aResult.iOpType = OperationType();
        aResult.iAttendeeStatus = iAttendeeStatus;
        Observer()->OperationCompleted( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::LaunchUiL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::LaunchUiL()
    {
    FUNC_LOG;
    if ( !iUILauncher )
        {
        iUILauncher = CESMRUiLauncher::NewL(
                            CalSession(),
                            iESMRInputParams,
                            *this );
        }

    iUILauncher->ExecuteCommandL( *Message(), *this );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::OperationError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRLoadMRInfoData )
        {
        if ( aResult.iResultCode == KErrNotFound && iMRRetrieverCommands.Count() > iCurrentCommand + 1 )
            {
            TRAPD( error, iMRRetrieverCommands[++iCurrentCommand]->ExecuteCommandL(
                    *Message(),
                    *this ) );
            if ( error != KErrNone )
                {
                aResult.iOpType = OperationType();
                aResult.iResultCode = error;
                aResult.iMessage = Message();
                Observer()->OperationError( aResult );
                }
            }
        else
            {
            aResult.iOpType  = OperationType();
            aResult.iMessage = Message();
            Observer()->OperationError( aResult );
            }
        }
    else
        {
        aResult.iOpType  = OperationType();
        aResult.iMessage = Message();
        aResult.iResultCode = KErrNotSupported;

        Observer()->OperationError( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::ExecuteAsyncCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::ExecuteAsyncCommandL()
    {
    FUNC_LOG;
    CESMRInfoIcalRetrieverCmd* cmd = CESMRInfoIcalRetrieverCmd::NewL(
                CalSession(),
                iESMRInputParams );

    CleanupStack::PushL( cmd );
    User::LeaveIfError(
            iMRRetrieverCommands.Append( cmd ) );
    CleanupStack::Pop( cmd ); cmd = NULL;

    CESMRImapIcalRetrieverCmd* cmd2 =
        CESMRImapIcalRetrieverCmd::NewL(
            CalSession(),
            iMailClient,
            iESMRInputParams );

    CleanupStack::PushL( cmd2 );
    User::LeaveIfError(
            iMRRetrieverCommands.Append( cmd2 ) );
    CleanupStack::Pop( cmd2 ); cmd2 = NULL;

    iCurrentCommand = 0;

    if(iMRRetrieverCommands.Count() > iCurrentCommand)
        {
        iMRRetrieverCommands[iCurrentCommand]->ExecuteCommandL(
                       *Message(),
                       *this );
        }
    else
        {
        User::Leave(KErrOverflow);
        }
    
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodASyncCmd::CancelAsyncCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::CancelAsyncCommand()
    {
    FUNC_LOG;
    TInt commandCount( iMRRetrieverCommands.Count() );

    for ( TInt i(0); i < commandCount; ++i )
        {
        iMRRetrieverCommands[iCurrentCommand]->CancelCommand();
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::ProcessCommandL(TInt aCommandId)
    {
    FUNC_LOG;
    ProcessCommandWithResultL( aCommandId );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::ProcessCommandWithResultL
// -----------------------------------------------------------------------------
//
TInt CESMRIcalViewerResponseToMRCmd::ProcessCommandWithResultL( TInt aCommandId )
    {
    FUNC_LOG;
    TInt retValue( KErrNotSupported );

    if ( IsCommandAvailable( aCommandId ) )
        {
        iMRViewerCommand = aCommandId;

        iCurrentCbCommand =
                ESMRIcalViewerUtils::EmailOperation(aCommandId);

        TBool asyncCommand(
                ESMRIcalViewerUtils::IsAsyncEmailCommand( iCurrentCbCommand ) );

        if ( asyncCommand )
            {
            iEmailCommand = CESMRIcalViewerEmailAsyncCmd::NewL(
                    iCurrentCbCommand,
                    CalSession(),
                    iCallback );
            }
        else
            {
            iEmailCommand = CESMRIcalViewerEmailSyncCmd::NewL(
                                    iCurrentCbCommand,
                                    CalSession(),
                                    iCallback );
            }

        iEmailCommand->ExecuteCommandL( *Message(), *this );

        if ( !asyncCommand )
            {
            // Email specific sync command has been executed.
            iCurrentCbCommand = EESMREmailCommand;
            }
        retValue = KErrNone;
        }

    return retValue;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::IsCommandAvailable
// -----------------------------------------------------------------------------
//
TBool CESMRIcalViewerResponseToMRCmd::IsCommandAvailable( TInt aCommandId )
    {
    FUNC_LOG;
    TESMRIcalViewerOperationType command(
             ESMRIcalViewerUtils::EmailOperation(aCommandId) );

    return iCallback.CanProcessCommand( command );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::HandleOperationCompletedL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResponseToMRCmd::HandleOperationCompletedL(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( EESMRLaunchUI == aResult.iOpType )
        {
        delete iUILauncher;
        iUILauncher = NULL;
        }

    if ( aResult.iOpType == EESMRLoadMRInfoData ||
         aResult.iOpType == EESMRLoadMRIcalData ||
         (aResult.iOpType == iCurrentCbCommand &&
          aResult.iOpType != EESMRCmdMailDelete) ) // <cmail>
        {
        // MRINFO data was parsed succesfully
        CCalEntry::TMethod method( CCalEntry::EMethodNone );
        method = iESMRInputParams.iCalEntry->MethodL();

        if ( CCalEntry::EMethodRequest == method )
            {
            iESMRInputParams.iMailClient = &iMailClient;
            iESMRInputParams.iMailMessage = Message();
            iESMRInputParams.iCommand = EESMRCmdAcceptMR;
            iESMRInputParams.iSpare = 0;

            if ( EESMRAttendeeStatusTentative == iAttendeeStatus )
                {
                iESMRInputParams.iCommand = EESMRCmdTentativeMR;
                }
            else if ( EESMRAttendeeStatusDecline == iAttendeeStatus )
                {
                iESMRInputParams.iCommand = EESMRCmdDeclineMR;
                }
            if ( ESMRIcalViewerUtils::ContainsAttachments(iESMRInputParams) &&
                 iMRViewerCommand != iCurrentCbCommand &&
                 aResult.iOpType == iCurrentCbCommand )
                {
                iCurrentCbCommand = EESMREmailCommand;
                iESMRInputParams.iCommand =
                        ESMRIcalViewerUtils::MRResponseCommandWithoutAttachmentCheck(
                                iMRViewerCommand);
                }
            LaunchUiL();
            }

        if ( iCurrentCbCommand == EESMREmailCommand &&
             method != CCalEntry::EMethodNone &&
             !iUILauncher )
            {
            // No email command in execution --> Notify observer
            aResult.iOpType = OperationType();
            aResult.iAttendeeStatus = iAttendeeStatus;
            Observer()->OperationCompleted( aResult );
            }
        }
    }

// EOF

