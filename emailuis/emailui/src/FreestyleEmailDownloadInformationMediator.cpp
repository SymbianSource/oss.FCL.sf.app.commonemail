/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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


//  INCLUDE FILES
#include "emailtrace.h"
#include <e32svr.h>
//<cmail>
#include "CFSMailClient.h"
//</cmail>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>

#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailDownloadInformationMediator.h"
#include "FreestyleEmailCenRepHandler.h"
#include "FreestyleEmailUiAppui.h"
#include "FSEmail.pan"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// CFSEmailDownloadInfoMediator::CFSEmailDownloadInfoMediator

// Default class constructor.
// ----------------------------------------------------
//
CFSEmailDownloadInfoMediator::CFSEmailDownloadInfoMediator( CFSMailClient& aMailClient )
    : CActive( EPriorityNormal ), iMailClient(aMailClient),
    iDownloadArray( KArrayGranularity, _FOFF(TDownload, iRequestId) ), iDownloadsStarted ( EFalse )
	{
    FUNC_LOG;
	CActiveScheduler::Add( this );
	}

// ----------------------------------------------------
// CFSEmailDownloadInfoMediator::ConstructL
// Second phase class constructor.
// ----------------------------------------------------
//
void CFSEmailDownloadInfoMediator::ConstructL()
	{
    FUNC_LOG;
	}

// ----------------------------------------------------
// CFSEmailDownloadInfoMediator::NewL
// Two-phased class constructor.
// Singleton
// ----------------------------------------------------
//
CFSEmailDownloadInfoMediator* CFSEmailDownloadInfoMediator::NewL( CFSMailClient& aMailClient )
	{
    FUNC_LOG;

	CFSEmailDownloadInfoMediator* singleton = NULL;

	// Check Thread Local Storage for instance pointer
	singleton = static_cast<CFSEmailDownloadInfoMediator*>( UserSvr::DllTls( KTlsHandleDownloadInfo ) );
	if ( !singleton )
		{
		singleton = new ( ELeave ) CFSEmailDownloadInfoMediator( aMailClient );
		CleanupStack::PushL( singleton );
		singleton->ConstructL();
		CleanupStack::Pop( singleton );
		
		// Store a pointer of a new instance in Thread Local Storage
		TInt err = UserSvr::DllSetTls( KTlsHandleDownloadInfo, singleton );
		if ( err )
			{
			delete singleton;
			singleton = NULL;
			User::Leave( err );
			}
		}

	return singleton;
	}


void CFSEmailDownloadInfoMediator::AddObserver( MFSEmailDownloadInformationObserver* aObserver, TFSMailMsgId aMessageId )
	{
    FUNC_LOG;
	TRequestObserver newObserver = {aObserver, aMessageId };
	iObserverArray.Append( newObserver );
	}

void CFSEmailDownloadInfoMediator::AddObserver( MFSEmailDownloadInformationObserver* aObserver )
	{
	iAllObserverArray.Append(aObserver);
	}

// ----------------------------------------------------
// CFSEmailDownloadInfoMediator::~CFSEmailDownloadInfoMediator
// Destructor of CFSEmailDownloadInfoMediator class.
// ----------------------------------------------------
//
CFSEmailDownloadInfoMediator::~CFSEmailDownloadInfoMediator()
	{
	FUNC_LOG;
	iAllObserverArray.Close();
	iObserverArray.Close();
	iDownloadArray.Close();
	iDownloadCountArray.Close();
	Cancel();
    }


void CFSEmailDownloadInfoMediator::RequestResponseL(
	TFSProgress aEvent, TInt aRequestId )
	{
	FUNC_LOG;
	// Find the correct download structure
	TDownload download( aRequestId, TPartData(), KNullDesC(), EFalse );
	TInt idx = iDownloadArray.Find( download );
	
    if ( idx >= 0 && idx < iDownloadArray.Count() )
    	{
    	// save the progress info from status events
    	if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_Status )
    	    {
        	iDownloadArray[idx].iCounter = aEvent.iCounter;
        	iDownloadArray[idx].iMaxCount = aEvent.iMaxCount;
    	    }
    	
		// Make a local copy of the download object as the object may be
		// removed from the array before we are finished
		download = iDownloadArray[idx];
        
    	// In case of error, show a note
		if ( aEvent.iError && aEvent.iError != KErrCancel )
		    {
			// Download failed, show error note
			if ( aEvent.iError == KErrCouldNotConnect ||
				 aEvent.iError == KErrConnectionTerminated )
			    { // connection error
			    TFsEmailUiUtility::ShowErrorNoteL(
			    	R_FREESTYLE_EMAIL_ERROR_GENERAL_CONNECTION_ERROR,
			    	ETrue );
			    }
			else // other error
			    {
			    TFsEmailUiUtility::ShowErrorNoteL(
			    	R_FREESTYLE_EMAIL_ERROR_GENERAL_UNABLE_TO_COMPLETE,
			    	ETrue );
			    }
		    }
		    
		// Remove download from array if it has been cancelled or failed
		if ( aEvent.iProgressStatus ==
			 TFSProgress::EFSStatus_RequestCancelled || aEvent.iError )
			{
			iDownloadArray.Remove(idx);
			}
		// Mark download as completed if download just finished
		else if ( aEvent.iProgressStatus ==
				  TFSProgress::EFSStatus_RequestComplete )
		    {
		    if ( !iDownloadArray[idx].iMaxCount )
		        {
		        // Set max count to 1 if no single progress event has arrived
		        // before download is finished.
		        iDownloadArray[idx].iMaxCount = 1;
		        }
		    iDownloadArray[idx].iCounter = iDownloadArray[idx].iMaxCount;
		    }
		
		TInt completedDownloadsToNotify = 0;        

		// Find and update the count array entry
        TDownloadCount countObject( download.iPartData.iMessageId );
        GetAndUpdateDownloadCountL( countObject, aEvent );

		// Notification may be given if all ongoing downloads from message
		// have finished and there are some properly completed downloads
        if ( download.iNotifyComplete &&
             countObject.iDownloadsCompletedCount &&
             countObject.iDownloadsCompletedCount ==
             	countObject.iDownloadsStartedCount )
            {
			completedDownloadsToNotify = countObject.iDownloadsCompletedCount;
            }
					    
		// if download is complete
		if ( aEvent.iProgressStatus ==
			 TFSProgress::EFSStatus_RequestComplete && !aEvent.iError )
			{
            // if file was set to be saved after download
    		if ( download.iSaveFileName.CompareC( KNullDesC ) )
    			{
    			CFSMailMessage* mailMessage = iMailClient.GetMessageByUidL(
    				download.iPartData.iMailBoxId,
    				download.iPartData.iFolderId,
    				download.iPartData.iMessageId,
    				EFSMsgDataEnvelope );
        		CleanupStack::PushL( mailMessage );
        		CFSMailMessagePart* attachment = mailMessage->ChildPartL(
        			download.iPartData.iMessagePartId );
        		CleanupStack::PopAndDestroy( mailMessage );
        		CleanupStack::PushL( attachment );
        		if ( TFsEmailUiUtility::OkToSaveFileL( download.iSaveFileName, *attachment ) )
        			{
	        		attachment->CopyContentFileL( download.iSaveFileName );       			
        			}
        		CleanupStack::PopAndDestroy( attachment );
    			}
			}
			
		// relay the event to observers
		NotifyObserversL( aEvent, download.iPartData );

        // <cmail> moved completion note after notifying so that observers can 
        // clear menus etc. before note is displayed
        // if download is complete
		if ( aEvent.iProgressStatus ==
			 TFSProgress::EFSStatus_RequestComplete && !aEvent.iError )
			{
			// Show "Download completed" if necessary
			if ( CompletionNotesInUseL() && completedDownloadsToNotify )
				{
				LaunchDownloadCompleteNoteL( download.iPartData,
					completedDownloadsToNotify );
				}
            // Notification of saved attachments may be given if all downloads of the given message has been completed.
            if ( download.iNotifyComplete && countObject.iDownloadsCompletedCount &&
                 countObject.iDownloadsCompletedCount == countObject.iDownloadsStartedCount )
                 {
                 TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( countObject.iSaveRequestedCount );
                 }        
			}
	    // </cmail>
        }
	}
	
TBool CFSEmailDownloadInfoMediator::IsAnyAttachmentDownloads()
	{
	FUNC_LOG;
	return iDownloadsStarted;
	}


TBool CFSEmailDownloadInfoMediator::IsAnyAttachmentDownloads( TFSMailMsgId aMessageId )
	{
	FUNC_LOG;
	for ( TInt i=0; i<iDownloadArray.Count(); i++ )
		{
		if ( iDownloadArray[i].iPartData.iMessageId == aMessageId )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TInt CFSEmailDownloadInfoMediator::GetDownloadPercentageL( TFSMailMsgId aMessageId )
    {
	FUNC_LOG;
    TInt count = 0;
    TInt maxCount = 0;
    
    for ( TInt i=0; i<iDownloadArray.Count(); i++ )
        {
        if ( iDownloadArray[i].iPartData.iMessageId == aMessageId )
            {
            // In case we find an attachment for which maxCount is still missing,
            // update all the missing maxCounts to match the file size of
            // the attachment.
            if ( iDownloadArray[i].iMaxCount <= 0 )
                {
                // This should happen at most once per call to GetDownloadPercentageL().
                UpdateAttachmentSizesL( iDownloadArray[i].iPartData );
                }

            count += iDownloadArray[i].iCounter;
            maxCount += iDownloadArray[i].iMaxCount;
            }
        }
    
    if ( maxCount <= 0 )
        {
        return KErrNotFound;
        }
    else
        {
        return 100*count/maxCount;
        }
    }

void CFSEmailDownloadInfoMediator::UpdateAttachmentSizesL( const TPartData& aMessageData )
    {
	FUNC_LOG;
    CFSMailMessage* mailMessage = iMailClient.GetMessageByUidL( aMessageData.iMailBoxId, 
                                                                aMessageData.iFolderId, 
                                                                aMessageData.iMessageId, 
                                                                EFSMsgDataEnvelope );
    CleanupStack::PushL( mailMessage );
    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
    mailMessage->AttachmentListL( attachments );
    
    for ( TInt i=0; i<iDownloadArray.Count(); i++ )
        {
        if ( iDownloadArray[i].iPartData.iMessageId == aMessageData.iMessageId )
            {
            if ( iDownloadArray[i].iMaxCount <= 0 )
                {
                // Find the matching attachment object and retrieve the attachment size
                for ( TInt j=0 ; j<attachments.Count() ; ++j )
                    {
                    if ( attachments[j]->GetPartId() == iDownloadArray[i].iPartData.iMessagePartId )
                        {
                        iDownloadArray[i].iMaxCount = attachments[j]->ContentSize();
                        break;
                        }
                    }
                }
            }
        }
    
    CleanupStack::PopAndDestroy( &attachments );
    CleanupStack::PopAndDestroy( mailMessage );
    }

TBool CFSEmailDownloadInfoMediator::IsDownloadableL( TPartData aPart )
	{
	FUNC_LOG;
	TBool ret(EFalse);
	// if part is downloading, no more checking is needed
	if ( !IsDownloading( aPart.iMessagePartId ) )
		{
		CFSMailMessage* mailMessage = iMailClient.GetMessageByUidL(
			aPart.iMailBoxId, aPart.iFolderId,
			aPart.iMessageId, EFSMsgDataEnvelope );
		if ( mailMessage )
			{
			CleanupStack::PushL( mailMessage );
			CFSMailMessagePart* mailMessagePart =
				mailMessage->ChildPartL( aPart.iMessagePartId );
			// if the file is fetched full
			if ( mailMessagePart &&
			        mailMessagePart->FetchLoadState() == EFSFull )
				{
				ret = EFalse;
				}
			else
				{
				ret =  ETrue;
				}
			delete mailMessagePart;
			CleanupStack::PopAndDestroy( mailMessage );			
			}
		}
	return ret;
	}


void CFSEmailDownloadInfoMediator::Destroy()
	{
	FUNC_LOG;
    CFSEmailDownloadInfoMediator* self = 
        static_cast<CFSEmailDownloadInfoMediator*>( UserSvr::DllTls( KTlsHandleDownloadInfo ) );
	UserSvr::DllFreeTls( KTlsHandleDownloadInfo );
    delete self;
    }

void CFSEmailDownloadInfoMediator::StopObserving( MFSEmailDownloadInformationObserver* aObserver, TFSMailMsgId aMessageId )
	{
	FUNC_LOG;
    for (TInt i=0; i<iObserverArray.Count(); i++)
    	{
   		if ( iObserverArray[i].iObserver == aObserver && iObserverArray[i].iMessageId == aMessageId )
   			{
   			iObserverArray.Remove(i);
   			}
    	}
	}

void CFSEmailDownloadInfoMediator::StopObserving( MFSEmailDownloadInformationObserver* aObserver )
	{
	FUNC_LOG;
    for (TInt i=0; i<iObserverArray.Count(); i++)
    	{
   		if ( iObserverArray[i].iObserver == aObserver )
   			{
   			iObserverArray.Remove(i);
   			}
    	}
    for (TInt i=0; i<iAllObserverArray.Count(); i++)
    	{
		if ( iAllObserverArray[i] == aObserver )
   			{
   			iAllObserverArray.Remove(i);
   			}
    	}
	}

void CFSEmailDownloadInfoMediator::DownloadL( TPartData aPart, TBool aCompleteNote )
	{
	FUNC_LOG;
	TFileName emptyFileName;
	DownloadAndSaveL( aPart, emptyFileName, aCompleteNote );
	}

void CFSEmailDownloadInfoMediator::DownloadAndSaveL( TPartData aPart, const TDesC& aSaveFileName, TBool aCompleteNote )
	{
	FUNC_LOG;
	// now there is at least one download started 
	iDownloadsStarted = ETrue;
	// fetch message part
	CFSMailMessage* mailMessage = iMailClient.GetMessageByUidL( aPart.iMailBoxId, aPart.iFolderId, aPart.iMessageId, EFSMsgDataEnvelope );
	CleanupStack::PushL( mailMessage );
	CFSMailMessagePart* messagePart = mailMessage->ChildPartL( aPart.iMessagePartId );
	CleanupStack::PopAndDestroy( mailMessage );
	
	// Panic in udeb builds, the condition !messagePart should not be possible
    ASSERT( messagePart );	

	// to be on the safer side only leave in urel builds
    if ( !messagePart )
	    {
	    User::Leave( KErrNotFound );
	    }	
	
	CleanupStack::PushL( messagePart );
	// start download and get request id
	TInt requestId = messagePart->FetchMessagePartL( aPart.iMessagePartId, *this, 0 );
	
	// store download information
	TDownload newDownload( requestId, aPart, aSaveFileName, aCompleteNote );
	AppendDownloadToInternalArraysL( newDownload );
    CleanupStack::PopAndDestroy( messagePart );
		
	// It may take some time before the first progress event is received from 
	// the protocol plug-in. This is true at least with the current Intellisync
	// plug-in implementation. Send artificial "0% event" to the UI to ensure that
	// the UI shows that the file is being downloaded.
	TFSProgress zeroEvent = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
	NotifyObserversL( zeroEvent, aPart );
	}

void CFSEmailDownloadInfoMediator::CancelDownloadL(
	TFSMailMsgId aMessagePartId )
	{
	FUNC_LOG;
	// Find the corresponding download object
	TDownload downloadToCancel;
	downloadToCancel.iPartData.iMessagePartId = aMessagePartId;
    TInt idx = iDownloadArray.Find( downloadToCancel, 
                                    TIdentityRelation<TDownload>(EqualMessagePartId) );
    // Cancel if download object was found
    if ( idx >= 0 && idx < iDownloadArray.Count() )
        {
        iMailClient.CancelL( iDownloadArray[idx].iRequestId );
        }
	}

void CFSEmailDownloadInfoMediator::CancelAllDownloadsL( TFSMailMsgId aMailBoxId )
    {
    // Cancel all downloads within mailbox indicated with aMailBoxId
    for (TInt i=0 ; i < iDownloadArray.Count() ; i ++ )
        {
        if ( iDownloadArray[i].iPartData.iMailBoxId == aMailBoxId )
            {
            iMailClient.CancelL( iDownloadArray[i].iRequestId );
            }
        }    
    }

TBool CFSEmailDownloadInfoMediator::IsDownloading(
	TFSMailMsgId aMessagePartId )
	{
	FUNC_LOG;
	TInt dlArrayCount = iDownloadArray.Count();
	for ( TInt i = 0; i < dlArrayCount; i++ )
    	{
    	const TDownload& downloadObject = iDownloadArray[i];
   		if ( downloadObject.iPartData.iMessagePartId == aMessagePartId )
   			{
   			if ( downloadObject.iCounter < downloadObject.iMaxCount ||
   			     downloadObject.iMaxCount <= 0 )
   			    {
   			    // Only return ETrue if the corresponding download entry is
   			    // found and it's not yet finished.
   	            return ETrue;
   			    }
   			}
    	}
	return EFalse;
	}

void CFSEmailDownloadInfoMediator::NotifyObserversIfAttachmentsDownloadedL(
	TPartData aMessageId )
    {
	FUNC_LOG;
	CFSMailMessage* mailMessage = iMailClient.GetMessageByUidL(
		aMessageId.iMailBoxId,
		aMessageId.iFolderId,
		aMessageId.iMessageId,
		EFSMsgDataEnvelope );
	CleanupStack::PushL( mailMessage );

	RPointerArray<CFSMailMessagePart> attachments;
	CleanupResetAndDestroyClosePushL( attachments );
	mailMessage->AttachmentListL( attachments );
	
	for ( TInt i=0 ; i<attachments.Count() ; ++i )
	    {
	    TInt fetched = attachments[i]->FetchedContentSize();
	    TInt totalSize = attachments[i]->ContentSize();
	    
	    if ( fetched == totalSize )
	        {
	        // download has happened => Download Manager should be available in menus
	        iDownloadsStarted = ETrue;
	        
	        // notify observers about completed attachment download
    	    TFSProgress completeEvent = 
    	        {
    	        TFSProgress::EFSStatus_RequestComplete, 
    	        fetched,
    	        totalSize,
    	        KErrNone
    	        };
    	    aMessageId.iMessagePartId = attachments[i]->GetPartId();
    	    NotifyObserversL( completeEvent, aMessageId );
	        }
	    }
	
	CleanupStack::PopAndDestroy( &attachments );
	CleanupStack::PopAndDestroy( mailMessage );
    }

void CFSEmailDownloadInfoMediator::AppendDownloadToInternalArraysL( const TDownload& aNewDownload )
    {
	FUNC_LOG;
    // In case of some unexpected errors, it's possible that we have an existing download object
    // with the same request ID as the new download. In that case, the original download has failed
    // and needs to be removed.
    TInt foundIdx = iDownloadArray.Find( aNewDownload );
    if ( foundIdx >= 0 )
        {
        TFSProgress cancelEvent = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, KErrUnknown };
        TDownload obsoleteDownload = iDownloadArray[foundIdx];
        TDownloadCount countObject( obsoleteDownload.iPartData.iMessageId );

        iDownloadArray.Remove( foundIdx );
        GetAndUpdateDownloadCountL( countObject, cancelEvent );
        NotifyObserversL( cancelEvent, obsoleteDownload.iPartData );     
        }
    
    // Add the download object to array now when we know that there's no duplicate IDs.
    iDownloadArray.Append( aNewDownload );

    // Add the download to the download count array
    TDownloadCount newCountObject( aNewDownload.iPartData.iMessageId );
    TInt idx = iDownloadCountArray.Find( newCountObject, 
                                         TIdentityRelation<TDownloadCount>(EqualMessageId) );
    if ( idx >= 0 && idx < iDownloadCountArray.Count() )
        {
        // if there are already downloads going on on the same message
        iDownloadCountArray[idx].iDownloadsStartedCount++;
        if ( aNewDownload.iSaveFileName.CompareC( KNullDesC ) )
            {
            iDownloadCountArray[idx].iSaveRequestedCount++;
            }      
        }
    else
        {
        newCountObject.iDownloadsStartedCount = 1;
        if ( aNewDownload.iSaveFileName.CompareC( KNullDesC ) )
            {
            newCountObject.iSaveRequestedCount=1;
            } 
        iDownloadCountArray.Append( newCountObject );
        }
    }

TBool CFSEmailDownloadInfoMediator::CompletionNotesInUseL() const
	{
	FUNC_LOG;
	// singleton, do not destroy
	CFSEmailCRHandler* cenRepHandler = CFSEmailCRHandler::InstanceL();
	
	return cenRepHandler->DownloadNotifications();
	}

void CFSEmailDownloadInfoMediator::LaunchDownloadCompleteNoteL(
        const TPartData& aPart, TInt aCompletedCount )
    {
	FUNC_LOG;

    Cancel(); // deletes previous iGlobalMsgQuery

    iPopupLaunchData = aPart;

    // fetch message part
	CFSMailMessage* mailMessage = iMailClient.GetMessageByUidL(
	    aPart.iMailBoxId,
	    aPart.iFolderId,
	    aPart.iMessageId,
	    EFSMsgDataEnvelope );

	CleanupStack::PushL( mailMessage );

    // From:
	HBufC* from = StringLoader::LoadLC(
	    R_FSE_VIEWER_ATTACHMENTS_TEXT_FROM );

	CFSMailAddress* senderData = mailMessage->GetSender(); // NOT OWNED

	TDesC& mailSender = senderData->GetDisplayName(); 

	// Subject:
	HBufC* mailSubject =
	    TFsEmailUiUtility::CreateSubjectTextLC( mailMessage );

	HBufC* subj = StringLoader::LoadLC(
	    R_FSE_VIEWER_ATTACHMENTS_TEXT_SUBJECT );

    // Attachment name or count
    HBufC* attachmentText = NULL;

	CFSMailMessagePart* messagePart =
	    mailMessage->ChildPartL( aPart.iMessagePartId );

    if ( messagePart )
        {
    	// create popup text

    	if ( aCompletedCount == 1 )
    		{
        	CleanupStack::PushL( messagePart );

    		attachmentText = messagePart->AttachmentNameL().AllocL(); 

        	CleanupStack::PopAndDestroy( messagePart );
        	
        	CleanupStack::PushL( attachmentText );
    		}
    	else if ( aCompletedCount > 1 )
    		{
    		attachmentText = StringLoader::LoadLC(
    		    R_FSE_VIEWER_ATTACHMENTS_DOWNLOADED, aCompletedCount );
    		iPopupLaunchData.iMessagePartId.SetNullId();
    		}
    	else
    	    {
    	    __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
    	    attachmentText = KNullDesC().AllocLC();
    	    }

        }
    else
        {
		attachmentText = StringLoader::LoadLC(
		    R_FSE_VIEWER_ATTACHMENTS_DOWNLOADED, aCompletedCount );
		iPopupLaunchData.iMessagePartId.SetNullId();
        }

    // Combine the text parts into one descriptor
    TInt textLength = from->Length() +
                      KSpace().Length() + 
                      mailSender.Length() + 
                      KLineFeed().Length() + 
                      subj->Length() + 
                      KSpace().Length() + 
                      mailSubject->Length() + 
                      KLineFeed().Length() +
                      attachmentText->Length();

    HBufC* popupText = HBufC::NewL( textLength );
    TPtr textPtr = popupText->Des();

	textPtr.Append( *from );
	textPtr.Append( KSpace );					
	textPtr.Append( mailSender );
	textPtr.Append( KLineFeed );
	textPtr.Append( *subj );
	textPtr.Append( KSpace );					
	textPtr.Append( *mailSubject );
	textPtr.Append( KLineFeed );
	textPtr.Append( *attachmentText );
	
	CleanupStack::PopAndDestroy( attachmentText );
	CleanupStack::PopAndDestroy( subj );
	CleanupStack::PopAndDestroy( mailSubject );
	CleanupStack::PopAndDestroy( from );

	CleanupStack::PopAndDestroy( mailMessage );
    
    CleanupStack::PushL( popupText );

	HBufC* popupHeader = StringLoader::LoadLC(
	    R_FSE_VIEWER_NOTE_ATTACHMENTS_DOWNLOAD_COMPLETE );

	iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();

    iGlobalMsgQuery->ShowMsgQueryL(
        iStatus,
        *popupText,
        R_AVKON_SOFTKEYS_OPEN_CLOSE,
        *popupHeader,
        KNullDesC );
	    
	CleanupStack::PopAndDestroy( popupHeader );

	CleanupStack::PopAndDestroy( popupText );
		
	SetActive();
    }
	
void CFSEmailDownloadInfoMediator::NotifyObserversL( const TFSProgress& aEvent, const TPartData& aPart )
    {
	FUNC_LOG;
	// go through all observers
	for ( TInt j=0; j<iObserverArray.Count(); j++)
		{
		// if observer is observing this message
		if ( iObserverArray[j].iMessageId == aPart.iMessageId )
			{
			// send response to observer
			iObserverArray[j].iObserver->RequestResponseL( aEvent, aPart );
			}
		}
	// send response to every 'all observer'
	for (TInt i=0; i<iAllObserverArray.Count(); i++)
		{
		iAllObserverArray[i]->RequestResponseL( aEvent, aPart );
		}
    }

void CFSEmailDownloadInfoMediator::RunL()
	{
	FUNC_LOG;
    // no close button pressed
    if (iStatus.Int() != EAknSoftkeyClose )
        {
        // open attachment list
        if ( iPopupLaunchData.iMessagePartId.IsNullId() )
            {
            TAttachmentListActivationData params;
            params.iMailBoxId = iPopupLaunchData.iMailBoxId;
            params.iFolderId = iPopupLaunchData.iFolderId;
            params.iMessageId = iPopupLaunchData.iMessageId;
            // use package buffer to pass the params
            TPckgBuf<TAttachmentListActivationData> buf( params );
            TUid emptyCustomMessageId = { 0 };
            CFreestyleEmailUiAppUi* appUi = (CFreestyleEmailUiAppUi*)CCoeEnv::Static()->AppUi();
            appUi->EnterFsEmailViewL( AttachmentMngrViewId, emptyCustomMessageId, buf );
            }
        // open attachment
        else
            {
            // Force FsEmailUI to foreground because global completion note may appear
            // while some other application is active and our local error notes are not shown
            // in that case. (This wouldn't be necessary in case of succesful file launching.)
            TFsEmailUiUtility::BringFsEmailToForeground();
            TFsEmailUiUtility::OpenAttachmentL( iPopupLaunchData );
            }
        }
    }

void CFSEmailDownloadInfoMediator::DoCancel()
    {
    }

void CFSEmailDownloadInfoMediator::Cancel()
    {
    FUNC_LOG;
    if ( iGlobalMsgQuery )
        {
        iGlobalMsgQuery->CancelMsgQuery();
        delete iGlobalMsgQuery;
        iGlobalMsgQuery = NULL;
        }
    if ( IsActive() )
        {
        CActive::Cancel();
        }
    }

TInt CFSEmailDownloadInfoMediator::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

void CFSEmailDownloadInfoMediator::GetAndUpdateDownloadCountL( TDownloadCount& aCountObject, 
                                                               const TFSProgress& aEvent )
    {
    FUNC_LOG;
    TInt idx = iDownloadCountArray.Find( aCountObject, 
                                         TIdentityRelation<TDownloadCount>(EqualMessageId) );
    if ( idx >= 0 && idx < iDownloadCountArray.Count() )
        {
        // Set started save requests, it does not change depending on the event received 
        aCountObject.iDownloadsStartedCount = iDownloadCountArray[idx].iDownloadsStartedCount;
        
        // If download completed succesfully, increase the completed downlods count
        if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete &&
             !aEvent.iError )
            {
            iDownloadCountArray[idx].iDownloadsCompletedCount++;
            }
        
        // If download was cancelled or failed, decrease started downloads count
        else if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled ||
                  aEvent.iError )
            {
            iDownloadCountArray[idx].iDownloadsStartedCount--;
            }
        else
            {
            // do nothing with other request status values
            }
        aCountObject = iDownloadCountArray[idx];
        
        if ( aCountObject.iDownloadsCompletedCount ==
             aCountObject.iDownloadsStartedCount )
            {
            // The count entry can be removed from the array when all ongoing
            // downloads have been completed or cancelled.
            iDownloadCountArray.Remove(idx);
            
            // Remove also all the connected download entries from the iDownloadArray.
            for ( TInt i = iDownloadArray.Count()-1 ; i >=0  ; --i )
                {
                if ( iDownloadArray[i].iPartData.iMessageId == aCountObject.iMessageId )
                    {
                    iDownloadArray.Remove(i);
                    }
                }
            }
        }
    }

TBool CFSEmailDownloadInfoMediator::EqualMessageId( const TDownloadCount& aFirst, const TDownloadCount& aSecond )
    {
    return (aFirst.iMessageId == aSecond.iMessageId);
    }

TBool CFSEmailDownloadInfoMediator::EqualMessagePartId( const TDownload& aFirst, const TDownload& aSecond )
    {
    return (aFirst.iPartData.iMessagePartId == aSecond.iPartData.iMessagePartId);
    }

