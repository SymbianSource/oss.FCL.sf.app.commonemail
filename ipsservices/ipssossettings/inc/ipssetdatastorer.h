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
* Description:  Defines an API to create and edit mailbox settings
*
*/



#ifndef IPSSETDATASTORER_H
#define IPSSETDATASTORER_H

#include <msvstd.h>             // TMsvId

class CRepository;
class CIpsSetDataExtension;
class CIpsSetDataCtrlCenrep;
class CEmailAccounts;
class CImImap4Settings;
class CImPop3Settings;
class CImSmtpSettings;

/**
 * Storer parameters.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class TIpsSetDataStorerParams
    {
public: 
    TIpsSetDataStorerParams(
        const TUint32& aAccountId,
        const TUid& aMtmId );
        
    TUint32 AccountId() const;
    TUid MtmId() const;

private:
    TUint32 iAccountId;
    TUid    iMtmId;
    };

/**
 * Class which stores the data to Central Repository.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class CIpsSetDataStorer : public CBase
    {
public:

    /**
     * Destructor
     */
    virtual ~CIpsSetDataStorer();

    /**
     * Creates the settings storer object
     */
    static CIpsSetDataStorer* NewL();

    /**
     * Creates the settings storer object
     */
    static CIpsSetDataStorer* NewLC();

public: 

    /**
     * Creates settings to store.
     *
     * @param aMailboxId Receiving mailbox ID.
     * @param aAccountId Id of the account to be used.
     * @param aProtocol Protocol of the settings.
     * @param aExtendedMailSettings Extended email settings object.
     */
    TInt CreateExtendedSettings(
        const TMsvId aMailboxId,
        const TUint32 aAccountId,
        const TUid& aProtocol,
        CIpsSetDataExtension& aExtendedMailSettings,
        const TBool aPopulateDefaults = ETrue );

    /**
     * Deletes extended settings.
     */
    TInt DeleteExtendedSettingsL(
        const TMsvEntry& aMailbox );

    /**
     * Deletes extended settings.
     *
     * @param aAccountId Id of the account to be used
     * @param aMtmId Current receiving MTM ID.
     * @return KErrNone if deleted successfully, else system wide error code.
     */
    TInt DeleteExtendedSettingsL(
        const TUint32 aAccountId,
        const TUid& aMtmId );

    /**
     * Saves the Extended settings to the Central Repository
     *
     * @param aExtendedMailSettings Extended email settings object.
     * @return KErrNone, if saving has been successful
     */
    TInt SaveExtendedSettings(
        const CIpsSetDataExtension& aExtendedMailSettings );

    /**
     * Saves the protocol related settings to Central Repository.
     *
     * @param aAccountId Id of the account to be used
     * @param aImap4Settings Imap4 setting object.
     */
    TInt SaveImap4Settings(
        const TUint32 aAccountId,
        const CImImap4Settings& aImap4Settings );

    /**
     * Saves the protocol related settings to Central Repository.
     *
     * @param aAccountId Id of the account to be used
     * @param aPop3Settings Pop3 setting object.
     */
    TInt SavePop3Settings(
        const TUint32 aAccountId,
        const CImPop3Settings& aPop3Settings );

    /**
     * Saves the protocol related settings to Central Repository.
     *
     * @param aAccountId Id of the account to be used
     * @param aSmtpSettings Smtp setting object.
     */
    TInt SaveSmtpSettings(
        const TUint32 aAccountId,
        const CImSmtpSettings& aSmtpSettings );

    /**
     * Loads the Extended settings from the Central Repository
     *
     * @param aParams Id of the account to be used
     * @param aExtendedSettings Settings object
     * @return KErrNone, if loading has been successful
     */
    TInt LoadExtendedSettings(
        const TIpsSetDataStorerParams& aParams,
        CIpsSetDataExtension& aExtendedMailSettings );

    /**
     * Loads protocol spesific settings from Central Repository.
     *
     * @param aAccountId Id of the account to be used
     * @param aImap4Settings Imap4 setting object.
     */
    TInt LoadImap4Settings(
        const TUint32 aAccountId,
        CImImap4Settings& aImap4Settings );

    /**
     * Loads protocol spesific settings from Central Repository.
     *
     * @param aAccountId Id of the account to be used
     * @param aPop3Settings Pop3 setting object.
     */
    TInt LoadPop3Settings(
        const TUint32 aAccountId,
        CImPop3Settings& aPop3Settings );

    /**
     * Loads protocol spesific settings from Central Repository.
     *
     * @param aAccountId Id of the account to be used
     * @param aSmtpSettings Smtp setting object.
     */
    TInt LoadSmtpSettings(
        const TUint32 aAccountId,
        CImSmtpSettings& aSmtpSettings );
    
    /**
     * Saves the Extended settings to the Central Repository
     *
     * @param aExtendedSettings, Settings object
     * @return KErrNone, if saving has been successful
     */
    void SaveExtendedSettingsL(
        const CIpsSetDataExtension& aExtendedMailSettings );

    /**
     * Loads the Extended settings from the Central Repository
     *
     * @param aAccountId Id of the account to be used
     * @param aMtmId Current receiving MTM ID.
     * @param aExtendedSettings Settings object
     */
    void LoadExtendedSettingsL(
        const TUint32 aAccountId,
        const TUid& aMtmId,
        CIpsSetDataExtension& aExtendedMailSettings );
        
    
    /**
     * @return Symbian settings API.
     */
    CEmailAccounts* AccountsL();

private: 

    /**
     * Default constructor
     */
    CIpsSetDataStorer();

    /**
     * ConstructL
     */
    void ConstructL();

private: 

    /**
     * Create extended settings to Central Repository.
     *
     * @param aExtendedMailSettings Extended email settings object.
     */
    void CreateExtendedSettingsL(
        const CIpsSetDataExtension& aExtendedMailSettings );

    /**
     * 
     *
     */
    TInt SolveOutgoingLoginMethodL(
        const TUint32 aAccountId,
        const TUid& aMtmId );


public:

    /**
     * Extended settings key collection.
     */
    enum TIpsSetDataCommonSettings
        {
        EIpsSetDataCommonSetStatusFlagsH = 0x00,
        EIpsSetDataCommonSetStatusFlagsL,
        EIpsSetDataCommonSetMailboxId,
        EIpsSetDataCommonSetAccountId,
        EIpsSetDataCommonSetExtensionId,
        EIpsSetDataCommonSetProtocol,
        EIpsSetDataCommonSetEmailAddress,
        EIpsSetDataExtSetEmailNotif,
        EIpsSetDataExtSetIndicators,
        EIpsSetDataExtSetHideMsgs,
        EIpsSetDataExtSetOpenHtmlMail,
        EIpsSetDataAoVersion,
        EIpsSetDataAoOnlineState,
        EIpsSetDataAoWeekDays,
        EIpsSetDataAoTimeStartH,
        EIpsSetDataAoTimeStartL,
        EIpsSetDataAoTimeStopH,
        EIpsSetDataAoTimeStopL,
        EIpsSetDataAoInboxRefreshTime,
        EIpsSetDataAoUpdateMode,
        EIpsSetDataAoLastSuccessfulUpdateH,
        EIpsSetDataAoLastSuccessfulUpdateL,
        EIpsSetDataAoLastUpdateFailed,
        EIpsSetDataAoUpdateSuccessfulWithCurSettings,
        /* Move the own settings somewhere very far away in the key space */
        EIpsSetDataFsOutgoingLogin      = 0xFA ,
        EIpsSetDataLastModifiedH,
        EIpsSetDataLastModifiedL,
        EIpsSetDataLastSyncStatus,
        //<cmail>        
        EIpsSetDataHideUserNameAndAddress,
        //</cmail>
        EIpsSetDateEmnReceivedButNotSyncedFlag,
		EIpsSetDataFirstEMNReceivedFlag,
        EIpsSetDataKeyLast
        };

private:
    /**
     * Repository for smtp settings
     * Owned.
     */
    CRepository*    iCenRepSmtp;

    /**
     * Repository for extended mail settings
     * Owned.
     */
    CRepository*    iCenRepExtMail;

    /**
     * Repository for always online settings
     * Owned.
     */
    CRepository*    iCenRepAlwaysOnline;

    /**
     * Repository for default settings data
     * Owned.
     */
    CRepository*    iCenRepDefaultData;

    /**
     * Pointer to accounts handler
     * Owned.
     */
    CEmailAccounts* iAccounts;

    /**
     * Class to handle Central Repository operations
     * Owned.
     */
    CIpsSetDataCtrlCenrep* iCenRepControl;

    };

#endif      // IPSSETDATASTORER_H

// End of File
