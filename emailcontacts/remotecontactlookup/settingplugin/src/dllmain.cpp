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
* Description:  Standard Symbian OS DLL entry point function.
*
*/


#include "emailtrace.h"
#include "cfscrclsettingextensionplugin.h"
#include "fscrclsettingextensionuid.h"
// System includes
#include <ecom/implementationproxy.h>

/// Implementation table
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KFscRclSettingExtensionImplementationUID,
        CFscRclSettingExtensionPlugin::NewL )
    };

// --------------------------------------------------------------------------
// ImplementationGroupProxy
// The one and only exported function that is the ECom entry point.
// --------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

// End of File

