/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email folder object
*
*/


//<cmail>
#include "emailtrace.h"
#include "CFSMailFolder.h"
#include "CFSMailPlugin.h"
//</cmail>

#include "CFSMailIterator.h"
#include "CFSMailRequestObserver.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailFolder::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailFolder* CFSMailFolder::NewLC( TFSMailMsgId aFolderId )
{
    FUNC_LOG;
  CFSMailFolder* api = new (ELeave) CFSMailFolder();
  CleanupStack:: PushL(api);
  api->ConstructL( aFolderId );
  return api;
} 

// -----------------------------------------------------------------------------
// CFSMailFolder::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailFolder* CFSMailFolder::NewL( TFSMailMsgId aFolderId )
{
    FUNC_LOG;
  CFSMailFolder* api =  CFSMailFolder::NewLC( aFolderId );
  CleanupStack:: Pop(api);
  return api;
}

// -----------------------------------------------------------------------------
// CFSMailFolder::CFSMailFolder
// -----------------------------------------------------------------------------
CFSMailFolder::CFSMailFolder()
{
    FUNC_LOG;

}

// -----------------------------------------------------------------------------
// CFSMailFolder::ConstructL
// -----------------------------------------------------------------------------
void CFSMailFolder::ConstructL( TFSMailMsgId aFolderId )
{
    FUNC_LOG;
	// get requesthandler pointer
	iRequestHandler = static_cast<CFSMailRequestHandler*>(Dll::Tls());

	// set folder id
	iFolderId = aFolderId;
}

// -----------------------------------------------------------------------------
// CFSMailFolder::~CFSMailFolder
// -----------------------------------------------------------------------------
EXPORT_C CFSMailFolder::~CFSMailFolder()
{
    FUNC_LOG;

}

// -----------------------------------------------------------------------------
// CFSMailFolder::ListMessagesL
// -----------------------------------------------------------------------------
EXPORT_C MFSMailIterator* CFSMailFolder::ListMessagesL( const TFSMailDetails aDetails,
        						const RArray<TFSMailSortCriteria>& aSorting)
{
    FUNC_LOG;

	CFSMailIterator* iterator = NULL;
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetFolderId()))
		{
		MFSMailIterator* pluginIterator =
		plugin->ListMessagesL(GetMailBoxId(),GetFolderId(),aDetails,aSorting);
		if(pluginIterator)
			{
			iterator = CFSMailIterator::NewL(*pluginIterator,iRequestHandler );
			}
		}
	return iterator;
}

// -----------------------------------------------------------------------------
// CFSMailFolder::FetchMessagesL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailFolder::FetchMessagesL( 	const RArray<TFSMailMsgId>& aMessageIds,
     							 				TFSMailDetails aDetails,
     							 				MFSMailRequestObserver& aObserver )
	{
    FUNC_LOG;
	// init asynchronous request
	CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetFolderId());
	
    TFSPendingRequest request = 
    	iRequestHandler->InitAsyncRequestL(	GetFolderId().PluginId(), aObserver );
    	
   	MFSMailRequestObserver* observer = request.iObserver;
    TRAPD(err,plugin->FetchMessagesL(	GetMailBoxId(),
    									GetFolderId(),
    									aMessageIds,
    									aDetails,
    									*observer,
    									request.iRequestId));
    if(err != KErrNone)
		{
		iRequestHandler->CompleteRequest(request.iRequestId);
		User::Leave(err);
		}
    return request.iRequestId;	
	}


// -----------------------------------------------------------------------------
// CFSMailFolder::GetSubFoldersL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolder::GetSubFoldersL(RPointerArray<CFSMailFolder>& aSubFolders)
{
    FUNC_LOG;
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetFolderId()))
		{
		TRAPD(err,plugin->ListFoldersL( GetMailBoxId(), GetFolderId(), aSubFolders));
		if(err != KErrNone)
			{
			aSubFolders.ResetAndDestroy();
			}
		}
}
	
// -----------------------------------------------------------------------------
// CFSMailFolder::RemoveMessageL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolder::RemoveMessageL(TFSMailMsgId aMessage)
{
    FUNC_LOG;
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetFolderId()))
		{
		RArray<TFSMailMsgId> messages;
		messages.Reset();
		messages.Append(aMessage);
		plugin->DeleteMessagesByUidL(GetMailBoxId(),GetFolderId(),messages);
		messages.Close();
		}
}

// -----------------------------------------------------------------------------
// CFSMailFolder::SupportsCopyFromL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailFolder::SupportsCopyFromL( TFSFolderType aFolderType )
	{
    FUNC_LOG;
	
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetFolderId()))
		{
		TFSMailBoxStatus onlineStatus = plugin->GetMailBoxStatus(GetMailBoxId());
		if(onlineStatus == EFSMailBoxOnline)
			{
			for(TInt i=0;i<iCopyOnlineBlocked.Count();i++)
				{
				if(iCopyOnlineBlocked[i] == aFolderType)
					{
					return EFalse;
					}			
				}
			}
		else if(onlineStatus == EFSMailBoxOffline)
			{
			for(TInt i=0;i<iCopyOfflineBlocked.Count();i++)
				{
				if(iCopyOfflineBlocked[i] == aFolderType)
					{
					return EFalse;
					}
				}
			}
		}
		return ETrue;
	}

// -----------------------------------------------------------------------------
// CFSMailFolder::SupportsMoveFromL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailFolder::SupportsMoveFromL( TFSFolderType aFolderType )
	{
    FUNC_LOG;

	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetFolderId()))
		{
		TFSMailBoxStatus onlineStatus = plugin->GetMailBoxStatus(GetMailBoxId());
		if(onlineStatus == EFSMailBoxOnline)
			{
			for(TInt i=0;i<iMoveOnlineBlocked.Count();i++)
				{
				if(iMoveOnlineBlocked[i] == aFolderType)
					{
					return EFalse;
					}			
				}
			}
		else if(onlineStatus == EFSMailBoxOffline)
			{
			for(TInt i=0;i<iMoveOfflineBlocked.Count();i++)
				{
				if(iMoveOfflineBlocked[i] == aFolderType)
					{
					return EFalse;
					}
				}
			}
		}
		return ETrue;
	
	}
// -----------------------------------------------------------------------------
// CFSMailFolder::RemoveDownLoadedAttachmentsL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolder::RemoveDownLoadedAttachmentsL()
    {
    FUNC_LOG;
    
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetFolderId());
    if(plugin != NULL)
        {
        MFSMailIterator* iterator = NULL;

        // select message details to be listed
        TFSMailDetails details(EFSMsgDataEnvelope);
        
        // sorting is free, give empty array
        RArray<TFSMailSortCriteria> sorting;
        sorting.Reset();
        iterator = plugin->ListMessagesL( GetMailBoxId(),
                                          GetFolderId(),
                                          details,
                                          sorting );
        if(iterator)
            {
            TFSMailMsgId nullId;
            RPointerArray<CFSMailMessage> messages;
            messages.Reset();
            iterator->NextL(nullId,GetMessageCount(),messages);
            for(TInt i=0;i<messages.Count();i++)
                {
                if(messages[i]->IsFlagSet(EFSMsgFlag_Attachments))
                    {
                messages[i]->RemoveDownLoadedAttachmentsL();
                }
                }
            messages.ResetAndDestroy();
            delete iterator;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSMailFolder::ReleaseExtension
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolder::ReleaseExtension( CEmailExtension* aExtension )
    {
    FUNC_LOG;
    // no specialized behaviour, call base class
    CExtendableEmail::ReleaseExtension( aExtension );
    }
    
// -----------------------------------------------------------------------------
// CFSMailFolder::ExtensionL
// -----------------------------------------------------------------------------
EXPORT_C CEmailExtension* CFSMailFolder::ExtensionL( const TUid& aInterfaceUid )
    {
    FUNC_LOG;
    return CExtendableEmail::ExtensionL( aInterfaceUid );
    }

