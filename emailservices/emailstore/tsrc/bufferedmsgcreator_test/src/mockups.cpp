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
* Description:
*
*/

//mockups below:
#include "containerstore.h" 
#include "containerstorecontentmanager.h"
#include "containerstoreutils.h"


/*static*/ CContainerStore* CContainerStore::NewL()
    {
    CContainerStore* self = new (ELeave) CContainerStore;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }
    
void CContainerStore::ConstructL()
    {
    iContentManager = new (ELeave) CContainerStoreContentManager;
    iUtils = CContainerStoreUtils::NewL();
    }

CContainerStore::CContainerStore() :
    iSequence( 0 ), iInTransaction( EFalse ), iLeaveAfter( -1 )
    {
    }
    
CContainerStore::~CContainerStore()
    {
    delete iUtils;
    delete iContentManager;
    }
    
TContainerId CContainerStore::CreateContainerL(
    TContainerId  aType,
    TContainerId  aParentId, 
    TContainerId  aGrandparentId, 
    const TDesC8& aProperties,
    TContainerId  aId )
    {
    SimulateLeaveL();
    
    return 0;
    }

void CContainerStore::CommitContainerL(
    TContainerId             aId,
    TContainerId             aParentId,
    TContainerId             aMailBoxId,
    MContainerStoreObserver* aObserver,
    TContainerId             aCopiedFromOriginalMsgId )
    {
    SimulateLeaveL();
    }
    
void CContainerStore::BeginDatabaseTransactionLC()
    {
    SimulateLeaveL();
    iInTransaction = ETrue;
    }
    
void CContainerStore::CommitDatabaseTransactionL()
    {
    SimulateLeaveL();
    iInTransaction = EFalse;
    }

void CContainerStore::AllocateIdsBlockL(
    RArray<TContainerId>& aIds,
    TInt aBlockSize )
    {
    SimulateLeaveL();
    for ( TInt i = 0; i < aBlockSize; ++i )
        {
        aIds.Append( ++iSequence );
        }    
    }

TBool CContainerStore::IsEncryptionEnabled()
    {
    return EFalse;
    }
        
CContainerStoreUtils& CContainerStore::StoreUtils()
    {
    return *iUtils;
    }
    
CContainerStoreContentManager& CContainerStore::ContentManager()
    {
    return *iContentManager;
    }
    
const TDesC& CContainerStore::PrivatePath() const
    {
    return iUtils->PrivatePath();
    }

void CContainerStore::SetLeaveAfter( TInt aLeaveAfter )
    {
    iLeaveAfter = aLeaveAfter;
    }

void CContainerStore::SimulateLeaveL()
    {
    if ( 0 == iLeaveAfter )
        {
        User::Leave( KErrGeneral );
        }
    else if ( 0 < iLeaveAfter )
        {
        --iLeaveAfter;
        }
    }

/////////////////////////////////////
// CContainerStoreContentManager   //
/////////////////////////////////////

CContainerStoreContentManager::CContainerStoreContentManager() :
    iLeaveAfter( -1 )
    {
    
    }

void CContainerStoreContentManager::SetLeaveAfter( TInt aLeaveAfter )
    {
    iLeaveAfter = aLeaveAfter;
    }

void CContainerStoreContentManager::SimulateLeaveL()
    {
    if ( 0 == iLeaveAfter )
        {
        User::Leave( KErrGeneral );
        }
    else if ( 0 < iLeaveAfter )
        {
        --iLeaveAfter;
        }
    }

void CContainerStoreContentManager::ReplaceFileWithFileL(
    RFile& aSource,
    const TDesC& aTarget )
    {
    SimulateLeaveL();
    }
    
void CContainerStoreContentManager::PrependBufferAndMoveFileL(
    const TDesC& aSource,
    const TDesC& aTarget,
    const TDesC8& aPrepend )
    {
    SimulateLeaveL();
    }
    
TInt CContainerStoreContentManager::TransferContentFile(
    TContainerId aId, 
    const TDesC& aContentPath )
    {
    return KErrNone;
    }


///////////////////////////////
// CContainerStoreUtils      //
///////////////////////////////

CContainerStoreUtils* CContainerStoreUtils::NewL()
    {

    CContainerStoreUtils* self = new (ELeave) CContainerStoreUtils();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }
    
void CContainerStoreUtils::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    
    User::LeaveIfError( iPrivatePath.Create( KMaxPath ) );
    iFs.PrivatePath( iPrivatePath );
    }
    
CContainerStoreUtils::CContainerStoreUtils()
    {
    
    }
    
CContainerStoreUtils::~CContainerStoreUtils()
    {
    iPrivatePath.Close();
    iFs.Close();
    }
    
const TDesC& CContainerStoreUtils::PrivatePath() const
    {
    return iPrivatePath;
    }

RFs& CContainerStoreUtils::FileSystem()
    {
    return iFs;
    }

void CContainerStoreUtils::LeaveIfLowDiskSpaceL( TUint aBytesToWrite )
    {
    
    }
