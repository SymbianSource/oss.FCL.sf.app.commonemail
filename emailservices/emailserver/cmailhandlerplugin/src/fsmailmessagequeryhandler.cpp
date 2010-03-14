/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSMailMessageQueryHandler.
*
*/


//<cmail>
#include "emailtrace.h"
#include "cfsmailclient.h"
//</cmail>

#include "fsmailmessagequeryhandler.h"


CFSMailMessageQueryHandler* CFSMailMessageQueryHandler::NewL(
    MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
    CFSMailMessageQueryHandler* self =
        new (ELeave) CFSMailMessageQueryHandler( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CFSMailMessageQueryHandler::CFSMailMessageQueryHandler(
    MFSNotificationHandlerMgr& aOwner ) :
    CFSNotificationHandlerBase( aOwner )
    {
    FUNC_LOG;
    }

void CFSMailMessageQueryHandler::ConstructL()
    {
    FUNC_LOG;
    CFSNotificationHandlerBase::ConstructL();

    // We are now ready to observe mail events
    SetObserving( ETrue );
    }

CFSMailMessageQueryHandler::~CFSMailMessageQueryHandler()
    {
    FUNC_LOG;
    iNotifierInitiators.ResetAndDestroy();
    }

void CFSMailMessageQueryHandler::NotifierInitiatorDoneL(
    CFSNotificationHandlerNotifierInitiator* aInitiator )
    {
    FUNC_LOG;
    
    TInt initiatorIndexer( FindInitiator( aInitiator ) );
   
    if ( initiatorIndexer == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
        
    iNotifierInitiators.Remove( initiatorIndexer );
    CleanupStack::PushL( aInitiator );
    
    MFSMailExceptionEventCallback* callback = aInitiator->Callback();
    if( callback )
        {
        TFSMailMsgId mailboxId = aInitiator->MailBoxId();
        TFsEmailNotifierSystemMessageType msgType = aInitiator->MessageType();
        TBool response = aInitiator->GetLastResponse();
        callback->ExceptionEventCallbackL( mailboxId, msgType, response );
        }
    
    CleanupStack::PopAndDestroy( aInitiator );
    }

TBool CFSMailMessageQueryHandler::CapabilitiesToContinueL(
    TFSMailEvent /*aEvent*/,
    TFSMailMsgId /*aMailbox*/,
    TAny* /*aParam1*/,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ ) const
    {
    FUNC_LOG;
    // Authentication query is always shown by instance of this class
    // if necessary.
    return ETrue;
    }

/*
static const TInt KTestCountMin( 0 );
static const TInt KTestCountMax( 3 );
_LIT( KTestText, "Very fancy test text." );
*/

void CFSMailMessageQueryHandler::HandleEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* aParam2,
    TAny* aParam3 )
    {
    FUNC_LOG;
    switch ( aEvent )
        {
        case TFSEventException:
            {
            TFsEmailNotifierSystemMessageType& msgType = *static_cast<TFsEmailNotifierSystemMessageType*>( aParam1 );
            TDesC& msgText = *static_cast<TDesC*>( aParam2 );
            MFSMailExceptionEventCallback* callback = static_cast<MFSMailExceptionEventCallback*>( aParam3 );
            
            
            ShowMessageQueryL( aMailbox, msgType, msgText, callback );
            
            break;
            }
        /*
        case TFSEventMailboxOnline:
            {
            // ONLY FOR TESTING
            static TInt testCounter( KTestCountMin );

            testCounter++;
            if( testCounter > KTestCountMax )
                {
                testCounter = KTestCountMin;
                }

            TFsEmailNotifierSystemMessageType msgType = static_cast<TFsEmailNotifierSystemMessageType>( testCounter );
            
            ShowMessageQueryL( aMailbox, msgType, KTestText(), NULL );
            
            break;
            }
            */
        default:
            {
            break;
            }
        }
    }


void CFSMailMessageQueryHandler::ShowMessageQueryL(
        TFSMailMsgId aMailbox,
        TFsEmailNotifierSystemMessageType aMessageType,
        const TDesC& aCustomMessageText,
        MFSMailExceptionEventCallback* aCallback )
    {
    FUNC_LOG;
    
    TInt index( FindInitiator( aMailbox, aMessageType ) );
    
    // Only one message query is permitted for one mailbox-message type
    // combination. So one mailbox can have several message query dialogs
    // open, if they are not of the same type. On the other hand, two
    // or more mailboxes can show the same message.
    if ( index == KErrNotFound )
        {

        CFSMailBox* mailBox(
            MailClient().GetMailBoxByUidL( aMailbox ) );
        User::LeaveIfNull( mailBox );
        CleanupStack::PushL( mailBox );
        TDesC& mailboxName( mailBox->GetName() );
        
        CFSNotificationHandlerNotifierInitiator* newNotifierInitiator =
            CFSNotificationHandlerNotifierInitiator::NewL( *this, iOwner );
        CleanupStack::PushL( newNotifierInitiator );

        newNotifierInitiator->SetMailBoxId( aMailbox );
        newNotifierInitiator->SetMessageType( aMessageType );
        newNotifierInitiator->SetCallback( aCallback );

        newNotifierInitiator->RequestMessageQueryL(
                mailboxName,
                aMessageType,
                aCustomMessageText );
        
        iNotifierInitiators.AppendL( newNotifierInitiator );
        CleanupStack::Pop( newNotifierInitiator );
        CleanupStack::PopAndDestroy( mailBox );
        }
    else
        {
        // notifierInitiator exists so we are already showing
        // this message query for this mailbox. Let's not
        // show/queue a new one.      
        }
    }


TInt CFSMailMessageQueryHandler::FindInitiator(
    CFSNotificationHandlerNotifierInitiator* aInitiator ) const
    {
    FUNC_LOG;
    return iNotifierInitiators.Find( aInitiator );
    }

TInt CFSMailMessageQueryHandler::FindInitiator(
    TFSMailMsgId aMailBoxId,
    TFsEmailNotifierSystemMessageType aMessageType ) const
    {
    FUNC_LOG;
    TInt initiatorCount( iNotifierInitiators.Count() );
    TInt initiatorIndexer( 0 );
    
    while ( initiatorIndexer < initiatorCount )
        {
        if( iNotifierInitiators[initiatorIndexer]->MailBoxId() == aMailBoxId &&
            iNotifierInitiators[initiatorIndexer]->MessageType() == aMessageType )
            {
            return initiatorIndexer;
            }
        
        ++initiatorIndexer;
        }
    
    return KErrNotFound;
    }


void CFSMailMessageQueryHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    }

void CFSMailMessageQueryHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    }

