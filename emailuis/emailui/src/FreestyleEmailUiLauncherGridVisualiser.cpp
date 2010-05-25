/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FreestyleEmailUi main grid implementation
*
*/


// SYSTEM INCLUDE FILES
#include "emailtrace.h"
#include <coemain.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <apgcli.h> // RApaLsSession
#include <e32math.h>
#include <FreestyleEmailUi.rsg>
#include <touchlogicalfeedback.h>
#include <alf/alfutil.h>
#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alftextvisual.h>
#include <aknnotewrappers.h>
#include <freestyleemailui.mbg>
#include <gulicon.h>
#include <akntoolbar.h>
#include "cfsmailmessage.h"
#include <alf/alfframebrush.h>
#include "cfsmailbox.h"
#include "cfsmailclient.h"
#include <hlplch.h>
#include <akntitle.h>
#include <centralrepository.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfbrusharray.h>
#include <alf/alfstatic.h>
#include <alf/alfgencomponent.h>
#include <alf/alfconstants.h>
#include "fsalfscrollbarlayout.h"
#include <csxhelp/cmail.hlp.hrh>
#include <featmgr.h>
#include <coecntrl.h>

#include <aknmessagequerydialog.h>
#include <aknstyluspopupmenu.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <touchfeedback.h>
#include <aknphysics.h>

// INTERNAL INCLUDE FILES
#include "FSEmailBuildFlags.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiLauncherGrid.h"
#include "FreestyleEmailUiLauncherGridVisualiser.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiMailListVisualiser.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FSDelayedLoader.h"

// Utility clean up function
void CleanupEComArray( TAny* aArray );

// CONSTANT VALUES
const TReal KDefaultCaptionOpacity = 1.0;
const TInt KDefaultSelection = 0;
const TInt KSelectTransitionTimeMs = 300;
const TInt KIconScalingTransitionTimeMs = 350;
const TInt KStartupAnimationTime = 0;
const TReal KScaleSelected = 1.0;
const TReal KScaleNotSelected = 0.77;


CFSEmailUiLauncherGridVisualiser* CFSEmailUiLauncherGridVisualiser::NewL(CAlfEnv& aEnv,
												 CFSEmailUiLauncherGrid* aControl,
												 CFreestyleEmailUiAppUi* aAppUi,
												 CAlfControlGroup& aControlGroup,
												 TInt aColumns, TInt aRows)
    {
    FUNC_LOG;
    CFSEmailUiLauncherGridVisualiser* self = CFSEmailUiLauncherGridVisualiser::NewLC(aEnv, aControl, aAppUi, aControlGroup, aRows, aColumns);
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiLauncherGridVisualiser* CFSEmailUiLauncherGridVisualiser::NewLC(CAlfEnv& aEnv,
												  CFSEmailUiLauncherGrid* aControl,
												  CFreestyleEmailUiAppUi* aAppUi,
												  CAlfControlGroup& aControlGroup,
												  TInt aColumns, TInt aRows)
    {
    FUNC_LOG;
    CFSEmailUiLauncherGridVisualiser* self = new (ELeave) CFSEmailUiLauncherGridVisualiser(aEnv, aControl, aAppUi, aControlGroup);
    CleanupStack::PushL(self);
    self->ConstructL(aColumns, aRows);
    return self;
    }

CFSEmailUiLauncherGridVisualiser::CFSEmailUiLauncherGridVisualiser(CAlfEnv& aEnv,
											 CFSEmailUiLauncherGrid* aControl,
										     CFreestyleEmailUiAppUi* aAppUi,
										     CAlfControlGroup& aControlGroup)
    : CFsEmailUiViewBase(aControlGroup, *aAppUi),
    iEnv( aEnv ),
    iVisibleRows( 0 ),
    iVisibleColumns( 0 ),
    iRowCount( 0 ),
    iFirstVisibleRow( 0 ),
    iRowHeight( 0 ),
    iColumnWidth( 0 ),
	iSelector( 0 ),
	iStartupAnimation( 0 ),
	iStartupEffectStyle( 0 ),
  	iWizardWaitnoteShown( EFalse ),
  	iPointerAction( EFalse ),
    iIsDragging( EFalse ),
    iScrolled( EFalse ),
    iLaunchWizardExecuted( EFalse )
    {
    FUNC_LOG;
    iItemIdInButtonDownEvent.iItemId = KErrNotFound;
    iItemIdInButtonDownEvent.iLaunchSelection = EFalse;
    iControl = aControl;
    }

void CFSEmailUiLauncherGridVisualiser::ConstructL( TInt aColumns, TInt aRows )
    {
    FUNC_LOG;
    BaseConstructL( R_FSEMAILUI_MAINUI_VIEW );
    iVisibleRows = aRows;
    iVisibleColumns = aColumns;
    iConstructionCompleted = EFalse;
    iDoubleClickLock = EFalse;
    iUiOperationLaunched = EFalse;

    iMailboxDeleter = CFSEmailUiMailboxDeleter::NewL( *iAppUi.GetMailClient(), *this );

    // Create startup timer
    iStartupCallbackTimer = CFSEmailUiGenericTimer::NewL( this );

    iCurrentLevel.iParentPos.iY = 0; 
    iCurrentLevel.iParentPos.iX = 0;

    if ( CAknPhysics::FeatureEnabled() )
        {
        iPhysics = CAknPhysics::NewL(*this, NULL);
        }
    }

// ----------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::DoFirstStartL()
// Purpose of this function is to do first start only when grid is really
// needed to be shown. Implemented to make app startup faster.
// ----------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::DoFirstStartL()
    {
    FUNC_LOG;
    iPluginIdIconIdPairs.Reset();

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
									   mainPaneRect );

    TAknLayoutRect scrollBarRect;
    scrollBarRect.LayoutRect( mainPaneRect,
    	AknLayoutScalable_Avkon::aid_size_touch_scroll_bar() );
    TRect gridRect = mainPaneRect;
    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

    iVisibleRows = iAppUi.LayoutHandler()->GridRowsInThisResolution();
    iVisibleColumns = iAppUi.LayoutHandler()->GridColumnsInThisResolution();

    iStartupAnimation = ETrue;
    iCurrentLevel.iSelected = KDefaultSelection;
    CAlfTextureManager& manager = iEnv.TextureManager();

    iParentLayout = CAlfDeckLayout::AddNewL( *iControl );
    iParentLayout->SetFlags( EAlfVisualFlagLayoutUpdateNotification );

    // Widget layout divides the screen between grid and scroll bar
    iWidgetLayout = CAlfAnchorLayout::AddNewL( *iControl, iParentLayout );
	TSize displaySize = mainPaneRect.Size();
    iWidgetLayout->SetSize( displaySize );

    // Constructed here, updated later, #0 item in iWidgetLayout
    ConstructScrollbarL( iWidgetLayout );

    // Grid layout is constructed here, #1 item in iWidgetLayout
    iCurrentLevel.iGridLayout =
		CAlfGridLayout::AddNewL( *iControl, iVisibleColumns, iVisibleRows,
								 iWidgetLayout );
    iCurrentLevel.iGridLayout->EnableScrollingL( ETrue );
    iCurrentLevel.iGridLayout->SetFlags( EAlfVisualFlagAutomaticLocaleMirroringEnabled );

    // Selector is added to iGridLayout
    iSelector = iControl->AppendLayoutL( EAlfLayoutTypeLayout,
										 iCurrentLevel.iGridLayout );
    iSelector->SetFlags( EAlfVisualFlagManualLayout );

    iRingMovementXFunc = CAlfTableMappingFunction::NewL( iEnv );
    iRingMovementYFunc = CAlfTableMappingFunction::NewL( iEnv );

    TAlfTimedPoint selectorPos = iSelector->Pos();
    selectorPos.iX.SetMappingFunctionIdentifier( iRingMovementXFunc->MappingFunctionIdentifier() );
    selectorPos.iY.SetMappingFunctionIdentifier( iRingMovementYFunc->MappingFunctionIdentifier() );
    iSelector->SetPos( selectorPos );

    UpdateFocusVisibility();
    iSelectorImageVisual = CAlfImageVisual::AddNewL( *iControl, iSelector );
    iSelectorImageVisual->SetScaleMode( CAlfImageVisual::EScaleFit );

    const TInt var( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );

    // Use layout data instead of hard-coded values
    TAknLayoutRect itemRect;
    itemRect.LayoutRect( gridRect,
    	AknLayoutScalable_Apps::cell_cmail_l_pane( var, 0, 0 ) );
    iSelectorImageVisual->SetSize( itemRect.Rect().Size() );
    iSelectorImageVisual->EnableBrushesL();
    CAlfFrameBrush* brush = iAppUi.FsTextureManager()->GridSelectorBrushL();
    iSelectorImageVisual->Brushes()->AppendL( brush, EAlfDoesNotHaveOwnership );
    iStartupEffectStyle = EFalse;

    iAiwSHandler = CAiwServiceHandler::NewL();
    iAiwSHandler->AttachL( R_AIW_INTEREST_LAUNCH_SETUP_WIZARD );

    if( !iStylusPopUpMenu )
        {
        // Construct the long tap pop-up menu.
        TPoint point( 0, 0 );
        iStylusPopUpMenu = CAknStylusPopUpMenu::NewL( this , point );
		TResourceReader reader;
		iCoeEnv->CreateResourceReaderLC( reader,
			R_STYLUS_POPUP_MENU_LAUNCHER_GRID_VIEW );
		iStylusPopUpMenu->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader
        }
    
    iCoeControl = new( ELeave )CCoeControl;
    // Initial visual layout update is done when the view gets activated.
    iRefreshNeeded = ETrue;

    UpdatePhysicsL(); // init sizes for scrooling
    
    // First start toggle
    iConstructionCompleted = ETrue;
    }

// ---------------------------------------------------------------------------
// HandleButtonReleaseEventL is called when Launcher grid visualiser
// gets pointer event that indicates that button is released.
// function should decide if focus should still be drawn or not.
// ---------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::HandleButtonReleaseEvent()
    {
    iItemIdInButtonDownEvent.iItemId = KErrNotFound;
    iItemIdInButtonDownEvent.iLaunchSelection = EFalse;

    UpdateFocusVisibility();

	if( !IsFocusShown() )
		{
        // No items are focused anymore. Shrink the icon.
		ResizeItemIcon( ETrue );
		}
	else
	    {
        // Reset selected icon size back to normal
        ResizeItemIcon( EFalse );
	    }
    }

// ---------------------------------------------------------------------------
// Reduces icon size of seleceted item
// Called when grag event is made.
// ---------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::ResizeItemIcon( TBool aReduce )
    {
    TInt selectedItem( iCurrentLevel.iSelected );
    if( selectedItem >= 0 )
        {
        TReal transition( KScaleNotSelected );
        if( !aReduce )
            {
            transition = KScaleSelected;
            }
        TAlfTimedValue scaleValue;
        scaleValue.SetTarget( transition, KIconScalingTransitionTimeMs * 2 );
        iCurrentLevel.iItemVisualData[selectedItem].iImage->SetScale( scaleValue );
        if ( !iScrolled )
            {
            HandleRowMovement( EDirectionTouch, selectedItem );
            }
        }
    }

CFSEmailUiLauncherGridVisualiser::~CFSEmailUiLauncherGridVisualiser()
    {
    FUNC_LOG;
    
    if ( iStartupCallbackTimer )
    	{
    	iStartupCallbackTimer->Cancel();
    	delete iStartupCallbackTimer;
    	}
    
    iPluginIdIconIdPairs.Reset();
    iIconArray.Close();
    iMailboxRequestIds.Close();
    iLauncherItems.ResetAndDestroy();
    iLauncherItemUids.Close();
    iCurrentLevel.iItemVisualData.Close();
    iCurrentLevel.iItems.Close();
    DetachSelectorMappingFunctions();
    delete iRingMovementXFunc;
    delete iRingMovementYFunc;
    delete iModel;
    delete iAiwSHandler;
    delete iScrollbar;
    delete iMailboxDeleter;
    delete iStylusPopUpMenu;
    delete iCoeControl;
    delete iPhysics;
    }

void CFSEmailUiLauncherGridVisualiser::CreateModelL()
    {
    FUNC_LOG;

    RArray<TBool> itemInModel;
    CleanupClosePushL( itemInModel );
    iIconArray.Reset();

	iPluginTextureId = EGridPluginIconFirst;
	iPluginIdIconIdPairs.Reset();

    delete iModel;
    iModel = NULL;
    iModel = new (ELeave) CFSEmailUiLauncherGridModel();
    iModel->ConstructL();
	CAlfTexture* iconTexture = 0;

	// Get item ordering from resources
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, R_FSEMAILUI_LAUNCHER_GRID );

    UpdateLauncherItemListL();
	MFSMailBrandManager& brandManager = iAppUi.GetMailClient()->GetBrandManagerL();

    for ( TInt i = 0; i < iLauncherItems.Count(); i++ )
        {
        itemInModel.Append( EFalse );
        }

    TInt count = reader.ReadInt16();

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

    TAknLayoutRect scrollBarRect;
    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());
    TRect gridRect = mainPaneRect;
    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    TAknLayoutRect itemRect;
    itemRect.LayoutRect(gridRect, AknLayoutScalable_Apps::cell_cmail_l_pane(var,0,0));

    TAknLayoutRect gridIconLRect;
    gridIconLRect.LayoutRect(itemRect.Rect(), AknLayoutScalable_Apps::cell_cmail_l_pane_g1(var));
    TSize iconSize = gridIconLRect.Rect().Size();

    for ( TInt itemIndex = 0; itemIndex < count; itemIndex++ )
        {
        TInt itemId = reader.ReadInt16();
        switch ( itemId )
            {
            case EDefaultMailboxItem:
                {

                RPointerArray<CFSMailBox> mailBoxes;
                CleanupResetAndDestroyClosePushL( mailBoxes );
                TFSMailMsgId id;
                TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                    id,
                    mailBoxes );

                if ( mailBoxes.Count() > 0 )
                    {
                    // Try to get branded graphic
                    CGulIcon* mbIcon(0);
		         	TRAPD( err, mbIcon = brandManager.GetGraphicL( EFSMailboxIcon, mailBoxes[0]->GetId() ) );
					if ( err == KErrNone && mbIcon )
						{
						CleanupStack::PushL( mbIcon );
				    	AknIconUtils::SetSize(mbIcon->Bitmap(), iconSize);
					    AknIconUtils::SetSize(mbIcon->Mask(), iconSize);

					    // Create texture into TextureManager, If not already existing
					    // Note: size(0,0) means original icon size
					    iAppUi.FsTextureManager()->CreateBrandedMailboxTexture( mbIcon,
					    		                                                 mailBoxes[0]->GetId().PluginId(),
					    		                                                 mailBoxes[0]->GetId().Id(),
					    		                                                 TSize(0,0));
					    // Get branded mailbox icon
					    iconTexture = &iAppUi.FsTextureManager()->TextureByMailboxIdL( mailBoxes[0]->GetId().PluginId(),
					    		                                                        mailBoxes[0]->GetId().Id(),
					    		                                                        TSize(0,0));

						CleanupStack::PopAndDestroy( mbIcon );
						}
					else
						{
	                    iconTexture = &iAppUi.FsTextureManager()->TextureByIndex( EGridInboxTexture );
						}

   			        iIconArray.AppendL( iconTexture );

   			        // Branded mailbox name is nowadays set in new mailbox event
   			        // handling, so we don't need to use brand manager here anymore.
   			        iModel->AddL(
                        EShortcut,
                        EDefaultMailboxItem,
                        mailBoxes[0]->GetName(),
                        *iconTexture,
                        mailBoxes[0]->GetId(),
                        mailBoxes[0]->GetStandardFolderId( EFSInbox ) );

   			        iAppUi.SubscribeMailboxL( mailBoxes[0]->GetId() );
                    }

                CleanupStack::PopAndDestroy( &mailBoxes );
                }
                break;
            case EOtherMailboxItems:
                {
                RPointerArray<CFSMailBox> mailBoxes;
                CleanupResetAndDestroyClosePushL( mailBoxes );
                TFSMailMsgId id;
                TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                    id,
                    mailBoxes );

                for ( TInt i = 1; i < mailBoxes.Count(); i++ )
                    {
                   // Try to get branded graphic
                    CGulIcon* mbIcon(0);
		         	TRAPD( err, mbIcon = brandManager.GetGraphicL( EFSMailboxIcon,  mailBoxes[i]->GetId() ) );
					if ( err == KErrNone && mbIcon )
						{
						CleanupStack::PushL( mbIcon );
				    	AknIconUtils::SetSize(mbIcon->Bitmap(), iconSize);
					    AknIconUtils::SetSize(mbIcon->Mask(), iconSize);

					    // Create texture into TextureManager, If not already existing
					    iAppUi.FsTextureManager()->CreateBrandedMailboxTexture( mbIcon,
					    		                                                 mailBoxes[i]->GetId().PluginId(),
					    		                                                 mailBoxes[i]->GetId().Id(),
					    		                                                 TSize(0,0));
					    // Get branded mailbox icon
					    iconTexture = &iAppUi.FsTextureManager()->TextureByMailboxIdL( mailBoxes[i]->GetId().PluginId(),
					    		                                                        mailBoxes[i]->GetId().Id(),
					    		                                                        TSize(0,0));
						CleanupStack::PopAndDestroy( mbIcon );
						}
					else
						{
	                    iconTexture = &iAppUi.FsTextureManager()->TextureByIndex( EGridInboxTexture );
						}
  	                iIconArray.AppendL( iconTexture );
                    // Branded mailbox name is nowadays set in new mailbox event
                    // handling, so we don't need to use brand manager here anymore.
                    iModel->AddL(
                        EShortcut,
                        EDefaultMailboxItem,
                        mailBoxes[i]->GetName(),
                        *iconTexture,
                        mailBoxes[i]->GetId(),
                        mailBoxes[i]->GetStandardFolderId( EFSInbox ) );

                    iAppUi.SubscribeMailboxL( mailBoxes[i]->GetId() );
                    }

                CleanupStack::PopAndDestroy( &mailBoxes );
                }
                break;
            case EDirectoryItem:
                {
                RPointerArray<CFSMailBox> mailBoxes;
                CleanupResetAndDestroyClosePushL( mailBoxes );
                TFSMailMsgId id;
                TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                    id,
                    mailBoxes );

                for ( TInt i = 0; i < mailBoxes.Count(); i++ )
                    {
                    if ( TFsEmailUiUtility::IsRemoteLookupSupported( *mailBoxes[i] ) )
                        {
                        HBufC* text = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_GRIDITEM_DIRECTORY );
                        iconTexture = &iAppUi.FsTextureManager()->TextureByIndex( EGridDirectoryTexture );
                        iModel->AddL(EShortcut, EDirectoryItem, *text, *iconTexture );
                        CleanupStack::PopAndDestroy( text );
                        iIconArray.AppendL( iconTexture );
                        break;
                        }
                    }

                CleanupStack::PopAndDestroy( &mailBoxes );
                }
                break;
            case ESettingsItem:
                {
                HBufC* text = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_GRIDITEM_SETTINGS );
                iconTexture = &iAppUi.FsTextureManager()->TextureByIndex( EGridSettingsTexture );
                iModel->AddL(EShortcut, ESettingsItem, *text, *iconTexture );
                CleanupStack::PopAndDestroy( text );
                iIconArray.AppendL( iconTexture );
                }
                break;
            case EAddNewMailboxItem:
                {
                HBufC* text = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_GRIDITEM_ADD_NEW_MAILBOX );
                iconTexture = &iAppUi.FsTextureManager()->TextureByIndex( EGridAddNewBoxTexture );
                iModel->AddL(EShortcut, EAddNewMailboxItem, *text, *iconTexture );
                CleanupStack::PopAndDestroy( text );
                iIconArray.AppendL( iconTexture );
                }
                break;
            case EHelpItem:
                {
     		   // remove help support in pf5250
        	    if (! FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
        		   {
                   HBufC* text = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_GRIDITEM_HELP );
                   iconTexture = &iAppUi.FsTextureManager()->TextureByIndex( EGridHelpTexture );
                   iModel->AddL(EShortcut, EHelpItem, *text, *iconTexture );
                   CleanupStack::PopAndDestroy( text );
                   iIconArray.AppendL( iconTexture );
        		   }
                }
                break;
            case EIntellisyncFileSyncItem:
            case EIntellisyncTravelInfoItem:
            case EIntellisyncBackupItem:
            case EIntellisyncRestoreItem:
            case EIntellisyncUpgradeItem:
           	case EIntellisyncRemoteControlItem:
                {
                for ( TInt i = 0; i < iLauncherItems.Count(); i++ )
                    {
                    if ( iLauncherItems[i]->Id() == itemId )
                        {
                        itemInModel[i] = ETrue;
                        AddItemToModelL( iLauncherItems[i], i );
                        break;
                        }
                    }
                }
                break;
            default:
                break;
            }
        }

    CleanupStack::PopAndDestroy(); // reader internal state

    // Rest of the launcher items
    for ( TInt i = 0; i < iLauncherItems.Count(); i++ )
        {
        if ( ! itemInModel[i] )
            {
            AddItemToModelL( iLauncherItems[i], i );
            }
        }

    CleanupStack::PopAndDestroy( &itemInModel );
	}

void CFSEmailUiLauncherGridVisualiser::CreateCaptionForApplicationL(TUid aUid,
                                                TDes& aCaption,
                                                TBool aShortCaption)
    {
    FUNC_LOG;
    RApaLsSession ls;
    User::LeaveIfError( ls.Connect() );
    TApaAppInfo appInfo;
    TInt ret = ls.GetAppInfo( appInfo, aUid );
    if ( ret == KErrNone )
        {
        if ( !aShortCaption )
            {
            aCaption = appInfo.iCaption;
            }
        else
            {
            aCaption = appInfo.iShortCaption;
            }
        }
    ls.Close();
    }


TUid CFSEmailUiLauncherGridVisualiser::Id() const
	{
    FUNC_LOG;
	return AppGridId;
	}

void CFSEmailUiLauncherGridVisualiser::ChildDoActivateL(const TVwsViewId& aPrevViewId,
                     TUid /*aCustomMessageId*/,
                     const TDesC8& /*aCustomMessage*/)
    {
    FUNC_LOG;
    if ( !iConstructionCompleted )
        {
        DoFirstStartL();
        }

    if( iAppUi.CurrentFixedToolbar() )
        {
        iAppUi.CurrentFixedToolbar()->SetToolbarVisibility( EFalse );
        }

    // For initial mailbox query
	TBool startedFromOds = EFalse;
	// NULL wizard started parameter every time when activated again.
	iDoubleClickLock = EFalse;

    if ( aPrevViewId.iAppUid.iUid == 0 &&
        iAppUi.CurrentActiveView() != this )
        {
        // Started from wizard do not show query
        startedFromOds = ETrue;
        // This view activation has not come through according normal view
        // activation procedure, so we should ignore this by activating
        // the currently active view again. This has been made to
        // avoid problems when ODS setup is completed.
        if ( iAppUi.CurrentActiveView()->Id() == MailListId )
            {
            TMailListActivationData tmp;
            tmp.iReturnAfterWizard = ETrue;
            const TPckgBuf<TMailListActivationData> pkgOut( tmp );
            iAppUi.EnterFsEmailViewL( MailListId, KStartListReturnToPreviousFolder, pkgOut );
            }
        else if ( iAppUi.CurrentActiveView()->Id() == MailViewerId )
            {
            TMsgViewerActivationData tmp;
            const TPckgBuf<TMsgViewerActivationData> pkgOut( tmp );
            iAppUi.EnterFsEmailViewL( MailViewerId, KStartViewerReturnToPreviousMsg,  pkgOut);
            }
        else
            {
            iAppUi.EnterFsEmailViewL( iAppUi.CurrentActiveView()->Id() );
            }

        return;
        }

    if ( iRefreshNeeded )
        {
        CreateModelL();
        RescaleIconsL();
        VisualLayoutUpdatedL();
        }
    else
        {
        // scroll bar needs to be updated manually anyway
        UpdateScrollBarRangeL();
        }

    SetDefaultStatusPaneTextL();

    // Mailbox query is called here but shown only once in appui if needed
    // doNotshowQuery is ETrue when started from wizard
  	if ( !iFirstStartComplete )
  		{
  		iFirstStartComplete = ETrue;
	  	iAppUi.GridStarted( startedFromOds );
	    iAppUi.ShowMailboxQueryL();
  		}
  	else
  	    {
  	    // Ensure that FSMailServer is running, but don't do it on first
  	    // activation, as it's done anyway in AppUi's ConstructL
  	    TFsEmailUiUtility::EnsureFsMailServerIsRunning( iEikonEnv->WsSession() );
  	    }

    if( iRowCount > iVisibleRows )
        {
        iScrollbar->MakeVisible(ETrue);
        }
    else
        {
        iScrollbar->MakeVisible(EFalse);
        }

    iAppUi.HideTitlePaneConnectionStatus();

  	// Erase the navigation history when main grid activated. This is the default view
  	// of the application and back navigation is never possible from here. The view stack
  	// might be in inconsistent state because of some unexpected error in view switching.
  	// Erasing the history helps recovering from such situations.
  	iAppUi.EraseViewHistory();
  	FocusVisibilityChange( iAppUi.IsFocusShown() );
  	UpdateFocusVisibility();
    }

void CFSEmailUiLauncherGridVisualiser::ChildDoDeactivate()
    {
    FUNC_LOG;
    iScrollbar->MakeVisible(EFalse);
    }

void CFSEmailUiLauncherGridVisualiser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
    FUNC_LOG;

	if ( aResourceId == R_FSEMAILUI_MAINUIGRID_MENUPANE )
		{
	    if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
		   }

	    TFSLauncherGridMailboxStatus mbStatus = CheckMailboxStatusL();

	    // Checks if a device has a keyboard or not.
		if( !IsFocusShown() )
			{
			if( mbStatus.iMailboxCount <= 0 )
				{
				// If no mailboxes configured, dim all mailbox related items.
				aMenuPane->SetItemDimmed( EFsEmailUiCmdDeleteMailbox, ETrue );
				}

		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdOpen, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdSync, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdSyncAll, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelSync, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOffline, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOfflineAll, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnline, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnlineAll, ETrue );
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdAbout, ETrue );
			}
		else
			{
		   	aMenuPane->SetItemDimmed( EFsEmailUiCmdAbout, ETrue );

		   	if( mbStatus.iMailboxCount <= 0 )
		   	    {
		   	    // If no mailboxes configured, dimm all mailbox related items
		   	    aMenuPane->SetItemDimmed( EFsEmailUiCmdDeleteMailbox, ETrue );
	            aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelSync, ETrue);
	            aMenuPane->SetItemDimmed( EFsEmailUiCmdSync, ETrue );
	            aMenuPane->SetItemDimmed( EFsEmailUiCmdSyncAll, ETrue );
	            aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnline, ETrue );
	            aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnlineAll, ETrue );
	            aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOffline, ETrue );
	            aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOfflineAll, ETrue );
		   	    }
		   	else
		   	    {
		   	    // Handle items related to sync cancelling
		   	    if( mbStatus.iMailboxesSyncing == 0 )
		   	        {
		   	        // All mailboxes are already not syncing
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelSync, ETrue );
		   	        }

	            // Handle items related to sync starting
		   	    if( mbStatus.iMailboxesSyncing == mbStatus.iMailboxCount )
		   	        {
		   	        // All mailboxes are already syncing
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdSync, ETrue );
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdSyncAll, ETrue );
		   	        }
		   	    else if( mbStatus.iMailboxCount == 1 )
		   	        {
		   	        // Only one mailbox configured, dimm "Synchronise all"
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdSyncAll, ETrue );
		   	        }
		   	    else
		   	        {
		   	        // Several mailboxes configured, dimm "Synchronise"
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdSync, ETrue );
		   	        }

	            // Handle items related to online
		   	    if( mbStatus.iMailboxesOnline == mbStatus.iMailboxCount )
		   	        {
		   	        // All mailboxes are already online
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnline, ETrue );
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnlineAll, ETrue );
		   	        }
		   	    else if( mbStatus.iMailboxCount == 1 )
		   	        {
	                // Only one mailbox configured, dimm "Connect all"
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnlineAll, ETrue );
		   	        }
		   	    else
		   	        {
	                // Several mailboxes configured, dimm "Connect"
	                aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOnline, ETrue );
		   	        }

	            // Handle pop accounts that can't sync
	            RPointerArray<CFSMailBox> mailBoxes;
	            CleanupResetAndDestroyClosePushL( mailBoxes );
	            TFSMailMsgId id;
	            bool onlyPop = true;
	            TInt err = iAppUi.GetMailClient()->ListMailBoxes(
	                id,
	                mailBoxes );

                for ( TInt i = 0; i < mailBoxes.Count(); i++ )
                    {
                    if (mailBoxes[i]->HasCapability( EFSMBoxCapaSupportsSync ))
                        {
                        onlyPop = false;
                        }
                    }

                if (onlyPop)
                    {
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdSync, ETrue );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdSyncAll, ETrue );
                    }

                CleanupStack::PopAndDestroy( &mailBoxes );

                // Handle items related to offline
                if( mbStatus.iMailboxesOffline == mbStatus.iMailboxCount )
                    {
                    // All mailboxes are already offline
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOffline, ETrue );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOfflineAll, ETrue );
                    }
                else if( mbStatus.iMailboxCount == 1 )
                    {
                    // Only one mailbox configured, dimm "Disconnect all"
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOfflineAll, ETrue );
                    }
                else
                    {
                    // Several mailboxes configured, dimm "Disconnect"
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdGoOffline, ETrue );
                    }
                }

            // Add shortcut hints
            iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane,
    	                             CFSEmailUiShortcutBinding::EContextMainGrid );
            }
        }
	}

void CFSEmailUiLauncherGridVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;

    switch ( aCommand )
        {
		case EAknSoftkeyOpen:
			{
			if( !iAppUi.IsFocusShown() )
				{
				// No need to handle return value
                iAppUi.SetFocusVisibility( ETrue );
                UpdateFocusVisibility();
                ResizeItemIcon( EFalse );
				break;
				}
			}
       	case EFsEmailUiCmdOpen:
			{
			SelectL();
			break;
			}
		case EFsEmailUiCmdDeleteMailbox:
			{
			// Deletion by using the option menu.
			iMailboxDeleter->DeleteMailboxL();
			break;
			}
		case EFsEmailUiCmdDeleteSelectedMailbox:
			{
			// Deletion by using the long tap pop-up menu.
			iMailboxDeleter->DeleteMailboxL( iMailboxToDelete );

	    	// Hide the focus.
	        iAppUi.SetFocusVisibility( EFalse );
	        HandleButtonReleaseEvent(); // Finishes the focus removal.
	        iStylusPopUpMenuLaunched = EFalse;
			break;
			}
		case EFsEmailUiCmdSync:
        case EFsEmailUiCmdSyncAll:
			{
            RPointerArray<CFSMailBox> mailBoxes;
            CleanupResetAndDestroyClosePushL( mailBoxes );
            TFSMailMsgId id;
            TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                id,
                mailBoxes );

            for ( TInt i = 0; i < mailBoxes.Count(); i++ )
                {
                iAppUi.SubscribeMailboxL( mailBoxes[i]->GetId() );
                mailBoxes[i]->GetMailBoxStatus();
                mailBoxes[i]->RefreshNowL( iAppUi );
                }

            CleanupStack::PopAndDestroy( &mailBoxes );
			}
            break;
		case EFsEmailUiCmdCancelSync:
			{
            RPointerArray<CFSMailBox> mailBoxes;
            CleanupResetAndDestroyClosePushL( mailBoxes );
            TFSMailMsgId id;
            TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                id,
                mailBoxes );

            for ( TInt i = 0; i < mailBoxes.Count(); i++ )
                {
                iAppUi.SubscribeMailboxL( mailBoxes[i]->GetId() );
                mailBoxes[i]->CancelSyncL();
                }

            CleanupStack::PopAndDestroy( &mailBoxes );
			}
            break;

		case EFsEmailUiCmdGoOffline:
        case EFsEmailUiCmdGoOfflineAll:
			{
            if (aCommand == EFsEmailUiCmdGoOfflineAll)
            	{
                iAppUi.ManualMailBoxDisconnectAll(ETrue);
            	}
            RPointerArray<CFSMailBox> mailBoxes;
            CleanupResetAndDestroyClosePushL( mailBoxes );
            TFSMailMsgId id;
            TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                id,
                mailBoxes );

            for ( TInt i = 0; i < mailBoxes.Count(); i++ )
                {
                iAppUi.SubscribeMailboxL( mailBoxes[i]->GetId() );
                mailBoxes[i]->GoOfflineL();
                }

            CleanupStack::PopAndDestroy( &mailBoxes );
			}
            break;
		case EFsEmailUiCmdGoOnline:
        case EFsEmailUiCmdGoOnlineAll:
			{
            if (aCommand == EFsEmailUiCmdGoOnlineAll)
            	{
               	iAppUi.ManualMailBoxConnectAll(ETrue);
            	}
            RPointerArray<CFSMailBox> mailBoxes;
            CleanupResetAndDestroyClosePushL( mailBoxes );
            TFSMailMsgId id;
            TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                id,
                mailBoxes );

            for ( TInt i = 0; i < mailBoxes.Count(); i++ )
                {
                iAppUi.SubscribeMailboxL( mailBoxes[i]->GetId() );
                mailBoxes[i]->GoOnlineL();
                }

            CleanupStack::PopAndDestroy( &mailBoxes );
			}
            break;
       	case EFsEmailUiCmdAbout:
			{
			DisplayProductInfoL();
			}
			break;
       	case EFsEmailUiCmdHelp:
			{
            TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
			}
			break;
       	case KErrCancel:
       		{
        	// The pop-up menu was closed. Hide the focus.
            iAppUi.SetFocusVisibility( EFalse );
            HandleButtonReleaseEvent(); // Finishes the focus removal.
	        iStylusPopUpMenuLaunched = EFalse;
       		break;
       		}
        case EFsEmailUiCmdHide:
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EFsEmailUiCmdExit:
            {
            iAppUi.Exit();
            }
            break;
        default:
            break;
        } // switch ( aCommand )
    }

TBool CFSEmailUiLauncherGridVisualiser::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
        {
        // If wait note is being shown while a mailbox is being created,
        // then do not react to key presses.
        if ( iWizardWaitnoteShown )
            {
            return ETrue; // key consumed
            }

        // Swap right and left controls in mirrored layout
        TInt scanCode = aEvent.KeyEvent().iScanCode;
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            if (scanCode == EStdKeyRightArrow) scanCode = EStdKeyLeftArrow;
            else if (scanCode == EStdKeyLeftArrow ) scanCode = EStdKeyRightArrow;
            }

        if ((scanCode == EStdKeyRightArrow)
        		|| (scanCode == EStdKeyLeftArrow)
        		|| (scanCode == EStdKeyUpArrow)
        		|| (scanCode == EStdKeyDownArrow)
        		|| (scanCode == EStdKeyEnter)
        		|| (scanCode == EStdKeyDeviceA)
        		|| (scanCode ==EStdKeyDevice3))
        	{
            TBool scrolled = iScrolled;
            if ( iScrolled )
                {
                iScrolled = EFalse;
                SetFocusedItemL( iFirstVisibleRow * iVisibleColumns );
                }
            
            //iCurrentLevel.iSelected = 
        	if ( !iAppUi.SetFocusVisibility( ETrue ) ||
        	     scrolled )
        		{
				// focus is now activated. ignore key press.
				UpdateFocusVisibility();
				ResizeItemIcon( EFalse );
				return ETrue;
        		}
        	}

        switch(scanCode)
            {
            case EStdKeyRightArrow:
                MoveSelection(EDirectionRight);
                return ETrue;
            case EStdKeyLeftArrow:
                MoveSelection(EDirectionLeft);
                return ETrue;
            case EStdKeyUpArrow:
                MoveSelection(EDirectionUp);
                return ETrue;
            case EStdKeyDownArrow:
                MoveSelection(EDirectionDown);
                return ETrue;
            case EStdKeyEnter:
            case EStdKeyDeviceA:
            case EStdKeyDevice3:
            	HandleButtonReleaseEvent();
                SelectL();
                return ETrue;
            default:
	       	    // Check keyboard shortcuts.
	       	    TInt shortcutCommand =
	       	        iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
	       	                                                         CFSEmailUiShortcutBinding::EContextMainGrid );
	       	    if ( shortcutCommand != KErrNotFound )
	       	        {
       	            HandleCommandL( shortcutCommand );
	       	        return ETrue;
	       	        }
               return EFalse;
            }
        }
    return EFalse; // was not consumed
    }


// ---------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::HandlePointerEventL
//
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiLauncherGridVisualiser::HandlePointerEventL(
	const TAlfEvent& aEvent )
    {
    FUNC_LOG;
    if( !IsViewActive() )
        {
        return EFalse;
        }
    TPointerEvent::TType type = aEvent.PointerEvent().iType;
    TInt id = FindPointedItem( aEvent );

    switch( type )
        {
        case TPointerEvent::EButton1Down:
            {
            iPreviousPosition = iOriginalPosition = aEvent.PointerEvent().iParentPosition;
            iPointerAction = ETrue;
            iIsDragging = EFalse;

            if( iPhysics )
                {
                iPhysics->StopPhysics();
                iPhysics->ResetFriction();
                iStartTime.HomeTime();
                UpdatePhysicsL();
                iTotalDragging = 0;
                }            
            if ( id != KErrNotFound)
                {
                // tactile feedback
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( ETouchFeedbackBasic );
                    }

                iItemIdInButtonDownEvent.iItemId = id;
                iItemIdInButtonDownEvent.iLaunchSelection = ETrue;
                SetFocusedItemL( id );
                UpdateFocusVisibility();
                }
            break;
            }
        case TPointerEvent::EButton1Up:
            {
            if( iIsDragging && iPhysics )
                {
                TPoint drag( iOriginalPosition - aEvent.PointerEvent().iParentPosition ); 
                iPhysics->StartPhysics( drag, iStartTime );
                iIsDragging = EFalse;
                iPointerAction = EFalse;
                iTotalDragging = 0;
                }
            else if ( id != KErrNotFound )
                {
                if ( iStylusPopUpMenuLaunched )
                    {
                    // A pop-up menu was launched. Do not open the selected
                    // item.
                    iItemIdInButtonDownEvent.iLaunchSelection = EFalse;
                    break;
                    }

                // Hide focus always after pointer up event.
                iAppUi.SetFocusVisibility( EFalse );

                // If key was released on item that had focus
                // trigger selection.
                if( iItemIdInButtonDownEvent.iItemId == id &&
                    iItemIdInButtonDownEvent.iLaunchSelection )
                    {
                    HandleButtonReleaseEvent();
                    // LAUNCH OPENING. This may leave if user cancels the
                    // operation
                    SelectL();
                    }
                else
                    {
                    HandleButtonReleaseEvent();
                    }
                }
            else if( iItemIdInButtonDownEvent.iItemId != KErrNotFound )
                {
                iItemIdInButtonDownEvent.iLaunchSelection = EFalse;
                ResizeItemIcon( ETrue );

                // Hide focus always after pointer up event.
                iAppUi.SetFocusVisibility( EFalse );
                iItemIdInButtonDownEvent.iItemId = KErrNotFound;
                HandleButtonReleaseEvent();
                }
            else
                {
                iAppUi.SetFocusVisibility( EFalse );
                }
			break;
            }
        case TPointerEvent::EDrag:
            {
            if( iPhysics )
                {
                TPoint position = aEvent.PointerEvent().iParentPosition;
                TPoint delta( 0, iPreviousPosition.iY - position.iY );
                iTotalDragging = iTotalDragging + delta.iY;
                if (Abs(iTotalDragging) >= iPhysics->DragThreshold() || iIsDragging )
                    {
                    // Hide focus always when dragging.
                    iAppUi.SetFocusVisibility( EFalse );
                    if ( iSelector )
                        {
                        TAlfTimedValue selectorOpacity;
                        selectorOpacity.SetValueNow( 0 );
                        iSelector->SetOpacity( selectorOpacity );
                        }

                    iIsDragging = ETrue;
                
                    iPhysics->RegisterPanningPosition( delta );
                    iScrolled = ETrue;
                    }
                // Save current position as previous pos for future calculations
                iPreviousPosition = position;                
                }
            break;
            }
        case TPointerEvent::EButtonRepeat:
          	{
           	if (!iIsDragging && id != KErrNotFound )
           	    {
   	            // Check the type of the currently selected item.
    			TInt itemType = iCurrentLevel.iItems[id].iId;

				if ( itemType == EDefaultMailboxItem ||
					 itemType == EOtherMailboxItems )
   					{
   					// The selected item is a mail box. Launch the pop-up
   					// menu.
   	            	LaunchStylusPopupMenu( id );
    				}
           	    }
           	break;
            }
        default:
            {
            break;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::FindPointedItem
//
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiLauncherGridVisualiser::FindPointedItem( const TAlfEvent& aEvent )
    {
    FUNC_LOG;
    TInt result = KErrNotFound;

    TInt count = iCurrentLevel.iItemVisualData.Count();
    const TPoint pos = aEvent.PointerEvent().iParentPosition;

    for ( TInt a = 0; count > a; a++ )
        {
        const TRect rect( iCurrentLevel.iItemVisualData[a].iBase->DisplayRect() );
        if ( rect.Contains( pos ) )
            {
            result = a;
            break;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::SetFocusedItemL
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::SetFocusedItemL( TInt aId )
    {
    FUNC_LOG;
    TInt oldSelection = iCurrentLevel.iSelected;

    HandleRowMovement( EDirectionTouch, aId );

    FocusItem( EFalse, oldSelection );
    FocusItem( ETrue, iCurrentLevel.iSelected );
    MoveSelectorToCurrentItem( EDirectionTouch );
    }

void CFSEmailUiLauncherGridVisualiser::MoveSelection( TDirection aDir )
    {
    FUNC_LOG;
    // NULL double click flag just be sure that the UI does not
    // go into state that wizard cannot be started.
    iDoubleClickLock = EFalse;
    // Store old selection
    TInt oldSelection = iCurrentLevel.iSelected;
    TInt itemCount = iCurrentLevel.iItemVisualData.Count();

    HandleRowMovement( aDir, iCurrentLevel.iSelected );

    // Set the correct icon focuses (i.e. enlarged)
    if ( aDir != EDirectionNone && oldSelection >= 0 && oldSelection < itemCount )
        {
        FocusItem( EFalse, oldSelection );
        }
    FocusItem( ETrue, iCurrentLevel.iSelected );

    // Move the selector over the newly focused icon
    MoveSelectorToCurrentItem( aDir );
    UpdateFocusVisibility();
    }


void CFSEmailUiLauncherGridVisualiser::HandleRowMovement( TDirection aDir, TInt aSelected )
    {
    FUNC_LOG;
    // NULL double click flag just be sure that the UI does not
    // go into state that wizard cannot be started.
    iDoubleClickLock = EFalse;
    // Store old selection
    TInt oldSelection = iCurrentLevel.iSelected;
    TInt itemCount = iCurrentLevel.iItemVisualData.Count();
    TInt oldX = 0;
    TInt oldY = 0;
    if (itemCount <= 1)
        {
        iCurrentLevel.iSelected = 0;
        }
    else
        {
        oldX = oldSelection % iVisibleColumns;
        oldY = (oldSelection-oldX) / iVisibleColumns;

        switch( aDir )
            {
            case EDirectionRight:
                iCurrentLevel.iSelected++;
                if ( iCurrentLevel.iSelected >= itemCount )
                    {
                    iCurrentLevel.iSelected = 0;
                    }
                break;

            case EDirectionLeft:
                iCurrentLevel.iSelected--;
                if ( iCurrentLevel.iSelected < 0 )
                    {
                    iCurrentLevel.iSelected = itemCount-1;
                    }
                break;

            case EDirectionUp:
                iCurrentLevel.iSelected -= iVisibleColumns; // one row up
                if ( iCurrentLevel.iSelected < 0 )
                    {
                    iCurrentLevel.iSelected += (iRowCount*iVisibleColumns);
                    if ( iCurrentLevel.iSelected >= itemCount )
                        {
                        // Wrapping is about to move the cursor on empty spot.
                        // To be consistent with S60 app grid, move selection to the last item.
                        iCurrentLevel.iSelected = itemCount-1;
                        }
                    }
                break;

            case EDirectionDown:
                iCurrentLevel.iSelected += iVisibleColumns; // one row down
                if ( iCurrentLevel.iSelected >= itemCount )
                    {
                    if ( oldY < iRowCount-1 )
                        {
                        iCurrentLevel.iSelected = itemCount-1;
                        }
                    else
                        {
                        iCurrentLevel.iSelected %= (iRowCount*iVisibleColumns);
                        }
                    }
                break;

            case EDirectionReset:
                iCurrentLevel.iSelected = KDefaultSelection;
                break;

            case EDirectionNone:
                break;

            case EDirectionTouch:
                iCurrentLevel.iSelected = aSelected;
                break;

            }

        if ( iCurrentLevel.iSelected < 0 )
            {
            iCurrentLevel.iSelected = 0;
            }
        if ( iCurrentLevel.iSelected >= itemCount )
            {
            iCurrentLevel.iSelected = itemCount - 1;
            }
        }
    if (!iPointerAction)
        {
        TInt x = iCurrentLevel.iSelected % iVisibleColumns;
        TInt y = (iCurrentLevel.iSelected-x) / iVisibleColumns;
        ScrollToRow( y );
        }
    }


void CFSEmailUiLauncherGridVisualiser::MoveSelectorToCurrentItem( TDirection aDir )
    {
    FUNC_LOG;
    TAlfRealPoint curPos = iSelector->Pos().ValueNow(); // this is the wrapped value of the current position
    iSelector->SetPos( curPos, 0 ); // wrap position now

    // Calculate where are we heading
    CAlfVisual* selectedBase = iCurrentLevel.iItemVisualData[iCurrentLevel.iSelected].iBase;
    TPoint displayPos = selectedBase->LocalToDisplay( selectedBase->Pos().Target() );
    TPoint targetPos = iSelector->DisplayToLocal( displayPos );

    // Check if we need to wrap the selector over the edge of the screen
    TPoint ringWrapOffset( 0, 0 );
    const TInt KGridWrapWidth = (iVisibleColumns+2)*iColumnWidth;
    const TInt KGridWrapHeight = (iRowCount+2)*iRowHeight;

    if ( aDir == EDirectionRight && ( targetPos.iX <= curPos.iX || iCurrentLevel.iSelected == 0 ) )
        {
        ringWrapOffset.iX = KGridWrapWidth;
        }
    else if ( aDir == EDirectionLeft && ( targetPos.iX >= curPos.iX  || iCurrentLevel.iSelected == iCurrentLevel.iItems.Count()-1 ) )
        {
        ringWrapOffset.iX = -KGridWrapWidth;
        }
    else if ( aDir == EDirectionUp && targetPos.iY > curPos.iY )
        {
        ringWrapOffset.iY = -KGridWrapHeight;
        }
    else if ( aDir == EDirectionDown && targetPos.iY < curPos.iY )
        {
        ringWrapOffset.iY = KGridWrapHeight;
        }
    targetPos += ringWrapOffset;

    // Animate the movement to the new position
    TInt animTime = KSelectTransitionTimeMs;
    if ( aDir == EDirectionReset || aDir == EDirectionNone || aDir == EDirectionTouch )
        {
        animTime = KStartupAnimationTime;
        }
    iSelector->SetPos( targetPos, animTime );
    }

void CFSEmailUiLauncherGridVisualiser::FocusItem( TBool aHasFocus, TInt aItem )
    {
    FUNC_LOG;
    if ( iCurrentLevel.iItemVisualData.Count() <= aItem ||
         aItem < 0 )
        {
        // Invalid item index
        return;
        }

    if( aHasFocus && ( IsFocusShown() || iItemIdInButtonDownEvent.iItemId >= 0 ) )
        {
        TAlfTimedValue scaleValue;
        scaleValue.SetTarget( KScaleSelected, KIconScalingTransitionTimeMs );
        iCurrentLevel.iItemVisualData[aItem].iImage->SetScale( scaleValue );
        }
    else
        {
        TAlfTimedValue scaleValue;
        scaleValue.SetTarget( KScaleNotSelected, KIconScalingTransitionTimeMs * 2 );
        iCurrentLevel.iItemVisualData[aItem].iImage->SetScale( scaleValue );
        }
    }


void CFSEmailUiLauncherGridVisualiser::ScrollToRow( TInt aRow )
    {
    FUNC_LOG;
	const TInt KScrollTransitionTimeMs = KSelectTransitionTimeMs;
	TReal offset( 0.0 );

    if ( aRow >= iRowCount || aRow < 0 )
        {
        // Invalid row
        return;
        }

    if ( iFirstVisibleRow + iVisibleRows - 1 < aRow )
        {
        // Scroll downwards
        iFirstVisibleRow = aRow - iVisibleRows + 1 ;
        }
    else if ( iFirstVisibleRow > aRow )
        {
        // Scroll upwards
        if ( aRow == 0 )
            {
            iFirstVisibleRow = 0;
            }
        else
            {
            iFirstVisibleRow = aRow;
            }
        }


    offset = iFirstVisibleRow * iRowHeight;

    TAlfTimedPoint alfScrollOffset;
    alfScrollOffset.iY.SetTarget( offset , KScrollTransitionTimeMs );
    iCurrentLevel.iGridLayout->SetScrollOffset(alfScrollOffset);
    iScrollbarModel.SetFocusPosition(offset);
    TRAP_IGNORE( iScrollbar->SetModelL(&iScrollbarModel) );
    iScrollbar->DrawNow();
    }

void CFSEmailUiLauncherGridVisualiser::RefreshLauncherViewL()
    {
    FUNC_LOG;
    if ( iConstructionCompleted )
        {
        iDoubleClickLock = EFalse;
        TInt count = iCurrentLevel.iItemVisualData.Count();

        CreateModelL();
        PopulateL( iCurrentLevel );
        SetRingWrapLimits();
        if ( count != iCurrentLevel.iItemVisualData.Count() )
            {
            MoveSelection( EDirectionReset );
            }
        else
            {
            FocusItem( ETrue, iCurrentLevel.iSelected );
            }
        UpdateScrollBarRangeL();
        }
    }

TBool CFSEmailUiLauncherGridVisualiser::UiOperationLaunched()
    {
    FUNC_LOG;
    return iUiOperationLaunched;
    }

void CFSEmailUiLauncherGridVisualiser::PopulateL( TLevel& aLevel )
    {
    FUNC_LOG;

    if ( iConstructionCompleted )
        {
        for( TInt i = 0; i < aLevel.iItemVisualData.Count(); i++ )
            {
            aLevel.iItemVisualData[i].iBase->RemoveAndDestroyAllD();
            }

        aLevel.iItemVisualData.Reset();
        aLevel.iItems.Reset();

	    TRect mainPaneRect;
	    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

	    TAknLayoutRect scrollBarRect;
	    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());
	    TRect gridRect = mainPaneRect;
	    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

	    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;

	    TAknLayoutRect itemRect;
	    itemRect.LayoutRect(gridRect, AknLayoutScalable_Apps::cell_cmail_l_pane(var, 0, 0));

	    TRect itemrc = itemRect.Rect();
	    itemrc.SetRect(TPoint(0,0), itemRect.Rect().Size());

	    TAknLayoutRect gridIconRect;
	    gridIconRect.LayoutRect(itemrc, AknLayoutScalable_Apps::cell_cmail_l_pane_g1(var));

	    TAknLayoutText gridText;
	    gridText.LayoutText(itemrc, AknLayoutScalable_Apps::cell_cmail_l_pane_t1(var));

        if ( aLevel.iParent >= 0 )
            {
            iModel->FindChildren( aLevel.iParent, aLevel.iItems );

            for ( TInt i = 0; i < aLevel.iItems.Count(); i++ )
                {
                TItemVisualData newItem;

                newItem.iBase = CAlfAnchorLayout::AddNewL( *iControl, aLevel.iGridLayout );
                newItem.iBase->SetTactileFeedbackL( ETouchEventStylusDown,  ETouchFeedbackBasic );

                // Set anchors for text.
                newItem.iBase->SetAnchor(
                    EAlfAnchorTopLeft,
                    0,
                    EAlfAnchorOriginLeft,
                    EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint( gridText.TextRect().iTl.iX, gridText.TextRect().iTl.iY ) );// 0, gridIconSize+KTopMargin ) );
                newItem.iBase->SetAnchor(
                    EAlfAnchorBottomRight,
                    0,
                    EAlfAnchorOriginLeft,
                    EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint( gridText.TextRect().iBr.iX, gridText.TextRect().iBr.iY ) );//0, iRowHeight ) );

                // Set anchors for image
                newItem.iBase->SetAnchor(
                    EAlfAnchorTopLeft,
                    1,
                    EAlfAnchorOriginLeft,
                    EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint( gridIconRect.Rect().iTl.iX, gridIconRect.Rect().iTl.iY ) ); //-gridIconSize/2, KTopMargin ) );
                newItem.iBase->SetAnchor(
                    EAlfAnchorBottomRight,
                    1,
                    EAlfAnchorOriginLeft,
                    EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint( gridIconRect.Rect().iBr.iX, gridIconRect.Rect().iBr.iY ) ); //gridIconSize/2, gridIconSize+KTopMargin ) );

                newItem.iText = CAlfTextVisual::AddNewL( *iControl, newItem.iBase );
                newItem.iText->EnableShadow( EFalse );

				TRgb itemColor( KRgbGray );

			    // text #9 application grid unfocused application title texts #215
				if( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
					itemColor, KAknsIIDQsnTextColors,
					EAknsCIQsnTextColorsCG9 ) != KErrNone )
					{
					itemColor = gridText.Color();
					}

				newItem.iText->SetColor( itemColor );
                newItem.iText->SetTextL( *aLevel.iItems[i].iCaption );

                TAlfTimedValue opacity;
                opacity.SetValueNow ( KDefaultCaptionOpacity );
                newItem.iText->SetOpacity( opacity );

                newItem.iText->SetTextStyle( iAppUi.LayoutHandler()->FSTextStyleFromLayoutL(AknLayoutScalable_Apps::cell_cmail_l_pane_t1(var)).Id() );//FSTextStyleFromIdL( EFSFontTypeSmall )->Id() );
                newItem.iText->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
                newItem.iText->SetAlign( EAlfAlignHCenter, EAlfAlignVTop );

                newItem.iImage = CAlfImageVisual::AddNewL( *iControl, newItem.iBase );
                newItem.iImage->SetScaleMode( CAlfImageVisual::EScaleFit );
                newItem.iImage->SetImage( TAlfImage( *aLevel.iItems[i].iIconTexture ) );
                newItem.iImage->SetFlag( EAlfVisualFlagManualSize );

                newItem.iImage->SetSize( gridIconRect.Rect().Size() );
                newItem.iImage->SetScale( KScaleNotSelected );

                User::LeaveIfError( aLevel.iItemVisualData.Append( newItem ) );
                }
            }

        // Set columns and rows
        iVisibleRows = AknLayoutScalable_Apps::cell_cmail_l_pane_ParamLimits(var).LastRow() + 1;
        iVisibleColumns = AknLayoutScalable_Apps::cell_cmail_l_pane_ParamLimits(var).LastColumn() + 1;

        iRowCount = ( iCurrentLevel.iItemVisualData.Count() + iVisibleColumns - 1 ) / iVisibleColumns;

        aLevel.iGridLayout->MoveToFront();
        }
    }

void CFSEmailUiLauncherGridVisualiser::SelectL()
	{
    FUNC_LOG;
	if ( !iAppUi.ViewSwitchingOngoing() )
		{
        iPointerAction = EFalse;
        iItemIdInButtonDownEvent.iItemId = KErrNotFound;
        UpdateFocusVisibility();

		CFSEmailLauncherItem* launcherItem =
		    iCurrentLevel.iItems[iCurrentLevel.iSelected].iLauncherItem;
		if ( launcherItem )
		    {
	        launcherItem->ExecuteActionL();
		    }
		else
		    {
		    iUiOperationLaunched = ETrue;
			switch ( iCurrentLevel.iItems[iCurrentLevel.iSelected].iId )
				{
				case EDefaultMailboxItem:
				case EOtherMailboxItems:
					{
					TMailListActivationData tmp;
					tmp.iFolderId = iCurrentLevel.iItems[iCurrentLevel.iSelected].iMailBoxInboxId;
					tmp.iMailBoxId = iCurrentLevel.iItems[iCurrentLevel.iSelected].iMailBoxId;
					const TPckgBuf<TMailListActivationData> pkgOut( tmp );
					iAppUi.ShowTitlePaneConnectionStatus();
					iAppUi.EnterFsEmailViewL( MailListId, KStartListWithFolderId, pkgOut );
					}
					break;
				case EDirectoryItem:
				    {
				    // TO prevent accidental double clicks of the directory item,
				    // prevents multiple RCL windows to be seen
                    if ( !iDoubleClickLock )
                        {
                        // Lock to make sure that wizard is not started twice in a row
                        iDoubleClickLock = ETrue;
                        RPointerArray<CFSMailBox> mailBoxes;
                        CleanupResetAndDestroyClosePushL( mailBoxes );
                        TFSMailMsgId id;
                        TInt err = iAppUi.GetMailClient()->ListMailBoxes(
                            id,
                            mailBoxes );

                        // Remove mailboxes that doesn't support RCL
                        for ( TInt i = mailBoxes.Count()-1; i >= 0 ; i-- )
                            {
                            if ( !TFsEmailUiUtility::IsRemoteLookupSupported( *mailBoxes[i] ) )
                                {
                                delete mailBoxes[i];
                                mailBoxes.Remove( i );
                                }
                            }

                        CFSMailBox* mailBox = NULL;
                        if( mailBoxes.Count() == 1 )
                            {
                            // Only one mailbox with RCL support so we use that
                            mailBox = mailBoxes[0];
                            }
                        else if( mailBoxes.Count() > 1 )
                            {
                            // Several mailboxes that support RCL so we need to
                            // ask the user which one to use
                            mailBox = ShowMailboxSelectionQueryL( mailBoxes );
                            }

                        if ( mailBox )
                            {
                            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->LaunchRemoteLookupL( *mailBox );
                            }
                        iDoubleClickLock = EFalse;
                        CleanupStack::PopAndDestroy( &mailBoxes );
                        }
				    }
					break;
	            case ESettingsItem:
	                {
	                TInt tmp = 0;
	                const TPckgBuf<TInt> pkgBuf( tmp );
	      			iAppUi.EnterFsEmailViewL(
	      					SettingsViewId,
	      					TUid::Uid(KMailSettingsOpenMainList),
	      					pkgBuf );
	                }
	            	break;
	            case EAddNewMailboxItem:
	                {
                    // To prevent accidental double clicks of the wizard item
	                // wizard would crash without this - moved to function
	                    LaunchWizardL();
	                }
	            	break;
	            case EHelpItem:
	                {
                    // To prevent accidental double clicks of the help item
                    // Help app might crash without this
	                if ( !iDoubleClickLock )
	                    {
	                    iDoubleClickLock = ETrue;
	                    TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
	                    }
	                }
	            	break;
				default:
					return;
				}
		    }
		}
	}


void CFSEmailUiLauncherGridVisualiser::HandleForegroundEventL( TBool aForeground )
    {
    FUNC_LOG;

    UpdateFocusVisibility();

    // Toggle safety lock always when receiving foreground back.
    if ( aForeground && iDoubleClickLock && iFirstStartComplete )
        {
        iDoubleClickLock = EFalse;
        }
    }

void CFSEmailUiLauncherGridVisualiser::GetParentLayoutsL(
        RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    aLayoutArray.AppendL( iParentLayout );
    }

// hide or show CAlfVisuals ( used for activation or deactivation )
void CFSEmailUiLauncherGridVisualiser::FadeOut(TBool aDirectionOut)
	{
	if ( iParentLayout != NULL )
		{
	    TAlfTimedValue timedValue( 0, 0 );
	    if ( !aDirectionOut )
	        {
	        timedValue.SetTarget( 1, 0 );
	        }
	    iParentLayout->SetOpacity( timedValue );
		}
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::ShowMailboxSelectionQueryL
// -----------------------------------------------------------------------------
CFSMailBox* CFSEmailUiLauncherGridVisualiser::ShowMailboxSelectionQueryL(
        const RPointerArray<CFSMailBox>& aMailBoxes )
    {
    FUNC_LOG;
    CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat( 2 );
    CleanupStack::PushL( array );

    for( TInt i = 0; i < aMailBoxes.Count(); i++ )
        {
        array->AppendL( aMailBoxes[i]->GetName() );
        }

    TInt selectedOption;
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedOption );
    dlg->PrepareLC( R_FSEMAILUI_LAUNCHER_GRID_MAILBOX_SELECTION_DIALOG  );

    dlg->SetItemTextArray( array );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    CFSMailBox* mailbox = NULL;
    if( dlg->RunLD() )
        {
        // safety check.
        if( selectedOption >= 0 &&
            selectedOption < aMailBoxes.Count() )
            {
            mailbox = aMailBoxes[selectedOption];
            }
        }

    CleanupStack::PopAndDestroy( array );
    return mailbox;
    }

void CFSEmailUiLauncherGridVisualiser::LaunchWizardL()
    {
    FUNC_LOG;
    // Start wizard.

    // Make sure that FSMailServer is running, so that the mailbox is added
    // also to MCE. It should be enough to do it here, because it takes some
    // time from wizard startup until the actual mailbox is created, so the
    // serve has some time to get itself up and running before that.
    TFsEmailUiUtility::EnsureFsMailServerIsRunning( iEikonEnv->WsSession() );
    //we are calling DoFirstStarL() method because in case User goes to GeneralSettings->Email
    //iAiwSHandler isnt constructed and in that case EmailUi panics
    if ( !iConstructionCompleted )
        {
        DoFirstStartL();
        }

    if ( ! iLaunchWizardExecuted  ) // prevent reentrant calling
        {
        iLaunchWizardExecuted = ETrue;
        TRAPD( err, iAiwSHandler->ExecuteServiceCmdL( KAiwCmdSettingWizardFsEmail.iUid,
                                          iAiwSHandler->InParamListL(),
                                          iAiwSHandler->OutParamListL() ) );
       // ExecuteServiceCmdL is synchronous - uses CActiveSchedulerWait
        iLaunchWizardExecuted = EFalse;
        User::LeaveIfError( err );
        }

    }

void CFSEmailUiLauncherGridVisualiser::GoToInboxL( TFSMailMsgId& aMailboxId, TFSMailMsgId& aMailboxInboxId )
    {
    FUNC_LOG;
	TMailListActivationData tmp;

	tmp.iMailBoxId = aMailboxId;
	tmp.iFolderId = aMailboxInboxId;

	const TPckgBuf<TMailListActivationData> pkgOut( tmp );

	iAppUi.EnterFsEmailViewL( MailListId, KStartListWithFolderId, pkgOut );
    }

void CFSEmailUiLauncherGridVisualiser::RescaleIconsL()
	{
    FUNC_LOG;
    if ( iConstructionCompleted )
        {
		TRect mainPaneRect;
	    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
	    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;

	    TAknLayoutRect scrollBarRect;
	    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());
	    TRect gridRect = mainPaneRect;
	    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

	    TAknLayoutRect itemRect;
	    itemRect.LayoutRect(gridRect, AknLayoutScalable_Apps::cell_cmail_l_pane(var, 0, 0));

	    TAknLayoutRect gridIconRect;
	    gridIconRect.LayoutRect(itemRect.Rect(), AknLayoutScalable_Apps::cell_cmail_l_pane_g1(var));

	   	//TInt gridIconSize = iAppUi.LayoutHandler()->GridIconSize();
		TSize iconSize = gridIconRect.Rect().Size();
		//iconSize.SetSize( gridIconSize, gridIconSize );

        // Scale bitmaps
        for( TInt i = 0 ; i < iIconArray.Count() ; i++ )
            {
            iIconArray[i]->Size().SetSize( iconSize.iWidth, iconSize.iHeight );
            }
        // Scale visuals
        for ( TInt item = 0; item < iCurrentLevel.iItemVisualData.Count() ; item++ )
            {
            iCurrentLevel.iItemVisualData[item].iImage->SetSize( iconSize );
            }
        }
	}

void CFSEmailUiLauncherGridVisualiser::AddItemToModelL( CFSEmailLauncherItem* aItem, TInt aPluginArrayIndex )
    {
    FUNC_LOG;
    if ( iConstructionCompleted && aItem->IsVisible() )
        {
        HBufC* launcherItemText = aItem->Caption( EFalse ).AllocLC();
        CAknIcon* launcherItemIcon = aItem->Icon();

        if ( launcherItemIcon )
            {
            TRect mainPaneRect;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

            TAknLayoutRect scrollBarRect;
            scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());
            TRect gridRect = mainPaneRect;
            gridRect.iBr.iX -= scrollBarRect.Rect().Width();

            TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;

            TAknLayoutRect itemRect;
            itemRect.LayoutRect(gridRect, AknLayoutScalable_Apps::cell_cmail_l_pane(var, 0, 0));

            TAknLayoutRect gridIconRect;
            gridIconRect.LayoutRect(itemRect.Rect(), AknLayoutScalable_Apps::cell_cmail_l_pane_g1(var));

            TSize iconSize = gridIconRect.Rect().Size();

            const CFbsBitmap* bitmap = launcherItemIcon->Bitmap();
            const CFbsBitmap* mask = launcherItemIcon->Mask();

			// First add plugin id and icon id pair to array, needed in provide bitmap
			TPluginIdIconIdPair idPair;
			idPair.iPluginArrayIndex = aPluginArrayIndex;
			idPair.iIconId = iPluginTextureId;
			iPluginIdIconIdPairs.Append( idPair );

			// Create texture, goes to provide bitmap
			CAlfTexture* texture = &CAlfStatic::Env().TextureManager().CreateTextureL( iPluginTextureId, this, EAlfTextureFlagDefault );
			// Update texture id
			iPluginTextureId++; // Id is updated dynamically
			// Set initial size
  			texture->Size().SetSize( iconSize.iHeight, iconSize.iWidth );
           iModel->AddL(
                EShortcut,
                aItem->Id(),
                *launcherItemText,
                *texture,
                0,
                aItem );
	        iIconArray.AppendL( texture );
            }
        else
            {
            CAlfTexture* texture = &iAppUi.FsTextureManager()->TextureByIndex( EGridInboxTexture );

            iModel->AddL(
                EShortcut,
                aItem->Id(),
                *launcherItemText,
                *texture,
                0,
                aItem );
            iIconArray.AppendL( texture );
            }

        CleanupStack::PopAndDestroy( launcherItemText );
        }
    }

void CFSEmailUiLauncherGridVisualiser::VisualLayoutUpdatedL()
    {
    FUNC_LOG;
    if ( iConstructionCompleted )
        {
	    iCurrentLevel.iParent = 0;

	    TRect mainPaneRect;
	    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
	    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;

	    TAknLayoutRect scrollBarRect;

	    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::scroll_pane_cp03());

	    TRect gridRect = mainPaneRect;
	    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

	    TAknLayoutRect cellRect;
	    cellRect.LayoutRect(gridRect, AknLayoutScalable_Apps::aid_size_cell_cmail_l(var, 0, 0));

	    TAknLayoutRect itemRect;
	    itemRect.LayoutRect(gridRect, AknLayoutScalable_Apps::cell_cmail_l_pane(var, 0, 0));

	    TAknLayoutRect gridIconRect;
	    gridIconRect.LayoutRect(itemRect.Rect(), AknLayoutScalable_Apps::cell_cmail_l_pane_g1(var));

	    TAknLayoutRect selectorRect;
	    selectorRect.LayoutRect(itemRect.Rect(), AknLayoutScalable_Apps::grid_highlight_pane_cp018(var));

	    CFSEmailUiLayoutHandler* layoutHandler = iAppUi.LayoutHandler();
	    iSelectorImageVisual->SetSize( selectorRect.Rect().Size() );

	    TSize displaySize = mainPaneRect.Size();

	    TInt columns = iVisibleColumns = AknLayoutScalable_Apps::cell_cmail_l_pane_ParamLimits(var).LastColumn() + 1; 
	    TInt rows = iVisibleRows = AknLayoutScalable_Apps::cell_cmail_l_pane_ParamLimits(var).LastRow() + 1; 

        iCurrentLevel.iGridLayout->SetSize( gridRect.Size() );
        iCurrentLevel.iGridLayout->SetColumnsL( columns );
        iCurrentLevel.iGridLayout->SetRowsL( rows );

        PopulateL( iCurrentLevel );
        UpdateScrollBarRangeL();

        TInt scrollbarWidth = scrollBarRect.Rect().Width();
        if( iRowCount > iVisibleRows )
            {
            iScrollbar->MakeVisible(ETrue);
            }
        else
            {
            iScrollbar->MakeVisible(EFalse);
            }

        TInt scrollbarTopLeftX = displaySize.iWidth - scrollbarWidth;
        TInt scrollbarTopLeftY = 0;
        TInt scrollbarBottomRightX = displaySize.iWidth;
        TInt scrollbarBottomRightY = displaySize.iHeight;
        TInt gridTopLeftX = 0;
        TInt gridTopLeftY = 0;
        TInt gridBottomRightX = displaySize.iWidth - scrollbarWidth;
        TInt gridBottomRightY = displaySize.iHeight;

        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // Mirrored layout
            scrollbarTopLeftX = 0;
            scrollbarTopLeftY = 0;
            scrollbarBottomRightX = scrollbarWidth;
            scrollbarBottomRightY = displaySize.iHeight;
            gridTopLeftX = scrollbarWidth;
            gridTopLeftY = 0;
            gridBottomRightX = displaySize.iWidth;
            gridBottomRightY = displaySize.iHeight;
            }

        // Set anchors for the scroll bar
        iWidgetLayout->SetAnchor(
            EAlfAnchorTopLeft,
            0,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( scrollbarTopLeftX, scrollbarTopLeftY ) );
        iWidgetLayout->SetAnchor(
            EAlfAnchorBottomRight,
            0,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( scrollbarBottomRightX, scrollbarBottomRightY ) );

        // Set anchors for the grid
        iWidgetLayout->SetAnchor(
            EAlfAnchorTopLeft,
            1,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( gridTopLeftX, gridTopLeftY ) );
        iWidgetLayout->SetAnchor(
            EAlfAnchorBottomRight,
            1,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint( gridBottomRightX, gridBottomRightY ) );

        iParentLayout->UpdateChildrenLayout();

        TSize gridSize = iCurrentLevel.iGridLayout->Size().IntTarget().AsSize();
        iColumnWidth = gridSize.iWidth / iVisibleColumns;
        iRowHeight = gridSize.iHeight / iVisibleRows;

        // Selector ring wrap limits can be calculated when row and column sizes are known.
        SetRingWrapLimits();
        MoveSelection( EDirectionNone );

        TRect scrollbarRect;
        scrollbarRect.SetRect(scrollbarTopLeftX, scrollbarTopLeftY, scrollbarBottomRightX, scrollbarBottomRightY);
        scrollbarRect.Move(mainPaneRect.iTl);
        iScrollbar->SetRect(scrollbarRect);
        iScrollbar->DrawDeferred();
        iRefreshNeeded = EFalse;
        }
    }

void CFSEmailUiLauncherGridVisualiser::SetRingWrapLimits()
    {
    FUNC_LOG;
    if ( iConstructionCompleted )
        {
        iRingMovementXFuncMappingDataProvider.SetStart(-iColumnWidth);
        iRingMovementXFuncMappingDataProvider.SetEnd((iVisibleColumns+1)*iColumnWidth);
        iRingMovementYFuncMappingDataProvider.SetStart(-iRowHeight);
        iRingMovementYFuncMappingDataProvider.SetEnd((iRowCount+1)*iRowHeight);

        iRingMovementXFunc->SetMappingTableValues( -(iVisibleColumns+2)*iColumnWidth, (iVisibleColumns*2+2)*iColumnWidth, &iRingMovementXFuncMappingDataProvider );
        iRingMovementYFunc->SetMappingTableValues( -(iRowCount+2)*iRowHeight, (iRowCount*2+2)*iRowHeight, &iRingMovementYFuncMappingDataProvider );
        }
    }

void CFSEmailUiLauncherGridVisualiser::DisplayProductInfoL()
	{
    FUNC_LOG;
	//Load the texts from resource
	HBufC* text_1 = StringLoader::LoadLC( R_DISCLAIMER_PART_1 );
	HBufC* text_2 = StringLoader::LoadLC( R_DISCLAIMER_PART_2 );
    HBufC* text_3 = StringLoader::LoadLC( R_DISCLAIMER_PART_3 );
    HBufC* text_4 = StringLoader::LoadLC( R_DISCLAIMER_PART_4 );
    HBufC* text_5 = StringLoader::LoadLC( R_DISCLAIMER_PART_5 );
    HBufC* text_6 = StringLoader::LoadLC( R_DISCLAIMER_PART_6 );
    HBufC* text_7 = StringLoader::LoadLC( R_DISCLAIMER_PART_7 );
    HBufC* text_8 = StringLoader::LoadLC( R_DISCLAIMER_PART_8 );
    HBufC* text_9 = StringLoader::LoadLC( R_DISCLAIMER_PART_9 );
    HBufC* text_10 = StringLoader::LoadLC( R_DISCLAIMER_PART_10 );
    HBufC* text_11 = StringLoader::LoadLC( R_DISCLAIMER_PART_11 );

    //Create a buffer for dialog content
    HBufC* text = HBufC::NewLC( text_1->Length() + text_2->Length() + text_3->Length() +
                                text_4->Length() + text_5->Length() + text_6->Length() +
                                text_7->Length() + text_8->Length() + text_9->Length() +
                                text_10->Length() + text_11->Length() );
    //Copy the disclaimer text parts to dialog content
    text->Des() += *text_1;
    text->Des() += *text_2;
    text->Des() += *text_3;
    text->Des() += *text_4;
    text->Des() += *text_5;
    text->Des() += *text_6;
    text->Des() += *text_7;
    text->Des() += *text_8;
    text->Des() += *text_9;
    text->Des() += *text_10;
    text->Des() += *text_11;

    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *text );
    CleanupStack::PopAndDestroy( text );
    CleanupStack::PopAndDestroy( text_11 );
    CleanupStack::PopAndDestroy( text_10 );
    CleanupStack::PopAndDestroy( text_9 );
    CleanupStack::PopAndDestroy( text_8 );
    CleanupStack::PopAndDestroy( text_7 );
    CleanupStack::PopAndDestroy( text_6 );
    CleanupStack::PopAndDestroy( text_5 );
    CleanupStack::PopAndDestroy( text_4 );
    CleanupStack::PopAndDestroy( text_3 );
    CleanupStack::PopAndDestroy( text_2 );
    CleanupStack::PopAndDestroy( text_1 );
    dlg->PrepareLC( R_DISCLAIMER_MESSAGE );

    //Create heading for the dialog, load and create the heading text
    CAknPopupHeadingPane* headingPane = dlg->Heading();
    HBufC* appName = StringLoader::LoadLC(R_FSEMAIL_APP_NAME );
    HBufC* version = StringLoader::LoadLC( R_DISCLAIMER_FSEMAIL_VERSION );
    HBufC* title = HBufC::NewLC(version->Length() + appName->Length() + 1);
    title->Des() += *appName;
    title->Des() += KSpace;
    title->Des() += *version;
    headingPane->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    CleanupStack::PopAndDestroy( version );
    CleanupStack::PopAndDestroy( appName );
    dlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OK_EMPTY );
    //show dialog
    TInt ret = dlg->RunLD();
	}

void CFSEmailUiLauncherGridVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType )
	{
    FUNC_LOG;

	CFsEmailUiViewBase::HandleDynamicVariantSwitchL( aType );

    if ( iConstructionCompleted )
        {
        iItemIdInButtonDownEvent.iItemId = KErrNotFound;

	    TRect mainPaneRect;
	    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
	    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;

	    TAknLayoutRect scrollBarRect;
	    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());
	    TRect gridRect = mainPaneRect;
	    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

	    TAknLayoutRect cellRect;
	    cellRect.LayoutRect(gridRect, AknLayoutScalable_Apps::aid_size_cell_cmail_l(var, 0, 0));

		TSize displaySize = cellRect.Rect().Size();
        iParentLayout->SetSize( displaySize );
        RescaleIconsL();
        VisualLayoutUpdatedL();

        UpdateFocusVisibility();
        
        // Stylus pop-up menu is closed during Layout switching
        if( iStylusPopUpMenuLaunched )
            {
            TRAP_IGNORE(iStylusPopUpMenu->HandleControlEventL(iCoeControl,
                    MCoeControlObserver::EEventRequestExit ));

            iStylusPopUpMenuLaunched = EFalse;
            }
       
        }
 	}

void CFSEmailUiLauncherGridVisualiser::HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType aType )
    {
    FUNC_LOG;
    iRefreshNeeded = ETrue;

    CFsEmailUiViewBase::HandleDynamicVariantSwitchOnBackgroundL( aType );
    }

void CFSEmailUiLauncherGridVisualiser::HandleContentChangeL()
    {
    FUNC_LOG;
    if ( iConstructionCompleted )
        {
        CreateModelL();
        if ( iAppUi.CurrentActiveView() == this )
            {
            VisualLayoutUpdatedL();
            }
        else
            {
            iRefreshNeeded = ETrue;
            }
        }
    }

void CFSEmailUiLauncherGridVisualiser::ConstructScrollbarL( CAlfLayout* aParent )
	{
    FUNC_LOG;
	CAlfLayout::AddNewL(*iControl, aParent);
	iScrollbar = new (ELeave) CAknDoubleSpanScrollBar(0);
	iScrollbar->ConstructL(ETrue, this, 0, CEikScrollBar::EVertical, 1000);
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    TRect scrollbarRect = aParent->DisplayRectTarget();
    scrollbarRect.Move(mainPaneRect.iTl);
    iScrollbar->SetRect(scrollbarRect);

    iScrollbar->MakeVisible(EFalse);
    iScrollbar->SetModelL(&iScrollbarModel);
	}

void CFSEmailUiLauncherGridVisualiser::UpdateScrollBarRangeL()
	{
    FUNC_LOG;

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    TAknLayoutRect scrollBarRect;
    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());

    iScrollbarModel.SetScrollSpan(iRowCount * (scrollBarRect.Rect().Height() / iVisibleRows));
    iScrollbarModel.SetWindowSize(scrollBarRect.Rect().Height());  
	iScrollbarModel.SetFocusPosition(iFirstVisibleRow);
	iScrollbar->SetModelL(&iScrollbarModel);
	iScrollbar->DrawNow();
	}

void CFSEmailUiLauncherGridVisualiser::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
    {
    FUNC_LOG;
    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }
    
    if (aScrollBar == iScrollbar)
         {
         switch( aEventType )
             {
             case EEikScrollHome :
                 //Jump to beginning
                 break;
             case EEikScrollEnd :
                 //Jump to end
                 break;
             default:
                 iScrolled = ETrue;
                 iAppUi.SetFocusVisibility( EFalse );
                 if ( iSelector )
                     {
                     TAlfTimedValue selectorOpacity;
                     selectorOpacity.SetValueNow( 0 );
                     iSelector->SetOpacity( selectorOpacity );
                     }                 
                 iFirstVisibleRow = iCurrentLevel.iParentPos.iY/iRowHeight;
                 TReal offset = aScrollBar->ThumbPosition() + 1;
                 TAlfTimedPoint alfScrollOffset;
                 alfScrollOffset.iY.SetTarget( offset , 0);
				 iCurrentLevel.iGridLayout->SetScrollOffset(alfScrollOffset);
                 iCurrentLevel.iParentPos.iY = offset;
                 break;
             }
         }
    }

void CFSEmailUiLauncherGridVisualiser::UpdateLauncherItemListL()
    {
    FUNC_LOG;

    // Read info about all implementations into infoArray
    RImplInfoPtrArray infoArray;
    CleanupResetAndDestroyClosePushL( infoArray );
    CFSEmailLauncherItem::ListAllImplementationsL( infoArray );

    RArray<TBool> itemsFoundArray;
    CleanupClosePushL( itemsFoundArray );

    itemsFoundArray.ReserveL( iLauncherItemUids.Count() );
    for ( TInt i = 0; i < iLauncherItemUids.Count(); i++ )
        {
        itemsFoundArray.Append( EFalse );
        }

    // Iterate through all launcher item implementations in info array
    TInt const infoCount = infoArray.Count();
    for ( TInt infoIndex = 0; infoIndex < infoCount; infoIndex++ )
        {
        TUid implementationUid = infoArray[infoIndex]->ImplementationUid();

        // Check whether item can be found from current laucher items
        TBool itemFound = EFalse;
        for ( TInt itemIndex = 0; itemIndex < iLauncherItemUids.Count(); itemIndex++ )
            {
            if ( iLauncherItemUids[itemIndex] == implementationUid )
                {
                itemsFoundArray[itemIndex] = ETrue;
                itemFound = ETrue;
                break;
                }
            }

        if ( !itemFound )
            {
            // Add new item to the end of the laucher item array
            CFSEmailLauncherItem* launcherItem = NULL;
            TRAPD( error, launcherItem = CFSEmailLauncherItem::NewL( implementationUid, this ) );
            if ( error == KErrNone )
                {
                iLauncherItems.Append( launcherItem );
                iLauncherItemUids.Append( implementationUid );
                }
            }
        }

    // Remove all launcher items not found from the info array
    for ( TInt i = itemsFoundArray.Count() - 1 ; i >= 0; --i )
        {
        if ( !itemsFoundArray[i] )
            {
            delete iLauncherItems[i];
            iLauncherItems.Remove( i );
            iLauncherItemUids.Remove( i );
            }
        }

    // Clean up
    CleanupStack::Pop( &itemsFoundArray );
    itemsFoundArray.Close();
    CleanupStack::PopAndDestroy( &infoArray );
    }

/**
 * Sets application title when leaving this view
 */
void CFSEmailUiLauncherGridVisualiser::SetDefaultStatusPaneTextL()
	{
    FUNC_LOG;
	iAppUi.ResetTitlePaneTextL();
	}

void CFSEmailUiLauncherGridVisualiser::SetWizardWaitnoteShown( TBool aWaitnoteShown )
    {
    FUNC_LOG;
    iWizardWaitnoteShown = aWaitnoteShown;
    }

/**
 * Check status of all configured mailboxes
 */
TFSLauncherGridMailboxStatus CFSEmailUiLauncherGridVisualiser::CheckMailboxStatusL()
    {
    FUNC_LOG;
    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    TFSMailMsgId id;
    TInt err = iAppUi.GetMailClient()->ListMailBoxes(
        id,
        mailBoxes );

    TFSLauncherGridMailboxStatus returnStatus;
    returnStatus.iMailboxCount = mailBoxes.Count();
    // Reset counters
    returnStatus.iMailboxesOnline = returnStatus.iMailboxesOffline = returnStatus.iMailboxesSyncing = 0;

    for ( TInt i = 0; i < mailBoxes.Count(); i++ )
        {
        // First check online/offline status
        TFSMailBoxStatus onOffStatus = mailBoxes[i]->GetMailBoxStatus();
        if( onOffStatus == EFSMailBoxOffline )
            {
            returnStatus.iMailboxesOffline++;
            }
        else if( onOffStatus == EFSMailBoxOnline )
            {
            returnStatus.iMailboxesOnline++;
            }

        // Then check sync state
        TSSMailSyncState latestSyncstate = mailBoxes[i]->CurrentSyncState();
        if( latestSyncstate == InboxSyncing ||
            latestSyncstate == StartingSync ||
            latestSyncstate == EmailSyncing )
            {
            returnStatus.iMailboxesSyncing++;
            }
        }

    CleanupStack::PopAndDestroy( &mailBoxes );
    return returnStatus;
    }

// Bitmap provider for grid ecom plugins icons
void CFSEmailUiLauncherGridVisualiser::ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap)
	{
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    TInt var = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;

    TAknLayoutRect scrollBarRect;
    scrollBarRect.LayoutRect(mainPaneRect, AknLayoutScalable_Avkon::aid_size_touch_scroll_bar());
    TRect gridRect = mainPaneRect;
    gridRect.iBr.iX -= scrollBarRect.Rect().Width();

    TAknLayoutRect itemRect;
    itemRect.LayoutRect(gridRect, AknLayoutScalable_Apps::cell_cmail_l_pane(var, 0, 0));

    TAknLayoutRect gridIconRect;
    gridIconRect.LayoutRect(itemRect.Rect(), AknLayoutScalable_Apps::cell_cmail_l_pane_g1(var));

	CAknIcon* launcherItemIcon(0);
	for ( TInt i=0; i<iPluginIdIconIdPairs.Count(); i++ )
		{
		if ( aId == iPluginIdIconIdPairs[i].iIconId )
			{
			launcherItemIcon = iLauncherItems[iPluginIdIconIdPairs[i].iPluginArrayIndex]->Icon();
			}
		}

	if ( launcherItemIcon )
		{
		// Set bitmap size
		TSize iconSize = gridIconRect.Rect().Size();
		if( launcherItemIcon->Bitmap() )
            {
            AknIconUtils::DisableCompression( launcherItemIcon->Bitmap() );
            AknIconUtils::SetSize( launcherItemIcon->Bitmap(), iconSize, EAspectRatioPreserved );
            }
        if( launcherItemIcon->Mask() )
            {
            AknIconUtils::DisableCompression( launcherItemIcon->Mask());
            AknIconUtils::SetSize( launcherItemIcon->Mask(), iconSize, EAspectRatioPreserved );
            }
        // Create duplicate sof original for alf
        CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
     	bitmap->Duplicate( launcherItemIcon->Bitmap()->Handle() );
	    CFbsBitmap* mask = new (ELeave) CFbsBitmap;
     	mask->Duplicate( launcherItemIcon->Mask()->Handle() );

        aBitmap = bitmap;
  		aMaskBitmap = mask;
		}
	else
		{
		User::Leave(KErrNotFound); // Leave as icon is not found, should not go here
		}
	}

void CFSEmailUiLauncherGridVisualiser::SetRefreshNeeded()
	{
    FUNC_LOG;
	iRefreshNeeded = ETrue;
	iDoubleClickLock = EFalse;
	}

TReal32 CFSEmailUiLauncherGridVisualiser::TRingMovementFuncMappingDataProvider::MapValue(TReal32 aValue, TInt /*aMode*/) const
    {
    FUNC_LOG;
    AlfUtil::WrapValue(aValue, iStart, iEnd);
    return aValue;
    }

void CFSEmailUiLauncherGridVisualiser::DetachSelectorMappingFunctions()
    {
    FUNC_LOG;
    if ( iSelector )
        {
        TAlfTimedPoint selectorPos;
        selectorPos.iX.SetMappingFunctionIdentifier( 0 );
        selectorPos.iY.SetMappingFunctionIdentifier( 0 );
        iSelector->SetPos( selectorPos );
        }
    }


// -----------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::UpdateFocusVisibility()
// Updates the selector and the text color of a focused item.
// -----------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::UpdateFocusVisibility()
    {
    const TBool showFocus =	( IsFocusShown() ||
							  iItemIdInButtonDownEvent.iItemId >= 0 );

    // Update the selector.
    if ( iSelector )
        {
        TAlfTimedValue selectorOpacity;

        if ( showFocus )
            {
            selectorOpacity.SetValueNow( 1 );
            }
        else
            {
            selectorOpacity.SetValueNow( 0 );
            }

        iSelector->SetOpacity( selectorOpacity );
        }

    // Update the text color of the previously and newly selected items. 
	TRgb normalColor( KRgbGray );
	TRgb focusedColor( KRgbGray );

	if ( ( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), normalColor,
								      KAknsIIDQsnTextColors,
								      EAknsCIQsnTextColorsCG9 ) != KErrNone ) ||
		 ( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), focusedColor,
									  KAknsIIDQsnTextColors,
									  EAknsCIQsnTextColorsCG11 ) != KErrNone ) )
		{
		// No colors available.
		return;
		}

	CAlfTextVisual* textVisual = NULL;
	const TInt itemCount( iCurrentLevel.iItemVisualData.Count() );

	// Set the colors of the captions not focused to normal and if an item is
	// focused, set the color of its caption using the highlighted theme color.
	for ( TInt i = 0; i < itemCount; ++i )
		{
		textVisual = iCurrentLevel.iItemVisualData[i].iText;

		if ( textVisual )
			{
		    if ( showFocus && iCurrentLevel.iSelected == i )
		        {
                textVisual->SetColor( focusedColor );
		        }
		    else
		        {
                textVisual->SetColor( normalColor );
		        }
			}
		}
    }


void CFSEmailUiLauncherGridVisualiser::FlipStateChangedL( TBool aKeyboardFlipOpen )
    {
    CFsEmailUiViewBase::FlipStateChangedL( aKeyboardFlipOpen );
    UpdateFocusVisibility();
    }

void CFSEmailUiLauncherGridVisualiser::MailboxDeletionComplete()
    {
    TRAP_IGNORE( HandleContentChangeL() );
    }

void CFSEmailUiLauncherGridVisualiser::FocusVisibilityChange( TBool aVisible )
	{
    CFsEmailUiViewBase::FocusVisibilityChange( aVisible );
    UpdateFocusVisibility();
    ResizeItemIcon( !aVisible );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::ViewPositionChanged
// From MAknPhysicsObserver
// -----------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::ViewPositionChanged(
        const TPoint& aNewPosition,
        TBool /*aDrawNow*/,
        TUint /*aFlags*/ )
    {
    FUNC_LOG;

    // controls must be created first
    if ( !iConstructionCompleted )
        {
        return;
        }

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    iCurrentLevel.iParentPos.iY = aNewPosition.iY - mainPaneRect.Height()/2;
    TAlfTimedPoint alfScrollOffset;
    alfScrollOffset.iY.SetTarget( iCurrentLevel.iParentPos.iY , 0 );
    iCurrentLevel.iGridLayout->SetScrollOffset(alfScrollOffset);
    iScrollbarModel.SetFocusPosition( iCurrentLevel.iParentPos.iY );
    TRAP_IGNORE( iScrollbar->SetModelL(&iScrollbarModel) );
    iScrollbar->DrawNow();
    iFirstVisibleRow = iCurrentLevel.iParentPos.iY/iRowHeight;
    if ( iFirstVisibleRow < 0 )
        {
        iFirstVisibleRow = 0;
        }
    else if ( iFirstVisibleRow > iRowCount )
        {
        iFirstVisibleRow = iRowCount;
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::PhysicEmulationEnded
// From MAknPhysicsObserver
// -----------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::PhysicEmulationEnded()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ViewPosition
// From MAknPhysicsObserver
// -----------------------------------------------------------------------------
//
TPoint CFSEmailUiLauncherGridVisualiser::ViewPosition() const
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
    TInt current = iCurrentLevel.iParentPos.iY;
    TInt viewCentre = mainPaneRect.Size().iHeight / 2;
    TPoint point( 0, current + viewCentre );
    return point;
    }

// ----------------------------------------------------------------------------
// LaunchStylusPopupMenuL()
// Launches and displays the pop-up menu invoked by a long tap event.
// ----------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::LaunchStylusPopupMenu(
	const TInt aItemId )
	{
	// Get the ID of the mailbox in case the user wants to delete it.
	iMailboxToDelete = iCurrentLevel.iItems[aItemId].iMailBoxId;

	// Get the item position and use it to determine the position of the menu.
	const TRect rect(
		iCurrentLevel.iItemVisualData[aItemId].iBase->DisplayRect() );
	TPoint position( rect.iTl.iX + rect.Width() / 2,
					 rect.iTl.iY + rect.Height() / 2 );
	iStylusPopUpMenu->SetPosition( position );

	// Display the menu.
	iStylusPopUpMenu->ShowMenu();
	iStylusPopUpMenuLaunched = ETrue;
	}

void CFSEmailUiLauncherGridVisualiser::HandleAppForegroundEventL( TBool aForeground )
    {
    FUNC_LOG;
    
    // If the view is not visible try to visualise it after a while
    if ( aForeground && !iWasActiveControlGroup )
        {
        iStartupCallbackTimer->Cancel(); // just in case
        iStartupCallbackTimer->SetPriority( CActive::EPriorityIdle );
        iStartupCallbackTimer->Start( 200 );
        }

    CFsEmailUiViewBase::HandleAppForegroundEventL( aForeground );	
    }

// ----------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::TimerEventL()
// Fire timer callback
// ----------------------------------------------------------------------------
void CFSEmailUiLauncherGridVisualiser::TimerEventL( CFSEmailUiGenericTimer* /* aTriggeredTimer */ )
    {
    FUNC_LOG;

    // if view is still active then 
    if ( iAppUi.CurrentActiveView() && iAppUi.CurrentActiveView()->Id() == Id() )	  
        {
        iWasActiveControlGroup = ETrue;
        CFsEmailUiViewBase::HandleAppForegroundEventL( ETrue );
        }

    iStartupCallbackTimer->Cancel();
    }

// ---------------------------------------------------------------------------
// CFSEmailUiLauncherGridVisualiser::UpdatePhysicsL()
// ---------------------------------------------------------------------------
//
void CFSEmailUiLauncherGridVisualiser::UpdatePhysicsL()
    {
    FUNC_LOG;
    if ( iPhysics )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        const TSize viewSize( mainPaneRect.Size() );        
        const TSize worldSize( 0, iRowCount * iRowHeight );
        iPhysics->InitPhysicsL( worldSize, viewSize, EFalse );
        }
    }

