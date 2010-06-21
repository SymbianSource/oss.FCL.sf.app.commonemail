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
* Description:  File for dialog control.
*
*/



#ifndef IPSSETUIDIALOG_H
#define IPSSETUIDIALOG_H


#include <iapprefs.h>           // TImIAPChoice
#include <msvapi.h>             // MMsvSessionObserver

#include "ipssetuibasedialog.h" // CIpsSetUiBaseDialog

class CIpsSetData;
class CIpsSetUi;

class CIpsSetUiDialogCtrl :
    public CIpsSetUiBaseDialog
    {
public:

    /**
     * Class destructor.
     */
    ~CIpsSetUiDialogCtrl();

    /**
     * 2-phase constructor.
     */
    static CIpsSetUiDialogCtrl* NewL(
        CIpsSetUi& aDialog,
        CEikFormattedCellListBox& aListBox,
        CAknTitlePane& aTitlePane,
        CEikButtonGroupContainer& aButtons,
        CMsvSession& aSession,
        TUint64& aFlags );

    /**
     * 2-phase constructor with stack push.
     */
    static CIpsSetUiDialogCtrl* NewLC(
        CIpsSetUi& aDialog,
        CEikFormattedCellListBox& aListBox,
        CAknTitlePane& aTitlePane,
        CEikButtonGroupContainer& aButtons,
        CMsvSession& aSession,
        TUint64& aFlags );

// New functions

    /**
     * Retrieves the current help context.
     *
     * @param aContext Current help context.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    
    void DynInitMenuPaneL(   /*TInt aResourceId,*/
            CEikMenuPane* aMenuPane );    

    /**
     * Saves the settings.
     */
    void StoreSettingsToAccountL();

// From base class CIpsSetUiBaseDialog

    /**
     * Catches key events.
     *
     * @param aKeyEvent Invoked event.
     * @param aType Type of event.
     * @return State of the key event after procesessing.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * Processes the dialog commands.
     *
     * @param aCommandId Invoked command.
     */
    virtual void ProcessCommandL( TInt aCommandId );

    /**
     * Check whether the dialog can be exited.
     *
     * @param aButtonId Pressed button.
     */
    virtual TBool OkToExitL(
        const TInt aButtonId );

protected:

    /**
     * Constructor.
     *
     * @param aDialog Actual dialog.
     * @param aListBox Listbox the dialog is showing.
     * @param aTitlePane Guess...
     * @param aFlags Flags to control dialog behavior and exit.
     */
    CIpsSetUiDialogCtrl(
        CIpsSetUi& aDialog,
        CEikFormattedCellListBox& aListBox,
        CAknTitlePane& aTitlePane,
        CMsvSession& aSession,
        TUint64& aFlags );

private:

    /**
     * 2nd phase of construction.
     *
     * @param aButtons Button container of the dialog
     * @leave For any odd reason.
     */
    void ConstructL( CEikButtonGroupContainer& aButtons );

// New functions

    /**
     * Gets IapChoice from the item.
     *
     * @param aItemId Preferably access point item id.
     * @return Access point choice item.
     */
    TImIAPChoice GetIapPref( const TInt aItemId );

    /**
     * Handles any event from the system.
     *
     * @param aCommandId Commands to be handled.
     */
    virtual TBool EventHandlerL( const TInt aCommandId );

    /**
     * Handles softkey pressed event.
     *
     * @param aButton Button that is pressed.
     * @return ETrue, when handled.
     */
    TBool KeyPressSoftkeyL( const TInt aButton );

    /**
     * Handles OK key press.
     *
     * @param aButton Button that is pressed.
     */
    TBool KeyPressOKButtonL( const TInt aButton );

    /**
     * @param aItem Port item for initialization.
     */
    void InitPort( CIpsSetUiItem& aItem );

    /**
     * Get the port item
     *
     * @param aIncoming Defines item for fetching.
     * @param aEditor Defines if the editor should be found.
     * @return Requested item.
     */
    CIpsSetUiItem& PortItem(
        const TBool aIncoming,
        const TBool aEditor );

    /**


    /**
     * Open folder subscription dialog.
     */
    TIpsSetUiEventResult HandleEventSubscribeFoldersL();

    /**
     * @param aBaseItem Port item for handling after editor close.
     */
    void HandleEventPort( CIpsSetUiItem& aBaseItem );

    /**
     * Handle the change of Outgoing User Authentication value
     *
     * @param aBaseItem Authentication item
     * @param aForceClear Clears the username and password fields.
     */
    void HandleEventAuthentication(
        CIpsSetUiItem& aBaseItem,
        const TBool aForceClear );

    /**
     * Hides/Unhides roaming depending on the status
     *
     * @param aBaseItem Scheduling item
     */
    void HandleEventSchedule( CIpsSetUiItem& aBaseItem );

    /**
     * Change the setting label for day item.
     *
     * @param aParam Day item
     */
    void HandleEventDaysL( CIpsSetUiItem& aBaseItem );

    /**
     * Updates the settings text for download size item.
     *
     * @param aBaseItem Download size page
     */
    void HandleDownloadSizeL( CIpsSetUiItem& aBaseItem );

    /**
     * Updates the settings text for email retrieval settings.
     *
     * @param aBaseItem Download size page
     */
    //void HandleEmailsToRetrieve( CIpsSetUiItem& aBaseItem );

    /**
     * Changes resource for port editor
     */
    TIpsSetUiEventResult HandlePort();

    /**
     * Launch IAP page.
     *
     * @since FS v1.0
     * @param aBaseItem Access point item.
     */
    TIpsSetUiEventResult HandleAccessPointOpeningL( CIpsSetUiItem& aBaseItem );

    /**
     * @return Access point settings from the item.
     */
    TImIAPChoice GetIapChoiceL( const TUid& aId ) const;

    /**
     * Gets IapPrefs from the item.
     *
     * @return Access point preferences item.
     */
    CImIAPPreferences& GetIapPrefsL(const TUid& aId);

    
    /**
     * Initialize signature item
     *
     * @param aBaseItem Signature item for init
     */
    void InitSignatureL(
        CIpsSetUiItem& aBaseItem );

    /**
     * Initializes download size items
     *
     * @param aBaseItem Download size item
     * @param aValue Value for initialization
     */
    void InitDownloadSizeL(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue );

    /**
     * @param aBaseItem Access point item for initialization
     */
    void InitIapL( CIpsSetUiItem& aBaseItem );

    /**
     * Initializes retrieve limit editors
     *
     * @param aBaseItem Editor for initialization.
     * @param aValue Stored value.
     */
    void InitRetrieve(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue );

    /**
     * Initializes hours dialog
     *
     * @param aBaseItem Hours item
     * @param aStartTime Starting hour
     * @param aStopTime Stop hour
     */
    void InitHoursL(
        CIpsSetUiItem& aBaseItem,
        const TInt64 aStartTime,
        const TInt64 aStopTime );

    /**
     * Initializes roaming item
     *
     * @param aBaseItem Roaming item for initialization
     * @param aValue for initialization
     */
    void InitRoaming(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue );

    //<cmail>
    void InitSecuritySetting(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue );
    
    //</cmail>  

    /**
     * Stores the retrieval limit status.
     */
    void StoreRetrievalLimit();

    /**
     * Formats time string for setting item.
     *
     * @param aText Return string
     * @param aStartTime Starting hour
     * @param aStopTime Stopping hour
     */
    void FormatTimeStringL(
        TDes& aText,
        const TInt64 aStartTime,
        const TInt64 aStopTime ) const;

// From base class CIpsSetUiBaseArray

    /**
     * @param aBaseItem Item for initialization.
     */
    void InitUserData( CIpsSetUiItem& aBaseItem );

    
    /**
     * @param aBaseItem Item for initialization.
     */
    void InitUserDataL( CIpsSetUiItem& aBaseItem );
    
    /**
     * Initializes user name. Should be used for Outgoing setting.
     *
     * @param aBaseItem Container for username
     * @param aLogin Text to be stored
     */
    void InitLogin(
        CIpsSetUiItem& aBaseItem,
        const TDesC& aLogin );

    /**
     * Creates array of custom items.
     *
     * @param aId Item which stores the array
     * @return Item which contains the array and ownership to it.
     */
    CIpsSetUiItem* CreateCustomItemToArrayLC( const TUid& aId );

    /**
     * Make hide check including protocol checks.
     *
     * @param aItem Item to be checked
     * @return KErrNotSupported when the item should not be visible
     */
    TInt IsHidden(
        const CIpsSetUiItem& aItem ) const;

// From base class CIpsSetUiBaseDialog

    /**
     * Setting page event handling.
     *
     * @param aSettingPage Setting page that invokes the event.
     * @param aEventType Invoked event.
     */
    virtual void HandleSettingPageEventL(
        CAknSettingPage* aSettingPage,
        TAknSettingPageEvent aEventType );

    /**
     * Array change event handling.
     *
     * @param aBaseItem Item that invoked the event.
     * @return Result of the event handling.
     */
    virtual TInt EventSubArrayChangeL(
        CIpsSetUiItem& aBaseItem );

    /**
     * Handles the events, after the settings editor has been closed
     *
     * @param aBaseItem Item that has been edited.
     * @return Result of the handling
     */
    TInt EventItemEditEndsL( CIpsSetUiItem& aBaseItem );

    /**
     * Handles the events, before the settings editor is opened
     *
     * @param aBaseItem Item that is going to be edited.
     * @return Result of the handling
     */
    TIpsSetUiEventResult EventItemEditStartsL( CIpsSetUiItem& aBaseItem );

    /**
     * Launches the multiline editor
     *
     * @param aBaseItem Multiline editor item
     * @param aUpdateMode Editors mode
     * @return Result of edit
     */
    TIpsSetUiEventResult SettingLaunchMultilineEditorL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

// Data

    /**
     * Dialog which created the controller.
     */
    CIpsSetUi&      iDialog;


    /**
     * Message server session.
     */
    CMsvSession&    iSession;

    /**
     * Flags to control dialog behavior.
     */
    TUint64&        iFlags;

    /**
     * Data object store.
     * Owned.
     */
    CIpsSetData*    iData;

    /**
     * Temporary storage for signature.
     */
    RBuf            iSignature;

    };

#endif      // IPSSETUIDIALOG_H

// End of File
