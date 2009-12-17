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
* Description:  Decleares base dialog controlling functionality.
*
*/



#ifndef IPSSETUIBASEDIALOG_H
#define IPSSETUIBASEDIALOG_H


#include <eiklbo.h>                 // MEikListBoxObserver
#include <coedef.h>                 // TKeyResponse
#include <w32std.h>                 // TKeyEvent
#include <eikcmobs.h>               // MEikCommandObserver
#include <aknsettingpage.h>         // MAknSettingPageObserver

#include "ipssetuibasearray.h"
#include "ipssetuictrlpageresource.h"

typedef CArrayFixFlat<TInt> CIpsSetUiIntArray;
typedef CArrayPtrFlat<CAknSettingPage> CIpsSetUiPages;
typedef CAknSettingPage::TAknSettingPageUpdateMode TIpsSetUiUpdateMode;

class CEikFormattedCellListBox;
class CEikScrollBarFrame;
class CAknSettingPage;
class CAknTitlePane;
class CAknQueryDialog;
class CEikButtonGroupContainer;
class CSelectionItemList;
class CIpsSetUiApprover;
class CIpsSetUiItem;
class CIpsSetUiCtrlSoftkey;

/**
 * Base dialog control functionality.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class CIpsSetUiBaseDialog :
    public CIpsSetUiBaseArray,
    public MEikListBoxObserver,
    public MEikCommandObserver,
    public MAknSettingPageObserver
    {
public: // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CIpsSetUiBaseDialog();

public: // New virtual functions

    /**
     * Setting has started.
     *
     * @return ETrue, when continue is ok.
     */
    virtual TBool SettingsStartedL();

public: // New functions

    /**
     * @return Currently active item.
     */
    TInt CurrentItemIndex() const;

    /**
     * @return Currently focused item.
     */
    CIpsSetUiItem* CurrentItem() const;

    //<cmail>
    /**
     * Count of list box items
     * 
     * @return count of list box items
     */
    TInt CountInListBox() const;
    //</cmail>

protected:  // Constructors

    /**
     * Constructor.
     *
     * @param aListBox Listbox owned by the dialog.
     * @param aTitlePane Just take a wild guess.
     */
    CIpsSetUiBaseDialog(
        CEikFormattedCellListBox& aListBox,
        CAknTitlePane& aTitlePane );

    /**
     * 2nd phase of constuction
     *
     * @param aButtons Buttons owned by the dialog.
     * @param aResource Settings resource.
     * @param aUpdateMSK Middle softkey support.
     */
    void ConstructL(
        CEikButtonGroupContainer& aButtons,
        const TInt aResource = NULL,
        const TBool aUpdateMSK = EFalse );

protected:  // New virtual functions

// From MAknSettingPageObserver

    /**
     * Handle event invoked by the setting page.
     *
     * @param aSettingPage SettingPage that invoked the event.
     * @param aEventType Type of invoked event.
     */
    virtual void HandleSettingPageEventL(
        CAknSettingPage* aSettingPage,
        TAknSettingPageEvent aEventType ) = 0;

    /**
     * Check if the exiting is ok.
     *
     * @param aButtonId Button that is pressed.
     * @return ETrue, when exit is ok.
     */
    virtual TBool OkToExitL(
        const TInt aButtonId ) = 0;

    /**
     * Initializes undefined item.
     *
     * @param aBaseItem Item to be initialized.
     * @param aValue Value to be used in initialization.
     * @param aText Text to be used in initialization.
     * @return KErrNone, when init is success.
     */
    virtual TInt InitItemUnidentified(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue,
        const TDesC& aText );

    /**
     * Launch multiline editor.
     *
     * @param aBaseItem Preferably multiline editor item.
     * @param aUpdateMode Mode to be used to launch the editor.
     * @return Result of the edit.
     */
    virtual TIpsSetUiEventResult SettingLaunchMultilineEditorL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Key event catcher.
     *
     * @param aKeyEvent Invoked key event.
     * @param aType Type of the event.
     * @return Consumed/Not consumed.
     */
    virtual TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * Get unique setting page.
     *
     * @param aReturnPage Page to be created.
     * @param aReturnUpdateMode Update mode of the page.
     * @param aReturnSettingValue User input value.
     * @param aReturnSettingText User input text.
     * @param aRetrunRadioButtonArray Custom array of buttons.
     * @param aParamBaseItem Item invoked the event.
     */
    virtual void AcquireCustomSettingPageL(
        CAknSettingPage*& aReturnPage,
        TIpsSetUiUpdateMode& aReturnUpdateMode,
        TInt& aReturnSettingValue,
        TDes& aReturnSettingText,
        CDesCArrayFlat& aReturnRadioButtonArray,
        CIpsSetUiItem& aParamBaseItem );

    /**
     *
     *
     * @param aBaseItem Item invoked the event.
     */
    virtual TInt EventSubArrayChangeL(
        CIpsSetUiItem& aBaseItem );

    /**
     *
     *
     * @param aBaseItem Item invoked the event.
     */
    virtual TIpsSetUiEventResult EventItemEditStartsL(
        CIpsSetUiItem& aBaseItem );

    /**
     *
     *
     * @param aBaseItem Item invoked the event.
     */
    virtual TInt EventItemEditEndsL(
        CIpsSetUiItem& aBaseItem );

    /**
     *
     *
     * @param aBaseItem Item invoked the event.
     */
    virtual TIpsSetUiEventResult EventItemEvaluateValue(
        const CIpsSetUiItem& aBaseItem,
        TInt& aNewValue );

    /**
     * Checks user input.
     *
     * @param aBaseItem Item invoked the event.
     * @param aNewValue User input value.
     */
    virtual TIpsSetUiEventResult EventItemEvaluateRadioButton(
        const CIpsSetUiItem& aBaseItem,
        TInt& aNewValue );

    /**
     * Checks user input.
     *
     * @param aBaseItem Item invoked the event.
     * @param aNewText User input text.
     */
    virtual TIpsSetUiEventResult EventItemEvaluateText(
        const CIpsSetUiItem& aBaseItem,
        TDes& aNewText );

protected:  // New functions

    // Setting page opening


    /**
     *
     *
     * @param .
     * @return .
     */
    TIpsSetUiPageResult OpenSettingPageL(
        const TInt aIndex = KErrNotFound );

    /**
     *
     *
     * @param .
     * @return .
     */
    TIpsSetUiPageResult OpenSettingPageL(
        CIpsSetUiItem& aBaseItem );

    /**
     *
     *
     * @param .
     * @param .
     */
    void SetMainMBoxMenuTitlePaneTextL(
        const TDesC& aText,
        const TBool aSaveCurrent = ETrue );

    /**
     *
     *
     * @param .
     */
    void SetSettingsMenuTitlePaneText(
        const TBool aSaveCurrent = ETrue );
    

    /**
     *
     *
     * @param .
     * @param .
     * @param .
     * @return .
     */
    CIpsSetUiItem* SettingFindEditor(
        const CIpsSetUiItem& aBaseItem,
        const TInt aIndex,
        const TBool aExcludeHidden = ETrue );

    /**
     *
     *
     * @param .
     * @param .
     * @param .
     * @return .
     */
    CIpsSetUiItem* SettingFindEditor(
        const CIpsSetUiItem& aBaseItem,
        const TUid& aId,
        const TBool aExcludeHidden = ETrue );

    /**
     *
     *
     * @param .
     * @param .
     * @param .
     * @return .
     */
    TInt InitAnyItem(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue,
        const TDesC& aText );

    /**
     *
     *
     * @param .
     * @return .
     */
    TInt ShowCustomQueryL(
        const TInt aResourceStringId );

    /**
     *
     *
     * @param .
     * @return .
     */
    TInt GetTextEditorFlags( const CIpsSetUiItem& aBaseItem );

    /**
     *
     *
     * @param .
     * @return .
     */
    TInt GetValueEditorFlags( const CIpsSetUiItem& aBaseItem );


    /**
     *
     *
     * @param .
     */
    void SetListboxEmptyTextL( const TDesC& aText );

    /**
     * Gets the setting page resource of specific item
     *
     * @param .
     * @return resource id of the page
     */
    TInt SettingPageResource(
        const TIpsSetUiPageResource aSettingPage ) const;

    /**
     * Sets the new setting page resource for specific item
     *
     * @param aSettingPage Inserts the resource for the page
     * @param .
     */
    void SetSettingPageResource(
        const TIpsSetUiPageResource aSettingPage,
        const TInt aResourceId );

    /**
     *
     *
     * @param .
     * @param .
     */
    void CheckRadioButton(
        CIpsSetUiItemLink& aRadioButtonArray,
        const TInt aRadioButtonIndex );

    /**
     *
     *
     * @param .
     * @param .
     */
    void CheckRadioButton(
        CIpsSetUiItemLink& aRadioButtonArray,
        const TUid& aId );

    /**
     *
     *
     * @param .
     * @param .
     * @param .
     * @return .
     */
     TInt FindCheckedRadiobutton(
        const CIpsSetUiItemLinkExt& aArray,
        TUid& aId,
        TInt& aIndex );

    /**
     *
     *
     * @param .
     */
    void AddBackupItemL(
        const CIpsSetUiItem& aBaseItem );

    /**
     *
     *
     */
    void RemoveLastBackupItem();

    /**
     *
     *
     * @return .
     */
    CIpsSetUiItem* GetLastBackupItem();

    /**
     *
     *
     */
    void RestoreFromBackup();

    /**
     *
     *
     */
    void CleanBackupArray();

protected:  // Functions from base classes

    // MEikCommandObserver

    /**
     *
     *
     * @param .
     */
    virtual void ProcessCommandL( TInt aCommandId ) = 0;

private:  // New functions

    // Setting page opening

    /**
     *
     *
     * @return .
     */
    TIpsSetUiPageResult SettingsPageOpenCheckL();

    /**
     *
     *
     * @return .
     */
    TIpsSetUiPageResult SettingPageOpenEditorL();

    /**
     *
     *
     */
    void SettingPageOpenFinishL();

    /**
     *
     *
     * @param .
     * @return .
     */
    TIpsSetUiPageResult SettingPageOpenL(
        CIpsSetUiItem& aBaseItem );

    /**
     * Launches setting page.
     *
     * @param aBaseItem Item to be edited by the UI.
     * @param aUpdateMode Mode of the editor.
     * @return Result of the edit operation.
     */
    TIpsSetUiPageResult SettingLaunchPageLD(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Launches setting page.
     *
     * @param aBaseItem Item to be edited by the UI.
     * @param aUpdateMode Mode of the editor.
     * @return Result of the edit operation.
     */
    TIpsSetUiPageResult SettingLaunchRadioButtonPageLD(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Launches setting page.
     *
     * @param aBaseItem Item to be edited by the UI.
     * @param aUpdateMode Mode of the editor.
     * @return Result of the edit operation.
     */
    TIpsSetUiPageResult SettingLaunchCheckboxPageLD(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Launches setting page.
     *
     * @param aBaseItem Item to be edited by the UI.
     * @param aUpdateMode Mode of the editor.
     * @return Result of the edit operation.
     */
    TIpsSetUiPageResult SettingLaunchTextEditorPageLD(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Launches setting page.
     *
     * @param aBaseItem Item to be edited by the UI.
     * @param aUpdateMode Mode of the editor.
     * @return Result of the edit operation.
     */
    TIpsSetUiPageResult SettingLaunchNumberEditorPageLD(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Launches setting page.
     *
     * @param aBaseItem Item to be edited by the UI.
     * @param aUpdateMode Mode of the editor.
     * @return Result of the edit operation.
     */
    TIpsSetUiPageResult SettingLaunchScNumberEditorPageLD(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Launches setting page.
     *
     * @param aBaseItem Item to be edited by the UI.
     * @param aUpdateMode Mode of the editor.
     * @return Result of the edit operation.
     */
    TIpsSetUiPageResult SettingLaunchMultilinePageL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Check if new editor must be opened.
     *
     * @param .
     */
    void SettingsCheckNextEditorOpenL(
        TIpsSetUiPageResult& aResult );

    // Setting page creation

    /**
     * Create specific setting editor.
     *
     * @param aBaseItem Item to be edited through the UI.
     * @param aUpdateMode Mode of the editor.
     */
    void SettingCreateEditorL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Create specific setting editor.
     *
     * @param aBaseItem Item to be edited through the UI.
     * @param aUpdateMode Mode of the editor.
     */
    void SettingCreateRadioButtonPageL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Create specific setting editor.
     *
     * @param aBaseItem Item to be edited through the UI.
     * @param aUpdateMode Mode of the editor.
     */
    void SettingCreateCheckboxPageL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Create specific setting editor.
     *
     * @param aBaseItem Item to be edited through the UI.
     * @param aUpdateMode Mode of the editor.
     */
    void SettingCreatePlainTextEditorPageL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Create specific setting editor.
     *
     * @param aBaseItem Item to be edited through the UI.
     * @param aUpdateMode Mode of the editor.
     */
    void SettingCreatePasswordTextEditorPageL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Create specific setting editor.
     *
     * @param aBaseItem Item to be edited through the UI.
     * @param aUpdateMode Mode of the editor.
     */
    void SettingCreatePlainNumberEditorPageL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );

    /**
     * Create specific setting editor.
     *
     * @param aBaseItem Item to be edited through the UI.
     * @param aUpdateMode Mode of the editor.
     */
    void SettingCreateScNumberEditorPageL(
        CIpsSetUiItem& aBaseItem,
        TIpsSetUiUpdateMode& aUpdateMode );


    /**
     * Gets default setting page values.
     *
     * @param aBase Item being edited.
     * @return Correct update mode.
     */
    TIpsSetUiUpdateMode DefineDefaultSettingPageL(
        const CIpsSetUiItem& aBase );

    /**
     * @param aButtonArray Radiobutton array to be filled.
     */
    void SettingCreateButtonArrayL(
        CIpsSetUiBaseItemArray& aButtonArray );

    /**
     * @param aButtonArray Checkbox array to be filled..
     */
    void SettingCreateCheckboxArrayL(
        CIpsSetUiBaseItemArray& aButtonArray );

    /**
     * Prepares setting page creation.
     *
     * @param aUpdateMode Updatemode for editor.
     * @param Item being edited.
     */
    void SettingPrepareAcquireL(
        TIpsSetUiUpdateMode& aUpdateMode,
        CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item being prepared.
     */
    void SettingPrepareAcquireRadioButtonPageL(
        CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item being prepared.
     */
    void SettingPrepareAcquireCheckboxPageL(
        CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item being prepared.
     */
    void SettingPrepareAcquireTextEditorPageL(
        CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item being prepared.
     */
    void SettingPrepareAcquireValueEditorPageL(
        CIpsSetUiItem& aBaseItem );


    /**
     * Initializes link array item.
     *
     * @param aBaseItem Item to be initialized.
     * @param aText Text data to be set to item.
     * @return KErrNone on success.
     */
    TInt InitArrayLink(
        CIpsSetUiItem& aBaseItem,
        const TDesC& aText );

    /**
     * Initializes radio button array.
     *
     * @param aBaseItem Item to be initialized.
     * @param aValue Numeric data to be set to item.
     * @param aText Text data to be set to item.
     * @return KErrNone on success.
     */
    TInt InitArrayRadioButton(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue,
        const TDesC& aText );

    /**
     * Initializes checkbox array.
     *
     * @param aBaseItem Item to be initialized.
     * @param aValue Numeric data to be set to item.
     * @param aText Text data to be set to item.
     * @return KErrNone on success.
     */
    TInt InitArrayCheckBox(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue,
        const TDesC& aText );

    /**
     * Initializes text item.
     *
     * @param aBaseItem Item to be initialized.
     * @param aText Text data to be set to item.
     * @return KErrNone on success.
     */
    TInt InitItemTextEdit(
        CIpsSetUiItem& aBaseItem,
        const TDesC& aText );

    /**
     * Initializes value item.
     *
     * @param aBaseItem Item to be initialized.
     * @param aValue Numeric data to be set to item.
     * @param aText Text data to be set to item.
     * @return KErrNone on success.
     */
    TInt InitItemNumberEdit(
        CIpsSetUiItem& aBaseItem,
        const TInt aValue,
        const TDesC& aText );
        
    /**
     * Prepares subarray event
     *
     * @param aBaseItem Item invoked the event.
     * @param aForward Type of the event.
     * @return Result of the handling.
     */
    TInt EventPrepareSubArrayL(
        CIpsSetUiItem& aBaseItem,
        const TBool aForward = ETrue );

    /**
     * Gets the previous stored value and removes it
     *
     * @param aRemove Removes the returned index from the array.
     * @return Last index
     */
    TInt SettingsLastItemIndex( const TBool aRemove = ETrue ) const;

    /**
     * Adds current itemindex to stack
     *
     * @param aSettingsLastItemIndex, new index
     */
    void SetSettingsLastItemIndexL( const TInt aLastItemIndex );

    /**
     * Returns current settings page as item.
     *
     * @param Remove from array.
     * @return Current setting page.
     */
    CIpsSetUiItem* CurrentSettingsPage(
        const TBool aRemove = EFalse );

    /**
     * @param aBaseItem Page to be set as active.
     */
    void SetCurrentSettingsPageL(
        CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item to be backed up before editing.
     */
    void BackupItemCreateLinkL(
        const CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item to be backed up before editing.     
     */
    void BackupItemCreateRadioL(
        const CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item to be backed up before editing.
     */
    void BackupItemCreateValueL(
        const CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item to be backed up before editing.     
     */
    void BackupItemCreateTextL(
        const CIpsSetUiItem& aBaseItem );

    /**
     * @param aBaseItem Item to be backed up before editing.     
     */
    void BackupItemCreateUndefinedL(
        const CIpsSetUiItem& aBaseItem );

    /**
     * Create backup item.
     *
     * @param aBaseItem Item being edited.
     * @return Backup item with client ownership.
     */
    virtual CIpsSetUiItem* CreateItemForBackupL(
        const CIpsSetUiItem& aBaseItem );

    /**
     * @return Result of the canceling.
     */
    TIpsSetUiPageResult HandleEditorCancelL();

    /**
     * @return Result of editor accept.
     */
    TIpsSetUiPageResult HandleEditorOk();

    /**
     * @return Result of editor invalid input.
     */
    TIpsSetUiPageResult HandleEditorDisapproved();

    /**
     * @return Text resource for MSK.
     */
    TInt GetResourceForMiddlekey();
    
    void UpdateTitlePaneTextL( TInt aId );

private:  // Functions from base classes

    /**
     * Array changed event handling.
     *
     * @param aEvent Type of the event.
     * @return KErrNone, when handled properly.
     */
    TInt EventArrayChangedL(
        const TIpsSetUiArrayEvent aEvent );

    /**
     * Custom text line for item.
     *
     * @param aId Unique id of the item.
     * @param aString Textual string for listbox.
     * @return ETrue, when item is handled.
     */
    TBool EventCustomMdcaPoint(
        const TUid& aId,
        TPtr& aString ) const;

// MEikListBoxObserver

     /**
      * Listbox event handler.
      *
      * @param aListBox that invoked the event.
      * @param aEventType Type of the event.
      */
     virtual void HandleListBoxEventL(
        CEikListBox* aListBox,
        TListBoxEvent aEventType );

protected:  // Data

    // DIALOG REFERENCES

    /**
     * The listbox that is shown in view for user
     */
    CEikFormattedCellListBox& iListBox;

    /**
     * Used titlepane in settings dialog
     */
    CAknTitlePane&      iTitlePane;

    /**
     * Buttons used to show in the dialog
     * Owned.
     */
    CIpsSetUiCtrlSoftkey* iSoftkeys;

    /**
     * Temporary storage for settings page
     * Owned.
     */
    CAknSettingPage*        iSettingPage;
    
    /*
     * Title pane text for main menu view, currently
     * title for main menu is mailbox name
     */
    HBufC* iMainMBoxMenuTitleText;

private:    // Data

    /**
     * Flags to control dialog behavior.
     */
    enum TIpsSetUiSettingsFlags
        {
        EIpsSetUiStartIssued = 0,
        EIpsSetUiRunningCustomPage,
        };

    // RESOURCES


    /**
     * Status flags
     */
    TUint64          iBaseDialogFlags;

    /**
     * Container for resource page ids
     * Owned.
     */
    CIpsSetUiCtrlPageResource* iSettingPageResources;

    /**
     * Container array for items to be edited
     * Owned.
     */
    CIpsSetUiBaseItemArray* iSettingsPageArray;

    // DIALOG

    /**
     * Pointer to scrollbar used in settings page
     * Owned.
     */
    CEikScrollBarFrame* iScrollBar;

    /**
     * Temporary place to store the previous titlepane text
     * Owned.
     */
    HBufC*              iOldTitleText;

    /**
     * Object for query dialog
     * Owned.
     */
    CAknQueryDialog*    iQueryDialog;

    // SETTING PAGE

    /**
     * Previous location of cursor
     * Owned.
     */
    CIpsSetUiIntArray*    iSettingsLastItemIndex;

    /**
     * Temporary value for setting page
     */
    TInt                    iSettingValue;

    /**
     * Temporary text for setting page
     */
    RBuf                    iSettingText;

    /**
     * Temporary text for password setting page
     * Owned.
     */
    TIpsSetUiPasswordText*      iNewPassword;

    /**
     * Temporary text for password setting page
     * Owned.
     */
    TIpsSetUiPasswordText*      iOldPassword;

    /**
     * Temporary array for buttons
     * Owned.
     */
    CDesCArrayFlat*         iButtonArray;

    /**
     * Temporary array for checkboxes
     * Owned.
     */
    CSelectionItemList*     iCheckboxArray;

    /**
     * Array to temporary store the items to be edited
     * Owned.
     */
    CIpsSetUiBaseItemArray*   iBackupArray;

    /**
     * Index to focused list box item
     */
    TInt                    iCurrentItemIndex;

    /**
     * Checks user input
     * Owned.
     */
    CIpsSetUiApprover*      iItemApprover;
    

    };

#endif /* IPSSETUIBASEDIALOG_H */

// End of File


