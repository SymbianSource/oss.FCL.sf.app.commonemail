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
* Description: encapsulates navipane controls of mail viewer view
*
*/

#include <coecontrolarray.h>
#include <eiklabel.h>
#include <eikimage.h>
#include <data_caging_path_literals.hrh>
#include <barsread.h>               // TResourceReader
#include <gulicon.h>                // CGulIcon
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <avkon.rsg>
#include <layoutmetadata.cdl.h>     // Layout
#include <applayout.cdl.h>          // LAF
#include <AknStatuspaneUtils.h>     // AknStatuspaneUtils
#include <aknlayoutscalable_apps.cdl.h>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>
#include <touchfeedback.h>
#include <avkon.mbg>

#include "FreestyleEmailUiNaviPaneControl2MailViewer.h"
#include "FreestyleEmailUiUtilities.h"

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CFreestyleEmailUiNaviPaneControlContainer2MailViewer::CFreestyleEmailUiNaviPaneControlContainer2MailViewer()
    {
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CFreestyleEmailUiNaviPaneControlContainer2MailViewer::ConstructL()
    {
    iControlArray = CCoeControlArray::NewL( *this );
    iControlArray->SetControlsOwnedExternally( EFalse );
    ActivateL();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CFreestyleEmailUiNaviPaneControlContainer2MailViewer* CFreestyleEmailUiNaviPaneControlContainer2MailViewer::NewL()
    {
    CFreestyleEmailUiNaviPaneControlContainer2MailViewer* self =
        new(ELeave) CFreestyleEmailUiNaviPaneControlContainer2MailViewer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CFreestyleEmailUiNaviPaneControlContainer2MailViewer::~CFreestyleEmailUiNaviPaneControlContainer2MailViewer()
    {
    delete iControlArray;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CFreestyleEmailUiNaviPaneControlContainer2MailViewer::AddToControlArrayL( CCoeControl* aControl, TFreestyleEmailNaviControlId aControlId )
    {
    iControlArray->AppendLC( aControl, aControlId );
    CleanupStack::Pop( aControl );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CFreestyleEmailUiNaviPaneControlContainer2MailViewer::RemoveFromControlArray( TFreestyleEmailNaviControlId aControlId )
    {
    CCoeControl* control = iControlArray->RemoveById( aControlId );
    delete control;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TInt CFreestyleEmailUiNaviPaneControlContainer2MailViewer::CountComponentControls() const
    {
    return iControlArray->Count();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CCoeControl* CFreestyleEmailUiNaviPaneControlContainer2MailViewer::ComponentControl(TInt aIndex) const
    {
    return iControlArray->At( aIndex ).iControl;
    }
