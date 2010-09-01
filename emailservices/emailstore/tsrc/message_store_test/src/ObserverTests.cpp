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
#include <connect/sbdefs.h>
#include <e32property.h>
#include "ObserverTests.h"
//#include <ismsgstorepropertykeys.h>
#include "messagestoreclientserver.h"

const TInt32 KOwner1 = 1234;
const TInt32 KOwner2 = 5678;

_LIT( KAccount1, "my account 1");
_LIT( KAccount2, "my account 2");

//PointSec related tests. Must match ImsPointsecMonitor.h
// KPsUidPointsec
// The Pointsec status as defined by TPointsecStatus
#if 0
const TUint KUidPointsecStatus = { 0x102070e3 };
#endif

enum TPointsecStatus
    {
    EPointsecRecognizerStarted = 1,
    EPointsecInitializing,
    EPointsecLocked,
    EPointsecUnlocked,
    EPointSecUnknown,              //Do not know what 5 means, ignore it for now
    EPointSecEncryptionStarting,   //We received this while PointSec says "Encrypting Files"
                                   //encryption is done, we received EPointSecUnlocked
    EPointsecNotStarted = -1
    };



// ============================ MEMBER FUNCTIONS ===============================

CObserverTests* CObserverTests::NewL( CStifLogger* aLog )
    {
    CObserverTests* self = new(ELeave) CObserverTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CObserverTests::CObserverTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CObserverTests::~CObserverTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CObserverTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CObserverTests::ExecuteL()
    {
    LogHeader( _L("Observer") );
    
    CMsgStore* session1 = CMsgStore::NewL();
    CleanupStack::PushL( session1 );                                            //+1
    CMsgStore* session2 = CMsgStore::NewL();
    CleanupStack::PushL( session2 );                                            //+2

    StartRecordingObserverEvents();         

    TObserverEvent         event;
    RArray<TObserverEvent> expectedEvents;

    iLog->Log( _L("Case %d: Add observer events"), iCaseId++ );
    
    session1->AddObserverL( this );
    session2->AddObserverL( this );
    
    // one per each session         
    event.iMailBoxId   = -1;
    event.iOperation   = EMsgStoreAvailable;
    event.iType        = -1;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = -1;
    event.iParentId    = -1;
    event.iOtherId     = -1;    
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Create Account events"), iCaseId++ );
    
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewLC( KOwner1, KAccount1 ); //+3
    CMsgStoreMailBox* mailBox1 = session1->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                            //+4
    
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAccountCreated;
    event.iType        = KMsgStoreInvalidId;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = KOwner1;
    event.iParentId    = KMsgStoreInvalidId;
    event.iOtherId     = KMsgStoreInvalidId;    
    event.iName.Copy( KAccount1 );
    event.iNewName.Copy( KNullDesC );
    
    expectedEvents.AppendL( event );
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    CMsgStoreAccount* account2 = CMsgStoreAccount::NewLC( KOwner2, KAccount2 ); //+5
    CMsgStoreMailBox* mailBox2 = session2->CreateAccountL( *account2 );
    CleanupStack::PushL( mailBox2 );                                            //+6

    event.iMailBoxId   = mailBox2->Id();
    event.iOperation   = EMsgStoreAccountCreated;
    event.iType        = KMsgStoreInvalidId;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = KOwner2;
    event.iParentId    = KMsgStoreInvalidId;
    event.iOtherId     = KMsgStoreInvalidId;    
    event.iName.Copy( KAccount2 );
    event.iNewName.Copy( KNullDesC );
    
    expectedEvents.AppendL( event );
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Rename Account events"), iCaseId++ );
    
    session1->RenameAccountL( KOwner1, KAccount1, KAccount2 );
    
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAccountRenamed;
    event.iType        = KMsgStoreInvalidId;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = KOwner1;
    event.iParentId    = KMsgStoreInvalidId;
    event.iOtherId     = KMsgStoreInvalidId;    
    event.iName.Copy( KAccount1 );
    event.iNewName.Copy( KAccount2 );
    
    expectedEvents.AppendL( event );
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         

    iLog->Log( _L("Case %d: Delete Account events"), iCaseId++ );
    
    CMsgStoreAccount* newAccount = CMsgStoreAccount::NewLC( KOwner1, KAccount2 ); //+7
    session1->DeleteAccountL( *newAccount );
    
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAccountDeleted;
    event.iType        = KMsgStoreInvalidId;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = KOwner1;
    event.iParentId    = KMsgStoreInvalidId;
    event.iOtherId     = KMsgStoreInvalidId;    
    event.iName.Copy( KAccount2 );
    event.iNewName.Copy( KNullDesC );
    
    expectedEvents.AppendL( event );
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Create Account events"), iCaseId++ );
    
    mailBox1 = session1->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                            //+8
    
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAccountCreated;
    event.iType        = KMsgStoreInvalidId;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = KOwner1;
    event.iParentId    = KMsgStoreInvalidId;
    event.iOtherId     = KMsgStoreInvalidId;    
    event.iName.Copy( KAccount1 );
    event.iNewName.Copy( KNullDesC );
    
    expectedEvents.AppendL( event );
    
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Mailbox events"), iCaseId++ );
    
    mailBox1->AddObserverL( this );
    
    //This mailbox should NOT get any notifications for events on mailBox1
    mailBox2->AddObserverL( this );  
    
    ///This mailbox should get notifications for ALL events on mailBox1
    CMsgStoreMailBox *mailBox1_too = session2->OpenAccountL( *account1 );
    CleanupStack::PushL( mailBox1_too );                                          //+9
    mailBox1_too->AddObserverL( this );
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties );                                            //+10
    _LIT8( KProperty1, "PROP1" );
    _LIT8( KProperty2, "PROP2" );
    TestAddIntegerPropertyL( *properties, KProperty1, 1 );
    TestAddIntegerPropertyL( *properties, KProperty2, 1 );
    
    iLog->Log( _L("Case %d: Create Folder events"), iCaseId++ );
    
    TMsgStoreId folderId = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAdd;
    event.iType        = EMsgStoreFolderContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = folderId;
    event.iParentId    = mailBox1->Id();
    event.iOtherId     = KMsgStoreRootMailBoxId;    
    event.iName.SetLength(0);
    event.iNewName.SetLength(0);
    
    expectedEvents.AppendL( event );

    TMsgStoreId folder2Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAdd;
    event.iType        = EMsgStoreFolderContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = folder2Id;
    event.iParentId    = mailBox1->Id();
    event.iOtherId     = KMsgStoreRootMailBoxId;    
    event.iName.SetLength(0);
    event.iNewName.SetLength(0);
    
    expectedEvents.AppendL( event );
    
    iLog->Log( _L("Case %d: Create Message events"), iCaseId++ );
    
    CMsgStoreMessage* message1 = mailBox1->CreateMessageL( folderId, *properties );
    CleanupStack::PushL( message1 );                                                //+11
    message1->CommitL();
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAdd;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = message1->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    event.iName.SetLength(0);
    event.iNewName.SetLength(0);
    
    expectedEvents.AppendL( event );

    TestAddIntegerPropertyL( *properties, KMsgStorePropertyFlags, 12345 );

    CMsgStoreMessage* message2 = mailBox1->CreateMessageL( folderId, *properties );
    CleanupStack::PushL( message2 );                                                //+12
    message2->CommitL();
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAdd;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = 12345;
    event.iId          = message2->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    event.iName.SetLength(0);
    event.iNewName.SetLength(0);
    expectedEvents.AppendL( event );
    
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Create Part events"), iCaseId++ );
    
    CMsgStoreMessagePart* part1 = message1->AddChildPartL( *properties );
    CleanupStack::PushL( part1 );                                                    //+13
    
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAdd;
    event.iType        = EMsgStorePartContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = part1->Id();
    event.iParentId    = message1->Id();
    event.iOtherId     = message1->ParentId();  
    event.iName.SetLength(0);
    event.iNewName.SetLength(0);
    expectedEvents.AppendL( event );
    
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    _LIT( KFilename, "C:\\test_content.txt" );
    _LIT8( KContent, "THIS IS THE CONTENT FOR THIS TEST" );
    RFs   fs;
    User::LeaveIfError( fs.Connect() );
    RFile file;
    User::LeaveIfError( file.Replace( fs, KFilename, EFileWrite ) );
    file.Write( KContent );
    file.Close();
    fs.Close();
    
    part1->ReplaceContentWithFileL( KFilename  );

    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateContent;
    event.iType        = EMsgStorePartContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = part1->Id();
    event.iParentId    = message1->Id();
    event.iOtherId     = message1->ParentId();  
    event.iName.SetLength(0);
    event.iNewName.SetLength(0);
    
    expectedEvents.AppendL( event );    
    
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    // Test message creation with operations between CreateMessageL and CommitL.
    iLog->Log( _L("Case %d: Test operations between CreateMessageL and CommitL"), iCaseId++ );

    TUint index;
    properties->FindProperty( KMsgStorePropertyFlags, index );
    TestUpdateIntegerPropertyL( *properties, index, 99999 );
    
    CMsgStoreMessage* message3 = mailBox1->CreateMessageL( folderId, *properties );
    CleanupStack::PushL( message3 );                                                      //+14
    message3->ReplaceContentL( _L8("CONTENT") );
    CMsgStoreMessagePart* part3 = message3->AddChildPartL( *properties );
    part3->ReplaceContentL( _L8("NEW CONTENT" ) );
    part3->AppendToContentL( _L8("MORE CONTENT" ) );
    part3->ReplaceContentWithFileL( KFilename );
    message3->CommitL();
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAdd;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = 99999;
    event.iId          = message3->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    event.iName.SetLength(0);
    event.iNewName.SetLength(0);
    expectedEvents.AppendL( event );
    delete part3;

    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Move events"), iCaseId++ );
    
    mailBox1->MoveMessageL( message1->Id(), folderId, folder2Id );          
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreMove;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = message1->Id();
    event.iParentId    = folderId;
    event.iOtherId     = folder2Id; 
    expectedEvents.AppendL( event );
        
    mailBox1->MoveMessageL( message1->Id(), folder2Id, folderId );
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreMove;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iType        = EMsgStoreMessageContainer;
    event.iId          = message1->Id();
    event.iParentId    = folder2Id;
    event.iOtherId     = folderId;  
    expectedEvents.AppendL( event );
        
    mailBox1->MoveMessageL( message3->Id(), folderId, folder2Id );
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreMove;
    event.iFlags       = 99999;
    event.iType        = EMsgStoreMessageContainer;
    event.iId          = message3->Id();
    event.iParentId    = folderId;
    event.iOtherId     = folder2Id; 
    expectedEvents.AppendL( event );
        
    mailBox1->MoveMessageL( message3->Id(), folder2Id, folderId );
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreMove;
    event.iFlags       = 99999;
    event.iType        = EMsgStoreMessageContainer;
    event.iId          = message3->Id();
    event.iParentId    = folder2Id;
    event.iOtherId     = folderId;  
    expectedEvents.AppendL( event );
        
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Update properties events"), iCaseId++ );
    
    message1->StorePropertiesL();           
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateProperties;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = message1->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    expectedEvents.AppendL( event );
        
    message2->StorePropertyL( 0 );          
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateProperties;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = 12345;
    event.iId          = message2->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    expectedEvents.AppendL( event );
    
    CMsgStoreFolder* folder = mailBox1->FolderL( folderId );    
    CleanupStack::PushL( folder );                                               //+15
    folder->StorePropertiesL();         
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateProperties;
    event.iType        = EMsgStoreFolderContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = folderId;
    event.iParentId    = mailBox1->Id();
    event.iOtherId     = KMsgStoreRootMailBoxId;    
    expectedEvents.AppendL( event );                
        
    folder->StorePropertyL( 0 );            
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateProperties;
    event.iType        = EMsgStoreFolderContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = folderId;
    event.iParentId    = mailBox1->Id();
    event.iOtherId     = KMsgStoreRootMailBoxId;    
    expectedEvents.AppendL( event );                
        
    part1->StorePropertiesL();          
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateProperties;
    event.iType        = EMsgStorePartContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = part1->Id();
    event.iParentId    = message1->Id();
    event.iOtherId     = folderId;  
    expectedEvents.AppendL( event );
    
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Update content events"), iCaseId++ );
    
    part1->AppendToContentL( _L8("SOME MORE CONTENT") );                            
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateContent;
    event.iType        = EMsgStorePartContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = part1->Id();
    event.iParentId    = message1->Id();
    event.iOtherId     = folderId;  
    expectedEvents.AppendL( event );
        
    message1->ReplaceContentL( _L8("NEW CONTENT") );                            
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateContent;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = message1->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    expectedEvents.AppendL( event );
        
    message2->ReplaceContentWithFileL( KFilename );                         
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreUpdateContent;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = message2->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    expectedEvents.AppendL( event );
        
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    iLog->Log( _L("Case %d: Remove content events"), iCaseId++ );
    
    part1->RemoveContentL();
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreRemoveContent;
    event.iType        = EMsgStorePartContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = part1->Id();
    event.iParentId    = message1->Id();
    event.iOtherId     = folderId;  
    expectedEvents.AppendL( event );
        
    message2->RemoveContentL();                         
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreRemoveContent;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = message2->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    expectedEvents.AppendL( event );
        
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
        
    iLog->Log( _L("Case %d: Copy events"), iCaseId++ );
    
    TMsgStoreId copyId = mailBox1_too->CopyMessageL( message1->Id(), folderId, folder2Id );

    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreAdd;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = copyId;
    event.iParentId    = folder2Id;
    //Per Gypsy UI's request, when copying messages, we now send the original message id
    // in the aOtherId field of the notification ( MotificationNotify() ). 
    event.iOtherId     = message1->Id();    
    expectedEvents.AppendL( event );
        
    VerifyRecordedObserverEventsL( EFalse, expectedEvents );            
            
    iLog->Log( _L("Case %d: Delete events"), iCaseId++ );
        
    mailBox1->DeleteMessageL( message2->Id(), folderId );
    
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreDelete;
    event.iType        = EMsgStoreMessageContainer;
    event.iFlags       = 12345;
    event.iId          = message2->Id();
    event.iParentId    = folderId;
    event.iOtherId     = mailBox1->Id();    
    
    expectedEvents.AppendL( event );
    
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
        
    message1->RemoveChildPartL( part1->Id() );
    
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreDelete;
    event.iType        = EMsgStorePartContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = part1->Id();
    event.iParentId    = message1->Id();
    event.iOtherId     = folderId;  
    expectedEvents.AppendL( event );
        
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );         
    
    mailBox1->DeleteFolderL( folderId );
        
    event.iMailBoxId   = mailBox1->Id();
    event.iOperation   = EMsgStoreDelete;
    event.iType        = EMsgStoreFolderContainer;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = folderId;
    event.iParentId    = mailBox1->Id();
    event.iOtherId     = KMsgStoreRootMailBoxId;    
    expectedEvents.AppendL( event );
        
    VerifyRecordedObserverEventsL( ETrue, expectedEvents );

    iLog->Log( _L("Case %d: Database wiped system event"), iCaseId++ );
            
    session1->WipeEverythingL();
    
    // one per each session         
    event.iMailBoxId   = -1;
    event.iOperation   = EMsgStoreDatabaseWiped;
    event.iType        = -1;
    event.iFlags       = (TUint32)-1;
    event.iId          = -1;
    event.iParentId    = -1;
    event.iOtherId     = -1;
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    VerifyRecordedObserverEventsL( EFalse, expectedEvents );            
    
    iLog->Log( _L("Case %d: Backup/restore system events"), iCaseId++ );

    TriggerBackupRestoreEventL( conn::EBURBackupFull );

    // one per each session         
    event.iMailBoxId   = -1;
    event.iOperation   = EMsgStoreBackupOrRestoreInProgress;
    event.iType        = -1;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = -1;
    event.iParentId    = -1;
    event.iOtherId     = -1;    
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    // one per each session         
    event.iOperation   = EMsgStoreUnavailable;
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    VerifyRecordedObserverEventsL( EFalse, expectedEvents );            
    
    
    TriggerBackupRestoreEventL( 0 );

    // one per each session         
    event.iOperation   = EMsgStoreBackupOrRestoreCompleted;
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    // one per each session         
    event.iOperation   = EMsgStoreAvailable;
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

   VerifyRecordedObserverEventsL( EFalse, expectedEvents );            
        
    iLog->Log( _L("Case %d: PointSec system lock events"), iCaseId++ );

    TriggerPointSecEventL( ETrue );

    // one per each session         
    event.iMailBoxId   = -1;
    event.iOperation   = EMsgStorePointSecLockStarted;
    event.iType        = -1;
    event.iFlags       = KMsgStoreFlagsNotFound;
    event.iId          = -1;
    event.iParentId    = -1;
    event.iOtherId     = -1;    
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    // one per each session         
    event.iOperation   = EMsgStoreUnavailable;
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    VerifyRecordedObserverEventsL( EFalse, expectedEvents );            
    
    TriggerPointSecEventL( EFalse );

    // one per each session         
    event.iOperation   = EMsgStorePointSecLockEnded;
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    // one per each session         
    event.iOperation   = EMsgStoreAvailable;
    
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    VerifyRecordedObserverEventsL( EFalse, expectedEvents );            
        
    
    //Begin PointSec
#if 0    
    TInt rc = RProperty::Define( KPsUidPointsec, KUidPointsecStatus, RProperty::EInt );
    rc = RProperty::Set( KPsUidPointsec, KUidPointsecStatus, EPointsecLocked );
    event.iOperation   = EMsgStorePointSecLockStarted;
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    VerifyRecordedObserverEventsL( EFalse, expectedEvents );
    
    rc = RProperty::Set( KPsUidPointsec, KUidPointsecStatus, EPointsecUnlocked );
    event.iOperation   = EMsgStorePointSecLockEnded;
    expectedEvents.AppendL( event );
    expectedEvents.AppendL( event );

    VerifyRecordedObserverEventsL( EFalse, expectedEvents );
#endif    
    //End PointSec
    
    StopRecordingObserverEvents();
    
    
    iLog->Log( _L("Case %d: Remove Mailbox Observers"), iCaseId++ );
    mailBox1->RemoveObserverL( this );
    mailBox2->RemoveObserverL( this );
    
    iLog->Log( _L("Case %d: Large number of observers on large number of sessions"), iCaseId++ );

    RPointerArray<CMsgStore> sessions;
    
    for( TInt i = 0; i < 20; i++ )
        {
        CMsgStore* session = CMsgStore::NewL();
        session->AddObserverL( this );
        
        sessions.AppendL( session );        
        }

    session1->WipeEverythingL();
    
    for( TInt i = 0; i < 20; i++ )
        {
        sessions[i]->RemoveObserverL( this );
        sessions[i]->AddObserverL( this );        
        }

    session1->WipeEverythingL();
    
    sessions.ResetAndDestroy();
    
    CleanupStack::PopAndDestroy( 15 );
    
    //we are done 
    return ETrue;
    }


void CObserverTests::StartRecordingObserverEvents()
    {
    iRecordEvents = ETrue;
    }

void CObserverTests::StopRecordingObserverEvents()
    {
    iRecordEvents = ETrue;
    iRecordedEvents.Reset();
    }

void CObserverTests::VerifyRecordedObserverEventsL( TBool aVerifyOrder, RArray<TObserverEvent>& aExpectedEvents )
    {
    iLog->Log( _L("  >> VerifyRecordedObserverEventsL") );
    
    // Allow all events to propagate.
    TInt loopCount = 0;
    while( loopCount < 1000 && iRecordedEvents.Count() < aExpectedEvents.Count() )
        {
        Yield( 10000 );
        loopCount++;
        }

    TBool matched = ( aExpectedEvents.Count() == iRecordedEvents.Count() );
    
    CheckCondition( _L("correct number of recorded events"), matched );
    
    TObserverEvent expectedEvent;
    TObserverEvent recordedEvent;
    
    while( matched && (aExpectedEvents.Count() > 0 ) )
        {
        expectedEvent = aExpectedEvents[0];
        
        TUint highestCompareIndex = 0; 
        if( !aVerifyOrder )
            {
            highestCompareIndex = iRecordedEvents.Count() - 1;
            }
            
        TBool found = EFalse;
        TUint compareIndex = 0;
        while( !found && compareIndex <= highestCompareIndex )
            {
            recordedEvent = iRecordedEvents[compareIndex];
            
            if (recordedEvent.iMailBoxId   == expectedEvent.iMailBoxId &&
                recordedEvent.iOperation   == expectedEvent.iOperation &&
                recordedEvent.iType        == expectedEvent.iType      &&
                recordedEvent.iFlags       == expectedEvent.iFlags     &&
                recordedEvent.iId          == expectedEvent.iId        &&
                recordedEvent.iParentId    == expectedEvent.iParentId  &&
                recordedEvent.iOtherId     == expectedEvent.iOtherId   &&
                recordedEvent.iName        == expectedEvent.iName      &&
                recordedEvent.iNewName     == expectedEvent.iNewName )
                {
                found = ETrue;
                iRecordedEvents.Remove( compareIndex );
                }
            else
                {                   
                compareIndex++;                     
                } // end if
            }
        aExpectedEvents.Remove( 0 );
        
        matched = (matched && found);
        }
    
    aExpectedEvents.Reset();
    
    CheckCondition( _L("recorded events matched"), matched );
    
    iLog->Log( _L("  << VerifyRecordedObserverEventsL") );
    }

void CObserverTests::ModificationNotify( TMsgStoreId aMailBoxId, TMsgStoreOperation aOperation, TMsgStoreContainerType aType, TUint32 aFlags, TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aOtherId )
    {
    iLog->Log( _L("  ModificationNotify %i"), iRecordedEvents.Count()+1 );
    
    if( iRecordEvents )
        {
        TObserverEvent event;
        event.iMailBoxId   = aMailBoxId;
        event.iOperation   = aOperation;
        event.iType        = aType;
        event.iFlags       = aFlags;
        event.iId          = aId;
        event.iParentId    = aParentId;
        event.iOtherId     = aOtherId;
        event.iName.SetLength( 0 );
        event.iNewName.SetLength( 0 );
        
        iRecordedEvents.Append( event );
        }
    }
        
void CObserverTests::SystemEventNotify( TMsgStoreSystemEvent aEvent )
    {
    iLog->Log( _L("  >> SystemEventNotify") );
    
    if( iRecordEvents )
        {
        TObserverEvent event;
        event.iMailBoxId   = -1;
        event.iOperation   = aEvent;
        event.iType        = -1;
        event.iFlags       = KMsgStoreFlagsNotFound;
        event.iId          = -1;
        event.iParentId    = -1;
        event.iOtherId     = -1;
        event.iName.SetLength( 0 );
        event.iNewName.SetLength( 0 );
        
        iRecordedEvents.Append( event );
        }
    
    iLog->Log( _L("  << SystemEventNotify") );
    }
void CObserverTests::AccountEventNotify( TMsgStoreAccountEvent aEvent, TInt32 aOwnerId, const TDesC& aName, const TDesC& aNewName, TMsgStoreId aMailboxId )
    {
    iLog->Log( _L("  >> AccountEventNotify") );
    if( iRecordEvents )
        {
        TObserverEvent event;
        event.iMailBoxId   = aMailboxId;
        event.iOperation   = aEvent;
        event.iType        = KMsgStoreInvalidId;
        event.iFlags       = KMsgStoreFlagsNotFound;
        event.iId          = aOwnerId;
        event.iParentId    = KMsgStoreInvalidId;
        event.iOtherId     = KMsgStoreInvalidId;
        event.iName.Copy( aName );
        event.iNewName.Copy( aNewName );
        
        iRecordedEvents.Append( event );
        }
    iLog->Log( _L("  << AccountEventNotify") );
    }

//  End of File
