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
* Description:  Interface definition for ESMR task factory implementation
*
*/

#include <ecom/ecom.h>
#include "esmrinternaluid.h"

inline CESMRTaskFactory* CESMRTaskFactory::NewL(
            TESMRCalendarEventType aType,
            MESMRCalDbMgr& aCalDbMgr )
    {
    CESMRTaskFactory* impl = NULL;
    
    switch ( aType )
       {
       case EESMREventTypeMeetingRequest:
           {
           impl = static_cast< CESMRTaskFactory* >( 
                   REComSession::CreateImplementationL(
                           TUid::Uid( KMRTaskPluginImplementationUID ), 
                           _FOFF( CESMRTaskFactory, iDtor_ID_Key ), 
                           &aCalDbMgr ) );
           break;
           }
       case EESMREventTypeAppt:
       case EESMREventTypeETodo:      
       case EESMREventTypeEEvent:
       case EESMREventTypeEReminder:
       case EESMREventTypeEAnniv:
           {
           impl = static_cast< CESMRTaskFactory* >( 
                   REComSession::CreateImplementationL(
                           TUid::Uid( KMRCalEventTaskPluginImplementationUID ), 
                           _FOFF(  CESMRTaskFactory, iDtor_ID_Key ), 
                           &aCalDbMgr ) );
           break;
           }
       default:
           {
           // never should come here.
           User::Leave( KErrGeneral );
           break;
           }
       }

    return impl;
    }

inline CESMRTaskFactory::~CESMRTaskFactory()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
