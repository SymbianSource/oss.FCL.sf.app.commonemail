/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MRUI field builder plugin proxy
*
*/


#include "emailtrace.h"
#include "CESMRFieldBuilderPlugin.h"
#include <e32std.h>
#include <ecom/implementationproxy.h>

// -----------------------------------------------------------------------------
// TImplementationProxy
// Ecom implementation table
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KESMRUIFieldBuilderPluginImplUId,
                                CESMRFieldBuilderPlugin::NewL )
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

