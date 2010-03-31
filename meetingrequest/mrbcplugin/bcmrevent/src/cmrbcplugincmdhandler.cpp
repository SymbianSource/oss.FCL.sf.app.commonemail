/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing the RECAL customisation interface
*
*/

#include "cmrbcplugincmdhandler.h"
#include "cmrbcplugincreatenewmrcmd.h"
#include "cmrbcpluginopenmrviewercmd.h"
#include "cmrbcpluginresourcereader.h"
#include "cmrbcpluginentryloader.h"
#include "tmrbcplugincommand.h"
#include "cmrbcpluginnewcaleventcommand.h"

#include <calenservices.h>
#include <caleninterimutils2.h>
#include <calencommands.hrh>
#include <calenlauncher.h>
#include "emailtrace.h"

namespace { // codescanner::namespace

#ifdef _DEBUG

_LIT( KMRBCPluginCmdHandler, "MRBCPluginCmdHandler" );

enum TMRBCPluginCmdHandlerPanic
    {
    EInvalidCommand // Invalid command
    };

void Panic( TMRBCPluginCmdHandlerPanic aPanic )
    {    
    User::Panic( KMRBCPluginCmdHandler, aPanic );
    }

#endif // _DEBUG

CCalEntry::TType CommandToType(
        TInt aCommand )
    {
    CCalEntry::TType calEntryType( CCalEntry::EAppt );
    
    switch( aCommand )
        {
        case ECalenNewMeeting:
            {
            calEntryType = CCalEntry::EAppt;
            }
            break;

        case ECalenNewTodo:
            {
            calEntryType = CCalEntry::ETodo;
            }
            break;

        case ECalenNewAnniv:
            {
            calEntryType = CCalEntry::EAnniv;
            }
            break;
            
        case ECalenNewDayNote:
            {
            calEntryType = CCalEntry::EEvent;
            }
            break;            
        }
    
    return calEntryType;
    }

} // namespace

// -----------------------------------------------------------------------------
// CMRBCPluginCmdHandler::CMRBCPluginCmdHandler
// -----------------------------------------------------------------------------
//
CMRBCPluginCmdHandler::CMRBCPluginCmdHandler( 
        MCalenServices& aServices )
:   iServices( aServices )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginCmdHandler::~CMRBCPluginCmdHandler
// -----------------------------------------------------------------------------
//
CMRBCPluginCmdHandler::~CMRBCPluginCmdHandler()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginCmdHandler::NewL
// -----------------------------------------------------------------------------
//
CMRBCPluginCmdHandler* CMRBCPluginCmdHandler::NewL(
        MCalenServices& aServices )
    {
    FUNC_LOG;
    
    CMRBCPluginCmdHandler* self = 
            new (ELeave) CMRBCPluginCmdHandler(
                    aServices );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginCmdHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CMRBCPluginCmdHandler::ConstructL()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginCmdHandler::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CMRBCPluginCmdHandler::HandleCommandL( 
        const TCalenCommand& aCommand )
    {
    FUNC_LOG;
    
    MMRBCPluginCommand* command = NULL;
    TInt commandCode( aCommand.Command() );
    
    if ( ECalenNewMeetingTimeSpan == commandCode )
        {
        // command is triggered from day view bt using visual 
        // selection. We need to check whether MR is enabled or not.
        
        commandCode = ECalenNewMeeting;
        if ( iServices.InterimUtilsL().MRViewersEnabledL() )
            {
            commandCode = ECalenNewMeetingRequest;
            }        
        }    
    
    switch ( commandCode )
        {
        case ECalenNewMeetingRequest:            
            {
            command = CMRBCPluginCreateNewMRCmd::NewL( iServices );
            break;
            }
            
        case ECalenEventView:
            {
            command = CMRBCPluginOpenMRViewerCmd::NewL( iServices );
            break;
            }
            
        case ECalenNewMeeting:
        case ECalenNewTodo:
        case ECalenNewAnniv:
        case ECalenNewDayNote:
            {
            command = CMRPluginNewCalEntryCommand::NewL(
                    iServices,
                    CommandToType( aCommand.Command() ) );
            break;
            }
            
        default:
            {
            __ASSERT_DEBUG( command, Panic(EInvalidCommand) );
            ERROR( KErrArgument, "Invalid command" );
            User::Leave( KErrArgument );
            break;
            }
        }
    
    CleanupDeletePushL( command );
    command->ExecuteCommandL( aCommand );
    
    CleanupStack::PopAndDestroy( command ); // codescanner::cleanup
    
    // Returning EFalse means that command is not continued
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginCmdHandler::CalenCommandHandlerExtensionL
// -----------------------------------------------------------------------------
//
TAny* CMRBCPluginCmdHandler::CalenCommandHandlerExtensionL( 
        TUid /* aExtensionUid */ )
    {
    FUNC_LOG;
    return NULL;
    }

// EOF
