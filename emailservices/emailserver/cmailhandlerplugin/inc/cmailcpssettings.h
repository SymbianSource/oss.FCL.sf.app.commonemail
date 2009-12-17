/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  handles interface to CenRep settings
*
*/


#ifndef CMAILCPSSETTINGS_H_
#define CMAILCPSSETTINGS_H_

class CRepository;

/*
 * Callback interface for observer
 * 
 *  @lib fsmailserver.exe
 *  @since S60 v5.1
 */
class MMailCpsSettingsCallback
    {
public:
    /**
     * Callback from CMailCpsSettings class
     * Informs that settings have changed
     *
     * @since S60 v5.1
     */
    virtual void SettingsChangedCallback() = 0;
    };

/**
 *  CMail ContentPublishingService Settings Handler class
 *  Class implementing interface towards cenrep and email widget settings
 * 
 *  @lib fsmailserver.exe
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CMailCpsSettings ) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aMailClient reference to CFSMailClient class. Mailserver offers
     *        this for all handlers in order to prevent parallel instantiation
     */
    static CMailCpsSettings* NewL( CFSMailClient& aMailClient );

    /**
    * Destructor.
    */
    virtual ~CMailCpsSettings();

    /**
     * Starts observing CenRep changes
     * @param aObserver will be callbacked once changes occur
     */
    void StartObservingL( MMailCpsSettingsCallback* aObserver );
    
    /**
     * Stops observing CenRep changes
     */
    void StopObserving();
    
    /**
     * Gets array of mailboxes that should be published to widget
     * @return array of mailboxes
     */
    RArray<TFSMailMsgId>& Mailboxes();

    /**
     * Adds mailbox to widget settings
     * Method ensures that same mailbox is not added again, if it happens
     * to exist already in the settings. If already exists, method simply returns without
     * doing nothing.
     * @param aMailbox Id of the mailbox to add to settings
     */
    void AddMailboxL( const TFSMailMsgId aMailbox );
    
    /**
     * Removes mailbox from widget settings
     * @param aMailbox Id of the mailbox to remove from settings
     */
    void RemoveMailboxL( const TFSMailMsgId aMailbox );

    /**
     * Removes mailbox from widget settings
     * @param aIndex refers to index in mailbox array that is returned by
     *        CMailCpsSettings::Mailboxes()
     */
    void RemoveMailboxL( const TInt aIndex );

    /**
     * Gets maximum row count
     * @return maximum row count
     */
    TInt MaxRowCount();

    /**
     * Gets maximum mailbox count
     * @return maximum mailbox count
     */
    TInt MaxMailboxCount();

    /**
     * 
     * @return
     */
    TBool AssociateWidgetToSetting( const TDesC& aContentId );

    /**
     * 
     */
    void DissociateWidgetFromSetting( const TDesC& aContentId );

    /**
     * 
     */
    void GetContentId( TInt aId, TDes16& aValue );
    
    /**
     * 
     */	
	TBool IsSelected( TInt aId );
	
    /**
     * 
     */    
    TUint GetMailboxUidByContentId( const TDesC& aContentId );

    /**
     * 
     */    
    TUint GetPluginUidByContentId( const TDesC& aContentId );    
    
    /**
     * Gets a bitmasked configuration value (see cmailwidgetcenrepkeysinternal.h)
     * @return bitmasked configuration value
     */
    TInt32 Configuration();

    /**
     * 
     */    
    TInt GetTotalMailboxCount();
    
protected:
    /**
     * From CActive
     * Cancellation logic for the asynchronous request
     *
     * @since S60 v5.0
     */
     virtual void DoCancel();

     /**
      * From CActive
      *
      * @since S60 v5.0
      */
     virtual void RunL();

private:
    /**
     * Constructor
     * @param aMailClient reference to CFSMailClient class. Mailserver offers
     *        this for all handlers in order to prevent parallel instantiation
     */
    CMailCpsSettings( CFSMailClient& aMailClient );

    void ConstructL();

    /**
     * Restarts observing after previous notification request was triggered
     */
    void StartObservingL();

    /**
     * Loads settings from CenRep
     */
    void LoadSettingsL();

    /**
     * Loads configuration data from CenRep
     * (This data contains internal configuration data for publishing logic
     * and should not be mixed with user modifiable settings)
     */
    void LoadConfigurationL();
    
    /**
     * Resolves mailbox based on mailbox id
     * (mailbox id is stored in CenRep as integer)
     * 
     * @param aMailboxId mailboxId from CenRep
     * @param aMsg on completion, contains the mailbox Id in TFSMailMsgId format
     * @return KErrNone if mailbox exists
     * @return KErrNotFound if aValue points to non-existent mailbox
     */
    TInt ResolveMailbox( const TInt aMailboxId, TFSMailMsgId& aMsg ); 

    /**
     * Gets array of CenRep keys representing mailboxes configured to widget
     * @param aKeys array of keys
     */
    void GetMailboxNonZeroKeysL( RArray<TUint32>& aKeys );

    /**
     * 
     * @param aContentId
     */
    TBool IsAlreadyAssociated( const TDesC& aContentId );
	
    /**
     * 
     */	
    TUint32 GetSettingToAssociate();
    
private: // data
    // reference to mailclient
    CFSMailClient&            iMailClient;
    // pointer to central repository
    CRepository*              iCenRep;
    // callback interface
    MMailCpsSettingsCallback* iObserver;
    // array of mailboxes
    RArray<TFSMailMsgId>      iMailboxArray;
    // Internal configuration data
    TInt32                    iConfigData; 
    };

#endif /*CMAILCPSSETTINGS_H_*/
