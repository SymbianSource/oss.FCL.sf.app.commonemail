/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSNotificationHandlerBase.
*
*/

#include <centralrepository.h>
//<cmail>
#include "emailtrace.h"
#include "cfsmailclient.h"
//</cmail>

#include "fsnotificationhandlermgr.h"
#include "fsnotificationhandlerbase.h"
#include "cmailhandlerpluginpanic.h"
#include "commonemailcrkeys.h"
#include "FreestyleEmailCenRepKeys.h"
#include "FreestyleEmailuiConstants.h"


// ======== MEMBER FUNCTIONS ========

CFSNotificationHandlerBase::CFSNotificationHandlerBase(
    MFSNotificationHandlerMgr& aOwner ) :
    iOwner( aOwner ),
    iObserving( EFalse )
    {
    FUNC_LOG;
    }

void CFSNotificationHandlerBase::ConstructL()
    {
    FUNC_LOG;
    }

CFSNotificationHandlerBase::~CFSNotificationHandlerBase()
    {
    FUNC_LOG;
    REComSession::DestroyedImplementation( iDestructorKey );
    }

CFSMailClient& CFSNotificationHandlerBase::MailClient() const
    {
    FUNC_LOG;
    return iOwner.MailClient();
    }
void CFSNotificationHandlerBase::EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
                                         TAny* aParam1, TAny* aParam2, TAny* aParam3 )
    {
    FUNC_LOG;
    if ( !iObserving )
        {
        return;
        }   
    
   /* TBool capabilitiesToContinue( CapabilitiesToContinueL( aEvent,
                                                          aMailbox,
                                                          aParam1,
                                                          aParam2,
                                                          aParam3) );
    if ( !capabilitiesToContinue )
        {
        return;
        }*/
    
    HandleEventL( aEvent, aMailbox, aParam1, aParam2, aParam3 );
    }


    
void CFSNotificationHandlerBase::SetObserving( TBool aNewValue )
    {
    FUNC_LOG;
    iObserving = aNewValue;
    }

TBool CFSNotificationHandlerBase::MsgIsUnread( CFSMailMessage& aMessage ) const
    {
    FUNC_LOG;
    TBool read( aMessage.IsFlagSet( EFSMsgFlag_Read ) );
    TBool read_locally( aMessage.IsFlagSet( EFSMsgFlag_Read_Locally ) );
    
    if ( !read && !read_locally )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CFSNotificationHandlerBase::MessagesCauseNotificationL( TFSMailMsgId aMailboxId,
                                                              CFSMailFolder& aParentFolder,
                                                              const RArray<TFSMailMsgId>& aMsgIdList )
    {
    FUNC_LOG;
    CFSMailMessage* newestMsg( NULL );
    TRAPD( notFoundError,
           newestMsg =
               NewestMsgInFolderL( aParentFolder ) );
    if ( notFoundError == KErrNotFound )
        {
        // For some odd reason we are not able to get the newest
        // message from the folder. This should not be possible
        // as we just received notification of a new message.
        // Possibly something has moved/deleted the message?
        return EFalse;
        }
    User::LeaveIfError( notFoundError );
    
    TTime dateOfNewest( newestMsg->GetDate() );

    delete newestMsg;
    newestMsg = NULL;

    TFSMailMsgId parentFolderId( aParentFolder.GetFolderId() );

    TInt index( 0 );
    const TInt entriesCount( aMsgIdList.Count() );
    while ( index < entriesCount )
        {
        // Let's get the message. We need to check from it that
        // it is really unread. This info is stored in the
        // flags. Also check that the message is newest.
        // EFSMsgDataEnvelope is used as TFSMailDetails 
        // so that we get enough data.
        CFSMailMessage*
            currentMessage( MailClient().GetMessageByUidL(
                aMailboxId, 
                parentFolderId,
                aMsgIdList[index], 
                EFSMsgDataEnvelope ) );
        User::LeaveIfNull( currentMessage );
        const TTime dateOfCurrentMsg( currentMessage->GetDate() );
        
        
        const TBool msgIsUnread( MsgIsUnread( *currentMessage ) );
        delete currentMessage;
        currentMessage = NULL;
        
        if ( msgIsUnread &&
             ( dateOfCurrentMsg >= dateOfNewest ) )
            {
            // At least one of the messages is unread and newest.
            return ETrue;
            }
            
        ++index;
        }
    
    return EFalse;
    }

TBool CFSNotificationHandlerBase::Observing() const
    {
    FUNC_LOG;
    return iObserving;
    }

TBool CFSNotificationHandlerBase::CapabilitiesToContinueL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* /*aParam1*/,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ ) const
    {
    FUNC_LOG;
    if ( aEvent != TFSEventMailboxDeleted )
    	{
    	CFSMailBox* mailBox( MailClient().GetMailBoxByUidL( aMailbox ) );
    	if ( mailBox == NULL )
    		{
    		User::Leave( KErrArgument );
    		}

    	if ( mailBox->HasCapability( EFSMBoxCapaNewEmailNotifications ) )
    		{
    		delete mailBox;
    		return EFalse;
    		}
    	else
    		{
    		delete mailBox;
    		return ETrue;
    		}
    	}
    else
    	{
    	return ETrue;
    	}
    }

void CFSNotificationHandlerBase::HandleEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* aParam2,
    TAny* /*aParam3*/ )
    {
    FUNC_LOG;
    // Only event TFSEventNewMail means that the mail is completely new.
    if ( aEvent == TFSEventNewMail )
        {
        // In case of TFSEventNewMail we have parent folder id
        // in aParam2
        TFSMailMsgId* parentFolderId( NULL );
        parentFolderId = static_cast< TFSMailMsgId* >( aParam2 );
        if ( parentFolderId == NULL )
            {
            User::Leave( KErrArgument );
            }
        CFSMailFolder* parentFolder(
            MailClient().GetFolderByUidL( aMailbox, *parentFolderId ) );
        User::LeaveIfNull( parentFolder );
        CleanupStack::PushL( parentFolder );
        
        // Set the notification on only in cases that the new mail is
        // in folder of type EFSInbox
        if ( parentFolder->GetFolderType() == EFSInbox )
            {
            
            RArray<TFSMailMsgId>* newEntries(
                static_cast< RArray<TFSMailMsgId>* >( aParam1 ) );

            if ( MessagesCauseNotificationL(
                     aMailbox,
                     *parentFolder,
                     *newEntries ) )
                {
                // it doesn't matter if the mailindicator won't get updated
                // its better have it not updated compared to a leave
                TRAP_IGNORE(MailIndicatorHandlingL(aMailbox));                
                }
            }
         else
            {
            // If messages are in some other folder than in inbox
            // they have no effect on the notification
            }
        CleanupStack::PopAndDestroy( parentFolder );
        }
    else
        {
        // No other events than new mail are handled. For example
        // moving of messages and changing message status has no
        // effect on the notification.
        }
    }

void CFSNotificationHandlerBase::MailIndicatorHandlingL(TFSMailMsgId aMailbox)
    {
    /** 
     * There is always a pair of keys for one mailbox
     * 1st keys is the plugin id (numberOfMailboxes*2-1)
     * 2nd is the mailboxId (numberOfMailboxes*2)
     */
    TInt numberOfMailboxes(0);
    CRepository* emailRepository = CRepository::NewL( KFreestyleEmailCenRep );
    CleanupStack::PushL(emailRepository);
    emailRepository->Get(KNumberOfMailboxesWithNewEmails, numberOfMailboxes);
    CRepository* commonEmailRepository = CRepository::NewL( KCmailDataRepository );
    CleanupStack::PushL(commonEmailRepository);
    if(numberOfMailboxes != 0)
        {
        RArray<TInt> repositoryIds;
        CleanupClosePushL(repositoryIds);
        TInt tmp(0);
        for(TInt i = 1 ; i <= numberOfMailboxes * 2; i++ )
            {                        
            emailRepository->Get(KNumberOfMailboxesWithNewEmails+i, tmp);
            repositoryIds.Append(tmp);
            }                    
        TInt index = repositoryIds.Find(static_cast<TInt>(aMailbox.Id()));
        // If it is we delete it from the array
        if(index != KErrNotFound )
            {
            //unless it is the last item, then we don't have to do anything
            if(index != (repositoryIds.Count()-1))
                {
                repositoryIds.Remove(index);
                repositoryIds.Remove(index-1);               
                // Rearrange the mailbox/mailplugin ids
                // in the repository
                for(TInt j = 0; j < repositoryIds.Count(); j++)
                    {
                    emailRepository->Set(KNumberOfMailboxesWithNewEmails + 1 + j, repositoryIds.operator [](j));
                    }       
                // And write the current mailbox/plugin ids back to repository as the last items
                emailRepository->Set(KNumberOfMailboxesWithNewEmails + ( numberOfMailboxes * 2 - 1 ), static_cast<TInt>(aMailbox.PluginId().iUid));
                emailRepository->Set(KNumberOfMailboxesWithNewEmails + ( numberOfMailboxes * 2 ),static_cast<TInt>(aMailbox.Id()) );
                commonEmailRepository->Set(KCmailNewEmailDisplayText,MailClient().GetMailBoxByUidL(aMailbox)->GetName());
                }
            }
        else
            {
            //Create keys with right values and update the number of mailboxes
            numberOfMailboxes += 1;
            emailRepository->Set(KNumberOfMailboxesWithNewEmails, numberOfMailboxes);
            emailRepository->Create(KNumberOfMailboxesWithNewEmails + (numberOfMailboxes * 2 - 1), static_cast<TInt>(aMailbox.PluginId().iUid));
            emailRepository->Create(KNumberOfMailboxesWithNewEmails + (numberOfMailboxes * 2), static_cast<TInt>(aMailbox.Id()));  
            commonEmailRepository->Set(KCmailNewEmailDisplayText,MailClient().GetMailBoxByUidL(aMailbox)->GetName());
            }
        repositoryIds.Reset();
        CleanupStack::PopAndDestroy();//repositoryIds
        }
    else
        {
        //Create keys with right values and pudate the number of mailboxes
        numberOfMailboxes += 1;
        emailRepository->Set(KNumberOfMailboxesWithNewEmails, numberOfMailboxes);
        emailRepository->Create(KNumberOfMailboxesWithNewEmails + (numberOfMailboxes * 2 - 1), static_cast<TInt>(aMailbox.PluginId().iUid));
        emailRepository->Create(KNumberOfMailboxesWithNewEmails + (numberOfMailboxes * 2), static_cast<TInt>(aMailbox.Id()));
        commonEmailRepository->Set(KCmailNewEmailDisplayText,MailClient().GetMailBoxByUidL(aMailbox)->GetName());
        }
    CleanupStack::PopAndDestroy(2);// emailRepository, CommonEmailRepository    
    TurnNotificationOn();
    }

CFSMailMessage* CFSNotificationHandlerBase::NewestMsgInFolderL(
    /*const*/ CFSMailFolder& aFolder ) const
    {
    FUNC_LOG;
    // Load info only necessary for sorting by date into the messages.
    TFSMailDetails details( EFSMsgDataDate );

    // Want to sort mails so that the newest is in the beginning
    TFSMailSortCriteria criteriaDate;
    criteriaDate.iField = EFSMailSortByDate;
    criteriaDate.iOrder = EFSMailDescending;

    RArray<TFSMailSortCriteria> sorting;
    CleanupClosePushL( sorting );
    // First criteria appended would be the primary criteria
    // but here we don't have any other criterias
    sorting.Append( criteriaDate );
    MFSMailIterator* iterator = aFolder.ListMessagesL( details, sorting );
    
    // Resetting array of sort criterias already here because
    // the iterator does not need it anymore.
    CleanupStack::PopAndDestroy(); // sorting
                                    
    // CleanupStack::PushL doesn't work for M-class
    CleanupDeletePushL( iterator ); 
    
    RPointerArray<CFSMailMessage> messages;
    CleanupClosePushL( messages );
    // Let's get only the first and therefore the newest message.
    TInt amount( 1 );
    iterator->NextL( TFSMailMsgId(), amount, messages );    
    if ( messages.Count() < 1 )
        {
        messages.ResetAndDestroy();
        User::Leave( KErrNotFound );
        }
            
    CFSMailMessage* outcome = messages[0];
    messages.Remove( 0 ); // remove from array to prevent destruction of element
    messages.ResetAndDestroy();
    CleanupStack::PopAndDestroy(); // messages
    CleanupStack::PopAndDestroy( iterator );
    return outcome;
    }

void Panic( TCmailhandlerPanic aPanic )
    {
    _LIT( KPanicText, "emailhandlerplugin" );
    User::Panic( KPanicText, aPanic );
    }

// End of file

