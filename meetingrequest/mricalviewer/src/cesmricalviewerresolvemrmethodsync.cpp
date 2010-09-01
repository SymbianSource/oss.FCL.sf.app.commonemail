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
#include "cesmricalviewerresolvemrmethodsync.h"
#include "cesmrmrinfoicalretriever.h"
#include "esmricalviewerutils.h"
//<cmail>
#include "esmricalvieweropcodes.hrh"
#include <calentry.h>
#include "esmrdef.h"
//</cmail>

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::CESMRIcalViewerResolveMRMethodSyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResolveMRMethodSyncCmd::CESMRIcalViewerResolveMRMethodSyncCmd(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient,
        TESMRMeetingRequestMethod& aMRMethod )
:   CESMRIcalViewerCommandBase(
        EESMRResolveMRMethodSync,
        aCalSession ),
        iMailClient( aMailClient ),
        iMRMethod( aMRMethod )
    {
    FUNC_LOG;
    iMRMethod = EESMRMeetingRequestMethodUnknown;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::~CESMRIcalViewerResolveMRMethodSyncCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResolveMRMethodSyncCmd::~CESMRIcalViewerResolveMRMethodSyncCmd()
    {
    FUNC_LOG;
    CancelCommand();
    delete iMRInfoRetrieverCmd;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::NewL
// -----------------------------------------------------------------------------
//
CESMRIcalViewerResolveMRMethodSyncCmd*
        CESMRIcalViewerResolveMRMethodSyncCmd::NewL(
                CCalSession& aCalSession,
                CFSMailClient& aMailClient,
                TESMRMeetingRequestMethod& aMRMethod )
    {
    CESMRIcalViewerResolveMRMethodSyncCmd* self =
            new (ELeave) CESMRIcalViewerResolveMRMethodSyncCmd(
                    aCalSession,
                    aMailClient,
                    aMRMethod);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodSyncCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::OperationCompleted
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodSyncCmd::OperationCompleted(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( EESMRLoadMRInfoData == aResult.iOpType )
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

        aResult.iOpType = OperationType();
        aResult.iMethod = iMRMethod;
        aResult.iMessage = Message();

        TBool isAttendee = EFalse;
        TRAP_IGNORE( isAttendee = ESMRIcalViewerUtils::IsMailboxOwnerAttendeeL(
                            *(iESMRInputParams.iCalEntry),
                            iMailClient,
                            *Message() ) );

        if ( !isAttendee )
            {
            iMRMethod = EESMRMeetingRequestMethodUnknown;
            aResult.iMethod = EESMRMeetingRequestMethodUnknown;
            }

        Observer()->OperationCompleted( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::OperationError
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodSyncCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    aResult.iOpType = OperationType();
    aResult.iMethod = iMRMethod;
    aResult.iMessage = Message();

    Observer()->OperationError( aResult );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodSyncCmd::ExecuteCommandL(
        CFSMailMessage& aMessage,
        MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    SetObserver( &aObserver );
    SetMessage( &aMessage );

    iMRInfoRetrieverCmd = CESMRInfoIcalRetrieverCmd::NewL(
                                CalSession(),
                                iESMRInputParams );

    iMRInfoRetrieverCmd->ExecuteCommandL( aMessage, *this );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerResolveMRMethodSyncCmd::CancelCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerResolveMRMethodSyncCmd::CancelCommand()
    {
    FUNC_LOG;
    if ( iMRInfoRetrieverCmd )
        {
        iMRInfoRetrieverCmd->CancelCommand();
        }
    }

// EOF

