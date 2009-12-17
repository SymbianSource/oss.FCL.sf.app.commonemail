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
* Description:  Implementation of class CFscContactActionService.
*
*/


#include <e32std.h>
#include <ecom/ecom.h>

//<cmail>
#include "fsccontactactionserviceuids.hrh"
//</cmail>

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
inline CFscContactActionService* CFscContactActionService::NewL(
    CVPbkContactManager& aContactManager )
    {
    TUid implUid = { KFscContactActionServiceImplImpUid };
    
    TFscContactActionServiceConstructParameters params( aContactManager );
    TAny* paramsPtr = reinterpret_cast< TAny* >( &params );

    TAny* interface = REComSession::CreateImplementationL(
        implUid, _FOFF( CFscContactActionService, iDtor_ID_Key ), paramsPtr );

    return reinterpret_cast< CFscContactActionService* >( interface );
    }
   

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
inline CFscContactActionService::~CFscContactActionService()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
