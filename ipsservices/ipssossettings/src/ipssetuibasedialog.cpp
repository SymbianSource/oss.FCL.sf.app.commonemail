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
* Description: This file implements class CIpsSetUiBaseDialog.
*
*/



#include "emailtrace.h"
#include <akntitle.h>               // CAknTitlePane
#include <AknQueryDialog.h>         // CAknQueryDialog
#include <aknradiobuttonsettingpage.h> // CAknRadioButtonSettingPage
#include <aknpasswordsettingpage.h> // CAknPasswordSettingPage
#include <akntextsettingpage.h>     // CAknTextSettingPage
#include <akncheckboxsettingpage.h> // CAknCheckBoxSettingPage
#include <ipssossettings.rsg>
#include <StringLoader.h>
//<cmail> checking if MSK is enabled in layout (Layout_Meta_Data::IsMSKEnabled)
#include <layoutmetadata.cdl.h>
//</cmail>

#include "ipssetutilsexception.h"
#include "ipssetuiapprover.h"
#include "ipssetuiitembase.h"      // CIpsSetUiItem
#include "ipssetuiitemvalue.h" // CIpsSetUiItemValue
#include "ipssetuiitemtext.h"  // CIpsSetUiItemEditText
#include "ipssetuiitemlink.h"      // CIpsSetUiItemLink
#include "ipssetuiitemextlink.h"   // CIpsSetUiItemExtLink
#include "ipssetuictrlsoftkey.h"
#include "ipssetdata.h" // CIpsSetData::EAllDay, KIpsSetDataTimeDefault
#include "ipssetuibasedialog.h"     // CIpsSetUiBaseDialog
#include "ipssetutilspageids.hrh"

// Prevent any looping problems by defining maximum amount of editor openings
const TInt KIpsSetUiMaxEditors = 100;



// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CIpsSetUiBaseDialog()
// ----------------------------------------------------------------------------
//
CIpsSetUiBaseDialog::CIpsSetUiBaseDialog(
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane )
    :
    CIpsSetUiBaseArray(),
    iListBox( aListBox ),
    iTitlePane( aTitlePane ),
    iSoftkeys( NULL ),
    iSettingPage( NULL ),
    iSettingPageResources( NULL ),
    iSettingsPageArray( NULL ),
    iScrollBar( NULL ),
    iOldTitleText( NULL ),
    iQueryDialog( NULL ),
    iSettingsLastItemIndex( NULL ),
    iSettingValue( KErrNotFound ),
    iSettingText( NULL ),
    iNewPassword( NULL ),
    iOldPassword( NULL ),
    iButtonArray( NULL ),
    iCheckboxArray( NULL )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::~CIpsSetUiBaseDialog()
// ----------------------------------------------------------------------------
//
CIpsSetUiBaseDialog::~CIpsSetUiBaseDialog()
    {
    FUNC_LOG;
    // Remove the scrollbar from listbox
    iListBox.SetScrollBarFrame( NULL, CEikListBox::EOwnedExternally );

    if ( iSoftkeys )
        {
        iSoftkeys->SetObserver( EFalse, EFalse, EFalse, *this );
        }
    delete iSoftkeys;
    iSoftkeys = NULL;

    delete iSettingPageResources;
    iSettingPageResources = NULL;

    if ( iSettingsLastItemIndex )
        {
        iSettingsLastItemIndex->Reset();
        }

    delete iSettingsLastItemIndex;
    iSettingsLastItemIndex = NULL;

    if ( iCheckboxArray )
        {
        iCheckboxArray->ResetAndDestroy();
        }

    if ( iSettingsPageArray )
        {
        iSettingsPageArray->Reset();
        }

    if ( iBackupArray )
        {
        iBackupArray->ResetAndDestroy();
        }

    delete iBackupArray;
    iBackupArray = NULL;
    delete iSettingsPageArray;
    iSettingsPageArray = NULL;
    delete iCheckboxArray;
    iCheckboxArray = NULL;
    delete iOldTitleText;
    iOldTitleText = NULL;
    delete iQueryDialog;
    iQueryDialog = NULL;
    delete iSettingPage;
    iSettingPage = NULL;
    delete iButtonArray;
    iButtonArray = NULL;
    iSettingText.Close();
    delete iNewPassword;
    iNewPassword = NULL;
    delete iOldPassword;
    iOldPassword = NULL;
    iScrollBar = NULL;
    delete iItemApprover;
    iItemApprover = NULL;
    delete iMainMBoxMenuTitleText;
    iMainMBoxMenuTitleText = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::ConstructL(
    CEikButtonGroupContainer& aSoftkeys,
    const TInt aResource,
    const TBool aUpdateMSK )
    {
    FUNC_LOG;
    BaseConstructL( aResource );

    // Set as observer
    iListBox.SetListBoxObserver( this );
    iListBox.View()->SetListEmptyTextL( KNullDesC );

    // Get scrollbar
    iScrollBar = iListBox.CreateScrollBarFrameL( ETrue );
    iScrollBar->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Create array
    CTextListBoxModel* model = iListBox.Model();
    model->SetItemTextArray( this );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set this dialog for button listening
    iSoftkeys = CIpsSetUiCtrlSoftkey::NewL( aSoftkeys );
    iSoftkeys->SetObserver( ETrue, ETrue, ETrue, *this );
    iSoftkeys->SetMSKLabelling( aUpdateMSK );
    iSoftkeys->UpdateMiddleSoftkeyLabelL(
        GetResourceForMiddlekey(), EAknSoftkeyOpen, this );

    // Prepare setting page arrays
    iSettingPageResources = CIpsSetUiCtrlPageResource::NewL();
    iSettingsLastItemIndex =
        new ( ELeave ) CIpsSetUiIntArray( KIpsSetUiArrayGranularity );

    // Add array to store the backup items
    iBackupArray = new ( ELeave ) CIpsSetUiBaseItemArray( KIpsSetUiArrayGranularity );

    // Define setting pages in enumeration order
    iSettingPageResources->CreateStackL( R_IPS_SET_DIALOG_LISTBOX );
    iSettingPageResources->CreateStackL( R_IPS_SET_DIALOG_CHECKLISTBOX );
    iSettingPageResources->CreateStackL( R_IPS_SET_DIALOG_TEXT );
    iSettingPageResources->CreateStackL( R_IPS_SET_DIALOG_NUMBER );
    iSettingPageResources->CreateStackL( R_IPS_SET_DIALOG_SC_NUMBER );
    iSettingPageResources->CreateStackL( R_IPS_SET_DIALOG_PASSWORD );
    iSettingPageResources->CreateStackL( NULL );

    iItemApprover = CIpsSetUiApprover::NewL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::HandleListBoxEventL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::HandleListBoxEventL(
    CEikListBox* /* aListBox */,
    TListBoxEvent aEventType )
    {
    FUNC_LOG;
    if( aEventType == EEventItemSingleClicked )
    	{
    	CIpsSetUiItem* base = CurrentItem();
    	OpenSettingPageL( *base );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CIpsSetUiBaseDialog::OfferKeyEventL(
    const TKeyEvent& /* aKeyEvent */,
    TEventCode aType )
    {
    FUNC_LOG;
    if ( aType == EEventKeyUp && iSoftkeys->MSKLabelling() )
        {
        iSoftkeys->UpdateMiddleSoftkeyLabelL(
            GetResourceForMiddlekey(), EAknSoftkeyOpen, this );
        }

    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::GetResourceForMiddlekey()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::GetResourceForMiddlekey()
    {
    FUNC_LOG;
//<cmail> run-time MSK variation check (enabled in platform + in current layout)
    TInt resource = 0;
    if ( AknLayoutUtils::MSKEnabled() && Layout_Meta_Data::IsMSKEnabled() )
        {
        CIpsSetUiItem* item = CurrentItem();
        IPS_ASSERT_DEBUG( item , KErrNotFound, EBaseDialog );
        if ( item )
            {
            resource = ItemSubArrayCheck( *item ) ?
                R_QTN_MSK_OPEN : R_QTN_MSK_CHANGE; // <cmail>
            }
        }
    return resource;
//</cmail>
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SetTitlePaneTextL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SetMainMBoxMenuTitlePaneTextL(
    const TDesC& aText,
    const TBool aSaveCurrent )
    {
    FUNC_LOG;
    // aText contains mailbox name
    HBufC* titleText = StringLoader::LoadLC(
            R_FSE_SETTINGS_LIST_MAILBOX_TITLE, aText );

    if ( aSaveCurrent )
        {
        delete iOldTitleText;
        iOldTitleText = NULL;
        iOldTitleText = iTitlePane.Text()->Alloc();
        delete iMainMBoxMenuTitleText;
        iMainMBoxMenuTitleText = NULL;
        iMainMBoxMenuTitleText = aText.AllocL();
        }

    iTitlePane.SetTextL( *titleText );
    CleanupStack::PopAndDestroy( titleText );
    titleText = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SetPreviousTitlePaneText()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SetSettingsMenuTitlePaneText(
    const TBool aSaveCurrent )
    {
    FUNC_LOG;
    TRAP_IGNORE( SetMainMBoxMenuTitlePaneTextL(
            *iOldTitleText, aSaveCurrent ) );
    }

/******************************************************************************

    Settingpage opening

    - Find out the item to be edited
    - Send start editing event with the item
    - Check the item type
    - Check the resources
    - If no resources available, ask them
    - Start editing
    - Send end editing event

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::OpenSettingPageL()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::OpenSettingPageL(
    const TInt aIndex )
    {
    FUNC_LOG;
    // To open setting page, check if the Setting Page index is provided,
    // if not get the current item index in array
    TInt index = ( aIndex == KErrNotFound ) ?
        iListBox.CurrentItemIndex() : aIndex;

    // Search for the item and open it
    return SettingPageOpenL( *GetItem( index ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::OpenSettingPageL()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::OpenSettingPageL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    return SettingPageOpenL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPageOpenL()
//
// This function contains the main logic for the setting editor handling
// in the settings dialog. The setting editors are kept opening as long
// as the all the settings are gone through or the editor is dismissed.
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingPageOpenL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    // Don't open the editor for read-only items
    if ( aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagReadOnly ) )
        {
        return EIpsSetUiPageResultReadOnly;
        }
    // Store current focused item for last item index array
    iCurrentItemIndex = iListBox.CurrentItemIndex();

    // Starting editing of the item, do not allow updating during editing
    iArrayFlags.ClearFlag( EIpsSetUiArrayAllowRefresh );

    // When starting to edit page, send event so that item can be prepared
    // for the editing. In case of error, editing won't start
    TIpsSetUiEventResult eventResult = EventItemEditStartsL( aBaseItem );
    TIpsSetUiPageResult result;

    if ( eventResult == EIpsSetUiPageEventResultApproved )
        {
        // Create the array if it doesn't exist yet
        if ( !iSettingsPageArray )
            {
            iSettingsPageArray = new ( ELeave ) CIpsSetUiBaseItemArray(
                KIpsSetUiArrayGranularity );
            }

        // Clear the array and add the editable item to array
        CleanBackupArray();
        iSettingsPageArray->Reset();
        SetCurrentSettingsPageL( aBaseItem );
        result = SettingsPageOpenCheckL();

        do
            {
            if ( result == EIpsSetUiPageResultEditing )
                {
                // Open the setting page
                result = SettingPageOpenEditorL();

                // Do open check. Note that the result can be modified!
                SettingsCheckNextEditorOpenL( result );
                }

            // Finish the editing of the item
            SettingPageOpenFinishL();
            }
        while ( result == EIpsSetUiPageResultEditing );
        }
    else if ( eventResult == EIpsSetUiPageEventResultDisapproved )
        {
        result = EIpsSetUiPageResultOk;
        }
    else
        {
        result = EIpsSetUiPageResultEventCancel;
        }

    // Send item edit ends
    EventItemEditEndsL( aBaseItem );
    aBaseItem.iItemFlags.ClearFlag32( KIpsSetUiFlagOneTimeForceViewOpen );
    iArrayFlags.SetFlag( EIpsSetUiArrayAllowRefresh );
    
    // Update the listbox
    Refresh();

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventPrepareSubArrayL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::EventPrepareSubArrayL(
    CIpsSetUiItem& aBaseItem,
    const TBool aForward )
    {
    FUNC_LOG;
    // Check direction in array
    UpdateTitlePaneTextL( GetLastResourceId( ) );
    if ( aForward )
        {
        // Store the index and set to beginning
        SetSettingsLastItemIndexL( iCurrentItemIndex );
        iListBox.SetCurrentItemIndex( 0 );
        }
    // When going backwards
    else
        {
        // Restore previous index
        iListBox.SetCurrentItemIndex( SettingsLastItemIndex() );
        }

    return EventSubArrayChangeL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::UpdateTitlePaneTextL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::UpdateTitlePaneTextL( TInt aId )
    {
    FUNC_LOG;
    HBufC* titleText = NULL;
    switch ( aId )
        {
        case R_IPS_SET_MENU_MAILBOX_SETTINGS:
            titleText = StringLoader::LoadLC( R_FSE_SETTINGS_TITLE_MAILBOX );
            iTitlePane.SetTextL( *titleText );
            break;
        case R_IPS_SET_MENU_ADVANCED_MAILBOX_SETTINGS:
            titleText = StringLoader::LoadLC(
                    R_FSE_SETTINGS_ADVANCED_MAILBOX_SETTINGS );
            iTitlePane.SetTextL( *titleText );
            break;
		// <cmail> ipssossettings to use correct loc strings in titles
        case R_IPS_SET_MENU_INCOMING_SETTINGS:
            titleText = StringLoader::LoadLC(
            	R_FSE_SETTINGS_INCOMING_TITLE );
            iTitlePane.SetTextL( *titleText );
            break;
        case R_IPS_SET_MENU_OUTGOING_SETTINGS:
            titleText = StringLoader::LoadLC(
            	R_FSE_SETTINGS_OUTGOING_TITLE );
            iTitlePane.SetTextL( *titleText );
            break;
        case R_IPS_SET_MENU_WHAT_TO_SYNC:
        //case R_IPS_SET_WHAT_TO_SYNC_ARRAY_EMAIL_RETRIEVE_POP3:
        case R_IPS_SET_WHAT_TO_SYNC_ARRAY_EMAIL_RETRIEVE_IMAP4:
            titleText = StringLoader::LoadLC( 
                    R_FSE_SETTINGS_SERVICE_WHAT_TO_SYNC_TITLE );
            iTitlePane.SetTextL( *titleText );
            break;
        case R_IPS_SET_MENU_WHEN_TO_SYNC:
            titleText = StringLoader::LoadLC( 
                    R_FSE_SETTINGS_SERVICE_WHEN_TO_SYNC_TITLE );
            iTitlePane.SetTextL( *titleText );
            break;
        // </cmail>
        default:
            {
            delete iMainMBoxMenuTitleText;
            iMainMBoxMenuTitleText = NULL;
            iMainMBoxMenuTitleText = GetItem( TUid::Uid( EIpsSetUiMailboxMailboxName ) )->Text().AllocL();
            Refresh();
            
            if ( iMainMBoxMenuTitleText )
                {
                titleText = StringLoader::LoadLC(
                        R_FSE_SETTINGS_LIST_MAILBOX_TITLE, *iMainMBoxMenuTitleText );
                iTitlePane.SetTextL( *titleText );
                }
            break;
            }
        }
    if ( titleText )
        {
        CleanupStack::PopAndDestroy( titleText );
        }
    }

/******************************************************************************

    Settingpage creation

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingsPageOpenCheckL()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingsPageOpenCheckL()
    {
    FUNC_LOG;
    CIpsSetUiItem* base = CurrentSettingsPage();

    // Before starting to open the page, check that item type is not
    // menuarray. In case of menuarray, just open the next level
    if ( HandleStackForwardL( *base ) == EIpsSetUiStackResultSubMenuOpen )
        {
        iSoftkeys->UpdateMiddleSoftkeyLabelL(
            GetResourceForMiddlekey(), EAknSoftkeyOpen, this );
        return EIpsSetUiPageResultSubMenuOpen;
        }

    // Don't open read only items
    if ( base->iItemFlags.Flag32( KIpsSetUiFlagReadOnly ) )
        {
        return EIpsSetUiPageResultReadOnly;
        }

    // The page is ok is for edit
    return EIpsSetUiPageResultEditing;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingsCheckNextEditorOpenL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingsCheckNextEditorOpenL(
    TIpsSetUiPageResult& aResult )
    {
    FUNC_LOG;
    // The settings user have selected, must be available to user.
    // For this reason, items shall restored to main array only, when
    // returning from editors

    switch ( aResult )
        {
        // Handle approved
        case EIpsSetUiPageResultOk:
            aResult = HandleEditorOk();
            break;

        // Handle the cancelled editor page
        case EIpsSetUiPageResultCancelled:
            aResult = HandleEditorCancelL();
            break;

        // Continue editing
        case EIpsSetUiPageResultEditing:
            aResult = HandleEditorDisapproved();
            break;

        default:
            IPS_ASSERT_DEBUG( EFalse, KErrUnknown, EBaseDialog );
            User::Leave( KErrUnknown );
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::HandleEditorCancelL()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::HandleEditorCancelL()
    {
    FUNC_LOG;
    // The editor was cancelled, so the first check needed to make
    // is deterimine, if the editor was first in the line.
    TIpsSetUiPageResult result = EIpsSetUiPageResultInvalidResult;

    TInt editorCount = iSettingsPageArray->Count();
    if ( editorCount == 1 )
        {
        // Restore all of the items
        RestoreFromBackup();
        result = EIpsSetUiPageResultCancelled;
        }
    // More than one page in the array, just jump back to the previous
    // and remove the previous value
    else if ( editorCount > 0 )
        {
        RemoveLastBackupItem();
        iSettingsPageArray->Delete( editorCount-1 );
        result = EIpsSetUiPageResultEditing;
        }
    // Array is empty, unexpected situation
    else
        {
        IPS_ASSERT_DEBUG( EFalse, KErrNotFound, EBaseDialog );
        User::Leave( KErrNotFound );
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::HandleEditorOk()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::HandleEditorOk()
    {
    FUNC_LOG;
    CIpsSetUiItem* nextItem = NULL;

    // Check if current selection contains subeditors
    if ( iSettingValue >= 0 )
        {
        nextItem = SettingFindEditor(
           *CurrentSettingsPage(), iSettingValue );
        }

    TIpsSetUiPageResult result = EIpsSetUiPageResultInvalidResult;

    // In case subeditor is found, set the item to list and continue
    // editing
    if ( nextItem )
        {
        TRAP_IGNORE( SetCurrentSettingsPageL( *nextItem ) );
        result = EIpsSetUiPageResultEditing;
        }
    // Cleanup the array and stop editing
    else
        {
        CleanBackupArray();
        result = EIpsSetUiPageResultOk;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::HandleEditorDisapproved()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::HandleEditorDisapproved()
    {
    FUNC_LOG;
    // Get current setting page
    CurrentSettingsPage()->SetText( iSettingText );

    return EIpsSetUiPageResultEditing;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPageOpenEditorL()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingPageOpenEditorL()
    {
    FUNC_LOG;
    CIpsSetUiItem* currentEditor = CurrentSettingsPage();

    // Prepare setting page. The setting page shall be acquired
    // from the owner class, if required, or the setting page is
    // created with default values
    TIpsSetUiUpdateMode updateMode;
    TIpsSetUtilsFlags& editorFlags = currentEditor->iItemFlags;

    // If enter pressed is set, the setting page won't be opened,
    // which means creation of page is not needed either
    if ( !( editorFlags.Flag32( KIpsSetUiFlagEnterPressed ) ) ||
         editorFlags.Flag32( KIpsSetUiFlagOneTimeForceViewOpen ) )
        {
        if ( editorFlags.Flag32( KIpsSetUiFlagAcquireCustomPage ) )
            {
            SettingPrepareAcquireL( updateMode, *currentEditor );
            IPS_ASSERT_DEBUG( iSettingPage , KErrUnknown, EBaseDialog );
            }
        else
            {
            SettingCreateEditorL( *currentEditor, updateMode );
            }
        }

    // Finally launch the editor
    return SettingLaunchPageLD( *currentEditor, updateMode );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPageOpenFinishL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingPageOpenFinishL()
    {
    FUNC_LOG;
    // Clean up
    iBaseDialogFlags &= ~EIpsSetUiRunningCustomPage;
    delete iButtonArray;
    iButtonArray = NULL;
    if ( iCheckboxArray )
        {
        iCheckboxArray->ResetAndDestroy();
        }
    delete iCheckboxArray;
    iCheckboxArray = NULL;
    iSettingPage = NULL;
    iSettingValue = KErrNotFound;
    iSettingText.Close();
    delete iNewPassword;
    iNewPassword = NULL;
    delete iOldPassword;
    iOldPassword = NULL;
    }

/******************************************************************************

    Settingpage Open

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::SettingFindEditor()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseDialog::SettingFindEditor(
    const CIpsSetUiItem& aBaseItem,
    const TInt aIndex,
    const TBool aExcludeHidden )
    {
    FUNC_LOG;
    CIpsSetUiItem* volunteerBase = NULL;

    // The settings structure is constructed in a way, that the radiobutton
    // or the checkbox editor has the array of buttons. To retrieve the editor
    // first the radiobutton has to be retrieved, then inside the radiobutton,
    // the editor can be taken.
    volunteerBase =
        GetSubItem( aBaseItem, aIndex, aExcludeHidden );

    if ( volunteerBase &&
        volunteerBase->iItemFlags.Flag32( KIpsSetUiFlagFindEditor ) )
        {
        // Find the editor from the link array
        volunteerBase = GetSubItem( *volunteerBase, 0, EFalse );

        if ( !volunteerBase )
            {
            // Not found, yet. It may be seperate editor in the button arrays
            volunteerBase = GetSubItemByType( aBaseItem, EIpsSetUiItemValue );
            }

        if ( !volunteerBase )
            {
            // Still not found... time editor perhaps??
            volunteerBase =
                GetSubItemByType( aBaseItem, EIpsSetUiItemMultiLine );
            }
        }
    else
        {
        volunteerBase = NULL;
        }

    // return the item
    return volunteerBase;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::SettingFindEditor()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseDialog::SettingFindEditor(
    const CIpsSetUiItem& aBaseItem,
    const TUid& aId,
    const TBool aExcludeHidden )
    {
    FUNC_LOG;
    // The settings structure is constructed in a way, that the radiobutton
    // or the checkbox editor has the array of buttons. To retrieve the editor
    // first the radiobutton has to be retrieved, then inside the radiobutton,
    // the editor can be taken.
    CIpsSetUiItem* volunteerBase =
        GetSubItem( aBaseItem, aId, aExcludeHidden );

    // Make sure the item is found and check that it is
    if ( volunteerBase )
        {
        volunteerBase = GetSubItem( *volunteerBase, 0, EFalse );
        }
    else
        {
        volunteerBase = NULL;
        }

    // return the item
    return volunteerBase;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPrepareAcquireRadioButtonPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingPrepareAcquireRadioButtonPageL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    // Convert the base item to link item and define button array
    CIpsSetUiItemLink* radio = static_cast<CIpsSetUiItemLink*>( &aBaseItem );

    // Make sure the link exist
    if ( !radio->iItemLinkArray )
        {
        radio->iItemLinkArray =
            new ( ELeave ) CIpsSetUiBaseItemArray( KIpsSetUiArrayGranularity );
        }

    SettingCreateButtonArrayL( *radio->iItemLinkArray );

    iSettingValue = radio->Value();
    radio = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPrepareAcquireCheckboxPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingPrepareAcquireCheckboxPageL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    // Convert the base item to link item and define button array
    CIpsSetUiItemLink* link = static_cast<CIpsSetUiItemLink*>( &aBaseItem );

    // Make sure the link array exist
    if ( link->iItemLinkArray )
        {
        SettingCreateButtonArrayL( *link->iItemLinkArray );
        }

    link = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPrepareAcquireTextEditorPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingPrepareAcquireTextEditorPageL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    // Create either password or plain texts
    if ( aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagPassword ) )
        {
        iNewPassword = new ( ELeave ) TIpsSetUiPasswordText( aBaseItem.Text() );
        iOldPassword = new ( ELeave ) TIpsSetUiPasswordText( aBaseItem.Text() );
        }
    else
        {
        iSettingText.CreateL( aBaseItem.Text() );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPrepareAcquireValueEditorPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingPrepareAcquireValueEditorPageL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    iSettingValue = aBaseItem.Value();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPrepareAcquireL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingPrepareAcquireL(
    TIpsSetUiUpdateMode& aUpdateMode,
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    // Handle the item, based on the type
    switch ( aBaseItem.Type() )
        {
        // Radio button page
        case EIpsSetUiRadioButtonArray:
            SettingPrepareAcquireRadioButtonPageL( aBaseItem );
            break;

        // Checkbox page
        case EIpsSetUiCheckBoxArray:
            SettingPrepareAcquireCheckboxPageL( aBaseItem );
            break;

        // Text editor
        case EIpsSetUiItemText:
            SettingPrepareAcquireTextEditorPageL( aBaseItem );
            break;

        // Number editor
        case EIpsSetUiItemValue:
            SettingPrepareAcquireValueEditorPageL( aBaseItem );
            break;

        // Type is not recognized
        default:
            IPS_ASSERT_DEBUG( EFalse, KErrUnknown, EBaseDialog );
            User::Leave( KErrUnknown );
        }

    // Acquire the custom setting page
    AcquireCustomSettingPageL(
        iSettingPage, aUpdateMode, iSettingValue,
        iSettingText, *iButtonArray, aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::AcquireCustomSettingPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::AcquireCustomSettingPageL(
    CAknSettingPage*& /* aReturnPage */,
    TIpsSetUiUpdateMode& /* aReturnUpdateMode */,
    TInt& /* aReturnSettingValue */,
    TDes& /* aReturnSettingText */,
    CDesCArrayFlat& /* aParamRadioButtonArray */,
    CIpsSetUiItem& /* aParamBaseItem */ )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreateEditorL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreateEditorL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Create different editors for password settings
    if ( aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagPassword ) )
        {
        switch ( aBaseItem.iItemType )
            {
            case EIpsSetUiItemText:
                SettingCreatePasswordTextEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            }
        }
    // Normal editors
    else
        {
        switch ( aBaseItem.iItemType )
            {
            case EIpsSetUiRadioButtonArray:
                SettingCreateRadioButtonPageL( aBaseItem, aUpdateMode );
                break;

            case EIpsSetUiCheckBoxArray:
                SettingCreateCheckboxPageL( aBaseItem, aUpdateMode );
                break;

            case EIpsSetUiItemText:
                SettingCreatePlainTextEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            case EIpsSetUiItemValue:
                SettingCreatePlainNumberEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            case EIpsSetUiItemScValue:
                SettingCreateScNumberEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            default:
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::DefineDefaultSettingPageL()
// ----------------------------------------------------------------------------
//
TIpsSetUiUpdateMode CIpsSetUiBaseDialog::DefineDefaultSettingPageL(
    const CIpsSetUiItem& aBase )
    {
    FUNC_LOG;
    // Create the aSetting page
    iSettingPage->SetSettingTextL( *aBase.iItemLabel );

    // Determine the update mode
    return static_cast<TIpsSetUiUpdateMode>( aBase.iItemFlags.ValueForFlag(
        KIpsSetUiFlagUpdateOnChange,
        CAknSettingPage::EUpdateWhenChanged,
        CAknSettingPage::EUpdateWhenAccepted ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreateButtonArrayL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreateButtonArrayL(
    CIpsSetUiBaseItemArray& aButtonArray )
    {
    FUNC_LOG;
    // Get the item count in array
    const TInt count = aButtonArray.Count();
    iButtonArray = new ( ELeave ) CDesCArrayFlat( KIpsSetUiArrayGranularity );

    // Do number conversion to all items and add them to temporary array
    for ( TInt item = 0; item < count; item++ )
        {
        CIpsSetUiItem* base = aButtonArray[item];

        // Append only visible items on the list
        if ( !IsHidden( *base ) )
            {
            HBufC* label = base->iItemLabel->AllocLC();
            TPtr labelPtr = label->Des();

            // Do number conversion when needed
            if ( base->iItemFlags.Flag32( KIpsSetUiFlagLangSpecificNumConv ) )
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                    labelPtr );
                }

            // Insert to array and delete old item
            iButtonArray->AppendL( *label );
            CleanupStack::PopAndDestroy( label );
            label = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreateCheckboxArrayL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreateCheckboxArrayL(
    CIpsSetUiBaseItemArray& aButtonArray )
    {
    FUNC_LOG;
    // Get the item count in array
    const TInt count = aButtonArray.Count();
    iCheckboxArray =
        new ( ELeave ) CSelectionItemList( KIpsSetUiArrayGranularity );

    // Do number conversion to all items and add them to temporary array
    for ( TInt item = 0; item < count; item++ )
        {
        CIpsSetUiItem* base = aButtonArray[item];

        // Append only visible items on the list
        if ( !IsHidden( *base ) )
            {
            HBufC* label = base->iItemLabel->AllocLC();
            TPtr labelPtr = label->Des();

            // Do number conversion when needed
            if ( base->iItemFlags.Flag32( KIpsSetUiFlagLangSpecificNumConv ) )
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                    labelPtr );
                }

            // Create item and check it
            CSelectableItem* checkbox =
                new ( ELeave ) CSelectableItem(
                   *label, base->iItemFlags.Flag32( KIpsSetUiFlagChecked ) );
            CleanupStack::PushL( checkbox );
            checkbox->ConstructL();

            // Insert to array and delete old item
            iCheckboxArray->AppendL( checkbox );
            CleanupStack::Pop( checkbox );
            CleanupStack::PopAndDestroy( label );
            label = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreateRadioButtonPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreateRadioButtonPageL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Convert the base item to radiobutton item
    CIpsSetUiItemLinkExt* radioArray =
        static_cast<CIpsSetUiItemLinkExt*>( &aBaseItem );

    // Create button array for the page and set the selection for page
    SettingCreateButtonArrayL( *radioArray->iItemLinkArray );
    iSettingValue = radioArray->Value();

    // Create the setting page to member
    iSettingPage = ( new ( ELeave ) CAknRadioButtonSettingPage(
        ( *iSettingPageResources )[EIpsSetUiRadioButton],
        iSettingValue, iButtonArray ) );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreateCheckboxPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreateCheckboxPageL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Convert the base item to radiobutton item
    CIpsSetUiItemLink* radioArray =
        static_cast<CIpsSetUiItemLink*>( &aBaseItem );

    // Create button array for the page
    SettingCreateCheckboxArrayL( *radioArray->iItemLinkArray );

    // Create the setting page to member
    iSettingPage = ( new ( ELeave ) CAknCheckBoxSettingPage(
        ( *iSettingPageResources )[EIpsSetUiCheckbox], iCheckboxArray ) );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreatePlainTextEditorPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreatePlainTextEditorPageL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Get the text to be edited
    CIpsSetUiItemsEditText* textEditor =
        static_cast<CIpsSetUiItemsEditText*>( &aBaseItem );
    iSettingText.CreateL( textEditor->iItemMaxLength );
    iSettingText.Copy( textEditor->Text() );

    // Define flags according to item
    TInt flags = GetTextEditorFlags( aBaseItem );

    // Create the setting page using the flags
    TInt pageResource = ( *iSettingPageResources )[EIpsSetUiText];
    CAknTextSettingPage* settingPage =
        new ( ELeave ) CAknTextSettingPage(
            pageResource, iSettingText, flags );
    CleanupStack::PushL( settingPage );
    settingPage->ConstructL();
    CEikEdwin& txtCtrl = *settingPage->TextControl();
    txtCtrl.SetTextLimit( textEditor->iItemMaxLength );

    if ( textEditor->iItemFlags.Flag32( KIpsSetUiFlagLatinOnly ) )
        {
        txtCtrl.SetAknEditorFlags(
            EAknEditorFlagLatinInputModesOnly | EAknEditorFlagNoT9 );
        }

    iSettingPage = settingPage;
    CleanupStack::Pop( settingPage );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreatePasswordTextEditorPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreatePasswordTextEditorPageL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Prepare password and save the old
    CIpsSetUiItemsEditText* textEditor =
        static_cast<CIpsSetUiItemsEditText*>( &aBaseItem );

    iNewPassword = new ( ELeave ) TIpsSetUiPasswordText(
        textEditor->Text().Left( KIpsSetUiMaxPasswordLength ) );
    iOldPassword = new ( ELeave ) TIpsSetUiPasswordText(
        textEditor->Text().Left( KIpsSetUiMaxPasswordLength ) );

    // Create the settings page
    iSettingPage = ( new ( ELeave ) CAknAlphaPasswordSettingPage(
        ( *iSettingPageResources )[EIpsSetUiSecret],
        *iNewPassword, *iOldPassword ) );
    iSettingPage->ConstructL();
    CAknAlphaPasswordSettingPage* pwPage =
        static_cast<CAknAlphaPasswordSettingPage*>( iSettingPage );
    pwPage->SetMaxPasswordLength( textEditor->iItemMaxLength );
    if ( pwPage->AlphaPasswordEditor() )
        {
        pwPage->AlphaPasswordEditor()->SetMaxLength(
            textEditor->iItemMaxLength );
        }

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreatePlainNumberEditorPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreatePlainNumberEditorPageL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Conver item and set the value
    CIpsSetUiItemValue* numberEditor =
        static_cast<CIpsSetUiItemValue*>( &aBaseItem );
    iSettingValue = numberEditor->Value();

    // Define flags according to item
    TInt flags = GetValueEditorFlags( aBaseItem );

    // Create aSetting page
    iSettingPage = ( new ( ELeave ) CAknIntegerSettingPage(
        ( *iSettingPageResources )[EIpsSetUiNumber],
           *&iSettingValue, flags ) );
    iSettingPage->ConstructL();
    iSettingPage->SetSettingTextL( *numberEditor->iItemLabel );

    // Apply defaults, for number editors, the update mode has to be onchange
    aBaseItem.iItemFlags.SetFlag32( KIpsSetUiFlagUpdateOnChange );
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingCreateScNumberEditorPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SettingCreateScNumberEditorPageL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Conver item and set the value
    CIpsSetUiItemsEditText* textEditor =
        static_cast<CIpsSetUiItemsEditText*>( &aBaseItem );
    iSettingText.CreateL( textEditor->iItemMaxLength );
    iSettingText.Copy( textEditor->Text() );

    if ( aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagLangSpecificNumConv ) )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
            iSettingText );
        }

    // Define flags according to item
    TInt flags = CAknIntegerSettingPage::EInvalidValueNotOffered;
    flags |= !aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagForceMustFill ) &
        CAknIntegerSettingPage::EEmptyValueAllowed;
    flags |= aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagNoInitialSelection ) &
        CAknIntegerSettingPage::ENoInitialSelection;
    flags |= aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagEditorCursorAtBeginning ) &
        CAknIntegerSettingPage::EPutCursorAtBeginning;

    // Create aSetting page
    iSettingPage = ( new ( ELeave ) CAknTextSettingPage(
        ( *iSettingPageResources )[EIpsSetUiScNumber],
        iSettingText, flags ) );
    iSettingPage->ConstructL();
    static_cast<CAknTextSettingPage*>( iSettingPage
        )->TextControl()->SetTextLimit( textEditor->iItemMaxLength );
    iSettingPage->SetSettingTextL( *textEditor->iItemLabel );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchPageLD()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingLaunchPageLD(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    switch ( aBaseItem.Type() )
        {
        case EIpsSetUiRadioButtonArray:
            return SettingLaunchRadioButtonPageLD(
                aBaseItem, aUpdateMode );

        case EIpsSetUiCheckBoxArray:
            return SettingLaunchCheckboxPageLD( aBaseItem, aUpdateMode );

        case EIpsSetUiItemText:
        case EIpsSetUiItemScValue:
            return SettingLaunchTextEditorPageLD( aBaseItem, aUpdateMode );

        case EIpsSetUiItemValue:
            return SettingLaunchNumberEditorPageLD( aBaseItem, aUpdateMode );

        // Multiline dialogs are launched from client only
        case EIpsSetUiItemMultiLine:
            return SettingLaunchMultilinePageL( aBaseItem, aUpdateMode );

        default:
            IPS_ASSERT_DEBUG( EFalse, KErrUnknown, EBaseDialog );
            return EIpsSetUiPageResultCancelled;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchRadioButtonPageLD()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingLaunchRadioButtonPageLD(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Convert the base item to radio item
    CIpsSetUiItemLink* linkArray = static_cast<CIpsSetUiItemLink*>( &aBaseItem );
    TIpsSetUiPageResult result = EIpsSetUiPageResultOk;

    if ( linkArray->iItemFlags.Flag32( KIpsSetUiFlagEnterPressed ) &&
         !linkArray->iItemFlags.Flag32( KIpsSetUiFlagOneTimeForceViewOpen ) )
        {
        TInt value = !linkArray->Value();
        TIpsSetUiEventResult evaluation =
            EventItemEvaluateRadioButton( *linkArray, value );

        // Handle approved item
        if ( evaluation == EIpsSetUiPageEventResultApproved )
            {
            // Change the value and the text
            linkArray->SetValue( value );
            linkArray->iItemSettingText->Copy(
               *( *linkArray->iItemLinkArray
                    )[value]->iItemLabel );
            }

        result = EIpsSetUiPageResultOk;
        }
    else
        {
        // Opening the editor, clear flag immediately
        linkArray->iItemFlags.ClearFlag32( KIpsSetUiFlagOneTimeForceViewOpen );

        // Launch the settings dialog and update the item, if item is valid
        if ( iSettingPage->ExecuteLD( aUpdateMode ) > 0 )
            {
            iSettingPage = NULL;
            TIpsSetUiEventResult evaluation =
                EventItemEvaluateRadioButton( *linkArray, iSettingValue );

            // Handle approved item
            if ( evaluation == EIpsSetUiPageEventResultApproved )
                {
                // First store the index value from the visible radiobutton array
                linkArray->SetValue( iSettingValue );
                CIpsSetUiItem* subItem =
                    GetSubItem( *linkArray, iSettingValue );

                // Mark the item as checked and copy the text
                if ( subItem )
                    {
                    CheckRadioButton( *linkArray, subItem->iItemId );
                    linkArray->iItemSettingText->Copy( *subItem->iItemLabel );
                    }

                if( linkArray->iItemId.iUid == EIpsSetUiWhenHours && 
                    iSettingValue == CIpsSetData::EAllDay )
                    {
                    // set related 'from' and 'to' settings to 0
                    CIpsSetUiItem* item = GetItem(
                        TUid::Uid( EIpsSetUiWhenHoursEditCustomizeFrom ) );
                    item->SetValue( KIpsSetDataTimeDialogDefault );
                    item = GetItem(
                        TUid::Uid( EIpsSetUiWhenHoursEditCustomizeTo ) );
                    item->SetValue( KIpsSetDataTimeDialogDefault );
                    }

                result = EIpsSetUiPageResultOk;
                }
            // Handle disapproved item
            else if ( evaluation == EIpsSetUiPageEventResultDisapproved )
                {
                result = EIpsSetUiPageResultEditing;
                }
            // Handle cancelled item
            else
                {
                result = EIpsSetUiPageResultCancelled;
                }
            }
        else
            {
            // Editor cancelled
            iSettingPage = NULL;
            result = EIpsSetUiPageResultCancelled;
            }
        }

    linkArray = NULL;

    // Return the result
    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchCheckboxPageLD()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingLaunchCheckboxPageLD(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Convert the base item to link item
    CIpsSetUiItemLinkExt* checkboxArray =
        static_cast<CIpsSetUiItemLinkExt*>( &aBaseItem );
    TIpsSetUiPageResult result = EIpsSetUiPageResultOk;

    // Launch the settings dialog and update the item, if item is valid
    if ( iSettingPage->ExecuteLD( aUpdateMode ) )
        {
        iSettingPage = NULL;
        TIpsSetUiEventResult evaluation =
            EventItemEvaluateValue( aBaseItem, iSettingValue );

        if ( evaluation == EIpsSetUiPageEventResultApproved )
            {
            // Clean the value
            TInt checkbox = 0;

            // Check all items in the array, and set the flags to item
            for ( TInt item = iCheckboxArray->Count()-1; item >= 0; item-- )
                {
                TBool checked = iCheckboxArray->At( item )->SelectionStatus();
                if ( checked )
                    {
                    checkboxArray->iItemLinkArray->At(
                        item )->iItemFlags.SetFlag32( KIpsSetUiFlagChecked );
                    }
                else
                    {
                    checkboxArray->iItemLinkArray->At(
                        item )->iItemFlags.ClearFlag32( KIpsSetUiFlagChecked );
                    }
                checkbox |= ( checked << item );
                }

            checkboxArray->SetValue( checkbox );

            result = EIpsSetUiPageResultOk;
            }
        // Handle disapproved item
        else if ( evaluation == EIpsSetUiPageEventResultDisapproved )
            {
            result = EIpsSetUiPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EIpsSetUiPageResultCancelled;
            }
        }
    else
        {
        // Editor cancelled
        iSettingPage = NULL;
        result = EIpsSetUiPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchTextEditorPageLD()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingLaunchTextEditorPageLD(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    TIpsSetUiPageResult result = EIpsSetUiPageResultOk;

    if ( iSettingPage->ExecuteLD( aUpdateMode ) > 0 )
        {
        iSettingPage = NULL;
        RBuf tempText;

        // Depending on the text editor, choose the correct text
        if ( aBaseItem.iItemFlags.Flag32( KIpsSetUiFlagPassword ) )
            {
            tempText.CreateL( *iNewPassword );
            }
        else
            {
            tempText.CreateL( iSettingText );
            }

        TIpsSetUiEventResult evaluation =
            EventItemEvaluateText( aBaseItem, tempText );

        if ( evaluation == EIpsSetUiPageEventResultApproved )
            {
            // Text is ok, store the text and close editor
            aBaseItem.SetText( tempText );
            }
        // Handle disapproved item
        else if ( evaluation == EIpsSetUiPageEventResultDisapproved )
            {
            result = EIpsSetUiPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EIpsSetUiPageResultCancelled;
            }

        tempText.Close();
        }
    else
        {
        iSettingPage = NULL;
        result = EIpsSetUiPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchNumberEditorPageLD()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingLaunchNumberEditorPageLD(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    TIpsSetUiPageResult result = EIpsSetUiPageResultOk;

    // Convert baseitem to number editor
    CIpsSetUiItemValue* numberEdit =
        static_cast<CIpsSetUiItemValue*>( &aBaseItem );

    // Launch the editor and fill the settings if approved
    if ( iSettingPage->ExecuteLD( aUpdateMode ) )
        {
        iSettingPage = NULL;
        TIpsSetUiEventResult evaluation =
            EventItemEvaluateValue( aBaseItem, iSettingValue );

        // Evaluate item
        if ( evaluation == EIpsSetUiPageEventResultApproved )
            {
            // Item ok
            numberEdit->SetValue( iSettingValue );
            numberEdit->iItemSettingText->Num( iSettingValue );
            result = EIpsSetUiPageResultOk;
            }
        // Handle disapproved item
        else if ( evaluation == EIpsSetUiPageEventResultDisapproved )
            {
            result = EIpsSetUiPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EIpsSetUiPageResultCancelled;
            }
        }
    else
        {
        // Editor cancelled
        iSettingPage = NULL;
        result = EIpsSetUiPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchScNumberEditorPageLD()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingLaunchScNumberEditorPageLD(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    // Convert baseitem to number editor
    CIpsSetUiItemsEditText* textEditor =
        static_cast<CIpsSetUiItemsEditText*>( &aBaseItem );
    TIpsSetUiPageResult result = EIpsSetUiPageResultOk;

    // Launch the editor and fill the settings if approved
    if ( iSettingPage->ExecuteLD( aUpdateMode ) )
        {
        iSettingPage = NULL;
        TIpsSetUiEventResult evaluation =
            EventItemEvaluateText( aBaseItem, iSettingText );

        if ( evaluation == EIpsSetUiPageEventResultApproved )
            {
            // Item ok
            AknTextUtils::ConvertDigitsTo( iSettingText, EDigitTypeWestern );
            textEditor->SetText( iSettingText );
            result = EIpsSetUiPageResultOk;
            }
        // Handle disapproved item
        else if ( evaluation == EIpsSetUiPageEventResultDisapproved )
            {
            result = EIpsSetUiPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EIpsSetUiPageResultCancelled;
            }
        }
    else
        {
        // Editor cancelled
        iSettingPage = NULL;
        result = EIpsSetUiPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchMultilinePageL()
// ----------------------------------------------------------------------------
//
TIpsSetUiPageResult CIpsSetUiBaseDialog::SettingLaunchMultilinePageL(
    CIpsSetUiItem& aBaseItem,
    TIpsSetUiUpdateMode& aUpdateMode )
    {
    FUNC_LOG;
    TIpsSetUiEventResult result =
        SettingLaunchMultilineEditorL( aBaseItem, aUpdateMode );

    switch ( result )
        {
        case EIpsSetUiPageEventResultApproved:
            return EIpsSetUiPageResultOk;

        case EIpsSetUiPageEventResultDisapproved:
            return EIpsSetUiPageResultEditing;

        default:
        case EIpsSetUiPageEventResultCancel:
            break;
        }

    return EIpsSetUiPageResultCancelled;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingLaunchMultilineEditorL()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiBaseDialog::SettingLaunchMultilineEditorL(
    CIpsSetUiItem& /* aBaseItem */,
    TIpsSetUiUpdateMode& /* aUpdateMode */ )
    {
    FUNC_LOG;
    // This should be called from the client side
    IPS_ASSERT_DEBUG( EFalse, KErrUnknown, EBaseDialog );

    return EIpsSetUiPageEventResultCancel;
    }

/******************************************************************************

    Event handling

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventArrayChangedL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::EventArrayChangedL(
    const TIpsSetUiArrayEvent aEvent )
    {
    FUNC_LOG;
    switch ( aEvent )
        {
        case EIpsSetUiArrayAdded:
            iListBox.HandleItemAdditionL();
            break;

        case EIpsSetUiArrayRemoved:
            iListBox.HandleItemRemovalL();
            break;

        // When array has changed, send event before updating listbox
        case EIpsSetUiArrayStackForward:
        case EIpsSetUiArrayStackBackward:
            EventPrepareSubArrayL(
               *GetItem( 0 ),
                aEvent == EIpsSetUiArrayStackForward ? ETrue : EFalse );

        //lint -fallthrough
        case EIpsSetUiArrayChanged:
            iListBox.HandleItemRemovalL();
            iListBox.HandleItemAdditionL();
            break;
        }

    return KErrNone;
    }


// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventSubArrayChangeL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::EventSubArrayChangeL(
    CIpsSetUiItem& /* aBaseItem */ )
    {
    FUNC_LOG;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventItemEditStartsL()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiBaseDialog::EventItemEditStartsL(
    CIpsSetUiItem& /* aBaseItem */ )
    {
    FUNC_LOG;
    return EIpsSetUiPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventItemEditEndsL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::EventItemEditEndsL(
    CIpsSetUiItem& /* aBaseItem */ )
    {
    FUNC_LOG;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventItemEvaluateRadioButton()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiBaseDialog::EventItemEvaluateRadioButton(
    const CIpsSetUiItem& aBaseItem,
    TInt& aNewValue )
    {
    FUNC_LOG;
    // This check fails always
    return iItemApprover->EvaluateValue( aBaseItem, aNewValue );;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventItemEvaluateValue()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiBaseDialog::EventItemEvaluateValue(
    const CIpsSetUiItem& aBaseItem,
    TInt& aNewValue )
    {
    FUNC_LOG;
    return iItemApprover->EvaluateValue( aBaseItem, aNewValue );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventItemEvaluateText()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiBaseDialog::EventItemEvaluateText(
    const CIpsSetUiItem& aBaseItem,
    TDes& aNewText )
    {
    FUNC_LOG;
    // This check fails always
    return iItemApprover->EvaluateText( aBaseItem, aNewText );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::EventCustomMdcaPoint()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiBaseDialog::EventCustomMdcaPoint(
    const TUid& /* aId */,
    TPtr& /* aString */ ) const
    {
    FUNC_LOG;
    return EFalse;
    }

/******************************************************************************

    Utilities

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingsStartedL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::SettingsStartedL()
    {
    FUNC_LOG;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingPageResource()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::SettingPageResource(
    const TIpsSetUiPageResource aSettingPage ) const
    {
    FUNC_LOG;
    return ( *iSettingPageResources )[aSettingPage];
    }



// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SetSettingPageResource()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SetSettingPageResource(
    const TIpsSetUiPageResource aSettingPage,
    const TInt aResourceId )
    {
    FUNC_LOG;
    TRAP_IGNORE( iSettingPageResources->PushResourceL( aSettingPage, aResourceId ) );
    }


// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::ShowCustomQueryL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::ShowCustomQueryL(
    const TInt aResourceStringId )
    {
    FUNC_LOG;
    iQueryDialog = CAknQueryDialog::NewL();
    TInt result = iQueryDialog->ExecuteLD( aResourceStringId );
    iQueryDialog = NULL;
    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CurrentItemIndex()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::CurrentItemIndex() const
    {
    FUNC_LOG;
    return iListBox.CurrentItemIndex();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CurrentItem()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseDialog::CurrentItem() const
    {
    FUNC_LOG;
    return GetItem( iListBox.CurrentItemIndex() );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::GetValueEditorFlags()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::GetValueEditorFlags(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    const TIpsSetUtilsFlags& itemFlags = aBaseItem.iItemFlags;

    // Get the flags for editor
    TInt flags =
        CAknIntegerSettingPage::EInvalidValueNotOffered;
    flags |= itemFlags.ValueForFlag(
        KIpsSetUiFlagForceMustFill,
        0,
        CAknIntegerSettingPage::EEmptyValueAllowed );

    flags |= itemFlags.ValueForFlag(
        KIpsSetUiFlagForceMustFill,
        CAknIntegerSettingPage::ENoInitialSelection,
        0 );

    flags |= itemFlags.ValueForFlag(
        KIpsSetUiFlagForceMustFill,
        CAknIntegerSettingPage::EPutCursorAtBeginning,
        0 );

    return flags;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::GetTextEditorFlags()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::GetTextEditorFlags(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    const TIpsSetUtilsFlags& itemFlags = aBaseItem.iItemFlags;

    TInt flags =
        CAknTextSettingPage::EZeroLengthNotOffered;
    flags |= itemFlags.ValueForFlag(
        KIpsSetUiFlagForceMustFill,
        0,
        CAknTextSettingPage::EZeroLengthAllowed );

    flags |= itemFlags.ValueForFlag(
        KIpsSetUiFlagForceMustFill,
        CAknTextSettingPage::ENoInitialSelection,
        0 );

    flags |= itemFlags.ValueForFlag(
        KIpsSetUiFlagForceMustFill,
        CAknTextSettingPage::EPutCursorAtBeginning,
        0 );

    return flags;
    }

/******************************************************************************

    Item initialization

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::InitAnyItem()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::InitAnyItem(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue,
    const TDesC& aText )
    {
    FUNC_LOG;
    TInt error = KErrNone;

    // Initialize any item
    switch ( aBaseItem.Type() )
        {
        case EIpsSetUiMenuArray:
            error = InitArrayLink( aBaseItem, aText );
            break;

        case EIpsSetUiRadioButtonArray:
            error = InitArrayRadioButton( aBaseItem, aValue, aText );
            break;

        case EIpsSetUiCheckBoxArray:
            error = InitArrayCheckBox( aBaseItem, aValue, aText );
            break;

        case EIpsSetUiItemText:
            error = InitItemTextEdit( aBaseItem, aText );
            break;

        case EIpsSetUiItemValue:
            error = InitItemNumberEdit( aBaseItem, aValue, aText );
            break;

        // Radiobutton and checkbox initialization is done during
        // the array initialization
        default:
            error = KErrNotFound;
            break;
        }

    // Set the initialized flag on for the item
    if ( !error )
        {
        aBaseItem.iItemFlags.SetFlag32( KIpsSetUiFlagIsInitialized );
        }
    else
        {
        aBaseItem.iItemFlags.ClearFlag32( KIpsSetUiFlagIsInitialized );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::InitArrayLink()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::InitArrayLink(
    CIpsSetUiItem& aBaseItem,
    const TDesC& aText )
    {
    FUNC_LOG;
    // Convert item
    CIpsSetUiItemLink* link = static_cast<CIpsSetUiItemLink*>( &aBaseItem );

    // Get the item count in array
    TInt items = KErrNotFound;
    if ( link->iItemLinkArray )
        {
        items = link->iItemLinkArray->Count();
        }

    // Clear checked flag from all items
    while ( --items >= 0 )
        {
        link->iItemFlags.ClearFlag32( KIpsSetUiFlagChecked );

        // Uncheck the buttons in the array
        CIpsSetUiItem* base = GetSubItem( aBaseItem, items );
        if ( base )
            {
            base->iItemFlags.ClearFlag32( KIpsSetUiFlagChecked );
            base = NULL;
            }
        }

    // Set the text if provided
    if ( link->iItemSettingText )
        {
        link->iItemSettingText->Copy( aText );
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::InitArrayRadioButton()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::InitArrayRadioButton(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue,
    const TDesC& aText )
    {
    FUNC_LOG;
    // Convert item
    CIpsSetUiItemLinkExt* radioArray =
        static_cast<CIpsSetUiItemLinkExt*>( &aBaseItem );
    CIpsSetUiItem* base = NULL;

    // Do normal link array initialization
    InitArrayLink( aBaseItem, KNullDesC );

    TInt result = KErrNotFound;

    // If the item has no value, check if any of the radiobuttons is
    // checked, and get the index from there
    if ( aValue == KErrNotFound && radioArray->iItemLinkArray )
        {
        TUid id = TUid::Uid( 0 );
        TInt index;
        FindCheckedRadiobutton( *radioArray, id, index );
        base = GetSubItem( *radioArray, id );

        radioArray->SetValue( index );
        }
    // For listbox item, simply check one of the buttons
    else if ( aValue >= 0 && radioArray->iItemLinkArray &&
         aValue < radioArray->iItemLinkArray->Count() )
        {
        // In initialization phase, the received index value is from
        // the main resource. Get the visible array's item index
        // and setting the item checked and getting the checked item
        TInt value = GetSubItemIndex( *radioArray, aValue, EFalse );
        CheckRadioButton( *radioArray, value );
        base = GetSubItem( *radioArray, value );

        // Set number of radiobox item to indicate the checked item
        radioArray->SetValue( value );
        result = KErrNone;
        }
    else
        {
        result = KErrNotFound;
        }

    // Make sure the setting text exist and copy text if provided
    if ( result == KErrNone && radioArray->iItemSettingText &&
        ( base && base->iItemLabel ) )
        {
        radioArray->iItemSettingText->Copy(
            aText.Length() ? aText : *base->iItemLabel );
        }

    base = NULL;
    radioArray = NULL;

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::InitArrayCheckBox()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::InitArrayCheckBox(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue,
    const TDesC& aText )
    {
    FUNC_LOG;
    // Convert item
    CIpsSetUiItemLinkExt* link =
        static_cast<CIpsSetUiItemLinkExt*>( &aBaseItem );
    CIpsSetUiItem* base;

    // Do normal link array initialization
    InitArrayLink( aBaseItem, KNullDesC );
    link->SetValue( aValue );

    // aValue can contain value KErrNotFound, which means 32 flags set.
    // This function trusts the user provides correct value in the field
    if ( link->iItemLinkArray )
        {
        // Get the item count in array
        TInt buttons = link->iItemLinkArray->Count();

        // Check the checkbox buttons
        while ( --buttons >= 0 )
            {
            base = link->iItemLinkArray->At( buttons );
            if ( ( aValue >> buttons ) & 0x01 )
                {
                base->iItemFlags.SetFlag32( KIpsSetUiFlagChecked );
                }
            else
                {
                base->iItemFlags.ClearFlag32( KIpsSetUiFlagChecked );
                }
            }
        }

    // Make sure the setting text exist and copy text if provided
    if ( link->iItemSettingText )
        {
        link->iItemSettingText->Copy( aText );
        }

    link = NULL;
    base = NULL;

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::InitItemTextEdit()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::InitItemTextEdit(
    CIpsSetUiItem& aBaseItem,
    const TDesC& aText )
    {
    FUNC_LOG;
    CIpsSetUiItemsEditText* textEditor =
        static_cast<CIpsSetUiItemsEditText*>( &aBaseItem );
    const TDesC& temp = aText.Left( textEditor->iItemMaxLength );
    aBaseItem.SetText( temp );

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::InitItemNumberEdit()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::InitItemNumberEdit(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue,
    const TDesC& aText )
    {
    FUNC_LOG;
    // Convert item
    CIpsSetUiItemValue* numberEditor =
        static_cast<CIpsSetUiItemValue*>( &aBaseItem );

    // Fill the existing fields
    numberEditor->SetValue( aValue );

    // Make sure all of the fields exists
    if ( numberEditor->iItemSettingText )
        {
         if ( !aText.Length() )
            {
            numberEditor->iItemSettingText->Num( aValue );
            }
         else
            {
            numberEditor->iItemSettingText->Copy( aText );
            }
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::InitItemUnidentified()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::InitItemUnidentified(
    CIpsSetUiItem& /* aBaseItem */,
    const TInt /* aValue */,
    const TDesC& /* aText */ )
    {
    FUNC_LOG;
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SettingsLastItemIndex()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::SettingsLastItemIndex(
    const TBool aRemove ) const
    {
    FUNC_LOG;
    // Store the last index to speed up fetching and removing
    TInt lastIndex = iSettingsLastItemIndex->Count() - 1;

    // Fetch the last item in the list and remove it
    TInt lastItem = iSettingsLastItemIndex->At( lastIndex );

    if ( aRemove )
        {
        iSettingsLastItemIndex->Delete( lastIndex );
        }

    // in case the array has been changed, set the new index in the boundaries
    // of the new array
    SetBetweenValues( lastItem, 0, MdcaCount() );

    return lastItem;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SetSettingsLastItemIndexL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SetSettingsLastItemIndexL(
    const TInt aLastItemIndex )
    {
    FUNC_LOG;
    iSettingsLastItemIndex->AppendL( aLastItemIndex );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SetListboxEmptyTextL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SetListboxEmptyTextL(
    const TDesC& aText )
    {
    FUNC_LOG;
    iListBox.View()->SetListEmptyTextL( aText );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CurrentSettingsPage()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseDialog::CurrentSettingsPage(
    const TBool aRemove )
    {
    FUNC_LOG;
    // Get the last item in the array
    TInt lastIndex = iSettingsPageArray->Count() - 1;

    // Get item
    CIpsSetUiItem* currentItem = NULL;

    if ( lastIndex >= 0 )
        {
        currentItem = iSettingsPageArray->At( lastIndex );
        }

    // Remove the item when required
    if ( aRemove )
        {
        iSettingsPageArray->Delete( lastIndex );
        }

    return currentItem;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::SetCurrentSettingsPageL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::SetCurrentSettingsPageL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    if ( iSettingsPageArray->Count() < KIpsSetUiMaxEditors )
        {
        // Set the new item as a last item
        iSettingsPageArray->AppendL( &aBaseItem );

        // Insert the backup item to array
        AddBackupItemL( aBaseItem );
        }
    else
        {
        // Too many editors, something is wrong
        IPS_ASSERT_DEBUG( EFalse, KErrOverflow, EBaseDialog );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CheckRadioButton()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::CheckRadioButton(
    CIpsSetUiItemLink& aRadioButtonArray,
    const TInt aRadioButtonIndex )
    {
    FUNC_LOG;
    // Make sure the array exists
    if ( aRadioButtonArray.iItemLinkArray )
        {
        CIpsSetUiBaseItemArray& array = *aRadioButtonArray.iItemLinkArray;

        // The loop must return the index of the item from the visible list
        // All items must be searched through
        // Each item has to be checked from their hidden properties
        const TInt buttons = array.Count();
        TInt index = buttons;

        // First, the item flags needs to be cleared
        while ( --index >= 0 )
            {
            array[index]->iItemFlags.ClearFlag32( KIpsSetUiFlagChecked );
            }

        // Get the item from the actual list
        CIpsSetUiItem* subItem =
            GetSubItem( aRadioButtonArray, aRadioButtonIndex );

        if ( subItem )
            {
            subItem->iItemFlags.SetFlag32( KIpsSetUiFlagChecked );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CheckRadioButton()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::CheckRadioButton(
    CIpsSetUiItemLink& aRadioButtonArray,
    const TUid& aId )
    {
    FUNC_LOG;
    // Make sure the array exists
    if ( aRadioButtonArray.iItemLinkArray )
        {
        CIpsSetUiItem* radioButton = NULL;

        TInt buttons = aRadioButtonArray.iItemLinkArray->Count();

        // Check the selected radiobutton
        while ( --buttons >= 0 )
            {
            radioButton = ( *aRadioButtonArray.iItemLinkArray )[buttons];
            if ( radioButton->iItemId == aId )
                {
                radioButton->iItemFlags.SetFlag32( KIpsSetUiFlagChecked );
                }
            else
                {
                radioButton->iItemFlags.ClearFlag32( KIpsSetUiFlagChecked );
                }
            }
        }

    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::FindCheckedRadiobutton()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::FindCheckedRadiobutton(
    const CIpsSetUiItemLinkExt& aArray,
    TUid& aId,
    TInt& aIndex )
    {
    FUNC_LOG;
    CIpsSetUiItem* radioButton = NULL;
    aId.iUid = KErrNotFound;
    aIndex = KErrNotFound;

    if ( aArray.iItemLinkArray )
        {
        // Find the checked radio button, as there can be only one!
        for ( TInt count = aArray.iItemLinkArray->Count()-1; count >= 0; --count)
            {
            radioButton = ( *aArray.iItemLinkArray )[count];

            // Search for the radiobuttons but ignore hidden ones,
            // even if it is checked
            if ( radioButton && !IsHidden( *radioButton ) )
                {
                ++aIndex;

                // If checked, return the value
                if ( radioButton->iItemFlags.Flag32( KIpsSetUiFlagChecked ) )
                    {
                    aId = radioButton->iItemId;
                    return KErrNone;
                    }
                }
            }
        }

    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::AddBackupItemL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::AddBackupItemL(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    switch ( aBaseItem.iItemType )
        {
        case EIpsSetUiItemMultiLine:
        case EIpsSetUiRadioButtonArray:
        case EIpsSetUiCheckBoxArray:
            BackupItemCreateRadioL( aBaseItem );
            break;

        case EIpsSetUiItemScValue:
        case EIpsSetUiItemText:
            BackupItemCreateTextL( aBaseItem );
            break;

        case EIpsSetUiItemValue:
            BackupItemCreateValueL( aBaseItem );
            break;

        case EIpsSetUiUndefined:
            BackupItemCreateUndefinedL( aBaseItem );
            break;

        default:
            // No need to backup, or unsupported
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::RemoveLastBackupItem()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::RemoveLastBackupItem()
    {
    FUNC_LOG;
    CIpsSetUiItem* base = GetLastBackupItem();
    iBackupArray->Delete( iBackupArray->Count() - 1 );

    if ( base )
        {
        // With link items do not delete array, since the pointer points
        // directly to original array. So set the pointer to null before
        // deleting the item.
        if ( base->HasLinkArray() )
            {
            static_cast<CIpsSetUiItemLink*>( &*base )->iItemLinkArray = NULL;
            }

        delete base;
        base = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::GetLastBackupItem()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseDialog::GetLastBackupItem()
    {
    FUNC_LOG;
    return iBackupArray->At( iBackupArray->Count() - 1 );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::RestoreFromBackup()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::RestoreFromBackup()
    {
    FUNC_LOG;
    // Restore
    for ( TInt count = iSettingsPageArray->Count()-1; count >= 0; count-- )
        {
        TInt lastPageNum = iSettingsPageArray->Count() - 1;
        TInt lastBackupNum = iBackupArray->Count() - 1;
        CIpsSetUiItem* currentPage = iSettingsPageArray->At( lastPageNum );
        CIpsSetUiItem* backupPage = iBackupArray->At( lastBackupNum );

        // Restore values
        const TDesC& text( ItemText( *backupPage ) );
        if ( text.Length() )
            {
            SetItemText( *currentPage, text );
            }

        currentPage->SetValue( backupPage->Value() );
        currentPage->SetText( backupPage->Text() );

        iSettingsPageArray->Delete( lastPageNum );
        RemoveLastBackupItem();
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CleanBackupArray()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::CleanBackupArray()
    {
    FUNC_LOG;
    for ( TInt count = iBackupArray->Count()-1; count >= 0; count-- )
        {
        RemoveLastBackupItem();
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::BackupItemCreateLinkL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::BackupItemCreateLinkL(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemLink* linkBackup = CIpsSetUiItemLink::NewL();
    CleanupStack::PushL( linkBackup );
    const CIpsSetUiItemLink* link =
        static_cast<const CIpsSetUiItemLink*>( &aBaseItem );
    *linkBackup = *link;

    iBackupArray->AppendL( linkBackup );
    CleanupStack::Pop( linkBackup );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::BackupItemCreateRadioL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::BackupItemCreateRadioL(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemLinkExt* radioBackup = CIpsSetUiItemLinkExt::NewL();
    CleanupStack::PushL( radioBackup );
    const CIpsSetUiItemLinkExt* radio =
        static_cast<const CIpsSetUiItemLinkExt*>( &aBaseItem );
    *radioBackup = *radio;

    iBackupArray->AppendL( radioBackup );
    CleanupStack::Pop( radioBackup );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::BackupItemCreateValueL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::BackupItemCreateValueL(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemValue* valueBackup = CIpsSetUiItemValue::NewL();
    CleanupStack::PushL( valueBackup );
    const CIpsSetUiItemValue* value =
        static_cast<const CIpsSetUiItemValue*>( &aBaseItem );
    *valueBackup = *value;

    iBackupArray->AppendL( valueBackup );
    CleanupStack::Pop( valueBackup );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::BackupItemCreateTextL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::BackupItemCreateTextL(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemsEditText* textBackup = CIpsSetUiItemsEditText::NewL();
    CleanupStack::PushL( textBackup );
    const CIpsSetUiItemsEditText* text =
        static_cast<const CIpsSetUiItemsEditText*>( &aBaseItem );
    *textBackup = *text;

    iBackupArray->AppendL( textBackup );
    CleanupStack::Pop( textBackup );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::BackupItemCreateUndefinedL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiBaseDialog::BackupItemCreateUndefinedL(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItem* base = CreateItemForBackupL( aBaseItem );

    iBackupArray->AppendL( base );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CreateItemForBackupL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiBaseDialog::CreateItemForBackupL(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemLinkExt* radioBackup = CIpsSetUiItemLinkExt::NewL();
    const CIpsSetUiItemLinkExt* radio =
        static_cast<const CIpsSetUiItemLinkExt*>( &aBaseItem );
    *radioBackup = *radio;

    return radioBackup;
    }

//<cmail>
// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CountInListBox()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiBaseDialog::CountInListBox() const
    {
    return iListBox.Model()->NumberOfItems();
    }
//</cmail>
//  End of File

