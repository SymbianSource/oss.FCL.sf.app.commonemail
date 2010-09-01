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
*  Description: This file implements class CIpsSetUi.
*
*/


#include "emailtrace.h"
#include <eikfrlb.h>                // CEikFormattedCellListBox
#include <aknappui.h>               // CAknAppUi
#include <AknsDrawUtils.h>
#include <akntitle.h>               // CAknTitlePane
#include <hlplch.h>                 // HlpLauncher
#include <eikrted.h>                // CEikRichTextEditor
#include <ipssossettings.rsg>
#include <data_caging_path_literals.hrh>
#include <AlwaysOnlineManagerClient.h>

#include "ipssetui.h"
#include "ipssetuidialog.h"
#include "ipssetutilsdisconnectlogic.h"

// becuase of RD_IPS_AO_PLUGIN flag, can be removed
// when flag is removed
#include "ipsplgsosbaseplugin.hrh"

const TInt KIpsSetOptionsMenuItemCount = 4;
// Resource file name and path, drive letter need to be parsed run time
_LIT( KIpsSetUiResourceFile, "\\resource\\IpsSosSettings.rsc" );

// ============================ LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUi::CIpsSetUi
// ----------------------------------------------------------------------------
//
CIpsSetUi::CIpsSetUi( const TMsvId aMailboxId, TBool aFolderSettingView ) //<cmail>
    :
    iFlags( 0x00 ),
    iListBox( NULL ),
    iSettings( NULL ),
    iResourceLoader( *iCoeEnv ),
    iMailboxId( aMailboxId )
    , iShowFolderSettings(aFolderSettingView), iIgnoreOneBackKey(ETrue) //<cmail>
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUi* CIpsSetUi::NewL( const TMsvId aMailboxId, TBool aFolderSettingView ) //<cmail>
    {
    FUNC_LOG;
    CIpsSetUi* self = NewLC( aMailboxId, aFolderSettingView ); //<cmail>
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUi* CIpsSetUi::NewLC( const TMsvId aMailboxId, TBool aFolderSettingView ) //<cmail>
    {
    FUNC_LOG;
    CIpsSetUi* self = new ( ELeave ) CIpsSetUi( aMailboxId, aFolderSettingView ); //<cmail>
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::~CIpsSetUi()
// ----------------------------------------------------------------------------
//
CIpsSetUi::~CIpsSetUi()
    {
    FUNC_LOG;
    iResourceLoader.Close();
    delete iSession;
    delete iSettings;
    iSettings = NULL;
    iListBox = NULL;
    iTextEditor = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUi::ConstructL()
    {
    FUNC_LOG;
    iResourceMenuBar = R_IPS_SET_MENUBAR;

    // Call the base ConstructL
    CAknDialog::ConstructL( iResourceMenuBar );
    LoadResourceL();
    iSession = CMsvSession::OpenSyncL( *this );
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::OkToExitL
// ----------------------------------------------------------------------------
//
TBool CIpsSetUi::OkToExitL( TInt aButtonId )
    {
    FUNC_LOG;
    TBool allowQuit = EFalse;

    // Exit not issued, forward the call to command processor
    if ( !( iFlags & EIpsSetUiShouldExit ) &&
         !( iFlags & EIpsSetUiShouldClose ) )
        {
        iSettings->ProcessCommandL( aButtonId );
        }
    else
        {
        // Should quit
        allowQuit = iSettings->OkToExitL( aButtonId );

        // Clear the flag after usage
        iFlags &= ~EIpsSetUiQueryClose;

        // If quit is allowed, store the settings, otherwise clear the flag
        if ( allowQuit && iFlags & EIpsSetUiAllowSave )
            {
            iFlags &= ~EIpsSetUiAllowSave;
            
            DisconnectIfOnlineL( EFalse );
            // Failing to store the settings should cause panic, to prevent
            // the settings to stop into unwanted errors
            TRAP_IGNORE( iSettings->StoreSettingsToAccountL() );

            // switch ao back on
// <cmail> RD_IPS_AO_PLUGIN flag removed
            RAlwaysOnlineClientSession aosession; 
            TInt err = aosession.Connect();
            if ( err == KErrNone )
                {
                TPckgBuf<TMsvId> mboxBuf( iMailboxId );
                TRAP( err, aosession.RelayCommandL( 
                        EServerAPIEmailTurnOn, 
                       mboxBuf ) );
                }
            aosession.Close();
// </cmail>
            }
        else
            {
            // Clear the flags
            iFlags &= ~EIpsSetUiShouldExit;
            iFlags &= ~EIpsSetUiShouldClose;
            }

        // Exit, now
        if ( allowQuit )
            {
            allowQuit = CAknDialog::OkToExitL( EAknSoftkeyCancel );
            }
        }

    return allowQuit;
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::HandleResourceChange
// ----------------------------------------------------------------------------
//
void CIpsSetUi::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CAknDialog::HandleResourceChange( aType );

    if ( iTextEditor && aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect rect = Rect();
        iTextEditor->SetRect( rect );
        iTextEditor->HandleResourceChange( aType );
        iListBox->SetRect( rect );
        iListBox->HandleResourceChange( aType );
        }
    if ( aType == KAknsMessageSkinChange )
        {
        SetFontAndSkin();
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::SetFontAndSkin
// ----------------------------------------------------------------------------
//
void CIpsSetUi::SetFontAndSkin()
    {
    FUNC_LOG;
    if ( iTextEditor )
        {
        MAknsControlContext* context =
            AknsDrawUtils::ControlContext( iListBox );
        TRAP_IGNORE(
            iTextEditor->SetSkinBackgroundControlContextL( context ) );

        const CFont* font = ControlEnv()->NormalFont();
        TFontSpec fontSpec = font->FontSpecInTwips();

        TCharFormat charFormat;
        TCharFormatMask formatMask;
        charFormat.iFontSpec = fontSpec;

        formatMask.SetAttrib( EAttFontTypeface );
        formatMask.SetAttrib( EAttFontHeight );
        formatMask.SetAttrib( EAttFontPosture );
        formatMask.SetAttrib( EAttFontStrokeWeight );

        TRgb textColor;
        TInt error = AknsUtils::GetCachedColor(
    	    AknsUtils::SkinInstance(),
            textColor,
            KAknsIIDQsnTextColors,
            EAknsCIQsnTextColorsCG7 );

        if ( error != KErrNone )
    	    {
    	    textColor = AKN_LAF_COLOR_STATIC( 215 );
    	    }

        charFormat.iFontPresentation.iTextColor = textColor;
        formatMask.SetAttrib( EAttColor );

        CCharFormatLayer* charFormatLayer = NULL;

        TRAP( error,
            charFormatLayer = CCharFormatLayer::NewL( charFormat, formatMask )
        );

        if ( error )
            {
            delete charFormatLayer;
            }
        else
            {
            iTextEditor->SetCharFormatLayer( charFormatLayer );
            }
        }

    }

// ----------------------------------------------------------------------------
// CIpsSetUi::PreLayoutDynInitL
// ----------------------------------------------------------------------------
//
void CIpsSetUi::PreLayoutDynInitL()
    {
    FUNC_LOG;
    // Get the pointer to titlepane
    CEikStatusPane* sp =
        static_cast<CAknAppUi*>(
            iEikonEnv->EikAppUi() )->StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*>(
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    CEikButtonGroupContainer& cba = ButtonGroupContainer();

    // check connect state before entering settings
    DisconnectIfOnlineL();
    
// <cmail> RD_IPS_AO_PLUGIN flag removed
    // prevent ao operations during settings edit
    RAlwaysOnlineClientSession aosession;
    TInt err = aosession.Connect();
    if ( err == KErrNone )
        {
        TPckgBuf<TMsvId> mboxBuf( iMailboxId );
        TRAP_IGNORE( aosession.RelayCommandL( 
                EServerAPIEmailTurnOff, 
                mboxBuf ) );
        }
    aosession.Close();
// </cmail>
    
    // Prepare the listbox
    iListBox = static_cast<CEikFormattedCellListBox*>(
        Control( EIpsSetUiIdListBox ) );

    iSettings = CIpsSetUiDialogCtrl::NewL(
        *this, *iListBox, *title, cba, *iSession, iFlags );
    
    //<cmail>
    //When settings are launched from folderlist view, we need to go directly to 'what to sync' view
    if(iShowFolderSettings)
        {
        if(iSettings->CountInListBox() > 1)
            {
            iListBox->SetCurrentItemIndex(1);
            iSettings->ProcessCommandL( EAknSoftkeySelect );
            if(iSettings->CountInListBox() > 2)
                {
                iListBox->SetCurrentItemIndex(2);
                }
            }
        }
    //</cmail>
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CIpsSetUi::ProcessCommandL( TInt aCommandId )
    {
    FUNC_LOG;
    // Forward the commands
    CAknDialog::ProcessCommandL( aCommandId );

    // Forward command either to wizard or setting dialog
    iSettings->ProcessCommandL( aCommandId );
    }

void CIpsSetUi::LaunchMenuL()
	{
	if ( iFlags & EIpsSetUiOpenOptionsMenu )
		{
		iFlags &= ~EIpsSetUiOpenOptionsMenu;
		TIpsSetUiSettingsType type = iSettings->CurrentItem()->iItemType;
		//sub-menu
		if ( type == EIpsSetUiMenuArray )
			{
			iFlags |= EIpsSetUiPositionSubMenu;
			iFlags &= ~EIpsSetUiSettingsMenuItem;
			}
		//settings item
		else if( type == EIpsSetUiRadioButtonArray || 
				 type == EIpsSetUiItemText ||
				 type == EIpsSetUiUndefined || 
				 type == EIpsSetUiCheckBoxArray )
			{
			iFlags |= EIpsSetUiSettingsMenuItem;
			iFlags &= ~EIpsSetUiPositionSubMenu;
			}
		else
			{
			iFlags &= ~EIpsSetUiPositionSubMenu;
			iFlags &= ~EIpsSetUiSettingsMenuItem;
			}
		DisplayMenuL();
		}
	}

// ----------------------------------------------------------------------------
// CIpsSetUi::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CIpsSetUi::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;

    //<cmail>
    // When settings are launched from folder list view, we need to show directly 'what to sync' view
    // this logic is used when coming back from that view
    if( (aType == EEventKeyUp) && (aKeyEvent.iScanCode == EStdKeyDevice1) 
            && (iShowFolderSettings) && iIgnoreOneBackKey)
        {
        iFlags |= EIpsSetUiShouldClose;
        }
    iIgnoreOneBackKey = ETrue;
    //</cmail>
        
    // Offer the key event to existing mailbox setting handler
    TKeyResponse ret = iSettings->OfferKeyEventL( aKeyEvent, aType );

    // Improve menu handling here!!!

    // Show the menu, if asked
    LaunchMenuL();
    
    // Check if exit has been issued
    if ( iFlags & EIpsSetUiShouldExit ||
         iFlags & EIpsSetUiShouldClose )
        {
        DoQuitL();
        
        // Quit attempted, so consume the key
        ret = EKeyWasConsumed;
        }

    if ( ret != EKeyWasConsumed )
        {
        ret = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::DynInitMenuPaneL()
// ----------------------------------------------------------------------------
//
void CIpsSetUi::DynInitMenuPaneL(
    TInt /* aResourceId */,
    CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    if ( aMenuPane->NumberOfItemsInPane() == KIpsSetOptionsMenuItemCount )
        {
        aMenuPane->SetItemDimmed(
            EAknSoftkeyOpen, iFlags & EIpsSetUiSettingsMenuItem );
        aMenuPane->SetItemDimmed(
            EAknSoftkeySelect, iFlags & EIpsSetUiPositionSubMenu );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::DoQuitL()
// ----------------------------------------------------------------------------
//
void CIpsSetUi::DoQuitL()
    {
    FUNC_LOG;
    // Start the exit process, the query is shown during TryExitL call
    if ( iFlags & EIpsSetUiShouldExit )
        {
        TryExitL( EAknSoftkeyExit );
        }
    else
        {
        TryExitL( EAknSoftkeyCancel );
        }
    }


// ----------------------------------------------------------------------------
// CIpsSetUi::GetHelpContext()
// ----------------------------------------------------------------------------
//
void CIpsSetUi::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    FUNC_LOG;
    iSettings->GetHelpContext( aContext );
    }

// ----------------------------------------------------------------------------
// CIpsSetUi::LaunchHelpL()
// ----------------------------------------------------------------------------
//
void CIpsSetUi::LaunchHelpL()
    {
    FUNC_LOG;
    CCoeAppUi* appUi = static_cast<CCoeAppUi*>( ControlEnv()->AppUi() );
    CArrayFix<TCoeHelpContext>* helpContext = appUi->AppHelpContextL();
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
    }

void CIpsSetUi::HandleSessionEventL(
        TMsvSessionEvent /*aEvent*/, 
        TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    FUNC_LOG;
    // handle general server events if needed
    // EMsvCloseSession
    // EMsvServerReady
    // EMsvServerFailedToStart
    // EMsvCorruptedIndexRebuilt
    // EMsvServerTerminated
    }

// ---------------------------------------------------------------------------
// CIpsSetUi::LoadResourceL()
// ---------------------------------------------------------------------------
//
void CIpsSetUi::LoadResourceL()
    {
    FUNC_LOG;

    // Load resource file from the same drive where this dll is loaded from
    TFileName dllFileName;
    Dll::FileName( dllFileName );

    TParse parse;
    parse.Set( KIpsSetUiResourceFile, &dllFileName, NULL );
    TFileName fileName( parse.FullName() );
    
    // Attempt to load the resource
    iResourceLoader.OpenL( fileName );
    }
    
void CIpsSetUi::DisconnectIfOnlineL( TBool aDoShowQuery )
    {
    FUNC_LOG;
    CIpsSetUtilsDisconnectLogic* dLogic = CIpsSetUtilsDisconnectLogic::NewL( 
            *iSession, iMailboxId );
    CleanupStack::PushL( dLogic );
    // check if mailbox is online and disconnect
    TInt result = KErrNone;
    // <cmail>
    TInt error = KErrNone;
    TRAP( error, result = dLogic->RunDisconnectLogicL( aDoShowQuery ) );
    __ASSERT_DEBUG( error == KErrNone, 
            User::Panic( KIpsSetGenPanicLit, error ));
    // </cmail>

    if ( result == KErrCancel )
        {
        User::Leave( KErrCancel );
        }
    CleanupStack::PopAndDestroy( dLogic );
    }

// ---------------------------------------------------------------------------
// CIpsSetUi::MailboxId()
// ---------------------------------------------------------------------------
//
TMsvId CIpsSetUi::MailboxId()
    {
    FUNC_LOG;
    return iMailboxId;
    }

//<cmail>
// ---------------------------------------------------------------------------
// CIpsSetUi::SetIgnoreOneBackKey()
// ---------------------------------------------------------------------------
//
void CIpsSetUi::SetIgnoreOneBackKey(TBool aIgnoreBackKey)
    {
    iIgnoreOneBackKey = aIgnoreBackKey;
    }
//</cmail>

    

//  End of File



