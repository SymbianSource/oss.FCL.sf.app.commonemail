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
* Description:  Decleares settings array class.
*
*/


#ifndef IPSSETUIARRAY_H
#define IPSSETUIARRAY_H


#include <barsread.h>       // TResourceReader
#include <bamdesca.h>                // MDesCArray
#include <eikenv.h>
#include "ipssetuifinderinterface.h"


/**
 * Stack states.
 */
enum TIpsSetUiStackResult
    {
    EIpsSetUiStackResultOk,
    EIpsSetUiStackResultSubMenuOpen,
    EIpsSetUiStackResultInvalid
    };

typedef RArray<TInt> RResourceStack;

class CIpsSetUiItem;
class CIpsSetUiFinder;
class CIpsSetUiItemLink;
class CIpsSetUiItemLinkExt;

/**
 * Defines settings array behavior.
 *
 * @lib IpsSosSettings.lib
 * @since FS v1.0
 */
class CIpsSetUiBaseArray :
    public CBase,
    public MDesCArray,
    public MIpsSetUiFinder
    {
public: // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CIpsSetUiBaseArray();

public: // New functions

    /**
     * Forces the redraw
     */
    void Refresh();

    /**
     * Sets the setting text inside the item.
     *
     * @param aBaseItem Item for modification.
     * @param aText Text to be stored.
     * @param Force update of the view.
     */
    void SetItemText(
        CIpsSetUiItem& aBaseItem,
        const TDesC& aText,
        const TBool aUpdate = EFalse );

    /**
     * Retrieves the setting text inside the item.
     *
     * @param aBaseItem Item which contains the text.
     */
    const TDesC& ItemText(
        CIpsSetUiItem& aBaseItem );

    /**
     * Goes forward in resoruce stack.
     *
     * @param aBase Item which has been opened.
     * @return Status of the operation.
     */
    TIpsSetUiStackResult HandleStackForwardL( const CIpsSetUiItem& aBase );

    /**
     * Goes backward in resource stack.
     *
     * @param aCount Number of submenus to be moved.
     * @return Status of the operation.
     */
    TIpsSetUiStackResult HandleStackBackwardL( TInt aCount = 1 );

    /**
     * Event invoked by stack change.
     *
     * @param aForward Submenu item has opened.
     * @param aResourceId Resource ID of the item that caused the action.
     * @return Status of the operation.
     */
    TIpsSetUiStackResult HandleStackChangeL(
        const TInt aForward,
        const TInt aResourceId = KErrNotFound );

    /**
     * Checks if item is hidden
     *
     * @param aIndex index of the item
     * @return KErrNone, if item is visible
     * @return KErrNotFound, if item is hidden
     * @return KErrNotSupported, if item is permanently hidden
     */
    TInt IsHidden( const TInt aIndex ) const;

    /**
     * Checks if item is hidden
     *
     * @param aId id of the item
     * @return KErrNone, if item is visible
     * @return KErrNotFound, if item is hidden
     * @return KErrNotSupported, if item is permanently hidden
     */
    TInt IsHidden( const TUid& iId ) const;

    /**
     * Checks if item is hidden
     *
     * @param aItem the item
     * @return KErrNone, if item is visible
     * @return KErrNotFound, if item is hidden
     * @return KErrNotSupported, if item is permanently hidden
     */
    virtual TInt IsHidden( const CIpsSetUiItem& aItem ) const;

    /**
     * Hides the provided item.
     *
     * @param aHide Hide/Unhide.
     * @param aId Id of the item to be modified.
     * @param aUpdate Refresh the UI.
     */
    void SetHideItem(
        const TBool aHide,
        const TUid& aId,
        const TBool aUpdate = EFalse );

    /**
     * Hides the provided item.
     *
     * @param aHide Hide/Unhide.
     * @param aItem Item to be modified.
     * @param aUpdate Refresh the UI.
     */
    void SetHideItem(
        const TBool aHide,
        CIpsSetUiItem& aItem,
        const TBool aUpdate = EFalse );

    /**
     * Find the item from the tree structure.
     *
     * @param aId Unique ID of the item.
     * @param aExcludeHidden Skip all hidden item.
     */
    CIpsSetUiItem* GetItem(
        const TUid& aId,
        const TBool aExcludeHidden = ETrue ) const;

    /**
     * Find the item from the tree structure.
     *
     * @param Index of the item in resource.
     * @param aExcludeHidden Skip all hidden item.
     */
    CIpsSetUiItem* GetItem(
        const TInt aIndex,
        const TBool aExcludeHidden = ETrue ) const;

protected:  // Constructors

    /**
     *  Constructor.
     */
    CIpsSetUiBaseArray();

    /**
     * 2nd phase of construction.
     */
    void BaseConstructL();

    /**
     * 2nd phase of construction.
     *
     * @param aResourceId Tree root resource.
     */
    void BaseConstructL( const TInt aResourceId );

protected:  // Data

    /**
     * Events invoked by array changes.
     */
    enum TIpsSetUiArrayEvent
        {
        EIpsSetUiArrayAdded = 0,
        EIpsSetUiArrayRemoved,
        EIpsSetUiArrayChanged,
        EIpsSetUiArrayStackForward,
        EIpsSetUiArrayStackBackward
        };

// New virtual functions

    /**
     * Initializes the data in the item.
     *
     * @param aBaseItem Item to be initialized.
     */
    virtual void InitUserData( CIpsSetUiItem& aBaseItem ) = 0;

    /**
     * Handler for array change events.
     *
     * @param aEvent The invoked event.
     * @return KErrNone, when ok.
     */
    virtual TInt EventArrayChangedL(
        const TIpsSetUiArrayEvent aEvent ) = 0;

    /**
     * Custom drawing operation for items.
     *
     * @param aId Unique Id of the item to be drawn.
     * @param aString String to be constructed.
     * @return ETrue, when handled.
     */
    virtual TBool EventCustomMdcaPoint(
        const TUid& aId,
        TPtr& aString ) const = 0;

    /**
     * Creates array of items.
     *
     * @param aId Unique Id of the item to be drawn.
     * @return Created item with client ownership.
     */
    virtual CIpsSetUiItem* CreateCustomItemToArrayLC(
        const TUid& aId );

    /**
     * Fills the label text to item.
     *
     * @param aBaseItem Item which contains the label.
     * @param aText Return parameter for the label.
     */
    virtual void ListboxItemFillMultiLineLabel(
        const CIpsSetUiItem& aBaseItem,
        TIpsSetUtilsTextPlain& aText ) const;

    /**
     * Fills the label text to item.
     *
     * @param aBaseItem Item which contains the label.
     * @param aText Return parameter for the label.
     */
    virtual void ListboxItemFillEditTextLabel(
        const CIpsSetUiItem& aBaseItem,
        TIpsSetUtilsTextPlain& aText ) const;

    /**
     * Fills the label text to item.
     *
     * @param aBaseItem Item which contains the label.
     * @param aText Return parameter for the label.
     */
    virtual void ListboxItemFillEditValueLabel(
        const CIpsSetUiItem& aBaseItem,
        TIpsSetUtilsTextPlain& aText ) const;

    /**
     * Fills the label text to item.
     *
     * @param aBaseItem Item which contains the label.
     * @param aText Return parameter for the label.
     */
    virtual void ListboxItemFillEditTimeLabel(
        const CIpsSetUiItem& aBaseItem,
        TIpsSetUtilsTextPlain& aText ) const;

    /**
     * Fills the label text to item.
     *
     * @param aBaseItem Item which contains the label.
     * @param aText Return parameter for the label.
     */
    virtual void ListboxUndefinedFillLabel(
        const CIpsSetUiItem& aBaseItem,
        TIpsSetUtilsTextPlain& aText ) const;

    /**
     * Fills the label text to item.
     *
     * @param aBaseItem Item which contains the label.
     * @param aText Return parameter for the label.
     */
    virtual void ListboxRadioButtonFillArrayLabel(
        const CIpsSetUiItem& aBaseItem,
        TIpsSetUtilsTextPlain& aText ) const;

    /**
     * Fills the label text to item.
     *
     * @param aBaseItem Item which contains the label.
     * @param aText Return parameter for the label.
     */
    virtual void ListboxCheckboxFillArrayLabel(
        const CIpsSetUiItem& aBaseItem,
        TIpsSetUtilsTextPlain& aText ) const;

// New functions

    /**
     * Locates the item behind the provided item.
     *
     * @param aBaseItem Item which contains the array of items.
     * @param aId Unique Id to be searched.
     * @param aExcludeHidden Skip hidden items.
     * @return Located item.
     */
    CIpsSetUiItem* GetSubItem(
        const CIpsSetUiItem& aBaseItem,
        const TUid& aId,
        const TBool aExcludeHidden = ETrue ) const;

    /**
     * Locates the item behind the provided item.
     *
     * @param aBaseItem Item which contains the array of items.
     * @param aIndex Index of the item in array.
     * @param aExcludeHidden Skip hidden items.
     * @return Located item.
     */
    CIpsSetUiItem* GetSubItem(
        const CIpsSetUiItem& aBaseItem,
        const TInt aIndex,
        const TBool aExcludeHidden = ETrue ) const;

    /**
     * Locates item of specific subtype from array.
     *
     * @param aBaseItem Item which contains the array of items.
     * @param aType Type of the item to be searched.
     * @return Located item.
     */
    CIpsSetUiItem* GetSubItemByType(
        const CIpsSetUiItem& aBaseItem,
        const TInt aType ) const;

    /**
     * Gets the index of the item in the array.
     *
     * @param aLinkItem Item which contains the array.
     * @param aId Unique Id to be searched.
     * @param aExcludeHidden Skip hidden items.
     * @return Index of the item.
     */
    TInt GetSubItemIndex(
        const CIpsSetUiItemLink& aLinkItem,
        const TUid& aId,
        const TBool aExcludeHidden = ETrue ) const;

    /**
     * Gets the index of the item in the array.
     *
     * @param aLinkItem Item which contains the array.
     * @param aIndex Index of the item in array.
     * @param aActualIndex
     *    ETrue  : aIndex is from visible list and index
     *             from the actual resource is required
     *    EFalse : aIndex is from resource and visible
     *             index is required
     */
    TInt GetSubItemIndex(
        const CIpsSetUiItemLink& aLinkItem,
        const TInt aIndex,
        const TBool aActualIndex ) const;

    /**
     * Gets all item, which belongs to certain resource.
     *
     * @param aResourceId The certain resource.
     * @param aExcludeHidden Skip hidden items.
     * @return Array of items in certain resource with ownership.
     */
    CIpsSetUiFinderArray* GetResourceL(
        const TInt aResourceId,
        const TBool aExcludeHidden = ETrue ) const;

    /**
     * Gets all item, which belongs to certain resource.
     *
     * @param aResourceId The certain resource.
     * @param aExcludeHidden Skip hidden items.
     * @return Array of items in certain resource with ownership.
     */
    CIpsSetUiFinderArray* GetResourceLC(
        const TInt aResourceId,
        const TInt aExcludeHidden = ETrue ) const;

    /**
     * Index of the item in array.
     *
     * @param aId Unique Id to be searched.
     * @param aExcludeHidden Skip hidden items.
     * @return Index of the item.
     */
    TInt ItemIndex(
        const TUid& aId,
        const TBool aExcludeHidden = ETrue ) const;

    /**
     * Check whether the provided value is between the limits.
     *
     * @param aValue Value to be modified between the limits.
     * @param aMin Minimun value.
     * @param aMax Maximum value.
     */
    void SetBetweenValues(
        TInt& aValue,
        TInt aMin,
        TInt aMax ) const;

    /**
     * Checks if the item contains array of items.
     *
     * @param aBaseItem Item to be check.
     * @return ETrue, when item has submenu.
     */
    TBool ItemSubArrayCheck(
        const CIpsSetUiItem& aBaseItem ) const;

    /**
     * Checks if the item contains array of items.
     *
     * @param aBaseItem Item to be check.
     * @return ETrue, when item has submenu.
     */
    TBool ItemSubItemCheck(
        const CIpsSetUiItem& aBaseItem ) const;

    /**
     * @return Currently active resource.
     */
    TInt StackActiveResource() const;

    /**
     * Check if the item is visible or not.
     *
     * @param aBaseItem Item to be inspected.
     * @param aExcludeHidden To make the check useless.
     * @return ETrue, when item is visible.
     */
    TBool IsVisibleCheck(
        const CIpsSetUiItem& aBaseItem,
        const TBool aExcludeHidden ) const;

// Functions from base classes

    /**
     * @return Number of items in array
     */
    TInt MdcaCount() const;

// From MIpsSetUiFinder

    /**
     * Handle error in search.
     *
     * @return ETrue, Search can continue.
     */
    virtual TBool SearchDoError();

    /**
     * Check if the search criterias have been filled.
     *
     * @param aItem Item to be checked.
     * @param aIndex Index of the item.
     * @return ETrue, when search should continue.
     */
    virtual TBool SearchDoContinuationCheck(
        const CIpsSetUiItem& aItem,
        const TInt aIndex );

    /**
     * Checks if the item is the one we are searching for.
     *
     * @param aItem to be checked.
     * @return ETrue, Our precious.
     */
    virtual TBool SearchDoItemCheck(
        CIpsSetUiItem& aItem );
    
    TInt GetLastResourceId( );

private:  // New functions

    /**
     * Clear array of items.
     */
    void CleanArrayItems();

    /**
     * Load array of items from the specific resource.
     *
     * @param aResourceId specific resource.
     */
    void LoadSettingArrayFromResourceL( const TInt aResourceId );

    /**
     * Load array of items from the specific resource.
     *
     * @param aResourceId Specific resource.
     * @param aItemArray Array to be filled.
     * @return Result of the operation.
     */
    TInt LoadSettingItemsToArrayL(
        const TInt aResourceId,
        CIpsSetUiBaseItemArray& aItemArray );

    /**
     * Count items in the array.
     *
     * @param aNewCount Already counted number.
     */
    void UpdateActiveItemsCount( const TInt aNewCount = KErrNotFound );

    /**
     * Fill in the data in the item.
     *
     * @param aBaseItem Item to be filled.
     * @param aMaxLength Maximum length of user data.
     * @param aSubType Subtype of the item.
     * @param aLinkResourceId Possible id of subarray of items.
     */
    void HandleDynamicSettingsItemL(
        CIpsSetUiItem& aBaseItem,
        const TInt aMaxLength,
        const TIpsSetUiSettingsType aSubType,
        const TInt aLinkResourceId );

    /**
     * Loads the item from resource.
     *
     * @param aReader The resource reader.
     * @param aBaseItem Item to be filled.
     */
    void LoadDynamicSettingItemL(
        TResourceReader& aReader,
        CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item which contains data.
     * @return Label of the item.
     */
    TPtrC16 ListboxItemLabelFillL(
        CIpsSetUiItem& aBaseItem ) const;

    /**
     * Creates item to array.
     *
     * @param aType Type of the item.
     * @param aId Unique id of the item.
     * @param aItemArray Array the item will be added.
     * @return Created item.
     */
    CIpsSetUiItem* CreateItemToArrayL(
        const TIpsSetUiSettingsType aType,
        const TUid& aId,
        CIpsSetUiBaseItemArray& aItemArray );

    /**
     * Functions to load special item settings
     *
     * @param aBaseItem, Item to be used as base to settings
     * @param aItemArray, Array that item should be stored
     */
    void LoadDynamicSettingsItemLinkItemL(
        CIpsSetUiItem& aBaseItem,
        const TIpsSetUiSettingsType aSubType,
        const TInt aLinkResourceId );

    /**
     * Functions to load special item settings
     *
     * @param aBaseItem, Item to be used as base to settings
     * @param aItemArray, Array that item should be stored
     */
    void LoadDynamicSettingsItemRadioButtonArrayL(
        CIpsSetUiItem& aBaseItem,
        CIpsSetUiBaseArray& aItemArray );

    /**
     * Functions to load special item settings
     *
     * @param aBaseItem, Item to be used as base to settings
     * @param aItemArray, Array that item should be stored
     */
    void LoadDynamicSettingsItemCheckBoxArrayL(
        CIpsSetUiItem& aBaseItem,
        CIpsSetUiBaseArray& aItemArray );

    /**
     * Fills in text in the item.
     *
     * @param aBaseItem Item to be modified.
     * @param aMaxLength Maximum length of the text.
     */
    void UpdateItemEditTextL(
        CIpsSetUiItem& aBaseItem,
        const TInt aMaxLength );

    /**
     * Fills in value in the item.
     *
     * @param aBaseItem Item to be modified.
     * @param aMaxLength Maximum length of the value.
     */
    void UpdateItemEditValueL(
        CIpsSetUiItem& aBaseItem,
        const TInt aMaxLength );

    /**
     * @param aFlags Flags to be loaded.
     */
    void LoadDynamicSettingItemFlags(
        const TUint32 aFlags );

    /**
     * Search for the specific resource.
     *
     * @param aResourceId Teh specific resource.
     * @param aExcludeHidden Skip hidden item.
     */
    void SearchResourceL(
        const TInt aResourceId,
        const TBool aExcludeHidden ) const;

    /**
     * Hide items.
     *
     * @param aHide Hide/Unhide.
     */
    void DoHideItems( const TBool aHide );

    /**
     * Prepare search operation.
     *
     * @param aExcludeHidden Skip hidden item.
     */
    void InitDefaultSearch(
        const TBool aExcludeHidden = ETrue ) const;

    /**
     * count item in stack.
     *
     * @param aExcludeHidden Skip hidden item.
     * @return Number of items.
     */
    TInt StackCountItems(
        const TBool aExcludeHidden = ETrue ) const;

        
    /**
     * Forces the redraw
     */
    void RefreshL();

private:  // Functions from base classes

// from MDesCArray

    /**
     * Create listbox item string.
     *
     * @param aIndex Index of the item in array.
     * @return Item string.
     */
    TPtrC16 MdcaPoint( TInt aIndex ) const;

protected:  // Data

    /**
     * Flags to control the array behaviour
     */
    enum TIpsSetUiArrayFlags
        {
        /** Allows updating of the array items. */
        EIpsSetUiArrayAllowRefresh = 0x01
        };

    /**
     * Text array for temporary usage, should be deleted after the use
     */
    HBufC*                  iTempText;

    /**
     * Flags for array handling
     */
    TIpsSetUtilsFlags       iArrayFlags;

private:    // Data

    /**
     * Item finder object
     * Owned.
     */
    CIpsSetUiFinder* iFinder;

    /**
     * Stack to track the active resources
     * Owned.
     */
    RResourceStack*         iResourceStack;

    /**
     * Array of items to be shown
     */
    CIpsSetUiBaseItemArray* iItemArray;

    /**
     * String to fill empty compulsary fields
     * Owned.
     */
    HBufC*                  iCompulsoryText;

    /**
     * Char to replace character in password fields
     */
    TUint16                 iPasswordFill;

    /**
     * Count of active items in array
     */
    TInt                    iActiveCount;

    /**
     * Error state
     */
    TInt                    iError;
    
    /**
    * Not owned
    */
    CEikonEnv*              iEnv;
    
    };

#endif      // IPSSETUIARRAY_H

// End of File
