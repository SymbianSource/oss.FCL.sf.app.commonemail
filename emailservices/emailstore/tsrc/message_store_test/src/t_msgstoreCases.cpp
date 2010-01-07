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
* Description:  implementation of test cases methods.
*
*/



// [INCLUDE FILES] - do not remove
#include <e32math.h>
#include "t_msgstore.h"
#include "mailboxtests.h"
#include "propertytests.h"
#include "foldertests.h"
#include "messagetests.h"
#include "parttests.h"
#include "observertests.h"
#include "contenttests.h"
#include "mrutests.h"
#include "searchtests.h"
#include "sorttests.h"
#include "sizetests.h"
#include "DbCorruptTests.h"
#include "shutdownservertest.h"
#include "EncryptionTests.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ct_msgstore::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo Ct_msgstore::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    t_msgstore.cpp file and to t_msgstore.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // [test cases entries] - do not remove
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // Ct_msgstore::PrintTest. Otherwise the compiler
        // gives errors.
        ENTRY( "Mailbox Tests",     Ct_msgstore::MailboxTestsL ),
        ENTRY( "Property Tests",    Ct_msgstore::PropertyTestsL ),
        ENTRY( "Folder Tests",      Ct_msgstore::FolderTestsL ),
        ENTRY( "Message Tests",     Ct_msgstore::MessageTestsL ),
        ENTRY( "Database Corruption Test", Ct_msgstore::DbCorruptTestsL ),
        ENTRY( "Part Tests",        Ct_msgstore::PartTestsL ),
        ENTRY( "Observer Tests",    Ct_msgstore::ObserverTestsL ),
        ENTRY( "Content Tests",     Ct_msgstore::ContentTestsL ),
        ENTRY( "Mru Tests",         Ct_msgstore::MruTestsL ),
        ENTRY( "Search Tests",      Ct_msgstore::SearchTestsL ),
        ENTRY( "Sort Tests",        Ct_msgstore::SortTestsL ),
        ENTRY( "Encryption Tests",  Ct_msgstore::EncryptionTestsL ),
        ENTRY( "Size Tests",        Ct_msgstore::SizeTestsL ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", Ct_msgstore::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( Ct_msgstore::PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

// -----------------------------------------------------------------------------
// Ct_msgstore::MailboxTests
// Test Mailbox operations.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::MailboxTestsL( TTestResult& aResult )
    {
    _LIT( KMailboxTests, "Mailbox Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KMailboxTests, KEnter );
           
    // Run test cases
    CMailboxTests* testCase = CMailboxTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KMailboxTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Mailbox tests passed" );
    _LIT( KFailDescription, "Mailbox tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::PropertyTests
// Test Property related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::PropertyTestsL( TTestResult& aResult )
    {
    _LIT( KPropertyTests, "Property Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPropertyTests, KEnter );
           
    // Run test cases
    CPropertyTests* testCase = CPropertyTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KPropertyTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Property tests passed" );
    _LIT( KFailDescription, "Property tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::FolderTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::FolderTestsL( TTestResult& aResult )
    {
    _LIT( KFolderTests, "Folder Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KFolderTests, KEnter );
           
    // Run test cases
    CFolderTests* testCase = CFolderTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KFolderTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Folder tests passed" );
    _LIT( KFailDescription, "Folder tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::MessageTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::MessageTestsL( TTestResult& aResult )
    {
    _LIT( KMessageTests, "Message Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KMessageTests, KEnter );
           
    // Run test cases
    CMessageTests* testCase = CMessageTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KMessageTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Message tests passed" );
    _LIT( KFailDescription, "Message tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// Ct_msgstore::PartTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::PartTestsL( TTestResult& aResult )
    {
    _LIT( KPartTests, "Part Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPartTests, KEnter );
           
    // Run test cases
    CPartTests* testCase = CPartTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KPartTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Part tests passed" );
    _LIT( KFailDescription, "Part tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::ObserverTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::ObserverTestsL( TTestResult& aResult )
    {
    _LIT( KObserverTests, "Observer Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KObserverTests, KEnter );
           
    // Run test cases
    CObserverTests* testCase = CObserverTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KObserverTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Observer tests passed" );
    _LIT( KFailDescription, "Observer tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::ContentTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::ContentTestsL( TTestResult& aResult )
    {
    _LIT( KContentTests, "Content Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KContentTests, KEnter );
           
    // Run test cases
    CContentTests* testCase = CContentTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KContentTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Content tests passed" );
    _LIT( KFailDescription, "Content tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// Ct_msgstore::MruTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::MruTestsL( TTestResult& aResult )
    {
    _LIT( KMruTests, "Mru Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KMruTests, KEnter );
           
    // Run test cases
    CMruTests* testCase = CMruTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KMruTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "MRU tests passed" );
    _LIT( KFailDescription, "MRU tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::SearchTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::SearchTestsL( TTestResult& aResult )
    {
    _LIT( KSearchTests, "Search Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KSearchTests, KEnter );
           
    // Run test cases
    CSearchTests* testCase = CSearchTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KSearchTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Search tests passed" );
    _LIT( KFailDescription, "Search tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::SortTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::SortTestsL( TTestResult& aResult )
    {
    _LIT( KSortTests, "Sort Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KSortTests, KEnter );
           
    // Run test cases
    CSortTests* testCase = CSortTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KSortTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Sort tests passed" );
    _LIT( KFailDescription, "Sort tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::SizeTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::EncryptionTestsL( TTestResult& aResult )
    {
    _LIT( KEncryptionTests, "Encryption Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KEncryptionTests, KEnter );
           
    // Run test cases
    CEncryptionTests* testCase = CEncryptionTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KEncryptionTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Encryption tests passed" );
    _LIT( KFailDescription, "Encryption tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::SizeTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::SizeTestsL( TTestResult& aResult )
    {
    _LIT( KSizeTests, "Size Tests" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KSizeTests, KEnter );
           
    // Run test cases
    CSizeTests* testCase = CSizeTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KSizeTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Size tests passed" );
    _LIT( KFailDescription, "Size tests failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Ct_msgstore::DbCorruptTests
// Test Folder related features.
// -----------------------------------------------------------------------------
//
TInt Ct_msgstore::DbCorruptTestsL( TTestResult& aResult )
    {
    _LIT( KDbCorruptTests, "Database Corruption Test" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KDbCorruptTests, KEnter );
           
    // Run test cases
    CDbCorruptTests* testCase = CDbCorruptTests::NewL( iLog );
    CActiveScheduler::Start();
    
    TInt result = testCase->Result();    
    
    delete testCase;
    
    TestModuleIf().Printf( 0, KDbCorruptTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KPassDescription, "Database Corruption Tests test passed" );
    _LIT( KFailDescription, "Database Corruption Tests test failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
/*
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  [End of File] - do not remove
