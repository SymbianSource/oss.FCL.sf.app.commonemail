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
* Description: This file implements class IsMailMtm.
*
*/



#include "emailtrace.h"
#include <miuthdr.h>                // TMsvEmailEntry
#include <SendUiConsts.h>
#include <smtpset.h>

#include "ipssetutilsconsts.h"
#include "ipssetutils.h"
#include "ipssetdata.h"

_LIT( KIpsSetUtilsRfc2822Specials,"()<>@,;:\\\"[]");
_LIT( KIpsSetUtilsImapInboxName,"INBOX");
const TInt KIpsSetUtilsCharQuote = '\"';
const TInt KIpsSetUtilsCharBackSlash = '\\';
const TInt KIpsSetUtilsCharDot = '.';
const TInt KIpsSetUtilsCharSpace = ' ';
const TInt KIpsSetUtilsCharDel = 127;
const TInt KIpsSetUtilsCharAt = '@';
const TInt KIpsSetUtilsSpecialCharStrLen = 12;



// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// IpsSetUtils::IsMailbox()
// ----------------------------------------------------------------------------
//
TBool IpsSetUtils::IsMailbox(
    const TMsvEntry& aEntry )
    {
    FUNC_LOG;
    return aEntry.iType.iUid == KUidMsvServiceEntryValue &&
        IsMailMtm( aEntry.iMtm );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::IsMailMtm()
// ----------------------------------------------------------------------------
//
TBool IpsSetUtils::IsMailMtm( const TUid& aMtm )
    {
    FUNC_LOG;
    // Basic mail mtm's
    return
        ( aMtm == KSenduiMtmSmtpUid ) ||
        ( aMtm == KSenduiMtmPop3Uid ) ||
        ( aMtm == KSenduiMtmImap4Uid );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetMailboxEntryL()
// ----------------------------------------------------------------------------
//
TMsvEntry IpsSetUtils::GetMailboxEntryL(
    CMsvSession& aMsvSession,
    const TMsvId aMailboxId )
    {
    FUNC_LOG;
    // Get the entry of the mailbox
    TMsvId serviceId;
    TMsvEntry mailbox;
    User::LeaveIfError(
        aMsvSession.GetEntry( aMailboxId, serviceId, mailbox ) );

    return mailbox;
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetMailboxEntry()
// ----------------------------------------------------------------------------
//
TInt IpsSetUtils::GetMailboxEntry(
    CMsvSession& aMsvSession,
    const TMsvId aMailboxId,
    TMsvEntry& aMailbox )
    {
    FUNC_LOG;
    // Get the entry of the mailbox
    TMsvId serviceId;
    return aMsvSession.GetEntry( aMailboxId, serviceId, aMailbox );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetMailboxEntriesL()
// ----------------------------------------------------------------------------
//
TUid IpsSetUtils::GetMailboxEntriesL(
    const TMsvId aMailboxId,
    CMsvSession& aMsvSession,
    TMsvEntry& aSmtpEntry,
    TMsvEntry& aRelatedEntry )
    {
    FUNC_LOG;
    // Find the entry
    TMsvEntry entry = IpsSetUtils::GetMailboxEntryL(
        aMsvSession, aMailboxId );

    return GetMailboxEntriesL( entry, aMsvSession, aSmtpEntry, aRelatedEntry );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetMailboxEntriesL()
// ----------------------------------------------------------------------------
//
TUid IpsSetUtils::GetMailboxEntriesL(
    const TMsvEntry& aEntry,
    CMsvSession& aMsvSession,
    TMsvEntry& aSmtpEntry,
    TMsvEntry& aRelatedEntry )
    {
    FUNC_LOG;
    // Make sure the entry is mailbox
    if ( !IpsSetUtils::IsMailbox( aEntry ) )
        {
        User::Leave( KErrUnknown );
        }

    // Get the related entry
    TMsvEntry entry = IpsSetUtils::GetMailboxEntryL(
        aMsvSession, aEntry.iRelatedId );

    // Check if the main entry is smtp, and set the entries correctly
    if ( aEntry.iMtm == KSenduiMtmSmtpUid )
        {
        aSmtpEntry = aEntry;
        aRelatedEntry = entry;
        }
    else
        {
        aRelatedEntry = aEntry;
        aSmtpEntry = entry;
        }

    // return the mtm id
    return aRelatedEntry.iMtm;
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetSmtpAccountIdL()
// ----------------------------------------------------------------------------
//
TSmtpAccount IpsSetUtils::GetSmtpAccountIdL(
    CMsvSession& aMsvSession,
    const TMsvId aMailboxId )
    {
    FUNC_LOG;
    TMsvEntry mailbox =
        IpsSetUtils::GetMailboxEntryL( aMsvSession, aMailboxId );
    return GetSmtpAccountIdL( aMsvSession, mailbox );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetImapAccountIdL()
// ----------------------------------------------------------------------------
//
TImapAccount IpsSetUtils::GetImapAccountIdL(
    CMsvSession& aMsvSession,
    const TMsvId aMailboxId )
    {
    FUNC_LOG;
    TMsvEntry mailbox =
        IpsSetUtils::GetMailboxEntryL( aMsvSession, aMailboxId );
    return GetImapAccountIdL( aMsvSession, mailbox );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetPopAccountIdL()
// ----------------------------------------------------------------------------
//
TPopAccount IpsSetUtils::GetPopAccountIdL(
    CMsvSession& aMsvSession,
    const TMsvId aMailboxId )
    {
    FUNC_LOG;
    TMsvEntry mailbox =
        IpsSetUtils::GetMailboxEntryL( aMsvSession, aMailboxId );
    return GetPopAccountIdL( aMsvSession, mailbox );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetSmtpAccountIdL()
// ----------------------------------------------------------------------------
//
TSmtpAccount IpsSetUtils::GetSmtpAccountIdL(
    CMsvSession& aMsvSession,
    const TMsvEntry& aEntry )
    {
    FUNC_LOG;
    TMsvEntry smtpMailbox = aEntry;

    // Make sure the entry belongs to smtp mailbox
    if ( aEntry.iMtm != KSenduiMtmSmtpUid )
        {
        // Get the related mailbox
        smtpMailbox = IpsSetUtils::GetMailboxEntryL(
            aMsvSession, aEntry.iRelatedId );
        }

    return IpsSetUtils::GetSmtpAccountIdL( smtpMailbox.Id() );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetImapAccountIdL()
// ----------------------------------------------------------------------------
//
TImapAccount IpsSetUtils::GetImapAccountIdL(
    CMsvSession& aMsvSession,
    const TMsvEntry& aEntry )
    {
    FUNC_LOG;
    TMsvEntry imapMailbox = aEntry;

    switch ( aEntry.iMtm.iUid )
        {
        // Pop cannot transform to imap
        case KSenduiMtmPop3UidValue:
            User::Leave( KErrNotSupported );
            break;

        // Smtp need to be changed to related one
        case KSenduiMtmSmtpUidValue:
            imapMailbox = IpsSetUtils::GetMailboxEntryL(
                aMsvSession, aEntry.iRelatedId );
            break;

        // Accept the imap mailbox straight away
        default:
        case KSenduiMtmImap4UidValue:
            break;
        }

    // Make sure the entry belongs to imap mailbox
    if ( aEntry.iMtm.iUid != KSenduiMtmImap4UidValue )
        {
        User::Leave( KErrNotSupported );
        }

    return IpsSetUtils::GetImapAccountIdL( imapMailbox.Id() );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetPopAccountIdL()
// ----------------------------------------------------------------------------
//
TPopAccount IpsSetUtils::GetPopAccountIdL(
    CMsvSession& aMsvSession,
    const TMsvEntry& aEntry )
    {
    FUNC_LOG;
    TMsvEntry popMailbox = aEntry;

    switch ( aEntry.iMtm.iUid )
        {
        // Imap cannot transform to pop
        case KSenduiMtmImap4UidValue:
            User::Leave( KErrNotSupported );
            break;

        // Smtp need to be changed to related one
        case KSenduiMtmSmtpUidValue:
            popMailbox = IpsSetUtils::GetMailboxEntryL(
                aMsvSession, aEntry.iRelatedId );
            break;

        // Accept the pop mailbox straight away
        default:
        case KSenduiMtmPop3UidValue:
            break;
        }

    // Make sure the entry belongs to pop mailbox
    if ( aEntry.iMtm.iUid != KSenduiMtmPop3UidValue )
        {
        User::Leave( KErrNotSupported );
        }

    return IpsSetUtils::GetPopAccountIdL( popMailbox.Id() );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetPopAccountIdL()
// ----------------------------------------------------------------------------
//
TPopAccount IpsSetUtils::GetPopAccountIdL(
    const TMsvId aPopMailboxId )
    {
    FUNC_LOG;
    CEmailAccounts* account = CEmailAccounts::NewLC();

    TPopAccount popAccountId;
    account->GetPopAccountL( aPopMailboxId, popAccountId );

    CleanupStack::PopAndDestroy( account );

    return popAccountId;
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetImapAccountIdL()
// ----------------------------------------------------------------------------
//
TImapAccount IpsSetUtils::GetImapAccountIdL(
    const TMsvId aImapMailboxId )
    {
    FUNC_LOG;
    CEmailAccounts* account = CEmailAccounts::NewLC();

    TImapAccount imapAccountId;
    account->GetImapAccountL( aImapMailboxId, imapAccountId );

    CleanupStack::PopAndDestroy( account );

    return imapAccountId;
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetSmtpAccountIdL()
// ----------------------------------------------------------------------------
//
TSmtpAccount IpsSetUtils::GetSmtpAccountIdL(
    const TMsvId aSmtpMailboxId )
    {
    FUNC_LOG;
    CEmailAccounts* account = CEmailAccounts::NewLC();

    TSmtpAccount smtpAccountId;
    account->GetSmtpAccountL( aSmtpMailboxId, smtpAccountId );

    CleanupStack::PopAndDestroy( account );

    return smtpAccountId;
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetMailboxServiceId()
// ----------------------------------------------------------------------------
//
TInt IpsSetUtils::GetMailboxServiceId(
    CMsvSession& aMsvSession,
    TMsvEntry& aMailbox,
    const TMsvId aMailboxId,
    const TBool aGetSendingService )
    {
    FUNC_LOG;
    // Get the entry based on mailbox id
    TInt error = IpsSetUtils::GetMailboxEntry(
        aMsvSession, aMailboxId, aMailbox );
    TInt32 mtm = aMailbox.iMtm.iUid;

    // Continue handling, if successful
    if ( error == KErrNone && IpsSetUtils::IsMailbox( aMailbox ) )
        {
        // Get related entry, if:
        // Mailbox type is smtp and pop3 or imap4 is wanted OR
        // Mailbox type is pop3 or imap4 and smtp is wanted
        if ( aGetSendingService && mtm != KSenduiMtmSmtpUidValue ||
            !aGetSendingService && mtm == KSenduiMtmSmtpUidValue )
            {
            error = IpsSetUtils::GetMailboxEntry(
                aMsvSession, aMailbox.iRelatedId, aMailbox );
            }
        }

    return error;
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::CountSubscribedFoldersL()
// ----------------------------------------------------------------------------
//
TInt IpsSetUtils::CountSubscribedFoldersL(
    CMsvSession& aMsvSession,
    const TMsvId aMailboxId )
    {
    FUNC_LOG;
    TInt count = 0;

    DoCountSubscribedFoldersL( aMsvSession, aMailboxId, count );

    return count;
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::DoCountSubscribedFoldersL()
// ----------------------------------------------------------------------------
//
void IpsSetUtils::DoCountSubscribedFoldersL(
    CMsvSession& aMsvSession,
    const TMsvId aFolderId,
    TInt& aCount )
    {
    FUNC_LOG;
    // Get the folder entry
    CMsvEntry* folderEntry = aMsvSession.GetEntryL( aFolderId );
    CleanupStack::PushL( folderEntry );

    // Subscribed folders are hidden entries before the first sync
    TMsvSelectionOrdering order = folderEntry->SortType();
    order.SetShowInvisibleEntries( ETrue );
    folderEntry->SetSortTypeL( order );

    // Search through all the folders and locate the subscribed ones
    const TInt count = folderEntry->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        TMsvEmailEntry mailEntry = ( *folderEntry )[ loop ];
        if ( mailEntry.iType.iUid == KUidMsvFolderEntryValue )
            {
            if ( mailEntry.LocalSubscription() &&
                !IsInbox( aMsvSession, mailEntry) )
                {
                aCount++;
                }

            // Go into the folder to search the new entry
            DoCountSubscribedFoldersL( aMsvSession, mailEntry.Id(), aCount );
            }
        }

    CleanupStack::PopAndDestroy( folderEntry );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::HasSubscribedFoldersL()
// ----------------------------------------------------------------------------
//
TBool IpsSetUtils::HasSubscribedFoldersL(
    CMsvSession& aMsvSession,
    const TMsvId aFolderId )
    {
    FUNC_LOG;
    // Get the folder entry
    CMsvEntry* folderEntry = aMsvSession.GetEntryL( aFolderId );
    CleanupStack::PushL( folderEntry );

    // Subscribed folders are hidden entries before the first sync
    TMsvSelectionOrdering order = folderEntry->SortType();
    order.SetShowInvisibleEntries( ETrue );
    folderEntry->SetSortTypeL( order );

    TBool found( EFalse );

    // Search through all the folders and locate the subscribed ones
    const TInt count = folderEntry->Count();
    for ( TInt loop = 0; !found && loop < count; loop++ )
        {
        TMsvEmailEntry mailEntry = ( *folderEntry )[ loop ];
        if ( mailEntry.iType.iUid == KUidMsvFolderEntryValue )
            {
            if ( mailEntry.LocalSubscription() &&
                !IsInbox( aMsvSession, mailEntry ) )
                {
                found = ETrue;
                }
            else
                {
                // Go into the folder to search the new entry
                found = HasSubscribedFoldersL( aMsvSession, mailEntry.Id() );
                }
            }
        }

    CleanupStack::PopAndDestroy( folderEntry );

    return found;
    }


// ----------------------------------------------------------------------------
// IpsSetUtils::IsInbox()
// ----------------------------------------------------------------------------
//
TBool IpsSetUtils::IsInbox(
    CMsvSession& aMsvSession,
    const TMsvEntry& aFolderEntry )
    {
    FUNC_LOG;
    TBool isInbox( EFalse );
    if ( aFolderEntry.iDetails.CompareF( KIpsSetUtilsImapInboxName ) == 0 )
        {
        // If parent is Mailbox service, then this is IMAP4 protocol inbox,
        // otherwise it is something else...
        TMsvId serviceId;
        TMsvEntry entry;
        TInt ret = aMsvSession.GetEntry(
            aFolderEntry.Parent(), serviceId, entry );
        if ( ( ret == KErrNone ) && ( entry.Id() == serviceId ) )
            {
            isInbox = ETrue;
            }
        }
    return isInbox;
    }


// ----------------------------------------------------------------------------
// IpsSetUtils::IsValidEmailAddressL
// ----------------------------------------------------------------------------
EXPORT_C TBool IpsSetUtils::IsValidEmailAddressL( const TDesC& aAddress )
    {
    FUNC_LOG;
    TInt c;
    TInt length = aAddress.Length ();
    TBufC<KIpsSetUtilsSpecialCharStrLen>
        rfc822Specials ( KIpsSetUtilsRfc2822Specials );

    // first we validate the name portion (name@domain)
    if ( length && aAddress[0] == KIpsSetUtilsCharDot )
        {
        return EFalse;
        }
    for ( c = 0 ; c < length ; c++ )
        {
        if ( aAddress[c] == KIpsSetUtilsCharQuote && ( c == 0 ||
            aAddress[c-1] == KIpsSetUtilsCharDot ||
            aAddress[c-1] == KIpsSetUtilsCharQuote ) )
            {
            while ( ++c < length )
                {
                if ( aAddress[c] == KIpsSetUtilsCharQuote )
                    {
                    break;
                    }
                if ( aAddress[c] == KIpsSetUtilsCharBackSlash &&
                    ( aAddress[++c] == KIpsSetUtilsCharSpace) )
                    {
                    continue;
                    }
                if ( aAddress[c] <= KIpsSetUtilsCharSpace ||
                    aAddress[c] >= KIpsSetUtilsCharDel )
                    {
                    return EFalse;
                    }
                }
            if ( c++ == length )
                {
                return EFalse;
                }
            if ( aAddress[c] == KIpsSetUtilsCharAt )
                {
                break;
                }
            if ( aAddress[c] != KIpsSetUtilsCharDel )
                {
                return EFalse;
                }
            continue;
            }
        if ( aAddress[c] == KIpsSetUtilsCharAt )
            {
            break;
            }
        if ( aAddress[c] <= KIpsSetUtilsCharSpace || aAddress[c] >= KIpsSetUtilsCharDel )
            {
            return EFalse;
            }
        if ( rfc822Specials.Locate ( aAddress[c] ) != KErrNotFound )
            {
            return EFalse;
            }
        }
    if ( c == 0 || aAddress[c-1] == KIpsSetUtilsCharDot )
        {
        return EFalse;
        }

    // next we validate the domain portion (name@domain)
    if ( c == length )
        {
        return EFalse;
        }
    else
        {
        c++;
        return IsValidDomainL ( aAddress.Mid ( ( c ) , length-c ) );
        }
    }

// -----------------------------------------------------------------------------
// IpsSetUtils::IsValidDomainL
// -----------------------------------------------------------------------------
TBool IpsSetUtils::IsValidDomainL ( const TDesC& aDomain )
    {
    FUNC_LOG;
    TInt c = 0;
    TInt count = 0;
    TInt length = aDomain.Length ();
    TBufC<KIpsSetUtilsSpecialCharStrLen>
        rfc2822Specials( KIpsSetUtilsRfc2822Specials );

    if ( length == 0 )
        {
        return EFalse;
        }

    do
        {
        if ( aDomain[c] == KIpsSetUtilsCharDot )
            {
            if ( c == 0 || aDomain[c-1] == KIpsSetUtilsCharDot )
                {
                return EFalse;
                }
            count++;
            }
        if ( aDomain[c] <= KIpsSetUtilsCharSpace ||
             aDomain[c] >= KIpsSetUtilsCharDel )
            {
            return EFalse;
            }
        if ( rfc2822Specials.Locate( aDomain[c] ) != KErrNotFound )
            {
            return EFalse;
            }
        }
    while ( ++c < length );

    return ( ( count >= 1 ) && ( aDomain[length-1] != '.' ) );
    }

// ----------------------------------------------------------------------------
// IpsSetUtils::GetRelatedMailboxId()
// ----------------------------------------------------------------------------
//
TInt IpsSetUtils::GetRelatedMailboxId(
    CMsvSession& aMsvSession,
    TMsvId& aMailboxId )
    {
    FUNC_LOG;
    // Get the entry based on mailbox id
    TMsvEntry entry;
    TMsvId serviceId;
    TInt error = aMsvSession.GetEntry( aMailboxId, serviceId, entry );

    // Get the related id
    aMailboxId = ( error == KErrNone ) ? entry.iRelatedId : 0;

    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::GetDefaultSecurityPort()
// ----------------------------------------------------------------------------
//
TUint32 IpsSetUtils::GetDefaultSecurityPort(
    const TInt aSecurity,
    const TBool aIncoming,
    const TBool aIsImap4 )
    {
    FUNC_LOG;
    TUint32 port( 0 );

    switch( aSecurity )
        {
        case CIpsSetData::EStartTls:
        case CIpsSetData::ESecurityOff:
            {
            if ( aIncoming )
                {
                port = aIsImap4 ? KIpsSetDataDefaultPortImap4 :
                    KIpsSetDataDefaultPortPop3;
                }
            else
                {
                port = KIpsSetDataDefaultPortSmtp;
                }
            }
            break;
        case CIpsSetData::ESslTls:
            {
            if ( aIncoming )
                {
                port = aIsImap4 ? KIpsSetDataDefaultSecurityPortImap4 :
                    KIpsSetDataDefaultSecurityPortPop3;
                }
            else
                {
                port = KIpsSetDataDefaultSecurityPortSmtp;
                }
            }
            break;
        default:
            break;
        }
    return port;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::GetServiceIdFromAccountId()
// ----------------------------------------------------------------------------
//
void IpsSetUtils::GetServiceIdFromAccountIdL(
    const TUint32 aAccountId,
    const TUid aMtmId,
    TMsvId& aServiceId,
    TMsvId& aSmtpId )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( (aMtmId == KSenduiMtmPop3Uid || 
            aMtmId == KSenduiMtmImap4Uid ), 
            User::Panic(KIpsSetGenPanicLit, KErrArgument) );
    
    TMsvId retService = KErrNotFound;
    TMsvId retSmtp = KErrNotFound;
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    if ( aMtmId == KSenduiMtmPop3Uid )
        {
        RArray<TPopAccount> popAccounts;
        accounts->GetPopAccountsL( popAccounts );
        for ( TInt i = 0; i < popAccounts.Count() ; i++ )
            {
            if ( popAccounts[i].iPopAccountId == aAccountId )
                {
                retService = popAccounts[i].iPopService;
                retSmtp = popAccounts[i].iSmtpService;
                }
            }
        popAccounts.Close();
        }
    else if ( aMtmId == KSenduiMtmImap4Uid )
        {
        RArray<TImapAccount> imapAccounts;
        accounts->GetImapAccountsL( imapAccounts );
        for ( TInt i = 0; i < imapAccounts.Count() ; i++ )
            {
            if ( imapAccounts[i].iImapAccountId == aAccountId )
                {
                retService = imapAccounts[i].iImapService;
                retSmtp = imapAccounts[i].iSmtpService;
                }
            }
        imapAccounts.Close();
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    if ( retService != KErrNotFound )
        {
        aServiceId = retService;
        aSmtpId = retSmtp;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( accounts );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::GetSmtpSettingsFromAccountIdL()
// ----------------------------------------------------------------------------
//
CImSmtpSettings* IpsSetUtils::GetSmtpSettingsFromAccountIdL(
    const TUint32 aAccountId,
    const TUid aMtmId )
    {
    FUNC_LOG;
    TMsvId serviceId;
    TMsvId smtpId;
    IpsSetUtils::GetServiceIdFromAccountIdL(
        aAccountId, aMtmId, serviceId, smtpId );
    
    CImSmtpSettings* smtpSettings = new ( ELeave ) CImSmtpSettings();
    CleanupStack::PushL( smtpSettings );
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TSmtpAccount smtpAcc;
    accounts->GetSmtpAccountL( smtpId, smtpAcc );
    accounts->LoadSmtpSettingsL( smtpAcc, *smtpSettings );
    CleanupStack::PopAndDestroy( accounts );
    CleanupStack::Pop( smtpSettings );
    return smtpSettings;
    }


// End of File

