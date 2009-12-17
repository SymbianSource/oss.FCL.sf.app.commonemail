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
* Description:  Implementation of class CFscContactActionMenu.
*
*/


#include <e32std.h>
#include <ecom/ecom.h>

//<cmail>
#include "fsccontactactionmenuuids.hrh"
#include "tfsccontactactionmenuconstructparameters.h"
//</cmail>

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
inline CFscContactActionMenu* CFscContactActionMenu::NewL(
    CFscContactActionService& aService,
    TFscContactActionMenuMode aMode,
    TBool aOpenedFromMR )
    {
    TUid implUid = { KFscContactActionMenuImplImpUid };
    
    TFscContactActionMenuConstructParameters params( aService, aMode );
    TAny* paramsPtr = reinterpret_cast< TAny* >( &params );

    TAny* interface = REComSession::CreateImplementationL(
        implUid, _FOFF( CFscContactActionMenu, iDtor_ID_Key ), paramsPtr );

    CFscContactActionMenu* contactActionMenu = 
        reinterpret_cast< CFscContactActionMenu* >( interface );

    if ( contactActionMenu )
        {
        contactActionMenu->SetOpenedFromMR( aOpenedFromMR );
        }
        
    return contactActionMenu;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
inline CFscContactActionMenu::~CFscContactActionMenu()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
