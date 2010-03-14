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
* Description:  implements all sorting related test cases
*
*/


#include <e32math.h>
#include <tz.h>
#include "SortTests.h"

const TInt32 KOwner1 = 1234;
_LIT(KAccount1, "My Account");
_LIT(KInbox,    "inbox");

// ============================ MEMBER FUNCTIONS ===============================

CSortTests* CSortTests::NewL( CStifLogger* aLog )
    {
    CSortTests* self = new(ELeave) CSortTests( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CSortTests::CSortTests( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CSortTests::~CSortTests()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CSortTests::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CSortTests::ExecuteL()
    {
    LogHeader( _L("Sort") );
    
    TInt msgCount = 200;
    
    iLog->Log( _L("  Create %d messages"), msgCount );
    DoCreateMessagesL( msgCount );
    
    //Open Account
    CMsgStoreAccount* account = CMsgStoreAccount::NewLC( KOwner1, KAccount1 );   //+1
    CMsgStoreMailBox* mailBox = iMsgStore->OpenAccountL( *account );
    CleanupStack::PushL( mailBox );                                              //+2
    
    TUint total, unread;
    mailBox->TotalMessageCountsL( total, unread );
    iLog->Log( _L("  Total msg=%u, unread=%u"), total, unread);
    
    RPointerArray<CMsgStoreFolder> folders;
    
    mailBox->FoldersL( mailBox->Id(), folders );
    
    TMsgStoreId inboxId = 0;
    for ( TInt ii = 0 ; ii < folders.Count() ; ii++ )
        {
        CMsgStoreFolder* folder = folders[ii];
        TUint index = 0;
        if ( folder->FindProperty( KMsgStorePropertyName, index ) )
            {
            if ( folder->PropertyValueDesL( index ) == KInbox )
                {
                //found inbox
                inboxId = folder->Id();
                break;
                }
            }
        }
    folders.ResetAndDestroy();
    
    if ( inboxId == 0 )
        {
        User::Leave( KErrNotFound );
        }
    
    
    RArray<RMsgStoreSortCriteria>              criteriaArray;
    RPointerArray<CMsgStoreSortResultIterator> iteratorArray;
    RArray<TMsgStoreId>                        curIdArray;
    RArray<TBool>                              hasMoreArray;
    RArray<TMsgStoreIteratorDirection>         checkDirectionArray;
    RArray<TInt>                               bucketSizeArray;
    
//    UpdateText( 2, _L("Start sorting") );
    
    RPointerArray<CMsgStorePropertyContainer> results;
    
    //test each sort by field
    for ( TInt i = 0 ; i < 10 ; i++ )
        {
        RMsgStoreSortCriteria criteria;
        criteria.iFolderId = inboxId;
        criteria.AddResultPropertyL( KMsgStorePropertyReceivedAt );
        criteria.AddResultPropertyL( KMsgStorePropertyFlags );
        criteria.AddResultPropertyL( KMsgStorePropertyMessageSizeOnServer );
        criteria.AddResultPropertyL( KMsgStorePropertyFrom );
        criteria.AddResultPropertyL( KMsgStorePropertyTo );
        criteria.AddResultPropertyL( KMsgStorePropertySubject );
        
        switch( i )
            {
            case 0:
                //sort by received date
                iLog->Log( _L("Case %d: Sort By: ReceivedData, Order: Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortByReceivedDate;
                criteria.iSortOrder = EMsgStoreSortDescending;
                break;
            
            case 1:
                //sort by EMsgStoreSortByPriority
                iLog->Log( _L("Case %d: Sort By: Priority, Order: Desc/Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortByPriority;
                criteria.iSortOrder = EMsgStoreSortDescending;          //primary   = DESC
                criteria.iSecondarySortOrder = EMsgStoreSortDescending; //secondary = DESC 
                break;
                
            case 2:
                //sort by EMsgStoreSortByFlagStatus
                iLog->Log( _L("Case %d: Sort By: Flag, Order: Desc/Asc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortByFlagStatus;
                criteria.iSortOrder = EMsgStoreSortDescending;          //Primary   = DESC
                criteria.iSecondarySortOrder = EMsgStoreSortAscending;  //secondary = ASC
                break;
                
            case 3:
                //sort by EMsgStoreSortByUnRead
                iLog->Log( _L("Case %d: Sort By: Unread, Order: Asc/Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortByUnRead;
                criteria.iSortOrder = EMsgStoreSortAscending;           //Primary   = ASC
                criteria.iSecondarySortOrder = EMsgStoreSortDescending;  //Secondary = DESC
                break;
                
            case 4:
                //sort by EMsgStoreSortByAttachment
                iLog->Log( _L("Case %d: Sort By: Attachment, Order: Asc/Asc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortByAttachment;
                criteria.iSortOrder = EMsgStoreSortAscending;            //Primary   = ASC
                criteria.iSecondarySortOrder = EMsgStoreSortAscending;  //Secondary = ASC
                break;
                
            case 5:
                //sort by EMsgStoreSortBySize
                iLog->Log( _L("Case %d: Sort By: Size, Order: Desc/Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortBySize;
                criteria.iSortOrder = EMsgStoreSortDescending;           //Primary   = DESC
                criteria.iSecondarySortOrder = EMsgStoreSortDescending;  //Secondary = DESC
                break;
                
            case 6:
                //sort by EMsgStoreSortBySender
                iLog->Log( _L("Case %d: Sort By: Sender, Order: Desc/Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortBySender;
                criteria.iSortOrder = EMsgStoreSortDescending;           //Primary   = DESC
                criteria.iSecondarySortOrder = EMsgStoreSortDescending;  //Secondary = DESC
                break;
                
            case 7:
                //sort by EMsgStoreSortByTo
                iLog->Log( _L("Case %d: Sort By: Recipient, Order: Desc/Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortByRecipient;
                criteria.iSortOrder = EMsgStoreSortDescending;           //Primary   = DESC
                criteria.iSecondarySortOrder = EMsgStoreSortDescending;  //Secondary = DESC
                break;
                
            case 8:
                //sort by EMsgStoreSortBySubject
                iLog->Log( _L("Case %d: Sort By: Subject, Order: Asc/Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortBySubject;
                criteria.iSortOrder = EMsgStoreSortAscending;            //Primary   = ASC
                criteria.iSecondarySortOrder = EMsgStoreSortDescending;  //Secondary = DESC
                break;
            
            case 9:
                //sort by EMsgStoreSortBySubject
                iLog->Log( _L("Case %d: Sort By: Subject, Order: Asc/Desc"), iCaseId++ );
                criteria.iSortBy   = EMsgStoreSortBySubject;
                criteria.iSortOrder = EMsgStoreSortAscending;            //Primary   = ASC
                criteria.iSecondarySortOrder = EMsgStoreSortDescending;  //Secondary = DESC
                break;
                
            default:
                break;
            }
        
        criteriaArray.Append( criteria );
        
        //set the direction
        TMsgStoreIteratorDirection direction = static_cast<TMsgStoreIteratorDirection>(i % 2);   //forward (next) or backward (previous)
        checkDirectionArray.Append( direction );
        
        //initialize currentId, if forward, start from the top, if backward, start from the bottom
        TMsgStoreId curId = ( direction == EMsgStoreIteratorForward ) ? KMsgStoreSortResultTop : KMsgStoreSortResultBottom ;
        curIdArray.Append( curId );
        
        //initialze hasMore to true
        hasMoreArray.Append(ETrue);

        CMsgStoreSortResultIterator* iterator = mailBox->SortL( criteria, (i % 2 ? ETrue : EFalse) ); 
        
        for( TInt jj = 0; jj < 10; jj++ )
            {                
            Yield( 25000 );
            }
        
        if ( i != 5 && i <= 8 )
            {
            iLog->Log( _L("Case %d: Test GroupCountL"), iCaseId++ );
            
            RArray<TUint> itemCounts;
            CleanupClosePushL( itemCounts );
            TInt count = iterator->GroupCountL( itemCounts );
            iLog->Log( _L("  GroupCount=%d"), count );
            
            CleanupStack::PopAndDestroy( &itemCounts );
            }
        
        TBool hasMore = EFalse;
        TInt  bucketSize = 20;

        //get the results
        do {
            results.ResetAndDestroy();
               
            TTime startTime;
            startTime.HomeTime();
            
            TInt startId = curIdArray[i];
               
            if ( checkDirectionArray[i] == EMsgStoreIteratorForward )
                {
                
                if ( i == 9 )
                    {
                    iLog->Log( _L("Case %d: Calling NextL(string)"), iCaseId++ );
                    hasMore = iterator->NextL( _L("nzz") , bucketSize, results );
                    }
                else
                    {
                    iLog->Log( _L("Case %d: Calling NextL"), iCaseId++ );
                    hasMore = iterator->NextL( curIdArray[i] , bucketSize, results );
                    }
                }
            else
                {
                if ( i == 9 )
                    {
                    iLog->Log( _L("Case %d: Calling PreviousL(string)"), iCaseId++ );
                    hasMore = iterator->PreviousL( _L("nzz") , bucketSize, results );
                    }
                else
                    {
                    iLog->Log( _L("Case %d: Calling PreviousL()"), iCaseId++ );
                    hasMore = iterator->PreviousL( curIdArray[i] , bucketSize, results );
                    }
                }
               
            iLog->Log( _L("  Comparing results") );
            curIdArray[i] = CompareSortResultsL( criteriaArray[i], checkDirectionArray[i], results );
            
            //TEST SkipAndNext() and SkupAndPrevioud
            if ( i >= 0 && i <= 8 )
                {
                RPointerArray<CMsgStorePropertyContainer> skipResults;
                
                startTime.HomeTime();
                
                TBool hasNextGroup = ETrue;
                if ( checkDirectionArray[i] == EMsgStoreIteratorForward )
                    {
                    iLog->Log( _L("Case %d: Testing SkipAndNextL()"), iCaseId++ );
                    
                    hasNextGroup = iterator->SkipAndNextL( startId , bucketSize, skipResults );
                    }
                else
                    {
                    iLog->Log( _L("Case %d: Testing SkipAndPreviousL()"), iCaseId++ );
                    
                    hasNextGroup = iterator->SkipAndPreviousL( startId, bucketSize, skipResults );
                    }
                iLog->Log( _L("  Skip returns %d"), hasNextGroup);
                   
                iLog->Log( _L("  Comparing results") );
                CompareSortResultsL( criteriaArray[i], checkDirectionArray[i], skipResults );
                skipResults.ResetAndDestroy();
                }
              
            if ( bucketSize == 100 )
                {
                hasMore = EFalse;
                }
            else
                {
                bucketSize += 10;
                }
            
            //add a message while there are sorting sessions
            if ( bucketSize == 30 )
                {
                iLog->Log( _L("Case %d: Add messages while sort session is open"), iCaseId++ );
                CreateRandomMessagesL( mailBox, inboxId, 2, ETrue, ETrue);
                }
            else if ( bucketSize == 40 )
                {
                //delete iMatches[0]
                iLog->Log( _L("Case %d: Delete message while sort session is open"), iCaseId++ );
                mailBox->DeleteMessageL( iMatches[0], inboxId );
                }
            else if ( bucketSize == 50 )
                {
                iLog->Log( _L("Case %d: Update message while sort session is open"), iCaseId++ );
                CMsgStoreMessage* msg = mailBox->FetchMessageL( iMatches[1], inboxId );
                CleanupStack::PushL( msg );                                                    //+msg
                
                TTime time;
                time.HomeTime();
                TInt64 seed = time.Int64();
                
                TInt64 date = Math::Rand( seed );
                TTime randTime( date );
                msg->AddOrUpdatePropertyL( KMsgStorePropertyReceivedAt, randTime );
                
                TUint32 flags = Math::Rand( seed );
                msg->AddOrUpdatePropertyL( KMsgStorePropertyFlags, (TUint32) flags );
                
                TUint32 size = Math::Rand( seed );
                msg->AddOrUpdatePropertyL( KMsgStorePropertyMessageSizeOnServer, (TUint32) size );
                TBuf<30> subject;
                CreateRandomString( subject, 30, seed );
                msg->AddOrUpdatePropertyL( KMsgStorePropertySubject, subject );
                
                TBuf<6> fname, lname;
                CreateRandomString( fname, 6, seed );
                CreateRandomString( lname, 6, seed );
                RMsgStoreAddress address;
                address.iDisplayName.Create( 50 );
                address.iEmailAddress.Create( 50 );
                
                address.iDisplayName.Format( _L("%S %S"), &fname, &lname );
                address.iEmailAddress.Format( _L("%S.%S@%S"), &fname, &lname, &_L("nokia.com"));
                msg->AddOrUpdatePropertyL( KMsgStorePropertyFrom, address );
                
                address.iDisplayName.Append(_L("TO"));
                msg->AddOrUpdatePropertyL( KMsgStorePropertyTo, address );
                
                msg->StorePropertiesL();
                
                //iLog->Log( _L("  msgId=%x, flags=%x, size=%d, date=%Ld"), msg->Id(), flags, size, date );
                //iLog->Log( _L("  subject=%S"), &subject );
                
                address.Close();
                CleanupStack::PopAndDestroy( msg );
                }
            
            //update iMatches[1]
                
        } while ( hasMore );
        
        
        //testing IdsAndFlags
        iLog->Log( _L("Case %d: Testing IdsAndFlagsL()"), iCaseId++ );
        RArray<TMsgStoreIdAndFlag> idsAndFlags;
        iterator->IdsAndFlagsL( idsAndFlags );
        
        iLog->Log( _L("  IdsAndFlagsL returned %d objects"), idsAndFlags.Count() );        
        
        idsAndFlags.Reset();
        
        //testing get sorted MessageIds
        iLog->Log( _L("Case %d: Testing MessageIdsL()"), iCaseId++ );
        RArray<TMsgStoreId> ids;
        //start sorting
        iterator->MessageIdsL( ids );        
        
        iLog->Log( _L("  MessageIds returned %d objects"), ids.Count() );        
        
        ids.Reset();
        
        //testing IdsAndGroupCount
        iLog->Log( _L("Case %d: Testing IdsAndGroupCountL()"), iCaseId++ );
        RArray<TUint> itemCounts;
        CleanupClosePushL( itemCounts );
        
        iterator->IdsAndGroupCountL( ids, itemCounts );        
        
        iLog->Log( _L("  GroupCount=%d"), itemCounts.Count() );
        
        //if ( itemCounts.Count() > 0 )
        //    {
        //    TInt needNToBeMod8 = 8 - itemCounts.Count() % 8;
        //    if ( needNToBeMod8 != 8 )
        //        {
        //        for ( TInt i = 0 ; i < needNToBeMod8 ; i++ )
        //            {
        //            itemCounts.Append(0);
        //            }
        //        }
        //    
        //    for ( TInt i = 0 ; i < itemCounts.Count() ; i += 8 )
        //        {
        //        iLog->Log( _L("  counts: %d, %d, %d, %d, %d, %d, %d, %d"), 
        //                itemCounts[i], itemCounts[i+1], itemCounts[i+2], itemCounts[i+3],
        //                itemCounts[i+4], itemCounts[i+5], itemCounts[i+6], itemCounts[i+7] );
        //        }
        //    }            
        
        CleanupStack::PopAndDestroy( &itemCounts );
        
        iLog->Log( _L("  MessageIds returned %d objects"), ids.Count() );
        
        ids.Reset();
        
        iLog->Log( _L("Case %d: Testing IndexOfL()"), iCaseId++ );
        //add a message after the iterator has been created
        CreateRandomMessagesL( mailBox, inboxId, 1, ETrue, ETrue );
        TInt index = iterator->IndexOfL( iMatches[0] );

        iLog->Log( _L("  id=%08x, index=%d"), iMatches[0], index);        
        
        delete iterator;
        bucketSizeArray.Append( bucketSize );
        }  //end for
    
    results.ResetAndDestroy();
    iMatches.Reset();

    
    CMsgStoreAccount* account2 = CMsgStoreAccount::NewLC( 9999, _L("") );         //+3
    CMsgStoreMailBox* mailBox2;
    TRAPD(err, mailBox2 = iMsgStore->OpenAccountL( *account2 ) )
    if ( err != KErrNone )
        {
        mailBox2 = iMsgStore->CreateAccountL( *account2 );
        }
    CleanupStack::PushL( mailBox2 );                                             //+4
    CMsgStorePropertyContainer* prop2 = CMsgStorePropertyContainer::NewL();     
    CleanupStack::PushL( prop2 );                                                //+5
    
    TMsgStoreId folder2;
    
    for ( TInt i = 0 ; i < 2 ; i++ )
        {
    
        folder2 = mailBox2->CreateFolderL( mailBox2->Id(), *prop2 );
        
        CreateRandomMessagesL( mailBox2, folder2, 50 );
        
        criteriaArray[0].iFolderId = folder2;
        criteriaArray[criteriaArray.Count()-1].iFolderId = folder2;
        
        CMsgStoreSortResultIterator* iterator1 = mailBox2->SortL( criteriaArray[0] ); 
        CMsgStoreSortResultIterator* iterator2 = mailBox2->SortL( criteriaArray[criteriaArray.Count()-1] ); 
        
        iterator1->NextL( KMsgStoreSortResultTop, 20, results );
        CheckCondition( _L("traverse halfway"), results.Count() == 20 );
        results.ResetAndDestroy();
        
        //now we have two sort session, one traversed to half way, one not travesed yet
        if ( i == 0 )
            {
            iLog->Log( _L("Case %d: Testing delete folder while iterator is open"), iCaseId++ );
            //delete the 
            mailBox2->DeleteFolderL( folder2 );
            }
        else
            {
            //delete the 
            iLog->Log( _L("Case %d: Testing delete mailbox while iterator is open"), iCaseId++ );
            iMsgStore->DeleteAccountL( *account2 );
            }
        
        TBool hasMore = iterator1->NextL( KMsgStoreSortResultTop , 20, results );
        CheckCondition( _L("hasMore==EFalse"), !hasMore );
        CheckCondition( _L("no result"), results.Count() == 0 );
        
        
        hasMore = iterator2->NextL( _L("nzz") , 20, results );
        CheckCondition( _L("hasMore==EFalse"), !hasMore );
        CheckCondition( _L("no result"), results.Count() == 0 );
        
        delete iterator1;
        delete iterator2;
        }
    
    for ( int i = 0 ; i < criteriaArray.Count() ; i++ )
        {
        criteriaArray[i].Close();
        }
    
    criteriaArray.Close();
    iteratorArray.ResetAndDestroy();
    curIdArray.Close();
    hasMoreArray.Close();
    checkDirectionArray.Close();
    bucketSizeArray.Close();
    
    CleanupStack::PopAndDestroy( 5 );
    
    //we are done 
    return ETrue;
    }


void CSortTests::DoCreateMessagesL( TInt aMsgCount )
    {
    iLog->Log( _L("  >> DoCreateMessages") );
    
    TBool isNew = ETrue;
    
    CMsgStore* session = CMsgStore::NewL();                                     //+1
    CleanupStack::PushL( session );

    CMsgStoreAccount* account = CMsgStoreAccount::NewLC( KOwner1, KAccount1 );  //+2
    CMsgStoreMailBox* mailBox = NULL;
    TRAPD( err, mailBox = session->CreateAccountL( *account ) );
    if ( err != KErrNone )
        {
        mailBox = session->OpenAccountL( *account );
        isNew = EFalse;
        }
    CleanupStack::PushL( mailBox );                                            //+3
    
    TMsgStoreId inboxId;
    
    if ( isNew )
        {
        CMsgStorePropertyContainer* prop = CMsgStorePropertyContainer::NewL();
        CleanupStack::PushL( prop );
        
        inboxId = mailBox->CreateFolderL( mailBox->Id(), *prop );
        
        CleanupStack::PopAndDestroy( prop );
        
        CMsgStoreFolder* folder = mailBox->FolderL( inboxId );
        CleanupStack::PushL( folder );
        
        folder->AddPropertyL( KMsgStorePropertyName, KInbox );
        folder->StorePropertiesL();
        
        CleanupStack::PopAndDestroy( folder );
        }
    else
        {
        RPointerArray<CMsgStoreFolder> folders;
        
        mailBox->FoldersL( mailBox->Id(), folders );
        
        for ( int i = 0 ; i < folders.Count() ; i++ )
            {
            CMsgStoreFolder* folder = folders[i];
            TUint index = 0;
            if ( folder->FindProperty( KMsgStorePropertyName, index ) )
                {
                if ( folder->PropertyValueDesL( index ) == KInbox )
                    {
                    //found inbox
                    inboxId = folder->Id();
                    break;
                    }
                }
            }
        folders.ResetAndDestroy();
        }
    
    CreateRandomMessagesL( mailBox, inboxId, aMsgCount );
    
    CleanupStack::PopAndDestroy( 3 );
    iLog->Log( _L("  << DoCreateMessages") );
    }

TMsgStoreId CSortTests::CompareSortResultsL( RMsgStoreSortCriteria aCriteria, 
                                             TMsgStoreIteratorDirection aDirection, 
                                             const RPointerArray<CMsgStorePropertyContainer>& aResults )
    {
    iLog->Log( _L("  >> CompareSortResultsL") );
    iLog->Log( _L("  result count=%d"), aResults.Count() );
    
    TMsgStoreId lastMsgId;
    TUint       index;
    TInt64      curVal1, curVal2, nextVal1, nextVal2;
    TUint       size;
    RBuf        curString, nextString;
    curString.Create(100);
    nextString.Create(100);
    TBuf<100>  timeString;
     
    TBool checkSecondaryOrder = EFalse;
    TMsgStoreSortOrder compareOrder1, compareOrder2;
    
    if ( ( aCriteria.iSortOrder == EMsgStoreSortDescending && aDirection == EMsgStoreIteratorForward ) ||
         ( aCriteria.iSortOrder == EMsgStoreSortAscending &&  aDirection == EMsgStoreIteratorBackward )   )
        {
        compareOrder1 = EMsgStoreSortDescending;
        curVal1 = nextVal1 = KMaxTInt64;
        curString.AppendFill(TChar('z'), 100);
        }
    else
        {
        compareOrder1 = EMsgStoreSortAscending;
        curVal1 = nextVal1 = KMinTInt64;
        curString.AppendFill(TChar('!'), 100);
        }
    
    if ( aCriteria.iSortBy != EMsgStoreSortByReceivedDate )
        {
        checkSecondaryOrder = ETrue;
        if ( ( aCriteria.iSecondarySortOrder == EMsgStoreSortDescending && aDirection == EMsgStoreIteratorForward ) ||
             ( aCriteria.iSecondarySortOrder == EMsgStoreSortAscending &&  aDirection == EMsgStoreIteratorBackward )  )
            {
            compareOrder2 = EMsgStoreSortDescending;
            curVal2 = nextVal2 = KMaxTInt64;
            }
        else
            {
            compareOrder2 = EMsgStoreSortAscending;
            curVal2 = nextVal2 = KMinTInt64;
            }
        }
    
    RTz timeZoneServer;
    User::LeaveIfError( timeZoneServer.Connect() );
    CleanupClosePushL( timeZoneServer );             //+timeZoneServer
    
    for ( int i = 0 ; i < aResults.Count() ; i++ )
        {
        CMsgStorePropertyContainer* msg = aResults[i];
        
        CheckCondition( _L("received date property exists"), msg->FindProperty( KMsgStorePropertyReceivedAt, index ), EFalse ); 
        TTime time;
        msg->PropertyValueTimeL( index, time );
        timeZoneServer.ConvertToLocalTime( time );
        
        CheckCondition( _L("flags property exists"), msg->FindProperty( KMsgStorePropertyFlags, index ), EFalse ); 
        TUint32 flags;
        flags = msg->PropertyValueUint32L( index );
        
        switch( aCriteria.iSortBy )
            {
            case EMsgStoreSortByReceivedDate:
                time.FormatL( timeString, _L("%F%M/%D/%Y %H:%T:%S") );
                nextVal1 = time.Int64();
                //iLog->Log( _L("  msg id=%x, received date=%S"), msg->Id(), &timeString);
                break;
                
            case EMsgStoreSortByPriority:
                nextVal1 = 0;
                if ( flags & EMsgStoreFlag_Important )
                    {
                    nextVal1 = 1;
                    }
                else if ( flags & EMsgStoreFlag_Low )
                    {
                    nextVal1 = -1;
                    }
                nextVal2 = time.Int64();
                break;
                
            case EMsgStoreSortByFlagStatus:
                nextVal1 = 0;
                if ( flags & EMsgStoreFlag_FollowUp )
                    {
                    nextVal1 = 2;
                    }
                else if ( flags & EMsgStoreFlag_FollowUpComplete )
                    {
                    nextVal1 = 1;
                    }
                nextVal2 = time.Int64();
                break;
                
            case EMsgStoreSortByUnRead:
                nextVal1 = flags & ( EMsgStoreFlag_Read | EMsgStoreFlag_Read_Locally );
                nextVal2 = time.Int64();
                //iLog->Log( _L("  msg id=%x, readFlag=%Ld date=%Ld"), msg->Id(), nextVal1, nextVal2);
                break;
                
            case EMsgStoreSortByAttachment:
                nextVal1 = flags & EMsgStoreFlag_Attachments; 
                nextVal2 = time.Int64();
                //iLog->Log( _L("  msg id=%x, attachmentFlag=%Ld date=%Ld"), msg->Id(), nextVal1, nextVal2);
                break;
                
            case EMsgStoreSortBySize:
                size = 0;
                if ( msg->FindProperty( KMsgStorePropertyMessageSizeOnServer, index ) )
                    {
                    size = msg->PropertyValueUint32L( index );
                    }
                nextVal1 = size;
                nextVal2 = time.Int64();
                //iLog->Log( _L("  msg id=%x, size=%Ld date=%Ld"), msg->Id(), nextVal1, nextVal2);
                break;
                
            case EMsgStoreSortBySender:
                {
                CheckCondition( _L("from property exists"), msg->FindProperty( KMsgStorePropertyFrom, index ), EFalse ); 
                RMsgStoreAddress address;
                msg->PropertyValueAddressL( index, address );
                nextString.Copy( address.iDisplayName );
                nextString.Append( address.iEmailAddress );
                address.Close();
                nextVal2 = time.Int64();
                //iLog->Log( _L("  msg id=%x, date=%Ld from=%S"), msg->Id(), nextVal2, &nextString );
                }
                break;
                
            case EMsgStoreSortByRecipient:
                {
                CheckCondition( _L("to property exists"), msg->FindProperty( KMsgStorePropertyTo, index ), EFalse ); 
                RMsgStoreAddress address;
                msg->PropertyValueAddressL( index, address );
                nextString.Copy( address.iDisplayName );
                nextString.Append( address.iEmailAddress );
                address.Close();
                nextVal2 = time.Int64();
                //iLog->Log( _L("  msg id=%x, date=%Ld to=%S"), msg->Id(), nextVal2, &nextString );
                }
                break;
                
            case EMsgStoreSortBySubject:
                {
                CheckCondition( _L("subject property exists"), msg->FindProperty( KMsgStorePropertySubject, index ), EFalse ); 
                const TDesC& subject = msg->PropertyValueDesL( index );
                nextVal2 = time.Int64();
                //iLog->Log( _L("  msg id=%x, date=%Ld subj=%S"), msg->Id(), nextVal2, &subject );
                
                //get rid of the "Alpha:" "AlphaAlpha:" "AlphaAlphaAlpha:"
                nextString.Copy( subject );
                TPtr16 ptr = nextString.MidTPtr(0);
                TBool prefixFound = ETrue;
                while ( prefixFound )
                    {
                    ptr.TrimLeft();
                    TInt pos = ptr.Locate( TChar(':') );
                    if ( pos > 0 && pos <= 3 )
                        {
                        for ( TInt i = 0; i < pos; i++ )
                            {
                            TChar ch = ptr[i];
                            if ( !ch.IsAlpha() )
                                {
                                prefixFound = EFalse;
                                break;
                                }                    
                            }
                        if ( prefixFound )
                            {
                            ptr = ptr.Mid( pos + 1 );
                            } 
                        }
                    else
                        {
                        prefixFound = EFalse;
                        }
                    }
                
                nextString.Copy( ptr );
                }
                break;
                
            default:
                break;
            }
        
        TInt primaryOrder = 0;
        if ( aCriteria.iSortBy == EMsgStoreSortBySender ||
             aCriteria.iSortBy == EMsgStoreSortByRecipient ||
             aCriteria.iSortBy == EMsgStoreSortBySubject   )
            {
            primaryOrder = nextString.CompareC( curString, 1, NULL );
            }
        else
            {
            primaryOrder = ( nextVal1 < curVal1 ? -1 : ( nextVal1 == curVal1 ? 0 : 1 )  );
            }
        
        if ( compareOrder1 == EMsgStoreSortDescending  )
            {
                CheckCondition( _L("primary order"), primaryOrder <= 0, EFalse );
            }
        else
            {
                CheckCondition( _L("primary order"), primaryOrder >= 0, EFalse );
            }
        
        if ( checkSecondaryOrder && primaryOrder == 0 )
            {
            //compare the secondary order
            if ( compareOrder2 == EMsgStoreSortDescending  )
                {
                CheckCondition( _L("2nd value order"), nextVal2 <= curVal2, EFalse );
                }
            else
                {
                CheckCondition( _L("2nd value order"), nextVal2 >= curVal2, EFalse );
                }
            }
        
        curVal1 = nextVal1;
        curVal2 = nextVal2;
        curString.Copy( nextString );
        
        if ( i == aResults.Count() - 1 )
            {
            lastMsgId = msg->Id();
            }
        }
    
    CleanupStack::PopAndDestroy( &timeZoneServer );
    
    curString.Close();
    nextString.Close();
    
    iLog->Log( _L("  << CompareSortResultsL - PASSED") );
    
    return lastMsgId;
    
    }


//  End of File
