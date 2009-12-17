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
* Description:  ESMR policy implementation
*
*/

#include "cesmrresponsedialog.h"
#include "cmrresponsedialogview.h"
#include "esmrgui.hrh"
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "esmrinternaluid.h"

#include <eikenv.h>
#include <eikappui.h>
#include <avkon.hrh>
#include <MAgnEntryUi.h>
#include <eikedwin.h>
#include <esmrgui.rsg>
#include <calentry.h>
#include <hlplch.h>
#include <akntoolbar.h>
#include <aknappui.h>
#include <aknViewAppUi.h>
#include <aknview.h>
#include <eikcolib.h>
#include <akntoolbarobserver.h>
#include <aknbutton.h>

#include <featmgr.h>
//</cmail>

// <cmail> custom sw help files not avilable in Cmail
//#include <fsmr.hlp.hrh>
// </cmail>

// DEBUG
#include "emailtrace.h"


// ---------------------------------------------------------------------------
// CESMRResponseDialog::CESMRResponseDialog
// ---------------------------------------------------------------------------
//
CESMRResponseDialog::CESMRResponseDialog( HBufC*& aText ): iText(aText)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialog::~CESMRResponseDialog
// ---------------------------------------------------------------------------
//
CESMRResponseDialog::~CESMRResponseDialog()
    {
    FUNC_LOG;
    TRAP_IGNORE( RestoreMrGuiToolbarL() );
    // iView is deleted by framework because it uses the
    // custom control mechanism.
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialog::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRResponseDialog* CESMRResponseDialog::NewL( HBufC*& aText )
    {
    FUNC_LOG;
    CESMRResponseDialog* self =
        new (ELeave) CESMRResponseDialog( aText );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRResponseDialog::ConstructL()
    {
    FUNC_LOG;
    CAknDialog::ConstructL( R_RESPONSE_DIALOG_MENUBAR );
    CEikonEnv* eikonEnv = CEikonEnv::Static();// codescanner::eikonenvstatic
    TRect clientRect = eikonEnv->EikAppUi()->ClientRect();
    iView = CESMRResponseDialogView::NewL();
    iView->SetRect( clientRect );
    iView->SetSize(clientRect.Size());

    MakeResponseToolbarL();
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialog::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRResponseDialog::ProcessCommandL( TInt aCommand )
    {
    FUNC_LOG;
    CAknDialog::ProcessCommandL( aCommand );

    switch ( aCommand )
        {
        case EAknCmdHelp:
            {
            // <cmail>
            // TUid uid = { KUidCalendarApplication };
            // </cmail>
            CArrayFix<TCoeHelpContext>* cntx = 
				new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
            CleanupStack::PushL( cntx );
            // <cmail>
            //cntx->AppendL( TCoeHelpContext( uid, KFSCA_HLP_RESPONSE_EDITOR ) );
            // /</cmail>
            CleanupStack::Pop( cntx );
            HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), cntx );
            break;
            }
        case EESMRCmdSendMR: // Fall through
        case EAknSoftkeyBack: // Fall through
        case EAknCmdExit:
            {
            TryExitL( aCommand );
            break;
            }
        default:
            {
            break;
            }
        }
    }
// ---------------------------------------------------------------------------
// CESMRResponseDialog::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CESMRResponseDialog::DynInitMenuPaneL(
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
// CESMRResponseDialog::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRResponseDialog::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    if ( !MenuShowing() )
        {
        response = iView->OfferKeyEventL( aEvent, aType );
        }

    if ( response == EKeyWasNotConsumed )
        {
        response = CAknDialog::OfferKeyEventL( aEvent, aType );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialog::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CESMRResponseDialog::OkToExitL(TInt aButtonId)
    {
    FUNC_LOG;
    TBool res( EFalse );
    // show options
    if ( aButtonId == EAknSoftkeyOptions )
        {
        CAknDialog::DisplayMenuL();
        }
    // exit dialog
    if ( aButtonId == EESMRCmdSendMR ||
         aButtonId == EAknSoftkeyBack ||
         aButtonId == EAknCmdExit )
        {
        iText = iView->GetTextL();
        res = ETrue;
        }

    return res;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialog::CreateCustomControlL
// ---------------------------------------------------------------------------
//
SEikControlInfo CESMRResponseDialog::CreateCustomControlL( TInt aType )
    {
    FUNC_LOG;
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iFlags = 0;
    controlInfo.iTrailerTextId = 0;

    if ( aType == EEsMrResponseDialogType )
        {
        controlInfo.iControl = iView;
        }

    return controlInfo;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialog::ExecuteViewLD
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRResponseDialog::ExecuteDlgLD()
    {
    FUNC_LOG;
    return ExecuteLD( R_RESPONSE_DIALOG );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::OfferToolbarEventL
// ---------------------------------------------------------------------------
//
void CESMRResponseDialog::OfferToolbarEventL( TInt aCommand ) 
    {
    TryExitL( aCommand );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::MakeResponseToolbarL
// ---------------------------------------------------------------------------
//
void CESMRResponseDialog::MakeResponseToolbarL()
    {
    CAknToolbar* currentToolbar = static_cast<CEikAppUiFactory*>( iEikonEnv->AppUiFactory() )->CurrentFixedToolbar();
    iOldObserver = currentToolbar->ToolbarObserver();
    currentToolbar->SetToolbarObserver( this );

    CAknButton* buttonSend = CAknButton::NewL();
    buttonSend->ConstructFromResourceL( R_SEND_BUTTON );
    CAknButton* buttonEmpty1 = CAknButton::NewL();
    CAknButton* buttonEmpty2 = CAknButton::NewL();
    
    currentToolbar->AddItemL( buttonSend, EAknCtButton, EESMRCmdSendMR, 0, 0 );
    currentToolbar->AddItemL( buttonEmpty1, EAknCtButton, EESMRCmdUndefined, 0, 1 );
    currentToolbar->AddItemL( buttonEmpty2, EAknCtButton, EESMRCmdUndefined, 0, 2 );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::RestoreMrGuiToolbarL
// ---------------------------------------------------------------------------
//
void CESMRResponseDialog::RestoreMrGuiToolbarL()
    {
    CAknToolbar* currentToolbar = static_cast<CEikAppUiFactory*>( iEikonEnv->AppUiFactory() )->CurrentFixedToolbar();

    currentToolbar->SetToolbarObserver( iOldObserver );

    CAknButton* buttonAccept    = CAknButton::NewL();
    CAknButton* buttonTentative = CAknButton::NewL();
    CAknButton* buttonDecline   = CAknButton::NewL();
    buttonAccept->ConstructFromResourceL( R_ACCEPT_BUTTON );
    buttonTentative->ConstructFromResourceL( R_TENTATIVE_BUTTON );
    buttonDecline->ConstructFromResourceL( R_DECLINE_BUTTON );
    currentToolbar->AddItemL( buttonAccept, EAknCtButton, EESMRCmdAcceptMR, 0, 0 );
    currentToolbar->AddItemL( buttonTentative, EAknCtButton, EESMRCmdTentativeMR, 0, 1 );
    currentToolbar->AddItemL( buttonDecline, EAknCtButton, EESMRCmdDeclineMR, 0, 2 );
    }
// EOF
