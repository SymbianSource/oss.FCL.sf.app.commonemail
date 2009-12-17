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
#include "FreestyleEmailUiAppUi.h"
#include "FreestyleEmailUiDownloadManagerModel.h"
#include "FreestyleEmailDownloadInformationMediator.h"
#include "FreestyleEmailUiUtilities.h"


//MODEL CLASS CONSTRUCTION
CFSEmailUiDownloadManagerModel* CFSEmailUiDownloadManagerModel::NewL( CFreestyleEmailUiAppUi& aAppUi,
                                                                      MFSEmailUiAttachmentsStatusObserver& aObserver )
    {
    FUNC_LOG;
    CFSEmailUiDownloadManagerModel* self = CFSEmailUiDownloadManagerModel::NewLC( aAppUi, aObserver );
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiDownloadManagerModel* CFSEmailUiDownloadManagerModel::NewLC( CFreestyleEmailUiAppUi& aAppUi,
                                                                       MFSEmailUiAttachmentsStatusObserver& aObserver )
    {
    FUNC_LOG;
    CFSEmailUiDownloadManagerModel* self = new (ELeave) CFSEmailUiDownloadManagerModel( aAppUi, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CFSEmailUiDownloadManagerModel::ConstructL()
    {
    FUNC_LOG;
   	CFSEmailUiAttachmentsModelBase::ConstructL();
	iMailClient = AppUi().GetMailClient();
	if ( iAppUi.DownloadInfoMediator() )
		{
		iAppUi.DownloadInfoMediator()->AddObserver( this );		
		}
    }
 
CFSEmailUiDownloadManagerModel::CFSEmailUiDownloadManagerModel( CFreestyleEmailUiAppUi& aAppUi,
                                                                MFSEmailUiAttachmentsStatusObserver& aObserver ) 
	: CFSEmailUiAttachmentsModelBase( aAppUi ), iObserver( aObserver )
    {
    FUNC_LOG;
    }
	
CFSEmailUiDownloadManagerModel::~CFSEmailUiDownloadManagerModel()
    {
    FUNC_LOG;
    for (TInt i=0; i<iMails.Count(); i++)
        {
        iMails[i].mailAttachments.Close();
        delete iMails[i].mailSubject;
        }
    iMails.Close();
    if ( iAppUi.DownloadInfoMediator() )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this );
        }
    }


// ---------------------------------------------------------------------------
// Returns attachment data for specified tree item, or NULL if not found.
// ---------------------------------------------------------------------------
//
TAttachmentData* CFSEmailUiDownloadManagerModel::GetItem( TFsTreeItemId aTreeId )
    {
    FUNC_LOG;
    const TInt mailCount = iMails.Count();
    for ( TInt i = 0; i < mailCount; i++ )
        {
        const TInt attachmentCount = iMails[i].mailAttachments.Count();
        for ( TInt j = 0; j < attachmentCount; j++)
            {
            if ( iMails[i].mailAttachments[j].treeId == aTreeId )
                {
                return &iMails[i].mailAttachments[j];
                }
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// Returns attachment data for specified tree item
// ---------------------------------------------------------------------------
//
TAttachmentData& CFSEmailUiDownloadManagerModel::GetItemL( TFsTreeItemId aTreeId )
	{
    FUNC_LOG;
	TAttachmentData* returnValue = GetItem( aTreeId );
	if ( !returnValue )
		{
		User::Leave( KErrNotFound );
		}
	return *returnValue;
	}

TPartData CFSEmailUiDownloadManagerModel::GetMessageL( TFsTreeItemId aTreeId )
    {
    FUNC_LOG;
	TPartData returnValue;
	TBool found = EFalse;
	
	for ( TInt i=0 ; i<iMails.Count() && !found ; i++ )
		{
		if ( iMails[i].treeId == aTreeId )
		    {
		    returnValue = iMails[i].partData;
		    found = ETrue;
		    }
		else
		    {
    		for ( TInt j=0 ; j<iMails[i].mailAttachments.Count() && !found ; j++ )
    	   		{
    	   		if ( iMails[i].mailAttachments[j].treeId == aTreeId )
    	   			{
    	   			returnValue = iMails[i].partData;
    	   			found = ETrue;
    	   			}
    			}
		    }
		}
	
	if ( !found )
		{
		User::Leave( KErrNotFound );
		}
	return returnValue;
    }

	
TBool CFSEmailUiDownloadManagerModel::RemoveItem( TFsTreeItemId aTreeId )
	{
    FUNC_LOG;
	TBool found = EFalse;
	for ( TInt i=0 ; !found && i<iMails.Count() ; i++ )
		{
		for ( TInt j=0 ; !found && j<iMails[i].mailAttachments.Count() ; j++ )
			{
			if ( iMails[i].mailAttachments[j].treeId == aTreeId )
				{
			   	iMails[i].mailAttachments.Remove(j);
			   	if ( iMails[i].mailAttachments.Count() == 0 )
			   		{
			   		iMails.Remove(i);			   		
			   		}
			   	found = ETrue;
				}
			}
	   	}
	return found;
	}

void CFSEmailUiDownloadManagerModel::SetItemIdL(TUint aMailIndex, TUint aAttachmentIndex, TFsTreeItemId aTreeId)
	{
    FUNC_LOG;
	if ( iMails.Count()>aMailIndex && iMails[aMailIndex].mailAttachments.Count() > aAttachmentIndex )
		{
		iMails[aMailIndex].mailAttachments[aAttachmentIndex].treeId = aTreeId;
		}
	else
		{
		User::Leave ( KErrNotFound );
		}
	}

const RArray<TMessageData>& CFSEmailUiDownloadManagerModel::GetModel()
	{
	return iMails;
	}

TInt CFSEmailUiDownloadManagerModel::AttachmentCount() const
    {
    FUNC_LOG;
    TInt count = 0;
    
    for ( TInt i=0 ; i<iMails.Count() ; ++i )
        {
        count += iMails[i].mailAttachments.Count();
        }
        
    return count;
    }
	
void CFSEmailUiDownloadManagerModel::RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart )
	{
    FUNC_LOG;
	TBool attachmentFound = EFalse;
	TInt i;
	for (i=0; i<iMails.Count(); i++)
		{
		for (TInt j=0; j<iMails[i].mailAttachments.Count(); j++)
			{
		   	if ( iMails[i].mailAttachments[j].partData.iMessagePartId == aPart.iMessagePartId )
		   		{
		   		attachmentFound = ETrue;
    	   		// Currently, it's not very well defined what should be the values
    	   		// of each field of TFSProgressStatus in case the download is completed
    	   		// or cancelled. For now, we make the assumption that these values are
    	   		// correctly set for status events. In complete event we set the progress
    	   		// to 100% and in cancel event to 0%, regardless of the counter values.
    	   		// These may be subjects to change. 
    	   		if ( aEvent.iError )
    	   		    {
    	   		    iMails[i].mailAttachments[j].downloadProgress = 0;
    	   		    }
    	   		else
    	   		    {
        	   		switch ( aEvent.iProgressStatus )
        	   		    {
        	   		    case TFSProgress::EFSStatus_Status:
        	   		        {
                	   		if ( aEvent.iMaxCount > 0 && aEvent.iCounter > 0 )
                	   			{
                				iMails[i].mailAttachments[j].downloadProgress = 
                				    KComplete * aEvent.iCounter / aEvent.iMaxCount;
                	   			}
                	   		else
                	   			{
                	   			iMails[i].mailAttachments[j].downloadProgress = KNone;
                	   			}
        	   		        }
        	   		    break;
        	   		    
        	   		    case TFSProgress::EFSStatus_RequestComplete:
        	   		        {
        	   		        iMails[i].mailAttachments[j].downloadProgress = KComplete;
        	   		        }
        	   		    break;
        	   		    
        	   		    case TFSProgress::EFSStatus_RequestCancelled:
        	   		        {
        	   		        iMails[i].mailAttachments[j].downloadProgress = KNone;
        	   		        }
        	   		    break;
        	   		    
        	   		    default:
        	   		        // do nothing with other events
        	   		    break;
        	   		    }
    	   		    }
    	   		
   	   		    iObserver.DownloadStatusChangedL(i);
				}
			}
		}
	if ( !attachmentFound )
		{
		CFSMailMessage* mailMessage = iAppUi.GetMailClient()->GetMessageByUidL( aPart.iMailBoxId, aPart.iFolderId, aPart.iMessageId, EFSMsgDataEnvelope );
		CleanupStack::PushL( mailMessage );
		CFSMailMessagePart* messagePart = mailMessage->ChildPartL( aPart.iMessagePartId );
		CleanupStack::PushL( messagePart );

    	TFileType fileType = TFsEmailUiUtility::GetFileType( messagePart->AttachmentNameL(),
    	                                                     messagePart->GetContentType() );
		TUint downloadProgress;
		if ( aEvent.iMaxCount > 0 )
			{
			downloadProgress = KComplete * aEvent.iCounter / aEvent.iMaxCount;
   			}
   		else
			{
			downloadProgress = KNone;
			}


		TAttachmentData attachment = 
			{
			messagePart->ContentSize(),
			downloadProgress,
			fileType,
			messagePart->AttachmentNameL(),
			KFsTreeNoneID,
			aPart
			};

		TBool messageFound = EFalse;
		for ( TInt i=0; i<iMails.Count(); i++ )
			{
			if ( iMails[i].partData.iMessageId  == aPart.iMessageId )
				{
				// Insert the new attachment as the first entry of the message node. 
				iMails[i].mailAttachments.InsertL( attachment, 0 );
				iObserver.DownloadStatusChangedL(i);
				messageFound = ETrue;
				}
			}

		if ( !messageFound )
			{
			// If no message node is already present in the list, a new one is created on
			// the top of the list.
			RArray<TAttachmentData> attachmentData;
			attachmentData.Append( attachment );
			
			HBufC* subjectText = TFsEmailUiUtility::CreateSubjectTextLC( mailMessage );
			TMessageData msgData = { subjectText, attachmentData, aPart, KFsTreeNoneID };
			iMails.InsertL( msgData, 0 );
            CleanupStack::Pop( subjectText );
			iObserver.DownloadStatusChangedL( 0 );
			}
		CleanupStack::PopAndDestroy( messagePart );
		CleanupStack::PopAndDestroy( mailMessage );
		}		
	}

void CFSEmailUiDownloadManagerModel::SetNodeIdL( TInt aNodeIndex, TFsTreeItemId aTreeItemId )
	{
    FUNC_LOG;
	if ( iMails.Count() > aNodeIndex )
		{
		iMails[aNodeIndex].treeId = aTreeItemId;
		}
	else
		{
		User::Leave ( KErrNotFound );
		}

	}

TBool CFSEmailUiDownloadManagerModel::DownloadAllAttachmentsL()
    {
    FUNC_LOG;
    TBool retVal = EFalse;
	for (TInt i=0; i<iMails.Count(); i++)
		{
		for (TInt j=0; j<iMails[i].mailAttachments.Count(); j++)
	   		{
			TBool downloadStarted = StartDownloadL( iMails[i].mailAttachments[j].treeId );
			retVal = downloadStarted || retVal;
		   	}
		}
	return retVal;	
    }
	
TBool CFSEmailUiDownloadManagerModel::SaveAllAttachmentsL( const TDesC& aFileName )
	{
    FUNC_LOG;
	TBool retVal = EFalse;
	TInt savedCount( 0 );
	for (TInt i=0; i<iMails.Count(); i++)
		{
		for (TInt j=0; j<iMails[i].mailAttachments.Count(); j++)
	   		{
			TBool downloadStarted = SaveAttachmentL( iMails[i].mailAttachments[j].treeId, aFileName, savedCount );
			retVal = downloadStarted || retVal;
		   	}
		}
	if ( savedCount )
	    {
	    // <cmail>
	    TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( savedCount );        
	    }
	return retVal;
	}

void CFSEmailUiDownloadManagerModel::CancelAllDownloadsL()
    {
    FUNC_LOG;
	for (TInt i=0; i<iMails.Count(); i++)
		{
		for (TInt j=0; j<iMails[i].mailAttachments.Count(); j++)
	   		{
	   		TFSMailMsgId attPartId = iMails[i].mailAttachments[j].partData.iMessagePartId;
   			if ( iAppUi.DownloadInfoMediator() )
   				{
	   			iAppUi.DownloadInfoMediator()->CancelDownloadL( attPartId );				
   				}
			}
		}
    }

TBool CFSEmailUiDownloadManagerModel::IsThereAnyMessageAttachments()
    {
    FUNC_LOG;
    TBool msgFound = EFalse;
    TInt mailCount = iMails.Count();
    for ( TInt i = 0 ; i < mailCount && !msgFound ; i++ )
        {
        TInt attCount = iMails[i].mailAttachments.Count();
        for ( TInt j = 0 ; j < attCount && !msgFound ; j++ )
            {
            const TAttachmentData& attachment = iMails[i].mailAttachments[j];
            msgFound = IsMessage( attachment );
            }
        }
    return msgFound;
    }
    
TBool CFSEmailUiDownloadManagerModel::ClearInvalidItemsL()
	{
    FUNC_LOG;
	TBool itemWasRemoved = EFalse;
	
	// Loop from back to beginning, easier to destroy items.
	if ( iMails.Count() )
		{
		for ( TInt i=iMails.Count()-1; i>=0; i-- )
			{
			// Remove items from model if they are not valid anymore
		    CFSMailMessage* confirmedMsgPtr(0);
		    // TRAP KErrNotFound leave with ignore cause null pointer is checked after this
		    TRAP_IGNORE( confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL( iMails[i].partData.iMailBoxId, 
				          iMails[i].partData.iFolderId, iMails[i].partData.iMessageId , EFSMsgDataIdOnly ) ); // Check if id is valid
			if ( confirmedMsgPtr )
				{
				delete confirmedMsgPtr; // delete unnecessary object
				}		
			else
				{
		        iMails[i].mailAttachments.Close();
		        delete iMails[i].mailSubject;
				iMails.Remove(i); // Remove
				itemWasRemoved = ETrue;
				}
			}			
		}
	
	return itemWasRemoved;
	}

