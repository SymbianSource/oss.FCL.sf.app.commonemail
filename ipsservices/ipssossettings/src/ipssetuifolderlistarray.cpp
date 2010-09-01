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
* Description: This file implements class CIpsSetUiFolderListArray.
*
*/



#include "emailtrace.h"
#include <miuthdr.h>
#include <msvapi.h>
#include <msvuids.h>

#include "ipssetuifoldersubscription.h"
#include "ipssetutilsconsts.h"

#include "ipssetuifolderlistarray.h"

// how the indentation is done 
_LIT( KIndentString, ".." ); 
const TInt KIndentStringLength = 2;  // KIndentString().Length();
#define KMaxIndentLevel 3

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::NewL
// ----------------------------------------------------------------------------
//
CIpsSetUiFolderListArray* CIpsSetUiFolderListArray::NewL(
    CMsvSession& aMsvSession,
    TMsvId aMailboxId )
    {
    FUNC_LOG;
    CIpsSetUiFolderListArray* self=new (ELeave) CIpsSetUiFolderListArray(
        aMsvSession, aMailboxId );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::~CIpsSetUiFolderListArray()
// ----------------------------------------------------------------------------
//
CIpsSetUiFolderListArray::~CIpsSetUiFolderListArray()
    {
    FUNC_LOG;
    iArrayBuf.ResetAndDestroy();
    delete iText;
    delete iEntry;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ConstructL
// ----------------------------------------------------------------------------
//
void CIpsSetUiFolderListArray::ConstructL()
    {
    FUNC_LOG;
    RefreshFolderListArrayL();

    iText = HBufC::NewL( KIpsSetUiMaxSettingsTextLength );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::CIpsSetUiFolderListArray
// ----------------------------------------------------------------------------
//
CIpsSetUiFolderListArray::CIpsSetUiFolderListArray(
    CMsvSession& aMsvSession,
    TMsvId aMailboxId)
    :
    iSession( aMsvSession ),
    iMailboxId( aMailboxId )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::MdcaPoint
// list item contains: icon_index separator item_text
// ----------------------------------------------------------------------------
//
TPtrC CIpsSetUiFolderListArray::MdcaPoint( TInt aIndex ) const
    {
    FUNC_LOG;
    //    const TMsvEmailEntry emailEntry( (*iEntry)[aIndex] );
    
// firstly switch to correct directory to search for folders
    TMsvId origFolder = iEntry->EntryId();
    TArrayFolderItem * const pArrayItem = iArrayBuf[aIndex]; // not owning
    TMsvId targetFolder = pArrayItem->Parent();
    // if item has subitem it follows immediately
    TBool itemHasChildFolders( ( aIndex < (iCount - 1) ) 
          && (iArrayBuf[aIndex+1]->Parent() == pArrayItem->Id() ) );
// return value init
    TPtr tempText = iText->Des();
    tempText.Zero();

    TInt result( KErrNone );
    if ( origFolder != targetFolder ) 
        {
        TRAP( result, iEntry->SetEntryL( targetFolder ) );
        }
    TMsvEmailEntry emailEntry;
    if ( KErrNone == result )
        {
        TRAP( result, emailEntry = iEntry->ChildDataL( pArrayItem->Id() ) );
        }

    if ( KErrNone != result )    
        {    // Handle error.
            // item index dir or item
            tempText.AppendNum( ( itemHasChildFolders 
                ? EIpsSetUiFolderSubfoldersUnsubbed 
                : EIpsSetUiFolderSingleUnsubbed ) );
            tempText.Append( KColumnListSeparator );
            // but name can't be find
            return tempText;
        }
//    const TMsvEmailEntry emailEntry( curentEntry );

    TInt iconIndex = 0;
    if ( itemHasChildFolders )
        {
        iconIndex = ( emailEntry.LocalSubscription() ?
            EIpsSetUiFolderSubfoldersSubbed : EIpsSetUiFolderSubfoldersUnsubbed );
        }
    else
        {
        iconIndex = ( emailEntry.LocalSubscription() ?
            EIpsSetUiFolderSingleSubbed : EIpsSetUiFolderSingleUnsubbed );
        }
    
    tempText.AppendNum( iconIndex );
    tempText.Append( KColumnListSeparator );

    // add indentation text of subfolders 2,4 or 6 dots 
    TInt i = ( ( pArrayItem->IndentLevel() >= KMaxIndentLevel ) 
            ? KMaxIndentLevel : pArrayItem->IndentLevel() );
    TInt siz ( KIpsSetUiMaxSettingsTextLength -  i * KIndentStringLength ); 
    for ( ; i > 0; i-- ) 
        {
        tempText.Append(KIndentString);
        }

    // add item text
    tempText.Append( emailEntry.iDetails.Left( siz ) );
    
    return tempText;
    
    }


// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::MdcaCount
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiFolderListArray::MdcaCount() const
    {
    FUNC_LOG;
    return iCount;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::Id
// ----------------------------------------------------------------------------
//
TMsvId CIpsSetUiFolderListArray::Id( TInt aIndex )
    {
    FUNC_LOG;
//    return (*iEntry)[aIndex].Id();
    return iArrayBuf[aIndex]->Id();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ChangEIpsSetUiFolderL
// ----------------------------------------------------------------------------
//
void CIpsSetUiFolderListArray::ChangEIpsSetUiFolderL( TMsvId aFolderId )
    {
    FUNC_LOG;
    iEntry->SetEntryL( aFolderId );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ContextHasChildren
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::ContextHasChildren( TMsvId aId ) const
    {
    FUNC_LOG;
    return ContextHasChildFolders( aId );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::ContextHasChildFolders
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::ContextHasChildFolders( TMsvId aId ) const
    {
    FUNC_LOG;
    TBool hasFolders = EFalse;
    TRAPD( error, hasFolders = DoContextHasChildFoldersL( aId ) );
    if ( error != KErrNone )
        {
        return EFalse;
        }
    return hasFolders;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::DoContextHasChildFoldersL
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::DoContextHasChildFoldersL( TMsvId aId ) const
    {
    FUNC_LOG;
    TMsvSelectionOrdering selectionOrdering(
        KMsvGroupByType, EMsvSortByDetails, ETrue );
    CMsvEntry* entry = CMsvEntry::NewL(iSession, aId, selectionOrdering);
    CleanupStack::PushL( entry );
    TBool retval ( ( entry->Count() > 0 ) && ( KUidMsvFolderEntry == (*entry)[0].iType ) ); 
    CleanupStack::PopAndDestroy( entry );
    return retval;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::GetContextChildrenL
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CIpsSetUiFolderListArray::GetContextChildrenL(
    TMsvId aId,
    CMsvEntry& aEntry ) const
    {
    FUNC_LOG;
    TMsvSelectionOrdering selectionOrdering(
        KMsvGroupByType, EMsvSortByDetails, ETrue );
    aEntry.SetEntryL( aId );
    aEntry.SetSortTypeL( selectionOrdering );
    CMsvEntrySelection* sel = aEntry.ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( sel );

    TInt i;
    TInt cnt = sel->Count();

    for( i=0; i<cnt; i++ )
        {
        if( ContextHasChildren( sel->At(i) ) )
            {
            //Note! recursion
            CMsvEntrySelection* selection =
                GetContextChildrenL( sel->At(i), aEntry );
            CleanupStack::PushL( selection );
            TInt c = selection->Count();

            TInt j;
            for( j=0; j<c; j++ )
                {
                sel->AppendL( selection->At(j) );
                }
            CleanupStack::PopAndDestroy( selection );
            }
        }

    CleanupStack::Pop( sel );
    return sel;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::FoldersUpdated
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFolderListArray::FoldersUpdated() const
    {
    FUNC_LOG;
    return ( iEntry->Count() > 0 ? ETrue : EFalse );
    }


// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::RefreshFolderListArrayL
// original function 
// ----------------------------------------------------------------------------

void CIpsSetUiFolderListArray::RefreshFolderListArrayL()
    {
    FUNC_LOG;
    if( iEntry )
        {
        delete iEntry;
        iEntry = NULL;
        }
    iCount = 0;
    
    iEntry = iSession.GetEntryL( iMailboxId );
    const TMsvSelectionOrdering originalOrdering=iEntry->SortType();
    TMsvSelectionOrdering newOrdering=originalOrdering;
    newOrdering.SetShowInvisibleEntries(ETrue);
    newOrdering.SetGroupByType(ETrue);
    newOrdering.SetSorting( EMsvSortByDetails );
    iEntry->SetSortTypeL(newOrdering);

    // code to fill in internal array
    TInt arrayIndex(0);
    TInt size = iArrayBuf.Count();
    iArrayBuf.ResetAndDestroy();
    if ( size > 8 )
        {
        iArrayBuf.ReserveL(size);
        } 
    RefreshFolderListArrayL(iMailboxId, arrayIndex, 0);
    iArrayBuf.Compress();
    iCount = iArrayBuf.Count();
    
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFolderListArray::RefreshFolderListArrayL(
//      TMsvId aIdFolder, TInt & aArrayIndex, TInt aIndentLevel)
// recursion function to fill in / refresh iArrayBuf.
// ----------------------------------------------------------------------------
void CIpsSetUiFolderListArray::RefreshFolderListArrayL(
                       TMsvId aIdFolder, TInt & aArrayIndex, TInt aIndentLevel)
    {
    TMsvId origFolder = iEntry->EntryId();
    if ( origFolder != aIdFolder ) 
        iEntry->SetEntryL( aIdFolder );
    CMsvEntrySelection* sel = iEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( sel );
    TInt count = sel->Count();
    for (TInt i = 0; i < count; i++)
        {
        const TMsvEmailEntry emailEntry( (*iEntry)[i] );
        TArrayFolderItem * item = new (ELeave) TArrayFolderItem( emailEntry.Id(), aIdFolder, aIndentLevel );
        CleanupStack::PushL( item );
        if ( iArrayBuf.Count() > aArrayIndex + 1 )
            {
            if ( iArrayBuf[aArrayIndex] ) 
                {
                delete iArrayBuf[aArrayIndex];
                }
            iArrayBuf[aArrayIndex] = item;
            }
        else
            {
            iArrayBuf.AppendL( item );
            }
//        iArrayBuf.Insert( item, aArrayIndex );
        aArrayIndex++;
        CleanupStack::Pop( item );
        // recursion is here
        RefreshFolderListArrayL(emailEntry.Id(), aArrayIndex, aIndentLevel + 1 );
        }
    CleanupStack::PopAndDestroy( sel );
    iEntry->SetEntryL( origFolder );
    }

// End of File

