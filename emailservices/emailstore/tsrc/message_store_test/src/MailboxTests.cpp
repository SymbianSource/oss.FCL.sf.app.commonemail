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
#include "MailboxTests.h"
#include <msgstoreapi.h>
//#include <ismsgstorepropertykeys.h>        
        
// ============================ MEMBER FUNCTIONS ===============================

CMailboxTests* CMailboxTests::NewL( CStifLogger* aLog )
    {
    CMailboxTests* self = new(ELeave) CMailboxTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CMailboxTests::CMailboxTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMailboxTests::~CMailboxTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CMailboxTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CMailboxTests::ExecuteL()
    {
    LogHeader( _L("Mailbox") );
    
    //StartRecordingObserverEvents();
    TInt32 owner1 = 1234;
    TInt32 owner2 = 5678;
    _LIT(KName1, "My Account");
    _LIT(KName2, "Your Account");
    
    iLog->Log( _L("Case %d: Account Object"), iCaseId++ );
    
    iLog->Log( _L("  Test Accounts in Memory") );
    
    CMsgStoreAccount* testAcc = CMsgStoreAccount::NewLC(9999, _L("test accoun"));  //+testAcc
    CheckCondition( _L("owner id"), testAcc->Owner() == 9999 );
    CheckCondition( _L("account name"), testAcc->Name() == _L("test accoun") );
    
    _LIT( KChangeAccountOwner, "  Change Account Owner" );
    iLog->Log( KChangeAccountOwner );
    testAcc->SetOwner( 8888 );
    CheckCondition( _L("new owner id"), testAcc->Owner() == 8888 );
    
    _LIT( KChangeAccountNameLong, "  Change Account Name (long)" );
    iLog->Log( KChangeAccountNameLong );
    testAcc->SetNameL( _L("this is a very very long account name") );
    CheckCondition( _L("new account name"), testAcc->Name() == _L("this is a very very long account name") );
    
    _LIT( KChangeAccountNameShort, "  Change Account Name (short)" );
    iLog->Log( KChangeAccountNameShort );
    testAcc->SetNameL( _L("sl name") );
    CheckCondition( _L("new account name"), testAcc->Name() == _L("sl name") ) ;
    
    CleanupStack::PopAndDestroy( testAcc );
    
    iLog->Log( _L("Case %d: Create Mailboxes"), iCaseId++ );
    
    //account 1
    CMsgStoreMailBox* mailBox1 = NULL;
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewLC( owner1, KNullDesC );   //+1
    
    TRAPD( err, mailBox1 = iMsgStore->CreateAccountL( *account1 ) );
    
    CleanupStack::PushL( mailBox1 );                                            //+2
    CheckCondition( _L("Id1, empty name"), err == KErrNone );
    
    //mailbox 1 should be set as default mailbox now
    iLog->Log( _L("  mailbox1Id=%d, isCorporate=%d"), mailBox1->Id(), mailBox1->IsCorporateMailbox() );
    
    //account 2, with propertyes
    CMsgStoreMailBox* mailBox2 = NULL;
    CMsgStoreAccount* account2 = CMsgStoreAccount::NewLC( owner1, KName1 );      //+3
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    properties->AddPropertyL(_L8("mailbox property"), _L("maibox property value"));
    
    TUint32 aSourceId = 0x80001;
    TUint32 feature = 0x1f;
    properties->AddOrUpdatePropertyL( KIsPropertyEmailSourceId, aSourceId );
    properties->AddOrUpdatePropertyL( KIsPropertyCapabilities, feature );
    
    mailBox2 = iMsgStore->CreateAccountL( *account2, properties );    
   
    CleanupStack::PushL( mailBox2 );                                            //+4
    
    //mailbox 1 should be set as default mailbox now
    iLog->Log( _L("  mailbox2Id=%d, isCorporate=%d"), mailBox2->Id(), mailBox2->IsCorporateMailbox() );
    
    ComparePropertiesL( *mailBox2, *properties );
    delete properties;
    
    //account 3
    CMsgStoreMailBox* mailBox3 = NULL;
    CMsgStoreAccount* account3 = CMsgStoreAccount::NewLC( owner2, KNullDesC );    //+5
    
    TRAP( err, mailBox3 = iMsgStore->CreateAccountL( *account3 ) );    
    
    CleanupStack::PushL( mailBox3 );                                            //+6
    CheckCondition( _L("Id2, empty name"), err == KErrNone );
    //mailbox 1 should be set as default mailbox now
    iLog->Log( _L("  mailbox3Id=%d, isCorporate=%d"), mailBox3->Id(), mailBox3->IsCorporateMailbox() );
    
    //account 4
    CMsgStoreMailBox* mailBox4 = NULL;
    CMsgStoreAccount* account4 = CMsgStoreAccount::NewLC( owner2, KName2 );       //+7
    
    properties = CMsgStorePropertyContainer::NewL();
    properties->AddOrUpdatePropertyL( KIsPropertyServerType, (TUint32)ESrvExchange );
    
    TRAP( err, mailBox4 = iMsgStore->CreateAccountL( *account4, properties ) );  
    delete properties;
    
    CleanupStack::PushL( mailBox4 );                                              //+8
    CheckCondition( _L("Id2, name2"), err == KErrNone );
    iLog->Log( _L("  mailbox4Id=%d, isCorporate=%d"), mailBox4->Id(), mailBox4->IsCorporateMailbox() );
    
    //test account aready exists error
    iLog->Log( _L("Case %d: Error - Test Account Exists"), iCaseId++ );
    
    CMsgStoreMailBox* mailBox = NULL;
    TRAP( err, mailBox = iMsgStore->CreateAccountL( *account1 ) );    
    delete mailBox;
    mailBox = NULL;
    CheckCondition( _L("account1 exists - empty name"), err == KErrAlreadyExists );
    
    TRAP( err, mailBox = iMsgStore->CreateAccountL( *account4 ) );    
    delete mailBox;
    mailBox = NULL;
    CheckCondition( _L("account2 exists - name2"), err == KErrAlreadyExists );

    DoListAccountsL( *iMsgStore );

    iLog->Log( _L("Case %d: Set/Get Default Mailbox"), iCaseId++ );
    iMsgStore->SetDefaultMailboxL( mailBox3->Id() );
    
    TMsgStoreId id = iMsgStore->DefaultMailboxL();
    CheckCondition( _L("DefaultMailboxL() is now account3"), mailBox3->Id() );
    
    iLog->Log( _L("Case %d: Open Account by Account Object"), iCaseId++ );
    mailBox = iMsgStore->OpenAccountL( *account1 );
    CheckCondition( _L("open account 1"), mailBox->Id() == mailBox1->Id() );
    iLog->Log( _L("  mailbox1Id=%d, isCorporate=%d"), mailBox->Id(), mailBox->IsCorporateMailbox() );
    delete mailBox;
    
    mailBox = iMsgStore->OpenAccountL( *account2 );
    CheckCondition( _L("open account 2"), mailBox->Id() == mailBox2->Id() );
    iLog->Log( _L("  mailbox2Id=%d, isCorporate=%d"), mailBox->Id(), mailBox->IsCorporateMailbox() );
    delete mailBox;
    
    mailBox = iMsgStore->OpenAccountL( *account3 );
    CheckCondition( _L("open account 3"), mailBox->Id() == mailBox3->Id() );
    iLog->Log( _L("  mailbox3Id=%d, isCorporate=%d"), mailBox->Id(), mailBox->IsCorporateMailbox() );
    delete mailBox;
    
    mailBox = iMsgStore->OpenAccountL( *account4 );
    CheckCondition( _L("open account 4"), mailBox->Id() == mailBox4->Id() );
    iLog->Log( _L("  mailbox4Id=%d, isCorporate=%d"), mailBox->Id(), mailBox->IsCorporateMailbox() );
    delete mailBox;
    
    iLog->Log( _L("Case %d: Open Account by MailBoxId"), iCaseId++ );
    mailBox = iMsgStore->OpenAccountL( mailBox1->Id() );
    CheckCondition( _L("open account 1 by MailBoxId"), mailBox->Id() == mailBox1->Id() );
    delete mailBox;
    
    mailBox = iMsgStore->OpenAccountL( mailBox2->Id() );
    CheckCondition( _L("open account 2 by MailBoxId"), mailBox->Id() == mailBox2->Id() );
    //ComparePropertiesL( *mailBox, *mailBox2 );
    delete mailBox;
    
    mailBox = iMsgStore->OpenAccountL( mailBox3->Id() );
    CheckCondition( _L("open account 3 by MailBoxId"), mailBox->Id() == mailBox3->Id() );
    //ComparePropertiesL( *mailBox, *mailBox3 );
    delete mailBox;
    
    mailBox = iMsgStore->OpenAccountL( mailBox4->Id() );
    CheckCondition( _L("open account 4 by MailBoxId"), mailBox->Id() == mailBox4->Id() );
    //ComparePropertiesL( *mailBox, *mailBox4 );
    delete mailBox;
    mailBox = NULL;
    
    iLog->Log( _L("Case %d: Error - Open Account"), iCaseId++ );
    iLog->Log( _L("  Open an nonexisting account - bad owner id") );
    //Open an nonexisting account - bad owner id
    CMsgStoreAccount* badAccount1 = CMsgStoreAccount::NewLC( 1, KName2 );             //+9
    TRAP(err, mailBox = iMsgStore->OpenAccountL(*badAccount1) );
    CheckCondition( _L("bad ownerId"), err==KErrNotFound );
    
    iLog->Log( _L("  Open an nonexisting account - bad name") );
    //Open an nonexisting account - bad name
    CMsgStoreAccount* badAccount2 = CMsgStoreAccount::NewLC( owner1, _L("bad name") ); //+10
    TRAP(err, mailBox = iMsgStore->OpenAccountL(*badAccount2) );
    CheckCondition( _L("bad name"), err==KErrNotFound );
    
    iLog->Log( _L("Case %d: Rename Account"), iCaseId++ );
    
    TRAP(err, iMsgStore->RenameAccountL( account1->Owner(), account1->Name(), _L("New acc 1")) );
    CheckCondition( _L("rename account1"), err==KErrNone );
    
    TRAP(err, iMsgStore->RenameAccountL( account1->Owner(), account1->Name(), _L("New acc 1")) );
    CheckCondition( _L("rename account1 again - should return -1"), err==KErrNotFound );
    
    TRAP(err, iMsgStore->RenameAccountL( account4->Owner(), account4->Name(), _L("New acc 4")) );
    CheckCondition( _L("rename account4"), err==KErrNone );
    
    iLog->Log( _L("Case %d: Error - Rename Account"), iCaseId++ );
    TRAP(err, iMsgStore->RenameAccountL( account1->Owner(), _L("New acc 1"), KName1) );
    CheckCondition( _L("new name already exists"), err==KErrAlreadyExists );
    
    DoListAccountsL( *iMsgStore );
    
    iLog->Log( _L("Case %d: Delete Account"), iCaseId++ );
    
    TRAP(err, iMsgStore->DeleteAccountL(*account2));
    CheckCondition( _L("delete account2"), err==KErrNone );
    
    TRAP(err, iMsgStore->DeleteAccountL(*account3));
    CheckCondition( _L("delete account3"), err==KErrNone );
    
    iLog->Log( _L("Case %d: Error - Delete Account"), iCaseId++ );
    TRAP(err, iMsgStore->DeleteAccountL(*account2));
    CheckCondition( _L("delete account2 again - should return -1"), err==KErrNotFound );
    
    TRAP(err, iMsgStore->DeleteAccountL(*account4));
    CheckCondition( _L("delete account4 using old name, should return -1"), err==KErrNotFound );
    
    DoListAccountsL( *iMsgStore );
    
    CleanupStack::PopAndDestroy( 10 );
    
    //we are done 
    return ETrue;
    }

void CMailboxTests::DoListAccountsL( CMsgStore& aMsgStore )
{
    iLog->Log( _L("  --List Accounts--") );
    RPointerArray<CMsgStoreAccount> accounts;
    TRAPD( err, aMsgStore.AccountsL( accounts ) );
    CheckCondition( _L("Accounts"), err == KErrNone );
    
    for ( int i = 0 ; i < accounts.Count() ; i++ )
        {
        CMsgStoreAccount* account = accounts[i];
        iLog->Log( _L("  id=%i, name=%S"), account->Owner(), &account->Name() );
        }
    
    accounts.ResetAndDestroy();
}


//  End of File
