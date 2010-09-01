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



#ifndef IPSSETDATAEXTENSION_H
#define IPSSETDATAEXTENSION_H

#include <msvstd.h>

#include "ipssetutilsconsts.h"

/**
 * Settings extension to current mailbox settings
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class CIpsSetDataExtension : public CBase
    {
public: // Constructors and destructor

    /**
     * Creates mailbox extension object
     */
    IMPORT_C static CIpsSetDataExtension* NewLC();

    /**
     * Creates mailbox extension object
     */
    IMPORT_C static CIpsSetDataExtension* NewL();

    /**
     * Destructor
     */
    virtual ~CIpsSetDataExtension();

public: // New functions

    /**
     * Verifies the validy of the metadata in the settings
     * @return ETrue if the settings are valid
     * @return EFalse if the settings are invalid
     */
    IMPORT_C TBool IsSettingsValid() const;

    // COMMON

    /**
     * Retrieves the status flags of the settings
     * @return Status flags
     */
    IMPORT_C void SetStatusFlags( const TUint64 aStatusFlags );

    /**
     * Retrieves the status flags of the settings
     * @return Status flags
     */
    IMPORT_C TUint64 StatusFlags() const;

    /**
     * Makes identical copy of settings
     * @param aExtension, Setting to be copied
     */
    IMPORT_C void CopyL(
        const CIpsSetDataExtension& aExtension );

    /**
     * Sets the id of the mailbox the setting is associated with
     * This is not exported
     * @param aMailboxId, Mailbox, who owns the settings
     */
    IMPORT_C void SetMailboxId(
        const TMsvId aMailboxId );

    /**
     * Returns the id of the owner of the setting
     * @return Mailbox id
     */
    IMPORT_C TMsvId MailboxId() const;

    /**
     * Set the account id of the base settings
     * This is not exported
     * @param aAccountId, Sets value of iAccountId
     */
    IMPORT_C void SetAccountId( const TUint32 aAccountId );

    /**
     * Get the account id of the base settings
     * @return value of iAccountId
     */
    IMPORT_C TUint32 AccountId() const;

    /**
     * Set the account id of the extended settings
     * This is not exported
     * @param aExtensionId, Sets value of iExtensionId
     */
    IMPORT_C void SetExtensionId( const TUint32 aExtensionId );

    /**
     * Get the account id of the extended settings
     * @return value of iExtensionId
     */
    IMPORT_C TUint32 ExtensionId() const;

    /**
     * Set method for member iProtocol
     * This is not exported
     * @param aProtocol, Sets value of iProtocol
     */
    IMPORT_C void SetProtocol( const TUid& aProtocol );

    /**
     * Get function for member iProtocol
     * @return value of iProtocol
     */
    IMPORT_C TUid Protocol() const;

    /**
     * Set method for member iEmailAddress
     * @param aEmailAddress, Sets value of iEmailAddress
     */
    IMPORT_C void SetEmailAddress( const TDesC& aEmailAddress );

    /**
     * Get function for member iEmailAddress
     * @return value of iEmailAddress
     */
    IMPORT_C const TDesC& EmailAddress() const;

    // EXTENDED MAIL SETTINGS

    /**
     * Sets the state of email notification -flag
     */
    IMPORT_C void SetEmailNotificationState(
        const TIpsSetDataEmnStates aEmnState );

    /**
     * Fetches the state of email notification -flag
     * @return ETrue or EFalse, according to flag state
     */
    IMPORT_C TIpsSetDataEmnStates EmailNotificationState() const;

    /**
     * A flag to
     * @param aNewMailIndicators, Sets value of iNewMailIndicators
     */
    IMPORT_C void SetNewMailIndicators(
        const TBool aNewState );

    /**
     *
     * @return value of EMailNewIndicators
     */
    IMPORT_C TBool NewMailIndicators() const;

    /**
     * Sets the state of hide messages -flag
     * @param aNewState, The new state of the flag
     */
    IMPORT_C void SetHideMsgs(
        const TBool aNewState );

    /**
     * Fetches the state of hide messages -flag
     * @return ETrue or EFalse, according to flag state
     */
    IMPORT_C TBool HideMsgs() const;

    /**
     * Sets the value of open html mail -setting
     * @param aOpenHtmlMail, New value of the setting
     */
    IMPORT_C void SetOpenHtmlMail(
        const TInt aOpenHtmlMail );

    /**
     * Fetches the state of open html mail -setting
     * @return Setting value
     */
    IMPORT_C TInt OpenHtmlMail() const;

    // ALWAYS ONLINE

    /**
     * Get function for member iVersion
     * @return value of iVersion
     */
    IMPORT_C TInt Version() const;

    /**
     * Set method for member iAlwaysOnlineState
     * @param aAlwaysOnlineState, Sets value of iAlwaysOnlineState
     */
    IMPORT_C void SetAlwaysOnlineState(
        const TIpsSetDataAoStates aAlwaysOnlineState,
        TBool aIgnoreStateFlag=EFalse );

    /**
     * Get function for member iAlwaysOnlineState
     * @return value of iAlwaysOnlineState
     */
    IMPORT_C TIpsSetDataAoStates AlwaysOnlineState() const;

    /**
     * Set method for member iSelectedWeekDays
     * @param aSelectedWeekDays, Sets value of iSelectedWeekDays
     */
    IMPORT_C void SetSelectedWeekDays(
        const TUint aSelectedWeekDays );

    /**
     * Get function for member iSelectedWeekDays
     * @return value of iSelectedWeekDays
     */
    IMPORT_C TUint SelectedWeekDays() const;

    /**
     * Set method for member iSelectedTimeStart
     * @param aSelectedTimeStart, Sets value of iSelectedTimeStart
     */
    IMPORT_C void SetSelectedTimeStart(
        const TTime aSelectedTimeStart );

    /**
     * Get function for member iSelectedTimeStart
     * @return value of iSelectedTimeStart
     */
    IMPORT_C TTime SelectedTimeStart() const;

    /**
     * Set method for member iSelectedTimeStop
     * @param aSelectedTimeStop, Sets value of iSelectedTimeStop
     */
    IMPORT_C void SetSelectedTimeStop(
        const TTime aSelectedTimeStop );

    /**
     * Get function for member iSelectedTimeStop
     * @return value of iSelectedTimeStop
     */
    IMPORT_C TTime SelectedTimeStop() const;

    /**
     * Set method for member iInboxRefreshTime
     * @param aInboxRefreshTime, Sets value of iInboxRefreshTime
     */
    IMPORT_C void SetInboxRefreshTime(
        const TInt aInboxRefreshTime );

    /**
     * Get function for member iInboxRefreshTime
     * @return value of iInboxRefreshTime
     */
    IMPORT_C TInt InboxRefreshTime() const;

    /**
     * Set method for member iUpdateMode
     * @param aUpdateMode, Sets value of iUpdateMode
     */
    IMPORT_C void SetUpdateMode(
        const TIpsSetDataAoUpdateModes aUpdateMode );

    /**
     * Get function for member iUpdateMode
     * @return value of iUpdateMode
     */
    IMPORT_C TIpsSetDataAoUpdateModes UpdateMode() const;

    /**
     * Set method for member iLastUpdateInfo
     * @param aLastUpdateInfo, Sets value of iLastUpdateInfo
     */
    IMPORT_C void SetLastUpdateInfo( const TAOInfo& aLastUpdateInfo );

    /**
     * Get function for member iLastUpdateInfo
     * @return value of iLastUpdateInfo
     */
    IMPORT_C TAOInfo LastUpdateInfo() const;

    /**
      *
      *
      * @param
      * @return
      */
    IMPORT_C void SetOutgoingLogin( const TInt aOutgoingLogin );

    /**
      *
      *
      * @param
      * @return
      */
    IMPORT_C TInt OutgoingLogin() const;
        
    /**
    *
    */
    IMPORT_C void SetLastModified( TTime aTime );

    /**
    *
    */    
    IMPORT_C TTime LastModified() const;
    
    /**
    *
    */    
    IMPORT_C void SetSyncStatus( TInt aLastSyncStatus );
    
    /**
    *
    */
    IMPORT_C TInt LastSyncStatus() const;
    
    /**
    *
    */
    IMPORT_C void SetEmnReceivedButNotSyncedFlag( TBool aFlag );
    
    /**
    *
    */
    IMPORT_C TBool EmnReceivedButNotSyncedFlag() const;

    /**
    *
    */
    IMPORT_C void PopulateDefaults();
    
//<cmail>  
    /**
    * Set method for member iHiddenData 
    * @param ETrue if username, servers etc. are hidden
    */
    IMPORT_C void SetDataHidden( const TBool aIsHidden );
    
    /**
    * Get method for member iHiddenData 
    * @return value of iLastUpdateInfo
    */
    IMPORT_C TBool DataHidden() const;
    
    /**
     * Sets a flag when mailbox receives it's first OMA EMN.
     * (email notification)
     */
    IMPORT_C void SetFirstEmnReceived( TBool aValue );
    
    /**
     * Returns flag which indicates has mailbox received 
     * it's first OMA EMN. 
     */
    IMPORT_C TBool FirstEmnReceived() const;
    
private:  // Constructors and destructor

    /**
     * ConstructL
     */
    void ConstructL();

    /**
     * Default constructor
     */
    CIpsSetDataExtension();

private:    // Data

    /**
     * Flags for storing.
     */
    /*enum TMailExtensionFlags
        {
        EMailNewIndicators      = 0x01,
        EMailNewNotifications   = 0x02,
        EMailHideMsgsOverLimit  = 0x04,
        EMailShowFieldCC        = 0x08,
        EMailShowFieldBCC       = 0x10,
        EMailShowFieldSubject   = 0x20,
        EMailRoamHomeOnly       = 0x40
        };*/

    /**
     * Common: Id of the mailbox this settings belongs
     */
    TInt                iMailboxId;
    /**
     * Common: Account id of the base settings
     */
    TUint32             iAccountId;
    /**
     * Common: Account id of the extended settings
     */
    TUint32             iExtensionId;
    /**
     * Common: Protocol of the base settings
     */
    TUid                iProtocol;
    /**
     * Extended: HTML mail opening mode.
     */
    TInt                iOpenHtmlMail;
    /**
     * AO: Settings version.
     */
    TInt                iVersion;
    /**
     * AO:
     */
    TIpsSetDataAoStates       iAlwaysOnlineState;
    /**
     * AO: Selected weekdays: 0-6 = Monday-Sunday
     */
    TUint               iSelectedWeekDays;
    /**
     * AO: Connection starting time
     */
    TTime               iSelectedTimeStart;
    /**
     * AO: Connection ending time
     */
    TTime               iSelectedTimeStop;
    /**
     * AO: Inbox refresh interval in minutes
     */
    TInt                iInboxRefreshTime;
    /**
     * AO: Mail update mode.
     */
    TIpsSetDataAoUpdateModes iUpdateMode;
    /**
     * AO: Email address for Always Online.
     */
    RBuf                iEmailAddress;
    /**
     * EMN: Emn states
     */
    TIpsSetDataEmnStates iEmnState;
            
    TBool               iHideMessages;
    TBool               iNewMailIndicators;
    
//<cmail>    
    /**
     * User name and server addresses are hidden from the user
     */
    TBool               iHiddenData;
//</cmail>    
    
    /**
     * AO: Last update status.
     */
    TAOInfo             iLastUpdateInfo;
    /**
     * Method to make outgoing login
     */
    TInt                iOutgoingLogin;
    /**
    * Timestamp to indicate last modification timevoid SetLastModified()
    */
    TTime               iLastModified;
    
    TInt                iSyncStatus;
    
    TBool               iEmnReceivedButNotSyncedFlag;
    
    /**
     * Common: Status flags of settings
     */
    TUint64             iStatusFlags;
    
    TBool               iFirstEMNReceived;
    

    };

#endif      // IPSSETDATAEXTENSION_H

// End of File
