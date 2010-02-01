/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common mailbox object
*
*/


#ifndef CFSMAILBOX_H
#define CFSMAILBOX_H

#include "CFSMailBoxBase.h"

// forward declarations
class CFSMailPlugin;
class MFSMailBoxSearchObserver;
class MFSMailEventObserver;
class MFSMailRequestObserver;

/**
 *  mailbox data handling
 *
 *  @lib FSFWCommonLib

 */
class CFSMailBox : public CFSMailBoxBase
{
 public:
  
    /**
     * Destructor.
     */  
  	 ~CFSMailBox();

    /**
     * two based constructor
     *
     * @param aMailBoxId mailbox id in plugin containing mailbox
     */
  	 IMPORT_C static CFSMailBox* NewL( TFSMailMsgId aMailBoxId	);
  	
    /**
     * two based constructor
     *
     * @param aMailBoxId mailbox id in plugin containing mailbox
     */
  	 IMPORT_C static CFSMailBox* NewLC( TFSMailMsgId aMailBoxId );
  	
    /**
     * Enables synchronization of the mailbox. This means
     * that the framework can connect to server to do synchronization
     * of the mailbox when necessary or stay connected all the time.
     * The actual functionality depends on user settings and
     * connection implementation.
     *
     * All collections (mail etc.) supported by the connection and
     * enabled by user settings are synchronized.
     *
     * This function sets the mailbox online but it may
     * have a side effect that also other mailboxes provided
     * through the same connection are set online too.
     *
     * Connection is set offline normally by using specific function
     * for this purpose. There are also other reasons for ending up
     * in offline state like network error or scheduling.
     * 
	 */
     IMPORT_C void GoOnlineL();

    /**
     * Disables mailbox synchronization. Connection to server
     * in order to synchronize the mailbox is not allowed
     * after this.
     *
     * If GoOnlineL has effect on other mailboxes using the
     * same connection then this function has effect on those
     * mailboxes also.
     * 
	 */
     IMPORT_C void GoOfflineL();

    /**
     * Returns last synchronizing operation progress status.
     *
     * @return last sync operation progress status
     * 
	 */
     IMPORT_C TFSProgress GetLastSyncStatusL();

    /**
     * Forces synchronization of mailbox. This can be called
     * whether the mailbox is currently offline or online. In the
     * case the mailbox is offline in the beginning, connection
     * is made up for the time of the synchronization.
     *
     * All collections (mail etc.) supported by the connection and
     * enabled by user settings are synchronized.
     *     
     * This is an asynchronous operation and the request id
     * is returned for cancelling purpose.
     * 
     * User can provide an observer as a parameter,
     * which will pass progress notifications during the operation
     * as callbacks related to this operation.
     *
     * Alternatively user can use overloaded parallel implementation
     * of function without an observer function and rely only on
     * general events. Notice that the general event system is not
     * as comprehensive as the observing using dedicated observer.
     *
     * @param aOperationObserver optional observer given by user
     *
     * @return asynchronous request id
	 */
     IMPORT_C TInt RefreshNowL( MFSMailRequestObserver& aOperationObserver );

     IMPORT_C TInt RefreshNowL( );
  	
    /**
     * cancels all ongoing synchronizations in this mailbox regardless
     * of initiator, by user or by plugin itself
     *
	 */
     IMPORT_C void CancelSyncL();

    /**
     * creates into this mailbox new draft email 
     *
     * @return new draft email object, ownership is transferred to user
     */
  	 IMPORT_C CFSMailMessage* CreateMessageToSend( );

    /**
     * creates into this mailbox new draft email to be forwarded 
     *
     * @param aOriginalMessageId message id of the original message to be forwarded
     * @param aHeaderDescriptor user can give quote headers data to plugin as
     *  parameter if needed
     *
     * @return new draft email object, ownership is transferred to user
     */
  	 IMPORT_C CFSMailMessage* CreateForwardMessage( const TFSMailMsgId aOriginalMessageId,
  	 												const TDesC& aHeaderDescriptor = KNullDesC  );

    /**
     * creates into this mailbox new email for reply
     *
     * @param aOriginalMessageId id of the original message to be replied
     * @param aReplyToAll is set true if reply to all is requested
     * @param aHeaderDescriptor user can give quote headers data to plugin as
     *  parameter if needed
     *
     * @return new draft email object, ownership is transferred to user
     */
  	 IMPORT_C CFSMailMessage* CreateReplyMessage(	const TFSMailMsgId aOriginalMessageId,
  	 							 					const TBool aReplyToAll,
  	 							 					const TDesC& aHeaderDescriptor = KNullDesC  );
    /**
     * sends email
     *
     * @param aMessage message to be sent
     */
	 IMPORT_C void SendMessageL(CFSMailMessage& aMessage);

	/**
	 * List subfolders of given folder.
	 * Only direct subfolders of given folder are returned.
	 *
	 * folder data : 
	 * - folder id
	 * - folder name
	 * - folder type ; Inbox, Outbox, Sent, Drafts, Deleted, Other
	 * - message count
	 * - unread message count
	 * - mailbox id 
	 * - parent folder
	 * - subfolder count
	 *
	 * @param aFolderId defines parent folder id. TFSMailId::NullId() for root level list.
	 * @param aFolderList plugin writes results in this array owned by user
	 */
	 IMPORT_C void ListFolders( const TFSMailMsgId aFolderId, RPointerArray<CFSMailFolder>& aFolderList); 

	/**
	 * List all subfolders of mailbox.
	 *
	 * folder data : 
	 * - folder id
	 * - folder name
	 * - folder type ; Inbox, Outbox, Sent Items, Drafts, Deleted Items, Other
	 * - message count
	 * - unread message count
	 * - mailbox id 
	 * - parent folder
	 * - subfolder count
	 *
	 * @return results in this array owned by this object, caller must determine
	 *         tree structure by examining parent id of each returned folder.
	 */
	 IMPORT_C RPointerArray<CFSMailFolder>& ListFolders(  ); 

    /**
     * Standard folder id accessor.
     *
     * @param aFolderType defines folder type
     * @return standard folder id
     */
	 IMPORT_C TFSMailMsgId GetStandardFolderId( const TFSFolderType aFolderType );


    /**
     * Branding id accessor for this mailbox. This function is used by Branding Manager
     * to associate mailbox to a branding definition.
     *
     * @return branding id
     */
     IMPORT_C TDesC& GetBrandingIdL( );


    /**
     * Moves a messages between folders.
     *
     * @param aMessageIds ids of the messages to be transferred
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */    
     IMPORT_C void MoveMessagesL( const RArray<TFSMailMsgId>& aMessageIds, 
                                  const TFSMailMsgId aSourceFolderId, 
                                  const TFSMailMsgId aDestinationFolderId );

     /**
     * Moves a messages between folders. Async version.
     *                            
     * @param MFSMailRequestObserver& aOperationObserver for callback
     * @param aMessageIds ids of the messages to be transferred
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */    
	 IMPORT_C TInt MoveMessagesL( MFSMailRequestObserver& aOperationObserver,
                                  const RArray<TFSMailMsgId>& aMessageIds, 
                                  const TFSMailMsgId aSourceFolderId, 
                                  const TFSMailMsgId aDestinationFolderId );
    /**
     * Copies a messages from one folder to another folder.
     * including the properties, content, and all attachments. 
     * (note only works if the store is in an authenticated state,
     *  otherwise this function leaves with KErrNotReady)
     *
     * @param aMessageIds ids of the messages to be copied
     * @param aCopiedMessages table containing (new) ids of the copied messages
     *        owned by user
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */    
     IMPORT_C void CopyMessagesL(	const RArray<TFSMailMsgId>& aMessageIds, 
							  		RArray<TFSMailMsgId>& aCopiedMessages,
                              		const TFSMailMsgId aSourceFolderId, 
                              		const TFSMailMsgId aDestinationFolderId );
                              		
    /**
     * Asyncronous call for starting search for given string. Only one search can be
     * performed at a time.
     *
     * This function will search for message's containing the given search string.
     * The search will be performed on the all message fields: To, Cc, Bcc, subject, body.
     * The search client will be notified of each found message,
     * and upon completion of the search.  Only one search can be performed at a time.  
     *  
     * To change the sort order in the search result, use the same search string in the
     * but change the aSortCriteria parameter.  The store "caches" the search
     * results generated by the same search string.
     *    
     * The function will leave with KErrInUse if a search is already in progress.
	 *
     * note only works if the store is in an authenticated state,
     * otherwise this function leaves with KErrNotReady
     *
     * @param aSearchStrings text strings that will be searched from different message fields.
	 * @param aSortCriteria sort criteria for the results
	 * @param aSortCriteria sort criteria for the results
     * @param aFolderIds user can give list of folders to be searched
     */    
	 IMPORT_C void SearchL( const RPointerArray<TDesC>& aSearchStrings,
						    const TFSMailSortCriteria& aSortCriteria,
					 	    MFSMailBoxSearchObserver& aSearchObserver );

	 IMPORT_C void SearchL( const RPointerArray<TDesC>& aSearchStrings,
						    const TFSMailSortCriteria& aSortCriteria,
					 	    MFSMailBoxSearchObserver& aSearchObserver,
                            RArray<TFSMailMsgId> aFolderIds );
    /**
     * Cancels current search. Does nothing if there is not any search ongoing.
     * The search client will not be called back after this function is called.
	 *
     * note CancelSearch() method does NOT clear the search result cached in the store. 
     *       A different sort order can be used for the same search string, and the 
     *       cache will be utilized.  Only by using a different search string can the
     *       cache be cleaned.
     */    
	 IMPORT_C void CancelSearch();

    /**
     * Inform the store to clean up its cache for search results.
     *        
     *  This method cancels the the ongoing search (if exists), and then clean ups store's cache.
     * 
     *  This function should be called by the UI when search results are no longer in display.
     */
     IMPORT_C void ClearSearchResultCache();

	/**
	 * DEPRECATED
	 *
     * @param aFolderId...
     * @param aObserver observer
     */    
	 IMPORT_C void AddObserver(const TFSMailMsgId aFolderId, MFSMailEventObserver& aObserver);

    /** 
     *  Get a list of most recently used addresses of this mailbox.
     *  Each MRU entry consists of two descriptors placed sequentially
     *  in the array. First one is the display name and the second
     *  one is the actual email address. If for example some entry does
     *  not contain a name at all then a KNullDesC is found in place of
     *  the name.
     *        
     * @return Array of descriptors containing MRUs. Ownership
     *         is transferred. Empty if no entries are found.
     */
     IMPORT_C MDesCArray* ListMrusL() const;

    /**
     * Returns the current synchronizing state of this mailbox.
     *
     * @return mailbox synchronizing state data structure
     */
     IMPORT_C TSSMailSyncState CurrentSyncState() const;

    /**
     * mailbox capability check for user
     *
     * @param aCapa mailbox capability to be checked
     */
	 IMPORT_C TBool HasCapability(const TFSMailBoxCapabilities aCapability) const; 

    /**
     * mailbox connection status accessor
     *
     * @return connection status, online / offline
     */
	 IMPORT_C TFSMailBoxStatus GetMailBoxStatus( );

    /**
     * mailbox authentication data mutator
     *
     * @param aUsername account user name
     * @param aPassword account password
     */
     IMPORT_C void SetCredentialsL( const TDesC& aUsername, const TDesC& aPassword );

     /**
      * removes downloaded attachments from local/terminal memory 
      */
     IMPORT_C void RemoveDownLoadedAttachmentsL();
	 
     /**
      * reads connection id from plugin
      *
      * @param  aConnectionId reference to connection id
      * @return KErrNone or error code
      */
     IMPORT_C TInt GetConnectionId( TUint32& aConnectionId );

     /**
      * checks from plugin if connection is allowed when roaming
      *
      * @param  aConnectionAllowed 
      * @return KErrNone or error code
      */
     IMPORT_C TInt IsConnectionAllowedWhenRoaming( TBool& aConnectionAllowed) ;

     /**
      * creates email from RFC822 format data stream located
      * in given attachment file
      *
      * @param  aFile access to file containing data stream contents
      *         given by user
      *
      * return new email object, ownership is transferred to user
      */
      IMPORT_C CFSMailMessage* CreateMessageFromFileL( const RFile& aFile );

public: // from  CExtendableEmail

     /**
     * @see CExtendableEmail::ReleaseExtension()
     */
     IMPORT_C void ReleaseExtension( CEmailExtension* aExtension );

     /**
      * Returns extension by uid, leaves KErrNotSupported if extension is
      * not available.
      * @param aInterfaceUid extension interface uid
      * @return extension pointer. Ownership depends on extension.
      */
     IMPORT_C CEmailExtension* ExtensionL( const TUid& aInterfaceUid );

private:

  	/**
     * Two-phased constructor
     */
  	 void ConstructL( TFSMailMsgId aMailBoxId );

    /**
     * C++ default constructor.
     */
  	 CFSMailBox();

    /**
     * Function used internally to inform that Mru list related to this
     * mailbox should now be updated using the given recipient info. In
     * other words this function makes sure that the given recipients are
     * found from the mru list in which the mrus of this mailbox are stored.
     * Mrus are stored in the plugin.
     *
     * @param aRecipients email recipients in TO-field
     * @param aCCRecipients email recipients in CC-field
     * @param aBCCRecipients email recipients in BCC-field
     */
    void UpdateMrusL(
        const RPointerArray<CFSMailAddress>& aRecipients,
        const RPointerArray<CFSMailAddress>& aCCRecipients,
        const RPointerArray<CFSMailAddress>& aBCCRecipients ) const;
    
    /**
     * Function used to copy array contents to another array.
     *
     * @param aArrayToBeCopied Array that should be copied. 
     * @return Copy of the given array.
     */
    CDesCArraySeg* CopyArrayL( MDesCArray& aArrayToBeCopied ) const;


    /**
     * Function used to update given mru list with
     * recently used address info. If this info is already found
     * from the mru list then it is moved to the end of the mru list.
     * This way the most recent ones are found from the end
     * of the list.
     *
     * @param aMruList Mru list that should be updated.
     * @param aNewRecentlyUsedOnes Addresses that should be used
     *                             to update the given mru list.
     */
    void UpdateMruListL(
        CDesCArraySeg& aMruList,
        const RPointerArray<CFSMailAddress>& aNewRecentlyUsedOnes ) const;


    /**
     * Function used to search an address from given mru list.
     * Fucntion goes through the addresses in the mru list
     * and returns the first one matching the given address.
     *
     * @param aMruList Mru list that is searched through.
     * @param searchedAddress Address that is searched from the
     *                        given mru list.
     * @param aPos Reference parameter used to return the position
     *             of a matching address if such was found.
     * @return Integer value to indicate whether the searched address
     *         was found or not. Zero is returned if the address was found.
     *         Otherwise 1 is returned.
     */
    TInt FindAddressFromMruList( CDesCArraySeg& aMruList,
                             TDesC& searchedAddress,
                             TInt& aPos ) const;

    /**
     * Removes the oldest entry from the mru list and adds
     * given address info to the mru list. 
     *
     * @param aMruList Mru list.
     * @param aToBeAdded Address info to be added into the mru list.
     */
    void AddAndRemoveExcessMruL( CDesCArraySeg& aMruList,
                                 CFSMailAddress& aToBeAdded ) const;

    /**
     * Function to append mru entry to the mru list. This implementation
     * adds display name and then the address in this order into the
     * mru list.
     *
     * @param aMruList Mru list.
     * @param aToBeAdded Address info to be added into the mru list.
     */
    void AppendMruItemL( CDesCArraySeg& aMruList,
                         CFSMailAddress& aToBeAppended ) const;

    /**
     * Function to set the already existing entry in the mru list
     * as the most recent one.
     *
     * @param aMruList Mru list that should be edited.
     * @param aPosition Position where from the address element of
     *                  the most recent address is found. The display
     *                  name of the address is found with index one less
     *                  from the mru list.
     * @param aMostRecent After removal of the mru entry with given index,
     *                    this entry is added into the mru list as the
     *                    most recently used one.
     */
    void SetAsMostRecentMruL( CDesCArraySeg& aMruList,
                              TInt aPosition,
                              CFSMailAddress& aMostRecent ) const;

 private: // data
 
    /**
     * request handler for plugin requests
     */
	 CFSMailRequestHandler* 	iRequestHandler;	

	/**
     * mailbox folder list
     */
	 RPointerArray<CFSMailFolder> iFolders;

};

#endif
