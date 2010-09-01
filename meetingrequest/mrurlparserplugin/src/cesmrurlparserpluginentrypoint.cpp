/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  URL Parser ECom plugin entry point
*
*/

#include "emailtrace.h"
#include <ecom/implementationproxy.h>

#include "cesmrurlparserpluginimpl.h"
#include "esmrinternaluid.h"



const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KESMRURLParserPluginImplementationUid, 
                                CESMRUrlParserPluginImpl::NewL )
    };

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Entry point function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    FUNC_LOG;
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

