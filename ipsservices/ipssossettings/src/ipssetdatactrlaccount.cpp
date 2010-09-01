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
* Description: This file implements classes Account, Account. 
*
*/


#include "emailtrace.h"
#include <e32base.h>
#include <SendUiConsts.h>

#include "ipssetutils.h"
#include "ipssetdatactrlaccount.h"
#include "ipssetdatastorer.h"
#include "ipssetutilsexception.h"

enum TIpsSetUiAccountErrors
    {
    EIpsSetUiNoError = 0,
    EIpsSetUiMboxToAccIdMtmError,
    EIpsSetUiAccIdToExtIdMtmError,
    EIpsSetUiExtIdToAccIdMtmError
    };

// Account Id conversion to Extension Id
enum TIpsSetUiKeyShifts
    {
    EIpsSetUiKeyShiftImap4  = 0,
    EIpsSetUiKeyShiftPop3   = 8
    };

const TInt KIpsSetDataShiftAccountId = 8;

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// IpsSetDataCtrlAccount::CreateBaseKeyFromAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IpsSetDataCtrlAccount::CreateBaseKeyFromAccountId(
    const TUint32 aAccountId,
    const TUid& aProtocol )
    {
    FUNC_LOG;
    TUint32 extendedAccountId =
        AccountIdToExtendedAccountId( aAccountId, aProtocol );
    return ( extendedAccountId << KIpsSetDataShiftAccountId );
    }

// ----------------------------------------------------------------------------
// IpsSetDataCtrlAccount::CreateBaseKeyFromExtendedAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IpsSetDataCtrlAccount::CreateBaseKeyFromExtendedAccountId(
    const TUint32 aExtendedAccountId )
    {
    FUNC_LOG;
    return ( aExtendedAccountId << KIpsSetDataShiftAccountId );
    }

// ----------------------------------------------------------------------------
// IpsSetDataCtrlAccount::MailboxIdToAccountIdL()
// ----------------------------------------------------------------------------
//
TUint32 IpsSetDataCtrlAccount::MailboxIdToAccountIdL(
    const TMsvId aMailboxId,
    const TUid& aMtmId )
    {
    FUNC_LOG;
    TUint32 accountId = 0;

    switch ( aMtmId.iUid )
        {
        case KSenduiMtmSmtpUidValue:
            accountId = IpsSetUtils::GetSmtpAccountIdL(
                aMailboxId ).iSmtpAccountId;
            break;

        case KSenduiMtmPop3UidValue:
            accountId = IpsSetUtils::GetPopAccountIdL(
                aMailboxId ).iPopAccountId;
            break;

        case KSenduiMtmImap4UidValue:
            accountId = IpsSetUtils::GetImapAccountIdL(
                aMailboxId ).iImapAccountId;
            break;

        default:
            {
            IPSASSERT_LEAVE_OR_PANIC_L( EAccountCtrl, EInvalidAccount );
            }
            break;
        }

    return accountId;
    }

// ----------------------------------------------------------------------------
// IpsSetDataCtrlAccount::AccountIdToMailboxId()
// ----------------------------------------------------------------------------
//
TMsvId IpsSetDataCtrlAccount::AccountIdToMailboxId(
    CEmailAccounts& aAccounts,
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    FUNC_LOG;
    TMsvId mailboxId = KErrNotFound;

    // Get the correct accounts according to the protocol
    if ( aMtmId.iUid == KSenduiMtmImap4UidValue )
        {
        // IMAP4
        TImapAccount imapAccount;
        RArray<TImapAccount> accountArray;
        TRAPD( error, aAccounts.GetImapAccountsL( accountArray ) );

        TInt account = ( error == KErrNone ) ?
            accountArray.Count() : KErrNotFound;

        while ( mailboxId < 0 && --account >= 0 )
            {
            imapAccount = accountArray[account];

            // Check if the id's match
            if ( imapAccount.iImapAccountId == aAccountId )
                {
                mailboxId = imapAccount.iImapService;
                }
            }
        }
    else
        {
        // POP3
        TPopAccount popAccount;
        RArray<TPopAccount> accountArray;
        TRAPD( error, aAccounts.GetPopAccountsL( accountArray ) );

        TInt account = ( error == KErrNone ) ?
            accountArray.Count() : KErrNotFound;

        while ( mailboxId < 0 && --account >= 0 )
            {
            popAccount = accountArray[account];

            // Check if the id's match
            if ( popAccount.iPopAccountId == aAccountId )
                {
                mailboxId = popAccount.iPopService;
                }
            }
        }

    return mailboxId;
    }


// ----------------------------------------------------------------------------
// IpsSetDataCtrlAccount::AccountIdToExtendedAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IpsSetDataCtrlAccount::AccountIdToExtendedAccountId(
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    FUNC_LOG;
    // Imap4 accounts are 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    if ( aMtmId == KSenduiMtmImap4Uid )
        {
        return ( aAccountId << EIpsSetUiKeyShiftImap4 );
        }
    // Pop3 accounts are 0x100, 0x200, 0x300, 0x400, 0x500, 0x600
    else if ( aMtmId == KSenduiMtmPop3Uid )
        {
        return ( aAccountId << EIpsSetUiKeyShiftPop3 );
        }
    else
        {
        IPSASSERT_LEAVE_OR_PANIC_L( EAccountCtrl, EInvalidMtm );
        }

    return 0;
    }

// ----------------------------------------------------------------------------
// IpsSetDataCtrlAccount::ExtendedAccountIdToAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IpsSetDataCtrlAccount::ExtendedAccountIdToAccountId(
    const TUint32 aExtendedAccountId,
    const TUid& aMtmId )
    {
    FUNC_LOG;
    // Imap4 accounts are 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    if ( aMtmId == KSenduiMtmImap4Uid )
        {
        return ( aExtendedAccountId >> EIpsSetUiKeyShiftImap4 );
        }
    // Pop3 accounts are 0x100, 0x200, 0x300, 0x400, 0x500, 0x600
    else if ( aMtmId == KSenduiMtmPop3Uid )
        {
        return ( aExtendedAccountId >> EIpsSetUiKeyShiftPop3 );
        }
    else
        {        
        IPSASSERT_LEAVE_OR_PANIC_L( EAccountCtrl, EInvalidMtm );
        }

    return 0;
    }

// ----------------------------------------------------------------------------
// IpsSetDataCtrlAccount::CreateSettingKey()
// ----------------------------------------------------------------------------
//
TUint32 IpsSetDataCtrlAccount::CreateSettingKey(
    const TUint32 aAccountId,
    const TUint32 aSetting,
    const TUid& aMtmId )
    {
    FUNC_LOG;
    return CreateBaseKeyFromAccountId( aAccountId, aMtmId ) + aSetting;
    
    }    
// End of File

