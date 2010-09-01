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
* Description:  Encapsulates saving and removing email accounts
*
*/

#ifndef IPSSETDATAMANAGER_H
#define IPSSETDATAMANAGER_H


typedef RArray<TImapAccount> RIpsSetDataImap4IdArray;
typedef RArray<TPopAccount> RIpsSetDataPop3IdArray;
typedef RArray<TSmtpAccount> RIpsSetDataSmtpIdArray;

class CIpsSetData;
class CIpsSetDataStorer;
class CEmailAccounts;
class CMsvSession;
class CIMAMailboxDoctor;

// Maximum length of IMEI code
const TInt KIMAMaxPhoneIdLength = 50;

/**
 * This class encapsulates saving and removing email accounts.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class CIpsSetDataManager : public CBase
    {
public:

    /**
     * 2 phase construction.
     *
     * @param aSession Message server session.
     * @return self pointer
     */
    static CIpsSetDataManager* NewL(
        CMsvSession& aSession );

    /**
     * 2 phase construction with stack push.
     *
     * @param CMsvSession Message server session.
     * @return self pointer
     */
    static CIpsSetDataManager* NewLC(
        CMsvSession& aSession );

    /**
     * Destructor
     */
    virtual ~CIpsSetDataManager();

public:

    /**
     * Creates email service and account.
     *
     * @param aSettings Settings data object.
     * @param aPopulateDefaults Sets default data.
     */
    TInt CreateEmailAccount(
        CIpsSetData& aSettings,
        const TBool aPopulateDefaults = EFalse );

    /**
     * Saves email settings
     *
     * @param aSettings Settings data object.
     * @return KErrNone, when savng is ok.
     */
    TInt SaveEmailSettingsL(
        CIpsSetData& aSettings );

    /**
     * Loads email settings.
     *
     * @param aEntry Mailbox entry.
     * @param aSettings Settings data object.
     */
    void LoadEmailSettingsL(
        const TMsvEntry& aEntry,
        CIpsSetData& aSettings );

    /**
     * Removes whole mailbox account.
     *
     * @param aRelatedAccountId Receiving account ID.
     * @param aMtmId Service protocol.
     */
    void RemoveAccountL(
        const TMsvEntry& aMailbox );

    /**
     * Removes settings from the account.
     *
     * @param aAccountId Account ID.
     * @param aMtmId Service protocol.
     */
    /*TInt DeleteAccountSettings(
        const TUint32 aAccountId,
        const TUid& aMtmId );*/

private:

    /**
     * C++ constructor
     *
     * @param aSession Message server session.
     */
    CIpsSetDataManager( CMsvSession& aSession );

    /**
     * 2nd phase of construction.
     */
    void ConstructL();

private:

    /**
     * Creates mailbox service and account.
     *
     * @param aSettings Settings data object.
     * @param aPopulateDefaults Populates default settings.
     */
    void DoCreateEmailAccountL(
        CIpsSetData& aSettings,
        const TBool aPopulateDefaults );


    /**
     * Stores signature to stream.
     *
     * @param aSettings Settings data object.
     */
    void StoreSignatureL( const CIpsSetData& aSettings );

    /**
     * Retrieves signature from stream.
     *
     * @param aSettings Settings data object.
     */
    void RestoreSignatureL( CIpsSetData& aSettings );

    /**
     * Create Imap4 service and account.
     *
     * @param aSettings Settings data object.
     * @param aPopulateDefaults Populates default settings.
     * @param aReadOnly The service is created as read only.
     */
    TUint32 CreateImap4AccountL(
        CIpsSetData& aSettings,
        const TBool aPopulateDefaults,
        const TBool aReadOnly = EFalse );

    /**
     * Create Pop3 service and account.
     *
     * @param aSettings Settings data object.
     * @param aPopulateDefaults Populates default settings.
     * @param aReadOnly The service is created as read only.
     */
    TUint32 CreatePop3AccountL(
        CIpsSetData& aSettings,
        const TBool aPopulateDefaults,
        const TBool aReadOnly = EFalse );

    /**
     * Create extended settings.
     *
     * @param aMailboxId Mailbox's service Id.
     * @param aAccountId Account Id.
     * @param aProtocol Service protocol.
     * @param aSettings Settings data object.
     * @param aPopulateDefaults Populates default settings.
     */
    void CreateExtendedSettingsL(
        const TMsvId aMailboxId,
        const TUint32 aAccountId,
        const TUid& aProtocol,
        CIpsSetData& aSettings,
        const TBool aPopulateDefaults = EFalse );

    /**
     * imei code saving function
     *
     */
    TInt StoreIMEIToMailbox(
        const TMsvId aMailboxId ) const;

    /**
     * imei code saving function
     *
     */
    TInt StoreIMEIToEntry( const TMsvId aMailboxId ) const;

    /**
     * imei code saving function
     *
     */        
    void GetAndStoreIMEIToEntryL( const TMsvId aMailboxId ) const;

    /**
     * Loads Pop3 settings.
     *
     * @param aSettings Settings data object.
     */
    void LoadPop3SettingsL( CIpsSetData& aSettings );

    /**
     * Loads Imap4 settings.
     *
     * @param aSettings Settings data object.
     */
    void LoadImap4SettingsL( CIpsSetData& aSettings );

    /**
     * Saves Pop3 settings.
     *
     * @param aSettings Settings data object.
     */
    void SavePop3SettingsL( CIpsSetData& aSettings );

    /**
     * Saves Imap4 settings.
     *
     * @param aSettings Settings data object.
     */
    void SaveImap4SettingsL( CIpsSetData& aSettings );

    /**
     * Used to send event to email plugin
     * that settings have been changed
     */
    TInt SendPropertyEventL( TInt aMailbox, TBool aIsImap );
    
    /**
     * Cleans all global folders (drafts, outbox, sent) out of entries that
     * belong to the given mailbox
     * @param aMailbox mailbox that is being deleted
     */
    void CleanGlobalFoldersL( const TMsvEntry& aMailbox );
    
    /**
     * Cleans one folder (drafts, outbox, sent) out of entries that
     * belong to the given mailbox
     * @param aFolderId standard/global folder id
     * @param aMailbox mailbox that is being deleted
     */
    void CleanGlobalFolderL( const TMsvId& aFolderId, const TMsvEntry& aMailbox );

    TBool VerifyMailboxCreation( const CIpsSetData&  aSettings );
    
    void CleanInvalidMailbox( const CIpsSetData&  aSettings );
    
    void SetRelatedIdL( TMsvId aEntryId, TMsvId aRelatedId );

private:

    /**
     * Message Server Session
     */
    CMsvSession&                iSession;

    /**
     * An object to handle the storing of the settings
     * Owned.
     */
    CIpsSetDataStorer*         iStorer;

    /**
     * An object to handle account creation
     * Owned.
     */
    CEmailAccounts*             iAccounts;
    };

#endif // IPSSETDATAMANAGER_H

// End of File
