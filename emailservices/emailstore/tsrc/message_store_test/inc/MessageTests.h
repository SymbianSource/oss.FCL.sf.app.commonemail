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



#ifndef MESSAGE_TESTS_H
#define MESSAGE_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"
#include <msgstoreapi.h>

class CMessageTests : public CAsyncTestCaseBase 
    {
    public:  // Constructors and destructor

        static CMessageTests* NewL( CStifLogger* aLog );
        
        virtual ~CMessageTests();

    private:
        
        CMessageTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
        void VerifyMessageIdsL( CMsgStoreMailBox& aMailBox, TMsgStoreId aFolderId, RArray<TMsgStoreId>& expectedIds );
        void DoMessageCountsL( CMsgStoreMailBox& aMailBox );
        
    };

#endif      // MESSAGE_TESTS_H

// End of File
