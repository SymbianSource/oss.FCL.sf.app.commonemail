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
#include "t_bufferedmsgcreatore.h"
#include "containerstore.h" //mockup
#include "containerstorecontentmanager.h" //mockup
#include "bufferedmessagecreator.h" //real
#include "messagestoreclientserver.h" //real


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
void CallSequence1L( CContainerStore& aStore, CBufferedMessageCreator& aCreator,
    RBuf8& aMsgProps, RBuf8& aMsgProps1, RBuf8& aPartProps, RBuf8& aPartProps1,
    RBuf8& aContent, TMsgStoreId& aSequence );


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
const TCaseInfo Ct_bufferedmsgcreator::Case ( 
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
        ENTRY( "Oom1",              Ct_bufferedmsgcreator::Oom1L ),
        ENTRY( "ErrHandling1",      Ct_bufferedmsgcreator::ErrHandling1L ),
        ENTRY( "ErrHandling2",      Ct_bufferedmsgcreator::ErrHandling2L ),
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
// 
// -----------------------------------------------------------------------------
//
TInt Ct_bufferedmsgcreator::Oom1L( TTestResult& aResult )
    {
    _LIT( KFolderTests, "Oom1" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KFolderTests, KEnter );

    //
    CContainerStore* store = CContainerStore::NewL();
    CleanupStack::PushL( store );
    
    CBufferedMessageCreator* creator
        = CBufferedMessageCreator::NewL( *store ); 
    CleanupStack::PushL( creator );

    //setup
    _LIT8( KTestProperties, "Test123,123,123" );
    _LIT8( KTestContent, "Content123,123,123" );
    TMsgStoreId sequence = 0;
    
    TMsgStoreCreateContainerCmdParams msgParams={0};
    msgParams.iType = EMsgStoreMessageBits;
    TMsgStoreCreateContainerCmdParams partParams={0};
    partParams.iType = EMsgStorePartBits;
    
    //main loop
    for ( TInt i = 0; i < 100; ++i )
        {
        RBuf8 msgProps;
        msgProps.CreateL( KTestProperties );
        msgProps.CleanupClosePushL();

        RBuf8 msgProps1;
        msgProps1.CreateL( KTestProperties );
        msgProps1.CleanupClosePushL();

        RBuf8 partProps;
        partProps.CreateL( KTestProperties );
        partProps.CleanupClosePushL();

        RBuf8 partProps1;
        partProps1.CreateL( KTestProperties );
        partProps1.CleanupClosePushL();

        RBuf8 content;
        content.CreateL( KTestContent );
        content.CleanupClosePushL();
    
        __UHEAP_SETFAIL( RAllocator::EDeterministic, i );

        TRAP_IGNORE(
            ::CallSequence1L( *store, *creator, msgProps, msgProps1, partProps,
                 partProps1, content, sequence );
        );
        
        __UHEAP_RESET;
        
        CleanupStack::PopAndDestroy( &content );
        CleanupStack::Pop( &partProps1 );
        CleanupStack::Pop( &partProps );
        CleanupStack::Pop( &msgProps1 );
        CleanupStack::Pop( &msgProps );
        }
    
    CleanupStack::PopAndDestroy( creator );
    CleanupStack::PopAndDestroy( store );
    
    TestModuleIf().Printf( 0, KFolderTests, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    TInt result = KErrNone;
    _LIT( KPassDescription, "Oom1 passed" );
    _LIT( KFailDescription, "Oom1 failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }


TInt Ct_bufferedmsgcreator::ErrHandling1L( TTestResult& aResult )
    {
    _LIT( KTestName, "ErrHandling1" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KTestName, KEnter );

    //
    CContainerStore* store = CContainerStore::NewL();
    CleanupStack::PushL( store );
    
    CBufferedMessageCreator* creator
        = CBufferedMessageCreator::NewL( *store ); 
    CleanupStack::PushL( creator );

    //setup
    _LIT8( KTestProperties, "Test123,123,123" );
    _LIT8( KTestContent, "Content123,123,123" );
    TMsgStoreId sequence = 0;
    
    //main loop
    for ( TInt i = 0; i < 100; ++i )
        {
        RBuf8 msgProps;
        msgProps.CreateL( KTestProperties );
        msgProps.CleanupClosePushL();

        RBuf8 msgProps1;
        msgProps1.CreateL( KTestProperties );
        msgProps1.CleanupClosePushL();

        RBuf8 partProps;
        partProps.CreateL( KTestProperties );
        partProps.CleanupClosePushL();

        RBuf8 partProps1;
        partProps1.CreateL( KTestProperties );
        partProps1.CleanupClosePushL();

        RBuf8 content;
        content.CreateL( KTestContent );
        content.CleanupClosePushL();
    
        store->SetLeaveAfter( i );

        TRAP_IGNORE(
            ::CallSequence1L( *store, *creator, msgProps, msgProps1, partProps,
                 partProps1, content, sequence );
        );        
        
        CleanupStack::PopAndDestroy( &content );
        CleanupStack::Pop( &partProps1 );
        CleanupStack::Pop( &partProps );
        CleanupStack::Pop( &msgProps1 );
        CleanupStack::Pop( &msgProps );
        }
    
    CleanupStack::PopAndDestroy( creator );
    CleanupStack::PopAndDestroy( store );
    
    TestModuleIf().Printf( 0, KTestName, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    TInt result = KErrNone;
    _LIT( KPassDescription, "ErrHandling1 passed" );
    _LIT( KFailDescription, "ErrHandling1 failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }


TInt Ct_bufferedmsgcreator::ErrHandling2L( TTestResult& aResult )
    {
    _LIT( KTestName, "ErrHandling2" );
    _LIT( KEnter, "Enter" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KTestName, KEnter );

    //
    CContainerStore* store = CContainerStore::NewL();
    CleanupStack::PushL( store );
    
    CBufferedMessageCreator* creator
        = CBufferedMessageCreator::NewL( *store ); 
    CleanupStack::PushL( creator );

    //setup
    _LIT8( KTestProperties, "Test123,123,123" );
    _LIT8( KTestContent, "Content123,123,123" );
    TMsgStoreId sequence = 0;
    
    //main loop
    for ( TInt i = 0; i < 10; ++i )
        {
        RBuf8 msgProps;
        msgProps.CreateL( KTestProperties );
        msgProps.CleanupClosePushL();

        RBuf8 msgProps1;
        msgProps1.CreateL( KTestProperties );
        msgProps1.CleanupClosePushL();

        RBuf8 partProps;
        partProps.CreateL( KTestProperties );
        partProps.CleanupClosePushL();

        RBuf8 partProps1;
        partProps1.CreateL( KTestProperties );
        partProps1.CleanupClosePushL();

        RBuf8 content;
        content.CreateL( KTestContent );
        content.CleanupClosePushL();
    
        store->ContentManager().SetLeaveAfter( i );

        TRAP_IGNORE(
            ::CallSequence1L( *store, *creator, msgProps, msgProps1, partProps,
                 partProps1, content, sequence );
        );        
        
        CleanupStack::PopAndDestroy( &content );
        CleanupStack::Pop( &partProps1 );
        CleanupStack::Pop( &partProps );
        CleanupStack::Pop( &msgProps1 );
        CleanupStack::Pop( &msgProps );
        }
    
    CleanupStack::PopAndDestroy( creator );
    CleanupStack::PopAndDestroy( store );
    
    TestModuleIf().Printf( 0, KTestName, KExit );
    
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    TInt result = KErrNone;
    _LIT( KPassDescription, "ErrHandling2 passed" );
    _LIT( KFailDescription, "ErrHandling2 failed" );
    aResult.SetResult( result, (result==KErrNone ? KPassDescription : KFailDescription) );

    // Case was executed
    return KErrNone;
    }


void CallSequence1L( CContainerStore& aStore, CBufferedMessageCreator& aCreator,
    RBuf8& aMsgProps, RBuf8& aMsgProps1, RBuf8& aPartProps, RBuf8& aPartProps1,
    RBuf8& aContent, TMsgStoreId& aSequence )
    {
    TMsgStoreCreateContainerCmdParams msgParams={0};
    msgParams.iType = EMsgStoreMessageBits;
    TMsgStoreCreateContainerCmdParams partParams={0};
    partParams.iType = EMsgStorePartBits;

    msgParams.iId = ++aSequence;

    aCreator.EnqueueL(
            aStore,
            msgParams,
            aMsgProps );
    
    aCreator.AppendContentL( aStore, msgParams.iId, aContent );
    aCreator.PrependContentL( aStore, msgParams.iId, aContent );
    aCreator.ReplaceContentL( aStore, msgParams.iId, aContent );
    
    aCreator.AbandonMessageL( aStore, msgParams.iId );

    partParams.iId = ++aSequence;
    partParams.iParentId = msgParams.iId;

    aCreator.EnqueueL(
            aStore,
            partParams,
            aPartProps );
    
    aCreator.AppendContentL( aStore, partParams.iId, aContent );
    aCreator.PrependContentL( aStore, partParams.iId, aContent );
    aCreator.ReplaceContentL( aStore, partParams.iId, aContent );

    //
    aCreator.ResetL( aStore );
    
    //
    msgParams.iId = ++aSequence;

    aCreator.EnqueueL(
            aStore,
            msgParams,
            aMsgProps1 );
    
    aCreator.AppendContentL( aStore, msgParams.iId, aContent );
    aCreator.PrependContentL( aStore, msgParams.iId, aContent );
    aCreator.ReplaceContentL( aStore, msgParams.iId, aContent );

    partParams.iId = ++aSequence;
    partParams.iParentId = msgParams.iId;

    aCreator.EnqueueL(
            aStore,
            partParams,
            aPartProps1 );

    aCreator.AppendContentL( aStore, partParams.iId, aContent );
    aCreator.PrependContentL( aStore, partParams.iId, aContent );
    aCreator.ReplaceContentL( aStore, partParams.iId, aContent );
    
    //
    aCreator.CommitMessageL( aStore, &aStore );
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
