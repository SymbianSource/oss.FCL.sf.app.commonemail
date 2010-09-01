/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <ceunittestsuite.h>
#include "MT_mrmbutilsextension.h"

/**
* T_HelloWorld.dll test suite factory function.
*/
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    _LIT( KMRMbUtilsExtensionTest, "MRMBUtils extension test" );
    
    CEUnitTestSuite* rootSuite = 
			CEUnitTestSuite::NewLC( KMRMbUtilsExtensionTest );

    // Note that NewLC leaves the object in the cleanupstack.
    MT_MRMbUtilsExtension* moduleTest = 
            MT_MRMbUtilsExtension::NewLC();
    
    rootSuite->AddL( moduleTest );

    CleanupStack::Pop( moduleTest );
    CleanupStack::Pop( rootSuite );

    return rootSuite;
    }
