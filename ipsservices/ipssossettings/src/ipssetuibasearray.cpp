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
* Description: This file implements class CIpsSetUiBaseArray.
*
*/


#include "emailtrace.h"
#include <e32base.h>            // Base symbian include
#include <gulutil.h>            // KColumnListSeparator
#include <eikenv.h>             // CEikonEnv
#include <eikrutil.h>           // EikResourceUtils
#include <AknUtils.h>           // AknTextUtils

#include <data_caging_path_literals.hrh>
#include <ipssossettings.rsg>

#include "ipssetutilsexception.h"
#include "ipssetutilsconsts.h"
#include "ipssetuifinder.h"
#include "ipssetuiitembase.h"   
#include "ipssetuiitemvalue.h"  
#include "ipssetuiitemtext.h"   
#include "ipssetuiitemlink.h"   
#include "ipssetuiitemextlink.h"
#include "ipssetuifinder.h"
#include "ipssetutilspageids.hrh"
#include "ipssetuibasearray.h"

// This is added at the end of the compulsory setting item string
_LIT( KIpsSetUiStrCompulsory, "\t*" );
// The number of characters that can be shown in the screen
const TInt KIpsSetUiVisibleTextLength = 30;
//
const TInt KIpsSetUiPasswordSecLength = 4;

_LIT( KCharsToReplace, "\r\n\t\x2028\x2029" );

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::CIpsSetUiBaseArray()
// ----------------------------------------------------------------------------
//
CIpsSetUiBaseArray::CIpsSetUiBaseArray()
    :
    iFinder( NULL ),
    iItemArray( NULL ),
    iPasswordFill( 0x00 ),
    iActiveCount( 0 ),
    iError( KErrNone ),
    iEnv( CEikonEnv::Static() ) // faulty CS warning
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::~CIpsSetUiBaseArray()
// ----------------------------------------------------------------------------
//
CIpsSetUiBaseArray::~CIpsSetUiBaseArray()
    {
    FUNC_LOG;
    CleanArrayItems();

    if ( iResourceStack )
        {
        iResourceStack->Reset();
        delete iResourceStack;
        iResourceStack = NULL;
        }
    delete iItemArray;
    delete iTempText;
    delete iFinder;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::BaseConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::BaseConstructL()
    {
    FUNC_LOG;
    // Prepare the arrays
    iItemArray = new ( ELeave ) CIpsSetUiBaseItemArray( KIpsSetUiArrayGranularity );
    iResourceStack = new ( ELeave ) RResourceStack( KIpsSetUiArrayGranularity );
    iFinder = CIpsSetUiFinder::NewL( *this );
    iArrayFlags.SetFlag( EIpsSetUiArrayAllowRefresh );
        
    // Prepare strings
    iTempText = HBufC::NewL( KIpsSetUiMaxSettingsTextLength );
    iPasswordFill = EikResourceUtils::ReadTInt16L( 
        R_IPS_SET_DIALOG_PASSWORD_FILL, iEnv );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::BaseConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::BaseConstructL(
    const TInt aResourceId )
    {
    FUNC_LOG;
    // Initialize the members
    BaseConstructL();

    // If resource provided add it into the setting array
    LoadSettingArrayFromResourceL( aResourceId );

    UpdateActiveItemsCount();
    }

TInt CIpsSetUiBaseArray::GetLastResourceId( )
    {
    FUNC_LOG;
    TInt ret = KErrNotFound;
    if ( iResourceStack && iResourceStack->Count() > 0 )
        {
        ret = (*iResourceStack)[ iResourceStack->Count() -1 ];
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::CleanArrayItems()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::CleanArrayItems()
    {
    FUNC_LOG;
    if ( iItemArray )
        {
        // Search through all items in the array
        for ( TInt index = iItemArray->Count() - 1 ; index >= 0; index-- )
            {
            // Only certain type of items can contain subarrays
            CIpsSetUiItem* base = ( *iItemArray )[index];
            delete base;
            base = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::CreateCustomItemToArrayLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseArray::CreateCustomItemToArrayLC(
    const TUid& /* aId */ )
    {
    FUNC_LOG;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxItemFillMultiLineLabel()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::ListboxItemFillMultiLineLabel(
    const CIpsSetUiItem& aBaseItem,
    TIpsSetUtilsTextPlain& aText ) const
    {
    FUNC_LOG;
    aText = static_cast<const CIpsSetUiItemLink*>( 
        &aBaseItem )->iItemSettingText->Left( KIpsSetUiVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxItemFillEditTextLabel()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::ListboxItemFillEditTextLabel(
    const CIpsSetUiItem& aBaseItem,
    TIpsSetUtilsTextPlain& aText ) const
    {
    FUNC_LOG;
    aText.Copy( aBaseItem.Text().Left( KIpsSetUiVisibleTextLength ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxItemFillEditValueLabel()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::ListboxItemFillEditValueLabel(
    const CIpsSetUiItem& aBaseItem,
    TIpsSetUtilsTextPlain& aText ) const
    {
    FUNC_LOG;
    const TInt KNumberSize = 10;
    
    if ( aText.MaxLength() - aText.Length() >= KNumberSize )
        {
        aText.AppendNum( aBaseItem.Value() );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxItemFillEditTimeLabel()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::ListboxItemFillEditTimeLabel(
    const CIpsSetUiItem& aBaseItem,
    TIpsSetUtilsTextPlain& aText ) const
    {
    FUNC_LOG;
    aText.Append( aBaseItem.Value() );
    aText.SetLength( KIpsSetUiVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxUndefinedFillLabel()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::ListboxUndefinedFillLabel(
    const CIpsSetUiItem& aBaseItem,
    TIpsSetUtilsTextPlain& aText ) const
    {
    FUNC_LOG;
    aText = static_cast<const CIpsSetUiItemLinkExt*>( 
        &aBaseItem )->iItemSettingText->Left( KIpsSetUiVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxRadioButtonFillArrayLabel()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::ListboxRadioButtonFillArrayLabel(
    const CIpsSetUiItem& aBaseItem,
    TIpsSetUtilsTextPlain& aText ) const
    {
    FUNC_LOG;
    aText = static_cast<const CIpsSetUiItemLinkExt*>( 
        &aBaseItem )->iItemSettingText->Left( KIpsSetUiVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxCheckboxFillArrayLabel()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::ListboxCheckboxFillArrayLabel(
    const CIpsSetUiItem& aBaseItem,
    TIpsSetUtilsTextPlain& aText ) const
    {
    FUNC_LOG;
    aText = static_cast<const CIpsSetUiItemLinkExt*>( 
        &aBaseItem )->iItemSettingText->Left( KIpsSetUiVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ListboxItemLabelFillL()
// ----------------------------------------------------------------------------
//
TPtrC16 CIpsSetUiBaseArray::ListboxItemLabelFillL(
    CIpsSetUiItem& aBaseItem ) const
    {
    FUNC_LOG;
    // Get the type of the setting item
    TIpsSetUiSettingsType type = aBaseItem.iItemType;
    TIpsSetUtilsTextPlain tempSettingText;
    tempSettingText.Zero();

    switch ( type )
        {
        case EIpsSetUiItemMultiLine:
            ListboxItemFillMultiLineLabel( aBaseItem, tempSettingText );
            break;

        case EIpsSetUiUndefined:
            ListboxUndefinedFillLabel( aBaseItem, tempSettingText );
            break;

        case EIpsSetUiRadioButtonArray:
            ListboxRadioButtonFillArrayLabel( aBaseItem, tempSettingText );
            break;

        case EIpsSetUiCheckBoxArray:
            ListboxCheckboxFillArrayLabel( aBaseItem, tempSettingText );
            break;

        case EIpsSetUiItemText:
            ListboxItemFillEditTextLabel( aBaseItem, tempSettingText );
            break;

        case EIpsSetUiItemValue:
            ListboxItemFillEditValueLabel( aBaseItem, tempSettingText );
            break;

        case EIpsSetUiItemTime:
            ListboxItemFillEditTimeLabel( aBaseItem, tempSettingText );
            break;

        default:
            // Nothing to show, send empty text
            break;
        }

    // remove line feed from signature line's visible part
    if( aBaseItem.iItemId == TUid::Uid(EIpsSetUiMailboxSignature) || 
        aBaseItem.iItemId == TUid::Uid(EIpsSetUiMailboxMailboxName) )
        {
        AknTextUtils::ReplaceCharacters( tempSettingText, KCharsToReplace, ' ' );
        }

    // If the setting is marked as password, fill special character
    // instead of readable text
    TInt length = tempSettingText.Length();
    if ( length > 0 && aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagPassword ) )
        {
        // Add special chars to all type characters in setting field
        TChar passFill = TChar( iPasswordFill );
        tempSettingText.Zero();
        tempSettingText.AppendFill( passFill, KIpsSetUiPasswordSecLength );
        }

    return tempSettingText;
    }

/******************************************************************************

    Item property editors

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SetItemText()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::SetItemText(
    CIpsSetUiItem& aBaseItem,
    const TDesC& aText,
    const TBool aUpdate )
    {
    FUNC_LOG;
    switch ( aBaseItem.iItemType )
        {
        case EIpsSetUiItemValue:
            static_cast<CIpsSetUiItemValue*>( 
                &aBaseItem )->iItemSettingText->Copy( aText );
            break;

        case EIpsSetUiRadioButtonArray:
        case EIpsSetUiCheckBoxArray:
            static_cast<CIpsSetUiItemLinkExt*>( 
                &aBaseItem )->iItemSettingText->Copy( aText );
            break;

        default:
            aBaseItem.SetText( aText );
            break;
        }

    // Issue update when needed
    if ( aUpdate )
        {
        Refresh();
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ItemText()
// ----------------------------------------------------------------------------
//
const TDesC& CIpsSetUiBaseArray::ItemText(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    switch ( aBaseItem.iItemType )
        {
        case EIpsSetUiItemValue:
            return *static_cast<CIpsSetUiItemValue*>( 
                &aBaseItem )->iItemSettingText;

        case EIpsSetUiRadioButtonArray:
        case EIpsSetUiCheckBoxArray:
            return *static_cast<CIpsSetUiItemLinkExt*>( 
                &aBaseItem )->iItemSettingText;
        }

    return aBaseItem.Text();
    }

/******************************************************************************

    Base class function definitions

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SearchDoError()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiBaseArray::SearchDoError()
    {
    FUNC_LOG;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SearchDoContinuationCheck()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiBaseArray::SearchDoContinuationCheck(
    const CIpsSetUiItem& /* aItem */,
    const TInt /* aIndex */ )
    {
    FUNC_LOG;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SearchDoItemCheck()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiBaseArray::SearchDoItemCheck(
    CIpsSetUiItem& /* aItem */ )
    {
    FUNC_LOG;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::MdcaCount()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::MdcaCount() const
    {
    FUNC_LOG;
    return iActiveCount;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::MdcaPoint()
// ----------------------------------------------------------------------------
//
TPtrC16 CIpsSetUiBaseArray::MdcaPoint( TInt aIndex ) const
    {
    FUNC_LOG;
    // First search the currently active resource to array and check
    // the given index is in the current array
    IPS_ASSERT_DEBUG( aIndex < StackCountItems(), KErrOverflow, EBaseArray );

    // Temp text ptr
    TPtr tempText = iTempText->Des();
    
    // Do a sanity check for index and return null text if out of range
    if ( !iFinder || aIndex < 0 || aIndex >= StackCountItems())
        {       
        tempText.Zero();
        tempText.Append(KNullDesC);        
        return tempText;
        }

    // Search for item from the active array
    CIpsSetUiItem* setting = iFinder->FinderArray()->At( aIndex ).iItem;  // faulty CS warning
    IPS_ASSERT_DEBUG( setting , KErrNotFound, EBaseArray );

    // Prepare strings
    // Send custom event and if not used, prepare default
    if ( !EventCustomMdcaPoint( setting->iItemId, tempText ) )
        {
        // Initialize string
        tempText.Zero();
        tempText.Append( KIpsSetUiStrSpace );
        tempText.Append( KColumnListSeparator );
        tempText.Append( *setting->iItemLabel );

        // Submenus doesn't contain other information
        if ( !ItemSubArrayCheck( *setting ) )
            {
            tempText.Append( KColumnListSeparator );
            tempText.Append( KColumnListSeparator );

            TIpsSetUtilsTextPlain tempSettingText;
            // Try to add the text
            TRAP_IGNORE( tempSettingText =
                ListboxItemLabelFillL( *setting ) );

            // Fill the setting text with string
            if ( tempSettingText.Length() )
                {
                tempText.Append( tempSettingText );
                }
            else
                {
                tempText.Append( KIpsSetUiStrSpace );
                }

            // Add the special string after compulsory fields
            if ( setting->iItemFlags.Flag32( KIpsSetUiFlagCompulsory ) )
                {
                tempText.Append( KIpsSetUiStrCompulsory );
                }

            // Do the number conversion, if allowed
            if ( setting->iItemFlags.Flag32( KIpsSetUiFlagLangSpecificNumConv ) )
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                    tempText );
                }
            }
        }

    setting = NULL;
    return tempText;
    }

/******************************************************************************

    Resource loading functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::LoadDynamicSettingItemL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::LoadDynamicSettingItemL(
    TResourceReader& aReader,
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    // Read rest of the settings first
    // STRUCT IPS_SET_ITEM : LTEXT itemlabel
    HBufC* txt = aReader.ReadHBufCL();   // message text

    if ( txt )
        {
        aBaseItem.iItemLabel->Copy( *txt );
        }

    delete txt;
    txt = NULL;

    // STRUCT IPS_SET_ITEM : LLINK itemmaxlength
    TInt maxLength = aReader.ReadInt32();

    // STRUCT IPS_SET_ITEM : LONG itemflags
    aBaseItem.iItemFlags = MAKE_TUINT64( aReader.ReadInt32(), 0 );

    // STRUCT IPS_SET_ITEM : LLINK itemarray
    TInt linkResourceId = aReader.ReadInt32();

    // STRUCT IPS_SET_ITEM : WORD itemappereance
    TIpsSetUiSettingsType subtype =
        static_cast<TIpsSetUiSettingsType>( aReader.ReadInt16() );

    // Finally add the item to list
    HandleDynamicSettingsItemL(
        aBaseItem, maxLength, subtype, linkResourceId );

    // The basic data has been set, now init the userdata section
    InitUserData( aBaseItem );        
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::UpdateItemEditTextL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::UpdateItemEditTextL(
    CIpsSetUiItem& aBaseItem,
    const TInt aMaxLength )
    {
    FUNC_LOG;
    CIpsSetUiItemsEditText* text = 
        static_cast<CIpsSetUiItemsEditText*>( &aBaseItem );
    text->iItemMaxLength = aMaxLength;
    text = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::UpdateItemEditValueL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::UpdateItemEditValueL(
    CIpsSetUiItem& aBaseItem,
    const TInt aMaxLength )
    {
    FUNC_LOG;
    CIpsSetUiItemValue* value = 
        static_cast<CIpsSetUiItemValue*>( &aBaseItem );
    value->iItemMaxLength = aMaxLength;
    value = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::HandleDynamicSettingsItemL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::HandleDynamicSettingsItemL(
    CIpsSetUiItem& aBaseItem,
    const TInt aMaxLength,
    const TIpsSetUiSettingsType aSubType,
    const TInt aLinkResourceId )
    {
    FUNC_LOG;
    switch ( aBaseItem.Type() )
        {
        // Load dynamically linked items or submenus
        case EIpsSetUiMenuArray:
        case EIpsSetUiRadioButtonArray:
        case EIpsSetUiCheckBoxArray:
        case EIpsSetUiItemMultiLine:
            LoadDynamicSettingsItemLinkItemL(
                aBaseItem, aSubType, aLinkResourceId );
            break;

        // Finish text item
        case EIpsSetUiItemText:
            UpdateItemEditTextL( aBaseItem, aMaxLength );
            break;

        // Finish number item
        case EIpsSetUiItemValue:
        case EIpsSetUiItemTime:
            UpdateItemEditValueL( aBaseItem, aMaxLength );
            break;

        // Other items are already finished
        case EIpsSetUiItemRadioButton:
        case EIpsSetUiItemCheckBox:
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::CreateItemToArrayL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseArray::CreateItemToArrayL(
    const TIpsSetUiSettingsType aType,
    const TUid& aId,
    CIpsSetUiBaseItemArray& aItemArray )
    {
    FUNC_LOG;
    CIpsSetUiItem* base = NULL;

    switch ( aType )
        {
        // Create array item
        case EIpsSetUiMenuArray:
            base = CIpsSetUiItemLink::NewLC();
            break;

        // Force manual item creation
        case EIpsSetUiUndefined:
            base = CreateCustomItemToArrayLC( aId );
            IPS_ASSERT_DEBUG( base , EUnidentifiedItem, EBaseArray );
            break;

        // Create radiobutton array item
        case EIpsSetUiRadioButtonArray:
        case EIpsSetUiCheckBoxArray:
        case EIpsSetUiItemMultiLine:
            base = CIpsSetUiItemLinkExt::NewLC();
            break;

        // Create edit text item
        case EIpsSetUiItemText:
            base = CIpsSetUiItemsEditText::NewLC();
            break;

        // Create edit number item
        case EIpsSetUiItemValue:
        case EIpsSetUiItemTime:
            base = CIpsSetUiItemValue::NewLC();
            break;

        // Create normal base item
        case EIpsSetUiItemRadioButton:
        case EIpsSetUiItemCheckBox:
        default:
            base = CIpsSetUiItem::NewLC();
            break;
        }

    // Add to array
    aItemArray.AppendL( base );
    CleanupStack::Pop( base );

    return base;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::LoadDynamicSettingsItemLinkItemL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::LoadDynamicSettingsItemLinkItemL(
    CIpsSetUiItem& aBaseItem,
    const TIpsSetUiSettingsType aSubType,
    const TInt aLinkResourceId )
    {
    FUNC_LOG;
    // Prepare the item
    CIpsSetUiItemLink* linkItem = static_cast<CIpsSetUiItemLink*>( &aBaseItem );
    linkItem->iItemAppereance = aSubType;
    linkItem->iItemFlags.SetFlag32( KIpsSetUiFlagUpdateOnChange );

    // Start reading only if resource is provided
    if ( aLinkResourceId )
        {
        linkItem->iItemLinkArray =
            new ( ELeave ) CIpsSetUiBaseItemArray(
                KIpsSetUiArrayGranularity );

        // Start loading the settings
        LoadSettingItemsToArrayL( aLinkResourceId, *linkItem->iItemLinkArray );
        }

    linkItem = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::LoadSettingArrayFromResource()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::LoadSettingArrayFromResourceL(
    const TInt aResourceId )
    {
    FUNC_LOG;
    // Start reading only if resource is provided
    if ( aResourceId )
        {
        iResourceStack->AppendL( aResourceId );
        const TInt items = iItemArray->Count();

        // Begin to read the items from the resource
        TInt count = LoadSettingItemsToArrayL( aResourceId, *iItemArray );

        // Update the item count
        UpdateActiveItemsCount( count );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::LoadSettingItemsToArrayL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::LoadSettingItemsToArrayL(
    const TInt aResourceId,
    CIpsSetUiBaseItemArray& aItemArray )
    {
    FUNC_LOG;
    // Define new read based on last item
    TResourceReader reader;
    iEnv->CreateResourceReaderLC(
        reader, aResourceId );

    // Get the count of items in resource
    TInt count = reader.ReadInt16();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        // Read the settings from the resource

        // STRUCT IPS_SET_ITEM : WORD itemtype
        TIpsSetUiSettingsType type =
            static_cast<TIpsSetUiSettingsType>( reader.ReadInt16() );

        // STRUCT IPS_SET_ITEM : WORD itemid
        TUid id = TUid::Uid( reader.ReadInt16() );

        // Create the setting item
        CIpsSetUiItem* base =
            CreateItemToArrayL( type, id, aItemArray );
        base->iItemId = id;
        base->iItemType = type;
        base->iItemResourceId = aResourceId;

        // Start loading the item, based on item type
        LoadDynamicSettingItemL( reader, *base );
        base = NULL;
        }

    CleanupStack::PopAndDestroy(); // reader

    return count;
    }

/******************************************************************************

    Item hide functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::IsHidden( const TUid& aId ) const
    {
    FUNC_LOG;
    return IsHidden( *GetItem( aId ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::IsHidden(
    const CIpsSetUiItem& aItem ) const
    {
    FUNC_LOG;
    if ( aItem.iItemFlags.Flag32( KIpsSetUiFlagPermanentlyHidden ) )
        {
        return KErrNotSupported;
        }
    else if ( aItem.iItemFlags.Flag32( KIpsSetUiFlagHidden ) )
        {
        return KErrNotFound;
        }
    else
        {
        return KErrNone;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::IsHidden( const TInt aIndex ) const
    {
    FUNC_LOG;
    return IsHidden( *GetItem( aIndex ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::DoHideItems()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::DoHideItems( const TBool aHide )
    {
    FUNC_LOG;
    // All items collected, set their flags hidden flags
    CIpsSetUiFinderArray& array = *iFinder->FinderArray();
    TInt item = array.Count();
    while ( --item >= 0 )
        {
        CIpsSetUiItem* base = array[item].iItem;
        if ( aHide )
            {
            base->iItemFlags.SetFlag32( KIpsSetUiFlagHidden );
            }
        else
            {
            base->iItemFlags.ClearFlag32( KIpsSetUiFlagHidden );
            }        
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SetHideItem()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::SetHideItem(
    const TBool aHide,
    const TUid& aId,
    const TBool aUpdate )
    {
    FUNC_LOG;
    if ( aHide )
        {
        GetItem( aId )->iItemFlags.SetFlag32( KIpsSetUiFlagHidden );
        }
    else
        {
        GetItem( aId )->iItemFlags.ClearFlag32( KIpsSetUiFlagHidden );
        }    

    // Handle the array change
    if ( aUpdate )
        {
        Refresh();
        }
    }
    
// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SetHideItem()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::SetHideItem(
    const TBool aHide,
    CIpsSetUiItem& aItem,
    const TBool aUpdate )
    {
    FUNC_LOG;
    if ( aHide )
        {
        aItem.iItemFlags.SetFlag32( KIpsSetUiFlagHidden );
        }
    else
        {
        aItem.iItemFlags.ClearFlag32( KIpsSetUiFlagHidden );
        }    

    // Handle the array change
    if ( aUpdate )
        {
        Refresh();
        }
    }
    
/******************************************************************************

    Item search functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::InitDefaultSearch()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::InitDefaultSearch(
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    // In default search, subfolders are included and hidden items are not
    // included into the search
    TUint64 search = 0x00;
    search |= CIpsSetUiFinder::EFinderSearchSubFolders;
    if ( aExcludeHidden )
        {
        search |= CIpsSetUiFinder::EFinderDoHideCheck;
        }
    else
        {
        search &= CIpsSetUiFinder::EFinderDoHideCheck;
        }    
    iFinder->SetSearchFlags( search );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ItemIndex()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::ItemIndex(
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    InitDefaultSearch( aExcludeHidden );

    // Try to make the search, but if the search fails, allow caller
    // to handle the result
    TRAP_IGNORE( iFinder->StartSearchL( *iItemArray, aId ) );

    TInt ret( KErrNotFound );
    if ( iFinder->FinderArray()->Count() )
        {
    // Return the found item
        ret = iFinder->FinderArray()->At( 0 ).iIndex;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetItem()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseArray::GetItem(
    const TInt aIndex,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    // First search the currently active resource to array and check
    // the given index is in the current array
    TBool ok = aIndex < StackCountItems( aExcludeHidden );
    IPS_ASSERT_DEBUG( ok, KErrOverflow, EBaseArray );
    IPS_ASSERT_DEBUG( aIndex >= 0, KErrUnderflow, EBaseArray );

    // Return the found item
    return ok ? iFinder->FinderArray()->At( aIndex ).iItem : NULL; // CS warning
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetItem()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseArray::GetItem(
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    InitDefaultSearch( aExcludeHidden );

    // Try to make the search, but if the search fails, allow caller
    // to handle the result
    TRAP_IGNORE( iFinder->StartSearchL( *iItemArray, aId ) );

    // Return the found item
    return iFinder->FinderArray()->At( 0 ).iItem;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetSubItem()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseArray::GetSubItem(
    const CIpsSetUiItem& aBaseItem,
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    // Make sure the item has subitems
    if ( ItemSubItemCheck( aBaseItem ) )
        {
        // Convert the item to link item
        const CIpsSetUiItemLink* link = 
            static_cast<const CIpsSetUiItemLink*>( &aBaseItem );

        // Get item count
        TInt count = link->iItemLinkArray ?
            link->iItemLinkArray->Count() : KErrNotFound;

        // Find the item from the array
        while ( --count >= 0)
            {
            CIpsSetUiItem* base = ( *link->iItemLinkArray )[count];

            if ( IsVisibleCheck( aBaseItem, aExcludeHidden ) &&
                 base->iItemId == aId )
                {
                return base;
                }
            }
        }

    return NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetSubItem()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseArray::GetSubItem(
    const CIpsSetUiItem& aBaseItem,
    const TInt aIndex,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    CIpsSetUiItem* base = NULL;

    // Make sure the item has subitems
    if ( ItemSubItemCheck( aBaseItem ) )
        {
        // Convert the item to link item
        const CIpsSetUiItemLink* link = 
            static_cast<const CIpsSetUiItemLink*>( &aBaseItem );

        // Get the item count and make sure the required index is
        // below the number of items
        TInt count = link->iItemLinkArray ?
            link->iItemLinkArray->Count() : KErrNotFound;
        count = ( aIndex >= 0 && aIndex < count ) ? count : KErrNotFound;
        TInt index = KErrNotFound;
        TInt item = 0;

        for ( ; index != aIndex && item < count; item++ )
            {
            base = ( *link->iItemLinkArray )[item];

            if ( IsVisibleCheck( *base, aExcludeHidden ) )
                {
                index++;
                }
            }

        // At the end of the items and still not found
        if ( item == count && index != aIndex )
            {
            base = NULL;
            }
        }

    return base;
    }


// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetSubItemByType()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseArray::GetSubItemByType(
    const CIpsSetUiItem& aBaseItem,
    const TInt aType ) const
    {
    FUNC_LOG;
    CIpsSetUiItem* base = NULL;

    // Make sure the item has subitems
    if ( ItemSubItemCheck( aBaseItem ) )
        {
        // Convert the item to link item
        const CIpsSetUiItemLink* link = 
            static_cast<const CIpsSetUiItemLink*>( &aBaseItem );

        // Get the item count and make sure the required index is
        // below the number of items
        TInt count = link->iItemLinkArray ?
            link->iItemLinkArray->Count() : KErrNotFound;

        for ( TInt item = 0 ; !base && item < count; item++ )
            {
            base = ( *link->iItemLinkArray )[item];
            base = ( base && base->Type() == aType ) ? base : NULL;                
            }
        }

    return base;
    }
    
// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetSubItemIndex()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::GetSubItemIndex(
    const CIpsSetUiItemLink& aLinkItem,
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    TBool found = EFalse;
    TInt index = KErrNotFound;

    if ( aLinkItem.iItemLinkArray )
        {
        CIpsSetUiItem* subItem = NULL;

        const TInt items = aLinkItem.iItemLinkArray->Count();
        TInt button = KErrNotFound;

        // Seek the item id and return its index
        while ( !found && ++button < items )
            {
            subItem = ( *aLinkItem.iItemLinkArray )[button];

            // Ignore hidden items AND item is not hidden OR
            // check all the items
            if ( IsVisibleCheck( *subItem, aExcludeHidden ) )
                {
                ++index;

                found = ( subItem->iItemId == aId );
                }
            }
        }

    return found ? index : KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetSubItemIndex()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::GetSubItemIndex(
    const CIpsSetUiItemLink& aLinkItem,
    const TInt aIndex,
    const TBool aActualIndex ) const
    {
    FUNC_LOG;
    TInt result = KErrNotFound;

    if ( aLinkItem.iItemLinkArray )
        {
        CIpsSetUiItem* radioButton = NULL;
        TInt visibleCount = KErrNotFound;
        TInt resourceCount = KErrNotFound;

        TInt items = aLinkItem.iItemLinkArray->Count();

        for ( TInt index = 0; result < 0 && index < items; index++ )
            {
            radioButton = ( *aLinkItem.iItemLinkArray )[index];

            resourceCount++;
            if ( !IsHidden( *radioButton ) )
                {
                visibleCount++;
                }

            if ( aActualIndex )
                {
                if ( aIndex == visibleCount )
                    {
                    result = resourceCount;
                    }
                }
            else
                {
                if ( aIndex == resourceCount )
                    {
                    result = visibleCount;
                    }
                }
            }
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SearchResourceL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::SearchResourceL(
    const TInt aResourceId,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    // When hiding all, search through all subfolders and allow the state
    // change
    TUint64 search = 0x00;
    search |= CIpsSetUiFinder::EFinderSearchSubFolders;
    search |= CIpsSetUiFinder::EFinderResourceSearch;
    if ( aExcludeHidden )
        {
        search |= CIpsSetUiFinder::EFinderDoHideCheck;
        }
    else
        {
        search &= ~CIpsSetUiFinder::EFinderDoHideCheck;
        }    
    iFinder->SetSearchFlags( search );

    // Try to hide items and panic in any error
    TRAPD( error, iFinder->StartSearchL( *iItemArray, aResourceId ) );
    if ( error != KErrNone )
        {
        IPS_ASSERT_DEBUG( EFalse, error, EBaseArray );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetResource()
// ----------------------------------------------------------------------------
//
CIpsSetUiFinderArray* CIpsSetUiBaseArray::GetResourceL(
    const TInt aResourceId,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    // Start the resource search
    SearchResourceL( aResourceId, aExcludeHidden );

    // Return the pointer to array
    return iFinder->FinderArray();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::GetResourceLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiFinderArray* CIpsSetUiBaseArray::GetResourceLC(
    const TInt aResourceId,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    // Start the resource search
    SearchResourceL( aResourceId, aExcludeHidden );
    TUint64 search = iFinder->SearchFlags();
    search |= CIpsSetUiFinder::EFinderMoveOwnership;
    iFinder->SetSearchFlags( search );

    // Return the copy of the array
    return iFinder->FinderArrayLC();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ItemSubArrayCheck()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiBaseArray::ItemSubArrayCheck(
    const CIpsSetUiItem& aBaseItem ) const
    {
    FUNC_LOG;
    // Check if the item type is menu array
    return aBaseItem.Type() == EIpsSetUiMenuArray;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::ItemSubItemCheck()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiBaseArray::ItemSubItemCheck(
    const CIpsSetUiItem& aBaseItem ) const
    {
    FUNC_LOG;
    // if item is not any of the following types it cannot contain subitems
    if ( aBaseItem.iItemType != EIpsSetUiMenuArray &&
         aBaseItem.iItemType != EIpsSetUiRadioButtonArray &&
         aBaseItem.iItemType != EIpsSetUiCheckBoxArray &&
         aBaseItem.iItemType != EIpsSetUiUndefined &&
         aBaseItem.iItemType != EIpsSetUiItemMultiLine )
        {
        return EFalse;
        }

    const CIpsSetUiItemLink* link = 
        static_cast<const CIpsSetUiItemLink*>( &aBaseItem );

    // If the item has an array and it contains items, return true
    return ( link->iItemLinkArray &&
             link->iItemLinkArray->Count() > 0 );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::SetBetweenValues()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::SetBetweenValues(
    TInt& aValue,
    TInt aMin,
    TInt aMax ) const
    {
    FUNC_LOG;
    // Make a sanity check
    if ( aMin > aMax )
        {
        // Swap the values
        TInt temp = aMin;
        aMin = aMax;
        aMax = temp;
        }

    // Make sure that we are above the minimum
    if ( aValue < aMin )
        {
        aValue = aMin;
        }

    // Make sure that we are below the maximum
    if ( aValue > aMax )
        {
        aValue = aMax;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::Refresh()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::Refresh()
    {
    FUNC_LOG;
    TRAP_IGNORE( RefreshL() );
    }
    
// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::RefreshL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::RefreshL()
    {
    FUNC_LOG;
    if ( iArrayFlags.Flag( EIpsSetUiArrayAllowRefresh ) )
        {        
        UpdateActiveItemsCount();
        EventArrayChangedL( EIpsSetUiArrayChanged );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::UpdateActiveItemsCount()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseArray::UpdateActiveItemsCount(
    const TInt aNewCount )
    {
    FUNC_LOG;
    // If count not provided, update it manually
    if ( aNewCount == KErrNotFound )
        {
        iActiveCount = StackCountItems();
        }
    // Just update the count
    else
        {
        iActiveCount = aNewCount;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::HandleStackForwardL()
// ----------------------------------------------------------------------------
//
TIpsSetUiStackResult CIpsSetUiBaseArray::HandleStackForwardL(
    const CIpsSetUiItem& aBase )
    {
    FUNC_LOG;
    // Make sure the item is subarray item
    if ( ItemSubArrayCheck( aBase ) )
        {
        // Make sure the array exists and it contains items
        const CIpsSetUiItemLink* link = 
            static_cast<const CIpsSetUiItemLink*>( &aBase );
        IPS_ASSERT_DEBUG( link->iItemLinkArray ,
            ENullPointer, EBaseArray );
        const CIpsSetUiItem* nextItem = link->iItemLinkArray->At( 0 ) ;
        IPS_ASSERT_DEBUG( nextItem , ENullPointer, EBaseArray );

        // new item has been found, check the resource is not same
        IPS_ASSERT_DEBUG( nextItem->iItemResourceId != StackActiveResource(),
            KErrNotFound, EBaseArray );

        HandleStackChangeL( ETrue, nextItem->iItemResourceId );

        return EIpsSetUiStackResultSubMenuOpen;
        }

    return EIpsSetUiStackResultOk;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::HandleStackBackwardL()
// ----------------------------------------------------------------------------
//
TIpsSetUiStackResult CIpsSetUiBaseArray::HandleStackBackwardL( TInt aCount )
    {
    FUNC_LOG;
    // Remove the last item from stack number of times
    while ( aCount-- && iResourceStack->Count() )
        {
        iResourceStack->Remove( iResourceStack->Count() - 1 );
        }

    return HandleStackChangeL( EFalse );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::HandleStackChangeL()
// ----------------------------------------------------------------------------
//
TIpsSetUiStackResult CIpsSetUiBaseArray::HandleStackChangeL(
    const TInt aForward,
    const TInt aResourceId )
    {
    FUNC_LOG;
    // If resource id not provided
    if ( aResourceId != KErrNotFound )
        {
        iResourceStack->AppendL( aResourceId );
        }

    // Update the active count
    UpdateActiveItemsCount();

    // Inform of the array change
    EventArrayChangedL(
        aForward ? EIpsSetUiArrayStackForward : EIpsSetUiArrayStackBackward );

    return EIpsSetUiStackResultOk;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::StackCountItems()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::StackCountItems(
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    // Set the array finder to contain the current resource in array
    TRAP_IGNORE ( GetResourceL( StackActiveResource(), aExcludeHidden ) );

    return iFinder->FinderArray()->Count();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::StackActiveResource()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseArray::StackActiveResource() const
    {
    FUNC_LOG;
    return ( *iResourceStack )[iResourceStack->Count() - 1];
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseArray::IsVisibleCheck()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiBaseArray::IsVisibleCheck(
    const CIpsSetUiItem& aBaseItem,
    const TBool aExcludeHidden ) const
    {
    FUNC_LOG;
    return ( aExcludeHidden && !IsHidden( aBaseItem ) || !aExcludeHidden );
    }

//  End of File

