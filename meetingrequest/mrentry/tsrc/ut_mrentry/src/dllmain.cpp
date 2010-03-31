/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Conflict Checker unit and module test entrypoint
*
*/


// SYSTEM INCLUDES
#include <CEUnitTestSuite.h>

// USER INCLUDES
#include "ut_cesmrconflictchecker.h"
#include "ut_cesmrmeetingrequestentry.h"

// ---------------------------------------------------------------------------
// CreateTestSuiteL
// ---------------------------------------------------------------------------
//
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("MREntry Unit Tests"));
    
    CEUnitTestSuiteClass* testSuite;
    testSuite = UT_CESMRConflictChecker::NewLC();
    rootSuite->AddL( testSuite );
    CleanupStack::Pop( testSuite );
    
    CEUnitTestSuiteClass* testSuite2;
    testSuite2 = UT_CESMRMeetingRequestEntry::NewLC();
    rootSuite->AddL( testSuite2 );
    CleanupStack::Pop( testSuite2 );
    
    CleanupStack::Pop( rootSuite );
    return rootSuite;
    }
