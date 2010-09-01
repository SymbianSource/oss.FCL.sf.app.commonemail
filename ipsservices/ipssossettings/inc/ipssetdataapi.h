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
* Description:  Class which decleares the Settings API.
*
*/


#ifndef IPSSETDATAAPI_H
#define IPSSETDATAAPI_H


#include <msvstd.h>
#include <imapset.h>

const TInt KIpsSetDataMaxPhoneIdLength      = 50;

class CMsvSession;
class CMsvEntry;
class CIpsSetDataExtension;
class TIpsSetDataStorerParams;
class CEmailAccounts;

/**
 * Settings API for accessing mailbox settings.
 *
 * @lib IpsSosSettings.lib
 * @since FS v1.0
 */
class CIpsSetDataApi : public CBase
    {
public:

    /**
     * 2-phase constructor.
     *
     * @param aSession Messaging framework session
     * @return Api object with client ownership.
     */
    IMPORT_C static CIpsSetDataApi* NewL( CMsvSession& aSession );

    /**
     * Destructor.
     */
    virtual ~CIpsSetDataApi();

    /**
     * Sets new password.
     *
     * @param aService Mailbox service Id.
     * @param aPassword New password.
    */
    IMPORT_C void SetNewPasswordL(
        CMsvEntry& aService,
        const TDesC& aPassword );
    
    //<cmail>
    //This method is not used anywhere
    /**
    *
    */
    /*IMPORT_C TBool IsSmtpAllowedToConnectL( 
        TMsvId aSmtpService, 
        TBool aIsRoaming,
        CMsvSession& aSession );*/
    //</cmail>
    
    /**
    *
    */
    IMPORT_C TDesC& GetServerAddressL( const CMsvEntry& aEntry );
    
    /**
    *
    */
    IMPORT_C void GoOnlineL( TMsvEntry aMailbox );
    
    /**
    * Creates mailbox
    *
    * @param aUid uid of the caller plugin
    * @param aSession msv session reference
    * @return KErrNone if mailbox created successfully. 
    *         KErrNotSupported if uid not match for the wizard data cenrep. 
    *         KErrGeneral in any type of other errors.
    */
    IMPORT_C TInt HandleMailboxCreation( 
        const TUid aUid, 
        CMsvSession& aSession );
    
    
    /**
    *
    */
    IMPORT_C void GoOfflineL( TMsvEntry aMailbox );
    
    /**
    *
    */
    IMPORT_C void RemoveAccountL( 
        const TMsvEntry& aMailbox, 
        CMsvSession& aSession );
    
    /**
    *
    */
    IMPORT_C static TInt LaunchSettingsL( const TMsvId aMailboxId );

    /**
    * Creates cenrep key from given parameters
    *
    * @param aAccountId id of used account
    * @param aProtocol account's protocol
    * @param aSettingKey specific setting item to create this key for
    * @return key generated from the parameters
    */
    IMPORT_C TUint32 CreateCenRepKeyL( 
        const TUint32 aAccountId,
        const TUid& aProtocol,
        const TUint32 aSettingKey );
    
    /**
     * Reads the address of the mailbox (i.e. the 'own' address).
     * @param aMailboxEntry The service entry of the mailbox
     * @param aMailboxAddress The address string buffer. The ownership
     *     is moved to the client.
     */
    IMPORT_C void GetMailboxAddressL(
        const TMsvEntry& aMailboxEntry,
        HBufC*& aMailboxAddress);
    
    
    /**
    *
    */    
    IMPORT_C void SaveSyncStatusL( TMsvEntry aMailbox, TInt aState );
    
    IMPORT_C TInt GetLastSyncStatusL( TMsvEntry aMailboxId );
    
    IMPORT_C void GetIMEIFromThePhoneL( TBuf<KIpsSetDataMaxPhoneIdLength> &aIMEI );
    

    IMPORT_C void LoadExtendedSettingsL( 
            TMsvId aServiceId,
            CIpsSetDataExtension& aExtendedSettings );
    
    IMPORT_C void SaveExtendedSettingsL(
            const CIpsSetDataExtension& aExtendedSettings );
    
    IMPORT_C void GetSubscribedImapFoldersL( 
            TMsvId aServiceId, RArray<TMsvId>& aFoldersArray );
    
    IMPORT_C static void ConstructImapPartialFetchInfo( 
            TImImap4GetPartialMailInfo& aInfo, CImImap4Settings& aImap4Settings );
    
    IMPORT_C void SetMailboxNameL( 
            TMsvEntry& aService, 
            const TDesC& aMailboxName );
    
    /**
     * Returns pointer to descriptor containing signature text if signature
     * has been set to be added to outgoing mails. Otherwise returns NULL
     * pointer.
     * Ownership of the descriptor is passed to the caller.
     * @param aService Service entry for which signature text setting is 
     * *               resolved.
     * @return Pointer to descriptor containing the signature text.
     */
    IMPORT_C HBufC* SignatureTextL( const TMsvEntry& aService );
    
private:

    /**
     * 2nd phase of construction.
     */
    void ConstructL();

    /**
     * Constructor
     */
    CIpsSetDataApi( CMsvSession& aSession );

    /**
     * Sets Imap4 password.
     *
     * @param aService Mailbox service Id.
     * @param aPassword New password.
     */
    void SetNewImap4PasswordL(
        CMsvEntry& aService,
        const TDesC& aPassword );

    /**
     * Sets Pop3 password.
     *
     * @param aService Mailbox service Id.
     * @param aPassword New password.
     */
    void SetNewPop3PasswordL(
        CMsvEntry& aService,
        const TDesC& aPassword );
    
    static void GetImapChildFoldersL(
            CMsvSession& aSession,
            const CMsvEntrySelection& aRootFolders, 
            RArray<TMsvId>& aFillArray );

    /**
     * Checks whether outgoing settings is set as "same as incoming"
     * @param aParams contains account id and mtm id
     */
    TBool IsOutgoingLoginSameAsIncomingL( const TIpsSetDataStorerParams& aParams );

    /**
     * Sets outgoing (smtp) password settings
     * @param aSmtpServiceId specifies the service entry in messagestore
     *        (i.e. mailbox in question), where the settings are saved
     * @param aPassword new password
     * @param aEmailAccounts reference to helper class instance that is needed
     */
    void SetNewSmtpPasswordL(
        const TMsvId aSmtpServiceId,
        const TDesC8& aPassword,
        CEmailAccounts& aEmailAccounts );

private:
    
    CMsvSession& iSession;
    
    HBufC* iServerAddress;
    };

#endif /* IPSSETDATAAPI_H*/

// End of File
