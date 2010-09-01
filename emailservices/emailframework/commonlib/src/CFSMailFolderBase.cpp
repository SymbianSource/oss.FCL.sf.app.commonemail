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
* Description:  common email folder base object
*
*/


//<cmail>
#include "emailtrace.h"
#include "cfsmailfolderbase.h"
//</cmail>

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailFolderBase::CFSMailFolderBase
// -----------------------------------------------------------------------------
CFSMailFolderBase::CFSMailFolderBase()
{
    FUNC_LOG;
	iFolderName = HBufC::New(1);
	iFolderName->Des().Copy(KNullDesC());
	iMoveOfflineBlocked.Reset();
	iMoveOnlineBlocked.Reset();
	iCopyOfflineBlocked.Reset();
	iCopyOnlineBlocked.Reset();
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::~CFSMailFolderBase
// -----------------------------------------------------------------------------
EXPORT_C CFSMailFolderBase::~CFSMailFolderBase()
{
    FUNC_LOG;
	if(iFolderName)
	{
		delete iFolderName;
		iFolderName = NULL;
	}
	iMoveOfflineBlocked.Reset();
	iMoveOnlineBlocked.Reset();
	iCopyOfflineBlocked.Reset();
	iCopyOnlineBlocked.Reset();
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::ConstructL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::ConstructL( TFSMailMsgId aFolderId )
{
    FUNC_LOG;
	iFolderId.SetPluginId(aFolderId.PluginId());
	iFolderId.SetId(aFolderId.Id());
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailFolderBase* CFSMailFolderBase::NewLC( TFSMailMsgId aFolderId )
{
    FUNC_LOG;
  CFSMailFolderBase* folder = new (ELeave) CFSMailFolderBase();
  CleanupStack:: PushL(folder);
  folder->ConstructL(aFolderId);
  return folder;
} 

// -----------------------------------------------------------------------------
// CFSMailFolderBase::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailFolderBase* CFSMailFolderBase::NewL(const TFSMailMsgId aFolderId)
{
    FUNC_LOG;
  CFSMailFolderBase* folder =  CFSMailFolderBase::NewLC(aFolderId);
  CleanupStack:: Pop(folder);
  return folder;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailFolderBase::GetFolderId() const
{
    FUNC_LOG;
	return iFolderId;
}


// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetParentFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailFolderBase::GetParentFolderId() const
{
    FUNC_LOG;
	return iParentFolderId;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetParentFolderId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetParentFolderId(TFSMailMsgId aFolderId)
{
    FUNC_LOG;
	iParentFolderId.SetPluginId(aFolderId.PluginId());
	iParentFolderId.SetId(aFolderId.Id());
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailFolderBase::GetMailBoxId() const
{
    FUNC_LOG;
	return iMailBoxId;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetMailBoxId(TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
	iMailBoxId.SetPluginId(aMailBoxId.PluginId());
	iMailBoxId.SetId(aMailBoxId.Id());
}


// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetFolderType
// -----------------------------------------------------------------------------
EXPORT_C TFSFolderType CFSMailFolderBase::GetFolderType() const
{
    FUNC_LOG;
	return iFolderType;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetFolderType
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetFolderType(const TFSFolderType aFolderType)
{
    FUNC_LOG;
	iFolderType = aFolderType;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetFolderName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailFolderBase::GetFolderName() const
{
    FUNC_LOG;
	return *iFolderName;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetFolderName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetFolderName(const TDesC& aFolderName)
{
    FUNC_LOG;
	// init mailbox name
	HBufC* name = HBufC::New(aFolderName.Length());
	
	// store new mailbox name
	if(name)
	{
		delete iFolderName;
		iFolderName = name;
		iFolderName->Des().Copy(aFolderName);
	}
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetMessageCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetMessageCount() const
{
    FUNC_LOG;
	return iMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetUnreadCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetUnreadCount() const
{
    FUNC_LOG;
	return iUnreadMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetUnseenCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetUnseenCount() const
{
    FUNC_LOG;
	return iUnseenCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetSubFolderCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetSubFolderCount() const
{
    FUNC_LOG;
	return iSubFolderCount;
}
// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetMessageCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetMessageCount( TUint aMessageCount )
{
    FUNC_LOG;
	iMessageCount = aMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetUnreadCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetUnreadCount( TUint aMessageCount )
{
    FUNC_LOG;
	iUnreadMessageCount = aMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetUnseenCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetUnseenCount( TUint aMessageCount )
{
    FUNC_LOG;
	iUnseenCount = aMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetSubFolderCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetSubFolderCount(TUint aFolderCount)
{
    FUNC_LOG;
	iSubFolderCount = aFolderCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolder::BlockCopyFromL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::BlockCopyFromL( RArray<TFSFolderType> aFolderTypes, 
													TFSMailBoxStatus aMailBoxStatus )
	{
    FUNC_LOG;
	switch(aMailBoxStatus)
		{
			case EFSMailBoxOnline:
			for(TInt i=0;i<aFolderTypes.Count();i++)
				{
				iCopyOnlineBlocked.Append(aFolderTypes[i]);				
				}
			break;

			case EFSMailBoxOffline:
			for(TInt i=0;i<aFolderTypes.Count();i++)
				{
				iCopyOfflineBlocked.Append(aFolderTypes[i]);				
				}
			break;
			
			default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CFSMailFolder::BlockMoveFromL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::BlockMoveFromL( RArray<TFSFolderType> aFolderTypes, 
													TFSMailBoxStatus aMailBoxStatus )
	{
    FUNC_LOG;
	switch(aMailBoxStatus)
		{
			case EFSMailBoxOnline:
			for(TInt i=0;i<aFolderTypes.Count();i++)
				{
				iMoveOnlineBlocked.Append(aFolderTypes[i]);				
				}
			break;

			case EFSMailBoxOffline:
			for(TInt i=0;i<aFolderTypes.Count();i++)
				{
				iMoveOfflineBlocked.Append(aFolderTypes[i]);				
				}
			break;
			
			default:
			break;
		}
	
	}

