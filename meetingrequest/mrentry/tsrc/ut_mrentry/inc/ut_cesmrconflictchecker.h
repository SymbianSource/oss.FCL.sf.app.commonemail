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
* Description:  Header file for Conflict Checker unit and module tests
*
*/


#ifndef UT_CESMRConflictChecker_H
#define UT_CESMRConflictChecker_H

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "cmrutils.h"

// FORWARD DECLARATIONS

// CLASS DEFINITION
class CCalSession;
class CESMRCalDbMgr;
class CESMRConflictChecker;

/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CESMRConflictChecker ) : public CEUnitTestSuiteClass,
        public MMRUtilsObserver
    {
    public:  // Constructors and destructor
        
        static UT_CESMRConflictChecker* NewL();
        static UT_CESMRConflictChecker* NewLC();
        ~UT_CESMRConflictChecker();
        
    private: // Constructors
        UT_CESMRConflictChecker();
        void ConstructL();
        
    private: // From MMRUtilsObserver
        
        void HandleCalEngStatus( TMRUtilsCalEngStatus aStatus );
        
    private: // New methods
        
        void SetupL();
        void Teardown();
        
    private: // Test methods
        
        void T_Global_NewL();
        void T_Global_FindConflictsL();
        void T_Global_FindInstancesForEntryL();
        
    private: // Data
        
        // Own. Calendar session.
        CCalSession* iCalSession;
        // Own. Calendar database manager instance.
        CESMRCalDbMgr* iCalDbManager;
        // Own. Conflict checker instance.
        CESMRConflictChecker* iConflictChecker;
        // Own. Wait object for asynchronous operations.
        CActiveSchedulerWait* iWait;
        
        EUNIT_DECLARE_TEST_TABLE;
        
    };

#endif // UT_CESMRConflictChecker_H
