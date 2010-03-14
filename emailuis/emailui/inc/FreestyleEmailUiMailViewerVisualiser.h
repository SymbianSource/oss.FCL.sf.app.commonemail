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
* Description:  FreestyleEmailUi mail viewer visualiser definition
*
*/

 
#ifndef __FREESTYLEEMAILUI_MAILVIEWERVISUALISER_H__
#define __FREESTYLEEMAILUI_MAILVIEWERVISUALISER_H__

// SYSTEM INCLUDE FILES    
#include <e32base.h>
#include <finditemengine.h> 
#include <AknNaviDecoratorObserver.h>
//<cmail>
#include "fstextviewerobserver.h"
//#include <aknwaitnotewrapper.h>
#include <cstack.h>
#include <AknProgressDialog.h>
//</cmail>
//<cmail>
#include "mfsmailrequestobserver.h"
#include "mesmricalviewercallback.h"
#include "mesmricalviewerobserver.h"
//</cmail>

// INTERNAL INCLUDE FILES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiListVisualiser.h"
#include "FreestyleEmailDownloadInformationMediator.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiControlBarCallback.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiContactHandlerObserver.h"
#include "FreestyleEmailUiMailViewerRichText.h"

// FORWARD DECLARATIONS
class CFSMailMessage;
class CFreestyleEmailUiAppUi;
class CFsTextViewer;
class CFsTextViewerWatermark;
class CFsSmileyDictionary;
class CFreestyleEmailUiMailViewerControl;
class CAiwServiceHandler;
class CBrowserLauncher;
class CFSEmailUiStatusIndicator;
class CFlagSelectionGlobalNoteHandler;
class CAknGlobalListQuery;
class CAknGlobalNote;
class CAknWaitDialog; //<cmail>
class CAknNavigationDecorator;

// list of different types of action menus to be shown.
enum TActionMenuType
	{
	ENoActionMenuFocused = 0,
	EFocusOnNumberWithinMessage,
	EFocusOnHyperLinkWithinMessage,
	EFocusOnNameInAddressField,
	EFocusOnEMailInBodyText,
	EFocusOnAttachmentName,
	EFocusOnAttachmentsText
	};

	
/**
* CFSEmailUiMailViewerVisualiser implements message viewer view.
*		It uses FS Generic TV component, which displays rich text with hotspots.
*		View contains hotspot related action menu handling and many options menu
*/
class CFSEmailUiMailViewerVisualiser : public CFsEmailUiViewBase, 
                                       public MFsTextViewerObserver, 
                                       public MFSEmailDownloadInformationObserver,
                                       public MFSMailRequestObserver,
                                       public MFSEmailUiFolderListCallback, 
                                       public MProgressDialogCallback, //cmail
                                       public MFSEmailUiGenericTimerCallback,
                                       public MFSEmailUiContactHandlerObserver,
                                       public MESMRIcalViewerCallback,
                                       public MESMRIcalViewerObserver,
                                       public MAknNaviDecoratorObserver
	{
public: // construction and destruction
    static CFSEmailUiMailViewerVisualiser* NewL( CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aMailViewerControlGroup );    
    static CFSEmailUiMailViewerVisualiser* NewLC( CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aMailViewerControlGroup );               
	~CFSEmailUiMailViewerVisualiser();
	void PrepareForExit();

public: // methods
 	CAlfControl* ViewerControl();
 	void SetMskL();
	// Event handling forwarded from control
	TBool OfferEventL( const TAlfEvent& aEvent );
	
	// Handle accept/decline/tentative/remove commands given for meeting request message directly from list UI.
	void HandleMrCommandL( TInt aCommandId, TFSMailMsgId aMailboxId, TFSMailMsgId aFolderId, TFSMailMsgId aMessageId );
    
	void CompletePendingMrCommand();
	void CancelPendingMrCommandL();
  
	// Helper funcitons to get viewed message ID and Folder id
	TFSMailMsgId ViewedMessageFolderId();
	TFSMailMsgId ViewedMessageId();	
	
	// Called from global flag selection dialog
	void FlagselectionCompleteL( TInt aSelection );
	
    void HandleMailBoxEventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
        TAny* aParam1, TAny* aParam2, TAny* aParam3 );

public: // from CFsEmailUiViewBase
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    void HandleCommandL( TInt aCommand );

    TUid Id() const;               

    void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
    void HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType aType );
    void HandleForegroundEventL();

protected: // from CFsEmailUiViewBase
    /*void DoDoActivateL( const TVwsViewId& aPrevViewId,
                        TUid aCustomMessageId,
                        const TDesC8& aCustomMessage );*/
    void ChildDoDeactivate();
    void ActivateControlGroup( TInt aDelay = 0 );
    void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;
    void NavigateBackL();

public: // from MFsTextViewerObserver
    void HandleTextViewerEventL( TFsTextViewerEvent aEvent );
    
public : // from MFSEmailDownloadInformationObserver
    void RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart );

public : // for MFSMailRequestObserver
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

public: // from MFSEmailUiFolderListCallback
    void FolderSelectedL( TFSMailMsgId aSelectedFolderId,
                          TFSEmailUiCtrlBarResponse aResponse );         

public: // from MAknBackgroundProcess
    //<cmail> we are using different wait note
    //void StepL();
	//TBool IsProcessDone() const;
	//void ProcessFinished();
	//void DialogDismissedL( TInt /*aButtonId*/ ); 
	//TInt CycleError( TInt aError );

    // from MProgressDialogCallback
    void DialogDismissedL( TInt aButtonId);
    //</cmail>

public: // from MFSEmailUiGenericTimerCallback
    void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );

public: // from MFSEmailUiContactHandlerObserver
    void OperationCompleteL( TContactHandlerCmd aCmd,
        const RPointerArray<CFSEmailUiClsItem>& aContacts );
    void OperationErrorL( TContactHandlerCmd, TInt aError );

public: // from MESMRIcalViewerCallback
    void ProcessAsyncCommandL( TESMRIcalViewerOperationType aCommandId,	const CFSMailMessage& aMessage, MESMRIcalViewerObserver* aObserver = NULL );   
	void ProcessSyncCommandL( TESMRIcalViewerOperationType aCommandId, const CFSMailMessage& aMessage );   
    TBool CanProcessCommand( TESMRIcalViewerOperationType aCommandId  ) const;   
       
public: // from MESMRIcalViewerObserver
    void OperationCompleted( TIcalViewerOperationResult aResult );        
    void OperationError( TIcalViewerOperationResult aResult );     

public: // from MAknNaviDecoratorObserver
    
    /**
     * Called by navigationDecorator if navi arrows (left/right) have been pressed
     * @aParam aEventID specifies a tapping of either left or right arrow
     */
    void HandleNaviDecoratorEventL( TInt aEventID );

// <cmail> Toolbar    
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
    
    /**
     *  @see CFsEmailUiViewBase::OfferToolbarEventL
     */
    void OfferToolbarEventL( TInt aCommand );

    /**
     * @see CFsEmailUiViewBase::ToolbarResourceId
     */
    TInt ToolbarResourceId() const;
// </cmail> Toolbar 
        
private: // methods

	// Class constructing
	CFSEmailUiMailViewerVisualiser( CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi,
									 CAlfControlGroup& aMailViewerControlGroup );
	void ConstructL();

	// Constructing & refreshing the view
	void AddBackgroundPicturesL();
	void ClearMailViewer();
	void UpdateMailViewerL();
    void RefreshL( TBool aFirstStart = EFalse );
    // Callback function to encapsulate actions which are deferred form RefreshL() to 
    // make it complete faster
    static TInt DoPostRefresh( TAny* aSelfPtr );
    void PostRefreshL();
	void SetActionButtonIconAndHighLight();
	void CheckMessageStructureL();
	void UpdateDownloadIndicatorL( TFSProgress::TFSProgressStatus aStatus = TFSProgress::EFSStatus_Waiting );
	
	// Update our message pointer and change observed message
	void UpdateMessagePtrL( TFSMailMsgId aNewMailbox, TFSMailMsgId aNewFolder, TFSMailMsgId aNewMessage );
	void UpdateMessagePtr( CFSMailMessage* aNewMessagePtr );
	    
    // Message stack handling. Ownership of message is transferred when succesful.
    void PushMessageL( CFSMailMessage* aMessage, TBool aIsEmbedded );
    CFSMailMessage* PopMessage();
    void EraseMessageStack();
	// Html viewer opening
	void LaunchHtmlViewerL();
	
	// Changing message states
 	void SetMessageFollowupFlagL(); 
 	void ChangeMsgReadStatusL( TBool aRead, TBool aCmdFromMrui );
	
	// For deleting current mail
	// Return value indicating whether message was deleted
	TBool DeleteMailL( CFSMailMessage& aMessagePtr, TBool aReturnPreviousView, TBool aDisableNote = EFalse );

	// Options menu dimm checking
	TBool ShowActionsMenuInOptionsL() const;
	TBool ShowDownloadManagerMenuInOptions() const;
	TBool ShowNextMessageMenuInOptions() const;
	TBool ShowPreviousMessageMenuInOptions() const;
	void ShowNextMessageL();
	void ShowPreviousMessageL();

	// Action menu lauching and selection handling
	void LaunchActionMenuL();
	void CreateActionMenuItemsL( TActionMenuType aActionMenuType );
	void HandleActionMenuCommandL( TActionMenuCustomItemId aSelectedActionMenuItem, TActionMenuType aActionMenuType );


	// Action menu command handling
	void OpenLinkInIntranetL( TBool aMenuSelection = ETrue );
 	void OpenHotSpotUrlInBrowserL( CFindItemEngine::SFoundItem& aHotSpot );
 	void CallToBodyHotSpotNumberL( CFindItemEngine::SFoundItem& aHotSpot );
 	void ComposeMailL();
	void WriteEmailToHotSpotAddressL( CFindItemEngine::SFoundItem& aHotSpot );

	void OpenContactDetailsL();

	void LaunchRemoteLookupL( const TDesC& aHotspotText ) const;
	void AddToBookmarksL( const CFindItemEngine::SFoundItem& aBodyHotspotData ) const;
	
	void CallHotSpotAddressL();
	void CreateMessageL() const;

	TBool IsCopyToClipBoardAvailableL() const;
	void CopyCurrentHotspotToClipBoardL() const;
	
	// Attachment handling
	TBool ShowOpenAttachmentOptionL();
	TBool ShowSaveAttachmentOptionL();
	TBool ShowDownloadOptionL();
	TBool ShowMskDownloadOptionL();
	TBool ShowCancelDownloadOption();
	void OpenAttachmentL();
	void OpenAttachmentsViewL();
	void StartDowloadingAttachmentsL();
	void CancelDowloadingAttachmentsL();
	void SaveAllAttachmentsL();
	void RemoveFetchedAttachmentL();

	// Helper method for creating Part data needed when calling to download mediator
	TPartData MailData();

	// Content fecthing

	// list of different types of content
	enum TFetchedContentType
		{
		EMessagePlainTextBodyPart = 0,
		EMessageHtmlBodyPart,
		EMessageStructure
		};
	TBool StartFetchingBodyAfterOpeningL() const;
	TBool MessageStructureKnown( CFSMailMessage& aMsg ) const;
	TBool MessagePartFullyFetchedL( TFetchedContentType aFetchedContentType ) const;
	void StartFetchingMessagePartL( CFSMailMessage& aMessagePtr, 	
									TFetchedContentType aFetchedContentType );
	void StartFetchingMessageStructureL( CFSMailMessage& aMsg );
	void StartWaitedFetchingL( TFetchedContentType aFetchedContentType );
	void StartFetchingRemaininBodyLinesIfAtBottomL();
	void CancelFetchings();
	
	void SaveEmailAsContactL( const TDesC& aEmailAddress );
	void SavePhoneNumberAsContactL( const TDesC& aPhoneNumber );

	void SetMailboxNameToStatusPaneL();
	
	TInt ResolveBodyTextSchemaUrlTypeL( CFindItemEngine::SFoundItem aHotSpot );

	void SetActionMenuIconVisbilityL();
	
	void SendEventToAppUiL( TFSMailEvent aEventType );
	
    void DoFirstStartL();
	
    /**
     * Initiates the hotspot action, if currently focused header hotspot has
	 * some direct hotspot action defined (not opening action menu). Returns
	 * ETrue if direct action specified (and initiated) for the focused
	 * hotspot, otherwise EFalse.
     */
    TBool HandleHeaderHotspotActionL( TViewerHeadingHotspotType aHotspotType );
	
    /**
     * Handles updating of navigation pane content
     * (navi arrows + priority and followup icons)
     * 
     * @param aForESMR Whether the navi pane update is for meeting
     *                 request or not.
     */
    void UpdateNaviPaneL( TBool aForESMR = EFalse );
    
    /**
     * Sets mailviewer's navipane to background
     */
    void HideNaviPane();
    
private: // Methods for moving message

    // Start moving current visible message to different folder
    // Return EFalse if moving is not a supported function 
    TBool OpenFolderListForMessageMovingL();

    // Message is moved and viewer is closed asynchronously after selecting a folder
    // from the move to folder dialog. This is necessary because view swithching is asynchronous.
    static TInt MoveToFolderAndExitL( TAny* aMailViewerVisualiser );

private: // data for moving message to different folder action

    TBool iMoveToFolderOngoing;
    TBool iMovingMeetingRequest;
    CAsyncCallBack* iAsyncCallback;
    TFSMailMsgId iMoveDestinationFolder;

private: // data related to fetching

	TBool iFetchingMessageStructure;
	TInt iCurrentStructureFetchRequestId;

	TBool iFetchingPlainTextMessageBody;
	TInt iCurrentPlainTextBodyFetchRequestId;

	TBool iFetchingHtmlMessageBody;
	TInt iCurrentHtmlBodyFetchRequestId;

	TFetchedContentType iStartAsyncFetchType;

    // Wait note stuff
    //<cmail>
    //CAknWaitNoteWrapper* iAsyncWaitNote;
    CAknWaitDialog* iWaitDialog;
    CAknWaitDialog* iWaitDialogOpening;
    TBool iDialogNotDismissed;
    //</cmail>
	TBool iFetchingAlready;
	TBool iAsyncProcessComplete;

private: // data for fetching timer
	
	CFSEmailUiGenericTimer* iFetchingAnimationTimer;

private: // other class data

    // Handle to ALF environment
	CAlfEnv& iEnv;

	// Stack of open messages. Messages are owned by this stack. 
    CStack<CFSMailMessage, ETrue>* iOpenMessages;
    // Stack of embedded messages. Message are NOT owned by this stack. Similar to iOpenMessages
    // but has NULL pointer in slots which correspond non-embedded messages.
    CStack<CFSMailMessage, EFalse>* iEmbeddedMessages;
	// Mail message pointer, changes always in do activate
	CFSMailMessage* iMessage;

	// Mail box where the iMessage object is.
	CFSMailBox*	iMailBox;

	// AlfControl for capturing right and left navigation key events
	CFreestyleEmailUiMailViewerControl*	iControl;
	
    // Text viewer and control object
    CFsTextViewer* iTextViewer;
    
    // Pointer to TV component control
    CAlfControl* iTextViewerControl; // not owned
	
	// Rich text for the view. 
	CFSEmailUiMailViewerRichText* iViewerRichText;

	// Basic smileys for the 
	CFsSmileyDictionary* iSmDictionary;

	// For opening the intranet app
	CAiwServiceHandler* iIBServiceHandler;

	// Boolean to indicate that layout has changed while some other view was active
	TBool iLayoutChangedWhileNotActive;
	
	// Temporary mail address for opening editor
	CFSMailAddress* iNewMailTempAddress;
	
	// Meeting request observer stuff.
	// Pointer to observer to inform, not owned.
	MESMRIcalViewerObserver* iMrObserverToInform;
	MESMRIcalViewerObserver::TIcalViewerOperationResult iOpResult;
	TBool iMrUiActive;
	
	// Flag to indicate that we are moving into next or previous message
	TBool iNextOrPevMessageSelected;
	
	CAlfControlGroup* iDownloadProgressControlGroup; // owned by AlfEnv
	CFSEmailUiStatusIndicator* iDownloadProgressIndicator;

	// Flag selection handler active object for global note
	CFlagSelectionGlobalNoteHandler* iFlagSelectionHanler;	
	
    TBool iFirstStartCompleted;
    TBool iShowMskDownloadOption;
    
    // Flag to indicate we are viewing an embedded message. Many UI options need to be hidden in that case.
    TBool iEmbeddedMessageMode;
    
    //<cmail>
    // Message pointer to a deleted mail from Mrui
    TFSMailMsgId iDeletedMessageFromMrui;
    //</cmail>
	};


////////////////////////////////////////
// FLAG selection dialog global note handler
/////////////////////////////////////////
class CFlagSelectionGlobalNoteHandler : public CActive
    {
    public:  // Constructors and destructor
        static CFlagSelectionGlobalNoteHandler* NewL( CFSEmailUiMailViewerVisualiser& aViewerVisualiser );
        virtual ~CFlagSelectionGlobalNoteHandler();
        void Cancel();       
        void LaunchFlagListQueryDialogL();       
            
    private: 
    
        void RunL();       
        void DoCancel();       
        TInt RunError( TInt aError );
        
    private:
        CFlagSelectionGlobalNoteHandler(  CFSEmailUiMailViewerVisualiser& aVoIPDialogObserver );

        
    private:      
	    CFSEmailUiMailViewerVisualiser& iViewerVisualiser;
        CAknGlobalListQuery* iGlobalFlagQuery;
        HBufC* iPrompt;
        TInt iSelection;
    };


#endif //__FREESTYLEEMAILUI_MAILVIEWERVISUALISER_H__


