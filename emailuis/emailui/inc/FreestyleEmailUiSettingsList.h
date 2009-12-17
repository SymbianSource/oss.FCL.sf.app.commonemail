/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email header file
*
*/



#ifndef FSSETTINGSSYNCLIST_H
#define FSSETTINGSSYNCLIST_H

// SYSTEM INCLUDES
#include <coecntrl.h>
#include <e32cmn.h>
//<cmail>
#include "MFSMailRequestObserver.h"
//</cmail>
#include <AknWaitDialog.h>

// FORWARD DECLARATIONS
class CFSMailClient;        
class CAknSingleGraphicStyleListBox;
class CEikTextListBox;
class TFSMailMsgId;
class CFSMailBox;
class CFreestyleEmailUiAppUi;

typedef CAknSingleGraphicStyleListBox CSettingsListType;

/**
 * Container class for CFsEmailSettingsList
 * 
 * @class   CFsEmailSettingsList
 */
class CFsEmailSettingsList : 
    public CCoeControl, 
    public MEikListBoxObserver, 
    public MFSMailRequestObserver,
    public MProgressDialogCallback
    {
public:
    // constructors and destructor
    static CFsEmailSettingsList* NewL( 
        const TRect& aRect, 
        const CCoeControl* aParent,
        CFreestyleEmailUiAppUi& aAppUi,
        CFSMailClient& aMailClient,
        CFsEmailSettingsListView& aView );
    
    static CFsEmailSettingsList* NewLC( 
        const TRect& aRect, 
        const CCoeControl* aParent,
        CFreestyleEmailUiAppUi& aAppUi,
        CFSMailClient& aMailClient,
        CFsEmailSettingsListView& aView );
    
    virtual ~CFsEmailSettingsList();

public:
    // from base class CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

    TKeyResponse OfferKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aType );
    
    void HandleResourceChange( TInt aType );
    void AddAccountL();
    void RemoveAccountL();
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
    TBool PIMSyncItemVisible();

public:     
    CSettingsListType* ListBox();
    
    static HBufC* CreateListBoxItemLC( 
            const TDesC& aMainText );
    
    static HBufC* CreateListBoxItemLC( 
            const TDesC& aMainText,
            TInt aIconNum );
    
    // insert needed icons to list box
    void SetupListBoxIconsL();
    
    /**
     * Fills list with mailbox names given by fs mail client
     */
    void CreateAccountListL();

    /**
     * Displays mailbox create query if needed
     */
    void DisplayCreateMailboxNoteIfNeededL();

    /**
     * Fills list with captions given by settings plugin
     */
    void CreatePluginSubviewListL( const TUid& aPluginId );

    /**
     * Fills list with captions given by settings plugin
     */
    void CreatePluginPIMListL( const TUid& aPluginId );
    
    /**
     * Sets specific mailbox protocol icon by given id
     */
    void ProtocolBrandingIconL( TFSMailMsgId aMailBoxId );
    
    // list content commands
    void ClearListL();
    void AppendItemL( TDesC& aListItem );
    TInt Count();
    
    /**
     * list content will be drawn with captions 
     * received from settings plugin or
     * mailboxes received from mailclient
     * 
     * returns True if captions list
     * return False if mailbox list
     */
    TBool Get2ndLevelListIndicator();
    
    // handle user navigation
    void HandleUserSelectionsL();
    
    /**
     * Retuns specific mailbox by given index
     * Ownership tranferred
     */
    CFSMailBox* GetMailboxByIndexLC( const TInt& aIndex ) const;
    
    // get count of mailboxes in fs mailclient
    TInt GetMailboxCountL() const;
    
    /**
     * Retuns specific mailbox by given id
     * Ownership tranferred or NULL
     */
    CFSMailBox* GetMailBoxByIdL( const TUint& aMailboxId ) const;
    
    // set account info member data from selected mailbox
    void SetSelectedAccountInfo( const TFSMailMsgId& aAccountInfo );

    // set private settings plugin Uid to given value
    void SetPluginSettingsUid( const TUid& aPluginId );
    
    // activate selected sub view from plugin
    void ActivateMailSettingsPluginSubViewL( TInt aSubViewId );

    // open dialog with PIM service items
    void OpenPIMServiceDialogL();
    
    /**
     * returns needed settings plugin instance from view stack
     * or creates new one add it to view stack and return pointer
     * Ownership is not transferred
     */
    CESMailSettingsPlugin* GetSettingsPluginL( TUid aUid );
    
    // set subview list index to given value
    void SetSelectedSubListIndex( TInt aIndex );

    // set main list index in parent
    void SetSelectedMainListIndex( TInt aIndex );
    
    // get main list index from parent
    TInt GetSelectedMainListIndex() const;
    
    // This sets the flag that informs that we are in the personal information management subview   
    void SetPimListActivation(TBool aActivation);

    // Creates and passes ownership of title pane text appropriate for the current view
    HBufC* CreateTitlePaneTextLC() const;
    
    void GoToTop();
    void GoToBottom();
    
    void PageUp();
    void PageDown();
    
public: 
    enum TControls
        {
        EListBox,
        ELastControl
        };
    enum TListBoxImages
        {
        EListBoxFirstUserImageIndex
        };  
    
protected:
    // from base class CCoeControl
    void SizeChanged();
    
protected: // from MProgressDialogCallback
    void DialogDismissedL( TInt aButtonId );
    
private:
    // from base class CCoeControl
    void Draw( const TRect& aRect ) const;

private:
    void ConstructL( 
        const TRect& aRect, 
        const CCoeControl* aParent);
    
    CFsEmailSettingsList(CFreestyleEmailUiAppUi& aAppUi, CFSMailClient& aMailClient, CFsEmailSettingsListView& aView );
    void HandleListBoxEventL( CEikListBox *aListBox, TListBoxEvent aEventType );
    void InitializeControlsL();
    void LayoutControls();
    void LoadAccountsToListL();
    void SetDefaultPIMAccountL();
    TPtrC GetMailBoxNameL( const CFSMailBox& aMailBox ) const;
    
private:
    enum TFsEmailSettingsSubViewType
        {
        EFsEmailSettingsMainListView,
        EFsEmailSettingsMailboxView,
        EFsEmailSettingsPimView
        };
    TFsEmailSettingsSubViewType iCurrentSubView;

    CSettingsListType*          iListBox;
    CFreestyleEmailUiAppUi&     iAppUi;
    CCoeControl*                iFocusControl;
    CFSMailClient&              iMailClient;
    TFSMailMsgId                iSelectedAccountInfo;
    TUid                        iSelectedPluginSettings;
    TInt                        iPIMSyncMailboxIndex;
    TBool                       iPIMListActivation;
    TBool                       iPIMServiceSettingsSelection;
    CAknWaitDialog*             iWaitDialog;
    TInt                        iMailboxCount;
    TInt                        iDeleteMailboxId;
    TInt                        iDeletedIndex;
    TInt                        iPIMSyncCount;
    TInt                        iSelectedSubListIndex;
    CFsEmailSettingsListView& 	iView; 
	// it is used to properly handle taps
    // first tap highlights an item on the settings list, second tap selects the item
    TInt                        iPrevSelected;
    };
                
#endif // FSSETTINGSSYNCLIST_H
