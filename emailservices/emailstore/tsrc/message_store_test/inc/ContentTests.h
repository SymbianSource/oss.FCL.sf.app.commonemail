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



#ifndef CONTENT_TESTS_H
#define CONTENT_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"
#include <msgstoreapi.h>

class CContentTests : public CAsyncTestCaseBase 
    {
    public:  // Constructors and destructor

        static CContentTests* NewL( CStifLogger* aLog );
        
        virtual ~CContentTests();

    private:
        
        CContentTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
    private:
        
    };

#endif      // CONTENT_TESTS_H

// End of File
