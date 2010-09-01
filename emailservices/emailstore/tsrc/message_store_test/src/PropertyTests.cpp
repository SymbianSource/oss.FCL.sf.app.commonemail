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
* Description:  Base class for all test cases class member functions
*
*/



// [INCLUDE FILES] - do not remove
#include <tz.h>
#include "PropertyTests.h"
#include <msgstoreapi.h>
//#include <ismsgstorepropertykeys.h>
#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CPropertyTests* CPropertyTests::NewL( CStifLogger* aLog )
    {
    CPropertyTests* self = new(ELeave) CPropertyTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CPropertyTests::CPropertyTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CPropertyTests::~CPropertyTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CPropertyTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CPropertyTests::ExecuteL()
    {
    LogHeader( _L("Property") );
    
    iLog->Log( _L("Case %d: Manipulation of properties in RAM."), iCaseId++ );
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();

    CheckCondition( _L("default IDs"), properties->Id() == KMsgStoreInvalidId && properties->ParentId() == KMsgStoreInvalidId );

    // "quick" properties
    iLog->Log( _L("Case %d: Add integer properties"), iCaseId++ );
    TUint flagsIndex = TestAddIntegerPropertyL( *properties, KMsgStorePropertyFlags, EMsgStoreFlag_Read_Locally );
    TestAddIntegerPropertyL( *properties, KMsgStorePropertyReplyBy, 1 );
    TestAddIntegerPropertyL( *properties, KMsgStorePropertyCompletedAt, 2 );
    TestAddIntegerPropertyL( *properties, KMsgStorePropertyReceivedAt, 3, ETrue );
    
    // integer
    TUint intIndex1 = TestAddIntegerPropertyL( *properties, _L8("TOTALCONTENTSIZE"), 1000 );
    TUint intIndex2 = TestAddIntegerPropertyL( *properties, _L8("RETRIEVEMORE"), ETrue );
    TUint intIndex3 = TestAddIntegerPropertyL( *properties, _L8("ABC"), 1500 );

    // bool
    iLog->Log( _L("Case %d: Add boolean properties"), iCaseId++ );
    TUint boolIndex1 = TestAddBoolPropertyL( *properties, _L8("BOOL1"), ETrue );
    TUint boolIndex2 = TestAddBoolPropertyL( *properties, _L8("BOOL2"), EFalse, ETrue );
    
    
    // 8-bit descriptor
    iLog->Log( _L("Case %d: Add 8-bit descriptor properties"), iCaseId++ );
    TUint stringIndex1 = TestAddDes8PropertyL( *properties, _L8("SOME OTHER PROPERTY"), _L8("SOME VALUE") );
    TUint stringIndex2 = TestAddDes8PropertyL( *properties, _L8("YET ANOTHER PROPERTY"), _L8("YET ANOTHER VALUE"), ETrue );
    
    // 16-bit descriptor            
    iLog->Log( _L("Case %d: Add 16-bit descriptor properties"), iCaseId++ );
    TUint stringIndex3 = TestAddDesPropertyL( *properties, _L8("XYZ"), _L("sdfhsdgjg") );
    TestAddDesPropertyL( *properties, _L8("XYZ1"), _L("sdfhsdgjg1"), ETrue );

    // container            
    iLog->Log( _L("Case %d: Add container properties"), iCaseId++ );
    CMsgStorePropertyContainer* subproperties = CMsgStorePropertyContainer::NewL();
    TestAddIntegerPropertyL( *subproperties, _L8("TOTALCONTENTSIZE"), 1000 );
    TestAddDes8PropertyL( *subproperties, _L8("SOME OTHER PROPERTY"), _L8("SOME VALUE") );          
    TestAddDesPropertyL( *subproperties, _L8("XYZ"), _L("sdfhsdgjg") );
    
    TUint containerIndex1 = TestAddContainerPropertyL( *properties, _L8("container 1"), *subproperties );
    TUint containerIndex2 = TestAddContainerPropertyL( *properties, _L8("container 2"), *subproperties );
    
    delete subproperties;
    
    // time
    iLog->Log( _L("Case %d: Add TTime properties"), iCaseId++ );
    TTime time1;
    time1.HomeTime();
    TTime time2( 1000 );
    
    TUint timeIndex1 = TestAddTimePropertyL( *properties, _L8("TIME PROPERTY 1"), time1);
    TUint timeIndex2 = TestAddTimePropertyL( *properties, _L8("TIME PROPERTY 2"), time2);
    
    //Address
    iLog->Log( _L("Case %d: Add Address properties"), iCaseId++ );
    RMsgStoreAddress address1, address2;
    CleanupClosePushL( address1 );
    CleanupClosePushL( address2 );
    address1.iEmailAddress.Create(_L("myAddres1@nokia.com"));
    address1.iDisplayName.Create(_L("My Name 1"));
    address2.iEmailAddress.Create(_L("myAddres2@nokia.com"));
    address2.iDisplayName.Create(_L("My Name 2"));
    
    TUint addressIndex1 = TestAddAddressPropertyL( *properties, _L8("ADDRESS PROPERTY 1"), address1);
    TUint addressIndex2 = TestAddAddressPropertyL( *properties, _L8("ADDRESS PROPERTY 2"), address2);
    
    CleanupStack::PopAndDestroy(2);
    
    // Test long property names and values.
    iLog->Log( _L("Case %d: Test long property names and values"), iCaseId++ );
    TBuf8<255> longName;
    RBuf8 longValue;
    longValue.CreateL( 65535 );  // boundary case
    
    longName.Fill( 'N', 255 );
    longValue.Fill( 'V', 65535 );
    
    TUint longIndex = TestAddDes8PropertyL( *properties, longName, longValue );
    
    longValue.Close();
    
    // Test shrinking the size of a property from long to short.
    iLog->Log( _L("Case %d: Test shrinking the size of a property from long to short"), iCaseId++ );
    TestUpdateDes8PropertyL( *properties, longIndex, _L8("ASDFGHJKL:") );   
    
    // Test growing short property back to long property.
    iLog->Log( _L("Case %d: Test growing short property back to long property"), iCaseId++ );
    longValue.CreateL( 70000 );
    longValue.Fill( 'V', 70000 );
    
    TestUpdateDes8PropertyL( *properties, longIndex, longValue );   
    
    longValue.Close();      
    
    // Test update.         
    subproperties = CMsgStorePropertyContainer::NewL();
    TestAddIntegerPropertyL( *subproperties, _L8("X"), 1 );
    
    iLog->Log( _L("Case %d: Update boolean property"), iCaseId++ );
    TestUpdateBoolPropertyL( *properties, boolIndex1, EFalse );
    TestUpdateBoolPropertyL( *properties, boolIndex1, ETrue, ETrue );
    
    iLog->Log( _L("Case %d: Update integer property"), iCaseId++ );
    TestUpdateIntegerPropertyL( *properties, intIndex1, 987 );
    TestUpdateIntegerPropertyL( *properties, intIndex1, 986, ETrue );
    
    iLog->Log( _L("Case %d: Update 8-bit descriptor property"), iCaseId++ );
    TestUpdateDes8PropertyL( *properties, stringIndex1, _L8("ASDFGHJKL:") );
    TestUpdateDes8PropertyL( *properties, stringIndex1, _L8("ASDFGHJKL:1"), ETrue );
    
    iLog->Log( _L("Case %d: Update 16-bit descriptor property"), iCaseId++ );
    TestUpdateDesPropertyL( *properties, stringIndex3, _L(",mxcn,mxbxcbmbnxv") );
    TestUpdateDesPropertyL( *properties, stringIndex3, _L(",mxcn,mxbxcbmbnxv"), ETrue );
    
    iLog->Log( _L("Case %d: Update container property"), iCaseId++ );
    TestUpdateContainerPropertyL( *properties, containerIndex2, *subproperties );

    TestAddIntegerPropertyL( *subproperties, _L8("Y"), 2 );
    TestUpdateContainerPropertyL( *properties, containerIndex2, *subproperties, ETrue );

    delete subproperties;
    
    iLog->Log( _L("Case %d: Update TTime property"), iCaseId++ );
    time2 = 1000000;
    TestUpdateTimePropertyL( *properties, timeIndex1, time2 );
    time2 = 1000000;
    TestUpdateTimePropertyL( *properties, timeIndex1, time2, ETrue );

    iLog->Log( _L("Case %d: Update address property"), iCaseId++ );
    RMsgStoreAddress address3;
    
    CleanupClosePushL(address3);
    
    address3.iDisplayName.Create(_L("new name"));  
    TestUpdateAddressPropertyL( *properties, addressIndex1, address3 );
    address3.iEmailAddress.Create( _L("mail@com")); 
    TestUpdateAddressPropertyL( *properties, addressIndex1, address3, ETrue );
    
    CleanupStack::PopAndDestroy( &address3 );
                
    // Test remove.
    iLog->Log( _L("Case %d: Remove property"), iCaseId++ );
    TestRemovePropertyL( *properties, intIndex3 );
    TestRemovePropertyL( *properties, stringIndex3 );
    
    iLog->Log( _L("Case %d: Store properties as a message and verify stored message"), iCaseId++ );

    CMsgStoreAccount* account = CMsgStoreAccount::NewLC(1234, _L("my account") );  //+1
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account );
    CleanupStack::PushL( mailBox1 );                                               //+2
    
    TMsgStoreId folderId = mailBox1->CreateFolderL( mailBox1->Id(), *properties);

    CMsgStoreMessage* message = mailBox1->CreateMessageL( folderId, *properties );
    CleanupStack::PushL( message );                                               //+3
    message->CommitL();
    TMsgStoreId messageId = message->Id();
    
    ComparePropertiesL( *message, *properties );
    CheckCondition( _L("IDs"), message->Id() != KMsgStoreInvalidId && message->ParentId() == folderId );

    // fetch message
    CMsgStoreMessage* message2 = mailBox1->FetchMessageL( messageId, folderId );
    CleanupStack::PushL( message2 );                                                //+4
    ComparePropertiesL( *message2, *message );
    CheckCondition( _L("IDs"), message2->Id() == message->Id() && message2->ParentId() == message->ParentId() );

    // fetch message without specifying parent ID
    CMsgStoreMessage* message3 = mailBox1->FetchMessageL( messageId, KMsgStoreInvalidId );
    CleanupStack::PushL( message3 );                                                //+5
    ComparePropertiesL( *message3, *message );
    CheckCondition( _L("IDs"), message3->Id() == message->Id() && message3->ParentId() == message->ParentId() );

    iLog->Log( _L("Case %d: Test FetchPropertiesL"), iCaseId++ );
    //create another message that has the same properties as message1
    CMsgStoreMessage* message4 = mailBox1->CreateMessageL( folderId, *properties );
    CleanupStack::PushL( message4 );                                               //+6
    message4->CommitL();
    RArray<TMsgStoreId> ids;
    ids.Append( message->Id() );
    ids.Append( message4->Id() );
    RPointerArray<TDesC8> propNames;
    _LIT8( KAddProp1, "ADDRESS PROPERTY 1" );
    _LIT8( KAddProp2, "ADDRESS PROPERTY 2" );
    propNames.Append( &KAddProp1 );
    propNames.Append( &KAddProp2 );
    RPointerArray<CMsgStorePropertyContainer> propArray;
    mailBox1->FetchPropertiesL( ids, propNames, propArray );
    CheckCondition( _L("PropertyCounts match"), ids.Count() == propArray.Count() );
    ComparePropertiesL( *propArray[0], *propArray[1] );
    ids.Reset();
    propNames.Reset();
    propArray.ResetAndDestroy();
    
    iLog->Log( _L("Case %d: Update message properties and store"), iCaseId++ );
    TestAddIntegerPropertyL( *message, _L8("NEW INT PROPERTY"), 135  );         
    TestAddDes8PropertyL( *message, _L8("NEW STRING PROPERTY"), _L8("NEW VALUE") );
    TestUpdateIntegerPropertyL( *message, intIndex1, 17475 );
    TestUpdateDes8PropertyL( *message, stringIndex1, _L8("zxxcvb") );
    TestRemovePropertyL( *message, intIndex2 );
    TestRemovePropertyL( *message, stringIndex2 );
    
    message->StorePropertiesL();

    // fetch message
    message2 = mailBox1->FetchMessageL( messageId, folderId );
    ComparePropertiesL( *message2, *message );
    CheckCondition( _L("IDs"), message2->Id() == message->Id() && message2->ParentId() == message->ParentId() );
    delete message2;

    iLog->Log( _L("Case %d: Update individual message property and store"), iCaseId++ );
    TestUpdateIntegerPropertyL( *message, flagsIndex, 0 );

    message->StorePropertyL( flagsIndex );
    
    // fetch message
    message2 = mailBox1->FetchMessageL( messageId, folderId );
    ComparePropertiesL( *message2, *message );
    CheckCondition( _L("IDs"), message2->Id() == message->Id() && message2->ParentId() == message->ParentId() );
    delete message2;

    iLog->Log( _L("Case %d: Add property and store individually"), iCaseId++ );
    TMsgStoreId newPropertyIndex = TestAddIntegerPropertyL( *message, _L8("A NEW INTEGER PROPERTY"), 50 );

    message->StorePropertyL( newPropertyIndex );
    
    // fetch message
    message2 = mailBox1->FetchMessageL( messageId, folderId );
    ComparePropertiesL( *message2, *message );
    CheckCondition( _L("IDs"), message2->Id() == message->Id() && message2->ParentId() == message->ParentId() );
    delete message2;

    iLog->Log( _L("Case %d: Update individual message property 64K value to short value"), iCaseId++ );
    message->FindProperty( longName, longIndex );
    
    TestUpdateDes8PropertyL( *message, longIndex, _L8("SHORT VALUE AGAIN") );

    message->StorePropertyL( longIndex );
    
    // fetch message
    message2 = mailBox1->FetchMessageL( messageId, folderId );
    ComparePropertiesL( *message2, *message );
    CheckCondition( _L("IDs"), message2->Id() == message->Id() && message2->ParentId() == message->ParentId() );
    delete message2;

    iLog->Log( _L("Case %d: Update individual message property from short value to >64K value"), iCaseId++ );
    longValue.CreateL( 70000 );
    longValue.Fill( 'V', 70000 );
    TestUpdateDes8PropertyL( *message, longIndex, longValue );

    message->StorePropertyL( longIndex );
    
    longValue.Close();      

    // fetch message
    message2 = mailBox1->FetchMessageL( messageId, folderId );
    ComparePropertiesL( *message2, *message );
    CheckCondition( _L("IDs"), message2->Id() == message->Id() && message2->ParentId() == message->ParentId() );
    delete message2;

    // ERROR PATH TESTS
    iLog->Log( _L("Case %d: Error - Find property that does not exist"), iCaseId++ );
    TUint index = 500;
    TBool found = properties->FindProperty( _L8("SOME PROPERTY THAT DOES NOT EXIST"), index );
    CheckCondition( _L("property not found"), !found && (index == 500) );

    iLog->Log( _L("Case %d: Error - PropertyNameL with an invalid index"), iCaseId++ );
    TRAPD( result, properties->PropertyNameL( 5000 ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    iLog->Log( _L("Case %d: Error - PropertyValueDes8L with an invalid index"), iCaseId++ );
    TRAP( result, properties->PropertyValueDes8L( 5000 ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    iLog->Log( _L("Case %d: Error - PropertyValueUint32L with an invalid index"), iCaseId++ );
    TRAP( result, properties->PropertyValueUint32L( 5000 ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    iLog->Log( _L("Case %d: Error - UpdatePropertyL (string) with an invalid index"), iCaseId++ );
    TRAP( result, properties->UpdatePropertyL( 5000, _L8("abc") ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    iLog->Log( _L("Case %d: Error - UpdatePropertyL (integer) with an invalid index"), iCaseId++ );
    TRAP( result, properties->UpdatePropertyL( 5000, (TUint32)123 ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    iLog->Log( _L("Case %d: Error - UpdatePropertyL (bool) with an invalid index"), iCaseId++ );
    TRAP( result, properties->UpdatePropertyL( 5000, (TBool)123 ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    iLog->Log( _L("Case %d: Error - RemovePropertyL with an invalid index"), iCaseId++ );
    TRAP( result, properties->RemovePropertyL( 5000 ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    iLog->Log( _L("Case %d: Error - Fetch message that does not exist"), iCaseId++ );
    TRAP( result, message2 = mailBox1->FetchMessageL( EMsgStoreMessageBits | 50, folderId ) );
    CheckCondition( _L("message not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - Fetch message exists but is in a different folder"), iCaseId++ );
    TRAP( result, message2 = mailBox1->FetchMessageL( messageId, folderId + 1 ) );
    CheckCondition( _L("message not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - StoreProperty with an invalid property index"), iCaseId++ );
    TRAP( result, message->StorePropertyL( 5000 ) );
    CheckCondition( _L("property not found"), result == KErrNotFound );

    // OUT OF MEMORY ERROR PATH TESTS
    iLog->Log( _L("Case %d: Error - Out of memory, CreateMessageL"), iCaseId++ );
    for( TInt i = 0; i < 2; i++ )
        {        
        SimulateLowDiskSpaceL( i );
        TRAP( result,
            CMsgStoreMessage* msg = mailBox1->CreateMessageL( folderId, *properties );
            CleanupStack::PushL( msg );
            msg->CommitL();
            CleanupStack::PopAndDestroy( msg );
        );
        CheckCondition( _L("out of memory"), result == KErrNoMemory );
        }
        
    iLog->Log( _L("Case %d: Error - Out of memory, StorePropertiesL"), iCaseId++ );
    for( TInt i = 0; i < 2; i++ )
        {        
        SimulateLowDiskSpaceL( i );
        TRAP( result, message->StorePropertiesL() );
        CheckCondition( _L("out of memory"), result == KErrNoMemory );
        }
        
    iLog->Log( _L("Case %d: Error - Out of memory, StorePropertyL"), iCaseId++ );
    for( TInt i = 0; i < 2; i++ )
        {        
        SimulateLowDiskSpaceL( i );
        TRAP( result, message->StorePropertyL( 0 ) );
        CheckCondition( _L("out of memory"), result == KErrNoMemory );
        }
    
    CleanupStack::PopAndDestroy(6);
    
    delete properties;
    
    //we are done 
    return ETrue;
    }

//  End of File
