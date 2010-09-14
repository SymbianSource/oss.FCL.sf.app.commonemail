/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file implements classes CFSEmailUiMailListVisualiser, CMailListUpdater, CMsgMovedNoteTimer, CDateChangeTimer.
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <freestyleemailui.mbg>
#include <AknUtils.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>
#include <featmgr.h>
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cfsmailfolder.h"
#include "fscontrolbar.h"
#include "fscontrolbuttoninterface.h"
#include "fstreelist.h"
#include "fstreevisualizerbase.h"
#include "fstreeplainonelinenodedata.h"
#include "fstreeplainonelinenodevisualizer.h"
#include "fstreeplaintwolineitemdata.h"
#include "fstreeplaintwolineitemvisualizer.h"
#include "cfsmailcommon.h"
#include "ceuiemaillisttouchmanager.h"
#include "FSEmailBuildFlags.h"
#include "cfsccontactactionmenu.h"
#include "mfsccontactactionmenumodel.h"

#include <hlplch.h>
#include <AknIconArray.h>
#include <alf/alfdecklayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfframebrush.h>
#include <alf/alfevent.h>
#include <alf/alfstatic.h>

#include <aknnotewrappers.h>
#include <msvapi.h>
#include <akntitle.h>
#include "esmailsettingspluginuids.hrh"
#include "esmailsettingsplugin.h"
#include "mfsmailbrandmanager.h"
#include <AknWaitDialog.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <csxhelp/cmail.hlp.hrh>
// Meeting request
#include <MeetingRequestUids.hrh>
#include "cesmricalviewer.h"
#include <aknstyluspopupmenu.h>
#include <akntoolbar.h>

// INTERNAL INCLUDES
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiMailListModel.h"
#include "FreestyleEmailUiMailListVisualiser.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiMailListControl.h"
#include "FreestyleEmailUiStatusIndicator.h"
#include "freestyleemailcenrephandler.h"
#include "FreestyleEmailUiFolderListVisualiser.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiMsgDetailsVisualiser.h"
#include "FreestyleEmailDownloadInformationMediator.h"
#include "FreestyleEmailUiContactHandler.h"
#include "FreestyleEmailUiLauncherGridVisualiser.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FSDelayedLoader.h"
#include "FSEmail.pan"

#include "ipsplgcommon.h"
#include "cemailsettingsextension.h"

// CONST VALUES
const TInt KControlBarTransitionTime = 250;
const TInt KMaxPreviewPaneLength = 60;
const TInt KMsgUpdaterTimerDelay = 2500000; // Time to update list, 2,5sec
const TInt KNewMailTimerDelay = 20; // sleeping timer to start processing new messages
const TInt KSortTimerDelay = 40;    // sleeping timer to start sorting ( delay should be longer than abowe )
const TInt KSortCountdown = 5;      // number how many times will be tried do start sorting when it's forbidden
const TInt KNewMailMaxBatch = 7;    // number of new mails inserted into list at once
static const TInt KMsgDeletionWaitNoteAmount = 5;
_LIT( KMissingPreviewDataMarker, "..." );

static const TInt KMaxItemsFethed = 1000;
static const TInt KCMsgBlock = 15;
static const TInt KCMsgBlockSort = 50;

// ---------------------------------------------------------------------------
// Generic method for deleting a pointer and setting it NULL.
// ---------------------------------------------------------------------------
//
template <class T> void SafeDelete(T*& ptr)
    {
    delete ptr;
    ptr = NULL;
    }

// TDeleteTask
// Task class for deleting mail
// ---------------------------------------------------------------------------
// Class definition
// ---------------------------------------------------------------------------
template <class T> struct TDeleteTask
    {
    
public:
    
    typedef void ( T::*TDeleteMethod )( const RFsTreeItemIdList& aEntries );
    
public:    
    
    TDeleteTask(T& aObject, TDeleteMethod aMethod);    
    ~TDeleteTask();
    void ExecuteL();
    RFsTreeItemIdList& Entries();
    void Reset();
    
private:

    T& iObject;
    TDeleteMethod iMethod;    
    RFsTreeItemIdList iEntries;
    
    };

// ---------------------------------------------------------------------------
// TDeleteTask<T>::TDeleteTask
// ---------------------------------------------------------------------------
//
template <class T> TDeleteTask<T>::TDeleteTask( T& aObject, TDeleteMethod aMethod )
    : iObject(aObject), iMethod(aMethod)
    {    
    }

// ---------------------------------------------------------------------------
// TDeleteTask<T>::~TDeleteTask
// ---------------------------------------------------------------------------
//
template <class T> TDeleteTask<T>::~TDeleteTask()
    {
    iEntries.Close();
    }

// ---------------------------------------------------------------------------
// TDeleteTask<T>::ExecuteL
// ---------------------------------------------------------------------------
//
template <class T> void TDeleteTask<T>::ExecuteL()
    {
    (iObject.*iMethod)(iEntries);
    Reset();
    }

// ---------------------------------------------------------------------------
// TDeleteTask<T>::Entries
// ---------------------------------------------------------------------------
//
template <class T> RFsTreeItemIdList& TDeleteTask<T>::Entries()
    {
    return iEntries;
    }

// ---------------------------------------------------------------------------
// TDeleteTask<T>::Reset
// ---------------------------------------------------------------------------
//
template <class T> void TDeleteTask<T>::Reset()
    {
    iEntries.Reset();
    }


// CMailListModelUpdater


// ---------------------------------------------------------------------------
// CMailListModelUpdater::NewL
// ---------------------------------------------------------------------------
//
CMailListModelUpdater* CMailListModelUpdater::NewL()
    {
    CMailListModelUpdater* self = new (ELeave) CMailListModelUpdater();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CMailListModelUpdater::ConstructL
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// ---------------------------------------------------------------------------
// CMailListModelUpdater::CMailListModelUpdater
// ---------------------------------------------------------------------------
//
CMailListModelUpdater::CMailListModelUpdater() : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    iBlockSize = KCMsgBlock;
    }

// ---------------------------------------------------------------------------
// CMailListModelUpdater::~CMailListModelUpdater
// ---------------------------------------------------------------------------
//
CMailListModelUpdater::~CMailListModelUpdater()
    {
    iObserver = NULL;
    Cancel();
    iSorting.Close();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
// Returns arrays for sorting parameters. Updater owns the arrays because
// it has to stay alive as long as the iterator is being used.
// ---------------------------------------------------------------------------
//
RArray<TFSMailSortCriteria>& CMailListModelUpdater::Sorting()
    {
    iSorting.Reset();
    return iSorting;
    }

// ---------------------------------------------------------------------------
// Update the mail list model from given iterator. Update progress will be
// informed to the observer.
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::UpdateModelL(MObserver& aObserver, 
                                         MFSMailIterator* aIterator, TInt aBlockSize)
    {
    Cancel();
    iObserver = &aObserver;
    iIterator = aIterator;
    Signal(EInitialize);
    iBlockSize = aBlockSize;
    }

// ---------------------------------------------------------------------------
// Internal method. Sets new state and signals itself.
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::Signal(TState aState, TInt /*aError*/)
    {
    const TInt KDelayInMicroSeconds = 5 * 1000; // 5 ms
    
    // timer used to get CAknWaitDialog animation working
    iState = aState;
    iTimer.After(iStatus, KDelayInMicroSeconds);
    SetActive();
    
    
    //iState = aState;
    //iStatus = KRequestPending;
    //SetActive();
    //TRequestStatus* status = &iStatus;
    //User::RequestComplete(status, aError);
    }

// ---------------------------------------------------------------------------
// Initialization state. Reset update and call UpdateBeginL() for the observer.
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::InitializeL()
    {
    iParentId = KFsTreeRootID;
    iId = TFSMailMsgId();
    iItemsFetched = 0;
    iObserver->UpdateBeginL();
    Signal(EFetch);
    }

// ---------------------------------------------------------------------------
// Fetch state. Fetch next messages and if there is more messages signal
// fetch state again OR proceed to finalizing state.
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::FetchL()
    {
    TInt blockSize = iBlockSize;
    if ( iItemsFetched == 0 )
        {
        blockSize = KCMsgBlock;
        }

    RPointerArray<CFSMailMessage> messages(blockSize);
    CleanupClosePushL(messages);
   
    const TBool moreMessages(iIterator->NextL(iId, blockSize, messages));

    if (messages.Count() > 0)
        {
        iItemsFetched += messages.Count();
        iId = messages[messages.Count() - 1]->GetMessageId();
        iObserver->UpdateProgressL(iParentId, messages);
        }
    
    CleanupStack::PopAndDestroy(); // messages.Close()
    
    if (moreMessages && iItemsFetched < KMaxItemsFethed)
        {
        Signal(EFetch);
        }
    else
        {
        Signal(EFinalize);
        }
    }

// ---------------------------------------------------------------------------
// Finalizing state. Notify observer that model update has been done and
// free the resources.
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::FinalizeL()
    {
    iObserver->UpdateCompleteL();
    Reset();
    }

// ---------------------------------------------------------------------------
// If the state is anything but EIdle, returns ETrue.
// ---------------------------------------------------------------------------
//
TBool CMailListModelUpdater::IsUpdating() const
    {
    return iState > EIdle;
    }

// ---------------------------------------------------------------------------
// Reset the state to EIdle and free resources.
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::Reset()
    {
    iSorting.Reset();
    SafeDelete(iIterator);
    iState = EIdle;
    }

// ---------------------------------------------------------------------------
// Active objects RunL()
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::RunL()
    {
    const TInt error(iStatus.Int());
    if (!error)
        {
        switch (iState)
            {
            case EInitialize:
                InitializeL();
                break;
            case EFetch:
                FetchL();
                break;
            case EFinalize:
            default:
                FinalizeL();
                break;
            }
        }
    else
        {
        User::Leave(error);  // causes RunError to be called
        }
    }

// ---------------------------------------------------------------------------
// Update has been cancelled. Inform the observer and free resources.
// ---------------------------------------------------------------------------
//
void CMailListModelUpdater::DoCancel()
    {
    if (iObserver)
        {
        iObserver->UpdateCancelled(IsUpdating());
        }
    Reset();
    
    iTimer.Cancel();
    }
// ---------------------------------------------------------------------------
// CMailListModelUpdater::RunError
// ---------------------------------------------------------------------------
//
TInt CMailListModelUpdater::RunError(TInt aError)
    {
    if ( aError != KErrNone )
        {
        TRAP_IGNORE( iObserver->UpdateErrorL( aError ) );
        }
    
    return aError;
    }





// ---------------------------------------------------------------------------
// Static constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMailListVisualiser* CFSEmailUiMailListVisualiser::NewL(CAlfEnv& aEnv,
																 CFreestyleEmailUiAppUi* aAppUi,
																 CAlfControlGroup& aMailListControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiMailListVisualiser* self = CFSEmailUiMailListVisualiser::NewLC(aEnv, aAppUi, aMailListControlGroup );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Static constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMailListVisualiser* CFSEmailUiMailListVisualiser::NewLC(CAlfEnv& aEnv,
																  CFreestyleEmailUiAppUi* aAppUi,
																  CAlfControlGroup& aMailListControlGroup)
    {
    FUNC_LOG;
    CFSEmailUiMailListVisualiser* self =
        new (ELeave) CFSEmailUiMailListVisualiser( aEnv, aAppUi, aMailListControlGroup );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ConstructL()
	{
    FUNC_LOG;

	BaseConstructL( R_FSEMAILUI_MAIL_LIST_VIEW );

	iMailListModelUpdater = CMailListModelUpdater::NewL();

	// Don't construct this anywhere else than here.
	// Don't delete this until in the destructor to avoid NULL checks.
    iModel = CFSEmailUiMailListModel::NewL( &iAppUi );

	// Set list as initial focused control
	iFocusedControl = EMailListComponent;
	iThisViewActive = EFalse;
	iSkinChanged = EFalse;

	iFirstStartCompleted = EFalse;

	iAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
	iAsyncRedrawer = new (ELeave) CAsyncCallBack( CActive::EPriorityLow );
	iLastFocus = EFalse;
    iDeleteTask = new (ELeave) TDeleteTask<CFSEmailUiMailListVisualiser> (*this, HandleDeleteTaskL);

    iNewMailTimer = CFSEmailUiGenericTimer::NewL( this );
    iSortTimer = CFSEmailUiGenericTimer::NewL( this );
    iSortState = ESortNone;

	iTouchFeedBack = MTouchFeedback::Instance();
 	}

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
// CFSEmailUiMailListVisualiser::DoFirstStartL()
// Purpose of this function is to do first start things only when list is
// really needed to be shown. Implemented to make app startuo faster.
void CFSEmailUiMailListVisualiser::DoFirstStartL()
    {
    FUNC_LOG;
    // Create mail list updater timer
    iMailListUpdater = CMailListUpdater::NewL( this );

    // Set initial sort criteria
    iCurrentSortCriteria.iField = EFSMailSortByDate;
    iCurrentSortCriteria.iOrder = EFSMailDescending;

    // Create screen control and anchor layout
    iMailListControl = CFreestyleEmailUiMailListControl::NewL( iEnv, this );
    iScreenAnchorLayout = CAlfAnchorLayout::AddNewL( *iMailListControl );
    iScreenAnchorLayout->SetFlags(EAlfVisualFlagAutomaticLocaleMirroringEnabled);

    iTouchManager = CEUiEmailListTouchManager::NewL(*this);

    // Create control bar control and append to control group and anchor layout
    iControlBarControl = CFsControlBar::NewL( iEnv );
    iControlBarControl->AddObserverL( *this );
    iControlBarControl->AddObserverL( *iTouchManager );

    CreateControlBarLayoutL();
    SetMailListLayoutAnchors();

    iControlBarControl->SetSelectorTransitionTimeL( KControlBarTransitionTime );
    iControlBarControl->ClearBackgroundColor();
    iControlBarLayout = CAlfDeckLayout::AddNewL( *iControlBarControl, iScreenAnchorLayout );
    iControlBarVisual = iControlBarControl->Visual();

    // Create list and append to control group and anchor layout

    iListLayout = CAlfDeckLayout::AddNewL( *iMailListControl, iScreenAnchorLayout );
    iMailTreeListVisualizer = CFsTreeVisualizerBase::NewL(iMailListControl, *iListLayout);
    iMailTreeListVisualizer->SetFlipState( iKeyboardFlipOpen );
    iMailTreeListVisualizer->SetFocusVisibility( iFocusVisible );
    iMailList = CFsTreeList::NewL(*iMailTreeListVisualizer, iEnv );

    // Set mark type and icon
    iMailList->SetMarkTypeL( CFsTreeList::EFsTreeListMultiMarkable );
    iMailList->SetIndentationL(0);

    ControlGroup().AppendL( iMailListControl );
    ControlGroup().AppendL( iControlBarControl );
    ControlGroup().AppendL( iMailList->TreeControl() );

    iTreeItemArray.Reset();

    // Set empty text
    HBufC* emptyText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_EMPTY_MSG_LIST_TEXT );
    iMailTreeListVisualizer->SetEmptyListTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );

    iFocusedTextColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
    iNormalTextColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
    iNodeTextColor = iAppUi.LayoutHandler()->ListNodeTextColor();
    iMailTreeListVisualizer->RootNodeVisualizer()->SetNormalStateTextColor( iNormalTextColor );

    // Set page up and page down keys
    iMailTreeListVisualizer->AddCustomPageUpKey( EStdKeyPageUp );
    iMailTreeListVisualizer->AddCustomPageDownKey( EStdKeyPageDown );

    // Set selector brushes
    CAlfBrush* listSelectorBrush = iAppUi.FsTextureManager()->ListSelectorBrushL(); // not owned
    iMailTreeListVisualizer->SetSelectorPropertiesL( listSelectorBrush, 1.0, CFsTreeVisualizerBase::EFsSelectorMoveSmoothly );

    CAlfBrush* cbSelectorBrush = iAppUi.FsTextureManager()->NewCtrlBarSelectorBrushLC();
    iControlBarControl->SetSelectorImageL( cbSelectorBrush );
    CleanupStack::Pop( cbSelectorBrush ); // ownership transferred to control bar

    iNewEmailText = StringLoader::LoadL( R_COMMAND_AREA_NEW_EMAIL );

    // Set menu, mark and background icons
    iMailTreeListVisualizer->SetMarkIcon( iAppUi.FsTextureManager()->TextureByIndex( EListControlMarkIcon ) );
    iMailTreeListVisualizer->SetMenuIcon( iAppUi.FsTextureManager()->TextureByIndex( EListControlMenuIcon ) );
    iMailList->AddObserverL( *this );
    iMailList->AddObserverL( *iTouchManager );
    // Initializing the default stylus long tap popup menu
    if( !iStylusPopUpMenu )
        {
        TPoint point( 0, 0 );
        iStylusPopUpMenu = CAknStylusPopUpMenu::NewL( this , point );
		TResourceReader reader;
		iCoeEnv->CreateResourceReaderLC( reader,
				R_STYLUS_POPUP_MENU_MESSAGE_LIST_VIEW );
		iStylusPopUpMenu->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader
        }

    iAppUi.LayoutHandler()->SetListMarqueeBehaviour( iMailList );

    iDateChangeTimer = CDateChangeTimer::NewL( *this );
    iDateChangeTimer->Start();

    UpdateMailListSettingsL();

    iMailList->SetIndentationL(0);
    iMailList->SetLoopingType( EFsTreeListLoopingDisabled );
    iMailTreeListVisualizer->SetItemExpansionDelay( iAppUi.LayoutHandler()->ListItemExpansionDelay() );
    iMailList->SetScrollTime( iAppUi.LayoutHandler()->ListScrollingTime(), 0.5 );

    iFirstStartCompleted = ETrue;
    }


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMailListVisualiser::CFSEmailUiMailListVisualiser( CAlfEnv& aEnv,
    CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aMailListControlGroup )
    : CFsEmailUiViewBase( aMailListControlGroup, *aAppUi ),
    iEnv( aEnv ),
    iListMarkItemsState( ETrue ), //Initlly list has no markings
    iMoveToFolderOngoing( EFalse ),
    iConsumeStdKeyYes_KeyUp( EFalse ), // use to prevent Call application execution if call for contact processed
    iMailOpened( EFalse ),
	iMarkingMode( EFalse ),
    iMarkingModeWaitingToExit( EFalse ),
    iMarkingModeTextVisual( NULL )
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMailListVisualiser::~CFSEmailUiMailListVisualiser()
    {
    FUNC_LOG;
/*
    if (iExtension)
        {
        iAppUi.GetMailClient()->ReleaseExtension(iExtension);
        }
*/
    SafeDelete(iMailListModelUpdater);
    SafeDelete(iMailFolder);
    delete iTouchManager;
    delete iStylusPopUpMenu;
    delete iMailList;
    delete iNewEmailText;

	// Don't construct this anywhere else than in constructor.
	// Don't delete anywhere else thatn here to avoid NULL checks.
    delete iModel;

    delete iDeleteTask;
    DeleteSortWaitNote();
    delete iNewMailTimer;
    delete iSortTimer;
    iNewMailIds.Close();
    }

void CFSEmailUiMailListVisualiser::PrepareForExit()
    {
    FUNC_LOG;
    iMailListModelUpdater->Cancel();
    if ( iMsgNoteTimer )
        {
        // delete also cancels timer
        SafeDelete(iMsgNoteTimer);
        }
    if ( iDateChangeTimer )
        {
        // delete also cancels timer
        SafeDelete(iDateChangeTimer);
        }
    if ( iMailListUpdater )
        {
        // delete also cancels timer
        SafeDelete(iMailListUpdater);
        }
    if ( iAsyncRedrawer )
        {
        SafeDelete(iAsyncRedrawer);
        }
    if ( iAsyncCallback )
        {
        SafeDelete(iAsyncCallback);
        }
    if ( iMailList )
        {
        iMailList->RemoveObserver( *this );
        }
    if ( iControlBarControl )
        {
        iControlBarControl->RemoveObserver( *this );
        }

    if ( iNewMailTimer )
        {
        // delete also cancels timer
        SafeDelete( iNewMailTimer );
        }

    if ( iSortTimer )
        {
        // delete also cancels timer
        SafeDelete( iSortTimer );
        }
    
    SafeDelete(iMailFolder);
    iTreeItemArray.Reset();
	// Reset, not delete to avoid NULL checks.
    iModel->Reset();
    // Resources freed, new start required
    iFirstStartCompleted = EFalse;
    }

// ---------------------------------------------------------------------------
// Returns reference to mail list.
// ---------------------------------------------------------------------------
//
CFsTreeList& CFSEmailUiMailListVisualiser::GetMailList()
	{
    FUNC_LOG;
	return *iMailList;
	}

// ---------------------------------------------------------------------------
// @see CMailListModelUpdater::MObserver::UpdateErrorL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateErrorL(TInt aError)
    {
    FUNC_LOG;
    
    DeleteSortWaitNote();
    User::Leave(aError);
    }

// ---------------------------------------------------------------------------
// @see CMailListModelUpdater::MObserver::UpdateBeginL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateBeginL()
    {
    FUNC_LOG;
    iMailList->BeginUpdate();
    iMailList->RemoveAllL();
    iTreeItemArray.Reset();
    iModel->Reset();
    iMailList->EndUpdateL();

    SetListAndCtrlBarFocusL();
    }

// ---------------------------------------------------------------------------
// @see CMailListModelUpdater::MObserver::UpdateProgressL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateProgressL(TFsTreeItemId& aParentId, RPointerArray<CFSMailMessage>& aMessages)
    {
    FUNC_LOG;
    
    const TInt itemsInModel(iModel->Count());
    CreateModelItemsL(aMessages);
    RefreshListItemsL(aParentId, itemsInModel, iModel->Count());
    }

// ---------------------------------------------------------------------------
// @see CMailListModelUpdater::MObserver::UpdateCompleteL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateCompleteL()
    {
    FUNC_LOG;
    TBool sorting = EFalse;
    if ( iSortWaitNote )
        {
        sorting = ETrue;
        }
    DeleteSortWaitNote();

    if ( !iModel->Count() )
        {
        iFocusedControl = EControlBarComponent;
        }
    else
        {
        iFocusedControl = EMailListComponent;
        if (iMailList->FocusedItem() == KFsTreeNoneID)
            {
            iMailList->SetFocusedItemL( iTreeItemArray[0].iListItemId );
            }
        }
    SetListAndCtrlBarFocusL();
    
    if( !CheckAutoSyncSettingL() && !sorting )
        {
        iAppUi.StartMonitoringL();
        }
    
    TIMESTAMP( "Locally stored messages fetched for message list" );
    }

// ---------------------------------------------------------------------------
// @see CMailListModelUpdater::MObserver::UpdateCancelled
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateCancelled(const TBool aForceRefresh)
    {
    FUNC_LOG;
    
    iForceRefresh = aForceRefresh;
    DeleteSortWaitNote();
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::DeleteSortWaitNote
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DeleteSortWaitNote()
    {
    if ( iSortWaitNote )
        {
        TRAP_IGNORE( iSortWaitNote->ProcessFinishedL() );
        iSortWaitNote = NULL;
        }
    }


// ---------------------------------------------------------------------------
// Asynchronous mail list model sort.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SortMailListModelAsyncL()
    {
    FUNC_LOG;

    DeleteSortWaitNote();
  
    TBool ret = UpdateMailListModelAsyncL( KCMsgBlockSort );
    if ( ret )
        {
        TFsEmailUiUtility::ShowWaitNoteL( iSortWaitNote, R_FSE_WAIT_SORTING_TEXT, EFalse, ETrue );
        }
    }

// ---------------------------------------------------------------------------
// Asynchronous mail list model update.
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::UpdateMailListModelAsyncL(TInt aBlockSize)
    {
    FUNC_LOG;
    TBool ret = EFalse;
    if ( iMailFolder )
        {
        TFSMailDetails details( EFSMsgDataEnvelope );
        RArray<TFSMailSortCriteria>& sorting(iMailListModelUpdater->Sorting());
        sorting.AppendL( iCurrentSortCriteria );
        if ( iCurrentSortCriteria.iField != EFSMailSortByDate )
            {
            // Add date+descending as secondary sort criteria if primary field is something else than date
            TFSMailSortCriteria secondarySortCriteria;
            secondarySortCriteria.iField = EFSMailSortByDate;
            secondarySortCriteria.iOrder = EFSMailDescending;
            sorting.AppendL( secondarySortCriteria );
            }
        // List all or maximum number of messages
        iMailListModelUpdater->UpdateModelL(*this, iMailFolder->ListMessagesL(details, sorting), aBlockSize);
        ret = ETrue;
        }
    else
        {
        UpdateCompleteL();
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateMailListModelL()
    {
    FUNC_LOG;
    // Make sure asynchronous update is not going on
    iMailListModelUpdater->Cancel();
    // Reset model with each update
	iModel->Reset();
	if ( iMailFolder )
		{
		// Update folder if provided, otherwise use current folder
		RPointerArray<CFSMailMessage> folderMessages( KCMsgBlock );
		CleanupClosePushL( folderMessages );

		RPointerArray<CFSMailMessage> tempFolderMessages( KCMsgBlock );
        CleanupClosePushL( tempFolderMessages );

	 	TFSMailDetails details( EFSMsgDataEnvelope );
	    RArray<TFSMailSortCriteria> sorting;
		CleanupClosePushL( sorting );
	    sorting.Append( iCurrentSortCriteria );
	    if ( iCurrentSortCriteria.iField != EFSMailSortByDate )
	    	{
            // Add date+descending as secondary sort criteria if primary field is something else than date
            TFSMailSortCriteria secondarySortCriteria;
            secondarySortCriteria.iField = EFSMailSortByDate;
            secondarySortCriteria.iOrder = EFSMailDescending;
            sorting.Append( secondarySortCriteria );
	    	}

	    // List all or maximum number of messages
	    MFSMailIterator* iterator = iMailFolder->ListMessagesL( details, sorting );
		CleanupDeletePushL( iterator ); // standard CleanupStack::PushL does not work with non-C-class pointer

		// Use iterator to get messages in peaces of KCMsgBlock to avoid OOM in FSStore
		TFSMailMsgId dummy;
        TBool moreMessagesToFollow = iterator->NextL( dummy, KCMsgBlock, folderMessages  );
        for ( TInt i = KCMsgBlock; i < KMaxItemsFethed && moreMessagesToFollow ; i += KCMsgBlock )
            {
            tempFolderMessages.Reset();
            moreMessagesToFollow = iterator->NextL( folderMessages[i-1]->GetMessageId(), KCMsgBlock, tempFolderMessages  );
            for ( TInt a=0 ; a<tempFolderMessages.Count() ; a++)
                {
                folderMessages.Append(tempFolderMessages[a]);
                }
            }

		CleanupStack::PopAndDestroy( iterator );
		CleanupStack::PopAndDestroy( &sorting );

		// Update mail list model based on sorting criteria
		CreateModelItemsL( folderMessages );

		CleanupStack::PopAndDestroy( &tempFolderMessages );
		CleanupStack::PopAndDestroy( &folderMessages );
		}
    }


// ---------------------------------------------------------------------------
// CreateModelItemsL
// This function creates model items for the given messages and appends them
// to the model. Also title divider items are created when necessary.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::CreateModelItemsL( RPointerArray<CFSMailMessage>& aMessages )
	{
    FUNC_LOG;
	// New Items
	CFSEmailUiMailListModelItem* newItem(NULL);

	// Draw first separator if there are messages.
	if ( aMessages.Count() && iNodesInUse == EListControlSeparatorEnabled )
		{
		if (iModel->Count())
		    {
		    CFSMailMessage* nextMessage = aMessages[0];
		    CFSEmailUiMailListModelItem* previousMessage(static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(iModel->Count() - 1)));
            if ( previousMessage ) // Safety
                {
                TBool needANewDivider =
                    !MessagesBelongUnderSameSeparatorL( previousMessage->MessagePtr(), *nextMessage );
                if ( needANewDivider )
                    {
                    newItem = CreateSeparatorModelItemLC( *nextMessage );
                    iModel->AppendL( newItem );
                    CleanupStack::Pop( newItem );
                    }
                }
            }
		else
		    {
	    newItem = CreateSeparatorModelItemLC( *aMessages[0] );
	    iModel->AppendL( newItem );
		CleanupStack::Pop( newItem );
		}
		}

	// Start appending items
    for (int i = 0; i < aMessages.Count(); i++)
	    {
	    CFSMailMessage* curMessage = aMessages[i];
	    if ( curMessage )
	        {
	        newItem = CFSEmailUiMailListModelItem::NewL( curMessage, ETypeMailItem );
	        CleanupStack::PushL( newItem );
	        iModel->AppendL( newItem );
	        CleanupStack::Pop( newItem );

	        // Append new separator if needed
	        if ( i != aMessages.Count()-1 && iNodesInUse == EListControlSeparatorEnabled )
	            {
	            CFSMailMessage* nextMessage = aMessages[i+1];   // This msg is next in the list

	            if ( nextMessage )
	                {
	                TBool needANewDivider =
	                    !MessagesBelongUnderSameSeparatorL( *curMessage, *nextMessage );
	                if ( needANewDivider )
	                    {
	                    newItem = CreateSeparatorModelItemLC( *nextMessage );
	                    iModel->AppendL( newItem );
	                    CleanupStack::Pop( newItem );
	                    }
	                }
	            }
	        }
	    }
	}

// ---------------------------------------------------------------------------
// CreateSeparatorModelItemLC
// Create separator model item for the given message. Separator text depends
// on active sorting mode.
// ---------------------------------------------------------------------------
//
CFSEmailUiMailListModelItem* CFSEmailUiMailListVisualiser::CreateSeparatorModelItemLC( CFSMailMessage& aMessage ) const
    {
    FUNC_LOG;
    CFSEmailUiMailListModelItem* separator =
        CFSEmailUiMailListModelItem::NewL( &aMessage, ETypeSeparator );
    CleanupStack::PushL( separator );

    HBufC* separatorText = NULL;

    switch ( iCurrentSortCriteria.iField )
        {
        case EFSMailSortByFlagStatus:
            {
            if ( aMessage.IsFlagSet( EFSMsgFlag_FollowUp ) )
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_FOLLOW_UP );
                }
            else if ( aMessage.IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_FLAG_COMPLETE );
                }
            else
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_NO_FLAG );
                }
            }
            break;
        case EFSMailSortByPriority:
            {
            if ( aMessage.IsFlagSet( EFSMsgFlag_Important ) )
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_PRIO_HIGH );
                }
            else if ( aMessage.IsFlagSet( EFSMsgFlag_Low ) )
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_PRIO_LOW );
                }
            else
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_PRIO_NORMAL );
                }
            }
            break;
        case EFSMailSortByRecipient:
        case EFSMailSortBySender:
            {
            TInt folderType(EFSInbox);
            if( iMailFolder )
                {
                folderType = iMailFolder->GetFolderType();
                }
            TBool useRecipient = ( folderType == EFSSentFolder ||
                                   folderType == EFSDraftsFolder ||
                                   folderType == EFSOutbox );

            CFSMailAddress* fromAddress(0);
            // Check address based on whether we are using recipient or sender sort
            if ( useRecipient )
                {
                RPointerArray<CFSMailAddress>& toArray = aMessage.GetToRecipients();
                if ( toArray.Count() )
                    {
                    fromAddress = toArray[0];
                    }
                }
            else
                {
                fromAddress = aMessage.GetSender();
                }
            TDesC* diplayName(0);
            if ( fromAddress )
                {
                diplayName = &fromAddress->GetDisplayName();
                }

            if ( fromAddress && diplayName && diplayName->Length() != 0 )
                {
                separatorText = diplayName->AllocLC();
                }
            else if ( fromAddress && fromAddress->GetEmailAddress().Length() != 0 )
                {
                separatorText = fromAddress->GetEmailAddress().AllocLC();
                }
            else
                {
                if ( useRecipient )
                    {
                    separatorText = KNullDesC().AllocLC();
                    }
                else
                    {
                    separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_SENDER_INFO_AVAILABLE );
                    }
                }
			// Get rid of possible unwanted characters in display name
           	if ( separatorText )
           		{
	           	TFsEmailUiUtility::StripDisplayName( *separatorText );
           		}
            }
            break;
        case EFSMailSortByAttachment:
            {
            if ( aMessage.IsFlagSet( EFSMsgFlag_Attachments ) )
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_ATTACHMENTS );
                }
            else
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_NO_ATTACHMENTS );
                }
            }
            break;
        case EFSMailSortByUnread:
            {
            if ( aMessage.IsFlagSet( EFSMsgFlag_Read ) )
                {
                separatorText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SEPARATOR_READ_MSGS );
                }
            else
                {
                separatorText = StringLoader::LoadLC(R_FREESTYLE_EMAIL_UI_SEPARATOR_UNREAD_MSGS);
                }
            }
            break;
        case EFSMailSortBySubject:
            {
            separatorText = TFsEmailUiUtility::CreateSubjectWithoutLocalisedPrefixLC( &aMessage );
            }
            break;
        case EFSMailSortByDate:
            {
            TBool eventToday = EFalse;
            TBool eventYesterday = EFalse;
            HBufC* weekDay = TFsEmailUiUtility::WeekDayTextFromMsgLC( &aMessage, ETrue, EFalse, eventToday, eventYesterday );
            if ( eventToday || eventYesterday )
                {
                separatorText = weekDay;
                }
            else
                {
                HBufC* dateTextFromMsg = TFsEmailUiUtility::DateTextFromMsgLC( &aMessage );
                separatorText = HBufC::NewL( weekDay->Length() +
                                             KSpace().Length() +
                                             dateTextFromMsg->Length() );
                separatorText->Des().Append( *weekDay );
                separatorText->Des().Append( KSpace );
                separatorText->Des().Append( *dateTextFromMsg );
                CleanupStack::PopAndDestroy( dateTextFromMsg );
                CleanupStack::PopAndDestroy( weekDay );
                CleanupStack::PushL( separatorText );
                }
            }
            break;
        default:
            {
            separatorText = KNullDesC().AllocLC();
            }
            break;
        }

    separator->SetSeparatorTextL( *separatorText );
    CleanupStack::PopAndDestroy( separatorText );

    return separator;
    }

// ---------------------------------------------------------------------------
// MessagesBelongUnderSameSeparatorL
// Checks if the given messages belong under the same title divider. This
// depends on the currect sorting mode.
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::MessagesBelongUnderSameSeparatorL(
                                            const CFSMailMessage& aMessage1,
                                            const CFSMailMessage& aMessage2 ) const
    {
    FUNC_LOG;
    return CFSEmailUiMailListModel::MessagesBelongUnderSameSeparatorL( aMessage1,
                                                                       aMessage2,
                                                                       iCurrentSortCriteria.iField );
    }

// ---------------------------------------------------------------------------
// InsertNewMessagesL
// Gets new message items from the framework using the given IDs and inserts
// them in the correct place within the mail list. Both model and tree list
// are updated during this function.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::InsertNewMessagesL( const RArray<TFSMailMsgId>& aMessages )
    {
    FUNC_LOG;
    iModel->SetSortCriteria( iCurrentSortCriteria );

    // Do complete refresh in case the list is not currently correctly ordered
    // (alternatively, we could first reorder the existing items and then insert
    // the new ones)
    if ( iListOrderMayBeOutOfDate )
        {
        if ( iMailListUpdater )
            {
            if ( iMailListUpdater->IsActive() )
                {
                iMailListUpdater->Stop();
                }
            iMailListUpdater->StartL();
            }
        }
    // Otherwise, just add the new items to correct places.
    else
        {
        // Set extended status before inserting messages
        SetMailListItemsExtendedL();
        TInt count(0);
        count = aMessages.Count();

        for ( TInt i = 0 ; i < count ; ++i )
            {
            // Make sure we don't add duplicate items.
            TInt existingIdx = ItemIndexFromMessageId( aMessages[i] );
            if ( existingIdx < 0 )
                {
                iNewMailIds.Append( aMessages[ i ] );
                }
            }

        // start timer only when all conditions are met:
        //   timer isn`t already active
        //   there are new mails in the array
        //   timer event isn`t processing
        //   sorting timer isn't running
        if ( !iNewMailTimer->IsActive() && iNewMailIds.Count() && 
             iNewMailTimer->iStatus != KErrInUse  && iSortState == ESortNone )
            {
            iNewMailTimer->Start( KNewMailTimerDelay );
            }
        }
    }


// ---------------------------------------------------------------------------
// TimerEventL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer )
    {
	
    if( aTriggeredTimer == iSortTimer )
        {
        switch ( iSortState )
            {
            case ESortNone:
                {
                iSortTryCount = 0;
                iSortState = ESortRequested;
                break;
                }
            case ESortRequested:
                {
                if( !iNewMailTimer->IsActive() && iNewMailTimer->iStatus != KErrInUse )
        	{
                    // Size sorting does not use nodes, so disable those, otherwise check from CR
                    iNodesInUse = iAppUi.GetCRHandler()->TitleDividers();
                    SetSortButtonIconL();

                    iFocusedControl = EControlBarComponent;
                    iMailList->SetFocusedL( EFalse );
                    iControlBarControl->SetFocusByIdL( iSortButtonId );

                    SortMailListModelAsyncL();  // sort can take long time    
                    SetMskL();
                    iSortState = ESortStarted;
                    }
                else
                    {
                    iSortTryCount = 0;
                    iSortState = ESortPostponed;
                    iNewMailTimer->Stop(); // stop mail timer
                    }
                break;
                }
            case ESortPostponed:
                {
                if ( !iNewMailTimer->IsActive() && iNewMailTimer->iStatus != KErrInUse )
                    {
                    iSortState = ESortRequested; // start request again
                    }
                else
                    {
                    iSortTryCount++;
                    }
                if( iSortTryCount >= KSortCountdown )// repeat a few times
                    {
                    iSortTryCount = 0; // reset
                    iSortState = ESortStartError; // can't start sorting  some error
                    }
                break;
                }
            case ESortStarted:
                {
                if ( !iSortWaitNote )// it will restart the timer elsewhere (see below)
                    {
                    iSortState = ESortCompleted;
                    }
                break;
                }
            case ESortStartError:
            case ESortCompleted:
                {
            	if( !iNewMailTimer->IsActive() )
            	    {
            	    // refresh the whole mail list if list was sorted
            	    if(iSortState != ESortStartError )
            	    	{
            	        RefreshL();
            	    	}
            	    // stop sorting process
                    iSortState = ESortNone;
                    iSortTimer->Stop();
            	    // restart if more messages available
            	    if ( iNewMailIds.Count() )
                    {
                    iNewMailTimer->Start( KNewMailTimerDelay );
                    }
                return;
            		}
                iNewMailTimer->Stop();
            	break;
                }
            default :
                {
                // do nothing
                }
            }
        // start timer again
        if ( !iSortTimer->IsActive() )
            {
            iSortTimer->Start( KSortTimerDelay );
            }
        }
    
    if( aTriggeredTimer == iNewMailTimer )
        {
        if ( ! iThisViewActive ) // don't proceed if view is not active 
            { 
            iNewMailTimer->Stop();
            return;
            }
        
        TInt count = Min( KNewMailMaxBatch, iNewMailIds.Count() );
        CFSMailClient* mailClient = iAppUi.GetMailClient();

        // Enter critical section
        // Because CFSMailClient::GetMessageByUidL can use CActiveSchedulerWait
        // to ensure that only one TimerEventL method is processed at time
        // CFSEmailUiGenericTimer`s iStatus will be used as mutex
        iNewMailTimer->iStatus = KErrInUse;
        iModel->SetSortCriteria(iCurrentSortCriteria);

        for ( TInt i = 0; i < count; i++ )
            {
            // stop synchronization if mail list is being sorted
            if( iSortState != ESortNone || iSortWaitNote )
                {
                iNewMailTimer->iStatus = KErrNone;
                iNewMailTimer->Stop();
                return; // leave  method
                }
    
            CFSMailMessage* msgPtr(NULL);
            TRAPD( err, msgPtr = mailClient->GetMessageByUidL( iAppUi.GetActiveMailboxId(),
                                                               iMailFolder->GetFolderId(),
                                                               iNewMailIds[ 0 ],
                                                               EFSMsgDataEnvelope ) );
            if ( KErrNone != err ) 
                {
                iNewMailTimer->iStatus = KErrNone;
                User::Leave(err);
                }
            if ( msgPtr != NULL )
 {
                __ASSERT_DEBUG( FolderId() == msgPtr->GetFolderId(), User::Invariant() );
                CleanupStack::PushL( msgPtr );
                //first item - show scrollbar
                //last item - update scrollbar
                TBool allowRefresh = ( i == 0 || i == count - 1 );
                InsertNewMessageL( msgPtr, allowRefresh );
                CleanupStack::Pop( msgPtr ); // ownership transferred to model
                }
            // pop processed id from the queue, this is single thread operation
            iNewMailIds.Remove( 0 ); 
            }
            
        // End critical section
        iNewMailTimer->iStatus = KErrNone;

        // if timer stoped then restart if more messages available
        if ( iNewMailIds.Count() && ! iNewMailTimer->IsActive() && iSortState == ESortNone )
            {
            iNewMailTimer->Start( KNewMailTimerDelay );
            }
        }//iNewMailTimer
    }

// ---------------------------------------------------------------------------
// InsertNewMessageL
// Creates a single new message item to the model and tree list from the given
// message pointer. Ownership of the message is transferred. Also new
// separator item is created if necessary.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::InsertNewMessageL( CFSMailMessage* aNewMessage, const TBool aAllowRefresh )
    {
    FUNC_LOG;
    // Use simple heuristic rule: if the first item is highlighted before the new
    // item is added, the highlight is forced to remain on the (possibly new) first
    // item. Otherwise the highligh stays on the same item as before.
    TBool firstItemWasFocused =
        ( iTreeItemArray.Count() && iTreeItemArray[0].iListItemId == iMailList->FocusedItem() );

    CFSEmailUiMailListModelItem* newItem =
        CFSEmailUiMailListModelItem::NewLC( aNewMessage, ETypeMailItem );
    TInt idx = KErrNotFound;
    TFsTreeItemId parentId = KFsTreeRootID;
    TInt childIdx = KErrNotFound;
    TInt moveViewPortPosition = 0;

    if ( !iNodesInUse )
        {
        // Simple case: nodes are not in use. Just insert to correct place under root node.
        idx = iModel->GetInsertionPointL( *newItem );
        childIdx = idx;
        }
    else
        {
        // More complicated case: nodes are in use.
        TInt parentIdx = KErrNotFound;
        idx = iModel->GetInsertionPointL( *newItem, childIdx, parentIdx );

        CFSEmailUiMailListModelItem* parentNode = NULL;
        if ( parentIdx < 0 )
            {
            // Suitable parent node for the new item was not found and has to be created.
            parentNode = CreateSeparatorModelItemLC( *aNewMessage );
            iModel->InsertL( parentNode, idx );
            CleanupStack::Pop( parentNode );
            idx++; // The originally found index was used for the new node and the new item will reside in the next index

            // To insert the node item to the tree list, we need to figure out the index of the
            // new node under the root node. If the new item is not the last item in the list,
            // then the insertion location currently contains the node in front of which the new
            // node is inserted.
            if ( idx == iModel->Count() )
                {
                InsertNodeItemL( idx-1, KErrNotFound, aAllowRefresh ); // append
                }
            else
                {
                CFSEmailUiMailListModelItem* nextNode =
                    static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(idx) );
                TFsTreeItemId nextNodeId = nextNode->CorrespondingListId();
                TInt nodeIdxUnderRoot = iMailList->ChildIndex( KFsTreeRootID, nextNodeId );
                InsertNodeItemL( idx-1, nodeIdxUnderRoot, aAllowRefresh );
                }
            moveViewPortPosition += iAppUi.LayoutHandler()->OneLineListItemHeight();
            }
        else
            {
            // Suitable parent node exists and was found
            parentNode = static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(parentIdx) );
            }
        parentId = parentNode->CorrespondingListId();
        }

    // Insert the model item
    iModel->InsertL( newItem, idx );
    CleanupStack::Pop( newItem );

    // Insert the tree list item
    TPoint viewPortCenterPos = iMailTreeListVisualizer->ViewPosition();
    TPoint viewPortTopPos = iMailTreeListVisualizer->ViewPortTopPosition();
    TInt itemPos = iMailTreeListVisualizer->GetItemWorldPosition( idx );
    TBool refresh = viewPortTopPos.iY != 0 && itemPos < viewPortCenterPos.iY;

    if( refresh )
    	{
		InsertListItemL( idx, parentId, childIdx, EFalse );
		// viewPort update needed
		moveViewPortPosition += iAppUi.LayoutHandler()->TwoLineListItemHeight(); // add mail item height
		viewPortCenterPos.iY += moveViewPortPosition;
		iMailTreeListVisualizer->ViewPositionChanged( viewPortCenterPos, EFalse, 1 );
    	}
    else
    	{
    	InsertListItemL( idx, parentId, childIdx, aAllowRefresh );
    	}

    // Move focus after insertion if necessary
    if ( firstItemWasFocused )
        {
        TFsTreeItemId firstItemId = iMailList->Child( KFsTreeRootID, 0 );
        TFsTreeItemId focusedId = iMailList->FocusedItem();

        if ( firstItemId != focusedId )
            {
            // Set the first item as focused and set
            // list/ctrl bar focus according iFocusedControl
            // <cmail>
            //iMailList->SetFocusedItemL( firstItemId );
            iMailTreeListVisualizer->SetFocusedItemL( firstItemId, EFalse );
            // </cmail>
            SetListAndCtrlBarFocusL();
            }
        }
    }

// ---------------------------------------------------------------------------
// HighlightedIndex
// This function return highlighted inxed
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::HighlightedIndex() const
    {
    FUNC_LOG;
    TFsTreeItemId focusedId = static_cast<TFsTreeItemId>( iMailList->FocusedItem() );
    // Map id to the index in model
    TInt ret( KErrNotFound );
    if ( focusedId != KFsTreeNoneID )
        {
        for ( TInt i=0; i<iTreeItemArray.Count();i++ )
            {
            if ( focusedId == iTreeItemArray[i].iListItemId )
                {
                ret = i;
                break;
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// HighlightedIndex
// Use this function to set highlighted inxed
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetHighlightedIndexL( TInt aWantedIndex )
	{
    FUNC_LOG;
	if ( aWantedIndex >= 0 )
		{
		iMailTreeListVisualizer->SetFocusedItemL( iTreeItemArray[aWantedIndex].iListItemId );
		}
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::NewEmailsInModelL() const
	{
    FUNC_LOG;
    // <cmail>
	TInt newCount(0);
	for ( TInt i=0; i<iModel->Count();i++)
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			if ( !item->MessagePtr().IsFlagSet(EFSMsgFlag_Read) )
				{
				newCount++;
				}
			}
		}
	return newCount;
    // </cmail>
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::EmailsInModelL() const
	{
    FUNC_LOG;
    // <cmail>
	TInt ret(0);
	for ( TInt i=0; i<iModel->Count(); i++ )
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			ret++;
			}
		}
	return ret;
	// </cmail>
	}


// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TUid CFSEmailUiMailListVisualiser::Id() const
	{
    FUNC_LOG;
	return MailListId;
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ChildDoActivateL(const TVwsViewId& aPrevViewId,
                     TUid aCustomMessageId,
                     const TDesC8& aCustomMessage)
	{
    FUNC_LOG;
    TIMESTAMP( "Opening message list view" );
    
    if (iMarkingModeWaitingToExit)
        {
        if ( iAppUi.CurrentActiveView()->Id() == MailListId )
            {
            iMarkingModeWaitingToExit = EFalse;
            TRAP_IGNORE( ExitMarkingModeL() );
            }
        }

    iShowReplyAll = EFalse;

	if ( !iFirstStartCompleted )
	    {
	    DoFirstStartL();
	    }
    // set when editor was called so reset is needed i.e. here (Called by DoActivate)
   	iMailOpened = EFalse; 

    // Make sure that pending popup is not displayd
	if ( iAppUi.FolderList().IsPopupShown() )
        {
        iAppUi.FolderList().HidePopupL();
        }
    DisableMailList( EFalse );

    UpdateFolderAndMarkingModeTextsL();
    
	// inform baseView if view entered with forward navigation
	TBool forwardNavigation = EFalse;
	if ( aCustomMessageId != KStartListReturnToPreviousFolder &&
	     aCustomMessageId != TUid::Uid(KMailSettingsReturnFromPluginSettings) )
	    {
	    ViewEntered( aPrevViewId );
	    forwardNavigation = ETrue;
	    }

	if( iAppUi.CurrentFixedToolbar() )
	    {
        iAppUi.CurrentFixedToolbar()->SetToolbarVisibility( EFalse );
	    }

    // Set control bar and list layout size always in activation
    TRect clientRect = iAppUi.ClientRect();
    iScreenAnchorLayout->SetSize( clientRect.Size() );
    SetMailListLayoutAnchors();
    //if the view is already active don't update the icons so they won't "blink" 
    //when the view is activated.
    if(!iThisViewActive && !iMarkingMode)
        {     
        ScaleControlBarL();
        
        // Set icons on toolbar
        iAppUi.FsTextureManager()->ClearTextureByIndex( EListControlBarMailboxDefaultIcon );
        iFolderListButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( EListControlBarMailboxDefaultIcon ) );
        iAppUi.FsTextureManager()->ClearTextureByIndex( EListTextureCreateNewMessageIcon );
        iNewEmailButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( EListTextureCreateNewMessageIcon ) );
        iAppUi.FsTextureManager()->ClearTextureByIndex( GetSortButtonTextureIndex() );
        iSortButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( GetSortButtonTextureIndex() ) );

        SetListAndCtrlBarFocusL();
        }

	FadeOut(EFalse); // we can show now CAlfVisuals from CurrentView (to show Folders before updating emails)
    
	// Update mail list settings and date formats, is done every time
	// the user might have changed these in settings, so the list needs to refresh
	// Store previous modes
	TInt prevListMode = iListMode;
	TInt prevNodesmode = iNodesInUse;
	TAknUiZoom prevZoomLevel = iCurrentZoomLevel;
	STimeDateFormats prevDateFormats = MailListTimeDateSettings();

	// Get new settings and list drawing modes
    CAknEnv::Static()->GetCurrentGlobalUiZoom( iCurrentZoomLevel );
	UpdateMailListSettingsL();
	UpdateMailListTimeDateSettings();

	// Check for changed settings, in that case a complete list refresh is needed
	TRefreshState refreshState = ERefreshNone;
	if (aCustomMessageId == KStartListWithFolderIdFromHomeScreen)
	    {
            refreshState = EFocusChangeNeeded;
	    }
	
	if ( iSkinChanged
	     || iDateChanged
	     || iListMode != prevListMode
		 || iNodesInUse != prevNodesmode
		 || iCurrentZoomLevel != prevZoomLevel
		 || prevDateFormats.iDateFormat != iDateFormats.iDateFormat
		 || prevDateFormats.iTimeFormat != iDateFormats.iTimeFormat
		 || prevDateFormats.iAmPmPosition != iDateFormats.iAmPmPosition
		 || prevDateFormats.iDateSeparator.GetNumericValue() != iDateFormats.iDateSeparator.GetNumericValue()
		 || prevDateFormats.iTimeSeparator.GetNumericValue() != iDateFormats.iTimeSeparator.GetNumericValue() )
		{
        refreshState = EFullRefreshNeeded;
		iSkinChanged = EFalse;
		iDateChanged = EFalse;
		}

	// Store previously used mailbox and folder IDs
	TFSMailMsgId prevMailBoxId = iAppUi.GetActiveMailboxId();
	TFSMailMsgId prevFolderId = FolderId();

    CFSMailClient* mailClient = iAppUi.GetMailClient();
    User::LeaveIfNull( mailClient ); // we can't go on if no mail client is available

	// FIGURE OUT WHICH FOLDER TO ACTIVATE
    TMailListActivationData activationData;

    // Opening folder given in custom message
    if ( aCustomMessage.Length() )
        {
        TPckgBuf<TMailListActivationData> viewData;
        viewData.Copy( aCustomMessage );
        activationData = viewData();
        }

    // Returning to previous folder
    if ( aCustomMessageId == KStartListReturnToPreviousFolder ||
         aCustomMessageId == TUid::Uid(KMailSettingsReturnFromPluginSettings) )
        {
        activationData.iMailBoxId = prevMailBoxId;
        activationData.iFolderId = prevFolderId;
        }

    // Proper mailbox ID is not given in custom message. Try using MSV session.
    if ( activationData.iMailBoxId.IsNullId() )
        {
        CMsvSession* msvSession = iAppUi.GetMsvSession();
        CFSMailBox* mailBox = NULL;

        if ( msvSession )
            {
            // MSV id is passed to us in aCustomMessageId when activation happens by email key
            if ( aCustomMessageId != TUid::Null() )
                {
                // <cmail>
                TInt error = KErrNone;
                TRAP( error, mailBox = TFsEmailUiUtility::GetMailboxForMtmIdL( *mailClient, *msvSession, aCustomMessageId.iUid ) );
                // </cmail>
                if ( mailBox )
                    {
                    activationData.iMailBoxId = mailBox->GetId();
                    delete mailBox;
                    mailBox = NULL;
                    }
                }

            // Try to get MCE default mailbox if we still haven't got any other box
            if ( activationData.iMailBoxId.IsNullId() )
                {
                // <cmail>
                TInt error = KErrNone;
                TRAP( error, mailBox = TFsEmailUiUtility::GetMceDefaultMailboxL( *mailClient, *msvSession ) );
                // </cmail>
                if ( mailBox )
                    {
                    activationData.iMailBoxId = mailBox->GetId();
                    delete mailBox;
                    mailBox = NULL;
                    }
                }
            }

        }

    // If still no mailbox then use first from the list of the framework
    if ( activationData.iMailBoxId.IsNullId() )
        {
        TFSMailMsgId id;
        RPointerArray<CFSMailBox> mailboxes;
        CleanupResetAndDestroyClosePushL( mailboxes );
        mailClient->ListMailBoxes( id, mailboxes );
        if ( mailboxes.Count() > 0 )
            {
            activationData.iMailBoxId = mailboxes[0]->GetId();
            }
        CleanupStack::PopAndDestroy( &mailboxes );
        }


    // Check if we got any mailbox
    if ( activationData.iMailBoxId.IsNullId() )
        {
        // could not get mailbox so leave
        User::Leave( KErrGeneral );
        }
    else
        {
        // Set the active mailbox of AppUi. Do this also when mailbox hasn't actually
        // changed to do the autoconnect when necessary and to verify that the previously
        // active mailbox is still valid.
        // Leave is caused if mailbox is no longer available, and BaseView will take care
        // of the error handling.
        iAppUi.SetActiveMailboxL( activationData.iMailBoxId, forwardNavigation );

        // we got mailbox but no folder has been given => use Inbox
        if ( activationData.iFolderId.IsNullId() ||
             activationData.iFolderId.Id() == 0 )
            {
            activationData.iFolderId = iAppUi.GetActiveBoxInboxId();
            }
        }

    // NOW WE HAVE A VALID MAILBOX AND FOLDER ID.
    // Tries to create an extension for the Ozone plugin
    CreateExtensionL();

    // if mailbox changed stop timer driven insertion of new mails into list
    if ( activationData.iMailBoxId != prevMailBoxId )
        {
        iNewMailTimer->Cancel();
        iSortTimer->Cancel(); // stop sorting timer
        iNewMailIds.Reset();
        }

    // CHECK IF MODEL NEEDS TO BE UPDATED
    if ( activationData.iMailBoxId != prevMailBoxId ||
         activationData.iFolderId != prevFolderId ||
         activationData.iRequestRefresh ||
         iForceRefresh )
         {
         iForceRefresh = EFalse;
         // Set initial sort criteria when folder is changed
         iCurrentSortCriteria.iField = EFSMailSortByDate;
         iCurrentSortCriteria.iOrder = EFSMailDescending;
         SetSortButtonIconL();

         SafeDelete(iMailFolder);
         TRAP_IGNORE( iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL(
                 activationData.iMailBoxId, activationData.iFolderId ) );
         if ( !iMailFolder )
             {
             if ( forwardNavigation )
                 {
                 iAppUi.StartMonitoringL();
                 }
             // Safety, try to revert back to standard folder inbox
             TFSMailMsgId inboxId = iAppUi.GetActiveMailbox()->GetStandardFolderId( EFSInbox );
             iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( activationData.iMailBoxId, inboxId );
             }
         iMailList->SetFocusedItemL( KFsTreeNoneID );
         refreshState = EFullRefreshNeeded;
         }

    // Set mailbox name to status pane
    SetMailboxNameToStatusPaneL();

    // Set branded watermark and mailbox icon
    SetBrandedListWatermarkL();
    SetBrandedMailBoxIconL();
    
    // Check sync icon timer and sync status
    ConnectionIconHandling();

    // REBUILD TREE LIST IF NECESSARY
    if ( refreshState == EFullRefreshNeeded )
        {
        // Try to maintain previously active item if possible.
        // This is of course not possible if folder has changed.
        TFSMailMsgId focused = MsgIdFromListId( iMailList->FocusedItem() );

        // Clear any previous items from the screen and then make the view visible
        iMailList->BeginUpdate();
        iMailList->RemoveAllL();
        iTreeItemArray.Reset();
        iModel->Reset();
        iMailList->EndUpdateL();
        iMailList->ShowListL();

        // If coming from wizard use synchronous list updating
        if (activationData.iReturnAfterWizard)
            {
            UpdateMailListModelL();
            }
        else
            {
            UpdateMailListModelAsyncL( KCMsgBlockSort );
            }
        }
    else if(refreshState == EFocusChangeNeeded)
        {//Move focus to the beginning of the list
        TInt firstIndex(0);
        TFsTreeItemId firstItemId(KFsTreeNoneID);
        if ( iMailList->CountChildren( KFsTreeRootID ) )
            {
            firstItemId = iMailList->Child( KFsTreeRootID, firstIndex );
            }
        iMailTreeListVisualizer->SetFocusedItemL( firstItemId, EFalse );
        //if the view is already active don't update the list so it won't "blink" 
        //when the view is activated.
        if(!iThisViewActive)
            {
            iMailList->ShowListL();
            }
        }
    // THE CORRECT FOLDER IS ALREADY OPEN. CHECK IF SOME PARTIAL UPDATE IS NEEDED.
    else
        {
        iMailList->ShowListL();
        TBool manualSync = CheckAutoSyncSettingL();
        if (forwardNavigation && !manualSync)
            {
            iAppUi.StartMonitoringL();
            }
        SetListAndCtrlBarFocusL(); // ShowListL() makes list focused and this may need to be reverted
        if ( aCustomMessageId == TUid::Uid(KMailSettingsReturnFromPluginSettings) )
            {
            // Better to refresh launcher grid view because mailbox branding might be changed.
            iAppUi.LauncherGrid().SetRefreshNeeded();
            }
        // Check the validity of focused message, it may be deleted or
        // reply/forward, read/unread status might have changed in editor or viewer
        UpdateItemAtIndexL( HighlightedIndex() );
        SetMskL();
        }

    iCurrentClientRect = clientRect;
	iThisViewActive = ETrue;
	
	//emailindicator handling, is removed from 9.2
	//TRAP_IGNORE(TFsEmailStatusPaneIndicatorHandler::StatusPaneMailIndicatorHandlingL( activationData.iMailBoxId.Id()));
	    
    //Update mailbox widget index status in homescreen
    TFsEmailUiUtility::ToggleEmailIconL(EFalse, activationData.iMailBoxId );

	iShiftDepressed = EFalse; // clear state just in case

	// Inform MR observer if needed, special MR case, returning from attachment list
	iAppUi.MailViewer().CompletePendingMrCommand();

	// Make sure that correct component is set to focused.
	if ( iFocusedControl == EMailListComponent )
	    {
	    SetTreeListFocusedL();
	    }
	else
	    {
	    SetControlBarFocusedL();
	    iControlBarControl->MakeSelectorVisible( iAppUi.IsFocusShown() );
	    }
	FocusVisibilityChange( iAppUi.IsFocusShown() );
	iAppUi.ShowTitlePaneConnectionStatus();

    // if timer stoped when quitting view then restart if more messages available
    if ( iNewMailIds.Count() && ( ! iNewMailTimer->IsActive() ) && iSortState == ESortNone )
        {
        iNewMailTimer->Start( KNewMailTimerDelay );
        }

	TIMESTAMP( "Message list view opened" );
	}  // CFSEmailUiMailListVisualiser::ChildDoActivateL

// ---------------------------------------------------------------------------
// Sets status bar layout
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetStatusBarLayout()
	{
    TInt res = R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT;
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        // landscape must use different layout
		res = R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT;
        }

	if ( StatusPane()->CurrentLayoutResId() != res )
		{
		TRAP_IGNORE(
			StatusPane()->SwitchLayoutL( res ));
		}
	}


// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ChildDoDeactivate()
	{
    FUNC_LOG;
    if (iMailListModelUpdater)
        {
        iMailListModelUpdater->Cancel();
        }
	if ( !iAppUi.AppUiExitOngoing() )
  	    {
  	    TRAP_IGNORE( {
            iMailList->HideListL();
            iMailList->SetFocusedL( EFalse );
  	        } );
  	    iMailTreeListVisualizer->NotifyControlVisibilityChange( EFalse );
  	    TRAP_IGNORE( UpdateFolderAndMarkingModeTextsL() );
  	    }
	iThisViewActive = EFalse;
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
CFSEmailUiMailListModel* CFSEmailUiMailListVisualiser::Model()
	{
    FUNC_LOG;
	return iModel;
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TFSMailMsgId CFSEmailUiMailListVisualiser::FolderId()
	{
    FUNC_LOG;
	TFSMailMsgId folderId; // constructs null ID
	if ( iMailFolder )
	    {
	    folderId = iMailFolder->GetFolderId();
	    }
	return folderId;
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
    FUNC_LOG;
	CompletePendingRefresh();

	// Get the list of items which will be targetted by the actions menu commands
	RFsTreeItemIdList targetEntries;
	CleanupClosePushL( targetEntries );
	GetActionsTargetEntriesL( targetEntries );

	TBool supportsMoving = iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder );
	TBool supportsFlag = TFsEmailUiUtility::IsFollowUpSupported( *iAppUi.GetActiveMailbox() );

	TInt currentFolderType = KErrNotFound;
	if ( iMailFolder )
	    {
	    currentFolderType = iMailFolder->GetFolderType();
	    }

	if ( aResourceId == R_FSEMAILUI_MAILLIST_MARKING_MODE_MENUPANE )
	    {
        if (!CountMarkedItemsL())
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeDelete, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeUnread, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeRead, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeMove, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeFollowUp, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeMarkAll, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeUnmarkAll, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeDelete, EFalse );
            if ( AreAllMarkedItemsUnreadL() )
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeUnread, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeUnread, EFalse );
                }
            if ( AreAllMarkedItemsReadL() )
                {            
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeRead, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeRead, EFalse );
                }
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeMove, !supportsMoving );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeFollowUp, !supportsFlag );            
            if ( AreAllItemsMarked() )
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeMarkAll, ETrue );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeUnmarkAll, EFalse );
                }
            else if ( AreAllItemsUnmarked() )
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeMarkAll, EFalse );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeUnmarkAll, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeMarkAll, EFalse );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingModeUnmarkAll, EFalse );
                }                
            }
	    }
	
    // MAIN MENU ***************************************************************************
	if ( aResourceId == R_FSEMAILUI_MAILLIST_MENUPANE )
	    {
		// Saves a focus visibility.
		iLastFocus = EFalse;
		if( iFocusedControl == EMailListComponent && IsFocusShown() )
		    {
		    iLastFocus = ETrue;
		    }
	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
		   }

	    // Checks if a device has a keyboard or not.
		if( !iLastFocus )
    	    {
            // Hide all the normal email message specific actions
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReply, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMailActions, ETrue);
            }
        else
    	    {
    		CFSMailMessage* targetMessage = NULL;
            if ( targetEntries.Count() == 1 )
                {
                targetMessage = &MsgPtrFromListIdL( targetEntries[0] );
                }

            // Hide the irrelevant reply / reply all / forward commands
            TInt numRecipients(0);
            if ( targetMessage )
                {
                //Get # of recipients
                numRecipients = TFsEmailUiUtility::CountRecipientsSmart( iAppUi, targetMessage );
                }
            // All reply/forward options are hidden when multiple marked messages or folder is outbox or drafts
            if ( targetEntries.Count() != 1 ||
                 currentFolderType == EFSOutbox ||
                 currentFolderType == EFSDraftsFolder )
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReply, ETrue );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, ETrue );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward, ETrue );
                }

            // Reply all is hidden also when the single target message has multiple recipients
            else if ( !targetMessage || numRecipients <= 1 )
                {
                aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, ETrue );
                }
            }

	    // EMPTY LIST, MOST OPTIONS ARE HIDDEN
	    if ( !iModel->Count() || !iMailFolder )
	        {
	        aMenuPane->SetItemDimmed( EFsEmailUiCmdMailActions, ETrue );
	        aMenuPane->SetItemDimmed( EFsEmailUiCmdSearch, ETrue );
	        aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkingMode, ETrue );
	        }

	    // FOLDER SPECIFIC COMMAND HIDING
	    // "Clear deleted folder" command is available only in Deleted folder
	    if ( currentFolderType != EFSDeleted || !iModel->Count() )
	        {
	        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsEmptyDeleted, ETrue );
	        }
	    
	    CConnectionStatusQueryExtension::TConnectionStatus connetionStatus;
        iAppUi.GetConnectionStatusL( connetionStatus );
        if ( connetionStatus == CConnectionStatusQueryExtension::ESynchronizing )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdSync, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelSync, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdSync, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelSync, ETrue );
            }
	    }
	
    // MAIN MENU ***************************************************************************


	// ACTIONS SUBMENU *********************************************************************
	if ( aResourceId == R_FSEMAILUI_MAILLIST_SUBMENU_MAIL_ACTIONS  )
		{
        // COMMON PART OF ACTIONS SUBMENU
		aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsRead, !IsMarkAsReadAvailableL() );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, !IsMarkAsUnreadAvailableL() );

		if ( !supportsMoving || !iMailFolder ) // Hide move from actions if not supported
			{
			aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMove, ETrue );
			}
		else
			{
			// Moving supported, show/hide moving options depending on the current folder
			// Check for outbox case
			if ( currentFolderType == EFSOutbox )
				{
				// moving from outbox is not allowed otherwise
				aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMove, ETrue );
				}
			// Handle rest of the folders
			else
				{
				aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMove, EFalse );
				}
			}

		TInt markedCount = CountMarkedItemsL();

	    // Hide expand/collapse all when not applicable
		if ( iNodesInUse == EListControlSeparatorDisabled || !iModel->Count() )
			{
			aMenuPane->SetItemDimmed(EFsEmailUiCmdActionsCollapseAll, ETrue);
			aMenuPane->SetItemDimmed(EFsEmailUiCmdActionsExpandAll, ETrue);
			}
		else
			{
			aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCollapseAll, AllNodesCollapsed() );
			aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsExpandAll, AllNodesExpanded() );
			}

		// Hide followup flagging if not applicable
		if ( !supportsFlag || !targetEntries.Count() )
		    {
		    aMenuPane->SetItemDimmed(EFsEmailUiCmdActionsFlag, ETrue);
		    }

	    // Some commands are hidden in the outbox and drafts folders
        if ( currentFolderType == EFSOutbox ||
             currentFolderType == EFSDraftsFolder )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsFlag, ETrue );
            }
		}
    // ACTIONS SUBMENU *********************************************************************

	CleanupStack::PopAndDestroy( &targetEntries );

	// Add shortcut hints
	iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane,
	                             CFSEmailUiShortcutBinding::EContextMailList );
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::MarkAllItemsL()
	{
    FUNC_LOG;
	if ( iTreeItemArray.Count() )
		{
		for ( TInt i=0;i<iTreeItemArray.Count(); i++ )
			{
			if ( !iMailList->IsNode(iTreeItemArray[i].iListItemId ) )
				{
		       	iMailList->MarkItemL( iTreeItemArray[i].iListItemId, ETrue );
				}
			}
		}
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UnmarkAllItemsL()
	{
    FUNC_LOG;
	for ( TInt i=0 ; i<iTreeItemArray.Count() ; i++ )
	    {
	    if ( !iMailList->IsNode(iTreeItemArray[i].iListItemId) )
	        {
	        iMailList->MarkItemL( iTreeItemArray[i].iListItemId, EFalse );
	        }
	    }
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::CountMarkedItemsL()
    {
    FUNC_LOG;
    RFsTreeItemIdList markedEntries;
    CleanupClosePushL( markedEntries );
    iMailList->GetMarkedItemsL( markedEntries );
    TInt count = markedEntries.Count();
    CleanupStack::PopAndDestroy( &markedEntries );
    return count;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::AreAllItemsMarked()
    {
    FUNC_LOG;

    TBool ret(ETrue);
    
	if ( iTreeItemArray.Count() )
		{
		for ( TInt i=0;i<iTreeItemArray.Count(); i++ )
			{
			if ( !iMailList->IsNode(iTreeItemArray[i].iListItemId ) &&
			     !iMailList->IsMarked( iTreeItemArray[i].iListItemId ))
                {
				ret = EFalse;
				break;    
				}
			}
		}
    return ret;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::AreAllItemsUnmarked()
    {
    FUNC_LOG;

    TBool ret(ETrue);
    
	if ( iTreeItemArray.Count() )
		{
		for ( TInt i=0;i<iTreeItemArray.Count(); i++ )
			{
			if ( !iMailList->IsNode(iTreeItemArray[i].iListItemId ) &&
			     iMailList->IsMarked( iTreeItemArray[i].iListItemId ))
                {
				ret = EFalse;
				break;    
				}
			}
		}
    return ret;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::AreAllMarkedItemsReadL()
    {
    FUNC_LOG;

    TBool ret(ETrue);
    
	if ( iTreeItemArray.Count() )
		{
        for ( TInt i=0;i<iTreeItemArray.Count(); i++ )
            {
            if ( !iMailList->IsNode(iTreeItemArray[i].iListItemId ) &&
                iMailList->IsMarked( iTreeItemArray[i].iListItemId ))
                {
                CFSEmailUiMailListModelItem* item =
                        static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(i) );
                if ( item ) // Safety
                    {
                    CFSMailMessage* confirmedMsgPtr(0);
                    confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL(iAppUi.GetActiveMailboxId(),
                    iMailFolder->GetFolderId(), item->MessagePtr().GetMessageId(), EFSMsgDataEnvelope );
                    TBool isReadMessage = confirmedMsgPtr->IsFlagSet( EFSMsgFlag_Read );
                    delete confirmedMsgPtr;
                    if ( !isReadMessage )
                        {
                        ret = EFalse;
                        break;
                        }
                    }
                }
            }
		}
    return ret;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::AreAllMarkedItemsUnreadL()
    {
    FUNC_LOG;

    TBool ret(ETrue);
    
    if ( iTreeItemArray.Count() )
        {
        for ( TInt i=0;i<iTreeItemArray.Count(); i++ )
            {
            if ( !iMailList->IsNode(iTreeItemArray[i].iListItemId ) &&
                iMailList->IsMarked( iTreeItemArray[i].iListItemId ))
                {
                CFSEmailUiMailListModelItem* item =
                        static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(i) );
                if ( item ) // Safety
                    {
                    CFSMailMessage* confirmedMsgPtr(0);
                    confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL(iAppUi.GetActiveMailboxId(),
                    iMailFolder->GetFolderId(), item->MessagePtr().GetMessageId(), EFSMsgDataEnvelope );
                    TBool isReadMessage = confirmedMsgPtr->IsFlagSet( EFSMsgFlag_Read );
                    delete confirmedMsgPtr;
                    if ( isReadMessage )
                        {
                        ret = EFalse;
                        break;
                        }
                    }
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::GetMarkedMessagesL( RArray<TFSMailMsgId>& aMessageIDs ) const
    {
    FUNC_LOG;
    aMessageIDs.Reset();

	RFsTreeItemIdList markedEntries;
	CleanupClosePushL( markedEntries );
	iMailList->GetMarkedItemsL( markedEntries );

	for ( TInt i=0 ; i<markedEntries.Count() ; ++i )
	    {
	    // This could be optimized if we could be certain that the order of the
	    // marked entries is the same as the order in the message list. This is
	    // probably true, but the interface does not guarantee that. Now the
	    // time counsumption is Theta(n*m) while Theta(n+m) could be possible
	    TFSMailMsgId messageId = MsgIdFromListId( markedEntries[i] );
	    if ( !messageId.IsNullId() )
	        {
	        aMessageIDs.Append( messageId );
	        }
	    }

	CleanupStack::PopAndDestroy( &markedEntries );
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::MarkMessagesIfFoundL( const RArray<TFSMailMsgId>& aMessageIDs )
    {
    FUNC_LOG;
    for ( TInt i=0 ; i<aMessageIDs.Count() ; ++i )
        {
        // Same performance tweaking possibility is here that is commented in GetMarkedMessagesL().
        TInt index = ItemIndexFromMessageId( aMessageIDs[i] );
        if ( index >= 0 )
            {
            iMailList->MarkItemL( iTreeItemArray[index].iListItemId, ETrue );
            }
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ExitMarkingModeL()
    {
    FUNC_LOG;
    iMarkingMode = EFalse;    
    // Hide marking mode text on the place of drop down menus
    UpdateFolderAndMarkingModeTextsL();
    UnmarkAllItemsL();
    // Change softkeys back to normal
    SetViewSoftkeysL( R_FREESTYLE_EMAUIL_UI_SK_OPTIONS_BACK );
    // Change options menu back to normal
    CEikMenuBar* menu = iAppUi.CurrentActiveView()->MenuBar();
    menu->StopDisplayingMenuBar();
    menu->SetMenuTitleResourceId(R_FSEMAILUI_MAILLIST_MENUBAR);
    // Change background back to normal
    DisplayMarkingModeBgL( EFalse );   
    // Display drop down menu buttons
    iControlBarControl->SetRectL( iAppUi.LayoutHandler()->GetControlBarRect() );
    iNewEmailButton->SetDimmed( EFalse );
    iFolderListButton->SetDimmed( EFalse );   
    iSortButton->SetDimmed( EFalse );
    ScaleControlBarL();

    iMailTreeListVisualizer->HideList();   
    SetMailListLayoutAnchors();
    iMailTreeListVisualizer->ShowListL();
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::EnterMarkingModeL()
    {
    FUNC_LOG;	
    iMarkingMode = ETrue;
	iListMarkItemsState = ETrue; // shift-scrolling does marking after one item is marked
    HandleCommandL( EFsEmailUiCmdActionsExpandAll );
    // Change softkeys for marking mode
    SetViewSoftkeysL( R_FREESTYLE_EMAUIL_UI_SK_OPTIONS_CANCEL );
    // Change options menu for marking mode
    CEikMenuBar* menu = iAppUi.CurrentActiveView()->MenuBar();
    menu->StopDisplayingMenuBar();
    menu->SetMenuTitleResourceId(R_FSEMAILUI_MAILLIST_MENUBAR_MARKING_MODE);
    // Hide drop down menu buttons
    iNewEmailButton->SetDimmed();
    iFolderListButton->SetDimmed();   
    iSortButton->SetDimmed();
    TRect rect(0,0,0,0);
    iControlBarControl->SetRectL( rect );
    UpdateFolderAndMarkingModeTextsL();
    
    iMailTreeListVisualizer->HideList();   
    SetMailListLayoutAnchors();
    iMailTreeListVisualizer->ShowListL();
    // Change background to marking mode
    DisplayMarkingModeBgL( ETrue );    
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::RefreshMarkingModeL()
    {
    FUNC_LOG;
    if ( iMarkingMode )
        {
        // Hide drop down menu buttons    
        TRect rect(0,0,0,0);
        iControlBarControl->SetRectL( rect );
        iNewEmailButton->SetDimmed();
        iFolderListButton->SetDimmed();
        iSortButton->SetDimmed();
        UpdateFolderAndMarkingModeTextsL();
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DisplayMarkingModeTextOnButtonsL()
    {
    FUNC_LOG;     
    if (!iMarkingModeTextVisual)
        {
        const TRect sortButtonRect( iAppUi.LayoutHandler()->GetControlBarSortButtonRect() );
        const TRect newMailButtonRect( iAppUi.LayoutHandler()->GetControlBarNewEmailButtonRect() );

        iMarkingModeTextRect = TRect( newMailButtonRect.iTl, sortButtonRect.iBr );
        iMarkingModeTextParentLayout = CAlfDeckLayout::AddNewL( *iMailListControl );
        iMarkingModeTextParentLayout->SetFlags( EAlfVisualFlagManualLayout );
        iMarkingModeTextParentLayout->SetRect( iMarkingModeTextRect );

        iMarkingModeTextContentLayout = CAlfAnchorLayout::AddNewL( *iMailListControl, iMarkingModeTextParentLayout );

        iMarkingModeTextVisual = CAlfTextVisual::AddNewL( *iMailListControl, iMarkingModeTextContentLayout );
        iMarkingModeTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            iMarkingModeTextVisual->SetAlign( EAlfAlignHRight, EAlfAlignVCenter );
            }
        else
            {
            iMarkingModeTextVisual->SetAlign( EAlfAlignHLeft, EAlfAlignVCenter );
            }    
        iMarkingModeTextVisual->SetStyle( EAlfTextStyleTitle );
        TRgb color = iAppUi.LayoutHandler()->ListNodeTextColor();
        iMarkingModeTextVisual->SetColor ( color );
        if( !Layout_Meta_Data::IsLandscapeOrientation() )
            {
            HBufC* msg = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MARKINGMODE );
            iMarkingModeTextVisual->SetTextL( *msg );
            CleanupStack::PopAndDestroy( msg );
            }
        else
            {
            iMarkingModeTextVisual->SetTextL( KNullDesC );
            }
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::RemoveMarkingModeTextOnButtonsL()
    {
    FUNC_LOG;
    if (iMarkingModeTextVisual)
        {
        iMarkingModeTextVisual->SetTextL( KNullDesC );
        iMarkingModeTextVisual->RemoveAndDestroyAllD();
        iMarkingModeTextVisual = NULL;
        iMarkingModeTextContentLayout->RemoveAndDestroyAllD();
        iMarkingModeTextContentLayout = NULL;
        iMarkingModeTextParentLayout->RemoveAndDestroyAllD();
        iMarkingModeTextParentLayout = NULL;
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DisplayMarkingModeBgL( TBool aDisplay )
    {
    FUNC_LOG;
    if (aDisplay)
        {
        CAlfBrush* brush = iAppUi.FsTextureManager()->NewMailListMarkingModeBgBrushLC();
        iMailTreeListVisualizer->SetBackgroundBrushL( brush );
        CleanupStack::Pop( brush );
        }
    else
        {
        iMailTreeListVisualizer->ClearBackground();
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::RefreshL( TFSMailMsgId* aFocusToMessage )
	{
    FUNC_LOG;
	iMailList->RemoveAllL();
	iTreeItemArray.Reset();

    // when we get refresh mail list updates should be fully enabled
    iMailOpened = EFalse;

	RefreshListItemsL();

	if ( !iModel->Count() )
		{
		iFocusedControl = EControlBarComponent;
		// If focus is not visible, hide selector
		iControlBarControl->MakeSelectorVisible( IsFocusShown() );
		}
	else
	    {
        // Try to keep the previous list item if focused message ptr is given,
	    // and it's not NULL id
        if ( aFocusToMessage && !aFocusToMessage->IsNullId() )
            {
            TInt idx = ItemIndexFromMessageId( *aFocusToMessage );
            if ( idx >= 0 )
                {
                TFsTreeItemId itemId = iTreeItemArray[idx].iListItemId;
                iMailList->SetFocusedItemL( itemId );
                // If contorl bar is focused, we need to hide
                // focus then.
                if( iFocusedControl == EControlBarComponent )
                    {
                    iMailTreeListVisualizer->UpdateItemL( itemId );
                    }
                }
            }
	    }

    SetListAndCtrlBarFocusL();
    SetMskL();

    iListOrderMayBeOutOfDate = EFalse;
	}

void CFSEmailUiMailListVisualiser::RefreshOrderL()
    {
    FUNC_LOG;
    if ( iTreeItemArray.Count() )
        {
        TInt itemIdxUnderNode = -1;
        TInt nodeIdx = -1;
        TFsTreeItemId parentNodeId = KFsTreeRootID;

        //iMailList->HideListL( EFalse, EFalse );
        TFsTreeItemId prevFocus = iMailList->FocusedItem();
        iMailList->SetFocusedItemL( iTreeItemArray[0].iListItemId ); // Try to prevent over indexing in generic list

        // iTreeItemArray is recreated on reordering.
        iTreeItemArray.Reset();
        SMailListItem mailListItem;

        for ( TInt i = 0 ; i < iModel->Count() ; ++i )
            {
            CFSEmailUiMailListModelItem* item = static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(i) );
            TFsTreeItemId itemId = item->CorrespondingListId();

            if ( item->ModelItemType() == ETypeSeparator )
                {
                nodeIdx++;
                itemIdxUnderNode = -1;
                parentNodeId = itemId;
                if ( iMailList->ChildIndex( itemId, KFsTreeRootID ) != nodeIdx )
                    {
                    iMailList->MoveItemL( itemId, KFsTreeRootID, nodeIdx );
                    }
                }
            else
                {
                itemIdxUnderNode++;
                if ( iMailList->Parent(itemId) != parentNodeId ||
                     iMailList->ChildIndex( parentNodeId, itemId ) != itemIdxUnderNode )
                    {
                    iMailList->MoveItemL( itemId, parentNodeId, itemIdxUnderNode );
                    }
                }

            mailListItem.iListItemId = itemId;
            mailListItem.iTreeItemData = &iMailList->ItemData(itemId);
            mailListItem.iTreeItemVisualiser = &iMailList->ItemVisualizer(itemId);
            iTreeItemArray.AppendL( mailListItem );
            }

        iMailList->SetFocusedItemL( prevFocus );
        //iMailList->ShowListL( EFalse, EFalse );
        }
    iListOrderMayBeOutOfDate = EFalse;
    }

// ---------------------------------------------------------------------------
// Start refresh list items asynchronously
//
// ---------------------------------------------------------------------------
//
//
void CFSEmailUiMailListVisualiser::RefreshDeferred( TFSMailMsgId* aFocusToMessage /*= NULL*/ )
    {
    FUNC_LOG;
    if ( aFocusToMessage )
        {
        iMsgToFocusAfterRedraw = *aFocusToMessage;
        }
    else
        {
        // SetNullId sets just the null id flag,
        // so we need to null the actual id manually
        iMsgToFocusAfterRedraw.SetId( 0 );
        iMsgToFocusAfterRedraw.SetNullId();
        }

    if ( iAsyncRedrawer )
        {
        TCallBack asyncRefresh( DoRefresh, this );
        iAsyncRedrawer->Cancel();
        iAsyncRedrawer->Set( asyncRefresh );
        iAsyncRedrawer->CallBack();
        }
    }

// ---------------------------------------------------------------------------
// Static wrapper function for RefreshL() to enable asynchronous calling
//
// ---------------------------------------------------------------------------
//
//
TInt CFSEmailUiMailListVisualiser::DoRefresh( TAny* aSelfPtr )
    {
    FUNC_LOG;
    CFSEmailUiMailListVisualiser* self =
        static_cast< CFSEmailUiMailListVisualiser* >( aSelfPtr );
    TRAPD( err, self->RefreshL( &self->iMsgToFocusAfterRedraw ) );
    return err;
    }

// ---------------------------------------------------------------------------
// Force any pending refresh event to be completed immediately
//
// ---------------------------------------------------------------------------
//
//
void CFSEmailUiMailListVisualiser::CompletePendingRefresh()
    {
    FUNC_LOG;

    if ( iAsyncRedrawer && iAsyncRedrawer->IsActive() )
        {
        iAsyncRedrawer->Cancel();
        DoRefresh( this );
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::RefreshListItemsL()
	{
    FUNC_LOG;
	// IMPLEMENTATION OF FILLING UP THE LIST
	TFsTreeItemId latestNodeId = KFsTreeRootID; // items will go under root node if no other nodes found in the model
    CFSEmailUiMailListModelItem* item( NULL );
    SetMailListItemsExtendedL();

    TInt count(0);
    count = iModel->Count();
    for ( TInt i=0; i < count; i++ )
		{
		item = static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));

        const TBool allowRefresh(i == 0 || (i == count - 1));

		// Append separator item text into the list
		if ( item && item->ModelItemType() == ETypeSeparator )
			{
			latestNodeId = InsertNodeItemL( i, KErrNotFound, allowRefresh );
			}
		// Append mail item into the list
		else if ( item && item->ModelItemType() == ETypeMailItem )
			{
			InsertListItemL( i, latestNodeId, KErrNotFound, allowRefresh );
			}
		}
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::RefreshListItemsL(TFsTreeItemId& aLatestNodeId, const TInt aStartIndex, const TInt aEndIndex)
    {
    FUNC_LOG;
    // IMPLEMENTATION OF FILLING UP THE LIST
    iMailList->BeginUpdate();
    CFSEmailUiMailListModelItem* item( NULL );
    if (aLatestNodeId == KFsTreeRootID && !aStartIndex)
        {
        iMailList->RemoveAllL();
        iTreeItemArray.Reset();
        SetMailListItemsExtendedL();
        }
    for ( TInt i = aStartIndex; i < aEndIndex; i++ )
        {
        item = static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
        // Append separator item text into the list
        if ( item && item->ModelItemType() == ETypeSeparator )
            {
            aLatestNodeId = InsertNodeItemL( i, KErrNotFound, EFalse );
            }
        // Append mail item into the list
        else if ( item && item->ModelItemType() == ETypeMailItem )
            {
            InsertListItemL( i, aLatestNodeId, KErrNotFound, EFalse );
            }
        }
    iMailList->EndUpdateL();
    }

void CFSEmailUiMailListVisualiser::SetMailListItemsExtendedL()
	{
    FUNC_LOG;
	// Set items always extended in double line preview on mode.
	if ( iListMode == EListControlTypeDoubleLinePreviewOn ||
		 iListMode == EListControlTypeDoubleLinePreviewOff )
		{
		if ( iMailTreeListVisualizer )
			{
	        iMailTreeListVisualizer->SetItemsAlwaysExtendedL( ETrue );
			}
 		}
	else
		{
		// Set the extendedability and extended size
		if ( iMailTreeListVisualizer )
			{
  			iMailTreeListVisualizer->SetItemsAlwaysExtendedL( EFalse );
			}
 		}
	}


// ---------------------------------------------------------------------------
// Create and insert one list node item according to given model item.
// Omitting the argument aChildIdx causes the new node to be appended as last
// child of the root node.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiMailListVisualiser::InsertNodeItemL( TInt aModelIndex,
                                                             TInt aChildIndex,
                                                             const TBool aAllowRefresh )
    {
    FUNC_LOG;
    TFsTreeItemId nodeId;
    CFsTreePlainOneLineNodeData* plainNodeData(0);
    CFsTreePlainOneLineNodeVisualizer* plainNodeVisualizer(0);

    CFSEmailUiMailListModelItem* item =
        static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(aModelIndex) );

    CreatePlainNodeL( item->SeparatorText(), plainNodeData, plainNodeVisualizer );
    CleanupStack::PushL( plainNodeData );
    CleanupStack::PushL( plainNodeVisualizer );
    // Set text aling for western or A&H layout
   	plainNodeVisualizer->SetTextAlign( EAlfAlignHLocale  );
   	//new node is expanded
   	plainNodeVisualizer->SetExpanded( ETrue );

    // In mail list, scrollbar does not need to be updated after node, since node is always followed by item
    if ( aChildIndex >= 0 )
        {
        nodeId = iMailList->InsertNodeL( *plainNodeData, *plainNodeVisualizer, KFsTreeRootID, aChildIndex, aAllowRefresh );
        }
    else
        {
        nodeId = iMailList->InsertNodeL( *plainNodeData, *plainNodeVisualizer, KFsTreeRootID, KErrNotFound, aAllowRefresh );
        }

    CleanupStack::Pop( 2, plainNodeData );

    SMailListItem mailListItem;
    mailListItem.iListItemId = nodeId;
    mailListItem.iTreeItemData = plainNodeData;
    mailListItem.iTreeItemVisualiser = plainNodeVisualizer;
    iTreeItemArray.InsertL( mailListItem, aModelIndex );
    item->AddCorrespondingListId( nodeId );

    return nodeId;
    }

// ---------------------------------------------------------------------------
// Create and insert one list item according the given model item. The item is
// added under the given node. Omitting the argument aChildIdx causes the new
// item to be appended as last child of the node.
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiMailListVisualiser::InsertListItemL( TInt aModelIndex,
                                                             TFsTreeItemId aParentNodeId,
                                                             TInt aChildIdx, /*= KErrNotFound*/
                                                             TBool aAllowRefresh /*= ETrue*/ )
    {
    FUNC_LOG;
    CFsTreePlainTwoLineItemData* itemData = NULL;
    CFsTreePlainTwoLineItemVisualizer* itemVisualizer = NULL;
    CFSEmailUiMailListModelItem* item =
        static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(aModelIndex) );

    // Create data
    itemData = CFsTreePlainTwoLineItemData::NewL();
    CleanupStack::PushL(itemData);

    // Read the data from the message
    UpdateItemDataL( itemData, &item->MessagePtr() );

    // Set placeholder for body preview
    itemData->SetPreviewPaneDataL( KMissingPreviewDataMarker );

    // Create item visualiser
    itemVisualizer = CFsTreePlainTwoLineItemVisualizer::NewL(
            *iMailList->TreeControl() );
    CleanupStack::PushL( itemVisualizer );

   	itemVisualizer->SetTextAlign( EAlfAlignHLocale );

   	TBool previewOn = ( iListMode == EListControlTypeDoubleLinePreviewOn ||
                        iListMode == EListControlTypeSingleLinePreviewOn );
   	itemVisualizer->SetPreviewPaneOn( previewOn );
   	itemVisualizer->SetExtendable( ETrue );

   	// Set correct skin text colors for the list items
   	itemVisualizer->SetFocusedStateTextColor( iFocusedTextColor );
    itemVisualizer->SetNormalStateTextColor( iNormalTextColor );

    // Set font height
    itemVisualizer->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );

    //Update icons and text bolding
    //DOES NOT update the item - will be drawn automatically (if needed) during InsertItemL
    UpdateMsgIconAndBoldingL( itemData, itemVisualizer, &item->MessagePtr() );

    // Insert or append under the given node
    TFsTreeItemId itemId(0);
    itemId = iMailList->InsertItemL( *itemData, *itemVisualizer, aParentNodeId, aChildIdx, aAllowRefresh );

    // The visualizer and data are now owned by the iMailList and should be removed from
    // the cleanup stack
    CleanupStack::Pop( itemVisualizer );
    CleanupStack::Pop( itemData );

    // Insert corresponding item to iTreeItemArray
    SMailListItem mailListItem;
    mailListItem.iListItemId = itemId;
    mailListItem.iTreeItemData = itemData;
    mailListItem.iTreeItemVisualiser = itemVisualizer;
    iTreeItemArray.InsertL( mailListItem, aModelIndex );
    item->AddCorrespondingListId( itemId );

    // Updating the preview pane must be handled separately in this case;
    // generic logic in TreeListEvent handler doesn't work while we are
    // still in the middle of adding the item.
    if ( iMailList->FocusedItem() == itemId )
        {
        UpdatePreviewPaneTextIfNecessaryL( itemId, ETrue );
        }

    return itemId;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
HBufC* CFSEmailUiMailListVisualiser::GetListFirstTextLineL( const CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	HBufC* ret( NULL );

	TDesC* displayName( NULL );
	TDesC* emailAddress( NULL );

	TInt folderType( EFSInbox );
    if( iMailFolder )
     {
     folderType = iMailFolder->GetFolderType();
     }

	// Use sender in all other folders than outbox, sent, and drafts
	if ( folderType != EFSOutbox &&
		 folderType != EFSSentFolder &&
		 folderType != EFSDraftsFolder )
		{
		CFSMailAddress* fromAddress = aMsgPtr->GetSender();
	    if ( fromAddress )
		    {
		    displayName = &fromAddress->GetDisplayName();
		    emailAddress = &fromAddress->GetEmailAddress();

		    if ( displayName && displayName->Length() != 0 )
                {
                ret = displayName->AllocL();
                }
            else if ( emailAddress && emailAddress->Length() != 0 )
                {
                ret = emailAddress->AllocL();
                }
		    }
		else // no fromAddress in that case we show "(No sender info available)",
		    // in case sender sends MR to himself/herself
		    {
		    ret = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_SENDER_INFO_AVAILABLE);
		    }
	    }
	// Use first recipient in case of outgoing folders
	else // folderType == EFSOutbox || folderType == EFSSentFolder || folderType == EFSDraftsFolder
		{
		CFSMailMessage* msgPtr = const_cast<CFSMailMessage*>( aMsgPtr ); // there's no const function in FW to get recipient data
		RPointerArray<CFSMailAddress>& toArray = msgPtr->GetToRecipients();
		if ( toArray.Count() )
			{
			CFSMailAddress* toAddress = toArray[0]; // Use first
			if ( toAddress )
				{
				displayName = &toAddress->GetDisplayName();
				emailAddress = &toAddress->GetEmailAddress();

	            if ( displayName && displayName->Length() )
	                {
	                ret = displayName->AllocL();
	                }
	            else if ( emailAddress && emailAddress->Length() )
	                {
	                ret = emailAddress->AllocL();
	                }
				}
			}
		}

	// Allocate empty string if everything else fails
    if ( !ret )
         {
         ret = KNullDesC().AllocL();
         }

	// Drop out unwanted characters from display name such as <> and ""
	TFsEmailUiUtility::StripDisplayName( *ret );

	return ret;
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateItemDataL( CFsTreePlainTwoLineItemData* aData, const CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;
    // Set first line of data
    HBufC* firstLineText = GetListFirstTextLineL( aMsgPtr );
    if ( firstLineText )
        {
        CleanupStack::PushL( firstLineText );
        aData->SetDataL( *firstLineText );
        CleanupStack::PopAndDestroy( firstLineText );
        }
    else
        {
        aData->SetDataL( KNullDesC );
        }

    // Set second line of data
    HBufC* subjectText = TFsEmailUiUtility::CreateSubjectTextLC( aMsgPtr );
    aData->SetSecondaryDataL( *subjectText );
    CleanupStack::PopAndDestroy( subjectText );

    // Set time item in mail
    HBufC* timeText = NULL;
    if ( iCurrentSortCriteria.iField == EFSMailSortByDate )
        {
        timeText = TFsEmailUiUtility::ListMsgTimeTextFromMsgLC( aMsgPtr, iNodesInUse );
        }
    else
        {
        // Use dates also in other sort criterias that sort by date.
        timeText = TFsEmailUiUtility::ListMsgTimeTextFromMsgLC( aMsgPtr, EFalse );
        }
    aData->SetDateTimeDataL( *timeText );
    CleanupStack::PopAndDestroy( timeText );
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdatePreviewPaneTextForItemL( TInt aListItemId, CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	if ( aMsgPtr && ( iListMode == EListControlTypeSingleLinePreviewOn ||
         iListMode == EListControlTypeDoubleLinePreviewOn ) )
        {
        CFsTreePlainTwoLineItemData* twoLineItemData = static_cast<CFsTreePlainTwoLineItemData*>( ItemDataFromItemId( aListItemId ) );
        UpdatePreviewPaneTextForItemL( twoLineItemData, aMsgPtr  );
        }
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdatePreviewPaneTextForItemL(
        CFsTreePlainTwoLineItemData* aTwoLineItemData, CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;
	// Preview pane data update
	if ( aTwoLineItemData && aMsgPtr && ( iListMode == EListControlTypeSingleLinePreviewOn ||
		 iListMode == EListControlTypeDoubleLinePreviewOn ) )
		{
		TBool previewSet = EFalse;
		CFSMailMessagePart* textPart = aMsgPtr->PlainTextBodyPartL();
		CleanupStack::PushL( textPart );
		if ( textPart && ( textPart->FetchLoadState() == EFSPartial ||
			 textPart->FetchLoadState() == EFSFull ) )
			{
			TInt previewSize = Min( KMaxPreviewPaneLength, textPart->FetchedContentSize() );
			HBufC* plainTextData16 = HBufC::NewLC( previewSize );
			TPtr textPtr = plainTextData16->Des();

			// Getting the content needs to be trapped as it seems to leave in some error cases
			// at least with IMAP
			TRAP_IGNORE( textPart->GetContentToBufferL( textPtr, 0 ) ); // Zero is start offset
            // Crop out line feed, paragraph break, and tabulator
            TFsEmailUiUtility::FilterListItemTextL( textPtr );

            if ( textPtr.Length() )
                {
                aTwoLineItemData->SetPreviewPaneDataL( *plainTextData16 );
                previewSet = ETrue;
                }

			CleanupStack::PopAndDestroy( plainTextData16 );
			}

		// Display message size in preview pane if we got no body content
		if ( !previewSet )
			{
			TUint contentSize = aMsgPtr->ContentSize();
			HBufC* sizeDesc = TFsEmailUiUtility::CreateSizeDescLC( contentSize );
			HBufC* msgSizeText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_PREV_PANE_MSG_SIZE, *sizeDesc );
			aTwoLineItemData->SetPreviewPaneDataL( *msgSizeText );
			CleanupStack::PopAndDestroy( msgSizeText );
			CleanupStack::PopAndDestroy( sizeDesc );
			}
		CleanupStack::PopAndDestroy( textPart );
		}
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdatePreviewPaneTextIfNecessaryL( TFsTreeItemId aListItemId, TBool aUpdateItem )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && aListItemId != KFsTreeNoneID )
        {
        if ( iListMode == EListControlTypeSingleLinePreviewOn ||
             iListMode == EListControlTypeDoubleLinePreviewOn )
            {
            if ( !iMailList->IsNode( aListItemId ) )
                {
                TInt idx = ModelIndexFromListId( aListItemId );
                if ( idx != KErrNotFound )
                    {
                    CFsTreePlainTwoLineItemData* data =
                        static_cast<CFsTreePlainTwoLineItemData*>( iTreeItemArray[idx].iTreeItemData );
                    if ( data->PreviewPaneData() == KMissingPreviewDataMarker )
                        {
                        CFSEmailUiMailListModelItem* modelItem =
                            static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(idx) );
                        if ( modelItem ) // For safety
                            {
                            CFSMailMessage& msgRef = modelItem->MessagePtr();
                            UpdatePreviewPaneTextForItemL( aListItemId, &msgRef );
                            if ( aUpdateItem )
                                {
                                iMailTreeListVisualizer->UpdateItemL( aListItemId );
                                }
                            }
                        }
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::CreatePlainNodeL( const TDesC* aItemDataBuff,
                                                     CFsTreePlainOneLineNodeData* &aItemData,
                                                     CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer ) const
    {
    FUNC_LOG;
    aItemData = CFsTreePlainOneLineNodeData::NewL();
    CleanupStack::PushL( aItemData );

    aItemData->SetDataL( *aItemDataBuff );
    aItemData->SetIconExpanded( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeExpanded ));
    aItemData->SetIconCollapsed( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeCollapsed ));
    aNodeVisualizer = CFsTreePlainOneLineNodeVisualizer::NewL(*iMailList->TreeControl());
    CleanupStack::PushL( aNodeVisualizer );
    TInt nodeHeight = iAppUi.LayoutHandler()->OneLineListNodeHeight();
    // use cached client rect instead of iAppUi.ClientRect() to save time
    // ASSERT in debug to be sure that rects are these same
    __ASSERT_DEBUG( iAppUi.ClientRect() == iCurrentClientRect, User::Invariant() );
    aNodeVisualizer->SetSize( TSize(iCurrentClientRect.Width(), nodeHeight) );
    aNodeVisualizer->SetExtendable(EFalse);
    // Set correct skin text colors for the list items
    aNodeVisualizer->SetFocusedStateTextColor( iFocusedTextColor );
    aNodeVisualizer->SetNormalStateTextColor( iNodeTextColor );
    // Set font height
    aNodeVisualizer->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );
    // Set font always bolded in nodes
    aNodeVisualizer->SetTextBold( ETrue );

    //<cmail>
    CAlfBrush* titleDividerBgBrush =
        iAppUi.FsTextureManager()->TitleDividerBgBrushL();
    // ownership is not transfered
    aNodeVisualizer->SetBackgroundBrush( titleDividerBgBrush );
    //</cmail>

    CleanupStack::Pop( aNodeVisualizer );
    CleanupStack::Pop( aItemData );
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleDynamicVariantSwitchL(
    CFsEmailUiViewBase::TDynamicSwitchType aType )
	{
    FUNC_LOG;
    CFsEmailUiViewBase::HandleDynamicVariantSwitchL( aType );

    if ( iFirstStartCompleted ) // Safety
        {
        if ( aType == EScreenLayoutChanged )
        	{
            SetStatusBarLayout();
        	}

        if ( aType == ESkinChanged )
            {
            UpdateThemeL();
            }
        else
            {
            iAppUi.FsTextureManager()->ClearTextureByIndex( EListControlBarMailboxDefaultIcon );
            iFolderListButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( EListControlBarMailboxDefaultIcon ) );
            iAppUi.FsTextureManager()->ClearTextureByIndex( EListTextureCreateNewMessageIcon );
            iNewEmailButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( EListTextureCreateNewMessageIcon ) );
            iAppUi.FsTextureManager()->ClearTextureByIndex( GetSortButtonTextureIndex() );
            iSortButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( GetSortButtonTextureIndex() ) );
           
            iMailTreeListVisualizer->HideList();
            // screen layout changed
            iCurrentClientRect = iAppUi.ClientRect();
            iScreenAnchorLayout->SetSize( iCurrentClientRect.Size() );
            SetMailListLayoutAnchors();
            // Set branded watermark
            SetBrandedListWatermarkL();
            iMailTreeListVisualizer->ShowListL();

            ScaleControlBarL();
            SetBrandedMailBoxIconL();
            }
        // Update the folder/sort popup layout in case it happened to be open
        if ( iAppUi.FolderList().IsPopupShown() )
            {
            iAppUi.FolderList().HandleDynamicVariantSwitchL( aType );
            }
        }
    if (iMarkingMode)
        {
        RefreshMarkingModeL();
        }
    UpdateFolderAndMarkingModeTextsL();
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleDynamicVariantSwitchOnBackgroundL(
        CFsEmailUiViewBase::TDynamicSwitchType aType )
    {
    FUNC_LOG;
    CFsEmailUiViewBase::HandleDynamicVariantSwitchOnBackgroundL( aType );
    if ( aType == ESkinChanged )
        {
        UpdateThemeL();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::HitTest(
        const CAlfControl& aControl, const TPoint& aPoint ) const
    {
    FUNC_LOG;
    TBool contains( EFalse );
    const TInt visualCount( aControl.VisualCount() );
    for( TInt index( 0 ); index < visualCount && !contains; ++index )
        {
        TRect rect( aControl.Visual( index ).DisplayRectTarget() );
        if( rect.Contains( aPoint ) )
            {
            contains = ETrue;
            }
        }
    return contains;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::HandleArrowEventInPortraitL(
        const TInt aScancode,
        const TAlfEvent& aEvent,
        const TBool aShiftState )
    {
    FUNC_LOG;
    TBool result( EFalse );

    if( aScancode == EStdKeyLeftArrow )
        {
        if( ( iControlBarControl ) &&
            ( iFocusedControl == EMailListComponent ) )
            {
            // Move focus to control bar (focused button is
            // sort button).
            HandleCommandL( EFsEmailUiCmdGoToSwitchSorting );
            result = ETrue;
            }
        else if( ( iControlBarControl ) &&
                 ( iFocusedControl == EControlBarComponent  ) )
            {
            TInt focusedButtonId( iControlBarControl->GetFocusedButton()->Id() );
            if ( focusedButtonId == iNewEmailButtonId )
                {
                // Change focus to mail list if there are any
                // emails in it.
                if( iModel->Count() )
                    {
                    iFocusedControl = EMailListComponent;
                    result = EFalse;
                    }
                // Leave focus to control bar because there are no
                // mails in mail list.
                else
                    {
                    iControlBarControl->MakeSelectorVisible( IsFocusShown() );
                    iFocusedControl = EControlBarComponent;
                    result = ETrue; // Do not set focus to empty list
                    }
                }
            else
                {
                iControlBarControl->MakeSelectorVisible( IsFocusShown() );
                iFocusedControl = EControlBarComponent;
                result = EFalse;
                }
            }
        else
            {
            iFocusedControl = EMailListComponent;
            result = EFalse;
            }
        SetMskL();
        }
    else if( aScancode == EStdKeyRightArrow )
        {
        // Show toolbar if there is data on the list
        const TInt modelCount( iModel->Count() );
        TBool focusShown( IsFocusShown() );

        // If control bar is focused.
        if( ( iControlBarControl ) &&
            ( iFocusedControl == EControlBarComponent ) )
            {
            TInt focusedButtonId( iControlBarControl->GetFocusedButton()->Id() );

            // If sort button is focused and there are mails in mail
            // list then focus is moved to mail list, otherwise
            // leave focus to control bar
            if( focusedButtonId == iSortButtonId )
                {
                if ( modelCount > 0 ) // Check for empty folder
                    {
                    iFocusedControl = EMailListComponent;
                    result = EFalse;
                    }
                else
                    {
                    iFocusedControl = EControlBarComponent;
                    iControlBarControl->MakeSelectorVisible( focusShown );
                    result = ETrue; // Do not set focus to empty list
                    }
                }
            else
                {
                iFocusedControl = EControlBarComponent;
                iControlBarControl->MakeSelectorVisible( focusShown );
                result = EFalse;
                }
            }
        else if( ( iControlBarControl ) &&
                 ( iFocusedControl == EMailListComponent ) )
            {
            HandleCommandL( EFsEmailUiCmdGoToSwitchNewEmail );
            result = ETrue;
            }
        else
            {
            iFocusedControl = EMailListComponent;
            result = EFalse;
            }
        SetMskL();
        }
    else if( aScancode ==  EStdKeyDownArrow )
        {
        const TInt itemCount( iModel->Count() );
        if ( iFocusedControl == EMailListComponent )
            {
            result = iMailList->TreeControl()->OfferEventL( aEvent );
            if ( aShiftState )
                {
                DoScrollMarkUnmarkL(); // marking is done after moving the cursor
                }
            }
        else if( ( iFocusedControl == EControlBarComponent ) &&
                 ( itemCount == 0 ) )
            {
            result = ETrue; // Do not set focus to empty list
            }
        else if( ( iFocusedControl == EControlBarComponent ) &&
                 ( itemCount > 0 ) )
            {
            iFocusedControl = EMailListComponent;

            result = iMailList->TreeControl()->OfferEventL(aEvent);
            if ( aShiftState )
                {
                DoScrollMarkUnmarkL(); // marking is done after moving the cursor
                }
            }
        else
            {
            result = EFalse;
            }
        SetMskL();
        }
    else if( aScancode == EStdKeyUpArrow )
        {
        iControlBarControl->MakeSelectorVisible( IsFocusShown() );
        if ( iFocusedControl == EMailListComponent )
            {
            if ( HighlightedIndex() == 0 )
                {
                HandleCommandL( EFsEmailUiCmdGoToSwitchFolder );
                result = ETrue;
                }
            else
                {
                result = iMailList->TreeControl()->OfferEventL( aEvent );
                if ( aShiftState )
                    {
                    // marking is done after moving the cursor
                    DoScrollMarkUnmarkL();
                    }
                SetMskL();
                }
            }
        else if( iFocusedControl == EControlBarComponent )
            {
            result = ETrue;
            }
        else
            {
            result = EFalse;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::HandleArrowEventInLandscapeL(
        const TInt aScancode,
        const TAlfEvent& aEvent,
        const TBool aShiftState )
    {
    FUNC_LOG;
    TBool result( EFalse );
    if( aScancode == EStdKeyRightArrow )
        {
        if ( iFocusedControl == EMailListComponent )
            {
            HandleCommandL( EFsEmailUiCmdGoToSwitchFolder );
            result = ETrue;
            }
        else if( iFocusedControl == EControlBarComponent )
            {
            iControlBarControl->MakeSelectorVisible( IsFocusShown() );
            result = ETrue;
            }
        }
    else if( aScancode == EStdKeyLeftArrow )
        {
        if ( iFocusedControl == EControlBarComponent )
            {
            const TInt modelCount( iModel->Count() );
            if( modelCount > 0 )
                {
                iFocusedControl = EMailListComponent;
                result = iMailList->TreeControl()->OfferEventL( aEvent );
                if( aShiftState )
                    {
                    // marking is done after moving the cursor
                    DoScrollMarkUnmarkL();
                    }
                }
            else
                {
                iControlBarControl->MakeSelectorVisible( IsFocusShown() );
                // Do not set focus to empty list
                result = ETrue;
                }
            }
        else if( iFocusedControl == EMailListComponent )
            {
            result = iMailList->TreeControl()->OfferEventL( aEvent );
            if ( aShiftState )
                {
                // marking is done after moving the cursor
                DoScrollMarkUnmarkL();
                }
           }
        SetMskL();
        }
    if( aScancode == EStdKeyUpArrow )
        {
        if( iFocusedControl == EMailListComponent )
            {
            result = iMailList->TreeControl()->OfferEventL( aEvent );
            if ( aShiftState )
                {
                // marking is done after moving the cursor
                DoScrollMarkUnmarkL();
                }
            }
        else if( iFocusedControl == EControlBarComponent )
            {
            TInt focusedButtonId( iControlBarControl->GetFocusedButton()->Id() );
            if ( focusedButtonId == iNewEmailButtonId )
                {
                // Change focus to mail list if there are any
                // mails in it.
                if( iModel->Count() > 0 )
                    {
                    iFocusedControl = EMailListComponent;
                    result = EFalse;
                    }
                // Leave focus to control bar because there are no
                // mails in mail list.
                else
                    {
                    // Do not set focus to empty list
                    iControlBarControl->MakeSelectorVisible( IsFocusShown() );
                    result = ETrue;
                    }
                }
            else
                {
                iControlBarControl->MakeSelectorVisible( IsFocusShown() );
                result = static_cast<CAlfControl*>( iControlBarControl )->OfferEventL( aEvent );
                }
            }
        SetMskL();
        }
    else if( aScancode == EStdKeyDownArrow )
        {
        if( iFocusedControl == EMailListComponent )
            {
            result = iMailList->TreeControl()->OfferEventL( aEvent );
            if( aShiftState )
                {
                // marking is done after moving the cursor
                DoScrollMarkUnmarkL();
                }
            }
        else if( iFocusedControl == EControlBarComponent )
            {
            const TInt modelCount( iModel->Count() );
            TInt focusedButtonId( iControlBarControl->GetFocusedButton()->Id() );
            // If sort button is focused and there are mails in mail
            // list then focus is moved to mail list, otherwise
            // leave focus to control bar
            if( focusedButtonId == iSortButtonId )
                {
                if ( modelCount > 0 ) // Check for empty folder
                    {
                    iFocusedControl = EMailListComponent;
                    result = EFalse;
                    }
                else
                    {
                    iControlBarControl->MakeSelectorVisible( IsFocusShown() );
                    result = ETrue; // Do not set focus to empty list
                    }
                }
            else
                {
                iControlBarControl->MakeSelectorVisible( IsFocusShown() );
                result = static_cast<CAlfControl*>( iControlBarControl )->OfferEventL( aEvent );
                }
            }
        SetMskL();
        }

    return result;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateThemeL(const TBool aSystemUpdate)
    {
    iSkinChanged = iSkinChanged || aSystemUpdate;

    iFocusedTextColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
    iNormalTextColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
    iNodeTextColor = iAppUi.LayoutHandler()->ListNodeTextColor();

    iNewEmailButton->SetNormalTextColor( iNormalTextColor );
    iNewEmailButton->SetFocusedTextColor( iFocusedTextColor );

    iFolderListButton->SetNormalTextColor( iNormalTextColor );
    iFolderListButton->SetFocusedTextColor( iFocusedTextColor );

    iSortButton->SetNormalTextColor( iNormalTextColor );
    iSortButton->SetFocusedTextColor( iFocusedTextColor );

    //sometimes theme wasn't properly refreshed on buttons, this helps
    iNewEmailButton->HideButton();
    iNewEmailButton->ShowButtonL();

    iFolderListButton->HideButton();
    iFolderListButton->ShowButtonL();

    iSortButton->HideButton();
    iSortButton->ShowButtonL();
    }

// ---------------------------------------------------------------------------
// HandleForegroundEventL
// Function checks in foregroundevent that whether settings have changed and
// If there is a need to refresh the list.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleForegroundEventL( TBool aForeground )
	{
    FUNC_LOG;
    if( iMailFolder )
        {
        //emailindicator handling, is removed  from 9.2
        //TRAP_IGNORE(TFsEmailStatusPaneIndicatorHandler::StatusPaneMailIndicatorHandlingL( iMailFolder->GetMailBoxId().Id()));
        //Update mailbox widget index status in homescreen
        TFsEmailUiUtility::ToggleEmailIconL(EFalse, iMailFolder->GetMailBoxId() );
        }
    
	if ( aForeground && iFirstStartCompleted ) // Safety
	    {
	    // Update mail list settings and date formats, is done every time
	    // the user might have changed these in settings, so the list needs to refresh
	    // Store previous modes
	    TInt prevListMode = iListMode;
	    TInt prevNodesmode = iNodesInUse;
	    TAknUiZoom prevZoomLevel = iCurrentZoomLevel;
	    STimeDateFormats prevDateFormats = MailListTimeDateSettings();

	    // Get new settings and list drawing modes
	    CAknEnv::Static()->GetCurrentGlobalUiZoom( iCurrentZoomLevel );
	    UpdateMailListSettingsL();
	    UpdateMailListTimeDateSettings();

	    // Check sync icon timer and sync status
	    ConnectionIconHandling();

	    // Updates the folder list's size after the screensaver
	    iAppUi.FolderList().HandleForegroundEventL();

	    // Check for changed settings, in that case a complete list refresh is needed
	    if ( iSkinChanged
	         || iDateChanged
	         || iListMode != prevListMode
	         || iNodesInUse != prevNodesmode
	         || iCurrentZoomLevel != prevZoomLevel
	         || prevDateFormats.iDateFormat != iDateFormats.iDateFormat
	         || prevDateFormats.iTimeFormat != iDateFormats.iTimeFormat
	         || prevDateFormats.iAmPmPosition != iDateFormats.iAmPmPosition
	         || prevDateFormats.iDateSeparator != iDateFormats.iDateSeparator
	         || prevDateFormats.iTimeSeparator != iDateFormats.iTimeSeparator )
	        {
	        // Store the list of marked items. The markings are erased when the list
	        // is repopulated and, hence, the markings need to be reset after the
	        // refreshing is done. The marked items must be identified by the message
	        // ID rather than with the list ID because refreshing may change the list IDs.
	        RArray<TFSMailMsgId> markedMessages;
	        CleanupClosePushL( markedMessages );
	        GetMarkedMessagesL( markedMessages );
	        // Store the message ID of the focused item
	        TFSMailMsgId msgIdBeforeRefresh = MsgIdFromListId( iMailList->FocusedItem() );
	        TFsTreeItemId firstVisibleListId = iMailTreeListVisualizer->FirstVisibleItem();
	        TInt modelFirstIndexBeforeUpdate = ModelIndexFromListId( firstVisibleListId );
	        TInt highlightedIndexBeforeUpdate = HighlightedIndex();
	        TInt modelCountBeforeUpdate = iModel->Count();

	        UpdateMailListModelL();
	        if ( markedMessages.Count() )
	            {
	            RefreshL( &msgIdBeforeRefresh );
	            // Restore the marking status
	            MarkMessagesIfFoundL( markedMessages );
	            }
	        else
	            {
	            RefreshDeferred( &msgIdBeforeRefresh );
	            }
	        CleanupStack::PopAndDestroy( &markedMessages );
	        iSkinChanged = EFalse; // Toggle handled
	        iDateChanged = EFalse; // Toggle handled
	        }
        if (iMarkingMode)
            {
            RefreshMarkingModeL();
            }
	    }
	}


// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TIMESTAMP( "Message list command for handling" );
    CompletePendingRefresh();

    // custom command handling for launching plugin settings
   	if ( EFsEmailUiCmdSettingsBaseCommandId <= aCommand
   		 && EFsEmailUiCmdSettingsBaseCommandIdMax > aCommand )
   		{
		CESMailSettingsPlugin::TSubViewActivationData activationData;
		activationData.iAccount = iAppUi.GetActiveMailbox()->GetId();
		// which settings plugin subview will be activated
		TInt selectedCommand = aCommand - EFsEmailUiCmdSettingsBaseCommandId;
		activationData.iSubviewId = selectedCommand;

		TUid pluginUid = iAppUi.GetActiveMailbox()->GetSettingsUid();

        // If plugin view does not exist, create and register it to app ui.
        if ( !iAppUi.View( pluginUid ) )
            {
            CESMailSettingsPlugin* plugin = CESMailSettingsPlugin::NewL( pluginUid );
            CleanupStack::PushL( plugin );
            iAppUi.AddViewL( plugin ); // Ownership is transferred.
            CleanupStack::Pop( plugin );
            }
   		TUid messageId = TUid::Uid( CESMailSettingsPlugin::EActivateMailSettingsSubview );
		const TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> pluginMessagePkg( activationData );

        StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
        iAppUi.EnterPluginSettingsViewL( pluginUid, messageId, pluginMessagePkg );
   		}

   	// Many commands may be targetted either to the focused item or to the marked item(s).
   	// Get list of marked or focused items.
    RFsTreeItemIdList actionTargetItems;
    CleanupClosePushL( actionTargetItems );
    GetActionsTargetEntriesL( actionTargetItems );

    // Check if the focus needs to be removed.
    if ( ( iStylusPopUpMenuVisible ) &&
         ( aCommand == KErrCancel ||
           aCommand == EFsEmailUiCmdActionsDelete ||
    	   aCommand == EFsEmailUiCmdMarkAsRead ||
    	   aCommand == EFsEmailUiCmdMarkAsUnread ||
    	   aCommand == EFsEmailUiCmdActionsMove ||
    	   aCommand == EFsEmailUiCmdActionsMoveMessage ||
    	   aCommand == EFsEmailUiCmdMarkingModeFromPopUp ||
    	   aCommand == EFsEmailUiCmdActionsCollapseAll ||
    	   aCommand == EFsEmailUiCmdActionsExpandAll ||
           aCommand == EFsEmailUiCmdActionsFlag ) )
    	{
   		// We end up here if the user selects an option from the pop up menu
    	// or exits the menu by tapping outside of it's area.
    	// Remove the focus from a list item if an item is focused.
    	iStylusPopUpMenuVisible = EFalse;
    	iAppUi.SetFocusVisibility( EFalse );
    	}

    switch( aCommand )
        {
    	case EAknSoftkeyOpen:
            {
            TIMESTAMP( "Open email selected from message list" );
            if( !iAppUi.IsFocusShown() )
                {
                iAppUi.SetFocusVisibility( ETrue );
                break;
                }
            }
       	case EAknSoftkeySelect:
       	    {
       	    TIMESTAMP( "Open email selected from message list" );
       	    if ( iFocusedControl == EMailListComponent )
       	        {
       	        CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
       	        // MAIL ITEM; OPEN MAIL
       	        if ( item && item->ModelItemType() == ETypeMailItem )
       	            {
       	            CFSMailMessage* messagePtr = &item->MessagePtr();
       	            if ( messagePtr )
       	                {
       	                OpenHighlightedMailL();
       	                }
       	            }
       	        }
       	    }
       	    break;
       	case EAknSoftkeyChange:
       	    {
    		if( !iAppUi.IsFocusShown() )
    			{
    			iAppUi.SetFocusVisibility( ETrue );
    			break;
    			}
       	    if( iFocusedControl == EControlBarComponent )
       	        {
       	        TInt focusedButtonId = iControlBarControl->GetFocusedButton()->Id();
       	        if ( focusedButtonId == iFolderListButtonId )
       	            {
                    //<cmail>
                    //Set touchmanager not active for preventing getting events.
                    DisableMailList(ETrue);
                    //</cmail>
       	            iAppUi.ShowFolderListInPopupL( FolderId(), this, iFolderListButton );
       	            }
       	        else if( focusedButtonId == iSortButtonId )
       	            {
       	            TFSFolderType folderType;
       	            if( iMailFolder )
       	                {
       	                folderType = iMailFolder->GetFolderType();
       	                }
       	            else
       	                {
       	                folderType = EFSInbox;
       	                }
                    // <cmail> Sorting set possible even for empty mailbox
                    // Show sort if model has data.
                    //if ( iModel->Count() )
                    //    {
                    // </cmail>
                        //<cmail>
                        //Set touchmanager not active for preventing getting events.
                        DisableMailList(ETrue);
                        //</cmail>
                        iAppUi.ShowSortListInPopupL( iCurrentSortCriteria, folderType, this, iSortButton );
                    // <cmail>
                    //    }
                    // </cmail>
       	            }
       	        }
       	    }
       	    break;
        case EAknSoftkeyBack:
       		if ( !iAppUi.ViewSwitchingOngoing() )
				{
				NavigateBackL();
				}
        	break;
        case EAknSoftkeyCancel:
            if (iMarkingMode)
                {
                ExitMarkingModeL();
                }
            break;
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EFsEmailUiCmdExit:
            TIMESTAMP( "Exit selected from message list" );
            iAppUi.Exit();
            break;
       	case EFsEmailUiCmdCalActionsReplyAsMail:
		case EFsEmailUiCmdActionsReply:
			{
			TIMESTAMP( "Reply to selected from message list" );
   	   	 	ReplyL( NULL ); // Function will check marked/highlighted msg
			}
            break;
		case EFsEmailUiCmdActionsReplyAll:
			{
			TIMESTAMP( "Reply to all selected from message list" );
			ReplyAllL( NULL ); // Function will check marked/highlighted msg
			}
            break;
       	case EFsEmailUiCmdCalActionsForwardAsMail:
		case EFsEmailUiCmdActionsForward:
			{
			TIMESTAMP( "Forward selected from message list" );
           	ForwardL( NULL ); // Function will check marked/highlighted msg
			}
            break;
       	case EFsEmailUiCmdActionsCallSender:
			{
       		if ( !iAppUi.ViewSwitchingOngoing() )
				{
				if ( iMailFolder && iMailFolder->GetFolderType() != EFSOutbox &&
					 iMailFolder->GetFolderType() != EFSDraftsFolder )
					{
					// Calling can happen only when there's exactly one message marked or in focus
					if ( actionTargetItems.Count() == 1 )
						{
						CFSMailMessage& message = MsgPtrFromListIdL( actionTargetItems[0] );
						CFSMailAddress* fromAddress = message.GetSender();
						TDesC* mailAddress(0);
						if ( fromAddress )
						    {
						    mailAddress = &fromAddress->GetEmailAddress();
						    }
						if ( mailAddress && mailAddress->Length() )
						    {
						    if ( iMailFolder->GetFolderType() == EFSSentFolder )
						        {
	                            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL(
	                                    *mailAddress, iAppUi.GetActiveMailbox(), this, EFalse );
						        }
						    else
						        {
                                CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL(
                                        *mailAddress, iAppUi.GetActiveMailbox(), this, ETrue );
						        }
						    }
						}
					}
				}
			}
			break;
        case EFsEmailUiCmdMarkAsRead:
        	{
			// Change status of marked or highlighted
			if ( CountMarkedItemsL() )
				{
				ChangeReadStatusOfMarkedL( ETrue );
				}
			else
				{
                ChangeReadStatusOfHighlightedL( ETrue );
				}
        	}
        	break;
        case EFsEmailUiCmdMarkAsUnread:
        	{
            // Change status of marked or highlighted
			if ( CountMarkedItemsL() )
				{
				ChangeReadStatusOfMarkedL( EFalse );
				}
			else
				{
                ChangeReadStatusOfHighlightedL( EFalse );
				}
        	}
			break;
		case EFsEmailUiCmdGoToTop:
			{
			// Safety check, ignore command if the list is empty
			if( iTreeItemArray.Count() )
			    {
	            if ( iFocusedControl != EMailListComponent )
	                {
	                iFocusedControl = EMailListComponent;
	                iMailList->SetFocusedL( ETrue );
	                iControlBarControl->SetFocusL( EFalse );
	                }
	            // No need to check for nodes as in move to bottom.
	            iMailTreeListVisualizer->SetFocusedItemL( iTreeItemArray[0].iListItemId );
			    }
            }
            break;
		case EFsEmailUiCmdGoToBottom:
			{
	         // Safety check, ignore command if the list is empty
            if( iTreeItemArray.Count() )
                {
             	if ( iFocusedControl != EMailListComponent )
            		{
            		iFocusedControl = EMailListComponent;
            		iMailList->SetFocusedL( ETrue );
            		iControlBarControl->SetFocusL( EFalse );
            		}

    		 	TFsTreeItemId bottomItem = iTreeItemArray[iTreeItemArray.Count()-1].iListItemId;
    		 	TFsTreeItemId parentItem = ParentNode( bottomItem ); // parent node of the bottom item

    			// Check whether the parent is node and is collapsed
    			if ( parentItem>0 && !iMailList->IsExpanded( parentItem ) )
    				{
    				bottomItem = parentItem;
    				}
    	       	iMailTreeListVisualizer->SetFocusedItemL( bottomItem );
                }
   			}
            break;
        case EFsEmailUiCmdPageUp:
            {
            TKeyEvent simEvent = { EKeyPageUp, EStdKeyPageUp, 0, 0 };
            iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
            }
            break;
        case EFsEmailUiCmdPageDown:
            {
            TKeyEvent simEvent = { EKeyPageDown, EStdKeyPageDown, 0, 0 };
            iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
            }
            break;
       	case EFsEmailUiCmdActionsDeleteCalEvent:
		case EFsEmailUiCmdActionsDelete:
			{
			TIMESTAMP( "Delete to selected from message list" );
			TInt index = HighlightedIndex();
			CFSEmailUiMailListModelItem* item =
				dynamic_cast<CFSEmailUiMailListModelItem*>(
						iModel->Item( index ) );

			// If selected item is separator (divider) mark/unmark all messages
			// under it.

			// Delete message only if mail list component is focused
			// or if there are some marked items
			TInt markedItems( CountMarkedItemsL() );
            const TBool isNode(iMailList->IsNode(iMailList->FocusedItem()));
            if (iFocusedControl == EMailListComponent || markedItems
                    || isNode)
                {
                DeleteMessagesL();
                }
            }
            break;
        case EFsEmailUiCmdCompose:
            {
            // Switching to another view can take some time and this view
            // can still receive some commands - so ignore them.
            if ( iAppUi.CurrentActiveView() == this )
                {
                TIMESTAMP( "Create new message selected from message list" );
                CreateNewMsgL();
                }
            }
            break;
       	case EFsEmailUiCmdMessageDetails:
			{
            // Message details can be viewed only when there's exactly one message marked or in focus
            if ( actionTargetItems.Count() == 1 )
                {
                CFSMailMessage& message = MsgPtrFromListIdL( actionTargetItems[0] );
	  			TMsgDetailsActivationData msgDetailsData;
	  			msgDetailsData.iMailBoxId = iAppUi.GetActiveMailboxId();
	  			msgDetailsData.iFolderId = FolderId();
	  			msgDetailsData.iMessageId = message.GetMessageId();
				const TPckgBuf<TMsgDetailsActivationData> pkgOut( msgDetailsData );
				iAppUi.EnterFsEmailViewL( MsgDetailsViewId, KStartMsgDetailsToBeginning, pkgOut );
				}
			}
			break;
       	case EFsEmailUiCmdSettingsGlobal:
       		{
  			iAppUi.EnterFsEmailViewL( GlobalSettingsViewId );
       		}
       		break;
       	case EFsEmailUiCmdSettingsService:
       	case EFsEmailUiCmdSettingsMailbox:
       		{
			CESMailSettingsPlugin::TSubViewActivationData activationData;
			activationData.iAccount = iAppUi.GetActiveMailbox()->GetId();
			activationData.iSubviewId = 0;
			if ( EFsEmailUiCmdSettingsService == aCommand )
		 		{
				activationData.iSubviewId = 1;
				}

			TUid pluginUid = iAppUi.GetActiveMailbox()->GetSettingsUid();
			// register plugin view if not exists so that activation can be made
			if ( !iAppUi.View( pluginUid ) )
				{
				CESMailSettingsPlugin* plugin = CESMailSettingsPlugin::NewL( pluginUid );
				CleanupStack::PushL( plugin );
				iAppUi.AddViewL( plugin );
				CleanupStack::Pop( plugin );
				}
       		TUid messageId = TUid::Uid( CESMailSettingsPlugin::EActivateMailSettingsSubview );
			const TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> pluginMessagePkg( activationData );
			iAppUi.EnterPluginSettingsViewL( pluginUid, messageId, pluginMessagePkg );
       		}
       		break;
       	case EFsEmailUiCmdOpen:
        case EFsEmailUiCmdActionsOpen:
        	{
        	TIMESTAMP( "Open email selected from message list" );
	        if ( iFocusedControl == EMailListComponent )
	            {
	            // Opening can happen only when there's exactly one message marked or in focus
	            if ( actionTargetItems.Count() == 1 )
	                {
	                OpenMailItemL( actionTargetItems[0] );
	                }
	            }
			}
        	break;
        case EFsEmailUiCmdActionsFlag:
	        {
            SetMessageFollowupFlagL();
	        }
	        break;
        case EFsEmailUiCmdActionsCollapseAll:
            // Safety check, ignore command if the list is empty
            if( iMailList->Count() )
                {
            	CollapseAllNodesL();
                }
        	break;
       	case EFsEmailUiCmdActionsExpandAll:
       	    // Safety check, ignore command if the list is empty
       	    if( iMailList->Count() )
       	        {
           		ExpandAllNodesL();
                }
			break;
        case EFsEmailUiCmdCollapse:
    		if( !iAppUi.IsFocusShown() )
    			{
    			iAppUi.SetFocusVisibility( ETrue );
    			break;
    			}
            // Safety check, ignore command if the list is empty
            if( iMailList->Count() )
                {
                ExpandOrCollapseL();
                }
            break;
        case EFsEmailUiCmdExpand:
    		if( !iAppUi.IsFocusShown() )
    			{
    			iAppUi.SetFocusVisibility( ETrue );
    			break;
    			}
            // Safety check, ignore command if the list is empty
            if( iMailList->Count() )
                {
                ExpandOrCollapseL();
                }
            break;
        case EFsEmailUiCmdSearch:
        	{
			TSearchListActivationData tmp;
			tmp.iMailBoxId = iAppUi.GetActiveMailbox()->GetId();
			const TPckgBuf<TSearchListActivationData> pkgOut( tmp );
			iAppUi.EnterFsEmailViewL( SearchListViewId, KStartNewSearch, pkgOut );
        	}
        	break;
// <cmail> Prevent Download Manager opening with attachments
//       	case EFsEmailUiCmdDownloadManager:
//  			iAppUi.EnterFsEmailViewL( DownloadManagerViewId );
//			break;
// </cmail>
        case EFsEmailUiCmdMarkingModeFromPopUp:
            {
            EnterMarkingModeL();
            // Mark current item first
            TInt focusedItem = iMailList->FocusedItem();
            if ( focusedItem > 0 && iFocusedControl == EMailListComponent )
                {
                CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
				if ( item && item->ModelItemType() == ETypeMailItem )
				    {
                    iMailList->MarkItemL( iMailList->FocusedItem(), ETrue );
					}
				else if ( item && item->ModelItemType() == ETypeSeparator )
					{
					TInt index = HighlightedIndex();
					MarkItemsUnderSeparatorL( ETrue, index );
					}
                }
            }
            break;        
        case EFsEmailUiCmdMarkingMode:
            {
            EnterMarkingModeL();
            }
            break;
        case EFsEmailUiCmdMarkingModeDelete:
            {
            DeleteMessagesL();
            }
            break;
        case EFsEmailUiCmdMarkingModeUnread:
            {
            ChangeReadStatusOfMarkedL( EFalse );
            ExitMarkingModeL();
            }
            break;
        case EFsEmailUiCmdMarkingModeRead:
            {
            ChangeReadStatusOfMarkedL( ETrue );
            ExitMarkingModeL();
            }
            break;            
        case EFsEmailUiCmdMarkingModeFollowUp:
            {
            SetMessageFollowupFlagL();
            }
            break;            
        case EFsEmailUiCmdMarkingModeMarkAll:
            {
            MarkAllItemsL();
            }
            break;
        case EFsEmailUiCmdMarkingModeUnmarkAll:
            {
            UnmarkAllItemsL();
            }
            break;
		case EFsEmailUiCmdReadEmail:
			{
			iAppUi.StartReadingEmailsL();
			}
			break;
       	case EFsEmailUiCmdSync:
			{
			TBool supportsSync = iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaSupportsSync );
			if ( supportsSync )
			    {
			    // If synchronizing is ongoing and a new sync is started we ignore it
			    if( !GetLatestSyncState() )
			        {
			        iAppUi.SyncActiveMailBoxL();
			        // Sync was started by the user
			        ManualMailBoxSync( ETrue );
			        }
			    }
			  else
			    {
			    //POP3 synchronise
				iAppUi.DoAutoConnectL();
				}
			}
			break;
       	case EFsEmailUiCmdCancelSync:
       	    {
       	    if ( GetLatestSyncState() )
       	        {
                iAppUi.StopActiveMailBoxSyncL();
       	        }
       	    }
       	    break;
       	case EFsEmailUiCmdGoOffline:
        	{
    	   	iAppUi.GetActiveMailbox()->GoOfflineL();
        	}
        	break;
        case EFsEmailUiCmdGoOnline:
        	{
			CFSMailBox* mb = iAppUi.GetActiveMailbox();
        	mb->GoOnlineL();
        	}
        	break;
       	case EFsEmailUiCmdHelp:
			{

			TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
			}
			break;


    	case EFsEmailUiCmdActionsAddContact:
    		{
    		// contact can be got from message only when there's exactly one target message
    		if ( actionTargetItems.Count() == 1 && iMailFolder )
				{
				CFSMailMessage& message = MsgPtrFromListIdL( actionTargetItems[0] );

				CFSMailAddress* addressToBeStored(0);
				TInt folderType = iMailFolder->GetFolderType();
				// Use first to recepient in outbox, drafts and sent if found.
				if ( folderType == EFSOutbox || folderType == EFSDraftsFolder || folderType == EFSSentFolder )
				    {
				    RPointerArray<CFSMailAddress>& toArray = message.GetToRecipients();
				    if ( toArray.Count() )
				        {
				        addressToBeStored = toArray[0];
				        }
				    }
				// Use sender in all other "normal" folders
				else
				    {
				    addressToBeStored = message.GetSender();
				    }
				TDesC* emailAddressText(0);
				if ( addressToBeStored )
				    {
				    emailAddressText = &addressToBeStored->GetEmailAddress();
				    }
				if ( emailAddressText && emailAddressText->Length() )
				    {
				    TAddToContactsType type;
				    //Query to "update existing" or "Create new" --> EFALSE = user choosed "cancel"
				    if ( CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddtoContactsQueryL( type ) )
				        {
				        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddToContactL(
				                *emailAddressText, EContactUpdateEmail, type, this );
				        }
				    }
				}
    		}
			break;
    	case EFsEmailUiCmdMarkingModeMove:
            {
			iMarkingModeWaitingToExit = ETrue;
			}
			// Flow through			
       	case EFsEmailUiCmdActionsMove:
       	case EFsEmailUiCmdActionsMoveMessage:
			{
			TFSMailMsgId folderID;
			folderID.SetNullId(); // Selection is popped up with NULL
			MoveMsgsToFolderL( folderID );
			}
			break;
       	case EFsEmailUiCmdActionsMoveToDrafts:
			{
			MoveMessagesToDraftsL();
			}
			break;
        case EFsEmailUiCmdActionsCopyMessage:
            {
            }
            break;
        case EFsEmailUiCmdActionsCollapseExpandAllToggle:
            {
           	ShortcutCollapseExpandAllToggleL();
            }
            break;
        case EFsEmailUiCmdMarkAsReadUnreadToggle:
            {
			ShortcutReadUnreadToggleL();
            }
            break;
        case EFsEmailUiCmdGoToSwitchFolder:
            {
            iControlBarControl->SetFocusByIdL( iFolderListButtonId );
            iControlBarControl->MakeSelectorVisible( IsFocusShown() );
            iFocusedControl = EControlBarComponent;
            iMailList->SetFocusedL( EFalse );
            }
            break;
        case EFsEmailUiCmdGoToSwitchSorting:
            {
            iControlBarControl->SetFocusByIdL( iSortButtonId );
            iControlBarControl->MakeSelectorVisible( IsFocusShown() );
            iFocusedControl = EControlBarComponent;
           	iMailList->SetFocusedL( EFalse );
            }
            break;
        case EFsEmailUiCmdGoToSwitchNewEmail:
            {
            iControlBarControl->SetFocusByIdL( iNewEmailButtonId );
            iControlBarControl->MakeSelectorVisible( IsFocusShown() );
            iFocusedControl = EControlBarComponent;
            iMailList->SetFocusedL( EFalse );
            break;
            }
        case EFsEmailUiCmdCalActionsAccept:
        case EFsEmailUiCmdCalActionsTentative:
        case EFsEmailUiCmdCalActionsDecline:
        case EFsEmailUiCmdCalRemoveFromCalendar:
			{
			if ( actionTargetItems.Count() == 1 )
                {
                TInt mrItemIdx = ModelIndexFromListId( actionTargetItems[0] );
                CFSEmailUiMailListModelItem* item =
                    dynamic_cast<CFSEmailUiMailListModelItem*>( iModel->Item(mrItemIdx) );
                if ( item && item->MessagePtr().IsFlagSet( EFSMsgFlag_CalendarMsg ) )
                    {
                    // Respond to meeting request
                    if (  iAppUi.MrViewerInstanceL() &&  iAppUi.MrViewerInstanceL()->CanViewMessage( item->MessagePtr() ) )
                        {
                        ChangeReadStatusOfHighlightedL( ETrue );
                        iAppUi.MailViewer().HandleMrCommandL( aCommand,
                                                              iAppUi.GetActiveMailbox()->GetId(),
                                                              FolderId(),
                                                              item->MessagePtr().GetMessageId() );
                        }
                    }
			    }
			}
			break;
       	case EFsEmailUiCmdActionsEmptyDeleted:
       		{
       		// <cmail>
            if ( iMailFolder && iMailFolder->GetFolderType() == EFSDeleted &&
       			 iModel->Count() != 0 )
       		// </cmail>
       			{
    			TBool okToDelete( EFalse );
				HBufC* msg = StringLoader::LoadLC( R_FREESTYLE_EMAIL_QUERY_NOTE_PERMANENTLY_DELETE );
 	    		CAknQueryDialog *queryNote = new ( ELeave ) CAknQueryDialog();
				CleanupStack::PushL( queryNote );
				queryNote->SetPromptL( *msg );
				CleanupStack::Pop( queryNote );
				CleanupStack::PopAndDestroy( msg );
				okToDelete = queryNote->ExecuteLD( R_FSEMAIL_QUERY_DIALOG );
				if ( okToDelete )
 					{
					// Empty deleted items folder
					RArray<TFSMailMsgId> msgIds;
					CleanupClosePushL( msgIds );
					for ( TInt i=0 ; i<iModel->Count();i++ )
						{
						CFSEmailUiMailListModelItem* item =
	    					static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
	    				if ( item && item->ModelItemType() == ETypeMailItem )
	    					{
							msgIds.Append( item->MessagePtr().GetMessageId() );
	    					}
						}
					TFSMailMsgId folderId = FolderId();
					TFSMailMsgId mailBox = iAppUi.GetActiveMailbox()->GetId();
					iAppUi.GetMailClient()->DeleteMessagesByUidL( mailBox, folderId, msgIds );
		            RemoveMsgItemsFromListIfFoundL( msgIds );
					CleanupStack::PopAndDestroy( &msgIds );
 					}
       			}
       		}
			break;
        case EFsEmailUiCmdSettings:
            {
            TInt tmp = 0;
            const TPckgBuf<TInt> pkgBuf( tmp );
            iAppUi.EnterFsEmailViewL( SettingsViewId,
                                      TUid::Uid(KMailSettingsOpenMainList), pkgBuf );
            }
            break;


	    default:
        	break;
        } // switch ( aCommand )
    CleanupStack::PopAndDestroy( &actionTargetItems );
    TIMESTAMP( "Message list selected operation done" );
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::MoveMsgsToFolderL( const TFSMailMsgId& aDestinationFolderId )
    {
    FUNC_LOG;
    TInt ret(KErrGeneral);
    RFsTreeItemIdList targetEntries;
    CleanupClosePushL( targetEntries );
    GetActionsTargetEntriesL( targetEntries );
    TBool supportsMove = iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder );
    // Moving is not possible, if there are no items or the focus is on separator and
    // there are no marked items. In outbox, moving is allowed only to the drafts folder;
    // folder selection popup is not allowed.
    // These have to be checked before launching the folder view.
    if ( targetEntries.Count() && supportsMove )
        {
        TBool outbox = (iMailFolder && iMailFolder->GetFolderType() == EFSOutbox);

        if ( !(outbox && aDestinationFolderId.IsNullId()) )
            {
            // If destination folder is not set, it needs to be asked messages are moved after callbak
            if ( aDestinationFolderId.IsNullId() )
                {
                // Activate folder selection view and handle moving after callback gets destination
                iMoveToFolderOngoing = ETrue;
                TFolderListActivationData folderListData;
                folderListData.iCallback = this;
                if(iMailFolder) // Coverity error fix , assuming that inbox is best bet for safe data. 
                    {
                    folderListData.iSourceFolderType = iMailFolder->GetFolderType();
                    }
                else
                    {
                    folderListData.iSourceFolderType = EFSInbox;
                    }
                const TPckgBuf<TFolderListActivationData> pkgOut( folderListData );
                if ( targetEntries.Count() == 1 )
                    {
                    iAppUi.EnterFsEmailViewL( FolderListId, KFolderListMoveMessage, pkgOut );
                    }
                else
                    {
                    iAppUi.EnterFsEmailViewL( FolderListId, KFolderListMoveMessages, pkgOut );
                    }
                ret = KErrNotReady;
                }
            // Destination folder is set, move message(s) immediately
            else
                {
                iMoveToFolderOngoing = EFalse;
                // Get message IDs from target list IDs
                RArray<TFSMailMsgId> msgIds;
                CleanupClosePushL( msgIds );
                for ( TInt i = 0 ; i < targetEntries.Count() ; i++)
                    {
                    msgIds.Append( MsgIdFromListId( targetEntries[i] ) );
                    }

                if ( iMailFolder && msgIds.Count() ) // Something to move
                    {
                    // check that source and destination are different
                    if( iMailFolder->GetFolderId() != aDestinationFolderId )
                        {
                        // Trap is needed because protocol might return KErrNotSupported
                        // If move away from current folder is not supprted
                        TRAPD( errMove, iAppUi.GetActiveMailbox()->MoveMessagesL( msgIds, iMailFolder->GetFolderId(), aDestinationFolderId ) );
                        if ( errMove != KErrNotSupported )
                            {
                            if ( errMove == KErrNone )
                                {
                                RemoveMsgItemsFromListIfFoundL( msgIds );
                                if ( !iMsgNoteTimer )
                                    {
                                    iMsgNoteTimer = CMsgMovedNoteTimer::NewL( &iAppUi, this );
                                    }
                                iMsgNoteTimer->Cancel();
                                iMsgNoteTimer->Start( msgIds.Count(), aDestinationFolderId );
                                ret = KErrNone;
                                }
                            else
                                {
                                User::Leave(errMove);
                                }
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( &msgIds );
                }
            }
        }
    CleanupStack::PopAndDestroy( &targetEntries );
    return ret;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::MoveMessagesToDraftsL()
	{
    FUNC_LOG;
	if ( iMailFolder && iMailFolder->GetFolderType() == EFSOutbox ) // Move to drafts should only be available from outbox, safety
		{
		MoveMsgsToFolderL( iAppUi.GetActiveMailbox()->GetStandardFolderId( EFSDraftsFolder ) );
		}
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::CollapseAllNodesL()
	{
    FUNC_LOG;
    // Safety check, ignore command if the list is empty
    if ( iMailList->Count() > 0 )
        {
    	iMailTreeListVisualizer->CollapseAllL();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ExpandAllNodesL()
	{
    FUNC_LOG;
    // Safety check, ignore command if the list is empty
	if ( iMailList->Count() > 0 )
	    {
    	TFsTreeItemId prevId = iMailList->FocusedItem();
    	iMailTreeListVisualizer->ExpandAllL();
    	iMailTreeListVisualizer->SetFocusedItemL( prevId );
        }
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ExpandOrCollapseL()
    {
    FUNC_LOG;
    CFSEmailUiMailListModelItem* item =
        dynamic_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
    if ( item && item->ModelItemType() == ETypeSeparator )
        {
        if ( iMailList->IsExpanded( iMailList->FocusedItem() ) )
            {
            //ChangeMskCommandL( R_FSE_QTN_MSK_COLLAPSE );
            iMailList->CollapseNodeL( iMailList->FocusedItem() );
            }
        else
            {
            //ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND );
            iMailList->ExpandNodeL( iMailList->FocusedItem() );
            }
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetMessageFollowupFlagL()
	{
    FUNC_LOG;
	if ( TFsEmailUiUtility::IsFollowUpSupported( *iAppUi.GetActiveMailbox() ) )
		{
	    RFsTreeItemIdList targetItems;
	    CleanupClosePushL( targetItems );
	    GetActionsTargetEntriesL( targetItems );

		// If there are marked or focused item(s)
		if ( targetItems.Count() )
			{
			// Convert list IDs to message IDs because it's possible that list refresh
			// causes list IDs to change while the flag dialog is open
			RArray< TFSMailMsgId > targetMsgIds;
			CleanupClosePushL( targetMsgIds );
			for ( TInt i = 0 ; i < targetItems.Count() ; ++i )
			    {
			    TFSMailMsgId msgId = MsgIdFromIndex( ModelIndexFromListId( targetItems[i] ) );
			    targetMsgIds.AppendL( msgId );
			    }

			TFollowUpNewState newFollowUpState = EFollowUpNoChanges;
			if ( TFsEmailUiUtility::RunFollowUpListDialogL( newFollowUpState ) )
				{
				// Update the target messages
				for ( TInt i = 0 ; i < targetMsgIds.Count() ; ++i )
				    {
	                // Confirm that message is still in the list after selection has closed
				    TInt itemIndex = ItemIndexFromMessageId( targetMsgIds[i] );
				    CFSEmailUiMailListModelItem* modelItem =
				        static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( itemIndex ) );
	                CFSMailMessage* confirmedPtr = NULL; // owned by the model
	                if ( modelItem )
	                    {
	                    confirmedPtr = &modelItem->MessagePtr();
	                    }
	                if ( confirmedPtr )
	                    {
	                    // Store flags to confirmed pointer for saving
	                    TInt currentFlags = confirmedPtr->GetFlags();
	                    switch ( newFollowUpState )
	                        {
	                        case EFollowUp:
	                            {
	                            confirmedPtr->SetFlag( EFSMsgFlag_FollowUp );
	                            confirmedPtr->ResetFlag( EFSMsgFlag_FollowUpComplete );
	                            }
	                            break;
	                        case EFollowUpComplete:
	                            {
	                            confirmedPtr->SetFlag( EFSMsgFlag_FollowUpComplete );
	                            confirmedPtr->ResetFlag( EFSMsgFlag_FollowUp );
	                            }
	                            break;
	                        case EFollowUpClear:
	                            {
	                            confirmedPtr->ResetFlag( EFSMsgFlag_FollowUp | EFSMsgFlag_FollowUpComplete );
	                            }
	                            break;
	                        }

	                    TInt newFlags = confirmedPtr->GetFlags();
	                    if ( newFlags != currentFlags )
	                        {
	                        // Save confirmed message
	                        confirmedPtr->SaveMessageL();

	                        // Update flag icon
                            UpdateMsgIconAndBoldingL( itemIndex );

                            // In case we are in flag sort mode, resetting the flag may cause the list order to be out of date.
                            if ( iCurrentSortCriteria.iField == EFSMailSortByFlagStatus )
	                            {
	                            iListOrderMayBeOutOfDate = ETrue;
	                            }
	                        }
	                    }
				    }
                if ( iMarkingMode ) 
                    {
                    ExitMarkingModeL();
                    }
				}
			CleanupStack::PopAndDestroy( &targetMsgIds );
			}
		CleanupStack::PopAndDestroy( &targetItems );
		}
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetViewSoftkeysL( TInt aResourceId )
	{
    FUNC_LOG;
    Cba()->SetCommandSetL( aResourceId );
    Cba()->DrawDeferred();
	}

// ---------------------------------------------------------------------------
// Method to set Middle SoftKey
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetMskL()
    {
    FUNC_LOG;
    // If there is no keyboard present, than Msk buttom must not be shown
    if ( iAppUi.ShortcutBinding().KeyBoardType() == 
                                    CFSEmailUiShortcutBinding::ENoKeyboard )
        {
        return;
        }
    // Get the mail list count
    TInt listCount(0);
    if ( iMailList )
        {
        listCount = iMailList->Count();
        }

    if ( iFocusedControl == EControlBarComponent )
        {
        ChangeMskCommandL( R_FSE_QTN_MSK_CHANGE ); // change - switch focus on
        }
    else  if ( iFocusedControl == EMailListComponent )
        {
        if ( listCount ) // Safety check
            {
            CFSEmailUiMailListModelItem* item =
                dynamic_cast<CFSEmailUiMailListModelItem*>( iModel->Item(HighlightedIndex()) );
            // SHIFT DEPRESSED. SELECTION KEY DOES MARK/UNMARK
            if ( iShiftDepressed )
                {
                // Nodes may not be marked
                if ( iMailList->IsNode( iMailList->FocusedItem() ) )
                    {
                    ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY ); // nothing
                    }
                else
                    {
                    if ( iMailList->IsMarked( iMailList->FocusedItem() ) )
                        {
                        ChangeMskCommandL( R_FSE_QTN_MSK_UNMARK ); // unmark item
                        }
                    else
                        {
                        ChangeMskCommandL( R_FSE_QTN_MSK_MARK ); // mark item
                        }
                    }
                }
            else
                {
                // MAIL ITEM; OPEN MAIL
                if ( item && item->ModelItemType() == ETypeMailItem )
                    {
                    CFSMailMessage* messagePtr = &item->MessagePtr();
                    if ( messagePtr )
                        {
                        if ( iMailFolder && iMailFolder->GetFolderType() == EFSOutbox )
                            {
                            ChangeMskCommandL( R_FSE_QTN_MSK_OPEN_BLOCKED ); // blocked email opening 
                            }
                        else
                            {
                            ChangeMskCommandL( R_FSE_QTN_MSK_OPEN ); // open email
                            }
                        }
                    }

                // SEPARAOR ITEM; COLLAPSE / EXPAND NODE
                else if ( item && item->ModelItemType() == ETypeSeparator )
                    {
                    if( iMailList->IsNode( iMailList->FocusedItem() ) )
                        {
                        if ( iMailList->IsExpanded( iMailList->FocusedItem() ) )
                            {
                            ChangeMskCommandL( R_FSE_QTN_MSK_COLLAPSE ); // collapse folder
                            }
                        else
                            {
                            ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND ); // expand folder
                            }
                        }
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::OfferEventL( const TAlfEvent& aEvent )
    {
    FUNC_LOG;
    TBool result( EFalse );

    if ( !aEvent.IsKeyEvent() )
        {
        // Only handle key events
        if ( aEvent.IsPointerEvent() )
            {
            if( aEvent.PointerDown() )
                {
                // If pointer down event was made on control bar area
                // focus needs to be changed to control bar control, if it
                // didn't already have focus.
               if( HitTest( *iControlBarControl, aEvent.PointerEvent().iPosition ) )
                    {
                    if( iFocusedControl != EControlBarComponent )
                        {
                        SetControlBarFocusedL();
                        }
                    }
                else
                    {
                    // If pointer down event was not made on control bar area
                    // then focus need to be set to mail list component, if it
                    // didn't already have focus. If mail list is empty, then
                    // focus is hidden and control bar stays as a focused item.
                    if( ( iFocusedControl != EMailListComponent ) &&
                        ( iMailList->Count() > 0 ) )
                        {
                        SetTreeListFocusedL();
                        }
                    else
                        {
                        iAppUi.SetFocusVisibility( EFalse );
                        }
                    }
                }

            // Offer event to focused control.
            if( iFocusedControl == EMailListComponent )
                {
                result = iMailList->TreeControl()->OfferEventL( aEvent );
                }
            else // iFocusedControl == EControlBarComponent
                {
                result = static_cast<CAlfControl*>(
                    iControlBarControl )->OfferEventL( aEvent );
                }
            }
        return result;
        }

    TInt scanCode = aEvent.KeyEvent().iScanCode;
    // Swap right and left controls in mirrored layout
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        if ( scanCode == EStdKeyRightArrow )
            {
            scanCode = EStdKeyLeftArrow;
            }
        else if ( scanCode == EStdKeyLeftArrow )
            {
            scanCode = EStdKeyRightArrow;
            }
        }

    // Toggle mark items state when shift key is pressed or released
    if ( iFocusedControl == EMailListComponent &&
    	 ( scanCode == EStdKeyLeftShift ||
    	   scanCode == EStdKeyRightShift ||
    	   scanCode == EStdKeyHash ) )
        {
        if ( aEvent.Code() == EEventKeyDown )
            {
            iShiftDepressed = ETrue;
            iOtherKeyPressedWhileShiftDepressed = EFalse;
            if ( iModel->Count() && iMailList->IsMarked( iMailList->FocusedItem() ) )
                {
                iListMarkItemsState = EFalse; // Unmark state
                }
            else
                {
                iListMarkItemsState = ETrue; // Mark items state
                }
            }
        else if ( aEvent.Code() == EEventKeyUp )
            {
            iShiftDepressed = EFalse;
            }
        }

    // If any other key event is gained while hash is depressed, the hash key is used only
    // as shift and not as individual key.
    if ( iShiftDepressed && scanCode &&
            scanCode != EStdKeyHash && scanCode != EStdKeyLeftShift && scanCode != EStdKeyRightShift )
        {
        iOtherKeyPressedWhileShiftDepressed = ETrue;
        }

    // MSK label can now be updated when shift key has been handled
    SetMskL();
    // On KeyUp of EStdKeyYes usually Call application is called - prevent it
    if ( iConsumeStdKeyYes_KeyUp && (aEvent.Code() == EEventKeyUp ))
		{
		iConsumeStdKeyYes_KeyUp = EFalse; // in case call button was consumed elsewhere first key up enables calling Call application
		if ( EStdKeyYes == scanCode)
			{
			  result = ETrue; // consume not to switch to Call application when call to contact was processed
			  return result;
			}
		}

    if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
        {
        TBool shiftState = ( aEvent.KeyEvent().iModifiers & EModifierShift );

        // Do the (un)marking if in shift state and suitable key is received
        if ( shiftState )
            {
            switch ( aEvent.KeyEvent().iScanCode )
                {
                case EStdKeyDownArrow:
                case EStdKeyUpArrow:
                    {
                    DoScrollMarkUnmarkL();
                    result = EFalse; // event is not consumed yet, because it must also move the cursor
                    }
                    break;
                case EStdKeyEnter:
                case EStdKeyDevice3:
                    {
                    if ( !aEvent.KeyEvent().iRepeats ) // no repeated (un)marking by holding selection key depressed
                        {
                        // The selection key always toggles the marking state of the current item.
                        // It also toggles the scroll marking/unmarking state.
                        if ( iModel->Count() )
                            {
                            CFSEmailUiMailListModelItem* item =
                                static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );

                            if ( item && item->ModelItemType() == ETypeMailItem )  // Separators are not markable
                                {
                                if ( iMailList->IsMarked( iMailList->FocusedItem() ) )
                                    {
                                    iMailList->MarkItemL( iMailList->FocusedItem(), EFalse );
                                    iListMarkItemsState = EFalse;
                                    }
                                else
                                    {
                                    iMailList->MarkItemL( iMailList->FocusedItem(), ETrue );
                                    iListMarkItemsState = ETrue;
                                    }
                                }
                            }
                        }
                    result = ETrue; // shift + selection is always consumed
                    }
                    break;
                default:
                    break;
                }

            }

        // If event not handled by now
        if ( !result )
            {
            // Handle possible focus visibility change
            if ( ( scanCode == EStdKeyRightArrow ) ||
                 ( scanCode == EStdKeyLeftArrow ) ||
                 ( scanCode == EStdKeyUpArrow ) ||
                 ( scanCode == EStdKeyDownArrow ) ||
                 ( scanCode == EStdKeyEnter ) ||
                 ( scanCode == EStdKeyDeviceA ) ||
                 ( scanCode ==EStdKeyDevice3 ) )
                {
                // If the focus was not active already, ignore the key press
                if( !iAppUi.SetFocusVisibility( ETrue ) )
                    {
                    return ETrue;
                    }
                }

            switch ( scanCode )
                {
                case EStdKeyDevice3: // CENTER CLICK
                case EStdKeyEnter:  // ENTER EITHER SELECTS ITEM IN TOOLBAR OR OPENS MAIL
                case EStdKeyNkpEnter:
                case EAknSoftkeySelect:
                    {
                    SetMskL();
                    if ( iFocusedControl == EMailListComponent )
                        {
                        TInt modelCount( modelCount = iModel->Count() );
                        if ( modelCount ) // Safety check
                            {
                            if (iMarkingMode)
                                {
                                //Do not open mail or expand/collapse separator in marking mode
                                DoHandleListItemOpenL();
                                }
                            else
                                {
                                CFSEmailUiMailListModelItem* item =
                                        dynamic_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
                                // MAIL ITEM; OPEN MAIL
                                if ( item && item->ModelItemType() == ETypeMailItem )
                                    {
                                    CFSMailMessage* messagePtr = &item->MessagePtr();
                                    if ( messagePtr )
                                        {
                                        TIMESTAMP( "Open email selected from message list" );
                                        OpenHighlightedMailL();
                                        return EKeyWasConsumed;
                                        }
                                    }
                                // SEPARAOR ITEM; COLLAPSE / EXPAND NODE
                                else if ( item && item->ModelItemType() == ETypeSeparator )
                                    {
                                    ExpandOrCollapseL();
                                    return EKeyWasConsumed;
                                    }
                                }
                            }
                        }
                    else
                        {
                        TInt focusedButtonId( iControlBarControl->GetFocusedButton()->Id() );
                        if ( focusedButtonId == iNewEmailButtonId )
                            {
                            HandleCommandL(EFsEmailUiCmdCompose);
                            }
                        else if ( focusedButtonId == iFolderListButtonId )
                            {
                            // Set touchmanager not active for preventing getting events.
                            DisableMailList( ETrue );
                            iAppUi.ShowFolderListInPopupL( FolderId(), this, iFolderListButton );
                            }
                        else if ( focusedButtonId == iSortButtonId )
                            {
                            TFSFolderType folderType;
                            if( iMailFolder )
                                {
                                folderType = iMailFolder->GetFolderType();
                                }
                            else
                                {
                                folderType = EFSInbox;
                                }
                            // Show sort if model has data.
                            if ( iModel->Count() )
                                {
                                // Set touchmanager not active for preventing getting events.
                                DisableMailList(ETrue);
                                iAppUi.ShowSortListInPopupL( iCurrentSortCriteria, folderType, this, iSortButton );
                                }
                            else
                                {
                                // hide selector focus if popup is not opened
                                // and selection was not made via HW-keys
                                iControlBarControl->MakeSelectorVisible(
                                    IsFocusShown() );
                                }
                            }
                        return ETrue; // iControlBar->OfferEventL( aEvent );
                        }
                    }
                    break;
                case EStdKeyLeftArrow:
                case EStdKeyRightArrow:
                case EStdKeyDownArrow:
                case EStdKeyUpArrow:
                    {
                    if( Layout_Meta_Data::IsLandscapeOrientation() )
                        {
                        result = HandleArrowEventInLandscapeL( scanCode,
                                    aEvent, shiftState );
                        }
                    else
                        {
                        result = HandleArrowEventInPortraitL( scanCode,
                                    aEvent, shiftState );
                        }
                    break;
                    }
                case EStdKeyYes:
                    {
                    if ( !iAppUi.ViewSwitchingOngoing() )
                        {
                        if ( iMailFolder && iMailFolder->GetFolderType() != EFSOutbox &&
                                iMailFolder->GetFolderType() != EFSDraftsFolder )
                            {
                            TInt modelCount = iModel->Count();
                            if ( modelCount ) // Safety check
                                {
                                CFSEmailUiMailListModelItem* item =
                                    dynamic_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
                                if ( item && item->ModelItemType() == ETypeMailItem )
                                    {
                                    CFSMailAddress* fromAddress = item->MessagePtr().GetSender();
                                    TDesC* mailAddress(0);
                                    if ( fromAddress )
                                        {
                                        mailAddress = &fromAddress->GetEmailAddress();
                                        }
                                    if ( mailAddress && mailAddress->Length() )
                                        {
                                        if ( iMailFolder->GetFolderType() == EFSSentFolder )
                                            {
                                            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL(
                                                    *mailAddress, iAppUi.GetActiveMailbox(), this, EFalse );
                                            }
                                        else
                                            {
                                            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL(
                                                    *mailAddress, iAppUi.GetActiveMailbox(), this, ETrue );
                                            }
// consume following KyUp event to prevent execution of Call application when call to contact processing
                                        iConsumeStdKeyYes_KeyUp = result = ETrue;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                case EStdKeyHash:
                    {
                    // Consume hash key events before they go to our shortcut framework. We do not want
                    // to react to those until the in the key up event. There hash key works as mark/unmark shortcut only
                    // if it hasn't been used as shift modifier.
                    result = ETrue;
                    }
                    break;
               // Backspace (C key) must be handled on Key-Down instead of Key-Up
               // ( this was the reason that deleted two emails from opened mail in viewer )
               case EStdKeyBackspace:
                   {
                   TInt shortcutCommand = iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
                                  CFSEmailUiShortcutBinding::EContextMailList );

                   if ( shortcutCommand != KErrNotFound )
                       {
                       HandleCommandL( shortcutCommand );
                       result = ETrue;
                       }
                   else result = EFalse;
                   }
                   break;
                default:
                    {
                    if ( iFocusedControl == EMailListComponent )
                        {
                        result = iMailList->TreeControl()->OfferEventL(aEvent);
                        }
                    else
                        {
                        result = EFalse;
                        }
                    }
                    break;
                }
            }
        }
    else if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKeyUp )
        {
        // Check keyboard shortcuts on key up events. These can't be checked on key event
        // because half-QWERTY keyboad has shortcuts also on shift and chr keys which do
        // no send any key events, only key down and key up.
        TInt shortcutCommand =
            iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
                CFSEmailUiShortcutBinding::EContextMailList );

        // Hash/shift key of ITU-T and half-QWERTY keyboards is an exception case to other
        // shortcuts: it is handled only if it hasn't been used as shift modifier
        if ( (scanCode == EStdKeyHash || scanCode == EStdKeyLeftShift || scanCode == EStdKeyRightShift) &&
             iOtherKeyPressedWhileShiftDepressed )
            {
            shortcutCommand = KErrNotFound;
            }

        // block Backspace (C key) handle on Key-Up
        if ( scanCode == EStdKeyBackspace )
            {
            shortcutCommand = KErrNotFound; // handled on Key-Down, see above
            }

        if ( shortcutCommand != KErrNotFound )
            {
            HandleCommandL( shortcutCommand );
            result = ETrue;
            }
        else
            {
            result = EFalse;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::DoHandleListItemOpenL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DoHandleListItemOpenL()
    {
    FUNC_LOG;
    if ( 0 < iModel->Count() ) // Safety check
        {
        CFSEmailUiMailListModelItem* item =
            dynamic_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
        if ( iMarkingMode )
			{
            // In marking mode. Mark/unmark it instead of opening it.
			if ( item && item->ModelItemType() == ETypeMailItem )
				{
				if (iMailList->IsMarked( iMailList->FocusedItem() ))
                    {
				    iMailList->MarkItemL( iMailList->FocusedItem(), EFalse );
                    }
				else
				    {
                    iMailList->MarkItemL( iMailList->FocusedItem(), ETrue );
				    }
				}
			else if ( item && item->ModelItemType() == ETypeSeparator )
			    {
                TInt index = HighlightedIndex();
                if ( AreAllItemsMarkedUnderSeparatorL( index ) )
                    {
                    MarkItemsUnderSeparatorL( EFalse, index );
                    }
                else
                    {
                    MarkItemsUnderSeparatorL( ETrue, index );
                    }
			    }
			}
        else if ( iAppUi.CurrentActiveView() == this )
            {

			// MAIL ITEM; OPEN MAIL
			if ( item && item->ModelItemType() == ETypeMailItem )
				{
				CFSMailMessage* messagePtr = &item->MessagePtr();
				if ( messagePtr )
					{
					TIMESTAMP( "Open email selected from message list" );
					OpenHighlightedMailL();
					}

				// Give feedback to user (vibration)
				iTouchFeedBack->InstantFeedback(ETouchFeedbackBasic);
				}
			// SEPARATOR ITEM; COLLAPSE / EXPAND NODE
			else if ( item && item->ModelItemType() == ETypeSeparator )
				{
					ExpandOrCollapseL();
					SetMskL();
				}
			}
        }
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::DoHandleControlBarOpenL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DoHandleControlBarOpenL( TInt aControlBarButtonId )
    {
    FUNC_LOG;

    if ( aControlBarButtonId == iNewEmailButtonId )
        {
        HandleCommandL(EFsEmailUiCmdCompose);
        }
    else if ( aControlBarButtonId == iFolderListButtonId )
        {
        //FOLDERLIST
        //Set touchmanager not active for preventing getting events.
        DisableMailList(ETrue);
        iAppUi.ShowFolderListInPopupL( FolderId(), this, iFolderListButton );
        }
    else if ( aControlBarButtonId == iSortButtonId )
        {
        //SORTLIST
        TFSFolderType folderType;
        if ( iMailFolder )
            {
            folderType = iMailFolder->GetFolderType();
            }
        else
            {
            folderType = EFSInbox;
            }
        // we can't show sort list when sorting is active
        if ( iModel->Count() && iSortState == ESortNone )
            {
            //Set touchmanager not active for preventing getting events.
            DisableMailList(ETrue);
            iAppUi.ShowSortListInPopupL( iCurrentSortCriteria, folderType, this, iSortButton );
            }
        else
            {
            // hide selector focus if popup is not opened and selection was not
            // made via HW-keys
            iControlBarControl->MakeSelectorVisible(  IsFocusShown() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::DoHandleListItemLongTapL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DoHandleListItemLongTapL( const TPoint& aPoint )
    {
    FUNC_LOG;

    if ( 0 < iModel->Count() ) // Safety check
        {
        if ( iMarkingMode )
			{
            iMailTreeListVisualizer->SetFocusVisibility( EFalse );
            TInt currentFocusState = iMailList->IsFocused();
            iMailList->SetFocusedL( EFalse );
            iMailList->SetFocusedL( currentFocusState );
			CFSEmailUiMailListModelItem* item =
                dynamic_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
            // In marking mode. Mark/unmark it instead of open popup.
			if ( item && item->ModelItemType() == ETypeMailItem )
				{
				if (iMailList->IsMarked( iMailList->FocusedItem() ))
                    {
				    iMailList->MarkItemL( iMailList->FocusedItem(), EFalse );
                    }
				else
				    {
                    iMailList->MarkItemL( iMailList->FocusedItem(), ETrue );
				    }
				}
			else if ( item && item->ModelItemType() == ETypeSeparator )
			    {
                TInt index = HighlightedIndex();
                if ( AreAllItemsMarkedUnderSeparatorL( index ) )
                    {
                    MarkItemsUnderSeparatorL( EFalse, index );
                    }
                else
                    {
                    MarkItemsUnderSeparatorL( ETrue, index );
                    }
			    }
			}
		else
		    {
            LaunchStylusPopupMenuL( aPoint );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::GetFocusedControl
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::GetFocusedControl() const
    {
    FUNC_LOG;
    return iFocusedControl;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::SetControlBarFocusedL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetControlBarFocusedL()
    {
    FUNC_LOG;
    iFocusedControl = EControlBarComponent;
    iMailList->SetFocusedL( EFalse );
    SetMskL();
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::SetTreeListFocusedL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetTreeListFocusedL()
    {
    FUNC_LOG;
    iFocusedControl = EMailListComponent;
    iMailList->SetFocusedL( ETrue );
    iControlBarControl->SetFocusL( EFalse );
    SetMskL();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::FlipStateChangedL
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::FlipStateChangedL( TBool aKeyboardFlipOpen )
    {
    FUNC_LOG;
    CFsEmailUiViewBase::FlipStateChangedL( aKeyboardFlipOpen );
    iMailTreeListVisualizer->SetFlipState( iKeyboardFlipOpen );
    }

// -----------------------------------------------------------------------------
//  CFSEmailUiMailListVisualiser::HandleTimerFocusStateChange
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::FocusVisibilityChange( TBool aVisible )
    {
    FUNC_LOG;

    CFsEmailUiViewBase::FocusVisibilityChange( aVisible );

    if( iFocusedControl == EControlBarComponent )
        {
        iControlBarControl->MakeSelectorVisible( aVisible );
        }

    if ( iStylusPopUpMenuVisible && !aVisible )
    	{
    	// Do not allow to remove the focus from a list element if the pop up
    	// menu was just launched.
    	return;
    	}

    iMailTreeListVisualizer->SetFocusVisibility( aVisible );
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::DisableMailList
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DisableMailList( TBool aValue )
    {
    FUNC_LOG;
    iTouchManager->SetDisabled( aValue );
    iControlBarControl->EnableTouch( (aValue) ? EFalse : ETrue  );
    }
//</cmail>

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ShortcutCollapseExpandAllToggleL()
	{
    FUNC_LOG;
	if ( !AllNodesCollapsed() )
		{
		CollapseAllNodesL();
		}
	else
		{
		ExpandAllNodesL();
		}
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::AllNodesCollapsed() const
    {
    FUNC_LOG;
	for ( TInt i=0;i<iTreeItemArray.Count();i++)
		{
		if ( iMailList->IsNode( iTreeItemArray[i].iListItemId ) )
			{
			if ( iMailList->IsExpanded( iTreeItemArray[i].iListItemId ) )
				{
				return EFalse;
				}
			}
		}

	return ETrue;
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::AllNodesExpanded() const
    {
    FUNC_LOG;
	for ( TInt i=0;i<iTreeItemArray.Count();i++)
		{
		if ( iMailList->IsNode( iTreeItemArray[i].iListItemId ) )
			{
			if ( !iMailList->IsExpanded( iTreeItemArray[i].iListItemId ) )
				{
				return EFalse;
				}
			}
		}

	return ETrue;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::IsMarkAsReadAvailableL()
// Function checks if mark as read option should be available for user
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::IsMarkAsReadAvailableL() const
    {
    FUNC_LOG;
    TBool available = EFalse;

    // In outbox the mark as read is always inavailable
    if ( iMailFolder && iMailFolder->GetFolderType() != EFSOutbox )
        {
        RFsTreeItemIdList targetEntries;
        CleanupClosePushL( targetEntries );
        GetActionsTargetEntriesL( targetEntries );

        // Mark as read is available if at least one of the target entries is unread
        for ( TInt i = 0 ; i < targetEntries.Count() ; ++i )
            {
            const CFSMailMessage& message = MsgPtrFromListIdL( targetEntries[i] );
            if ( !message.IsFlagSet(EFSMsgFlag_Read) )
                {
                available = ETrue;
                break;
                }
            }

        CleanupStack::PopAndDestroy( &targetEntries );
        }

    return available;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::IsMarkAsUnreadAvailableL()
// Function checks if mark as unread option should be available for user
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::IsMarkAsUnreadAvailableL() const
    {
    FUNC_LOG;
    TBool available = EFalse;

    // In outbox the mark as unread is always inavailable
    if ( iMailFolder && iMailFolder->GetFolderType() != EFSOutbox )
        {
        RFsTreeItemIdList targetEntries;
        CleanupClosePushL( targetEntries );
        GetActionsTargetEntriesL( targetEntries );

        // Mark as unread is available if at least one of the target entries is read
        for ( TInt i = 0 ; i < targetEntries.Count() ; ++i )
            {
            const CFSMailMessage& message = MsgPtrFromListIdL( targetEntries[i] );
            if ( message.IsFlagSet(EFSMsgFlag_Read) )
                {
                available = ETrue;
                break;
                }
            }

        CleanupStack::PopAndDestroy( &targetEntries );
        }

    return available;
    }

// ---------------------------------------------------------------------------
// Utility function to get list of entries which will be targeted by the
// Actions menu commands. The list contains either marked entries or the
// focused message entry or is empty.
// ---------------------------------------------------------------------------
void CFSEmailUiMailListVisualiser::GetActionsTargetEntriesL( RFsTreeItemIdList& aListItems ) const
    {
    FUNC_LOG;
    if ( iMailList )
        {
        iMailList->GetMarkedItemsL( aListItems );

        // If there are no marked entries the command target is the focused item.
        if ( !aListItems.Count() )
            {
            TFsTreeItemId focusedId = iMailList->FocusedItem();
            if ( iFocusedControl == EMailListComponent &&
                 focusedId != KFsTreeNoneID &&
                 !iMailList->IsNode( focusedId ) )
                {
                aListItems.AppendL( focusedId );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ShortcutReadUnreadToggleL()
    {
    FUNC_LOG;
    // Setting the read status is not possible in outbox
    // <cmail>
    if ( iModel->Count() && iMailFolder && iMailFolder->GetFolderType() != EFSOutbox )
    // </cmail>
        {
        RFsTreeItemIdList targetEntries;
        CleanupClosePushL( targetEntries );
        GetActionsTargetEntriesL( targetEntries );
        TInt targetCount = targetEntries.Count();

        // figure out are we marking items as read or unread
        TBool toggleAsRead(ETrue);
        for ( TInt i=0 ; i<targetCount ; i++ )
            {
            CFSMailMessage& msgPtr = MsgPtrFromListIdL( targetEntries[i] );
            if ( msgPtr.IsFlagSet(EFSMsgFlag_Read) )
                {
                toggleAsRead = EFalse;
                break;
                }
            }

        // change the status of the target items
        for ( TInt i=0 ; i<targetCount ; i++ )
            {
            TInt msgIndex = ModelIndexFromListId( targetEntries[i] );
            ChangeReadStatusOfIndexL( toggleAsRead, msgIndex );
            }

        CleanupStack::PopAndDestroy( &targetEntries );
        }
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleControlBarEvent( TFsControlBarEvent aEvent, TInt aData )
	{
    FUNC_LOG;

    switch( aEvent )
        {
        case EEventFocusLostAtBottom:
        case EEventFocusLostAtSide:
            {
            iFocusedControl = EMailListComponent;
            TRAP_IGNORE( iMailList->SetFocusedL( ETrue ) );
            break;
            }
        case EEventFocusGained:
            {
            break;
            }
        case EEventButtonPressed:
            {
            // Handle 2 control buttons
            if ( aData == iFolderListButtonId )
                {
                }
            else if ( aData == iSortButtonId )
                {
                }
            break;
            }
        case EEventFocusVisibilityChanged:
            {
            // Hide focus after button release
            iAppUi.SetFocusVisibility( EFalse );
            break;
            }
        default:
            {
            // No need to handle other events
            break;
            }
        }
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::CreateControlBarLayoutL()
	{
    FUNC_LOG;
    iControlBarControl->SetRectL( iAppUi.LayoutHandler()->GetControlBarRect() );

    // New email button
    iNewEmailButtonId = iControlBarControl->AddButtonL( ECBTypeOneLineLabelIconA );
    iNewEmailButton = iControlBarControl->ButtonById( iNewEmailButtonId );
    const TRect mailButtonRect( iAppUi.LayoutHandler()->GetControlBarNewEmailButtonRect() );
    iNewEmailButton->SetPos( mailButtonRect.iTl );
    iNewEmailButton->SetSize( mailButtonRect.Size() );
    ControlGroup().AppendL(iNewEmailButton->AsAlfControl());

    // Folder list button
    iFolderListButtonId = iControlBarControl->AddButtonL( ECBTypeOneLineLabelIconA );
    iFolderListButton = iControlBarControl->ButtonById( iFolderListButtonId );
    const TRect folderButtonRect( iAppUi.LayoutHandler()->GetControlBarFolderListButtonRect() );
    iFolderListButton->SetPos( folderButtonRect.iTl );
    iFolderListButton->SetSize( folderButtonRect.Size() );
    ControlGroup().AppendL(iFolderListButton->AsAlfControl());

    // Sort order button
    iSortButtonId = iControlBarControl->AddButtonL( ECBTypeOneLineLabelIconA );
    iSortButton = iControlBarControl->ButtonById( iSortButtonId );
    const TRect sortButtonRect( iAppUi.LayoutHandler()->GetControlBarSortButtonRect() );
    iSortButton->SetPos( sortButtonRect.iTl );
    iSortButton->SetSize( sortButtonRect.Size() );
    ControlGroup().AppendL(iSortButton->AsAlfControl());
// </cmail>

    // Set the text alignment according the layout
    TAlfAlignHorizontal horizontalAlign = EAlfAlignHLeft;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        horizontalAlign = EAlfAlignHRight;
        }
    // Icons and sort button text
    iFolderListButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( EListControlBarMailboxDefaultIcon ) );
    iNewEmailButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( EListTextureCreateNewMessageIcon ) );
    SetSortButtonIconL();

    iNewEmailButton->SetElemAlignL(
        ECBElemIconA,
        EAlfAlignHCenter,
        EAlfAlignVCenter );

    iFolderListButton->SetElemAlignL(
        ECBElemLabelFirstLine,
        horizontalAlign,
        EAlfAlignVCenter );
    iSortButton->SetElemAlignL(
        ECBElemLabelFirstLine,
        horizontalAlign,
        EAlfAlignVCenter );
    iSortButton->SetElemAlignL(
        ECBElemIconA,
        EAlfAlignHCenter,
        EAlfAlignVCenter );

	// Show the buttons
  	iNewEmailButton->ShowButtonL();
    iFolderListButton->ShowButtonL();
    iSortButton->ShowButtonL();
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ScaleControlBarL()
	{
    FUNC_LOG;
    
    if( !Layout_Meta_Data::IsLandscapeOrientation() && iMarkingMode)
        {
        // No control bar buttons in portrait marking mode
        return;    
        }    
    
    TRect screenRect = iAppUi.ClientRect();

	// First set pos, widht and height
  	iControlBarControl->SetRectL( iAppUi.LayoutHandler()->GetControlBarRect() );

    const TRect newEmailButtonRect( iAppUi.LayoutHandler()->GetControlBarNewEmailButtonRect() );
	//Add comments by luo gen(e0645320)
	//The position of button should be set according to the size of button, so set size first
    iNewEmailButton->SetSize( newEmailButtonRect.Size() );
    iNewEmailButton->SetPos( newEmailButtonRect.iTl );

    const TRect folderButtonRect( iAppUi.LayoutHandler()->GetControlBarFolderListButtonRect() );
    iFolderListButton->SetSize( folderButtonRect.Size() );
  	iFolderListButton->SetPos( folderButtonRect.iTl );

    const TRect sortButtonRect( iAppUi.LayoutHandler()->GetControlBarSortButtonRect() );
    iSortButton->SetSize( sortButtonRect.Size() );
   	iSortButton->SetPos( sortButtonRect.iTl );

	TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
	TAknLayoutText textLayout;
	if ( Layout_Meta_Data::IsLandscapeOrientation() )
		{
	    textLayout.LayoutText(TRect(0,0,0,0), AknLayoutScalable_Apps::cmail_ddmenu_btn02_pane_t2(0));
		}
	else
		{
	    textLayout.LayoutText(TRect(0,0,0,0), AknLayoutScalable_Apps::main_sp_fs_ctrlbar_ddmenu_pane_t1(var));
		}
	iNewEmailButton->SetTextFontL( textLayout.Font()->FontSpecInTwips() );
 	iFolderListButton->SetTextFontL( textLayout.Font()->FontSpecInTwips() );
 	iSortButton->SetTextFontL( textLayout.Font()->FontSpecInTwips() );

 	UpdateThemeL(EFalse);

    iNewEmailButton->ShowButtonL();
    iFolderListButton->ShowButtonL();
    iSortButton->ShowButtonL();
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetSortButtonIconL()
	{
    FUNC_LOG;
	iSortButton->SetIconL(
			iAppUi.FsTextureManager()->TextureByIndex( GetSortButtonTextureIndex() ),
			ECBElemIconA );

	}
    
// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
TFSEmailUiTextures CFSEmailUiMailListVisualiser::GetSortButtonTextureIndex()
    {
    FUNC_LOG;
    
    TFSEmailUiTextures textureIndex( ETextureFirst );
    switch ( iCurrentSortCriteria.iField )
		{
		case EFSMailSortBySubject:
			{
			textureIndex = iCurrentSortCriteria.iOrder == EFSMailAscending ?
						   ESortListSubjectDescTexture :
						   ESortListSubjectAscTexture;
			}
			break;
		case EFSMailSortByAttachment:
			{
			textureIndex = iCurrentSortCriteria.iOrder == EFSMailAscending ?
						   ESortListAttachmentDescTexture :
						   ESortListAttachmentAscTexture;
			}
			break;
		case EFSMailSortByFlagStatus:
			{
			textureIndex = iCurrentSortCriteria.iOrder == EFSMailAscending ?
						   ESortListFollowDescTexture :
						   ESortListFollowAscTexture;
			}
			break;
		case EFSMailSortByRecipient:
		case EFSMailSortBySender:
			{
			textureIndex = iCurrentSortCriteria.iOrder == EFSMailAscending ?
						   ESortListSenderDescTexture :
						   ESortListSenderAscTexture;
			}
			break;
		case EFSMailSortByPriority:
			{
			textureIndex = iCurrentSortCriteria.iOrder == EFSMailAscending ?
						   ESortListPriorityDescTexture :
						   ESortListPriorityAscTexture;
			}
			break;
		case EFSMailSortByUnread:
			{
			textureIndex = iCurrentSortCriteria.iOrder == EFSMailAscending ?
						   ESortListUnreadDescTexture :
						   ESortListUnreadAscTexture;
			}
			break;
		case EFSMailSortByDate:
		default:
			{
			textureIndex = iCurrentSortCriteria.iOrder == EFSMailAscending ?
						   ESortListDateDescTexture :
						   ESortListDateAscTexture;
			}
			break;
  		}
    return textureIndex;
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
HBufC* CFSEmailUiMailListVisualiser::GetSortButtonTextLC()
	{
    FUNC_LOG;
	HBufC* buttonText( 0 );

	switch ( iCurrentSortCriteria.iField )
		{
		case EFSMailSortBySubject:
			{
			buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_SUBJECT );
			}
			break;
		case EFSMailSortByAttachment:
			{
			buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_ATTACHMENT );
			}
			break;
		case EFSMailSortByFlagStatus:
			{
			buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_FLAG );
			}
			break;
		case EFSMailSortByRecipient:
		case EFSMailSortBySender:
			{
			TFSFolderType folderType( EFSInbox );
            if( iMailFolder )
               {
               folderType = iMailFolder->GetFolderType();
               }
			switch( folderType )
				{
				case EFSSentFolder:
				case EFSDraftsFolder:
				case EFSOutbox:
					{
					buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_RECIPIENT );
					}
					break;
				default:
					buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_SENDER );
					break;
				}
			}
			break;
		case EFSMailSortByPriority:
			{
			buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_PRIORITY );
			}
			break;
		case EFSMailSortByUnread:
			{
			buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_UNREAD );
			}
			break;
		case EFSMailSortByDate:
		default:
			{
			buttonText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SORT_BY_DATE );
			}
			break;
		}
	return buttonText;
	}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SetMailListLayoutAnchors()
	{
    FUNC_LOG;
 	// Set anchors so that list leaves space for control bar
    iScreenAnchorLayout->SetAnchor(EAlfAnchorTopLeft, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint(0, 0 ));
    iScreenAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(1, iAppUi.LayoutHandler()->ControlBarHeight() ));

    TRect listRect = iAppUi.LayoutHandler()->GetListRect();
	// Set anchors so that list leaves space for control bar
    if( Layout_Meta_Data::IsMirrored() ) 
        { 
        TInt tlX = listRect.iTl.iX; 
        TInt brX = listRect.iBr.iX;
        
        listRect.iTl.iX = AknLayoutScalable_Avkon::Screen().LayoutLine().iW - brX; 
        listRect.iBr.iX = AknLayoutScalable_Avkon::Screen().LayoutLine().iW - tlX; 
        } 
    iScreenAnchorLayout->SetAnchor(EAlfAnchorTopLeft, 1,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(listRect.iTl.iX, listRect.iTl.iY));
    
    if (iMarkingMode && Layout_Meta_Data::IsLandscapeOrientation())
        {
        iScreenAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 1,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricAbsolute,
                TAlfTimedPoint(1, listRect.iBr.iY));
        }
    else
        {
        iScreenAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 1,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                TAlfTimedPoint(listRect.iBr.iX, listRect.iBr.iY));    
        }

 	// Set anchors for connection icon
    TRect connectionIconRect( iAppUi.LayoutHandler()->GetControlBarConnectionIconRect() );
    const TPoint& tl( connectionIconRect.iTl );
 	iScreenAnchorLayout->SetAnchor(EAlfAnchorTopLeft, 2,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( tl.iX, tl.iY ));
    const TPoint& br( connectionIconRect.iBr );
    iScreenAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 2,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( br.iX, br.iY ));

    iScreenAnchorLayout->UpdateChildrenLayout();
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ChangeReadStatusOfHighlightedL( TBool aRead )
	{
    FUNC_LOG;
	ChangeReadStatusOfIndexL( aRead, HighlightedIndex() );
	}

// ---------------------------------------------------------------------------
// ChangeReadStatusOfMarkedL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ChangeReadStatusOfMarkedL( TBool aRead )
	{
    FUNC_LOG;
	RFsTreeItemIdList markedEntries;
	CleanupClosePushL( markedEntries );
	iMailList->GetMarkedItemsL( markedEntries );
	for ( TInt i=0; i<markedEntries.Count();i++ )
		{
		TInt msgIndex = ModelIndexFromListId( markedEntries[i] );
		ChangeReadStatusOfIndexL( aRead, msgIndex );
		}
	UnmarkAllItemsL();
	CleanupStack::PopAndDestroy( &markedEntries );
	}

void CFSEmailUiMailListVisualiser::ChangeReadStatusOfIndexL( TBool aRead, TInt aIndex )
    {
    FUNC_LOG;
    // <cmail>
    if ( iModel->Count() )
    // </cmail>
        {
        CFSEmailUiMailListModelItem* selectedItem =
            static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( aIndex ));
        if ( selectedItem ) // Safety
            {
            CFSMailMessage& msgPtr = selectedItem->MessagePtr();
            TBool msgWasReadBefore = msgPtr.IsFlagSet( EFSMsgFlag_Read );
            if ( aRead != msgWasReadBefore )
                {
                if ( aRead )
                    {
                    // Send flags, local and server
                    msgPtr.SetFlag( EFSMsgFlag_Read );
                    }
                else
                    {
                    // Send flags, local and server
                    msgPtr.ResetFlag( EFSMsgFlag_Read );
                    }
                msgPtr.SaveMessageL();  // Save flag

                // Switch icon to correct one if mail list is visible
                TBool needRefresh = ( iAppUi.CurrentActiveView()->Id() == MailListId );
                UpdateMsgIconAndBoldingL( aIndex, needRefresh);

                if ( iCurrentSortCriteria.iField == EFSMailSortByUnread )
                    {
                    // Attribute affecting the current sorting order has been changed.
                    // Thus, the list order may now be incorrect.
                    iListOrderMayBeOutOfDate = ETrue;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// DeleteMessagesL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DeleteMessagesL()
	{
    FUNC_LOG;
    const TInt markedCount(CountMarkedItemsL());
    const TFsTreeItemId focusedId(iMailList->FocusedItem());

    // Delete either marked items or the focused one
    if (markedCount)
        {
        DeleteMarkedMessagesL();
        }
    else if (iMailList->IsNode(focusedId))
        {
        DeleteMessagesUnderNodeL(focusedId);
        }
    else
        {
        DeleteFocusedMessageL();
        }

	// Set highlight to control bar if no items after delete
    // <cmail>
	if ( iModel->Count() == 0 )
    // </cmail>
		{
		iFocusedControl = EControlBarComponent;
		iMailList->SetFocusedL( EFalse );
		iControlBarControl->SetFocusL();
		}
	}

// ---------------------------------------------------------------------------
// UpdateItemAtIndexL
// Reload message pointer from mail client and update list item contents to
// match it. Item is removed if it isn't valid anymore.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateItemAtIndexL( TInt aIndex )
    {
    FUNC_LOG;
    // <cmail>

    if ( aIndex >= 0 && aIndex < iModel->Count() )
    // </cmail>
        {
        CFSEmailUiMailListModelItem* modelItem =
            static_cast<CFSEmailUiMailListModelItem*>( Model()->Item( aIndex ) );
        if ( modelItem && modelItem->ModelItemType() == ETypeMailItem )
            {
            // This is beacause message deleted event migh have occured.
            CFSMailMessage* confirmedMsgPtr = NULL;
            TRAP_IGNORE( confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL( iAppUi.GetActiveMailboxId(),
                                                                                     iMailFolder->GetFolderId(),
                                                                                     modelItem->MessagePtr().GetMessageId() ,
                                                                                     EFSMsgDataEnvelope ) );
            if( aIndex < iTreeItemArray.Count() )
                {
                if ( confirmedMsgPtr )
                    {
                    const SMailListItem& item = iTreeItemArray[aIndex];
                    // Replace message pointer in model with newly fetched one
                    Model()->ReplaceMessagePtr( aIndex, confirmedMsgPtr );

                    // Update the list item contents and formating to match the message pointer
                    CFsTreePlainTwoLineItemData* itemData =
                        static_cast<CFsTreePlainTwoLineItemData*>( item.iTreeItemData );
                    CFsTreePlainTwoLineItemVisualizer* itemVis =
                        static_cast<CFsTreePlainTwoLineItemVisualizer*>( item.iTreeItemVisualiser );

                    UpdateItemDataL( itemData, confirmedMsgPtr );
                    UpdatePreviewPaneTextForItemL( itemData, confirmedMsgPtr );
                    UpdateMsgIconAndBoldingL( itemData, itemVis, confirmedMsgPtr );
                    iMailTreeListVisualizer->UpdateItemL( item.iListItemId );
                    }
                else
                    {
                    // No confirmed message for highlighted, remove from list also
                    iMailList->RemoveL( iTreeItemArray[aIndex].iListItemId ); // remove from list
                    iTreeItemArray.Remove( aIndex ); // remove from internal array.
                    iModel->RemoveAndDestroy( aIndex ); // Remove from model
                    if ( iNodesInUse )
                        {
                        RemoveUnnecessaryNodesL();
                        }
                    }
            	}
            }
        }
    }

// ---------------------------------------------------------------------------
// CheckValidityOfHighlightedMsgL
// Check validity of highlighted msg, and remove from list if needed
// Typically called after view is returned from editot or viewer.
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::CheckValidityOfHighlightedMsgL()
	{
    FUNC_LOG;
	TBool ret(EFalse);
    // <cmail>
	if ( iModel->Count() )
    // </cmail>
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>( Model()->Item( HighlightedIndex() ) );
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			// This is beacause message deleted event migh have occured.
			CFSMailMessage* confirmedMsgPtr = NULL;
			TRAP_IGNORE( confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL( iAppUi.GetActiveMailboxId(),
								                                                     iMailFolder->GetFolderId(),
								                                                     item->MessagePtr().GetMessageId() ,
								                                                     EFSMsgDataEnvelope ) );
			if ( confirmedMsgPtr )
				{
			   	ret = ETrue;
				delete confirmedMsgPtr;
				confirmedMsgPtr = NULL;
				}
			else
				{
				// No confirmed message for highlighted, remove from list also
				ret = EFalse;
				SMailListItem item;
				item.iListItemId = iMailList->FocusedItem();
				TInt IndexToBeDestroyed = iTreeItemArray.Find( item );
				iTreeItemArray.Remove( IndexToBeDestroyed ); // remove from internal array.
		 		iModel->RemoveAndDestroy( IndexToBeDestroyed ); // Remove from model
				iMailList->RemoveL( iMailList->FocusedItem() );		// remove from list
				if ( iNodesInUse )
					{
					RemoveUnnecessaryNodesL();
					}
				}
			}
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// UpdateMsgIconAndBoldingL
// Updates our own message object with the data from a given message object.
// The messages are matched with the message ID.
// ---------------------------------------------------------------------------
void CFSEmailUiMailListVisualiser::UpdateMsgIconAndBoldingL( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
    // <cmail>
	if ( aMsgPtr && iModel->Count() )
    // </cmail>
		{
		TInt mailItemIdx = ItemIndexFromMessageId( aMsgPtr->GetMessageId() );
		if ( mailItemIdx >= 0 )
			{
			CFSEmailUiMailListModelItem* item =
				static_cast<CFSEmailUiMailListModelItem*>( Model()->Item( mailItemIdx ) );

            if ( item ) // For safety
                {
                // Update all flags
                TUint32 prevFlags = item->MessagePtr().GetFlags();
                TUint32 newFlags = aMsgPtr->GetFlags();
                if ( prevFlags != newFlags )
                    {
                    item->MessagePtr().ResetFlag( prevFlags );
                    item->MessagePtr().SetFlag( newFlags );

                    // Save changed flags in internal model array
                    item->MessagePtr().SaveMessageL();
                    }

                // Update the list item graphics
                UpdateMsgIconAndBoldingL( mailItemIdx );
                }
            }
		}
	}

// ---------------------------------------------------------------------------
// UpdateMsgIconAndBoldingL
// Updates list item at given index to match the state of the message object
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateMsgIconAndBoldingL( TInt aListIndex, TBool aRefreshItem )
    {
    FUNC_LOG;
    CFSEmailUiMailListModelItem* item =
        static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(aListIndex) );
    if ( item && item->ModelItemType() == ETypeMailItem )
        {
        CFSMailMessage* msgPtr = &item->MessagePtr();
        CFsTreePlainTwoLineItemData* itemData =
            static_cast<CFsTreePlainTwoLineItemData*>( iTreeItemArray[aListIndex].iTreeItemData );
        CFsTreePlainTwoLineItemVisualizer* itemVis =
            static_cast<CFsTreePlainTwoLineItemVisualizer*>( iTreeItemArray[aListIndex].iTreeItemVisualiser );

        UpdateMsgIconAndBoldingL( itemData, itemVis, msgPtr );

        //refresh item if requested
        if ( aRefreshItem )
            {
            iMailTreeListVisualizer->UpdateItemL( iTreeItemArray[aListIndex].iListItemId );
            }
        }
    }

// ---------------------------------------------------------------------------
// UpdateMsgIconAndBoldingL
// Updates list item at given index to match the state of the message object
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateMsgIconAndBoldingL( CFsTreePlainTwoLineItemData* aItemData,
                                                             CFsTreePlainTwoLineItemVisualizer* aItemVis,
                                                             CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;

    if ( aItemData && aItemVis && aMsgPtr )
        {
        CAlfTexture* itemTexture = &TFsEmailUiUtility::GetMsgIcon( aMsgPtr, *iAppUi.FsTextureManager() );
        aItemData->SetIcon( *itemTexture );
        TInt variety( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );
        CAlfTextStyle& textStyle =
            iAppUi.LayoutHandler()->FSTextStyleFromLayoutL(
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t1( variety ) );
        const TBool isNotRead( !aMsgPtr->IsFlagSet( EFSMsgFlag_Read ) );
        textStyle.SetBold( isNotRead );
        aItemVis->SetTextBold( isNotRead );
        aItemVis->SetTextStyleId ( textStyle.Id() );
        // Set follow up flag icon
        if ( aMsgPtr->IsFlagSet( EFSMsgFlag_FollowUp ) )
            {
            aItemData->SetFlagIcon( iAppUi.FsTextureManager()->TextureByIndex( EFollowUpFlagList ) );
            aItemVis->SetFlagIconVisible( ETrue );
            }
        else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
            {
            aItemData->SetFlagIcon( iAppUi.FsTextureManager()->TextureByIndex( EFollowUpFlagCompleteList ) );
            aItemVis->SetFlagIconVisible( ETrue );
            }
        else
            {
            aItemVis->SetFlagIconVisible( EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
// RemoveMsgItemsFromListIfFoundL
// Message removing from list if found. Does not panic or return found status.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::RemoveMsgItemsFromListIfFoundL( const RArray<TFSMailMsgId>& aEntryIds )
	{
    FUNC_LOG;
    
	TBool itemFound = EFalse;
    for ( TInt i=0 ; i<aEntryIds.Count() ; ++i )
	    {
	    const TFSMailMsgId& entryId = aEntryIds[i];
    	if ( !entryId.IsNullId() )
    		{
        	TInt idx = ItemIndexFromMessageId( entryId );
        	if ( idx >= 0 )
        	    {
    			iMailList->RemoveL( iTreeItemArray[idx].iListItemId ); // remove from list
    			iTreeItemArray.Remove( idx ); // remove from internal array.
    	 		iModel->RemoveAndDestroy( idx ); // Remove from model
    	 		itemFound = ETrue;
        	    }
    		}
	    }
	
    if ( !itemFound )
        {
        return;  // no items to be removed
        }
		
	if ( iNodesInUse )
		{
		RemoveUnnecessaryNodesL();
		}

	// Set highligh to control bar if no items left after
	// deleted items have been revoved from the list
	// Otherwise mail list takes care of highlight
    // <cmail>
	if ( iModel->Count() == 0 )
    // </cmail>
		{
		iFocusedControl = EControlBarComponent;
		iMailList->SetFocusedL( EFalse );
		iControlBarControl->SetFocusL();
		}
	}

// ---------------------------------------------------------------------------
// DeleteFocusedMessageL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DeleteFocusedMessageL()
	{
    FUNC_LOG;
	TInt currentItemIndex = HighlightedIndex();
	if ( Model()->Count() )
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(currentItemIndex) );
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			CFSMailMessage& messagePtr = item->MessagePtr();
            // Delete message from framework, and perform internal housekeeping
            TFSMailMsgId msgId = messagePtr.GetMessageId();
            RArray<TFSMailMsgId> msgIds;
            CleanupClosePushL( msgIds );
            msgIds.Append( msgId );
            TFSMailMsgId folderId = FolderId();
            TFSMailMsgId mailBox = iAppUi.GetActiveMailbox()->GetId();
            iAppUi.GetMailClient()->DeleteMessagesByUidL( mailBox, folderId, msgIds );
            RemoveMsgItemsFromListIfFoundL( msgIds );
            CleanupStack::PopAndDestroy( &msgIds );
			}
		}
	}

// ---------------------------------------------------------------------------
// DeleteMessagesUnderNodeL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DeleteMessagesUnderNodeL(
        const TFsTreeItemId aNodeId)
    {
    FUNC_LOG;
    iModel->GetItemIdsUnderNodeL(aNodeId, iDeleteTask->Entries());
    ConfirmAndStartDeleteTaskL(iDeleteTask);
    }

// ---------------------------------------------------------------------------
// DeleteMarkedMessagesL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DeleteMarkedMessagesL()
    {
    FUNC_LOG;
    iMailList->GetMarkedItemsL(iDeleteTask->Entries());
    ConfirmAndStartDeleteTaskL(iDeleteTask);
    }

// ---------------------------------------------------------------------------
// ConfirmAndStartDeleteTaskL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::ConfirmAndStartDeleteTaskL(
        TDeleteTask<CFSEmailUiMailListVisualiser>* aTask )
    {
    FUNC_LOG;
    const RFsTreeItemIdList& entries(aTask->Entries());
    if (entries.Count())
        {
        if (ConfirmDeleteL(entries.Count(), entries[0]))
            {
            ExitMarkingModeL();
            if (entries.Count() > KMsgDeletionWaitNoteAmount)
                {
                TFsEmailUiUtility::ShowWaitNoteL(iDeletingWaitNote,
                        R_FSE_WAIT_DELETING_TEXT, EFalse, ETrue);
                }
            if (iAsyncCallback)
                {
                // Call actual deletion asynchronously because we must give wait
                // note time to show up before deletion begins.
                iAsyncCallback->Cancel();
                iAsyncCallback->Set(TCallBack(DoExecuteDeleteTask, aTask));
                iAsyncCallback->CallBack();
                }
            }
        else
            {
            aTask->Reset();
            }
        }
    }

// ---------------------------------------------------------------------------
// DoExecuteDeleteTask
//
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::DoExecuteDeleteTask( TAny* aSelfPtr )
    {
    FUNC_LOG;
    TRAPD( error, 
            reinterpret_cast<TDeleteTask<CFSEmailUiMailListVisualiser>*>(aSelfPtr)->ExecuteL() );
    return error;
    }

// ---------------------------------------------------------------------------
// HandleDeleteTaskL
// This is called asynchronously by ConfirmAndStartDeleteTaskL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleDeleteTaskL( const RFsTreeItemIdList& aEntries )
    {
    FUNC_LOG;
    // Close wait note if it was used
    if ( iDeletingWaitNote )
        {
        TRAPD( result, HandleDeleteTaskLeavingCodeL(aEntries) );
        // closing the "Deleting" dialog message should not be skipped by leaving
        TRAP_IGNORE( iDeletingWaitNote->ProcessFinishedL() ); 
        if ( KErrNone != result )
            {
            // Handle error.
            User::Leave( result );
            }
        }
    else
        {
        HandleDeleteTaskLeavingCodeL( aEntries );
        }
    }

// ---------------------------------------------------------------------------
// original code HandleDeleteTaskL which may leave - help function to enable 
// calling iDeletingWaitNote->ProcessFinishedL() 
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleDeleteTaskLeavingCodeL( const RFsTreeItemIdList& aEntries )
    {
    FUNC_LOG;
    TFSMailMsgId folderId = FolderId();
    TFSMailMsgId mailBox = iAppUi.GetActiveMailboxId();
    RArray<TFSMailMsgId> msgIds;
    CleanupClosePushL( msgIds );
    for ( TInt i = aEntries.Count() - 1; i >= 0; i-- )
        {
        msgIds.AppendL( MsgIdFromListId( aEntries[i] ) );
        }

    // Clear the focused item to avoid repeated selection of a new focused
    // item, unnecessary scrolling of the viewport, etc.
    iMailTreeListVisualizer->SetFocusedItemL( KFsTreeNoneID );

    iMailList->BeginUpdate();
    iAppUi.GetMailClient()->DeleteMessagesByUidL( mailBox, folderId, msgIds );
    // Remove from mail list if not already removed by mailbox events
    RemoveMsgItemsFromListIfFoundL( msgIds );
    iMailList->EndUpdateL();

    CleanupStack::PopAndDestroy(); // msgIds.Close()
    }

// ---------------------------------------------------------------------------
// ConfirmDeleteL
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailListVisualiser::ConfirmDeleteL(const TInt aItemCount,
        const TFsTreeItemId aItemId) const
    {
    FUNC_LOG;
    TBool result(ETrue);
    if (iAppUi.GetCRHandler()->WarnBeforeDelete())
        {
        HBufC* noteText(NULL);
        // The note depends on the amount and type of message(s)
        if (aItemCount == 1)
            {
            const CFSMailMessage& msgPtr(MsgPtrFromListIdL(aItemId));
            HBufC* msgSubject = TFsEmailUiUtility::CreateSubjectTextLC(
                    &msgPtr);
            if (msgPtr.IsFlagSet(EFSMsgFlag_CalendarMsg))
                {
                noteText = StringLoader::LoadL(
                        R_FREESTYLE_EMAIL_DELETE_CALEVENT_NOTE, *msgSubject);
                }
            else
                {
                noteText = StringLoader::LoadL(
                        R_FREESTYLE_EMAIL_DELETE_MAIL_NOTE, *msgSubject);
                }
            CleanupStack::PopAndDestroy(msgSubject);
            CleanupStack::PushL(noteText);
            }
        else // markedEntries.Count() > 1
            {
            noteText = StringLoader::LoadLC(
                    R_FREESTYLE_EMAIL_DELETE_N_MAILS_NOTE, aItemCount);
            }

        // Show the note
        result = TFsEmailUiUtility::ShowConfirmationQueryL(*noteText);
        CleanupStack::PopAndDestroy(noteText);
        }
    return result;
    }

// ---------------------------------------------------------------------------
// RemoveUnnecessaryNodesL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::RemoveUnnecessaryNodesL()
	{
    FUNC_LOG;
	RArray<TInt> nodesToBeRemoved;
	CleanupClosePushL( nodesToBeRemoved );
	for ( TInt i=0;i<iTreeItemArray.Count();i++)
		{
		if ( iMailList->IsNode( iTreeItemArray[i].iListItemId ) )
			{
			if ( iMailList->CountChildren( iTreeItemArray[i].iListItemId ) == 0)
				{
				nodesToBeRemoved.Append( iTreeItemArray[i].iListItemId );
				}
			}
		}
	// Remove from the list itself and from iTreeItemArray
	for ( TInt a = 0; a < nodesToBeRemoved.Count(); a++ )
		{
		iMailList->RemoveL( nodesToBeRemoved[a] );
		SMailListItem item;
		item.iListItemId = nodesToBeRemoved[a];
		TInt IndexToBeDestroyed = iTreeItemArray.Find( item );
		iTreeItemArray.Remove( IndexToBeDestroyed ); // remove from internal array.
		// Remove from model
 		iModel->RemoveAndDestroy( IndexToBeDestroyed );
		}
	CleanupStack::PopAndDestroy( &nodesToBeRemoved );
	}

// ---------------------------------------------------------------------------
// MsgIdFromIndex
// Helper function to get message id from list index
// ---------------------------------------------------------------------------
//
TFSMailMsgId CFSEmailUiMailListVisualiser::MsgIdFromIndex( TInt aItemIdx ) const
    {
    FUNC_LOG;
	TFSMailMsgId msgId; // constructs null ID
    // <cmail>
    if ( 0 <= aItemIdx && aItemIdx < iModel->Count() )
    // </cmail>
        {
        CFSEmailUiMailListModelItem* item =
    			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(aItemIdx));
    	if ( item && item->ModelItemType() == ETypeMailItem )
    	    {
    	    msgId = item->MessagePtr().GetMessageId();
    	    }
        }

    return msgId;
    }

// ---------------------------------------------------------------------------
// MsgIdFromListId
// Get message ID corresponding given list ID. If the list ID is for a node,
// the message ID of its first child is returned
// ---------------------------------------------------------------------------
//
TFSMailMsgId CFSEmailUiMailListVisualiser::MsgIdFromListId( TFsTreeItemId aListId ) const
	{
    FUNC_LOG;
    TFSMailMsgId msgId; // constructs null ID

    if ( aListId != KFsTreeNoneID )
        {
        // Find corresponding message from the model
        const TInt modelCount( iModel->Count() );
        for( TInt i( 0 ) ; i < modelCount ; ++i )
            {
            CFSEmailUiMailListModelItem* item =
                static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( i ) );
            if ( item && aListId == item->CorrespondingListId() )
                {
                msgId = item->MessagePtr().GetMessageId();
                // if list id points to separator
                // set flag on. This is made for improving focus handling.
                if( item->ModelItemType() == ETypeSeparator )
                    {
                    msgId.SetSeparator( ETrue );
                    }
                break;
                }
            }
        }

	return msgId;
	}

// ---------------------------------------------------------------------------
// MsgPtrFromListIdL
//
// ---------------------------------------------------------------------------
//
CFSMailMessage& CFSEmailUiMailListVisualiser::MsgPtrFromListIdL( TFsTreeItemId aListId )
	{
    FUNC_LOG;
	TInt index = ModelIndexFromListId( aListId );
	if ( index >= 0 )
	    {
    	CFSEmailUiMailListModelItem* item =
    		static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(index) );
    	return item->MessagePtr();
	    }
	else
	    {
    	// Leave if no message found. As the function returns a reference, it is not
    	// possible to return any meaningful null value.
    	User::Leave( KErrNotFound );
	    }

	CFSMailMessage* dummy = NULL;
	return *dummy; // to surpress warning about missing return value; this is not really ever run
	}

// ---------------------------------------------------------------------------
// MsgPtrFromListIdL
//
// ---------------------------------------------------------------------------
//
const CFSMailMessage& CFSEmailUiMailListVisualiser::MsgPtrFromListIdL( TFsTreeItemId aListId ) const
    {
    FUNC_LOG;
    TInt index = ModelIndexFromListId( aListId );
    if ( index >= 0 )
        {
        CFSEmailUiMailListModelItem* item =
            static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(index) );
        return item->MessagePtr();
        }
    else
        {
        // Leave if no message found. As the function returns a reference, it is not
        // possible to return any meaningful null value.
        User::Leave( KErrNotFound );
        }

    const CFSMailMessage* dummy = NULL;
    return *dummy; // to surpress warning about missing return value; this is not really ever run
    }

// ---------------------------------------------------------------------------
// ItemDataFromItemId
// Function returns a valid pointer to list item data based on item id
// ---------------------------------------------------------------------------
//
MFsTreeItemData* CFSEmailUiMailListVisualiser::ItemDataFromItemId( TFsTreeItemId aItemId )
	{
    FUNC_LOG;
	MFsTreeItemData* itemData = NULL;
	if ( aItemId != KFsTreeNoneID )
	    {
	    itemData = &iMailList->ItemData( aItemId );
	    }
	return itemData;
	}

// ---------------------------------------------------------------------------
// ItemVisualiserFromItemId
// Function returns a valid pointer to list item visualiser based on item id
// ---------------------------------------------------------------------------
//
MFsTreeItemVisualizer* CFSEmailUiMailListVisualiser::ItemVisualiserFromItemId( TFsTreeItemId aItemId )
	{
    FUNC_LOG;
	MFsTreeItemVisualizer* itemVis = NULL;
    if ( aItemId != KFsTreeNoneID )
        {
        itemVis = &iMailList->ItemVisualizer( aItemId );
        }
	return itemVis;
	}

// ---------------------------------------------------------------------------
// ItemIndexFromMessageId
// Function returns list index based on message pointer. KErrNotFound if
// ID is not included in the list
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::ItemIndexFromMessageId( const TFSMailMsgId& aMessageId ) const
    {
    FUNC_LOG;
	TInt idx = KErrNotFound;

	// Check first the special case; if searched id is null id, there's no
	// point to loop the whole list. There shouldn't be any null id's in
	// the model anyway. There is currently also an error in FW's equality
	// operator implementation; it doesn't check the iNullId flag at all.
	if( !aMessageId.IsNullId() )
	    {
	    const TInt modelCount( iModel->Count() );
    	for ( TInt i( 0 ); i < modelCount ; ++i )
    		{
    		CFSEmailUiMailListModelItem* item =
                static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( i ) );
			// when the item is a separator check whether its MessagePtr is valid (actually it's a reference)
            if( item && &(item->MessagePtr()) != NULL)
                {
                if ( aMessageId == item->MessagePtr().GetMessageId() )
                    {
                    TModelItemType itemType = item->ModelItemType();
                    TBool separator( aMessageId.IsSeparator() );

                    // Because separator and the first message after separator
                    // have same message id, we need to separate these cases
                    // and that is made with separator flag which is stored to
                    // TFSMailMsgId object. If separator flag is on item need to be
                    // separator if it is not on item needs to be mail item.
                    if( ( separator && itemType == ETypeSeparator ) ||
                        ( !separator && itemType == ETypeMailItem ) )
                        {
                        idx = i;
                        break;
                        }
                    }
                }
            }
		}
	return idx;
    }

// ---------------------------------------------------------------------------
// NextMessageIndex
// Function returns next message index from highlighted index
// KErrNotFound if there is no next message
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::NextMessageIndex( TInt aCurMsgIdx ) const
    {
    FUNC_LOG;
	TInt idx = KErrNotFound;
	for ( TInt i=aCurMsgIdx+1 ; i<iModel->Count() ; i++ )
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(i) );
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			idx = i;
			break;
			}
		}

	return idx;
    }

// ---------------------------------------------------------------------------
// PreviousMessageIndex
// Function returns previous message index from highlighted index
// KErrNotFound if there is no previous message
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::PreviousMessageIndex( TInt aCurMsgIdx ) const
    {
    FUNC_LOG;
	TInt idx( KErrNotFound );
	if ( aCurMsgIdx < iModel->Count() )
	    {
    	for ( TInt i=aCurMsgIdx-1 ; i>=0 ; i-- )
    		{
    		CFSEmailUiMailListModelItem* item =
    			static_cast<CFSEmailUiMailListModelItem*>( iModel->Item(i) );
    		if ( item && item->ModelItemType() == ETypeMailItem )
    			{
    			idx = i;
    			break;
    			}
    		}
	    }
	return idx;
    }

// ---------------------------------------------------------------------------
// ModelIndexFromListId
// Function returns a model index corresponding the given tree list item ID
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailListVisualiser::ModelIndexFromListId( TFsTreeItemId aItemId ) const
	{
    FUNC_LOG;
    TInt ret = KErrNotFound;
    const TInt count = iModel->Count();
    for ( TInt i=0; i < count; i++ )
    	{
    	const CFSEmailUiMailListModelItem* item =
		    static_cast<const CFSEmailUiMailListModelItem*>( iModel->Item(i) );
    	if ( item && aItemId == item->CorrespondingListId() )
    		{
    		ret = i;
    		break;
    		}
    	}
    return ret;
	}

// ---------------------------------------------------------------------------
// FolderSelectedL
// Folder list selection callback. Function updates list when folder is changed
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::FolderSelectedL(
	TFSMailMsgId aSelectedFolderId,
	TFSEmailUiCtrlBarResponse aResponse )
	{
    FUNC_LOG;
    // hide selector if keys are not pressed
    iControlBarControl->MakeSelectorVisible( IsFocusShown() );
	if ( iMoveToFolderOngoing )
		{
		iMoveToFolderOngoing = EFalse;
		switch ( aResponse )
			{
			case EFSEmailUiCtrlBarResponseCancel:
			    iMarkingModeWaitingToExit = EFalse;
			    SetMskL();
				return;
			case EFSEmailUiCtrlBarResponseSelect:
			    {
				MoveMsgsToFolderL( aSelectedFolderId );
				SetMskL();
			    }
				break;
			default:
				break;
			}
		}
	else
		{
		// Ignore if cancel selected
		switch ( aResponse )
			{
			case EFSEmailUiCtrlBarResponseSwitchList:
				{
				TFSFolderType folderType;
				if ( iMailFolder )
					{
					folderType = iMailFolder->GetFolderType();
					}
				else
					{
					folderType = EFSInbox;
					}
			    iControlBarControl->SetFocusByIdL( iSortButtonId );
			    iControlBarControl->MakeSelectorVisible( IsFocusShown() );

			    // Sorting empty mail list disabled
                // Show sort list only if mail list is not empty
                if ( iModel->Count() )
                    {
                    iAppUi.ShowSortListInPopupL( iCurrentSortCriteria, folderType, this, iSortButton );
                    }
				}
				return;
			case EFSEmailUiCtrlBarResponseCancel:
			    iMarkingModeWaitingToExit = EFalse;
			    SetMskL();
                //Set touchmanager back to active
                DisableMailList(EFalse);
				return;
			case EFSEmailUiCtrlBarResponseSelect:
			    SetMskL();
                //Set touchmanager back to active
                DisableMailList(EFalse);
			default:
				break;
			}

		if ( !iMailFolder || ( iMailFolder && iMailFolder->GetFolderId() != aSelectedFolderId ) )
		    {
		    iMailListModelUpdater->Cancel();
		    iNewMailTimer->Cancel();
		    iNewMailIds.Reset();
		    SafeDelete(iMailFolder);
            iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( iAppUi.GetActiveMailboxId(), aSelectedFolderId );

            if ( !iMailFolder )
                {
                // Do nothing if can't get the folder object
                return;
                }

            UpdateFolderAndMarkingModeTextsL();

            // Set initial sort criteria when folder has changed
            iCurrentSortCriteria.iField = EFSMailSortByDate;
            iCurrentSortCriteria.iOrder = EFSMailDescending;
            // reload node state because in file sort mode this is disabled even when globally enabled
            iNodesInUse = iAppUi.GetCRHandler()->TitleDividers();
            SetSortButtonIconL();

            // Update the mail list contents
            UpdateMailListModelL();
            RefreshL();
		    }
        iFocusedControl = EControlBarComponent;
        SetListAndCtrlBarFocusL();
        iMoveToFolderOngoing = EFalse;
		}
	SetMskL();
	}

// ---------------------------------------------------------------------------
// MailboxSelectedL
// Mailbox selection callback. Function updates list when mailbox is changed
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::MailboxSelectedL( TFSMailMsgId aSelectedMailboxId )
	{
    FUNC_LOG;
    iControlBarControl->MakeSelectorVisible( IsFocusShown() );
    //Set touchmanager back to active
    DisableMailList(EFalse);
	iAppUi.SetActiveMailboxL( aSelectedMailboxId );
	iMailListModelUpdater->Cancel();
	SafeDelete(iMailFolder);
	iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( iAppUi.GetActiveMailboxId(), iAppUi.GetActiveBoxInboxId() );

	// Set initial sort criteria when folder has changed
    iCurrentSortCriteria.iField = EFSMailSortByDate;
    iCurrentSortCriteria.iOrder = EFSMailDescending;
    // reload node state because in file sort mode this is disabled even when globally enabled
    iNodesInUse = iAppUi.GetCRHandler()->TitleDividers();
    SetSortButtonIconL();

    // Set folder name to the control bar button
    UpdateFolderAndMarkingModeTextsL();

    // Set mailbox name and icons
    SetMailboxNameToStatusPaneL();
    SetBrandedListWatermarkL();
    SetBrandedMailBoxIconL();

    // Update model
    UpdateMailListModelL();
    iMailList->RemoveAllL();
    RefreshDeferred();

	// Check sync icon timer and sync status
    ConnectionIconHandling();
	iFocusedControl = EControlBarComponent;
	}

// ---------------------------------------------------------------------------
// FolderButtonRect
// Getter for folder button rectangle. Folder popup needs to get this
// information when screen layout changes.
// ---------------------------------------------------------------------------
//
TRect CFSEmailUiMailListVisualiser::FolderButtonRect()
    {
    FUNC_LOG;
    const TPoint& buttonPos = iFolderListButton->Pos().Target();
    const TPoint& buttonSize = iFolderListButton->Size().Target();
    TRect buttonRect( buttonPos, buttonSize.AsSize() );
    return buttonRect;
    }

// ---------------------------------------------------------------------------
// SortOrderChangedL
// Sort order selection callback. Function updates list when sorting is changed
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::SortOrderChangedL(
	TFSMailSortField aSortField, TFSEmailUiCtrlBarResponse aResponse )
	{
    FUNC_LOG;
	// Ignore if cancel selected
	switch( aResponse )
		{
		case EFSEmailUiCtrlBarResponseSwitchList:
			{
			iControlBarControl->MakeSelectorVisible( IsFocusShown() );
		    iControlBarControl->SetFocusByIdL( iFolderListButtonId );
	   		iAppUi.ShowFolderListInPopupL( FolderId(), this, iFolderListButton );
			}
            return;
		case EFSEmailUiCtrlBarResponseCancel:
		    iControlBarControl->MakeSelectorVisible( IsFocusShown() );
		    SetMskL();
		    // <cmail> Touch
            //Set touchmanager back to active
            DisableMailList(EFalse);
            return;

		case EFSEmailUiCtrlBarResponseSelect:
		    iControlBarControl->MakeSelectorVisible( IsFocusShown() );
            //Set touchmanager back to active
            DisableMailList(EFalse);
            // </cmail>
		default:
			break;
		}

	// Check is the selected sort field same as the previous one
	if( iCurrentSortCriteria.iField == aSortField )
		{
		// If same sort field selected, switch the sorting order
		if( iCurrentSortCriteria.iOrder == EFSMailAscending )
			{
			iCurrentSortCriteria.iOrder = EFSMailDescending;
			}
		else
			{
			iCurrentSortCriteria.iOrder = EFSMailAscending;
			}
		}
	else if ( aSortField == EFSMailSortBySubject ||
			  aSortField == EFSMailSortBySender ||
			  aSortField == EFSMailSortByUnread || // <cmail>
			  aSortField == EFSMailSortByRecipient )
		{
		iCurrentSortCriteria.iOrder = EFSMailAscending;
		}
	else
		{
		iCurrentSortCriteria.iOrder = EFSMailDescending;
		}

	iCurrentSortCriteria.iField = aSortField;
	iModel->SetSortCriteria(iCurrentSortCriteria);

	// rest of the code moved to TimerEventL ( iSortTimer part )
	// used also in DoHandleControlBarOpenL to prevent SortList reopening
	iSortState = ESortRequested; 
	iSortTimer->Start( KSortTimerDelay );	
	}

// ---------------------------------------------------------------------------
// SortButtonRect
// Getter for sort button rectangle. Sort popup needs to get this
// information when screen layout changes.
// ---------------------------------------------------------------------------
//
TRect CFSEmailUiMailListVisualiser::SortButtonRect()
    {
    FUNC_LOG;
    const TPoint& buttonPos = iSortButton->Pos().Target();
    const TPoint& buttonSize = iSortButton->Size().Target();
    TRect buttonRect( buttonPos, buttonSize.AsSize() );
    return buttonRect;
    }

// ---------------------------------------------------------------------------
// OpenHighlightedMailL
// Function opens highlighted mail either to internal viewer or to mrui if
// message type is meeting
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::OpenHighlightedMailL()
    {
    FUNC_LOG;
    OpenMailItemL( iMailList->FocusedItem() );
    }

// ---------------------------------------------------------------------------
// OpenHighlightedMailL
// Function opens given mail item either to internal viewer or to mrui if
// message type is meeting
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::OpenMailItemL( TFsTreeItemId aMailItem )
	{
    FUNC_LOG;
	TInt idx = ModelIndexFromListId( aMailItem );
	if ( idx >= 0 )
	    {
	    CFSEmailUiMailListModelItem* item =
	        static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(idx) );
	    if ( item && item->ModelItemType() == ETypeMailItem )
	        {
	        // First make sure that the highlighted message really exists in the store
	        // Get confirmed msg ptr
	        CFSMailMessage* confirmedMsgPtr(0);
	        confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL(iAppUi.GetActiveMailboxId(),
	                iMailFolder->GetFolderId(), item->MessagePtr().GetMessageId(), EFSMsgDataEnvelope );
	        if ( confirmedMsgPtr )
	            {
	            iMailOpened = ETrue;

	            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
	            // Pointer confirmed, store Id and delete not needed anymore
	            TFSMailMsgId confirmedId = confirmedMsgPtr->GetMessageId();
	      		TBool isCalMessage = confirmedMsgPtr->IsFlagSet( EFSMsgFlag_CalendarMsg );
	      		TBool isReadMessage = confirmedMsgPtr->IsFlagSet( EFSMsgFlag_Read );
	            delete confirmedMsgPtr;
	            // Open to editor from drafts
	            if ( iMailFolder->GetFolderType() == EFSDraftsFolder )
	                {
	                if ( !isCalMessage ) // Open only normal messages for now
	                	{
		                TEditorLaunchParams params;
		                params.iMailboxId = iAppUi.GetActiveMailboxId();
		                params.iActivatedExternally = EFalse;
		                params.iMsgId = confirmedId;
		                params.iFolderId = iMailFolder->GetFolderId();
		                iAppUi.LaunchEditorL( KEditorCmdOpen, params );
	                	}
	                else
	               	 	{
	                	//Try to open to editor if support for that is needed
	                	}
	                }
	            else if ( iMailFolder->GetFolderType() == EFSOutbox )
	                {
	                TFsEmailUiUtility::ShowErrorNoteL( R_FREESTYLE_EMAIL_UI_OPEN_FROM_OUTBOX_NOTE, ETrue );
	                }
	            else
	                {
                    THtmlViewerActivationData htmlData;
                    htmlData.iActivationDataType = THtmlViewerActivationData::EMailMessage;
                    htmlData.iMailBoxId = iAppUi.GetActiveMailbox()->GetId();
                    htmlData.iFolderId = FolderId();
                    htmlData.iMessageId = confirmedId;
                    TPckgBuf<THtmlViewerActivationData> pckgData( htmlData );

                    // Change read status only if needed
                    if ( !isReadMessage )
                        {
                        ChangeReadStatusOfHighlightedL( ETrue );
                        }

                    // Opening viewer seems to take more time than other views,
                    // so we do longer fade out in this case
                    SetNextTransitionOutLong( ETrue );

                    iAppUi.EnterFsEmailViewL( HtmlViewerId, KHtmlViewerOpenNew, pckgData );
	                }
	            }
	        }
	    }
	}

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// The ownership of the CLS items in the contacts array is transferred to the
// observer, and they must be deleted by the observer.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::OperationCompleteL(
    TContactHandlerCmd /*aCmd*/, const RPointerArray<CFSEmailUiClsItem>& /*aContacts*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// Handles error in contact handler operation.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::OperationErrorL( TContactHandlerCmd /*aCmd*/,
    TInt /*aError*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// NotifyDateChangedL
// Called when CDateChangeTimer completes. This happens when either when date
// changes or when user alters the system time. Redraws the list to ensure
// that time stamp texts in emails and nodes are up-to-date.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::NotifyDateChangedL()
    {
    FUNC_LOG;
    iDateChanged = ETrue;
    if ( iAppUi.IsForeground() )
        {
        HandleForegroundEventL(ETrue);
        }
    }

// ---------------------------------------------------------------------------
// CreateFolderNameLC
// Creates folder name from folder ptr. Localised if standard folder.
// ---------------------------------------------------------------------------
//
HBufC* CFSEmailUiMailListVisualiser::CreateFolderNameLC( const CFSMailFolder* aFolder ) const
	{
    FUNC_LOG;
	TInt resourceId(0);
	HBufC* ret(0);
	if ( !aFolder || aFolder->GetFolderId().IsNullId() )
		{
		// INBOX FOR NULL ID
		resourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_INBOX;
		}
	else
		{
		TInt folderType = aFolder->GetFolderType();
		switch ( folderType )
			{
			case EFSInbox:
				{
				resourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_INBOX;
				}
				break;
			case EFSOutbox:
				{
				resourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_OUTBOX;
				}
				break;
			case EFSDraftsFolder:
				{
				resourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_DRAFTS;
				}
				break;
			case EFSSentFolder:
				{
				resourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_SENT;
				}
				break;
			case EFSDeleted:
				{
				resourceId = R_FREESTYLE_EMAIL_UI_DROPDOWN_LIST_DELETED;
				}
				break;
			default:
				// Not a standard folder
				break;
			}
		}
	if ( resourceId )
		{
		ret = StringLoader::LoadLC( resourceId );
		}
	else
		{
		ret = aFolder->GetFolderName().AllocLC();
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// LaunchStylusPopupMenuL
// Function launches avkon stylus popup menu based on the selected message item/items
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::LaunchStylusPopupMenuL( const TPoint& aPoint )
	{
	// Irrelevant items for focused mail list item get dimmed at runtime

	// Check mail list item's type
	CFSEmailUiMailListModelItem* item =
		static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
	if ( item )
		{
		if ( item->ModelItemType() == ETypeMailItem )
			{
			// Add mark as read / unread options
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, !IsMarkAsUnreadAvailableL() );
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdMarkAsRead, !IsMarkAsReadAvailableL() );

			// Check support for object mail iten moving
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsMoveMessage,
         !(iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder )
         && iMailFolder->SupportsMoveFromL( iMailFolder->GetFolderType() ) ) );

			// Hide / show follow up
			TBool supportsFlag = TFsEmailUiUtility::IsFollowUpSupported( *iAppUi.GetActiveMailbox() );
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsFlag, !supportsFlag );

			// Hide collapse / expand all
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsCollapseAll, ETrue );
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsExpandAll, ETrue );
			}
		else if ( item->ModelItemType() == ETypeSeparator )
			{
			// Hide mark as read / unread options
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, ETrue );
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdMarkAsRead, ETrue );

			// Hide move & follow up
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsMoveMessage, ETrue );
			iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsFlag, ETrue );

			// Hide collapse / expand all when applicable
			if ( iNodesInUse == EListControlSeparatorDisabled || !iModel->Count() )
				{
				iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsCollapseAll, ETrue );
				iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsExpandAll, ETrue );
				}
			else
				{
				iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsCollapseAll, AllNodesCollapsed() );
				iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdActionsExpandAll, AllNodesExpanded() );
				}
			}
		}

	// Set the position for the popup
	TPoint point(aPoint.iX, aPoint.iY + 45);
	iStylusPopUpMenu->SetPosition( point, CAknStylusPopUpMenu::EPositionTypeRightBottom );

	// Display the popup and set the flag to indicate that the menu was
	// launched.
	iStylusPopUpMenu->ShowMenu();
	iStylusPopUpMenuVisible = ETrue;
	}

// ---------------------------------------------------------------------------
// CreateNewMsgL
// Launches editor.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::CreateNewMsgL()
	{
    FUNC_LOG;
	TEditorLaunchParams params;
	params.iMailboxId = iAppUi.GetActiveMailboxId();
	params.iActivatedExternally = EFalse;
    iAppUi.LaunchEditorL( KEditorCmdCreateNew, params );
	}

void CFSEmailUiMailListVisualiser::ReplyL( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
    // Replying not possible from drafts folder
    if ( iMailFolder && iMailFolder->GetFolderType() != EFSDraftsFolder )
        {
        DoReplyForwardL( KEditorCmdReply, aMsgPtr );
        }
	}

void CFSEmailUiMailListVisualiser::ReplyAllL(  CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
    // Replying all not possible from drafts folder
    if ( iMailFolder && iMailFolder->GetFolderType() != EFSDraftsFolder )
        {
        DoReplyForwardL( KEditorCmdReplyAll, aMsgPtr );
        }
	}

void CFSEmailUiMailListVisualiser::ForwardL( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
    DoReplyForwardL( KEditorCmdForward, aMsgPtr );
	}

// ---------------------------------------------------------------------------
// DoReplyForwardL
// Launches editor with either reply all forward command
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::DoReplyForwardL( TEditorLaunchMode aMode, CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;
    // Reply, reply all, and forward commands are inavailable in the outbox
    // <cmail>
    if ( iModel->Count() && iMailFolder && iMailFolder->GetFolderType() != EFSOutbox )
    // </cmail>
        {
        CFSMailMessage* messagePointer = aMsgPtr;
    	if ( !messagePointer  )
    		{
    		RFsTreeItemIdList targetEntries;
    		CleanupClosePushL( targetEntries );
    		GetActionsTargetEntriesL( targetEntries );
    		// action is possible only when there's exactly one marked or focused mail item
    		if ( targetEntries.Count() == 1 )
    			{
    			messagePointer = &MsgPtrFromListIdL( targetEntries[0] );
    			}
    		CleanupStack::PopAndDestroy( &targetEntries );
    		}
    	if ( messagePointer )
    		{
            // Opening editor is slower than opening most other views.
    		// Use longer transition effect to mask this.
            SetNextTransitionOutLong( ETrue );

    		TEditorLaunchParams params;
    		params.iMailboxId = iAppUi.GetActiveMailboxId();
    		params.iActivatedExternally = EFalse;
    		params.iMsgId = messagePointer->GetMessageId();
    		iAppUi.LaunchEditorL( aMode, params );
    		}
        }
    }

// ---------------------------------------------------------------------------
// HandleMailBoxEventL
// Mailbox event handler, responds to events sent by the plugin.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::HandleMailBoxEventL( TFSMailEvent aEvent,
    TFSMailMsgId aMailboxId, TAny* aParam1, TAny* aParam2, TAny* aParam3 )
    {
    FUNC_LOG;

    // Complete any pending asynchronous redrawing before handling the event
    // to make sure that all the arrays are in consistent state
    CompletePendingRefresh();

    // Handle the event
    if ( iFirstStartCompleted &&
         aMailboxId == iAppUi.GetActiveMailboxId() ) // Safety, in list events that only concern active mailbox are handled
        {
		if ( aEvent == TFSEventFoldersDeleted )
            {
			// Check whether the current folder gets deleted
			// Change mail item icon or read status
			RArray<TFSMailMsgId>* removedEntries = static_cast<RArray<TFSMailMsgId>*>( aParam1 );
			if ( iMailFolder && removedEntries && removedEntries->Count() )
				{
				TFSMailMsgId currentFolderId = iMailFolder->GetFolderId();
				for ( TInt i = 0; i < removedEntries->Count(); i++ )
					{
					TFSMailMsgId entryId = (*removedEntries)[i];
					if ( entryId == currentFolderId )
						{
						// Current folder deleted, try to revert back to standard folder inbox.
						iMailListModelUpdater->Cancel();
						SafeDelete(iMailFolder);
						TFSMailMsgId inboxId = iAppUi.GetActiveMailbox()->GetStandardFolderId( EFSInbox );
						if ( !inboxId.IsNullId() )
						    {
						    iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( aMailboxId, inboxId );
						    UpdateMailListModelL();
					        RefreshL();
						    UpdateFolderAndMarkingModeTextsL();
						    }
						else
							{
							// No standard folder inbox, go back to grid as a last option
							HandleCommandL( EAknSoftkeyBack );
							}
						break;
						}
					}
				}
			}
		else if ( aEvent == TFSEventMailboxRenamed )
            {
			if ( iThisViewActive )
				{
				SetMailboxNameToStatusPaneL();
				}
			}
	 	else if ( (aEvent == TFSEventMailDeleted || aEvent == TFSEventMailDeletedFromViewer) && iMailFolder ) // <cmail> Added iMailFolder null safety check </cmail>
			{
			// Change mail item icon or read status
			RArray<TFSMailMsgId>* removedEntries = static_cast<RArray<TFSMailMsgId>*>(aParam1);
			if ( removedEntries && removedEntries->Count() )
				{
				TFSMailMsgId* parentFolderId = static_cast<TFSMailMsgId*>( aParam2 );
				TFSMailMsgId currentFolderId = iMailFolder->GetFolderId();
				if ( parentFolderId && ( *parentFolderId == currentFolderId ) )
					{
	 				// Refresh mailfolder to get correct actual data
                    /*
                    iMailListModelUpdater->Cancel();
					SafeDelete(iMailFolder);
					iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( aMailboxId, currentFolderId );
					*/
					RemoveMsgItemsFromListIfFoundL( *removedEntries );
					}
				}
			}
		else if ( aEvent == TFSEventNewMail )
		    {
			// Switch to standardfolder inbox if we have null folderid.
			// This is usually the case with first time sync.
			if ( FolderId().IsNullId() )
				{
				// Refresh mailfolder to standard folder inbox in case of zero id
                iMailListModelUpdater->Cancel();
				SafeDelete(iMailFolder);
				TFSMailMsgId inboxId = iAppUi.GetActiveMailbox()->GetStandardFolderId( EFSInbox );
				if ( !inboxId.IsNullId() )
				    {
				    iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( aMailboxId, inboxId );
				    // Check that mailfolder fetching succeeded
				    if ( iMailFolder )
				    	{
					    UpdateFolderAndMarkingModeTextsL();
				    	}
				    }
				}
			// Null pointer check is needed here because of special cases such as new mail
			// creation in POP/IMAP before first sync where folder does not exists
			if ( iMailFolder )
				{
	            RArray<TFSMailMsgId>* newMessages = static_cast< RArray<TFSMailMsgId>* >( aParam1 );
				TFSMailMsgId* parentFolderId = static_cast<TFSMailMsgId*>( aParam2 );
				TFSMailMsgId currentFolderId = iMailFolder->GetFolderId();
				// Update the mail list only if the event is related to the currently open folder
				if ( *parentFolderId == currentFolderId )
				    {
				    InsertNewMessagesL( *newMessages );
				    }
				}
		    }
		else if ( aEvent == TFSEventMailMoved && iMailFolder )	// Added iMailFolder null safety check
			{
            // If message was moved FROM this folder, just remove the entry from the list.
            // If message is moved TO this folder, update the list completely.
            // Otherwise, the event does not have an influence on the current folder.
			RArray<TFSMailMsgId>* entries = static_cast< RArray<TFSMailMsgId>* >( aParam1 );
			TFSMailMsgId* toFolderId = static_cast<TFSMailMsgId*>( aParam2 );
			TFSMailMsgId* fromFolderId = static_cast<TFSMailMsgId*>( aParam3 );
			TFSMailMsgId currentFolderId = iMailFolder->GetFolderId();

            if ( toFolderId && ( currentFolderId == *toFolderId ) )
                {
                InsertNewMessagesL( *entries );
                }
            else if ( fromFolderId && ( currentFolderId == *fromFolderId ) )
                {
 	 			// Refresh mailfolder to get correct actual data
                /*
                iMailListModelUpdater->Cancel();
                SafeDelete(iMailFolder);
				iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( aMailboxId, currentFolderId );
				*/
				RemoveMsgItemsFromListIfFoundL( *entries );
                }
            else
                {
                // event is not related to the current folder => do nothing
                }
			}
		else if ( aEvent == TFSEventMailChanged && iMailFolder ) // Added iMailFolder null safety check
			{
			//if ( !iMailOpened ) // do not handle mail changed while viewer/editor is open. This may cause flickering
			    {
    			// Change mail item icon or read status
    			RArray<TFSMailMsgId>* entries = static_cast<RArray<TFSMailMsgId>*>( aParam1 );
    			TFSMailMsgId* parentFolderId = static_cast<TFSMailMsgId*>( aParam2 );
    			TFSMailMsgId currentFolderId = iMailFolder->GetFolderId();
    			if ( *parentFolderId == currentFolderId )
    				{
     	 			// Refresh mailfolder to get correct actual data
    				/*
                    iMailListModelUpdater->Cancel();
                    SafeDelete(iMailFolder);
    				iMailFolder = iAppUi.GetMailClient()->GetFolderByUidL( aMailboxId, currentFolderId );
    				*/
    				for ( TInt i=0 ; i<entries->Count() ; i++ )
    					{
    					TFSMailMsgId msgId = (*entries)[i];
    					TInt idx = ItemIndexFromMessageId( msgId );
    					if ( idx != KErrNotFound )
    					    {
    					    UpdateItemAtIndexL( idx );
    					    }
    					}
    				}
			    }
			}
   		else if ( aEvent == TFSEventMailboxSyncStateChanged )
   			{
	  		TSSMailSyncState* newSyncState = static_cast<TSSMailSyncState*>( aParam1 );
    		if ( newSyncState != 0 && *newSyncState != 0 )
    			{
	    		switch ( *newSyncState )
	    			{
	    			case StartingSync:
	   			    {
	   			    //If sync was started by user, show the synchronisation indicator
	   			    if ( iManualMailBoxSync )
	   			        {
			   		ManualMailBoxSync(EFalse);
	   				}
		   		    }
				    break;

	    			case SyncCancelled:
	    			    {
	    			    //If sync was started by user, show the synchronisation indicator
	    			    if ( iManualMailBoxSync )
	    			        {
	    			        ManualMailBoxSync(EFalse);
	    			        }
	    			    }
	    			    break;

	    			}
    			}
			}
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::TreeListEventL( const TFsTreeListEvent aEvent,
                                                   const TFsTreeItemId aId,
                                                   const TPoint& /*aPoint*/ )
	{
    FUNC_LOG;
    switch( aEvent )
        {
        case EFsTreeListItemWillGetFocused:
            {
            if( aId != KFsTreeNoneID )
                {
                UpdatePreviewPaneTextIfNecessaryL( aId, EFalse );
                }
            break;
            }
        case EFsTreeListItemTouchFocused:
            {
            break;
            }
        case EFsFocusVisibilityChange:
            {
            iAppUi.SetFocusVisibility( EFalse );
            break;
            }
        default:
            {
            // Do not handle other events
            break;
            }

        }
	}

// ---------------------------------------------------------------------------
// UpdateMailListSettingsL
// Function reads and updates mail list specific settings from the cen rep
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailListVisualiser::UpdateMailListSettingsL()
	{
    FUNC_LOG;
	if ( iAppUi.GetCRHandler() )
		{
		iNodesInUse = iAppUi.GetCRHandler()->TitleDividers();
		TInt lineValue = iAppUi.GetCRHandler()->MessageListLayout();
		TInt bodyPreviewValue = iAppUi.GetCRHandler()->BodyPreview();
		if ( lineValue == 1 ) // 1-line layouts
			{
			if ( bodyPreviewValue == 0 )
				{
				iListMode = EListControlTypeSingleLinePreviewOff;
				}
			else
				{
				iListMode = EListControlTypeSingleLinePreviewOn;
				}
			}
		else //
			{
			if ( bodyPreviewValue == 0 )
				{
				iListMode = EListControlTypeDoubleLinePreviewOff;
				}
			else
				{
				iListMode = EListControlTypeDoubleLinePreviewOn;
				}
			}
		}
	else
		{
		iNodesInUse = EListControlSeparatorDisabled;
		iListMode = EListControlTypeDoubleLinePreviewOff;
		}
	}

void CFSEmailUiMailListVisualiser::UpdateMailListTimeDateSettings()
	{
    FUNC_LOG;
    TLocale currentLocaleSettings;
	iDateFormats.iTimeFormat = currentLocaleSettings.TimeFormat();
	iDateFormats.iDateFormat = currentLocaleSettings.DateFormat();
	iDateFormats.iAmPmPosition = currentLocaleSettings.AmPmSymbolPosition();
	// Second time separartor
	iDateFormats.iTimeSeparator = currentLocaleSettings.TimeSeparator( 1 );
	// Second date separartor
	iDateFormats.iDateSeparator = currentLocaleSettings.DateSeparator( 1 );
	}

STimeDateFormats CFSEmailUiMailListVisualiser::MailListTimeDateSettings()
	{
    FUNC_LOG;
	return iDateFormats;
	}


void CFSEmailUiMailListVisualiser::SetMailboxNameToStatusPaneL()
	{
    FUNC_LOG;
    iAppUi.SetActiveMailboxNameToStatusPaneL();
	}

void CFSEmailUiMailListVisualiser::SetBrandedMailBoxIconL()
	{
    FUNC_LOG;
	// Get and draw branded mailbox icon
	MFSMailBrandManager& brandManager = iAppUi.GetMailClient()->GetBrandManagerL();
	CGulIcon* mailBoxIcon(0);
	TRAPD( err, mailBoxIcon = brandManager.GetGraphicL( EFSMailboxIcon,  iAppUi.GetActiveMailboxId() ) );
	if ( err == KErrNone && mailBoxIcon )
		{
		CleanupStack::PushL( mailBoxIcon );
        //<cmail>
		TInt iconSize( iAppUi.LayoutHandler()->GetControlBarMailboxIconRect().Width() );
		TSize defaultIconSize ( iconSize, iconSize );
        //</cmail>
    	AknIconUtils::SetSize(mailBoxIcon->Bitmap(), defaultIconSize);
	    AknIconUtils::SetSize(mailBoxIcon->Mask(), defaultIconSize);
		// Create texture into TextureManager, If not already existing
	    iAppUi.FsTextureManager()->CreateBrandedMailboxTexture( mailBoxIcon,
	    		                                                iAppUi.GetActiveMailboxId().PluginId(),
	    		                                                iAppUi.GetActiveMailboxId().Id(),
	    		                                                defaultIconSize );
	    // Get branded mailbox icon
	    iMailBoxIconTexture = &iAppUi.FsTextureManager()->TextureByMailboxIdL( iAppUi.GetActiveMailboxId().PluginId(),
	    		                                                               iAppUi.GetActiveMailboxId().Id(),
	    		                                                               defaultIconSize );
		iFolderListButton->SetIconL( *iMailBoxIconTexture );
		CleanupStack::PopAndDestroy( mailBoxIcon );
		}
	else
		{
		iFolderListButton->SetIconL( iAppUi.FsTextureManager()->TextureByIndex( EListControlBarMailboxDefaultIcon ) );
		}
	}


void CFSEmailUiMailListVisualiser::SetBrandedListWatermarkL()
	{
    FUNC_LOG;
	MFSMailBrandManager& brandManager = iAppUi.GetMailClient()->GetBrandManagerL();
	CGulIcon* mailBoxWatermarkIcon(0);
	TRAPD( err, mailBoxWatermarkIcon = brandManager.GetGraphicL( EFSWatermark,  iAppUi.GetActiveMailboxId() ) );
	if ( err == KErrNone && mailBoxWatermarkIcon )
		{
		CleanupStack::PushL( mailBoxWatermarkIcon );
		TRect mainPaneRect;
	 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
		TInt xPos = mainPaneRect.iBr.iX/100*40; // 40% of main pane width
		TInt yPos = mainPaneRect.iBr.iY/100*40; // 40% of main pane height
	    TSize defaultWaterMarkSize(xPos,yPos);
    	AknIconUtils::SetSize(mailBoxWatermarkIcon->Bitmap(), defaultWaterMarkSize);
	    AknIconUtils::SetSize(mailBoxWatermarkIcon->Mask(), defaultWaterMarkSize);
		if ( !iMailBoxWatermarkTexture )
			{
			// iMailBoxWatermarkTexture = CAlfTexture::NewL( *iEnv, Id() ); // DOLATER - Replaced by line below, creates blank texture until UploadL's replaced by something
			iMailBoxWatermarkTexture = &CAlfStatic::Env().TextureManager().BlankTexture();
			}
		// DOLATER - Gone from Alfred, needs to be replaced with something
		//iMailBoxWatermarkTexture->UploadL( *mailBoxWatermarkIcon->Bitmap(), mailBoxWatermarkIcon->Mask() );
		CleanupStack::PopAndDestroy( mailBoxWatermarkIcon );
		TPoint watermarkPosition( mainPaneRect.iBr.iX-xPos-20, mainPaneRect.iBr.iY-yPos-20  );
   		iMailTreeListVisualizer->SetWatermarkPos( watermarkPosition );
		iMailTreeListVisualizer->SetWatermarkSize( defaultWaterMarkSize );
		iMailTreeListVisualizer->SetWatermarkOpacity( 0.3 );
		iMailTreeListVisualizer->SetWatermarkL( iMailBoxWatermarkTexture );
		}
	else
		{
		// Not found, hide watermark
		iMailTreeListVisualizer->SetWatermarkOpacity( 0 );
		}
	}

TFsTreeItemId CFSEmailUiMailListVisualiser::ParentNode( TFsTreeItemId aItemId ) const
    {
    FUNC_LOG;
	if ( iNodesInUse )
		{
		TFsTreeItemId parentNodeId = iMailList->Parent( aItemId );
		if ( iMailList->IsNode( parentNodeId ) )
			{
			return parentNodeId;
			}
		}
	return KErrNotFound;
    }

TBool CFSEmailUiMailListVisualiser::DoScrollMarkUnmarkL()
    {
    FUNC_LOG;
    TBool ret = EFalse;

    if ( !iMarkingMode )
        {
        EnterMarkingModeL();   
        }

    // <cmail>
    if ( iFocusedControl == EMailListComponent && iModel->Count() )
    // </cmail>
        {
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );

		if ( item && item->ModelItemType() == ETypeMailItem )  // Separators are not markable
		    {
			if ( !iListMarkItemsState )
				{
       			iMailList->MarkItemL( iMailList->FocusedItem(), EFalse );
				}
			else
				{
       			iMailList->MarkItemL( iMailList->FocusedItem(), ETrue );
				}
			ret = ETrue;
           	}
        }

    return ret;
    }

// Mark / unmark all items under current separator
void CFSEmailUiMailListVisualiser::MarkItemsUnderSeparatorL( TBool aMarked, TInt aSeparatorId )
	{
	FUNC_LOG;

	if ( iTreeItemArray.Count() )
		{
		// Find all items under wanted separator.
		for ( TInt i = aSeparatorId + 1; i < iTreeItemArray.Count(); i++ )
			{
			CFSEmailUiMailListModelItem* item =
						static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( i ) );

			// Mark / unmark mail items.
			if ( item &&
				 item->ModelItemType() == ETypeMailItem &&
				 !iMailList->IsNode( iTreeItemArray[i].iListItemId ) )
				{
				iMailList->MarkItemL( iTreeItemArray[i].iListItemId, aMarked );
				}
			else
				{
				// Stop iteration since another iterator was reached.
				break;
				}
			}
		}
	}

// Check if all items under current separator are marked
TBool CFSEmailUiMailListVisualiser::AreAllItemsMarkedUnderSeparatorL( TInt aSeparatorId )
    {
    FUNC_LOG;

    TBool ret(ETrue);
    
    if ( iTreeItemArray.Count() )
        {
        // Find all items under wanted separator.
        for ( TInt i = aSeparatorId + 1; i < iTreeItemArray.Count(); i++ )
            {
            CFSEmailUiMailListModelItem* item =
                        static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( i ) );

            // Mark / unmark mail items.
            if ( item &&
                 item->ModelItemType() == ETypeMailItem &&
                 !iMailList->IsNode( iTreeItemArray[i].iListItemId ) )
                {
                if ( !iMailList->IsMarked( iTreeItemArray[i].iListItemId ))
                    {
                    // Att least one unmarked item found
                    ret = EFalse;
                    break;
                    }
                }
            else
                {
                // Stop iteration since another iterator was reached.
                break;
                }
            }
        }
    return ret;
    }

// Navigation functions, used mainly from viewer
TBool CFSEmailUiMailListVisualiser::IsNextMsgAvailable( TFSMailMsgId aCurrentMsgId,
														TFSMailMsgId& aFoundNextMsgId,
														TFSMailMsgId& aFoundNextMsgFolder ) const
	{
    FUNC_LOG;
	TBool ret(EFalse);
	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	if ( curIdx >= 0 )
	    {
	    TInt nextIdx = NextMessageIndex(curIdx);
	    if ( nextIdx >= 0 )
	        {
	        ret = ETrue;
	        aFoundNextMsgId = MsgIdFromIndex(nextIdx);
	        aFoundNextMsgFolder = iMailFolder->GetFolderId();
	        }
	    }

	return ret;
	}

TBool CFSEmailUiMailListVisualiser::IsPreviousMsgAvailable( TFSMailMsgId aCurrentMsgId,
															TFSMailMsgId& aFoundPreviousMsgId,
															TFSMailMsgId& aFoundPrevMsgFolder ) const
	{
    FUNC_LOG;
	TBool ret(EFalse);
	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	if ( curIdx >= 0 )
	    {
	    TInt prevIdx = PreviousMessageIndex(curIdx);
	    if ( prevIdx >= 0 )
	        {
	        ret = ETrue;
	        aFoundPreviousMsgId = MsgIdFromIndex(prevIdx);
	        aFoundPrevMsgFolder = iMailFolder->GetFolderId();
	        }
	    }

	return ret;
	}

TInt CFSEmailUiMailListVisualiser::MoveToNextMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId )
	{
    FUNC_LOG;
	TInt ret(KErrNotFound);

	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	TInt nextIdx = NextMessageIndex( curIdx );

	if ( curIdx >= 0 && nextIdx >= 0 )
	    {
	    // Focus the new message
	    // <cmail>
	    iMailTreeListVisualizer->SetFocusedItemL( iTreeItemArray[nextIdx].iListItemId, EFalse );
	    // </cmail>
	    ChangeReadStatusOfHighlightedL( ETrue );

        aFoundNextMsgId = MsgIdFromIndex( nextIdx );
	    ret = KErrNone;
	    }
	if ( ret == KErrNone )
		{
		OpenHighlightedMailL();
		}
	return ret;
	}

TInt CFSEmailUiMailListVisualiser::MoveToPreviousMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId )
	{
    FUNC_LOG;
	TInt ret(KErrNotFound);

	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	TInt prevIdx = PreviousMessageIndex( curIdx );

	if ( curIdx >= 0 && prevIdx >= 0 )
	    {
	    // Focus the new message
	    // <cmail>
	    iMailTreeListVisualizer->SetFocusedItemL( iTreeItemArray[prevIdx].iListItemId, EFalse );
	    // </cmail>
	    ChangeReadStatusOfHighlightedL( ETrue );
        aFoundPreviousMsgId = MsgIdFromIndex( prevIdx );
	    ret = KErrNone;
	    }

	if ( ret == KErrNone )
		{
		OpenHighlightedMailL();
		}
	return ret;
	}

TInt CFSEmailUiMailListVisualiser::MoveToPreviousMsgAfterDeleteL( TFSMailMsgId aFoundPreviousMsgId )
	{
	FUNC_LOG;
	TInt ret(KErrNotFound);

	TInt idx = ItemIndexFromMessageId( aFoundPreviousMsgId );
	if ( idx >= 0 )
		{
		// Focus the previous message
		iMailTreeListVisualizer->SetFocusedItemL( iTreeItemArray[idx].iListItemId, EFalse );
		ChangeReadStatusOfHighlightedL( ETrue );
		ret = KErrNone;
		}

	if ( ret == KErrNone )
		{
		OpenHighlightedMailL();
		}

	return ret;
	}

void CFSEmailUiMailListVisualiser::ManualMailBoxSync( TBool aManualMailBoxSync )
	{
    FUNC_LOG;
	iManualMailBoxSync = aManualMailBoxSync;
	}

TBool CFSEmailUiMailListVisualiser::GetLatestSyncState()
	{
    FUNC_LOG;
	CFSMailBox* activeMailbox = iAppUi.GetActiveMailbox();

    TBool ret = EFalse;

    if ( activeMailbox )
        {
    	TSSMailSyncState latestSyncstate = activeMailbox->CurrentSyncState();
    	if(latestSyncstate == InboxSyncing ||
    	   latestSyncstate == StartingSync ||
    	   latestSyncstate == EmailSyncing ||
    	   latestSyncstate == OutboxSyncing ||
    	   latestSyncstate == SentItemsSyncing ||
    	   latestSyncstate == DraftsSyncing ||
    	   latestSyncstate == CalendarSyncing ||
    	   latestSyncstate == ContactsSyncing ||
    	   latestSyncstate == TasksSyncing ||
    	   latestSyncstate == NotesSyncing ||
    	   latestSyncstate == FilesSyncing ||
    	   latestSyncstate == DataSyncStarting )
    		{
    		ret = ETrue;
    		}
        }

	return ret;
	}

void CFSEmailUiMailListVisualiser::ConnectionIconHandling()
    {
    FUNC_LOG;
    iAppUi.UpdateTitlePaneConnectionStatus();
    }

void CFSEmailUiMailListVisualiser::SetListAndCtrlBarFocusL()
    {
    FUNC_LOG;
    if ( iFocusedControl == EMailListComponent )
        {
        iMailList->SetFocusedL( ETrue );
        iControlBarControl->SetFocusL( EFalse );
        }
    else
        {
        if( iStylusPopUpMenuVisible )
        	{
        	return;
        	}

        iMailList->SetFocusedL( EFalse );
        TInt focusedBtnId = KErrNotFound;
        MFsControlButtonInterface* focusedBtn = iControlBarControl->GetFocusedButton();
        if ( focusedBtn )
            {
            focusedBtnId = focusedBtn->Id();
            }
        iControlBarControl->SetFocusL( ETrue );
        if ( focusedBtnId != KErrNotFound )
            {
            iControlBarControl->SetFocusByIdL( focusedBtnId );
            }
        iControlBarControl->MakeSelectorVisible( IsFocusShown() );
        }
    }

void CFSEmailUiMailListVisualiser::GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    if ( iScreenAnchorLayout )
        {
        aLayoutArray.AppendL( iScreenAnchorLayout );
        }
    if ( iControlBarControl )
        {
        aLayoutArray.AppendL( iControlBarControl->Visual() );
        }
    }

// hide or show CAlfVisuals ( used for activation or deactivation )
void CFSEmailUiMailListVisualiser::FadeOut(TBool aDirectionOut)
	{
    FUNC_LOG;
	TAlfTimedValue timedValue( 0, 0 );
	if ( !aDirectionOut )
        {
		timedValue.SetTarget( 1, 0 );
		}
	if ( iScreenAnchorLayout != NULL )
		{
	    iScreenAnchorLayout->SetOpacity( timedValue );
		}
	if (iControlBarControl && iControlBarControl->Visual() != NULL )
        {
	    iControlBarControl->Visual()->SetOpacity( timedValue );
        }  
	}

// Sets aActiveMailboxId and aActiveFolderId from iMailFolder if available
TInt CFSEmailUiMailListVisualiser::GetActiveFolderId(TFSMailMsgId& aActiveMailboxId, TFSMailMsgId& aActiveFolderId) const
    {
    if (iMailFolder != NULL)
        {
        aActiveMailboxId = iMailFolder->GetMailBoxId();
        aActiveFolderId = iMailFolder->GetFolderId();
        return KErrNone;
        }
    else
        {
         return KErrNotFound;
        }
    }

void CFSEmailUiMailListVisualiser::CreateExtensionL()
    {
    CFSMailBox* box = iAppUi.GetActiveMailbox();
    CEmailExtension* ext=NULL;
    if (box)
        {
        ext = box->ExtensionL( KEmailMailboxStateExtensionUid );
        }
    if (ext)
        {
        iExtension = reinterpret_cast<CMailboxStateExtension*>( ext );
        iExtension->SetStateDataProvider( this );
        }
    }

TBool CFSEmailUiMailListVisualiser::CheckAutoSyncSettingL()
    {
    CEmailExtension* ext=NULL;
    TBool manualSync = EFalse;
    CFSMailBox* box = iAppUi.GetActiveMailbox();
    if (box)
        {
        ext = box->ExtensionL( KEmailSettingExtensionUid );
        if (ext)
            {
            CEmailSettingsExtension* extension = reinterpret_cast<CEmailSettingsExtension*>( ext );
            extension->SetMailBoxId(box->GetId());
            manualSync = extension->IsSetL(EmailSyncInterval);
            box->ReleaseExtension(extension);
            }
        }
    return manualSync;
    }


void CFSEmailUiMailListVisualiser::UpdateFolderAndMarkingModeTextsL()
    {
    if ( iAppUi.CurrentActiveView()->Id() == MailListId )
        {
        HBufC* folder = CreateFolderNameLC( iMailFolder );
        if( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            iFolderListButton->SetTextL( KNullDesC );
            if (iMarkingMode)
                {
                RemoveMarkingModeTextOnButtonsL();
                HBufC* txt = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MARKINGMODE );
                iAppUi.SetNaviPaneTextL( *txt );
                CleanupStack::PopAndDestroy( txt );
                }
            else
                {
                iAppUi.SetNaviPaneTextL( *folder );
                }
            }
        else // Portrait orientation
            {
            iAppUi.SetNaviPaneTextL( KNullDesC );        
            if (iMarkingMode)
                {
                DisplayMarkingModeTextOnButtonsL();
                }
            else
                {
                RemoveMarkingModeTextOnButtonsL();
                iFolderListButton->SetTextL( *folder );
                }        
            }
        CleanupStack::PopAndDestroy( folder );
        }
    else
        {
        iAppUi.SetNaviPaneTextL( KNullDesC );
        RemoveMarkingModeTextOnButtonsL();
        }
    }

//////////////////////////////////////////////////////////////////
// Class implementation CMailListUpdater
///////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// CMailListUpdater::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
CMailListUpdater* CMailListUpdater::NewL( CFSEmailUiMailListVisualiser* aMailListVisualiser )
    {
    FUNC_LOG;
    CMailListUpdater* self = new (ELeave) CMailListUpdater( aMailListVisualiser );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMailListUpdater::~CMailListUpdater
// D'tor
// -----------------------------------------------------------------------------
//
CMailListUpdater::~CMailListUpdater()
    {
    FUNC_LOG;
    Cancel();
    }

// -----------------------------------------------------------------------------
// CMailListUpdater::CMailListUpdater
// C'tor
// -----------------------------------------------------------------------------
//
CMailListUpdater::CMailListUpdater( CFSEmailUiMailListVisualiser* aMailListVisualiser  )
	: CTimer( EPriorityStandard ),
	iMailListVisualiser( aMailListVisualiser )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMailListUpdater::RunL
// Timer trigger function.
// -----------------------------------------------------------------------------
//
void CMailListUpdater::RunL()
    {
    FUNC_LOG;
    if ( iIsSorting )
    	{
	   	iMailListVisualiser->UpdateMailListModelL();
	   	iMailListVisualiser->RefreshL();
		// Sorting is completed
		iIsSorting = EFalse;
	   	// Dismiss wait note.
	   	if ( iWaitNote )
	   		{
		   	iWaitNote->ProcessFinishedL();
	   		}
    	}
    else
    	{
		// Store the list of marked items. The markings are erased when the list
		// is repopulated and, hence, the markings need to be reset after the
		// refreshing is done. The marked items must be identified by the message
		// ID rather than with the list ID because refreshing may change the list IDs.
		RArray<TFSMailMsgId> markedMessages;
		CleanupClosePushL( markedMessages );
		iMailListVisualiser->GetMarkedMessagesL( markedMessages );

		// Store the message ID of the focused item
	    TFSMailMsgId msgIdBeforeRefresh;
	   	TFsTreeItemId firstVisibleListId = iMailListVisualiser->iMailTreeListVisualizer->FirstVisibleItem();
	    TInt modelFirstIndexBeforeUpdate = iMailListVisualiser->ModelIndexFromListId( firstVisibleListId );
	  	TInt highlightedIndexBeforeUpdate = iMailListVisualiser->HighlightedIndex();
	    TInt modelCountBeforeUpdate = iMailListVisualiser->iModel->Count();
	    if ( iMailListVisualiser->iModel->Count() )
	    	{
	  		CFSEmailUiMailListModelItem* item =
				static_cast<CFSEmailUiMailListModelItem*>(iMailListVisualiser->iModel->Item( highlightedIndexBeforeUpdate ) );
			if ( item && item->ModelItemType() == ETypeMailItem )
				{
			    msgIdBeforeRefresh = item->MessagePtr().GetMessageId();
				}
	    	}

	   	iMailListVisualiser->UpdateMailListModelL();
	   	iMailListVisualiser->RefreshL( &msgIdBeforeRefresh );

		// Focus is set to last focused item. After this, try to keep the same first item that previous
		// Check how many messages have been added
	  	TInt modelIndexDifference = highlightedIndexBeforeUpdate-modelFirstIndexBeforeUpdate; // 0 or more
		if ( modelIndexDifference < 0 )
			{
			modelIndexDifference = 0; // safety
			}
		TInt toBeFirstIdx = iMailListVisualiser->HighlightedIndex() - modelIndexDifference;
		CFSEmailUiMailListModelItem* toBeFirstVisItem =
			static_cast<CFSEmailUiMailListModelItem*>( iMailListVisualiser->iModel->Item(toBeFirstIdx) );
		if ( toBeFirstVisItem )
			{
			// <cmail>
			iMailListVisualiser->iMailTreeListVisualizer->SetFirstVisibleItemL( toBeFirstVisItem->CorrespondingListId() );
			// </cmail>
			}

		// Restore the marking status
		iMailListVisualiser->MarkMessagesIfFoundL( markedMessages );
		CleanupStack::PopAndDestroy( &markedMessages );
    	}
    Cancel();
   }

// -----------------------------------------------------------------------------
// CMailListUpdater::Start
// Timer starting function.
// -----------------------------------------------------------------------------
//
void CMailListUpdater::StartL( TBool aIsSorting )
    {
    FUNC_LOG;
    if ( iWaitNote )
    	{
    	iWaitNote->ProcessFinishedL();
    	}

    iIsSorting = aIsSorting;
    Cancel();
    if ( iIsSorting )
    	{
		// Start wait note
    	TFsEmailUiUtility::ShowWaitNoteL( iWaitNote, R_FSE_WAIT_SORTING_TEXT, EFalse, ETrue );
	 	After( 10 ); // Update shortly after selection has been made.
    	}
    else
    	{
    	 // Mail added update
	 	After( KMsgUpdaterTimerDelay ); // Update after 1,5 seconds
    	}
   	}

// -----------------------------------------------------------------------------
// CMailListUpdater::Stop
// Timer stopping function
// -----------------------------------------------------------------------------
//
void CMailListUpdater::Stop()
    {
    FUNC_LOG;
    Cancel();
    }



//////////////////////////////////////////////////////////////////
// Class implementation CMsgMovedNoteTimer
///////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
CMsgMovedNoteTimer* CMsgMovedNoteTimer::NewL(  CFreestyleEmailUiAppUi* aAppUi, CFSEmailUiMailListVisualiser* aMailListVisualiser  )
    {
    FUNC_LOG;
    CMsgMovedNoteTimer* self = new (ELeave) CMsgMovedNoteTimer( aAppUi, aMailListVisualiser );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::CMsgMovedNoteTimer
// D'tor
// -----------------------------------------------------------------------------
//
CMsgMovedNoteTimer::~CMsgMovedNoteTimer()
    {
    FUNC_LOG;
    Cancel();
    }

// -----------------------------------------------------------------------------
// CMailListUpdater::CMailListUpdater
// C'tor
// -----------------------------------------------------------------------------
//
CMsgMovedNoteTimer::CMsgMovedNoteTimer(  CFreestyleEmailUiAppUi* aAppUi, CFSEmailUiMailListVisualiser* aMailListVisualiser   )
	: CTimer( EPriorityStandard ),
	iAppUi( aAppUi),
	iMailListVisualiser( aMailListVisualiser )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::RunL
// Timer trigger function.
// -----------------------------------------------------------------------------
//
void CMsgMovedNoteTimer::RunL()
    {
    FUNC_LOG;
 	TFsEmailUiUtility::DisplayMsgsMovedNoteL( iMsgCount, iDestinationFolderId, EFalse );
    Cancel();
    }

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::Start
// Timer starting function.
// -----------------------------------------------------------------------------
//
void CMsgMovedNoteTimer::Start(  TInt aMsgCount, const TFSMailMsgId aDestinationFolderId  )
    {
    FUNC_LOG;
    Cancel();
	iMsgCount = aMsgCount,
	iDestinationFolderId = aDestinationFolderId;
	TInt viewSlideinTime = iAppUi->LayoutHandler()->ViewSlideEffectTime();
	if ( viewSlideinTime == 0 )
		{
		viewSlideinTime = 100;
		}
 	After( viewSlideinTime );
   	}

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::Stop
// Timer stopping function
// -----------------------------------------------------------------------------
//
void CMsgMovedNoteTimer::Stop()
    {
    FUNC_LOG;
    Cancel();
    }


//////////////////////////////////////////////////////////////////
// Class implementation CDateChnageTimer
///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::NewL
// Two phased constructor
// -----------------------------------------------------------------------------
//
CDateChangeTimer* CDateChangeTimer::NewL( CFSEmailUiMailListVisualiser& aMailListVisualiser )
    {
    FUNC_LOG;
    CDateChangeTimer* self = new ( ELeave ) CDateChangeTimer( aMailListVisualiser );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::CDateChangeTimer
// First phase constructor
// -----------------------------------------------------------------------------
//
CDateChangeTimer::CDateChangeTimer( CFSEmailUiMailListVisualiser& aMailListVisualiser )
    : CTimer( EPriorityStandard ), iMailListVisualiser( aMailListVisualiser )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    iDayCount = DayCount();
    }

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::~CDateChangeTimer
// Destructor
// -----------------------------------------------------------------------------
//
CDateChangeTimer::~CDateChangeTimer()
    {
    FUNC_LOG;
    // no implementation needed
    }

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::Start
// Request event at 00:00 tomorrow
// -----------------------------------------------------------------------------
//
void CDateChangeTimer::Start()
    {
    FUNC_LOG;
    TTime now;
    now.HomeTime();

    // Construct target time
    TTime eventTime = now + TTimeIntervalDays( 1 );
    TDateTime eventDateTime = eventTime.DateTime();
    eventDateTime.SetHour(0);
    eventDateTime.SetMinute(0);
    eventDateTime.SetSecond(0);
    eventDateTime.SetMicroSecond(0);
    eventTime = TTime( eventDateTime );

    // Issue the request
    At( eventTime );
    }

// -----------------------------------------------------------------------------
// CMsgMovedNoteTimer::RunL
// Function gets called when system time reaches the 00:00 in the next day or
// when the system time has been changed by the user. Both cases can be handled
// in the same way.
// -----------------------------------------------------------------------------
//
void CDateChangeTimer::RunL()
    {
    FUNC_LOG;

    if (iStatus.Int() != KErrNone)
        {
        	INFO_1("### CDateChangeTimer::RunL (err=%d) ###", iStatus.Int());
        }


    TBool dayChanged = EFalse;
    TInt dayCount = DayCount();
    if (dayCount != iDayCount)
        {

        iDayCount = dayCount;
        dayChanged = ETrue;
        }


    if ( KErrCancel == iStatus.Int() )
        {
        ;
        }
    else if ( KErrAbort == iStatus.Int() ) // System time changed
        {
        if (dayChanged)
            {
            TRAP_IGNORE( iMailListVisualiser.NotifyDateChangedL() );
            }
        Start();
        }
    else  // interval is over
        {
        // Update mail list and reissue the request for timer event
        TRAP_IGNORE( iMailListVisualiser.NotifyDateChangedL() );
        Start();
        }

    }


TInt CDateChangeTimer::DayCount()
    {
    TTime now;
    now.HomeTime();
    TTime minTime = Time::MinTTime();
    TTimeIntervalDays days = now.DaysFrom(minTime);
    return days.Int();
    }








