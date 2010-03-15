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
* Description:  plugin request handler
*
*/


#include "emailtrace.h"
#include "cfsclientapirequesthandler.h"
#include "cfsmailrequestobserver.h"
#include "emailclientpluginmanager.h"

// ================= MEMBER FUNCTIONS ==========================================	
// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CFSMailRequestHandler
// -----------------------------------------------------------------------------
CFSClientAPIRequestHandler::CFSClientAPIRequestHandler(  
    MEmailClientPluginManager* aPluginManager ) : iPluginManager(aPluginManager), iInstanceCounter( 1 )
{
    FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::~CFSMailRequestHandler
// -----------------------------------------------------------------------------
CFSClientAPIRequestHandler::~CFSClientAPIRequestHandler()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetInstanceL
// -----------------------------------------------------------------------------
CFSClientAPIRequestHandler* CFSClientAPIRequestHandler::GetInstanceL( MEmailClientPluginManager* aPluginManager )
    {
    CFSClientAPIRequestHandler *requestHandler = static_cast<CFSClientAPIRequestHandler*>( Dll::Tls() );
    if ( requestHandler == NULL )
        {
        requestHandler = CFSClientAPIRequestHandler::NewL( aPluginManager );
        }
    else
        {
        requestHandler->iInstanceCounter++;
        }
	
    return requestHandler;
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::ReleaseInstance
// -----------------------------------------------------------------------------
void CFSClientAPIRequestHandler::ReleaseInstance()
    {
    if( iInstanceCounter > 1 )
        {
        iInstanceCounter--;
        }
    else
        {
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::NewL
// -----------------------------------------------------------------------------
CFSClientAPIRequestHandler* CFSClientAPIRequestHandler::NewL( 
    MEmailClientPluginManager* aPluginManager)
{
    FUNC_LOG;
    CFSClientAPIRequestHandler* requestHandler = 
        new (ELeave) CFSClientAPIRequestHandler( aPluginManager );
    CleanupStack:: PushL(requestHandler);
    requestHandler->ConstructL( );
    CleanupStack:: Pop(requestHandler);
    return requestHandler;
} 


// -----------------------------------------------------------------------------
// CFSMailRequestHandler::ConstructL
// -----------------------------------------------------------------------------
void CFSClientAPIRequestHandler::ConstructL( )
{
    FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetPluginByUid
// -----------------------------------------------------------------------------
CFSMailPlugin* CFSClientAPIRequestHandler::GetPluginByUid(
    TFSMailMsgId aObjectId )
	{
    FUNC_LOG;
    return iPluginManager->GetPluginByUid(aObjectId.PluginId());
	}	
