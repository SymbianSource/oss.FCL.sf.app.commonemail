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
* Description:  MRUI field build plugin extension proxy implementation
*
*/

#include "emailtrace.h"
#include "CESMRFieldBuilderPluginExtension.h"
#include <e32std.h>
#include <ecom/implementationproxy.h>

// -----------------------------------------------------------------------------
// TImplementationProxy
// Ecom implementation table
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KESMRUIFieldBuilderPluginExtensionImplUId,
                                CESMRFieldBuilderPluginExtension::NewL )
    };

// -----------------------------------------------------------------------------
// TImplementationProxy
// This should be only exported function of ECom plugin DLL
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    FUNC_LOG;
    aTableCount = sizeof( ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

