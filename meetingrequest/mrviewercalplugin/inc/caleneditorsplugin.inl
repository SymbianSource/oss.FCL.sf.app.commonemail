/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline methods for CCalenEditorsPlugin.
*
*/


#include "esmrinternaluid.h"

// ---------------------------------------------------------------------------
// CCalenEditorsPlugin::NewL
// ---------------------------------------------------------------------------
//
inline CCalenEditorsPlugin* CCalenEditorsPlugin::NewL()
    {
    const TUid KS60CalenEditorsImplementationId = { KS60CalenEditorsImplementation };
    const TUid KCalEntryUIImplmentationId = { KCalEntryUIImplmentation }; 

    TAny* interface = NULL;
    
    // Trying to create S60 editor component
    TRAPD( err, interface = REComSession::CreateImplementationL(
								KS60CalenEditorsImplementationId, 
								_FOFF(CCalenEditorsPlugin, iDtor_ID_Key)) );    
    
    if ( err != KErrNone && !interface )
    	{
    	// If S60 editor component cannot be created 
    	// --> Create FS calendar viewer component
    	interface = REComSession::CreateImplementationL(
    								KCalEntryUIImplmentationId, 
    								_FOFF( CCalenEditorsPlugin, iDtor_ID_Key) );    	
    	} 
    
    return reinterpret_cast<CCalenEditorsPlugin*>(interface);
    }

// ---------------------------------------------------------------------------
// CCalenEditorsPlugin::~CCalenEditorsPlugin
// ---------------------------------------------------------------------------
//
inline CCalenEditorsPlugin::~CCalenEditorsPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// EOF
