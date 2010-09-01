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
#include "PartTests.h"
//#include <ismsgstorepropertykeys.h>
#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CPartTests* CPartTests::NewL( CStifLogger* aLog )
    {
    CPartTests* self = new(ELeave) CPartTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CPartTests::CPartTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CPartTests::~CPartTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CPartTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CPartTests::ExecuteL()
    {
    LogHeader( _L("Part") );
    
    //account 1
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );   //+1
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                           //+2

    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties );                                              //+3
    properties->AddPropertyL( _L8("name"), _L("some name") );
    
    TMsgStoreId inbox1Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    CMsgStoreFolder* inbox1 = mailBox1->FolderL( inbox1Id );
    CleanupStack::PushL( inbox1 );                                              //+4
    
    CMsgStoreMessage* message1 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( message1 );                                               //+5
    message1->CommitL();
    
    RPointerArray<CMsgStoreMessagePart> parts;

    iLog->Log( _L("Case %d: List parts when no parts yet"), iCaseId++ );
    
    message1->ChildPartsL( parts );
    CheckCondition( _L("no parts"), parts.Count() == 0 );               
    parts.ResetAndDestroy();

    iLog->Log( _L("Case %d: Add a part with no content"), iCaseId++ );

    CMsgStoreMessagePart* part = message1->AddChildPartL( *properties );
    ComparePropertiesL( *part, *properties );
    CheckCondition( _L("parent ID is correct"), part->ParentId() == message1->Id() );
    
    iLog->Log( _L("Case %d: Get ChildPart"), iCaseId++ );
    
    CMsgStoreMessagePart* compareMe = message1->ChildPartL( part->Id() );
    ComparePropertiesL( *part, *compareMe );
    CheckCondition( _L("parent ID is correct"), compareMe->ParentId() == message1->Id() );
    
    iLog->Log( _L("Case %d: Add a child part to a child child part"), iCaseId++ );
    
    CMsgStoreMessagePart* grandChild = part->AddChildPartL( *properties );
    ComparePropertiesL( *grandChild, *properties );
    CheckCondition( _L("grandChild's parent ID is correct"), grandChild->ParentId() == part->Id() );
    
    iLog->Log( _L("Case %d: Testing ChildPartL(aRecursive=ETrue)"), iCaseId++ );
    delete compareMe;
    compareMe = message1->ChildPartL( grandChild->Id(), ETrue );
    ComparePropertiesL( *grandChild, *compareMe );
    CheckCondition( _L("GrandChild's parent ID is correct"), compareMe->ParentId() == part->Id() );
    delete compareMe;
    
    iLog->Log( _L("Case %d: Testing RemoveChildPartL(aRecursive=ETrue)"), iCaseId++ );
    message1->RemoveChildPartL( grandChild->Id(), ETrue );
    delete grandChild;    

    iLog->Log( _L("  List parts") );
    
    message1->ChildPartsL( parts );
    CheckCondition( _L("1 part"), parts.Count() == 1 );             
    ComparePropertiesL( *parts[0], *properties );
    CheckCondition( _L("parent ID is correct"), parts[0]->ParentId() == message1->Id() );
    parts.ResetAndDestroy();
    
    TMsgStoreId partId = part->Id();
    delete part;
    
    iLog->Log( _L("Case %d: Add an attachment with content using file path"), iCaseId++ );

    _LIT( KFilename, "C:\\test_content.txt" );
    _LIT8( KContent, "THIS IS THE CONTENT FOR THIS TEST" );
    RFs   fs;
    User::LeaveIfError( fs.Connect() );
    RFile file;
    User::LeaveIfError( file.Replace( fs, KFilename, EFileWrite ) );
    file.Write( KContent );
    file.Close();
    fs.Close();

    part = message1->AddChildPartL( *properties, KFilename );    //new part
    
    iLog->Log( _L("  List parts") );
    
    message1->ChildPartsL( parts );
    CheckCondition( _L("2 parts"), parts.Count() == 2 );                

    compareMe = NULL;
    if( parts[0]->Id() == part->Id() )
        {
        compareMe = parts[0];               
        }
    else if( parts[1]->Id() == part->Id() )
        {
        compareMe = parts[1];                               
        } // end if
        
    CheckCondition( _L("found"), compareMe != NULL );
    CheckCondition( _L("parent ID is correct"), compareMe->ParentId() == message1->Id() );

    iLog->Log( _L("  Verify content") );
    
    TBuf8<100> contentBuf;
    compareMe->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf.Compare( KContent ) == 0 );

    parts.ResetAndDestroy();
    delete part;
    
    iLog->Log( _L("Case %d: Add an attachment with content using file handle"), iCaseId++ );
    RFs   fs2;
    User::LeaveIfError( fs2.Connect() );
    RFile file2; 
    fs2.ShareProtected();
    User::LeaveIfError( file2.Open( fs2, KFilename, EFileRead ) );

    part = message1->AddChildPartL( *properties, file2 );    //new part
    
    file2.Close();
    fs2.Close();
    
    iLog->Log( _L("  List parts") );
    
    message1->ChildPartsL( parts );
    CheckCondition( _L("2 parts"), parts.Count() == 3 );                

    compareMe = NULL;
    for (TInt i=0; i < parts.Count() ; i++ )
        {
        if( parts[i]->Id() == part->Id() )
            {
            compareMe = parts[i];
            break;
            }
        }
        
    CheckCondition( _L("found"), compareMe != NULL );
    CheckCondition( _L("parent ID is correct"), compareMe->ParentId() == message1->Id() );

    iLog->Log( _L("  Verify content") );
    
    compareMe->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf.Compare( KContent ) == 0 );

    parts.ResetAndDestroy();
    
    
    iLog->Log( _L("Case %d: Copy part"), iCaseId++ );
    
    CMsgStoreMessage* message2 = mailBox1->CreateMessageL( inbox1Id, *properties );
    CleanupStack::PushL( message2 );                                                //+6
    message2->CommitL();
    
    TMsgStoreId newId = message1->CopyChildPartL( part->Id(), message2->Id() );

    CMsgStoreMessagePart* newpart = message2->ChildPartL( newId );
    ComparePropertiesL( *part, *newpart );

    contentBuf.SetLength( 0 );
    newpart->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf.Compare( KContent ) == 0 );
    
    delete newpart;
    
    iLog->Log( _L("Case %d: Remove parts"), iCaseId++ );
    message1->RemoveChildPartL( part->Id() );
    message1->RemoveChildPartL( partId );
    
    message1->ChildPartsL( parts );
        
    CheckCondition( _L("one part left"), parts.Count() == 1 );              
    parts.ResetAndDestroy();
    
    delete part;
    
    iLog->Log( _L("Case %d: Nested parts"), iCaseId++ );
    // put an attachment back
    CMsgStorePropertyContainer* nestedProps = CMsgStorePropertyContainer::NewL();
    nestedProps->AddPropertyL(_L8("prop1"), _L("value 1") );
    
    part = message1->AddChildPartL( *properties );          
    CMsgStoreMessagePart* nestedPart = part->AddChildPartL( *nestedProps );
    nestedPart->ReplaceContentL( KContent );
    
    CMsgStoreMessagePart* comparePart = part->ChildPartL( nestedPart->Id() );
    ComparePropertiesL( *comparePart, *nestedProps );
    CheckCondition( _L("content length matches"), comparePart->ContentLengthL() == nestedPart->ContentLengthL() );
    comparePart->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf == KContent );
    
    delete comparePart;
    
    iLog->Log( _L("Case %d: Fetch nested part from mailBox object"), iCaseId++ );
    
    CMsgStoreWritablePropertyContainer* container = mailBox1->FetchPropertiesL( nestedPart->Id() );
    ComparePropertiesL( *container, *nestedPart );
    delete container;
    
    iLog->Log( _L("Case %d: Fetch part from mailBox object"), iCaseId++ );
    
    container = mailBox1->FetchPropertiesL( part->Id() );
    ComparePropertiesL( *container, *part );
    delete container;
    
    iLog->Log( _L("Case %d: Fetch message from mailBox object"), iCaseId++ );
    
    container = mailBox1->FetchPropertiesL( message1->Id() );
    ComparePropertiesL( *container, *message1 );
    delete container;
    
    iLog->Log( _L("Case %d: Fetch folder from mailBox object"), iCaseId++ );
    
    container = mailBox1->FetchPropertiesL( inbox1->Id() );
    ComparePropertiesL( *container, *inbox1 );
    delete container;
    
    iLog->Log( _L("Case %d: ContainerTypeById"), iCaseId++ );
    
    TMsgStoreContainerType contentType = mailBox1->ContainerTypeById( part->Id() );
    CheckCondition( _L("container type is Part"), contentType == EMsgStorePartContainer );
    
    contentType = mailBox1->ContainerTypeById( message1->Id() );
    CheckCondition( _L("container type is Message"), contentType == EMsgStoreMessageContainer );
    
    contentType = mailBox1->ContainerTypeById( inbox1->Id() );
    CheckCondition( _L("container type is Folder"), contentType == EMsgStoreFolderContainer );
    
    contentType = mailBox1->ContainerTypeById( mailBox1->Id() );
    CheckCondition( _L("container type is Mailbox"), contentType == EMsgStoreMailboxContainer );
    
    /******************************************************************
     * (end of) Testing of the new CMsgStoreMailBox methods: FetchPropertiesL() and
     * ContentTypeBuId() requested by the Mail for Exchange team
     * ****************************************************************/
    
    iLog->Log( _L("Case %d: Add a message as an attachment (embedded msg)"), iCaseId++ );
    
    CMsgStoreMessage* nestedMsg = part->AddChildMessageL( *nestedProps );
    nestedMsg->ReplaceContentL( KContent );
    comparePart = part->ChildPartL( nestedMsg->Id() );
    ComparePropertiesL( *nestedMsg, *comparePart );
    comparePart->FetchContentToBufferL( contentBuf );
    CheckCondition( _L("content matches"), contentBuf == KContent );
    CheckCondition( _L("IsEmbeddedMessage()"), comparePart->IsEmbeddedMessageL() );

    iLog->Log( _L("Case %d: CommitL() not supported for embedded msgs"), iCaseId++ );
    //it should be safe to cast it to message
    CMsgStoreMessage* castedMessage = static_cast<CMsgStoreMessage*>(comparePart);
    //CommitL() should leave with KErrNotSupported
    TRAPD( err, castedMessage->CommitL() );
    CheckCondition( _L("CommitL() not supported for embedded msgs"), err = KErrNotSupported );
    
    iLog->Log( _L("Case %d: AbandonL() not supported for embedded msgs"), iCaseId++ );
    TRAP( err, castedMessage->AbandonL() );
    CheckCondition( _L("AbandonL() not supported for embedded msgs"), err = KErrNotSupported );
    
    RPointerArray<CMsgStoreMessagePart> nestedParts;
    part->ChildPartsL( nestedParts );
    CheckCondition( _L("2 nested parts"), nestedParts.Count() == 2 );
    
    nestedParts.ResetAndDestroy();
    delete comparePart;
    delete nestedMsg;
    delete nestedPart;
    delete part;
    delete nestedProps;
    
    // ERROR PATH TESTS
    
    // put an attachment back
    part = message1->AddChildPartL( *properties );          
    CleanupStack::PushL( part );                                                //+7
    
    iLog->Log( _L("Case %d: Error - ChildPartL for invalid part ID"), iCaseId++ );
    
    TRAPD( result, message1->ChildPartL( EMsgStorePartBits | 50 ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - ChildPartL for wrong message"), iCaseId++ );
    
    TRAP( result, message2->ChildPartL( part->Id() ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    iLog->Log( _L("Case %d: Error - AddAttachmentL (content) with nonexistent file"), iCaseId++ );
    
    CMsgStoreMessagePart* tempPart = message2->AddChildPartL( *properties );
    TRAP( result, tempPart->ReplaceContentWithFileL( _L("C:\\some_nonexistent_file.txt") ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    delete tempPart;

    iLog->Log( _L("Case %d: Error - RemoveChildPartL for invalid part ID"), iCaseId++ );
    
    TRAP( result, message2->RemoveChildPartL( part->Id() + 50  ) );
    CheckCondition( _L("not found"), result == KErrNotFound );
    
    // wrong parameter types

    iLog->Log( _L("Case %d: Error - ChildPartL with wrong type for part"), iCaseId++ );
    
    TRAP( result, message2->ChildPartL( message1->Id() ) );
    CheckCondition( _L("argument error"), result == KErrArgument );
    
    iLog->Log( _L("Case %d: Error - RemoveChildPartL with wrong type for part"), iCaseId++ );
    
    TRAP( result, message2->RemoveChildPartL( message1->Id() ) );
    CheckCondition( _L("argument error"), result == KErrArgument );
    
    // OUT OF MEMORY ERROR PATH TESTS

    iLog->Log( _L("Case %d: Error - Out of memory"), iCaseId++ );
    for( TInt i = 0; i < 1; i++ )
        {        
        SimulateLowDiskSpaceL( i );
        TRAP( result, message1->AddChildPartL( *properties ) );
        CheckCondition( _L("out of memory"), result == KErrNoMemory );
        }
        
    CleanupStack::PopAndDestroy( 7 );
    
    //we are done 
    return ETrue;
    }


//  End of File
