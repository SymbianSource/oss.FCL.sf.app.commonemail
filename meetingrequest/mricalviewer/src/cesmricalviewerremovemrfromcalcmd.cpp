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
#include "cesmricalviewerremovemrfromcalcmd.h"
#include "cesmruilauncher.h"

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
// CESMRIcalViewerRemoveMRFromCalCmd::CESMRIcalViewerRemoveMRFromCalCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerRemoveMRFromCalCmd::CESMRIcalViewerRemoveMRFromCalCmd(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient )
:   CESMRIcalViewerAsyncCommand( EESMRRemoveMR, aCalSession ),
    iMailClient( aMailClient ),
    iCurrentCommand( EESMREmailCommand )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::~CESMRIcalViewerRemoveMRFromCalCmd
// -----------------------------------------------------------------------------
//
        CESMRIcalViewerRemoveMRFromCalCmd::~CESMRIcalViewerRemoveMRFromCalCmd()
    {
    FUNC_LOG;
    CancelCommand();
    delete iUILauncher;
    iMRRetrieverCommands.ResetAndDestroy();
    iMRRetrieverCommands.Close();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResponseToMRCmd::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerRemoveMRFromCalCmd* CESMRIcalViewerRemoveMRFromCalCmd::NewL(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient )
    {
    FUNC_LOG;
    CESMRIcalViewerRemoveMRFromCalCmd* self =
            new (ELeave) CESMRIcalViewerRemoveMRFromCalCmd(
                    aCalSession,
                    aMailClient );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::OperationCompleted
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::OperationCompleted(
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
    else if ( !iUILauncher )
        {
        aResult.iOpType = OperationType();
        Observer()->OperationCompleted( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::LaunchUiL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::LaunchUiL()
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
// CESMRIcalViewerRemoveMRFromCalCmd::OperationError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRLoadMRInfoData )
        {
        if ( aResult.iResultCode == KErrNotFound  && iMRRetrieverCommands.Count() > iCurrentCommand + 1 )
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
        Observer()->OperationError( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::ExecuteAsyncCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::ExecuteAsyncCommandL()
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
// CESMRIcalViewerRemoveMRFromCalCmd::CancelAsyncCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::CancelAsyncCommand()
    {
    FUNC_LOG;
    TInt commandCount( iMRRetrieverCommands.Count() );

    for ( TInt i(0); i < commandCount; ++i )
        {
        iMRRetrieverCommands[iCurrentCommand]->CancelCommand();
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::ProcessCommandL(TInt aCommandId)
    {
    FUNC_LOG;
    ProcessCommandWithResultL( aCommandId );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::ProcessCommandWithResultL
// -----------------------------------------------------------------------------
//
TInt CESMRIcalViewerRemoveMRFromCalCmd::ProcessCommandWithResultL(
        TInt /*aCommandId*/  )
    {
    FUNC_LOG;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::IsCommandAvailable
// -----------------------------------------------------------------------------
//
TBool CESMRIcalViewerRemoveMRFromCalCmd::IsCommandAvailable( TInt /*aCommandId*/ )
    {
    FUNC_LOG;
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CESMRIcalViewerRemoveMRFromCalCmd::HandleOperationCompletedL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerRemoveMRFromCalCmd::HandleOperationCompletedL(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( EESMRLaunchUI == aResult.iOpType )
        {
        delete iUILauncher;
        iUILauncher = NULL;
        }

    if ( aResult.iOpType == EESMRLoadMRInfoData ||
         aResult.iOpType == EESMRLoadMRIcalData )
        {
        // MRINFO data was parsed succesfully
        CCalEntry::TMethod method( CCalEntry::EMethodNone );
        method = iESMRInputParams.iCalEntry->MethodL();

        if ( CCalEntry::EMethodCancel == method )
            {
            // MR data is loaded
            iESMRInputParams.iMailClient = &iMailClient;
            iESMRInputParams.iMailMessage = Message();
            iESMRInputParams.iCommand = EESMRCmdRemoveFromCalendar;
            iESMRInputParams.iSpare = 0;
            LaunchUiL();
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }
    }

// EOF

