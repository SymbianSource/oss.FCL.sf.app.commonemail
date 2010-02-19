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
#include <s32file.h>
#include <d32dbms.h>
#include "DbCorruptTests.h"
#include <emailstoreuids.hrh>

//these must match those defined in ContainerStoreDefs.
_LIT( KDbFilename, "message_store.db" );
_LIT( KGeneralTableName,                      "G001" );
_LIT( KGeneralTableDbSchemaVersionCol,        "G002" );

// ============================ MEMBER FUNCTIONS ===============================

CDbCorruptTests* CDbCorruptTests::NewL( CStifLogger* aLog )
    {
    CDbCorruptTests* self = new(ELeave) CDbCorruptTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CDbCorruptTests::CDbCorruptTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CDbCorruptTests::~CDbCorruptTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CDbCorruptTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CDbCorruptTests::ExecuteL()
    {
    LogHeader( _L("Database Corruption") );
    
    RFs                         fs;
    RFile                       file;
    CPermanentFileStore*        fileStore;
    CMsgStorePropertyContainer* properties;
    RDbStoreDatabase            database;
    RDbTable                    table;
    CDbColSet*                  colSet;
    TUint                       unreadCount;
    TUint                       totalCount;
    TInt                        result;
    TFileName                   dbFileName;
    
    User::LeaveIfError( fs.Connect() );

    properties = CMsgStorePropertyContainer::NewL();

    iLog->Log( _L("Case %d: Database file exists but is empty"), iCaseId++ );
    
    iLog->Log( _L("  Shutdown the server") );
    //shutdown the server
    ShutdownServerL();    
    
    // The file cannot be opened until the server has shut down, so keep trying until it is possible.
    result = KErrInUse;
    while( result == KErrInUse )
        {       
        Yield( 100000 );
        dbFileName.Format( _L("C:\\private\\%x\\%S"), KUidMessageStoreExe, &KDbFilename );
        result = file.Replace( fs, dbFileName, EFileWrite );
        }
        
    file.Close();       
    
    CMsgStore* session = CMsgStore::NewL( ETrue );
    CleanupStack::PushL( session );                                           //+1
    
    iLog->Log( _L("Case %d: Create Mailbox"), iCaseId++ );
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );   //+2
    CMsgStoreMailBox* mailbox1 = session->CreateAccountL( *account1 );
    CleanupStack::PushL( mailbox1 );                                           //+3
    
    iLog->Log( _L("Case %d: Message counts are 0"), iCaseId++ );
    mailbox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("no crash, counts are zero"), totalCount == 0 && unreadCount == 0 );
    
    iLog->Log( _L("Case %d: Create Folder"), iCaseId++ );
    TMsgStoreId inbox1Id = mailbox1->CreateFolderL( mailbox1->Id(), *properties );
    CMsgStoreFolder* inbox1 = mailbox1->FolderL( inbox1Id );
    CleanupStack::PushL( inbox1 );                                             //+4
    
    iLog->Log( _L("Case %d: Create Message"), iCaseId++ );
    CMsgStoreMessage* msg = mailbox1->CreateMessageL( inbox1Id, *properties );
    delete msg;
    
    CleanupStack::PopAndDestroy(4);

    //shutdown the server again
    iLog->Log( _L("  Shutdown the server again") );
    ShutdownServerL();    
    
    iLog->Log( _L("Case %d: Database file exists but contains bad content"), iCaseId++ );
    result = KErrInUse;
    while( result == KErrInUse )
        {       
        Yield( 100000 );
        result = file.Replace( fs, dbFileName, EFileWrite );
        }
    
    file.Write( _L8("RANDOM CONTENT") );
    file.Close();       
    
    session = CMsgStore::NewL( ETrue );
    CleanupStack::PushL( session );                                            //+1
    
    iLog->Log( _L("Case %d: Create Mailbox"), iCaseId++ );
    account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );   //+2
    mailbox1 = session->CreateAccountL( *account1 );
    CleanupStack::PushL( mailbox1 );                                           //+3
    
    iLog->Log( _L("Case %d: Message counts are 0"), iCaseId++ );
    mailbox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("no crash, counts are zero"), totalCount == 0 && unreadCount == 0 );
    
    iLog->Log( _L("Case %d: Create Folder"), iCaseId++ );
    inbox1Id = mailbox1->CreateFolderL( mailbox1->Id(), *properties );
    inbox1 = mailbox1->FolderL( inbox1Id );
    CleanupStack::PushL( inbox1 );                                             //+4
    
    iLog->Log( _L("Case %d: Create Message"), iCaseId++ );
    msg = mailbox1->CreateMessageL( inbox1Id, *properties );

    delete msg;
    CleanupStack::PopAndDestroy(4);
    
    //shutdown the server again
    iLog->Log( _L("  Shutdown the server again") );
    ShutdownServerL();
    
    iLog->Log( _L("Case %d: Database file exists and contains an empty database"), iCaseId++ );

    result = KErrInUse;
    while( result == KErrInUse )
        {       
        Yield( 100000 );
        TRAP( result, fileStore = CPermanentFileStore::ReplaceL( fs, dbFileName, EFileRead|EFileWrite ) );
        }
    
    fileStore->SetTypeL( fileStore->Layout() );     
    TStreamId id = database.CreateL( fileStore );   
    fileStore->SetRootL( id );  
    fileStore->CommitL();
    database.Close();
    delete fileStore;

    session = CMsgStore::NewL( ETrue );
    CleanupStack::PushL( session );                                            //+1
    
    iLog->Log( _L("Case %d: Create Mailbox"), iCaseId++ );
    
    account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );   //+2
    mailbox1 = session->CreateAccountL( *account1 );
    CleanupStack::PushL( mailbox1 );                                           //+3
    
    iLog->Log( _L("Case %d: Message counts are 0"), iCaseId++ );
    mailbox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("no crash, counts are zero"), totalCount == 0 && unreadCount == 0 );
    
    iLog->Log( _L("Case %d: Create Folder"), iCaseId++ );
    inbox1Id = mailbox1->CreateFolderL( mailbox1->Id(), *properties );
    inbox1 = mailbox1->FolderL( inbox1Id );
    CleanupStack::PushL( inbox1 );                                             //+4
    
    iLog->Log( _L("Case %d: Create Message"), iCaseId++ );
    msg = mailbox1->CreateMessageL( inbox1Id, *properties );
    delete msg;
    
    CleanupStack::PopAndDestroy(4);    
    
    //shutdown the server again
    iLog->Log( _L("  Shutdown the server again") );
    ShutdownServerL();
    
    iLog->Log( _L("Case %d: Database schema mismatch"), iCaseId++ );

    result = KErrInUse;
    while( result == KErrInUse )
        {       
        Yield( 100000 );
        TRAP( result, fileStore = CPermanentFileStore::OpenL( fs, dbFileName, EFileRead|EFileWrite ) );
        }

    database.OpenL( fileStore, fileStore->Root() );     
    User::LeaveIfError( table.Open( database, KGeneralTableName ) );  
    colSet = table.ColSetL();
    TUint schemaVersionColNum = colSet->ColNo( KGeneralTableDbSchemaVersionCol );
    delete colSet;  
    table.FirstL();
    table.UpdateL();
    table.SetColL( schemaVersionColNum, 0 );
    table.PutL();
    table.Close();
    database.Close();
    delete fileStore;

    session = CMsgStore::NewL( ETrue );
    CleanupStack::PushL( session );                                            //+1
    
    iLog->Log( _L("Case %d: Create Mailbox"), iCaseId++ );
    account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );                     //+2
    mailbox1 = session->CreateAccountL( *account1 );
    CleanupStack::PushL( mailbox1 );                                           //+3
    
    iLog->Log( _L("Case %d: Message counts are 0"), iCaseId++ );
    mailbox1->TotalMessageCountsL( totalCount, unreadCount );
    CheckCondition( _L("no crash, counts are zero"), totalCount == 0 && unreadCount == 0 );
    
    iLog->Log( _L("Case %d: Create Folder"), iCaseId++ );
    inbox1Id = mailbox1->CreateFolderL( mailbox1->Id(), *properties );
    inbox1 = mailbox1->FolderL( inbox1Id );
    CleanupStack::PushL( inbox1 );                                             //+4
    
    iLog->Log( _L("Case %d: Create Message"), iCaseId++ );
    msg = mailbox1->CreateMessageL( inbox1Id, *properties );

    delete msg;
    CleanupStack::PopAndDestroy(4);
    
    delete properties;      
    fs.Close();
    
    //we are done 
    return ETrue;
    }


//  End of File
