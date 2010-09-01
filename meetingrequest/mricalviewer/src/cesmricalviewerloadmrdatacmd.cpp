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
* Description:  Loading MR data command implemenation
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmricalviewerloadmrdatacmd.h"
//<cmail>
#include "esmricalvieweropcodes.hrh"
#include <calsession.h>
#include "cfsmailclient.h"
//</cmail>

#include "cesmrmrinfoicalretriever.h"
#include "cesmrimapicalretriever.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerLoadMRDataCmd::CESMRIcalViewerLoadMRDataCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerLoadMRDataCmd::CESMRIcalViewerLoadMRDataCmd(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient,
        TESMRInputParams& aESMRInputParams )
:   CESMRIcalViewerAsyncCommand( EESMRLoadMRData, aCalSession ),
    iESMRInputParams( aESMRInputParams ),
    iMailClient( aMailClient )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerLoadMRDataCmd::~CESMRIcalViewerLoadMRDataCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerLoadMRDataCmd::~CESMRIcalViewerLoadMRDataCmd()
    {
    FUNC_LOG;
    CancelCommand();
    iMRRetrieverCommands.ResetAndDestroy();
    iMRRetrieverCommands.Close();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerLoadMRDataCmd::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerLoadMRDataCmd* CESMRIcalViewerLoadMRDataCmd::NewL(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient,
        TESMRInputParams& aESMRInputParams )
    {
    FUNC_LOG;
    CESMRIcalViewerLoadMRDataCmd* self =
            new (ELeave) CESMRIcalViewerLoadMRDataCmd(
                    aCalSession, aMailClient, aESMRInputParams );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerLoadMRDataCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerLoadMRDataCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerLoadMRDataCmd::OperationCompleted
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerLoadMRDataCmd::OperationCompleted(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRLoadMRInfoData ||
         aResult.iOpType == EESMRLoadMRIcalData     )
        {
        aResult.iOpType = OperationType();
        Observer()->OperationCompleted( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerLoadMRDataCmd::OperationError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerLoadMRDataCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRLoadMRInfoData )
        {
        if ( aResult.iResultCode == KErrNotFound )
            {
            if(iMRRetrieverCommands.Count() > iCurrentCommand + 1)
            	{
                TRAPD(error, iMRRetrieverCommands[++iCurrentCommand]->ExecuteCommandL(
                             *Message(),
                             *this )     );

                if ( error != KErrNone )
                    {
                    aResult.iOpType = OperationType();
                    aResult.iResultCode = error;
                    Observer()->OperationError( aResult );
                    }
            	}
            }
        else
            {
            aResult.iOpType = OperationType();
            Observer()->OperationError( aResult );
            }
        }
    else
        {
        aResult.iOpType = OperationType();
        Observer()->OperationError( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerLoadMRDataCmd::ExecuteAsyncCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerLoadMRDataCmd::ExecuteAsyncCommandL()
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
    CleanupStack::Pop( cmd2 ); 
    cmd2 = NULL;

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
// CESMRIcalViewerLoadMRDataCmd::CancelAsyncCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerLoadMRDataCmd::CancelAsyncCommand()
    {
    FUNC_LOG;
    TInt commandCount( iMRRetrieverCommands.Count() );

    for ( TInt i(0); i < commandCount; ++i )
        {
        iMRRetrieverCommands[iCurrentCommand]->CancelCommand();
        }
    }

// EOF

