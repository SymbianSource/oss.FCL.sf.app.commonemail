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
* Description:  Utility functions for setting UI.
*
*/


#ifndef IPSSETUTILS_H
#define IPSSETUTILS_H


#include <msvstd.h>                 // TEntry
#include <cemailaccounts.h>         // CEmailAccounts

class CMsvSession;
class CBaseMtmUi;
/**
 *  This class contains simple tools for mailbox handling
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class IpsSetUtils
    {
public:

// New functions

    /**
     * Checks if given entry contains mailbox properties
     *
     * @param aEntry entry to be checked
     * @return ETrue The entry is mailbox entry
     * @return EFalse The entry is not mailbox entry
     */
    static TBool IsMailbox(
        const TMsvEntry& aEntry );

    /**
     * Checks if given mtm-type matches known mail mtm's
     *
     * @param aMtm Mtm to be tested
     * @param aAllowExtended Other than basic mail mtm's are accepted also
     * @return EFalse if the mtm is not recognized
     * @return ETrue if the mtm is supported type (Imap4 / Pop3 / Smtp )
     */
    static TBool IsMailMtm( const TUid& aMtm );

    /**
     * Finds and returns entry of the mailbox with specified id
     *
     * @param aMsvSession Session to be used for the search
     * @param aMailboxId Id of the required mailbox
     * @return Mailbox entry
     */
    static TMsvEntry GetMailboxEntryL(
        CMsvSession& aMsvSession,
        const TMsvId aMailboxId );

    /**
     * Finds and entry of the mailbox with specified id
     *
     * @param aMsvSession Session to be used for the search
     * @param aMailboxId Id of the required mailbox
     * @param aMailbox The entry the result shall be stored
     * @return KErrNone when the search is successful
     */
    static TInt GetMailboxEntry(
        CMsvSession& aMsvSession,
        const TMsvId aMailboxId,
        TMsvEntry& aMailbox );

    /**
     * Searches the mailbox entries from specified mailbox id
     *
     * @param aMailboxId Id of the required mailbox
     * @param aMsvSession Session to be used in search
     * @param aSmtpEntry Entry for Sending part of mailbox
     * @param aRelatedEntry Entry for Receiving part of mailbox
     * @return Imap4 or Pop3 mtm, based on the related mail protocol
     */
    static TUid GetMailboxEntriesL(
        const TMsvId aMailboxId,
        CMsvSession& aMsvSession,
        TMsvEntry& aSmtpEntry,
        TMsvEntry& aRelatedEntry );

    /**
     * Searches the mailbox entries from specified mailbox id
     *
     * @param aEntry Entry of the required mailbox
     * @param aMsvSession Session to be used in search
     * @param aSmtpEntry Entry for Sending part of mailbox
     * @param aRelatedEntry Entry for Receiving part of mailbox
     * @return Imap4 or Pop3 mtm, based on the related mail protocol
     */
    static TUid GetMailboxEntriesL(
        const TMsvEntry& aEntry,
        CMsvSession& aMsvSession,
        TMsvEntry& aSmtpEntry,
        TMsvEntry& aRelatedEntry );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aMsvSession Session to be used in search
     * @param aMailboxId Id of the mailbox to be used for search
     * @return Account id of the mailbox
     */
    static TSmtpAccount GetSmtpAccountIdL(
        CMsvSession& aMsvSession,
        const TMsvId aMailboxId );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aMsvSession Session to be used in search
     * @param aMailboxId Id of the mailbox to be used for search
     * @return Account id of the mailbox
     */
    static TImapAccount GetImapAccountIdL(
        CMsvSession& aMsvSession,
        const TMsvId aMailboxId );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aMsvSession Session to be used in search
     * @param aMailboxId Id of the mailbox to be used for search
     * @return Account id of the mailbox
     */
    static TPopAccount GetPopAccountIdL(
        CMsvSession& aMsvSession,
        const TMsvId aMailboxId );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aMsvSession Session to be used in search
     * @param aEntry Mailbox entry to be used for search
     * @return Account id of the mailbox
     */
    static TSmtpAccount GetSmtpAccountIdL(
        CMsvSession& aMsvSession,
        const TMsvEntry& aEntry );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aMsvSession Session to be used in search
     * @param aEntry Mailbox entry to be used for search
     * @return Account id of the mailbox
     */
    static TImapAccount GetImapAccountIdL(
        CMsvSession& aMsvSession,
        const TMsvEntry& aEntry );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aMsvSession Session to be used in search
     * @param aEntry Mailbox entry to be used for search
     * @return Account id of the mailbox
     */
    static TPopAccount GetPopAccountIdL(
        CMsvSession& aMsvSession,
        const TMsvEntry& aEntry );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aEntry aPopMailboxId Id of the Pop3 mailbox
     * @return Account id of the mailbox
     */
    static TPopAccount GetPopAccountIdL(
        const TMsvId aPopMailboxId );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aEntry aImapMailboxId Id of the Imap4 mailbox
     * @return Account id of the mailbox
     */
    static TImapAccount GetImapAccountIdL(
        const TMsvId aImapMailboxId );

    /**
     * Searches the correct account id to be used to load settings
     *
     * @param aEntry aSmtpMailboxId Id of the Smtp mailbox
     * @return Account id of the mailbox
     */
    static TSmtpAccount GetSmtpAccountIdL(
        const TMsvId aSmtpMailboxId );

    /**
     * Searches the correct entry of specified protocol for required mailbox
     *
     * @param aMsvSession Session to be used in search
     * @param aEntry Entry for result to be stored
     * @param aMailboxId Id of the required mailbox
     * @param aGetSendingService Get either receiving or sending type
     * @return TInt KErrNone if search is successful
     */
    static TInt GetMailboxServiceId(
        CMsvSession& aMsvSession,
        TMsvEntry& aEntry,
        const TMsvId aMailboxId,
        const TBool aGetSendingService );

    /**
     * Searches through the provided mailbox for subscribed folders
     *
     * @param aMsvSession Session to be used in search
     * @param aMailboxId Id of the mailbox to be searched
     * @return Number of subscribed folders
     */
    static TInt CountSubscribedFoldersL(
        CMsvSession& aMsvSession,
        const TMsvId aMailboxId );

    /**
     * Checks if the mailbox has any subscribed folders.
     * Slightly faster than CountSubscribedFoldersL
     *
     * @param aMsvSession Session to be used in search
     * @param aMailboxId Id of the mailbox to be searched
     * @return ETrue if mailbox has subscribed folders
     * @return EFalse if mailbox has NOT subscribed folders
     */
    static TBool HasSubscribedFoldersL(
        CMsvSession& aMsvSession,
        const TMsvId aMailboxId );

    /**
     * Checks if the offered folder entry is inbox
     *
     * @param aMsvSession Session to be used
     * @param aFolderEntry Folder entry
     * @return ETrue if the folder is inbox
     * @return EFalse if the folder is NOT inbox
     */
    static TBool IsInbox(
        CMsvSession& aMsvSession,
        const TMsvEntry& aFolderEntry );

    /**
     * Email address check based on RFC2822
     *
     * @param aAddress: email address to be checked.
     * @return Returns ETrue if aAddress is a lexically valid emailaddress.
     */
    IMPORT_C static TBool IsValidEmailAddressL( const TDesC& aAddress );

    /**
     * Server address check based on RFC2822
     *
     * @param aDomain: domain to be checked.
     * @return Returns ETrue if aDomain is a lexically valid domain name.
     */
    static TBool IsValidDomainL( const TDesC& aAddress );

    /**
     * Gets related mailbox ID
     *
     * @param aMsvSession Message server session.
     * @param aMailboxId Unique mailbox entry id with return parameter.
     * @return KErrNone, when ok.
     */
    static TInt GetRelatedMailboxId(
        CMsvSession& aMsvSession,
        TMsvId& aMailboxId );
    
    /**    
     * Retrieves security port.
     *
     * @param aSecurity Current security setting.
     * @param aIncoming Defines security type.
     * @param aIsImap4 For protocol type.
     * @return Correct port number.
     */
    static TUint32 GetDefaultSecurityPort(
        const TInt aSecurity,
        const TBool aIncoming,
        const TBool aIsImap4 );
    
    /**    
     * Retrieves msv id of service entry
     *
     * @param aAccountId id of TPopAccount or TImapAccount
     * @param aMtmId KSenduiMtmImap4Uid or KSenduiMtmPop3Uid
     * @param aMsvId reference to msv id where service id is stored
     * @param aMsvId reference where smtp msv id is stored 
     */
    static void GetServiceIdFromAccountIdL(
        const TUint32 aAccountId,
        const TUid aMtmId,
        TMsvId& aServiceId,
        TMsvId& aSmtpId );
    
    /**    
     * Creates smtp settings object
     *
     * @param aAccountId id of TPopAccount or TImapAccount
     * @param aMtmId KSenduiMtmImap4Uid or KSenduiMtmPop3Uid
     * @return pointer to CImSmtpSettings
     */
    static CImSmtpSettings* GetSmtpSettingsFromAccountIdL(
        const TUint32 aAccountId,
        const TUid aMtmId );
    
    
        
private:

// New functions

    /**
     * Calculates the number of subscribed folders
     *
     * @param aMsvSession Session to be used in search
     * @param aFolderId Id of the folder to be searched
     * @param aCount Variable to store the number of subscribed folders
     */
    static void DoCountSubscribedFoldersL(
        CMsvSession& aMsvSession,
        const TMsvId aFolderId,
        TInt& aCount );

    /**
     * Simplifies the MtmUi's query Mtm
     *
     * @param aMailboxId, Mailbox id to be checked
     * @param aMtmUi, MtmUi to be used for the query
     * @return The result of the query
     */
    static TInt QueryMailboxValidityL(
        const TMsvId aMailboxId,
        CBaseMtmUi& aMtmUi );
    };

#endif // IPSSETUTILS_H

// End of File
