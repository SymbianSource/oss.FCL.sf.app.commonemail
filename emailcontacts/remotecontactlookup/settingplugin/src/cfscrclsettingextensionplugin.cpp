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
* Description:  Remote Contact Lookup Extension plug-in.
*
*/


#include "emailtrace.h"
#include <eikmenup.h>
#include "cfscrclsettingextensionplugin.h"
#include "cfscrclsettingview.h"

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CFscRclSettingExtensionPlugin* CFscRclSettingExtensionPlugin::NewL()
    {
    FUNC_LOG;
    CFscRclSettingExtensionPlugin* self = 
        new( ELeave ) CFscRclSettingExtensionPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::~CFscRclSettingExtensionPlugin
// --------------------------------------------------------------------------
//
CFscRclSettingExtensionPlugin::~CFscRclSettingExtensionPlugin()
    {
    FUNC_LOG;
    }
    
// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CFscRclSettingExtensionPlugin::CreateExtensionViewL
        ( TUid /*aViewId*/, CPbk2UIExtensionView& /*aView*/ )
    {
    FUNC_LOG;
    return NULL;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CFscRclSettingExtensionPlugin::DynInitMenuPaneL
        ( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/,
          MPbk2ContactUiControl& /*aControl*/ )
    {
    FUNC_LOG;
    // Do nothing
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CFscRclSettingExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    FUNC_LOG;
    // Do nothing
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CFscRclSettingExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/, const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    FUNC_LOG;
    return EFalse;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CFscRclSettingExtensionPlugin::ApplyDynamicViewGraphChangesL
        ( CPbk2ViewGraph& /*aViewGraph*/ )
    {
    FUNC_LOG;
    // Do nothing
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::ApplyDynamicPluginInformationDataL
// --------------------------------------------------------------------------
//
void CFscRclSettingExtensionPlugin::ApplyDynamicPluginInformationDataL
        ( CPbk2UIExtensionInformation& /*aUiExtensionInformation*/ )
    {
    FUNC_LOG;
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension* 
    CFscRclSettingExtensionPlugin::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& /*aContactManager*/,
          MVPbkStoreContact& /*aContact*/,
          MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
    CFscRclSettingExtensionPlugin::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension* CFscRclSettingExtensionPlugin::CreatePbk2SettingsViewExtensionL( CVPbkContactManager& /*aContactManager*/ )
    {
    FUNC_LOG;
    return CFscRclSettingView::NewL();
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CFscRclSettingExtensionPlugin::CreatePbk2AppUiExtensionL
    ( CVPbkContactManager& /*aContactManager*/ )
    {
    FUNC_LOG;
    return NULL;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CFscRclSettingExtensionPlugin::CreatePbk2CommandForIdL
    ( TInt /*aCommandId*/, MPbk2ContactUiControl& /*aUiControl*/) const
    {
    FUNC_LOG;
    MPbk2Command* result = NULL;
    return result;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CFscRclSettingExtensionPlugin::CreatePbk2AiwInterestForIdL
    ( TInt /*aInterestId*/, CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    FUNC_LOG;
    return NULL;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::CFscRclSettingExtensionPlugin
// --------------------------------------------------------------------------
//
CFscRclSettingExtensionPlugin::CFscRclSettingExtensionPlugin()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscRclSettingExtensionPlugin::ConstructL
// --------------------------------------------------------------------------
//
void CFscRclSettingExtensionPlugin::ConstructL()
    {
    FUNC_LOG;
    }

