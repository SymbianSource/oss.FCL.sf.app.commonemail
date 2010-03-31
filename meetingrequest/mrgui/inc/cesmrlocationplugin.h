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
* Description:  Interface definition for Location integration plug-in
*
*/


#ifndef CESMRLOCATIONPLUGIN_H
#define CESMRLOCATIONPLUGIN_H

#include <e32std.h>
#include <ecom/ecom.h>
#include "esmrinternaluid.h"
#include "mesmrlocationplugin.h"

class MESMRLocationPluginObserver;

/**
 *  CESMRLocationPlugin defines the interface for accessing location service provider
 *  from ESMR GUI.
 *
 */
class CESMRLocationPlugin : public CActive,
							public MESMRLocationPlugin
	{

public:

	/**
	 * Two-phased constructor
	 * 
	 * @param aObserver Observer for location plugin asynchronous events.
	 */
	inline static CESMRLocationPlugin* NewL();
	
	inline virtual ~CESMRLocationPlugin();

	virtual void SetObserver ( MESMRLocationPluginObserver* aObserver ) = 0;

protected:
	
	inline CESMRLocationPlugin( TInt aPriority );
	
private:	// data

	TUid iDtorIDKey;
    };

// INLINE FUNCTIONS

inline CESMRLocationPlugin* CESMRLocationPlugin::NewL()
	{
	TAny* impl = NULL;
	impl = REComSession::CreateImplementationL(
			TUid::Uid( KESMRLocationPluginImplementationUid ),
			_FOFF( CESMRLocationPlugin, iDtorIDKey ) );
	return reinterpret_cast< CESMRLocationPlugin* >( impl );
	}

inline CESMRLocationPlugin::CESMRLocationPlugin( TInt aPriority )
	: CActive( aPriority )
	{	
	}

inline CESMRLocationPlugin::~CESMRLocationPlugin()
	{
	REComSession::DestroyedImplementation( iDtorIDKey );
	}

#endif // CESMRLOCATIONPLUGIN_H
