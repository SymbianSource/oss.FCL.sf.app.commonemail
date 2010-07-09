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
* Description:  email framework user interface
*
*/

#include "emailtrace.h"

//<qmail>
#include <nmcommonheaders.h>
//</qmail>

#include "CFSMailClient.h"
#include "CFSFWImplementation.h"
#include "CFSMailPluginManager.h"
#include "CFSMailRequestObserver.h"
#include "CFSMailIterator.h"
#include "CFSMailBrandManagerImpl.h"


// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailClient::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailClient* CFSMailClient::NewLC(TInt aConfiguration)
{
    NM_FUNCTION;

	CFSMailClient* client = Instance();
	if(!client)
	{
		client = new (ELeave) CFSMailClient();
		CleanupStack:: PushL(client);
		client->ConstructL(aConfiguration);
		TInt err = Dll::SetTls(static_cast<TAny*>(client));
		User::LeaveIfError(err);
		}
	else
		{
		CleanupStack:: PushL(client);
		}

	client->IncReferenceCount();
	
	return client;

} 

// -----------------------------------------------------------------------------
// CFSMailClient::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailClient* CFSMailClient::NewL()
{
    NM_FUNCTION;
    
    CFSMailClient* client =  CFSMailClient::NewLC(EFSLoadPlugins);
    CleanupStack:: Pop(client);
    return client;
}

// -----------------------------------------------------------------------------
// CFSMailClient::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailClient* CFSMailClient::NewL(TInt aConfiguration)
{
    NM_FUNCTION;
    
    CFSMailClient* client =  CFSMailClient::NewLC(aConfiguration);
    CleanupStack:: Pop(client);
    return client;
}

// -----------------------------------------------------------------------------
// CFSMailClient::ConstructL
// -----------------------------------------------------------------------------
void CFSMailClient::ConstructL(TInt aConfiguration)
{
    NM_FUNCTION;
    
	iFWImplementation = CFSFWImplementation::NewL(aConfiguration);
}

// -----------------------------------------------------------------------------
// CFSMailClient::CFSMailClient
// -----------------------------------------------------------------------------
CFSMailClient::CFSMailClient()
{
    NM_FUNCTION;
    
	// clear pointers
	iFWImplementation = NULL;
	iBrandManager = NULL;
}


// -----------------------------------------------------------------------------
// CFSMailClient::~CFSMailClient
// -----------------------------------------------------------------------------
EXPORT_C CFSMailClient::~CFSMailClient()
{
    NM_FUNCTION;
    
	if(iBrandManager)
		{
		delete iBrandManager;
		}
	Close();
}

// -----------------------------------------------------------------------------
// CFSMailClient::GetMailBoxByUidL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBox* CFSMailClient::GetMailBoxByUidL(const TFSMailMsgId aMailBoxId)
{
    NM_FUNCTION;
    
	// select plugin
	CFSMailBox* mailBox = NULL;
	CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aMailBoxId);
	if(plugin)
	{
		// get mailbox from plugin
		mailBox = plugin->GetMailBoxByUidL(aMailBoxId);
	}

	return mailBox;
}

// -----------------------------------------------------------------------------
// CFSMailClient::GetFolderByUidL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailFolder* CFSMailClient::GetFolderByUidL( const TFSMailMsgId aMailBoxId,
													   const TFSMailMsgId aFolderId )
{
    NM_FUNCTION;
    
	CFSMailFolder* folder = NULL;
	
	// select plugin
	CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aFolderId);
	if(plugin)
	{
		// get folder from plugin
		folder = plugin->GetFolderByUidL( aMailBoxId, aFolderId );
	}
	return folder;
}

// -----------------------------------------------------------------------------
// CFSMailClient::GetMessageByUidL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailClient::GetMessageByUidL( const TFSMailMsgId aMailBoxId,
														 const TFSMailMsgId aFolderId,
														 const TFSMailMsgId aMessageId,
														 const TFSMailDetails aDetails)
{
    NM_FUNCTION;
    
	CFSMailMessage* message = NULL;
    // select plugin
    CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aMessageId);
	if(plugin)
	{
	    // get message from plugin
    	message = plugin->GetMessageByUidL( aMailBoxId, aFolderId, aMessageId, aDetails );
	}

    return message;
}

// -----------------------------------------------------------------------------
// CFSMailClient::DeleteMessagesByUidL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::DeleteMessagesByUidL( const TFSMailMsgId aMailBoxId,
												   const TFSMailMsgId aFolderId,
											 	   const RArray<TFSMailMsgId>& aMessages )
{
    NM_FUNCTION;
    
	CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aMailBoxId);
	if(plugin)
	{
		plugin->DeleteMessagesByUidL(aMailBoxId,aFolderId,aMessages);
	}
}

// -----------------------------------------------------------------------------
// CFSMailClient::DeleteMailBoxByUidL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailClient::DeleteMailBoxByUidL( const TFSMailMsgId aMailBoxId,
 								  MFSMailRequestObserver& aOperationObserver )
{
    NM_FUNCTION;
    
	// select plugin
	CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aMailBoxId);
	if(plugin)
	{
		// init asynchronous request
		TFSPendingRequest request = iFWImplementation->GetPluginManager().InitAsyncRequestL(  aMailBoxId.PluginId(),
																		aOperationObserver );

		TRAPD(err, plugin->DeleteMailBoxByUidL(aMailBoxId,*request.iObserver,request.iRequestId));
		
	    if(err != KErrNone)
		{
			iFWImplementation->GetPluginManager().CompleteRequest(request.iRequestId);	
		}
	    return request.iRequestId;
	}
	return 0;
}

// -----------------------------------------------------------------------------
// CFSMailClient::ListMailBoxes
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailClient::ListMailBoxes(const TFSMailMsgId aPlugin,
											 RPointerArray<CFSMailBox>& aMailBoxes)
{
    NM_FUNCTION;

	RArray<TFSMailMsgId> mailBoxList;
	mailBoxList.Reset();
	TInt ret = KErrNone;
	
    // <cmail>	
	CFSMailBox *mailBox = NULL;	
    // </cmail>	
	
	if(aPlugin.IsNullId())
	{
		for(TInt i=0;i<iFWImplementation->GetPluginManager().GetPluginCount();i++)
		{
			CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByIndex(i);	
			TRAPD(err,plugin->ListMailBoxesL(mailBoxList));
			if(err == KErrNone)
			{
				for(TInt ii=0;ii < mailBoxList.Count(); ii++)
				{
				// <cmail>
				    mailBox = NULL;
				    TRAP_IGNORE(
                        mailBox = plugin->GetMailBoxByUidL(mailBoxList[ii]) );
				    if ( mailBox )
    				    {
                        err = aMailBoxes.Append( mailBox );
                        }
				// </cmail>
					if(err != KErrNone)
					{
						ret = err;
					}
				}
			}
			else
			{
				ret = err;
			}
			mailBoxList.Reset();
		}
	}
	else
	{
		CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aPlugin);		
		// <cmail>
		TInt err(KErrNone);
		if ( plugin )
		    {
	        TRAP(err,plugin->ListMailBoxesL(mailBoxList));		    
		    }
		else
		    {
		    err = KErrNotFound;
		    }
        // </cmail>		
		if (err == KErrNone)
		    {
			for(TInt i=0;i < mailBoxList.Count(); i++)
			    {			    
                // <cmail>
                    mailBox = NULL;
				    TRAP_IGNORE(
                        mailBox = plugin->GetMailBoxByUidL(mailBoxList[i]) );
                    if ( mailBox )
                        {
                        err = aMailBoxes.Append( mailBox );
                        }
                // </cmail>				
				if(err != KErrNone)
				    {
					ret = err;
				    }
			    }
		    }
		else
		    {
			ret = err;
		    }
		mailBoxList.Reset();			
	}
	
	mailBoxList.Reset();
	return ret;
}

// -----------------------------------------------------------------------------
// CFSMailClient::ListMessages
// -----------------------------------------------------------------------------
EXPORT_C MFSMailIterator* CFSMailClient::ListMessages(const TFSMailMsgId aMailBoxId,
        						const TFSMailMsgId aFolderId, const TFSMailDetails aDetails,
        						const RArray<TFSMailSortCriteria>& aSorting)
{
    NM_FUNCTION;

	MFSMailIterator* iterator = NULL;
	MFSMailIterator* pluginIterator = NULL;
	
	// select plugin
	if(CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aFolderId))
	{
		// get message list iterator from plugin
		TRAPD(err, pluginIterator = plugin->ListMessagesL(aMailBoxId,aFolderId,aDetails,aSorting));
		if(err == KErrNone && pluginIterator)
		{
			TRAPD(err,iterator = CFSMailIterator::NewL(*pluginIterator,&iFWImplementation->GetPluginManager()));
			if(err == KErrNone)
			{
				return iterator;				
			}
		}
	}

	return iterator;
}
	
// -----------------------------------------------------------------------------
// CFSMailClient::GetBrandManagerL
// -----------------------------------------------------------------------------
EXPORT_C MFSMailBrandManager& CFSMailClient::GetBrandManagerL( void )
{
    NM_FUNCTION;

	if(iBrandManager == NULL)
		{
		TRAPD( creationError,
		       iBrandManager = CFSMailBrandManagerImpl::NewL( *this ) );
		if ( creationError != KErrNone )
		    {
		    User::Leave( creationError );
		    }
		}
		
 	return *iBrandManager;

}

// -----------------------------------------------------------------------------
// CFSMailClient::AddObserverL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::AddObserverL(MFSMailEventObserver& aObserver)
{
    NM_FUNCTION;
    
	for(TInt i=0;i<iFWImplementation->GetPluginManager().GetPluginCount();i++)
		{
		CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByIndex(i);
		if(plugin)
			{
			plugin->AddObserverL(aObserver);			
			}
		}
}

// -----------------------------------------------------------------------------
// CFSMailClient::RemoveObserver
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::RemoveObserver(MFSMailEventObserver& aObserver)
{
    NM_FUNCTION;
    
	for(TInt i=0;i<iFWImplementation->GetPluginManager().GetPluginCount();i++)
	{
		CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByIndex(i);
		if(plugin)
		{
		plugin->RemoveObserver(aObserver);			
		}
	}
}

// -----------------------------------------------------------------------------
// CFSMailClient::UnregisterRequestObserver
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::UnregisterRequestObserver(TInt aRequestId)
{
    NM_FUNCTION;
    
	for(TInt i=0;i<iFWImplementation->GetPluginManager().GetPluginCount();i++)
	{
		if(CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByIndex(i))
			{
			plugin->UnregisterRequestObserver(aRequestId);
			iFWImplementation->GetPluginManager().CompleteRequest(aRequestId);
			}
	}
}

// -----------------------------------------------------------------------------
// CFSMailClient::SubscribeMailboxEventsL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::SubscribeMailboxEventsL(TFSMailMsgId aMailBoxId,
													 	MFSMailEventObserver& aObserver)
	{
    NM_FUNCTION;
    
	// select plugin
	if(CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aMailBoxId))
		{
		// set observer
		plugin->SubscribeMailboxEventsL( aMailBoxId, aObserver );
		}
	}

// -----------------------------------------------------------------------------
// CFSMailClient::UnsubscribeMailboxEvents
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::UnsubscribeMailboxEvents(TFSMailMsgId aMailBoxId,
														MFSMailEventObserver& aObserver)
	{
    NM_FUNCTION;
    
	// select plugin
	if(CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid(aMailBoxId))
		{
		// set observer
		plugin->UnsubscribeMailboxEvents( aMailBoxId, aObserver );
		}
	}

// -----------------------------------------------------------------------------
// CFSMailClient::WizardDataAvailableL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailClient::WizardDataAvailableL()
	{
    NM_FUNCTION;
	
	TInt ret = KErrNone;
	for(TInt i=0;i<iFWImplementation->GetPluginManager().GetPluginCount();i++)
	{
		if(CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByIndex(i))
			{
			TInt rcode = plugin->WizardDataAvailableL( );
			if(rcode != KErrNone)		
				{
				ret = KErrGeneral;			
				}
			}
		else
			{
			ret = KErrGeneral;
			}
	}

	return ret;
	}

// -----------------------------------------------------------------------------
// CFSMailClient::AuthenticateL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailClient::AuthenticateL(MFSMailRequestObserver& aOperationObserver)
	{
    NM_FUNCTION;

	TInt requestId(0);
	
	for(TInt i=0;i<iFWImplementation->GetPluginManager().GetPluginCount();i++)
	{
		if(CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByIndex(i))
		{
		    // get async request id
		    TUid id = iFWImplementation->GetPluginManager().GetPluginIdByIndex(i);
    		TFSPendingRequest request = iFWImplementation->GetPluginManager().InitAsyncRequestL(	id,	aOperationObserver);
    		
    		// send authentication requests
			requestId = request.iRequestId;
			TRAPD(err,plugin->AuthenticateL( *request.iObserver, requestId ));			
			if(err != KErrNone)
			{
				iFWImplementation->GetPluginManager().CompleteRequest(requestId);
			}
		}
	}

	return requestId;

	}

// -----------------------------------------------------------------------------
// CFSMailClient::CleanTempDirL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::CleanTempDirL( )
	{
    NM_FUNCTION;
    
    iFWImplementation->GetPluginManager().CleanTempDirL();	
	}

// -----------------------------------------------------------------------------
// CFSMailClient::GetTempDir
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailClient::GetTempDirL( )
	{
    NM_FUNCTION;
    
	return iFWImplementation->GetPluginManager().GetTempDirL();
	}

// -----------------------------------------------------------------------------
// CFSMailClient::CancelL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::CancelL( const TInt aRequestId )
	{
    NM_FUNCTION;
    
	iFWImplementation->GetPluginManager().CancelRequestL(aRequestId);
	}

// -----------------------------------------------------------------------------
// CFSMailClient::CancelAllL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::CancelAllL( )
	{
    NM_FUNCTION;
    
		iFWImplementation->GetPluginManager().CancelAllRequestsL();
	}

// -----------------------------------------------------------------------------
// CFSMailClient::SetMailboxName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::SetMailboxName( const TFSMailMsgId aMailboxId, const TDesC& aMailboxName )
	{
    NM_FUNCTION;
    
	CFSMailPlugin* plugin = iFWImplementation->GetPluginManager().GetPluginByUid( aMailboxId );
	if ( plugin )
		{
		plugin->SetMailboxName( aMailboxId, aMailboxName );
		}
	}
	
// -----------------------------------------------------------------------------
// CFSMailClient::Close
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::Close()
{
    NM_FUNCTION;
    
	CFSMailClient* instance = Instance();
	if(!instance)
	{
		return;
	}

	if(instance->DecReferenceCount() == 0)
	{
		if(iFWImplementation)
		{
			delete iFWImplementation;
			iFWImplementation = NULL;		
		}
		Dll::FreeTls();
  		delete instance;
	}
}
	
// -----------------------------------------------------------------------------
// CFSMailClient::Instance
// -----------------------------------------------------------------------------
CFSMailClient* CFSMailClient::Instance()
{
    NM_FUNCTION;
    
	return static_cast<CFSMailClient*>(Dll::Tls());
}

// -----------------------------------------------------------------------------
// CFSMailClient::IncReferenceCount
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailClient::IncReferenceCount()
{
    NM_FUNCTION;
    
	return ++iReferenceCount;
}

// -----------------------------------------------------------------------------
// CFSMailClient::DecReferenceCount
// -----------------------------------------------------------------------------
TInt CFSMailClient::DecReferenceCount()
{
    NM_FUNCTION;
    
	return --iReferenceCount;
}

// -----------------------------------------------------------------------------
// CFSMailClient::ReleaseExtension
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailClient::ReleaseExtension( CEmailExtension* aExtension )
    {
    NM_FUNCTION;
    
    CExtendableEmail::ReleaseExtension( aExtension );
    }

// -----------------------------------------------------------------------------
// CFSMailClient::ExtensionL
// -----------------------------------------------------------------------------
EXPORT_C CEmailExtension* CFSMailClient::ExtensionL( const TUid& aInterfaceUid )
    {
    NM_FUNCTION;
    
    return CExtendableEmail::ExtensionL( aInterfaceUid );
    }
    
