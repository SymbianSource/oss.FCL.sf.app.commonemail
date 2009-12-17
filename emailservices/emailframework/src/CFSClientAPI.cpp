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
* Description:  implementation of CFSClientAPI
*
*/

#include "emailtrace.h"
#include "CFSClientAPIRequestHandler.h"
#include "CFSClientAPI.h"
#include "emailclientpluginmanager.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CFSClientAPI::CFSClientAPI()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CFSClientAPI::~CFSClientAPI()
    {
    FUNC_LOG;
    delete iAPIRequestHandler;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
EXPORT_C CFSClientAPI* CFSClientAPI::NewL(MEmailClientPluginManager* aPluginManager)
    {
    FUNC_LOG;
    CFSClientAPI* self = new (ELeave) CFSClientAPI();
    CleanupStack:: PushL(self);
    self->ConstructL(aPluginManager);
    CleanupStack:: Pop(self);
    return self;
    } 

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CFSClientAPI::ConstructL(MEmailClientPluginManager* aPluginManager)
    {
    FUNC_LOG;
    iAPIRequestHandler = CFSClientAPIRequestHandler::NewL(aPluginManager);
    }

