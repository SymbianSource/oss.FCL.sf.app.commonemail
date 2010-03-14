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
* Description:  CFsEmailUiHtmlViewerView class definition
*
*/



#ifndef __FREESTYLEEMAILUI_HTML_VIEWER_VIEW_H__
#define __FREESTYLEEMAILUI_HTML_VIEWER_VIEW_H__

#include <cstack.h>
#include "mfsmailrequestobserver.h"
#include "mesmricalviewercallback.h"
#include "mesmricalviewerobserver.h"
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiContactHandlerObserver.h"
#include "FreestyleEmailUiAttachmentsListModel.h"
#include <AknWaitDialog.h>
#include "FreestyleEmailDownloadInformationMediator.h"
#include "FreestyleEmailUiControlBarCallback.h"

class CFSMailMessage;
class CFreestyleEmailUiAppUi;
class CFsEmailUiHtmlViewerContainer;
class CFreestyleMessageHeaderURL;
class CAknWaitDialog; //<cmail>
class CFlagSelectionNoteHandler;
class CAknGlobalListQuery;
class CAknGlobalNote;
/**
 * Html viewer view.
 */
class CFsEmailUiHtmlViewerView : public CFsEmailUiViewBase,
        public MFSEmailUiContactHandlerObserver,
        public MFSEmailUiAttachmentsStatusObserver,
        public MFSEmailDownloadInformationObserver,
        public MFSMailRequestObserver,
        public MProgressDialogCallback,
		public MFSEmailUiFolderListCallback,
		public MESMRIcalViewerCallback,
    	public MESMRIcalViewerObserver
    {
public:

    /** Two-phased constructor. */
    static CFsEmailUiHtmlViewerView* NewL( CAlfEnv& aEnv, 
                                           CFreestyleEmailUiAppUi& aAppUi,
                                           CAlfControlGroup& aControlGroup );

    /** Destructor. */
    ~CFsEmailUiHtmlViewerView();

public:  // from CAknView

    /**
    * Id
    * @return Id Uid value
    */
    TUid Id() const;

    void HandleStatusPaneSizeChange();
    
    void HandleViewRectChange();
    
    /**
    * HandleCommandL
    * From CAknView, takes care of command handling.
    * @param aCommand Command to be handled
    */
    void HandleCommandL( TInt aCommand );
    
    // Handle accept/decline/tentative/remove commands given for meeting request message directly from list UI.
	void HandleMrCommandL( TInt aCommandId, TFSMailMsgId aMailboxId, TFSMailMsgId aFolderId, TFSMailMsgId aMessageId );
    
	void CompletePendingMrCommand();
	void CancelPendingMrCommandL();
	
    /**
    * ChildDoDeactivate
    * From CFsEmailUiViewBase, deactivate the AknView
    * Remove the container class instance from the App UI's stack and
    * deletes the instance
    */
    void ChildDoDeactivate();

    /**
    * DoExitL
    * Handler called when exiting the view.
    */
    void DoExitL();

    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    void SetMskL();
    void PrepareForExit();
    
public : // for MFSMailRequestObserver
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
    
public: 
    // from MProgressDialogCallback
    void DialogDismissedL( TInt aButtonId);
    
    CFSMailMessage* CurrentMessage();
    CFSEmailUiAttachmentsListModel* CurrentAttachmentsListModel();
    
    // Return view area that can be used for container
    TRect ContainerRect() const;

public: 
    void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
    void HandleMailBoxEventL( TFSMailEvent aEvent,
                              TFSMailMsgId aMailbox, TAny* aParam1, TAny* /*aParam2*/, TAny* /*aParam3*/ );
    
    void HandleEmailAddressCommandL( TInt aCommand, const TDesC& aEmailAddress ); 
    void HandleWebAddressCommandL( TInt aCommand, const TDesC& aUrl );
    
    void DownloadAttachmentL( const TAttachmentData& aAttachment );
    void DownloadAllAttachmentsL();
    void CancelAttachmentL( const TAttachmentData& aAttachment );    
    void OpenAttachmentL( const TAttachmentData& aAttachment );
    void SaveAttachmentL( const TAttachmentData& aAttachment );
    void SaveAllAttachmentsL();
    void RemoveAttachmentContentL( const TAttachmentData& aAttachment );
    void OpenAttachmentsListViewL();
    TBool IsEmbeddedMsgView();
    TBool IsEmbeddedMsgSavingAllowed();
    // Helper funcitons to get viewed message ID and Folder id
    TFSMailMsgId ViewedMessageFolderId();
    TFSMailMsgId ViewedMessageId(); 
    
    TBool GetAsyncFetchStatus();
    void StartFetchingMessageL();
    void ReloadPageL();
public: // from MFSEmailUiContactHandlerObserver
    void OperationCompleteL( TContactHandlerCmd aCmd,
                             const RPointerArray<CFSEmailUiClsItem>& aContacts );
    void OperationErrorL( TContactHandlerCmd, TInt aError );
    TBool IsRemoteLookupSupportedL();

public : // from MFSEmailDownloadInformationObserver
    void RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart );
    
public: // from MFSEmailUiAttachmentsStatusObserver
    void DownloadStatusChangedL( TInt aIndex );
    
public: // from MESMRIcalViewerCallback
    void ProcessAsyncCommandL( TESMRIcalViewerOperationType aCommandId,	const CFSMailMessage& aMessage, MESMRIcalViewerObserver* aObserver = NULL );   
	  void ProcessSyncCommandL( TESMRIcalViewerOperationType aCommandId, const CFSMailMessage& aMessage );   
    TBool CanProcessCommand( TESMRIcalViewerOperationType aCommandId  ) const;  
     
public: // from MESMRIcalViewerObserver
    void OperationCompleted( TIcalViewerOperationResult aResult );        
    void OperationError( TIcalViewerOperationResult aResult );
	
private: // from
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
                           TUid aCustomMessageId,
                           const TDesC8& aCustomMessage );

    /**
     * @see CFsEmailUiViewBase::ToolbarResourceId
     */
    TInt ToolbarResourceId() const;
    
    /**
     * @see CFsEmailUiViewBase::GetInitiallyDimmedItemsL
     */
    virtual void GetInitiallyDimmedItemsL( const TInt aResourceId,
                RArray<TInt>& aDimmedItems ) const;

    /**
     * @see CFsEmailUiViewBase::OfferToolbarEventL
     */
    void OfferToolbarEventL( TInt aCommand );
  
    /**
     * @see CFsEmailUiViewBase::NavigateBackL
     */ 
    void NavigateBackL();
    
    /**
	 * @see CFsEmailUiViewBase::SetStatusBarLayout
	 */ 
    void SetStatusBarLayout();
private: // New functions
    // list of different types of content
    enum TFetchedType
        {
        EMessagePlainTextBodyPart = 0,
        EMessageHtmlBodyPart,
        EMessageStructure
        };
    void ClearMailViewer();
    void LoadContentFromFileL( const TDesC& aFileName );
    void LoadContentFromFileL( RFile& aFile );
    void LoadContentFromUrlL( const TDesC& aUrl );
    void LoadContentFromMailMessageL( CFSMailMessage* aMailMessage, TBool aResetScrollPosition =ETrue );
    void DeleteMailL();
    void HideContainer();
    void ShowContainerL();
    void DynInitZoomMenuL( CEikMenuPane* aMenuPane );
    TInt ZoomLevelIndexL();
    void SetZoomLevelIndexL( TInt aIndex );
    void SaveEmailAsContactL(
        const TDesC& aEmailAddress );
    //new functions for support of HTML viewer
    void CallAdressL( const TDesC& aEmailAddress, TBool aVideoCall );
    void OpenContactDetailsL( const TDesC& aEmailAddress );
    void LaunchRemoteLookupL( const TDesC& aEmailAddress );
    void CreateMessageL( const TDesC& aEmailAddress );
    
    void UpdateDownloadIndicatorL( const TPartData& aPart, 
                                   const TFSProgress& aEvent );
    //fetching related API
    TBool MessagePartFullyFetchedL( TFetchedType aFetchedContentType ) const;
    void StartFetchingMessagePartL( CFSMailMessage& aMessagePtr,
                                                TFetchedType aFetchedContentType );
    void UpdateMessagePtrL( TFSMailMsgId aNewMailboxId,
                                                            TFSMailMsgId aNewFolderId,
                                                            TFSMailMsgId aNewMessageId );
    void CancelFetchings();
    void StartFetchingMessageStructureL( CFSMailMessage& aMsg );
    TBool MessageStructureKnown( CFSMailMessage& aMsg ) const;
    void StartWaitedFetchingL( TFetchedType aFetchedContentType );
    // Message stack handling. Ownership of message is transferred when succesful.
    void PushMessageL( CFSMailMessage* aMessage, TBool aIsEmbedded );
    CFSMailMessage* PopMessage();
    void EraseMessageStack();
    TBool OpenFolderListForMessageMovingL();
    void ShowPreviousMessageL();
    TBool ShowPreviousMessageMenuInOptions() const;
    void ShowNextMessageL();
    TBool ShowNextMessageMenuInOptions() const;
    void ChangeMsgReadStatusL(TBool aRead, TBool aCmdFromMrui );
    void SetMessageFollowupFlagL();
    void SendEventToAppUiL( TFSMailEvent aEventType );
    // Message is moved and viewer is closed asynchronously after selecting a folder
      // from the move to folder dialog. This is necessary because view swithching is asynchronous.
    static TInt MoveToFolderAndExitL( TAny* aMailViewerVisualiser );
    void FolderSelectedL( TFSMailMsgId aSelectedFolderId,
                          TFSEmailUiCtrlBarResponse aResponse ); 
    
    void UpdateEmailHeaderIndicators();
    TBool IsOpenedInMRViewerL();
    void CopyCurrentToClipBoardL( const TDesC& aArgument) const;
    void OpenLinkInBrowserL( const TDesC& aUrl) const;
    void SaveWebAddressToFavouritesL( const TDesC& aUrl ) const;
    void SetScrollPosition(TInt aPosition);
    
    void CheckMessageBodyL( CFSMailMessage& aMessage, TBool& aMessageBodyStructurePresent, TBool& aMessageBodyContentPresent);
    
private: // Constructors
    
    void ConstructL();
    CFsEmailUiHtmlViewerView( CAlfEnv& aEnv, 
                              CFreestyleEmailUiAppUi& aAppUi, 
                              CAlfControlGroup& aControlGroup );

private:
    CAlfEnv& iEnv;
    CFsEmailUiHtmlViewerContainer* iContainer;
    CFSMailMessage* iMessage;
    THtmlViewerActivationData iActivationData;
    CFSEmailUiAttachmentsListModel* iAttachmentsListModel;

    // Stack of open messages. Messages are owned by this stack. 
    CStack<CFSMailMessage, ETrue>* iOpenMessages;
    // Stack of embedded messages. Message are NOT owned by this stack. Similar to iOpenMessages
    // but has NULL pointer in slots which correspond non-embedded messages.
    CStack<CFSMailMessage, EFalse>* iEmbeddedMessages;
    TBool iCreateNewMsgFromEmbeddedMsg;
        
    // <cmail>
    // Temporary mail address for opening editor
    CFSMailAddress* iNewMailTempAddress;
    // </cmail>
    CFSMailBox* iMailBox;
private: // data for moving message to different folder action
    TBool iFetchingMessageStructure;
    TInt iCurrentStructureFetchRequestId;

    TBool iFetchingPlainTextMessageBody;
    TInt iCurrentPlainTextBodyFetchRequestId;

    TBool iFetchingHtmlMessageBody;
    TInt iCurrentHtmlBodyFetchRequestId;

    TFetchedType iStartAsyncFetchType;

    // <cmail>
    // Email forwarding initiated (so don't cancel fetch of message parts
    // during deactivation).
    TBool iForwardingMessage;
    // </cmail>

    // Wait note stuff
    //<cmail>
    //CAknWaitNoteWrapper* iAsyncWaitNote;
    CAknWaitDialog* iWaitDialog;
    CAknWaitDialog* iWaitDialogOpening;
    TBool iDialogNotDismissed;
    //</cmail>
    TBool iFetchingAlready;
    TBool iAsyncProcessComplete;
    CActiveSchedulerWait iWait;
	TBool iMoveToFolderOngoing;
    TBool iMovingMeetingRequest;
    CAsyncCallBack* iAsyncCallback;
    
    TFSMailMsgId iMoveDestinationFolder;  
    	// Meeting request observer stuff.
		// Pointer to observer to inform, not owned.
		MESMRIcalViewerObserver* iMrObserverToInform;
		MESMRIcalViewerObserver::TIcalViewerOperationResult iOpResult;
		TBool iMrUiActive;
	  //<cmail>
    // Message pointer to a deleted mail from Mrui
    TFSMailMsgId iDeletedMessageFromMrui;
    // Flag selection handler active object for global note
    CFlagSelectionNoteHandler* iFlagSelectionHandler;  
    TBool iNextOrPrevMessageSelected;
    };

////////////////////////////////////////
// FLAG selection dialog global note handler
/////////////////////////////////////////
class CFlagSelectionNoteHandler : public CActive
    {
    public:  // Constructors and destructor
        static CFlagSelectionNoteHandler* NewL( CFsEmailUiHtmlViewerView& aViewerVisualiser );
        virtual ~CFlagSelectionNoteHandler();
        void Cancel();       
        void LaunchFlagListQueryDialogL();       
            
    private: 
    
        void RunL();       
        void DoCancel();       
        TInt RunError( TInt aError );
        
    private:
        CFlagSelectionNoteHandler(  CFsEmailUiHtmlViewerView& aVoIPDialogObserver );

        
    private:      
        CFsEmailUiHtmlViewerView& iViewerView;
        CAknGlobalListQuery* iGlobalFlagQuery;
        HBufC* iPrompt;
        TInt iSelection;
    };


#endif // __FREESTYLEEMAILUI_HTML_VIEW_H__
