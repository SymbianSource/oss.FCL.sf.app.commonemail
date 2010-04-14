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
* Description:  FreestyleEmailUi main application UI control implementation
*
*/

// SYSTEM INCLUDE FILES
#include "emailtrace.h"
#include <avkon.hrh>
#include <eikmenup.h>
#include <AvkonInternalCRKeys.h>
#include <AknDef.h>
#include <aknnotewrappers.h>
#include <AknUtils.h>
#include <aknnavide.h>
// <cmail> SF
#include <alf/alfcontrolgroup.h>
// </cmail>
#include <apgcli.h>
#include <FreestyleEmailUi.rsg>
#include <fsmailserver.rsg>
#include <StringLoader.h>
//<cmail>
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cfsmailfolder.h"
#include "cfsmailmessage.h"
#include "cfsmailaddress.h"
#include "cfsmailcommon.h"
//</cmail>
#include <freestyleemailui.mbg>
#include <AknQueryDialog.h>
#include <AknBidiTextUtils.h>
#include <gdi.h>
#include <s32file.h>
#include <AknInfoPopupNoteController.h>
//<cmail>
// #include <AGMDATE.H>
//</cmail>
#include <CPbkContactItem.h>
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <cntviewbase.h>
#include <cntdef.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkContactStoreUris.h>       // VPbkContactStoreUris
#include <TVPbkContactStoreUriPtr.h>    // TVPbkContactStoreUriPtr
#include <e32uid.h>	// For dynamic library UID
// Meeting request
#include <MeetingRequestUids.hrh>
//<cmail>
#include "cesmricalviewer.h"
#include "fslayoutmanager.h"
//</cmail>
#include <etelmm.h>

#include "FSEmailBuildFlags.h"

#include <CPbkPhoneNumberSelect.h>
#include <RPbkViewResourceFile.h>
#include <commonphoneparser.h>
#include <bautils.h>
#include <hlplch.h>
#include <DocumentHandler.h>
#include <centralrepository.h>
#include <cstack.h>
#include <akntitle.h>
#include <eikspane.h>
#include <fbs.h>
#include <iaupdate.h>                   // For IAD
#include <iaupdateparameters.h>         // For IAD
#include <iaupdateresult.h>             // For IAD
#include <featmgr.h>                    // For FeatureManager
//<cmail>
#include <data_caging_path_literals.hrh> // hardcoded paths removal from cmail
#include "cfsccontactactionmenu.h"
//</cmail>
#include <layoutmetadata.cdl.h>         // for Layout_Meta_Data

// INTERNAL INCLUDE FILES
#include "FreestyleEmailUiContactHandler.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiLauncherGrid.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiMailListControl.h"
#include "FreestyleEmailUiMailListModel.h"
#include "FreestyleEmailUiMailListVisualiser.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiMsgDetailsVisualiser.h"
#include "FreestyleEmailUiFolderListVisualiser.h"
#include "ncsconstants.h"
#include "ncscomposeview.h"
#include "FreestyleEmailUiAttachmentsListModel.h"
#include "FreestyleEmailUiAttachmentsListVisualiser.h"
#include "FreestyleEmailUiAttachmentsListControl.h"
#include "freestyleemailcenrephandler.h"
#include "FreestyleEmailUiSettingsListView.h"
#include "FreestyleEmailUiGlobalSettingsListView.h"
#include "FreestyleEmailUiDownloadManagerModel.h"
#include "FreestyleEmailUiDownloadManagerVisualiser.h"
#include "FreestyleEmailUiDownloadManagerControl.h"
#include "FreestyleEmailUiSendAttachmentsListModel.h"
#include "FreestyleEmailUiSendAttachmentsListVisualiser.h"
#include "FreestyleEmailUiSearchListVisualiser.h"
#include "FreestyleEmailUiStatusIndicator.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FreestyleEmailDownloadInformationMediator.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FSDelayedLoader.h"
#include "FreestyleEmailUiWizardObserver.h"
#include "FSEmail.pan"
#include "ceuiexitguardian.h"

#include "FreestyleEmailUiNaviPaneControl2MailViewer.h"

// CONSTANT VALUES
//<cmail> //hard coded paths removed
_LIT( KETelCallEngPhCltResourceFile, "PhoneClient.rsc" );
//</cmail>
_LIT( KMsgReaderFsDll,"freestylemessagescanner.dll"); // DLL provided by msg reader.
_LIT( KPhoneModuleName, "Phonetsy.tsy");
_LIT( KPhoneName, "DefaultPhone");
// Message editor resource file with wild cards
_LIT( KMsgEditorAppUiResourceFileName, "msgeditorappui.r*" );
// FSMailServer resource file with path and wild cards
_LIT( KFSMailServerResourceFileNameWithPath, "\\resource\\apps\\fsmailserver.r*" );

// 6 seconds
const TTimeIntervalMicroSeconds32 KIdleTimeInterval = 6000000;

static TBool g_ApplicationExitOnGoing = EFalse;

TInt DelayedViewLoaderCallBackL( TAny* aObject )
    {
    return static_cast<CFreestyleEmailUiAppUi*>
    	( aObject )->ViewLoadIdleCallbackFunctionL();
    }

const TUint KConnectionStatusIconRotationInterval = 100;
const TInt KConnectionStatusIconRotationAmount = 18;
// Length of the drive letter descriptor (e.g. "c:")
const TInt KDriveDescLength = 2;


// TDisplayImagesCache

// ---------------------------------------------------------------------------
// TDisplayImagesCache::~TDisplayImagesCache
// ---------------------------------------------------------------------------
//
TDisplayImagesCache::~TDisplayImagesCache()
    {
    iCache.Reset();
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::AddMessageL
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::AddMessageL( const CFSMailMessageBase& aMsg )
    {
    AddMessageL(aMsg.GetMailBoxId(), aMsg.GetMessageId());
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::RemoveMessage
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::RemoveMessage( const CFSMailMessageBase& aMsg )
    {
    RemoveMessage(aMsg.GetMailBoxId(), aMsg.GetMessageId());
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::Contains
// ---------------------------------------------------------------------------
//
TBool TDisplayImagesCache::Contains( const CFSMailMessageBase& aMsg ) const
    {
    return Contains(aMsg.GetMailBoxId(), aMsg.GetMessageId());
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::AddMessageL
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::AddMessageL( const TFSMailMsgId& aBoxId, const TFSMailMsgId& aMsgId )
    {
    TInt index(MailBoxIndex(aBoxId));
    if (index == KErrNotFound)
        {
        AddMailBoxL(aBoxId, index);
        }
    iCache[index].AddMessageL(aMsgId);
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::RemoveMessage
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::RemoveMessage( const TFSMailMsgId& aBoxId, const TFSMailMsgId& aMsgId )
    {
    const TInt index(MailBoxIndex(aBoxId));
    if (index != KErrNotFound)
        {
        iCache[index].RemoveMessage(aMsgId);
        }
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::RemoveMailbox
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::RemoveMailbox( const TFSMailMsgId& aBoxId )
    {
    const TInt index(MailBoxIndex(aBoxId));
    if (index != KErrNotFound)
        {
        iCache.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::Contains
// ---------------------------------------------------------------------------
//
TBool TDisplayImagesCache::Contains( const TFSMailMsgId& aBoxId, const TFSMailMsgId& aMsgId ) const
    {
    const TInt index(MailBoxIndex(aBoxId));
    if (index != KErrNotFound)
        {
        return iCache[index].Contains(aMsgId);
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::MailBoxIndex
// ---------------------------------------------------------------------------
//
TInt TDisplayImagesCache::MailBoxIndex( const TFSMailMsgId& aBoxId ) const
    {
    return iCache.FindInOrder(TItem(aBoxId), TLinearOrder<TItem>(TItem::CompareItem));
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::AddMailBoxL
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::AddMailBoxL( const TFSMailMsgId& aBoxId, TInt& aCacheIndex )
    {
    aCacheIndex = KErrNotFound;
    iCache.InsertInOrderL(TItem(aBoxId), TLinearOrder<TItem>(TItem::CompareItem));
    aCacheIndex = MailBoxIndex(aBoxId);
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::TItem
// ---------------------------------------------------------------------------
//
TDisplayImagesCache::TItem::TItem( const TFSMailMsgId& aBoxId )
    : iMailBoxId( aBoxId )
    {
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::~TItem
// ---------------------------------------------------------------------------
//
TDisplayImagesCache::TItem::~TItem()
    {
    iMessageIds.Reset();
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::Contains
// ---------------------------------------------------------------------------
//
TBool TDisplayImagesCache::TItem::Contains( const TFSMailMsgId& aMsgId ) const
    {
    return MessageIndex(aMsgId) != KErrNotFound;
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::AddMessageL
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::TItem::AddMessageL( const TFSMailMsgId& aMsgId )
    {
    if (!Contains(aMsgId))
        {
        iMessageIds.InsertInOrderL(aMsgId, TLinearOrder<TFSMailMsgId>(TItem::CompareMsgId));
        }
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::RemoveMessage
// ---------------------------------------------------------------------------
//
void TDisplayImagesCache::TItem::RemoveMessage( const TFSMailMsgId& aMsgId )
    {
    const TInt index(MessageIndex(aMsgId));
    if (index != KErrNotFound)
        {
        iMessageIds.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::MessageIndex
// ---------------------------------------------------------------------------
//
TInt TDisplayImagesCache::TItem::MessageIndex( const TFSMailMsgId& aMsgId ) const
    {
    return iMessageIds.FindInOrder(aMsgId, TLinearOrder<TFSMailMsgId>(TItem::CompareMsgId));
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::CompareItem
// ---------------------------------------------------------------------------
//
TInt TDisplayImagesCache::TItem::CompareItem( const TItem& aItem1, const TItem& aItem2 )
    {
    return CompareMsgId(aItem1.iMailBoxId, aItem2.iMailBoxId);
    }

// ---------------------------------------------------------------------------
// TDisplayImagesCache::TItem::CompareMsgId
// ---------------------------------------------------------------------------
//
TInt TDisplayImagesCache::TItem::CompareMsgId( const TFSMailMsgId& aId1, const TFSMailMsgId& aId2 )
    {
    TInt result(aId1.PluginId().iUid - aId2.PluginId().iUid);
    if (!result)
        {
        result = TInt(aId1.Id() - aId2.Id());
        }
    return result;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiAppUi::CFreestyleEmailUiAppUi( CAlfEnv& aEnv )
    : CAknViewAppUi(),
    iIsForeground( EFalse ), // init foreground flag to "not on foreground"
    iIsCaptured( EFalse ), // init "end key captured" to false
    iAutomatedMailBoxOnline( EFalse ),
    iMsgReaderIsSupported( KErrGeneral ),
    iSwitchingToBackground( EFalse ),
    iHasEmbeddedApp( EFalse ),
    iPreviousAppEmbedded( EFalse ),
    iEditorStartedFromEmbeddedApp( EFalse )
    {
    FUNC_LOG;
    iEnv = &aEnv;
    TIMESTAMP( "Application starting" );
    }

// ---------------------------------------------------------------------------
// Load specified resource file, file name may (and should) contain wild cards
// ---------------------------------------------------------------------------
//
TInt CFreestyleEmailUiAppUi::LoadResourceFileL(
    const TDesC& aFileName,
    const TDesC& aFilePath )
    {
    FUNC_LOG;

    TFindFile search( iCoeEnv->FsSession() );
    CDir* dirList = NULL;
    TInt err = search.FindWildByDir( aFileName, aFilePath, dirList );

    if ( err == KErrNone )
        {
        // After succesfull FindWildByDir; dirList contains list of found file
        // names and search.File() returns the drive and path of the file(s)
        if ( dirList->Count() > 0 )
            {
            TParse parse;
            parse.Set( (*dirList)[0].iName, &search.File(), NULL );
            TFileName nearestFile( parse.FullName() );
            BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), nearestFile );
            delete dirList;

            INFO_1( "Load resource: %S", &nearestFile );
            // Return the resource file offset if file loaded succesfully
            return iEikonEnv->AddResourceFileL( nearestFile );
            }
        delete dirList;
        }

    // Leave if resource file is not found
    User::Leave( err );
    return 0; // To avoid compiler warnings
    }

// ---------------------------------------------------------------------------
// ConstructL is called by the application framework
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiAppUi::ConstructL()
    {
    FUNC_LOG;

    // Load message editor resources (needed for ENote fetch)
    iMsgEditorResourceOffset = LoadResourceFileL(
                                   KMsgEditorAppUiResourceFileName,
                                   KDC_RESOURCE_FILES_DIR );

    // Load FSMailServer resource file, search first the same drive from where
    // our process is loaded as FSMailServer should be installed in same drive
    RProcess ownProcess;
    TFileName ownProcessName( ownProcess.FileName() );
    ownProcess.Close();
    iFSMailServerResourceOffset = LoadResourceFileL(
                                      KFSMailServerResourceFileNameWithPath,
                                      ownProcessName.LeftTPtr( KDriveDescLength ) );

	// flag indicating whether we are in AppUi::ConstructL
	// workaround for compose view loadbackgroundcontext sending skin changed events
	// and causing texture manager to be reconstructed many times
	iConstructComplete = EFalse;

	// <cmail>
	iIADUpdateCompleted = !FeatureManager::FeatureSupported( KFeatureIdIAUpdate );
	// </cmail>

    // Create the 'Back' stack
    iNavigationHistory = new (ELeave) CStack<CFsEmailUiViewBase, EFalse>();

    // Enable Avkon skins.
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );

    // This MUST be after BaseConstructL
    MTouchFeedback* touchFeedback(MTouchFeedback::Instance());
    if (touchFeedback == NULL)
        {
        touchFeedback = MTouchFeedback::CreateInstanceL();
        iTouchFeedbackCreated = ETrue;
        }
    touchFeedback->SetFeedbackEnabledForThisApp(touchFeedback->TouchFeedbackSupported());


    // Create instance for Central repository handler
    iCRHandler = CFSEmailCRHandler::InstanceL();

    // Initialize the flip state and the observer for listening to the flip
    // state changed events. The initial state will be reported in construction
    iPropertySubscriber =
        CFreestyleEmailUiPropertySubscriber::NewL( KPSUidHWRM, KHWRMFlipStatus, *this );

    // Create Document Handler instance
    iDocumentHandler = CDocumentHandler::NewL();
    iFileHandleShutter = CFsEmailFileHandleShutter::NewL(*iDocumentHandler);

    CAlfDisplay& disp = iEnv->NewDisplayL( ClientRect(), CAlfEnv::ENewDisplayAsCoeControl ); // aAppUi->

 	// SET to use skin bakcground if needed
    disp.SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);

    // Create and load fs generci layout
    //<cmail>
    //iLayoutManager = new( ELeave )CFsLayoutManager();
    //iLayoutManager->LoadLayoutL();
    //<cmail>

    // Set up layouthandler
    iLayoutHandler = CFSEmailUiLayoutHandler::NewL( *iEnv );

    // Create mail client, list mailboxes
    iMailClient = CFSMailClient::NewL();

	// Connect to messaging server
	iMsvSession = CMsvSession::OpenSyncL( *this );

 	// Create freestyle texture manager
	iFsTextureManager = CFreestyleEmailUiTextureManager::NewL( *iEnv, this );
	CFsDelayedLoader::InstanceL()->Start();

	// New mailbox query async callback
 	iNewBoxQueryAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
	iNewBoxQueryAsyncCallback->Set( TCallBack( DisplayCreateQueryL, this ) );

    iActionMenuHandler = CFSEmailUiActionMenu::NewL( this );

  	// Create download info mediator
  	iDwnldMediator = CFSEmailDownloadInfoMediator::NewL( *iMailClient );

    // Create a control groups
    iGridControlGroup = &iEnv->NewControlGroupL( KGridDisplayGroup );

	// Create main UI grid and append it to control group
    TInt gridRows = iLayoutHandler->GridRowsInThisResolution();
	TInt gridColumns = iLayoutHandler->GridColumnsInThisResolution();
    iMainUiGrid = CFSEmailUiLauncherGrid::NewL(*iEnv, this );
    iMainUiGridVisualiser = CFSEmailUiLauncherGridVisualiser::NewL( *iEnv, iMainUiGrid, this,
    	*iGridControlGroup, gridRows, gridColumns );
   	iMainUiGrid->SetVisualiserL( iMainUiGridVisualiser );
    iGridControlGroup->AppendL(iMainUiGrid);
    //disp.iVisibleAreaObservers.AppendL(*iMainUiGridVisualiser); // Commented out as (hopefully) unnecessary, can't be accessed directly in Alfred anyway
    AddViewL( iMainUiGridVisualiser );

    // Create control groups
	iMailListControlGroup = &iEnv->NewControlGroupL( KMailListDisplayGroup );
	iEmailViewerControlGroup = &iEnv->NewControlGroupL( KEmailViewerDisplayGroup );
	iFolderListControlGroup = &iEnv->NewControlGroupL( KFolderListDisplayGroup );
	iSettingsListControlGroup = &iEnv->NewControlGroupL( KSettingsListDisplayGroup );
	iComposerControlGroup = &iEnv->NewControlGroupL( KEmailEditorDisplayGroup );
	iStatusIndicatorControlGroup = &iEnv->NewControlGroupL( KStatusIndicatorDisplayGroup );

	// Create settings list views
	CFsEmailSettingsListView* settingsListView = CFsEmailSettingsListView::NewLC( *iMailClient, this, *iSettingsListControlGroup );
	AddViewL( settingsListView );
	CleanupStack::Pop( settingsListView );
	CFsEmailUiGlobalSettingsListView* globalSettingsView = CFsEmailUiGlobalSettingsListView::NewLC( this, *iSettingsListControlGroup );
	AddViewL( globalSettingsView );
	CleanupStack::Pop( globalSettingsView );

    // Create and start view load in idle. Loads rest of the views when in idle.
    iViewLoadIdle = CIdle::NewL( CActive::EPriorityIdle );
    iViewLoadIdleCallback = new (ELeave) TCallBack( DelayedViewLoaderCallBackL, this );
    iViewLoadIdle->Start( *iViewLoadIdleCallback );

    // Create html viewer
    iHtmlViewerControlGroup = &iEnv->NewControlGroupL( KHtmlViewerDisplayGroup );
    iHtmlViewerView = CFsEmailUiHtmlViewerView::NewL( *iEnv, *this, *iHtmlViewerControlGroup );
    AddViewL( iHtmlViewerView );

	// Create mail list
    iMailListVisualiser = CFSEmailUiMailListVisualiser::NewL(*iEnv, this, *iMailListControlGroup );
 	AddViewL( iMailListVisualiser );

  	// Create mail composing view
	iComposeView = CNcsComposeView::NewL( *iMailClient, *iEnv, this,
						*iComposerControlGroup, *iMsvSession );
	AddViewL( iComposeView );

	// Create folder list view
	iFolderListVisualiser = CFSEmailUiFolderListVisualiser::NewL( *iEnv, *iFolderListControlGroup, *this );
	AddViewL( iFolderListVisualiser );

	// Create status indicator
	iStatusIndicator = CFSEmailUiStatusIndicator::NewL( *iEnv,
                                                 		*iStatusIndicatorControlGroup,
                                                 		this );


 	iCurrentActiveView = iMainUiGridVisualiser;
    SetDefaultViewL( *iMainUiGridVisualiser );


    // add this class to listen mail events, e.g. TFSEventNewMailbox
    iMailClient->AddObserverL( *this );

    iShortcutBinder = CFSEmailUiShortcutBinding::NewL();

    // Try to ensure that FSMailServer is running before creating the mailbox
    TFsEmailUiUtility::EnsureFsMailServerIsRunning( iEikonEnv->WsSession() );

    // If we have no mailboxes yet start a setup wizard. Else, if this app has been launched
	// by a setup wizard then handle the mailbox data in central repository
	iWizardObserver = CFSEmailUiWizardObserver::NewL( this, iMainUiGridVisualiser );
	iConstructComplete = ETrue;

    iExitGuardian = CEUiExitGuardian::NewL( *this );

    iConnectionStatusVisible = ETrue;
    // Create custom statuspane indicators object, which shows priority and followup flags
    CreateStatusPaneIndicatorsL();
    iConnectionStatusIconAnimTimer = CFSEmailUiGenericTimer::NewL(this, CActive::EPriorityLow);

    // this includes a wait note where code running will be pending
    // until a certain callback event is received
    // -> thus keep this last in this method!
    iWizardObserver->DoWizardStartupActionsL();
    TIMESTAMP( "Application started" );
    }

// Functions loads some views as delayed to reduce startup time
TInt CFreestyleEmailUiAppUi::ViewLoadIdleCallbackFunctionL()
	{
    FUNC_LOG;
    // Load the phone resources into this process
	//<cmail> hard coded paths removal
	TFileName phCltResPath(KDC_RESOURCE_FILES_DIR);
    phCltResPath.Append(KETelCallEngPhCltResourceFile);
    //</cmail>
    BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), phCltResPath );
    iPhCltResHandle = iEikonEnv->AddResourceFileL( phCltResPath );

	// Create attachment list visualiser and control
	iAttachmentControlGroup = &iEnv->NewControlGroupL( KAttachmentManagerDisplayGroup );
	iAttachmentListVisualiser = CFSEmailUiAttachmentsListVisualiser::NewL( *iEnv, this, *iAttachmentControlGroup );
	AddViewL( iAttachmentListVisualiser );

    // Download manager removed.

	// Create search list view
	iSearchResultControlGroup = &iEnv->NewControlGroupL( KSearchResultDisplayGroup );
	iSearchListVisualiser = CFSEmailUiSearchListVisualiser::NewL( *iEnv, this, *iSearchResultControlGroup );
	AddViewL( iSearchListVisualiser );

	// Create message details view
	iMsgDetailsControlGroup = &iEnv->NewControlGroupL( KMsgDetailsViewerControlGroup );
	iMsgDetailsVisualiser = CFSEmailUiMsgDetailsVisualiser::NewL( *iEnv, *iMsgDetailsControlGroup, *this );
	AddViewL( iMsgDetailsVisualiser );

    // Check updates from IAD, continue UI launching even if something fails there
    TRAP_IGNORE( CheckUpdatesL() );

    // Make sure there's no temp files left from previous sessions
    TFsEmailUiUtility::EraseTempDir();

	return KErrNone;
	}

CFreestyleEmailUiAppUi::~CFreestyleEmailUiAppUi()
    {
    FUNC_LOG;

    if ( iMsgEditorResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iMsgEditorResourceOffset );
        }
    if ( iFSMailServerResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iFSMailServerResourceOffset );
        }
    // Make sure we don't leave any temp files
    TFsEmailUiUtility::EraseTempDir();

    delete iExitGuardian;
    delete iUpdateClient;
    delete iUpdateParameters;
    delete iNewBoxQueryAsyncCallback;
    delete iVpkContactManager;
	delete iPropertySubscriber;

    DeleteStatusPaneIndicators();

	if ( iAutoSyncMonitor )
		{
		iAutoSyncMonitor->StopMonitoring();
		delete iAutoSyncMonitor;
		}

    if ( iViewLoadIdle )
        {
        iViewLoadIdle->Cancel();
        delete iViewLoadIdle;
        }
    if ( iViewLoadIdleCallback )
        {
        delete iViewLoadIdleCallback;
        }

    CCoeEnv::Static()->DeleteResourceFile( iPhCltResHandle );

    delete iLayoutHandler;
    iLayoutHandler = NULL;

    delete iMsvSession;
    iMsvSession = NULL;

    delete iWizardObserver;
    iWizardObserver = NULL;

    if ( iCRHandler )
    	{
    	// just to make sure:
    	// clears the two centrep keys that were passed as parameters to plugins
        iCRHandler->ClearWizardParams();
    	delete iCRHandler;
    	iCRHandler = NULL;
    	}

    if (iCustomMessageToMessageReader)
    	{
    	delete iCustomMessageToMessageReader;
    	}

   	delete iActionMenuHandler;
   	iActionMenuHandler = NULL;

    delete iShortcutBinder;
    iShortcutBinder = NULL;

    delete iDocumentHandler;
    iDocumentHandler = NULL;

    delete iFileHandleShutter;

	delete iFsTextureManager;
	iFsTextureManager = NULL;
    //<cmail> layout manager deltion removed </cmail>
    if ( iFeatureManagerInitialized )
        {
        // We can safely call UnInitializeLib as we have really intialized it!
        FeatureManager::UnInitializeLib();  // Decreases ref.count
        iFeatureManagerInitialized = EFalse;
        }

    delete iNavigationHistory;
    iNavigationHistory = NULL;

	// CAlfEnv creation and deletion was moved from here to Document class
	// because of cross-dependency between alf and S60 view architecture ->
	// AlfEnv must still exist when AppUI is destructed.
	// However, CAlfDisplay's destructor has a dependency to AppUI. That's
	// why display must be deleted explicitly here, when AppUI still exists.
	// Otherwise "delete env" in CDocument panics.
    CAlfDisplay *disp = &iEnv->PrimaryDisplay();
 	if ( disp )
 		{
 		delete disp;
 		}

 	TFsEmailUiUtility::DeleteStaticData();

    // destroys the Download Information mediator
    // Destruction must be done here as other Tls data depends on it.
    CFSEmailDownloadInfoMediator::Destroy();

    delete iNaviDecorator2MailViewer;

    delete iConnectionStatusIconAnimTimer;


    MTouchFeedback* touchFeedback( MTouchFeedback::Instance() );
    if (touchFeedback && touchFeedback->TouchFeedbackSupported())
        {
        touchFeedback->SetFeedbackEnabledForThisApp(EFalse);
        }

    if (iTouchFeedbackCreated)
        {
        MTouchFeedback::DestroyInstance();
        }

    TIMESTAMP( "Application exit" );
    }


void CFreestyleEmailUiAppUi::EnterFsEmailViewL( TUid aViewId )
    {
    FUNC_LOG;
    if ( AppUiExitOngoing() )
        {
        return;
        }

    iPreviousActiveView = iCurrentActiveView;

    // First check if the view to be activated is the current view. In that case,
    // the view history stack is not altered.
    if ( iCurrentActiveView->Id() == aViewId )
        {
        ActivateLocalViewL( aViewId );
        }

    // Then check if the view is in the history stack. In that case, we don't
    // actually navigate forward but backward.
    else if ( IsViewInHistoryStack( aViewId ) )
        {
        ReturnToViewL( aViewId );
        }

    // Otherwise this is just the normal case of entering some new view
    else
        {
    	iNavigationHistory->PushL( iCurrentActiveView );
        ActivateLocalViewL( aViewId );
        iCurrentActiveView = static_cast<CFsEmailUiViewBase*>( View(aViewId) );
        }
    }

void CFreestyleEmailUiAppUi::EnterFsEmailViewL( TUid aViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    FUNC_LOG;
    if ( AppUiExitOngoing() )
        {
        return;
        }

    iPreviousActiveView = iCurrentActiveView;

    // First check if the view to be activated is the current view. In that case,
    // the view history stack is not altered.
    if ( iCurrentActiveView->Id() == aViewId )
        {
        ActivateLocalViewL( aViewId, aCustomMessageId, aCustomMessage );
        }

    // Then check if the view is in the history stack. In that case, we don't
    // actually navigate forward but backward.
    else if ( IsViewInHistoryStack( aViewId ) )
        {
        ReturnToViewL( aViewId, aCustomMessageId, aCustomMessage );
        }

    // Otherwise this is just the normal case of entering some new view
    else
        {
    	iNavigationHistory->PushL( iCurrentActiveView );
        ActivateLocalViewL( aViewId, aCustomMessageId, aCustomMessage );
        iCurrentActiveView = static_cast<CFsEmailUiViewBase*>( View(aViewId) );
        }
    }

void CFreestyleEmailUiAppUi::EnterPluginSettingsViewL( TUid aViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    FUNC_LOG;
    // Plugin settings views need to be handled as special cases, as returning from them does not happen with our
    // ReturnToPreviousViewL() and they are not iherited from our BaseView.
    iNavigationHistory->PushL( iCurrentActiveView );
    ActivateLocalViewL( aViewId, aCustomMessageId, aCustomMessage );
	iSettingsViewActive = ETrue;

	// Fade out the current view here as it doesn't happen when activating the plugin settings view
	iCurrentActiveView->DoTransition( ETrue );
    }

void CFreestyleEmailUiAppUi::ReturnFromPluginSettingsView()
    {
    FUNC_LOG;
    // Check if we just returned from a plugin settings view. In that case, the current active view
    // and view history must be updated here (because RetrunToPreviousViewL() has not been used).
    if ( iSettingsViewActive )
        {
        iSettingsViewActive = EFalse;

        // Set the iCurrentActiveView pointer
        if ( iNavigationHistory->IsEmpty() )
            {
            // Activate grid view if the view history stack is empty. This happens only
            // if something has gone wrong.
            iCurrentActiveView = static_cast<CFsEmailUiViewBase*>( View(AppGridId) );
            }
        else
            {
            iCurrentActiveView = iNavigationHistory->Pop();
            }
        }
    // may be tricky; added here to handle activation of composer view from external app when email is plugin settings view
    // in this case also duplictaed view needs to be poped up, but iSettingsViewActive is already set to EFalse
    // other solution would need to expand api of this class to handle directly this case
    else
    	if ( iPreviousActiveView->Id() == SettingsViewId  || iPreviousActiveView->Id() == MailListId )
    		{
    		iCurrentActiveView = iNavigationHistory->Pop();
    		}
    }

void CFreestyleEmailUiAppUi::ViewActivatedExternallyL( TUid aViewId )
    {
    FUNC_LOG;
    if ( AppUiExitOngoing() )
        {
        return;
        }

    // Do nothing if the externally activated view was already active
    if ( iCurrentActiveView->Id() != aViewId )
        {
        iPreviousActiveView = iCurrentActiveView;

        // Check if the view is in the history stack. In that case, we don't
        // actually navigate forward but backward.
        if ( IsViewInHistoryStack( aViewId ) )
            {
            // Remove views from the stack until the desired one is found or the stack becomes empty
            do
                {
                iCurrentActiveView = iNavigationHistory->Pop();
                }
            while ( !iNavigationHistory->IsEmpty() && iCurrentActiveView->Id() != aViewId );
            }

        // Otherwise this is just the normal case of entering some new view
        else
            {
            iNavigationHistory->PushL( iCurrentActiveView );
            iCurrentActiveView = static_cast<CFsEmailUiViewBase*>( View(aViewId) );
            }
        }
    TIMESTAMP( "View activated" );
    }

void CFreestyleEmailUiAppUi::SetSwitchingToBackground( TBool aValue )
    {
    FUNC_LOG;
    iSwitchingToBackground = aValue;
    }

TBool CFreestyleEmailUiAppUi::SwitchingToBackground() const
    {
    FUNC_LOG;
    return iSwitchingToBackground;
    }

void CFreestyleEmailUiAppUi::ReturnFromHtmlViewerL( TBool aMessageWasDeleted )
	{
    FUNC_LOG;
	if ( aMessageWasDeleted )
	    {
        // Message deleted, remove mail viewer from the stack
        // => view returns directly mail list (or any other view where the viewer was started from)

		TMailListActivationData tmp;
		tmp.iRequestRefresh = ETrue;
		const TPckgBuf<TMailListActivationData> pkgOut( tmp );
		ReturnToPreviousViewL(pkgOut);
	    }
	else
	    {
       	ReturnToPreviousViewL();
	    }
	TIMESTAMP( "Returned from html viewer view to previous view" );
	}

TUid CFreestyleEmailUiAppUi::ReturnToPreviousViewL( const TDesC8& aCustomMessage /*= KNullDesC8*/ )
    {
    FUNC_LOG;
    iPreviousActiveView = iCurrentActiveView;
    CFsDelayedLoader::InstanceL()->GetContactHandlerL()->Reset();
    // Set the iCurrentActiveView pointer
    if ( iNavigationHistory->IsEmpty() )
        {
        // Activate grid view if the view history stack is empty. This happens only
        // if something has gone wrong.
        iCurrentActiveView = static_cast<CFsEmailUiViewBase*>( View(AppGridId) );
        }
    else
        {
        iCurrentActiveView = iNavigationHistory->Pop();
        }

    TUid viewId = iCurrentActiveView->Id();

    // Activate the proper view
	if ( viewId == AppGridId )
		{
		ActivateLocalViewL( AppGridId );
		TIMESTAMP( "Return to application grid view" );
		}
	else if ( viewId == MailListId )
		{
		if ( aCustomMessage.Length() )
		    {
		    ActivateLocalViewL( MailListId, KStartListReturnToPreviousFolder, aCustomMessage );
		    }
		else
		    {
    		const TPckgBuf<TMailListActivationData> pkgOut;
    		ActivateLocalViewL( MailListId, KStartListReturnToPreviousFolder, pkgOut );
		    }
		TIMESTAMP( "Return to message list view" );
		}
	else if ( viewId == SearchListViewId )
		{
		TSearchListActivationData tmp;
		tmp.iMailBoxId = GetActiveMailbox()->GetId();
		const TPckgBuf<TSearchListActivationData> pkgOut( tmp );
		ActivateLocalViewL( SearchListViewId, KStartWithPreviousResults, pkgOut );
		}
	else if ( viewId == HtmlViewerId )
	    {
	    // launch html viewer
		const TPckgBuf<THtmlViewerActivationData> pkgOut;
		ActivateLocalViewL( HtmlViewerId, KHtmlViewerReturnToPrevious, pkgOut );
		TIMESTAMP( "Return to html viewer view" );
	    }
	else if ( viewId == MailEditorId )
		{
		ActivateLocalViewL( MailEditorId, TUid::Uid(KEditorCmdReturnToPrevious), aCustomMessage );
		TIMESTAMP( "Return to email editor view" );
		}
	else if ( viewId == MsgDetailsViewId )
		{
		ActivateLocalViewL( MsgDetailsViewId, KStartMsgDetailsReturnToPrevious, aCustomMessage );
		}
	else if ( viewId == SettingsViewId )
	    {
	    ActivateLocalViewL( SettingsViewId, TUid::Uid(KMailSettingsReturnToPrevious), aCustomMessage );
	    }
	else
		{
		// Generic case where the view does not need any startup parameters
		ActivateLocalViewL( viewId );
		TIMESTAMP( "Return to previous view" );
		}

	return viewId;
    }

void CFreestyleEmailUiAppUi::EraseViewHistory()
    {
    FUNC_LOG;
    iNavigationHistory->Clear();
    }

TBool CFreestyleEmailUiAppUi::IsViewInHistoryStack( TUid aViewId ) const
    {
    FUNC_LOG;
    for ( TInt i=0 ; i<iNavigationHistory->Count() ; ++i )
        {
        if ( (*iNavigationHistory)[i]->Id() == aViewId )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

void CFreestyleEmailUiAppUi::ReturnToViewL( TUid aViewId,
                                            TUid aCustomMessageId /*= TUid::Null()*/,
                                            const TDesC8& aCustomMessage /*= KNullDesC8*/ )
    {
    FUNC_LOG;
    iPreviousActiveView = iCurrentActiveView;

    // Remove views from the stack until the desired one is found or the stack becomes empty
    iCurrentActiveView = iNavigationHistory->Pop();
    while ( !iNavigationHistory->IsEmpty() && iCurrentActiveView->Id() != aViewId )
        {
        iCurrentActiveView = iNavigationHistory->Pop();
        }

    // Return to grid in case the desired view was not found
    if ( iCurrentActiveView->Id() != aViewId )
        {
        iCurrentActiveView = static_cast<CFsEmailUiViewBase*>( View(AppGridId) );
        }

    if ( aCustomMessageId != TUid::Null() || aCustomMessage.Length() )
        {
        ActivateLocalViewL( iCurrentActiveView->Id(), aCustomMessageId, aCustomMessage );
        }
    else
        {
        ActivateLocalViewL( iCurrentActiveView->Id() );
        }
    TIMESTAMP( "Return to earlier view" );
    }

void CFreestyleEmailUiAppUi::ShowFolderListInPopupL(
        const TFSMailMsgId aFolderId,
        MFSEmailUiFolderListCallback* aCallback,
        MFsControlButtonInterface* aButton )
    {
    FUNC_LOG;
    iFolderListVisualiser->ShowInPopupL( aFolderId, aCallback, aButton );

    iCurrentActiveView->ControlGroup().SetAcceptInput( EFalse );
    iEnv->Send( TAlfGroupCommand(*iFolderListControlGroup, EAlfOpShow, &Display()), 0 );
    iFolderListControlGroup->SetAcceptInput( ETrue );
    }

void CFreestyleEmailUiAppUi::ShowSortListInPopupL(
        const TFSMailSortCriteria aCurrentSortCriteria,
        const TFSFolderType aFolderType,
        MFSEmailUiSortListCallback* aCallback,
        MFsControlButtonInterface* aButton )
    {
    FUNC_LOG;
    iFolderListVisualiser->ShowSortListPopupL( aCurrentSortCriteria, aFolderType, aCallback, aButton );

    iCurrentActiveView->ControlGroup().SetAcceptInput( EFalse );
    iEnv->Send( TAlfGroupCommand(*iFolderListControlGroup, EAlfOpShow, &Display()), 0 );
    iFolderListControlGroup->SetAcceptInput( ETrue );
    }

void CFreestyleEmailUiAppUi::FolderPopupClosed()
    {
    FUNC_LOG;
    iFolderListControlGroup->SetAcceptInput( EFalse );
    iEnv->Send( TAlfGroupCommand(*iFolderListControlGroup, EAlfOpHide, &Display()), 0 );
    iCurrentActiveView->ControlGroup().SetAcceptInput( ETrue );
    }

void CFreestyleEmailUiAppUi::ProcessCommandL( TInt aCommand )
    {
    FUNC_LOG;

    // For a reason unknown compose view propagates a leave when the options
    // menu open -command is sent to it during the time when it has started
    // the view deactivation but not yet completed that. It causes unnecessay
    // leave notes in a complete normal use cases in the emulator. To remove
    // the notes but keep the regression risk at minimum (ie. not do any major
    // refactoring) trap the leave here. Propagate other leaves normally.
    TRAPD( err, CAknViewAppUi::ProcessCommandL( aCommand ) );

    if ( err != KErrNcsComposeViewNotReady )
        {
        User::LeaveIfError( err );
        }
    }

void CFreestyleEmailUiAppUi::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EFsEmailUiCmdExit:
            Exit();
            break;
        default:
            break;
        }
    }

void CFreestyleEmailUiAppUi::Exit()
    {
    FUNC_LOG;
    TIMESTAMP ("Exiting from email application");
    g_ApplicationExitOnGoing = ETrue;
    if ( iExitGuardian->ExitApplication() == KRequestPending )
        {
        SendToBackground();
        }
    }

void CFreestyleEmailUiAppUi::ExitNow()
	{
		FUNC_LOG;

		//<cmail>
    if ( iCurrentActiveView )
        {
        // Let active view prepare itself for exit. This may be needed
        // if view's destructor is too late to do certain cleanup.
        TRAP_IGNORE( iCurrentActiveView->PrepareExitL() );
        }
//</cmail>
    // Closing of mail client (and also unsubscribing of mailbox events)
    // was moved from destructor to here to avoid panic in case of pressing
    // end key during attachment download. Panic arises because of
    // MailClient->Close makes plugins to cancel all requests, which means
    // also notifying observers -> UI gets DownloadStatusChangedL call and
    // tries to e.g. update layout. However, CEikonEnv is needed there but it
    // was already destoyed in this phase. Here in Exit() MailClient->Close
    // can finish safely before the environment gets destroyed.

    // First, prepare viewer, search list and composer for exit, destroys mailbox object before
    // This exit preparation should be moved to base class in the future.
    if ( iMsgDetailsVisualiser )
        {
        iMsgDetailsVisualiser->PrepareForExit();
        }
    if ( iMailListVisualiser )
        {
        iMailListVisualiser->PrepareForExit();
        }
    if ( iHtmlViewerView )
        {
        iHtmlViewerView->PrepareForExit();
        }
    if ( iSearchListVisualiser )
        {
        iSearchListVisualiser->PrepareForExit();
        }
    if ( iComposeView )
        {
        iComposeView->PrepareForExit();
        //<cmail>
        //if compose view is not prepared, it will exit app by itself
        if(!iComposeView->IsPreparedForExit())
            {
            return;
            }
        //</cmail>
        }

    delete iMRViewer;
    iMRViewer = NULL;

    // Unsubscribe active mailbox events from list visualiser
    if ( iActiveMailbox )
        {
        iMailClient->UnsubscribeMailboxEvents( iActiveMailboxId, *this );
        delete iActiveMailbox;
        iActiveMailbox = NULL;
        }

    // Unsubscribe events from mailboxes from appui
    for ( TInt i = 0; i < iSubscribedMailBoxes.Count(); i++ )
        {
        iMailClient->UnsubscribeMailboxEvents( iSubscribedMailBoxes[i], *this );
        }
    iSubscribedMailBoxesIds.Reset();
    iSubscribedMailBoxes.Reset();

	if ( iMailClient )
        {
        iMailClient->RemoveObserver( *this );
        iMailClient->Close();
        iMailClient = NULL;
        }

    CAknViewAppUi::Exit();
    }

void CFreestyleEmailUiAppUi::CreateNewMailL()
	{
    FUNC_LOG;
	// Fill params
	TEditorLaunchParams params;
	params.iMailboxId = iActiveMailbox->GetId();
	params.iActivatedExternally = EFalse;
	LaunchEditorL( KEditorCmdCreateNew, params );
	}

CFsEmailUiViewBase* CFreestyleEmailUiAppUi::CurrentActiveView()
    {
    FUNC_LOG;
    return iCurrentActiveView;
    }

CFsEmailUiViewBase* CFreestyleEmailUiAppUi::PreviousActiveView()
    {
    FUNC_LOG;
    return iPreviousActiveView;
    }

TBool CFreestyleEmailUiAppUi::IsPluginSettingsViewActive()
    {
    FUNC_LOG;
    return iSettingsViewActive;
    }

TInt CFreestyleEmailUiAppUi::NewEmailsInModelL()
	{
    FUNC_LOG;
	TInt ret(0);
	if ( iMailListVisualiser )
		{
		ret = iMailListVisualiser->NewEmailsInModelL();
		}
	return ret;
	}

TInt CFreestyleEmailUiAppUi::EmailsInModelL()
	{
    FUNC_LOG;
	TInt ret(0);
	if ( iMailListVisualiser )
		{
		ret = iMailListVisualiser->EmailsInModelL();
		}
	return ret;
	}

TInt CFreestyleEmailUiAppUi::EmailIndexInModel()
	{
    FUNC_LOG;
	TInt currentInboxListIndex = iMailListVisualiser->HighlightedIndex();
	TInt ret=0;
	for ( TInt i=0;i<currentInboxListIndex;i++ )
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>(iMailListVisualiser->Model()->Item(i+1));
		if ( item->ModelItemType() == ETypeMailItem )
			{
			ret++;
			}
		}
	return ret; // Plus 1 for the first index, because it is always separator
	}


TKeyResponse CFreestyleEmailUiAppUi::HandleKeyEventL( const TKeyEvent& /*aKeyEvent*/,
												      TEventCode /*aType*/)
    {
    FUNC_LOG;
	return EKeyWasNotConsumed;
	}

void CFreestyleEmailUiAppUi::HandleWsEventL(const TWsEvent &aEvent, CCoeControl* aDestination)
	{
    FUNC_LOG;
	#ifndef KAknFullOrPartialForegroundGained
	const TInt KAknFullOrPartialForegroundGained = 0x10281F36;
	const TInt KAknFullOrPartialForegroundLost   = 0x10281F37;
	#endif

    TBool closeMenu = EFalse;

	// Let folder list visualizer to handle event first because if the popup
	// is showed, that needs to be closed when pointer click happens outside of the
	// popup rect and also following pointer up event has to be consumed
    if ( iFolderListVisualiser && iFolderListVisualiser->HandleWsEventL( aEvent ) )
        {
        return;
        }

    if (aEvent.Type() == EEventPointer)
        {
        TAdvancedPointerEvent* pointerEvent(aEvent.Pointer());
        iLastPointerPosition = pointerEvent->iParentPosition;
        }
    
	TInt key = aEvent.Key()->iScanCode;
    // <cmail>
    // to disable voice commands during creating new mail message
    if (EStdKeyYes == key && aEvent.Type() == EEventKeyDown  && iCurrentActiveView == iComposeView )
	    {
	    return; //"consume" event
	    }
	//</cmail>
	if ( EStdKeyNo == key && aEvent.Type() == EEventKeyDown )
		{
		// composeview needs this info when exiting
		//iAppUiExitOngoing = ETrue;<cmail> set to true in Exit() method

                //<cmail> unnecessary exceptions removed from cmail
		/*if ( iCurrentActiveView == iComposeView )
			{
			iComposeView->HandleCommandL( ENcsCmdExit );
			}
		else
		    {*/
		if ( !AppUiExitOngoing() ) //avoid multiple red key presses during exiting
		    {
            HandleCommandL( EEikCmdExit );
            }
		    //} //<cmail>
		}

    TInt keyCode = aEvent.Key()->iCode;

    if ( EKeyQwertyOn == keyCode ||
         EKeyQwertyOff == keyCode )
        {
        // Close menu when keyboard is opened or closed.
        closeMenu = ETrue;
        }

    switch ( aEvent.Type() )
        {
        case KAknFullOrPartialForegroundLost:
            {
            if( iCurrentActiveView )
                {
                iCurrentActiveView->HandleAppForegroundEventL( EFalse );
                }
            // in case the popupmenu is shown propagate event to it 
            if( iFolderListVisualiser && iFolderListVisualiser->IsPopupShown() )
                {
                iFolderListVisualiser->HandleAppForegroundEventL( EFalse );
                }
            }
            break;
        case KAknFullOrPartialForegroundGained:
            {
            if ( iCurrentActiveView && !iSettingsViewActive )
                {
                iCurrentActiveView->HandleAppForegroundEventL( ETrue );
                if ( iPendingLayoutSwitch )
                    {
                    iCurrentActiveView->HandleDynamicVariantSwitchL( CFsEmailUiViewBase::EScreenLayoutChanged );
                    iPendingLayoutSwitch = EFalse;
                    }
                }
            // Call status indicator's foreground event to resize the connection screen after the screensaver
            if ( iStatusIndicator )
                {
                iStatusIndicator->HandleForegroundEventL();
                }
            // in case the popupmenu is shown propagate event to it 
            if( iFolderListVisualiser && iFolderListVisualiser->IsPopupShown() )
                {
                iFolderListVisualiser->HandleAppForegroundEventL( ETrue );
                }
            }
            break;
        default:
            break;
        }

    // Close menu
    if ( closeMenu && iCurrentActiveView != NULL )
        {
        CEikMenuBar* menu = iCurrentActiveView->MenuBar();

        if ( menu != NULL )
            {
            if ( menu->IsDisplayed() )
                {
                menu->StopDisplayingMenuBar();
                }
            }
        }

    CAknAppUi::HandleWsEventL(aEvent, aDestination);
	}

CAlfDisplay& CFreestyleEmailUiAppUi::Display()
	{
    FUNC_LOG;
	// Note: Alf doesn't need iCoeDisplay anymore, was return iCoeDisplay->Display();
	return iEnv->PrimaryDisplay();
	}

CAlfEnv& CFreestyleEmailUiAppUi::AlfEnv()
    {
    FUNC_LOG;
    return *iEnv;
    }

void CFreestyleEmailUiAppUi::HandleResourceChangeL( TInt aType )
    {
    FUNC_LOG;
    // Workaround fix for JWIS-7KAEFD
    // Dismiss action menu before doing dynamic variant switch, as it causes
    // a phone reset if theme special effects are used.
    // <cmail>
    /*
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        CFscContactActionMenu* actionMenu = CFSEmailUiActionMenu::GetActionMenu();
        if( actionMenu )
            {
            actionMenu->Dismiss( EFalse );
            }
        }
    */
    // </cmail>
    CAknAppUi::HandleResourceChangeL( aType );

    // Refresh mode is changed to manual to avoid any flickering during
    // resource change handling in list views. Trap any leaves so that we set
    // the automatic refresh mode back on even in case of error.
    iEnv->SetRefreshMode( EAlfRefreshModeManual );
    switch ( aType )
        {
        case KEikDynamicLayoutVariantSwitch:
            iEnv->NotifyLayoutChangedL();
            break;
        case KAknsMessageSkinChange:            
            iEnv->NotifySkinChangedL();
            break;
        }
    TRAPD( error, DoHandleResourceChangeL( aType ) );
    iEnv->SetRefreshMode( EAlfRefreshModeAutomatic );
    User::LeaveIfError( error );
    }

void CFreestyleEmailUiAppUi::DoHandleResourceChangeL( TInt aType )
    {
    FUNC_LOG;
    // on skin change recreate the texture manager
    // ignore skin change events during appui::constructL
    // compose screen loadbackgroundcontext sends 2 skin change events
    //if( aType == KAknsMessageSkinChange && iConstructComplete )
    //    {
    //    CAlfEnv::Static()->NotifySkinChangedL();
    //    }
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        // Changing layout for status pane (just in case it is not switched
        // correctly), fix for HMNN-82BAGR error
        // it's not related with skin change so "moved up" TJOS-83DELP fix 
        TBool landscape(Layout_Meta_Data::IsLandscapeOrientation());
        CEikStatusPane* statusPane = StatusPane();
        if(landscape)
        	{
			if( statusPane->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT)
				{
				statusPane->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT);
				}
        	}
        else
        	{
			if( statusPane->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT)
				{
				statusPane->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT);
				}
        	}       
    	}

    if( aType == KEikDynamicLayoutVariantSwitch || aType == KAknsMessageSkinChange )
        {

	  	TRect screenRect;
	 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
  		StatusPane()->DrawNow();
        if(iEnv)
            {
			Display().SetVisibleArea( screenRect );
            }
         }

    // Notify appropriate views
    if ( iCurrentActiveView )
        {
        CFsEmailUiViewBase::TDynamicSwitchType type = CFsEmailUiViewBase::EOther;

        switch (aType)
            {
            case KAknsMessageSkinChange:
                type = CFsEmailUiViewBase::ESkinChanged;
                break;
            case KEikDynamicLayoutVariantSwitch:
                type = CFsEmailUiViewBase::EScreenLayoutChanged;
                break;
            default:
                break;
            }

        // Report only events of known types
        if ( type != CFsEmailUiViewBase::EOther )
            {
            // In case the FsEmail app is on background, relaying the layout change event to the active view
            // is postponed until brought to foreground. This is done because otherwise it doesn't seem to work
            // correctly in views which use Generic List component.

            if ( !iIsForeground && type == CFsEmailUiViewBase::EScreenLayoutChanged )
                {
                iPendingLayoutSwitch = ETrue;
                }

            if ( iCurrentActiveView && !iSettingsViewActive )
                {
                iCurrentActiveView->HandleDynamicVariantSwitchL( type );
                }

            // Notify status indicator about layout change
            if ( iStatusIndicator )
                {
                iStatusIndicator->NotifyLayoutChange();
                }

            // All visible views are notified about resource change, so let's
            // change refresh mode back to automatic
            iEnv->SetRefreshMode( EAlfRefreshModeAutomatic );

            // In addition to the current view, the change is notified to all
            // the previous views because returning to them often does not
            // lead to complete redraw
            for ( TInt i = 0; i < iNavigationHistory->Count(); ++i )
                {
                CFsEmailUiViewBase* view = (*iNavigationHistory)[i];
                view->HandleDynamicVariantSwitchOnBackgroundL( type );
                }
            }
        }
    }

void CFreestyleEmailUiAppUi::HandleActionL(const TAlfActionCommand& aActionCommand)
    {
    FUNC_LOG;
    switch(aActionCommand.Id())
        {
       	case EFsEmailUiCmdHide:
			{
			TApaTaskList taskList( iEikonEnv->WsSession() );
			TApaTask task = taskList.FindApp( KFSEmailUiUid );
			if ( task.Exists() )
			    {
			    task.SendToBackground(); // Send self to background
			    }
			}
       		break;
        case EAknSoftkeyExit:
            RunAppShutter();
            break;
        }
    }

void CFreestyleEmailUiAppUi::ManualMailBoxSync( TBool aManualMailBoxSync )
    {
    FUNC_LOG;
    iManualMailBoxSync = aManualMailBoxSync;
    }

void CFreestyleEmailUiAppUi::ManualMailBoxSyncAll( TBool aManualMailBoxSyncAll )
    {
    FUNC_LOG;
    iManualMailBoxSyncAll = aManualMailBoxSyncAll;
    }

void CFreestyleEmailUiAppUi::ManualMailBoxConnectAll( TBool aManualMailBoxConnectAll )
    {
    FUNC_LOG;
    iManualMailBoxConnectAll = aManualMailBoxConnectAll;
    }

void CFreestyleEmailUiAppUi::ManualMailBoxDisconnectAll( TBool aManualMailBoxDisconnectAll )
    {
    FUNC_LOG;
    iManualMailBoxDisconnectAll = aManualMailBoxDisconnectAll;
    }

CFSEmailUiLayoutHandler* CFreestyleEmailUiAppUi::LayoutHandler()
	{
    FUNC_LOG;
	return iLayoutHandler;
	}

CFreestyleEmailUiTextureManager* CFreestyleEmailUiAppUi::FsTextureManager()
	{
    FUNC_LOG;
	return iFsTextureManager;
	}

/*CFSEmailUiStatusIndicator* CFreestyleEmailUiAppUi::StatusIndicator()
	{
    FUNC_LOG;
	return iStatusIndicator;
	}*/

void CFreestyleEmailUiAppUi::LaunchEditorL( TEditorLaunchMode aMode, const TEditorLaunchParams& aParams )
	{
    FUNC_LOG;
	// launch editor view with the given set of parameters
	TUid editorCommand = TUid::Uid( aMode );
	TPckgBuf<TEditorLaunchParams> buf( aParams );
	EnterFsEmailViewL( MailEditorId, editorCommand, buf );
	}

CFSMailClient* CFreestyleEmailUiAppUi::GetMailClient()
	{
    FUNC_LOG;
	return iMailClient;
	}

// Returns pointer to download info mediator, ownership is not transferred
CFSEmailDownloadInfoMediator* CFreestyleEmailUiAppUi::DownloadInfoMediator()
	{
    FUNC_LOG;
	return iDwnldMediator;
	}

// ---------------------------------------------------------------------------
// Returns current screen display mode
// ---------------------------------------------------------------------------
//
TDisplayMode CFreestyleEmailUiAppUi::DisplayMode() const
    {
    FUNC_LOG;
    return iEikonEnv->ScreenDevice()->DisplayMode();
    }

// ---------------------------------------------------------------------------
// Returns display images cache
// ---------------------------------------------------------------------------
//
TDisplayImagesCache& CFreestyleEmailUiAppUi::DisplayImagesCache()
    {
    return iDisplayImagesCache;
    }


// ---------------------------------------------------------------------------
// Returns the current flip status.
// ---------------------------------------------------------------------------
//
TBool CFreestyleEmailUiAppUi::IsFlipOpen() const
	{
	FUNC_LOG;
	return iFlipOpen;
	}


// ---------------------------------------------------------------------------
// Meeting request viewer instance
// ---------------------------------------------------------------------------
//
CESMRIcalViewer* CFreestyleEmailUiAppUi::MrViewerInstanceL()
	{
    FUNC_LOG;
	if ( !iMRViewer )
		{
	  	// Create meeting request viewer
	    RImplInfoPtrArray implementations;
		CleanupResetAndDestroyClosePushL( implementations );

	    REComSession::ListImplementationsL(
	        TUid::Uid( KESMRIcalViewerInterfaceUid ), implementations );
		if ( implementations.Count() && iHtmlViewerView )
			{
			// Found implementation of meeting request viewer
	        TRAPD( err, iMRViewer = CESMRIcalViewer::NewL( *iHtmlViewerView ) );
	        if ( err != KErrNone )
				{
				}
			}
	    CleanupStack::PopAndDestroy( &implementations );
		}
	return iMRViewer;
	}
/*Changing this since we now use HTML viewer*/
CFsEmailUiHtmlViewerView& CFreestyleEmailUiAppUi::MailViewer()
    {
    FUNC_LOG;
    return *iHtmlViewerView;
    }

CFSEmailUiFolderListVisualiser& CFreestyleEmailUiAppUi::FolderList()
    {
    FUNC_LOG;
    return *iFolderListVisualiser;
    }

CFSEmailUiLauncherGridVisualiser& CFreestyleEmailUiAppUi::LauncherGrid()
    {
    FUNC_LOG;
    return *iMainUiGridVisualiser;
    }

CFSEmailCRHandler* CFreestyleEmailUiAppUi::GetCRHandler()
	{
    FUNC_LOG;
	return iCRHandler;
	}

CFSMailBox* CFreestyleEmailUiAppUi::GetActiveMailbox()
	{
    FUNC_LOG;
	return iActiveMailbox;
	}


TFSMailMsgId CFreestyleEmailUiAppUi::GetActiveBoxInboxId()
	{
    FUNC_LOG;
	return iActiveBoxInboxId;
	}

TFSMailMsgId CFreestyleEmailUiAppUi::GetActiveMailboxId()
	{
    FUNC_LOG;
	return iActiveMailboxId;
	}

CMsvSession* CFreestyleEmailUiAppUi::GetMsvSession()
	{
    FUNC_LOG;
	return iMsvSession;
	}

CVPbkContactManager& CFreestyleEmailUiAppUi::GetVPbkManagerL()
	{
    FUNC_LOG;
	CVPbkContactManager* temp = CFsDelayedLoader::InstanceL()->
		GetContactHandlerL()->GetContactManager();
	return *(temp);
	}

// ---------------------------------------------------------------------------
// Launch editor with new message and one address in TO-field
// Address object ownership is not transferred
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiAppUi::LaunchEditorL( CFSMailAddress* aToAddress )
	{
    FUNC_LOG;
	// launch editor view with the given set of parameters
	TUid editorCommand = TUid::Uid( KEditorCmdInternalMailto );
	TEditorLaunchParams params;
	params.iMailboxId = GetActiveMailboxId();
	params.iActivatedExternally = EFalse;
	params.iExtra = aToAddress;
	TPckgBuf<TEditorLaunchParams> buf( params );
	EnterFsEmailViewL( MailEditorId, editorCommand, buf );
	}

// ---------------------------------------------------------------------------
// Handle APPUI to subscribe events also for connection info
// Once mailbox has been active, it's events will be handled
// Add mailbox to subscribed array if not already
// ---------------------------------------------------------------------------
void CFreestyleEmailUiAppUi::SubscribeMailboxL( TFSMailMsgId aActiveMailboxId )
    {
    FUNC_LOG;
    if ( iSubscribedMailBoxesIds.Find( aActiveMailboxId.Id() ) == KErrNotFound )
        {
        iSubscribedMailBoxesIds.Append( aActiveMailboxId.Id() );
        iMailClient->SubscribeMailboxEventsL( aActiveMailboxId, *this  );
        }
    }

void CFreestyleEmailUiAppUi::SetActiveMailboxL( TFSMailMsgId aActiveMailboxId, TBool aAutoSync )
	{
    FUNC_LOG;
 	// Set flag
	iAutomatedMailBoxOnline = EFalse;

	// Try to get the mailbox
	CFSMailBox* newActiveBox = iMailClient->GetMailBoxByUidL( aActiveMailboxId );
	User::LeaveIfNull( newActiveBox );

    // Replace active mailbox of the application if getting the mailbox was succesful
    delete iActiveMailbox;
    iActiveMailbox = newActiveBox;

    iActiveMailboxId = iActiveMailbox->GetId();
    iActiveBoxInboxId = iActiveMailbox->GetStandardFolderId( EFSInbox );

	SubscribeMailboxL( iActiveMailboxId );

	// Start connect automatically if asked by the caller
	// Disabled if offline or roaming
    if ( aAutoSync )
        {
        StartMonitoringL();
        }
	}

void CFreestyleEmailUiAppUi::StartMonitoringL()
    {
    FUNC_LOG;
    // Start connect automatically if asked by the caller
    // Disabled if offline or roaming
    if ( !TFsEmailUiUtility::IsOfflineModeL() )
        {
        // Create at first auto-connect, autosync monitor will check whether
        // it is ok to start auto-connect, e.g. phone is not roaming, searching for net, etc
        if ( !iAutoSyncMonitor )
            {
            // TRAP following since failing to create auto sync monitor should not prevent user
            // from opening mail list, leave only in OOM case.
            TRAPD( err, iAutoSyncMonitor = CFSEmailUiAutosyncMonitor::NewL( *this ) );
            if ( err == KErrNoMemory )
                {
                User::Leave( err );
                }
            }
        // Request auto sync if allowed in the current network mode and
        // autosyncmonitor has been succesfully created
        if ( iAutoSyncMonitor )
            {
            TIMESTAMP( "Starting auto sync" );
            iAutoSyncMonitor->StartMonitoring();
            }
        }
    }

void CFreestyleEmailUiAppUi::DoAutoConnectL()
	{
    FUNC_LOG;
	if ( iActiveMailbox && iActiveMailbox->HasCapability( EFSMBoxCapaSupportsSync ) )
 		{
		// Start sync automatically if not syncing already
		TSSMailSyncState latestSyncstate = iActiveMailbox->CurrentSyncState();
		if( latestSyncstate != InboxSyncing && latestSyncstate != StartingSync &&
			latestSyncstate != EmailSyncing )
			{
			if ( iActiveMailbox->GetMailBoxStatus() != EFSMailBoxOnline )
				{
				iActiveMailbox->GoOnlineL();
				}
			}
		}
	else if ( iActiveMailbox )// Go online automatically, e.g, refresh
		{
		// Check if non sync support mailbox is already online
		// Typically this is for Pop
		if ( iActiveMailbox->GetMailBoxStatus() == EFSMailBoxOnline )
			{
			// Already online, disable next note
			iAutomatedMailBoxOnline = ETrue;
			}
		iActiveMailbox->GoOnlineL();
		}
	}

TBool CFreestyleEmailUiAppUi::AppUiExitOngoing()
	{
    FUNC_LOG;
	return g_ApplicationExitOnGoing;
	}

TBool CFreestyleEmailUiAppUi::MessageReaderSupportsFreestyle()
	{
    FUNC_LOG;
	// Check availability of Reader if necessary. Msg reader cannot be installed
	// from sisx so checking is done only once. KErrGeneral at startup.
	if ( iMsgReaderIsSupported == KErrGeneral )
		{
		RLibrary freestyleMessageScannerLibrary;
        TUidType uidType( KDynamicLibraryUid, KSharedLibraryUid, TUid::Uid( 0x10281C61 ) );
		iMsgReaderIsSupported = freestyleMessageScannerLibrary.Load( KMsgReaderFsDll, uidType );
        if ( iMsgReaderIsSupported == KErrNone)
        	{
        	freestyleMessageScannerLibrary.Close();
        	}
		}
	TBool ret(EFalse);
	if ( iMsgReaderIsSupported == KErrNone )
		{
		ret = ETrue;
		}
	return ret;
	}

void CFreestyleEmailUiAppUi::StartReadingEmailsL()
	{
    FUNC_LOG;
	TVwsViewId viewId( TUid::Uid( 0x10201B00 ), TUid::Uid( 1 ) );
	TUid customMessageId( TUid::Uid( 2 ) );
	CFSMailBox* mb = GetActiveMailbox();

	// Check if view is in mail list
	if ( mb && View(MailListId)->IsForeground() )
		{
		RArray<TFSMailMsgId> markedMsgIdArray;
		CleanupClosePushL(markedMsgIdArray);
		iMailListVisualiser->GetMarkedMessagesL(markedMsgIdArray);
		// Read marked items
        if (markedMsgIdArray.Count() != 0 )
			{
            delete iCustomMessageToMessageReader;
            iCustomMessageToMessageReader = NULL;
			iCustomMessageToMessageReader = HBufC8::NewL(markedMsgIdArray.Count() * 16);
			TPtr8 msgPtr = iCustomMessageToMessageReader->Des();
			RDesWriteStream outStream( msgPtr );
			CleanupClosePushL( outStream );

			for (int i = 0; i < markedMsgIdArray.Count(); i++)
				{
			    outStream.WriteInt32L( mb->GetId().PluginId().iUid );
			    outStream.WriteUint32L( mb->GetId().Id() );
			    outStream.WriteUint32L( iMailListVisualiser->FolderId().Id() );
			    outStream.WriteUint32L( (markedMsgIdArray.operator[](i)).Id());
				}
			CleanupStack::PopAndDestroy( &outStream );

            ActivateViewL( viewId, customMessageId,
                *iCustomMessageToMessageReader ) ;
			}
		else //Read focused item
			{
            delete iCustomMessageToMessageReader;
            iCustomMessageToMessageReader = NULL;
			iCustomMessageToMessageReader = HBufC8::NewL(16);
			TPtr8 msgPtr = iCustomMessageToMessageReader->Des();
		    RDesWriteStream outStream( msgPtr );
			CleanupClosePushL( outStream );

		    outStream.WriteInt32L( mb->GetId().PluginId().iUid );
		    outStream.WriteUint32L( mb->GetId().Id() );
		    outStream.WriteUint32L( iMailListVisualiser->FolderId().Id() );
		    outStream.WriteUint32L( (iMailListVisualiser->MsgIdFromListId(iMailListVisualiser->GetMailList().FocusedItem())).Id() );

		    CleanupStack::PopAndDestroy( &outStream );

            if ((iMailListVisualiser->MsgIdFromListId(iMailListVisualiser->GetMailList().FocusedItem())).Id() != NULL)
                {
                ActivateViewL( viewId, customMessageId,
                    *iCustomMessageToMessageReader ) ;
                }
			}
		CleanupStack::PopAndDestroy( &markedMsgIdArray );
		}
	// Check if view is in mail viewer
	else if ( mb && ( View(HtmlViewerId)->IsForeground() || View(SearchListViewId)->IsForeground() ) )
		{
        delete iCustomMessageToMessageReader;
        iCustomMessageToMessageReader = NULL;
		iCustomMessageToMessageReader = HBufC8::NewL(16);
		TPtr8 msgPtr = iCustomMessageToMessageReader->Des();
	    RDesWriteStream outStream( msgPtr );
		CleanupClosePushL( outStream );
	    outStream.WriteInt32L( mb->GetId().PluginId().iUid );
	    outStream.WriteUint32L( mb->GetId().Id() );
		TFSMailMsgId folderId;
	   	TFSMailMsgId msgId;
	   	if ( View(HtmlViewerId)->IsForeground() && iHtmlViewerView )
	   		{
	   		folderId = iHtmlViewerView->ViewedMessageFolderId();
	   		msgId = iHtmlViewerView->ViewedMessageId();
	   		}
	   	else if ( View(SearchListViewId)->IsForeground() && iSearchListVisualiser )
	   		{
	   		folderId = iSearchListVisualiser->HighlightedMessageFolderId();
	   		msgId = iSearchListVisualiser->HighlightedMessageId();
	   		}

	    outStream.WriteUint32L( folderId.Id() );
	    outStream.WriteUint32L( msgId.Id() );

	    CleanupStack::PopAndDestroy( &outStream );

        if ( msgId.Id() )
            {
            ActivateViewL( viewId, customMessageId,
                *iCustomMessageToMessageReader ) ;
            }
		}
	}

TBool CFreestyleEmailUiAppUi::ViewSwitchingOngoing()
	{
    FUNC_LOG;
	return EFalse;
	}

void CFreestyleEmailUiAppUi::RequestResponseL( TFSProgress /*aEvent*/, TInt /*aRequestId*/ )
	{
    FUNC_LOG;

	}

// Force Sync on active mailbox.
void CFreestyleEmailUiAppUi::SyncActiveMailBoxL()
     {
     FUNC_LOG;
     if( iActiveMailbox )
         {
         // Start sync automatically if not syncing already
         TSSMailSyncState latestSyncstate = iActiveMailbox->CurrentSyncState();
         if( latestSyncstate != InboxSyncing && latestSyncstate != StartingSync && latestSyncstate != EmailSyncing )
	     {
    	     iSyncStatusReqId = iActiveMailbox->RefreshNowL( *this );
	     }
         }
      }
void CFreestyleEmailUiAppUi::StopActiveMailBoxSyncL()
     {
     FUNC_LOG;
     if ( iActiveMailbox && iActiveMailbox->GetMailBoxStatus()==EFSMailBoxOnline)
         {
         TSSMailSyncState latestSyncstate = iActiveMailbox->CurrentSyncState();
         if(latestSyncstate == InboxSyncing ||
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
             iActiveMailbox->CancelSyncL();
             }
         }
     }

MCoeMessageObserver::TMessageResponse CFreestyleEmailUiAppUi::HandleMessageL(
    TUint32 aClientHandleOfTargetWindowGroup,
    TUid aMessageUid,
    const TDesC8& aMessageParameters )
    {
    FUNC_LOG;

    if ( iWizardObserver )
        {
        iWizardObserver->HandleMessage( aClientHandleOfTargetWindowGroup,
                                        aMessageUid,
                                        aMessageParameters);
        }

    return CAknViewAppUi::HandleMessageL( aClientHandleOfTargetWindowGroup,
                                          aMessageUid,
                                          aMessageParameters);
    }

void CFreestyleEmailUiAppUi::HandleForegroundEventL( TBool aForeground )
    {
    FUNC_LOG;

    // store flag indicating whether we are foreground
    iIsForeground = aForeground;

    if ( iIsForeground )
        {
        if ( View( MailEditorId )->IsForeground() )
            {
            StartEndKeyCapture();
            }
        }
    else
        {
        StopEndKeyCapture();
        }

    if ( iWizardObserver )
        {
        iWizardObserver->HandleForegroundEventL( aForeground );
        }

    CAknViewAppUi::HandleForegroundEventL( aForeground );
    }


void CFreestyleEmailUiAppUi::EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
                                     TAny* aParam1, TAny* aParam2, TAny* aParam3 )
    {
    FUNC_LOG;
    INFO_1( "CMAIL Received event: %d", aEvent );

    if (iExitGuardian)
        {
        iExitGuardian->EnterLC();
        }

    TBool gridContentsChanged = EFalse;

    switch ( aEvent )
    	{
    	case TFSEventMailboxDeleted:
    		{
    		iDisplayImagesCache.RemoveMailbox(aMailbox);
    		gridContentsChanged = ETrue;
    		if ( iActiveMailbox && iActiveMailbox->GetId().Id() == aMailbox.Id() &&
    			 iCurrentActiveView->Id() != SettingsViewId )
    			{
				EnterFsEmailViewL( AppGridId );
				EraseViewHistory(); // the previous views probably no longer exist if we've got here
    			}
			}
			break;
   		case TFSEventMailboxOnline:
   		case TFSEventMailboxOffline:
   			{
   			if (aEvent == TFSEventMailboxOnline)
   			    {
   			    UpdateTitlePaneConnectionStatus(EForceToConnected);
   			    }
   			else if (aEvent == TFSEventMailboxOffline)
   			    {
   			    UpdateTitlePaneConnectionStatus(EForceToDisconnected);
   			    }
            // Cancel all ongoing downloads for the disconnected mailbox
            if (iDwnldMediator && aEvent == TFSEventMailboxOffline)
              {
              iDwnldMediator->CancelAllDownloadsL(aMailbox);
              }
			if ( !iAutomatedMailBoxOnline )
				{
	   			CFSMailBox* mb = iMailClient->GetMailBoxByUidL( aMailbox );
				CleanupStack::PushL( mb );
				TDesC* mbName = &mb->GetName();
		     	if ( aEvent == TFSEventMailboxOffline )
		     		{
			     	if ( !(mbName && mbName->Length() && !iManualMailBoxDisconnectAll) )
			     		{
                        ManualMailBoxDisconnectAll(EFalse);
				     	}
		     		}
		     	else if ( aEvent == TFSEventMailboxOnline )
		     		{
			     	if ( !(mbName && mbName->Length() && !iManualMailBoxConnectAll) )
			     	    {
				     	ManualMailBoxConnectAll(EFalse);
			     		}
		     		}
	  			CleanupStack::PopAndDestroy( mb );
				}
			else
				{
				iAutomatedMailBoxOnline = EFalse;
				}
   			}
  			break;
    	case TFSEventMailboxSyncStateChanged:
    		{
    		TSSMailSyncState* newSyncState = static_cast<TSSMailSyncState*>( aParam1 );
    		if ( newSyncState !=0 && *newSyncState )
    			{
        		switch ( *newSyncState )
        			{
                    case StartingSync:
                        {
                        TIMESTAMP( "Starting sync" );
                        //If syncs were started by user, show the synchoronisation indicator
                        if(iManualMailBoxSyncAll)
                            {
                            ManualMailBoxSyncAll(EFalse);
                            }
                        }
                        break;
                    case SyncError:
                        {
                        // error occured during "Connect" or "Send and receive" operation
                        // check if user needs to be notified
                        TIMESTAMP( "Sync error" );
                        if ( iManualMailBoxSync )
                            {
							/* 
							 * As a fix to TJOS-82ZFCW, this general popup is no longer needed
                             * // since error id is not provided by plugin, lets popup general note
                             * HBufC* text = StringLoader::LoadL( R_FS_MSERVER_TEXT_UNABLE_TO_COMPLETE );
                             * CleanupStack::PushL( text );
                             * CAknInformationNote* infoNote = new ( ELeave ) CAknInformationNote;
                             * infoNote->ExecuteLD( *text );
                             * CleanupStack::PopAndDestroy( text );
                             */
                            ManualMailBoxSync( EFalse );
                            }
                        }
                        break;
                    case FinishedSuccessfully:
                    case SyncCancelled:
                    case Idle:
                        {
                        TIMESTAMP( "Sync finished" );
                        ManualMailBoxSync( EFalse );
                        }
                        break;

       				case PushChannelOffBecauseBatteryIsLow:
    					{
    					}
    					break;
        			} //switch
    			}
    		UpdateTitlePaneConnectionStatus();
    		} //case
    		break;

		case TFSEventNewMailbox:
		    {
            gridContentsChanged = ETrue;
            iMailClient->GetBrandManagerL().UpdateMailboxNamesL( aMailbox );
		    }
            // fall through
		case TFSEventMailboxCreationFailure:
		    {
		    if ( iWizardObserver )
                {
                iWizardObserver->EventL( aEvent, aMailbox, aParam1, aParam2, aParam3 );
                }
            }
            break;
        case TFSEventMailboxRenamed:
            {
            gridContentsChanged = ETrue;
            if ( aMailbox == iActiveMailboxId )
                {
                // Update active mailbox
                delete iActiveMailbox;
                iActiveMailbox = NULL;
                iActiveMailbox = iMailClient->GetMailBoxByUidL( aMailbox );
                }
            }
            break;
        case TFSEventMailboxCapabilityChanged:
            // Refresh grid if mailbox capabilities have changed
        case TFSEventMailboxSettingsChanged:
			// Refresh grid if mailbox settings are changed,
			// name/branding might have changed.
	   	 	gridContentsChanged = ETrue;
			break;
        case TFSEventMailDeleted:
        case TFSEventMailDeletedFromViewer:
            {
            RArray<TFSMailMsgId>* entries = reinterpret_cast<RArray<TFSMailMsgId>*>(aParam1);
            for (TInt i = entries->Count() - 1; i >= 0; i--)
                {
                iDisplayImagesCache.RemoveMessage(aMailbox, entries->operator[](i));
                }
            }
   		    break;
        default:
            {
            }
        }
	// Forward events to mail list
    if ( iMailListVisualiser )
    	{
    	iMailListVisualiser->HandleMailBoxEventL( aEvent, aMailbox, aParam1, aParam2, aParam3 );
    	}
	// Forward events to Search list
    if ( iSearchListVisualiser )
    	{
    	iSearchListVisualiser->HandleMailBoxEventL( aEvent, aMailbox, aParam1, aParam2, aParam3 );
    	}
    if ( gridContentsChanged && iMainUiGridVisualiser )
        {
        iMainUiGridVisualiser->HandleContentChangeL();
        }
    // <cmail> forward online/offline events also to folder list
    if ( iFolderListVisualiser )
    	{
    	iFolderListVisualiser->HandleMailBoxEventL( aEvent, aMailbox, aParam1, aParam2, aParam3 );
    	}
    // </cmail>

	// MAIL DELETED EVENT HANDLING BASED ON VIEW ACTIVE STATE.
    // Handle mail deleted event in attachment list, as the viewed mail could be open
    if ( iAttachmentListVisualiser && aEvent == TFSEventMailDeleted  &&
    	 iCurrentActiveView->Id() == AttachmentMngrViewId)
    	{
    	iAttachmentListVisualiser->HandleMailBoxEventL( aEvent, aMailbox, aParam1, aParam2, aParam3 );
    	}
    // Handle mail deleted event in Html view list, as the mails might become obsolete
	// Pass sync finished event there too in order to dismiss status dialogs
    else if ( iHtmlViewerView && (aEvent == TFSEventMailDeleted || 
                                  aEvent == TFSEventMailDeletedFromViewer ||
								  aEvent == TFSEventMailboxSyncStateChanged || 
                                  aEvent == TFSEventNewMail) && iCurrentActiveView->Id() == HtmlViewerId)
    	{
    	iHtmlViewerView->HandleMailBoxEventL( aEvent, aMailbox, aParam1, aParam2, aParam3 );
    	}
    // Download manager removed

    if (iExitGuardian)
        {
        CleanupStack::PopAndDestroy(); // iExitGuardian->EnterLC()
        }
    }

void CFreestyleEmailUiAppUi::LaunchWizardL()
    {
    FUNC_LOG;
    TRAPD( err, iMainUiGridVisualiser->LaunchWizardL() );
	if ( err != KErrNone )
		{
		}
    if ( err == KErrNoMemory )
    	{
    	User::Leave( err ); // Leave in OOM, otherwise ignore
    	}
    }

const CFSEmailUiShortcutBinding& CFreestyleEmailUiAppUi::ShortcutBinding() const
    {
    FUNC_LOG;
    return *iShortcutBinder;
    }

void CFreestyleEmailUiAppUi::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
	{
    FUNC_LOG;
	// handle events
	}

// For checking whether next msg is available
TBool CFreestyleEmailUiAppUi::IsNextMsgAvailable( TFSMailMsgId aCurrentMsgId,
												  TFSMailMsgId& aFoundNextMsgId,
												  TFSMailMsgId& aFoundNextMsgFolder )
	{
    FUNC_LOG;
	TBool ret(EFalse);
    if( iNavigationHistory )
        {
        if ( !iNavigationHistory->IsEmpty() )
            {
            ret = iNavigationHistory->Head()->IsNextMsgAvailable( aCurrentMsgId, aFoundNextMsgId, aFoundNextMsgFolder );
            }
        }
	return ret;
	}

// For checking whether previous msg is available
TBool CFreestyleEmailUiAppUi::IsPreviousMsgAvailable( TFSMailMsgId aCurrentMsgId,
													  TFSMailMsgId& aFoundPreviousMsgId,
													  TFSMailMsgId& aFoundPrevMsgFolder )
	{
    FUNC_LOG;
	TBool ret(EFalse);
    if( iNavigationHistory )
        {
        if ( !iNavigationHistory->IsEmpty() )
            {
            ret = iNavigationHistory->Head()->IsPreviousMsgAvailable( aCurrentMsgId, aFoundPreviousMsgId, aFoundPrevMsgFolder );
            }
        }
	return ret;
	}

// Move to next msg
TInt CFreestyleEmailUiAppUi::MoveToNextMsgL( TFSMailMsgId aCurrentMsgId,
    TFSMailMsgId& aFoundNextMsgId )
	{
    FUNC_LOG;
	TInt ret(KErrNotFound);
	if ( !iNavigationHistory->IsEmpty() )
	    {
	    iNavigationHistory->Head()->MoveToNextMsgL( aCurrentMsgId, aFoundNextMsgId );
	    }
	return ret;
	}

// Move to previous msg
TInt CFreestyleEmailUiAppUi::MoveToPreviousMsgL( TFSMailMsgId aCurrentMsgId,
    TFSMailMsgId& aFoundPreviousMsgId )
	{
    FUNC_LOG;
	TInt ret(KErrNotFound);
	if ( !iNavigationHistory->IsEmpty() )
	    {
	    iNavigationHistory->Head()->MoveToPreviousMsgL( aCurrentMsgId, aFoundPreviousMsgId );
	    }
	return ret;
	}

// Move to previous message when the current message is deleted in viewer
TInt CFreestyleEmailUiAppUi::MoveToPreviousMsgAfterDeleteL( TFSMailMsgId aFoundPreviousMsgId )
	{
	FUNC_LOG;
	TInt ret(KErrNotFound);
	if ( !iNavigationHistory->IsEmpty() )
		{
		iNavigationHistory->Head()->MoveToPreviousMsgAfterDeleteL( aFoundPreviousMsgId );
		}
	return ret;
	}

CDocumentHandler& CFreestyleEmailUiAppUi::DocumentHandler()
	{
    FUNC_LOG;
	return *iDocumentHandler;
	}

CFsEmailFileHandleShutter& CFreestyleEmailUiAppUi::FileHandleShutter()
    {
    FUNC_LOG;
    return *iFileHandleShutter;
    }

const TDesC& CFreestyleEmailUiAppUi::TitlePaneTextL()
    {
    FUNC_LOG;
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle =
		StatusPane()->PaneCapabilities( titlePaneUid );

	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* titlePane = static_cast< CAknTitlePane* >(
				StatusPane()->ControlL( titlePaneUid ) );
		return *titlePane->Text();
		}
	return KNullDesC;
    }

void CFreestyleEmailUiAppUi::SetTitlePaneTextL( const TDesC& aText )
    {
    FUNC_LOG;
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle =
		StatusPane()->PaneCapabilities( titlePaneUid );

	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* titlePane = static_cast< CAknTitlePane* >(
				StatusPane()->ControlL( titlePaneUid ) );
		titlePane->SetTextL( aText, ETrue );
		}
    }

void CFreestyleEmailUiAppUi::ResetTitlePaneTextL()
    {
    FUNC_LOG;
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle =
		StatusPane()->PaneCapabilities( titlePaneUid );

	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
				StatusPane()->ControlL( titlePaneUid ) );
		titlePane->SetTextToDefaultL();
		}
    }

void CFreestyleEmailUiAppUi::SetActiveMailboxNameToStatusPaneL()
    {
    FUNC_LOG;
    // Branded mailbox name is nowadays set in new mailbox event
    // handling, so we don't need to use brand manager here anymore.

    // Get pointer to updated mb
    CFSMailBox* confirmedMailBox = GetMailClient()->GetMailBoxByUidL( GetActiveMailboxId() );
    CleanupStack::PushL( confirmedMailBox );
    TDesC* mbName(0);
    if ( confirmedMailBox )
        {
        mbName = &confirmedMailBox->GetName();

        if ( mbName && mbName->Length() )
            {
            SetTitlePaneTextL( *mbName );
            }
        }
    CleanupStack::PopAndDestroy( confirmedMailBox );
    }

void CFreestyleEmailUiAppUi::ShowTitlePaneConnectionStatus()
    {
    FUNC_LOG;
    iConnectionStatusVisible = ETrue;
    UpdateTitlePaneConnectionStatus(iForcedConnectionStatus);
    }

void CFreestyleEmailUiAppUi::HideTitlePaneConnectionStatus()
    {
    FUNC_LOG;
    iConnectionStatusVisible = EFalse;
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle =
		StatusPane()->PaneCapabilities( titlePaneUid );

	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* titlePane = NULL;
		TRAP_IGNORE( titlePane =
			(CAknTitlePane*)StatusPane()->ControlL( titlePaneUid ) );
		TRAP_IGNORE( FsTextureManager()->ProvideBitmapL(
		                        EStatusTextureDisconnectedGeneral, iConnectionIconBitmap, iConnectionIconMask ) );
		if ( iConnectionIconBitmap )
            {
            TSize iconSize = LayoutHandler()->statusPaneIconSize();
            AknIconUtils::SetSize( iConnectionIconBitmap, iconSize, EAspectRatioNotPreserved );
            }
		titlePane->SetSmallPicture( iConnectionIconBitmap, iConnectionIconMask, iConnectionIconBitmap && iConnectionIconMask);
		}
	iConnectionStatusIconAnimTimer->Stop();
    }

void CFreestyleEmailUiAppUi::UpdateTitlePaneConnectionStatus(
		TForcedStatus aForcedStatus /*= ENoForce*/ )
    {
    FUNC_LOG;
    iForcedConnectionStatus = aForcedStatus;

    if ( iConnectionStatusVisible )
    	{
    	// Get connection status of the current mailbox
    	TFSMailBoxStatus connectionStatus = EFSMailBoxOffline;
    	TSSMailSyncState syncState = Idle;
		CFSMailBox* mb = GetActiveMailbox();
		if ( mb )
			{
			connectionStatus = mb->GetMailBoxStatus();
			syncState = mb->CurrentSyncState();
			}
		TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
		CEikStatusPaneBase::TPaneCapabilities subPaneTitle =
			StatusPane()->PaneCapabilities( titlePaneUid );

		if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
			{
			CAknTitlePane* titlePane = NULL;
			TRAP_IGNORE( titlePane =
				(CAknTitlePane*)StatusPane()->ControlL( titlePaneUid ) );

			// Set connection icon
			iConnectionStatusIconAnimTimer->Stop();
			iConnectionIconBitmap = 0;
			iConnectionIconMask = 0;
			TSize iconSize = LayoutHandler()->statusPaneIconSize();
			TBool startTimer = EFalse;
			if ( ( connectionStatus == EFSMailBoxOnline && iForcedConnectionStatus != EForceToDisconnected ) ||
					(iForcedConnectionStatus == EForceToConnected || iForcedConnectionStatus == EForceToSync) )
				{
				if (syncState == EmailSyncing || syncState == InboxSyncing ||
				        syncState == OutboxSyncing || syncState == SentItemsSyncing ||
				        syncState == DraftsSyncing || syncState == CalendarSyncing ||
				        syncState == ContactsSyncing || syncState == TasksSyncing ||
				        syncState == FilesSyncing || iForcedConnectionStatus == EForceToSync )
				    {
				    TRAP_IGNORE( FsTextureManager()->ProvideBitmapL(
                        EStatusTextureSynchronising, iConnectionIconBitmap, iConnectionIconMask ) );
				    startTimer = ETrue;
				    }
				else
				    {
				    TRAP_IGNORE( FsTextureManager()->ProvideBitmapL(
                        EStatusTextureConnected, iConnectionIconBitmap, iConnectionIconMask ) );
				    }
				}
			else // EFSMailBoxOffline
				{
				TRAP_IGNORE( FsTextureManager()->ProvideBitmapL(
						EStatusTextureDisconnectedGeneral, iConnectionIconBitmap, iConnectionIconMask ) );
				}

			if ( iConnectionIconBitmap )
				{
				AknIconUtils::SetSize( iConnectionIconBitmap, iconSize, EAspectRatioNotPreserved );
				}
			titlePane->SetSmallPicture( iConnectionIconBitmap, iConnectionIconMask, iConnectionIconMask && iConnectionIconBitmap );
			if (startTimer)
			    {
			    iConnectionStatusIconAnimTimer->Start(KConnectionStatusIconRotationInterval);
			    }
            titlePane->DrawNow();
			}
    	}
    }

void CFreestyleEmailUiAppUi::CreateStatusPaneIndicatorsL()
	{
    FUNC_LOG;
    // Create custom statuspane with priority and followup indicator support for email editor
	iStatusPaneIndicators = CCustomStatuspaneIndicators::NewL( StatusPane() );
    AddToStackL( iStatusPaneIndicators,
				 ECoeStackPriorityDefault,
    		     ECoeStackFlagRefusesAllKeys | ECoeStackFlagRefusesFocus );
	}

void CFreestyleEmailUiAppUi::DeleteStatusPaneIndicators()
	{
    FUNC_LOG;
	if ( iStatusPaneIndicators )
		{
		RemoveFromStack( iStatusPaneIndicators );
		delete iStatusPaneIndicators;
		iStatusPaneIndicators = NULL;
		}
	}

CCustomStatuspaneIndicators* CFreestyleEmailUiAppUi::GetStatusPaneIndicatorContainer()
	{
    FUNC_LOG;
	return iStatusPaneIndicators;
	}

void CFreestyleEmailUiAppUi::StartEndKeyCapture()
    {
    FUNC_LOG;
    if ( !iIsCaptured )
        {
    	iEndCaptureHandle = iEikonEnv->RootWin().CaptureKey( EStdKeyNo, 0, 0 );
    	iEndUpDownCaptureHandle = iEikonEnv->RootWin().CaptureKeyUpAndDowns( EStdKeyNo, 0, 0 );
        }
	iIsCaptured = ETrue;
    }

void CFreestyleEmailUiAppUi::StopEndKeyCapture()
    {
    FUNC_LOG;
    if ( iIsCaptured )
        {
    	iEikonEnv->RootWin().CancelCaptureKey( iEndCaptureHandle );
    	iEikonEnv->RootWin().CancelCaptureKeyUpAndDowns( iEndUpDownCaptureHandle );
        }
	iIsCaptured = EFalse;
    }

void CFreestyleEmailUiAppUi::CheckUpdatesL()
    {
    FUNC_LOG;
    // Initialize feature manager, if not yet initialized
    if( !iFeatureManagerInitialized )
        {
        FeatureManager::InitializeLibL();  // Successfull call increases reference count!
        iFeatureManagerInitialized = ETrue;  // We can now call UnInitializeLib()
        }

    // Simple test to check whether we run against API stub or "real" IAD
    // implementation. Normal applications should use FeatureManager to check
    // whether KFeatureIdIAUpdate is on before using any IAUpdate API function.
    if( FeatureManager::FeatureSupported( KFeatureIdIAUpdate ) )
        {
        TRAP_IGNORE( iUpdateClient = CIAUpdate::NewL( *this ) );
        if ( iUpdateClient ) // IAUpdate is supported in the phone
            {
            iUpdateParameters = CIAUpdateParameters::NewL();
            // Search for updates to IAUpdateExampleClient, We know that the
            // SIS package uid is KIAParamUid, so let's use that as a
            // search criteria.
            iUpdateParameters->SetUid( KFSEmailUiUid );
            // Free-form search criteria is not supported in
            // IAUpdateExampleClient packages.
            //iUpdateParameters->SetSearchCriteriaL( KIAParamSearch );
            // We want iaupdateexampleclient.exe to be started after update is
            // finished
            iUpdateParameters->SetCommandLineExecutableL( KFSEmailUiExec );
            //iUpdateParameters->SetCommandLineArgumentsL( KIAParamArg );
            // We don't want any wait dialog to be shown, let's keep everything
            // silent.
            // EFalse is the default, so no need to call this.
            //iUpdateParameters->SetShowProgress( EFalse );
            // Check the updates
            iUpdateClient->CheckUpdates( *iUpdateParameters );
            }
        }
    }

void CFreestyleEmailUiAppUi::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    FUNC_LOG;
    if ( aErrorCode == KErrNone )
        {
        if ( aAvailableUpdates > 0 )
            {
            // There were some updates available. Let's ask if the user wants to update them.
            iUpdateClient->UpdateQuery();
            }
        else
            {
            // No updates available. CIAUpdate object should be deleted already now, since we are not
            // going to call anything else from the API. This will also close down the IAUpdate server.
            AllUpdateActivitiesCompleted();
            }
         }
    }

void CFreestyleEmailUiAppUi::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {
    FUNC_LOG;
    if ( aErrorCode == KErrNone )
        {
        if ( aUpdateNow )
            {
            // User choosed to update now, so let's launch the IAUpdate UI.
            iUpdateClient->ShowUpdates( *iUpdateParameters );
            }
        else
            {
            // The answer was 'Later'. CIAUpdate object could be deleted already now, since we are not
            // going to call anything else from the API. This will also close down the IAUpdate server.
            AllUpdateActivitiesCompleted();
            }
        }
    }

void CFreestyleEmailUiAppUi::UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResult )
    {
    FUNC_LOG;
    if ( aErrorCode == KErrNone )
        {
        // The update process that the user started from IAUpdate UI is now completed.
        // If the client application itself was updated in the update process, this callback
        // is never called, since the client is not running anymore.
        TInt successCount = aResult->SuccessCount();

        // Should we restart our application somehow if we come here?
        // Basically this can happen only when not doing a whole
        // update, so e.g. if only some plugin is updated.
        }

    delete aResult; // Ownership was transferred, so this must be deleted by the client
    // We do not need the client-server session anymore, let's delete the object to close the session
    AllUpdateActivitiesCompleted();
    }

void CFreestyleEmailUiAppUi::AllUpdateActivitiesCompleted()
    {
    FUNC_LOG;
    delete iUpdateClient;
    iUpdateClient = NULL;
    delete iUpdateParameters;
    iUpdateParameters = NULL;
    iIADUpdateCompleted = ETrue;

    // After completed all update activities, check is there any mailboxes
    // defined. If not, show a query to ask if user wants to create one.
    TRAP_IGNORE( ShowMailboxQueryL() );
    }


void CFreestyleEmailUiAppUi::GridStarted( TBool aStartedFromOds )
	{
    FUNC_LOG;
	iGridStarted = ETrue;
	if ( aStartedFromOds )
		{
		iGridFirstStartFromOds = ETrue;
		}
	}

// This is combined show mailbox query that can be displayed either when
// upcate client has finished or when laucnher grid has completed its construction
// Launches async callback so that wizard will not block UI creation.
void CFreestyleEmailUiAppUi::ShowMailboxQueryL()
	{
    FUNC_LOG;
	if ( !iStartupQueryShown && iGridStarted && !iGridFirstStartFromOds && iIADUpdateCompleted )
		{
	    RPointerArray<CFSMailBox> mailboxes;
	    CleanupResetAndDestroyClosePushL( mailboxes );
	    TFSMailMsgId id;
	    GetMailClient()->ListMailBoxes( id, mailboxes );
	    if( mailboxes.Count() == 0 && iNewBoxQueryAsyncCallback )
	        {
			iNewBoxQueryAsyncCallback->CallBack();
	        }
	    CleanupStack::PopAndDestroy( &mailboxes );

		// NULL FLAG, not to be shown again
		iStartupQueryShown = ETrue;
		}
	}


// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::DisplayCreateQueryL
// Async callback is needed since the UI and launcher grid construction cannot jam
// if wizard is launched
// -----------------------------------------------------------------------------
TInt CFreestyleEmailUiAppUi::DisplayCreateQueryL( TAny* aSelfPtr )
    {
    FUNC_LOG;
    CFreestyleEmailUiAppUi* self = static_cast<CFreestyleEmailUiAppUi*>( aSelfPtr );

    if (self->AppUiExitOngoing())
        {
        return KErrNone;
        }

    if (!self->iMainUiGridVisualiser || self->iMainUiGridVisualiser->UiOperationLaunched())
        {
        return KErrNone;
        }

    TRAPD( err, self->DisplayCreateMailboxQueryL() );

    if (err == KLeaveExit)
        {
        User::Leave(err);
        }

    return err;
    }

void CFreestyleEmailUiAppUi::DisplayCreateMailboxQueryL()
	{
    FUNC_LOG;
    if ( TFsEmailUiUtility::ShowConfirmationQueryL( R_FS_EMAIL_SETTINGS_ADD_MAILBOX_TXT ) )
    	{
        // Launch wizard if user selected Yes to create new mailbox query
        LaunchWizardL();
	    }
	}

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::SendToBackground
// Send self to background.
// -----------------------------------------------------------------------------
void CFreestyleEmailUiAppUi::SendToBackground()
    {
    FUNC_LOG;

    TApaTaskList taskList( iEikonEnv->WsSession() );
    TApaTask task = taskList.FindApp( KFSEmailUiUid );
    if ( task.Exists() )
        {
        // Send self to background
        task.SendToBackground();
        }

    iSwitchingToBackground = EFalse;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::SetEmbeddedApp
// Set flag for judging if there is a embedded app in FSEmail.
// -----------------------------------------------------------------------------
void CFreestyleEmailUiAppUi::SetEmbeddedApp( TBool aEmbeddedApp )
    {
    iHasEmbeddedApp = aEmbeddedApp;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::EmbeddedApp
// Get embedded app flag.
// -----------------------------------------------------------------------------
TBool CFreestyleEmailUiAppUi::EmbeddedApp() const
    {
    return iHasEmbeddedApp;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::SetEmbeddedAppToPreviousApp
// -----------------------------------------------------------------------------
void CFreestyleEmailUiAppUi::SetEmbeddedAppToPreviousApp( TBool aEmbeddedApp )
    {
    iPreviousAppEmbedded = aEmbeddedApp;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::EmbeddedAppIsPreviousApp
// -----------------------------------------------------------------------------
TBool CFreestyleEmailUiAppUi::EmbeddedAppIsPreviousApp() const
    {
    return iPreviousAppEmbedded;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::SetEditorStartedFromEmbeddedApp
// -----------------------------------------------------------------------------
void CFreestyleEmailUiAppUi::SetEditorStartedFromEmbeddedApp( TBool aEmbeddedApp )
    {
    iEditorStartedFromEmbeddedApp = aEmbeddedApp;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::EditorStartedFromEmbeddedApp
// -----------------------------------------------------------------------------
TBool CFreestyleEmailUiAppUi::EditorStartedFromEmbeddedApp() const
    {
    return iEditorStartedFromEmbeddedApp;
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::LastSeenPointerPosition
// -----------------------------------------------------------------------------
const TPoint& CFreestyleEmailUiAppUi::LastSeenPointerPosition() const
    {
    return iLastPointerPosition;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CFreestyleEmailUiAppUi::ConstructNaviPaneL()
    {
    FUNC_LOG;

    CAknNavigationControlContainer* naviPaneContainer =
        static_cast<CAknNavigationControlContainer*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    // Following navipane is for mailviewer view
    if ( !iNaviDecorator2MailViewer )
        {
        // Constructing a decorator with own decorated control,
        // which is (currently an empty) container.
        CFreestyleEmailUiNaviPaneControlContainer2MailViewer* c =
            CFreestyleEmailUiNaviPaneControlContainer2MailViewer::NewL();
        c->SetContainerWindowL( *naviPaneContainer );
        iNaviDecorator2MailViewer = CAknNavigationDecorator::NewL(
            naviPaneContainer,
            c,
            CAknNavigationDecorator::ENotSpecified );

        // In order to get navi arrows visible, they must be set visible AND not dimmed...
        iNaviDecorator2MailViewer->SetContainerWindowL( *naviPaneContainer );
        iNaviDecorator2MailViewer->MakeScrollButtonVisible( ETrue );
        iNaviDecorator2MailViewer->SetScrollButtonDimmed( CAknNavigationDecorator::ELeftButton, EFalse );
        iNaviDecorator2MailViewer->SetScrollButtonDimmed( CAknNavigationDecorator::ERightButton, EFalse );

        iNaviDecorator2MailViewer->SetComponentsToInheritVisibility( ETrue );
        naviPaneContainer->PushL( *iNaviDecorator2MailViewer );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CAknNavigationDecorator* CFreestyleEmailUiAppUi::NaviDecoratorL(  const TUid aViewId  )
    {
    FUNC_LOG;
    CAknNavigationDecorator* decorator( NULL );
    // Depending on who's asking, return proper navipane instance
    if ( aViewId.iUid == MailViewerId.iUid )
        {
        if ( !iNaviDecorator2MailViewer )
            {
            ConstructNaviPaneL();
            }
        decorator = iNaviDecorator2MailViewer;
        }
    return decorator;
    }

void CFreestyleEmailUiAppUi::TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer )
    {
    if (aTriggeredTimer == iConnectionStatusIconAnimTimer)
        {
        if ( iConnectionStatusVisible )
            {
            TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
            CEikStatusPaneBase::TPaneCapabilities subPaneTitle =
                StatusPane()->PaneCapabilities( titlePaneUid );

            if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
                {
                CAknTitlePane* titlePane = NULL;
                TRAP_IGNORE( titlePane =
                    (CAknTitlePane*)StatusPane()->ControlL( titlePaneUid ) );

                TSize iconSize = LayoutHandler()->statusPaneIconSize();
                iConnectionStatusIconAngle += KConnectionStatusIconRotationAmount;
                AknIconUtils::SetSizeAndRotation(iConnectionIconBitmap, iconSize, EAspectRatioNotPreserved, iConnectionStatusIconAngle);
                titlePane->DrawNow();
                iConnectionStatusIconAnimTimer->Start(KConnectionStatusIconRotationInterval);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CFreestyleEmailUiAppUi::SetFocusVisibility
// -----------------------------------------------------------------------------

TBool CFreestyleEmailUiAppUi::SetFocusVisibility( TBool aVisible )
	{
	FUNC_LOG;
	TBool oldFocusState( iFocusVisible );
	iFocusVisible = aVisible;
	CFsEmailUiViewBase* activeView = CurrentActiveView();
	if ( activeView )
		{
		activeView->FocusVisibilityChange( aVisible );
		}
	// If popup is visible inform it also.
	if( iFolderListVisualiser && iFolderListVisualiser->IsPopupShown() )
	    {
	    iFolderListVisualiser->FocusVisibilityChange( aVisible );
	    }
	return oldFocusState;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TBool CFreestyleEmailUiAppUi::IsFocusShown() const
	{
	FUNC_LOG;
	return iFocusVisible;
	}

// PUBLIC METHODS FROM MFREESTYLEEMAILUIPROPERTYCHANGEDOBSERVER

// -----------------------------------------------------------------------------
// From MFreestyleEmailUiPropertyChangedObserver.
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiAppUi::PropertyChangedL( TInt aValue )
	{
	EPSHWRMFlipStatus flipStatus = ( EPSHWRMFlipStatus )aValue;

    if( flipStatus == EPSHWRMFlipOpen )
         {
         iFlipOpen = ETrue;
         }
     else
         {
         iFlipOpen = EFalse;
         }

	CFsEmailUiViewBase* activeView = CurrentActiveView();
	if ( activeView )
		{
		activeView->FlipStateChangedL( iFlipOpen );
		}
	}



////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// CFSEmailUiAutosyncMonitor::CFSEmailUiAutosyncMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFSEmailUiAutosyncMonitor::CFSEmailUiAutosyncMonitor( CFreestyleEmailUiAppUi& aAppUi ) :
    CActive( CActive::EPriorityStandard ),
    iMonitoringStatus( EInactive ),
    iAppUi( aAppUi ),
    iRegisterationStatus( RMobilePhone::ERegistrationUnknown )
    {
    FUNC_LOG;
    }


// -----------------------------------------------------------------------------
// CCMRegistrationMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFSEmailUiAutosyncMonitor::ConstructL()
    {
    FUNC_LOG;
    User::LeaveIfError( iServer.Connect() );
    User::LeaveIfError( iServer.LoadPhoneModule( KPhoneModuleName ) );
    iServer.SetExtendedErrorGranularity( RTelServer::EErrorBasic );
    User::LeaveIfError( iPhone.Open( iServer, KPhoneName ) );
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CFSEmailUiAutosyncMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFSEmailUiAutosyncMonitor* CFSEmailUiAutosyncMonitor::NewL( CFreestyleEmailUiAppUi& aAppUi )
    {
    FUNC_LOG;
    CFSEmailUiAutosyncMonitor* self = new( ELeave ) CFSEmailUiAutosyncMonitor( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CFSEmailUiAutosyncMonitor::~CFSEmailUiAutosyncMonitor()
    {
    FUNC_LOG;
    if( IsActive() )
        {
        Cancel();
        }
    iPhone.Close();
    iServer.Close();
    }


// -----------------------------------------------------------------------------
// CCMRegistrationMonitor::StartMonitoring()
// Starts monitoring of the network registeration status
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFSEmailUiAutosyncMonitor::StartMonitoring()
    {
    FUNC_LOG;
    if ( IsActive() )
        {
        Cancel();
        }
#ifdef __WINS__ // do not try to connect on the emulator
    iRegisterationStatus = RMobilePhone::ERegisteredOnHomeNetwork;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
#else
    iPhone.GetNetworkRegistrationStatus( iStatus, iRegisterationStatus );
#endif
    iMonitoringStatus = ERequestingNetworkStatus;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiAutosyncMonitor::StopMonitoring()
// Stops the registeration monitor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFSEmailUiAutosyncMonitor::StopMonitoring()
    {
    FUNC_LOG;
    if ( IsActive() )
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiAutosyncMonitor::DoCancel()
// Cancels the monitoring
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFSEmailUiAutosyncMonitor::DoCancel()
    {
    FUNC_LOG;
    iPhone.CancelAsyncRequest( EMobilePhoneGetNetworkRegistrationStatus );
    }

TInt CFSEmailUiAutosyncMonitor::RunError( TInt /*aError*/ )
    {
    FUNC_LOG;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiAutosyncMonitor::RunL()
// Handles object’s request completion event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFSEmailUiAutosyncMonitor::RunL()
    {
    FUNC_LOG;
    switch ( iMonitoringStatus )
        {
        case ERequestingNetworkStatus:
           {
            switch( iRegisterationStatus )
                {
                // Not Ok cases
                case RMobilePhone::ERegistrationUnknown:
                case RMobilePhone::ENotRegisteredNoService:
                case RMobilePhone::ENotRegisteredEmergencyOnly:
                case RMobilePhone::ENotRegisteredSearching:
                case RMobilePhone::ERegisteredBusy:
                case RMobilePhone::ERegistrationDenied:
                //case RMobilePhone::ERegisteredRoaming:
                	// No autoconnect in these cases
                    break;
                // This is ok
                case RMobilePhone::ERegisteredOnHomeNetwork:
                case RMobilePhone::ERegisteredRoaming:
     				// TRAP autoconnect, because list UI must open regardless success
					// or failure of the auto-connect
                	TRAP_IGNORE( iAppUi.DoAutoConnectL() );
                    break;
                default:
                    break;
                }
            break;
            }
        default:
            break;
        }
    }

// End of file

