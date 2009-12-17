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
* Description:  Open meeting request command implementation
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmricalvieweropenmrcmd.h"
#include "cesmricalviewerloadmrdatacmd.h"
#include "cesmricalvieweremailasynccmd.h"
#include "cesmricalvieweremailsynccmd.h"
#include "cesmruilauncher.h"
#include "cesmrattachmentinfo.h"
#include "esmricalviewerutils.h"

//<cmail>
#include "esmricalvieweropcodes.hrh"
//</cmail>
#include <calsession.h>

// ======== MEMBER FUNCTIONS ========
// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd(
        MESMRIcalViewerCallback& aCallback,
        CFSMailClient& aMailClient,
        CCalSession& aCalSession )
:   CESMRIcalViewerAsyncCommand( EESMRViewLaunch, aCalSession ),
    iCallback( aCallback ),
    iMailClient( aMailClient ),
    iCurrentCbCommand( EESMREmailCommand )
    {
    FUNC_LOG;
    iResult.iResultCode = KErrNone;
    iResult.iOpType = OperationType();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerOpenMRCmd::~CESMRIcalViewerOpenMRCmd()
    {
    FUNC_LOG;
    CancelCommand();
    delete iLoadMRDataCmd;
    delete iUILauncher;
    
    // <cmail>
    if( iEmailCommand )
        {
        delete iEmailCommand;
        iEmailCommand = NULL;
        }
    // </cmail>
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerOpenMRCmd* CESMRIcalViewerOpenMRCmd::NewL(
        MESMRIcalViewerCallback& aCallback,
        CFSMailClient& aMailClient,
        CCalSession& aCalSession )
    {
    FUNC_LOG;
    CESMRIcalViewerOpenMRCmd* self =
            new (ELeave) CESMRIcalViewerOpenMRCmd(
                    aCallback,
                    aMailClient,
                    aCalSession );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::ExecuteAsyncCommandL()
    {
    FUNC_LOG;
    iResult.iOpType = OperationType();
    iResult.iMessage = Message();
    iResult.iResultCode = KErrNone;

    iLoadMRDataCmd =
            CESMRIcalViewerLoadMRDataCmd::NewL(
                    CalSession(),
                    iMailClient,
                    iInputParameters );

    iLoadMRDataCmd->ExecuteCommandL( *Message(), *this );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::CancelAsyncCommand()
    {
    FUNC_LOG;
    if ( iLoadMRDataCmd )
        {
        iLoadMRDataCmd->CancelCommand();
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::OperationCompleted(
         MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    TRAPD( err, HandleOperationCompeletedL( aResult ) );
    HandleError( err );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::CESMRIcalViewerOpenMRCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    iResult = aResult;
    iResult.iOpType = OperationType();

    if ( EESMRLaunchUI == aResult.iOpType )
        {
        delete iUILauncher;
        iUILauncher = NULL;
        }
    if ( !iUILauncher &&
          EESMREmailCommand == iCurrentCbCommand &&
          KErrCancel == aResult.iResultCode )
        {
        // We would like to launch mr again
        iInputParameters.iMailClient = &iMailClient;
        iInputParameters.iMailMessage = Message();
        iInputParameters.iCommand = EESMRCmdUndefined;
        iInputParameters.iSpare = 0;
        TRAPD( err, LaunchUIL() );
        iResult.iResultCode = err;

        if ( err != KErrNone )
            {
            Observer()->OperationError( iResult );
            }
        }
    else
        {
        Observer()->OperationError( iResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::ProcessCommandL(TInt aCommandId)
    {
    FUNC_LOG;
    ProcessCommandWithResultL( aCommandId );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::ProcessCommandWithResultL
// -----------------------------------------------------------------------------
//
TInt CESMRIcalViewerOpenMRCmd::ProcessCommandWithResultL( TInt aCommandId )
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

// <cmail>
        if( iEmailCommand )
            {
            delete iEmailCommand;
            iEmailCommand = NULL;
            }
// </cmail>

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
// CESMRIcalViewerOpenMRCmd::IsCommandAvailable
// -----------------------------------------------------------------------------
//
TBool CESMRIcalViewerOpenMRCmd::IsCommandAvailable( TInt aCommandId )
    {
    FUNC_LOG;

    TESMRIcalViewerOperationType command(
             ESMRIcalViewerUtils::EmailOperation(aCommandId) );

    TBool canProcessCommand( iCallback.CanProcessCommand( command  ) );

    return canProcessCommand;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::HandleOperationCompeletedL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::HandleOperationCompeletedL(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;


    if ( EESMRLaunchUI == aResult.iOpType )
            {
            delete iUILauncher;
            iUILauncher = NULL;
            }
    else if ( aResult.iOpType == EESMRLoadMRData )
        {
        // MR data is loaded
        iInputParameters.iMailClient = &iMailClient;
        iInputParameters.iMailMessage = Message();
        iInputParameters.iCommand = EESMRCmdUndefined;
        iInputParameters.iSpare = 0;
        LaunchUIL();
        }
    else if ( aResult.iOpType == iCurrentCbCommand )
        {
        TBool asyncCommand( ESMRIcalViewerUtils::IsAsyncEmailCommand(iCurrentCbCommand) );

        switch ( aResult.iOpType )
            {
            case EESMRCmdOpenAttachment://fallthrough
            case EESMRCmdOpenAttachmentView://fallthrough
// <cmail>
            case EESMRCmdDownloadAttachment://fallthrough
            case EESMRCmdDownloadAllAttachments://fallthrough
            case EESMRCmdSaveAttachment://fallthrough
            case EESMRCmdSaveAllAttachments://fallthrough
// </cmail>
                {
                if ( ESMRIcalViewerUtils::ContainsAttachments(iInputParameters) &&
                     iMRViewerCommand != iCurrentCbCommand )
                    {
                    iInputParameters.iCommand =
                            ESMRIcalViewerUtils::MRResponseCommandWithoutAttachmentCheck(
                                    iMRViewerCommand);
                    }
                iCurrentCbCommand = EESMREmailCommand;
                //LaunchUIL();
                }
                break;

            case EESMRCmdMailForwardAsMessage://fallthrough
            case EESMRCmdMailCopyMessage://fallthrough
            case EESMRCmdMailComposeMessage://fallthrough
            case EESMRCmdMailMessageDetails://fallthrough
            case EESMRCmdDownloadManager://fallthrough
            case EESMRCmdMailReply://fallthrough
            case EESMRCmdMailReplyAll://fallthrough
                {
                // Ui needs to be relaunched
                iCurrentCbCommand = EESMREmailCommand;
                LaunchUIL();
                }
                break;

            case EESMRCmdMailMoveMessage:
                {
                iCurrentCbCommand = EESMREmailCommand;
                if ( KErrCancel == aResult.iResultCode )
                    {
                    LaunchUIL();
                    }
                }
                break;

            case EESMRCmdMailPreviousMessage:
            case EESMRCmdMailNextMessage:
            case EESMRCmdMailMoveMessageToDrafts:
            case EESMRCmdMailDelete:
                {
                //ui won't be relaunched
                iCurrentCbCommand = EESMREmailCommand;
                }
                break;
            default:
                break;
            }
        }

    if ( !iUILauncher &&
         EESMREmailCommand == iCurrentCbCommand )
        {
        iResult.iAttendeeStatus =
                ESMRIcalViewerUtils::UserResponsedToMRL(
                        *iInputParameters.iCalEntry );
        Observer()->OperationCompleted( iResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::HandleError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::HandleError( TInt aError )
    {
    FUNC_LOG;
    if ( KErrNone != aError )
        {
        iResult.iResultCode = aError;
        Observer()->OperationError( iResult );
        }
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewerOpenMRCmd::LaunchUIL
// ---------------------------------------------------------------------------
//
void CESMRIcalViewerOpenMRCmd::LaunchUIL()
    {
    FUNC_LOG;
    if ( !iUILauncher )
        {
        iUILauncher = CESMRUiLauncher::NewL(
                            CalSession(),
                            iInputParameters,
                            *this );
        }

    iUILauncher->ExecuteCommandL( *Message(), *this );
    }

// EOF

