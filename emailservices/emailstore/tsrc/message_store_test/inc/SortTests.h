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



#ifndef SORT_TESTS_H
#define SORT_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"
#include <msgstoreapi.h>

class CSortTests : public CAsyncTestCaseBase 
    {
    public:  // Constructors and destructor

        static CSortTests* NewL( CStifLogger* aLog );
        
        virtual ~CSortTests();

    private:
        
        CSortTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
        void DoCreateMessagesL( TInt aMsgCount );
        
        TMsgStoreId CompareSortResultsL( RMsgStoreSortCriteria aCriteria, 
                                         TMsgStoreIteratorDirection aDirection, 
                                         const RPointerArray<CMsgStorePropertyContainer>& aResults );
        
                 
    private:
        
    };

#endif      // SORT_TESTS_H

// End of File
