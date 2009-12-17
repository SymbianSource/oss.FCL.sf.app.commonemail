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
* Description:  Folder list array.
*
*/


#ifndef IPSSETUIFOLDERSUBSCRIPTION_H
#define IPSSETUIFOLDERSUBSCRIPTION_H

#include <msvstd.h> // TMsvId
#include <AknForm.h> // CAknDialog
#include <eiklbo.h> // MEikListBoxObserver
#include <AknWaitNoteWrapper.h>

/**
 * Enumeration for folder icon indexes.
 */
enum TIpsSetUiFolderIconIndexes
    {
    EIpsSetUiFolderSingleUnsubbed = 0,
    EIpsSetUiFolderSingleSubbed,
    EIpsSetUiFolderSingleSubbedNew,
    EIpsSetUiFolderSubfoldersUnsubbed,
    EIpsSetUiFolderSubfoldersSubbed,
    EIpsSetUiFolderSubfoldersSubbedNew,
    EIpsSetUiFolderUnsubbedNew,
    EIpsSetUiFolderSubfoldersUnsubbedNew,
    };

class CBaseMtmUiData;
class CBaseMtmUi;
class CIpsSetUiFolderListArray;
class CIpsSetUiCtrlSoftkey;
class CClientMtmRegistry;
class CGulIcon;
class CAknTitlePane;
//<cmail>
class CAknWaitDialog;
class CSubscriptionDialogActiveHelper;
//</cmail>


/**
 * Folder subscription dialog class
 *
 * @lib IpsSosSettings.lib
 * @since FS v1.0
 */
class CIpsSetUiSubscriptionDialog :
    public CAknDialog,
    public MEikListBoxObserver,
    public MProgressDialogCallback, //<cmail>
    public MMsvSessionObserver
    {
public:

    //<cmail>
    /**
     * CIpsSetUiSubscriptionDialog keeps it's state using these enum values
     */
    enum TSubsDialogState { EIdle, ESingleOperation, EMultipleStart, 
                                    EMultipleSubscribe, EMultipleUnsubscribe };
    //</cmail>

    /**
     * NewL()
     * @param TMsvId
     * @param CMsvSession&
     * @return CIpsSetUiSubscriptionDialog*
     */
    static CIpsSetUiSubscriptionDialog* NewL(
        TUint64& aFlags,
        TMsvId aServiceId,
        CMsvSession& aSession,
        CAknTitlePane& aTitlePane,
        const TDesC& aTitlePaneText );

        ~CIpsSetUiSubscriptionDialog();


    /**
     * From MMsvSessionObserver
     * HandleSessionEventL
     * @param TMsvSessionEvent, event id
     * @param TAny*
     * @param TAny*
     * @param TAny*
     */
    virtual void HandleSessionEventL(
        TMsvSessionEvent aEvent,
        TAny* aArg1,
        TAny* aArg2,
        TAny* aArg3 );

    /**
     * SynchronisEIpsSetUiFoldersL()
     */
    void SynchronisEIpsSetUiFoldersL();

    /**
     * DisplayFinalProgressDialog()
     * To deal with final progress from suboperations
     * @param CMsvOperation&
     */
    void DisplayFinalProgressDialog(
        CMsvOperation& aCompletedOperation) const;

    /**
     * from CCoeControl
     * OfferKeyEventL()
     * @param TKeyEvent&, key event
     * @param TEventCode
     * @return TKeyResponse
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);


    /**
     * From CAknDialog
     * DynInitMenuPaneL()
     * @param TInt, resource id
     * @param CEikMenuPane*
     */
    virtual void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ) ;

    /**
     * From CAknDialog
     * ProcessCommandL()
     * @param TInt, command id
     */
    virtual void ProcessCommandL( TInt aCommandId );

    //<cmail>
    /**
     * This method is called by CSubscriptionDialogActiveHelper, when async
     * operation is completed
     * @param TInt, error id
     */
    void HandleAsyncRequestCompletedL( TInt aErrorId );
    
    /**
     * from MProgressDialogCallback
     */
    void DialogDismissedL( TInt aButtonId );
    //</cmail>
    

protected:

    /**
     * From MEikListBoxObserver
     * @param CEikListBox*
     * @param TListBoxEvent
     */
    virtual void HandleListBoxEventL(
        CEikListBox* aListBox,
        TListBoxEvent aEventType );

    /**
     * HandleSubscriptionL()
     * @param TInt type of subscription, either subscribed or unsubscribed
     * @param CMsvEntrySelection&, selection to subscribe
     */
    void HandleSubscriptionL( TInt aSubType, CMsvEntrySelection& aSelection );

    /**
     * CheckAreAllFoldersIncludedL()
     * Checks if all subfolders are either subscribed of unsubscribed
     * @param TInt, type of subscription, either subscribed or unsubscribed
     * @param CMsvEntrySelection*, selection to check
     * @return TBool, ETrue if all subfolders have same subscription state
     */
    TBool CheckAreAllFoldersIncludedL( TInt aSubType, CMsvEntrySelection* sel );

private:

    /**
     * PreLayoutDynInitL()
     */
    virtual void PreLayoutDynInitL();

    /**
     * PostLayoutDynInitL()
     */
    virtual void PostLayoutDynInitL();

    /**
     * OkToExitL()
     * @param TInt
     * @return TBool
     */
    virtual TBool OkToExitL(TInt aButtonId);

    /**
     * C++ constructor
     * @param TMsvId, mailbox id
     * @param CMsvSession& session reference
     */
    CIpsSetUiSubscriptionDialog(
        TUint64& aFlags,
        TMsvId aServiceId,
        CMsvSession& aSession,
        CAknTitlePane& aTitlePane );

    /**
     * ConstructL()
     */
    void ConstructL( const TDesC& aTitlePaneText );

    /**
     * CreateOperationL()
     * @param TInt, button id
     */
    void CreateOperationL(TInt aButtonId);

    /**
     * ContextHasChildFolders()
     * @return TBool, ETrue if has children
     */
    TBool ContextHasChildFolders() const;

    /**
     * GetMtmUiL()
     */
    void GetMtmUiL();


    /**
     * AppendIconsL()
     * Appends icons to icon array
     * @param TFileName&: Bitmap filename with path
     * @param CArrayPtrFlat<CGulIcon>*: icon array where bitmap is append
     */
    void AppendIconsL(
        TFileName& aFileName,
        CArrayPtrFlat<CGulIcon>* aIcons);

    /**
     * CurrentFolder()
     * @return TMsvId, id of current folder
     */
    TMsvId CurrentFolder();

    /**
     * HandleRefreshL
     */
    void HandleRefreshL();

    /**
     * LaunchHelpL()
     * Launches help
     */
    void LaunchHelpL( const TDesC& aContext );

    /**
     * From CCoeControl, returns help context
     * @param TCoeHelpContext&, help context
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

    /**
     * returns correct resource string for MSK
     * @return MSK label
     */
    TInt GetResourceForMiddlekey();
    
    /**
     * Solves which label should be shown in MSK
     * @return state of MSK (Open/Subscribe/Unsubscribe/empty)
     */
    TInt CheckMSKState();
    
    //<cmail>
    /**
     * cancels all async requests
     */
    void CancelAllAsyncRequests();

    /**
     * Shows a waitnote
     */
    void ShowWaitNoteL();
    //</cmail>
    
private:

    /**
     * Flags to control dialog. Mainly for exiting the dialog.
     */
    TUint64&                        iFlags;

    /**
     * MTM registry object.
     * Owned.
     */
    CClientMtmRegistry*             iMtmRegistry;

    /**
     * Current service Id.
     */
    TMsvId                          iServiceId;

    /**
     * Message server session.
     */
    CMsvSession&                    iSession;

    /**
     * Current folder entry.
     * Owned.
     */
    CMsvEntry*                      iContextEntry;

    /**
     * Base MTM object.
     * Owned.
     */
    CBaseMtm*                       iBaseMtm;

    /**
     * Container for folders.
     * Owned.
     */
    CIpsSetUiFolderListArray*       iFolderListArray;

    /**
     * Softkey control.
     * Pwned.
     */
    CIpsSetUiCtrlSoftkey*           iSoftkeys;

    /**
     * Commonly used ordering.
     */
    TMsvSelectionOrdering           iOrdering;

    /**
     * Currently open folder.
     */
    TMsvId                          iOpenedFolderId;

    /**
     * Previous folder subscription.
     */
    TInt                            iPreviousSubscriptionRID;

    /**
     * Previously opened folder.
     */
    TInt                            iPreviousFolderOpenRID;
    
    
    /**
     * Previous view title pane text
     */
    HBufC*                          iOldTitlePaneText;
    
    /**
     * title pane control
     */
    CAknTitlePane&                  iTitlePane;
    
    /**
     * wait note for folder list refresh
     */
    //<cmail>
    CAknWaitDialog*            iAsyncWaitNote;
    
    /**
     * Owned. Only one operation can be active at a time
     */
    CSubscriptionDialogActiveHelper* iAsyncHelper;
    
    /**
     * Current state of this object (which request is going on, if any)
     */
    CIpsSetUiSubscriptionDialog::TSubsDialogState iState;
    
    /**
     * True if fetch dialog is already dismissed, no need to call ProcessFinishedL
     */
    TBool iFetchDialogDismissed;
    
    /**
     * List of selected folder entries for subcribe/unsubscribe
     */
    CMsvEntrySelection* iEntrySelection;
    
    /**
     * Multiple commands to iBaseMtm are called during this active cycle 
     * with KIMAP4MTMStartBatch
     */
    TBool iMultipleStarted;
    
    /**
     * Original count in iEntrySelection when batch subscribe/unsubscribe command
     * was started
     */
    TInt iCount;
    //</cmail>
    
    };

//<cmail>
/**
* This class is used by CIpsSetUiSubscriptionDialog for waiting its async tasks.
* CIpsSetUiSubscriptionDialog can't be made active because of multiple times CBase 
* inheritance problems
*/    
class CSubscriptionDialogActiveHelper : public CActive
    {
public: // public functions
    
    /**
    * Constructor
    */
    CSubscriptionDialogActiveHelper(CIpsSetUiSubscriptionDialog* aSubscriptionDialog);

    /**
    * Destructor
    */
    ~CSubscriptionDialogActiveHelper( );
    
    /**
    * Active object start
    */
    void Start(CMsvOperation* aMsvOperation);

private: // private functions
    
    /**
    * Active object RunL
    */
    void RunL();
    
    /**
    * Active object DoCancel
    */
    void DoCancel();
    
    /**
    * pointer to CIpsSetUiSubscriptionDialog doesnt own
    */
    CIpsSetUiSubscriptionDialog* iSubscriptionDialog;
    
    /**
     * pointer to monitored operation, Owned
     */
    CMsvOperation* iMsvOperation;
            
    };
//</cmail>

#endif // IPSSETUIFOLDERSUBSCRIPTION_H
