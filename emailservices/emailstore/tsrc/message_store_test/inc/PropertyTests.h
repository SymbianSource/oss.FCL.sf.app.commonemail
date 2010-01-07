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



#ifndef PROPERTY_TESTS_H
#define PROPERTY_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"

class CPropertyTests : public CAsyncTestCaseBase 
    {
    public:  // Constructors and destructor

        static CPropertyTests* NewL( CStifLogger* aLog );
        
        virtual ~CPropertyTests();

    private:
        
        CPropertyTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
    };

#endif      // PROPERTY_TESTS_H

// End of File
