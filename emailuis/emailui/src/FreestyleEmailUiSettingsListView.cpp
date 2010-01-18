/*
* Copyright (c) 2007 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email source file
*
*/

// SYSTEM INCLUDES
#include "emailtrace.h"
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <barsread.h>
#include <StringLoader.h>
#include <aknlists.h>
#include <eikenv.h>
#include <AknIconArray.h>
#include <eikclbd.h>
#include <akncontext.h>
#include <akntitle.h>
#include <FreestyleEmailUi.rsg>
#include <featmgr.h>
#include <aknstyluspopupmenu.h>

#include "CFSMailCommon.h"
#include "CFSMailBox.h"
#include "ESMailSettingsPlugin.h"
#include "ESMailSettingsPluginUids.hrh"
#include <gsfwviewuids.h> // Uids for general settings activation
#include <csxhelp/cmail.hlp.hrh>

// LOCAL INCLUDES
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiSettingsListView.h"
#include "FreestyleEmailUiSettingsList.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiSettings.hrh"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiLauncherGridVisualiser.h"


// ---------------------------------------------------------------------------
// First phase of Symbian two-phase construction. Should not contain any
// code that could leave.
// ---------------------------------------------------------------------------
//
CFsEmailSettingsListView::CFsEmailSettingsListView( 
	CAlfControlGroup& aControlGroup,
	CFreestyleEmailUiAppUi& aAppUi,
	CFSMailClient& aMailClient  )
    : CFsEmailUiViewBase( aControlGroup, aAppUi ),
      iMailClient( aMailClient ),
      iMailboxSettings( EFalse )
	{
    FUNC_LOG;
	iFsEmailSettingsList = NULL;
	}

// ---------------------------------------------------------------------------
// ~CFsEmailSettingsListView
// ---------------------------------------------------------------------------
//
CFsEmailSettingsListView::~CFsEmailSettingsListView()
	{
    FUNC_LOG;

   	if ( iAsyncCallback )
		{
		iAsyncCallback->Cancel();
		delete iAsyncCallback;
		}

   	if ( iStylusPopUpMenu )
   		{
   		delete iStylusPopUpMenu;
   		iStylusPopUpMenu = NULL;
   		}
	}


// ---------------------------------------------------------------------------
// Symbian two-phase constructor.
// This creates an instance then calls the second-phase constructor
// without leaving the instance on the cleanup stack.
// @return new instance of CFsEmailSettingsListView
// ---------------------------------------------------------------------------
//
CFsEmailSettingsListView* CFsEmailSettingsListView::NewL( 
		CFSMailClient& aMailClient,
		CFreestyleEmailUiAppUi* aAppUi, 
		CAlfControlGroup& aControlGroup )
	{
    FUNC_LOG;

	CFsEmailSettingsListView* self =
		CFsEmailSettingsListView::NewLC( aMailClient, aAppUi, aControlGroup );
	CleanupStack::Pop( self );

	return self;
	}

// ---------------------------------------------------------------------------
// Symbian two-phase constructor.
// This creates an instance, pushes it on the cleanup stack,
// then calls the second-phase constructor.
// @return new instance of CFsEmailSettingsListView
// ---------------------------------------------------------------------------
//
CFsEmailSettingsListView* CFsEmailSettingsListView::NewLC( 
		CFSMailClient& aMailClient,
		CFreestyleEmailUiAppUi* aAppUi, 
		CAlfControlGroup& aControlGroup )
	{
    FUNC_LOG;

	CFsEmailSettingsListView* self = 
	    new ( ELeave ) CFsEmailSettingsListView(
	    	aControlGroup, *aAppUi, aMailClient );
	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
	}


// ---------------------------------------------------------------------------
// Second-phase constructor for view.  
// Initialize contents from resource.
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsListView::ConstructL()
	{
    FUNC_LOG;

	BaseConstructL( R_FS_EMAIL_SETTINGS_LIST_VIEW );

	iAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
	iAsyncCallback->Set( TCallBack( DisplayCreateQuery, this ) );
	}

// -----------------------------------------------------------------------------
// CFsEmailSettingsListView::DisplayCreateQuery
// -----------------------------------------------------------------------------
//
void CFsEmailSettingsListView::StartMailboxAsyncQueryL()
	{
    FUNC_LOG;

	if ( iAsyncCallback )
		{
		iAsyncCallback->CallBack();
		}
	}


// -----------------------------------------------------------------------------
// CFsEmailSettingsListView::DisplayStylusPopUpMenu()
// Displays the pop-up menu.
// -----------------------------------------------------------------------------
//
void CFsEmailSettingsListView::DisplayStylusPopUpMenu( const TPoint& aPosition )
	{
    if ( iStylusPopUpMenu )
    	{
    	iStylusPopUpMenu->SetPosition( aPosition );
    	iStylusPopUpMenu->ShowMenu();
    	}
	}


// -----------------------------------------------------------------------------
// CFsEmailSettingsListView::DisplayCreateQuery
// -----------------------------------------------------------------------------
//
TInt CFsEmailSettingsListView::DisplayCreateQuery( TAny* aViewPtr )
    {
    FUNC_LOG;
    CFsEmailSettingsListView* self =
		static_cast<CFsEmailSettingsListView*>( aViewPtr ); 
	TRAPD( err, self->DisplayCreateMailboxNoteIfNeededL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CFsEmailSettingsListView::DisplayCreateQueryL
// -----------------------------------------------------------------------------
//
void CFsEmailSettingsListView::DisplayCreateMailboxNoteIfNeededL()
    {
    FUNC_LOG;
    User::LeaveIfNull( iFsEmailSettingsList );
    iFsEmailSettingsList->DisplayCreateMailboxNoteIfNeededL();
    }

// -----------------------------------------------------------------------------
// CFsEmailSettingsListView::Id()
// From CAknView.
// -----------------------------------------------------------------------------
//
TUid CFsEmailSettingsListView::Id() const
	{
    FUNC_LOG;
	return SettingsViewId;
	}

// -----------------------------------------------------------------------------
// CFsEmailSettingsListView::HandleCommandL()
// From CAknView.
// -----------------------------------------------------------------------------
//
void CFsEmailSettingsListView::HandleCommandL( TInt aCommand )
	{
    FUNC_LOG;
	
	switch ( aCommand )
		{
		case EAknSoftkeyBack:
			{
			if ( iFsEmailSettingsList->Get2ndLevelListIndicator() 
			     && !iMailboxSettings )
			    {
			    // return from sub view
			    iFsEmailSettingsList->CreateAccountListL();
			    // when returning to mailboxlist set sublistindex to zero to avoid 
			    // wrong indexin when creating the sublistviews
			    iFsEmailSettingsList->SetSelectedSubListIndex(0);
	            SetupStatusPaneL();
			    }
			else
			    {
			    // navigate away from settings
	            NavigateBackL();
			    }

			break;
			}
		case EAknSoftkeyOpen:	
		case EFsEmailUiCmdOpen:
			{
			iFsEmailSettingsList->HandleUserSelectionsL();
			SetupStatusPaneL();
			break;
			}
		case EFsEmailUiCmdSettingsAddAccount:
			{
			iFsEmailSettingsList->AddAccountL();
			break;
			}
		case EFsEmailUiCmdSettingsRemoveAccount:
		case EFsEmailUiCmdDeleteSelectedMailbox: // From the pop-up menu.
			{
			iFsEmailSettingsList->RemoveAccountL();
			iFsEmailSettingsList->ClearFocus();
			break;
			}
		case EFsEmailUiCmdHelp:
			{
			TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
            break;
			}
		case EFsEmailUiCmdExit:
			{
			AppUi()->HandleCommandL( EEikCmdExit );
			break;
			}
		case EFsEmailUiCmdGoToTop:
			{
		    iFsEmailSettingsList->GoToTop();
		    break;
			}
		case EFsEmailUiCmdGoToBottom:
			{
		    iFsEmailSettingsList->GoToBottom();
		    break;
			}
        case EFsEmailUiCmdPageUp:
        	{
            iFsEmailSettingsList->PageUp();
            break;
        	}
        case EFsEmailUiCmdPageDown:
        	{
            iFsEmailSettingsList->PageDown();
            break;
        	}
        case KErrCancel:
        	{
        	// The pop-up menu was cancelled.
        	iFsEmailSettingsList->ClearFocus();
        	break;
        	}
		default:
			{
			break;
			}
		} // switch ( aCommand )
	}


// -----------------------------------------------------------------------------
// CFsEmailSettingsListView::ChildDoActivateL()
// From CFsEmailUiViewBase.
// -----------------------------------------------------------------------------
//
void CFsEmailSettingsListView::ChildDoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage )
	{
    FUNC_LOG;
    
    if ( aCustomMessageId.iUid != KMailSettingsReturnToPrevious &&
         aCustomMessageId.iUid != KMailSettingsReturnFromPluginSettings )
        {
        ViewEntered( aPrevViewId );
        }
    
	// create view control
	if ( iFsEmailSettingsList == NULL )
		{
		TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> subViewData;
		subViewData.Copy( aCustomMessage );
		CESMailSettingsPlugin::TSubViewActivationData subView = subViewData();
					
		iFsEmailSettingsList = CFsEmailSettingsList::NewL( ClientRect(), NULL, iAppUi, iMailClient, *this );
		iFsEmailSettingsList->SetMopParent( this );
		AppUi()->AddToStackL( *this, iFsEmailSettingsList );
		} // no else

	if ( iFsEmailSettingsList )
		{
		
		// Opened from launcher grid
		if ( aCustomMessageId == TUid::Uid( KMailSettingsOpenMainList ) )
			{
			iFsEmailSettingsList->CreateAccountListL();
			} // no else

		// Opened from S60 general settings
		if ( aCustomMessageId == TUid::Uid( KOpenMailSettingsFromGS ) )
			{
			iFsEmailSettingsList->CreateAccountListL();
			} // no else

		// Returning from global settings
		if ( aCustomMessageId == TUid::Uid( KMailSettingsReturnToPrevious ) )
			{
			iFsEmailSettingsList->CreateAccountListL();
			} // no else
				
		// open specific mailbox settings directly from mail list
		if ( aCustomMessageId == TUid::Uid( KMailSettingsOpenMailboxSettings ) )
			{
			// save previous view
			iMailboxSettings = ETrue;

			TMailListActivationData mailboxData;
			TPckgBuf<TMailListActivationData> mailboxPkgData( mailboxData );
			mailboxPkgData.Copy( aCustomMessage );
			mailboxData = mailboxPkgData();

			CFSMailBox* mailbox = iFsEmailSettingsList->GetMailBoxByIdL( mailboxData.iMailBoxId.Id() );
			if ( mailbox )
				{
				CleanupStack::PushL( mailbox );
				TUid settingsPluginId = mailbox->GetSettingsUid();
				iFsEmailSettingsList->SetPluginSettingsUid( settingsPluginId );
				iFsEmailSettingsList->SetSelectedAccountInfo( mailbox->GetId() );
				CleanupStack::PopAndDestroy( mailbox );
				iFsEmailSettingsList->ActivateMailSettingsPluginSubViewL( mailboxData.iRequestRefresh );
				}
			else // Mailbox fetching failed, create account list instead
				{
				iFsEmailSettingsList->CreateAccountListL();
				}
			} // no else

		// opening specific plugin caption list outside email ui
		if ( aCustomMessageId == TUid::Uid( KMailSettingsOpenPluginSettings ) )
			{
			CESMailSettingsPlugin::TSubViewActivationData subView;
			TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> SubViewData( subView );
			SubViewData.Copy( aCustomMessage );
			subView = SubViewData();
	
			CFSMailBox* mailbox = iFsEmailSettingsList->GetMailBoxByIdL( subView.iAccount.Id() );
			if ( mailbox )
				{
				CleanupStack::PushL( mailbox );
				iFsEmailSettingsList->SetSelectedAccountInfo( subView.iAccount );
				TUid settingsPluginId = mailbox->GetSettingsUid();
				CleanupStack::PopAndDestroy( mailbox );
				iFsEmailSettingsList->CreatePluginSubviewListL( settingsPluginId );				
				}
			else // Mailbox fetching failed, create account list instead
				{
				iFsEmailSettingsList->CreateAccountListL();
				}
			} // no else
		
		// coming from plugin settings
		if ( aCustomMessageId == TUid::Uid( KMailSettingsReturnFromPluginSettings ) )
			{
	    	// Better to refresh launcher grid view because mailbox branding might be changed.
			iAppUi.LauncherGrid().SetRefreshNeeded();
			CESMailSettingsPlugin::TSubViewActivationData subView;
			TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> SubViewData( subView );
			SubViewData.Copy( aCustomMessage );
			subView = SubViewData();
			
			// if subview was selected from list then return to subview list
			// otherwise return to account list
			if ( subView.iSubviewId > -1 )
				{
				CFSMailBox* mailbox = iFsEmailSettingsList->GetMailBoxByIdL( subView.iAccount.Id() );
				if ( mailbox ) // Malbox pointer can be null
					{
					CleanupStack::PushL( mailbox );
					iFsEmailSettingsList->SetSelectedAccountInfo( subView.iAccount );
					iFsEmailSettingsList->SetSelectedSubListIndex( subView.iSubviewId );
					
					TUid settingsPluginId = mailbox->GetSettingsUid();
					CleanupStack::PopAndDestroy( mailbox );
					if(subView.iPimAccount)
						{
						iFsEmailSettingsList->CreatePluginPIMListL( settingsPluginId );
						iFsEmailSettingsList->SetPimListActivation(ETrue);
						}
					else
						{
						iFsEmailSettingsList->CreatePluginSubviewListL( settingsPluginId );
						}					
					}
				// If mailbox id was null after returning from plugin settings
				// the only thing left to do is create account list once again
				// instead of goinf to subview.
				else 
	 				{
					iFsEmailSettingsList->CreateAccountListL();				
					}
				}
			else
				{
				iFsEmailSettingsList->CreateAccountListL();
				}
			} // no else

		if ( aCustomMessageId == TUid::Null() )
			{
			iFsEmailSettingsList->CreateAccountListL();
			}
		}
	// setup status pane title
    SetupStatusPaneL();

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

	// Make a deferred call to show the "create mailbox" query if we opened the main settings list
	if ( aCustomMessageId.iUid == KMailSettingsOpenMainList ||
	        aCustomMessageId.iUid == KOpenMailSettingsFromGS )
	    {
	    StartMailboxAsyncQueryL();
	    }

	iAppUi.HideTitlePaneConnectionStatus();
	}

// ---------------------------------------------------------------------------
// ChildDoDeactivate
// From CFsEmailUiViewBase
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsListView::ChildDoDeactivate()
	{
    FUNC_LOG;

	if ( iFsEmailSettingsList != NULL )
		{
		AppUi()->RemoveFromViewStack( *this, iFsEmailSettingsList );
		delete iFsEmailSettingsList;
		iFsEmailSettingsList = NULL;
		}

	}

// ---------------------------------------------------------------------------
// SetSelectedMainListIndex
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsListView::SetSelectedMainListIndex( TInt aIndex )
	{
    FUNC_LOG;
	iContainerListIndex = aIndex;
	}

// ---------------------------------------------------------------------------
// GetSelectedMainListIndex
// ---------------------------------------------------------------------------
//
TInt CFsEmailSettingsListView::GetSelectedMainListIndex() const
	{
    FUNC_LOG;
	return iContainerListIndex;
	}

// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// From CAknView.
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsListView::DynInitMenuPaneL( 
		TInt aResourceId, 
		CEikMenuPane* aMenuPane )
	{
    FUNC_LOG;

	if ( iFsEmailSettingsList && aResourceId == R_FS_EMAIL_SETTINGS_LIST_MENU_PANE )
		{
		
	    if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);      
		   }
	    		
        TInt lastIndex = iFsEmailSettingsList->Count()-1;
		// Dimm add/remove in 2nd level
        if ( iFsEmailSettingsList->Get2ndLevelListIndicator() )
            {
            aMenuPane->SetItemDimmed(EFsEmailUiCmdSettingsAddAccount, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdSettingsRemoveAccount, ETrue );
            }
		// Else dimm remove mailbox if no mailbox selected 
		else if ( iFsEmailSettingsList->ListBox()->CurrentItemIndex() == 0
			|| ( iFsEmailSettingsList->ListBox()->CurrentItemIndex() == lastIndex
			&& iFsEmailSettingsList->PIMSyncItemVisible())  )
			{
			aMenuPane->SetItemDimmed( EFsEmailUiCmdSettingsRemoveAccount, ETrue );
			}
		}

	// Add shortcut hints
	iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane, 
	                             CFSEmailUiShortcutBinding::EContextSettings );	
	}

// ---------------------------------------------------------------------------
// SetupStatusPaneL()
// 
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsListView::SetupStatusPaneL()
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
	HBufC* text = iFsEmailSettingsList->CreateTitlePaneTextLC();
	iAppUi.SetTitlePaneTextL( *text );
	CleanupStack::PopAndDestroy( text );

	}

// ---------------------------------------------------------------------------
// CleanupStatusPaneL()
// Sets application default title when leaving this view
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsListView::CleanupStatusPaneL()
	{
    FUNC_LOG;
    iAppUi.ResetTitlePaneTextL();
	}


// ---------------------------------------------------------------------------
// HandleStatusPaneSizeChange()
// From CAknView.
// Handle status pane size change for this view (override).
// ---------------------------------------------------------------------------
//
void CFsEmailSettingsListView::HandleStatusPaneSizeChange()
	{
    FUNC_LOG;
	if ( IsForeground() )
		{
		CAknView::HandleStatusPaneSizeChange();
		
		// this may fail, but we're not able to propagate exceptions here
		TInt result;
		TRAP( result, SetupStatusPaneL() ); 
		}
	}


// End of file.
