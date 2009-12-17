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
* Description:  Definition of the class CPbkxRclSettingsEngine.
*
*/


#ifndef CPBKXRCLSETTINGSENGINE_H
#define CPBKXRCLSETTINGSENGINE_H

#include "mpbkxremotecontactlookupsettingsui.h"

/**
*  Provides functionality to store and retrieve settings.
*
*  @lib pbkxrclengine.lib
*  @since S60 3.1
*/
class CPbkxRclSettingsEngine : public CBase, public MPbkxRemoteContactLookupSettingsUi
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    * 
    * @return instance
    */
    IMPORT_C static CPbkxRclSettingsEngine* NewL();
    
    /**
    * Two-phased constructor.
    *
    * Leaves a pointer to the created object on the cleanupstack.
    * 
    * @return instance
    */
    IMPORT_C static CPbkxRclSettingsEngine* NewLC();
        
    /**
    * Destructor.
    */
    virtual ~CPbkxRclSettingsEngine();

private: // from MPbkxRemoteContactLookupSettingsUi        

    /**
    * Creates new protocol account selector setting item.
    *
    * @return Created setting item.
    */
    CAknSettingItem* NewDefaultProtocolAccountSelectorSettingItemL() const;

private: // constructors

    /**
    * Constructor.
    */
    CPbkxRclSettingsEngine();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

    };

#endif // CPBKXRCLSETTINGSENGINE_H
