/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  InitTestCase test module.
*
*/



#ifndef ENCRYPTION_TESTS_H
#define ENCRYPTION_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"
#include <msgstoreapi.h>

class CEncryptionTests : public CAsyncTestCaseBase 
    {
    public:  // Constructors and destructor

        static CEncryptionTests* NewL( CStifLogger* aLog );
        
        virtual ~CEncryptionTests();

    private:
        
        CEncryptionTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
        void WaitUntilEncryptDecryptCompleteL( TInt aCurrentState, CMsgStoreMailBox* aMailBox );
        void CompareMessagesL( CMsgStoreMailBox* aMailBox );
        void CompareAccountsL( CMsgStore* aSession, RPointerArray<CMsgStoreAccount>& aAccounts );
        void GetSortedIdsL( CMsgStoreMailBox* aMailBox, TMsgStoreId aFolderId, TMsgStoreSortByField aSortedBy, TInt aCount, RArray<TMsgStoreId>& aSortedIds );
        void CompareSortedIdArraysL( RArray<TMsgStoreId>& aIDs1, RArray<TMsgStoreId>& aIDs2 );
        
    private:
        RArray<TPtrC8> iTestContentArray;
        
    };

#endif      // ENCRYPTION_TESTS_H

// End of File
