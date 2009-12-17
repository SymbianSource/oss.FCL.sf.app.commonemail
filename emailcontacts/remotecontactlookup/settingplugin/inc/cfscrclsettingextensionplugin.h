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
* Description:  RemoteContactLookup Setting Extension plug-in.
*
*/


#ifndef CFSCRCLSETTINGEXTENSIONPLUGIN_H
#define CFSCRCLSETTINGEXTENSIONPLUGIN_H

#include <CPbk2UIExtensionPlugin.h>

// FORWARD DECLARATIONS

/**
 *  RemoteContactLookup Setting Extension plugin.
 *  Adds RemoteContactLookup setting menu to the Phonebook2 Settings menu
 *
 *  @lib cfscrclsetting.lib
 *  @since S60 v3.2
 */
class CFscRclSettingExtensionPlugin : public CPbk2UIExtensionPlugin
    {
    
public:

    /**
     * Two-phased constructor.
     */
    static CFscRclSettingExtensionPlugin* NewL();    
    
    /**
    * Destructor.
    */
    virtual ~CFscRclSettingExtensionPlugin();

// from base class CPbk2UIExtensionPlugin
    MPbk2UIExtensionView* CreateExtensionViewL(
            TUid aViewId,
            CPbk2UIExtensionView& aView );

    void DynInitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane,
            MPbk2ContactUiControl& aControl );


    void UpdateStorePropertiesL(
            CPbk2StorePropertyArray& aPropertyArray );

    TBool GetHelpContextL(
            TCoeHelpContext& aContext,
            const CPbk2AppViewBase& aView,
            MPbk2ContactUiControl& aUiControl );

    void ApplyDynamicViewGraphChangesL(
            CPbk2ViewGraph& aViewGraph );

    void ApplyDynamicPluginInformationDataL(
            CPbk2UIExtensionInformation& aUiExtensionInformation );    

// from base class MPbk2UIExtensionFactory

    MPbk2ContactEditorExtension* CreatePbk2ContactEditorExtensionL(
            CVPbkContactManager& aContactManager,
            MVPbkStoreContact& aContact,
            MPbk2ContactEditorControl& aEditorControl );

    MPbk2ContactUiControlExtension*  CreatePbk2UiControlExtensionL(
            CVPbkContactManager& aContactManager );

    MPbk2SettingsViewExtension* CreatePbk2SettingsViewExtensionL(
            CVPbkContactManager& aContactManager );

    MPbk2AppUiExtension* CreatePbk2AppUiExtensionL(
            CVPbkContactManager& aContactManager );

    MPbk2Command* CreatePbk2CommandForIdL(
            TInt aCommandId,
            MPbk2ContactUiControl& aUiControl ) const;

    MPbk2AiwInterestItem* CreatePbk2AiwInterestForIdL(
            TInt aInterestId,
            CAiwServiceHandler& aServiceHandler ) const;
            
private:
    
    CFscRclSettingExtensionPlugin();

    void ConstructL();

private: // data
    };
    
#endif // CFSCRCLSETTINGEXTENSIONPLUGIN_H
