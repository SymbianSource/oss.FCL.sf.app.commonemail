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
* Description:  Class to handle content publishing for widget
*
*/


#ifndef __CMAILCPSHANDLER_H__
#define __CMAILCPSHANDLER_H__

#include <aknappui.h>
#include <memailobserverlistener.h> // base class
#include <AknGlobalNote.h>

// FSMailServer classes (base classes and callback interfaces)
#include "fsnotificationhandlerbase.h"
#include "fsnotificationhandlernotifierinitiator.h"
#include "fsnotificationhandlernotifierinitiatorobserver.h"
// Member classes
#include "cmailcpssettings.h" // MMailCpsSettingsCallback

class CMailCpsIf;
class CFSMailBox;
class CMailMailboxDetails;
class CMailCpsSettings;
class CMailExternalAccount;
class CMailPluginProxy;
class CEmailObserverPlugin;
class CMailCpsUpdateHelper;

/**
 * CMail ContentPublishingService Handler class
 * Class implementing a handler functionality towards rest of the MailServer
 * 
 *  @lib fsmailserver.exe
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CMailCpsHandler ) : 
    public CFSNotificationHandlerBase,
    public MMailCpsSettingsCallback,
    public EmailInterface::MEmailObserverListener
    {
public:
    
    /**
     * Two-phased constructor.
     * @param aOwner Owner and manager of this handler.
     */ 
    static CMailCpsHandler* NewL( MFSNotificationHandlerMgr& aOwner );
    
    /**
    * Destructor.
    */
    virtual ~CMailCpsHandler();

    /**
     * From MMailCpsSettingsCallback
     */
    virtual void SettingsChangedCallback();

    /**
     *
     */
    void SetUpdateNeeded( const TFSMailMsgId aMailbox );
    
    /**
     * Wraps all "Update*" methods
     */
    void UpdateFullL();    

    /**
     * Method that actually does the full update.
     * Used by CMailCpsUpdateHelper.
     */
    void DoUpdateFullL();    
	
    /**
     *
     */
    void UpdateMailboxesL(TInt aInstance, const TDesC& aContentId);

    /**
     *
     */
    void LaunchWidgetSettingsL( const TDesC& aContentId );

    /**
     *
     */
    void LaunchEmailUIL( const TDesC& aContentId );

    /**
     *
     */    
    void LaunchEmailWizardL( const TDesC& aContentId );
    /**
     * Launches an application based on the given contentId
     * This method is for external (3rd party) accounts
     * @param aContentId specifies the widget that was pressed by the user
     */
    void LaunchExtAppL( const TDesC& aContentId );

    /**
     *
     */		
    void DissociateWidgetFromSettingL( const TDesC& aContentId );    

    /**
     * Return total number of mailboxes (native+3rd party) in the system
     */
    TInt TotalMailboxCountL();
    
    /**
     * Return total number of native mailboxes in the system
     */     
    TInt TotalIntMailboxCount();

    /**
     * Return total number of 3rd party mailboxes in the system
     */
    TInt TotalExtMailboxCountL();
    
	/**
     * Gets correct localised format for time (or date) string
     */ 
    static HBufC* GetMessageTimeStringL( TTime aMessageTime );

    // From MEmailObserverListener
    void EmailObserverEvent( EmailInterface::MEmailData& aData );

    /**
     *
     */
    void SetWaitingForNewMailbox( const TDesC& aContentId );
    
    /**
     *
     */
    void CleanWaitingForNewMailbox();

    /**
     *
     */
    void SetWaitingForNewWidget( const TFSMailMsgId aMailbox );
    
    /**
     *
     */
    void CleanWaitingForNewWidget();

    /**
     *
     */
    TFSMailMsgId WaitingForNewWidget();

    /**
     *
     */
    void AssociateWidgetToSetting( const TDesC& aContentId,
                                   const TFSMailMsgId aMailbox );

    /**
     *
     */    
    TBool Associated( const TDesC& aContentId );

    /**
     *
     */
    void DisplayHSPageFullNoteL();
        
    /**
     * get count of unread messages in inbox
     * @param aMailbox mailbox id
     */    
    TInt GetUnreadCountL( TFSMailMsgId aMailbox);

protected:
    /**
     * From CFSNotificationHandlerBase
     * see baseclass for details
     */
    virtual TBool CapabilitiesToContinueL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 ) const;

    /**
     * From CFSNotificationHandlerBase
     * see baseclass for details
     */
    virtual void HandleEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 );
    
private:
    /**
     * Constructor
     * @param aOwner reference to owning class. CpsHandler is just one of
     *        the handlers running in MailServer process.
     */
    CMailCpsHandler( MFSNotificationHandlerMgr& aOwner );
    void ConstructL();

    // Initialization methods
    /**
     * Resets everything
     */
    void Reset();

    /**
     * Initializes native accounts
     */
    void InitializeL();
    
    /**
     * Initializes external accounts
     */
    void InitializeExternalAccountsL();
    
    // Creation methods
    /**
     * Creates an instance of mailboxdetails based on parameters
     * @param aMailbox mailbox
     * @return pointer to instance of CMailMailboxDetails.
     *         Caller gets ownership of the returned object.
     */
    CMailMailboxDetails* CreateMailboxDetailsL( CFSMailBox& aMailbox );

    /**
     * Finds mailbox details instance from the local array
     * @param aMailbox mailbox id
     * @return pointer to instance of CMailMailboxDetails
     */
    CMailMailboxDetails* FindMailboxDetails( TFSMailMsgId aMailbox );
    
    /**
     *
     */
    void CMailCpsHandler::UpdateMailboxNameL( const TInt aMailBoxNumber,
                                              const TInt aWidgetInstance,
                                              const TInt aRowNumber );

    /**
     * Handles publishing of message details
     */
	void UpdateMessagesL( const TInt aMailBoxNumber,
                          const TInt aWidgetInstance,
	                      const TInt aMessageNumber,
                          const TInt aFirstRow );
	
	/**
     *
     */
	void UpdateEmptyMessagesL( const TInt aWidgetInstance,
	                           const TInt aFirstRow );

    /**
     * Handles publishing of mailbox icon
     */
    void UpdateMailBoxIconL( const TInt aMailBoxNumber,
                             const TInt aWidgetInstance,
                             const TInt aRowNumber );

    /**
     * Handles publishing of unseen icon
     */    
    void UpdateIndicatorIconL( const TInt aMailBoxNumber, 
                               const TInt aWidgetInstance,
                               const TInt aRowNumber );

    // Event handling subroutines
    /**
     * handles new mailbox event
     * @param aMailbox mailbox id
     */
    void HandleNewMailboxEventL( const TFSMailMsgId aMailbox );

    /**
     * Handles mailbox renamed event.
     * @param aMailbox mailbox id
     */
    void HandleMailboxRenamedEventL( const TFSMailMsgId aMailbox );

    /**
     * handles mailbox deleted event
     * @param aMailbox mailbox id
     */
    void HandleMailboxDeletedEventL( const TFSMailMsgId aMailbox );
    
    /**
     * handles new mail event
     * @param aMailbox mailbox id
     * @param aParam1 contains event specific parameters
     * @param aParam2 contains event specific parameters
     */
    void HandleNewMailEventL( TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2 );

    /**
     * handles mail deleted event
     * @param aMailbox mailbox id
     * @param aParam1 contains event specific parameters
     * @param aParam2 contains event specific parameters
     */
    void HandleMailDeletedEventL( TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2 );

    /**
     * get count of unseen messages in inbox
     * @param aMailbox mailbox id
     */    
    TInt GetUnseenCountL( TFSMailMsgId aMailbox);    
    
    /**
     * Tells if outbox is empty
     * @param aMailbox mailbox id
     */    
    TBool IsOutboxEmptyL( TFSMailMsgId aMailbox);    
    
    /**
     * From CFSNotificationHandlerBase
     * Not used
     */
    virtual void TurnNotificationOn();

    /**
     * From CFSNotificationHandlerBase
     * Not used
     */
    virtual void TurnNotificationOff();

    /**
     *
     */
    TInt GetMailIcon( CFSMailMessage* aMsg );

    /**
     *
     */	
    TInt GetUnreadMsgIcon( CFSMailMessage* aMsg );
	
    /**
     *
     */	
    TInt GetReadMsgIcon( CFSMailMessage* aMsg );

    /**
     *
     */
    TInt GetUnreadCalMsgIcon( CFSMailMessage* aMsg );

    /**
     *
     */    
    TInt GetReadCalMsgIcon( CFSMailMessage* aMsg );
 
    /**
     *
     */
    TBool IsValidDisplayName(const TDesC& aDisplayName);

    /**
     *
     */     
    void PublishMessageL( TInt aWidgetInstance, CFSMailMessage& aMessage,
                          const TDesC& aSenderName,
                          const TDesC& aSubject,
                          const TDesC& aTime, TInt aFirstRow);
   
    /**
     * Checks if the given message is already know/published
     * @param aMailbox specifies the mailboxdetails object where duplicate is searched from
     * @param aMsgId Id of the new message
     * @return ETrue if duplicate, EFalse if new message
     */
    TBool IsDuplicate( const CMailMailboxDetails& aMailbox, const TFSMailMsgId& aMsgId );

    //
    // Private methods related to 3rd party email widget publishers
    //
    /**
     * Removes all plugin proxies (and plugins) that are not listed in aAccounts
     * @param aAccounts up-to-date list of external (3rd party) email accounts
     *        that are specified in settings
     */
    void RemoveUnusedPluginsL( RPointerArray<CMailExternalAccount>& aAccounts );
    
    /**
     * Goes through the array of external accounts and instantiates (newly added)
     * 3rd party plugins
     * @param aAccounts up-to-date list of external (3rd party) email accounts
     *        that are specified in settings
     */
    void AddNewPluginsL( RPointerArray<CMailExternalAccount>& aAccounts );
    
    /**
     * Goes through the array of existing 3rd party plugin (proxies) and
     * lets them take the accounts given in the param array.
     * Plugin proxies also update the widget data.
     * @param aAccounts up-to-date list of external (3rd party) email accounts
     *        that are specified in settings
     */
    void SelectAndUpdateExtAccountsL( RPointerArray<CMailExternalAccount>& aAccounts );
    
    /**
     * Checks whether aAccounts array has entries relating to plugin with id aPluginId
     * @param aPluginId 3rd party plugin identifier (implementation uid)
     * @param aAccounts array of 3rd party email account information
     * @return true or false
     */
    TBool IsPluginInArray( const TInt aPluginId, RPointerArray<CMailExternalAccount>& aAccounts );
    
    /**
     * Checks whether aPlugins array has entries relating to plugin with id aPluginId
     * @param aPluginId 3rd party plugin identifier (implementation uid)
     * @param aPlugins array of plugin proxies
     * @return true or false
     */
    TBool IsPluginInArray( const TInt aPluginId, RPointerArray<CMailPluginProxy>& aPlugins );

    /**
     * Goes through iExternalPlugins array and finds correct entry
     * @param aContentId specifying a widget/account
     * @return correct plugin proxy instance pointer (ownership not transferred)
     *         if not found, NULL is retuned
     */
    CMailPluginProxy* GetExtPluginL( const TDesC& aContentId );
    
private: // data
    CEikonEnv* iEnv;
    // pointer to liw wrapper that handles actual publishing
    CMailCpsIf*                        iLiwIf;
    // pointer to settings class that handles settings loading and observing logic
    CMailCpsSettings*                  iSettings;
    // local cache of mailbox details
    RPointerArray<CMailMailboxDetails> iAccountsArray;
    // array of plugin proxies. One plugin proxy handles one 3rd party plugin and its accounts
    RPointerArray<CMailPluginProxy>    iExternalPlugins;
    // tells if email wizard is started from widget and it is still running
    // and to which widget next added account is added.
    HBufC*                             iWaitingForNewMailbox;
    //
    TFSMailMsgId                       iWaitingForNewWidget;
    //
    CAknGlobalNote*                    iQuery;
    // Helper for limiting rate of updates to Homescreen widget
    CMailCpsUpdateHelper*              iUpdateHelper;
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Helper class for limiting rate of updates to Homescreen widget

// Don't make widget updates more often than this delay (milliseconds)
#define KMailCpsHandlerUpdateDelay 1000

class CMailCpsUpdateHelper : public CTimer
    {
    public:
        static CMailCpsUpdateHelper* NewLC( CMailCpsHandler *aHandler );
        static CMailCpsUpdateHelper* NewL( CMailCpsHandler *aHandler );

        virtual ~CMailCpsUpdateHelper();

        // Notify that Homescreen widget(s) should be updated
        void UpdateL();	

    protected:
        void ConstructL();
        virtual void RunL();
        TInt RunError( TInt aError );

    private:
        CMailCpsUpdateHelper( CMailCpsHandler *aHandler );

        // Performs the update, resets the timer, etc.
        void DoUpdateL();

        // Handler to use to do updates (not owned)
        CMailCpsHandler *iCpsHandler;
        // Whether or not an update is pending
        TBool iPending;
    };


#endif  //__CMAILCPSHANDLER_H__
