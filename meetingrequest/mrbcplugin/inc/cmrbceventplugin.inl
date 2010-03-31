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
* Description:  esmricalviewer inline methods 
*
*/

#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// CMRBCEventPlugin::NewL
// -----------------------------------------------------------------------------
//
inline CMRBCEventPlugin* CMRBCEventPlugin::NewL(
       TUid aUid,
       MCalenServices& aCalenServices )
    {
    TAny* param = static_cast<TAny*>( &aCalenServices );
    
    TAny* ptr = REComSession::CreateImplementationL(
            aUid,
            _FOFF(CMRBCEventPlugin, iDtor_ID_Key ),
            param );

    return reinterpret_cast<CMRBCEventPlugin*>(ptr);
    }

// -----------------------------------------------------------------------------
// CMRBCEventPlugin::~CMRBCEventPlugin
// -----------------------------------------------------------------------------
//
inline CMRBCEventPlugin::~CMRBCEventPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// EOF
