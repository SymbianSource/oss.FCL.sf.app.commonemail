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
#include "SearchTests.h"
//#include <ismsgstorepropertykeys.h>
//#include "messagestoreclientserver.h"

_LIT(KHigh,   "High");
_LIT(KNormal, "Normal");
_LIT(KLow,    "Low");

_LIT(KFollowUp,         "FollowUp");
_LIT(KFollowUpComplete, "FollowUpComplete");
_LIT(KNone,             "None");

// ============================ MEMBER FUNCTIONS ===============================

CSearchTests* CSearchTests::NewL( CStifLogger* aLog )
    {
    CSearchTests* self = new(ELeave) CSearchTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CSearchTests::CSearchTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CSearchTests::~CSearchTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CSearchTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CSearchTests::ExecuteL()
    {
    LogHeader( _L("Search") );
    
    //account 1
    CMsgStoreAccount* account1 = CMsgStoreAccount::NewLC( 1234, KNullDesC );   //+1
    CMsgStoreMailBox* mailBox1 = iMsgStore->CreateAccountL( *account1 );
    CleanupStack::PushL( mailBox1 );                                           //+2
    
    //NOTE: the CreateRandomMessagesL() only generage string fields with 'A' through 'Z' and 'a' though 'z'.
    // So to avoid the ramdom message have strings that match the search criteria, the search criteria must
    // include some non-alphabetical characters, e.g., ' ', '.', etc.
    _LIT( KDisplayName1, "FName1 LName1" );
    _LIT( KDisplayName2, "FName2 LName2" );
    _LIT( KDisplayName3, "FName3 LName3" );
    _LIT( KAddress1, "email1.addr1@yahoo.com" );
    _LIT( KAddress2, "email2.addr2@yahoo.com" );
    _LIT( KAddress3, "email3.addr3@yahoo.com" );
    _LIT( KSubject1,  "Real subject 1." );
    _LIT( KSubject2,  "Real subject 2." );
    _LIT( KSubject3,  "Real subject 3." );
    _LIT( KContent1, "This is the subject of a message." );
    _LIT( KContent2, "The quick brown fox jumped over the lazy dog.  Hurray!" );
    _LIT( KContent3, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()" );
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties );                                          //+3
    
    properties->AddPropertyL( _L8("1"), _L("PROPERTY 1") );
    properties->AddPropertyL( _L8("2"), 12345 );
    properties->AddPropertyL( _L8("3"), _L8("ANOTHER PROPERTY") );
    
    //create 3 folders, but only search 2 of them
    TMsgStoreId inbox1Id = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    TMsgStoreId sentFolderId = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    TMsgStoreId deletedFolderId = mailBox1->CreateFolderL( mailBox1->Id(), *properties );
    
    //create messages for the 3 folders
    TInt msgCountPerFolder = 20;
    
    CreateRandomMessagesL( mailBox1, inbox1Id, msgCountPerFolder );
    CreateRandomMessagesL( mailBox1, sentFolderId, msgCountPerFolder );
    CreateRandomMessagesL( mailBox1, deletedFolderId, msgCountPerFolder );
    
    //get message ids for inbox
    RArray<TMsgStoreId> msgIds;
    mailBox1->MessageIdsL( inbox1Id, msgIds );
    RPointerArray<CMsgStoreMessage> inboxMsgs;
    for ( TInt i = 0 ; i < msgCountPerFolder ; i++ )
        {
        inboxMsgs.Append( mailBox1->FetchMessageL( msgIds[i], inbox1Id ) );
        }
    msgIds.Reset();
    
    //get message ids for sentFolderId
    mailBox1->MessageIdsL( sentFolderId, msgIds );
    RPointerArray<CMsgStoreMessage> sentMsgs;
    for ( TInt i = 0 ; i < msgCountPerFolder ; i++ )
        {
        sentMsgs.Append( mailBox1->FetchMessageL( msgIds[i], sentFolderId ) );
        }
    msgIds.Reset();
    
    //get message ids for deletedFolder
    mailBox1->MessageIdsL( deletedFolderId, msgIds );
    RPointerArray<CMsgStoreMessage> deletedMsgs;
    for ( TInt i = 0 ; i < msgCountPerFolder ; i++ )
        {
        deletedMsgs.Append( mailBox1->FetchMessageL( msgIds[i], deletedFolderId ) );
        }
    msgIds.Close();
    
    TBuf8<500> contentBuf;

    for ( TInt i = 0 ; i < 3 ; i++ )
        {
        RPointerArray<CMsgStoreMessage>* msgs;
        if ( i == 0 )
            {
            msgs = &inboxMsgs;
            }
        else if ( i == 1 )
            {
            msgs = &sentMsgs;
            }
        else
            {
            msgs = &deletedMsgs;
            }
        CopyDesToDes8( KContent1, contentBuf );
        SetMessageContentL( (*msgs)[0], contentBuf );
        SetMessageContentL( (*msgs)[3], contentBuf );
        SetMessageContentL( (*msgs)[4], contentBuf );
    
        CopyDesToDes8( KContent2, contentBuf );
        SetMessageContentL( (*msgs)[1], contentBuf );
        SetMessageContentL( (*msgs)[6], contentBuf );
    
        CopyDesToDes8( KContent3, contentBuf );
        SetMessageContentL( (*msgs)[2], contentBuf );
        SetMessageContentL( (*msgs)[5], contentBuf );
        SetMessageContentL( (*msgs)[7], contentBuf );
        SetMessageContentL( (*msgs)[8], contentBuf );
        
        SetSubjectAndAddressL( (*msgs)[11], KSubject1, KDisplayName1, KAddress1, KMsgStorePropertyTo );
        SetSubjectAndAddressL( (*msgs)[14], KSubject1, KDisplayName1, KAddress1, KMsgStorePropertyCc );
        SetSubjectAndAddressL( (*msgs)[17], KSubject1, KDisplayName1, KAddress1, KMsgStorePropertyBcc );
        
        SetSubjectAndAddressL( (*msgs)[12], KSubject2, KDisplayName2, KAddress2, KMsgStorePropertyTo );
        SetSubjectAndAddressL( (*msgs)[15], KSubject2, KDisplayName2, KAddress2, KMsgStorePropertyCc );
        SetSubjectAndAddressL( (*msgs)[18], KSubject2, KDisplayName2, KAddress2, KMsgStorePropertyBcc );
        
        SetSubjectAndAddressL( (*msgs)[13], KSubject3, KDisplayName3, KAddress3, KMsgStorePropertyTo );
        SetSubjectAndAddressL( (*msgs)[16], KSubject3, KDisplayName3, KAddress3, KMsgStorePropertyCc );
        //make #19 use name 3 and addr2
        SetSubjectAndAddressL( (*msgs)[19], KSubject3, KDisplayName3, KAddress2, KMsgStorePropertyBcc );
        
        //add 3 TOs to message 10, 3 CCs to msg 13, and 3 BCCs to msg 16
        RMsgStoreAddress addr;
        CleanupClosePushL( addr );
        addr.iDisplayName.Create(100);
        addr.iEmailAddress.Create(100);
        
        addr.iDisplayName.Copy( KDisplayName1 );
        addr.iEmailAddress.Copy( KAddress1 );
        (*msgs)[10]->AddPropertyL( KMsgStorePropertyTo, addr ) ;
        (*msgs)[13]->AddPropertyL( KMsgStorePropertyCc, addr ) ;
        (*msgs)[16]->AddPropertyL( KMsgStorePropertyBcc, addr ) ;
        
        addr.iDisplayName.Copy( KDisplayName2 );
        addr.iEmailAddress.Copy( KAddress2 );
        (*msgs)[10]->AddPropertyL( KMsgStorePropertyTo, addr ) ;
        (*msgs)[13]->AddPropertyL( KMsgStorePropertyCc, addr ) ;
        (*msgs)[16]->AddPropertyL( KMsgStorePropertyBcc, addr ) ;
        
        addr.iDisplayName.Copy( KDisplayName3 );
        addr.iEmailAddress.Copy( KAddress3 );
        (*msgs)[10]->AddPropertyL( KMsgStorePropertyTo, addr ) ;
        (*msgs)[13]->AddPropertyL( KMsgStorePropertyCc, addr ) ;
        (*msgs)[16]->AddPropertyL( KMsgStorePropertyBcc, addr ) ;

        (*msgs)[10]->StorePropertiesL();
        (*msgs)[13]->StorePropertiesL();
        (*msgs)[16]->StorePropertiesL();
        
        CleanupStack::PopAndDestroy( &addr );
        }
    
    RArray<TMsgStoreId> expectedMatches;

    RMsgStoreSearchCriteria criteria;
    CleanupClosePushL( criteria );                                                // +4
    iSearchCriteria = &criteria;
    
    criteria.AddSearchStringL( _L("no match") );
    criteria.AddSearchStringL( _L("The") );
    criteria.iSortBy = EMsgStoreSortByReceivedDate;
    criteria.iSortOrder = EMsgStoreSortDescending;
    
    //WMT: test "global search". when no filder id is specified, all folders are searched
    //search inbox1Id first, than sendFolderId
    //criteria.AddFolderId( inbox1Id );
    //criteria.AddFolderId( sentFolderId );
    
    criteria.AddResultPropertyL( KMsgStorePropertyReceivedAt );
    criteria.AddResultPropertyL( KMsgStorePropertyFlags );
    criteria.AddResultPropertyL( KMsgStorePropertyMessageSizeOnServer );
    criteria.AddResultPropertyL( KMsgStorePropertyFrom );
    criteria.AddResultPropertyL( KMsgStorePropertySubject );
    
    iLog->Log( _L("Case %d: Search for \"%S\" AND \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.SearchStrings()[1], criteria.iSortBy );

    mailBox1->SearchL( criteria, *this );            
    ValidateSearchResults( expectedMatches );
    
    //search for "The", using the wild card...
    criteria.SearchStrings().ResetAndDestroy();
//    criteria.AddSearchStringL( _L("T?e") );
    criteria.AddSearchStringL( _L("The ") );  //add a space to avoid random match
    
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    
    expectedMatches.AppendL( inboxMsgs[0]->Id() );
    expectedMatches.AppendL( inboxMsgs[3]->Id() );
    expectedMatches.AppendL( inboxMsgs[4]->Id() );
    expectedMatches.AppendL( inboxMsgs[1]->Id() );
    expectedMatches.AppendL( inboxMsgs[6]->Id() );
    expectedMatches.AppendL( sentMsgs[0]->Id() );
    expectedMatches.AppendL( sentMsgs[3]->Id() );
    expectedMatches.AppendL( sentMsgs[4]->Id() );
    expectedMatches.AppendL( sentMsgs[1]->Id() );
    expectedMatches.AppendL( sentMsgs[6]->Id() );
    expectedMatches.AppendL( deletedMsgs[0]->Id() );
    expectedMatches.AppendL( deletedMsgs[3]->Id() );
    expectedMatches.AppendL( deletedMsgs[4]->Id() );
    expectedMatches.AppendL( deletedMsgs[1]->Id() );
    expectedMatches.AppendL( deletedMsgs[6]->Id() );
    
    mailBox1->SearchL( criteria, *this );
    //call validate but ignore extra match that may be generated by the wild card
    //because the ramdom message may happen to match the wild card
    ValidateSearchResults( expectedMatches, ETrue );
    
    //change the sort order
    criteria.iSortBy = EMsgStoreSortByFlagStatus;
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    mailBox1->SearchL( criteria, *this );
    //should have the same results as previous serach, just different sort order
    ValidateSearchResults( expectedMatches, ETrue );
    
    //change the sort order again
    criteria.iSortBy = EMsgStoreSortBySize;
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    mailBox1->SearchL( criteria, *this );
    //should have the same results as previous serach, just different sort order
    ValidateSearchResults( expectedMatches, ETrue );
    
    expectedMatches.Reset();
    
    //search for "HURRAY!"
    criteria.SearchStrings().ResetAndDestroy();
    criteria.AddSearchStringL( _L("HURRAY!") );  //added '!' to avoid match random string
    criteria.iSortBy = EMsgStoreSortBySubject;
    
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    
    expectedMatches.AppendL( inboxMsgs[1]->Id() );
    expectedMatches.AppendL( inboxMsgs[6]->Id() );
    expectedMatches.AppendL( sentMsgs[1]->Id() );
    expectedMatches.AppendL( sentMsgs[6]->Id() );
    
    //search inbox1Id first, than sendFolderId
    criteria.AddFolderId( inbox1Id );
    criteria.AddFolderId( sentFolderId );
    
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );
    
    //change sort order
    criteria.iSortBy = EMsgStoreSortByUnRead;
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy  );
    mailBox1->SearchL( criteria, *this );
    //should have the same results as previous serach, just different sort order
    ValidateSearchResults( expectedMatches );
    expectedMatches.Reset();
    
    //search for "()"
    criteria.SearchStrings().ResetAndDestroy();
    criteria.AddSearchStringL( _L("()") );
    criteria.iSortBy = EMsgStoreSortBySender;
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy  );
    
    expectedMatches.AppendL( inboxMsgs[2]->Id() );
    expectedMatches.AppendL( inboxMsgs[5]->Id() );
    expectedMatches.AppendL( inboxMsgs[7]->Id() );
    expectedMatches.AppendL( inboxMsgs[8]->Id() );
    expectedMatches.AppendL( sentMsgs[2]->Id() );
    expectedMatches.AppendL( sentMsgs[5]->Id() );
    expectedMatches.AppendL( sentMsgs[7]->Id() );
    expectedMatches.AppendL( sentMsgs[8]->Id() );
    
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );

    //Change the sort order
    criteria.iSortBy = EMsgStoreSortByPriority;
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    //should have the same results as previous serach, just different sort order
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );
    
    //Change the sort order again
    criteria.iSortBy = EMsgStoreSortByAttachment;
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    //should have the same results as previous serach, just different sort order
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );
    
    expectedMatches.Reset();
    
    //search subject fields
    criteria.iSortBy = EMsgStoreSortByReceivedDate;
    criteria.SearchStrings().ResetAndDestroy();
    criteria.AddSearchStringL( _L("subject ") );
    
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    
    expectedMatches.AppendL( inboxMsgs[0]->Id() );
    expectedMatches.AppendL( inboxMsgs[3]->Id() );
    expectedMatches.AppendL( inboxMsgs[4]->Id() );
    expectedMatches.AppendL( sentMsgs[0]->Id() );
    expectedMatches.AppendL( sentMsgs[3]->Id() );
    expectedMatches.AppendL( sentMsgs[4]->Id() );
    for (TInt i=11; i<=19; i++)
        {
        expectedMatches.AppendL( inboxMsgs[i]->Id() );
        expectedMatches.AppendL( sentMsgs[i]->Id() );
        }
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );
    expectedMatches.Reset();
    
    //search addresses fields
    criteria.SearchStrings().ResetAndDestroy();
    
    criteria.AddSearchStringL( _L("fname3") );
    criteria.AddSearchStringL( _L("addr3") );
    
    iLog->Log( _L("Case %d: Search for \"%S\" AND \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.SearchStrings()[1], criteria.iSortBy );
    
    expectedMatches.AppendL( inboxMsgs[10]->Id() );
    expectedMatches.AppendL( inboxMsgs[13]->Id() );
    expectedMatches.AppendL( inboxMsgs[16]->Id() );
//    expectedMatches.AppendL( inboxMsgs[19]->Id() );
    expectedMatches.AppendL( sentMsgs[10]->Id() );
    expectedMatches.AppendL( sentMsgs[13]->Id() );
    expectedMatches.AppendL( sentMsgs[16]->Id() );
//    expectedMatches.AppendL( sentMsgs[19]->Id() );
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );
    expectedMatches.Reset();
    
    //search addresses fields
    criteria.SearchStrings().ResetAndDestroy();
    criteria.AddSearchStringL( _L("addr2") );
    
    iLog->Log( _L("Case %d: Search for \"%S\", ordered by:%d"), iCaseId++, criteria.SearchStrings()[0], criteria.iSortBy );
    
    expectedMatches.AppendL( inboxMsgs[12]->Id() );
    expectedMatches.AppendL( inboxMsgs[15]->Id() );
    expectedMatches.AppendL( inboxMsgs[18]->Id() );
    expectedMatches.AppendL( inboxMsgs[10]->Id() );
    expectedMatches.AppendL( inboxMsgs[13]->Id() );
    expectedMatches.AppendL( inboxMsgs[16]->Id() );
    expectedMatches.AppendL( inboxMsgs[19]->Id() );
    expectedMatches.AppendL( sentMsgs[12]->Id() );
    expectedMatches.AppendL( sentMsgs[15]->Id() );
    expectedMatches.AppendL( sentMsgs[18]->Id() );
    expectedMatches.AppendL( sentMsgs[10]->Id() );
    expectedMatches.AppendL( sentMsgs[13]->Id() );
    expectedMatches.AppendL( sentMsgs[16]->Id() );
    expectedMatches.AppendL( sentMsgs[19]->Id() );
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );
    expectedMatches.Reset();
    
    iLog->Log( _L("Case %d: Cancel search"), iCaseId++ );

    criteria.SearchStrings().ResetAndDestroy();
    criteria.AddSearchStringL( _L("the ") );
    mailBox1->SearchL( criteria, *this );
    mailBox1->CancelSearch();
    for( TInt i = 0; i < 10; i++ )
        {                
        Yield( 25000 );
        }
    CheckCondition( _L("MatchFound not called"), iMatches.Count() == 0 );
    CheckCondition( _L("SearchCompleted not called"), !iSearchCompleted );
    
    iLog->Log( _L("Case %d: Cancel search during MatchFound callback"), iCaseId++ );
    
    iSessionToCancelSearch = mailBox1;
    criteria.SearchStrings().ResetAndDestroy();
    criteria.AddSearchStringL( _L("the ") );
    mailBox1->SearchL( criteria, *this );
    for( TInt i = 0; i < 10; i++ )
        {                
        Yield( 25000 );
        }
    CheckCondition( _L("MatchFound not called again"), iMatches.Count() == 1 );
    CheckCondition( _L("SearchCompleted not called"), !iSearchCompleted );
    iMatches.Reset();
    iSessionToCancelSearch = NULL;
    
    iLog->Log( _L("Case %d: Large content"), iCaseId++ );
    
    _LIT( KFilename, "c:\\content_test.txt" );
    _LIT( KSearchString, "this is the search string" );     
    CreateFileL( KFilename, 13000, 'X', KSearchString, 0 );     // beginning of file
    ReplaceMessageContentWithFileL( inboxMsgs[0], KFilename );
    CreateFileL( KFilename, 25000, 'X', KSearchString, 15813 ); // middle
    ReplaceMessageContentWithFileL( inboxMsgs[1], KFilename );
    CreateFileL( KFilename, 8763, 'X', KSearchString, 8762 );   // end
    ReplaceMessageContentWithFileL( inboxMsgs[2], KFilename );
    
    // THIS CONSTANT SHOULD BE KEPT CONSISTENT WITH THE SEARCH BUFFER SIZE IN CSearchHandler       
    const TInt KSearchBufferSize = 2500;
    CreateFileL( KFilename, 5000, 'X', KSearchString, KSearchBufferSize/2-2 ); 
    ReplaceMessageContentWithFileL( sentMsgs[3], KFilename );
    CreateFileL( KFilename, 7500, 'X', KSearchString, KSearchBufferSize-2 ); 
    ReplaceMessageContentWithFileL( sentMsgs[4], KFilename );
    
    criteria.SearchStrings().ResetAndDestroy();
    criteria.AddSearchStringL( KSearchString );
    expectedMatches.AppendL( inboxMsgs[0]->Id() );
    expectedMatches.AppendL( inboxMsgs[1]->Id() );
    expectedMatches.AppendL( inboxMsgs[2]->Id() );
    expectedMatches.AppendL( sentMsgs[3]->Id() );
    expectedMatches.AppendL( sentMsgs[4]->Id() );
    mailBox1->SearchL( criteria, *this );
    ValidateSearchResults( expectedMatches );
    expectedMatches.Reset();
    
    iLog->Log( _L("Case %d: Cleanup the search cache"), iCaseId++ ) ;
    mailBox1->ClearSearchResultCache();

    // ERROR CASES
    
    iLog->Log( _L("Case %d: Error - Search while another search is in progress"), iCaseId++ ) ;

    mailBox1->SearchL( criteria, *this );
    TRAPD( result, mailBox1->SearchL( criteria, *this ) );
    CheckCondition( _L("in use error"), result == KErrInUse );
        
    expectedMatches.AppendL( inboxMsgs[0]->Id() );
    expectedMatches.AppendL( inboxMsgs[1]->Id() );
    expectedMatches.AppendL( inboxMsgs[2]->Id() );
    expectedMatches.AppendL( sentMsgs[3]->Id() );
    expectedMatches.AppendL( sentMsgs[4]->Id() );
    ValidateSearchResults( expectedMatches );
    expectedMatches.Reset();
    
    iLog->Log( _L("Case %d: Error - Wrong type for folder"), iCaseId++ ) ;
    criteria.AddFolderId( 50 );
    
    TRAP( result,  mailBox1->SearchL( criteria, *this ) );
    CheckCondition( _L("argument error"), result == KErrArgument );

    criteria.FolderIds().Reset();
    
    criteria.AddFolderId( inbox1Id );
    criteria.AddFolderId( sentFolderId );
    
    iLog->Log( _L("Case %d: Error - Zero length search string"), iCaseId++ ) ;
    criteria.SearchStrings().ResetAndDestroy();
    
    TRAP( result, mailBox1->SearchL( criteria, *this ) );
    CheckCondition( _L("argument error"), result == KErrArgument );
    
    deletedMsgs.ResetAndDestroy();
    sentMsgs.ResetAndDestroy();
    inboxMsgs.ResetAndDestroy();
    
    iMatches.Reset();
    
    mailBox1->ClearSearchResultCache();
    
    CleanupStack::PopAndDestroy( 4 );    
    //we are done 
    return ETrue;
    }


void CSearchTests::MatchFound( CMsgStorePropertyContainer*& aMessage )
    {
    iLog->Log( _L("  >> MatchFound" ) );
    
    TUint index;
    TUint flags = 0;
    TInt err;
    if ( aMessage->FindProperty( KMsgStorePropertyFlags, index ) )
        {
        TRAP( err, flags = aMessage->PropertyValueUint32L( index ) );
        }
    
    TTime time;
    if ( aMessage->FindProperty( KMsgStorePropertyReceivedAt, index ) )
        {
        TRAP( err, aMessage->PropertyValueTimeL( index, time ) );
        }
    
    switch( iSearchCriteria->iSortBy )
    {
    case EMsgStoreSortByReceivedDate:
        iLog->Log( _L("  msg=%x, folder=%x, received date=%Ld"), aMessage->Id(), aMessage->ParentId(), time.Int64());
        break;
        
    case EMsgStoreSortBySender:
        if ( aMessage->FindProperty( KMsgStorePropertyFrom, index ) )
            {
            RMsgStoreAddress address;
            TRAP( err, aMessage->PropertyValueAddressL( index, address ) );
            iLog->Log( _L("  msg=%x, folder=%x, from=%S(%S)"), aMessage->Id(), aMessage->ParentId(), &address.iDisplayName, &address.iEmailAddress );
            address.Close();
            }
        else
            {
            iLog->Log( _L("  msg=%x folder=%x"), aMessage->Id(), aMessage->ParentId() );
            }
        break;
        
    case EMsgStoreSortBySubject:
        if ( aMessage->FindProperty( KMsgStorePropertySubject, index ) )
            {
            TRAP(err, const TDesC& subject = aMessage->PropertyValueDesL( index ) ;
                      iLog->Log( _L("  msg=%x, folder=%x, subj=%S"), aMessage->Id(), aMessage->ParentId(), &subject ) );
            }
        else
            {
            iLog->Log( _L("  msg=%x, folder=%x"), aMessage->Id(), aMessage->ParentId() );
            }
        break;
        
    case EMsgStoreSortByPriority:
        iLog->Log( _L("  msg=%x, folre=%x, priority=%S date=%Ld"), aMessage->Id(), aMessage->ParentId(), (flags & EMsgStoreFlag_Important ? &KHigh : (flags & EMsgStoreFlag_Low ? &KLow : &KNormal )) , time.Int64() );
        break;
        
    case EMsgStoreSortByFlagStatus:
        iLog->Log( _L("  msg=%x, folder=%x, flagStatus=%S date=%Ld"), aMessage->Id(), aMessage->ParentId(), (flags & EMsgStoreFlag_FollowUp ? &KFollowUp : (flags & EMsgStoreFlag_FollowUpComplete ? &KFollowUpComplete : &KNone )), time.Int64());
        break;
        
    case EMsgStoreSortByUnRead:
        {
        TInt readUnread = (TInt) flags & ( EMsgStoreFlag_Read | EMsgStoreFlag_Read_Locally );
        iLog->Log( _L("  msg=%x, folder=%x, readFlag=%d date=%Ld"), aMessage->Id(), aMessage->ParentId(), readUnread, time.Int64() );
        }
        break;
        
    case EMsgStoreSortBySize:
        {
        TUint size = 0;
        if ( aMessage->FindProperty( KMsgStorePropertyMessageSizeOnServer, index ) )
            {
            TRAP( err, size = aMessage->PropertyValueUint32L( index ) );
            }
        iLog->Log( _L("  msg=%x, folder=%x, size=%u date=%Ld"), aMessage->Id(), aMessage->ParentId(), size, time.Int64() );
        }
        break;
        
    case EMsgStoreSortByAttachment:
        {
        TBool hasAttachment = (flags & EMsgStoreFlag_Attachments > 0 );
        iLog->Log( _L("  msg=%x, folder=%x, attachmentFlag=%d date=%Ld"), aMessage->Id(), aMessage->ParentId(), hasAttachment, time.Int64());
        }
        break;
    }
    
    iMatches.Append( aMessage->Id() );
    
    if( iSessionToCancelSearch != NULL )
        {
        iSessionToCancelSearch->CancelSearch();
        }
    
    delete aMessage;
    aMessage = NULL;
    
    iLog->Log( _L("  << MatchFound" ) );
    }

void CSearchTests::SearchCompleted()
    {           
    iLog->Log( _L("  >> SearchCompleted, matches=%i"), iMatches.Count() );
    iSearchCompleted = ETrue;
    iLog->Log( _L("  << SearchCompleted" ) );
    }   

void CSearchTests::ValidateSearchResults( RArray<TMsgStoreId>& aExpectedMatches, TBool aIgnoreExtra )
    {
    iLog->Log( _L("  >> ValidateSearchResults" ) );
    
    while( !iSearchCompleted )
        {
        Yield( 1000 );
        }

    TBool matched = ETrue;
    for( TInt index = 0; index < aExpectedMatches.Count(); index++ )
        {
        TInt foundIndex = iMatches.Find( aExpectedMatches[index] );               
        if( foundIndex == KErrNotFound )
            {
            iLog->Log( _L("  %x not found"), aExpectedMatches[index] );
            matched = EFalse;
            }
        }
    
    if( matched && !aIgnoreExtra )
        {
        for( TInt index = 0; index < iMatches.Count(); index++ )
            {
            TInt foundIndex = aExpectedMatches.Find( iMatches[index] );               
            if( foundIndex == KErrNotFound )
                {
                iLog->Log( _L("  %x not expected"), iMatches[index] );
                matched = EFalse;
                }
            }
        }
    
    if( !matched )
        {
        for( TInt index = 0; index < aExpectedMatches.Count(); index++ )
            {
            iLog->Log( _L("  expected=%x"), aExpectedMatches[index] );
            }
        for( TInt index = 0; index < iMatches.Count(); index++ )
            {
            iLog->Log( _L("  found=%x"), iMatches[index] );
            }                
        }
    
    CheckCondition( _L("search results matched"), matched );            
    
    iMatches.Reset();            
    
    iSearchCompleted = EFalse;
    
    iLog->Log( _L("  << ValidateSearchResults" ) );
    }

void CSearchTests::ReplaceMessageContentWithFileL( CMsgStoreMessage* aMessage, const TDesC& aFilename )
    {
    RPointerArray<CMsgStoreMessagePart> parts;
    aMessage->ChildPartsL( parts );
    
    CMsgStoreMessagePart* body = parts[0];
    CMsgStoreMessagePart* plainTextPart = body;
    
    RPointerArray<CMsgStoreMessagePart> childParts;
    body->ChildPartsL( childParts );
    
    if ( childParts.Count() > 0 )
        {
        plainTextPart = childParts[0];
        }
    
    plainTextPart->ReplaceContentWithFileL( aFilename );

    childParts.ResetAndDestroy();
    parts.ResetAndDestroy();
    }

void CSearchTests::SetSubjectAndAddressL( CMsgStoreMessage* aMessage, const TDesC& aSubject, const TDesC& aDisplayName, const TDesC& aAddress, const TDesC8& aAddressType )
    {
    aMessage->AddOrUpdatePropertyL( KMsgStorePropertySubject, aSubject );
    
    RMsgStoreAddress addr;
    CleanupClosePushL( addr );
    
    addr.iDisplayName.Create( aDisplayName );
    addr.iEmailAddress.Create( aAddress );
    
    aMessage->AddOrUpdatePropertyL( aAddressType, addr );
    
    CleanupStack::PopAndDestroy( &addr );
    aMessage->StorePropertiesL();
    }


//  End of File
