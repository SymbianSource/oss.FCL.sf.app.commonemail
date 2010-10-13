/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  FreestyleEmailUi double line list model and model item implementation.
*
*/


// SYSTEM INCLUDES
//<cmail>
#include "emailtrace.h"
#include "cfsmailmessage.h"
//</cmail>


// INTERNAL INCLUDES
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiMailListModel.h"
#include "FreestyleEmailUiUtilities.h"


// MODEL ITEM CONSTRUCTION
CFSEmailUiMailListModelItem* CFSEmailUiMailListModelItem::NewL( CFSMailMessage* aMessagePtr,
																	TModelItemType aModelItemtype )
    {
    FUNC_LOG;
    CFSEmailUiMailListModelItem* self = CFSEmailUiMailListModelItem::NewLC( aMessagePtr, aModelItemtype );
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiMailListModelItem* CFSEmailUiMailListModelItem::NewLC( CFSMailMessage* aMessagePtr,
																	TModelItemType aModelItemtype )
	{
    FUNC_LOG;
    CFSEmailUiMailListModelItem* self = new (ELeave) CFSEmailUiMailListModelItem( aMessagePtr, aModelItemtype );
    CleanupStack::PushL(self);
    return self;
	}

CFSEmailUiMailListModelItem::CFSEmailUiMailListModelItem( CFSMailMessage* aMessagePtr,
															  TModelItemType aModelItemtype ) 
   	:iMessagePtr( aMessagePtr ),
   	iModelItemType( aModelItemtype ),
   	iSeparatorText( 0 ),
   	iFsTreeListId( 0 )
    {
    FUNC_LOG;
    }

CFSEmailUiMailListModelItem::~CFSEmailUiMailListModelItem() 
    {
    FUNC_LOG;
   	delete iSeparatorText;
   	if ( iModelItemType == ETypeMailItem )
   	    {
        delete iMessagePtr;
   	    }
   	else
   	    {
   	    // In case of separator item, the message is not owned. This is just a pointer
   	    // to the first message belonging under this separator. The ownership is on the
   	    // next mail item.
   	    iMessagePtr = NULL;
   	    }
    }


void CFSEmailUiMailListModelItem::SetSeparatorTextL( const TDesC& aSeparatorText )
    {
    FUNC_LOG;
    delete iSeparatorText;
    iSeparatorText = NULL;
    iSeparatorText = aSeparatorText.AllocL();
    }
	



//MODEL CLASS CONSTRUCTION
CFSEmailUiMailListModel* CFSEmailUiMailListModel::NewL( CFreestyleEmailUiAppUi* aAppUi, TBool aSearchList )
    {
    FUNC_LOG;
    CFSEmailUiMailListModel* self = CFSEmailUiMailListModel::NewLC( aAppUi, aSearchList );
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiMailListModel* CFSEmailUiMailListModel::NewLC( CFreestyleEmailUiAppUi* aAppUi, TBool aSearchList )
    {
    FUNC_LOG;
    CFSEmailUiMailListModel* self = new (ELeave) CFSEmailUiMailListModel( aAppUi );
    CleanupStack::PushL(self);
    self->ConstructL( aSearchList );
    return self;
    }

void CFSEmailUiMailListModel::ConstructL( TBool aSearchList )
    {
    FUNC_LOG;
    // set default sort criteria
    iSortCriteria.iField = EFSMailSortByDate;
    iSortCriteria.iOrder = EFSMailDescending;
    
    if ( !aSearchList )
        {
        // Set TLS pointer to point ourselves. This is needed to access our
        // data from static (find/sort) functions.
        TInt err = UserSvr::DllSetTls( KTlsHandleMailListModel, this );
        }
    }
 
CFSEmailUiMailListModel::CFSEmailUiMailListModel( CFreestyleEmailUiAppUi* aAppUi ) 
	: iAppUi( aAppUi )
    {
    FUNC_LOG;
    }
	
CFSEmailUiMailListModel::~CFSEmailUiMailListModel()
    {
    FUNC_LOG;
	iItems.ResetAndDestroy();
    }

void CFSEmailUiMailListModel::AppendL(MFSListModelItem* aItem)
    {
    FUNC_LOG;
	TInt err;
	CFSEmailUiMailListModelItem* newItem = static_cast< CFSEmailUiMailListModelItem* >(aItem);
	err = iItems.Append( newItem );	
	User::LeaveIfError(err);
	
	// If mail item was added as first child of a separator, the message pointer
	// of the separator has to be reset.
	TInt insertionPoint = iItems.Count()-1; 
    if ( insertionPoint > 0 &&
         newItem->ModelItemType() == ETypeMailItem &&
         iItems[insertionPoint-1]->ModelItemType() == ETypeSeparator )
        {
        iItems[insertionPoint-1]->SetMessagePtr( &newItem->MessagePtr() );
        }
    }
	
void CFSEmailUiMailListModel::InsertL( MFSListModelItem* aItem, TInt aIndex )
    {
    FUNC_LOG;
	TInt err;	
    CFSEmailUiMailListModelItem* newItem = static_cast< CFSEmailUiMailListModelItem* >(aItem);
	err = iItems.Insert( newItem, aIndex);	
	User::LeaveIfError(err);

    // If mail item was added as first child of a separator, the message pointer
    // of the separator has to be reset.
    if ( aIndex > 0 &&
         newItem->ModelItemType() == ETypeMailItem &&
         iItems[aIndex-1]->ModelItemType() == ETypeSeparator )
        {
        iItems[aIndex-1]->SetMessagePtr( &newItem->MessagePtr() );
        }
    }

void CFSEmailUiMailListModel::RemoveAndDestroy(TInt aIndex)
    {
    FUNC_LOG;
	delete Item(aIndex);
	iItems.Remove(aIndex);
	iItems.Compress();
	
	// If the removed item was the first child of a preceeding node item, then the message
	// pointer in the node item has to be updated.
	if ( aIndex > 0 && iItems[aIndex-1]->ModelItemType() == ETypeSeparator )
	    {
	    if ( aIndex < iItems.Count() && iItems[aIndex]->ModelItemType() == ETypeMailItem )
	        {
	        iItems[aIndex-1]->SetMessagePtr( &iItems[aIndex]->MessagePtr() );
	        }
	    else
	        {
	        iItems[aIndex-1]->SetMessagePtr( NULL );
	        }
	    }
    }

MFSListModelItem* CFSEmailUiMailListModel::Item( TInt aIndex )
	{
    FUNC_LOG;
	TInt numInList = iItems.Count();
	if ( aIndex < 0 || aIndex >= numInList )
	   {
	   return NULL;
       }	
	return iItems[aIndex];
	}

const MFSListModelItem* CFSEmailUiMailListModel::Item( TInt aIndex ) const
    {
    FUNC_LOG;
    TInt numInList = iItems.Count();
    if ( aIndex < 0 || aIndex >= numInList )
       {
       return NULL;
       }
    return iItems[aIndex];
    }

TInt CFSEmailUiMailListModel::Count() const
    {
    FUNC_LOG;
	return iItems.Count();
    }

CFreestyleEmailUiAppUi* CFSEmailUiMailListModel::AppUi()
	{
    FUNC_LOG;
	return iAppUi;
	}

void CFSEmailUiMailListModel::SetSortCriteria( TFSMailSortCriteria aSortCriteria )
    {
    FUNC_LOG;
    iSortCriteria = aSortCriteria;
    }

TFSMailSortCriteria CFSEmailUiMailListModel::SortCriteria()
    {
    FUNC_LOG;
    return iSortCriteria;
    }

void CFSEmailUiMailListModel::SortL()
    {
    FUNC_LOG;
    TLinearOrder<CFSEmailUiMailListModelItem> orderFunc( CompareModelItemsL );
    iItems.Sort( orderFunc );
    }

void CFSEmailUiMailListModel::ReplaceMessagePtr( TInt aIndex, CFSMailMessage* aNewPtr )
    {
    FUNC_LOG;
    CFSEmailUiMailListModelItem* item = 
        static_cast< CFSEmailUiMailListModelItem* >( Item(aIndex) );
    
    if ( item ) // Safety
        {
        // Mail type items own the message. Delete the existing message first.
        if ( item->ModelItemType() == ETypeMailItem )
            {
            delete &item->MessagePtr();
            }
        item->SetMessagePtr( aNewPtr );
        
        // If the item was the first child of a node, then also message pointer in the parent
        // node needs to be updated
        if ( aIndex > 0 &&
             item->ModelItemType() == ETypeMailItem &&
             iItems[aIndex-1]->ModelItemType() == ETypeSeparator )
            {
            iItems[aIndex-1]->SetMessagePtr( aNewPtr );
            }
        }
    }

TInt CFSEmailUiMailListModel::GetInsertionPointL( const CFSEmailUiMailListModelItem& aItemToInsert ) const
    {
    FUNC_LOG;
    TInt index;
    TLinearOrder<CFSEmailUiMailListModelItem> orderFunc( CompareModelItemsL );
    TInt ret = iItems.SpecificFindInOrder( &aItemToInsert, index, orderFunc, EArrayFindMode_First );
    return index;
    }

TInt CFSEmailUiMailListModel::GetInsertionPointL( const CFSEmailUiMailListModelItem& aItemToInsert,
                                                  TInt& aChildIdx,
                                                  TInt& aParentNodeIdx ) const
    {
    FUNC_LOG;
    TInt index = GetInsertionPointL( aItemToInsert );
    
    // Find out the parent node
    aChildIdx = KErrNotFound;
    aParentNodeIdx = KErrNotFound;
    if ( index > 0 )
        {
        CFSEmailUiMailListModelItem* preceedingItem = iItems[index-1];
        if ( MessagesBelongUnderSameSeparatorL( aItemToInsert.MessagePtr(), 
                                                preceedingItem->MessagePtr(),
                                                iSortCriteria.iField ) )
            {
            // The new item belongs under the same node as the previous item
            // (or the previous item is a node under which the item belongs).
            // Find out the previous node item.
            aParentNodeIdx = index-1;
            while ( aParentNodeIdx >= 0 && iItems[aParentNodeIdx]->ModelItemType() != ETypeSeparator )
                {
                aParentNodeIdx--;
                }
            if ( aParentNodeIdx >= 0 )
                {
                aChildIdx = index - aParentNodeIdx - 1;
                }
            }
        }
    
    return index;
    }

TInt CFSEmailUiMailListModel::CompareModelItemsL( const CFSEmailUiMailListModelItem& aItem1,
                                                  const CFSEmailUiMailListModelItem& aItem2 )
    {
    FUNC_LOG;
    // Retrieve the current sort criteria from model instance in TLS
    CFSEmailUiMailListModel* self = 
        static_cast<CFSEmailUiMailListModel*>( UserSvr::DllTls(KTlsHandleMailListModel) );
    TFSMailSortCriteria sortCriteria = self->SortCriteria();
    
    TInt value = CompareMessagesL( aItem1.MessagePtr(), 
                                   aItem2.MessagePtr(), 
                                   sortCriteria.iField );
    
    // The logic of the CompareMessagesL matches ascending order. In descending mode it must
    // be inverted.
    if ( sortCriteria.iOrder == EFSMailDescending )
        {
        value = -value;
        }
    
    // If items are equal with the primary sorting criterion and this primary criterion
    // is not date sort, then descending date order is used as secondary criterion.
    if ( !value && sortCriteria.iField != EFSMailSortByDate )
        {
        value = -CompareMessagesL( aItem1.MessagePtr(),
                                   aItem2.MessagePtr(),
                                   EFSMailSortByDate );
        }
    
    // Some magic is needed in case one of the items (but not both) is a separator
    if ( aItem1.ModelItemType() == ETypeSeparator && aItem2.ModelItemType() == ETypeMailItem )
        {
        if ( !value )
            { // In case an item is equal to the first item under the node, it's greater than the node
            value = -1;
            }
        else if ( value > 0 )
            {
            // In case an item is smaller than the first item under the node, it may still be greater 
            // than the node (i.e. when the item belongs under this same node).
            if ( MessagesBelongUnderSameSeparatorL( aItem1.MessagePtr(), 
                                                    aItem2.MessagePtr(), 
                                                    sortCriteria.iField ) )
                {
                value = -1;
                }
            }
        }
    else if ( aItem1.ModelItemType() == ETypeMailItem && aItem2.ModelItemType() == ETypeSeparator )
        {
        if ( !value )
            { // In case an item is equal to the first item under the node, it's greater than the node
            value = 1;
            }
        else if ( value < 0 )
            {
            // In case an item is smaller than the first item under the node, it may still be greater 
            // than the node (i.e. when the item belongs under this same node).
            if ( MessagesBelongUnderSameSeparatorL( aItem1.MessagePtr(), 
                                                    aItem2.MessagePtr(), 
                                                    sortCriteria.iField ) )
                {
                value = 1;
                }
            }
        }
    
    return value;
    }

// ---------------------------------------------------------------------------
// CompareMessagesL
// Compares given messages using the given sorting mode. Return positive 
// value if aMessage1 is greater, negative value if aMessage1 is smaller, and 
// 0 if the messages are equal.
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListModel::CompareMessagesL( const CFSMailMessage& aMessage1, 
                                                const CFSMailMessage& aMessage2,
                                                TFSMailSortField aSortField )
    {
    FUNC_LOG;
    // For now, cast away the constness from the argument because CFSMailMessage does 
    // not declare all its logically constant functions as const. This is dirty and
    // should be fixed in the CFSMailMessage interface.
    CFSMailMessage& message1 = const_cast<CFSMailMessage&>( aMessage1 );
    CFSMailMessage& message2 = const_cast<CFSMailMessage&>( aMessage2 );
    
    TInt retVal = 0;
    
    switch ( aSortField )
        {
        case EFSMailSortByFlagStatus:
            {
            TInt compVal1 = 0;
            if ( message1.IsFlagSet(EFSMsgFlag_FollowUpComplete) ) { compVal1 = 1; }
            else if ( message1.IsFlagSet(EFSMsgFlag_FollowUp) ) { compVal1 = 2; }

            TInt compVal2 = 0;
            if ( message2.IsFlagSet(EFSMsgFlag_FollowUpComplete) ) { compVal2 = 1; }
            else if ( message2.IsFlagSet(EFSMsgFlag_FollowUp) ) { compVal2 = 2; }

            retVal = compVal1 - compVal2;
            }
            break;
        case EFSMailSortByPriority:
            {
            TInt compVal1 = 1;
            if ( message1.IsFlagSet(EFSMsgFlag_Low) ) { compVal1 = 0; }
            else if ( message1.IsFlagSet(EFSMsgFlag_Important) ) { compVal1 = 2; }

            TInt compVal2 = 1;
            if ( message2.IsFlagSet(EFSMsgFlag_Low) ) { compVal2 = 0; }
            else if ( message2.IsFlagSet(EFSMsgFlag_Important) ) { compVal2 = 2; }

            retVal = compVal1 - compVal2;
            }
            break;
        case EFSMailSortByRecipient:
            {
            CFSMailAddress* toAddress1(NULL);
            CFSMailAddress* toAddress2(NULL); 

            RPointerArray<CFSMailAddress>& toArray1 = message1.GetToRecipients();
            RPointerArray<CFSMailAddress>& toArray2 = message2.GetToRecipients();
            if ( toArray1.Count() )
                {
                toAddress1 = toArray1[0];
                }           
            if ( toArray2.Count() )
                {
                toAddress2 = toArray2[0];
                }           
            retVal = TFsEmailUiUtility::CompareMailAddressesL( toAddress1, toAddress2 );
            }
            break;
        case EFSMailSortBySender:
            {
            CFSMailAddress* fromAddress1 = message1.GetSender();
            CFSMailAddress* fromAddress2 = message2.GetSender();
            retVal = TFsEmailUiUtility::CompareMailAddressesL( fromAddress1, fromAddress2 );
            }
            break;
        case EFSMailSortByAttachment:
            {
            TInt compVal1 = 0;
            if ( message1.IsFlagSet(EFSMsgFlag_Attachments) ) { compVal1 = 1; }

            TInt compVal2 = 0;
            if ( message2.IsFlagSet(EFSMsgFlag_Attachments) ) { compVal2 = 1; }

            retVal = compVal1 - compVal2;
            }
            break;
        case EFSMailSortByUnread:
            {
            TInt compVal1 = 0;
            if ( message1.IsFlagSet(EFSMsgFlag_Read) ) { compVal1 = 1; }

            TInt compVal2 = 0;
            if ( message2.IsFlagSet(EFSMsgFlag_Read) ) { compVal2 = 1; }

            retVal = compVal1 - compVal2;
            }
            break;
        case EFSMailSortBySubject:
            {
            retVal = TFsEmailUiUtility::CompareMailSubjectsL( &message1, &message2 );
            }
            break;  
        case EFSMailSortByDate:
            {
            TTime time1 = message1.GetDate();
            TTime time2 = message2.GetDate();
            TTimeIntervalSeconds diff;
            TInt err = time1.SecondsFrom( time2, diff );
            if ( !err )
                {
                // Normal case, no overflow
                retVal = diff.Int();
                }
            else
                {
                // Overflow happens if the difference in time stamps is more than roughly 68 years.
                // This is a very unlikely special case but handle it properly just in case.
                retVal = ( time1 > time2 ? 1 : -1 );
                }
            }
            break;
        default:
            {
            retVal = 0;
            }
            break;
        }
    
    return retVal;
    }

TBool CFSEmailUiMailListModel::MessagesBelongUnderSameSeparatorL( 
                                                           const CFSMailMessage& aMessage1, 
                                                           const CFSMailMessage& aMessage2,
                                                           TFSMailSortField aSortField )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    
    switch ( aSortField )
        {
        case EFSMailSortByFlagStatus:
        case EFSMailSortByPriority:
        case EFSMailSortByRecipient:
        case EFSMailSortBySender:
        case EFSMailSortByAttachment:
        case EFSMailSortByUnread:
        case EFSMailSortBySubject:
            {
            retVal = ( !CompareMessagesL(aMessage1, aMessage2, aSortField) );
            }
            break;
        case EFSMailSortByDate:
            {
            TTime time1InHomeTime = aMessage1.GetDate();
            TTime time2InHomeTime = aMessage2.GetDate();
            TLocale currentLocaleSettings;      
            time1InHomeTime += currentLocaleSettings.UniversalTimeOffset();                      
            time1InHomeTime += currentLocaleSettings.QueryHomeHasDaylightSavingOn() 
                                  ? TTimeIntervalHours(1) : TTimeIntervalHours(0);          
            time2InHomeTime += currentLocaleSettings.UniversalTimeOffset();                      
            time2InHomeTime += currentLocaleSettings.QueryHomeHasDaylightSavingOn() 
                                  ? TTimeIntervalHours(1) : TTimeIntervalHours(0);      
            // Check that the day of the year and the year number are the same
            TInt day1 = time1InHomeTime.DayNoInYear();
            TInt day2 = time2InHomeTime.DayNoInYear();
            TTimeIntervalYears year1 = time1InHomeTime.YearsFrom(0);
            TTimeIntervalYears year2 = time2InHomeTime.YearsFrom(0);
            retVal = ( day1 == day2 && year1 == year2 );
            }
            break;
        default:
            {
            retVal = EFalse;
            }
            break;
        }
    
    return retVal;
    }

/**
 * CFSEmailUiMailListModel::Reset
 */
void CFSEmailUiMailListModel::Reset()
    {
    FUNC_LOG;
	iItems.ResetAndDestroy();
    iSortCriteria.iField = EFSMailSortByDate;
    iSortCriteria.iOrder = EFSMailDescending;
    }

void CFSEmailUiMailListModel::GetItemIdsUnderNodeL( const TFsTreeItemId aNodeId, 
        RFsTreeItemIdList& aMessageIds ) const
    {
    FUNC_LOG;
    aMessageIds.Reset();
    const TInt count(iItems.Count());
    for (TInt index = 0; index < count; index++)
        {
        if (iItems[index]->CorrespondingListId() == aNodeId)
            {            
            // Found the node Id, now get all the items under it
            while (++index < count && iItems[index]->ModelItemType() == ETypeMailItem)
                {
                aMessageIds.AppendL(iItems[index]->CorrespondingListId());
                }
            break;
            }
        }        
    }
TBool CFSEmailUiMailListModel::ContainsItem( const CFSMailMessage& aMessage ) const
    {
	FUNC_LOG;
	
	TBool found( EFalse );
	for (TInt i = iItems.Count() - 1; i >= 0; i--) 
		{
		if (aMessage.GetMessageId() == iItems[i]->MessagePtr().GetMessageId())
			{
			found = ETrue;
			break;
			}
		}
	return found;
	}

