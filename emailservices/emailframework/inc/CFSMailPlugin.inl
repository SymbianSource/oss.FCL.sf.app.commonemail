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
* Description: Inline methods for class CFSMailPlugin. 
*
*/


#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin::CFSMailPlugin()
    {

    }

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin::~CFSMailPlugin()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::Close
// -----------------------------------------------------------------------------
//
inline void CFSMailPlugin::Close()
    {
    REComSession::FinalClose();
    }
    
// -----------------------------------------------------------------------------
// CFSMailPlugin::ListImplementationsL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::ListImplementationsL(
    RPointerArray<CImplementationInformation>& aInfo)
    {
    REComSession::ListImplementationsL(KFSMailPluginInterface, aInfo);
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::NewL
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin* CFSMailPlugin::NewL(TUid aUid)
    {
    return static_cast<CFSMailPlugin*>( 
        REComSession::CreateImplementationL(
            aUid, _FOFF( CFSMailPlugin, iDtor_ID_Key ) ) );
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::GetConnectionId
// -----------------------------------------------------------------------------
inline TInt CFSMailPlugin::GetConnectionId(
    TFSMailMsgId /*aMailBoxId*/,
    TUint32& /*aConnectionId*/ )
    {
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CFSMailPlugin::IsConnectionAllowedWhenRoaming
// -----------------------------------------------------------------------------
inline TInt CFSMailPlugin::IsConnectionAllowedWhenRoaming(
	TFSMailMsgId /*aMailBoxId*/,
	TBool& /*aConnectionAllowed*/ )
    {
    return KErrNotSupported;
    }
    
// End of File
