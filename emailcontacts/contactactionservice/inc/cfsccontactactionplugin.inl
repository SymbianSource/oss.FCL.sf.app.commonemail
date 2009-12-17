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
* Description:  Implementation of class CFscContactActionPlugin.
*
*/


#include <e32std.h>
#include <ecom/ecom.h>

//<cmail>
#include "fsccontactactionserviceuids.hrh"
#include "tfsccontactactionpluginparams.h"
//</cmail>

// ---------------------------------------------------------------------------
// CFscContactActionPlugin::NewL
// ---------------------------------------------------------------------------
//
inline CFscContactActionPlugin* CFscContactActionPlugin::NewL(
    TUid aPluginUid,
    TFscContactActionPluginParams& aParams )
    {
    TAny* paramsPtr = reinterpret_cast< TAny* >( &aParams );
    TAny* interface = REComSession::CreateImplementationL( aPluginUid, 
            _FOFF( CFscContactActionPlugin, iDtor_ID_Key ), paramsPtr );

    return reinterpret_cast< CFscContactActionPlugin* >( interface );
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionPlugin::~CFscContactActionPlugin
// ---------------------------------------------------------------------------
//
inline CFscContactActionPlugin::~CFscContactActionPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
