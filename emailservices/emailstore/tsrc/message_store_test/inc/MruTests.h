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



#ifndef MRU_TESTS_H
#define MRU_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"
#include <msgstoreapi.h>

class CMruTests : public CAsyncTestCaseBase 
    {
    public:  // Constructors and destructor

        static CMruTests* NewL( CStifLogger* aLog );
        
        virtual ~CMruTests();

    private:
        
        CMruTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
        void CompareMruArraysL( CMsgStoreMailBox* aMailBox, RPointerArray<CMsgStoreAddress>& aExpected );
        
    private:
        
    };

#endif      // MRU_TESTS_H

// End of File
