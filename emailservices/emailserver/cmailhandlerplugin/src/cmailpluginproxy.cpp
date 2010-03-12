/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file implements class CMailMailboxDetails.
*
*/

#include <cmailhandlerplugin.mbg> // icons
#include <StringLoader.h>
#include <emailwidget.rsg>

#include <memaildata.h>
#include <emailobserverplugin.h>
#include <memailmailboxdata.h>
#include <memailapplaunchdata.h>

#include "emailtrace.h"
#include "cmailpluginproxy.h"
#include "cmailexternalaccount.h"
#include "cmailcpsif.h"
#include "cmailcpsutils.h"

using namespace EmailInterface;

// ---------------------------------------------------------
// CMailPluginProxy::NewL
// ---------------------------------------------------------
//
CMailPluginProxy* CMailPluginProxy::NewL(
    const TInt aPluginId,
    CMailCpsIf& aPublisher )
    {
    FUNC_LOG;
    CMailPluginProxy* self = new(ELeave) CMailPluginProxy( aPluginId, aPublisher );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMailPluginProxy::CMailPluginProxy
// ---------------------------------------------------------
//
CMailPluginProxy::CMailPluginProxy(
    const TInt aPluginId,
    CMailCpsIf& aPublisher ) :
    iPluginId( aPluginId ),
    iPublisher( aPublisher )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CMailPluginProxy::~CMailPluginProxy
// ---------------------------------------------------------
//
CMailPluginProxy::~CMailPluginProxy()
    {
    FUNC_LOG;
    delete iPlugin;
    TRAP_IGNORE( RemoveAccountsL() );
    iAccounts.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CMailPluginProxy::ConstructL
// ---------------------------------------------------------
//
void CMailPluginProxy::ConstructL()
    {
    FUNC_LOG;
    TUid implUid = TUid::Uid( iPluginId ); 
    iPlugin = EmailInterface::CEmailObserverPlugin::NewL( implUid, this );
    }

// ---------------------------------------------------------
// CMailPluginProxy::EmailObserverEvent
// ---------------------------------------------------------
//
void CMailPluginProxy::EmailObserverEvent( MEmailData& /*aData*/ )
    {
    FUNC_LOG;
    TRAP_IGNORE( PublishAccountsL() );
    }

// ---------------------------------------------------------
// CMailPluginProxy::PluginId
// ---------------------------------------------------------
//
TInt CMailPluginProxy::PluginId()
    {
    FUNC_LOG;
    return iPluginId;
    }

// ---------------------------------------------------------
// CMailPluginProxy::RemoveAccountsL
// ---------------------------------------------------------
//
void CMailPluginProxy::RemoveAccountsL()
    {
    FUNC_LOG;
    while ( iAccounts.Count() )
        {
        delete iAccounts[0];
        iAccounts.Remove(0);
        }
    }

// ---------------------------------------------------------
// CMailPluginProxy::ResetAccountL
// ---------------------------------------------------------
//
void CMailPluginProxy::ResetAccountL( const TDesC& aContentId )
    {
    FUNC_LOG;
    TInt instId = ResolveWidgetInstance( aContentId );
    if ( instId >= 0 )
        {
        TFSMailMsgId dummy;
        iPublisher.PublishMailboxIconL( instId, KNullDesC );
        iPublisher.PublishActiveMailboxNameL( instId, 1, KNullDesC);
        iPublisher.PublishIndicatorIconL( instId, 1, KNullIcon );

        ResetMessageRowL( instId, 2 ); // 1st message row
        ResetMessageRowL( instId, 3 ); // 2nd..
        }
    }

// ---------------------------------------------------------
// CMailPluginProxy::ResetMessageRowL
// ---------------------------------------------------------
//
void CMailPluginProxy::ResetMessageRowL( const TInt aWidgetInstanceId, const TInt aRow )
    {
    FUNC_LOG;
    TFSMailMsgId dummy;
    iPublisher.PublishMailboxIconL( aWidgetInstanceId, aRow, KNullIcon, dummy );
    iPublisher.PublishMailDetailL( aWidgetInstanceId, aRow, KNullDesC, ESender );
    iPublisher.PublishMailDetailL( aWidgetInstanceId, aRow, KNullDesC, ETime );
    }

// ---------------------------------------------------------
// CMailPluginProxy::SelectOwnAccountsL
// ---------------------------------------------------------
//
void CMailPluginProxy::SelectOwnAccountsL( RPointerArray<CMailExternalAccount>& aAccounts )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < aAccounts.Count(); i++ )
        {
        if ( aAccounts[i]->PluginId() == iPluginId )
            {
            // own account, take ownership
            iAccounts.AppendL( aAccounts[i] );
            aAccounts.Remove(i);
            i--; // update loop indexing
            }
        }
    }

// ---------------------------------------------------------
// CMailPluginProxy::PublishAccountsL
// ---------------------------------------------------------
//
void CMailPluginProxy::PublishAccountsL()
    {
    FUNC_LOG;
    // Loops through all external accounts of this plugin (that are configured
    // to be published), and calls publish on them
    for ( TInt i = 0; i < iAccounts.Count(); i++ )
        {
        TInt instId = ResolveWidgetInstance( iAccounts[i]->ContentId() );
        if ( instId != KErrNotFound && iPublisher.AllowedToPublish( instId ) )
            {
            PublishAccountL( instId, iAccounts[i]->MailboxId() );
            }
        else
            {
            INFO("Correct widget not found, not publishing..");
            }
        }
    }

// ---------------------------------------------------------
// CMailPluginProxy::PublishAccountL
// ---------------------------------------------------------
//
void CMailPluginProxy::PublishAccountL( const TInt aWidgetInstanceId, const TInt aMailboxId )
    {
    FUNC_LOG;
    // Loops through plugin's mailbox array and finds matching mailbox
    MEmailData& data( iPlugin->EmailDataL() );
    RPointerArray<MMailboxData> mailboxes = data.MailboxesL();
    for ( TInt j = 0; j < mailboxes.Count(); j++ )
        {
        if ( mailboxes[j]->MailboxId() == aMailboxId )
            {
            PublishAccountL( aWidgetInstanceId, *(mailboxes[j]) );
            }
        }
    }

// ---------------------------------------------------------
// CMailPluginProxy::PublishAccountL
// ---------------------------------------------------------
//
void CMailPluginProxy::PublishAccountL( const TInt aWidgetInstanceId, MMailboxData& aMailboxData )
    {
    FUNC_LOG;    
    // 1st row
    PublishFirstRowL( aWidgetInstanceId, aMailboxData );
    
    // message rows
    TInt msgCount = aMailboxData.LatestMessagesL().Count();
    for ( TInt i = 0; i < KMaxMsgCount; i++ )
        {
        if ( msgCount > i )
            {
            // enough messages for this row
            PublishMessageRowL( aWidgetInstanceId, *(aMailboxData.LatestMessagesL()[i]), i+2 ); // message-rows start from 2
            }
        else
            {
            // make sure this row is empty
            TFSMailMsgId dummy;
            iPublisher.PublishMailboxIconL( aWidgetInstanceId, i+2, KNullIcon, dummy );
            iPublisher.PublishMailDetailL( aWidgetInstanceId, i+2, KNullDesC, ESender );
            iPublisher.PublishMailDetailL( aWidgetInstanceId, i+2, KNullDesC, ESubject );            
            iPublisher.PublishMailDetailL( aWidgetInstanceId, i+2, KNullDesC, ETime );
            }
        }
    }

// ---------------------------------------------------------
// CMailPluginProxy::PublishMessageL
// ---------------------------------------------------------
//
void CMailPluginProxy::PublishMessageRowL( const TInt aWidgetInstanceId, EmailInterface::MMessageData& aMessageData, const TInt aRow )
    {
    FUNC_LOG;
    TFSMailMsgId dummy;
    TInt iconId = TMailCpsUtils::ResolveIcon(
        aMessageData.Unread(),
        aMessageData.CalendarMsg(),
        aMessageData.Attachments(),
        aMessageData.Priority(),
        aMessageData.Replied(),
        aMessageData.Forwarded() );
    
    iPublisher.PublishMailboxIconL( aWidgetInstanceId, aRow, iconId, dummy );
    iPublisher.PublishMailDetailL( aWidgetInstanceId, aRow, aMessageData.Sender(), ESender );
    iPublisher.PublishMailDetailL( aWidgetInstanceId, aRow, aMessageData.Subject(), ESubject );
    HBufC* timeString = CMailCpsHandler::GetMessageTimeStringL( aMessageData.TimeStamp() );
    CleanupStack::PushL( timeString );
    iPublisher.PublishMailDetailL( aWidgetInstanceId, aRow, *timeString, ETime );
    CleanupStack::PopAndDestroy( timeString );
    }

// ---------------------------------------------------------
// CMailPluginProxy::PublishFirstRowL
// ---------------------------------------------------------
//
void CMailPluginProxy::PublishFirstRowL( const TInt aWidgetInstanceId, EmailInterface::MMailboxData& aMailboxData )
    {
    FUNC_LOG;
    HBufC* firstLineText = ConstructMailboxNameWithMessageCountLC( aMailboxData );
    TInt indicator = ResolveIndicatorIcon( aMailboxData );
    iPublisher.PublishMailboxIconL( aWidgetInstanceId, aMailboxData.BrandingIcon() );
    iPublisher.PublishMailboxNameL( aWidgetInstanceId, *firstLineText, KTriggerEmailUi8 );
    iPublisher.PublishIndicatorIconL( aWidgetInstanceId, 1, indicator );
    CleanupStack::PopAndDestroy( firstLineText );
    }

// ---------------------------------------------------------
// CMailPluginProxy::ConstructMailboxNameWithMessageCountLC
// ---------------------------------------------------------
//
HBufC* CMailPluginProxy::ConstructMailboxNameWithMessageCountLC( MMailboxData& aMailboxData )
    {
    FUNC_LOG;
    HBufC* mailboxAndCount( NULL );
    TInt unreadCount = UnreadCountL( aMailboxData );
    if ( unreadCount > 0 )
        {
        // Arrays must be used when loc string contains indexed parameters
        CDesCArrayFlat* strings = new CDesCArrayFlat( 1 );
        CleanupStack::PushL( strings );
        strings->AppendL( aMailboxData.Name() ); // replace "%0U" with mailbox name
    
        CArrayFix<TInt>* ints = new(ELeave) CArrayFixFlat<TInt>( 1 );
        CleanupStack::PushL( ints );
        ints->AppendL( unreadCount ); // replace "%1N" with number of unread messages
    
        mailboxAndCount = StringLoader::LoadL( R_EMAILWIDGET_TEXT_MAILBOX_AND_MAILCOUNT, *strings, *ints );
        CleanupStack::PopAndDestroy(ints);
        CleanupStack::PopAndDestroy(strings);
        CleanupStack::PushL( mailboxAndCount );
        }
    else
        {
        // if 0 messages, first line text is pure mailbox name
        mailboxAndCount = aMailboxData.Name().AllocLC();
        }
    INFO_1("first line: %S", mailboxAndCount);
    return mailboxAndCount;
    }

// ---------------------------------------------------------
// CMailPluginProxy::UnreadCountL
// ---------------------------------------------------------
//
TInt CMailPluginProxy::UnreadCountL( EmailInterface::MMailboxData& aMailboxData )
    {
    FUNC_LOG;
    TInt count( 0 );
    for ( TInt i = 0; i < aMailboxData.LatestMessagesL().Count(); i++ )
        {
        if ( aMailboxData.LatestMessagesL()[i]->Unread() )
            {
            count++;
            }
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::ResolveIndicatorIcon
// ---------------------------------------------------------------------------
//
TInt CMailPluginProxy::ResolveIndicatorIcon( EmailInterface::MMailboxData& aMailboxData )
    {
    FUNC_LOG;
    TInt indicator( 0 );
    if ( aMailboxData.Unseen() )
        {
        indicator = EMbmCmailhandlerpluginQgn_stat_message_mail_uni;
        }
    else if ( !aMailboxData.IsOutboxEmpty() )
        {
        indicator = EMbmCmailhandlerpluginQgn_indi_cmail_outbox_msg;
        }
    else
        {
        indicator = KNullIcon;
        }
    return indicator;
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::ResolveWidgetInstance
// ---------------------------------------------------------------------------
//
TInt CMailPluginProxy::ResolveWidgetInstance( const TDesC& aContentId )
    {
    FUNC_LOG;
    TInt id( KErrNotFound );
    for ( TInt i = 0; i < iPublisher.iInstIdList.Count(); i++ )
        {
        if ( aContentId.Compare( *(iPublisher.iInstIdList[i]) ) == 0 )
            {
            id = i;
            }
        }
    return id;
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::HasAccount
// ---------------------------------------------------------------------------
//
TBool CMailPluginProxy::HasAccount( const TDesC& aContentId )
    {
    FUNC_LOG;
    TBool found( EFalse );
    for ( TInt i = 0; i < iAccounts.Count(); i++ )
        {
        if ( !( iAccounts[i]->ContentId().Compare( aContentId ) ) )
            {
            found = ETrue;
            }
        }
    return found;
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::LaunchExtAppL
// ---------------------------------------------------------------------------
//
void CMailPluginProxy::LaunchExtAppL( const TDesC& aContentId )
    {
    FUNC_LOG;
    MMailboxData& mailbox = GetMailboxDataL( GetMailboxId( aContentId ) );
    MEmailLaunchParameters& launchParams = mailbox.LaunchParameters();

    CEikonEnv* env = CEikonEnv::Static();
    TVwsViewId view( launchParams.ApplicationUid(), launchParams.ViewId() );
    if ( launchParams.CustomMessageId().iUid == 0 )
        {
        env->EikAppUi()->ActivateViewL( view, launchParams.CustomMessageId(), launchParams.CustomMessage() );
        }
    else
        {
        env->EikAppUi()->ActivateViewL( view );
        }
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::ResolveMailboxId
// ---------------------------------------------------------------------------
//
TInt CMailPluginProxy::GetMailboxId( const TDesC& aContentId )
    {
    FUNC_LOG;
    TInt mailboxId( 0 );
    for ( TInt i = 0; i < iAccounts.Count(); i++ )
        {
        if ( !(iAccounts[i]->ContentId().Compare( aContentId )) )
            {
            mailboxId = iAccounts[i]->MailboxId();
            }
        }
    return mailboxId;
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::GetMailboxDataL
// ---------------------------------------------------------------------------
//
EmailInterface::MMailboxData& CMailPluginProxy::GetMailboxDataL( TInt aMailboxId )
    {
    FUNC_LOG;
    TInt index( KErrNotFound );
    RPointerArray<MMailboxData> mailboxes = iPlugin->EmailDataL().MailboxesL();
    for ( TInt i = 0; i < mailboxes.Count(); i++ )
        {
        if ( mailboxes[i]->MailboxId() == aMailboxId )
            {
            index = i;
            }
        }

    if ( index == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    
    return *(mailboxes[index]);
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::SelectAndUpdateAccountsL
// ---------------------------------------------------------------------------
//
void CMailPluginProxy::SelectAndUpdateAccountsL( RPointerArray<CMailExternalAccount>& aAccounts )
    {
    FUNC_LOG;
    RemoveAccountsL();
    SelectOwnAccountsL( aAccounts );
    PublishAccountsL();
    }

// ---------------------------------------------------------------------------
// CMailPluginProxy::UpdateAccountL
// ---------------------------------------------------------------------------
//
void CMailPluginProxy::UpdateAccountL( const TDesC& aContentId )
    {
    FUNC_LOG;
    PublishAccountL(
        ResolveWidgetInstance( aContentId ), 
        GetMailboxId( aContentId ) );
    }
