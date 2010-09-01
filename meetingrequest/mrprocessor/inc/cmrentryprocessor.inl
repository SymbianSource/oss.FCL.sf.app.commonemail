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
#include "esmrinternaluid.h"
// -----------------------------------------------------------------------------
// CMREntryProcessor::NewL
// -----------------------------------------------------------------------------
//
inline CMREntryProcessor* CMREntryProcessor::NewL(
        TESMRCalendarEventType aEventType,
        MESMRCalDbMgr& aCalDb )
    {
    TAny* param = static_cast<TAny*>( &aCalDb );
    
    TAny* ptr = NULL;
    
    switch (aEventType )
        {
        case EESMREventTypeMeetingRequest:
            {
            TUid uid = TUid::Uid( KMRMeetingRequestProcessorImplementationUID );
            ptr = REComSession::CreateImplementationL(
                        uid,
                        _FOFF(CMREntryProcessor, iDtor_ID_Key ),
                        param );            
            }
            break;

        case EESMREventTypeAppt:
        case EESMREventTypeETodo:
        case EESMREventTypeEEvent:
        case EESMREventTypeEReminder:
        case EESMREventTypeEAnniv: 
            {
            TUid uid = TUid::Uid( KMRCalEventProcessorImplementationUID );
            ptr = REComSession::CreateImplementationL(
                        uid,
                        _FOFF(CMREntryProcessor, iDtor_ID_Key ),
                        param );             
            }
            break;
        }
    
    ASSERT( ptr );
    
    return reinterpret_cast<CMREntryProcessor*>(ptr);
    }

// -----------------------------------------------------------------------------
// CMRBCEventPlugin::~CMRBCEventPlugin
// -----------------------------------------------------------------------------
//
inline CMREntryProcessor::~CMREntryProcessor()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// EOF
