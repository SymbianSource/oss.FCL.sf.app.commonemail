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
#include "cesmricalviewerresolvemrmethodasync.h"
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
// CESMRIcalViewerResolveMRMethodASyncCmd::CESMRIcalViewerResolveMRMethodASyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResolveMRMethodASyncCmd::CESMRIcalViewerResolveMRMethodASyncCmd(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient )
:   CESMRIcalViewerAsyncCommand( EESMRResolveMRMethod, aCalSession ),
    iMailClient( aMailClient ),
    iMRMethod( EESMRMeetingRequestMethodUnknown )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodASyncCmd::~CESMRIcalViewerResolveMRMethodASyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResolveMRMethodASyncCmd::~CESMRIcalViewerResolveMRMethodASyncCmd()
    {
    FUNC_LOG;
    CancelCommand();
    iMRRetrieverCommands.ResetAndDestroy();
    iMRRetrieverCommands.Close();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodASyncCmd::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResolveMRMethodASyncCmd* CESMRIcalViewerResolveMRMethodASyncCmd::NewL(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient )
    {
    FUNC_LOG;
    CESMRIcalViewerResolveMRMethodASyncCmd* self =
            new (ELeave) CESMRIcalViewerResolveMRMethodASyncCmd(
                    aCalSession, aMailClient  );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodASyncCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodASyncCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodASyncCmd::OperationCompleted
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodASyncCmd::OperationCompleted(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRLoadMRInfoData ||
         aResult.iOpType == EESMRLoadMRIcalData         )
        {
        // MRINFO data was parsed succesfully
        CCalEntry::TMethod method( CCalEntry::EMethodNone );
        // If this leaves we just go to case default.
        TRAP_IGNORE( method = iESMRInputParams.iCalEntry->MethodL() );

        switch ( method )
            {
            case CCalEntry::EMethodRequest:
                {
                iMRMethod = EESMRMeetingRequestMethodRequest;
                }
                break;
            case CCalEntry::EMethodReply:
                {
                iMRMethod = EESMRMeetingRequestMethodResponse;
                }
                break;
            case CCalEntry::EMethodCancel:
                {
                iMRMethod = EESMRMeetingRequestMethodCancellation;
                }
                break;
            default:
                {
                iMRMethod = EESMRMeetingRequestMethodUnknown;
                }
                break;
            }

        aResult.iOpType  = OperationType();
        aResult.iMethod  = iMRMethod;
        aResult.iMessage = Message();

        TBool isAttendee = EFalse;
        TRAP_IGNORE( isAttendee = ESMRIcalViewerUtils::IsMailboxOwnerAttendeeL(
                            *(iESMRInputParams.iCalEntry),
                            iMailClient,
                            *Message() )) ;

        if ( !isAttendee )
            {
            iMRMethod = EESMRMeetingRequestMethodUnknown;
            aResult.iMethod = EESMRMeetingRequestMethodUnknown;
            }

        Observer()->OperationCompleted( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodASyncCmd::OperationError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodASyncCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRLoadMRInfoData )
        {
        if ( aResult.iResultCode == KErrNotFound && iMRRetrieverCommands.Count() > iCurrentCommand + 1)
            {
            TRAPD( error, iMRRetrieverCommands[++iCurrentCommand]->ExecuteCommandL(
                    *Message(),
                    *this ) );
            if ( error != KErrNone )
                {
                aResult.iOpType = OperationType();
                aResult.iResultCode = error;
                aResult.iMethod  = iMRMethod;
                aResult.iMessage = Message();
                Observer()->OperationError( aResult );
                }
            }
        else
            {
            aResult.iOpType  = OperationType();
            aResult.iMethod  = iMRMethod;
            aResult.iMessage = Message();

            Observer()->OperationError( aResult );
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodASyncCmd::ExecuteAsyncCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodASyncCmd::ExecuteAsyncCommandL()
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
void CESMRIcalViewerResolveMRMethodASyncCmd::CancelAsyncCommand()
    {
    FUNC_LOG;
    TInt commandCount( iMRRetrieverCommands.Count() );

    for ( TInt i(0); i < commandCount; ++i )
        {
        iMRRetrieverCommands[iCurrentCommand]->CancelCommand();
        }
    }

// EOF

