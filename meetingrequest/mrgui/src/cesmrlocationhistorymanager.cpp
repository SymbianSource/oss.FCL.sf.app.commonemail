/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Location history manager class implementation
*
*/

#include "emailtrace.h"
#include "cesmrlocationhistorymanager.h"

#include <centralrepository.h>
#include <s32mem.h>

#include "mruiprivatecrkeys.h"
#include "cesmrlocationhistoryitemfactory.h"
#include "cesmrlocationhistoryitem.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::NewL
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryManager* CESMRLocationHistoryManager::NewL()
    {
    FUNC_LOG;
    CESMRLocationHistoryManager* object = CESMRLocationHistoryManager::NewLC();
    CleanupStack::Pop ( object );
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::NewLC
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryManager* CESMRLocationHistoryManager::NewLC()
    {
    FUNC_LOG;
    CESMRLocationHistoryManager* object = new (ELeave) CESMRLocationHistoryManager();
    CleanupStack::PushL( object );
    object->ConstructL();
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::~CESMRLocationHistoryManager
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryManager::~CESMRLocationHistoryManager( )
    {
    FUNC_LOG;
    delete iFactory;
    if ( iNotifyHandler )
        {
        iNotifyHandler->StopListening();
        }
    delete iNotifyHandler;    
    delete iCRSession;     
    
    iOrder.Reset();
    iHistoryList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::UpdateLocationHistoryL
// -----------------------------------------------------------------------------
//
void CESMRLocationHistoryManager::UpdateLocationHistoryL( const MESMRLocationHistoryItem* aItem )
    {
    FUNC_LOG;
    // Update order list
    TBool itemWriteNeeded( ETrue );
    TInt index(-1);
    for( TInt i=0; i<iHistoryList.Count(); i++ )
        {
        // check if the history item already exists or there is one with identical address
        if(( aItem->Id() == iHistoryList[i]->Id() ) ||
           ( aItem->Address().Compare( iHistoryList[i]->Address() ) == 0 ))
            {
            index = i;
            for( TInt j=0; j<iOrder.Count(); j++  )
                {
                if( i == iOrder[j] )
                    {
                    itemWriteNeeded = EFalse;
                    iOrder.Remove( j );
                    iOrder.Insert( index, 0 );
                    // If the received item is new, delete it as it's not used 
                    if( aItem->Id() != iHistoryList[i]->Id() )
                        {
                        delete aItem;
                        }
                    break;
                    }
                }
            }
        if( !itemWriteNeeded )
            {
            break;
            }
        }
    
    iNotifyHandler->StopListening();
    if( index == -1 )
        {
        // new history item
        if( iOrder.Count() == iMaxCount )
            {
            // max amount of history already stored, replace the oldest item
            index = iOrder[iOrder.Count() - 1];            
            MESMRLocationHistoryItem* item = iHistoryList[index];
            iHistoryList.Remove( index );
            delete item;
            iHistoryList.Insert( aItem, index );
            iOrder.Remove( iOrder.Count() - 1 );
            iOrder.Insert( index, 0 );            
            }
        else
            {
            // add new history item
            index = iOrder.Count();
            iOrder.Insert( index, 0 );  
            iHistoryList.Append( aItem );
            // update current item count cenrep key
            User::LeaveIfError( iCRSession->Set( KESMRUILocationHistoryItemCount, iOrder.Count() ));
            }
        }
    
    // update order cenrep key
    HBufC8* orderBuf = HBufC8::NewLC( NCentralRepositoryConstants::KMaxBinaryLength );
    TPtr8 orderPtr( orderBuf->Des() );
    RDesWriteStream orderStream( orderPtr );
    orderStream.PushL();  
    for( TInt i=0; i<iOrder.Count(); i++ )
        {
        orderStream.WriteUint16L( iOrder[i] );
        }
    orderStream.CommitL();
    CleanupStack::PopAndDestroy(); // codescanner::cleanup
    User::LeaveIfError( iCRSession->Set( KESMRUILocationHistoryItemOrder, orderPtr ));
    CleanupStack::PopAndDestroy( orderBuf );
      
    // updated changed location cenrep key
    if( itemWriteNeeded )
        {        
        HBufC8* buf = HBufC8::NewLC( NCentralRepositoryConstants::KMaxBinaryLength );
        TPtr8 bufPtr = buf->Des();
        RDesWriteStream stream( bufPtr );
        stream.PushL();

        stream.WriteUint16L( aItem->Address().Length() );
        if( aItem->Address().Length() != 0 )
            {
            stream.WriteL( aItem->Address() );        
            }
        stream.WriteUint16L( aItem->Url().Length() );
        if( aItem->Url().Length() != 0 )
            {
            stream.WriteL( aItem->Url() );        
            }
    
        stream.CommitL();
        CleanupStack::PopAndDestroy(); // codescanner::cleanup
    
        TUint32 key = KESMRUILocationHistoryItemFirstInt;
    
        key = key + (( index + 1 ) * KESMRUILocationHistoryItemIndexMask );
        TInt err = iCRSession->Set( key, bufPtr );
        if( err == KErrNotFound )
            {
            err = iCRSession->Create( key, bufPtr );
            }
        User::LeaveIfError( err );
        CleanupStack::PopAndDestroy( buf );
        }

    iNotifyHandler->StartListeningL();
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::LocationHistoryItemL
// -----------------------------------------------------------------------------
//
const MESMRLocationHistoryItem&
CESMRLocationHistoryManager::LocationHistoryItemL( TInt aIndex )
    {
    FUNC_LOG;
    if(( aIndex < 0 ) || ( aIndex >= iOrder.Count() ))
        {
        User::Leave( KErrNotFound );
        }
    
    if( iOrder.Count() != iHistoryList.Count() )
        {
        ReadHistoryDataL();        
        }
    
    return *iHistoryList[iOrder[aIndex]]; 
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::ItemCount
// -----------------------------------------------------------------------------
//
TUint CESMRLocationHistoryManager::ItemCount()
    {
    FUNC_LOG;
    return iOrder.Count();
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::LocationHistoryItemFactory
// -----------------------------------------------------------------------------
//
MESMRLocationHistoryItem* CESMRLocationHistoryManager::CreateLocationHistoryItemL( 
            const TDesC& aAddress, 
            const TDesC& aUrl )
    {
    FUNC_LOG;
    return iFactory->CreateLocationHistoryItemL( aAddress, aUrl );
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::CESMRLocationHistoryManager
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryManager::CESMRLocationHistoryManager() :
    iMaxCount( 0 )
    {
    FUNC_LOG;
    }

void CESMRLocationHistoryManager::HandleNotifyGeneric( TUint32 /*aId*/ )
    {
    FUNC_LOG;
    /* One or more of the keys in KCRUidESMRUIPreviousLocations has changed.
     * Update all needed keys data by reading them. In reality this case is very
     * rare, so performance is not seen as a problem here.
     */
    TRAP_IGNORE( ReadOrderDataL() );
    TRAP_IGNORE( ReadHistoryDataL() );
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRLocationHistoryManager::ConstructL()
    {
    FUNC_LOG;
    iFactory = CESMRLocationHistoryItemFactory::NewL();
    iCRSession  = CRepository::NewL( KCRUidESMRUIPreviousLocations );  
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iCRSession );
    iNotifyHandler->StartListeningL();
    
    User::LeaveIfError( iCRSession->Get( KESMRUILocationHistoryItemMaxCount, iMaxCount ));
    
    TInt currentCount = 0;
    User::LeaveIfError( iCRSession->Get( KESMRUILocationHistoryItemCount, currentCount ));
    
    if( currentCount > 0 )
        {
        ReadOrderDataL();
        }
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::ReadOrderDataL
// -----------------------------------------------------------------------------
//
void CESMRLocationHistoryManager::ReadOrderDataL()
    {
    FUNC_LOG;
    TInt currentCount = 0;
    User::LeaveIfError( iCRSession->Get( KESMRUILocationHistoryItemCount, currentCount ));
    
    iOrder.Reset();
    iOrder.ReserveL( currentCount );
    HBufC8* orderBuf = HBufC8::NewLC( NCentralRepositoryConstants::KMaxBinaryLength );
    TPtr8 orderPtr( orderBuf->Des() );

    User::LeaveIfError( iCRSession->Get( KESMRUILocationHistoryItemOrder, orderPtr ));
    RDesReadStream orderStream( orderPtr );
    orderStream.PushL();  
    
    TUint pos( 0 );
    for( TInt i=0; i<currentCount; i++ )
        {
        User::LeaveIfError( pos = orderStream.ReadUint16L() );
        iOrder.Append( pos );
        }
    CleanupStack::PopAndDestroy(); // codescanner::cleanup
    CleanupStack::PopAndDestroy( orderBuf );
    }
    
// -----------------------------------------------------------------------------
// CESMRLocationHistoryManager::ReadHistoryDataL
// -----------------------------------------------------------------------------
//
void CESMRLocationHistoryManager::ReadHistoryDataL()
    {
    FUNC_LOG;
    TInt currentCount = iOrder.Count();
    
    iHistoryList.ResetAndDestroy();
    iHistoryList.ReserveL( iMaxCount );
    TUint32 key = KESMRUILocationHistoryItemFirstInt;
    for( TInt i=0; i<currentCount; i++ )
        {
        HBufC8* buf = HBufC8::NewLC( NCentralRepositoryConstants::KMaxBinaryLength );
        TPtr8 ptr( buf->Des() );
        key += KESMRUILocationHistoryItemIndexMask;
        User::LeaveIfError( iCRSession->Get( key, ptr ));
        if( buf != 0 )
            {
            RDesReadStream stream( ptr );
            stream.PushL();

            TInt addressLength = stream.ReadUint16L();
            HBufC* address = HBufC::NewLC( addressLength );
            TPtr addressPtr = address->Des();
            if( addressLength != 0 )
                {
                stream.ReadL( addressPtr, addressLength );                
                }

            TInt urlLength = stream.ReadUint16L();
            HBufC* url = HBufC::NewLC( urlLength );
            TPtr urlPtr = url->Des();
            if( urlLength != 0 )
                {
                stream.ReadL( urlPtr, urlLength );
                }
            
            MESMRLocationHistoryItem* item = 
                iFactory->CreateLocationHistoryItemL( addressPtr, urlPtr );
            iHistoryList.Append( item );
            
            CleanupStack::PopAndDestroy( url );
            CleanupStack::PopAndDestroy( address );
            CleanupStack::PopAndDestroy(); // codescanner::cleanup
            }
        CleanupStack::PopAndDestroy( buf );
        }
    }

// EOF

