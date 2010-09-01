/*
* Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <emailobserverinterface.hrh>
#include <emailobserverplugin.h>
#include <memaildata.h>
#include <memailmailboxdata.h>
#include <scs/cleanuputils.h> // CleanupResetAndDestroyPushL

#include "emailtrace.h"
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cfsmailfolder.h"
#include "cmailcpshandler.h"
#include "cmailcpssettings.h"
#include "cmailcpsif.h"
#include "cmailmessagedetails.h"
#include "cmailmailboxdetails.h"
#include "cmailexternalaccount.h"
#include "cmailcpsifconsts.h"
#include "FreestyleEmailUiConstants.h"
#include "cmailpluginproxy.h"
#include "cmailhandlerpluginpanic.h"

#include "fsemailserverpskeys.h"

using namespace EmailInterface;

// ---------------------------------------------------------
// CMailCpsHandler::CMailCpsHandler
// ---------------------------------------------------------
//
CMailCpsHandler::CMailCpsHandler( MFSNotificationHandlerMgr& aOwner ): 
    CFSNotificationHandlerBase( aOwner ),
    iWaitingForNewMailbox( NULL ),
    iWaitingForNewWidget(),
    iUpdateHelper( NULL )
    {
    FUNC_LOG;
    iWaitingForNewWidget.SetPluginId( KNullUid );
    iWaitingForNewWidget.SetId( 0 );
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
    InitializeExternalAccountsL();

    iSettings->StartObservingL( this );

    TInt err = RProperty::Define( KPSUidEmailServerCategory, 
                                  KIntMailboxCount, 
                                  RProperty::EInt );
    if ( err != KErrAlreadyExists && err != KErrNone )
        {
        User::LeaveIfError( err );
        }
    
    // set mailbox initial count 
    TInt intCount = TotalIntMailboxCount();
    User::LeaveIfError( RProperty::Set( KPSUidEmailServerCategory, KIntMailboxCount, intCount ) );
    }

// ---------------------------------------------------------
// CMailCpsHandler::~CMailCpsHandler
// ---------------------------------------------------------
//
CMailCpsHandler::~CMailCpsHandler()
    {
    FUNC_LOG;
    delete iUpdateHelper;
    delete iLiwIf;
    delete iSettings;
    iAccountsArray.ResetAndDestroy();
    iExternalPlugins.ResetAndDestroy();
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
    INFO_1("iSettings->Mailboxes().Count() = %d", iiMax );
    for ( TInt ii = 0; ii < iiMax; ii++ )
        {
        CFSMailBox* mailbox( NULL );
        mailbox = MailClient().GetMailBoxByUidL( iSettings->Mailboxes()[ii] );

        TInt mailboxId(0);
        if (mailbox)
            {
            CleanupStack::PushL( mailbox );
            mailboxId = mailbox->GetId().Id();
            if ( mailboxId )
                {
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
                }
            CleanupStack::PopAndDestroy( mailbox );
            }
        }
     INFO_1("iAccountsArray.Count() = %d", iAccountsArray.Count() );
    }

// ---------------------------------------------------------
// CMailCpsHandler::InitializeExternalAccountsL
// ---------------------------------------------------------
//
void CMailCpsHandler::InitializeExternalAccountsL()
    {
    FUNC_LOG;
    // Read external account data from settings
    RPointerArray<CMailExternalAccount> extAccounts;
    CleanupResetAndDestroyPushL( extAccounts );  // owning - GetExtMailboxesL transfers ownership
    iSettings->GetExtMailboxesL( extAccounts );

    // Delete removed plugins
    RemoveUnusedPluginsL( extAccounts );

    // Instantiate new plugins
    AddNewPluginsL( extAccounts );

    // Set accounts under correct pluginProxies
    SelectAndUpdateExtAccountsL( extAccounts );

    __ASSERT_DEBUG( extAccounts.Count() == 0, Panic( ECmailHandlerPluginPanicNoFailedState ) );
    CleanupStack::PopAndDestroy( &extAccounts );
    }

// ---------------------------------------------------------
// CMailCpsHandler::RemoveUnusedPluginsL
// ---------------------------------------------------------
//
void CMailCpsHandler::RemoveUnusedPluginsL( RPointerArray<CMailExternalAccount>& aAccounts )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iExternalPlugins.Count(); i++ )
        {
        if ( !IsPluginInArray( iExternalPlugins[i]->PluginId(), aAccounts ) )
            {
            // all plugin's accounts have been removed from widget settings, unloading resources
            CMailPluginProxy* proxy = iExternalPlugins[i];
            iExternalPlugins.Remove(i--); // also change the loop index
            delete proxy;
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::AddNewPluginsL
// ---------------------------------------------------------
//
void CMailCpsHandler::AddNewPluginsL( RPointerArray<CMailExternalAccount>& aAccounts )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < aAccounts.Count(); i++ )
        {
        if ( !IsPluginInArray( aAccounts[i]->PluginId(), iExternalPlugins ) )
            {
            // new plugin instantiation
            INFO_1("Instantiating plugin 0x%x", aAccounts[i]->PluginId() );
            CMailPluginProxy* proxy = CMailPluginProxy::NewL( aAccounts[i]->PluginId(), *iLiwIf );
            CleanupStack::PushL( proxy );
            iExternalPlugins.AppendL( proxy );
            CleanupStack::Pop( proxy );
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::UpdateExtAccountsL
// ---------------------------------------------------------
//
void CMailCpsHandler::SelectAndUpdateExtAccountsL( RPointerArray<CMailExternalAccount>& aAccounts )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iExternalPlugins.Count(); i++ )
        {
        iExternalPlugins[i]->SelectAndUpdateAccountsL( aAccounts );
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::IsPluginInArray
// ---------------------------------------------------------
//
TBool CMailCpsHandler::IsPluginInArray( const TInt aPluginId, RPointerArray<CMailExternalAccount>& aAccounts )
    {
    FUNC_LOG;
    TBool found( EFalse );
    for ( TInt i = 0; i < aAccounts.Count(); i++ )
        {
        if ( aAccounts[i]->PluginId() == aPluginId )
            {
            found = ETrue;
            }
        }
    return found;
    }

// ---------------------------------------------------------
// CMailCpsHandler::IsPluginInArray
// ---------------------------------------------------------
//
TBool CMailCpsHandler::IsPluginInArray( const TInt aPluginId, RPointerArray<CMailPluginProxy>& aPlugins )
    {
    FUNC_LOG;
    TBool found( EFalse );
    for ( TInt i = 0; i < aPlugins.Count(); i++ )
        {
        if ( aPlugins[i]->PluginId() == aPluginId )
            {
            found = ETrue;
            }
        }
    return found;
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
    TRAP_IGNORE( InitializeExternalAccountsL() );
    // Update widget contents after settings change
    TInt count = iLiwIf->GetWidgetInstanceCount();
    for (TInt i = 0; i < count; i++ )
        {
        iLiwIf->SetUpdateNeeded( i, ETrue );
        }
    TRAP_IGNORE( UpdateFullL() );
    CleanWaitingForNewMailbox();
    }

// ---------------------------------------------------------
// CMailCpsHandler::SetUpdateNeeded
// ---------------------------------------------------------
//
void CMailCpsHandler::SetUpdateNeeded( const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    TInt count = iSettings->WidgetCountByMailbox( aMailbox.Id() );
    for (TInt i = 0; i < count; i++ )
        {
        TBuf<KMaxDescLen> cid;
        iSettings->GetContentId( aMailbox.Id(), i + 1, cid );
        TInt widgetInstance = iLiwIf->GetWidgetInstanceId( cid );
        if ( widgetInstance != KErrNotFound )
            {
            iLiwIf->SetUpdateNeeded( widgetInstance, ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::UpdateFullL
// ---------------------------------------------------------
//
void CMailCpsHandler::UpdateFullL()
    {
    FUNC_LOG;
    if ( !iUpdateHelper )
        {
        iUpdateHelper = CMailCpsUpdateHelper::NewL( this );
        }
    iUpdateHelper->UpdateL();
    }

// ---------------------------------------------------------
// CMailCpsHandler::DoUpdateFullL
// ---------------------------------------------------------
//
void CMailCpsHandler::DoUpdateFullL()
    {
    FUNC_LOG;
    for (TInt instance = 0; instance < iLiwIf->GetWidgetInstanceCount(); instance++)
        {
        if ( iLiwIf->AllowedToPublish(instance) )
            {
            UpdateMailboxesL(instance, iLiwIf->iInstIdList[instance].iCid);
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
    // Look up 3rd party plugin that handles this account
    CMailPluginProxy* plugin = GetExtPluginL( aContentId );
    if ( plugin ) // handle external 3rd party accounts 
        {
        // Publish its data
        plugin->UpdateAccountL( aContentId );
        }
    else // handle internal accounts
        {
        if ( iLiwIf->UpdateNeeded( aInstance ) )
            {
            TInt row(1); // start from first row
            TInt mailbox(0);
            TBool found( EFalse );
            // try to find mailbox with matching contentId
            for ( mailbox = 0; mailbox < iAccountsArray.Count(); mailbox++ )
                {
                if ( !aContentId.Compare( *iAccountsArray[mailbox]->iWidgetInstance ) )
                    {
                    INFO_1("iAccountsArray[mailbox] mailbox = %d", mailbox);
                    found = ETrue;
                    break;
                    }
                }

            // if contentId found from array, update the mailbox
            if ( found )
                {
                INFO_1("found = TRUE iAccountsArray.Count() == %d", iAccountsArray.Count());
                // Update fields from left to right
                UpdateMailBoxIconL( mailbox, aInstance, row );
                UpdateMailboxNameL( mailbox, aInstance, row );
                UpdateIndicatorIconL( mailbox, aInstance, row );
                row++;
                UpdateMessagesL( mailbox, aInstance, 1, row);
                row++;
                UpdateMessagesL( mailbox, aInstance, 2, row);
                }
            else
                {
                INFO_1("found = FALSE iAccountsArray.Count() == %d", iAccountsArray.Count());
                // Clean mailbox name
                iLiwIf->PublishActiveMailboxNameL( aInstance, 1, KNullDesC );
                // Clean mailbox icon
                TFSMailMsgId mailBoxId; // id not essential here        
                iLiwIf->PublishMailboxIconL( aInstance, 1, KNullIcon, mailBoxId);
                // Clean indicator icon
                iLiwIf->PublishIndicatorIconL( aInstance, 1, KNullIcon);
                // Clean message rows
                UpdateEmptyMessagesL( aInstance, 2 );
                UpdateEmptyMessagesL( aInstance, 3 );
                }
            iLiwIf->SetUpdateNeeded( aInstance, EFalse );
            }
        }
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
                CDesCArrayFlat* strings = new( ELeave) CDesCArrayFlat( 1 );
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
                        aRowNumber, *mailboxAndCount);

                CleanupStack::PopAndDestroy(mailboxAndCount);
                CleanupStack::PopAndDestroy(ints);
                CleanupStack::PopAndDestroy(strings);
                }
            else
                {
                // show only mailbox name

                iLiwIf->PublishActiveMailboxNameL(
                        aWidgetInstance,
                        aRowNumber, accountName);
                }
            }
        }
    else if(!mailBoxFound)
        {
        // Publishing empty descriptor for rows that should not contain
        // anything on widget UI.
        iLiwIf->PublishActiveMailboxNameL( aWidgetInstance,
                                           aRowNumber,
                                           KNullDesC);
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

    if ( aMailBoxNumber < iAccountsArray.Count() )
        {
        TFSMailMsgId mailBoxId;
        mailBoxId = iAccountsArray[aMailBoxNumber]->iMailboxId;

        CFSMailBox* mailbox( NULL );
        mailbox = MailClient().GetMailBoxByUidL( mailBoxId ); // transferred ownership
        if(mailbox)
            {
            TFSMailMsgId parentFolder( mailbox->GetStandardFolderId( EFSInbox ) );
            delete mailbox;
            // Check that folder is correct
            CFSMailFolder* folder = MailClient().GetFolderByUidL( mailBoxId, parentFolder );
            if ( !folder )
                {
                UpdateEmptyMessagesL( aWidgetInstance, aRow );
                return;
                }
            CleanupStack::PushL( folder );

            TInt msgCount = folder->GetMessageCount();

            if(msgCount<1 || (msgCount == 1 && aRow == 3))
                {
                UpdateEmptyMessagesL( aWidgetInstance, aRow );
                CleanupStack::PopAndDestroy( folder );
                return;
                }

            folder->SetFolderType(EFSInbox);

            // Update folder if provided, otherwise use current folder
            RPointerArray<CFSMailMessage> folderMessages(1);
            CleanupResetAndDestroyPushL( folderMessages ); // owning array, from emailapiutils.h

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
            if(!count || (count == 1 && aRow == 3))
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
            TBuf<KMaxDescLen> subject( NULL );
            subject.Append(msg->GetSubject());
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
                              subject,
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
                                        const TDesC& aSenderName, const TDesC& aSubject,
                                        const TDesC& aTime, TInt aRow )
    {
    FUNC_LOG;

    TFSMailMsgId msgId( aMessage.GetMessageId() );

    iLiwIf->PublishMailboxIconL( aWidgetInstance, aRow, GetMailIcon( &aMessage ), msgId );
    iLiwIf->PublishMailDetailL( aWidgetInstance, aRow, aSenderName, ESender );
    iLiwIf->PublishMailDetailL( aWidgetInstance, aRow, aSubject, ESubject );    
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

    iLiwIf->PublishMailDetailL(
            aWidgetInstance,
            aRow,
            KNullDes,
            ESubject);    
    
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

        if ( iSettings->GetNewMailStateL( mailBoxId, GetUnreadCountL(mailBoxId) ) )
            {
            iLiwIf->PublishIndicatorIconL( aWidgetInstance,
                                           aRowNumber,
                                           EMbmCmailhandlerpluginQgn_indi_cmail_unseen_msg );
            }
        else if( !IsOutboxEmptyL(mailBoxId) )
            {
            iLiwIf->PublishIndicatorIconL( aWidgetInstance,
                                           aRowNumber,
                                           EMbmCmailhandlerpluginQgn_indi_cmail_outbox_msg);
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
    TAny* /*aParam1*/,
    TAny* /*aParam2*/,
    TAny* /*aParam3*/ )
    {
    FUNC_LOG;

    // If backup or restore is ongoing ignore all events
    if ( iSettings->BackupOrRestoreMode() )
        {
        return;
        }
    switch ( aEvent )
        {
        case TFSEventNewMailbox:
            {
            HandleNewMailboxEventL( aMailbox );
            break;
            }
        case TFSEventMailboxRenamed:
            {
            HandleMailboxRenamedEventL( aMailbox );
            SetUpdateNeeded( aMailbox );
            UpdateFullL();
            break;
            }
        case TFSEventMailboxDeleted:
            {
            HandleMailboxDeletedEventL( aMailbox );
            SetUpdateNeeded( aMailbox );
            UpdateFullL();
            break;
            }
        case TFSEventNewMail:
            {
            iSettings->ToggleWidgetNewMailIconL( ETrue, aMailbox );
            SetUpdateNeeded( aMailbox );
            UpdateFullL();
            break;
            }
        case TFSEventMailDeleted:
            {
            SetUpdateNeeded( aMailbox );
            UpdateFullL();
            break;
            }
        case TFSEventMailChanged:
            {
            SetUpdateNeeded( aMailbox );
            UpdateFullL();
            break;
            }
        case TFSEventMailMoved:
            {
            SetUpdateNeeded( aMailbox );
            UpdateFullL();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleNewMailboxEventL
// ---------------------------------------------------------
//
void CMailCpsHandler::HandleNewMailboxEventL( const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    if (iWaitingForNewMailbox)
        {
        iSettings->AssociateWidgetToSetting( iWaitingForNewMailbox->Des(), aMailbox );
        }
    else
        {
        iLiwIf->AddWidgetToHomescreenL( aMailbox );
        }

    // update total mailbox count. 
    TInt intCount = TotalIntMailboxCount();
    User::LeaveIfError( RProperty::Set( KPSUidEmailServerCategory, KIntMailboxCount, intCount ) );
    }

// ---------------------------------------------------------
// CMailCpsHandler::HandleMailboxRenamedEventL
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
            iSettings->ToggleWidgetNewMailIconL( EFalse, aMailbox );
            break;
            }
        }
    // update total mailbox count. 
    TInt intCount = TotalIntMailboxCount();
    User::LeaveIfError( RProperty::Set( KPSUidEmailServerCategory, KIntMailboxCount, intCount ) );
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
        delete mailbox; // transferred ownership
        // Check that folder is correct
        CFSMailFolder* folder(NULL);                                                    
        TRAPD( err, folder = MailClient().GetFolderByUidL( aMailbox, folderId ) );
        if ( !folder || err != KErrNone )
            {
            return 0;
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
        delete mailbox; // transferred ownership
        // Check that folder is correct
        CFSMailFolder* folder(NULL);                                                    
        TRAPD( err, folder = MailClient().GetFolderByUidL( aMailbox, folderId ) );
        if ( !folder || err != KErrNone )        
            {
            return 0;
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
        delete mailbox; // ownership was transferred
        // Check that folder is correct
        CFSMailFolder* folder(NULL);
        TRAPD( err, folder = MailClient().GetFolderByUidL( aMailbox, folderId ) );
        if ( !folder || err != KErrNone )        
            {
            return ret;
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
        CleanupResetAndDestroyPushL( messages ); // owning array
        iterator->NextL( TFSMailMsgId(), msgCount, messages );

        TInt arrayCount(messages.Count());
        for (TInt i = 0; i < arrayCount; i++)
            {
            TFSMailMsgId msgId = messages[i]->GetMessageId();
            CFSMailMessage* msg( NULL );
            msg = MailClient().GetMessageByUidL( aMailbox, folderId, msgId, EFSMsgDataEnvelope );
			if (msg)
                {
                CleanupDeletePushL( msg );
                TFSMailMsgId mailboxId = msg->GetMailBoxId();
                CleanupStack::PopAndDestroy( msg );

                if (mailboxId == aMailbox)
                    {
                    ret = EFalse;
                    break;
                    }
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
    SetWaitingForNewMailbox( aContentId );
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::LaunchEmailUIL()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::LaunchEmailUIL( const TDesC& aContentId )
    {
    FUNC_LOG;

    TInt nativeMailboxId( iSettings->GetMailboxUidByContentId( aContentId ) );
    // Is the contentId related to internal mailbox or external?
    if( nativeMailboxId )
        {
        TUid mailBoxUid;
        mailBoxUid.iUid = nativeMailboxId;

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
            iEnv->EikAppUi()->ActivateViewL(
                TVwsViewId(KUidEmailUi, KMailListId),
                KStartListWithFolderIdFromHomeScreen, pkgOut);
            }
        CleanupStack::PopAndDestroy( mailBox );
        }
    else
        {
        LaunchExtAppL( aContentId );
        }
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::LaunchEmailWizardL()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::LaunchEmailWizardL( const TDesC& aContentId )
    {
    FUNC_LOG;
    TUid viewUid(KNullUid);
    iEnv->EikAppUi()->ActivateViewL( TVwsViewId(KUidWizardApp, KUidEmailWizardView),
                                     viewUid,
                                     KNullDesC8);
    SetWaitingForNewMailbox( aContentId );
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::LaunchExtAppL()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::LaunchExtAppL( const TDesC& aContentId )
    {
    FUNC_LOG;
    CMailPluginProxy* plugin = GetExtPluginL( aContentId );
    if ( plugin )
        {
        plugin->LaunchExtAppL( aContentId );
        }
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::GetExtPluginL
// -----------------------------------------------------------------------------
//
CMailPluginProxy* CMailCpsHandler::GetExtPluginL( const TDesC& aContentId )
    {
    FUNC_LOG;
    CMailPluginProxy* plugin( NULL );
    for( TInt i = 0; i < iExternalPlugins.Count(); i++ )
        {
        if ( iExternalPlugins[i]->HasAccount( aContentId ) )
            {
            plugin = iExternalPlugins[i];
            }
        }
    return plugin;
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
// CMailCpsHandler::TotalMailboxCount
// ---------------------------------------------------------------------------
//
TInt CMailCpsHandler::TotalMailboxCountL()
    {
    FUNC_LOG;
    return TotalIntMailboxCount() + TotalExtMailboxCountL();
    }

// ---------------------------------------------------------------------------
// CMailCpsHandler::GetMailboxCount
// ---------------------------------------------------------------------------
//
TInt CMailCpsHandler::TotalIntMailboxCount()
    {
    FUNC_LOG;
    return iSettings->TotalIntMailboxCount();
    }

// ---------------------------------------------------------
// CMailCpsHandler::TotalExtMailboxCount
// ---------------------------------------------------------
//
TInt CMailCpsHandler::TotalExtMailboxCountL()
    {
    FUNC_LOG;
    TInt totalMailboxCount( 0 );
    TUid interfaceUid = TUid::Uid( KEmailObserverInterfaceUid );
    RImplInfoPtrArray plugins;
    CleanupResetAndDestroyPushL(plugins); // CleanupClosePushL does not call ptr destructors
    REComSession::ListImplementationsL( interfaceUid, plugins);

    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        TUid implUid = plugins[i]->ImplementationUid();
        INFO_1("Instantiating plugin %d", implUid.iUid);
        EmailInterface::CEmailObserverPlugin* plugin =
            EmailInterface::CEmailObserverPlugin::NewL( implUid, this );
        MEmailData& data( plugin->EmailDataL() );
        totalMailboxCount += data.MailboxesL().Count();
        }

    CleanupStack::PopAndDestroy( &plugins ); // plugins
    return totalMailboxCount;
    }

// ---------------------------------------------------------------------------
// CMailCpsHandler::EmailObserverEvent
// ---------------------------------------------------------------------------
void CMailCpsHandler::EmailObserverEvent( EmailInterface::MEmailData& /*aData*/ )
    {
    FUNC_LOG;
    // Nothing to do
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::SetWaitingForNewMailbox()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::SetWaitingForNewMailbox( const TDesC& aContentId )
    {
    FUNC_LOG;
    HBufC* newCid = aContentId.Alloc();
    if ( newCid )
        {
        delete iWaitingForNewMailbox;
        iWaitingForNewMailbox = newCid;
        }
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::CleanWaitingForNewMailbox()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::CleanWaitingForNewMailbox()
    {
    FUNC_LOG;
	if (iWaitingForNewMailbox)
        {
        delete iWaitingForNewMailbox;
        iWaitingForNewMailbox = NULL;
        }
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::SetWaitingForNewWidget()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::SetWaitingForNewWidget( const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    iWaitingForNewWidget.SetPluginId( aMailbox.PluginId() );
    iWaitingForNewWidget.SetId( aMailbox.Id() );   
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::CleanWaitingForNewWidget()
// -----------------------------------------------------------------------------
//
void CMailCpsHandler::CleanWaitingForNewWidget()
    {
    FUNC_LOG;
    iWaitingForNewWidget.SetPluginId( KNullUid );
    iWaitingForNewWidget.SetId( 0 );
    }

// -----------------------------------------------------------------------------
//  CMailCpsHandler::WaitingForNewWidget()
// -----------------------------------------------------------------------------
//
TFSMailMsgId CMailCpsHandler::WaitingForNewWidget()
    {
    FUNC_LOG;
    return iWaitingForNewWidget;    
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::AssociateWidgetToSetting
// ---------------------------------------------------------------------------
//
void CMailCpsHandler::AssociateWidgetToSetting( const TDesC& aContentId,
                                                const TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    iSettings->AssociateWidgetToSetting( aContentId, aMailbox );
    }


// ---------------------------------------------------------------------------
// CMailCpsHandler::Associated
// ---------------------------------------------------------------------------
//
TBool CMailCpsHandler::Associated( const TDesC& aContentId )
    {
    FUNC_LOG;    
    return iSettings->Associated( aContentId );
    }

// ---------------------------------------------------------------------------
// CMailCpsSettings::DisplayHSPageFullNoteL
// ---------------------------------------------------------------------------
//
void CMailCpsHandler::DisplayHSPageFullNoteL()
    {
    FUNC_LOG;
    if (!iQuery)
        {
        iQuery = CAknGlobalNote::NewL();
        iQuery->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY);
        }   
    HBufC* str( NULL );
    str = StringLoader::LoadLC( R_EMAILWIDGET_TEXT_HS_PAGE_FULL );
    iQuery->ShowNoteL(EAknGlobalConfirmationNote, str->Des());
    CleanupStack::PopAndDestroy( str );    
    }

// ----------------------------------------------------------------------------
// class CMailCpsUpdateHelper : public CTimer
// Used to limit the rate of updates to Homescreen widget
// ----------------------------------------------------------------------------
//

CMailCpsUpdateHelper::CMailCpsUpdateHelper(
    CMailCpsHandler *aHandler )
    : CTimer( EPriorityStandard )  // Could be EPriorityLow
    , iCpsHandler( aHandler )
    , iPending( EFalse )
    {
    FUNC_LOG;
    }

CMailCpsUpdateHelper* CMailCpsUpdateHelper::NewLC( CMailCpsHandler* aHandler )
    {
    FUNC_LOG;
    CMailCpsUpdateHelper* self = new ( ELeave ) CMailCpsUpdateHelper( aHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CMailCpsUpdateHelper* CMailCpsUpdateHelper::NewL( CMailCpsHandler *aHandler )
    {
    FUNC_LOG;
    CMailCpsUpdateHelper* self = CMailCpsUpdateHelper::NewLC( aHandler );
    CleanupStack::Pop( self );
    return self;
    }

void CMailCpsUpdateHelper::ConstructL()
    {
    FUNC_LOG;
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

CMailCpsUpdateHelper::~CMailCpsUpdateHelper()
    {
    FUNC_LOG;
    iCpsHandler = NULL; // Not owned
    Cancel();   // Stop any pending request
    Deque();    // Remove from CActiveScheduler
    }

// Notification that Homescreen widget(s) should be updated
void CMailCpsUpdateHelper::UpdateL()
    {
    FUNC_LOG;
    if ( IsActive() )
        {
        // Timer is running, so just flag that an update should be sent when
        // the timer expires.
        iPending = ETrue;
        }
    else
        {
        // Timer is not running, so perform an update and set the timer
        // running.
        DoUpdateL();
        }
    }

// Actually perform an update of the Homescreen widget.  Also sets the timer
// running and clears the pending flag.
void CMailCpsUpdateHelper::DoUpdateL()
    {
    FUNC_LOG;
    // Set the timer running.
    After( KMailCpsHandlerUpdateDelay );
    // Clear the pending flag.
    iPending = EFalse;
    // Do the update.
    iCpsHandler->DoUpdateFullL();
    }

void CMailCpsUpdateHelper::RunL()
    {
    FUNC_LOG;
    if ( iPending )
        {
        // There was an update request since the last update, so do another
        // update (and set the timer running again, etc.).
        DoUpdateL();
        }
    }

TInt CMailCpsUpdateHelper::RunError( TInt aError )
    {
    FUNC_LOG;
    if ( KErrNone != aError )
        {
        Cancel();   // Stop any pending request
        }
    return KErrNone;    // Don't panic the thread
    }

