/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email application attachment download info mediator
*
*/

#ifndef FSEMAILDOWNLOADINFORMATIONMEDIATOR_H
#define FSEMAILDOWNLOADINFORMATIONMEDIATOR_H

//  INCLUDES
#include <e32base.h>
//<cmail>
#include "MFSMailRequestObserver.h"
//</cmail>
#include <AknGlobalMsgQuery.h>
#include "FreestyleEmailUiConstants.h"

class CFSMailClient;

// CONSTANTS

class MFSEmailDownloadInformationObserver
	{
    /**
    * RequestResposeL
    * Download information callback
    * @param aEvent event information
    * @param aPart message part which the event belongs
    */

	public:
		virtual void RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart ) = 0;
	};

// CLASS DECLARATION
class CFSEmailDownloadInfoMediator : public CActive, public MFSMailRequestObserver
    {
    public:
	/**
	 * Two-phased class constructor.
	 */
	static CFSEmailDownloadInfoMediator* NewL( CFSMailClient& aMailClient );

	// destroys the instance
	static void Destroy();
	
	// from MFSMailRequestObserver
    virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
    
    /**
    * AddObserver
    * Adds observer to receive download information callbacks from one message  
    * @param aObserver observer, ownership is not transferred
    * @param aMessageId message to observe
    */
	void AddObserver( MFSEmailDownloadInformationObserver* aObserver, TFSMailMsgId aMessageId );
    
	/**
    * AddObserver
    * Adds observer to receive download information callbacks from all messages
    * @param aObserver observer, ownership is not transferred
    */
	void AddObserver( MFSEmailDownloadInformationObserver* aObserver );

    /**
    * StopObserving
    * Removes observer  
    * @param aObserver observer to be removed, ownership is not transferred
    * @param aMessageId message which should not be observed anymore
    */
	void StopObserving( MFSEmailDownloadInformationObserver* aObserver, TFSMailMsgId aMessageId );

    /**
    * StopObserving
    * Removes observer  
    * @param aObserver observer to be removed, ownership is not transferred
    */
	void StopObserving( MFSEmailDownloadInformationObserver* aObserver );	
	
    /**
    * DownloadL
    * Starts attachment download
    * @param aPartData message part to download
    * @param aCompleteNote show download compete note
    */
	void DownloadL( TPartData aPart, TBool aCompleteNote=ETrue );

    /**
    * DownloadAndSaveL
    * Starts attachment download and saves the file after download is complete 
    * @param aPart message part to download
    * @param aSaveFileName file name to save
    * @param aCompleteNote show download compete note
    */
	void DownloadAndSaveL( TPartData aPart, const TDesC& aSaveFileName, TBool aCompleteNote = ETrue );

    /**
    * CancelDownloadL
    * Cancels attachment download
    * @param aMessagePartId message part id to cancel
    */
	void CancelDownloadL( TFSMailMsgId aMessagePartId );


	/**
	* CancelAllDownloadsL
	* Cancels all mailbox downloads
	* @param aMailBoxId mailbox id of cancelled downloads
	*/
	void CancelAllDownloadsL( TFSMailMsgId aMailBoxId );
	
    /**
    * IsAnyAttachmentDownloads
    * Checks if there has been at least one download since program has been started
    * @return has there been a download status
    */
	TBool IsAnyAttachmentDownloads();

    /**
    * IsAnyAttachmentDownloads
    * Checks if there is at least one ongoing download for a message
    * @param aMessageId message id
    * @return current download status
    */
	TBool IsAnyAttachmentDownloads( TFSMailMsgId aMessageId );
	
    /**
    * GetDownloadPercentage
    * Checks if there is at least one ongoing download for a message
    * @param aMessageId ID of the message for which downloads are checked
    * @return           Percentage (0...100) of downloaded content out of all content currently under downloading
    *                   KErrNotFound if there are no ongoing downloads for the given message.
    */
	TInt GetDownloadPercentageL( TFSMailMsgId aMessageId );
	
    /**
    * IsDownloadableL
	* Checks if the message part is in "downloadable state" ( not downloaded and not downloading at the moment )
    * @param aPart message information
    * @return downloadable state
    */
	TBool IsDownloadableL( TPartData aPart );

    /**
    * IsDownloading
    * Checks if the message part is currently being downloaded
    * @return download status
    * @param aMessagePart message part
    */
	TBool IsDownloading( TFSMailMsgId aMessagePart );

    /**
    * Check if attachments of a message have been fully downloaded and notify observers 
    * if they are. This is used to inform the observers about the completion of 
    * automatic attachment download which takes place with POP protocol.
    * @param aMessageId The mailbox, folder, and message IDs identifying the message.
    *                   Member aMessageId.messagePartId is ignored.
    */
    void NotifyObserversIfAttachmentsDownloadedL( TPartData aMessageId );
    
public: // construction

	/**
	 * Destructor of CFSEmailDownloadInfoMediator class.
	 */

	CFSEmailDownloadInfoMediator( CFSMailClient& aMailClient );
   	
	/**
	 * Second phase class constructor.
	 */
	void ConstructL();  	

public: // from CActive
    /**
    * @see CActive.
    */
    void RunL();

    /**
    * @see CActive.
    */
    void DoCancel();
    
    /**
    * @see CActive.
    */
    TInt RunError( TInt aError );

    /**
    * Cancels querys.
    */
    void Cancel();

private: // methods

	struct TRequestObserver
		{
		MFSEmailDownloadInformationObserver* iObserver;
		TFSMailMsgId iMessageId;
        TBool iDeleted;
		};

	class TDownload
		{
	public: // members
		TInt iRequestId;
		TPartData iPartData;
		TFileName iSaveFileName;
		TBool iNotifyComplete;
		TInt iCounter;
		TInt iMaxCount;
		
	public: // constructors
		inline TDownload() 
		    : iRequestId(0), iPartData(), iSaveFileName(), iNotifyComplete(EFalse), iCounter(0), iMaxCount(0) {}
		inline TDownload( TInt aRequestId, TPartData aPartData, const TDesC& aFileName, TBool aNotifyComplete )
		    : iRequestId( aRequestId ), iPartData( aPartData ), iSaveFileName( aFileName ), 
		      iNotifyComplete( aNotifyComplete ), iCounter( 0 ), iMaxCount( 0 ) {}
		};

	class TDownloadCount
		{
	public: // members
		TFSMailMsgId iMessageId;
		TInt iDownloadsStartedCount;
		TInt iDownloadsCompletedCount;
		TInt iSaveRequestedCount;
		
	public: // constructors	
		inline TDownloadCount()
		    : iMessageId(), iDownloadsStartedCount(0), iDownloadsCompletedCount(0), iSaveRequestedCount(0) {}
		inline TDownloadCount( TFSMailMsgId aMessageId )
		    : iMessageId( aMessageId ), iDownloadsStartedCount(0), iDownloadsCompletedCount(0), iSaveRequestedCount(0) {}
		};

	/**
	 * Appends a new download to internal arrays iDownloadArray and iDonwloadCountArray.
	 * The function makes sure that there's no previous downloads with the same request ID.
	 */
	void AppendDownloadToInternalArraysL( const TDownload& aNewDownload );
	
	/**
	 * Checks if download complete notification should be show for this message part
	 */
	TBool CompletionNotesInUseL() const;
	
	/**
	* Launches a global note about completed download(s).
	* @param    aPart   A bundle of IDs identifying the message for which the note is shown.
	* @param    aCompletedCount How many attachments were succesfully downloaded.
	*/
	void LaunchDownloadCompleteNoteL( const TPartData& aPart, TInt aCompletedCount );

	/**
	* Notify all the registered observers about a download event.
	*/
	void NotifyObserversL( const TFSProgress& aEvent, const TPartData& aPart );

	/**
	* Notify all the registered observers for a particular message part about a download event.
	*/
	void NotifyPartObserversL( const TFSProgress& aEvent, const TPartData& aPart );

    /**
     * Removes deleted observers from the observer array.
     */
    void CleanUpObservers();

    /**
     * Safely removes an entry from the observer array in case notifications
     * are currently in process.
     */
    void RemoveObserver( TInt aIdx );

	/**
	* Updates the entry in iDownloadCountArray according a progress event and gets a copy of 
	* its data. The original entry may be removed from the array as a result.
	* @param    aCountObject       Input/output argument for the count array entry. The entries are matched by the iMessageId.
	* @param    aEvent             The event which causes the entry to be updated.
	*/
	void GetAndUpdateDownloadCountL( TDownloadCount& aCountObject, const TFSProgress& aEvent );
	
	/**
	 * Updates missing iMaxCount values to match the file sizes of the corresponding attachments.
	 * @param  aMessageData    Identifies the message for which the update is done. The member
	 *                         messagePartId is ignored.
	 */
	void UpdateAttachmentSizesL( const TPartData& aMessageData );
	
	/**
	* Identity relation which chekcs if two TDownloadCount objects have matchinng
	* message ID. 
	*/
	static TBool EqualMessageId( const TDownloadCount& aFirst, const TDownloadCount& aSecond );
	
    /**
    * Identity relation which chekcs if two TDownload objects have matchinng
    * message part ID. 
    */
    static TBool EqualMessagePartId( const TDownload& aFirst, const TDownload& aSecond );

    /**
	 * Destructor of CFSEmailDownloadInfoMediator class.
	 */
	virtual ~CFSEmailDownloadInfoMediator();
        
private: // data

	// mail client 
	CFSMailClient& iMailClient;

	// download complete query
	CAknGlobalMsgQuery* iGlobalMsgQuery;
	
	// message view data or attachment data to launch from user query response
	TPartData iPopupLaunchData;
	
	// array of current downloads
	RArray<TDownload> iDownloadArray;
	
	// array of download counts of one message
	RArray<TDownloadCount> iDownloadCountArray;

    // array of download information observers (one message)
    RArray<TRequestObserver> iObserverArray;
    
	// array of download observers (all messages)
	RPointerArray<MFSEmailDownloadInformationObserver> iAllObserverArray;

	// has there been any downloads since program started
	TBool iDownloadsStarted;

    // counter indicating how many observer notification operations are
    // currently in progress
    TInt iNotificationsInProgress;

    // have any observers been deleted during notification
    TBool iObserverDeleted;
    };

#endif  // FSEMAILDOWNLOADINFORMATIONMEDIATOR_H

// end of file
