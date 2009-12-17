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
* Description:  RemoteContactLookup Setting Extension view control
*
*/

#ifndef CFSCRCLSETTINGCONTROL_H
#define CFSCRCLSETTINGCONTROL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <aknsettingitemlist.h>
#include <MPbk2SettingsControlExtension.h>

// FORWARD DECLARATIONS
class CAknSettingItem;
class CPbkxRemoteContactLookupEnv;

/**
*  CFscRclSettingControl -  Remote Contact Lookup setting control for Phonebook 2.
*  
*/
class CFscRclSettingControl : public CAknSettingItemList, 
                              public MPbk2SettingsControlExtension
    {
    public:     // CFscRclSettingControl public constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CFscRclSettingControl* NewL();

        static CFscRclSettingControl* NewLC();
        
        /**
        * Destructor.
        */       
        ~CFscRclSettingControl();
        
        // Methods from MPbk2SettingsControlExtension
        
        /**
         * Modifies the setting item list to contain settings
         * from extensions.
         *
         * @param aSettingItemList  The setting item list to modify.
         */
        void ModifySettingItemListL(CAknSettingItemList& aSettingItemList);
        
        /**
         * Creates a setting item from extension.
         *
         * @param aSettingId    Id of the setting item.
         * @return  Created setting item.
         */
        CAknSettingItem* CreateSettingItemL(TInt aSettingId);     
        
        // Methods from MPbk2UiReleasable

        /**
         * Implement to release this object and any resources it owns.
         */
        void DoRelease();     

    private:    // CFscRclSettingControl private constructors

        /**
         * Default constructor 
         */
        CFscRclSettingControl();

        /**
         * ConstructL
         */
        void ConstructL();
        
    private: // data
        CAknSettingItem* iRclSettingItem; 
        // Own: freestyle remote contact lookup for its setting item
        CPbkxRemoteContactLookupEnv* iRclEnv;
    };

#endif // CFSCRCLSETTINGCONTROL_H