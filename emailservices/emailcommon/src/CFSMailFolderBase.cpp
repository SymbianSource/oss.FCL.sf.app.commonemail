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


#include <nmcommonheaders.h>
#include "emailtrace.h"
#include "CFSMailFolderBase.h"
#include "nmfolder.h"


// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailFolderBase::CFSMailFolderBase
// -----------------------------------------------------------------------------
CFSMailFolderBase::CFSMailFolderBase()
{
    FUNC_LOG;
    // <qmail>
    //iFolderName = HBufC::New(1);
    //iFolderName->Des().Copy(KNullDesC());
    // </qmail>
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
    // <qmail>
    //if(iFolderName)
    //    {
    //    delete iFolderName;
    //    iFolderName = NULL;
    //    }
    // </qmail>
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
    iFolderId = aFolderId;
    iNmPrivateFolder = new NmFolderPrivate();
    iNmPrivateFolder->mFolderId = aFolderId.GetNmId();
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
EXPORT_C CFSMailFolderBase* CFSMailFolderBase::NewL( const TFSMailMsgId aFolderId )
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
	return TFSMailMsgId(iNmPrivateFolder->mFolderId);
}


// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetParentFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailFolderBase::GetParentFolderId() const
{
    FUNC_LOG;
    return TFSMailMsgId(iNmPrivateFolder->mParentFolderId);
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetParentFolderId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetParentFolderId( TFSMailMsgId aFolderId )
{
    FUNC_LOG;
    iNmPrivateFolder->mParentFolderId = aFolderId.GetNmId();
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailFolderBase::GetMailBoxId() const
{
    FUNC_LOG;
	return TFSMailMsgId(iNmPrivateFolder->mMailboxId);
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetMailBoxId( TFSMailMsgId aMailBoxId )
{
    FUNC_LOG;
    iNmPrivateFolder->mMailboxId = aMailBoxId.GetNmId();
}


// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetFolderType
// -----------------------------------------------------------------------------
EXPORT_C TFSFolderType CFSMailFolderBase::GetFolderType() const
{
    FUNC_LOG;
    return static_cast<TFSFolderType>(iNmPrivateFolder->mFolderType);
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetFolderType
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetFolderType( const TFSFolderType aFolderType )
{
    FUNC_LOG;
    iNmPrivateFolder->mFolderType = static_cast<NmFolderType>(aFolderType);
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetFolderName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailFolderBase::GetFolderName() const
{
    FUNC_LOG;
    iTextPtr.Set(
        reinterpret_cast<const TUint16*> (iNmPrivateFolder->mName.utf16()),
        iNmPrivateFolder->mName.length());
    return iTextPtr;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetFolderName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetFolderName( const TDesC& aFolderName )
{
    FUNC_LOG;
  
    QString qtName = QString::fromUtf16(aFolderName.Ptr(), aFolderName.Length());
    iNmPrivateFolder->mName = qtName;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetMessageCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetMessageCount() const
{
    FUNC_LOG;
    return (TUint)iNmPrivateFolder->mMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetUnreadCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetUnreadCount() const
{
    FUNC_LOG;
    return (TUint)iNmPrivateFolder->mUnreadMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetUnseenCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetUnseenCount() const
{
    FUNC_LOG;
    return (TUint)iNmPrivateFolder->mUnseenCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::GetSubFolderCount
// -----------------------------------------------------------------------------
EXPORT_C TUint CFSMailFolderBase::GetSubFolderCount() const
{
    FUNC_LOG;
    return (TUint)iNmPrivateFolder->mSubFolderCount;
}
// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetMessageCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetMessageCount( TUint aMessageCount )
{
    FUNC_LOG;
    iNmPrivateFolder->mMessageCount = aMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetUnreadCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetUnreadCount( TUint aMessageCount )
{
    FUNC_LOG;
    iNmPrivateFolder->mUnreadMessageCount = aMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetUnseenCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetUnseenCount( TUint aMessageCount )
{
    FUNC_LOG;
    iNmPrivateFolder->mUnseenCount = aMessageCount;
}

// -----------------------------------------------------------------------------
// CFSMailFolderBase::SetSubFolderCount
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailFolderBase::SetSubFolderCount( TUint aFolderCount )
{
    FUNC_LOG;
    iNmPrivateFolder->mSubFolderCount = aFolderCount;
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

EXPORT_C NmFolder* CFSMailFolderBase::GetNmFolder()
{
    NmFolder* nmFolder = new NmFolder(iNmPrivateFolder);
    return nmFolder;
}
