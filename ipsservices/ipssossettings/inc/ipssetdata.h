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
* Description:  Decleares email data class.
*
*/

#ifndef CIPSSETDATA_H
#define CIPSSETDATA_H


#include <iapprefs.h>
#include <cemailaccounts.h>

class CIpsSetDataExtension;
class CIpsSetDataSignature;
class CMsvSession;
class CImBaseEmailSettings;

// These are related to synchronisation times
// NOTE: these are not defaults when mailbox is created,
// when mailbox is created all time are set to zero means "all day"
const TInt64 KIpsSetDataTimeDialogDefault         = MAKE_TINT64( 0x00000000, 0x00000000 );
const TInt64 KIpsSetDataTimeDialogDefaultStart    = MAKE_TINT64( 0x00000006, 0xB49D2000 ); // 08:00
const TInt64 KIpsSetDataTimeDialogDefaultStop     = MAKE_TINT64( 0x0000000F, 0x16618800 ); // 18:00

/**
 * Container for email settings
 *
 * @lib IpsSossetting.lib
 * @since FS v1.0
 */
class CIpsSetData : public CBase
    {
public:

// Datatypes

    /** Security types */
    enum TSecurityType
        {
        EStartTls = 0,
        ESslTls,
        ESecurityOff
        };

    /** */
    enum TRetrieveLimitButtons
        {
        EAll = 0,
        EUserDefined
        };

    /** */
    enum TRetrieveLimit
        {
        EPop3Limit = 0,
        EImap4Inbox,
        EImap4Folders
        };

    /** */
    enum TUserAuthentication
        {
        ENoAuth = 0,
        EUseIncoming,
        EOwnOutgoing
        };

    /***/
    enum THoursSetting
        {
        EAllDay = 0,
        ECustomHours
        };


    /** */
    enum TSyncInterval
        {
        E5Mins = 0,
        E10Mins,
        E15Mins,
        E30Mins,
        E1Hour,
        E4Hours,
        E12Hours,
        E1Day,
        ENotification,
        EManual
        };

    /** */
    enum TDownloadButtons
        {
        EHeaders = 0,
        EHeadersPlus,
        EWholeBody
        };

public:

// Constructors and destructor

    /**
     * Creates object from CIpsSetData and leaves it to cleanup stack
     * @return, Constructed object
     */
    static CIpsSetData* NewLC();

    /**
     * Create object from CIpsSetData
     * @return, Constructed object
     */
    static CIpsSetData* NewL();

    /**
     * Destructor
     */
    virtual ~CIpsSetData();

public: // New functions

    /**
     * @return Pointer to Imap4 settings for modification
     */
    CImImap4Settings* Imap4Settings();

    /**
     * @return Pointer to Pop3 settings for modification
     */
    CImPop3Settings* Pop3Settings();

    /**
     * @return Pointer to Smtp settings for modification
     */
    CImSmtpSettings* SmtpSettings();

    /**
     * @return Pointer to Extended settings for modification
     */
    CIpsSetDataExtension* ExtendedSettings();

    /**
     * @return Pointer to incoming Access Point settings for modification
     */
    CImIAPPreferences* IncomingIapPref() const;

    /**
     * @return Pointer to outgoing Access Point settings for modification
     */
    CImIAPPreferences* OutgoingIapPref() const;

    /**
     * Loads the settings from message store and central repository.
     *
     * @param aMailboxId Id of the mailbox
     * @param aSession Message Server session
     */
    void LoadL( const TMsvId aMailboxId, CMsvSession& aSession );

    /**
     * @param aSession Message Server session
     */
    void SaveL( CMsvSession& aSession );

    /**
     * @return ETrue, when settings are valid
     */
    TBool IsOk();

    /**
     * @return Protocol of the settings
     */
    const TUid& Protocol() const;

    /**
     * @return Reference to Imap4 account ID
     */
    TImapAccount& ImapAccount();

    /**
     * @return Constant reference to Imap4 account ID
     */
    const TImapAccount& ImapAccount() const;

    /**
     * @return Reference to Pop3 account ID
     */
    TPopAccount& PopAccount();

    /**
     * @return Constant reference to Pop3 account ID
     */
    const TPopAccount& PopAccount() const;

    /**
     * @return Reference to Smtp account ID
     */
    TSmtpAccount& SmtpAccount();

    /**
     * @return Constant reference to Smtp account ID
     */
    const TSmtpAccount& SmtpAccount() const;

    /**
     * @return Read-only reference to email address setting.
     */
    const TDesC& EmailAddress() const;

    /**
     * @return Read-only reference to reply-to address setting.
     */
    const TDesC& ReplyToAddress() const;

    /**
     * @return Read-only reference to mailbox name setting.
     */
    const TDesC& MailboxName() const;

    /**
     * @return Read-only reference to own name setting.
     */
    const TDesC& MyName() const;

    /**
     * @return Include signature setting.
     */
    TBool IncludeSignature() const;

    /**
     * @return Write access to signature setting.
     */
    CIpsSetDataSignature& Signature();

    /**
     * @return Read-only reference to signature setting.
     */
    const CIpsSetDataSignature& Signature() const;

    /**
     * @param aIncoming Retireve incoming or outgoing setting.
     * @return Read-only reference to username setting.
     */
    const TDesC& UserName( const TBool aIncoming ) const;

    /**
     * @param aIncoming Retireve incoming or outgoing setting.
     * @return Read-only reference to password setting.
     */
    const TDesC& UserPwd( const TBool aIncoming ) const;

    /**
     * @param aIncoming Retireve incoming or outgoing setting.
     * @return Read-only reference to server address setting.
     */
    const TDesC& MailServer( const TBool aIncoming ) const;

    /**
     * @param aIncoming Retireve incoming or outgoing setting.
     * @return Access point setting.
     */
    TImIAPChoice Iap( const TBool aIncoming ) const;

    /**
     * @param aIncoming Retireve incoming or outgoing setting.
     * @return Current security setting.
     */
    TInt Security( const TBool aIncoming ) const;

    /**
     * @param aIncoming Retireve incoming or outgoing setting.
     * @return Value from port setting.
     */
    TInt Port( const TBool aIncoming ) const;

    /**
     * @return Returns APOP setting value.
     */
    TInt APop() const;

    /**
     * @return User authentication setting
     */
    TInt UserAuthentication() const;

    /**
     * @return Read-only reference to Imap folder path
     */
    const TDesC& ImapPath() const;

    /**
     * @return Email download size
     */
    TInt DownloadSize() const;

    /**
     * @return Remove older -setting status
     */
    TInt RetrieveLimit( const TRetrieveLimit aType ) const;

    /**
     * @return Scheduling interval setting
     */
    TInt Schedule() const;

    /**
     * @return Checked days as flag mask
     */
    TInt Days() const;

    /**
     * Retrieves peak start time or stop time
     *
     * @param aStartTime When ETrue, start time is retrieved
     * @return Returns time based on the parameter
     */
    TTime Hours( const TBool aStartTime ) const;

    /**
     * @param aProtocol Protocol MTM ID for settings
     */
    void SetProtocol( const TUid& aProtocol );

    /**
     * @param aEmailAddress Address to be stored.
     */
    void SetEmailAddressL( const TDesC& aEmailAddress );

    /**
     * @param aReplyToAddress Address to be stored.
     */
    void SetReplyToAddressL( const TDesC& aReplyToAddress );

    /**
     * @param aMailboxName Mailboxes name to be stored.
     */
    void SetMailboxName( const TDesC& aMailboxName );

    /**
     * @param aMyName User alias to be stored.
     */
    void SetMyNameL( const TDesC& aMyName );

    /**
     * @param aIncludeSignature Signature status to be stored.
     */
    void SetIncludeSignature( TBool aIncludeSignature );

    /**
     * @param aSignature Signature text to be stored.
     */
    void SetSignatureL( const TDesC& aSignature );

    /**
     * @param aIncomingUserName Incoming name to be stored.
     * @param aOutgoingUserName Outgoing username to be store.
     */
    void SetUserNameL(
        const TDesC& aIncomingUserName,
        const TDesC& aOutgoingUserName );

    /**
     * @param aIncomingUserPwd Incoming password to be stored.
     * @param aOutgoingUserPwd Outgoing password to be stored.
     */
    void SetUserPwdL(
        const TDesC& aIncomingUserPwd,
        const TDesC& aOutgoingUserPwd );

    /**
     * @param aIncomingMailServer Incoming mail server to be stored.
     * @param aOutgoingMailServer Outgoing mail server to be stored.
     */
    void SetMailServerL(
        const TDesC& aIncomingMailServer,
        const TDesC& aOutgoingMailServer );

    /**
     * @param aIncomingIap Incoming access point to be stored.
     * @param aOutgoingIap Outgoing access point to be stored.
     */
    void SetIapL(
        TImIAPChoice aIncomingIap,
        TImIAPChoice aOutgoingIap );
    /**
     * 
     * @param aIncomingIap Incoming access point to be stored.
     * @param aOutgoingIap Outgoing access point to be stored.
     */

    void SetIapL( 
        const CImIAPPreferences& aIncomingIap, 
        const CImIAPPreferences& aOutgoingIap );

    
    /**
     * @param aIncomingSecurity Incoming security to be stored.
     * @param aOutgoingSecurity Outgoing security to be stored.
     */
    void SetSecurity(
        TInt aIncomingSecurity,
        TInt aOutgoingSecurity );

    /**
     * @param aIncomingPort Incoming Port to be stored.
     * @param aOutgoingPort Outgoing Port to be stored.
     */
    void SetPort(
        TInt aIncomingPort,
        TInt aOutgoingPort );

    /**
     * @param aAPop APOP status to be stored.
     */
    void SetAPop( TInt aAPop );

    /**
     * @param aUserAuthentication Outgoing authentication status to be stored.
     */
    void SetUserAuthentication( TInt aUserAuthentication );

    /**
     * @param aImapPath Imap4 inbox path to be stored.
     */
    void SetImapPathL( const TDesC& aImapPath );

    /**
     * @param aDownloadControl Download control state for saving.
     * @param aDownloadSize Download size for saving.
     */
    void SetDownloadSizeL(
        const TInt aDownloadControl,
        const TInt aDownloadSize );

    /**
     * @param aRetrieveLimitPop Pop retrieval limit for storing.
     * @param aRetrieveLimitImapInbox Imap inbox retrieval limit for storing.
     * @param aRetrieveLimitImapFolders Imap folder retrieval limit for storing.
     */
    void SetRetrieveLimit(
        TInt aRetrieveLimitPop,
        TInt aRetrieveLimitImapInbox,
        TInt aRetrieveLimitImapFolders );

    /**
     * @param aSchedule Scheduling status to be stored.
     * @param aRoaming Roaming status for saving.
     */
    void SetSchedule(
        const TInt aSchedule,
        const TBool aRoaming );

    /**
     * @param aDays Retrieval days setting to be stored.
     */
    void SetDays( TInt aDays );

    /**
     * @param aStartHour Sync start time to be stored.
     * @param aStopHour Sync stop time to be stored.
     */
    void SetHours(
        const TInt64 aStartHour,
        const TInt64 aStopHour );

    /**
     * @param aHoursCustom to be stored.
     */
    void SetHoursCustom( TInt64 aHoursCustom );
    
    //<cmail>
    /**
     * @param aHideData ETrue if there are some field to hide.
     */
    void SetDataHide( TBool aHideData );
    //</cmail>

protected:

// Constructors

    /**
     * Default constructor for classCIpsSetData
     * @return, Constructed object
     */
    CIpsSetData();

    /**
     * Symbian 2-phase constructor
     */
    void ConstructL();

private:

// New functions

    /**
     * Sets the security settings for the protocols.
     *
     * @param aSecurity Current security setting.
     * @param aSettings Settings object for storing the security setting.
     */
    void DoSetSecurity(
        const TInt aSecurity,
        CImBaseEmailSettings& aSettings );

    /**
     * Sets default data values to all settings
     *
     */
    void SetDefaultDataL();

private:    // Data

    /**
     * Settings data protocol.
     */
    TUid                        iProtocol;

    /**
     * Name of the mailbox.
     */
    RBuf                        iName;

    /**
     * Signature object.
     * Owned.
     */
    CIpsSetDataSignature*       iSignature;

    /**
     * Default account Id.
     */
    TMsvId                      iDefaultAccountId;

    /**
     * Imap4 settings data object.
     * Owned.
     */
    CImImap4Settings*           iImap4Settings;

    /**
     * Imap4 account Id.
     */
    TImapAccount                iImap4AccountId;

    /**
     * Pop3 settings data object.
     * Owned.
     */
    CImPop3Settings*            iPop3Settings;

    /**
     * Pop3 account id.
     */
    TPopAccount                 iPop3AccountId;

    /**
     * Smtp account id.
     */
    TSmtpAccount                iSmtpAccountId;

    /**
     * Smtp settings data object.
     * Owned.
     */
    CImSmtpSettings*            iSmtpSettings;

    /**
     * Extended settings data object.
     * Owned.
     */
    CIpsSetDataExtension*       iExtendedSettings;

    /**
     * Outgoing access point preferences.
     * Owned.
     */
    CImIAPPreferences*          iIncomingIapPref;

    /**
     * Outgoing access point preferences.
     * Owned.
     */
    CImIAPPreferences*          iOutgoingIapPref;

    /**
     * Temporary text store.
     */
    RBuf                        iTempStore;
    };

#endif // CIPSSETDATA_H

// End of File
