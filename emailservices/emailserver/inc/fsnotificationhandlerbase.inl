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
* Description: Inline methods for class CFSNotificationHandlerBase.
*
*/
#include <ecom/ecom.h>
#include "cmailhandlerpluginuids.h"

// ----------------------------------------------------
// CFSNotificationHandlerBase::NewL
// Create a new CPS Handler
// ----------------------------------------------------
//
inline CFSNotificationHandlerBase* CFSNotificationHandlerBase::NewL( TInt aHandlerUid,
                                                                     MFSNotificationHandlerMgr& aOwner )
    {
    TAny* ptr = NULL;
    TUid implementationUid = { aHandlerUid };
    ptr = REComSession::CreateImplementationL(
            implementationUid, _FOFF( CFSNotificationHandlerBase, iDestructorKey ), ( TAny* )&aOwner );
    return ptr != NULL ? reinterpret_cast<CFSNotificationHandlerBase*>( ptr ) : NULL;
    }
