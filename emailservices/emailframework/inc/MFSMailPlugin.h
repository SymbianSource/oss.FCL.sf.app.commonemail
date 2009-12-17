/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines Framework Plugin API functions.
*
*/


#ifndef M_FSMAILPLUGINAPI_H
#define M_FSMAILPLUGINAPI_H

//  INCLUDES
//<cmail>
#include "mfsmailrequestobserver.h"
#include "mfsmaileventobserver.h"
#include "cfsmailbox.h"
//</cmail>

// FORWARD DECLARATIONS
class MDesCArray;

// CLASS DECLARATION

/**
 *  Freestyle email plugin API abstract interface.
 *
 *  Plugin is identified using ecom implementation uid.
 *  Framework searches existing plugins and creates plugin api instance per each plugin.
 *
 *  plugin API provides methods to fetch from plugins service, mailbox,
 *  folder, message and attachment objetcs,
 *  which further contain more detailed data of each object to be read.
 *
 *  When updating data Freestyle framework manipulates corresponding
 *  service, mailbox, folder, message and attachment objetcs and
 *  after manipulation stores manipulated object to plugin by calling
 *  corresponding store method.
 *
 *	Asynchronous requests -> framework gives request id to every async request,
 *  which plugin gives to framework when calling back observer to be matched
 *  request data in framework. Framework gives observer pointer in async request
 *  to be called back.
 *
 *  API allows Symbian leaves from plugins and will trap them, error codes are
 *  expected to be symbian error codes.
 *  @since ...
 */
class MFSMailPlugin
  	{
public: // Data

public: // Methods

	// PLUGIN HANDLING
    /**
     * returns plugin capabilities
     *
     * @return plugin capabilities data structure
     */
  	 virtual const TFSMailPluginCapabilities ReadPluginCapabilitiesL() = 0;

	// MAILBOX HANDLING
    /**
     * lists existing mailboxes implemented by given plugin
     *
     * @param aPlugin defines which plugin request is accessed to
     * @param aMailBoxes plugin writes results to this array
     * owned by framework
     */
  	 virtual void ListMailBoxesL( RArray<TFSMailMsgId>& aMailboxes) = 0;

    /**
     * returns email mailbox object related to given mailbox id
     * mailbox data set by plugin :
     *				 - mailbox id
     *               - mailbox name
     *               - branding id
     *               - mailbox capabilities
     *
     * @param aMailBoxId defines mailbox id
     * @return mailBox object ( CFSMailBox ) to be owned by user
     */
	 virtual CFSMailBox* GetMailBoxByUidL( const TFSMailMsgId& aMailBoxId) = 0;

    /**
     * removes mailbox
     *
     * @param aMailBoxId defines id of the mailbox to be removed
     */
	 virtual void DeleteMailBoxByUidL(const TFSMailMsgId& aMailBoxId) = 0;

    /**
     * Returns branding id for this mailbox. This function is used by Branding Manager
     * to associate mailbox to a branding definition. Plugin implementation should
     * return incoming mail server associated with this mailbox.
     *
     * @param aMailBoxId defines the target mailbox
     * @return branding id
     */
     virtual TDesC& GetBrandingIdL(const TFSMailMsgId& aMailboxId) = 0;

    /**
     * Moves a messages to the given folder.
     *
     * @param aMailBoxId defines mailbox id
     * @param aMessageIds ids of the messages to be transferred
     * @param aSourceFolderId id of the folder where messages are located
     * @param aDestinationFolderId destination folder id
     *
     */
     virtual void MoveMessagesL( 	const TFSMailMsgId& aMailBoxId,
     							 	const RArray<TFSMailMsgId>& aMessageIds,
                                	const TFSMailMsgId& aSourceFolderId,
                                	const TFSMailMsgId& aDestinationFolderId ) = 0;
    /**
     * Copies a messages from one folder to another.
     * This function copies a messages to the given folder,
     * including the properties, content, and all attachments.
     * (note Only works if the store is in an authenticated state,
     *  otherwise this function leaves with KErrNotReady)
     *
     * @param aMailBoxId defines mailbox id
     * @param aMessageIds ids of the messages to be copied
     * @param aNewMessages ids of the copied messages.
     * @param aSourceFolderId id of the folder where messages are located
     * @param aDestinationFolderId destination folder id
     */
     virtual void CopyMessagesL( const TFSMailMsgId& aMailBoxId,
     							 const RArray<TFSMailMsgId>& aMessageIds,
     							 RArray<TFSMailMsgId>& aNewMessages,
                                 const TFSMailMsgId& aSourceFolderId,
                                 const TFSMailMsgId& aDestinationFolderId ) = 0;

    /** Get a list of most recently used addresses of desired mailbox.
     *  Each MRU entry consists of two descriptors placed sequentially
     *  in the array. First one is the display name and the second
     *  one is the actual email address. If for example some entry does
     *  not contain a name at all then a KNullDesC is found in place of
     *  the name.
     *
     *  The function will leave with KErrNotFound if a mailbox with
     *  given id is not found.
     *
     * @param aMailBoxId defines mailbox id
     * @return Array of descriptors containing MRUs. Ownership
     *         is transferred. Empty if no entries are found.
     */
     virtual MDesCArray* GetMrusL( const TFSMailMsgId& aMailBoxId ) = 0;

	 // old one for building purposes
     virtual MDesCArray* GetMrusL( TFSMailMsgId aMailBoxId ) const= 0;

    /** Set a list of most recently used addresses for given mailbox.
     *  Each MRU entry consists of two descriptors placed sequentially
     *  in the array. First one is the display name and the second
     *  one is the actual email address. If for example some entry does
     *  not contain a name at all then a KNullDesC is found in place of
     *  the name.
     *
     *  The function will leave with KErrNotFound if a mailbox with
     *  given id is not found.
     *
     * @param aMailBoxId defines mailbox id
     * @param aNewMruList Array of descriptors containing MRUs. Empty if no entries are found.
     */
     virtual void SetMrusL( const TFSMailMsgId& aMailBoxId,
     						MDesCArray* aNewMruList ) = 0;

	// MAILBOX SYNC
    /**
     * DEPRECATED
     *
	 */
     virtual void GoOnlineL( ) = 0;

    /**
     * DEPRECATED
     *
	 */
     virtual void GoOfflineL( ) = 0;

    /**
     * DEPRECATED
     *
	 */
     virtual  TFSProgress GetLastSyncStatusL( ) = 0;

    /**
     * DEPRECATED
     *
	 */
     virtual TInt RefreshNowL( ) = 0;

    /**
     * Enables synchronization of identified mailbox. This means
     * that the plugin can connect toserver to do synchronization
     * of the mailbox when necessary or stay connected all the time.
     * The actual functionality depends on user settings and plugin
     * implementation.
     *
     * All collections (mail etc.) supported by the plugin and
     * enabled by user settings are synchronized.
     *
     * This function sets identified mailbox online but it may
     * have a side effect that also other mailboxes of the same
     * plugin are set online.
     *
     * Connection is set offline normally by using specific function
     * for this purpose. There are also other reasons for ending up
     * in offline state like network error or scheduling.
     *
     * @param aMailBoxId defines mailbox
     *
	 */
     virtual void GoOnlineL( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Disables mailbox synchronization. Connection by the plugin
     * to synchronize identified mailbox is not allowed after this.
     *
     * If GoOnlineL has effect on other mailboxes of the
     * same plugin then this function has effect on those
     * mailboxes also.
     *
     * @param aMailBoxId defines mailbox
     *
	 */
     virtual void GoOfflineL( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Returns last sync operation status.
     *
     * @param aMailBoxId defines mailbox
     * @return Last sync operation status
     *
	 */
     virtual const TFSProgress GetLastSyncStatusL( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Forces synchronization of mailbox. This can be called
     * whether the mailbox is currently offline or online. In
     * case the mailbox is offline in the beginning, connection
     * is made up for the time of the synchronization.
     *
     * All collections (mail etc.) supported by the plugin and
     * enabled by user settings are synchronized.
     *
     * This is an asynchronous operation and the request id
     * is returned for cancelling purpose.
     *
     * Observer is given as a parameter to enable callbacks
     * related to this operation. Alternatively user can use
     * empty implementation of the observer function and rely
     * only on events. Notice that the event system is not as
     * comprehensive as the observing way do this.
     *
     * @param aMailBoxId defines mailbox
     * @param aOperationObserver is client provided observer that
     *        will receive progress notifications during the
     *        operation.
     * @param aRequestId identifies asynchronous request
	 */
     virtual void RefreshNowL(	const TFSMailMsgId& aMailBoxId,
         						MFSMailRequestObserver& aOperationObserver,
         						TInt aRequestId ) = 0;

	// FOLDER HANDLING
    /**
     * returns email folder object related to given folder id
     * folder data set by plugin :
     *				 - folder id
     *               - folder name
     *				 - folder type ; Inbox, Outbox, Sent, Drafts, Deleted, Other
     *               - message count
     *               - unread message count
     *               - mailbox id
	 *				 - parent folder
     * @param aMailBoxId defines mailbox id
     * @param aFolderId defines folder id
     * @return folder object (CFSMailFolder) to be owned by user
     */
	 virtual CFSMailFolder* GetFolderByUidL( const TFSMailMsgId& aMailBoxId,
	 										 const TFSMailMsgId& aFolderId ) = 0;

    /**
     * creates new folder
     *
     * @param aMailBoxId defines mailbox where folder is created
     * @param aFolderId defines id of the parent folder
     * @param aFolderName defines name of the new folder
     * @param aSync defines if new folder is local only (false)
     * or if it also should sync to server (true)
     * @return new folder object CFSMailFolder to be owned by user
     */
	 virtual CFSMailFolder* CreateFolderL( 	const TFSMailMsgId& aMailBoxId,
	 										const TFSMailMsgId& aParentFolderId,
	 									   	const TDesC& aFolderName,
	 									   	const TBool aSync ) = 0;

    /**
     * deletes email folder related to given folder id
     *
     * @param aMailBoxId defines mailbox where folder is deleted
     * @param aFolderId defines id of folder to be deleted
     */
	 virtual void DeleteFolderByUidL(	const TFSMailMsgId& aMailBoxId,
	 									const TFSMailMsgId& aFolderId) = 0;

	/**
	 * List subfolders of given folder. Only direct subfolders of given folder are returned.
	 * Folder data :
	 * - folder id
	 * - folder name
	 * - folder type ; Inbox, Outbox, Sent, Drafts, Deleted, Other
	 * - message count
	 * - unread message count
	 * - mailbox id
	 * - parent folder
	 * - subfolder count
	 *
	 * @param aMailBoxId defines mailbox where parent folder is
	 * @param aFolderId defines parent folder id. TFSMailId::NullId() for root level list.
	 * @param aFolderList plugin writes results in this array given by user
	 */
	virtual void ListFoldersL(	const TFSMailMsgId& aMailBoxId,
	    						const TFSMailMsgId& aParentFolderId,
	    						RPointerArray<CFSMailFolder>& aFolderList) = 0;

	/**
	 * List all subfolders of given mailbox.
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
	 * @param aMailBoxId defines mailbox
	 * @param aFolderList plugin writes results in this array given by user.
	 * Caller must determine tree structure by examining parent id of each returned folder.
	 */
	 virtual void ListFoldersL(	const TFSMailMsgId& aMailBoxId,
	    						RPointerArray<CFSMailFolder>& aFolderList) = 0;
    /**
     * return folder id of given folder type
     *
     * @param aMailBoxId defines mailbox where folder is
     * @param aFolderType folder type
     *
     * return folder id
     */
	 virtual TFSMailMsgId GetStandardFolderIdL( const TFSMailMsgId& aMailBoxId,
	 											const TFSFolderType aFolderType ) = 0;

	// MESSAGE FETCH AND STORE

    /**
     * Get message iterator for given folder
     *
     * @param aMailBoxId defines mailbox where parent folder is
     * @param aFolderId defines folder id
     * @param aDetails describes details requested to CFSMailMessage objects returned by the iterator
     * @param aSorting describes requested sort criteria. First item in array is primary sort criteria.
     * @return iterator (remember virtual destructor)
	 */

     virtual MFSMailIterator* ListMessagesL( const TFSMailMsgId& aMailBoxId,
        									 const TFSMailMsgId& aFolderId,
        									 const TFSMailDetails aDetails,
        									 const RArray<TFSMailSortCriteria>& aSorting ) = 0;

    /**
     * returns email message object related to given message id
     *
     * message info set by plugin :
     *				  - message id
     *                - mailbox
     *				  - parent folder
     *                - message size
	 *                - the rest is defined by message attributes ;
     * @param aMailBoxId defines mailbox where message is
     * @param aFolderId defines parent folder id where message is
     * @param aMessageId defines message id
     *
     * @param aDetails defines which message parts are included in message
	 * EFSMsgDataIdOnly  -> Doesn't get any data. The object just contains the ID.
	 * EFSMsgDataDate    -> Get received date only. To be used when showing messages sorted by date.
     *                      The rest would be retreived when needed but date is needed to build the list.
	 * EFSMsgDataSubject -> Like above but when sorting by subject
	 * EFSMsgDataSender  -> Likewise for sender address.
	 * EFSMsgDataEnvelope -> Date, subject, Sender, To, Cc at least.
	 * EFSMsgDataStructure -> the part structure including mime type, size and name headers.
     *
     * @return message data (CFSMailMessage), ownership is transferred to user
     */
	 virtual CFSMailMessage* GetMessageByUidL( 	const TFSMailMsgId& aMailBoxId,
	 											const TFSMailMsgId& aParentFolderId,
	 											const TFSMailMsgId& aMessageId,
	 											const TFSMailDetails aDetails) = 0;

    /**
     * deletes email message defined by message id
     *
     * @param aMailBoxId defines mailbox
     * @param aFolderId defines message parent folder id
     * @param aMessages defines ids of messages to be deleted
     */
	 virtual void DeleteMessagesByUidL( const TFSMailMsgId& aMailBoxId,
	 									const TFSMailMsgId& aFolderId,
	 									const RArray<TFSMailMsgId>& aMessages ) = 0;

    /**
     * creates new message template into drafts folder
     *
     * @param aMailBoxId defines mailbox where message is created
     * @return message object to be modified by user (ownership is transferred to user)
     */
	 virtual CFSMailMessage* CreateMessageToSendL( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * creates new message template to drafts folder to be forwarded
     *
     * @param aMailBoxId defines mailbox where message is created
     * @param aOriginalMessageId defines original message,which is forwarded
     * @return message object to be modified by user (ownership is transferred to user)
     */
	 virtual CFSMailMessage* CreateForwardMessageL( const TFSMailMsgId& aMailBoxId,
	 												const TFSMailMsgId& aOriginalMessageId ) = 0;

    /**
     * creates new reply message template to drafts folder
     * @param aMailBoxId defines mailbox where message is created
     * @param aOriginalMessageId defines id of original message,which is replied to
     * @param aReplyToAll
     * @return message object to be modified by user (ownership is transferred to user)
     */
	 virtual CFSMailMessage* CreateReplyMessageL( const TFSMailMsgId& aMailBoxId,
	 											  const TFSMailMsgId& aOriginalMessageId,
	 											  const TBool aReplyToAll ) = 0;

    /**
     * stores message data to message store after modifications (commit)
     *
     * @param aMailBoxId defines mailbox where message is stored
     * @param aMessage message data to be stored by plugin
     */
	 virtual void StoreMessageL( const TFSMailMsgId& aMailBoxId,
	 							 CFSMailMessage& aMessage ) = 0;

    /**
     * starts asynchronous message fetching done by plugin
	 *
     * @param aMailBoxId defines mailbox where message is located
     * @param aFolderId defines parent folder where message is located
     * @param aMessageIds defines ids of messages to be fetched
     *
     * @param aDetails defines which message parts are included in message
	 * EFSMsgDataIdOnly  -> Doesn't get any data. The object just contains the ID.
	 * EFSMsgDataDate    -> Get received date only. To be used when showing messages sorted by date.
     *                      The rest would be retreived when needed but date is needed to build the list.
	 * EFSMsgDataSubject -> Like above but when sorting by subject
	 * EFSMsgDataSender  -> Likewise for sender address.
	 * EFSMsgDataEnvelope -> Date, subject, Sender, To, Cc at least.
	 * EFSMsgDataStructure -> the part structure including mime type, size and name headers.
	 *
     * @param aObserver defines callback medhod, which plugin calls after
     *        async request has completed
     * @param aRequestId identifies asynchronous request
     */
     virtual void FetchMessagesL(	const TFSMailMsgId& aMailBoxId,
     							 	const TFSMailMsgId& aParentFolderId,
     							 	const RArray<TFSMailMsgId>& aMessageIds,
     							 	TFSMailDetails aDetails,
     							 	MFSMailRequestObserver& aObserver,
     							 	TInt aRequestId) = 0;

    /**
     * DEPRECATED
     * Fetches message part contents from server asynchronously.
     *
     * @param aMessagePartIds define message part ids of message parts that are asked to be fetched
     * @param aOperationObserver is FW provided observer that should be used to report progress notifications during the operation.
     * @param aRequestId identifies asynchronous request
     */
     virtual void FetchMessagesPartsL( 	const RArray<TFSMailMsgId>& aMessagePartIds,
     									MFSMailRequestObserver& aOperationObserver,
										const TInt aRequestId) = 0;

    /**
     * Fetches message part contents from server asynchronously.
     *
     * @param aMessagePartIds define message part ids of message parts that are asked to be fetched
     * @param aOperationObserver is FW provided observer that should be used to report
     *  progress notifications during the operation.
     * @param aRequestId identifies asynchronous request
     * @param aPreferredByteCount indicates how much more content for part(s) UI wants to fetch.
	 * Default value zero indicates that all content should be fetched.
	 * Actual amount of data fetched may differ from requested (possibly all fetched in any case).
     */
     virtual void FetchMessagePartsL( 	const RArray<TFSMailMsgId>& aMessagePartIds,
        								MFSMailRequestObserver& aOperationObserver,
										const TInt aRequestId,
        								const TUint aPreferredByteCount) = 0;


    /* synchronous message accessor
	 *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageIds defines ids of messages to be read from store
 	 * @param aMessageList plugin writes results into this
     *
     * @param aDetails defines which message parts are included in message
	 * EFSMsgDataIdOnly  -> Doesn't get any data. The object just contains the ID.
	 * EFSMsgDataDate    -> Get received date only. To be used when showing messages sorted by date.
     *                      The rest would be retreived when needed but date is needed to build the list.
	 * EFSMsgDataSubject -> Like above but when sorting by subject
	 * EFSMsgDataSender  -> Likewise for sender address.
	 * EFSMsgDataEnvelope -> Date, subject, Sender, To, Cc at least.
	 * EFSMsgDataStructure -> the part structure including mime type, size and name headers.
 	 *
	 */
     virtual void GetMessagesL(	const TFSMailMsgId& aMailBoxId,
     							const TFSMailMsgId& aParentFolderId,
     							const RArray<TFSMailMsgId>& aMessageIds,
     							RPointerArray<CFSMailMessage>& aMessageList,
     							const TFSMailDetails aDetails ) = 0;

    /**
     * Returns child part objects for given message part. Ownership of objects
     * is transferred to caller.
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aParentId gives the id of the parent message part
     * @param aParts list of child parts of given parent message part
     */
     virtual void ChildPartsL( 	const TFSMailMsgId& aMailBoxId,
     							const TFSMailMsgId& aParentFolderId,
								const TFSMailMsgId& aMessageId,
     						   	const TFSMailMsgId& aParentId,
     						 	RPointerArray<CFSMailMessagePart>& aParts) = 0;

    /**
     * Creates and adds a new child part to this part.
     * If aInsertBefore is NULL id then new part is added as last.
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aParentPartId parent of the new part
     * @param aInsertBefore id of existing part that new part should precede.
     * If NULL id then new part is added as last.
     */
     virtual CFSMailMessagePart* NewChildPartL( const TFSMailMsgId& aMailBoxId,
				     							const TFSMailMsgId& aParentFolderId,
												const TFSMailMsgId& aMessageId,
     											const TFSMailMsgId& aParentPartId,
        										const TFSMailMsgId& aInsertBefore,
												const TDesC& aContentType) = 0;

    /**
     * Copies given message as new child part to this part.
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aParentPartId parent of the new part
     * @param aInsertBefore id of existing part that new part should precede.
     * If NULL id then new part is added as last.
     * @param aMessage specifies the message that is copied
     */
     virtual CFSMailMessagePart* CopyMessageAsChildPartL(const TFSMailMsgId& aMailBoxId,
						     							 const TFSMailMsgId& aParentFolderId,
														 const TFSMailMsgId& aMessageId,
        												 const TFSMailMsgId& aParentPartId,
        												 const TFSMailMsgId& aInsertBefore,
        												 const CFSMailMessage& aMessage) = 0;

    /**
     * Removes child part (and its children, if any) from this part
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aParentPartId parent of the part
     * @param aPartId part to remove
     */
     virtual void RemoveChildPartL(	const TFSMailMsgId& aMailBoxId,
	     							const TFSMailMsgId& aParentFolderId,
								 	const TFSMailMsgId& aMessageId,
        							const TFSMailMsgId& aParentPartId,
        							const TFSMailMsgId& aPartId) = 0;

    /**
     * Returns given message part. Ownership of object is transferred to caller.
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aMessagePartId gives the id of the message part
     */
     virtual CFSMailMessagePart* MessagePartL(  const TFSMailMsgId& aMailBoxId,
			         							const TFSMailMsgId& aParentFolderId,
 												const TFSMailMsgId& aMessageId,
     											const TFSMailMsgId& aMessagePartId) = 0;

    /**
     *
     * Retrieves a read-only file handle for the content file of this message part.
     * Should return KErrNotSupported if handle can not be given directly. In that case
     * FW will next ask to copy file to a temporary location so that FW can open the RFile
     * itself. Ownership of the handle is transferred. Caller is responsible for closing the
     * handle.
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aMessagePartId gives the id of the message part
     * @param aFileHandle returns the opened read-only file handle
     */
     virtual TInt GetMessagePartFileL(  const TFSMailMsgId& aMailBoxId,
		      							const TFSMailMsgId& aParentFolderId,
    									const TFSMailMsgId& aMessageId,
     									const TFSMailMsgId& aMessagePartId,
     									RFile& aFileHandle) = 0;

    /**
     * copies message part to given location
     *
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aMessagePartId gives the id of the message part
     * @param aFilePath message part location
     */
     virtual void CopyMessagePartFileL( const TFSMailMsgId& aMailBoxId,
     	     							const TFSMailMsgId& aParentFolderId,
										const TFSMailMsgId& aMessageId,
     									const TFSMailMsgId& aMessagePartId,
     									const TDesC& aFilePath) = 0;


    /**
     * gets message part content to given buffer
     *
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aMessagePartId gives the id of the message part
     * @param aBuffer
     * @param aStartOffset
     */
	 virtual void GetContentToBufferL(	const TFSMailMsgId& aMailBoxId,
	 	     							const TFSMailMsgId& aParentFolderId,
								 		const TFSMailMsgId& aMessageId,
	 							 		const TFSMailMsgId& aMessagePartId,
	 							 		TDes& aBuffer,
	 							 		const TUint aStartOffset) = 0;

    /**
     * stores message part content
     *
     *
     * @param aBuffer message part content
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aMessagePartId gives the id of the message part
     */
	 virtual void SetContentL(	const TDesC& aBuffer,
	 						  	const TFSMailMsgId& aMailBoxId,
     							const TFSMailMsgId& aParentFolderId,
								const TFSMailMsgId& aMessageId,
	 							const TFSMailMsgId& aMessagePartId) = 0;

    /**
     * Removes fetched contents of these parts.
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aPartIds Array of message part ids.
     */
     virtual void RemovePartContentL( 	const TFSMailMsgId& aMailBoxId,
     	     							const TFSMailMsgId& aParentFolderId,
									  	const TFSMailMsgId& aMessageId,
     								  	const RArray<TFSMailMsgId>& aPartIds) = 0;

    /**
     * Sets contents from given file. Possible old contents are replaced.
     *
     *
     * @param aMailBoxId defines mailbox where message is located
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aMessagePartId gives the id of the message part
     * @param aFilePath Path to source file
     */
     virtual void SetPartContentFromFileL(	const TFSMailMsgId& aMailBoxId,
			     							const TFSMailMsgId& aParentFolderId,
		 									const TFSMailMsgId& aMessageId,
         									const TFSMailMsgId& aMessagePartId,
         									const TDesC& aFilePath) = 0;
    /**
     * stores message part data to message store after modifications (commit)
     *
     * @param aMailBoxId defines mailbox where message part is stored
     * @param aParentFolderId defines parent folder where message is located
     * @param aMessageId gives the id of the message that message part belongs to
     * @param aMessagePart message part data to be stored by plugin
     */
     virtual void StoreMessagePartL( const TFSMailMsgId& aMailBoxId,
   	     							 const TFSMailMsgId& aParentFolderId,
									 const TFSMailMsgId& aMessageId,
         							 CFSMailMessagePart& aMessagePart) = 0;

    /**
     * unregisters plugin request observer to cancel pending events
     *
     * @param aRequestId request id of the pending asynchronous request
     */
	 virtual void UnregisterRequestObserver(TInt aRequestId) = 0;

    // MESSAGE SENDING
    /**
     * DEPRECATED
     *
     * starts message sending done by plugin
     * cancellation is deleting the message from outbox
     *
     * @param aMessageId defines message id to be sent
     */
     virtual void SendL( TFSMailMsgId aMessageId ) = 0;

    /**
     * starts message sending done by plugin
     * cancellation is deleting the message from outbox
     *
     * @param aMessage message to be sent
     */
     virtual void SendMessageL( CFSMailMessage& aMessage ) = 0;

    /**
     * method to require pending asynchronous request status
	 *
     * @param aRequestId identifies request if parallel requests exists
     * @return pending request status
     */
     virtual TFSProgress StatusL( const TInt aRequestId ) = 0;

    /**
     * method for canceling pending request
     * @param aRequestId identifies request if parallel requests exists
	 */
     virtual void CancelL( const TInt aRequestId ) = 0;

	// Search API
 	// OPEN: IDS plugin nippets?

    /**
     * Asyncronous call for starting search for given string. Only one search can be
     * performed at a time.
     *
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
     * /note Only works if the store is in an authenticated state,
     *  otherwise this function leaves with KErrNotReady
     *
     * @paran aMailBoxId mailbox where messages are to be searched
     * @param aFolderIds list of folders where messages are to be searched
     * @param aSearchStrings text strings that will be searched from different message fields.
	 * @param aSortCriteria sort criteria for the results
     * @param aSearchObserver client observer that will be notified about search status.
     *
     */
	virtual void SearchL( const TFSMailMsgId& aMailBoxId,
						  const RArray<TFSMailMsgId>& aFolderIds,
						  const RPointerArray<TDesC>& aSearchStrings,
						  const TFSMailSortCriteria& aSortCriteria,
					      MFSMailBoxSearchObserver& aSearchObserver ) = 0;


    /**
     * Cancels current search. Does nothing if there is not any search.
     * The search client will not be called back after this function is called.
	 *
     * \note CancelSearch() method does NOT clear the search result cached in the store.
     *       A different sort order can be used for the same search string, and the
     *       cache will be utilized.  Only by using a different search string can the
     *       cache be cleaned.
	 *
     * @paran aMailBoxId mailbox where the search should be cancelled
	 *
     */
	virtual void CancelSearch( const TFSMailMsgId& aMailBoxId ) = 0;

    /** Inform the store to clean up its cache for search results.
     *
     *  This method cancels the the ongoing search (if exists), and then clean ups store's cache.
     *
     *  This function should be called when search results are no longer in display.
	 *
     * @paran aMailBoxId mailbox where the search cache should be cleared
	 *
    */
    virtual void ClearSearchResultCache( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Adds a new event observer. There can be several observers active at the same time.
     * Caller MUST call RemoveObserver before destroying given observer object.
     *
     * @param aObserver observer implementation
     */
    virtual void AddObserverL(MFSMailEventObserver& aObserver) = 0;

    /**
     * Removes observer
     *
     * @param aObserver observer implementation
     */
    virtual void RemoveObserver(MFSMailEventObserver& aObserver) = 0;

    /**
     * Subscribes events from given mailbox.
     * UnsubscribeMailboxEvents MUST be called before destroying given observer object.
     *
     * @param aMailboxId id of target mailbox
     * @param aObserver observer implementation
     */
    virtual void SubscribeMailboxEventsL(const TFSMailMsgId& aMailboxId,
    									 MFSMailEventObserver& aObserver) = 0;

    /**
     * Unsubscribes events from given mailbox
     *
     * @param aMailboxId id of target mailbox
     * @param aObserver observer implementation
     */
    virtual void UnsubscribeMailboxEvents(const TFSMailMsgId& aMailboxId,
    									  MFSMailEventObserver& aObserver) = 0;

    /**
     * Returns the current sync state for this mailbox.
     *
     * @param aMailboxId id of target mailbox
     */
    virtual TSSMailSyncState CurrentSyncState(const TFSMailMsgId& aMailboxId) = 0;

    /**
     * Sets the mailbox name for this mailbox.
     *
     * @param aMailboxId id of target mailbox
     * @param aMailboxName new name for the mailbox
     */
    virtual void SetMailboxName(const TFSMailMsgId& aMailboxId, const TDesC& aMailboxName );

  	};


#endif  // M_FSMAILPLUGIN_H

// End of File
