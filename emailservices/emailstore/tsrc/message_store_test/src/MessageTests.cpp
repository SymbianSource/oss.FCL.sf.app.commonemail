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
#include "MessageTests.h"
//#include <ismsgstorepropertykeys.h>
#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CMessageTests* CMessageTests::NewL( CStifLogger* aLog )
    {
    CMessageTests* self = new(ELeave) CMessageTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CMessageTests::CMessageTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMessageTests::~CMessageTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CMessageTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CMessageTests::ExecuteL()
    {
    LogHeader( _L("Message") );
    
    //account 1
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );   //+1
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                           //+2

    //accoun 2
    CMsgStoreAccount* account2 = CMsgStoreAccount::NewLC( 4567, _L("Account 2") );  //+3
    CMsgStoreMailBox* mailBox2 = iMsgStore->CreateAccountL( *account2 );
    CleanupStack::PushL( mailBox2 );                                           //+4
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    
    CleanupStack::PushL( properties );                                          //+5
    TestAddIntegerPropertyL( *properties, _L8("INTEGER PROPERTY"), 99999 );
    TestAddDes8PropertyL( *properties, _L8("STRING PROPERTY"), _L8("STRING VALUE") );
    TestAddDesPropertyL( *properties, _L8("name"), _L("Inbox") );
    
    TMsgStoreId inbox1Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    CMsgStoreFolder* inbox1 = mailBox1->FolderL( inbox1Id );
    CleanupStack::PushL( inbox1 );                                              //+6
    
    TMsgStoreId inbox2Id = mailBox2->CreateFolderL( mailBox2->Id(), *properties );
    CMsgStoreFolder* inbox2 = mailBox2->FolderL( inbox2Id );
    CleanupStack::PushL( inbox2 );                                              //+7
    
    //create outbox folders for mailbox1 and mailbox2
    CMsgStorePropertyContainer* outboxProp = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( outboxProp );                                          //+8
    
    TestAddIntegerPropertyL( *outboxProp, _L8("INTEGER PROPERTY"), 99999 );
    TestAddDes8PropertyL( *outboxProp, _L8("STRING PROPERTY"), _L8("STRING VALUE") );
    TestAddDesPropertyL( *outboxProp, _L8("name"), _L("Outbox") );
    
    TMsgStoreId outbox1Id = mailBox1->CreateFolderL( mailBox1->Id(), *outboxProp );
    CMsgStoreFolder* outbox1 = mailBox1->FolderL( outbox1Id );
    CleanupStack::PushL( outbox1 );                                              //+9
    
    TMsgStoreId outbox2Id = mailBox1->CreateFolderL( mailBox2->Id(), *outboxProp );
    CMsgStoreFolder* outbox2 = mailBox1->FolderL( outbox2Id );
    CleanupStack::PushL( outbox2 );                                              //+10
    
    
    RArray<TMsgStoreId> expectedIds;
    
    CMsgStorePropertyContainer* properties2 = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties2 );                                           //+11
    TestAddIntegerPropertyL( *properties2, KMsgStorePropertyFlags, 0 );
    TestAddIntegerPropertyL( *properties2, KMsgStorePropertyReplyBy, 1 );
    TestAddIntegerPropertyL( *properties2, _L8("SOME INTEGER PROPERTY"), KMaxTUint32 );
    TestAddDes8PropertyL( *properties2, _L8("SOME OTHER PROPERTY"), _L8("SOME VALUE") );
    
    iLog->Log( _L("Case %d: Create messages"), iCaseId++ );
    
    _LIT8( KTestContent, "TEST CONTENT" );
    
    CMsgStoreMessage* message1 = mailBox1->CreateMessageL( inbox1Id, *properties2 );
    CleanupStack::PushL( message1 );                                             //+12
    
    iLog->Log( _L("Case %d: Total Message Count"), iCaseId++ );
    
    //get message counts
    TUint totalCount, unreadCount;
    mailBox1->TotalMessageCountsL( totalCount, unreadCount );
    
    CheckCondition( _L("Msg Not Committed. Count=0"), totalCount==0 && unreadCount==0 );
    
    message1->CommitL();
    
    mailBox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("Msg Commited. Count=1"), totalCount==1 && unreadCount==1 );
    
    iLog->Log( _L("Case %d: Create message in a different way"), iCaseId++ );
    CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
    props->AddOrUpdatePropertyL( KMsgStorePropertyFlags, (TUint32) 0 );  //set the unread flag
        
    CMsgStoreMessage* tmpMsg = mailBox1->CreateMessageL( inbox1Id, *props );
    
    mailBox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("Msg Not Committed. Count=1"), totalCount==1 && unreadCount==1 );
    
    mailBox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("Msg Not Committed. Count=1"), totalCount==1 && unreadCount==1 );
    
    tmpMsg->CommitL();
    
    mailBox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("Msg Commited. Count=2"), totalCount==2 && unreadCount==2 );
    
    iLog->Log( _L("Case %d: Delete Message"), iCaseId++ );
    mailBox1->DeleteMessageL( tmpMsg->Id(), inbox1Id );
    
    delete tmpMsg;
    
    iLog->Log( _L("Case %d: Abandon Uncommitted Message"), iCaseId++ );
    tmpMsg = mailBox1->CreateMessageL( inbox1Id, *props );
    //Instead commit, abandon it
    tmpMsg->AbandonL();
    
    delete tmpMsg;
    
    delete props;
    
    iLog->Log( _L("Case %d: StorePropertiesL in between create and commit"), iCaseId++ );
    CMsgStoreMessage* message2 = mailBox1->CreateMessageL( inbox1Id, *properties2 );
    CleanupStack::PushL( message2 );                                             //+13
    
    // Test StorePropertiesL in between create and commit. This is no supported
    // for uncommitted messages.
    TRAPD( err, message2->StorePropertiesL() );
    // Test StorePropertyL in between create and commit.
    TRAPD( err1, message2->StorePropertyL( 0 ) );
    
    CheckCondition( _L("StoreProperties failed as expected. Count=2"),
        err==KErrNotFound && err1==KErrNotFound );    
    message2->CommitL();
    
    iLog->Log( _L("Case %d: Create message in different folders/mailboxes"), iCaseId++ );
    CMsgStoreMessage* message3 = mailBox1->CreateMessageL( outbox1Id, *properties2 );
    CleanupStack::PushL( message3 );                                             //+14
    message3->CommitL();
    
    CMsgStoreMessage* message4 = mailBox1->CreateMessageL( outbox1Id, *properties2 );
    CleanupStack::PushL( message4 );                                             //+15
    message4->CommitL();
    
    CMsgStoreMessage* message5 = mailBox2->CreateMessageL( inbox2Id, *properties2 );
    CleanupStack::PushL( message5 );                                             //+16
    message5->CommitL();
    
    CMsgStoreMessage* message6 = mailBox2->CreateMessageL( inbox2Id, *properties2 );
    CleanupStack::PushL( message6 );                                             //+17
    message6->CommitL();
    
    CMsgStoreMessage* message7 = mailBox2->CreateMessageL( outbox2Id, *properties2 );
    CleanupStack::PushL( message7 );                                             //+18
    message7->CommitL();
    
    CMsgStoreMessage* message8 = mailBox2->CreateMessageL( outbox2Id, *properties2 );
    CleanupStack::PushL( message8 );                                             //+19
    message8->CommitL();

    message1->ReplaceContentL( KTestContent );
    message2->ReplaceContentL( KTestContent );
    message3->ReplaceContentL( KTestContent );
    message4->ReplaceContentL( KTestContent );
    message5->ReplaceContentL( KTestContent );
    
    iLog->Log( _L("  Verify contents of each folder") );
    
    expectedIds.AppendL( message1->Id() );
    expectedIds.AppendL( message2->Id() );
    VerifyMessageIdsL( *mailBox1, inbox1Id, expectedIds );
    expectedIds.Reset();

    expectedIds.AppendL( message3->Id() );
    expectedIds.AppendL( message4->Id() );
    VerifyMessageIdsL( *mailBox1, outbox1Id, expectedIds );
    expectedIds.Reset();

    expectedIds.AppendL( message5->Id() );
    expectedIds.AppendL( message6->Id() );
    VerifyMessageIdsL( *mailBox2, inbox2Id, expectedIds );
    expectedIds.Reset();
    
    expectedIds.AppendL( message7->Id() );
    expectedIds.AppendL( message8->Id() );
    VerifyMessageIdsL( *mailBox2, outbox2Id, expectedIds );
    expectedIds.Reset();

    //Message counts
    DoMessageCountsL( *mailBox1 );
    DoMessageCountsL( *mailBox2 );
    
    iLog->Log( _L("Case %d: Move messages"), iCaseId++ );
    
    mailBox1->MoveMessageL( message1->Id(), inbox1Id, outbox1Id );
    mailBox1->MoveMessageL( message2->Id(), inbox1Id, outbox1Id );
    mailBox2->MoveMessageL( message7->Id(), outbox2Id, inbox2Id );
    mailBox2->MoveMessageL( message8->Id(), outbox2Id, inbox2Id );

    iLog->Log( _L("  Verify contents of each folder") );
    
    VerifyMessageIdsL( *mailBox1, inbox1Id, expectedIds );
    expectedIds.Reset();

    expectedIds.AppendL( message1->Id() );
    expectedIds.AppendL( message2->Id() );
    expectedIds.AppendL( message3->Id() );
    expectedIds.AppendL( message4->Id() );
    VerifyMessageIdsL( *mailBox1, outbox1->Id(), expectedIds );
    expectedIds.Reset();

    VerifyMessageIdsL( *mailBox2, outbox2->Id(), expectedIds );
    expectedIds.Reset();

    expectedIds.AppendL( message5->Id() );
    expectedIds.AppendL( message6->Id() );
    expectedIds.AppendL( message7->Id() );
    expectedIds.AppendL( message8->Id() );
    VerifyMessageIdsL( *mailBox2, inbox2->Id(), expectedIds );
    expectedIds.Reset();

    DoMessageCountsL( *mailBox1 );
    DoMessageCountsL( *mailBox2 );
    
    iLog->Log( _L("Case %d: Delete messages"), iCaseId++ );
    
    mailBox1->DeleteMessageL( message1->Id(), outbox1Id );
    mailBox2->DeleteMessageL( message8->Id(), inbox2Id );
    
    iLog->Log( _L("  Verify contents of each folder") );
    
    VerifyMessageIdsL( *mailBox1, inbox1Id, expectedIds );
    expectedIds.Reset();
    
    expectedIds.AppendL( message2->Id() );
    expectedIds.AppendL( message3->Id() );
    expectedIds.AppendL( message4->Id() );
    VerifyMessageIdsL( *mailBox1, outbox1->Id(), expectedIds );
    expectedIds.Reset();
    
    VerifyMessageIdsL( *mailBox2, outbox2->Id(), expectedIds );
    expectedIds.Reset();
    
    expectedIds.AppendL( message5->Id() );
    expectedIds.AppendL( message6->Id() );
    expectedIds.AppendL( message7->Id() );
    VerifyMessageIdsL( *mailBox2, inbox2->Id(), expectedIds );
    expectedIds.Reset();
    
    DoMessageCountsL( *mailBox1 );
    DoMessageCountsL( *mailBox2 );

    iLog->Log( _L("Case %d: Copy message"), iCaseId++ );
    
    TMsgStoreId newId = mailBox1->CopyMessageL( message3->Id(), outbox1->Id(), inbox1->Id() );
    
    iLog->Log( _L("  Delete all other messages") );
    
    mailBox1->DeleteMessageL( message2->Id(), outbox1->Id() );
    mailBox1->DeleteMessageL( message3->Id(), outbox1->Id() );
    mailBox1->DeleteMessageL( message4->Id(), outbox1->Id() );
    mailBox2->DeleteMessageL( message5->Id(), inbox2->Id() );
    mailBox2->DeleteMessageL( message6->Id(), inbox2->Id() );
    mailBox2->DeleteMessageL( message7->Id(), inbox2->Id() );

    iLog->Log( _L("  Verify contents of each folder") );
    
    expectedIds.AppendL( newId );
    VerifyMessageIdsL( *mailBox1, inbox1->Id(), expectedIds );
    expectedIds.Reset();

    VerifyMessageIdsL( *mailBox1, outbox1->Id(), expectedIds );
    VerifyMessageIdsL( *mailBox2, inbox2->Id(), expectedIds );
    VerifyMessageIdsL( *mailBox2, outbox2->Id(), expectedIds );

    iLog->Log( _L("  Fetch copied message") );
    
    CMsgStoreMessage* newMessage = mailBox1->FetchMessageL( newId, inbox1->Id() );
    
    iLog->Log( _L("  Verify properties and content") );
    
    ComparePropertiesL( *newMessage, *properties2 );

    TBuf8<1000> contentBuf;
    newMessage->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf.Compare( KTestContent ) == 0  );
    
    delete newMessage;
    
    iLog->Log( _L("Case %d: Message Commit"), iCaseId++ );
    
    CMsgStoreMessage* msg1 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( msg1 );                                                    //+20
    msg1->ReplaceContentL( _L8("CONTENT1") );           
    TInt result = KErrNone;
    TRAP( result, mailBox1->FetchMessageL( msg1->Id(), inbox1Id ) );            
    CheckCondition( _L("uncommitted message not found"), result == KErrNotFound );          
    
    CMsgStoreMessagePart* part1 = msg1->AddChildPartL( *properties );
    part1->AppendToContentL( _L8("CONTENT2") );         
//  TRAP( result, msg1->ChildPartL( part1->Id() ) );
//  CheckCondition( _L("uncommitted message not found"), result == KErrNotFound );          
    
    _LIT( KFilename, "C:\\test_content.txt" );
    _LIT8( KContent, "THIS IS THE CONTENT FOR THIS TEST" );
    RFs   fs;
    User::LeaveIfError( fs.Connect() );
    RFile file;
    User::LeaveIfError( file.Replace( fs, KFilename, EFileWrite ) );
    file.Write( KContent );
    file.Close();
    fs.Close();

//  CMsgStoreMessagePart* part2 = msg1->AddChildPartL( *properties );
//  part2->ReplaceContentWithFileL( KFilename );
//  TRAP( result, tmpPart = msg1->ChildPartL( part2->Id() ) );
//  CheckCondition( _L("uncommitted message not found"), result == KErrNotFound );                              
    
    msg1->CommitL();
    iLog->Log( _L("  Message committed") );
    
    TMsgStoreId message1Id = msg1->Id();
    
    iLog->Log( _L("Case %d: Fetch message in a specific folder"), iCaseId++ );
    message2 = mailBox1->FetchMessageL( message1Id, inbox1->Id() );
    ComparePropertiesL( *message2, *properties );
    delete message2;
    
    iLog->Log( _L("Case %d: Fetch message in an unknown folder"), iCaseId++ );
    message2 = mailBox1->FetchMessageL( message1Id, KMsgStoreInvalidId );
    ComparePropertiesL( *message2, *properties );
    
    iLog->Log( _L("  Verify contents") );
    
    TBuf8<100> tmpBuf;
    message2->FetchContentToBufferL( tmpBuf );
    CheckCondition( _L("content fetched"), tmpBuf.Compare(_L8("CONTENT1")) == 0 );                              
    
    CMsgStoreMessagePart* part3 = msg1->ChildPartL( part1->Id() );
    ComparePropertiesL( *part3, *properties );

    part3->FetchContentToBufferL( tmpBuf );
    CheckCondition( _L("content fetched"), tmpBuf.Compare(_L8("CONTENT2")) == 0 );                              
    
    delete part1;
//    delete part2;
    delete part3;
    delete message2;
    
    // OUT OF MEMORY ERROR PATH TESTS

    iLog->Log( _L("Case %d: Error - Out of memory"), iCaseId++ );
    for( TInt i = 0; i < 4; i++ )
        {        
        SimulateLowDiskSpaceL( i );
        TRAP( result, mailBox1->CopyMessageL( message1Id, inbox1Id, outbox1Id ) );
        CheckCondition( _L("out of memory"), result == KErrNoMemory );
        }
        
    // ERROR PATH TESTS
    
    // Nonexistent message/folder or wrong folder

    iLog->Log( _L("Case %d: Error - Delete nonexistent message"), iCaseId++ );

    TRAP( result, mailBox1->DeleteMessageL( message1Id, outbox1Id ) );
    CheckCondition( _L("not found"), result == KErrNotFound );                              

    iLog->Log( _L("Case %d: Error - Move nonexistent message"), iCaseId++ );
    
    TRAP( result, mailBox1->MoveMessageL( message1Id, outbox1Id, inbox1Id ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
//    __LOG_WRITE_INFO( "Replace content for nonexistent message." )

//  TRAP( result, message1->ReplaceContentL( _L8("CONTENT") ) );
//  CheckCondition( _L("not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - ReplaceContentWithFileL for nonexistent message"), iCaseId++ );

    TRAP( result, message1->ReplaceContentWithFileL( KFilename ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - Append to content for nonexistent message"), iCaseId++ );

    TRAP( result, message1->AppendToContentL( _L8("CONTENT") ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - ContentLengthL for nonexistent message"), iCaseId++ );

    TRAP( result, message1->ContentLengthL() );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - FetchContentToBufferL for nonexistent message"), iCaseId++ );

    TRAP( result, message1->FetchContentToBufferL( contentBuf ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - FetchContentToFileL for nonexistent message"), iCaseId++ );

    TRAP( result, message1->FetchContentToFileL( KFilename ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    CMsgStoreMessage* msg3 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( msg3 );                                                 //+21
    msg3->CommitL();

    iLog->Log( _L("Case %d: Error - Delete message from wrong folder"), iCaseId++ );

    TRAP( result, mailBox1->DeleteMessageL( msg3->Id(), outbox1Id ) );
    CheckCondition( _L("not found"), result == KErrNotFound );                              

    iLog->Log( _L("Case %d: Error - Move message from wrong folder"), iCaseId++ );

    TRAP( result, mailBox1->MoveMessageL( msg3->Id(), outbox1Id, inbox1Id ) );
    CheckCondition( _L("not found"), result == KErrNotFound );                              

    iLog->Log( _L("Case %d: Error - Move message from nonexistent folder"), iCaseId++ );

    TRAP( result, mailBox1->MoveMessageL( msg3->Id(), inbox1Id, inbox1Id + 50 ) ); 
    CheckCondition( _L("not found"), result == KErrNotFound );                              

    iLog->Log( _L("Case %d: Error - Create message in nonexistent folder"), iCaseId++ );

    TRAP( result,
        CMsgStoreMessage* msg = mailBox1->CreateMessageL( inbox1Id + 50, *properties );
        CleanupStack::PushL( msg );
        msg->CommitL();
        CleanupStack::PopAndDestroy( msg );
    );
    CheckCondition( _L("not found"), result == KErrNotFound );                              

    // Content file not found
    iLog->Log( _L("Case %d: Error - ReplaceContentWithFileL with nonexistent file"), iCaseId++ );
    
    TRAP( result, msg3->ReplaceContentWithFileL( _L("c:\\some_nonexistent_file.txt") ) );
    CheckCondition( _L("not found"), result == KErrNotFound );

    TMsgStoreId messageId = msg3->Id(); 

    iLog->Log( _L("Case %d: Error - Commit message into deleted folder"), iCaseId++ );

    TMsgStoreId folderId = mailBox1->CreateFolderL( inbox1Id, *properties );
    CMsgStoreMessage* msg4 = mailBox1->CreateMessageL( folderId, *properties ); 
    CleanupStack::PushL( msg4 );                                                 //+22
    mailBox1->DeleteFolderL( folderId );    
    TRAP( result, msg4->CommitL() );
    CheckCondition( _L("not found"), result == KErrNotFound );                              

    // Wrong types

    iLog->Log( _L("Case %d: Error - Create message with invalid type for folder"), iCaseId++ );

    TRAP( result, mailBox1->CreateMessageL( messageId, *properties ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    iLog->Log( _L("Case %d: Error - Delete message with invalid type for message"), iCaseId++ );

    TRAP( result, mailBox1->DeleteMessageL( folderId, folderId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    iLog->Log( _L("Case %d: Error - Delete message with invalid type for folder"), iCaseId++ );

    TRAP( result, mailBox1->DeleteMessageL( messageId, messageId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    iLog->Log( _L("Case %d: Error - Move message with invalid type for message"), iCaseId++ );

    TRAP( result, mailBox1->MoveMessageL( folderId, folderId, folderId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    iLog->Log( _L("Case %d: Error - Move message with invalid type for source folder"), iCaseId++ );

    TRAP( result, mailBox1->MoveMessageL( messageId, messageId, folderId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    iLog->Log( _L("Case %d: Error - Move message with invalid type for destination folder"), iCaseId++ );

    TRAP( result, mailBox1->MoveMessageL( messageId, folderId, messageId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    iLog->Log( _L("Case %d: Error - Fetch message with invalid type for message"), iCaseId++ );

    TRAP( result, mailBox1->FetchMessageL( folderId, folderId ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    CleanupStack::PopAndDestroy( 22 );
    
    //we are done 
    return ETrue;
    }

void CMessageTests::VerifyMessageIdsL( CMsgStoreMailBox& aMailBox, TMsgStoreId aFolderId, RArray<TMsgStoreId>& expectedIds )
    {   
    iLog->Log( _L(" >> VerifyMessageIdsL") );
            
    RArray<TMsgStoreId> ids;
    
    aMailBox.MessageIdsL( aFolderId, ids );

    TBool passed = (ids.Count() == expectedIds.Count() );
    
    for( TInt i = 0; i < ids.Count(); i++ )
        {
        TBool found = EFalse;
        for ( TInt j = 0; j < expectedIds.Count() && !found; j++ )
            {
            found = (ids[i] == expectedIds[j]);
            }
        passed = passed && found;               
        }
    
    if( !passed )
        {
        for( TInt i = 0; i < expectedIds.Count(); i++ )
            {                   
            iLog->Log( _L("  expected=%i"), expectedIds[i] );
            }
        for( TInt i = 0; i < ids.Count(); i++ )
            {                   
            iLog->Log( _L("  found=%i"), ids[i] );
            }               
        }
    
    TBuf<30> text;
    text.Format( _L("%i ids matched"), ids.Count() );
    CheckCondition( text, passed );         
    
    ids.Close();
    
    iLog->Log( _L(" << VerifyMessageIdsL") );
    }
                

void CMessageTests::DoMessageCountsL( CMsgStoreMailBox& aMailBox )
    {
    iLog->Log( _L("  Message counts") );
    
    TUint totalCount, unreadCount;
    aMailBox.TotalMessageCountsL( totalCount, unreadCount );
    iLog->Log( _L("  mb=%u total=%u, unread=%u"), aMailBox.Id(), totalCount, unreadCount );

    RPointerArray<CMsgStoreFolder> folders; 
    aMailBox.FoldersL( folders );
    
    for ( TInt i = 0 ; i < folders.Count() ; i++ )
        {
        aMailBox.MessageCountsL( folders[i]->Id(), totalCount, unreadCount );
        iLog->Log( _L("  folder=%u total=%u, unread=%u"), folders[i]->Id(), totalCount, unreadCount );
        }

    folders.ResetAndDestroy();
    }



//  End of File
