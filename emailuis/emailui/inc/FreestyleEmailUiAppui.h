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
* Description:  FreestyleEmailUiAppUi class definition
*
*/


#ifndef __FREESTYLEEMAILUI_APPUI_H__
#define __FREESTYLEEMAILUI_APPUI_H__

// SYSTEM INCLUDES
#include <aknViewAppUi.h>
//<cmail> SF
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
//</cmail>
#include <cntdef.h>
#include <cntviewbase.h>
// <cmail> CPhCltDialer.h API removed
//#include <CPhCltDialer.h>
#include <etelmm.h>
// </cmail>
#include <TPbkContactItemField.h>
#include <hwrmdomainpskeys.h> // For flip change status.
//<cmail>
#include "mfsmailrequestobserver.h"
#include "mfsmaileventobserver.h"
#include "FreestyleEmailUiUtilities.h"
//</cmail>
#include <msvapi.h>
#include <cstack.h>
#include <iaupdateobserver.h>
#include "cfsmailcommon.h"
#include "cmailcustomstatuspaneindicators.h"

// INTERNAL INCLUDES
#include "FreestyleEmailUiPropertySubscriber.h"
#include "FreestyleEmailUiConstants.h"

enum TForcedStatus
	{
    ENoForce = 0,
    EForceToSync,
    EForceToConnected,
    EForceToDisconnected
    };

// FORWARD DECLARATIONS
class CFreestyleEmailUiView;
class CFSEmailUiViewerControl;
class CFSEmailUiLauncherGrid;
class CFSEmailUiLauncherGridVisualiser;
class CFSEmailUiGridMenuModel;
class CFSEmailUiLayoutHandler;
class CFSEmailUiViewSwitchTimer;
class CFSEmailUiList;
class CFSEmailUiMailViewerVisualiser;
class CFSEmailUiMailListVisualiser;
class CFSMailClient;
class CFSMailMessage;
class CFSMailFolder;
class CFSMailBox;
class CMailListUpdater;
class CFSEmailUiEditor;
class CAknInfoPopupNoteController;
class CFSEmailUiSingleListModel;
class CFSEmailUiSingleListVisualiser;
class CFreestyleEmailUiTextureManager;
class CFSEmailUiEditorVisualiser;
class CFSEmailUiMsgDetailsVisualiser;
class CFSEmailUiFolderListVisualiser;
class CNcsComposeView;
class CAknsLayeredBackgroundControlContext;
class CAlfAnchorLayout;
class CFsTreeList;
class CFSEmailCRHandler;
class CFSEmailUiAttachmentsListVisualiser;
class CFreestyleEmailUiAttachmentsListControl;
class MFSEmailUiFolderListCallback;
class MFSEmailUiSortListCallback;
class MFsControlButtonInterface;
class CFSEmailUiSendAttachmentsListVisualiser;
class TFSProgress;
class CFSEmailUiDownloadManagerVisualiser;
class CFreestyleEmailUiDownloadManagerControl;
class CAlfLayout;
class CFSEmailUiSearchListVisualiser;
class CFSEmailUiStatusIndicator;
class CFsEmailUiHtmlViewerView;
class CAknWaitNoteWrapper;
class CFSEmailUiActionMenu;
class CFSEmailUiShortcutBinding;
class CFSEmailUiWizardObserver;
class CDocumentHandler;
class CAknProgressDialog;
class CEikProgressInfo;
class CFsEmailUiViewBase;
class CVPbkContactManager;
class CESMRIcalViewer;
class CFsEmailFileHandleShutter;
class CFsLayoutManager;
class CIAUpdate;
class CIAUpdateParameters;
class CFSEmailUiAutosyncMonitor;
class CEUiExitGuardian;
class CAknNavigationDecorator;
class CFSEmailDownloadInfoMediator;
class MAknNaviDecoratorObserver;
class CCustomStatuspaneIndicators;

/**
 * TDisplayImagesCache
 * 
 * Non-persistant cache for display images per message. When user allows images to be downloaded
 * for a message, that message's id will be stored into the cache. Later if the message is re-opened 
 * and it is found in cache, images are allowed to be downloaded no matter what the global setting 
 * is. Message is added to cache only when the user presses the button, it will be removed from cache
 * if the mailbox or the message itself is deleted.
 */
class TDisplayImagesCache
    {
public:
    
    /**
     * Destructor
     */
    ~TDisplayImagesCache();

    /**
     * Add message to cache.
     */
    void AddMessageL( const CFSMailMessageBase& aMsg );
    
    /**
     * Remove message from cache.
     */
    void RemoveMessage( const CFSMailMessageBase& aMsg );
    
    /**
     * Check if the message is in cache. Returns ETrue if the message is found.
     */
    TBool Contains( const CFSMailMessageBase& aMsg ) const;    
    
    /**
     * Removes message from cache.
     */
    void RemoveMessage( const TFSMailMsgId& aBoxId, const TFSMailMsgId& aMsgId );
    
    /**
     * Removes mailbox from cache.
     */
    void RemoveMailbox( const TFSMailMsgId& aBoxId );

private: // internal methods
    
    /**
     * Adds message to cache.
     */
    void AddMessageL( const TFSMailMsgId& aBoxId, const TFSMailMsgId& aMsgId );

    /**
     * Check if the message is in cache. Returns ETrue if the message is found.
     */
    TBool Contains( const TFSMailMsgId& aBoxId, const TFSMailMsgId& aMsgId ) const;    

    /**
     * Returns index for given mailbox in cache or KErrNotFound if the mailbox cannot
     * be found.
     */
    TInt MailBoxIndex( const TFSMailMsgId& aBoxId ) const;
    
    /**
     * Adds new mailbox and returns index in cache.
     */
    void AddMailBoxL( const TFSMailMsgId& aBoxId, TInt& aCacheIndex );

private:    
    
    /**
     * Cache item.
     */
    class TItem  
        {
    public:
        /**
         * Constructor
         */        
        TItem( const TFSMailMsgId& aBoxId );
        
        /**
         * Destructor
         */
        ~TItem();
        
        /**
         * Returns ETrue if given message Id is found in this box.
         */
        TBool Contains( const TFSMailMsgId& aMsgId ) const;

        /**
         * Adds new message Id into box.
         */
        void AddMessageL( const TFSMailMsgId& aMsgId );

        /**
         * Removes message from box.
         */
        void RemoveMessage( const TFSMailMsgId& aMsgId );
    
        /**
         * Comparator for TLinearOrder, compares two items
         */
        static TInt CompareItem( const TItem& aItem1, const TItem& aItem2 );
    
    private:       

        /**
         * Comparator for TLinearOrder, compares two message Ids
         */
        static TInt CompareMsgId( const TFSMailMsgId& aId1, const TFSMailMsgId& aId2 );
        
        /**
         * Returns index of the message in box or KErrNotFound.
         */
        TInt MessageIndex( const TFSMailMsgId& aMsgId ) const;
    
    private:
        
        // Mailbox Id        
        TFSMailMsgId iMailBoxId;
        
        // Message Ids
        RArray<TFSMailMsgId> iMessageIds; 
        };
    
    // Cache
    RArray<TItem> iCache;    
    };


class CFreestyleEmailUiAppUi : public CAknViewAppUi,
    			               public MAlfActionObserver,
    			               public MFSMailRequestObserver,
    			               public MFSMailEventObserver,
    			               public MMsvSessionObserver,
    			               public MIAUpdateObserver,
    			               public MFSEmailUiGenericTimerCallback,
    			               public MFreestyleEmailUiPropertyChangedObserver
    {
friend class CMailListUpdater;
public:

    CFreestyleEmailUiAppUi( CAlfEnv& aEnv );
    void ConstructL();
    ~CFreestyleEmailUiAppUi();
	CAlfDisplay& Display();
	CAlfEnv& AlfEnv();
   // void HandleCommandL(TInt aCommand);
    void HandleActionL(const TAlfActionCommand& aActionCommand);
	// <cmail>
    void ProcessCommandL( TInt aCommand );
	// </cmail>
	void HandleCommandL( TInt aCommand );

	// View switching functions
	void EnterFsEmailViewL( TUid aViewId );
	void EnterFsEmailViewL( TUid aViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage );
	void EnterPluginSettingsViewL( TUid aViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage );
	void ReturnFromPluginSettingsView();
    void ViewActivatedExternallyL( TUid aViewId );

    void SetSwitchingToBackground( TBool aValue );
    TBool SwitchingToBackground() const;

	/**
	 * Tries to activate the local view where the current view was navigated from.
	 * Activates the grid view if no other previous view available.
	 * @return UID of the view to be activated
	 */
	TUid ReturnToPreviousViewL( const TDesC8& aCustomMessage = KNullDesC8 );

	void ReturnFromHtmlViewerL( TBool aMessageWasDeleted = EFalse );

    // Clears the navigation history used with Back functionality.
    // Thus, next call to ReturnToPreviousViewL() will return to the main grid.
    void EraseViewHistory();

	// Create new mail with compose command
	void CreateNewMailL();

	// Returns pointer to the current active view. Ownership not transferred.
	CFsEmailUiViewBase* CurrentActiveView();
    // Returns pointer to the previous active view. Ownership not transferred.
    CFsEmailUiViewBase* PreviousActiveView();
    // Tells if plugin settings view is currently open
    TBool IsPluginSettingsViewActive();

	// getter to generic components
	CFSEmailUiLayoutHandler* LayoutHandler();
	CFreestyleEmailUiTextureManager* FsTextureManager();
//	CFSEmailUiStatusIndicator* StatusIndicator();

	// Functions used by grid and viewer
	TInt NewEmailsInModelL();
	TInt EmailsInModelL();
	TInt EmailIndexInModel();

    /**
    * LaunchEditorL
    * Launch editor with the given address in TO-field and focus in MSG-field
    * @param aToAddress The email address to put in TO-field on startup. Ownership
    * is not transferred.
    */
	void LaunchEditorL( CFSMailAddress* aToAddress );

    /**
    * LaunchEditorL
    * Launch editor
    * @param aMode The mode in which the editor is started
    * @param aParams Editor launch parameters
    */
	void LaunchEditorL( TEditorLaunchMode aMode, const TEditorLaunchParams& aParams );

	CFSMailClient* GetMailClient();
	CFSEmailCRHandler* GetCRHandler();

	// Gets active mailbox pointer from the UI
	// Ownership IS NOT transferred to the callers
	CFSMailBox* GetActiveMailbox();

	void SubscribeMailboxL( TFSMailMsgId aActiveMailboxId );
	void SetActiveMailboxL( TFSMailMsgId aActiveMailboxId, TBool aAutoSync = ETrue );
	void StartMonitoringL();
	CMsvSession* GetMsvSession();

	TFSMailMsgId GetActiveBoxInboxId();
	TFSMailMsgId GetActiveMailboxId();

    /**
    * Show folder list or sort list as control bar popup list.
    *
    * @param aCurrentSortCriteria Current sort criteria in mail list
    * @param aFolderId/Type Folder id/type of the currently active mail folder
    * @param aCallback Callback pointer to use when reporting user selection
    * @param aButton Selected control bar button, used when positioning the list
    */
	void ShowFolderListInPopupL( const TFSMailMsgId aFolderType,
                                 MFSEmailUiFolderListCallback* aCallback,
								 MFsControlButtonInterface* aButton );
	void ShowSortListInPopupL( const TFSMailSortCriteria aCurrentSortCriteria,
	                           const TFSFolderType aFolderType,
							   MFSEmailUiSortListCallback* aCallback,
							   MFsControlButtonInterface* aButton );
	void FolderPopupClosed();

	// Transition effects for Alf layouts, call these in doactivate and dodeactivate.
	TBool ViewSwitchingOngoing();

	// Force Sync on active mailbox.
	void SyncActiveMailBoxL();

	// Stop mailbox sync
	void StopActiveMailBoxSyncL();

	// From sync observer
	void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

    /**
     * Helper method to be called from Settings. Launches mailbox setup wizard.
     */
     void LaunchWizardL();

    // Overriden exit
    void Exit();

    void ExitNow();

    const CFSEmailUiShortcutBinding& ShortcutBinding() const;

    // Navigation functions, used mainly from viewer
    TBool IsNextMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId, TFSMailMsgId& aFoundNextMsgFolder );
    TBool IsPreviousMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId, TFSMailMsgId& aFoundPrevMsgFolder );
    TInt MoveToNextMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId );
    TInt MoveToPreviousMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId );
    TInt MoveToPreviousMsgAfterDeleteL( TFSMailMsgId aFoundPreviousMsgId );

    /** Returns document handler. */
    CDocumentHandler& DocumentHandler();

    /**
     * Returns reference file handle shutter. The shutter is set to observe
     * embedded application exit events delegated by the appui's document
     * handler and it closes the set file handle on exit event. */
    CFsEmailFileHandleShutter& FileHandleShutter();

    // Returns unmodifiable reference to the current title pane text
    const TDesC& TitlePaneTextL();
    // Sets given text to title pane
    void SetTitlePaneTextL( const TDesC& aText );
    // Resets title pane text to its default value
    void ResetTitlePaneTextL();
    // Sets active mailbox name to title pane
    void SetActiveMailboxNameToStatusPaneL();

    // Set connection status icon of title pane to visible
    void ShowTitlePaneConnectionStatus();
    // Set connection status icon of title pane to hided
    void HideTitlePaneConnectionStatus();
    // Draw connection status icon into title pane if set as visible.
    // It is also possible to force connection indicator to wanted value
    // to achieve better user experience.
    void UpdateTitlePaneConnectionStatus( TForcedStatus aForcedStatus = ENoForce );
    // Return pointer to custom status pane indicators object
    // Which can be used to show priority and followup flags
    // on navi pane area. (Real navipane is not used)
    CCustomStatuspaneIndicators* GetStatusPaneIndicatorContainer();

    // Getter for appui owned virtual phonebook manager
    CVPbkContactManager& GetVPbkManagerL();

    // MessageReader implementation
    void StartReadingEmailsL();
    TBool MessageReaderSupportsFreestyle();

    static TBool AppUiExitOngoing();

    // Meeting request viewer instance, owned by the appui
    CESMRIcalViewer* MrViewerInstanceL();

    // Get Mail viewer instance, owned by the AppUi
    CFsEmailUiHtmlViewerView& MailViewer();

    // Get Folder list instance, owned by the AppUi
    CFSEmailUiFolderListVisualiser& FolderList();

    // Get Launcher grid instance, owned by the AppUi
    CFSEmailUiLauncherGridVisualiser& LauncherGrid();

    // View loading in idle
    TInt ViewLoadIdleCallbackFunctionL();

    /**
     * Sets the manual mailbox syncronisation/connection flag if the
     * user chose to synchorise/connect mail boxes from the menu
     */
    void ManualMailBoxSync( TBool aManualMailBoxSync );
    void ManualMailBoxSyncAll( TBool aManualMailBoxSyncAll );
    void ManualMailBoxConnectAll( TBool aManualMailBoxConnectAll );
    void ManualMailBoxDisconnectAll( TBool aManualMailBoxConnectAll );

	// For showing mailbox query
	void GridStarted( TBool aStartedFromOds );
    void ShowMailboxQueryL();

    void DoAutoConnectL();

    CFSEmailDownloadInfoMediator* DownloadInfoMediator();

    // <cmail>
    TDisplayMode DisplayMode() const;
    // </cmail>

    
	
	
	/**
     * Returns the current flip status.
     * @return True if the flip is open, false otherwise.
     */
    TBool IsFlipOpen() const;

    TDisplayImagesCache& DisplayImagesCache();

    // Set flag for judging if there is a embedded app in FSEmail.
    void SetEmbeddedApp( TBool aEmbeddedApp );
    
    // Return embedded app.
    TBool EmbeddedApp() const;
    
    // Set flag for judging if previous app is embedded.
    void SetEmbeddedAppToPreviousApp( TBool aEmbeddedApp );
    
    // if previous app is embedded.
    TBool EmbeddedAppIsPreviousApp() const;
    
    // Set flag for judging if email editor started from embedded app.
    void SetEditorStartedFromEmbeddedApp( TBool aEmbeddedApp );
    
    // if email editor started from embedded app.
    TBool EditorStartedFromEmbeddedApp() const;

public: //from MFSMailEventObserver
    /**
     * Framework event message.
     * Used here to catch the events thrown from new mailbox creation process.
     *
     * @param aEvent One of pre-defined events in TFSMailEvent
     * @param aMailbox maibox related for this event
     * @param aParam1 intepretation is event specific (see TFSMailEvent descriptions)
     * @param aParam2 intepretation is event specific (see TFSMailEvent descriptions)
     * @param aParam3 intepretation is event specific (see TFSMailEvent descriptions)
     */
    void EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
                 TAny* aParam1, TAny* aParam2, TAny* aParam3);

public:
	void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

public: // MIAUpdateObserver
    virtual void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates );
    virtual void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow );
    virtual void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResultDetails );

public:

    /**
    * StartEndKeyCapture
    * Start key capture for end-key
    */
    void StartEndKeyCapture();

    /**
    * StopEndKeyCapture
    * Stop key capture for end-key
    */
    void StopEndKeyCapture();

    /**
     * Accessor for navipane decorator object.
     * @param aViewId: Caller identify itself by giving the view Id (see FreestyleEmailUiConstants.h)
     * This is used for choosing correct decorator.
     * @return instance of correct navidecorator. NULL if aView doesn't match.
     */
    CAknNavigationDecorator* NaviDecoratorL( const TUid aViewId );

public:

    /**
     * From MFSEmailUiGenericTimerCallback.
     * Generic timer event callback.
     */
    virtual void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );

    /**
    * Sets visibility for focus.
    *
    * @param aVisible EFalse if focus should be hidden.
    * @return whether or not focus was already visible.
    */
    TBool SetFocusVisibility( TBool aVisible );

    /**
    * IsFocusShown
    * Used for finding out if focus is visible.
    * @return whether or not the focus is visible.
    */
    TBool IsFocusShown() const;


public: // From MFreestyleEmailUiPropertyChangedObserver.

	void PropertyChangedL( TInt aValue );


protected:
	TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void HandleResourceChangeL(TInt aType);
	void HandleWsEventL(const TWsEvent &aEvent,
 	                    CCoeControl *aDestination);

 	void HandleForegroundEventL( TBool aForeground );
    MCoeMessageObserver::TMessageResponse HandleMessageL( TUint32 aClientHandleOfTargetWindowGroup,
                                                          TUid aMessageUid,
                                                          const TDesC8& aMessageParameters );

private:
    void DoHandleResourceChangeL(TInt aType);
	void UpdateSettingsListModelL();
	CAlfTexture& LoadItemIconL( TInt aUid );
    void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap,
                                  CFbsBitmap*& aMaskBitmap);

    /**
     * Helper method for launching a setup wizard, or handling data received from one.
     */
    void DoWizardStartupActionsL();

    TBool IsViewInHistoryStack( TUid aViewId ) const;

    void ReturnToViewL( TUid aViewId, TUid aCustomMessageId = TUid::Null(), const TDesC8& aCustomMessage = KNullDesC8 );

    // Checks updates from IAD client
    void CheckUpdatesL();
    void AllUpdateActivitiesCompleted();

	// This async callback functionality is needed because UI and app construction
	// cannot jam while the new mailbox query has been displayed
   	static TInt DisplayCreateQueryL( TAny* aSelfPtr );
   	void DisplayCreateMailboxQueryL();

   	// Send self to background.
   	void SendToBackground();

   	/*
   	 * Called from ConstructL during start of application.
   	 * Constructs navipane(s) that are used by different views.
   	 */
   	void ConstructNaviPaneL();

    // Create custom status pane indicators. (priority and followup)
    void CreateStatusPaneIndicatorsL();
    // Delete custom status pane indicators
    void DeleteStatusPaneIndicators();

    // Load specified resource file, file name may (and should) contain wild
    // cards. aFilePath should contain drive letter if some specific drive
    // is wanted to be searched first. Returns the resource file offset of the
    // found file. Leaves if resource file not found.
    TInt LoadResourceFileL( const TDesC& aFileName,
                            const TDesC& aFilePath );

private:
	// ALF environment NOTE: Display removed from Alfred version
    CAlfEnv* iEnv;

    // Pointers to current and previous active views
    CFsEmailUiViewBase* iCurrentActiveView; // not owned
    CFsEmailUiViewBase* iPreviousActiveView; // not owned
    CStack<CFsEmailUiViewBase, EFalse>* iNavigationHistory; // items not owned

 	CFSEmailDownloadInfoMediator* iDwnldMediator;

     // Pointers to actual display control groups containing UI elements
    CAlfControlGroup* iGridControlGroup;
  	CAlfControlGroup* iMailListControlGroup;
    CAlfControlGroup* iEmailViewerControlGroup;
  	CAlfControlGroup* iFolderListControlGroup;
  	CAlfControlGroup* iSettingsListControlGroup;
   	CAlfControlGroup* iSendingAnimationControlGroup;
  	CAlfControlGroup* iMsgDetailsControlGroup;
  	CAlfControlGroup* iAttachmentControlGroup;
  	CAlfControlGroup* iComposerControlGroup;
  	CAlfControlGroup* iDownloadManagerControlGroup;
  	CAlfControlGroup* iSearchResultControlGroup;
  	CAlfControlGroup* iStatusIndicatorControlGroup;
  	CAlfControlGroup* iHtmlViewerControlGroup;

 	// FS Email UI specific layout handler object
    CFSEmailUiLayoutHandler* iLayoutHandler;

	// FS specific layout
        //<cmail> layout manager doesn't exists in cmail
	//CFsLayoutManager* iLayoutManager;
        //</cmail>

	// FS Email UI specific texture mngr object
 	CFreestyleEmailUiTextureManager* iFsTextureManager;

	// Main UI grid control and view
  	CFSEmailUiLauncherGrid* iMainUiGrid;
    CFSEmailUiLauncherGridVisualiser* iMainUiGridVisualiser;

	// Mail view
	CFSEmailUiMailListVisualiser* iMailListVisualiser;

  	// Plain text viewer view, control inside text view;
    CFSEmailUiMailViewerVisualiser* iViewerVisualiser;

	// Message editor view
	CNcsComposeView* iComposeView;

	// Html viewer view
	CFsEmailUiHtmlViewerView* iHtmlViewerView;

	// Message details view
	CFSEmailUiMsgDetailsVisualiser* iMsgDetailsVisualiser;

	// Folder list view
	CFSEmailUiFolderListVisualiser* iFolderListVisualiser;

	// Attachment list
	CFSEmailUiAttachmentsListVisualiser* iAttachmentListVisualiser;
	CFreestyleEmailUiAttachmentsListControl* iAttachmentHanlerControl;

    // Download manager removed.

	// Search result list view
	CFSEmailUiSearchListVisualiser* iSearchListVisualiser;

	// Status indicator
	CFSEmailUiStatusIndicator* iStatusIndicator;

    // Mailclient, list of mailboxes, active mailbox and
    // latest opened folder
    CFSMailClient* iMailClient;

 	// Active mailbox, pointer owned by the APP UI!
 	CFSMailBox* iActiveMailbox;

	TFSMailMsgId iActiveBoxInboxId;
	TFSMailMsgId iActiveMailboxId;

	// Mail sending progress dialog
	CAknProgressDialog* iSendingProgressDialog;
	CEikProgressInfo* iSendingProgressInfo;

	// Phonebook engine and resource handle
	TInt iPhCltResHandle;

	// Central repository handler
	CFSEmailCRHandler*	iCRHandler;

	// End key capture handles
	TInt32 iEndCaptureHandle;
	TInt32 iEndUpDownCaptureHandle;

    CMsvSession* iMsvSession;

    CFSEmailUiActionMenu* iActionMenuHandler;

	RArray<TFSMailMsgId> iSubscribedMailBoxes;
	RArray<TUint> iSubscribedMailBoxesIds;


	// Helper class for keyboard shortcut handling
	CFSEmailUiShortcutBinding* iShortcutBinder;

	CFSEmailUiWizardObserver* iWizardObserver;

    CDocumentHandler* iDocumentHandler;
    CFsEmailFileHandleShutter* iFileHandleShutter;

	TBool iConstructComplete;

	TInt iSyncStatusReqId;

	TBool iIsForeground;

  	CIdle* iViewLoadIdle;
	TCallBack* iViewLoadIdleCallback;
	TBool iIsCaptured;

    CVPbkContactManager* iVpkContactManager;

    //This is used to pass needed information to messagereader
	HBufC8* iCustomMessageToMessageReader;

	// Meeting request viewer instance
  	CESMRIcalViewer* iMRViewer;

  	// Flag to be used to disable online/offline note in automated
  	// Mailbob online
  	TBool iAutomatedMailBoxOnline;

	// Value indicating that msg reader is supported, KErrNone if supported.
  	TInt iMsgReaderIsSupported;
  	// For IAD
    CIAUpdate* iUpdateClient;
    CIAUpdateParameters* iUpdateParameters;

    // For feature manager
    TBool iFeatureManagerInitialized;

    //ETrue if user started the sync/connect all
    TBool iManualMailBoxSync;
    TBool iManualMailBoxSyncAll;
    TBool iManualMailBoxConnectAll;
    TBool iManualMailBoxDisconnectAll;

    // ETrue if the screen layout has chaged while the application was on background
    TBool iPendingLayoutSwitch;

    // Startup mailbox creation query flag.
    TBool iStartupQueryShown;
    TBool iGridStarted;
    TBool iGridFirstStartFromOds;
    TBool iSettingsViewActive;
    TBool iIADUpdateCompleted;

    CFSEmailUiAutosyncMonitor* iAutoSyncMonitor;

    // Async callback for create mailbox query
    // This is needed because query can not stop execution of UI construction
    CAsyncCallBack* iNewBoxQueryAsyncCallback;

    // Exit guardian.
    CEUiExitGuardian* iExitGuardian;

    // navipane decorator
    // (used at least in mail viewer)
    CAknNavigationDecorator* iNaviDecorator2MailViewer;

    // Resource file offset for message editor external resources
    TInt iMsgEditorResourceOffset;

    // Resource file offset for FSMailServer external resources
    TInt iFSMailServerResourceOffset;

    // Custom status pane indicators
    CCustomStatuspaneIndicators* iStatusPaneIndicators;
    TBool iConnectionStatusVisible;
    CFSEmailUiGenericTimer* iConnectionStatusIconAnimTimer;
    CFbsBitmap* iConnectionIconBitmap;
    CFbsBitmap* iConnectionIconMask;
    TInt iConnectionStatusIconAngle;
    TForcedStatus iForcedConnectionStatus;

    // Set true, when application is going to be switched to backgound
    // after the view deactivation.
    TBool iSwitchingToBackground;
    
    TDisplayImagesCache iDisplayImagesCache;

    // For handling the flip state.
    CFreestyleEmailUiPropertySubscriber* iPropertySubscriber;
    TBool iFlipOpen;

    /// Focus visibility state
    TBool iFocusVisible;
    TBool iTouchFeedbackCreated;

    // Embedded app flag.
    TBool iHasEmbeddedApp;
    
    // Flag for judging if previous app is embedded app.
    TBool iPreviousAppEmbedded;
    
    // Flag for judging if email editor started from embedded app.
    TBool iEditorStartedFromEmbeddedApp;
    };


/**
*  CFSEmailUiAutosyncMonitor.
* Needed for auto sync feature, not wanted when roaming.
*/
class CFSEmailUiAutosyncMonitor : public CActive
    {
    public:
        enum TNetworkStatus
            {
            EStatusUnknown = 0,
            EHomenetwork,
            ERoaming,
            ERequestCancelled
            };

        enum TMonitoringStatus
            {
            EInactive = 0,
            ERequestingNetworkStatus,
            EDone
            };

        static CFSEmailUiAutosyncMonitor* NewL( CFreestyleEmailUiAppUi& aAppUi );
        ~CFSEmailUiAutosyncMonitor();

    public:
        void StartMonitoring( );
        void StopMonitoring( );

    private:
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    private:
        CFSEmailUiAutosyncMonitor( CFreestyleEmailUiAppUi& aAppUi );
        void ConstructL();

    private:
        TMonitoringStatus iMonitoringStatus;
        CFreestyleEmailUiAppUi& iAppUi;
        RTelServer iServer;
        RMobilePhone iPhone;
        RMobilePhone::TMobilePhoneRegistrationStatus iRegisterationStatus;
        RTelServer::TPhoneInfo iPhoneInfo;
    };



#endif // __FREESTYLEEMAILUI_APPUI_H__

