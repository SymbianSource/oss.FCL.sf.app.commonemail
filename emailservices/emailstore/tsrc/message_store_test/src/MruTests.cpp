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


//#include <BAUTILS.H>
#include "MruTests.h"
//#include <ismsgstorepropertykeys.h>
#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CMruTests* CMruTests::NewL( CStifLogger* aLog )
    {
    CMruTests* self = new(ELeave) CMruTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CMruTests::CMruTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMruTests::~CMruTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CMruTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CMruTests::ExecuteL()
    {
    LogHeader( _L("MRU") );
    
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewL(1234, _L("Account 1") );
    CleanupStack::PushL( account1 );                                            //+1
    
    CMsgStoreAccount* account2 = CMsgStoreAccount::NewL(1234, _L("") );
    CleanupStack::PushL( account2 );                                            //+2
    
    CMsgStoreAccount* account3 = CMsgStoreAccount::NewL(5678, _L("Account 3") );
    CleanupStack::PushL( account3 );                                            //+3
    
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                            //+4
    
    CMsgStoreMailBox* mailBox2 = iMsgStore->CreateAccountL( *account2 );
    CleanupStack::PushL( mailBox2 );                                            //+5
    
    CMsgStoreMailBox* mailBox3 = iMsgStore->CreateAccountL( *account3 );
    CleanupStack::PushL( mailBox3 );                                            //+6

    iLog->Log( _L("Case %d: Calling MruAddressesL() on empty mailbox"), iCaseId++ );
    RPointerArray<CMsgStoreAddress> expectedArray;
    CompareMruArraysL( mailBox2, expectedArray );
    
    RPointerArray<CMsgStoreAddress> inputArray;
    
    TBuf<100> addrBuf;
    TBuf<100> nameBuf;
    
    for ( TInt i = 0 ; i < 20 ; i++ )
        {
        addrBuf.Format(_L("name%d@nok%d.com"), i, i );
        nameBuf.Format(_L("Fname%d LName%d"), i, i );
        if ( i == 3 || i == 6 )
            {
            nameBuf.Copy( KNullDesC );
            }
        CMsgStoreAddress* address = CMsgStoreAddress::NewL( addrBuf, nameBuf );
        inputArray.Append( address );
        expectedArray.Append( address );
        }
    
    TRAPD( err, mailBox1->AddMruAddressesL( inputArray ) );
    CheckCondition( _L("AddMruAddressesL() ok"), err == KErrNone );
    
    CompareMruArraysL( mailBox1, expectedArray );
    
    inputArray.Reset();  //do not destroy because the memory is also in the expectedArray
   
    iLog->Log( _L("Case %d: Add more address to the top of MRU address list"), iCaseId++ );
    for ( TInt i = 0 ; i < 20 ; i++ )
        {
        addrBuf.Format(_L("new name%d@nok%d.com"), i, i );
        nameBuf.Format(_L("new Fname%d LName%d"), i, i );
        if ( i == 3 || i == 7 )
            {
            nameBuf.Copy( KNullDesC );
            }
        CMsgStoreAddress* address = CMsgStoreAddress::NewL( addrBuf, nameBuf );
        inputArray.Append( address );
        //add the new addresss to the top of the array in the correct order
        expectedArray.Insert( address, i );
        }
    
    iLog->Log( _L("Case %d: Calling AddMruAddressesL()"), iCaseId++ );
    TRAP( err, mailBox1->AddMruAddressesL( inputArray ) );
    CheckCondition( _L("AddMruAddressesL() ok"), err == KErrNone );
    
    CompareMruArraysL( mailBox1, expectedArray );
    
    iLog->Log( _L("Case %d: Promoting some address to top and add some to the top"), iCaseId++ );
    inputArray.Reset();
    for ( TInt i = 0 ; i < expectedArray.Count() ; i += 3 )
        {
        CMsgStoreAddress* address = expectedArray[i];
        inputArray.Insert( address, 0 );
        expectedArray.Remove(i);
        expectedArray.Insert( address, 0 );
        }
    for ( TInt i = 40 ; i < 43 ; i++ )
        {
        addrBuf.Format(_L("new name%d@nok%d.com"), i, i );
        nameBuf.Format(_L("new Fname%d LName%d"), i, i );
        CMsgStoreAddress* address = CMsgStoreAddress::NewL( addrBuf, nameBuf );
        inputArray.Insert(address, 5);
        expectedArray.Insert(address, 5);
        }
    TRAP( err, mailBox1->AddMruAddressesL( inputArray ) );
    CheckCondition( _L("AddMruAddressesL() ok"), err == KErrNone );
    
    CompareMruArraysL( mailBox1, expectedArray );
    
    TInt newMaxCount=30;
    iLog->Log( _L("Case %d: Change the MaxMruAddressCount"), iCaseId++ );
    iMsgStore->SetMaxMruAddressCountL( newMaxCount );
    while( expectedArray.Count() > newMaxCount )
        {
        CMsgStoreAddress* address = expectedArray[newMaxCount];
        expectedArray.Remove(newMaxCount);
        delete address;
        }
    CompareMruArraysL( mailBox1, expectedArray );

    inputArray.Reset();
    
    iLog->Log( _L("Case %d: Add some more to make sure only %d are returned"), iCaseId++ );
    for ( int i = 0 ; i < 10 ; i++ )
        {
        addrBuf.Format(_L("newer name%d@nok%d.com"), i, i );
        nameBuf.Format(_L("newer Fname%d LName%d"), i, i );
        CMsgStoreAddress* address = CMsgStoreAddress::NewL( addrBuf, nameBuf );
        inputArray.Insert(address, 0);
        expectedArray.Insert(address, 0);
        }
    while( expectedArray.Count() > newMaxCount )
        {
        CMsgStoreAddress* address = expectedArray[newMaxCount];
        expectedArray.Remove(newMaxCount);
        delete address;
        }
    
    TRAP( err, mailBox1->AddMruAddressesL( inputArray ) );
    CheckCondition( _L("AddMruAddressesL() ok"), err == KErrNone );
    
    CompareMruArraysL( mailBox1, expectedArray );
    
    //__LOG_WRITE_INFO("Test Dynamic Encryption")
    //TRAP( err, iMsgStore->EnableEncryptionL() );
    //CheckCondition( _L("EnableEncryption() should return KErrNone or KErrAlreadyExists() ok"), err == KErrNone || err == KErrAlreadyExists);
    //if ( err == KErrAlreadyExists )
    //    {
        //encryption may be still going on, make sure it's complete before calling DisableEncryption()
    //    WaitUntilEncryptDecryptCompleteL( EMsgStoreESEncrypting, mailBox1 );
        
    //    __LOG_WRITE_INFO("Store is encrypted, decrypt it")
    //    iMsgStore->DisableEncryptionL();
    //    __LOG_WRITE_INFO("Wait for decryption to complete")
    //    WaitUntilEncryptDecryptCompleteL( EMsgStoreESDecrypting, mailBox1 );
    //    }
    //else
    //    {
    //    __LOG_WRITE_INFO("Wait for encryption to complete")
    //    WaitUntilEncryptDecryptCompleteL( EMsgStoreESEncrypting, mailBox1 );
    //    }
    
    //__LOG_WRITE_INFO("Comparing results")
    //CompareMruArraysL( mailBox1, expectedArray );
    
    //add a few more address to see if it works when encryption is turned on

    inputArray.Reset();
    
    CleanupStack::PopAndDestroy( 6 );
    
    //we are done 
    return ETrue;
    }

void CMruTests::CompareMruArraysL( CMsgStoreMailBox* aMailBox, RPointerArray<CMsgStoreAddress>& aExpected )
    {
    iLog->Log(_L("  >> CompareMruArraysL") );
    
    RPointerArray<CMsgStoreAddress> results;
    TRAPD(err, aMailBox->MruAddressesL( results ) );
    CheckCondition( _L("MruAddressesL() ok"), err == KErrNone );

    CheckCondition( _L("same counts"), aExpected.Count() == results.Count() );
    
    TInt count = aExpected.Count();
    
    iLog->Log( _L("  comparing %d addresses"), count);
    
    for ( TInt i = 0 ; i < count  ; i++ )
        {
        CMsgStoreAddress* addr1 = aExpected[i];
        CMsgStoreAddress* addr2 = results[i];
        iLog->Log( _L("  i=%d, addr=%S, dispName=%S"), i, &addr2->EmailAddress(), &addr2->DisplayName() );
        if ( addr1->EmailAddress() != addr2->EmailAddress() )
            {
            iLog->Log( _L("  Different email address found!! index=%d addr1=%S, addr2=%S"), i, &addr1->EmailAddress(), &addr2->EmailAddress() );
            User::Leave( KErrNotFound );
            }
        if ( addr1->DisplayName() != addr2->DisplayName() )
            {
            iLog->Log( _L("  Different display name found!! index=%d name1=%S, name2=%S"), i, &addr1->DisplayName(), &addr2->DisplayName() );
            User::Leave( KErrNotFound );
            }
        }
    
    results.ResetAndDestroy();
    
    iLog->Log(_L("  addresses all match!") );
    
    iLog->Log(_L("  << CompareMruArraysL") );
    
    }


//  End of File
