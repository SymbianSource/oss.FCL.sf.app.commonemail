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

// <qmail>
#include <nmcommonheaders.h>
// </qmail>

#include "emailtrace.h"

// <qmail>
#include "CFSMailFolderBase.h"
#include "nmfolder.h"
// </qmail>

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailFolderBase::CFSMailFolderBase
// -----------------------------------------------------------------------------
CFSMailFolderBase::CFSMailFolderBase()
{
    FUNC_LOG;
// <qmail> unnecessary iFolderName initialization removed </qmail>
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
// <qmail> unnecessary iFolderName destruction removed </qmail>
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
// <qmail>
    iNmPrivateFolder = new NmFolderPrivate();
    iNmPrivateFolder->mFolderId = aFolderId.GetNmId();
// <qmail>
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
// <qmail>
	return TFSMailMsgId(iNmPrivateFolder->mFolderId);
// </qmail>
}


// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetParentFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailFolderBase::GetParentFolderId() const
{
    FUNC_LOG;
// <qmail>
    return TFSMailMsgId(iNmPrivateFolder->mParentFolderId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetParentFolderId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetParentFolderId(TFSMailMsgId aFolderId)
{
    FUNC_LOG;
// <qmail>
    iNmPrivateFolder->mParentFolderId = aFolderId.GetNmId();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailFolderBase::GetMailBoxId() const
{
    FUNC_LOG;
// <qmail>
	return TFSMailMsgId(iNmPrivateFolder->mMailboxId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetMailBoxId(TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
// <qmail>
    iNmPrivateFolder->mMailboxId = aMailBoxId.GetNmId();
// </qmail>
}


// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetFolderType
// -----------------------------------------------------------------------------
EXPORT_C TFSFolderType CFSMailFolderBase::GetFolderType() const
{
    FUNC_LOG;
// <qmail>
    return static_cast<TFSFolderType>(iNmPrivateFolder->mFolderType);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetFolderType
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetFolderType(const TFSFolderType aFolderType)
{
    FUNC_LOG;
// <qmail>
    iNmPrivateFolder->mFolderType = static_cast<NmFolderType>(aFolderType);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetFolderName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailFolderBase::GetFolderName() const
{
    FUNC_LOG;
// <qmail>
    iTextPtr.Set(
        reinterpret_cast<const TUint16*> (iNmPrivateFolder->mName.utf16()),
        iNmPrivateFolder->mName.length());
    return iTextPtr;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetFolderName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetFolderName(const TDesC& aFolderName)
{
    FUNC_LOG;
// <qmail>
    QString qtName = QString::fromUtf16(aFolderName.Ptr(), aFolderName.Length());
    iNmPrivateFolder->mName = qtName;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetMessageCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetMessageCount() const
{
    FUNC_LOG;
// <qmail>
    return (TUint)iNmPrivateFolder->mMessageCount;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetUnreadCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetUnreadCount() const
{
    FUNC_LOG;
// <qmail>
    return (TUint)iNmPrivateFolder->mUnreadMessageCount;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetUnseenCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetUnseenCount() const
{
    FUNC_LOG;
// <qmail>
    return (TUint)iNmPrivateFolder->mUnseenCount;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetSubFolderCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetSubFolderCount() const
{
    FUNC_LOG;
// <qmail>
    return (TUint)iNmPrivateFolder->mSubFolderCount;
// </qmail>
}
// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetMessageCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetMessageCount( TUint aMessageCount )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateFolder->mMessageCount = aMessageCount;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetUnreadCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetUnreadCount( TUint aMessageCount )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateFolder->mUnreadMessageCount = aMessageCount;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetUnseenCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetUnseenCount( TUint aMessageCount )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateFolder->mUnseenCount = aMessageCount;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetSubFolderCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetSubFolderCount(TUint aFolderCount)
{
    FUNC_LOG;
// <qmail>
    iNmPrivateFolder->mSubFolderCount = aFolderCount;
// </qmail>
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

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetNmFolder
// -----------------------------------------------------------------------------
EXPORT_C NmFolder* CFSMailFolderBase::GetNmFolder()
{
    NmFolder* nmFolder = new NmFolder(iNmPrivateFolder);
    return nmFolder;
}
// </qmail>

