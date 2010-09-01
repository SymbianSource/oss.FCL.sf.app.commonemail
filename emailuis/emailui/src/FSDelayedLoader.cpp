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
* Description:  Helper to do delayed processing after application startup
*
*/


#include "emailtrace.h"
#include <eikenv.h>
#include"FSDelayedLoader.h"
#include"FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiContactHandler.h"


// -----------------------------------------------------------------------------
// CFsDelayedLoader::InstanceL
// 
// -----------------------------------------------------------------------------
//
CFsDelayedLoader* CFsDelayedLoader::InstanceL()
	{
    FUNC_LOG;
	CFsDelayedLoader* instance = static_cast<CFsDelayedLoader*>( CCoeEnv::Static( KDelayedLoaderUid ) );
	if ( !instance )
		{
	    instance = new ( ELeave ) CFsDelayedLoader;
	    CleanupStack::PushL( instance );
	    instance->ConstructL();
	    CleanupStack::Pop();
	  	}
    return instance;
	}

// -----------------------------------------------------------------------------
// CFsDelayedLoader::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CFsDelayedLoader::ConstructL()
	{
    FUNC_LOG;
	iIdle = CIdle::NewL( CActive::EPriorityIdle );
	iIdleCallback = new (ELeave) TCallBack( IdleCallbackFunction, this );
	}

// -----------------------------------------------------------------------------
// CFsDelayedLoader::~CFsDelayedLoader
// 
// -----------------------------------------------------------------------------
//
CFsDelayedLoader::~CFsDelayedLoader()
	{
    FUNC_LOG;
	iLoaders.Reset();

	delete iContactHandler;

	delete iIdleCallback;
	if ( iIdle )
	    {
    	iIdle->Cancel();
    	delete iIdle;
	    }
	}
	
// -----------------------------------------------------------------------------
// CFsDelayedLoader::CFsDelayedLoader
// 
// -----------------------------------------------------------------------------
//
CFsDelayedLoader::CFsDelayedLoader(): CCoeStatic( KDelayedLoaderUid )
	{
    FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CFsDelayedLoader::Start
// 
// -----------------------------------------------------------------------------
//
void CFsDelayedLoader::Start()
	{
    FUNC_LOG;
	iLoaderIndex = 0;
	iIdle->Start( *iIdleCallback );
	}

// -----------------------------------------------------------------------------
// CFsDelayedLoader::GetPbkContactEngineL
// 
// -----------------------------------------------------------------------------
//
CFSEmailUiContactHandler* CFsDelayedLoader::GetContactHandlerL()
	{
    FUNC_LOG;
	
	if ( !iContactHandler )
		{
		iContactHandler = CFSEmailUiContactHandler::NewL( CEikonEnv::Static()->FsSession() );
		}
	
	return iContactHandler;
	}
// -----------------------------------------------------------------------------
// CFsDelayedLoader::IdleCallbackFunction
// 
// -----------------------------------------------------------------------------
//
TInt CFsDelayedLoader::IdleCallbackFunction( TAny* aParam )
	{
    FUNC_LOG;
	// load own items
	CFsDelayedLoader* self = static_cast< CFsDelayedLoader* >( aParam );
	TRAPD( error, self->GetContactHandlerL() );
	if ( KErrNone != error )
	    {
	    //Panic!
	    }
	
	// if there are no loaders we can stop the callback
	if ( self->iLoaders.Count() <= 0 )
		{
		return 0;
		}
	
	// handle loaders
	TInt r = self->iLoaders[self->iLoaderIndex]->DelayLoadCallback();

	if ( r	== 0 )
		{
		self->iLoaderIndex++;
		if ( self->iLoaderIndex >= self->iLoaders.Count() )
			{
			return 0;
			}
		}
		
	return 1;
	}

// -----------------------------------------------------------------------------
// CFsDelayedLoader::AppendLoader
// 
// -----------------------------------------------------------------------------
//
void CFsDelayedLoader::AppendLoader( MFsDelayedLoadingInterface& aLoader )
	{
    FUNC_LOG;
	iLoaders.Append( &aLoader );
	}

// -----------------------------------------------------------------------------
// CFsDelayedLoader::RemoveLoader
// 
// -----------------------------------------------------------------------------
//
void CFsDelayedLoader::RemoveLoader( MFsDelayedLoadingInterface& aLoader )
	{
    FUNC_LOG;
	for ( TInt i=0; i<iLoaders.Count(); i++ )
	    {
	    if ( iLoaders[i] == &aLoader )
	        {
	        iLoaders.Remove( i );
	        return;
	        }
	    }
	}

