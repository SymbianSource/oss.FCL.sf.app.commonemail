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
* Description:  Implements visualiser for the attachment list view
*
*/



// SYSTEM INCLUDES
#include "emailtrace.h"
#include <freestyleemailui.mbg>
#include <AknUtils.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>
#include <aknlistquerydialog.h>
#include <e32cmn.h>
#include <featmgr.h>                    // For FeatureManager
#include <alf/alfcontrolgroup.h>

#include <csxhelp/cmail.hlp.hrh>

// INTERNAL INCLUDES
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiSendAttachmentsListVisualiser.h"
#include "FreestyleEmailUiSendAttachmentsListControl.h"
#include "FreestyleEmailUiSendAttachmentsListModel.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiShortcutBinding.h"


// CLASS IMPLEMENTATION

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListVisualiser* CFSEmailUiSendAttachmentsListVisualiser::NewL(
		CAlfEnv& aEnv, 
		CFreestyleEmailUiAppUi* aAppUi,
		CAlfControlGroup& aMailListControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiSendAttachmentsListVisualiser* self = CFSEmailUiSendAttachmentsListVisualiser::NewLC(aEnv, aAppUi, aMailListControlGroup );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListVisualiser* CFSEmailUiSendAttachmentsListVisualiser::NewLC(
		CAlfEnv& aEnv, 
		CFreestyleEmailUiAppUi* aAppUi,
		CAlfControlGroup& aMailListControlGroup )
	{
    FUNC_LOG;
    CFSEmailUiSendAttachmentsListVisualiser* self = new (ELeave) CFSEmailUiSendAttachmentsListVisualiser(aAppUi, aEnv, aMailListControlGroup);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::ConstructL()
	{
    FUNC_LOG;

	BaseConstructL( R_FSEMAILUI_SENDATTACHMENTS_VIEW );
	
	// Create list screen (list) control and screen anchor layout
	iScreenControl = CFreestyleEmailUiSendAttachmentsListControl::NewL( *iEnv, this, &iAppUi );
	// insert control to control group given from AppUi
	// owner transferred
    //<cmail> Compared to S60 3.2.3 in S60 5.0 Alf offers the key events in
    // opposite order.
	ControlGroup().AppendL( iScreenControl );
	ControlGroup().AppendL( iScreenControl->Model()->AttachmentList()->TreeControl() );
	//</cmail>

	}


// ---------------------------------------------------------------------------
// c++ constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListVisualiser::CFSEmailUiSendAttachmentsListVisualiser( 
		CFreestyleEmailUiAppUi* aAppUi,
		CAlfEnv& aEnv, 
		CAlfControlGroup& aAttachmentsListControlGroup )
    : CFsEmailUiViewBase( aAttachmentsListControlGroup, *aAppUi ),
      iEnv( &aEnv )
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// c++ destructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListVisualiser::~CFSEmailUiSendAttachmentsListVisualiser()
	{
    FUNC_LOG;
	if ( iScreenControl->Model() )
		{
		iScreenControl->DeleteModel();
		}
	}


// ---------------------------------------------------------------------------
// Id
// from aknview
// ---------------------------------------------------------------------------
//
TUid CFSEmailUiSendAttachmentsListVisualiser::Id() const
	{
    FUNC_LOG;
	return SendAttachmentMngrViewId;
	}			   

// ---------------------------------------------------------------------------
// DoActivateL
// From aknview
// ---------------------------------------------------------------------------
//
// <cmail> Toolbar
/*void CFSEmailUiSendAttachmentsListVisualiser::DoActivateL( 
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage )*/
void CFSEmailUiSendAttachmentsListVisualiser::ChildDoActivateL( 
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage )
// </cmail> Toolbar
	{
    FUNC_LOG;

	if ( aCustomMessageId == TUid::Uid( KEditorCmdOpenAttachmentsList ) )
		{
	    TPckgBuf<TEditorLaunchParams> buf( iEditorParams );
	    buf.Copy( aCustomMessage );
	    iEditorParams = buf();
	    	    
	    iPrevViewId = aPrevViewId;
	    
		RefreshL();
		}
	SetMskL();
	}

// ---------------------------------------------------------------------------
// ChildDoDeactivate
// From CFsEmailUiViewBase
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::ChildDoDeactivate()
	{
    FUNC_LOG;
    if ( !iAppUi.AppUiExitOngoing() )
        {
        iScreenControl->Model()->GetVisualizer()->NotifyControlVisibilityChange( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// From aknview
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::DynInitMenuPaneL( 
		TInt aResourceId, 
		CEikMenuPane* aMenuPane )
	{
    FUNC_LOG;
	if ( aResourceId == R_FS_EMAILUI_SENDATTACHMENTS_MENU_PANE )
		{
		// if there are read-only attachments or there's no multiple attachments, 
		// remove all functionality is disabled
		if ( iScreenControl->Model()->Count() <= 1 ||
		     iScreenControl->Model()->HasReadOnlyAttachments() )
		    {
			aMenuPane->SetItemDimmed( ESendAttachmentMenuRemoveAllAttachments, ETrue );
		    }

        // if no attachment is focused then disable open and remove
		CFSEmailUiSendAttachmentsListModelItem* item = iScreenControl->Model()->GetSelectedItem();
		if ( !item || item->IsRemote() )
		    {
		    aMenuPane->SetItemDimmed( ESendAttachmentMenuOpen, ETrue );
		    }
		
		if ( !item || item->IsReadOnly() )
		    {
		    aMenuPane->SetItemDimmed( ESendAttachmentMenuRemoveAttachment, ETrue );
		    }

        // remove help support in pf5250
		if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
	       {
	       aMenuPane->SetItemDimmed( ESendAttachmentMenuHelp, ETrue);
	       }   
	    }

	// Add shortcut hints
	iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane, 
	                             CFSEmailUiShortcutBinding::EContextSendAttachmentList );	
	}

// ---------------------------------------------------------------------------
// RefreshL
// Refreshes list items
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::RefreshL()
	{
    FUNC_LOG;
	iScreenControl->Model()->ReFreshListL();
	}
	

// ---------------------------------------------------------------------------
// HandleDynamicVariantSwitchL
// from aknview
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType /*aType*/ )
	{
    FUNC_LOG;
	ReScaleUiL();
	}

// ---------------------------------------------------------------------------
// GetParentLayoutsL
// From CFsEmailUiViewBase
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::GetParentLayoutsL(
        RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    aLayoutArray.AppendL( iScreenControl->Model()->GetParentLayout() );
    }

// ---------------------------------------------------------------------------
// HandleCommandL
// from aknview
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
  		{
  		case EAknSoftkeyOpen:
  		case ESendAttachmentMenuOpen:
  			// open file in suitable program
  			iScreenControl->OpenHighlightedFileL();
  			break;
  		case EFsEmailUiCmdCollapse:
  		    iScreenControl->HandleCommandL(EFsEmailUiCmdCollapse);
  		    break;
  		case EFsEmailUiCmdExpand:
  		    iScreenControl->HandleCommandL(EFsEmailUiCmdExpand);
  			break;
  		case ESendAttachmentMenuAddAttachment:
			iScreenControl->AppendAttachmentToListL();
  			break;
  		case ESendAttachmentMenuRemoveAttachment:
  			iScreenControl->RemoveAttachmentFromListL();
  			break;
  		case ESendAttachmentMenuRemoveAllAttachments:
  			iScreenControl->RemoveAllAttachmentsL();
  			break;
        case ESendAttachmentMenuExit:
            iAppUi.Exit();
            break;
        case ESendAttachmentMenuHelp:
        	TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
        	break;
        case EAknSoftkeyBack:
        	{
        	TPckgBuf<TEditorLaunchParams> buf( iEditorParams );
  	       	iAppUi.ReturnToPreviousViewL( buf );
        	break;
        	}
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
        default:
        	break;
        }
    
    // Handling the command may have changed the focused item
    SetMskL();
    }


// ---------------------------------------------------------------------------
// SetViewSoftkeysL
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListVisualiser::SetViewSoftkeysL( TInt aResourceId )
	{
    FUNC_LOG;
    Cba()->SetCommandSetL( aResourceId );
    Cba()->DrawDeferred();	
	}

void CFSEmailUiSendAttachmentsListVisualiser::SetMskL()
    {
    FUNC_LOG;
    TFsTreeItemId curId = iScreenControl->Model()->AttachmentList()->FocusedItem(); 
    
    if ( curId != KFsTreeNoneID )
        {
        if ( (iScreenControl->Model()->AttachmentList()->IsNode( curId )) )
            {
            if ( iScreenControl->Model()->AttachmentList()->IsExpanded( curId ) )
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_COLLAPSE );
                } 
            else  
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND );
                }
            }
        else // non-node item
            {
            CFSEmailUiSendAttachmentsListModelItem* item =
                iScreenControl->Model()->GetSelectedItem();
            if ( item && !item->IsRemote() )
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_OPEN );
                }
            else
                { // Open command is inavailable for remote attachments
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                }
            }
        }
    else
        {
        // No item in focus
        ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
        }
    }

// ---------------------------------------------------------------------------
// OfferEventL
// 
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiSendAttachmentsListVisualiser::OfferEventL( const TAlfEvent& /*aEvent*/ )
    {
    FUNC_LOG;
    return EFalse;
    }

void CFSEmailUiSendAttachmentsListVisualiser::ReScaleUiL()
	{
    FUNC_LOG;
	iScreenControl->Model()->ReScaleUiL();
	}

CAlfControl* CFSEmailUiSendAttachmentsListVisualiser::ViewerControl()
	{
    FUNC_LOG;
	return iScreenControl;
	}

TEditorLaunchParams CFSEmailUiSendAttachmentsListVisualiser::EditorParams()
	{
    FUNC_LOG;
	return iEditorParams;
	}

