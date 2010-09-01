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
* Description:  Email setup plugin inline implementation
*
*/


// INCLUDES
#include "emailsetuppluginparams.h"

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CEmailSetupPluginInterface::NewL
// Two-phased constructor
// ----------------------------------------------------------------------------
//
inline CEmailSetupPluginInterface* CEmailSetupPluginInterface::NewL( 
                                            const TUid& aImplementationUid,
                                            TEmailSetupPluginParam& aParams )
    {
    // Find implementation for our interface.
    TAny* interface = 
            REComSession::CreateImplementationL( 
                            aImplementationUid,
                            _FOFF( CEmailSetupPluginInterface, iDtor_ID_Key ),
                            static_cast<TAny*>( &aParams ) );

    return reinterpret_cast<CEmailSetupPluginInterface*>( interface );
    }

inline CEmailSetupPluginInterface* CEmailSetupPluginInterface::NewL(
					    const TDesC8& aMatchString,
                                            TEmailSetupPluginParam& aParams )
	{
	TEComResolverParams resolverParams;
	resolverParams.SetDataType( aMatchString );


        TAny* interface = REComSession::CreateImplementationL( 
			KEmailSetupPluginIFUid, 
			_FOFF( CEmailSetupPluginInterface, iDtor_ID_Key ),
			static_cast<TAny*>( &aParams ), 
			resolverParams );

	return reinterpret_cast<CEmailSetupPluginInterface*>( interface );
	}


// ----------------------------------------------------------------------------
// CEmailSetupPluginInterface::~CEmailSetupPluginInterface
// Destructor
// ----------------------------------------------------------------------------
//
inline CEmailSetupPluginInterface::~CEmailSetupPluginInterface()
    {
    // If in the NewL some memory is reserved for member data, it must be
    // released here. This interface does not have any instance variables so
    // no need to delete anything.

    // Inform the E-com framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }


// End Of File
