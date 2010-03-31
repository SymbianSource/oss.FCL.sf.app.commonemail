/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikcolib.h>
#include <eikappui.h>
#include <avkon.hrh>
#include <magnentryui.h>
#include <eikedwin.h>
#include <esmrgui.rsg>
#include <calentry.h>
#include <hlplch.h>
#include <akntoolbar.h>

// <cmail> custom sw help files not avilable in Cmail
#include <csxhelp/cmail.hlp.hrh>
// </cmail>

// DEBUG
#include "emailtrace.h"
#include "FreestyleEmailUiConstants.h"

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
    static_cast<CEikAppUiFactory*>( 
    		iEikonEnv->AppUiFactory() )->CurrentFixedToolbar()->
    		SetToolbarVisibility( iPreviousVisibility );
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
    iPreviousVisibility = static_cast<CEikAppUiFactory*>( 
    		iEikonEnv->AppUiFactory() )->CurrentFixedToolbar()->IsShown();
    static_cast<CEikAppUiFactory*>( 
        	iEikonEnv->AppUiFactory() )->CurrentFixedToolbar()->SetToolbarVisibility( EFalse );
    TRect clientRect = iEikonEnv->EikAppUi()->ClientRect();
    iView = CESMRResponseDialogView::NewL();
    iView->SetRect( clientRect );
    iView->SetSize(clientRect.Size());
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
            CArrayFix<TCoeHelpContext>* cntx =
				new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
            CleanupStack::PushL( cntx );
            cntx->AppendL( TCoeHelpContext( KFSEmailUiUid, KFSCA_HLP_RESPONSE_EDITOR ) );
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

// EOF
