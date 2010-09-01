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
* Description:  Mockup Container store.
*
*/
#ifndef __CONTAINER_STORE_H__
#define __CONTAINER_STORE_H__

#include "msgstoretypes.h"
#include <e32base.h>

typedef TUint TContainerId;    
const TContainerId KContainerInvalidId = 0;

class CContainerStoreContentManager;
class CContainerStoreUtils;


NONSHARABLE_CLASS( MContainerStoreObserver )
    {
    
    };


/**
 * 
 */
NONSHARABLE_CLASS( CContainerStore ) :
    public CBase,
    public MContainerStoreObserver
    
	{
public:
    friend class CBufferedMessageCreator;
    
    static CContainerStore* NewL();
    
    void ConstructL();
    
    CContainerStore();
    
    ~CContainerStore();
    
    TContainerId CreateContainerL( TContainerId  aType,
                                   TContainerId  aParentId, 
                                   TContainerId  aGrandparentId, 
                                   const TDesC8& aProperties,
                                   TContainerId  aId = KMsgStoreInvalidId );

    void CommitContainerL( TContainerId             aId,
                           TContainerId             aParentId,
                           TContainerId             aMailBoxId,
                           MContainerStoreObserver* aObserver,
                           TContainerId             aCopiedFromOriginalMsgId = KContainerInvalidId );
    
    void BeginDatabaseTransactionLC();
    
    void CommitDatabaseTransactionL();

    void AllocateIdsBlockL(
        RArray<TContainerId>& aIds,
        TInt aBlockSize = 4 );

    TBool IsEncryptionEnabled();
		
    CContainerStoreUtils& StoreUtils();
    
    CContainerStoreContentManager& ContentManager();
    
    const TDesC& PrivatePath() const;
    
    void SetLeaveAfter( TInt aLeaveAfter );
    
    void SimulateLeaveL();

public:    
    CContainerStoreContentManager* iContentManager;
    CContainerStoreUtils* iUtils;
    TContainerId iSequence;
    TBool iInTransaction;
    TInt iLeaveAfter;
	};

	
#endif // __CONTAINER_STORE_H__

