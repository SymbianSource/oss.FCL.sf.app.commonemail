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
* Description:  Implementation of the class CPbkxRemoteContactLookupProtocolAdapter
*
*/


// -----------------------------------------------------------------------------
// CPbkxRemoteContactLookupProtocolAdapter::NewL
// -----------------------------------------------------------------------------
//
inline CPbkxRemoteContactLookupProtocolAdapter* CPbkxRemoteContactLookupProtocolAdapter::NewL(
    TUid aProtocolUid,
    MPbkxRemoteContactLookupProtocolEnv& aProtocolEnv )
    {
    CPbkxRemoteContactLookupProtocolAdapter* self = NULL;
        
	TAny* interface = REComSession::CreateImplementationL (
		aProtocolUid, 
		_FOFF( CPbkxRemoteContactLookupProtocolAdapter, iDtor_ID_Key ),
		&aProtocolEnv );

	return reinterpret_cast< CPbkxRemoteContactLookupProtocolAdapter* >( interface );
    }
    
// -----------------------------------------------------------------------------
// CPbkxRemoteContactLookupProtocolAdapter::~CPbkxRemoteContactLookupProtocolAdapter
// -----------------------------------------------------------------------------
//
inline CPbkxRemoteContactLookupProtocolAdapter::~CPbkxRemoteContactLookupProtocolAdapter()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
