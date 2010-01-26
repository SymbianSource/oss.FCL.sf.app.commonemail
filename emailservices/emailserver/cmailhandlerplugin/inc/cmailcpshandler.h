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

/**
 * CMail ContentPublishingService Handler class
 * Class implementing a handler functionality towards rest of the MailServer
 * 
 *  @lib fsmailserver.exe
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CMailCpsHandler ) : 
    public CFSNotificationHandlerBase,
    public MMailCpsSettingsCallback
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
     * Wraps all "Update*" methods
     */
    void UpdateFullL();    
	
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
    void LaunchEmailWizardL();
    /**
     *
     */	
    TBool AssociateWidgetToSetting( const TDesC& aContentId );
	
    /**
     *
     */		
    void DissociateWidgetFromSettingL( const TDesC& aContentId );    

    /**
     *
     */     
    TInt GetMailboxCount();    

    /**
     *
     */    
    void ManualAccountSelectionL( const TDesC& aContentId );
    
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
     * Initializes everything
     */
    void InitializeL();
    
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
     * Gets correct localised format for time (or date) string
     */	
	HBufC* GetMessageTimeStringL( TTime aMessageTime );
	
	/**
     *
     */
	void UpdateEmptyMessagesL( const TInt aWidgetInstance,
	                           const TInt aFirstRow );

    /**
     * Handles publishing of connection state icon
     */
    void UpdateConnectStateL( const TInt aMailBoxNumber, const TInt aRowNumber );       

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
    
    /**
     * Handles publishing of empty message time string
     */
// void ClearMessageTimeL( const TInt aRowNumber );

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
     * handles mail changed event
     * @param aMailbox mailbox id
     * @param aParam1 contains event specific parameters
     * @param aParam2 contains event specific parameters
     */
    void HandleMailChangedEventL( TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2 );

    /**
     * handles new folder event
     * @param aMailbox mailbox id
     * @param aParam1 contains event specific parameters
     * @param aParam2 contains event specific parameters
     */
    void HandleNewFolderEventL( TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2 );
	
    /**
     * get count of unread messages in inbox
     * @param aMailbox mailbox id
     */    
    TInt GetUnreadCountL( TFSMailMsgId aMailbox);

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
                           const TDesC& aSenderName, const TDesC& aTime, TInt aFirstRow);
   
    /**
     * Checks if the given message is already know/published
     * @param aMailbox specifies the mailboxdetails object where duplicate is searched from
     * @param aMsgId Id of the new message
     * @return ETrue if duplicate, EFalse if new message
     */
    TBool IsDuplicate( const CMailMailboxDetails& aMailbox, const TFSMailMsgId& aMsgId );
    
    /**
     *
     */
    TBool FirstBootL();
    
private: // data
    CEikonEnv* iEnv;
    // pointer to liw wrapper that handles actual publishing
    CMailCpsIf*                        iLiwIf;
    // pointer to settings class that handles settings loading and observing logic
    CMailCpsSettings*                  iSettings;
    // local cache of mailbox details
    RPointerArray<CMailMailboxDetails> iAccountsArray;
    };

#endif  //__CMAILCPSHANDLER_H__
