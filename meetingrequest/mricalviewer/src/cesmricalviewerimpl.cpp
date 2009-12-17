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
* Description:  ESMR UI launcher object definition
*
*/

#include "emailtrace.h"
#include "cesmricalviewerimpl.h"
#include "cesmricalviewercmdhandler.h"

//<cmail>
#include "mesmricalviewercallback.h"
#include "esmricalvieweropcodes.hrh"
#include "esmricalviewerdefs.hrh"
#include "esmrdef.h"
//</cmail>
#include <eikenv.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRIcalViewer::CESMRIcalViewer
// ---------------------------------------------------------------------------
//
CESMRIcalViewerImpl::CESMRIcalViewerImpl(
        MESMRIcalViewerCallback& aCallback )
:   iCallback( aCallback )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::CESMRIcalViewer
// ---------------------------------------------------------------------------
//
CESMRIcalViewerImpl::~CESMRIcalViewerImpl()
    {
    FUNC_LOG;
    delete iCmdHandler;
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::NewL
// ---------------------------------------------------------------------------
//
CESMRIcalViewerImpl* CESMRIcalViewerImpl::NewL(
            TAny* aCallback )
    {
    FUNC_LOG;
    MESMRIcalViewerCallback* callback =
            static_cast<MESMRIcalViewerCallback*>( aCallback );

    CESMRIcalViewerImpl* self = new (ELeave) CESMRIcalViewerImpl( *callback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRIcalViewerImpl::ConstructL()
    {
    FUNC_LOG;
    iCmdHandler = CESMRIcalViewerCmdHandler::NewL( iCallback );
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::CanViewMessage
// ---------------------------------------------------------------------------
//
TBool CESMRIcalViewerImpl::CanViewMessage(
      CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    TBool canViewMessage( EFalse );

    TRAPD( error, iCmdHandler->ExecuteCommandL(
            EESMRCanViewMR,
            NULL,
            aMessage,
            &canViewMessage ) );

    if ( error != KErrNone )
        {
        CEikonEnv::Static()-> // codescanner::eikonenvstatic
            HandleError( error );
        }

    return canViewMessage;
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::ExecuteViewL
// ---------------------------------------------------------------------------
//
void CESMRIcalViewerImpl::ExecuteViewL(
      CFSMailMessage& aMessage,
      MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    iCmdHandler->ExecuteCommandL(
            EESMRViewLaunch,
            &aObserver,
            aMessage );
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::ResponseToMeetingRequestL
// ---------------------------------------------------------------------------
//
void CESMRIcalViewerImpl::ResponseToMeetingRequestL(
      TESMRAttendeeStatus aAttendeeStatus,
      CFSMailMessage& aMessage,
      MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    iCmdHandler->ExecuteCommandL(
            EESMRResponseToMR,
            &aObserver,
            aMessage,
            &aAttendeeStatus );
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::RemoveMeetingRequestFromCalendarL
// ---------------------------------------------------------------------------
//
void CESMRIcalViewerImpl::RemoveMeetingRequestFromCalendarL(
      CFSMailMessage& aMessage,
      MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    iCmdHandler->ExecuteCommandL(
            EESMRRemoveMR,
            &aObserver,
            aMessage );
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::ResolveMeetingRequestMethodL
// ---------------------------------------------------------------------------
//
void CESMRIcalViewerImpl::ResolveMeetingRequestMethodL(
      CFSMailMessage& aMessage,
      MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    iCmdHandler->ExecuteCommandL(
            EESMRResolveMRMethod,
            &aObserver,
            aMessage );
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::ResolveMeetingRequestMethodL
// ---------------------------------------------------------------------------
//
TESMRMeetingRequestMethod CESMRIcalViewerImpl::ResolveMeetingRequestMethodL(
      CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    TESMRMeetingRequestMethod mrMethod;

    iCmdHandler->ExecuteCommandL(
            EESMRResolveMRMethodSync,
            NULL,
            aMessage,
            &mrMethod );

    return mrMethod;
    }

// ---------------------------------------------------------------------------
// CESMRIcalViewer::CESMRIcalViewer
// ---------------------------------------------------------------------------
//
void CESMRIcalViewerImpl::CancelOperation()
    {
    FUNC_LOG;
    iCmdHandler->CancelOperation();
    }

// EOF

