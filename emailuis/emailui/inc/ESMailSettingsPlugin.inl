/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CESMailSettingsPlugin.
*
*/


// SYSTEM INCLUDES
#include <e32std.h>
#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// CESMailSettingsPlugin::NewL
// -----------------------------------------------------------------------------
//
inline CESMailSettingsPlugin* CESMailSettingsPlugin::NewL(
	const TUid& aImplementationUid )
    {
	TAny* interface = REComSession::CreateImplementationL(
			aImplementationUid, _FOFF( CESMailSettingsPlugin, iDtor_ID_Key ) );

	return reinterpret_cast< CESMailSettingsPlugin* >( interface );
    }
    
// -----------------------------------------------------------------------------
// CESMailSettingsPlugin::~CESMailSettingsPlugin
// -----------------------------------------------------------------------------
//
inline CESMailSettingsPlugin::~CESMailSettingsPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }