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
#include "SizeTests.h"
//#include <ismsgstorepropertykeys.h>
//#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CSizeTests* CSizeTests::NewL( CStifLogger* aLog )
    {
    CSizeTests* self = new(ELeave) CSizeTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CSizeTests::CSizeTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CSizeTests::~CSizeTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CSizeTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CSizeTests::ExecuteL()
    {
    LogHeader( _L("Size") );
    
    CMsgStoreAccount* account = CMsgStoreAccount::NewLC( 1234, KNullDesC );         //+1
    CMsgStoreMailBox* mailBox = iMsgStore->CreateAccountL( *account );
    CleanupStack::PushL( mailBox );                                                 //+2
    
    CMsgStorePropertyContainer* bigProperties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( bigProperties );                                           //+3
    
    TBuf8<1000> buf;
    buf.Fill( 'X', 1000 );
    
    // 100K properties
    for( TInt i = 0; i < 100; i++ )
        {
        TBuf8<20> propertyName;
        propertyName.Format( _L8("property%i"), &propertyName );
        
        bigProperties->AddPropertyL( propertyName, buf );       
        }

    iLog->Log( _L("Case %d: Create a folder with big properties"), iCaseId++ );
    
    
    
    TMsgStoreId folderId = mailBox->CreateFolderL( mailBox->Id(), *bigProperties );

    CMsgStoreFolder* folder = mailBox->FolderL( folderId );
    ComparePropertiesL( *folder, *bigProperties );
    delete folder;
    
    RPointerArray<CMsgStoreFolder> folders;
    mailBox->FoldersL( folders );
    ComparePropertiesL( *folders[folders.Count()-1], *bigProperties );
    folders.ResetAndDestroy();    

    iLog->Log( _L("Case %d: Create a message with big properties"), iCaseId++ );
    
    CMsgStoreMessage* message = mailBox->CreateMessageL( folderId, *bigProperties );
    CleanupStack::PushL( message );                                                      //+4
    message->CommitL();
    
    CMsgStoreMessage* message2 = mailBox->FetchMessageL( message->Id(), folderId );
    ComparePropertiesL( *message2, *bigProperties );
    delete message2;
    
    iLog->Log( _L("Case %d: Create an attachment with big properties"), iCaseId++ );
    
    CMsgStoreMessagePart* attachment = message->AddChildPartL( *bigProperties );
    CleanupStack::PushL( attachment );                                                   //+5

    CMsgStoreMessagePart* attachment2 = message->AddChildPartL( *bigProperties );
    ComparePropertiesL( *attachment2, *bigProperties );
    delete attachment2;
    
    RPointerArray<CMsgStoreMessagePart> attachments;
    message->ChildPartsL( attachments );
    ComparePropertiesL( *attachments[0], *bigProperties );
    attachments.ResetAndDestroy();    
    
    iLog->Log( _L("Case %d: Add large content to message with Appends"), iCaseId++ );
    
    buf.Fill( 'C', 1000 );
    for( TInt i = 0; i < 100; i++ )
        {       
        message->AppendToContentL( buf );   
        } // end for
        
    buf.Copy( _L8("D") );
    message->AppendToContentL( buf );   
    
    iLog->Log( _L("Case %d: Fetch large content to buffer"), iCaseId++ );
    
    RBuf8 heapBuf;
    heapBuf.CleanupClosePushL();                                                         //+6
    heapBuf.CreateL( 100002 ); 
    
    message->FetchContentToBufferL( heapBuf );
    
    CheckCondition( _L( "content OK"), heapBuf.Length() == 100001 && heapBuf[0] == 'C' && 
                                       heapBuf[99999] == 'C' && heapBuf[100000] == 'D' );
    
    iLog->Log( _L("Case %d: Fetch large content to file"), iCaseId++ );
    
    _LIT( KFilename, "c:\\content_test.txt" );
    message->FetchContentToFileL( KFilename );
    
    iLog->Log( _L("Case %d: Add large content to attachment with file"), iCaseId++ );
    
    attachment->ReplaceContentWithFileL( KFilename );
    
    iLog->Log( _L("Case %d: Fetch large content to buffer"), iCaseId++ );
    
    heapBuf.FillZ();    
    
    message->FetchContentToBufferL( heapBuf );
    
    CheckCondition( _L( "content OK"), heapBuf.Length() == 100001 && heapBuf[0] == 'C' && 
                                       heapBuf[99999] == 'C' && heapBuf[100000] == 'D' );
    

    CleanupStack::PopAndDestroy(6);
    
    //we are done 
    return ETrue;
    }


//  End of File
