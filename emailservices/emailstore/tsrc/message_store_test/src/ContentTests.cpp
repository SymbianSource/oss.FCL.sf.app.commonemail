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


#include <bautils.h>
#include "ContentTests.h"
#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CContentTests* CContentTests::NewL( CStifLogger* aLog )
    {
    CContentTests* self = new(ELeave) CContentTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CContentTests::CContentTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CContentTests::~CContentTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CContentTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CContentTests::ExecuteL()
    {
    LogHeader( _L("Content") );
    
    // Create messages for later use.
    CMsgStoreAccount* account = CMsgStoreAccount::NewLC(1234, _L("new account") );     //+1
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account );
    CleanupStack::PushL( mailBox1 );                                                 //+2
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties );                                              //+3
    
    TMsgStoreId inbox1Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    CMsgStoreFolder* inbox1 = mailBox1->FolderL( inbox1Id );
    CleanupStack::PushL( inbox1 );                                                    //+4
    
    CMsgStoreMessage* message1 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( message1 );                                                  //+5
    message1->CommitL();
    
    CMsgStoreMessage* message2 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( message2 );                                                  //+6
    message2->CommitL();
    
    CMsgStoreMessage* message3 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( message3 );                                                  //+7
    message3->CommitL();
    
    // BUFFER-ORIENTED TESTS

    RBuf8 expectedContent;
    RBuf8 contentBuf;
    expectedContent.CreateL( 1000 );
    contentBuf.CreateL( 1000 );
    
    iLog->Log( _L("Case %d: Fetch content to buffer before content added"), iCaseId++ );
    
    message1->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("no content"), (contentBuf.Length() == 0) && (message1->ContentLengthL() == 0) );
    
    iLog->Log( _L("Case %d: ReplaceContentL"), iCaseId++ );
    
    _LIT8( KContent1, "THIS IS SOME CONTENT" );
    message1->ReplaceContentL( KContent1 );
    CheckCondition( _L("ContentLength matches"), message1->ContentLengthL() == KContent1().Length() );
    
    iLog->Log( _L("Case %d: FetchContentToBufferL"), iCaseId++ );
    
    message1->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf.Compare( KContent1 ) == 0 );

    iLog->Log( _L("Case %d: Fetch content with start offset"), iCaseId++ );
    
    message1->FetchContentToBufferL( contentBuf, 4 );
    CheckCondition( _L("content matches"), contentBuf.Compare( KContent1().Mid( 4 ) ) == 0 );

    iLog->Log( _L("Case %d: Append to content"), iCaseId++ );

    _LIT8( KContent2, "MORE CONTENT" );
    message1->AppendToContentL( KContent2 );
    TInt contentLength = message1->ContentLengthL();
    TInt expectedLength = KContent1().Length() + KContent2().Length();
    CheckCondition( _L("ContentLength matches"), contentLength == expectedLength );

    iLog->Log( _L("Case %d: Append null buffer to content"), iCaseId++ );

    _LIT8( KNullBuffer, "" );
    message1->AppendToContentL( KNullBuffer );
    contentLength = message1->ContentLengthL();
    CheckCondition( _L("ContentLength matches"), contentLength == expectedLength );

    iLog->Log( _L("Case %d: Append null buffer to content"), iCaseId++ );
    
    expectedContent.Copy( KContent1 );
    expectedContent.Append( KContent2 );
    message1->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf.Compare( expectedContent ) == 0 );

    iLog->Log( _L("Case %d: Long content"), iCaseId++ );
    
    expectedContent.Fill( 'C', 998 );
    expectedContent.Insert( 0, _L8("A") );
    expectedContent.Append( _L8("B") );
    message1->ReplaceContentL( expectedContent );
    message1->FetchContentToBufferL( contentBuf );
    contentLength = message1->ContentLengthL();
    expectedLength = expectedContent.Length();
    TBool passed = (contentLength == expectedLength && contentBuf.Compare( expectedContent ) == 0);
    //if( !passed )
    //    {
    //    __LOG_HEX_DUMP_ERROR( "expected", expectedContent )
    //    __LOG_HEX_DUMP_ERROR( "actual", contentBuf )
    //    }   
    CheckCondition( _L("content matches"), passed );
    
    iLog->Log( _L("Case %d: Open the original content file"), iCaseId++ );
    RFile testFile;
    TRAPD(err, message1->OpenOriginalContentFileL( testFile ) );
        
        //try to read from the file
        CheckCondition( _L("OpenOriginalContentFileL succeeded"), err == KErrNone );
        
        testFile.Read( contentBuf );
        CheckCondition( _L("Content in original file"), contentBuf.Compare( expectedContent ) == 0 );
        
        iLog->Log( _L("Case %d: Fetch while client has original file open"), iCaseId++ );
        
        message1->FetchContentToBufferL( contentBuf );
        CheckCondition( _L("Fetch while client has original file open"), contentBuf.Compare( expectedContent ) == 0 );
        
        iLog->Log( _L("Case %d: ContentLength while client has original file open"), iCaseId++ );
        
        contentLength = message1->ContentLengthL();
        CheckCondition( _L("ContentLength while client has original file open"), contentLength == expectedLength );
        
        iLog->Log( _L("Case %d: ReplaceContent while client has original file open"), iCaseId++ );
        
        message1->ReplaceContentL( KContent1 );
        message1->FetchContentToBufferL( contentBuf );
        expectedContent.Copy( KContent1 );
        CheckCondition( _L("ReplaceContent while client has original file open"), contentLength == expectedLength );
        
        iLog->Log( _L("Case %d: Can't Remove Content while client has original file open"), iCaseId++ );
        
        TRAP( err, message1->RemoveContentL() );
        CheckCondition( _L("can't RemoveContentL"), err == KErrInUse );       
    
        iLog->Log( _L("Case %d: Delete message while client has original file open"), iCaseId++ );
        //delete message while content file is locked
        mailBox1->DeleteMessageL( message1->Id(), inbox1Id );
        
        iLog->Log( _L("  Wait 3 seconds for server delete handler to complete") );
        Yield(3000000);
        
        iLog->Log( _L("  Create a dummy message") );
        CMsgStoreMessage* message4 = mailBox1->CreateMessageL( inbox1Id, *properties );
        CleanupStack::PushL( message4 );                                                         //+8
        message4->CommitL();
        
        testFile.Close();
        iLog->Log( _L("  Client closed content file") );
        iLog->Log( _L("  delete dummy message after the file is closed, to trigger the delete handle") );
        
        mailBox1->DeleteMessageL( message4->Id(), inbox1Id );
        
        iLog->Log( _L("  Wait 3 secs for server delete handler to complete") );
        Yield(3000000);
    
    CMsgStoreMessage* message5 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( message5 );                                                         //+9
    message5->CommitL();

    iLog->Log( _L("  Make sure other messages are not affected") );
    
    message2->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("no content"), (contentBuf.Length() == 0) && (message2->ContentLengthL() == 0) );

    message3->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("no content"), (contentBuf.Length() == 0) && (message3->ContentLengthL() == 0) );

    // FILE-ORIENTED TESTS

    iLog->Log( _L("Case %d: FetchContentToFileL"), iCaseId++ );
    _LIT( KFilename, "c:\\content_test.txt" );

    expectedContent.Fill( 'C', 998 );
    expectedContent.Insert( 0, _L8("A") );
    expectedContent.Append( _L8("B") );
    message5->ReplaceContentL( expectedContent );
    
    message5->FetchContentToFileL( KFilename );
    
    iLog->Log( _L("Case %d: ReplaceContentWithFileL"), iCaseId++ );

    message2->ReplaceContentWithFileL( KFilename );

    contentBuf.SetLength(0);            
    message2->FetchContentToBufferL( contentBuf );

    CheckCondition( _L("content matches"), contentBuf.Compare( expectedContent ) == 0 );

    // REMOVE CONTENT
    
    iLog->Log( _L("Case %d: Remove content"), iCaseId++ );
    
    message5->RemoveContentL();
    message5->FetchContentToBufferL( contentBuf );
    
    TInt length1 = contentBuf.Length();
    TInt length2 = message5->ContentLengthL();
    CheckCondition( _L("no content"), length1 == 0 && length2 == 0 );       

    iLog->Log( _L("case %d: Fetch content to file with no content"), iCaseId++ );

    RFs fs;
    User::LeaveIfError( fs.Connect() );
    
    message5->FetchContentToFileL( KFilename );
    RFile file;
    User::LeaveIfError( file.Open( fs, KFilename, EFileRead ) );
    TInt size;
    User::LeaveIfError( file.Size( size ) );
    file.Close();
    CheckCondition( _L("file size = 0"), size == 0 );

    // TESTS WITH VARIOUS CONTENT SIZES, TO DETECT FENCEPOST ERRORS   

    iLog->Log( _L("Case %d: FENCEPOST ERRORS Detection"), iCaseId++ );
    
    // THESE SHOULD EXACTLY MATCH THE VALUES INSIDE ContainerStoreContentManager.cpp
    const TUint KBlockSize = 64;
    const TUint KBufferSize = 4096;            

    // THESE MUST BE IN INCREASING ORDER.
    const TInt KSizesToTest[] = 
        {
        0, 
        1, 
        KBlockSize-1, 
        KBlockSize, 
        KBlockSize+1, 
        KBufferSize-1, 
        KBufferSize,
        KBufferSize+1, 
        2*KBufferSize-1, 
        2*KBufferSize, 
        2*KBufferSize+1, 
        5*KBufferSize-1, 
        5*KBufferSize, 
        5*KBufferSize+1,
        -1
        };

    expectedContent.ReAllocL( 7*KBufferSize );
    expectedContent.SetLength( 0 );        

    contentBuf.ReAllocL( 7*KBufferSize );
    contentBuf.SetLength( 0 );        

    TBuf8<KBufferSize+1> appendBuffer;
                        
    TInt index = 0;
    while( KSizesToTest[index] != -1 )
        {
        TInt growSizeBy = KSizesToTest[index] - expectedContent.Length();
        expectedContent.SetLength( KSizesToTest[index] );
        expectedContent.RightTPtr( growSizeBy ).Fill( 'A' + index );
        
        contentBuf.SetLength( 0 );
        message5->ReplaceContentL( expectedContent );
        message5->FetchContentToBufferL( contentBuf );    
        CheckCondition( _L("content matches"), message5->ContentLengthL() == expectedContent.Length() && contentBuf.Compare( expectedContent ) == 0 );

        contentBuf.SetLength( 0 );
        BaflUtils::DeleteFile( fs, KFilename );
        message5->FetchContentToFileL( KFilename );        
        message2->ReplaceContentWithFileL( KFilename );
        message2->FetchContentToBufferL( contentBuf );    
        contentLength = message2->ContentLengthL();
        expectedLength = expectedContent.Length();
        CheckCondition( _L("content matches"), contentLength == expectedLength && contentBuf.Compare( expectedContent ) == 0 );

        message3->ReplaceContentWithFileL( KFilename );

        appendBuffer.Fill( 'w', 1 );
        expectedContent.Append( appendBuffer );
        message5->AppendToContentL( appendBuffer );
        message5->FetchContentToBufferL( contentBuf );    
        contentLength = message5->ContentLengthL();
        expectedLength = expectedContent.Length();
        CheckCondition( _L("content matches"), contentLength == expectedLength && contentBuf.Compare( expectedContent ) == 0 );
            
        appendBuffer.Fill( 'x', KBlockSize-1 );
        expectedContent.SetLength( KSizesToTest[index] );
        expectedContent.Append( appendBuffer );
        message2->AppendToContentL( appendBuffer );
        message2->FetchContentToBufferL( contentBuf );    
        contentLength = message2->ContentLengthL();
        expectedLength = expectedContent.Length();
        CheckCondition( _L("content matches"), contentLength == expectedLength && contentBuf.Compare( expectedContent ) == 0 );
            
        appendBuffer.Fill( 'y', KBlockSize );
        expectedContent.SetLength( KSizesToTest[index] );
        expectedContent.Append( appendBuffer );
        message3->AppendToContentL( appendBuffer );
        message3->FetchContentToBufferL( contentBuf );    
        contentLength = message3->ContentLengthL();
        expectedLength = expectedContent.Length();
        CheckCondition( _L("content matches"), contentLength == expectedLength && contentBuf.Compare( expectedContent ) == 0 );
        
        appendBuffer.Fill( 'z', KBufferSize+1 );
        expectedContent.Append( appendBuffer );
        message3->AppendToContentL( appendBuffer );
        message3->FetchContentToBufferL( contentBuf );
        contentLength  = message3->ContentLengthL();
        expectedLength = expectedContent.Length();   
        CheckCondition( _L("content matches"), contentLength == expectedLength && contentBuf.Compare( expectedContent ) == 0 );
        
        expectedContent.SetLength( KSizesToTest[index] );
        
        index++;
        } // end for
        
    expectedContent.Close();
    contentBuf.Close();
        
    fs.Close();

    CleanupStack::PopAndDestroy( 9 );
    
    //we are done 
    return ETrue;
    }


//  End of File
