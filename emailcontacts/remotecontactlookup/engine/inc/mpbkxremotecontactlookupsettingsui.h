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
* Description:  Remote Contact Lookup Settings UI interface definition
*
*/


#ifndef MPBKXREMOTECONTACTLOOKUPSETTINGSUI_H
#define MPBKXREMOTECONTACTLOOKUPSETTINGSUI_H

#include <aknsettingitemlist.h> 
#include "cpbkxremotecontactlookupprotocolaccount.h"

/**
 *  Remote Contact Lookup Settings UI interface definition.
 *
 *  @since S60 3.1
 */
class MPbkxRemoteContactLookupSettingsUi 
    {

public:

    /**
     * Create a setting item for selecting the default account.
     * 
     * @return setting item
     */
    virtual CAknSettingItem* NewDefaultProtocolAccountSelectorSettingItemL() const = 0;
    };

#endif // MPBKXREMOTECONTACTLOOKUPSETTINGSUI_H
