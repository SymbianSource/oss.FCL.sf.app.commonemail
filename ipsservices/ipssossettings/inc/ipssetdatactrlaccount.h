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
* Description:  This class handles the complex account id shifting operations.
*
*/


#ifndef IPSSETDATACTRLACCOUNT_H
#define IPSSETDATACTRLACCOUNT_H


#include <msvstd.h>                     // TMsvId

class CEmailAccounts;

/**
 *  Definitions:
 *   - Mailbox Id :  TMsvId, TEntry.Id()
 *                   Entry Id of mail service entry.
 *   - AccountId  :  TUint32, TEntry.iMtmData2
 *                   Index of the Symbian account in the Central Repository
 *                   for the specific protocol. Each protocol can have
 *                   account id of 1. Can get values from 1 to 6.
 *   - ExtensionId : TUint32 (a.k.a ExtendedAccountId)
 *                   Index of the S60 account in the Central Repository for
 *                   the specific protocol. Extension Id is always unique.
 *                   Can get any value except 0.
 *                       Imap4 keys   : 0x00000001 - 0x000000ff
 *                        Pop3 keys   : 0x00000100 - 0x0000ff00
 *   - Base Key   :  Extension id shifted left by 8 (1 byte). This basekey
 *                   is then used to form the unique key id to refer the
 *                   setting item.
 *                        Imap4 keys  : 0x000001xx - 0x0000ffxx
 *                        Pop3 keys   : 0x000100xx - 0x00ff00xx
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class IpsSetDataCtrlAccount
    {
public:

// New functions

    /**
     * Function to create a base key.
     * Creates the base key from account id for storing the keys
     * into extended settings.
     *
     * @param aAccountId Id of the symbian account
     * @param aProtocol Protocol MTM ID of the account
     * @return Base key
     */
    static TUint32 CreateBaseKeyFromAccountId(
        const TUint32 aAccountId,
        const TUid& aProtocol );

    /**
     * Function to create a base key.
     * Creates base key from own extended settings. The key is used
     * to store the settings to extended settings part.
     *
     * @param aExtenededAccountId
     * @return Base key
     */
    static TUint32 CreateBaseKeyFromExtendedAccountId(
        const TUint32 aExtendedAccountId );

    /**
     * Converts the account ID to extended account ID.
     *
     * @param aAccountId Symbian account ID.
     * @param aProtocol Protocol MTM ID of the used account
     * @return Extended account Id
     */
    static TUint32 AccountIdToExtendedAccountId(
        const TUint32 aAccountId,
        const TUid& aMtmId );

    /**
     * Converts extended account ID to Symbian account ID.
     *
     * @param aExtension Account's Extension ID
     * @param aMtmID Protocol MTM ID of the used account
     * @return Symbian account ID
     */
    static TUint32 ExtendedAccountIdToAccountId(
        const TUint32 aExtension,
        const TUid& aMtmId );

    /**
     * Converts mailbox ID to account ID.
     *
     * @param aMailboxId Mailbox entry ID
     * @param aMtmId Protocol MTM ID of the used entry.
     * @return Symbian account ID
     */
    static TUint32 MailboxIdToAccountIdL(
        const TMsvId aMailboxId,
        const TUid& aMtmId );

    /**
     * Converts Symbian mailbox account ID to mailbox service entry ID.
     *
     * @param aAccountId Symbian account ID.
     * @param aMtmId Protocol MTM ID of the used entry.
     */
    static TMsvId AccountIdToMailboxId(
        CEmailAccounts& aAccounts,
        const TUint32 aAccountId,
        const TUid& aMtmId );

    /**
     * Creates setting key for account and its item.
     *
     * @param aAccountId Symbian account ID.
     * @param aSetting Unique ID of the setting
     * @param aMtmId Protocol MTM ID of the used entry.
     */
    static TUint32 CreateSettingKey(
        const TUint32 aAccountId,
        const TUint32 aSetting,
        const TUid& aMtmId );
    };

#endif // IPSSETDATACTRLACCOUNT_H

// End of File
