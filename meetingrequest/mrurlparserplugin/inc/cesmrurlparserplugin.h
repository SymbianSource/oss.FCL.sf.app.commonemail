/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for Location URL Parser plug-in
*
*/


#ifndef CESMRURLPARSERPLUGIN_H
#define CESMRURLPARSERPLUGIN_H

#include <e32std.h>
#include <ecom/ecom.h>
#include "mruiuids.h"
#include "mesmrurlparserplugin.h"



/**
 *  CESMRUrlParserPlugin defines the interface for Location URL Parser.
 *
 */
class CESMRUrlParserPlugin : public CBase,
                             public MESMRUrlParserPlugin
    {

public:

    /**
     * Two-phased constructor
     * 
     */
    inline static CESMRUrlParserPlugin* NewL();
    
    inline virtual ~CESMRUrlParserPlugin();

protected:
    
    inline CESMRUrlParserPlugin();
    
private:    // data

    TUid iDtorIDKey;
    };

// INLINE FUNCTIONS

inline CESMRUrlParserPlugin* CESMRUrlParserPlugin::NewL()
    {
    TAny* impl = NULL;
    impl = REComSession::CreateImplementationL(
            TUid::Uid( KESMRURLParserPluginImplementationUid ),
            _FOFF( CESMRUrlParserPlugin, iDtorIDKey ));
    return reinterpret_cast< CESMRUrlParserPlugin* >( impl );
    }

inline CESMRUrlParserPlugin::CESMRUrlParserPlugin()
    {   
    }

inline CESMRUrlParserPlugin::~CESMRUrlParserPlugin()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

#endif // CESMRURLPARSERPLUGIN_H



