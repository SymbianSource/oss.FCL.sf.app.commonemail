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
#include "EncryptionTests.h"
//#include <ismsgstorepropertykeys.h>
//#include "messagestoreclientserver.h"

// This must match that's defined in ContainerStoreDefs.h
enum TEncryptionState
{
    EMsgStoreESIdle,
    EMsgStoreESEncrypting,
    EMsgStoreESDecrypting
}; // end TEncryptionState

// ============================ MEMBER FUNCTIONS ===============================

CEncryptionTests* CEncryptionTests::NewL( CStifLogger* aLog )
    {
    CEncryptionTests* self = new(ELeave) CEncryptionTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CEncryptionTests::CEncryptionTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CEncryptionTests::~CEncryptionTests()
    {
    iTestContentArray.Reset();
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CEncryptionTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    iTestContentArray.AppendL( _L8("This is the first test content!") );
    iTestContentArray.AppendL( _L8("This is the second test content!") );
    iTestContentArray.AppendL( _L8("This is the first test content!") );
    iTestContentArray.AppendL( _L8("What-ever!!!!") );
    iTestContentArray.AppendL( _L8("ok fine!!!") );
    iTestContentArray.AppendL( _L8("Here is a long content (>300 bytes).   Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.") );
    iTestContentArray.AppendL( _L8("Is that long enough") );
    iTestContentArray.AppendL( _L8("aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff") );
    iTestContentArray.AppendL( _L8("Here is a >300 unicode characters (> 600 bytes).   Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.  Here is a long content.") );
    iTestContentArray.AppendL( _L8("Hi") );
    iTestContentArray.AppendL( _L8("") );
    iTestContentArray.AppendL( _L8("This is another test content") );
    iTestContentArray.AppendL( _L8("How many more?") );
    iTestContentArray.AppendL( _L8("Six more contents to go") );
    iTestContentArray.AppendL( _L8("Five more contents to go") );
    iTestContentArray.AppendL( _L8("     ") );
    iTestContentArray.AppendL( _L8("Three more...") );
    iTestContentArray.AppendL( _L8("Two more...") );
    iTestContentArray.AppendL( _L8("One more...") );
    iTestContentArray.AppendL( _L8("Last one! We are done") );
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CEncryptionTests::ExecuteL()
    {
    LogHeader( _L("Encryption") );
    
    iLog->Log( _L("Case %d: Testing SetPasswordL()"), iCaseId++ );
    iMsgStore->SetPasswordL( _L("mypassword") );
    
    iLog->Log( _L("Case %d: Testing AuthenticatedL()"), iCaseId++ );
    CheckCondition( _L("AuthenticatedL() is TRUE"), iMsgStore->AuthenticatedL() );
    
    iLog->Log( _L("Case %d: Testing HasPasswordL()"), iCaseId++ );
    CheckCondition( _L("HasPasswordL() is TRUE"), iMsgStore->HasPasswordL() );
    
    iLog->Log( _L("Case %d: Create Mailboxes"), iCaseId++ );
    RPointerArray<CMsgStoreAccount> accountArray;
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewL(1234, _L("Account 1") );
    accountArray.Append( account1 );                                            
    
    CMsgStoreAccount* account2 = CMsgStoreAccount::NewL(1234, _L("") );
    accountArray.Append( account2 );                                            
    
    CMsgStoreAccount* account3 = CMsgStoreAccount::NewL(5678, _L("Account 3") );
    accountArray.Append( account3 );                                            
    
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                            //+1
    
    CMsgStoreMailBox* mailBox2 = iMsgStore->CreateAccountL( *account2 );
    CleanupStack::PushL( mailBox2 );                                            //+2
    
    CMsgStoreMailBox* mailBox3 = iMsgStore->CreateAccountL( *account3 );
    CleanupStack::PushL( mailBox3 );                                            //+3
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties );                                          //+4
    
    properties->AddPropertyL( _L8("1"), _L("PROPERTY 1") );
    properties->AddPropertyL( _L8("2"), 12345 );
    properties->AddPropertyL( _L8("3"), _L8("ANOTHER PROPERTY") );
    
    iLog->Log( _L("Case %d: Create Folders"), iCaseId++ );
    
    TMsgStoreId inbox1Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    TMsgStoreId sentFolderId = mailBox2->CreateFolderL( mailBox2->Id(), *properties );
    TMsgStoreId deletedFolderId = mailBox3->CreateFolderL( mailBox3->Id(), *properties );
    
    iLog->Log( _L("  inbox=%x, sent=%x, deleted=%x"), inbox1Id, sentFolderId, deletedFolderId  );
    
    TInt msgCountPerFolder = 20;
    
    iLog->Log( _L("Case %d: Create messages in Inbox"), iCaseId++ );
    //keep ids, message ids will be stored in iMatches
    CreateRandomMessagesL( mailBox1, inbox1Id, msgCountPerFolder, ETrue ); 
    
    iLog->Log( _L("Case %d: Create messages in Sent folder"), iCaseId++ );
    CreateRandomMessagesL( mailBox2, sentFolderId, msgCountPerFolder );
    
    iLog->Log( _L("Case %d: Create messages in Deleted folder"), iCaseId++ );
    CreateRandomMessagesL( mailBox3, deletedFolderId, msgCountPerFolder );
    
    iLog->Log( _L("Case %d: Set msg contents and store them for comparison"), iCaseId++ );
    for ( TInt i = 0 ; i < iMatches.Count() ; i++ )
        {
        CMsgStoreMessage* msg = mailBox1->FetchMessageL( iMatches[i], inbox1Id );
        SetMessageContentL( msg, iTestContentArray[i] );
        iMessages.Append( msg );
        }

    iLog->Log( _L("Case %d: Store Msg Ids Sorted by Subject and Sender for comparison"), iCaseId++ );
    RArray<TMsgStoreId> idsSortdBySubject;
    GetSortedIdsL( mailBox2, sentFolderId, EMsgStoreSortBySubject, msgCountPerFolder, idsSortdBySubject );
    
    RArray<TMsgStoreId> idsSortdByFrom;
    GetSortedIdsL( mailBox3, deletedFolderId, EMsgStoreSortBySender, msgCountPerFolder, idsSortdByFrom );
    
    iLog->Log( _L("Case %d: Start Encryption"), iCaseId++ );
    iMsgStore->EnableEncryptionL();
    
    iLog->Log( _L("  Wait until Encryption completes") );
    WaitUntilEncryptDecryptCompleteL( EMsgStoreESEncrypting, mailBox1 );
    
    iLog->Log( _L("  Encryption completed") );
    
    iLog->Log( _L("Case %d: Comparing messages"), iCaseId++ );
    CompareMessagesL( mailBox1 );
    
    iLog->Log( _L("Case %d: Comparing Accounts"), iCaseId++ );
    CompareAccountsL( iMsgStore, accountArray );

    iLog->Log( _L("Case %d: Comparing msg IDs sorted by Subject"), iCaseId++ );
    RArray<TMsgStoreId> aSortedIds;
    GetSortedIdsL( mailBox2, sentFolderId, EMsgStoreSortBySubject, msgCountPerFolder, aSortedIds );
    CompareSortedIdArraysL( aSortedIds, idsSortdBySubject );
    aSortedIds.Reset();
    
    iLog->Log( _L("Case %d: Comparing msg IDs sorted by Sender"), iCaseId++ );
    GetSortedIdsL( mailBox3, deletedFolderId, EMsgStoreSortBySender, msgCountPerFolder, aSortedIds );
    CompareSortedIdArraysL( aSortedIds, idsSortdByFrom );
    aSortedIds.Reset();
    
    iLog->Log( _L("Case %d: Start Decryption"), iCaseId++ );
    
    iMsgStore->DisableEncryptionL();
    
    iLog->Log( _L("  Wait until Decryption completes") );
    
    WaitUntilEncryptDecryptCompleteL( EMsgStoreESDecrypting, mailBox1 );
    
    iLog->Log( _L("  Decryption completed") );
    
    iLog->Log( _L("Case %d: Comparing messages"), iCaseId++ );
    CompareMessagesL( mailBox1 );
    
    iLog->Log( _L("Case %d: Comparing Accounts"), iCaseId++ );
    CompareAccountsL( iMsgStore, accountArray );
    
    iLog->Log( _L("Case %d: Comparing msg IDs sorted by Subject"), iCaseId++ );
    GetSortedIdsL( mailBox2, sentFolderId, EMsgStoreSortBySubject, msgCountPerFolder, aSortedIds );
    CompareSortedIdArraysL( aSortedIds, idsSortdBySubject );
    aSortedIds.Reset();
    
    iLog->Log( _L("Case %d: Comparing msg IDs sorted by Sender"), iCaseId++ );
    GetSortedIdsL( mailBox3, deletedFolderId, EMsgStoreSortBySender, msgCountPerFolder, aSortedIds );
    CompareSortedIdArraysL( aSortedIds, idsSortdByFrom );
    aSortedIds.Reset();
    
    idsSortdBySubject.Close();
    idsSortdByFrom.Close();
    accountArray.ResetAndDestroy();
    iMatches.Reset();
    iMessages.ResetAndDestroy();
    
    CleanupStack::PopAndDestroy( 4 );
    
    //test authentication
    iLog->Log( _L("Case %d: Test AuthenticateL"), iCaseId++ );
    TRAPD(err, iMsgStore->AuthenticateL( _L("mypassword")) );
    CheckCondition( _L("AuthenticateL successful"), err == KErrNone );
    
    iLog->Log( _L("Case %d: Test ChangePasswordL"), iCaseId++ );
    TRAP(err, iMsgStore->ChangePasswordL( _L("mypassword"), _L("newpassword") ) );
    CheckCondition( _L("ChangePasswordL successful"), err == KErrNone );
        
    iLog->Log( _L("Case %d: Test ClearAuthenticationL"), iCaseId++ );
    TRAP(err, iMsgStore->ClearAuthenticationL() );
    CheckCondition( _L("ClearAuthenticationL successful"), err == KErrNone );
    
    //we are done 
    return ETrue;
    }


void CEncryptionTests::WaitUntilEncryptDecryptCompleteL( TInt aCurrentState, CMsgStoreMailBox* aMailBox )
    {
    RDebugSession debugSession;
    User::LeaveIfError( debugSession.Connect() );
    
    TInt encryptionState = aCurrentState; 
    TInt loopCount = 0;
    TMsgStoreId folderId;
    while( encryptionState !=  EMsgStoreESIdle && loopCount < 4000 )
        {
        Yield( 20000 );
        loopCount++;
        if ( loopCount % 20 == 0 )
            {
            encryptionState = debugSession.GetEncryptionStateL();
            }
        
        if ( loopCount == 2 && encryptionState == aCurrentState )
            {
            iLog->Log( _L("Case %d: Testing fetch msgs while encryption/decription is in progress"), iCaseId++ );
            CompareMessagesL( aMailBox );
            }
        
        if ( loopCount == 5 && encryptionState == aCurrentState )
            {
            iLog->Log( _L("Case %d: Testing create msgs while encryption/decription is in progress"), iCaseId++ );
            CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
            folderId = aMailBox->CreateFolderL( aMailBox->Id(), *properties);
            delete properties;
            CreateRandomMessagesL(aMailBox, folderId, 5);
            }
        
        if ( loopCount == 8 && encryptionState == aCurrentState )
            {
            iLog->Log( _L("Case %d: Testing delete folder while encryption/decription is in progress"), iCaseId++ );
            aMailBox->DeleteFolderL(folderId);
            }
        
        if ( encryptionState != aCurrentState && encryptionState != EMsgStoreESIdle )
            {
            break;
            }
        }
    
    debugSession.Close();
    if ( encryptionState != EMsgStoreESIdle )
        {
        iLog->Log( _L("  Encryption did NOT complete") );
        User::Leave( KErrCancel );
        }
    }

void CEncryptionTests::CompareMessagesL( CMsgStoreMailBox* aMailBox )
    {
    iLog->Log( _L("  >> CompareMessagesL") );
    
    RBuf8 contentBuf;
    contentBuf.Create( 1024 );
    contentBuf.CleanupClosePushL();
    
    for( TInt i = 0 ; i < iMessages.Count() ; i++ )
        {
        CMsgStoreMessage* msg = aMailBox->FetchMessageL( iMessages[i]->Id(), KMsgStoreInvalidId );
        CleanupStack::PushL( msg );                               //+1
        
        ComparePropertiesL( *msg, *(iMessages[i]) );
        
        iLog->Log( _L("   Comparing contents...") );
        
        RPointerArray<CMsgStoreMessagePart> parts;
        msg->ChildPartsL( parts );
        CMsgStoreMessagePart* part =parts[0];
        
        RPointerArray<CMsgStoreMessagePart> childParts;
        part->ChildPartsL( childParts );
        CMsgStoreMessagePart* plain_text_part = childParts[0];
        
        plain_text_part->FetchContentToBufferL( contentBuf );
        if ( contentBuf == iTestContentArray[i] )
            {
            iLog->Log( _L("   Contents match!") );
            }
        else
            {
            iLog->Log( _L("   Error: Contents don't match! index=%d"), i );
            User::Leave( KErrNotFound );
            }

        childParts.ResetAndDestroy();
        parts.ResetAndDestroy();
        CleanupStack::PopAndDestroy( msg );
        }
    CleanupStack::PopAndDestroy( &contentBuf );
    
    iLog->Log( _L("  << CompareMessagesL - PASSED") );
    }

void CEncryptionTests::CompareAccountsL( CMsgStore* aSession, RPointerArray<CMsgStoreAccount>& aAccounts )
    {
    iLog->Log( _L("  >> CompareAccountsL") );
    
    RPointerArray<CMsgStoreAccount> accs;
    aSession->AccountsL( accs );

    CheckCondition( _L("Counts match"), accs.Count() == aAccounts.Count() );
    
    for ( TInt i = 0 ; i < accs.Count() ; i++ )
        {
        CMsgStoreAccount* acc = accs[i];
        TBool found = EFalse;
        for (TInt j = 0 ; !found && j < aAccounts.Count() ; j++ )
            {
            CMsgStoreAccount* acc2 = aAccounts[j];
            if ( acc->Owner() == acc2->Owner() && acc->Name() == acc2->Name() )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            iLog->Log( _L("  Account:%X %S not found!"), acc->Owner(), &( acc->Name() ) );
            User::Leave(KErrNotFound);
            }
        }
    
    accs.ResetAndDestroy();
    
    iLog->Log( _L("  << CompareAccountsL - PASSED") );
    }

void CEncryptionTests::GetSortedIdsL( CMsgStoreMailBox* aMailBox, TMsgStoreId aFolderId, TMsgStoreSortByField aSortedBy, TInt aCount, RArray<TMsgStoreId>& aSortedIds )
    {
    iLog->Log( _L("  >> GetSortedIdsL") );
    
    RMsgStoreSortCriteria criteria;
    CleanupClosePushL( criteria );
    
    criteria.iFolderId = aFolderId;
    criteria.AddResultPropertyL( KMsgStorePropertyFrom );
    criteria.AddResultPropertyL( KMsgStorePropertySubject );
    criteria.iSortBy   = aSortedBy;
    criteria.iSortOrder = EMsgStoreSortDescending;
    
    CMsgStoreSortResultIterator* iterator = aMailBox->SortL( criteria ); 
    CleanupStack::PushL( iterator );
    
    RPointerArray<CMsgStorePropertyContainer> results;
    
    iterator->NextL( KMsgStoreSortResultTop, aCount, results );
    for ( TInt i = 0 ; i < results.Count() ; i++ )
        {
        TMsgStoreId msgId = results[i]->Id();
        aSortedIds.Append( msgId );
        }

    results.ResetAndDestroy();
    CleanupStack::PopAndDestroy( iterator );
    CleanupStack::PopAndDestroy( &criteria );
    
    iLog->Log( _L("  << GetSortedIdsL") );
    }

void CEncryptionTests::CompareSortedIdArraysL( RArray<TMsgStoreId>& aIDs1, RArray<TMsgStoreId>& aIDs2 )
    {
    iLog->Log( _L("  >> CompareSortedIdArraysL") );
    
    CheckCondition(_L("Counts match"), aIDs1.Count() == aIDs2.Count() );
    
    for ( TInt i = 0 ; i < aIDs1.Count() ; i++ )
        {
        if ( aIDs1[i] != aIDs2[i] )
            {
            iLog->Log( _L("  Arrays don't match. Index=%d, val1=%u, val2=%u") , i, aIDs1[i], aIDs2[i] );
            User::Leave( KErrNotFound );
            }
        }
    
    iLog->Log( _L("  << CompareSortedIdArraysL - PASSED") );
    }

//  End of File
