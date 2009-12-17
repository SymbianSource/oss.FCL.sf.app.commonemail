/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR UI Container class
*
*/

// <cmail> custom sw help files not avilable in Cmail
//#include <fscale.hlp.hrh> //for custom_sw helps
//#include <fsmr.hlp.hrh> //for custom_sw helps
// </cmail>
#include "cesmrview.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrinternaluid.h"
#include "cesmrtitlepane.h"
#include "cesmrfield.h"
#include "mesmrcalentry.h"
#include "nmrlayoutmanager.h"
#include "FreestyleEmailUiConstants.h"

#include <eiklabel.h>
#include <avkon.hrh>
#include <MAgnEntryUi.h>
#include <StringLoader.h>
#include <gulcolor.h>
#include <eikimage.h>
#include <esmrgui.rsg>
#include <AknIconUtils.h>
#include <eikenv.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <hlplch.h>
#include <csxhelp/cmail.hlp.hrh>

#include <featmgr.h>
//</cmail>

#include "emailtrace.h"


// <cmail> Removed profiling. </cmail>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRView::CESMRView()
// ---------------------------------------------------------------------------
//
CESMRView::CESMRView( MESMRFieldStorage* aStorage, MESMRCalEntry& aEntry ) :
	iStorage(aStorage),
	iEntry(aEntry)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRView::NewL()
// ---------------------------------------------------------------------------
//
CESMRView* CESMRView::NewL(
        MESMRFieldStorage* aStorage,
        MESMRCalEntry& aEntry,
        const TRect& aRect )
    {
    FUNC_LOG;
    CESMRView* self = new (ELeave) CESMRView( aStorage, aEntry );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRView::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRView::ConstructL( const TRect& aRect )
    {
    FUNC_LOG;
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgAreaMain, aRect, ETrue);

    iLayout = CESMRLayoutManager::NewL();
    iLayout->SetObserver( NULL );
    iTitle = CESMRTitlePane::NewL();
    // Default title text is set here.

    HBufC* title ;
    switch (iEntry.Type())
        {
        case MESMRCalEntry::EESMRCalEntryTodo:
        	{
            title = StringLoader::LoadLC ( R_QTN_CALENDAR_TITLE_NEW_TODO );
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryMemo:
        	{
            title = StringLoader::LoadLC ( R_QTN_CALENDAR_TITLE_NEW_MEMO );
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryAnniversary:
        	{
            title = StringLoader::LoadLC(R_QTN_CALENDAR_TITLE_NEW_ANNIVERSARY);
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryMeetingRequest: // Fall through
        case MESMRCalEntry::EESMRCalEntryMeeting: // Fall through
        case MESMRCalEntry::EESMRCalEntryReminder: // Fall through
        default:
        	{
            title = StringLoader::LoadLC( R_QTN_MEET_REQ_TITLE );
            break;
        	}
        }

    iTitle->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    SetRect( aRect );
    }

// ---------------------------------------------------------------------------
// CESMRView::~CESMRView()
// ---------------------------------------------------------------------------
//
CESMRView::~CESMRView()
    {
    FUNC_LOG;
    delete iTitle;
    delete iList;
    delete iLayout;
    delete iBgContext;
    }

// ---------------------------------------------------------------------------
// CESMRView::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRView::ExternalizeL( TBool aForceValidation )
    {
    FUNC_LOG;
    // externalize all fields:
    iList->ExternalizeL( iEntry, aForceValidation );
    // externalize title pane for priority value
    iTitle->ExternalizeL( iEntry );
    }

// ---------------------------------------------------------------------------
// CESMRView::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRView::InternalizeL()
    {
    FUNC_LOG;
    // internalize title pane ( for priority )
    iTitle->InternalizeL( iEntry );
    iList->DisableSizeChange( ETrue );
    // internalize all fields:
    iList->InternalizeL( iEntry );
    iList->DisableSizeChange( EFalse );
    iList->SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRView::IsComponentVisible()
// ---------------------------------------------------------------------------
//
TBool CESMRView::IsComponentVisible( TESMREntryFieldId aField ) const
    {
    FUNC_LOG;
    return iList->ListObserver().IsControlVisible( aField );
    }


// ---------------------------------------------------------------------------
// CESMRView::CanProcessEditorCommandL
// ---------------------------------------------------------------------------
//
TBool CESMRView::CanProcessEditorCommandL( TInt aCommand )
    {
    FUNC_LOG;
    CESMRField*  field = iList->FocusedItem();
    TBool result(ETrue);
    switch ( aCommand )
        {
        case EESMRCmdClipboardCopy:
        switch (field->FieldId())
            {
            // quite few fields allow copiing
            case EESMRFieldOrganizer:       // fall through 
            case EESMRFieldAttendee:        // fall through 
            case EESMRFieldDetailedSubject: // fall through 
            case EESMRFieldDescription:     // fall through
                result = ETrue;
                break;
            }
        break;
        default:
            result = EFalse;
            break;
        }
    return result;    
    }
    
// ---------------------------------------------------------------------------
// CESMRView::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRView::ProcessEditorCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case EESMRCmdPriorityHigh: // Fall through
        case EESMRCmdPriorityNormal: // Fall through
        case EESMRCmdPriorityLow: 
        	{
            iTitle->HandleSetPriorityCmdL( aCommand );
            break;
        	}
        case EESMRCmdAddOptAttendee:
        	{
            iList->ListObserver().InsertControl( EESMRFieldOptAttendee );
            SetControlFocusedL(EESMRFieldOptAttendee);
            break;
        	}
        case EAknCmdHelp:
        	{
        	break;
        	}
        case EESMRCmdEdit:
            {
            iList->ListObserver().InsertControl( EESMRFieldResponseArea );
            iList->ListObserver().RemoveControl( EESMRFieldResponseReadyArea );
            iList->ListObserver().SetControlFocusedL( EESMRFieldResponseArea );
            break;
            }
        default://forward to fields
        	{
            iList->FocusedItem()->ExecuteGenericCommandL(aCommand);
            break;
        	}
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CESMRView::DynInitMenuPaneL(
        /*TInt aResourceId,*/
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
	if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
		{
		// remove help support in pf5250
		aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
		}
    }


// ---------------------------------------------------------------------------
// CESMRView::LaunchEditorHelpL
// ---------------------------------------------------------------------------
//
void CESMRView::LaunchEditorHelpL()
    {
    FUNC_LOG;
    LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
    }

// ---------------------------------------------------------------------------
// CESMRView::LaunchViewerHelpL
// ---------------------------------------------------------------------------
//
void CESMRView::LaunchViewerHelpL()
    {
    FUNC_LOG;
    LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
    }

// ---------------------------------------------------------------------------
// CESMRView::IsComponentfocused
// ---------------------------------------------------------------------------
//
TBool CESMRView::IsComponentFocused( TESMREntryFieldId aField ) const
	{
    FUNC_LOG;
	return 	FocusedField() == aField;
	}

// ---------------------------------------------------------------------------
// CESMRView::IsComponentfocused
// ---------------------------------------------------------------------------
//
TESMREntryFieldId CESMRView::FocusedField() const
    {
    FUNC_LOG;
    TESMREntryFieldId fieldId( ( TESMREntryFieldId ) 0 );
    
	CESMRField* field = iList->FocusedItem();
    
    if ( field )
        {
        fieldId = field->FieldId();
        }
    
    return fieldId;
	}

// ---------------------------------------------------------------------------
// CESMRView::LaunchHelpL
// ---------------------------------------------------------------------------
//
void CESMRView::LaunchHelpL( const TDesC& aContext )
    {
    FUNC_LOG;
    
    CArrayFix<TCoeHelpContext>* cntx = 
		new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(cntx);
    cntx->AppendL( TCoeHelpContext( KFSEmailUiUid, aContext ) );
    CleanupStack::Pop(cntx);
    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), cntx );
    }

// ---------------------------------------------------------------------------
// CESMRView::UpdateTitlePaneTextL
// ---------------------------------------------------------------------------
//
void CESMRView::UpdateTitlePaneTextL( const TDesC& aText )
    {
    FUNC_LOG;
    SetTitleL( aText );
    }

// ---------------------------------------------------------------------------
// CESMRView::UpdateTitlePanePriorityIconL()
// ---------------------------------------------------------------------------
//
void CESMRView::UpdateTitlePanePriorityIconL( TUint aPriority )
    {
    FUNC_LOG;
    iTitle->SetPriorityL( aPriority );
    }

// ---------------------------------------------------------------------------
// CESMRView::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRView::SetContainerWindowL(const CCoeControl& aContainer)
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );
    iTitle->SetContainerWindowL( *this );
    /**
     * Somehow if list is constructed before this SetContainerWindowL is called,
     * all redraws throws KERN-EXEC 3 (the CCoeControl's iWin variable is NULL)
     */
    
    iList = CESMRListComponent::NewL( this, iStorage, iLayout );
    iList->SetBackground( this );
    // Font size setting Observer can now be set
    iLayout->SetObserver( this );
    }

// ---------------------------------------------------------------------------
// CESMRView::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRView::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );

    if ( iTitle )
        ++count;

    if ( iList )
        ++count;

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRView::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRView::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
        	{
            return iTitle;
        	}
        case 1:
        	{
            return iList;
        	}
        default:
        	{
            return NULL;
        	}
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRView::SizeChanged()
    {
    FUNC_LOG;
    
    TRect rect( Rect() );
    if ( iTitle )
        {
        TAknWindowComponentLayout titleLayout =
            NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutCtrlBar );
        AknLayoutUtils::LayoutControl( iTitle, rect, titleLayout );
        }
    if ( iList )
        {
        TAknWindowComponentLayout listLayout =
            NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutContentArea );
        AknLayoutUtils::LayoutControl( iList, rect, listLayout );
        }
    if( iBgContext )
        {
        iBgContext->SetRect( rect );
        if ( &Window() )
            {
            iBgContext->SetParentPos( PositionRelativeToScreen() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::MinimumSize()
// Gets the main pane size needed by CEikDialog to layout itself correctly
// ---------------------------------------------------------------------------
//
TSize CESMRView::MinimumSize()
    {
    FUNC_LOG;
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    return rect.Size();
    }

// ---------------------------------------------------------------------------
// CESMRView::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRView::OfferKeyEventL(
		const TKeyEvent &aKeyEvent, 
		TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    if ( iList )
        {
        response = iList->OfferKeyEventL( aKeyEvent, aType );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRView::Draw
// This Draw is called when background for title pane (iTitle) is drawn
// ---------------------------------------------------------------------------
//
void CESMRView::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();
      
    // Draw the background for iTitle using the current skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );
    }

// ---------------------------------------------------------------------------
// CESMRView::Draw
// This draw method draws the dialog area
// ---------------------------------------------------------------------------
//
void CESMRView::Draw(
		CWindowGc&  aGc , 
		const CCoeControl& /* aControl */, 
		const TRect& /*aRect*/) const
    {
    FUNC_LOG;
    aGc.Clear();
    }

// ---------------------------------------------------------------------------
// CESMRView::GetTextDrawer
// ---------------------------------------------------------------------------
//
void CESMRView::GetTextDrawer(
		CCoeTextDrawerBase*& /*aText*/, 
		const CCoeControl* /*aControl*/) const
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRView::SetTitleL()
// ---------------------------------------------------------------------------
//
void CESMRView::SetTitleL( const TDesC& aTitle, TBool aDefault)
    {
    FUNC_LOG;
    iTitle->SetTextL( aTitle );
    iTitle->SetDefaultIcon(aDefault);
    }

// ---------------------------------------------------------------------------
// CESMRView::SetControlFocusedL()
// ---------------------------------------------------------------------------
//
void CESMRView::SetControlFocusedL( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    iList->ListObserver().SetControlFocusedL( aField );
    }

// ---------------------------------------------------------------------------
// CESMRView::FontSizeSettingsChanged()
// ---------------------------------------------------------------------------
//
void CESMRView::FontSizeSettingsChanged()
    {
    FUNC_LOG;
    iList->HandleFontChange();
    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRView::HandleResourceChange()
// ---------------------------------------------------------------------------
//
void CESMRView::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );
    TInt error = KErrNone;
    
    switch ( aType )
        {
        case  KAknsMessageSkinChange:
            {
            TRAP( error, iList->InitializeL() );
            SizeChanged();
            break;
            }
        case KEikMessageColorSchemeChange:
            {
            TRAP( error, iList->InitializeL() );
            SizeChanged();
            break;
            }
        case KEikDynamicLayoutVariantSwitch:
        case KAknLocalZoomLayoutSwitch:
            {
            SizeChanged();
            break;
            }
        default:
            {
            break;
            }
        }

    if ( error != KErrNone )
        {
        iCoeEnv->HandleError( error );
        }
    }

// ---------------------------------------------------------------------------
// CESMRView::MopSupplyObject()
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CESMRView::MopSupplyObject( TTypeUid aId )
    {
    if ( iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }
    
// <cmail>
// ---------------------------------------------------------------------------
// CESMRView::FocusChanged
// ---------------------------------------------------------------------------
//
void CESMRView::FocusChanged(TDrawNow aDrawNow)
    {
    CCoeControl::FocusChanged(aDrawNow);
    
    // if attachments are focused, update the field to make sure
    // attachment status is shown correctly
    if( IsComponentFocused( EESMRFieldAttachments ) )
        {
        TRAP_IGNORE( iList->FocusedItem()->SetOutlineFocusL(ETrue) );
        }
    }
// </cmail>

// EOF

