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
* Description:  Implementation of the class CPbkxRemoteContactLookupEnv.
*
*/


#include <e32std.h>
#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// CPbkxRemoteContactLookupEnv::NewL
// -----------------------------------------------------------------------------
//
inline CPbkxRemoteContactLookupEnv* CPbkxRemoteContactLookupEnv::NewL()
    {
    CPbkxRemoteContactLookupEnv* self = NULL;
        
    TUid implUid = {KPbkxRemoteContactLookupServiceImplImpUid};

    TAny* interface = REComSession::CreateImplementationL (
        implUid, _FOFF( CPbkxRemoteContactLookupEnv, iDtor_ID_Key ) );

    return reinterpret_cast< CPbkxRemoteContactLookupEnv* >( interface );
    }
    
// -----------------------------------------------------------------------------
// CPbkxRemoteContactLookupEnv::~CPbkxRemoteContactLookupEnv
// -----------------------------------------------------------------------------
//
inline CPbkxRemoteContactLookupEnv::~CPbkxRemoteContactLookupEnv()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
