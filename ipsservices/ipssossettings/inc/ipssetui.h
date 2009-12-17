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
* Description:  Decleares main dialog class.
 *
*/


#ifndef IPSSETUI_H
#define IPSSETUI_H


#include <AknForm.h> // CAknDialog
#include <msvapi.h>
#include <ConeResLoader.h>  // RConeResourceLoader

/**
 * Flags to control the UI.
 */
enum TIpsSetUiFlags
    {
    // Indicates that the settings should be closed
    EIpsSetUiShouldClose    = 0x01,
    // Indicates if the settings should be exitted
    EIpsSetUiShouldExit     = 0x02,
    // Indicates the need of query in exit
    EIpsSetUiQueryClose     = 0x04,
    // Indicates the need of opening settings menu
    EIpsSetUiOpenOptionsMenu = 0x08,
    // Indicates if the submenu item is currently selected
    EIpsSetUiPositionSubMenu = 0x10,
    // Indicates, if the main settings view is open
    EIpsSetUiMainSettingsMenu = 0x20,
    // Allow saving
    EIpsSetUiAllowSave        = 0x40,
    // Indicates if settings item is currently selected
    //<cmail>    
    EIpsSetUiSettingsMenuItem = 0x80,
    // Indicates if account data should be hidden
    EIpsSetUiHideAccountData = 0x100
    //</cmail>    
    };

class CEikFormattedCellListBox;
class CIpsSetDataManager;
class CAknTitlePane;
class CEikEdwin;
class CEikRichTextEditor;
class CIpsSetData;
class CIpsSetUiDialogCtrl;



/**
 * Main dialog class.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class CIpsSetUi :
    public CAknDialog,
    public MMsvSessionObserver
    {
public:  // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CIpsSetUi();

    /**
     * 2 phase construction.
     *
     * @return Ui object with client ownership.
     */
    static CIpsSetUi* NewL( const TMsvId aMailboxId, TBool aFolderSettingView = EFalse ); //<cmail>

    /**
     * 2 phase construction with stack push.
     *
     * @return Ui object with client ownership.
     */
    static CIpsSetUi* NewLC( const TMsvId aMailboxId, TBool aFolderSettingView = EFalse  ); //<cmail>

// New Functions

    /**
     * Launches help dialog.
     */
    void LaunchHelpL();
    
    /**
     * Launches menu
     */
    void LaunchMenuL();

    /**
     * Returns the id of the mailbox
     *
     * @return Current mailbox Id
     */
    TMsvId MailboxId();

// From base class CAknDialog

    /**
     * Processes command
     *
     * @param aCommandId Command id
     */
    void ProcessCommandL( TInt aCommandId );

    /**
     * Used here to handle exit event from subsettings
     *
     * @param aKeyEvent event id
     * @param aType event type
     * @return Information of event handling.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * Handles invoked commands.
     *
     * @param aCommand Invoked command.
     */
    void HandleCommandL( TInt aCommand );

    /**
     * Handles resource changes, such as screen switch.
     *
     * @param aType Change type.
     */
    virtual void HandleResourceChange( TInt aType );

    /**
     * Gets outta here.
     */
    void DoQuitL();

    //<cmail>    
    /**
     * Setter for iIgnoreOneBackKey. See iIgnoreOneBackKey for more info.
     *  
     *  @param aIgnoreBackKey iIgnoreOneBackKey value
     */
    void SetIgnoreOneBackKey(TBool aIgnoreBackKey);
    //</cmail>    
    
protected: // Functions from base classes

    //From CEikDialog

    /**
     * Check wheter its ok to exit or not.
     *
     * @param aButtonId, button id
     * @return ETrue, when exit is ok.
     */
    virtual TBool OkToExitL( TInt aButtonId );

    /**
     * Dialog initialization.
     */
    virtual void PreLayoutDynInitL();

    /**
     * Initializes menu pane.
     *
     * @param aResourceId Type of id.
     * @param aMenuPane Menupane object
     */
    virtual void DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane );

    /**
     * Retrieves current help context.
     *
     * @param aContext Return parameter for context.
     */
    virtual void GetHelpContext(
        TCoeHelpContext& aContext ) const;

    // From MMsvSessionObserver
    
    /*
     * 
     */
    virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent, 
            TAny* aArg1, TAny* aArg2, TAny* aArg3 );

private: // Constructors

    /**
     * C++ default constructor.
     *
     * @param aMailboxd Currently edited mailbox
     */
    CIpsSetUi( const TMsvId aMailboxId, TBool aFolderSettingView = EFalse  ); //<cmail>

    /**
     * 2nd phase of construction.
     */
    void ConstructL();

private: // New functions

    /**
     * Check to see if we are editing an existing
     * or a new account
     *
     * @return ETrue if editing, EFalse if creating new
     */
    inline TBool EditingAnAccount() const;

    /**
     * Change font color in startup and when skin is changed.
     */
    void SetFontAndSkin();

    /**
     * Opens the resource file.
     */
    void LoadResourceL();
    /*
     * Check mailbox online state or ask user for 
     * disconnection, leave in case of error or if user
     * choose not to disconnect
     */
    void DisconnectIfOnlineL( TBool aDoShowQuery = ETrue );

private:    // Data

    /**
     * Flags of settings
     */
    TUint64                 iFlags;

    /**
     * The listbox with skin support
     * Owned.
     */
    CEikFormattedCellListBox* iListBox;

    /**
     * Richtext editor.
     * Owned.
     */
    CEikRichTextEditor*     iTextEditor;

    /**
     * Setting dialog controller.
     * Owned.
     */
    CIpsSetUiDialogCtrl*    iSettings;

    /**
     * Used when loading settings
     */
    CMsvSession*            iSession;

    /**
     * Menubar resource.
     */
    TInt                    iResourceMenuBar;

    /**
     * Object to load the resources
     */
    RConeResourceLoader     iResourceLoader;

    /**
     * Stored mailbox ID
     */
    TMsvId                  iMailboxId;
    
    //<cmail>
    /**
     * This shows that settings are opened from folder list view.
     * When settings are opened from folder list view, we go directly to 
     * 'what to sync' dialog. Bypassing mailbox's main settings view
     */
    TBool                   iShowFolderSettings;
    
    /**
     * When settings are shown from folder list view, open returning we
     * need to ignore one back key. Logic is so that we go back from 'what to sync'
     * view to folder list view directly, bypassing mailbox's main settings view
     */    
    TBool                   iIgnoreOneBackKey;
    //<cmail>
    
    };

#endif      // IPSSETUI_H

// End of File
