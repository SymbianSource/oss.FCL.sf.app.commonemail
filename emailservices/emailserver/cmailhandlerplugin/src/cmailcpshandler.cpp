/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class to handle content publishing for widget
*
*/

#include <cmailhandlerplugin.mbg>
#include <ecom/ecom.h>
#include <StringLoader.h>
#include <emailwidget.rsg>
#include <AknUtils.h>
#include <apgcli.h>
#include <centralrepository.h>
#include <starterdomaincrkeys.h>

#include "emailtrace.h"
#include "CFSMailClient.h"
#include "CFSMailBox.h"
#include "CFSMailFolder.h"
#include "cmailcpshandler.h"
#include "cmailcpssettings.h"
#include "cmailcpsif.h"
#include "cmailmessagedetails.h"
#include "cmailmailboxdetails.h"
#include "cmailcpsifconsts.h"
#include "FreestyleEmailUiConstants.h"

// ---------------------------------------------------------
// CMailCpsHandler::CMailCpsHandler
// ---------------------------------------------------------
//
CMailCpsHandler::CMailCpsHandler( MFSNotificationHandlerMgr& aOwner ) : CFSNotificationHandlerBase( aOwner )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CMailCpsHandler::NewL
// ---------------------------------------------------------
//
CMailCpsHandler* CMailCpsHandler::NewL( MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
    CMailCpsHandler* self = new(ELeave) CMailCpsHandler( aOwner );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMailCpsHandler::ConstructL
// ---------------------------------------------------------
//
void CMailCpsHandler::ConstructL()
    {
    FUNC_LOG;

    CFSNotificationHandlerBase::ConstructL();
    
    iEnv = CEikonEnv::Static( );
    
    SetObserving( ETrue );

    // Instantiate helper classes
    // Leaves if ContentPublishing service not found
    iLiwIf = CMailCpsIf::NewL(this);
    // Leaves if CenRep file not found
    iSettings = CMailCpsSettings::NewL( MailClient() );

    InitializeL();
    
    iSettings->StartObservingL( this );
    }

// ---------------------------------------------------------
// CMailCpsHandler::~CMailCpsHandler
// ---------------------------------------------------------
//
CMailCpsHandler::~CMailCpsHandler()
    {
    FUNC_LOG;
    delete iLiwIf;
    delete iSettings;
    iAccountsArray.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CMailCpsHandler::Reset
// ---------------------------------------------------------
//
void CMailCpsHandler::Reset()
    {
    FUNC_LOG;
    iAccountsArray.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CMailCpsHandler::InitializeL
// ---------------------------------------------------------
//
void CMailCpsHandler::InitializeL()
    {
    FUNC_LOG;
    // Get current configuration from settings interface,
    // and pass it on to actual publisher interface
    iLiwIf->SetConfiguration( iSettings->Configuration() );
    
    TInt iiMax( iSettings->Mailboxes().Count() );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        CFSMailBox* mailbox( NULL );
        mailbox = MailClient().GetMailBoxByUidL( iSettings->Mailboxes()[ii] );

        TInt mailboxId(0);
        if (mailbox)
            {
            mailboxId = mailbox->GetId().Id();
            }
        if ( !mailboxId )
            {
            // Remove box from settings
            iSettings->RemoveMailboxL( ii );
            // Array indexing changes now since one item was deleted
            ii--;
            }
        else
            {
            CleanupStack::PushL( mailbox );
            CMailMailboxDetails* mailboxDetails = CreateMailboxDetailsL( *mailbox );
            CleanupStack::PushL( mailboxDetails );
            
            TBuf<KMaxDescLen> cid;
            TInt next(1);

            // Check if same mailbox is already in iAccountsArray once or more
            for ( TInt i = 0; i < iAccountsArray.Count(); i++ )
                {               
                TInt id = (TInt)iAccountsArray[i]->iMailboxId.Id();
                if (id == mailboxId)
                    {
                    next++;
                    }
                }
            iSettings->GetContentId(mailboxId, next, cid);          
            mailboxDetails->SetWidgetInstance(cid);
            iAccountsArray.AppendL( mailboxDetails );
            CleanupStack::Pop( mailboxDetails );                            
            CleanupStack::PopAndDestroy( mailbox );            
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::CreateMailboxDetailsL
// ---------------------------------------------------------
//
CMailMailboxDetails* CMailCpsHandler::CreateMailboxDetailsL( CFSMailBox& aMailbox )
    {
    FUNC_LOG;
    // Create instance to return
    CMailMailboxDetails* details = 
        CMailMailboxDetails::NewL( aMailbox.GetId(), aMailbox.GetName() );
    return details;
    }

// ---------------------------------------------------------
// CMailCpsHandler::SettingsChangedCallback
// ---------------------------------------------------------
//
void CMailCpsHandler::SettingsChangedCallback()
    {
    FUNC_LOG;
    Reset();
    // Trying to keep callback interface non-leaving
    TRAP_IGNORE( InitializeL() );
    // Update widget contents after settings change
    TRAP_IGNORE( UpdateFullL() );
    }

// ---------------------------------------------------------
// CMailCpsHandler::UpdateFullL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateFullL()
    {
    FUNC_LOG;
    for (TInt instance = 0; instance < iLiwIf->GetWidgetInstanceCount(); instance++)
        {
        if ( iLiwIf->AllowedToPublish(instance) )
            {
            UpdateMailboxesL(instance, iLiwIf->iInstIdList[instance]->Des());
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::UpdateMailboxesL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateMailboxesL(TInt aInstance, const TDesC& aContentId)
    {
    FUNC_LOG;
    TInt row(1); // start from first row
    TInt mailbox(0);
    
    for ( mailbox = 0; mailbox < iAccountsArray.Count(); mailbox++ )
        {           
        TInt compare = aContentId.Compare(*iAccountsArray[mailbox]->iWidgetInstance);
        if (!compare)
            {
            break;
            }
        }

    // Update fields from left to right
    UpdateMailBoxIconL( mailbox, aInstance, row );
    UpdateMailboxNameL( mailbox, aInstance, row );
	UpdateIndicatorIconL( mailbox, aInstance, row );
	row++;
	UpdateMessagesL( mailbox, aInstance, 1, row);
	row++;
	UpdateMessagesL( mailbox, aInstance, 2, row);
    }

// ---------------------------------------------------------
// CMailCpsHandler::UpdateMailboxNameL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateMailboxNameL( const TInt aMailBoxNumber,
                                          const TInt aWidgetInstance,
                                          const TInt aRowNumber )
    {
    FUNC_LOG;
    TFSMailMsgId mailBoxId;
    TFSMailMsgId folderId;
    TBool mailBoxFound(EFalse);
    if ( aMailBoxNumber < iAccountsArray.Count() )
        {
        mailBoxId = iAccountsArray[aMailBoxNumber]->iMailboxId;
        CFSMailBox* mailbox( NULL );
        mailbox = MailClient().GetMailBoxByUidL( mailBoxId );
        
        if(mailbox)
            {
            mailBoxFound = ETrue;
            TFSMailMsgId folderId( mailbox->GetStandardFolderId( EFSInbox ) );
            delete mailbox;

            TBuf<KMaxDescLen> accountName;
            accountName.Append(*iAccountsArray[aMailBoxNumber]->iMailboxName);
                                    
            TInt unreadCount(GetUnreadCountL(iAccountsArray[aMailBoxNumber]->iMailboxId));
            if ( unreadCount > 0 )
                {
                // Use localisation format when displaying also unread messages

                // Arrays must be used when loc string contains indexed parameters
                CDesCArrayFlat* strings = new CDesCArrayFlat( 1 );
                CleanupStack::PushL( strings );
                strings->AppendL( accountName ); // replace "%0U" with mailbox name
            
                CArrayFix<TInt>* ints = new(ELeave) CArrayFixFlat<TInt>( 1 );
                CleanupStack::PushL( ints );
                ints->AppendL( unreadCount ); // replace "%1N" with number of unread messages
            
                HBufC* mailboxAndCount = StringLoader::LoadLC( R_EMAILWIDGET_TEXT_MAILBOX_AND_MAILCOUNT, 
                                                               *strings,
                                                               *ints );
            
                iLiwIf->PublishActiveMailboxNameL(
				        aWidgetInstance,
                        aRowNumber, *mailboxAndCount,
                        iAccountsArray.Count(),
                        mailBoxId, folderId); 
            
                CleanupStack::PopAndDestroy(mailboxAndCount);
                CleanupStack::PopAndDestroy(ints);
                CleanupStack::PopAndDestroy(strings);
                }
            else
                {
                // show only mailbox name
            
                iLiwIf->PublishActiveMailboxNameL(
				        aWidgetInstance,
                        aRowNumber, accountName,
                        iAccountsArray.Count(),
                        mailBoxId, folderId);  
                }
            }
        }
    else if(!mailBoxFound)
        {
        // Publishing empty descriptor for rows that should not contain
        // anything on widget UI.
        iLiwIf->PublishActiveMailboxNameL( aWidgetInstance, 
		                                   aRowNumber,
										   KNullDesC,
										   NULL,
										   mailBoxId,
										   folderId );
        }
    }


// ---------------------------------------------------------
// CMailCpsHandler::UpdateMessagesL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateMessagesL( const TInt aMailBoxNumber,
                                       const TInt aWidgetInstance,
                                       const TInt aMessageNumber,
                                       const TInt aRow )
    {
    FUNC_LOG;    

    TBool mailBoxFound(EFalse);
    if ( aMailBoxNumber < iAccountsArray.Count() )
        {
        TFSMailMsgId mailBoxId;
        mailBoxId = iAccountsArray[aMailBoxNumber]->iMailboxId;

        CFSMailBox* mailbox( NULL );
        mailbox = MailClient().GetMailBoxByUidL( mailBoxId );
        if(mailbox)
            {
            mailBoxFound = ETrue;
            TFSMailMsgId parentFolder( mailbox->GetStandardFolderId( EFSInbox ) );
            // Check that folder is correct
            CFSMailFolder* folder = MailClient().GetFolderByUidL( mailBoxId, parentFolder );
            if ( !folder )
                {
				UpdateEmptyMessagesL( aWidgetInstance, aRow );
                return;
                }
            CleanupStack::PushL( folder );

            TInt msgCount = folder->GetMessageCount();
            if(msgCount<1)
                {
                UpdateEmptyMessagesL( aWidgetInstance, aRow );
                CleanupStack::PopAndDestroy( folder );
                return;
                }

            folder->SetFolderType(EFSInbox);

            // Update folder if provided, otherwise use current folder
            RPointerArray<CFSMailMessage> folderMessages(1);
            CleanupClosePushL( folderMessages );

            TFSMailDetails details( EFSMsgDataEnvelope );
            RArray<TFSMailSortCriteria> sorting;
            CleanupClosePushL( sorting );
    
            TFSMailSortCriteria sortCriteria;
            sortCriteria.iField = EFSMailSortByDate;
            sortCriteria.iOrder = EFSMailDescending;
            sorting.Append( sortCriteria );
            // List all or maximum number of messages   
            MFSMailIterator* iterator = folder->ListMessagesL( details, sorting );
            CleanupDeletePushL( iterator ); // standard CleanupStack::PushL does not work with non-C-class pointer  
        
            TFSMailMsgId dummy;
            iterator->NextL( dummy, aMessageNumber, folderMessages);
            TInt count (folderMessages.Count());
            if(!count)
                {
                UpdateEmptyMessagesL( aWidgetInstance, aRow );
            
                CleanupStack::PopAndDestroy( iterator );
                CleanupStack::PopAndDestroy( &sorting );
                CleanupStack::PopAndDestroy( &folderMessages );
                CleanupStack::PopAndDestroy( folder );

                return;
                }
            TFSMailMsgId msgId = folderMessages[aMessageNumber - 1]->GetMessageId();

            CFSMailMessage* msg( NULL );
            msg = MailClient().GetMessageByUidL( mailBoxId, parentFolder, msgId, EFSMsgDataEnvelope );
            CleanupDeletePushL( msg );

            // Get sender information
            TBuf<KMaxDescLen> sender( NULL );
            CFSMailAddress* fromAddress = msg->GetSender();
            if ( fromAddress )
                {
                if ( IsValidDisplayName(fromAddress->GetDisplayName()) )
                    {
                    sender.Append(fromAddress->GetDisplayName());
                    }
                else
                    {
                    sender.Append(fromAddress->GetEmailAddress());
                    }
                }
            
            // Get message time and format it correctly 
            TTime msgTime = msg->GetDate();
            HBufC* timeString( NULL );
            timeString = GetMessageTimeStringL( msgTime );
            CleanupDeletePushL( timeString );     
        
            // Publish message details           
            PublishMessageL( aWidgetInstance, 
                              *msg,
                              sender,
                              *timeString,
                              aRow );
        
            CleanupStack::PopAndDestroy( timeString );
            CleanupStack::PopAndDestroy( msg );
            CleanupStack::PopAndDestroy( iterator );
            CleanupStack::PopAndDestroy( &sorting );
            CleanupStack::PopAndDestroy( &folderMessages );
            CleanupStack::PopAndDestroy( folder );
            }
        }
    else if(!mailBoxFound)
        {
        UpdateEmptyMessagesL( aWidgetInstance, aRow );
        }
    }


// ---------------------------------------------------------
// CMailCpsHandler::GetMessageTimeStringL
// ---------------------------------------------------------
//
HBufC* CMailCpsHandler::GetMessageTimeStringL( TTime aMessageTime )
    {
    FUNC_LOG;

    // Get current local time
    TTime currentTime;
    currentTime.HomeTime();
    
    // Get universal time offset and add it to message time
    TLocale locale;    
    TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );
    aMessageTime += universalTimeOffset;      
    
    // If daylight saving is on, add one hour offset to message time
    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        TTimeIntervalHours daylightSaving( 1 );
        aMessageTime += daylightSaving;
        }
    
    // Get datetime objects and compare dates
    TDateTime currentDate = currentTime.DateTime();
    TDateTime messageDate = aMessageTime.DateTime();
    HBufC* formatString( NULL );
    
    if ( currentDate.Year() == messageDate.Year() 
        && currentDate.Month() == messageDate.Month() 
        && currentDate.Day() == messageDate.Day())
        {
        // Message arrived today
        formatString = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );       
        }
    else
        {
        // Message arrived earlier than today
        formatString = StringLoader::LoadLC( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );
        }
    
    // Format time string using chosen time or date formatString
    TBuf<KMaxShortDateFormatSpec*2> timeString;
    TRAP_IGNORE( aMessageTime.FormatL( timeString, formatString->Des() ) );
    CleanupStack::PopAndDestroy( formatString );
    // If format fails, result is empty
    
    HBufC* result = timeString.Alloc();
    TPtr ptr = result->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    return result;
}


// ---------------------------------------------------------
// CMailCpsHandler::PublishMessageL
//
// ---------------------------------------------------------
//
void CMailCpsHandler::PublishMessageL( TInt aWidgetInstance, CFSMailMessage& aMessage,
                                        const TDesC& aSenderName, const TDesC& aTime, TInt aRow )
    {
    FUNC_LOG;

    TFSMailMsgId msgId( aMessage.GetMessageId() );

    iLiwIf->PublishMailboxIconL( aWidgetInstance, aRow, GetMailIcon( &aMessage ), msgId );
    iLiwIf->PublishMailDetailL( aWidgetInstance, aRow, aSenderName, ESender );
    iLiwIf->PublishMailDetailL( aWidgetInstance, aRow, aTime, ETime );
    }

// ---------------------------------------------------------
// CMailCpsHandler::UpdateEmptyMessagesL
//
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateEmptyMessagesL( const TInt aWidgetInstance,
                                            const TInt aRow )
    {
    FUNC_LOG;   
    
    TFSMailMsgId mailBoxId; // id not essential here
    
    iLiwIf->PublishMailDetailL(
            aWidgetInstance,
            aRow,
            KNullDes,
            ESender);

    iLiwIf->PublishMailboxIconL(
            aWidgetInstance,
            aRow,
            KNullIcon,
            mailBoxId);

    iLiwIf->PublishMailDetailL(
            aWidgetInstance,
            aRow, 
            KNullDes,
            ETime);
    }
/*
// ---------------------------------------------------------
// CMailCpsHandler::UpdateConnectStateL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateConnectStateL( const TInt aMailBoxNumber, const TInt aRowNumber )
    {
    FUNC_LOG;
    if ( aMailBoxNumber < iAccountsArray.Count() )
        {
        TFSMailMsgId mailBoxId;
        mailBoxId = iAccountsArray[aMailBoxNumber]->iMailboxId;                
        CFSMailBox* mailbox( NULL );
        mailbox = MailClient().GetMailBoxByUidL( mailBoxId );
        if(mailbox)
            {
            iLiwIf->PublishConnectIconL(mailbox->GetMailBoxStatus(), aRowNumber);
            }
        }
    else
        {
        iLiwIf->PublishConnectIconL( EFSMailBoxOffline, aRowNumber );
        }
    }
*/
// ---------------------------------------------------------
// CMailCpsHandler::UpdateMailBoxIconL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateMailBoxIconL( const TInt aMailBoxNumber, 
                                          const TInt aWidgetInstance,
                                          const TInt aRowNumber )
    {
    FUNC_LOG;
    
    TFSMailMsgId mailBoxId; // id required only for getting branded mailbox icon
    
    if ( aMailBoxNumber < iAccountsArray.Count() )
        {
        mailBoxId = iAccountsArray[aMailBoxNumber]->iMailboxId;
        iLiwIf->PublishMailboxIconL( aWidgetInstance, 
		                             aRowNumber,
									 EMbmCmailhandlerpluginQgn_indi_cmail_drop_email_account,
									 mailBoxId);
        }
    else
        {    
        iLiwIf->PublishMailboxIconL( aWidgetInstance, aRowNumber, KNullIcon, mailBoxId);
        }
    }
    
// ---------------------------------------------------------
// CMailCpsHandler::UpdateIndicatorIconL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateIndicatorIconL( const TInt aMailBoxNumber, 
                                            const TInt aWidgetInstance,
                                            const TInt aRowNumber )

    {
    FUNC_LOG;

    if ( aMailBoxNumber < iAccountsArray.Count() )
        {
        TFSMailMsgId mailBoxId;
        mailBoxId = iAccountsArray[aMailBoxNumber]->iMailboxId;    
    
        if ( GetUnseenCountL(mailBoxId) > 0 )
            {
            iLiwIf->PublishIndicatorIconL( aWidgetInstance,
                                           aRowNumber,
                                           EMbmCmailhandlerpluginQgn_indi_ai_eplg_unread );
            }
    
        else if( !IsOutboxEmptyL(mailBoxId) )
            {
            iLiwIf->PublishIndicatorIconL( aWidgetInstance,
                                           aRowNumber,                
                                           EMbmCmailhandlerpluginQgn_prop_mce_outbox_small);
            }
        else
            {    
            iLiwIf->PublishIndicatorIconL( aWidgetInstance,
                                           aRowNumber,
                                           KNullIcon);
            }
        }
    else
        {    
        iLiwIf->PublishIndicatorIconL( aWidgetInstance,
                                       aRowNumber,
                                       KNullIcon);
        }        
    }
    
/*
// ---------------------------------------------------------
// CMailCpsHandler::ClearMessageTimeL
// ---------------------------------------------------------
//
void CMailCpsHandler::ClearMessageTimeL( const TInt aRowNumber )
    {
    FUNC_LOG;

    iLiwIf->PublishMailDetailL(
            aRowNumber, 
            KNullDes,
            ETime);
    }
*/

// ---------------------------------------------------------
// CMailCpsHandler::CapabilitiesToContinueL
// ---------------------------------------------------------
//
TBool CMailCpsHandler::CapabilitiesToContinueL(
    TFSMailEvent /*aEvent*/,
    TFSMailMsgId /*aMailbox*/,
    TAny* /*aParam1*/,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ ) const
    {
    FUNC_LOG;
    // Inherited from baseclass, not used
    return ETrue;
    }

// ---------------------------------------------------------
// CMailCpsHandler::FindMailboxDetails
// ---------------------------------------------------------
//
CMailMailboxDetails* CMailCpsHandler::FindMailboxDetails( TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    const TInt iiMax( iAccountsArray.Count() );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        if ( iAccountsArray[ii]->iMailboxId == aMailbox )
            {
            return iAccountsArray[ii];
            }
        }
    // mailbox id not found from local cache, returning NULL
    return NULL;
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* aParam2,
    TAny* /*aParam3*/ )
    {
    FUNC_LOG;

    // Is the mailbox in our list
    switch ( aEvent )
        {
        case TFSEventNewMailbox:
            {
//            HandleNewMailboxEventL( aMailbox );
            break;
            }
        case TFSEventMailboxRenamed:
            {
            HandleMailboxRenamedEventL( aMailbox );
            break;
            }
        case TFSEventMailboxDeleted:
            {
            HandleMailboxDeletedEventL( aMailbox );
            break;
            }
        case TFSEventMailboxSettingsChanged:
            {
//            HandleMailboxDeletedEventL( aMailbox );
//            HandleNewMailboxEventL( aMailbox );
            break;
            }            
        case TFSEventNewMail:
            {
            HandleNewMailEventL( aMailbox, aParam1, aParam2 );
            break;
            }
        case TFSEventMailDeleted:
            {
            HandleMailDeletedEventL( aMailbox, aParam1, aParam2 );
            break;
            }
        case TFSEventMailChanged:
            {
            HandleMailChangedEventL( aMailbox, aParam1, aParam2 );
            break;
            }
        case TFSEventNewFolder:
            {
            HandleNewFolderEventL( aMailbox, aParam1, aParam2 );
            break;
            }
        default:
            {
            break;
            }            
        }
	UpdateFullL();
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleNewMailboxEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleNewMailboxEventL( const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;

    // Check that max mailbox count is not already reached
    const TInt iiMax( iAccountsArray.Count() );
    if( iiMax >= iSettings->MaxMailboxCount() )
        {
        return;
        }

    // Check that mailbox is not already in widget
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        if( iAccountsArray[ii]->iMailboxId.Id() == aMailbox.Id() )
            {
            return;
            }
        }
    
    CFSMailBox* mailbox( NULL );
    mailbox = MailClient().GetMailBoxByUidL( aMailbox );
    if ( !mailbox )
        {
        return; // nothing that could be done
        }
    else
        {
        CleanupStack::PushL( mailbox );
        CMailMailboxDetails* mailboxDetails = CreateMailboxDetailsL( *mailbox );
        CleanupStack::PushL( mailboxDetails );
        iAccountsArray.AppendL( mailboxDetails );
        CleanupStack::Pop( mailboxDetails );
        CleanupStack::PopAndDestroy( mailbox ); 
        }
    
    iSettings->AddMailboxL( aMailbox );
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleMailborRenamedEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleMailboxRenamedEventL( const TFSMailMsgId aMailbox )
    {
    for ( TInt ii = iAccountsArray.Count() - 1; ii >= 0; --ii )
        {
        if ( iAccountsArray[ii]->iMailboxId.Id() == aMailbox.Id() )
            {
            CFSMailBox* mailbox = NULL;
            mailbox = MailClient().GetMailBoxByUidL( aMailbox );
            if ( mailbox )
                {
                iAccountsArray[ii]->SetMailboxName( mailbox->GetName() );
                }
            delete mailbox;
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleMailboxDeletedEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleMailboxDeletedEventL( const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;

    // Look from local cache
    const TInt iiMax( iAccountsArray.Count() );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        if( iAccountsArray[ii]->iMailboxId.Id() == aMailbox.Id() )
            {
            CMailMailboxDetails* tmp = iAccountsArray[ii];
            iAccountsArray.Remove( ii );
            delete tmp;
            tmp = NULL;

            // Remove from cenrep
            iSettings->RemoveMailboxL( aMailbox );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleNewMailEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleNewMailEventL(
    TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2 )
    {
    FUNC_LOG;
    // Basic assertions
    if ( !aParam1 || !aParam2 )
        {
        User::Leave( KErrArgument );
        }
    
    // Find mailbox instance from array
    CMailMailboxDetails* mailbox = FindMailboxDetails( aMailbox );
    if ( !mailbox )
        {
        return;
        }

    // typecast param2
    TFSMailMsgId* parentFolder = static_cast<TFSMailMsgId*>( aParam2 );

    // Check that folder is correct
    CFSMailFolder* folder = MailClient().GetFolderByUidL( aMailbox, *parentFolder );
    if ( !folder )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( folder );
    if ( folder->GetFolderType() != EFSInbox )
        {
        CleanupStack::PopAndDestroy( folder );
        return;
        }
    CleanupStack::PopAndDestroy( folder );

    // typecast param1
    RArray<TFSMailMsgId>* newEntries(
            static_cast< RArray<TFSMailMsgId>* >( aParam1 ) );
    CleanupClosePushL( *newEntries );
    const TInt iiMax( newEntries->Count() );
    if ( iiMax == 0 )
        {
        User::Leave( KErrArgument );
        }

    // Loop through message array
    TFSMailMsgId msgId;
    CFSMailMessage* msg( NULL );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        msgId = (*newEntries)[ii];

        msg = MailClient().GetMessageByUidL( 
            aMailbox, *parentFolder, (*newEntries)[ii], EFSMsgDataEnvelope );
        if ( !msg )
            {
            User::Leave( KErrNotFound );
            }
        CleanupStack::PushL( msg );

        // Check if message read
        if ( !MsgIsUnread( *msg ) )
            {
            // Ignore already read messages
            CleanupStack::PopAndDestroy( msg );
            continue;
            }

        // Check if message is duplicate
        if ( IsDuplicate( *mailbox, msgId ) )
            {
            // Ignore already known messages
            CleanupStack::PopAndDestroy( msg );
            continue;
            }
        
        CMailMessageDetails* messageDetails = CMailMessageDetails::NewL(
            msg->GetMessageId(),
            mailbox,
            msg->GetSender()->GetEmailAddress(),
            msg->GetSubject(),
            msg->GetDate() );
        CleanupStack::PopAndDestroy( msg );
        CleanupStack::PushL( messageDetails );

        // Place message to array
        mailbox->iMessageDetailsArray.AppendL( messageDetails );
        CleanupStack::Pop( messageDetails );
        }
    CleanupStack::PopAndDestroy( newEntries );
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleMailDeletedEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleMailDeletedEventL(
    TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2 )
    {
    FUNC_LOG;
    // Basic assertions
    if ( !aParam1 || !aParam2 )
        {
        User::Leave( KErrArgument );
        }
    // Typecast parameters
    RArray<TFSMailMsgId>* entries(
            static_cast< RArray<TFSMailMsgId>* >( aParam1 ) );
    CleanupClosePushL( *entries );
    if ( entries->Count() == 0 )
        {
        User::Leave( KErrArgument );
        }
    
    // Find correct mailbox
    CMailMailboxDetails* mailbox = FindMailboxDetails( aMailbox );
    if ( !mailbox )
        {
        //<Cmail>  
        CleanupStack::PopAndDestroy( entries );
        //<Cmail>
        return;
        }
    
    const TInt iiMax( entries->Count() );
    const TInt jjMax( mailbox->iMessageDetailsArray.Count() );
    // Loop through entries in the array
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        // Loop through messages in the local cache
        for ( TInt jj = 0; jj < jjMax; jj++ )
            {
            if ( mailbox->iMessageDetailsArray[jj]->iMsgId.Id() ==
                (*entries)[ii].Id() )
                {
                CMailMessageDetails* details = mailbox->iMessageDetailsArray[jj];
                delete details;
                details = NULL;
                mailbox->iMessageDetailsArray.Remove(jj);
                break; // breaks out from the inner loop and starts new outer loop if necessary
                }
            else
                {
                }
            }
        }

    CleanupStack::PopAndDestroy( entries );
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleMailChangedEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleMailChangedEventL(
    TFSMailMsgId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleNewFolderEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleNewFolderEventL(
    TFSMailMsgId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/ )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------
// CMailCpsHandler::GetUnreadCountL
// ---------------------------------------------------------
//
TInt CMailCpsHandler::GetUnreadCountL(TFSMailMsgId aMailbox)
    {
    FUNC_LOG;
    CFSMailBox* mailbox( NULL );
    TInt unread (0);
    mailbox = MailClient().GetMailBoxByUidL( aMailbox );
    if(mailbox)
        { 
        TFSMailMsgId folderId( mailbox->GetStandardFolderId( EFSInbox ) );
        // Check that folder is correct
        CFSMailFolder* folder = MailClient().GetFolderByUidL( aMailbox, folderId );
        if ( !folder )
            {
            return KErrNotFound;
            }
        CleanupStack::PushL( folder );
    
        unread = folder->GetUnreadCount();

        CleanupStack::PopAndDestroy( folder );
    
        if (unread > KMaxUnreadCount)
            {
            unread = KMaxUnreadCount;
            }
        }

    return unread;
    }

// ---------------------------------------------------------
// CMailCpsHandler::GetUnseenCountL
// ---------------------------------------------------------
//
TInt CMailCpsHandler::GetUnseenCountL(TFSMailMsgId aMailbox)
    {
    FUNC_LOG;
    CFSMailBox* mailbox( NULL );
    TInt unseen (0);
    mailbox = MailClient().GetMailBoxByUidL( aMailbox );
    if(mailbox)
        { 
        TFSMailMsgId folderId( mailbox->GetStandardFolderId( EFSInbox ) );
        // Check that folder is correct
        CFSMailFolder* folder = MailClient().GetFolderByUidL( aMailbox, folderId );
        if ( !folder )
            {
            return KErrNotFound;
            }
        CleanupStack::PushL( folder );
    
        unseen = folder->GetUnseenCount();

        CleanupStack::PopAndDestroy( folder );
    
        if (unseen > KMaxUnreadCount)
            {
            unseen = KMaxUnreadCount;
            }
        }

    return unseen;
    }

// ---------------------------------------------------------
// CMailCpsHandler::IsOutboxEmptyL
// ---------------------------------------------------------
//
TBool CMailCpsHandler::IsOutboxEmptyL(TFSMailMsgId aMailbox)
    {
    FUNC_LOG;
    CFSMailBox* mailbox( NULL );
    TBool ret(ETrue);
    TInt msgCount(0);
    mailbox = MailClient().GetMailBoxByUidL( aMailbox );
    if(mailbox)
        { 
        TFSMailMsgId folderId( mailbox->GetStandardFolderId( EFSOutbox ) );
        // Check that folder is correct
        CFSMailFolder* folder = MailClient().GetFolderByUidL( aMailbox, folderId );
        if ( !folder )
            {
            return KErrNotFound;
            }
        CleanupStack::PushL( folder );
    
        msgCount = folder->GetMessageCount();

        if ( msgCount<1 )
            {
            CleanupStack::PopAndDestroy( folder );
            return ret;
            }
        
        TFSMailDetails details( EFSMsgDataEnvelope );
        RArray<TFSMailSortCriteria> sorting;
        CleanupClosePushL( sorting );
        TFSMailSortCriteria sortCriteria;
        sortCriteria.iField = EFSMailDontCare;
        sortCriteria.iOrder = EFSMailDescending;
        sorting.Append( sortCriteria );       
        // List all or maximum number of messages
        MFSMailIterator* iterator = folder->ListMessagesL( details, sorting );
        CleanupStack::PopAndDestroy( &sorting );        
        CleanupDeletePushL( iterator );
        
        RPointerArray<CFSMailMessage> messages;
        CleanupClosePushL( messages );
        TInt amount( msgCount );
        iterator->NextL( TFSMailMsgId(), amount, messages );
        
        for (TInt i = 0; i < msgCount; i++)
            {
            TFSMailMsgId msgId = messages[0]->GetMessageId();
            CFSMailMessage* msg( NULL );
            msg = MailClient().GetMessageByUidL( aMailbox, folderId, msgId, EFSMsgDataEnvelope );
            CleanupDeletePushL( msg );
            TFSMailMsgId mailboxId = msg->GetMailBoxId();
            CleanupStack::PopAndDestroy( msg );
            
            if (mailboxId == aMailbox)
                {
                ret = EFalse;
                break;
                }
            }

        CleanupStack::PopAndDestroy( &messages );
        CleanupStack::PopAndDestroy( iterator );
        CleanupStack::PopAndDestroy( folder );
        }

    return ret;
    }

// ---------------------------------------------------------
// CMailCpsHandler::TurnNotificationOn
// ---------------------------------------------------------
//
void CMailCpsHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CMailCpsHandler::TurnNotificationOff
// ---------------------------------------------------------
//
void CMailCpsHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMailCpsHandler::GetMailIcon
// ---------------------------------------------------------------------------
//
TInt CMailCpsHandler::GetMailIcon( CFSMailMessage* aMsg )
    {
    FUNC_LOG;
    TInt ret(EMbmCmailhandlerpluginQgn_indi_cmail_unread);
    if (aMsg->IsFlagSet( EFSMsgFlag_Read ))
        {
        ret = GetReadMsgIcon(aMsg);
        }
    else
        {
        ret = GetUnreadMsgIcon(aMsg);
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMailCpsHandler::GetUnreadMsgIcon
// -----------------------------------------------------------------------------
TInt CMailCpsHandler::GetUnreadMsgIcon( CFSMailMessage* aMsg )
    {
    FUNC_LOG;    
    TInt icon(EMbmCmailhandlerpluginQgn_indi_cmail_unread);

    // Unread calendar invitation
    if ( aMsg->IsFlagSet( EFSMsgFlag_CalendarMsg ))
        {
        icon = GetUnreadCalMsgIcon( aMsg );
        }   
    else    // Normal message icons
        {
        // Check whether msg has attachment or not
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments )) // Has attachments
            {
            if ( aMsg->IsFlagSet( EFSMsgFlag_Important )) // High priority, has attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_attach_high_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_attach_high_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_attach_high_prio;
                    }
                }
            else if ( aMsg->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, has attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_attach_low_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_attach_low_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_attach_low_prio;
                    }  
                }
            else // Normal priority, has attachments
                {       
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_attach;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_attach;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_attach;
                    }
                }
            }
    
        else // No attachments
            {
            if ( aMsg->IsFlagSet( EFSMsgFlag_Important )) // High priority, no attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_high_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_high_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_high_prio;
                    }                                                                                                                                           
                }
            else if ( aMsg->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, no attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_low_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_low_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_low_prio;
                    }  
                }
            else // Normal priority, no attachments
                {       
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded;
                    }
                }
            }               
        }
    return icon;
    }

// -----------------------------------------------------------------------------
// CMailCpsHandler::GetReadMsgIcon
// -----------------------------------------------------------------------------
TInt CMailCpsHandler::GetReadMsgIcon( CFSMailMessage* aMsg )
    {
    FUNC_LOG;    
    TInt icon(EMbmCmailhandlerpluginQgn_indi_cmail_read);

    if ( aMsg->IsFlagSet( EFSMsgFlag_CalendarMsg ))
        {
        icon = GetReadCalMsgIcon( aMsg );
        }
    else    // Normal message icons
        {
        // Check whether msg has attachment or not
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments )) // Has attachments
            {
            if ( aMsg->IsFlagSet( EFSMsgFlag_Important )) // High priority, has attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_attach_high_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_attach_high_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_attach_high_prio;
                    }                                                                                                                                           
                }
            else if ( aMsg->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, has attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_attach_low_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_attach_low_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_attach_low_prio;
                    }  
                }
            else // Normal priority, has attachments
                {       
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_attach;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_attach;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_attach;
                    }                                                                                       
                }           
            }
        else // No attachments
            {
            if ( aMsg->IsFlagSet( EFSMsgFlag_Important )) // High priority, no attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_high_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_high_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_high_prio;
                    }                                                                                                                                           
                }
            else if ( aMsg->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, no attachments
                {
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_low_prio;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_low_prio;
                    }
                else
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_low_prio;
                    }  
                }
            else // Normal priority, no attachments
                {       
                if ( aMsg->IsFlagSet( EFSMsgFlag_Answered ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_replied;
                    }
                else if ( aMsg->IsFlagSet( EFSMsgFlag_Forwarded ) )
                    {
                    icon = EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded;
                    }
                }
            }
        }
    return icon;
    }

// -----------------------------------------------------------------------------
// CMailCpsHandler::GetUnreadCalMsgIcon
// -----------------------------------------------------------------------------
TInt CMailCpsHandler::GetUnreadCalMsgIcon( CFSMailMessage* aMsg )
    {
    FUNC_LOG;    
    TInt icon(0);

    if ( aMsg->IsFlagSet( EFSMsgFlag_Important ) )
        {
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments ) )
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_attachments_high_prio ;                                               
            }
        else 
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_high_prio;                                
            }           
        }
    else if ( aMsg->IsFlagSet( EFSMsgFlag_Low ) )
        {
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments ) )
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_attachments_low_prio;                                             
            }
        else
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_low_prio;                             
            }                           
        }
    else
        {
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments ) )
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_attachments;                                              
            }
        else
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread;                              
            }                       
        }       

    return icon;
    }

// -----------------------------------------------------------------------------
// CMailCpsHandler::GetReadCalMsgIcon
// -----------------------------------------------------------------------------
TInt CMailCpsHandler::GetReadCalMsgIcon( CFSMailMessage* aMsg )
    {
    FUNC_LOG;    
    TInt icon(0);

    if ( aMsg->IsFlagSet( EFSMsgFlag_Important ) )
        {
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments ) )
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_attachments_high_prio ;                                             
            }
        else 
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_high_prio;                              
            }           
        }
    else if ( aMsg->IsFlagSet( EFSMsgFlag_Low ) )
        {
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments ) )
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_attachments_low_prio;                                               
            }
        else
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_low_prio;                               
            }                           
        }
    else
        {
        if ( aMsg->IsFlagSet( EFSMsgFlag_Attachments ) )
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_attachments;                                                
            }
        else
            {
            icon = EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read;                                
            }                       
        }       

    return icon;
    }

// -----------------------------------------------------------------------------
// CMailCpsHandler::IsValidDisplayName
// -----------------------------------------------------------------------------
TBool CMailCpsHandler::IsValidDisplayName(const TDesC& aDisplayName)
    {
    FUNC_LOG;
    TInt length(aDisplayName.Length());

    if (length == 0)
        {
        return EFalse;
        }

    // If displayname is in <xxx(at)yyyyy.zzz> format use e-mail address instead of display name.
    if ((aDisplayName.Find(KLessThan) == 0) &&
        (aDisplayName.Right(1).Find(KGreaterThan) == 0) &&
        (aDisplayName.Find(KAt)) &&
        (aDisplayName.Find(KDot)))
        {
        return EFalse;
        }
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CMailCpsHandler::IsDuplicate
// -----------------------------------------------------------------------------
TBool CMailCpsHandler::IsDuplicate( const CMailMailboxDetails& aMailbox, const TFSMailMsgId& aMsgId )
    {
    FUNC_LOG;
    TBool isDuplicate( EFalse );
    const TInt size( aMailbox.iMessageDetailsArray.Count() );
    for ( TInt jj = 0; jj < size; jj++ )
        {
        if ( aMailbox.iMessageDetailsArray[jj]->iMsgId.Id() == aMsgId.Id() )
            {
            isDuplicate = ETrue;
            }
        }
    return isDuplicate;
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::LaunchWidgetSettingsL()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::LaunchWidgetSettingsL( const TDesC& aContentId )
    {
    FUNC_LOG;    

    RApaLsSession appArcSession;
    CleanupClosePushL( appArcSession );
    User::LeaveIfError( appArcSession.Connect( ) );
    TApaAppInfo appInfo;
    appArcSession.GetAppInfo( appInfo, KUidEMWSettings );

    TPtrC fullFileName;
    TPtrC arguments;

    fullFileName.Set( appInfo.iFullName );
    arguments.Set( aContentId );

    TParse fileName;
    User::LeaveIfError(fileName.Set(fullFileName, NULL, NULL));

    RProcess process;
    process.Create(fileName.FullName(), arguments );
    CApaCommandLine* commandLine = CApaCommandLine::NewLC();
    commandLine->SetDocumentNameL(KNullDesC);
    commandLine->SetExecutableNameL(fullFileName);
    commandLine->SetCommandL( EApaCommandRun );
    commandLine->SetProcessEnvironmentL(process);
    CleanupStack::PopAndDestroy(commandLine);
    
    process.Resume();
    process.Close();    
    
    CleanupStack::PopAndDestroy(&appArcSession);        
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::LaunchEmailUIL()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::LaunchEmailUIL( const TDesC& aContentId )
    {
    FUNC_LOG;

    TUid mailBoxUid;
    mailBoxUid.iUid = iSettings->GetMailboxUidByContentId(aContentId);
    TFSMailMsgId mailBoxId;
    mailBoxId.SetId(mailBoxUid.iUid);
    TUid pluginUid;
    pluginUid.iUid = iSettings->GetPluginUidByContentId(aContentId);
    mailBoxId.SetPluginId(pluginUid);
    CFSMailBox* mailBox = MailClient().GetMailBoxByUidL( mailBoxId );
    CleanupStack::PushL( mailBox );
    if ( mailBox )
        {
        TFSMailMsgId inboxFolderId = mailBox->GetStandardFolderId( EFSInbox );    
        
        TMailListActivationData tmp;
        tmp.iFolderId = inboxFolderId;
        tmp.iMailBoxId = mailBoxId; 
        const TPckgBuf<TMailListActivationData> pkgOut( tmp );
        iEnv->EikAppUi()->ActivateViewL( TVwsViewId(KUidEmailUi, KMailListId), 
                                         KStartListWithFolderId, 
                                         pkgOut);
        }
    CleanupStack::PopAndDestroy( mailBox );
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::LaunchEmailUIL()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::LaunchEmailWizardL()
    {
    FUNC_LOG;
    TUid viewUid(KNullUid);
    iEnv->EikAppUi()->ActivateViewL( TVwsViewId(KUidWizardApp, KUidEmailWizardView), 
                                     viewUid, 
                                     KNullDesC8);
    }
// ---------------------------------------------------------------------------
// CMailCpsHandler::AssociateWidget
// ---------------------------------------------------------------------------
//
TBool CMailCpsHandler::AssociateWidgetToSetting( const TDesC& aContentId )
    {
    FUNC_LOG;
    return iSettings->AssociateWidgetToSetting( aContentId );
    }

// ---------------------------------------------------------------------------
// CMailCpsHandler::DissociateWidget
// ---------------------------------------------------------------------------
//
void CMailCpsHandler::DissociateWidgetFromSettingL( const TDesC& aContentId )
    {
    FUNC_LOG;
    iSettings->DissociateWidgetFromSettingL( aContentId );
    }

// ---------------------------------------------------------------------------
// CMailCpsHandler::GetMailboxCount
// ---------------------------------------------------------------------------
//
TInt CMailCpsHandler::GetMailboxCount()
    {
    FUNC_LOG;
    return iSettings->GetTotalMailboxCount();
    }

// ---------------------------------------------------------------------------
// CMailCpsHandler::ManualAccountSelectionL
// ---------------------------------------------------------------------------
//
void CMailCpsHandler::ManualAccountSelectionL( const TDesC& aContentId )
    {
    FUNC_LOG;

    if ( FirstBootL() )
        {
        if (!iSettings->FindFromContentIdListL(aContentId))
            {
            iSettings->AddToContentIdListL(aContentId);
            }
        }
    else
        {
        if (!iSettings->FindFromContentIdListL(aContentId))
            {
            iSettings->AddToContentIdListL(aContentId);
            if (GetMailboxCount())
                {
                LaunchWidgetSettingsL(aContentId);
                }
            else
                {
                LaunchEmailWizardL();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsHandler::FirstBootL
// ---------------------------------------------------------------------------
TBool CMailCpsHandler::FirstBootL()
    {
    FUNC_LOG;
    TInt value( 0 );
    TBool ret(EFalse);

    CRepository* repository(NULL);

    TRAPD( err, repository = CRepository::NewL( KCRUidStartup ) );
    if ( err == KErrNone )
        {
        err = repository->Get( KStartupFirstBoot, value );
        }
    delete repository;
    
    if (!value)
        {
        ret = ETrue;
        }

    return ret;
    }

