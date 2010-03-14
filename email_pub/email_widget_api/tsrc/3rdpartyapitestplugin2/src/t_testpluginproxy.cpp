/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  3rd party api testplugin proxy
*
*/

#include <ecom/implementationproxy.h>

#include "t_testpluginimpl.h"
#include "t_testpluginconst.hrh"

// ---------------------------------------------------------
// Pairs ECom implementation UIDs with a pointer to the instantiation
// method for that implementation. Required for all ECom implementation
// collections.
// ---------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    { {KTestPlugin2ImplUid}, reinterpret_cast<TProxyNewLPtr>( CEmailObserverPluginImpl::NewL ) }
    };

// ---------------------------------------------------------
// Returns an instance of the proxy table.
// Returns: KErrNone
// ---------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
