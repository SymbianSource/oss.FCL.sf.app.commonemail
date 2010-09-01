/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email implementation file
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <akncontext.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <barsread.h>
#include <FreestyleEmailUi.rsg>
// <cmail> 
#include <csxhelp/cmail.hlp.hrh>
// </cmail>
#include <featmgr.h>
//</cmail>

// LOCAL INCLUDES
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiGlobalSettings.hrh"
#include "FreestyleEmailUiGlobalSettingsListView.h"
#include "FreestyleEmailUiGlobalSettingsList.h"
#include "FreestyleEmailUiGlobalSettingsData.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiConstants.h" 
#include "FreestyleEmailUiShortcutBinding.h"
#include "FSEmail.pan"

/**
 * First phase of Symbian two-phase construction. Should not contain any
 * code that could leave.
 */
CFsEmailUiGlobalSettingsListView::CFsEmailUiGlobalSettingsListView( CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup )
	: CFsEmailUiViewBase( aControlGroup, *aAppUi )
	{
    FUNC_LOG;
	}

/** 
 * Destructor
 */
CFsEmailUiGlobalSettingsListView::~CFsEmailUiGlobalSettingsListView()
	{
    FUNC_LOG;
	}

/**
 * Symbian two-phase constructor.
 * This creates an instance then calls the second-phase constructor
 * without leaving the instance on the cleanup stack.
 * @return new instance of CFsEmailUiGlobalSettingsListView
 */
CFsEmailUiGlobalSettingsListView* CFsEmailUiGlobalSettingsListView::NewL(
		CFreestyleEmailUiAppUi* aAppUi,
		CAlfControlGroup& aControlGroup )
	{
    FUNC_LOG;

	CFsEmailUiGlobalSettingsListView* self = CFsEmailUiGlobalSettingsListView::NewLC( aAppUi, aControlGroup );
	CleanupStack::Pop( self );

	return self;
	}

/**
 * Symbian two-phase constructor.
 * This creates an instance, pushes it on the cleanup stack,
 * then calls the second-phase constructor.
 * @return new instance of CFsEmailUiGlobalSettingsListView
 */
CFsEmailUiGlobalSettingsListView* CFsEmailUiGlobalSettingsListView::NewLC(
		CFreestyleEmailUiAppUi* aAppUi, 
		CAlfControlGroup& aControlGroup )
	{
    FUNC_LOG;

	CFsEmailUiGlobalSettingsListView* self = new ( ELeave ) CFsEmailUiGlobalSettingsListView( aAppUi, aControlGroup );
	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
	}


/**
 * Second-phase constructor for view.  
 * Initialize contents from resource.
 */ 
void CFsEmailUiGlobalSettingsListView::ConstructL()
	{
    FUNC_LOG;

	BaseConstructL( R_GLOBAL_SETTINGS_LIST_VIEW );

	}
	
/**
 * @return The UID for this view
 */
TUid CFsEmailUiGlobalSettingsListView::Id() const
	{
    FUNC_LOG;
	return GlobalSettingsViewId;
	}

void CFsEmailUiGlobalSettingsListView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
	{
    FUNC_LOG;
    if ( aResourceId == R_GLOBAL_SETTINGS_MENU_PANE )
        {
		
	    if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);      
		   }
  // Dim "Open" and "Change" 
	    aMenuPane->SetItemDimmed( EFsEmailUiCmdChange, ETrue );
	    aMenuPane->SetItemDimmed( EFsEmailUiCmdOpen, ETrue );
                 
        // Add shortcut hints
        iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane, 
                                     CFSEmailUiShortcutBinding::EContextSettings );         
        }
	}

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
void CFsEmailUiGlobalSettingsListView::HandleCommandL( TInt aCommand )
	{
    FUNC_LOG;

	switch ( aCommand )
		{
		case EAknSoftkeyBack:
			iAppUi.ReturnToPreviousViewL();
			break;
		case EFsEmailUiCmdChange:
		case EAknSoftkeyChange:
		case EFsEmailUiCmdOpen:
		case EAknSoftkeyOpen:
			HandleChangeSelectedSettingItemL( aCommand );
			break;
		case EFsEmailUiCmdHelp:
			TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
			break;
		case EFsEmailUiCmdExit:
		case EEikCmdExit:
		case EAknCmdExit:
			AppUi()->HandleCommandL( EEikCmdExit );
			break;
		case EFsEmailUiCmdGoToTop:
		    iGlobalSettingsSettingItemList->GoToTop();
		    break;
		case EFsEmailUiCmdGoToBottom:
		    iGlobalSettingsSettingItemList->GoToBottom();
		    break;
        case EFsEmailUiCmdPageUp:
            iGlobalSettingsSettingItemList->PageUp();
            break;
        case EFsEmailUiCmdPageDown:
            iGlobalSettingsSettingItemList->PageDown();
            break;
		default:
		    __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
			break;
		}	
	}

/**
 * Set MSK label depending on the currently focused item
 */
void CFsEmailUiGlobalSettingsListView::SetMskL()
    {
    FUNC_LOG;
    if ( iGlobalSettingsSettingItemList )
        {
        TInt focusedId = iGlobalSettingsSettingItemList->FocusedItemId();
        if ( focusedId == EGlobalSettingsActiveIdle )
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_OPEN );
            }
        else
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_CHANGE );
            }
        }
    }

/**
 *	Handles actions during activation of the view, 
 *	such as initializing the content.
 */
// <cmail> Toolbar
/*void CFsEmailUiGlobalSettingsListView::DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage )*/
void CFsEmailUiGlobalSettingsListView::ChildDoActivateL(
        const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/ )
// </cmail> Toolbar
	{
    FUNC_LOG;
	
	SetupStatusPaneL();
	
	if ( !iGlobalSettingsSettingItemList )
		{
		iSettings = CFsEmailUiGlobalSettingsData::NewL();
		iGlobalSettingsSettingItemList = new ( ELeave ) CFsEmailUiGlobalSettingsList( *iSettings, this );
		iGlobalSettingsSettingItemList->SetMopParent( this );
		iGlobalSettingsSettingItemList->ConstructFromResourceL( R_GLOBAL_SETTINGS_ITEM_LIST );
		iGlobalSettingsSettingItemList->ActivateL();
		iGlobalSettingsSettingItemList->LoadSettingValuesL();
		iGlobalSettingsSettingItemList->LoadSettingsL();
		AppUi()->AddToStackL( *this, iGlobalSettingsSettingItemList );
		} 
	iAppUi.HideTitlePaneConnectionStatus();

	}

/**
 * ChildDoDeactivate
 * From CFsEmailUiViewBase (override)
 */
void CFsEmailUiGlobalSettingsListView::ChildDoDeactivate()
	{
    FUNC_LOG;

	if ( iGlobalSettingsSettingItemList )
		{
		AppUi()->RemoveFromStack( iGlobalSettingsSettingItemList );
		delete iGlobalSettingsSettingItemList;
		iGlobalSettingsSettingItemList = NULL;
		delete iSettings;
		iSettings = NULL;
		}

	}

/**
 * SetupStatusPaneL
 * From CAknView
 */
void CFsEmailUiGlobalSettingsListView::SetupStatusPaneL()
	{
    FUNC_LOG;

	// reset the context pane
	TUid contextPaneUid = TUid::Uid( EEikStatusPaneUidContext );
	CEikStatusPaneBase::TPaneCapabilities subPaneContext = 
		StatusPane()->PaneCapabilities( contextPaneUid );
	if ( subPaneContext.IsPresent() && subPaneContext.IsAppOwned() )
		{
		CAknContextPane* context = static_cast< CAknContextPane* > ( 
			StatusPane()->ControlL( contextPaneUid ) );
		context->SetPictureToDefaultL();
		}
	
	// setup the title pane
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
		StatusPane()->PaneCapabilities( titlePaneUid );
	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* title = static_cast< CAknTitlePane* >( 
			StatusPane()->ControlL( titlePaneUid ) );
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GLOBAL_SETTINGS_TITLE_RESOURCE );
		title->SetFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
				
	}

/**
 * CleanupStatusPaneL
 * Change status pane title back to default
 */
void CFsEmailUiGlobalSettingsListView::CleanupStatusPaneL()
	{
    FUNC_LOG;
    iAppUi.ResetTitlePaneTextL();
	}


/** 
 * Handle status pane size change for this view (override)
 * 
 */
void CFsEmailUiGlobalSettingsListView::HandleStatusPaneSizeChange()
	{
    FUNC_LOG;
	if ( IsForeground() )
		{
		CAknView::HandleStatusPaneSizeChange();
		
		// this may fail, but we're not able to propagate exceptions here
		TRAPD( err, SetupStatusPaneL() );
		if (err)
		    {
		    }
		}
	}


/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CFsEmailUiGlobalSettingsListView::HandleChangeSelectedSettingItemL( TInt aCommand )
	{
    FUNC_LOG;

  	TBool calledFromMenu = 
        ( aCommand != EAknSoftkeyChange && aCommand != EAknSoftkeyOpen );
	iGlobalSettingsSettingItemList->ChangeSelectedItemL( calledFromMenu );

	return ETrue;
	}
				
