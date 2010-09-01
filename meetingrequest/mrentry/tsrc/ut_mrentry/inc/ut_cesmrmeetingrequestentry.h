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
* Description:  Header file for MR Entry unit and module tests
*
*/


#ifndef UT_CESMRMeetingRequestEntry_H
#define UT_CESMRMeetingRequestEntry_H

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include <cmrutils.h>

// FORWARD DECLARATIONS
class CCalEntry;
class CMRMailboxUtils;
class CESMRCalDbMgr;
class CCalSession;
class CActiveSchedulerWait;
class CESMRMeetingRequestEntry;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CESMRMeetingRequestEntry ) : public CEUnitTestSuiteClass, MMRUtilsObserver
    {
    public:  // Constructors and destructor

        static UT_CESMRMeetingRequestEntry* NewL();
        static UT_CESMRMeetingRequestEntry* NewLC();
        ~UT_CESMRMeetingRequestEntry();
        
    public:
        void ExecuteL();
        
    protected: // From MMRUtilsObserver
        void HandleCalEngStatus( TMRUtilsCalEngStatus aStatus );
        void HandleOperation(
                TInt aType,
                TInt aPercentageCompleted,
                TInt aStatus );
        
    private: // Constructors
        UT_CESMRMeetingRequestEntry();
        void ConstructL();        
        
        
    private: // New methods

         void SetupL();
         void Teardown();
         
         /**
          * Utility functions
          */
         void CreateCalEntryL();
         void CreateMREntryL();
         CCalEntry* CCalEntryL(); //ownership transfered to caller
         
         /**
          * Test functions
          */
         void T_Global_CESMRMeetingRequestEntry_NewL();
         void T_ValidateEntryL_1();
         void T_ValidateEntryL_2();
         void T_EntryAttendeeInfoL();
         void T_GetRecurrenceL();
         
    private: //members
        
        CESMRMeetingRequestEntry* iMrEntry;
        CCalEntry* iEntry;
        CMRMailboxUtils* iMRMailboxUtils;
        CESMRCalDbMgr* iCalDbMgr;
        TBool iConflictsExists;
        CCalSession* iCalSession;
         
        /// Own: MR UTILS syncher
        CActiveSchedulerWait* iCtrlSyncher;
        /// Own: Operation error value
        TInt iExecutionError;
        
    private: // Data
       
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  UT_CESMRMeetingRequestEntry_H
