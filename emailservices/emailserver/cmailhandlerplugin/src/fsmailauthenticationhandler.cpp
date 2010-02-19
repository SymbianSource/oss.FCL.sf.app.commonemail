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

#include "fsmailauthenticationhandler.h"



CFSMailAuthenticationHandler* CFSMailAuthenticationHandler::NewL(
    MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
    CFSMailAuthenticationHandler* self =
        new (ELeave) CFSMailAuthenticationHandler( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CFSMailAuthenticationHandler::CFSMailAuthenticationHandler(
    MFSNotificationHandlerMgr& aOwner ) :
    CFSNotificationHandlerBase( aOwner )
    {
    FUNC_LOG;
    }

void CFSMailAuthenticationHandler::ConstructL()
    {
    FUNC_LOG;
    CFSNotificationHandlerBase::ConstructL();

    // We are now ready to observe mail events
    SetObserving( ETrue );
    }

CFSMailAuthenticationHandler::~CFSMailAuthenticationHandler()
    {
    FUNC_LOG;
    iNotifierInitiators.ResetAndDestroy();
    }

void CFSMailAuthenticationHandler::NotifierInitiatorDoneL(
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
    
    // User has just given new authentication values. Pass them to
    // framework.
    
    CFSMailBox* mailBox(
        MailClient().GetMailBoxByUidL( aInitiator->MailBoxId() ) );
    User::LeaveIfNull( mailBox );
    CleanupStack::PushL( mailBox );
    
    TBuf<KFsEmailNotifierPasswordMaxLength> newPassword;
    aInitiator->GetLastAuthenticationOutput( newPassword );
    
    mailBox->SetCredentialsL( KNullDesC, newPassword );
    
    CleanupStack::PopAndDestroy( mailBox );
    CleanupStack::PopAndDestroy( aInitiator );
    }

TBool CFSMailAuthenticationHandler::CapabilitiesToContinueL(
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

void CFSMailAuthenticationHandler::HandleEventL(
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
            
            
            if ( state == PasswordExpiredError ||
                 state == PasswordNotVerified )
                {
                ShowAuthenticationQueryL( aMailbox );
                }
                
            break;
            }
            /*
        case TFSEventMailboxOffline:
            {
            // ONLY FOR TESTING
            
            ShowAuthenticationQueryL( aMailbox );
                
            break;
            }
            */
        default:
            {
            break;
            }
        }
    }


void CFSMailAuthenticationHandler::ShowAuthenticationQueryL(
    TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    
    TInt index( FindInitiator( aMailbox ) );
    
    // Only one authentication query is permitted for one mailbox
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
        newNotifierInitiator->RequestAuthenticationL(
            mailboxName );
        
        iNotifierInitiators.AppendL( newNotifierInitiator );
        CleanupStack::Pop( newNotifierInitiator );
        CleanupStack::PopAndDestroy( mailBox );
        }
    else
        {
        // notifierInitiator exists so we are already showing
        // an authentication query for the mailbox. Let's not
        // show/queue a new one.      
        }
    }


TInt CFSMailAuthenticationHandler::FindInitiator(
    CFSNotificationHandlerNotifierInitiator* aInitiator ) const
    {
    FUNC_LOG;
    return iNotifierInitiators.Find( aInitiator );
    }

TInt CFSMailAuthenticationHandler::FindInitiator(
    TFSMailMsgId aMailBoxId ) const
    {
    FUNC_LOG;
    TInt initiatorCount( iNotifierInitiators.Count() );
    TInt initiatorIndexer( 0 );
    
    while ( initiatorIndexer < initiatorCount )
        {
        if( iNotifierInitiators[initiatorIndexer]->MailBoxId()
                == aMailBoxId )
            {
            return initiatorIndexer;
            }
        
        ++initiatorIndexer;
        }
    
    return KErrNotFound;
    }


void CFSMailAuthenticationHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    }

void CFSMailAuthenticationHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    }

