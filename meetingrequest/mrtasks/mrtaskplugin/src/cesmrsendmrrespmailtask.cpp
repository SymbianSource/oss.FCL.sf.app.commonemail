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
* Description:  Implementation for ESMR base task
*
*/

#include "emailtrace.h"
#include "cesmrsendmrrespmailtask.h"

#include <calentry.h>
#include <caluser.h>
#include <cmrmailboxutils.h>
#include <avkon.hrh>
#include "cesmrcalimportexporter.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrhelper.h"
#include "cesmrlistquery.h"
#include "cesmrmeetingrequestsender.h"
#include "cesmrresponsedialog.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSendMRRespMailTask::CESMRSendMRRespMailTask
// ---------------------------------------------------------------------------
//
CESMRSendMRRespMailTask::CESMRSendMRRespMailTask(
        TESMRCommand aCommand,
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRResponseType aResponseType  )
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils ),
    iCommand( aCommand ),
    iResponseType( aResponseType )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespMailTask::~CESMRSendMRRespMailTask
// ---------------------------------------------------------------------------
//
CESMRSendMRRespMailTask::~CESMRSendMRRespMailTask()
    {
    FUNC_LOG;
    delete iResponseMessage;
    delete iResponseEntry;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespMailTask::NewL
// ---------------------------------------------------------------------------
//
CESMRSendMRRespMailTask* CESMRSendMRRespMailTask::NewL(
        TESMRCommand aCommand,
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRResponseType aResponseType,
        const TDesC& aResponseMessage )
    {
    FUNC_LOG;
    CESMRSendMRRespMailTask* self =
        new (ELeave) CESMRSendMRRespMailTask(
            aCommand,
            aCalDbMgr,
            aEntry,
            aMRMailboxUtils,
            aResponseType );

    CleanupStack::PushL(self);
    self->ConstructL( aResponseMessage );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespMailTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespMailTask::ConstructL(
        const TDesC& aResponseMessage )
    {
    FUNC_LOG;
    BaseConstructL();
    iResponseMessage = aResponseMessage.AllocL();
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespMailTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespMailTask::ExecuteTaskL()
    {
    FUNC_LOG;
    // Checking input paramters
    if ( EESMRResponseMessage == iResponseType &&
         !iResponseMessage->Length() )
        {
        User::Leave( KErrArgument );
        }

    if( CCalEntry::EMethodReply != ESMREntry().Entry().MethodL() )
        {
        User::Leave( KErrArgument );
        }

    if ( EESMRResponseDontSend != iResponseType )
        {
        SendResponseMessageL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespMailTask::SendResponseMessageL
// ---------------------------------------------------------------------------
//
void CESMRSendMRRespMailTask::SendResponseMessageL()
    {
    FUNC_LOG;
    CESMRMeetingRequestSender* sender =
        CESMRMeetingRequestSender::NewL( MailboxUtils() );
    CleanupStack::PushL( sender );

    if ( iResponseMessage )
        {
        sender->CreateAndSendMeetingRequestL(
                ESMREntry(),
                *iResponseMessage );
        }
    else
        {
        sender->CreateAndSendMeetingRequestL(
                ESMREntry() );
        }

    CleanupStack::PopAndDestroy( sender );
    }

