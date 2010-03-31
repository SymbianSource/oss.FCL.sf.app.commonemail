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
#include "UT_cmrmbutilssettings.h"

EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    _LIT( KMRMBUtilsSettings, "CMRMBUtilsSettings test" );
    
    CEUnitTestSuite* rootSuite = 
            CEUnitTestSuite::NewLC( KMRMBUtilsSettings );

    // Note that NewLC leaves the object in the cleanupstack.
    UT_CMRMBUtilsSettings* unitTest = UT_CMRMBUtilsSettings::NewLC();    
    rootSuite->AddL( unitTest );
    CleanupStack::Pop( unitTest );

    CleanupStack::Pop( rootSuite );

    return rootSuite;
    }
