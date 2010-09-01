/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class to handle mail led turning on.
*
*/


//<cmail>
#include "emailtrace.h"
#include "cfsmailclient.h"
//</cmail>
#include <aknlists.h>
#include <e32cmn.h>
#include <f32file.h>
#include <bautils.h>
#include <StringLoader.h>

#include <fsmailserver.rsg>

#include "fsmailoutofmemoryhandler.h"
#include "fsnotificationhandlertimer.h"

const TInt32 KReshowNoteDelay = 60000000;  //60 seconds


CFSMailOutOfMemoryHandler* CFSMailOutOfMemoryHandler::NewL(
    MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
    CFSMailOutOfMemoryHandler* self =
        new (ELeave) CFSMailOutOfMemoryHandler( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CFSMailOutOfMemoryHandler::CFSMailOutOfMemoryHandler(
    MFSNotificationHandlerMgr& aOwner ) :
    CFSNotificationHandlerBase( aOwner )
    {
    FUNC_LOG;
    }

void CFSMailOutOfMemoryHandler::ConstructL()
    {
    FUNC_LOG;
    CFSNotificationHandlerBase::ConstructL();

    iTimer = CFSNotificationHandlerTimer::NewL( *this );

    // We are now ready to observe mail events
    SetObserving( ETrue );
    }

CFSMailOutOfMemoryHandler::~CFSMailOutOfMemoryHandler()
    {
    FUNC_LOG;
    delete iNotifierInitiator;
    iNotifierInitiator = NULL;
    
    delete iTimer;
    iTimer = NULL;
    }

void CFSMailOutOfMemoryHandler::NotifierInitiatorDoneL(
        CFSNotificationHandlerNotifierInitiator* aInitiator )
    {
    FUNC_LOG;
    if ( aInitiator != iNotifierInitiator )
        {
        User::Leave( KErrNotFound );
        }
        
    delete iNotifierInitiator;
    iNotifierInitiator = NULL;
    
    // User has just seen and accepted the note. Let's wait
    // a proper time before showing the note again.
    
    SetObserving( EFalse );
    //cancel the current timer
    iTimer->Cancel();
    //start a new timer
    iTimer->After( KReshowNoteDelay );
    }

// ---------------------------------------------------------------------------
// Method to be called by when the timer expires.
// ---------------------------------------------------------------------------
//
void CFSMailOutOfMemoryHandler::TimerCallBackL( TInt aError )
    {
    FUNC_LOG;
    if ( aError == KErrNone )
        {
        //enable note showing again
        SetObserving( ETrue );
        }
    }

TBool CFSMailOutOfMemoryHandler::CapabilitiesToContinueL(
    TFSMailEvent /*aEvent*/,
    TFSMailMsgId /*aMailbox*/,
    TAny* /*aParam1*/,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ ) const
    {
    FUNC_LOG;
    // Out of memory is always shown by instance of this class.
    return ETrue;
    }

void CFSMailOutOfMemoryHandler::HandleEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ )
    {
    FUNC_LOG;
    switch ( aEvent )
        {
        case TFSEventMailboxSyncStateChanged:
            {
            TSSMailSyncState& state = *static_cast<TSSMailSyncState*>( aParam1 );
            
            
            if ( state == OutOfDiskSpace )
                {
                ShowOutOfMemoryNoteL( aMailbox );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }


void CFSMailOutOfMemoryHandler::ShowOutOfMemoryNoteL(
        TFSMailMsgId aMailboxId )
    {
    FUNC_LOG;
    
    if ( !iNotifierInitiator )
        {

        CFSMailBox* mailBox(
            MailClient().GetMailBoxByUidL( aMailboxId ) );
        User::LeaveIfNull( mailBox );
        CleanupStack::PushL( mailBox );
        TDesC& mailboxName( mailBox->GetName() );

        iNotifierInitiator = CFSNotificationHandlerNotifierInitiator::NewL(
                *this, iOwner );
        iNotifierInitiator->RequestMessageQueryL(
                mailboxName,
                EFsEmailNotifErrOutOfMemory );
        
        CleanupStack::PopAndDestroy( mailBox );
        }
    else
        {
        // iEmailNotifierInitiator exists so we are already showing
        // an error note. Let's not show a new one.      
        }
    }

void CFSMailOutOfMemoryHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    }

void CFSMailOutOfMemoryHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    }

