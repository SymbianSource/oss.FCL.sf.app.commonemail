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
* Description: This file defines classes MIpsPlgPropertyObserver, CMBoxEventObserver, CIPSAccountInfo, CIpsPlgEventHandler
*
*/


#ifndef IPSPLGEVENTHANDLER_H
#define IPSPLGEVENTHANDLER_H


#include "ipsplgsettingsobserver.h"
#include "ipsplgpropertywatcher.h"
#include <e32property.h>

class CIpsPlgSosBasePlugin;
class CMsvSession;
class CRepository;
class CIpsPlgSettingsObserver;
class CIpsPlgSyncStateObserver;
class CIpsSetDataApi;
/**
* MIpsPlgPropertyObserver
*
*/
class MIpsPlgPropertyObserver
    {
public:
    virtual void HandlePropertyEventL(
            TInt aEvent,
            TInt aMailbox,
            TInt aPluginId ,
            TInt aError ) = 0;
    };

/**
* Helper class to store observer references
*/
class CMBoxEventObserver : public CBase
    {
public:
    CMBoxEventObserver(
        MFSMailEventObserver& aObserver,
        const TFSMailMsgId& aMBoxId ) :
            iObserver( aObserver ),
            iMBoxId( aMBoxId ) { }

    MFSMailEventObserver&   iObserver;
    const TFSMailMsgId      iMBoxId;
    };

/**
* Helper class to keep track of accounts
*/
class CIPSAccountInfo : public CBase
    {
public:

    static CIPSAccountInfo* NewL(
        const TDesC& aName,
        const TFSMailMsgId& aMBoxId );

    virtual ~CIPSAccountInfo();

    HBufC*                  iName;
    const TFSMailMsgId      iMBoxId;

private:

    CIPSAccountInfo( const TFSMailMsgId& aMBoxId );

    void ConstructL( const TDesC& aName );
    };

/**
 *
 *  @lib ipssosplugin.lib
 *  @since
 */
class CIpsPlgEventHandler : public CBase,
                            public MMsvSessionObserver,
                            public MIpsPlgSettingsObserverCallback,
                            public MIpsPlgPropertyObserver,
                            public MFSMailExceptionEventCallback
    {

public://from MFSMailExceptionEventCallback

    void ExceptionEventCallbackL( TFSMailMsgId aMailboxId, TInt aEventType, TBool aResponse );

public: //from MIpsPlgPropertyObserver
    void HandlePropertyEventL(
            TInt aEvent, TInt aMailbox, TInt aPluginId, TInt aError );

public: // from MMsvSessionObserver

    virtual void HandleSessionEventL(
        TMsvSessionEvent aEvent,
        TAny* aArg1, TAny* aArg2, TAny* aArg3);

public: //from MIpsPlgSettingsObserverCallback

    virtual void SettingsChangedL( TFSMailMsgId aAccount );

public:

    /**
     * Two-phased constructor.
     */
     static CIpsPlgEventHandler* NewL(
        CIpsPlgSosBasePlugin& aBasePlugin );

    /**
     * Destructor.
     */
    virtual ~CIpsPlgEventHandler();

    /**
     * Sets msv session pointer, and does initialization that need
     * valid session
     * @param aSession valid msv session pointer
     */
    void CompleteConstructL( CMsvSession* aSession );

    /**
     * Add observer for common plugin events like changes in mailboxes
     * @param aObserver interface to observer object
     */
    void AddPluginObserverL( MFSMailEventObserver* aObserver );

    /**
     * Removes plugin observer
     * @param aObserver indicates object to be removed
     */
	void RemovePluginObserver( MFSMailEventObserver* aObserver );

    /**
     * Sets observer to mailbox events like changes in messages and folders
     * under mailbox
     * @param aMailboxId FS mailbox id, corresponds
     *                   imap/pop msv service entry id
     * @param aObserver interface to observer object
     * @param aKey key to mailbox settings
     */
    void SubscribeMailboxEventsL(
        const TFSMailMsgId& aMailboxId,
        MFSMailEventObserver& aObserver,
        TUint32 aKey );

    /**
     * Removes mailbox event observer
     * @param aMailboxId fs mailbox id
     * @param aObserver observer object to be removed
     */
    void UnsubscribeMailboxEvents(
        const TFSMailMsgId& aMailboxId,
        MFSMailEventObserver& aObserver );

    /**
     * Signal event to all plugin instances via p&s (property) api
     * @param aMailboxId entry id to mailbox
     * @param aEvent event enumeration
     * @param aError error code
     * @return error code from RProperty::Set function
     */
    TInt SetNewPropertyEvent( TInt aMailbox, TInt aEvent, TInt aError );

    /**
     * Adds property observer, p&s (property) events are signaled to these
     * observers
     * @param aObservers interface to observer object
     * @return error code from RProperty::Define function
     */
    TInt RegisterPropertyObserverL( MIpsPlgPropertyObserver* aObserver );

    /**
     * Removes property observer
     * @param aObserver pointer to observer to be removed, if null or
     *                  unregisterd observer then function simply returns
     */
    void UnRegisterPropertyObserver( MIpsPlgPropertyObserver* aObserver );

    /**
     * Used by CIpsPlgPropetyWatcher active object
     * @param aEvent property event
     */
    void NotifyPropertyEventL( TIpsPlgPropertyEvent aEvent );

    /**
     * used to inform ui offline state if connection attempt
     * was not successfull
     * @param aAccount FS mailbox id
     */
    void SignalMailboxOfflineStateL( const TFSMailMsgId& aAccount );

    /**
     * Send user password query request to fs email ui.
     * Request is handled by emailserver.
     * Only one query at a time is allowed - restriction of CFSMailMessageQueryHandler
     * Operation callback is registered in handler and will receive notification
     * on query password finished.
     * 
     * @param aMbox entry id of mailbox service
     * @param aCallback callback interface to connection operation, defaut NULL
     * @param aIncoming incoming or outgoing password query
     * @return ETrue query invoked, wait for notification
     *         EFalse another query is in progress
     */
    TBool QueryUsrPassL(
        TMsvId aMbox,
        MIpsPlgConnectOpCallback* aCallback=NULL,
        TBool aIncoming=ETrue );

    /**
     * Query in progress is for incoming or outgoing mail.
     */
    TBool IncomingPass() const;

    /**
     * Send user password (=credentials) set signal to all plugin
     * instances via p&s (property) mechanism
     * @param aMailboxId entry id of mailbox
     * @param aCancelled indicates is user cancelled password dialog
     */
    void SignalCredientialsSetL( TInt aMailboxId, TBool aCancelled );

private:

    /** States of Query user password operation */
    enum TQueryUsrPassState
        {
        EReady,                 // ready for action
        EBusy,                  // request in progress (invoker eventhandler`s state)
        ENotificationRequest,   // user notification request (emailserver eventhandler`s state)
        EPasswordRequest,       // password request (emailserver eventhandler`s state)
        ERequestResponding      // response for request received, handling in progress (invoker eventhandler`s state)
        };

    /**
    * Checks the source type of event and also
    * checks is the event from IPS type  ( imap/pop ) entry
    *
    * @param aSourceType return parameted indicating is the source:
    *   -service
    *   -message
    *   -folder
    *
    * @return ETrue if event is from IPS source
    */
    TBool IsEventFromIpsSourceL( TAny* aArg1,
            TUid& aSourceType,
            TMsvEntry& aEntry );

    /**
    * Passes events to FS framework after a delay.
    * NOTE: delay not implemented until there is proof to its necessity
    */
    void SendDelayedEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aArg1,
        TAny* aArg2,
        TAny* aArg3 );

    /**
    * gets TMsvIds of all IPS mail accouts for later use.
    * @param aAccounts Reference to list of accounts
    */
    void IPSAccountsL();

    /**
    *
    *
    */
    void HandleEntriesCreatedL( TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    /**
    *
    *
    */
    void HandleEntriesMovedL( TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    /**
    *
    *
    */
    void HandleEntriesDeletedL( TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    /**
    *
    *
    */
    void HandleEntriesChangedL( TAny* aArg1, TAny* aArg2, TAny* aArg3 );
    
    //<cmail>    
    /**
    *
    *
    */
    void HandleMediaChangedL( TAny* aArg1, TAny* aArg2, TAny* aArg3 );
    
    /**
    *
    *
    */
    TUid MtmId() const;
    //</cmail>
    
    /**
    *
    *
    */
    TBool IsConnectionChangeL( TAny* aArg1, TAny* aArg2 );

    /**
    *
    *
    */
    TBool IsAccountNameChangeL( TAny* aArg1, TAny* aArg2 );

    /**
    * Relays connection state change events to framework.
    * It is protocol plugin's responsibility to choose correct
    * event to pass to framework.
    */

    void RelayConnectionEventL(
        TFSMailMsgId aMBoxId,
        TSSMailSyncState aNewState );


    /**
    * Appends settings observer for each mailbox
    * @param aSettingKey central repository key to observe.
    */
    void AppendSettingsObserverL(
        TFSMailMsgId aAccount,
        TUint32 aSettingKey );

    /**
    * Removes mailbox's settings observer
    * @param aMailboxId mailbox who's settings observer is to be removed.
    */
    void RemoveSettingsObserver( TFSMailMsgId aMailboxId );

    MFSMailEventObserver* MailboxObserverL(
        TFSMailMsgId aMailbox );
    void MailboxObserversL(
        TFSMailMsgId aMailbox,
        RPointerArray<MFSMailEventObserver>& aObservers );

    /**
     * Constructor
     */
    CIpsPlgEventHandler(
        CIpsPlgSosBasePlugin& aBasePlugin );

    /**
     * Constructor for leaving methods
     */
    void ConstructL( );

    /**
    * Checks for duplicate accounts
    */
    TBool AccountExistsL( const TMsvEntry& aEntry );

    /**
    * Founds correct msg entries that are directly under folder.
    * Parent is a inbox or subscribed folder in IMAP, and service
    * in POP
    */
    void FindCorrectDeletedEntryAndParentL(
        const TMsvEmailEntry& aParent,
        const CMsvEntrySelection& aDeletedIds,
        TFSMailMsgId& aFSParent,
        RArray<TFSMailMsgId>& aFSDeletedArray );


    /**
    * Saves last signalled sync status to settings
    */
    void SaveSyncStatusL( TMsvId aMailboxId, TInt aState );

    /**
    * Appends folder id to array if not already in there
    */
    void SetFolderIdToArrayL( TMsvId aNewId );

    /**
    * Checks if folder exists in the array and removes it.
    * Returns true in case of folder id,
    * otherwise we have a message deleted.
    */
    TBool MatchFolderIdFound( TMsvId aDeletedId );

    /**
    * Passes root folders to recursive function
    */
    void CollectSubscribedFoldersL( TMsvId aMailboxId );

    /**
    * Sets property event and defines property if not yet defined
    */
    TInt SetProperty( TIpsPlgPropertyEvent aEvent );

    /**
     * Signal sync started event via p&s api to all plugin instances
     */
    void SignalStartSyncL( const TFSMailMsgId& aAccount );

    /**
     * Signal sync completed event via p&s api to all plugin instances
     */
    void SignalSyncCompletedL( const TFSMailMsgId& aAccount, TInt aError );

private: // datak

    inline TFSMailMsgId SymId2FsId( TMsvId aId, TUint aMtmUid );

    inline TFSMailMsgId SymId2FsId( const TMsvEntry& aEntry );

    /**
    * Fills RArray FSMessage objects created from CMsvEntrySelection
    */
    inline void FillFSMessageArray(
            RArray<TFSMailMsgId>& aFSArray,
            const CMsvEntrySelection* aSelection,
            TUint aMtmUid );

    CRepository*                            iCenRep;
    CIpsSetDataApi*                         iSettingsApi;

    // not owned
    CMsvSession*                            iSession;

    CIpsPlgSosBasePlugin&                   iBasePlugin;

    //not owned.
    MFSMailEventObserver*                   iPluginObserver;

    RPointerArray<CMBoxEventObserver>       iMBoxObservers;

    RPointerArray<CIPSAccountInfo>          iIPSAccounts;

    RPointerArray<CIpsPlgSettingsObserver>  iIPSSettingsObservers;

    TBool                                   iIsConnected;

    CIpsPlgSettingsObserver*                iSettingsObserver;

    RPointerArray<MIpsPlgPropertyObserver>  iPropertyObservers;

    CIpsPlgPropertyWatcher*                 iPropertyWatcher;

    TPckgBuf<TIpsPlgPropertyEvent>          iPropertyBuf;

    // base plugin id
    TUint                                   iPluginId;

    RArray<TMsvId>                          iImapFolderIds;

    // state of the query user password operation
    // diffrent states are for query invoker and query handler (emailserver)
    TQueryUsrPassState                      iQueryPassState;
    // array of operations which wait for query response
    // or possibilty to perform a query
    RPointerArray<MIpsPlgConnectOpCallback> iConnOpCallbacks;
    // query user password for incoming or outgoing
    TBool                                   iIncomingPass;
    };


#endif // IPSPLGEVENTHANDLER_H

// End of file
