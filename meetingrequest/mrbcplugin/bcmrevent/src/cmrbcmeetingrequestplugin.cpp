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
* Description: MBUtils ECOM definition
*
*/
#include "cmrbcmeetingrequestplugin.h"
#include "cmrbcplugincmdhandler.h"
#include "emailtrace.h"

namespace { // codescanner::namespace

} // namespace

// -----------------------------------------------------------------------------
// CMRBCMeetingRequestPlugin::CMRBCMeetingRequestPlugin
// -----------------------------------------------------------------------------
//
CMRBCMeetingRequestPlugin::CMRBCMeetingRequestPlugin(
        MCalenServices& aCalenServices )
:   iCalenServices( aCalenServices )
    {
    
    }

// -----------------------------------------------------------------------------
// CMRBCMeetingRequestPlugin::~CMRBCMeetingRequestPlugin
// -----------------------------------------------------------------------------
//
CMRBCMeetingRequestPlugin::~CMRBCMeetingRequestPlugin()
    {
    FUNC_LOG;
    delete iCommandHandler;
    }
        
// -----------------------------------------------------------------------------
// CMRBCMeetingRequestPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CMRBCMeetingRequestPlugin::ConstructL()
    {
    iCommandHandler = CMRBCPluginCmdHandler::NewL( iCalenServices );
    }

// -----------------------------------------------------------------------------
// CMRBCMeetingRequestPlugin::NewL
// -----------------------------------------------------------------------------
//
CMRBCMeetingRequestPlugin* CMRBCMeetingRequestPlugin::NewL(
        TAny* aCalenServices )
    {
    FUNC_LOG;
    
    MCalenServices* calenServices = 
        static_cast<MCalenServices*>( aCalenServices );
        
    CMRBCMeetingRequestPlugin* self = 
            new (ELeave) CMRBCMeetingRequestPlugin( *calenServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMRBCMeetingRequestPlugin::CommandHandler
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CMRBCMeetingRequestPlugin::CommandHandler()
    {
    FUNC_LOG;
    
    return iCommandHandler;
    }

// EOF
