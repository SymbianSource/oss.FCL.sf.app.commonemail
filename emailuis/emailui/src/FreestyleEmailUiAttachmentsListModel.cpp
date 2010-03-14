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
* Description:  Implements model for the attachment list view
*
*/


// SYSTEM INCLUDEfS
//<cmail>
#include "emailtrace.h"
#include "cfsmailclient.h"
//</cmail>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>

// INTERNAL INCLUDES
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiAttachmentsListModel.h"
#include "FreestyleEmailUiUtilities.h"


////////////////////////////////////
// CFSEmailUiAttachmentsModelBase //
////////////////////////////////////
CFSEmailUiAttachmentsModelBase::CFSEmailUiAttachmentsModelBase( CFreestyleEmailUiAppUi& aAppUi )
    : iAppUi( aAppUi )
    {
    FUNC_LOG;
    
    }
    
void CFSEmailUiAttachmentsModelBase::ConstructL()
    {
    FUNC_LOG;
    }

CFSEmailUiAttachmentsModelBase::~CFSEmailUiAttachmentsModelBase()
    {
    FUNC_LOG;
    // iDownloadInfoMediator is singleton and destroyed by AppUi
    }

TBool CFSEmailUiAttachmentsModelBase::StartDownloadL( TFsTreeItemId aTreeId )
	{
	TAttachmentData& item = GetItemL( aTreeId );
	return StartDownloadL( item );
	}

TBool CFSEmailUiAttachmentsModelBase::StartDownloadL( const TAttachmentData& aAttachment )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iAppUi.DownloadInfoMediator() && iAppUi.DownloadInfoMediator()->IsDownloadableL( aAttachment.partData ) )
        {
        iAppUi.DownloadInfoMediator()->DownloadL( aAttachment.partData );
        retVal = ETrue;
        }
    return retVal;
    }

TBool CFSEmailUiAttachmentsModelBase::SaveAttachmentL(
	TFsTreeItemId aTreeId, const TDesC& aFileName, TInt& aSavedCount )
	{
    FUNC_LOG;
	TBool retVal = EFalse;
	TAttachmentData* attData = GetItem( aTreeId );
	if ( attData )
	    {
	    retVal = SaveAttachmentL( *attData, aFileName, aSavedCount );
		}
	return retVal;
	}

TBool CFSEmailUiAttachmentsModelBase::SaveAttachmentL(
        const TAttachmentData& aAttachment, const TDesC& aFileName, TInt& aSavedCount )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iAppUi.DownloadInfoMediator() )
        {
        if ( aAttachment.downloadProgress != KComplete &&
            !iAppUi.DownloadInfoMediator()->IsDownloading( aAttachment.partData.iMessagePartId ) )
            {
            iAppUi.DownloadInfoMediator()->DownloadAndSaveL( aAttachment.partData, aFileName );
            retVal = ETrue;
            }
        else if ( aAttachment.downloadProgress == KComplete )
            {
            CFSMailMessage* mailMessage =
                iAppUi.GetMailClient()->GetMessageByUidL(
                    aAttachment.partData.iMailBoxId,
                    aAttachment.partData.iFolderId,
                    aAttachment.partData.iMessageId,
                    EFSMsgDataEnvelope );
            CleanupStack::PushL( mailMessage );
                
            CFSMailMessagePart* attachment = mailMessage->ChildPartL(
                    aAttachment.partData.iMessagePartId ); 
            CleanupStack::PushL( attachment );  
            if ( TFsEmailUiUtility::OkToSaveFileL( aFileName, *attachment ) )       
                {
                attachment->CopyContentFileL( aFileName  ); 
                aSavedCount++;              
                }           
            CleanupStack::PopAndDestroy( attachment );              
            CleanupStack::PopAndDestroy( mailMessage );         
            }
        }
    return retVal;
    }

void CFSEmailUiAttachmentsModelBase::CancelDownloadL( TFsTreeItemId aTreeId )
	{
    FUNC_LOG;
	TAttachmentData& item = GetItemL( aTreeId );
	CancelDownloadL( item );
  	}

void CFSEmailUiAttachmentsModelBase::CancelDownloadL( const TAttachmentData& aAttachment )
    {
    FUNC_LOG;
    iAppUi.DownloadInfoMediator()->CancelDownloadL( aAttachment.partData.iMessagePartId );
    }

void CFSEmailUiAttachmentsModelBase::RemoveContentL( TFsTreeItemId aTreeId )
    {
    FUNC_LOG;
    TAttachmentData& item = GetItemL( aTreeId );
    RemoveContentL( item );
  	}

void CFSEmailUiAttachmentsModelBase::RemoveContentL( const TAttachmentData& aAttachment )
    {
    FUNC_LOG;
    const TPartData& partData = aAttachment.partData;

    CFSMailMessage* mailMessage = iAppUi.GetMailClient()->GetMessageByUidL(
        partData.iMailBoxId,
        partData.iFolderId,
        partData.iMessageId,
        EFSMsgDataEnvelope );
    CleanupStack::PushL( mailMessage );

    CFSMailMessagePart* attachmentPart = mailMessage->ChildPartL( partData.iMessagePartId );
    CleanupStack::PushL( attachmentPart );
    
    // try to remove attachment contents from the plugin side
    attachmentPart->RemoveContentL();
    
    // Reset download progress if the content was actually removed
    if ( !IsCompletelyDownloadedL( aAttachment ) )
        {
        TAttachmentData& attachment = CONST_CAST( TAttachmentData&, aAttachment );
        attachment.downloadProgress = KNone;
        }
    
    CleanupStack::PopAndDestroy( attachmentPart );
    CleanupStack::PopAndDestroy( mailMessage );
    }

TBool CFSEmailUiAttachmentsModelBase::IsDownloading(
	const TAttachmentData& aAttachment )
	{
    FUNC_LOG;
	TBool retVal = iAppUi.DownloadInfoMediator()->IsDownloading(
		aAttachment.partData.iMessagePartId );
    return retVal;
	}

TBool CFSEmailUiAttachmentsModelBase::IsCompletelyDownloadedL(
	const TAttachmentData& aAttachment )
    {
    FUNC_LOG;
    TBool completed = !IsDownloading( aAttachment ) &&
                      !iAppUi.DownloadInfoMediator()->IsDownloadableL(
                      		aAttachment.partData );
    return completed;
    }
    
TBool CFSEmailUiAttachmentsModelBase::IsMessage( const TAttachmentData& aAttachment ) const
    {
    FUNC_LOG;
    TBool isMessage = (aAttachment.fileType == EMessageType);
    return isMessage;
    }

TBool CFSEmailUiAttachmentsModelBase::IsThereAnyCurrentDownloads()
	{
    FUNC_LOG;
  	TBool ret(EFalse);
  	if ( iAppUi.DownloadInfoMediator() )
  		{
  		ret = iAppUi.DownloadInfoMediator()->IsAnyAttachmentDownloads();
  		}
	return ret;
	}

HBufC* CFSEmailUiAttachmentsModelBase::CreateSizeDescLC( TInt aSize )
    {
    FUNC_LOG;
    HBufC* sizeDesc = TFsEmailUiUtility::CreateSizeDescLC( aSize );
    HBufC* decoratedDesc = StringLoader::LoadL( R_FSE_VIEWER_ATTACHMENTS_LIST_SIZE, *sizeDesc );
    CleanupStack::PopAndDestroy( sizeDesc );
    CleanupStack::PushL( decoratedDesc );
    return decoratedDesc;
    }

CFreestyleEmailUiAppUi& CFSEmailUiAttachmentsModelBase::AppUi()
	{
    FUNC_LOG;
	return iAppUi;
	}

  	
////////////////////////////////////
// CFSEmailUiAttachmentsListModel //
////////////////////////////////////
//MODEL CLASS CONSTRUCTION
CFSEmailUiAttachmentsListModel* CFSEmailUiAttachmentsListModel::NewL(
    CFreestyleEmailUiAppUi& aAppUi, MFSEmailUiAttachmentsStatusObserver& aObserver )
    {
    FUNC_LOG;
    CFSEmailUiAttachmentsListModel* self = CFSEmailUiAttachmentsListModel::NewLC( aAppUi, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiAttachmentsListModel* CFSEmailUiAttachmentsListModel::NewLC( CFreestyleEmailUiAppUi& aAppUi,
                                                                       MFSEmailUiAttachmentsStatusObserver& aObserver )
    {
    FUNC_LOG;
    CFSEmailUiAttachmentsListModel* self = new (ELeave) CFSEmailUiAttachmentsListModel( aAppUi, aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CFSEmailUiAttachmentsListModel::ConstructL()
    {
    FUNC_LOG;
  	CFSEmailUiAttachmentsModelBase::ConstructL();
    }
 
CFSEmailUiAttachmentsListModel::CFSEmailUiAttachmentsListModel( CFreestyleEmailUiAppUi& aAppUi,
                                                                MFSEmailUiAttachmentsStatusObserver& aObserver )
	: CFSEmailUiAttachmentsModelBase( aAppUi ), iObserver( aObserver )
    {
    FUNC_LOG;
    }
	
CFSEmailUiAttachmentsListModel::~CFSEmailUiAttachmentsListModel()
    {
    FUNC_LOG;
    if ( iAppUi.DownloadInfoMediator() )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this );
        }
    iAttachments.Reset();
    }

TAttachmentData* CFSEmailUiAttachmentsListModel::GetItem( TFsTreeItemId aTreeId )
    {
    FUNC_LOG;
    TBool found = EFalse;
    TAttachmentData* returnValue = NULL;
    const TInt attachmentCount = iAttachments.Count();
    for ( TInt i = 0; i < attachmentCount && !found; i++ )
        {
        if ( iAttachments[i].treeId == aTreeId )
            {
            returnValue = &iAttachments[i];
            found = ETrue;
            }
        }
    return returnValue;
    }

TAttachmentData& CFSEmailUiAttachmentsListModel::GetItemL( TFsTreeItemId aTreeId )
    {
    FUNC_LOG;
    TAttachmentData* returnValue = GetItem( aTreeId );
    if ( !returnValue )
        {
        User::Leave( KErrNotFound );
        }
    return *returnValue;
    }

TPartData CFSEmailUiAttachmentsListModel::GetMessageL( TFsTreeItemId /*aTreeId*/ )
    {
    FUNC_LOG;
    // All the items in the attachments list view belong to the same message
    return iMessage;
    }

const RArray<TAttachmentData>& CFSEmailUiAttachmentsListModel::GetModel()
	{
	return iAttachments;
  	}

void CFSEmailUiAttachmentsListModel::SetNodeIdL( TInt aAttachmentIndex, TFsTreeItemId aTreeItemId )
	{
    FUNC_LOG;
	if ( iAttachments.Count() > aAttachmentIndex )
		{
		iAttachments[aAttachmentIndex].treeId = aTreeItemId;
		}
	else
		{
		User::Leave ( KErrNotFound );
		}
	}

void CFSEmailUiAttachmentsListModel::RequestResponseL(
	const TFSProgress& aEvent, const TPartData& aPart )
	{
    FUNC_LOG;

    TBool downloadCompletedAndListUpdated = EFalse;

    if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
        {
        // We may need to update the list here as POP protocol
        // creates a new entry when redownloading an
        // attachment (in POP case the whole mail is
        // downloaded again). Thus, the list might not be 
        // up-to-date at this point. Similar kind of handling
        // is done in UI side.
    	CFSMailMessage* mailMessage =
    	    iAppUi.GetMailClient()->GetMessageByUidL(
    		    aPart.iMailBoxId,
    		    aPart.iFolderId,
    		    aPart.iMessageId,
    		    EFSMsgDataEnvelope );

        CleanupStack::PushL( mailMessage );
        
    	RPointerArray<CFSMailMessagePart> mailParts;

    	CleanupResetAndDestroyClosePushL( mailParts );

    	mailMessage->AttachmentListL( mailParts );
    	
        TBool listNeedsUpdate = ETrue;

    	TInt count = mailParts.Count();

        for ( TInt i = 0; i < count && listNeedsUpdate; i++ )
    	    {
    	    TInt partId = mailParts[ i ]->GetPartId().Id();
    	    
    	    listNeedsUpdate = ( aPart.iMessagePartId.Id() != partId );
    	    }

    	CleanupStack::PopAndDestroy( 2, mailMessage );

        if ( listNeedsUpdate )
            {
            UpdateListL( aPart );
            
            downloadCompletedAndListUpdated = ETrue;
            }
        }

    if ( downloadCompletedAndListUpdated )
        {
        // Giving KErrNotFound causes a refresh to UI
        iObserver.DownloadStatusChangedL( KErrNotFound );
        }
    else
        {
        TInt index = KErrNotFound;

        TInt attaCount = iAttachments.Count();

    	for ( TInt i = 0; i < attaCount && index == KErrNotFound; i++ )
    		{
    	   	if ( iAttachments[i].partData.iMessagePartId ==
    	   	        aPart.iMessagePartId )
    	   		{
    	   		index = i;
    	   		}
    		}

        if ( index != KErrNotFound )
            {
       		// Currently, it's not very well defined what should be the values
       		// of each field of TFSProgressStatus in case the download is
       		// completed or cancelled. For now, we make the assumption that
       		// these values are correctly set for status events. In complete 
       		// event we set the progress to 100% and in cancel event to 0%, 
       		// regardless of the counter values. These may be subjects to
       		// change.
            if ( aEvent.iError )
       		    {
       		    iAttachments[index].downloadProgress = KNone;
       		    }
       		else if ( aEvent.iProgressStatus ==
       		            TFSProgress::EFSStatus_Status )
   		        {
    	   		if ( aEvent.iMaxCount > 0 && aEvent.iCounter > 0 )
    	   			{
    				iAttachments[index].downloadProgress =
    				    KComplete * aEvent.iCounter / aEvent.iMaxCount;
    	   			}
    	   		else
    	   			{
    	   			iAttachments[index].downloadProgress = KNone;
    	   			}
   		        }
       		else if ( aEvent.iProgressStatus ==
       		            TFSProgress::EFSStatus_RequestComplete )
       		    {
   		        if ( IsCompletelyDownloadedL( iAttachments[index] ) )
   		            {
   		            iAttachments[index].downloadProgress = KComplete;
   		            }
   		        }
       		else if ( aEvent.iProgressStatus ==
       		            TFSProgress::EFSStatus_RequestCancelled )
   		        {
   		        iAttachments[index].downloadProgress = KNone;
   		        }

    	    iObserver.DownloadStatusChangedL( index );
            }
        }
	}

void CFSEmailUiAttachmentsListModel::CancelAllDownloadsL()
	{
    FUNC_LOG;
  	TInt attCount = iAttachments.Count();
	for ( TInt i = 0; i < attCount; i++ )
		{
		if ( iAppUi.DownloadInfoMediator() && iAppUi.DownloadInfoMediator()->IsDownloading(
			iAttachments[i].partData.iMessagePartId ) )
			{
	    	iAppUi.DownloadInfoMediator()->CancelDownloadL(
	    		iAttachments[i].partData.iMessagePartId );
			}
		}
	}
	
TBool CFSEmailUiAttachmentsListModel::IsThereAnyMessageAttachments() const
    {
    FUNC_LOG;
    TBool msgFound = EFalse;
    TInt attCount = iAttachments.Count();
    for ( TInt i = 0 ; i < attCount && !msgFound ; i++ )
        {
        msgFound = IsMessage( iAttachments[i] );
        }
    return msgFound;
    }

void CFSEmailUiAttachmentsListModel::UpdateListL( CFSMailMessage* aEmbeddedMessage )
	{
    FUNC_LOG;
	iAttachments.Reset();

	CFSMailMessage* mailMessage = aEmbeddedMessage;
    
	RPointerArray<CFSMailMessagePart> mailParts;
	CleanupResetAndDestroyClosePushL( mailParts );
	mailMessage->AttachmentListL( mailParts );
	
	TInt mailPartsCount = mailParts.Count();
	for ( TInt i = 0; i < mailPartsCount; i++ )
		{
        TFileType fileType = TFsEmailUiUtility::GetFileType(
        	mailParts[i]->AttachmentNameL(),
        	mailParts[i]->GetContentType() );
		TPartData partData(	aEmbeddedMessage->GetMailBoxId(),
                            aEmbeddedMessage->GetFolderId(),
                            aEmbeddedMessage->GetMessageId(),
			                mailParts[i]->GetPartId() );
		TAttachmentData attachment = 
			{
			mailParts[i]->ContentSize(),
			0,
			fileType,
			mailParts[i]->AttachmentNameL(),
			KFsTreeNoneID,
			partData
			};
		
		// if not fully fetched than restart download;
		if (IsCompletelyDownloadedL(attachment))
			attachment.downloadProgress = KComplete;
		else
			attachment.downloadProgress = KNone;
		
		iAttachments.Append( attachment );
		}
	CleanupStack::PopAndDestroy( &mailParts );
	}
	
void CFSEmailUiAttachmentsListModel::UpdateListL( TPartData aMessage )
	{
    FUNC_LOG;
	iAttachments.Reset();
	
	iMessage = aMessage;
	iAppUi.DownloadInfoMediator()->StopObserving( this );
	iAppUi.DownloadInfoMediator()->AddObserver( this, iMessage.iMessageId );
	CFSMailMessage* mailMessage = iAppUi.GetMailClient()->GetMessageByUidL(
		iMessage.iMailBoxId,
		iMessage.iFolderId,
		iMessage.iMessageId,
		EFSMsgDataEnvelope );
    CleanupStack::PushL( mailMessage );
    
	RPointerArray<CFSMailMessagePart> mailParts;
	CleanupResetAndDestroyClosePushL( mailParts );
	mailMessage->AttachmentListL( mailParts );
	
	
    CFSMailMessagePart* calendarPart = 
        mailMessage->FindBodyPartL( KFSMailContentTypeTextCalendar );
    CleanupStack::PushL( calendarPart );

	TInt mailPartsCount = mailParts.Count();
	for ( TInt i = 0; i < mailPartsCount; i++ )
		{
        CFSMailMessagePart* part = mailParts[i];
		
        TFileType fileType = TFsEmailUiUtility::GetFileType(
        	part->AttachmentNameL(),
        	part->GetContentType() );
		TPartData partData(	iMessage.iMailBoxId,
			                iMessage.iFolderId,
			                iMessage.iMessageId,
			                part->GetPartId() );
		
		TAttachmentData attachment = 
			{
			part->ContentSize(),
			0,
			fileType,
			part->AttachmentNameL(),
			KFsTreeNoneID,
			partData
			};

        // figure out and set download progress
		TUint downloadProgress;
		if ( IsCompletelyDownloadedL ( attachment ) )
		    {
		    downloadProgress = KComplete;
		    }
		else if ( part->ContentSize() > 0 )
			{
			downloadProgress = KComplete *
							   mailParts[i]->FetchedContentSize() /
							   mailParts[i]->ContentSize();
			}
		else
			{
			downloadProgress = KNone;
			}
		attachment.downloadProgress = downloadProgress;
		
    	// If MR,do not add ICS file to  attachment list
		if (! ( mailMessage->IsFlagSet( EFSMsgFlag_CalendarMsg ) 
		     && calendarPart
		     && part->GetPartId() == calendarPart->GetPartId() ) )
		    {
    		iAttachments.Append( attachment );
		    }
		
		}
    CleanupStack::PopAndDestroy( calendarPart );
	CleanupStack::PopAndDestroy( &mailParts );
	CleanupStack::PopAndDestroy( mailMessage );
  	}
	
TBool CFSEmailUiAttachmentsListModel::SaveAllAttachmentsL(
	const TDesC& aFileName )
	{
    FUNC_LOG;
	TBool retVal = EFalse;
	TInt attCount = iAttachments.Count();
    TInt savedCount( 0 );
	for ( TInt i = 0; i < attCount; i++ )
		{
		TBool downloadStarted = SaveAttachmentL( iAttachments[i], aFileName, savedCount );
		retVal = downloadStarted || retVal;
		}
	if ( savedCount )
	    {
	    // <cmail>
	    TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( savedCount ); 	    
	    }
	
	return retVal;
	}

TBool CFSEmailUiAttachmentsListModel::DownloadAllAttachmentsL()
	{
    FUNC_LOG;
  	TBool retVal = EFalse;
  	TInt attCount = iAttachments.Count();
	for ( TInt i = 0; i < attCount; i++ )
		{
		TBool downloadStarted = StartDownloadL( iAttachments[i] );
		retVal = downloadStarted || retVal;
		}
  	return retVal;
	}

HBufC* CFSEmailUiAttachmentsListModel::CreateMailSubjectTextLC() const
    {
    FUNC_LOG;
	CFSMailMessage* mailMessage = iAppUi.GetMailClient()->GetMessageByUidL(
		iMessage.iMailBoxId,
		iMessage.iFolderId,
		iMessage.iMessageId,
		EFSMsgDataEnvelope );
    CleanupStack::PushL( mailMessage );
    
    HBufC* subjectText = TFsEmailUiUtility::CreateSubjectTextLC( mailMessage );
	CleanupStack::Pop( subjectText );
    CleanupStack::PopAndDestroy( mailMessage );
    CleanupStack::PushL( subjectText );
    
    return subjectText;
    }

