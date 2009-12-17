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
* Description:  Remote Contact Lookup Extension setting view.
*
*/


#ifndef CFSCRCLSETTINGVIEW_H
#define CFSCRCLSETTINGVIEW_H

// INCLUDES
#include <MPbk2SettingsViewExtension.h>

// FORWARD DECLARATIONS
class CFscRclSettingControl;
/**
*  CFscRclSettingView - phonebook settings view extension
*  
*/
class CFscRclSettingView : public CBase, public MPbk2SettingsViewExtension
    {
    public:  // Construction and destruction
        
        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CFscRclSettingView* NewL();
        
        /**
         * Creates a new instance of this class.
         * 
         * @return  A new instance of this class.
         */
        static CFscRclSettingView* NewLC();
        
        /**
         * Destructor.
         */
        ~CFscRclSettingView();
        
    public:     // Methods from MPbk2SettingsViewExtension
        
        MPbk2SettingsControlExtension* CreatePbk2SettingsControlExtensionL();
        
        void DynInitMenuPaneL(TInt aMenuId,CEikMenuPane* aMenuPane);
        
        TBool HandleCommandL(TInt aCommand);
        
    public:     // Methods from MPbk2UiReleasable
        void DoRelease();
        
    private:    // CFscRclSettingView private constructors
        
        CFscRclSettingView();
        
        void ConstructL();
        
    private:    // CFscRclSettingView private member variables

        // Phonebook2 settings extension reference
        CFscRclSettingControl* iSettingsControl;        
    };

#endif // CFSCRCLSETTINGVIEW_H
