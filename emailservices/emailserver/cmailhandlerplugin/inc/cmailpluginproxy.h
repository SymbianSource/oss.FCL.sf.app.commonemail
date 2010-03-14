/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines class CMailMessageDetails.
*
*/

#ifndef CMAILPLUGINPROXY_H_
#define CMAILPLUGINPROXY_H_

#include <e32def.h>
#include <e32base.h>
#include <emailobserverplugin.h>
#include <memailobserverlistener.h>
#include <memailmailboxdata.h>

class CMailExternalAccount;
class CMailCpsIf;

/**
 * Class that represents the 3rd party email widget plugin to the rest of the cps logic
 * Also takes care of all the email accounts that the plugin handles
 * @lib cmailhandlerplugin.lib
 * @since S60 v9.2
 */
NONSHARABLE_CLASS( CMailPluginProxy ) : public CBase, public EmailInterface::MEmailObserverListener
    {
public:
    static CMailPluginProxy* NewL(
        const TInt aPluginId,
        CMailCpsIf& aPublisher );

    virtual ~CMailPluginProxy();
    
    // From MEmailObserverListener
    /**
     * Called by 3rd party plugins when there are chnages in the data to be published
     * @param aData offers access to all the necessary data needed for publishing data on widget
     */
    void EmailObserverEvent( EmailInterface::MEmailData& aData );

    /**
     * Accessor for the plugin id
     * @return this plugin's 'implementation id'
     */
    TInt PluginId();
        
    /**
     * Whether this plugin handles the specified contentId (i.e. email account)
     * @param aContentId specifying a widget instance and a mailbox related to it
     * @return boolean
     */
    TBool HasAccount( const TDesC& aContentId );
    
    /**
     * Looks up the data related to trigger event, and launches the specified application
     * @param aContentId content id of the widget that received the key press
     */
    void LaunchExtAppL( const TDesC& aContentId );
    
    /**
     * Picks the accounts that belong to this plugin, and updates the data
     * publishing of them all
     * @param aAccounts list of external accounts
     */
    void SelectAndUpdateAccountsL( RPointerArray<CMailExternalAccount>& aAccounts );

    /**
     * Handles updating of one account/widget data based on contentId
     * @param aContentId content id of the widget
     */
    void UpdateAccountL( const TDesC& aContentId );

private:

    void ConstructL();

    CMailPluginProxy(
        const TInt aPluginId,
        CMailCpsIf& aPublisher );

    void RemoveAccountsL();
    
    void ResetAccountL( const TDesC& aContentId );

    void ResetMessageRowL( const TInt aWidgetInstanceId, const TInt aRow );

    void SelectOwnAccountsL( RPointerArray<CMailExternalAccount>& aAccounts );
    
    TInt ResolveWidgetInstance( const TDesC& aContentId );

    TInt GetMailboxId( const TDesC& aContentId );

    void PublishAccountsL();

    void PublishAccountL( const TInt aWidgetInstanceId, const TInt aMailboxId );

    void PublishAccountL( const TInt aWidgetInstanceId, EmailInterface::MMailboxData& aMailboxData );

    void PublishFirstRowL( const TInt aWidgetInstanceId, EmailInterface::MMailboxData& aMailboxData );
    
    void PublishMessageRowL( const TInt aWidgetInstanceId, EmailInterface::MMessageData& aMessageData, const TInt aRow );
    
    HBufC* ConstructMailboxNameWithMessageCountLC( EmailInterface::MMailboxData& aMailboxData );
    
    TInt ResolveIndicatorIcon( EmailInterface::MMailboxData& aMailboxData );

    EmailInterface::MMailboxData& GetMailboxDataL( TInt aMailboxId );
    
    TInt UnreadCountL( EmailInterface::MMailboxData& aMailboxData );
    
private: // data
    // identifier of the plugin
    TInt iPluginId;
    // plugin instance where rest of the data is buffered
    EmailInterface::CEmailObserverPlugin* iPlugin;
    // reference to publisher interface
    CMailCpsIf& iPublisher;
    // array of accounts that have widget and this plugin is responsible for
    RPointerArray<CMailExternalAccount> iAccounts;
    };

#endif /* CMAILPLUGINPROXY_H_ */
