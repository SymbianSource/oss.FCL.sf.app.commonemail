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
#include "FolderTests.h"
//#include <ismsgstorepropertykeys.h>
#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CFolderTests* CFolderTests::NewL( CStifLogger* aLog )
    {
    CFolderTests* self = new(ELeave) CFolderTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CFolderTests::CFolderTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CFolderTests::~CFolderTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CFolderTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CFolderTests::ExecuteL()
    {
    LogHeader( _L("Folder") );
    
    //account 1
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );   //+1
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                           //+2
    
    iLog->Log( _L("Case %d: Create Top-Level folders"), iCaseId++ );
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties );                                         //+3
    
    TestAddIntegerPropertyL( *properties, _L8("INTEGER PROPERTY"), 99999 );
    TestAddDes8PropertyL( *properties, _L8("STRING PROPERTY"), _L8("STRING VALUE") );
    
    TMsgStoreId folder1Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    
    CMsgStoreFolder* folder = mailBox1->FolderL( folder1Id );
    
    ComparePropertiesL( *properties, *folder );
    CheckCondition( _L("parent folder ID"), folder->ParentId() ==  mailBox1->Id() );
    delete folder;
    
    iLog->Log( _L("Case %d: Create local sub-folder"), iCaseId++ );
    TMsgStoreId folder2Id = mailBox1->CreateFolderL( folder1Id, *properties, ETrue );
    
    folder = mailBox1->FolderL( folder2Id );
    CheckCondition( _L("is local flag"), folder->IsLocalL() );
    
    ComparePropertiesL( *properties, *folder );
    CheckCondition( _L("parent folder ID"), folder->ParentId() == folder1Id );
    delete folder;
    
    iLog->Log( _L("Case %d: Move folder"), iCaseId++ );
    TMsgStoreId folder3Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    mailBox1->MoveFolderL( folder3Id, mailBox1->Id(), folder2Id );
    
    iLog->Log( _L("Case %d: Delete folder"), iCaseId++ );
    mailBox1->DeleteFolderL( folder3Id );
    
    iLog->Log( _L("Case %d: Error - Open non-existing folder"), iCaseId++ );
    TRAPD( err, folder = mailBox1->FolderL( folder3Id ) );  //folder should be deleted
    CheckCondition( _L("open non-existing folder - -1"), err == KErrNotFound );
    
    iLog->Log( _L("Case %d: Get root folders"), iCaseId++ );
    //get the system folders
    TMsgStoreRootFolders rootFolders; 
    MsgStoreFolderUtils::InitializeRootFoldersL( *mailBox1, rootFolders ); 
    
    RPointerArray<CMsgStoreFolder> folders;
    mailBox1->FoldersL( folders );
    
    RArray<TMsgStoreId> expectedIds;
    expectedIds.AppendL( folder1Id );
    expectedIds.AppendL( folder2Id );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreInbox] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreOutbox] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreDrafts] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreSent] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreDeleted] );

    
    VerifyFolderIdsL( folders, expectedIds );
    
    for ( int i = 0 ; i < folders.Count() ; i++ )
        {
        iLog->Log( _L("  folderId=%d, parentId=%d"), folders[i]->Id(), folders[i]->ParentId() );
        }
    
    folders.ResetAndDestroy();
    expectedIds.Reset();
    
    
    iLog->Log( _L("Case %d: Create more user defined folders, with messages in them"), iCaseId++ );
    folder3Id = mailBox1->CreateFolderL( folder1Id, *properties );

    CMsgStoreMessage* message = mailBox1->CreateMessageL( folder2Id, *properties );
    TMsgStoreId messageId = message->Id();
    message->CommitL();
    delete message;
    message = mailBox1->CreateMessageL( folder2Id, *properties );
    message->CommitL();
    delete message;
    
    TMsgStoreId folder4Id = mailBox1->CreateFolderL( folder2Id, *properties );
    
    message = mailBox1->CreateMessageL( folder4Id, *properties );
    message->CommitL();
    delete message;

    TMsgStoreId folder5Id = mailBox1->CreateFolderL( folder2Id, *properties );

    iLog->Log( _L("Case %d: List all folders"), iCaseId++ );
    mailBox1->FoldersL( folders );
    
    expectedIds.AppendL( folder1Id );
    expectedIds.AppendL( folder2Id );
    expectedIds.AppendL( folder3Id );
    expectedIds.AppendL( folder4Id );
    expectedIds.AppendL( folder5Id );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreInbox] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreOutbox] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreDrafts] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreSent] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreDeleted] );
    VerifyFolderIdsL( folders, expectedIds );
    
    folders.ResetAndDestroy();
    
    iLog->Log( _L("Case %d: List child folders"), iCaseId++ );
    mailBox1->FoldersL( folder1Id, folders );
    
    RArray<TMsgStoreId> expectedIds2;
    expectedIds2.AppendL( folder2Id );
    expectedIds2.AppendL( folder3Id );
    VerifyFolderIdsL( folders, expectedIds2 );
    
    folders.ResetAndDestroy();
    expectedIds2.Reset();

    iLog->Log( _L("Case %d: List child folders recursively"), iCaseId++ );
    //list all subfolders of folder1Id
    mailBox1->FoldersL( folder1Id, folders, ETrue );
    
    expectedIds2.Reset();
    expectedIds2.AppendL( folder2Id );
    expectedIds2.AppendL( folder3Id );
    expectedIds2.AppendL( folder4Id );
    expectedIds2.AppendL( folder5Id );
    VerifyFolderIdsL( folders, expectedIds2 );
    
    folders.ResetAndDestroy();          
    expectedIds2.Reset();

    mailBox1->FoldersL( folder2Id, folders );
    
    expectedIds2.AppendL( folder4Id );
    expectedIds2.AppendL( folder5Id );
    VerifyFolderIdsL( folders, expectedIds2 );

    folders.ResetAndDestroy();          
    expectedIds2.Reset();
    
    iLog->Log( _L("Case %d: Delete user defined folder with no children"), iCaseId++ );
    mailBox1->DeleteFolderL( folder5Id );

    mailBox1->FoldersL( folders );
    
    expectedIds.Remove( 4 ); // folder5
    VerifyFolderIdsL( folders, expectedIds );
    folders.ResetAndDestroy();
    expectedIds.Reset();
    
    iLog->Log( _L("Case %d: Delete user defined folder with children"), iCaseId++ );
    mailBox1->DeleteFolderL( folder2Id );

    mailBox1->FoldersL( folders );
    
    expectedIds.AppendL( folder1Id ); 
    expectedIds.AppendL( folder3Id ); 
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreInbox] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreOutbox] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreDrafts] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreSent] );
    expectedIds.AppendL( rootFolders.iFolders[EMsgStoreDeleted] );
    
    VerifyFolderIdsL( folders, expectedIds );
    
    folders.ResetAndDestroy();          
    expectedIds.Reset();
    
    // ERROR PATH TESTS
    
    iLog->Log( _L("Case %d: Error - Delete nonexistent folder"), iCaseId++ );
    TRAPD( result, mailBox1->DeleteFolderL( folder5Id ) );
    CheckCondition( _L("not found error"), result == KErrNotFound );
        
    iLog->Log( _L("Case %d: Error - Create folder in nonexistent folder"), iCaseId++ );
    TRAP( result, mailBox1->CreateFolderL( folder5Id, *properties ) );
    CheckCondition( _L("not found error"), result == KErrNotFound );
        
    iLog->Log( _L("Case %d: Error - FolderL for nonexistent folder"), iCaseId++ );
    TRAP( result, mailBox1->FolderL( folder4Id ) );
    CheckCondition( _L("not found error"), result == KErrNotFound );
        
    iLog->Log( _L("Case %d: Error - FoldersL for nonexistent parent folder"), iCaseId++ );
    TRAP( result, mailBox1->FoldersL( folder2Id, folders ) );
    CheckCondition( _L("not found error"), result == KErrNotFound );
        
    iLog->Log( _L("Case %d: Error - Create folder with wrong type for folder"), iCaseId++ );
    TRAP( result, mailBox1->CreateFolderL( messageId, *properties ) );
    CheckCondition( _L("argument error"), result == KErrArgument );
        
    iLog->Log( _L("Case %d: Error - Delete folder with wrong type for folder"), iCaseId++ );
    TRAP( result, mailBox1->DeleteFolderL( messageId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );
        
    iLog->Log( _L("Case %d: Error - FolderL with wrong type for folder"), iCaseId++ );
    TRAP( result, mailBox1->FolderL( messageId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );
        
    iLog->Log( _L("Case %d: Error - FoldersL with wrong type for folder"), iCaseId++ );
    TRAP( result, mailBox1->FoldersL( messageId, folders ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    // OUT OF MEMORY ERROR PATH TESTS

    iLog->Log( _L("Case %d: Error - Out of memory"), iCaseId++ );
    for( TInt i = 0; i < 1; i++ )
        {        
        SimulateLowDiskSpaceL( i );
        TRAP( result, mailBox1->CreateFolderL( folder1Id, *properties ) );
        CheckCondition( _L("out of memory"), result == KErrNoMemory );
        }
        
    CleanupStack::PopAndDestroy( 3 );
    
    //we are done 
    return ETrue;
    }

void CFolderTests::VerifyFolderIdsL( RPointerArray<CMsgStoreFolder>& aFolders, RArray<TMsgStoreId>& aExpectedIds )
    {           
    iLog->Log( _L(" >> VerifyFolderIdsL") );
    
    TBool passed = (aFolders.Count() == aExpectedIds.Count() );
    
    for( TInt i = 0; i < aFolders.Count(); i++ )
        {
        TBool found = EFalse;
        for ( TInt j = 0; j < aExpectedIds.Count() && !found; j++ )
            {
            found = (aFolders[i]->Id() == aExpectedIds[j]);
            }
        passed = passed && found;               
        }

    if( !passed )
        {
        for( TInt i = 0; i < aExpectedIds.Count(); i++ )
            {                   
            iLog->Log( _L("  expected[%i]=%x"), i, aExpectedIds[i] );
            }
        for( TInt i = 0; i < aFolders.Count(); i++ )
            {                   
            iLog->Log( _L("  found[%i]=%x"), i, aFolders[i]->Id() );
            }               
        }
    
    TBuf<30> text;
    text.Format( _L("%i ids matched"), aFolders.Count() );
    CheckCondition( text, passed );         
    
    iLog->Log( _L(" << VerifyFolderIdsL") );
    }
                


//  End of File
