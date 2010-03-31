/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for MR policy resolver ECom interface
*
*/

#include <e32def.h>
#include <ecom/ecom.h>
#include "esmrinternaluid.h"

inline CMRPolicyResolver* CMRPolicyResolver::NewL( TESMRCalendarEventType aType )
    {
    TAny* impl = NULL;
    
    switch ( aType )
        {
        case EESMREventTypeAppt:
        case EESMREventTypeETodo:
        case EESMREventTypeEEvent:
        case EESMREventTypeEReminder:
        case EESMREventTypeEAnniv:
            {
            impl = REComSession::CreateImplementationL(
                    TUid::Uid( KMRCalEntryPolicyImplementationUID ), 
                    _FOFF( CMRPolicyResolver, iDtor_ID_Key ) );
            break;
            }
        case EESMREventTypeMeetingRequest:
            {
            impl = REComSession::CreateImplementationL(
                    TUid::Uid( KMREntryPolicyImplementationUID ), 
                    _FOFF( CMRPolicyResolver, iDtor_ID_Key ) );
            break;
            }
        default:
            {
            ASSERT( EFalse ); // ASSERT for debug builds
            User::Leave( KErrNotSupported );
            }
        }
    
    return static_cast< CMRPolicyResolver* >( impl );
    }

inline CMRPolicyResolver::~CMRPolicyResolver()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
