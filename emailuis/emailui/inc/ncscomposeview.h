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
* Description: declares compose view
*
*/



#ifndef __CNCSCOMPOSEVIEW_H__
#define __CNCSCOMPOSEVIEW_H__

#include "cfsmailcommon.h"
#include "mfsmailrequestobserver.h"
#include <alf/alfenv.h>
#include "AknServerApp.h"
#include <AknProgressDialog.h>
#include <MsgAttachmentUtils.h>

#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiConstants.h"
#include "FSComposerFetchLogic.h"
#include "FreestyleEmailUi.hrh"
#include "cmailcustomstatuspaneindicators.h"

class CNcsComposeViewContainer;
class CFSMailClient;
class CAlfControl;
class CAlfAnchorLayout;
class CFSMailBox;
class CFSMailMessage;
class CFSMailMessagePart;
class CNcsEmailAddressObject;
class CFSMailAddress;
class CFSEmailCRHandler;
class CFreestyleEmailUiAppUi;
class CAlfEnv;
class CFreestyleEmailUiSendAttachmentsListControl;
class CMsvSession;
class CAknsLayeredBackgroundControlContext;
class CFsAutoSaver;
class CAknWaitDialog;
class CActiveHelper;
class CAknStylusPopUpMenu;
class CAknToolbarExtension;


const TInt KErrNcsComposeViewNotReady = KErrNotReady - 1000;

/**
* CNcsComposeView view class.
*/
class CNcsComposeView: 
    public CFsEmailUiViewBase,
	public MAlfActionObserver,
    public MComposerFetchLogicCallback,
    public MProgressDialogCallback,
    public MAknServerAppExitObserver
    {
    
private:

    /**
     * Type of commit
     */
    enum TCommitType
        {
        // This is transient commit, message may get further modifications
        ETransient,
        // This is final commit, no more incoming changes
        EFinal
        };

     // can't close view if ChildDoActivateL()
     enum TChildActivationState 
         {
         EChildActivationDeactivated,
         EChildActivationStarted,
         EChildActivationError,  // error during activation
         EChildActivationExitRequired, // activation ended, exit the view
         EChildActivationProcessed
         };
        
public:   // constructors and destructor

    /**
    * NewL.
    * Two-phased constructor.
    * Construct a CNcsComposeView for the AVKON application aApp.
    * Using two phase construction,and return a pointer to the created object
    * @return a pointer to the created instance of CNcsComposeView
    */
    static CNcsComposeView* NewL( 
        CFSMailClient& aMailClient,
        CAlfEnv& aEnv, 
        CFreestyleEmailUiAppUi* aAppUi,
        CAlfControlGroup& aControlGroup,
        CMsvSession& aMsvSession );

    /**
    * NewLC.
    * Two-phased constructor.
    * Construct a CNcsComposeView for the AVKON application aApp.
    * Using two phase construction,and return a pointer to the created object
    * @return a pointer to the created instance of CNcsComposeView
    */
    static CNcsComposeView* NewLC( 
        CFSMailClient& aMailClient,
        CAlfEnv& aEnv, 
        CFreestyleEmailUiAppUi* aAppUi,
        CAlfControlGroup& aControlGroup,
        CMsvSession& aMsvSession );

    /**
    * ~CNcsComposeView.
    * Virtual Destructor.
    */
    virtual ~CNcsComposeView();
        
    // Exit preparation function.
    void PrepareForExit();
        
public: // from CFsEmailUiViewBase
    
    /**
    * Id
    * @return Id Uid value
    */
    TUid Id() const;

    /**
    * HandleCommandL
    * From CAknView, takes care of command handling.
    * @param aCommand Command to be handled
    */
    void HandleCommandL( TInt aCommand );

    /**
    * ChildDoDeactivate
    * Deactivate an AknView
    * Remove the container class instance from the App UI's stack and
    * deletes the instance
    */
    void ChildDoDeactivate();
        
    /**
    * DynInitMenuPaneL
    * Called by the framework before menu is shown to
    * show/hide items dynamically.
    * @param aResourceId The Id of the menu pane.
    * @param aMenuPane The menu pane.
    */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
    * HandleDynamicVariantSwitchL
    * Recalculates component layout 
    */
    void HandleDynamicVariantSwitchL( 
            CFsEmailUiViewBase::TDynamicSwitchType aType );
    /**  
    * to hide or show CAlfVisuals ( used for activation or deactivation )
    */             
    virtual void FadeOut( TBool aDirectionOut );
        
public: // from MAlfActionObserver
    	
    void HandleActionL( const TAlfActionCommand& aActionCommand );

public: // from MComposerFetchLogicCallback

    TBool FetchLogicComplete( TComposerFetchState aState, TInt aError );

public: // new functions

    /**
    * LaunchStylusPopupMenu
    * Show stylus popup menu for the attachments line
    */
    void LaunchStylusPopupMenu( const TPoint& aPenEventScreenLocation );
    
    /**
    * AttachmentsListControl
    * Returns pointer to attachment list control
    */       
    CFreestyleEmailUiSendAttachmentsListControl* AttachmentsListControl();

    /**
    * NewMessage
    * Returns pointer to current composer message
    */
    CFSMailMessage* NewMessage();

    /**
    * Commits changes to message. This method sets the field contents from
    * the UI to the message object, but does not save the message unless
    * explicitely requested.
    */
    void CommitL( TBool aParseAddresses = ETrue,
        TFieldToCommit aFieldToCommit = EAllFields, 
        TBool aSaveNow = EFalse,
        TCommitType aType = ETransient );

    /**
     * Saves the message content, if it has been changed since the last save.
     */
    TInt SaveMessage();
    void SaveMessageL();

    void DialogDismissedL( TInt aButtonId );
    
    void ShowFetchingWaitNoteL();
    
    TBool IsPreparedForExit();
    
    void AsyncExitL();
    
    void HandleContainerChangeRequiringToolbarRefresh();

    /**
     * Callback method for setting body content in asynchronous way.  
     */
    void SetBodyContentComplete();
    
    /**
     * Checks if "Opening" wait note is visible.
     */
    TBool IsOpeningWaitNoteVisible();
    
protected:

    void ProcessCommandL( TInt aCommand );

private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );  

    /**
     * From MAknToolbarObserver.
     */
    void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );

    /**
     *  @see CFsEmailUiViewBase::OfferToolbarEventL
     */
    void OfferToolbarEventL( TInt aCommand );

    /**
     * @see CFsEmailUiViewBase::ToolbarResourceId
     */
    TInt ToolbarResourceId() const;
    
    /**
     * @see CFsEmailUiViewBase::GetInitiallyDimmedItemsL
     */   
    void GetInitiallyDimmedItemsL( const TInt aResourceId, 
            RArray<TInt>& aDimmedItems ) const;
    
    /**
     * Update toolbar items.
     */
    void RefreshToolbar();    
    
private: // new functions
    
    /**
    * HandleActivationCommandL
    * Called from DoActivateL() to do the part of view activation which depends on the
    * activation command given in TUid argument.
    */
    void HandleActivationCommandL( TUid aCustomMessageId );

    /**
    * DoSaveDraftL
    * Saves message to drafts
    * @param aAskUser If ETrue asks user whether to save.
    * @return ETrue if option was select and 
    *         EFalse if Cancel was pressed, or if saving failed.
    */
    TBool DoSaveDraftL( TBool aAskUser );
        
    /**
    * DoSendL
    * Sends email message.
    */
    void DoSendL();
        
    enum TExitMode 
        {
        ESaveDraftQuery,
        ESaveDraft,
        ENoSave
        };
        
    /**
    * DoSafeExit
    */
    void DoSafeExit( TExitMode aMode=ESaveDraftQuery );

    /**
    * DoExitL
    * Handler called when exiting the view.
    */
    void DoExitL( TExitMode aMode=ESaveDraftQuery );
        
    /**
    * DoOpenAttachmentListL
    * Saves current editor message to drafts and opens
    * attachment list
    */
    void DoOpenAttachmentListL();
        
    /**
    * SetAttachmentLabelContent
    * Set files from model to view label
    */       
    void SetAttachmentLabelContentL();
        
    /**
    * GetAttachmentsFromMail
    * get list of attachments from mail
    */       
    void GetAttachmentsFromMailL();
        
    /**
    * FileExistsInModel
    * Check for duplicates in model when reading attachments from mail
    */       
    TBool FileExistsInModel( TFSMailMsgId aAttachmentId );

    /**
    * DoQuickTextL
    * Display the Quick Text dialog
    */
    void DoQuickTextL();
        
    /**
    * AsyncAddAttachment
    * Add attachment file to message
    */
    static TInt AsyncAddAttachment( TAny* aSelfPtr );
    
    /**
    * RunFollowUpFlagDlgL()
    * Query user for followup flag
    */
    void RunFollowUpFlagDlgL();
    
    /**
    * InitFieldsL
    * Inits fields with message data.
    */       
    void InitFieldsL();
 
    /**
    * InitReplyFieldsL
    * Inits fields when composing reply message.
    * @param aReplyAll ETrue if this is replyall message.
    */       
    void InitReplyFieldsL( TBool aReplyAll );

    /**
    * RemoveOwnAddress
    * Removes own address while doing reply all.
    * @param aAddressList The address list.
    */       
    void RemoveOwnAddress( 
            RPointerArray<CNcsEmailAddressObject>& aAddressList );
         
    /**
    * InitForwardFieldsL
    * Inits fields when composing forward message.
    */       
    void InitForwardFieldsL();
        
    /**
    * IncludeMessageTextL
    * Inits body text when forwarding, replying, or opening draft message.
    * @param aEnsureEmptyLineInBegin When true, the function ensures there's
    *                                empty lines in the beginning of the 
    *                                message. I.e. line break(s) are inserted
    *                                in front of any quote or signature
    */        
    void IncludeMessageTextL( TBool aEnsureSpaceInBegin = EFalse );
    
    /**
     * Async version of IncludeMessageTextL.
     * Inits body text async when forwarding, replying message.
     * @param aEnsureEmptyLineInBegin When true, the function ensures there's empty lines
     *                                in the beginning of the message. I.e. line break(s)
     *                                are inserted in front of any quote or signature
     */
    void IncludeMessageTextAsyncL( TBool aEnsureSpaceInBegin = EFalse );
        
    /**
    * SetPriority
    * Sets priority when using saved draft.
    */
    void SetPriority();

    /**
     * Sets follow-up flag to status pane indicator from draft message.
     */
    void SetFollowUp();

    /**
    * GetMessageBodyL
    * Gets message body in a buffer.
    * @return Message body
    */
    HBufC* GetMessageBodyL();
        
    /**
    * ReturnToPreviousViewL
    * Exit editor and return to previous view
    */
    void ReturnToPreviousViewL();
        
    /**
    * FetchTextPartL
    * Fetch text part
    * @param aMsg The message whiches text part is to be fetched
    */
    void FetchTextPartL( CFSMailMessage& aMsg );
	
    /**
     * Do the needed initializations for reply/reply all/forward commands
     * depending on the iCustomMessageId.
     */
    void InitReplyOrForwardUiL();
    
    /**
    * GenerateReplyMessageL
    * Creates reply message
    * @param aReplyAll ETrue for reply all and EFalse for reply
    */
    void GenerateReplyMessageL( TBool aReplyAll );
		
    /**
    * InitReplyUiL
    * Inits UI in reply case
    * @param aReplyAll ETrue for reply all and EFalse for reply
    */
    void InitReplyUiL( TBool aReplyAll );
		
    /**
    * GenerateForwardMessageL
    * Creates forward message
    */
    void GenerateForwardMessageL();
		
    /**
    * InitForwardUiL
    * Inits UI in forward case
    */
    void InitForwardUiL();
		
    /**
    * InitUiGeneralL
    * Inits UI in general case
    */
    void InitUiGeneralL();
    /**
    * GenerateReplyHeaderLC
    * Creates header lines for reply/forward message
    * @return Reply header
    */
    HBufC* GenerateReplyHeaderLC();

    /**
    * GenerateSmartTagLC
    * Creates smart reply/forward tag
    * @return Smart tag
    */
    HBufC* GenerateSmartTagLC();
        
    /**
    * SetReplyForwardFlagL
    * Set reply/forward flag into original message if needed
    */
    void SetReplyForwardFlagL();
         
    /**
    * MailBoxSupportsSmartReply
    * @return ETrue if mailbox supports smart reply
    */
    TBool MailBoxSupportsSmartReply();

    /**
    * MailBoxSupportsSmartForward
    * @return ETrue if the mailbox supports smart forward
    */
    TBool MailBoxSupportsSmartForward();
        
    /**
    * HasUnfetchedAttachmentsL
    * @param aMsg Message to be tested
    * @return ETrue if the message has unfetched attachments
    */
    TBool HasUnfetchedAttachmentsL( CFSMailMessage& aMsg );
        
    /**
    * SaveToDraftsL
    * @param aParseAddresses ETrue to parse text in address fields now
    */
    void SaveToDraftsL( TBool aParseAddresses );
        
    void ExitComposer();
        
    void CreateContainerL();

    void ResetComposer();
    
    void SaveAndCleanPreviousMessage();
    
    // First start function to improve app loading perf.
    void DoFirstStartL();
    
    static TInt AsyncExit( TAny* aSelfPtr );

    // Gets button from toolbar extension.
    CAknButton* Button( TInt aCmdId, 
        CAknToolbarExtension* aExtension = NULL );
    TBool WLANConnectionActive();

private:   // Constructors and destructor

    /**
    * CNcsComposeView
    * C++ default constructor.
    * @param aAppUi App UI
    * @param aEnv Alfred environment
    * @param aSendAttachmentControlGroup Send attachment control group
    * @param aMailClient Email framework client object.
    * @param aBgControl Background control.
    */
    CNcsComposeView( CFreestyleEmailUiAppUi& aAppUi, 
                     CAlfEnv& aEnv, 
                     CAlfControlGroup& aControlGroup, 
                     CFSMailClient& aMailClient, 
                     CMsvSession& aMsvSession );

    /**
    * ConstructL
    * 2nd phase constructor.
    */
    void ConstructL();
        
private: // data
    
    /**
    * control container
    */
    CNcsComposeViewContainer* iContainer;

    /**
    * view identifier
    */
    TUid iIdentifier;
        
    /**
    * email framework client
    */
    CFSMailClient& iMailClient;

    /**
    * email framework mailbox
    */		
    CFSMailBox* iMailBox;
    	
    /**
    * original message
    */
    CFSMailMessage* iOrigMessage;
    	
    /**
    * new message
    */
    CFSMailMessage* iNewMessage;
    	
    /**
    * new message text part
    */
    CFSMailMessagePart* iNewMessageTextPart;

    /**
    * central repository handler
    */        
    CFSEmailCRHandler* iCrHandler;
        
    /**
    * launch parameters
    */
    TEditorLaunchParams iLaunchParams;
        
    /**
    * flag to indicate on exit whether mail was sent
    */
    TBool iMailSent;

    /**
    * flag to indicate on exit whether mail sending failed
    */
    TBool iMailSendFailed;

    /**
    * error code for fetching message, KErrNone if fetched succesfully
    */
    TInt iMailFetchingErrCode;
    
    /**
    * flag to indicate whether view is ready to handle user commands
    */ 
    TBool iViewReady;
    
    /**
    * flag to indicate is save to draft called by attachment list
    */
    TBool iAttachmentListSaveDraft;
        
    /**
    * session to messaging server
    */
    CMsvSession& iMsvSession;
        
    /**
    * stored TUid passes to DoActivateL
    */
    TUid iCustomMessageId;

    /**
    * background size
    */
    TSize iBackgroundImageSize;

    /**
    * alfred environment
    */
    CAlfEnv& iEnv;
        
    /**
    * autosave
    */
    CFsAutoSaver* iAutoSaver;
        
    /**
    * fetch logic
    */
    CFsComposerFetchLogic* iFetchLogic;

    TBool iFirstStartCompleted;

    /**
     * dialog query (send or delete)
     */
    CAknListQueryDialog* iDlg;

    /**
     * checkquery (send or delete)
     */
    TBool iCheckQuery;
    
    CAsyncCallBack* iAsyncCallback;
    
    CAknWaitDialog* iFetchWaitDialog; //<cmail>
    
    //when some plugin makes fake sync, we need these
    TBool iFakeSyncGoingOn;
    TBool iFetchDialogCancelled;
    CActiveHelper* iActiveHelper;
 
    // For the fake synchronous operations of the mail plugin run in DoExitL
    // we need to knwo when the method is running and block all the commands
    // during that time.
    TBool iExecutingDoExitL;
    
    CAsyncCallBack* iAsyncAttachmentAdd;
    MsgAttachmentUtils::TMsgAttachmentFetchType iAttachmentAddType;
    
    // Status pane indicators (priority and followup)
    CCustomStatuspaneIndicators* iStatusPaneIndicators;
    
    // Popup menu for attachment list.
    CAknStylusPopUpMenu* iStylusPopUpMenu;

    // Flag to indicate whether message text part has been modified.
    TBool iMessageTextPartModified;

    // Flag to indicate whether message has been modified.
    TBool iMessageModified;
    
    /**
     * Flag indicating async way of inluding message body text.
     */
    TBool iIncludeMessageTextAsync;
    
    /**
     * Buffer for text of message's body.
     * Own.
     */
    HBufC* iBody;
    
    /**
     * Flag indicating that view was fully activated.
     */
    TBool iViewFullyActivated;
    
    /**
     * Flag indicating that "Opening" wait note is visible.
     */
    TBool iOpeningWaitNoteVisible;
    
    /**
     * "Opening" wait dialog.
     */
    CAknWaitDialog* iOpeningWaitDialog;
    
    /**
     * Flag indicating that adding attachment dialog is opened.
     */
    TBool iAddingAttachmentDialogOpened;
    
    TChildActivationState iChildActivationState;
    };

/**
* This class is used by CNcsComposeView for waiting its own async tasks.
*  
* @since S60 v3.2
*/    
class CActiveHelper : public CActive
    {
    public: // public functions
    
        /**
        * Default 1st phase factory method.
        * Creates an instance of CCacheSessionActiveHelper
        *
        * @return created instance of the CCacheSessionActiveHelper
        */
        static CActiveHelper* NewL( CNcsComposeView* aComposeView );
        
        /**
        * Destructor
        */
        ~CActiveHelper( );
        
        /**
        * Active object start
        */
        void Start();
    
    private: // private functions
        
        /**
        * Constructor
        */
        CActiveHelper( CNcsComposeView* aSession );

        /**
        * Active object RunL
        */
        void RunL();
        
        /**
        * Active object DoCancel
        */
        void DoCancel();
        
        /**
        * ConstructL
        */
        void ConstructL();
        
        /**
        * pointer to CNcsComposeView doesnt own
        */
        CNcsComposeView* iComposeView;
            
    };

#endif // CNCSCOMPOSEVIEW_H


// End of File
