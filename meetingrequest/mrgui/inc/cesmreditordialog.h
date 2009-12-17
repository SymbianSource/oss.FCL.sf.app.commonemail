/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR editor dialog
*
*/

#ifndef CESMREDITORDIALOG_H
#define CESMREDITORDIALOG_H

#include <AknDialog.h>
#include "mesmruibase.h"
#include "mesmrresponseobserver.h"
#include "mesmrfieldeventobserver.h"

class CESMRPolicy;
class MESMRCalEntry;
class CESMRView;
class MAgnEntryUiCallback;
class CESMRSendUI;
class CAiwServiceHandler;
class CESMRField;
class CESMRTitlePaneHandler;
class CCalenInterimUtils2;
class CESMRLocationPluginHandler;
class CESMRAddressInfoHandler;
class CESMRFeatureSettings;

/**
 *  CESMREditorDialog implements the editor dialog for meeting requests.
 *
 *  @see mesmruibase.h, mesmruifactory.h
 *  @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMREditorDialog ) :
		public CAknDialog,
		public MESMRUiBase,
		public MESMRResponseObserver,
		public MESMRFieldEventObserver
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aPolicy contains all the fields for list component.
     * @param aEntry ESMR calendar entry (wraps the CCalEntry object)
     * @param aCallback Callback interface for AgnEntryUI
     * @return Pointer to created and initialized esmr editor dialog.
     */
    IMPORT_C static CESMREditorDialog* NewL(
            CESMRPolicy* aPolicy,
            MESMRCalEntry& aEntry,
            MAgnEntryUiCallback& aCallback );

    /*
     * Destructor.
     */
    ~CESMREditorDialog();

public: // From CAknDialog
    SEikControlInfo CreateCustomControlL( TInt aType );
    TKeyResponse OfferKeyEventL(
            const TKeyEvent& aEvent,
            TEventCode aType);
    TBool OkToExitL ( TInt aButtonId );
    void ActivateL();
    void ProcessCommandL( TInt aCommand );
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

public: // From MESMRUIBase
    TInt ExecuteViewLD();

public: // From MESMRResponseObserver
    TBool Response( TInt aCommand );
    void ExitDialog();
    void ChangeReadyResponseL();
    
protected: // From MESMRFieldEventObserver
    void HandleFieldEventL( const MESMRFieldEvent& aEvent );

private: // Implementation

    void DoProcessCommandL( TInt aCommand );

    /**
     * Constructor.
     *
     * @param aEntry ESMR calendar entry (wraps the CCalEntry object)
     * @param aCallback Callback interface for AgnEntryUI
     */
    CESMREditorDialog(
            MESMRCalEntry& aEntry,
            MAgnEntryUiCallback& aCallback );

    /**
     * Second phase constructor.
     *
     * @param aPolicy contains all the fields for list component.
     */
    void ConstructL( CESMRPolicy* aPolicy );

    /**
     * Handles dialog exit, saves entry etc.
     * @return TInt
     */
    TInt HandleExitL();

    /**
     * Handles dialog exit, saves, no queries.
     * @param aShowConfirmationQuery ETrue shows 'save changes' query before saving entry.
     * @return TInt
     */
    TInt HandleForcedExitL( TBool aShowConfirmationQuery = EFalse );

    /**
     * Sends calendar entry.
     * @param aCommandId
     */
    void SendCalendarEntryL(TInt aCommandId);

    /**
     * Tries to add send menu item to options menu if needed.
     * @param aMenuPane
     */
    void TryInsertSendMenuL(CEikMenuPane* aMenuPane);

    /**
     * Handles print command.
     * @param aCommand
     */
    void HandlePrintCommandL(TInt aCommand);

    /**
     * Executes user related queries whether the entry should be
     * save or not.
     * @return TBool
     */
    TBool IsAllowedToSaveL();

    /**
     * Checks if cfsmailbox actually supports attendee status
     * @return TBool
     */
    TBool CESMREditorDialog::SupportsAttendeeStatusL( );
    
    /**
     * Handle options menu command "open"
     */
    void HandleOpenCmdL();

    /**
     * Handles Location Options submenu content
     * 
     * @param aResourceId Menu resource id
     * @param aMenuPane menu pane 
     */
    void HandleLocationOptionsL( TInt aResourceId, CEikMenuPane* aMenuPane );
    
    /**
     * Loads options menu text for given command.
     * 
     * @param aCommandId option menu item command id
     * @return text
     */
    HBufC* LoadOptionsMenuTextLC( TInt aCommandId );
    
    CESMRLocationPluginHandler& LocationPluginHandlerL();
    
    CESMRAddressInfoHandler& AddressInfoHandlerL();
    
    void HandleFieldCommandEventL( const MESMRFieldEvent& aEvent );
    
    void HandleFieldChangeEventL( const MESMRFieldEvent& aEvent );
    
private:

    /**
     * Own: Feature manager initialization flag
     */
    TBool iFeatureManagerInitialized;

    /**
     * Ref: Policy for building fields and menus
     */
    CESMRPolicy* iPolicy;

    /**
     * Ref: The only control in this dialog. Fwk deletes.
     */
    CESMRView* iView;

    /**
     * Ref: Reference to MESRCalEntry (CCalEntry wrapper)
     */
    MESMRCalEntry& iEntry;

    /**
     * Ref: Agn Entry UI callback interface
     */
    MAgnEntryUiCallback& iCallback;

    /**
     * Own: Calendar Global data
     */
    CESMRSendUI* iESMRSendUI;

    /**
     * Own: AIW Service handler
     */
    CAiwServiceHandler* iServiceHandler;

    /**
     * Own: Boolean indicating if required attendees field contains items.
     */
    TBool iRequiredAttendeesEnabled;

    /**
     * Own: Boolean indicating if optional attendees field contains items.
     */
    TBool iOptionalAttendeesEnabled;

    /// Own: Titlepane handler
    CESMRTitlePaneHandler* iTitlePane;

    /**
     * Pointer to instance of CCalenInterimUtils2.
     * Own: Release on destruct.
     */
    CCalenInterimUtils2* iInterimUtils;
    
	/**
	 * Location plugin handler.
	 * Own.
	 */
    CESMRLocationPluginHandler* iLocationPluginHandler;
	
    /**
     * Address info handler.
     * Own.
     */
    CESMRAddressInfoHandler* iAddressInfoHandler;
    
    /**
     * Feature settings.
     * Own.
     */
    CESMRFeatureSettings* iFeatures;
    
    /**
     * Indicates if meeting request occurs in past. 
     */
    TBool iOccursInPast;
    
    /**
     * Indicates if location has been modified.
     */
    TBool iLocationModified;
    };

#endif  // CESMREDITORDIALOG_H
