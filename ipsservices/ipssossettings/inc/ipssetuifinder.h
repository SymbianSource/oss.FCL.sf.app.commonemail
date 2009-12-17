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
* Description:  Decleares class for tree structure searching.
*
*/


#ifndef IPSSETUIITEMFINDER_H
#define IPSSETUIITEMFINDER_H

#include <e32base.h>

#include "ipssetuiitembase.h"
#include "ipssetuifinderinterface.h"

class CIpsSetUiItem;


/**
 *  Class to find item(s) from the setting tree
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiFinder : public CBase
    {
public:

// Constructors and destructor

    /**
     * Destructor
     */
    ~CIpsSetUiFinder();

    /**
     * @param aObserver Observer for search operation.
     * @return New finder object.
     */
    static CIpsSetUiFinder* NewL(
        MIpsSetUiFinder& aObserver );

    /**
     * @param aObserver Observer for search operation.
     * @return New finder object.
     */
    static CIpsSetUiFinder* NewLC(
        MIpsSetUiFinder& aObserver );

// New functions

    /**
     * Set method to member iSearchFlags
     * @param aSearchFlags, Sets value of iSearchFlags
     */
    void SetSearchFlags( const TUint64& aSearchFlags );

    /**
     * Get function for member iSearchFlags
     * @return value of iSearchFlags
     */
    TUint64 SearchFlags() const;

    /**
     *
     * @param aItemArray Array to be searched.
     */
    void StartSearchL(
        CIpsSetUiBaseItemArray& aItemArray,
        const TUid& aId );

    /**
     *
     * @param aItemArray Array to be searched.
     *
     */
    void StartSearchL(
        CIpsSetUiBaseItemArray& aItemArray,
        const TInt aItem );

    /**
     * Get function for member iFinderArray
     * @return value of iFinderArray
     */
    CIpsSetUiFinderArray* FinderArray() const;

    /**
     * Gets the copy of finder array and saves it to stack
     * @return value of iFinderArray
     */
    CIpsSetUiFinderArray* FinderArrayLC() const;

private:

// Constructors

    /**
     * Constructor
     *
     * @param aObserver Observer for search operation.
     */
    CIpsSetUiFinder(
        MIpsSetUiFinder& aObserver );

    /**
     * 2nd phase constructor
     */
    void ConstructL();

// New functions

    /**
     * Prepare for search.
     *
     * @param aNth Start from nth item in the array.
     */
    void InitializeSearchL( const TInt aNth );

    /**
     * Clear all internal data.
     */
    void FinalizeSearchL();

    /**
     * Check, if the search criterias have been met.
     *
     * @param aCurrentItem Index of the current item
     * @param aMaxItems Maximum number of items in the array.
     * @return ETrue, when the search should be continued.
     */
    TBool ContinueSearch(
        const TInt aCurrentItem,
        const TInt aMaxItems );

    /**
     * @return ETrue, when the item fills the search criterias.
     */
    TBool SearchingItem();

    /**
     * Adds new branch to search array.
     */
    void ItemToArrayL();

    /**
     * Item check for ID search algorithm.
     *
     * @param aId Id to be searched
     */
    void ItemCheckL( const TUid& aId );

    /**
     * Item check for index search algorithm.
     */
    void ItemCheckL();

    /**
     * @return ETrue, when item doesn't have container for other items.
     */
    TBool ItemSubArrayCheck();

    /**
     * @return ETrue, when search criterias are not met.
     */
    TBool ContinueSubSearch();

    /**
     * Intelligence to search the ID from the subarrays.
     *
     * @param aId Id to be searched
     */
    void DoIdSearchSubArraysL( const TUid& aId );

    /**
     * Intelligence to search index from the sub arrays.
     */
    void DoIndexSearchSubArraysL();

    /**
     * Stops the search opertion.
     */
    void DoEndSearch();

    /**
     * Main intelligence to search ID from the tree.
     *
     * @param aItemArray Array to be searched.
     * @param aId Id to be searched
     */
    void DoIdSearchL(
        CIpsSetUiBaseItemArray& aItemArray,
        const TUid& aId );

    /**
     * Main intelligence for index search.
     * @param aItemArray Array to be searched.
     */
    void DoIndexSearchL( CIpsSetUiBaseItemArray& aItemArray );

    /**
     * Searches for the item from the array based on the id.
     *
     * @param aItemArray Array, which may contain the item.
     * @param aId Id to be searched
     */
    void SearchIdFromTreeL(
        CIpsSetUiBaseItemArray& aItemArray,
        const TUid& aId );

    /**
     * Searches for the item's index from the given array.
     *
     * @param aItemArray Array to be searched.
     */
    void SearchIndexFromTreeL(
        CIpsSetUiBaseItemArray& aItemArray );

    /**
     * @return ETrue, when item is hidden. Otherwise EFalse.
     */
    TBool IsHidden();

    /**
     * Check wheter searching has met the conditions to finish itself.
     */
    void SearchShouldStop();

public:

// Data

    /** Flags to control finder's search routine. */
    enum TIpsSetUiFinderFlags
        {
        // Following flags are for set up the search

        // When enabled, user defined check is made
        EFinderCustomCheck          = 0x001,
        // Includes subfolders in search
        EFinderSearchSubFolders     = 0x002,
        // Overrides the item spesific tests for one round
        EFinderOverrideChecks       = 0x004,
        // Hidden items will not be included into the search
        EFinderDoHideCheck          = 0x008,
        // Searches for resources instead of item
        EFinderResourceSearch       = 0x010,
        // Goes through whole array regardless of hits
        EFinderSearchAll            = 0x020,
        // Keeps the last search result in memory
        EFinderKeepLastResult       = 0x040,
        // Searches items that doesn't match the set
        EFinderExlusiveSearch       = 0x080,
        // The result tree's ownership is moved to client
        EFinderMoveOwnership        = 0x100,

        // Following flags are mainly for internal use
        EFinderItemFound            = 0x200,
        EFinderItemFindError        = 0x400,
        EFinderExit                 = 0x800
        };

private:

// Data


    /**
     * Observer for search operation.
     */
    MIpsSetUiFinder&        iObserver;

    /**
     * Flags to control the finder
     */
    TUint64                 iSearchFlags;

    /**
     * Container for subarrays.
     * Owned.
     */
    CIpsSetUiBaseItemArray* iSubArrays;

    /**
     * Temporary item storage.
     * Not owned.
     */
    CIpsSetUiItem*          iTempItem;

    /**
     * Starting point as index for search.
     */
    TInt                    iSearchItem;

    /**
     * Current item index, used for passing forward
     */
    TInt                    iCurrentItem;

    /**
     * Container for search results.
     * Owned.
     */
    CIpsSetUiFinderArray*   iFinderArray;
    };

#endif      // IPSSETUIITEMFINDER_H

// End of File
