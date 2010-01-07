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
* Description: This file implements classes CPIMSettingItemListDlg, CFsEmailSettingsList.
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <barsread.h>
#include <StringLoader.h>
#include <aknlists.h>
#include <eikenv.h>
#include <AknIconArray.h>
#include <eikclbd.h>
#include <aknViewAppUi.h>
#include <eikappui.h>
#include <aknlistquerydialog.h>
#include <e32cmn.h>
#include <FreestyleEmailUi.rsg>
//<cmail>
#include "CFSMailClient.h"
#include "CFSMailBox.h"
#include <AknGlobalConfirmationQuery.h> // confirmation
#include <freestyleemailui.mbg> // icons
#include "ESMailSettingsPluginUids.hrh"
#include "ESMailSettingsPlugin.h"
//</cmail>
#include <aknnotewrappers.h> // for note
#include <AknDialog.h> // for settings dialog
#include <aknsettingitemlist.h> // for settings dialog

// 9.11.2009: Temporary flagging SYMBIAN_ENABLE_SPLIT_HEADERS
// Can be removed when header structure change is complete
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <eikctrlstatus.h> // EEikControlHasEars
#else
#include <uikon/eikctrlstatus.h> // EEikControlHasEars
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

// LOCAL INCLUDES
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiSettingsListView.h"
#include "FreestyleEmailUiSettingsList.h"
#include "FreestyleEmailUiSettings.hrh"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLauncherGridVisualiser.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiUtilities.h"
#include "FSEmail.pan"

// CONSTANTS


//** PIM selection dialog **

//----------------------------------------------------------------------------
// CPIMSettingsItem extends
//----------------------------------------------------------------------------
class CPIMSettingsItem : public CAknEnumeratedTextPopupSettingItem
    {
    public:
        void SetDefaultPIM( TInt aNewValue );
    };

void CPIMSettingsItem::SetDefaultPIM( TInt aNewValue )
    {
    FUNC_LOG;
    SetInternalValue( aNewValue );
    SetSelectedIndex( aNewValue );
    }

//----------------------------------------------------------------------------
// CSettingItemList class declaration
//----------------------------------------------------------------------------
class CSettingItemList : public CAknSettingItemList
    {
public:
    CSettingItemList();

    virtual CAknSettingItem* CreateSettingItemL(TInt aIdentifier);

    TInt                iItemId;
    CSettingItemList*   iSettingsList;
    };

//----------------------------------------------------------------------------
// constructor CSettingItemList
//----------------------------------------------------------------------------
CSettingItemList::CSettingItemList()
    {
    FUNC_LOG;
    }
//----------------------------------------------------------------------------
// CreateSettingItemL
//----------------------------------------------------------------------------
CAknSettingItem* CSettingItemList::CreateSettingItemL( TInt aIdentifier )
    {
    FUNC_LOG;
    CAknSettingItem* settingitem = NULL;

    switch (aIdentifier)
        {
        case 0:
            {
            settingitem = new (ELeave) CAknEnumeratedTextPopupSettingItem( aIdentifier, iItemId );
            }
        break;
        }

    return settingitem;
    }

//----------------------------------------------------------------------------
// CPIMSettingItemListDlg Class declaration
//----------------------------------------------------------------------------
class CPIMSettingItemListDlg : public CAknDialog
    {
    public:
        CPIMSettingItemListDlg( CFSMailClient& aMailClient );
        ~CPIMSettingItemListDlg();
        virtual SEikControlInfo CreateCustomControlL(TInt aControlType);
        virtual CPIMSettingItemListDlg::TFormControlTypes ConvertCustomControlTypeToBaseControlType(TInt aControlType) const;

        virtual void PostLayoutDynInitL();
        void AddPIMSettingItemsL( TInt aIndex );

        virtual TBool OkToExitL( TInt aButtonId );

    protected:
        CSettingItemList*   iSettingsList;
        CFSMailClient&      iMailClient;
        RArray<TUid>        iPluginUids;
    };

//----------------------------------------------------------------------------
// CPIMSettingItemListDlg
//----------------------------------------------------------------------------
CPIMSettingItemListDlg::CPIMSettingItemListDlg( CFSMailClient& aMailClient)
: iMailClient(aMailClient)
    {
    FUNC_LOG;
    }

//----------------------------------------------------------------------------
// CPIMSettingItemListDlg::~CPIMSettingItemListDlg
//----------------------------------------------------------------------------
CPIMSettingItemListDlg::~CPIMSettingItemListDlg( )
    {
    FUNC_LOG;
    iPluginUids.Close();
    }

//----------------------------------------------------------------------------
// CreateCustomControlL
//----------------------------------------------------------------------------
SEikControlInfo CPIMSettingItemListDlg::CreateCustomControlL(TInt /*aControlType*/)
    {
    FUNC_LOG;
    iSettingsList = new (ELeave) CSettingItemList;

    SEikControlInfo controlInfo;
    controlInfo.iFlags = EEikControlHasEars;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iControl = iSettingsList;
    STATIC_CAST(CSettingItemList*, controlInfo.iControl)->SetContainerWindowL(*this);

    return controlInfo;
    }

//----------------------------------------------------------------------------
// ConvertCustomControlTypeToBaseControlType
//----------------------------------------------------------------------------
CPIMSettingItemListDlg::TFormControlTypes CPIMSettingItemListDlg::ConvertCustomControlTypeToBaseControlType(TInt /*aControlType*/) const
    {
    FUNC_LOG;
    return EPopfieldDerived;
    }

//----------------------------------------------------------------------------
// PostLayoutDynInitL
// This safely loads the dialog with the data
//----------------------------------------------------------------------------
void CPIMSettingItemListDlg::PostLayoutDynInitL()
    {
    FUNC_LOG;
    // there are only one item in list
    AddPIMSettingItemsL( 0 );
    }

//----------------------------------------------------------------------------
// AddPIMSettingItemsL
//
//----------------------------------------------------------------------------
void CPIMSettingItemListDlg::AddPIMSettingItemsL( TInt aIndex )
    {
    FUNC_LOG;
    CPIMSettingsItem* item = static_cast<CPIMSettingsItem*>(
            (*iSettingsList->SettingItemArray())[aIndex] );
    CArrayPtr<CAknEnumeratedText>* texts = item->EnumeratedTextArray();
    texts->ResetAndDestroy();

    // Read mailboxes ****************************
    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    TFSMailMsgId plugin;
    iMailClient.ListMailBoxes( plugin, mailBoxes );

    TUid PIMpluginId;
    TInt mailBoxesCount = mailBoxes.Count();
    for ( TInt i = 0; i < mailBoxesCount; ++i )
        {
        // get mailbox name and insert it to listBox
        CFSMailBox* mailBox = mailBoxes[i];

        // Insert mailbox to dialog list if can handle PIM sync
        if ( mailBox->GetSettingsUid() != TUid::Null() )
            {
            TUid pluginUid = mailBox->GetSettingsUid();
            CESMailSettingsPlugin* settingsPlugin =
                CESMailSettingsPlugin::NewL( pluginUid );
            CleanupStack::PushL(settingsPlugin);
            HBufC* itemTxt =
                settingsPlugin->LocalizedProtocolName().AllocLC();
            TBool canHandlePIMSync = settingsPlugin->CanHandlePIMSync();

            if ( canHandlePIMSync && PIMpluginId != pluginUid )
                {
                PIMpluginId = pluginUid;
                iPluginUids.Append( pluginUid );
                // takes ownership of itemTxt
                texts->AppendL(
                    new ( ELeave ) CAknEnumeratedText( i, itemTxt ) );
                // set as default item if selected for PIM sync
                if ( settingsPlugin->IsSelectedForPIMSync() )
                    {
                    item->SetDefaultPIM( i );
                    }
                CleanupStack::Pop( itemTxt );
                }
            else
                {
                CleanupStack::PopAndDestroy( itemTxt );
                }
            CleanupStack::PopAndDestroy();//settingsPlugin
            }
        }
    item->UpdateListBoxTextL();

    CleanupStack::PopAndDestroy( &mailBoxes );
    }

//----------------------------------------------------------------------------
// OkToExitL
//
//----------------------------------------------------------------------------
TBool CPIMSettingItemListDlg::OkToExitL( TInt aButtonId )
    {
    FUNC_LOG;
    if ( aButtonId == EFSEmailUiPIMDialogBack )
        return ETrue;

    if ( aButtonId == EFSEmailUiPIMDialogChange )
        {
        const TInt current = iSettingsList->ListBox()->CurrentItemIndex();
        iSettingsList->EditItemL( current, ETrue );
        iSettingsList->StoreSettingsL();

        for ( TInt i=0; i < iPluginUids.Count(); ++i )
            {
            CESMailSettingsPlugin* settingsPlugin = CESMailSettingsPlugin::NewL( iPluginUids[i] );
            if ( iSettingsList->iItemId == i )
                {
                settingsPlugin->SelectForPIMSync( ETrue );
                }
            else
                {
                settingsPlugin->SelectForPIMSync( EFalse );
                }
            delete settingsPlugin;
            settingsPlugin = NULL;
            }
        iPluginUids.Close();
        }

    return EFalse;
    }


//**** CFsEmailSettingsList ****

// ---------------------------------------------------------------------------
// c++ Constructor
//
// First phase of Symbian two-phase construction. Should not
// contain any code that could leave.
// ---------------------------------------------------------------------------
//
CFsEmailSettingsList::CFsEmailSettingsList(CFreestyleEmailUiAppUi& aAppUi, CFSMailClient& aMailClient, CFsEmailSettingsListView& aView )
:iAppUi(aAppUi), iMailClient(aMailClient), iView( aView )
    {
    FUNC_LOG;

    iListBox = NULL;
    iCurrentSubView = EFsEmailSettingsMainListView;
    iPIMListActivation = EFalse;
    iPIMServiceSettingsSelection = EFalse;
    iPIMSyncMailboxIndex = -1;
    iPIMSyncCount = 0;
    iSelectedSubListIndex = 0;
    }

// ---------------------------------------------------------------------------
// c++ destructor
//
// Destroy child controls
// ---------------------------------------------------------------------------
//
CFsEmailSettingsList::~CFsEmailSettingsList()
    {
    FUNC_LOG;

    if ( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }

    }

// ---------------------------------------------------------------------------
// Construct the control (first phase).
//
// @param aRect bounding rectangle
// @param aParent owning parent, or NULL
// @param aAppUi pointer to AppUi
// @param aMailClient reference to freestyle mail client
// ---------------------------------------------------------------------------
//
CFsEmailSettingsList* CFsEmailSettingsList::NewL(
        const TRect& aRect,
        const CCoeControl* aParent,
        CFreestyleEmailUiAppUi& aAppUi,
        CFSMailClient& aMailClient,
        CFsEmailSettingsListView& aView )
    {
    FUNC_LOG;

    CFsEmailSettingsList* self = CFsEmailSettingsList::NewLC(
            aRect,
            aParent,
            aAppUi,
            aMailClient,
            aView );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Construct the control (first phase).
//
// @param aRect bounding rectangle
// @param aParent owning parent, or NULL
// @param aAppUi pointer to AppUi
// @param aMailClient reference to freestyle mail client
// ---------------------------------------------------------------------------
//
CFsEmailSettingsList* CFsEmailSettingsList::NewLC(
        const TRect& aRect,
        const CCoeControl* aParent,
        CFreestyleEmailUiAppUi& aAppUi,
        CFSMailClient& aMailClient,
        CFsEmailSettingsListView& aView )
    {
    FUNC_LOG;

    CFsEmailSettingsList* self = new ( ELeave ) CFsEmailSettingsList(aAppUi, aMailClient, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent);


    return self;
    }

// ---------------------------------------------------------------------------
// Construct the control (first phase).
//
// @param aRect bounding rectangle
// @param aParent owning parent, or NULL
// @param aAppUi pointer to AppUi
// @param aMailClient reference to freestyle mail client
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::ConstructL(
        const TRect& aRect,
        const CCoeControl* aParent)
    {
    FUNC_LOG;

    if ( aParent == NULL )
        {
        CreateWindowL();
        }
    else
        {
        SetContainerWindowL( *aParent );
        }

    iFocusControl = NULL;
    InitializeControlsL();
    SetRect( aRect );
    ActivateL();

    }

// ---------------------------------------------------------------------------
// Return the number of controls in the container (override)
// @return count
// ---------------------------------------------------------------------------
//
TInt CFsEmailSettingsList::CountComponentControls() const
    {
    FUNC_LOG;
    return ( int ) ELastControl;
    }

// ---------------------------------------------------------------------------
// Get the control with the given index
// @param aIndex Control index [0...n) (limited by #CountComponentControls)
// @return Pointer to control
// ---------------------------------------------------------------------------
//
CCoeControl* CFsEmailSettingsList::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;

    switch ( aIndex )
        {
        case EListBox:
            return iListBox;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// SizeChanged
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SizeChanged()
    {
    FUNC_LOG;

    CCoeControl::SizeChanged();
    LayoutControls();

    }


// ---------------------------------------------------------------------------
// Layout components as specified in the UI Designer
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::LayoutControls()
    {
    FUNC_LOG;

    iListBox->SetExtent( TPoint( 0, 0 ), iListBox->MinimumSize() );

    }

// ---------------------------------------------------------------------------
// Handle events from listbox.
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::HandleListBoxEventL( CEikListBox *aListBox, TListBoxEvent aEventType )
    {
    FUNC_LOG;

    if ( aListBox )
        {
        if ( aEventType == EEventItemSingleClicked ||
        	 aEventType == EEventEnterKeyPressed )
            {
            HandleUserSelectionsL();
            }
        }

    }

// ---------------------------------------------------------------------------
// HandleUserSelectionsL
//
// check where we are in navigation and decide where to go
// based on selection commands
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::HandleUserSelectionsL()
    {
    FUNC_LOG;

    TInt index = iListBox->CurrentItemIndex();
    TInt count = Count();

    // If we are at the main level
    if ( iCurrentSubView == EFsEmailSettingsMainListView )
        {
        // Open global settings
        if ( index == 0 )
            {
            // activate global settings list
            SetSelectedMainListIndex( index );
            iAppUi.EnterFsEmailViewL( GlobalSettingsViewId );
            }
        // Open selected mailbox settings
        else if ( index > 0 && index <= iMailboxCount  )
            {
            // set mailbox info
            CFSMailBox* mailbox = GetMailboxByIndexLC( index-1 );
            iSelectedAccountInfo = mailbox->GetId();
            iSelectedPluginSettings = mailbox->GetSettingsUid();
            CleanupStack::PopAndDestroy( mailbox );
            // don't active pimlist subview, set flag to false
            iPIMListActivation = EFalse;
            if ( iSelectedPluginSettings == TUid::Null() )
                {
                #ifdef _DEBUG
                CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
                informationNote->ExecuteLD(_L("No plugin Uid found"));
                #endif // _DEBUG
                }
            else
                {
                SetSelectedMainListIndex( index );
                CreatePluginSubviewListL( iSelectedPluginSettings );
                }
            }
        // Create PIM settings as a second level items
        else if ( index > iMailboxCount )
            {
            CFSMailBox* mailbox = GetMailboxByIndexLC( iPIMSyncMailboxIndex );
            iSelectedAccountInfo = mailbox->GetId();
            iSelectedPluginSettings = mailbox->GetSettingsUid();
            CleanupStack::PopAndDestroy( mailbox );
            iPIMListActivation = ETrue;
            SetSelectedMainListIndex( index );
            CreatePluginPIMListL( iSelectedPluginSettings );
            // Update the title pane, needed for touch support
            HBufC* text = CreateTitlePaneTextLC();
            iAppUi.SetTitlePaneTextL( *text );
            CleanupStack::PopAndDestroy( text );
            }
        else // index < 0; this should never happen
            {
            __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
            }
        }

    // In second level list, activate selected plugin settings view or
    // open PIM service dialog if first item selected in PIM list
    else
        {
        if ( iPIMServiceSettingsSelection && index == 0 )
            {
            // open PIM service settings selection dialog
            OpenPIMServiceDialogL();
            }
        else
            {
            ActivateMailSettingsPluginSubViewL( index );
            }
        }

    }

// ---------------------------------------------------------------------------
// Handle key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CFsEmailSettingsList::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    FUNC_LOG;

    // Handle shortcuts
    if ( aType == EEventKey )
        {
        TInt commandId = iAppUi.ShortcutBinding().CommandForShortcutKey( aKeyEvent, CFSEmailUiShortcutBinding::EContextSettings );
        if ( commandId >= 0 )
            {
            iAppUi.View( SettingsViewId )->HandleCommandL( commandId );
            }
        }

    if ( aKeyEvent.iCode == EKeyLeftArrow
        || aKeyEvent.iCode == EKeyRightArrow )
        {
        // Listbox takes all events even if it doesn't use them
        return EKeyWasNotConsumed;
        }

    if ( iFocusControl != NULL
        && iFocusControl->OfferKeyEventL( aKeyEvent, aType ) == EKeyWasConsumed )
        {
        return EKeyWasConsumed;
        }

    return CCoeControl::OfferKeyEventL( aKeyEvent, aType );
    }


// ---------------------------------------------------------------------------
// Initialize each control upon creation.
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::InitializeControlsL()
    {
    FUNC_LOG;

    // Create list box control
    iListBox = new ( ELeave ) CSettingsListType;
    iListBox->SetContainerWindowL( *this );
        {
        TResourceReader reader;
        iEikonEnv->CreateResourceReaderLC( reader, R_FS_EMAIL_SETTINGS_LIST_LIST_BOX );
        iListBox->ConstructFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // reader internal state
        }
    // the listbox owns the items in the list and will free them
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // setup the icon array so graphics-style boxes work
    SetupListBoxIconsL();

    // Create scrollbar
    iListBox->CreateScrollBarFrameL();
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );

    iListBox->SetFocus( ETrue );
    iFocusControl = iListBox;

    // event listener this class
    iListBox->SetListBoxObserver( this );

    }


// ---------------------------------------------------------------------------
// Handle global resource changes, such as scalable UI or skin events (override)
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;

    CCoeControl::HandleResourceChange( aType );
    SetRect( iAvkonViewAppUi->View( SettingsViewId )->ClientRect() );

    }

// ---------------------------------------------------------------------------
// Draw container contents.
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;

    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );

    }


// ---------------------------------------------------------------------------
// Count
//
// Retuns number of item in list
// ---------------------------------------------------------------------------
//
TInt CFsEmailSettingsList::Count()
    {
    FUNC_LOG;

    CDesCArray* itemArray = static_cast<CDesCArray*>( iListBox->Model()->ItemTextArray() );
    TInt count( itemArray->Count() );

    return count;
    }

// ---------------------------------------------------------------------------
// Get the listbox pointer.
// ---------------------------------------------------------------------------
//
CSettingsListType* CFsEmailSettingsList::ListBox()
    {
    FUNC_LOG;
    return iListBox;
    }

// ---------------------------------------------------------------------------
// Get the listbox pointer.
// ---------------------------------------------------------------------------
//
TBool CFsEmailSettingsList::PIMSyncItemVisible()
    {
    FUNC_LOG;
    return ( iPIMSyncMailboxIndex == -1 ) ? EFalse : ETrue;
    }

// ---------------------------------------------------------------------------
// Create a list box item for plain text
// ---------------------------------------------------------------------------
//
HBufC* CFsEmailSettingsList::CreateListBoxItemLC(
        const TDesC& aMainText )
    {
    FUNC_LOG;

    _LIT ( KStringHeader, "\t%S" );
    HBufC* temp = HBufC::NewLC( aMainText.Length() + KStringHeader().Length() );
    TPtr tempPtr = temp->Des();
    tempPtr.Format( KStringHeader(), &aMainText );

    return temp;
    }

// ---------------------------------------------------------------------------
// Create a list box item for text and icon
// ---------------------------------------------------------------------------
//
HBufC* CFsEmailSettingsList::CreateListBoxItemLC(
        const TDesC& aMainText,
        TInt aIconNum )
    {
    FUNC_LOG;

    _LIT ( KStringHeader, "\t%S\t%d" );
    HBufC* temp = HBufC::NewLC( aMainText.Length() + KStringHeader().Length() );
    TPtr tempPtr = temp->Des();
    tempPtr.Format( KStringHeader(), &aMainText, aIconNum );

    return temp;
    }

// ---------------------------------------------------------------------------
// Insert given item in a list
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::AppendItemL( TDesC& aListItem )
    {
    FUNC_LOG;

    CDesCArray* itemArray = static_cast<CDesCArray*>(
            iListBox->Model()->ItemTextArray() );
    itemArray->AppendL( aListItem );
    }

// ---------------------------------------------------------------------------
// Set up the list's icon array.
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SetupListBoxIconsL()
    {
    FUNC_LOG;

    CArrayPtr<CGulIcon>* icons = new (ELeave) CArrayPtrFlat<CGulIcon>( 3 );
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    }

// ---------------------------------------------------------------------------
// ProtocolBrandingIconL
//
// Sets mailbox protocol branding icon to list box by given id
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::ProtocolBrandingIconL( TFSMailMsgId aMailBoxId )
    {
    FUNC_LOG;

    MFSMailBrandManager& brandManager = iAppUi.GetMailClient()->GetBrandManagerL();
    CGulIcon* mailBoxIcon(0);
    TRAPD( err, mailBoxIcon = brandManager.GetGraphicL( EFSMailboxIcon,  aMailBoxId ) );

    if ( err != KErrNone || !mailBoxIcon )
        {
        CFbsBitmap* bitmap(0);
        CFbsBitmap* mask(0);

        TFileName iconFileName;
        TFsEmailUiUtility::GetFullIconFileNameL( iconFileName );

// <cmail> icons changed
        AknIconUtils::CreateIconL( bitmap , mask , iconFileName,
                EMbmFreestyleemailuiQgn_indi_cmail_drop_email_account,
                EMbmFreestyleemailuiQgn_indi_cmail_drop_email_account_mask );
// </cmail>
        CleanupStack::PushL( bitmap );
        CleanupStack::PushL( mask );

        // create default mailbox icon from bitmaps
        mailBoxIcon = CGulIcon::NewL( bitmap, mask );
        CleanupStack::Pop( mask );
        CleanupStack::Pop( bitmap );
        }

    CArrayPtr<CGulIcon>* icons = iListBox->ItemDrawer()->ColumnData()->IconArray();
    CleanupStack::PushL( mailBoxIcon );
    icons->AppendL( mailBoxIcon );
    CleanupStack::Pop( mailBoxIcon );
    }


// ---------------------------------------------------------------------------
// ClearListL
//
// Removes all items from list
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::ClearListL()
    {
    FUNC_LOG;

    CDesCArray* itemArray = static_cast<CDesCArray*>(
            iListBox->Model()->ItemTextArray() );
    itemArray->Reset();
    iListBox->ItemDrawer()->ColumnData()->IconArray()->ResetAndDestroy();
    iListBox->HandleItemRemovalL();
    }

// ---------------------------------------------------------------------------
// GetMailboxByIndexLC
//
// @return CFSMailBox pointer, ownership is transferred
// ---------------------------------------------------------------------------
//
CFSMailBox* CFsEmailSettingsList::GetMailboxByIndexLC( const TInt& aIndex ) const
    {
    FUNC_LOG;

    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    mailBoxes.Reset();
    TFSMailMsgId plugin;
    iMailClient.ListMailBoxes( plugin, mailBoxes );
    CFSMailBox* mailbox = NULL;
    if ( aIndex > -1 && aIndex < mailBoxes.Count() )
        {
        mailbox = iMailClient.GetMailBoxByUidL( mailBoxes[aIndex]->GetId() );
        }
    User::LeaveIfNull( mailbox );
    CleanupStack::PopAndDestroy( &mailBoxes );
    CleanupStack::PushL( mailbox );
    return mailbox;
    }

// ---------------------------------------------------------------------------
// GetMailBoxByIdL
//
// @return CFSMailBox pointer, ownership is transferred
// ---------------------------------------------------------------------------
//
CFSMailBox* CFsEmailSettingsList::GetMailBoxByIdL( const TUint& aMailboxId ) const
    {
    FUNC_LOG;

    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    mailBoxes.Reset();
    TFSMailMsgId plugin;
    iMailClient.ListMailBoxes( plugin, mailBoxes );
    CFSMailBox* mailbox = NULL;
    for ( TInt i = 0; i < mailBoxes.Count(); ++i )
        {
         if ( mailBoxes[i]->GetId().Id() == aMailboxId )
             {
             TRAP_IGNORE( mailbox = iMailClient.GetMailBoxByUidL( mailBoxes[i]->GetId() ) );
             break;
             }
        }
    CleanupStack::PopAndDestroy( &mailBoxes );
    return mailbox;
    }

// ---------------------------------------------------------------------------
// GetMailboxCountL
//
// @return TInt amount of mailboxes
// ---------------------------------------------------------------------------
//
TInt CFsEmailSettingsList::GetMailboxCountL() const
    {
    FUNC_LOG;

    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    mailBoxes.Reset();
    TFSMailMsgId plugin;
    iMailClient.ListMailBoxes( plugin, mailBoxes );
    TInt count = mailBoxes.Count();
    CleanupStack::PopAndDestroy( &mailBoxes );

    return count;
    }

// ---------------------------------------------------------------------------
// CreateAccountListL
// Creates mailbox list for settings
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::CreateAccountListL()
    {
    FUNC_LOG;

    LoadAccountsToListL();

    // we are on first level of the list box
    iCurrentSubView = EFsEmailSettingsMainListView;

    }

// ---------------------------------------------------------------------------
// Get2ndLevelListIndicator
// Indicates the state of list. Are the "second level" of items displayed
// @return TBool
// ---------------------------------------------------------------------------
//
TBool CFsEmailSettingsList::Get2ndLevelListIndicator()
    {
    FUNC_LOG;
    return ( iCurrentSubView != EFsEmailSettingsMainListView );
    }

// ---------------------------------------------------------------------------
// SetPluginSettingsUid
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SetPluginSettingsUid( const TUid& aPluginId )
    {
    FUNC_LOG;
    iSelectedPluginSettings = aPluginId;
    }

// ---------------------------------------------------------------------------
// GetSettingsPluginL
// Creates needed settings plugin instance and register it to view stack
// Ownership is not transferred
// @return CESMailSettingsPlugin*
// ---------------------------------------------------------------------------
//
CESMailSettingsPlugin* CFsEmailSettingsList::GetSettingsPluginL( TUid aUid )
    {
    FUNC_LOG;
    CESMailSettingsPlugin* settingsPlugin =
        static_cast<CESMailSettingsPlugin*>( iAppUi.View( aUid ) );

    if ( settingsPlugin == NULL )
        {
        TInt err( KErrNone );

        TRAP( err, settingsPlugin = CESMailSettingsPlugin::NewL( aUid ););
        if ( err == KErrNoMemory )
            {
            User::Leave( err );
            }
        else if ( err == KErrNone )
            {
            CleanupStack::PushL(settingsPlugin);
            iAppUi.AddViewL( settingsPlugin );
            CleanupStack::Pop(settingsPlugin);
            }
        else
            {
            }
        }

    return settingsPlugin;
    }


// ---------------------------------------------------------------------------
// SetSelectedSubListIndex
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SetSelectedSubListIndex( TInt aIndex )
    {
    FUNC_LOG;
    if ( aIndex >= 0 )
        {
        iSelectedSubListIndex = aIndex;
        }
    else
        {
        iSelectedSubListIndex = 0;
        }
    }

// ---------------------------------------------------------------------------
// SetSelectedMainListIndex
// Saves mailbox list index to parent when settings plugin view is activated
// because this container is destroyed during the view switch
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SetSelectedMainListIndex( TInt aIndex )
    {
    FUNC_LOG;
    CFsEmailSettingsListView* parent =
        static_cast<CFsEmailSettingsListView*>( iAppUi.View( SettingsViewId ) );

    if ( aIndex >= 0 )
        {
        parent->SetSelectedMainListIndex( aIndex );
        }
    }

// ---------------------------------------------------------------------------
// GetSelectedMainListIndex
// Returns latest mailbox list item index which is saved in parent view
// @return void
// ---------------------------------------------------------------------------
//
TInt CFsEmailSettingsList::GetSelectedMainListIndex() const
    {
    FUNC_LOG;
    CFsEmailSettingsListView* parent =
        static_cast<CFsEmailSettingsListView*>( iAppUi.View( SettingsViewId ) );

    TInt index = parent->GetSelectedMainListIndex();

    if ( index < 0 )
        {
        index = 0;
        }

    return index;
    }

// ---------------------------------------------------------------------------
// LoadAccountsToListL
// Loads global settings and account names to list dynamically
// from fs mail client
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::LoadAccountsToListL()
    {
    FUNC_LOG;

    // Remove current items
    ClearListL();
    // Insert Global settings as a first item
    HBufC* localizedText = StringLoader::LoadLC(
            R_FS_EMAIL_SETTINGS_LIST_GLOBAL_SETTINGS_TXT );
    HBufC* listItemBuffer = CreateListBoxItemLC( *localizedText );
    AppendItemL( *listItemBuffer );
    CleanupStack::PopAndDestroy( 2, localizedText ); // localizedText listItemBuffer

    // get mailboxes from every plugin
    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    mailBoxes.Reset();
    TFSMailMsgId plugin;
    iMailClient.ListMailBoxes( plugin, mailBoxes );

    TUid PIMpluginUid = TUid::Null();
    // Set initial pim sync values
    iPIMSyncCount = 0;
    iPIMSyncMailboxIndex = KErrNotFound;

    iMailboxCount = mailBoxes.Count();


    for ( TInt i=0 ; i < iMailboxCount ; ++i )
        {
        CFSMailBox* mailBox = mailBoxes[i];

        // get protocol icon index
        ProtocolBrandingIconL( mailBox->GetId() );

        // get mailbox name and insert it to listBox
        TPtrC mbName = GetMailBoxNameL( *mailBox );
        listItemBuffer = CreateListBoxItemLC( mbName, i );
        AppendItemL( *listItemBuffer );
        CleanupStack::PopAndDestroy( listItemBuffer ); // listItemBuffer

        // Does mailbox have settings plugin
        if ( mailBox->GetSettingsUid() != TUid::Null() )
            {
            // load settings ECOM plugin
            TUid pluginUid = mailBox->GetSettingsUid();
            CESMailSettingsPlugin* tempPlugin;
            tempPlugin = GetSettingsPluginL( pluginUid ); // ownership not transferred

            if ( tempPlugin != NULL )
                {
                // check if mailboxes can handle PIM sync
                TBool canHandlePIMSync = tempPlugin->CanHandlePIMSync();
                if ( canHandlePIMSync && PIMpluginUid != pluginUid )
                    {
                    PIMpluginUid = pluginUid;
                    ++iPIMSyncCount;
                    if ( tempPlugin->IsSelectedForPIMSync() )
                        {
                        iPIMSyncMailboxIndex = i;
                        }
                    // if protocol supports PIM sync, but is not selected yet
                    if ( iPIMSyncCount > 0 && iPIMSyncMailboxIndex == -1 )
                        {
                        iPIMSyncMailboxIndex = i;
                        }
                    }
                }
            }
        else
            {
            }
        }
    CleanupStack::PopAndDestroy( &mailBoxes );

    // if one of the mailboxes can handle PIM sync
    // display PIM settings item in list.
    if ( iPIMSyncCount > 0 )
        {
        HBufC* text2 = StringLoader::LoadLC( R_FS_EMAIL_SETTINGS_LIST_PIM_SETTINGS_TXT );
        listItemBuffer = CreateListBoxItemLC( *text2 );
        AppendItemL( *listItemBuffer );
        CleanupStack::PopAndDestroy( 2, text2 ); // text2 listItemBuffer
        }

    // update list
    iListBox->HandleItemAdditionL();
    if ( GetSelectedMainListIndex() > iListBox->BottomItemIndex() )
        {
        iListBox->SetCurrentItemIndex( 0 ) ;
        }
    else if ( GetSelectedMainListIndex() < 0 )
        {
        iListBox->SetCurrentItemIndex( 0 ) ;
        }
    else
        {
        iListBox->SetCurrentItemIndex( GetSelectedMainListIndex() );
        }
    }


// ---------------------------------------------------------------------------
// DisplayCreateMailboxNoteIfNeededL
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::DisplayCreateMailboxNoteIfNeededL()
    {
    FUNC_LOG;
    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    mailBoxes.Reset();
    TFSMailMsgId plugin;
    iMailClient.ListMailBoxes( plugin, mailBoxes );

    // display "create mailbox" query if no mailboxes defined
    TInt mailboxCount = mailBoxes.Count();
    CleanupStack::PopAndDestroy( &mailBoxes );

    // Display query if count == zero
    if ( mailboxCount == 0 )
        {
        if ( TFsEmailUiUtility::ShowConfirmationQueryL( R_FS_EMAIL_SETTINGS_ADD_MAILBOX_TXT ) )
              {
              AddAccountL();
              }
        }
    }


// ---------------------------------------------------------------------------
// CreatePluginSubviewListL
// Fill list with captions returned by settings ECOM plugin
// @param aPluginId implementation TUid of the settings ECom plugin
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::CreatePluginSubviewListL( const TUid& aPluginId )
    {
    FUNC_LOG;

    // Remove items
    ClearListL();

    // change list content to account specific settings
    HBufC *listString = NULL;

    // get pointer to plugin settings that user selected
    iSelectedPluginSettings = aPluginId;
    CESMailSettingsPlugin* settingsPlugin = GetSettingsPluginL(
        iSelectedPluginSettings );

    // check do we want to load long or short caption depending on screen size
    TBool longCaption = EFalse;
    if ( Rect().Width() > 240 )
        {
        longCaption = ETrue;
        }

    TInt subViewCount( 0 );
    if ( settingsPlugin != NULL )
        {
        subViewCount = settingsPlugin->MailSettingsSubviewCount();
        }
    else
        {
        }

    // load setting view names to list
    TInt index = 0;
    if ( subViewCount > 0 )
        {
        while ( index < subViewCount )
            {
            TPtrC subViewCaption = settingsPlugin->MailSettingsSubviewCaption(
                    iSelectedAccountInfo, index, longCaption );
            listString = CreateListBoxItemLC( subViewCaption );
            AppendItemL( *listString );
            CleanupStack::PopAndDestroy( listString );
            ++index;
            }
        iListBox->HandleItemAdditionL();
        iListBox->SetCurrentItemIndex( iSelectedSubListIndex );
        }
    else
        {
        // activate POP/IMAP view
        ActivateMailSettingsPluginSubViewL( -1 );
        }
    iCurrentSubView = EFsEmailSettingsMailboxView;
    }

// ---------------------------------------------------------------------------
// CreatePluginPIMListL
// Fill list with PIM captions returned by settings ECOM plugin
// This list is displayed only with Intellisync or Mfe account
// @param aPluginId implementation TUid of the settings ECom plugin
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::CreatePluginPIMListL( const TUid& aPluginId )
    {
    FUNC_LOG;

    // Remove items
    ClearListL();

    // change list content to account specific settings
    TInt index = 0;
    HBufC *listString = NULL;

    // get pointer to plugin settings that user selected
    iSelectedPluginSettings = aPluginId;
    CESMailSettingsPlugin* settingsPlugin = GetSettingsPluginL(
        iSelectedPluginSettings );

    // When we return from the plugin PIM settings we don't know if there is
    // one or two mailboxes that contains PIM settings.
    // So we veed to check that here:
    if(iPIMSyncCount == 0)
        {
        TUid PIMpluginUid = TUid::Null();
        // get mailboxes from every plugin
        RPointerArray<CFSMailBox> mailBoxes;
        CleanupResetAndDestroyClosePushL( mailBoxes );
        mailBoxes.Reset();
        TFSMailMsgId plugin;
        iMailClient.ListMailBoxes( plugin, mailBoxes );

        for ( TInt i = 0; i < mailBoxes.Count(); ++i )
                {
                // get mailbox name and insert it to listBox
                CFSMailBox* mailBox = mailBoxes[i];
                // Does mailbox have settings plugin
                if ( mailBox->GetSettingsUid() != TUid::Null() )
                    {
                    // load settings ECOM plugin
                    TUid pluginUid = mailBox->GetSettingsUid();
                    CESMailSettingsPlugin* tempPlugin;
                    tempPlugin = GetSettingsPluginL( pluginUid ); // ownership not transferred

                    if ( tempPlugin != NULL )
                        {
                        // check if mailboxes can handle PIM sync
                        TBool canHandlePIMSync = tempPlugin->CanHandlePIMSync();
                        if ( canHandlePIMSync && PIMpluginUid != pluginUid )
                            {
                            ++iPIMSyncCount;
                            }
                        }
                    }
                }
        CleanupStack::PopAndDestroy( &mailBoxes );
        }

    // If there are more than one plugin that is capable of syncing personal
    // information, then user needs to select which is used
    if ( iPIMSyncCount > 1 )
        {
        HBufC* text = StringLoader::LoadLC( R_FS_EMAIL_SETTINGS_PIM_SERVICE_TXT );
        listString = CreateListBoxItemLC( *text );
        AppendItemL( *listString );
        CleanupStack::PopAndDestroy( 2, text ); // text, listString
        iPIMServiceSettingsSelection = ETrue;
        }


    // check do we want to load long or short caption depending on screen size
    TBool longCaption = EFalse;
    if ( Rect().Width() > 240 )
        {
        longCaption = ETrue;
        }

    TInt subViewCount(0);
    if ( settingsPlugin )
        {
        subViewCount = settingsPlugin->PIMSettingsSubviewCount();
        }

    // load setting view names to list
    while ( subViewCount > index )
        {
        TPtrC subViewCaption = settingsPlugin->PIMSettingsSubviewCaption(
                iSelectedAccountInfo, index, longCaption );
        listString = CreateListBoxItemLC( subViewCaption );
        AppendItemL( *listString );
        CleanupStack::PopAndDestroy( listString ); // listString
        ++index;
        }

    iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndex( iSelectedSubListIndex );
    iCurrentSubView = EFsEmailSettingsPimView;
    }


// ---------------------------------------------------------------------------
// SetDefaultPIMAccountL
// Set account specific variables point to a selected PIM account
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SetDefaultPIMAccountL()
    {
    FUNC_LOG;
    CFSMailBox* mailbox;

    for ( TInt i = 0 ; i < iMailboxCount ; ++i )
        {
        mailbox = GetMailboxByIndexLC(i);
        TUid pluginUid = mailbox->GetSettingsUid();
        CESMailSettingsPlugin* tempPlugin;
        tempPlugin = GetSettingsPluginL( pluginUid );
        if ( tempPlugin != NULL
             && tempPlugin->CanHandlePIMSync()
             && tempPlugin->IsSelectedForPIMSync() )
            {
            SetSelectedAccountInfo( mailbox->GetId() );
            iSelectedPluginSettings = pluginUid;
            }
        CleanupStack::PopAndDestroy( mailbox );
        }
    }

// ---------------------------------------------------------------------------
// GetMailBoxNameL
// Get text pointer with the (possibly branded) mailbox name. Mailbox or
// framework owns the pointed data.
// ---------------------------------------------------------------------------
//
TPtrC CFsEmailSettingsList::GetMailBoxNameL( const CFSMailBox& aMailBox ) const
    {
    FUNC_LOG;
    // Branded mailbox name is nowadays set in new mailbox event
    // handling, so we don't need to use brand manager here anymore.
    const TDesC& name = aMailBox.GetName();

    TPtrC ret;
    ret.Set( name );

    return ret;
    }

// ---------------------------------------------------------------------------
// ActivateMailSettingsPluginSubViewL
// Activate selected mail settings plugin subview
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::ActivateMailSettingsPluginSubViewL( TInt aSubViewId )
    {
    FUNC_LOG;

    TUid messageId;
    CESMailSettingsPlugin::TSubViewActivationData activationData;
    TInt subViewId = iListBox->CurrentItemIndex();
    activationData.iAccount = iSelectedAccountInfo;
    activationData.iSubviewId = aSubViewId;
    activationData.iLaunchedOutsideFSEmail = EFalse;

    if ( iPIMListActivation )
        {
        messageId = TUid::Uid( CESMailSettingsPlugin::EActivatePIMSettingsSubview );
        // use account selected for PIM sync
        SetDefaultPIMAccountL();
        iPIMListActivation = EFalse;
        // pass information to the plugin side that we were in the personal information management subview
        activationData.iPimAccount = ETrue;
        }
    else
        {
        messageId = TUid::Uid( CESMailSettingsPlugin::EActivateMailSettingsSubview );
        activationData.iPimAccount = EFalse;
        }

    const TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData>
        pluginMessagePkg( activationData );
    TInt errNo( KErrNone );
    TRAP( errNo, iAppUi.EnterPluginSettingsViewL(
            iSelectedPluginSettings, messageId, pluginMessagePkg ); );
    if ( errNo != KErrNone )
        {
        }
    }

// ---------------------------------------------------------------------------
// SetSelectedAccountInfo
// Set given account info as a member data
// @param aAccountInfo
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SetSelectedAccountInfo( const TFSMailMsgId& aAccountInfo )
    {
    FUNC_LOG;

    iSelectedAccountInfo = aAccountInfo;

    }

// ---------------------------------------------------------------------------
// AddAccount
// Handles new account creation with wizard
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::AddAccountL()
    {
    FUNC_LOG;

    iAppUi.LaunchWizardL();

    }

// ---------------------------------------------------------------------------
// RemoveAccount
// Handle account removal from fs mail client
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::RemoveAccountL()
    {
    FUNC_LOG;

    // Make sure that FSMailServer is running, so that the mailbox is removed
    // also from MCE. Let's do it already here, so that the server has some
    // time to launch itself before the actual mailbox deletion happens.
    TFsEmailUiUtility::EnsureFsMailServerIsRunning( iEikonEnv->WsSession() );

    // if item index is 0 then we are on global settings item
    // this shouldn't be happening, but just in case
    iDeletedIndex = iListBox->CurrentItemIndex();
    if ( iDeletedIndex == 0 )
        {
        return;
        }

    // get mailbox
    CFSMailBox* mailBox = GetMailboxByIndexLC(iDeletedIndex-1);
    CAknQueryDialog* queryNote = new ( ELeave ) CAknQueryDialog();
    CleanupStack::PushL( queryNote );

    // load remove mailbox text from resource
    HBufC* question = StringLoader::LoadLC(
           R_FS_EMAIL_SETTINGS_REMOVE_MAILBOX_TXT,
           mailBox->GetName() );

    queryNote->SetPromptL( *question );
    CleanupStack::PopAndDestroy( question );
    CleanupStack::Pop( queryNote );

    // launch note and check answer
    if ( queryNote->ExecuteLD( R_FSEMAIL_QUERY_DIALOG ) )
        {
        // check if protocol supports delete
        if ( mailBox->HasCapability( EFSMBoxCapaCanBeDeleted ) )
            {
            // start wait note
            iWaitDialog = new (ELeave) CAknWaitDialog(
                    (REINTERPRET_CAST(CEikDialog**, &iWaitDialog)), EFalse );
            iWaitDialog->PrepareLC( R_FS_WAIT_NOTE_REMOVING_MAILBOX );
            iWaitDialog->SetCallback( this );
            iWaitDialog->RunLD();
            // Set email indicator off.
            TFsEmailUiUtility::ToggleEmailIconL(EFalse);
            // delete mailbox and wait event (RequestResponseL)
            iDeleteMailboxId = iMailClient.DeleteMailBoxByUidL( mailBox->GetId(), *this );
            }
        }
    CleanupStack::PopAndDestroy(); // mailBox
    }


// ---------------------------------------------------------------------------
// RequestResponseL
// handles request state from mail client
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::RequestResponseL( TFSProgress /*aEvent*/, TInt /*aRequestId*/ )
    {
    FUNC_LOG;
    if( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        // refresh settings mailboxlist
        if ( GetSelectedMainListIndex() > 0 )
            {
            SetSelectedMainListIndex( GetSelectedMainListIndex()-1 );
            }
        else
            {
            SetSelectedMainListIndex( 0 );
            }

        LoadAccountsToListL();
        // refresh launcher grid
        CFSEmailUiLauncherGridVisualiser* grid =
            static_cast<CFSEmailUiLauncherGridVisualiser*>( iAppUi.View( AppGridId ) );
        grid->RefreshLauncherViewL();
        }
    iWaitDialog = NULL;
    }

// ---------------------------------------------------------------------------
// DialogDismissedL
// called when wait has ended
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::DialogDismissedL( TInt /*aButtonId */ )
    {
    FUNC_LOG;
    iWaitDialog = NULL;
    }

// ---------------------------------------------------------------------------
// OpenPIMServiceDialog
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::OpenPIMServiceDialogL()
    {
    FUNC_LOG;
    CPIMSettingItemListDlg* dlg = new (ELeave) CPIMSettingItemListDlg( iMailClient );
    dlg->ExecuteLD( R_FS_PIMSETTINGITEMLIST_DIALOG );
    }

// ---------------------------------------------------------------------------
// SetPimListActivation
// @return void
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::SetPimListActivation(TBool aActivation)
    {
    FUNC_LOG;
    iPIMListActivation = aActivation;
    }

// ---------------------------------------------------------------------------
// Creates and passes ownership of title pane text appropriate for the current
// view
// ---------------------------------------------------------------------------
//
HBufC* CFsEmailSettingsList::CreateTitlePaneTextLC() const
    {
    FUNC_LOG;
    HBufC* text = NULL;

    switch ( iCurrentSubView )
        {
        case EFsEmailSettingsMailboxView:
            {
            // Use mailbox name as title pane text in the mailbox view
            TInt curMailBoxIdx = GetSelectedMainListIndex()-1;
            if ( curMailBoxIdx < 0 )
                {
                text = StringLoader::LoadLC( R_FS_EMAIL_SETTINGS_TITLE );
                break;
                }
            CFSMailBox* mailBox = GetMailboxByIndexLC( curMailBoxIdx );
            TPtrC mailBoxName = GetMailBoxNameL( *mailBox );
            text = mailBoxName.AllocL();
            CleanupStack::PopAndDestroy( mailBox );
            CleanupStack::PushL( text );
            }
            break;
        case EFsEmailSettingsPimView:
            {
            text = StringLoader::LoadLC( R_FS_EMAIL_PIM_SETTINGS_TITLE );
            }
            break;
        case EFsEmailSettingsMainListView:
        default:
            {
            text = StringLoader::LoadLC( R_FS_EMAIL_SETTINGS_TITLE );
            }
            break;
        }

    return text;
    }

// ---------------------------------------------------------------------------
// Move list focus to top
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::GoToTop()
    {
    FUNC_LOG;
    TInt count = iListBox->Model()->NumberOfItems();
    if ( count )
        {
        iListBox->SetCurrentItemIndexAndDraw( 0 );
        }
    }

// ---------------------------------------------------------------------------
// Move list focus to bottom
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::GoToBottom()
    {
    FUNC_LOG;
    TInt count = iListBox->Model()->NumberOfItems();
    if ( count )
        {
        iListBox->SetCurrentItemIndexAndDraw( count-1 );
        }
    }

// ---------------------------------------------------------------------------
// Move selector one page up
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::PageUp()
    {
    FUNC_LOG;
    TInt count = iListBox->Model()->NumberOfItems();
    if ( count )
        {
        TInt itemsOnPage = ListBox()->BottomItemIndex() - ListBox()->TopItemIndex();
        TInt currentIdx = ListBox()->CurrentItemIndex();
        TInt newIdx = Max( 0, currentIdx - itemsOnPage );
        ListBox()->SetCurrentItemIndexAndDraw( newIdx );
        }
    }

// ---------------------------------------------------------------------------
// Move selector one page down
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsList::PageDown()
    {
    FUNC_LOG;
    TInt count = ListBox()->Model()->NumberOfItems();
    if ( count )
        {
        TInt itemsOnPage = ListBox()->BottomItemIndex() - ListBox()->TopItemIndex();
        TInt currentIdx = ListBox()->CurrentItemIndex();
        TInt newIdx = Min( count-1, currentIdx + itemsOnPage );
        ListBox()->SetCurrentItemIndexAndDraw( newIdx );
        }
    }

