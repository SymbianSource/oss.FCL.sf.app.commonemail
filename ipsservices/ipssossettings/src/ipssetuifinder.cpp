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
* Description:  Implements class for tree structure searching.
*
*/



#include "emailtrace.h"
#include <e32base.h>

#include "ipssetutilsconsts.h"
#include "ipssetuiitemlink.h"

#include "ipssetuifinder.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::CIpsSetUiFinder()
// ----------------------------------------------------------------------------
CIpsSetUiFinder::CIpsSetUiFinder(
    MIpsSetUiFinder& aObserver )
    :
    iObserver( aObserver ),
    iSubArrays( NULL ),
    iTempItem( NULL ),
    iSearchItem( 0 ),
    iCurrentItem( 0 ),
    iFinderArray( NULL )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::ConstructL()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::~CIpsSetUiFinder()
// ----------------------------------------------------------------------------
//
CIpsSetUiFinder::~CIpsSetUiFinder()
    {
    FUNC_LOG;
    // Shouldn't exist but remove anyway
    if ( iSubArrays )
        {
        iSubArrays->Reset();
        }

    delete iSubArrays;
    iSubArrays = NULL;

    iTempItem = NULL;

    // Clean and remove the array
    if ( iFinderArray )
        {
        iFinderArray->Reset();
        }

    delete iFinderArray;
    iFinderArray = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiFinder* CIpsSetUiFinder::NewL(
    MIpsSetUiFinder& aObserver )
    {
    FUNC_LOG;
    CIpsSetUiFinder* self = NewLC( aObserver );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiFinder* CIpsSetUiFinder::NewLC(
    MIpsSetUiFinder& aObserver )
    {
    FUNC_LOG;
    CIpsSetUiFinder* self =
        new ( ELeave ) CIpsSetUiFinder( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SetSearchFlags()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::SetSearchFlags(
    const TUint64& aSearchFlags )
    {
    FUNC_LOG;
    iSearchFlags = aSearchFlags;
    }


// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SearchFlags()
// ----------------------------------------------------------------------------
//
TUint64 CIpsSetUiFinder::SearchFlags() const
    {
    FUNC_LOG;
    return iSearchFlags;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::StartSearchL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::StartSearchL(
    CIpsSetUiBaseItemArray& aItemArray,
    const TUid& aId )
    {
    FUNC_LOG;
    InitializeSearchL( KErrUnknown );
    SearchIdFromTreeL( aItemArray, aId );
    FinalizeSearchL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::StartSearchL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::StartSearchL(
    CIpsSetUiBaseItemArray& aItemArray,
    const TInt aNth )
    {
    FUNC_LOG;
    InitializeSearchL( aNth );
    SearchIndexFromTreeL( aItemArray );
    FinalizeSearchL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::InitializeSearchL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::InitializeSearchL(
    const TInt aItem )
    {
    FUNC_LOG;
    iSearchItem = aItem;
    iTempItem = NULL;

    // Clear flags
    iSearchFlags &= ~EFinderItemFound;
    iSearchFlags &= ~EFinderItemFindError;
    iSearchFlags &= ~EFinderExit;

    iSubArrays =
        new ( ELeave ) CIpsSetUiBaseItemArray( KIpsSetUiArrayGranularity );

    // Create arrays
    if ( !iFinderArray )
        {
        iFinderArray =
            new ( ELeave ) CIpsSetUiFinderArray( KIpsSetUiArrayGranularity );
        }
    // Clean and remove the array
    else if ( !( iSearchFlags & EFinderKeepLastResult ) )
        {
        iFinderArray->Reset();
        }
    else
        {
        // make lint to keep quiet
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::FinalizeSearchL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::FinalizeSearchL()
    {
    FUNC_LOG;
    if ( iSubArrays )
        {
        iSubArrays->Reset();
        }

    delete iSubArrays;
    iSubArrays = NULL;

    iTempItem = NULL;

    // Clear flags
    iSearchFlags &= ~( EFinderItemFound | EFinderItemFindError | EFinderExit );

    iSearchItem = 0;
    iCurrentItem = 0;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::ContinueSearch()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFinder::ContinueSearch(
    const TInt aCurrentItem,
    const TInt aMaxItems )
    {
    FUNC_LOG;
    // First check if the search should be cancelled
    if ( iSearchFlags & EFinderExit )
        {
        return EFalse;
        }

    // Check if the item is found, and that it's enough
    if ( iSearchFlags & EFinderItemFound &&
         !iSearchFlags & EFinderSearchAll &&
         !iSearchFlags & EFinderResourceSearch )
        {
        return EFalse;
        }

    // Set the current item
    iCurrentItem = aCurrentItem;

    // Check if the last item is in progress
    if ( iCurrentItem >= aMaxItems )
        {
        if ( !iSearchFlags & EFinderItemFound )
            {
            iSearchFlags |= EFinderItemFindError;
            }

        iSearchFlags |= EFinderExit;
        return EFalse;
        }

    // Do custom checks if needed
    if ( iSearchFlags & EFinderCustomCheck &&
         !iObserver.SearchDoContinuationCheck( *iTempItem, iCurrentItem ) )
        {
        iSearchFlags |= EFinderExit;
        return EFalse;
        }

    // New round is about to start, clear the item found flag
    iSearchFlags &= ~EFinderItemFound;

    // All checks done, continue searching
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SearchingItem()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFinder::SearchingItem()
    {
    FUNC_LOG;
    TBool result = EFalse;

    // If resource search, compare against resource id of the
    // current item
    if ( iSearchFlags & EFinderResourceSearch )
        {
        result = ( iTempItem->iItemResourceId == iSearchItem );
        }
    // When going through all items in the list, set found
    // item as true
    else if ( iSearchFlags & EFinderSearchAll )
        {
        result = ETrue;
        }
    // Index search is on, compare against the item index
    else
        {
        result = !iSearchItem--;
        }

    // Turn the flag according the search mode
    result ^= iSearchFlags & EFinderExlusiveSearch;

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::ItemToArrayL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::ItemToArrayL()
    {
    FUNC_LOG;
    TIpsSetFinderItem item;

    item.iIndex = iCurrentItem;
    item.iItem = iTempItem;
    iFinderArray->AppendL( item );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::ItemCheck()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::ItemCheckL( const TUid& aId )
    {
    FUNC_LOG;
    // Make custom check for item, EXIT search
    if ( iSearchFlags & EFinderCustomCheck &&
         !iObserver.SearchDoItemCheck( *iTempItem ) )
        {
        return;
        }

    // If user has chosen to ingore rest of the checks, CONTINUE the search
    if ( iSearchFlags & EFinderOverrideChecks )
        {
        // Remove flags
        iSearchFlags &= ~EFinderItemFound;
        iSearchFlags &= ~EFinderExit;

        return;
        }

    // Id check
    if ( iTempItem->iItemId == aId )
        {
        // Id's of both items matches, so quit the search
        iSearchFlags |= EFinderItemFound;
        ItemToArrayL();
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::ItemCheckL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::ItemCheckL()
    {
    FUNC_LOG;
    // Make custom check for item, EXIT search
    if ( iSearchFlags & EFinderCustomCheck &&
         !iObserver.SearchDoItemCheck( *iTempItem ) )
        {
        // It's decided that item has matched, add to array
        iSearchFlags |= EFinderItemFound;
        ItemToArrayL();
        return;
        }

    // If user has chosen to ingore rest of the checks, CONTINUE the search
    if ( iSearchFlags & EFinderOverrideChecks )
        {
        // Remove flags
        iSearchFlags &= ~EFinderItemFound;
        iSearchFlags &= ~EFinderExit;
        iSearchFlags &= ~EFinderOverrideChecks;

        return;
        }

    // Make hidden item check
    // If EFinderDoHideCheck is turned on, hidden items are not allowed
    // to be included into the search. When off, hidden items, except
    // permanently hidden items, are included into search
    // The item is found, when iSearchItem reaches zero.
    if ( !IsHidden() && SearchingItem() )
        {
        // Id's of both items matches, so quit the search
        iSearchFlags |= EFinderItemFound;
        ItemToArrayL();
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::ItemSubArrayCheck()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFinder::ItemSubArrayCheck()
    {
    FUNC_LOG;
    // Check if item quit is issued. The search shouldn't be cancelled even
    // if the item is found
    if ( iSearchFlags & EFinderExit )
        {
        return EFalse;
        }

    // Check if the subfolders need to be searched
    if ( !iSearchFlags & EFinderSearchSubFolders )
        {
        return EFalse;
        }

    // Check if the item is subarray item
    if ( iTempItem->iItemType != EIpsSetUiMenuArray &&
         iTempItem->iItemType != EIpsSetUiRadioButtonArray &&
         iTempItem->iItemType != EIpsSetUiCheckBoxArray &&
         iTempItem->iItemType != EIpsSetUiItemMultiLine )
        {
        return EFalse;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::ContinueSubSearch()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiFinder::ContinueSubSearch()
    {
    FUNC_LOG;
    // If search is finished, don't continue the search
    if ( iSearchFlags & EFinderExit )
        {
        return EFalse;
        }

    // If items left in subarray list
    return iSubArrays->Count() > 0;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiFinder::IsHidden()
    {
    FUNC_LOG;
    // Permanently hidden items are shown only, if the ownership of the
    // array is moved to client. As then the client will get full access
    // to items in the array
    TInt hidden = iObserver.IsHidden( *iTempItem );
    if ( hidden == KErrNotSupported && 
        !( ( iSearchFlags & EFinderMoveOwnership ) == EFinderMoveOwnership ) && 
        ( ( iSearchFlags & EFinderDoHideCheck ) == EFinderDoHideCheck ) )
        {
        return KErrNotSupported;
        }
    // Hidden items are available only, if flag EFinderDoHideCheck
    // is turned off.
    else if ( ( iSearchFlags & EFinderDoHideCheck ) && hidden == KErrNotFound )
        {
        return KErrNotFound;
        }
    // The item is included into the search
    else
        {
        return KErrNone;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::DoIndexSearchL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::DoIndexSearchL(
    CIpsSetUiBaseItemArray& aItemArray )
    {
    FUNC_LOG;
    // Search for the item in the current array before moving inside
    // the subarrays
    TInt currentItem = 0;
    TInt max = aItemArray.Count();
    for ( ; ContinueSearch( currentItem, max ); currentItem++ )
        {
        // Store the item in array to member
        iTempItem = aItemArray[currentItem];

        // Check if the item matches and set quit flag if the id of the item
        // matches with the one that is searched
        ItemCheckL();

        // If the item wasn't the searched one, check if it contains subarray
        // and append it to stack, for later searching
        if ( ItemSubArrayCheck() )
            {
            iSubArrays->AppendL( iTempItem );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::DoIdSearchL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::DoIdSearchL(
    CIpsSetUiBaseItemArray& aItemArray,
    const TUid& aId )
    {
    FUNC_LOG;
    // Search for the item in the current array before moving inside
    // the subarrays
    TInt currentItem = 0;
    TInt max = aItemArray.Count();
    for ( ; ContinueSearch( currentItem, max ); currentItem++ )
        {
        // Store the item in array to member
        iTempItem = aItemArray[iCurrentItem];

        // Check if the item matches and set quit flag if the id of the item
        // matches with the one that is searched
        ItemCheckL( aId );

        // If the item wasn't the searched one, check if it contains subarray
        // and append it to stack, for later searching
        if ( ItemSubArrayCheck() )
            {
            iSubArrays->AppendL( iTempItem );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SearchSubArraysL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::DoIdSearchSubArraysL(
    const TUid& aId )
    {
    FUNC_LOG;
    // If the correct item couldn't be found from the array,
    // search from the subarrays
    while ( ContinueSubSearch() )
        {
        // Get the first item in the array and remove it from there
        CIpsSetUiItemLink* item =
            static_cast<CIpsSetUiItemLink*>( iSubArrays->At( 0 ) );
        iSubArrays->Delete( 0 );

        // Search through the new tree
        if ( item->HasLinkArray() )
            {            
            SearchIdFromTreeL( *item->iItemLinkArray, aId );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SearchSubArraysL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::DoIndexSearchSubArraysL()
    {
    FUNC_LOG;
    // If the correct item couldn't be found from the array,
    // search from the subarrays
    while ( ContinueSubSearch() )
        {
        // Get the first item in the array and remove it from there
        CIpsSetUiItemLink* item =
            static_cast<CIpsSetUiItemLink*>( iSubArrays->At( 0 ) );
        iSubArrays->Delete( 0 );

        // Search through the new tree
        if ( item->HasLinkArray() )
            {            
            SearchIndexFromTreeL( *item->iItemLinkArray );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SearchIdFromTreeL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::SearchIdFromTreeL(
    CIpsSetUiBaseItemArray& aItemArray,
    const TUid& aId )
    {
    FUNC_LOG;
    // The search is made in following pattern:
    // First: The given array is searched through until EFinderExit flag is
    //        set or the end has been reached. The flag is set when given
    //        item(s) is (are) found. Each matching item and its index will
    //        be stored into array.
    //        Each item is checked for subarrays (during the first search).
    //        If the item has subarray, the item will be stored for
    //        searching.
    // Second: After the given array has been searched, each subarray will
    //         be searched, until conditions mentioned above have met, or
    //         each of the subarray's has been searched through.
    // The function is recursive

    // Search for the item in the current array
    DoIdSearchL( aItemArray, aId );

    // Before searching subarrays, check if subarray search is actually
    // need at all
    SearchShouldStop();

    // Search through the subarrays, if any exists
    DoIdSearchSubArraysL( aId );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SearchIndexFromTreeL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::SearchIndexFromTreeL(
    CIpsSetUiBaseItemArray& aItemArray )
    {
    FUNC_LOG;
    // Search for the index from the existing array
    DoIndexSearchL( aItemArray );

    // Before searching subarrays, check if subarray search is actually
    // need at all
    SearchShouldStop();

    // Check the subarrays for the existing index
    DoIndexSearchSubArraysL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::SearchShouldStop()
// ----------------------------------------------------------------------------
//
void CIpsSetUiFinder::SearchShouldStop()
    {
    FUNC_LOG;
    // Check if searching has been issued to be stopped
    if ( iSearchFlags & EFinderExit )
        {
        // Store flags to variables to keep things readable
        TBool found = iSearchFlags & EFinderItemFound;
        TBool seekAll =
            iSearchFlags & EFinderResourceSearch ||
            iSearchFlags & EFinderSearchAll;

        // If item has been found and only single item is searched
        // allow quitting, otherwise the search must continue
        if ( found && !seekAll )
            {
            iSearchFlags |= EFinderExit;
            }
        else
            {
            iSearchFlags &= ~EFinderExit;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::FinderArray()
// ----------------------------------------------------------------------------
//
CIpsSetUiFinderArray* CIpsSetUiFinder::FinderArray() const
    {
    FUNC_LOG;
    return iFinderArray;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiFinder::CopyFinderArrayLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiFinderArray* CIpsSetUiFinder::FinderArrayLC() const
    {
    FUNC_LOG;
    // Create new array object
    CIpsSetUiFinderArray* finderArray = 
        new ( ELeave ) CIpsSetUiFinderArray( KIpsSetUiArrayGranularity );
    CleanupStack::PushL( finderArray );

    // Duplicate the array
    TInt items = iFinderArray->Count();
    for ( TInt item = 0; item < items; item++ )
        {
        finderArray->AppendL( iFinderArray->At( item ) );
        }

    return finderArray;
    }



//  End of File

