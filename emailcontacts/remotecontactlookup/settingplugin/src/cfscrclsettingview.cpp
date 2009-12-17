/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Remote Contact Lookup Extension setting view
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cfscrclsettingview.h"
#include "cfscrclsettingcontrol.h"

// -----------------------------------------------------------------------------
// CFscRclSettingView::CFscRclSettingView
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFscRclSettingView::CFscRclSettingView()
    {
    FUNC_LOG;
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CFscRclSettingView::ConstructL
// Actual constructor.
// ----------------------------------------------------------------------------
//
void CFscRclSettingView::ConstructL()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFscRclSettingView::NewLC
// CFscRclSettingView two-phased constructor.
// -----------------------------------------------------------------------------
//
CFscRclSettingView* CFscRclSettingView::NewLC()
    {
    FUNC_LOG;
    CFscRclSettingView* self = new (ELeave)CFscRclSettingView();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CFscRclSettingView::NewL
// CFscRclSettingView two-phased constructor.
// -----------------------------------------------------------------------------
//
CFscRclSettingView* CFscRclSettingView::NewL()
    {
    FUNC_LOG;
    CFscRclSettingView* self = CFscRclSettingView::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CFscRclSettingView::~CFscRclSettingView
// CFscRclSettingView Destructor
// -----------------------------------------------------------------------------
//
CFscRclSettingView::~CFscRclSettingView()
    {
    FUNC_LOG;
    // iSettingsControl is released by Phonebook extension manager
    }

// -----------------------------------------------------------------------------
//                  MPbk2SettingsViewExtension public methods
// -----------------------------------------------------------------------------
// CFscRclSettingView::CreatePbk2SettingsControlExtensionL
// Phonebook2 Settings control extension object.
// -----------------------------------------------------------------------------
//
MPbk2SettingsControlExtension*
CFscRclSettingView::CreatePbk2SettingsControlExtensionL()
    {
	FUNC_LOG;
    return (iSettingsControl = CFscRclSettingControl::NewL());
    }

// -----------------------------------------------------------------------------
// CFscRclSettingView::DynInitMenuPaneL()
// Dynamic menu content
// -----------------------------------------------------------------------------
//
void CFscRclSettingView::DynInitMenuPaneL(TInt /*aMenuId*/,CEikMenuPane* /*aMenuPane*/)
    {
    FUNC_LOG;
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CFscRclSettingView::HandleCommandL
// Settings view command handler
// -----------------------------------------------------------------------------
//
TBool CFscRclSettingView::HandleCommandL(TInt /*aCommand*/)
    {
    FUNC_LOG;
    return EFalse;
    }

// -----------------------------------------------------------------------------
//                      MPbk2UiReleasable implementation
// -----------------------------------------------------------------------------
// CFscRclSettingView::DoRelease
// Release this
// -----------------------------------------------------------------------------
//
void CFscRclSettingView::DoRelease()
    {
    FUNC_LOG;
    delete this;
    }

