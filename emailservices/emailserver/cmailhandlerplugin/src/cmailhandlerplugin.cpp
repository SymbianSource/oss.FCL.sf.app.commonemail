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
* Description: Ecom proxy definition for cmailhandlerplugin.
*
*/
#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include "cmailcpshandler.h"
#include "fsmailmtmhandler.h"
#include "fsmailledhandler.h"
#include "fsmailiconhandler.h"
#include "fsmailsoundhandler.h"
#include "cmailhandlerpluginuids.h"
#include "fsmailoutofmemoryhandler.h"
#include "fsmailmessagequeryhandler.h"
#include "fsmailauthenticationhandler.h"

// ---------------------------------------------------------
// TImplementationProxy
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
#if !defined(FF_CMAIL_INTEGRATION)
    IMPLEMENTATION_PROXY_ENTRY( KCMailCpsHandlerUid, CMailCpsHandler::NewL ),
#endif // FF_CMAIL_INTEGRATION
    IMPLEMENTATION_PROXY_ENTRY( KMessageQueryHandlerUid, CFSMailMessageQueryHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KAuthenticationHandlerUid, CFSMailAuthenticationHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KOutofMemoryHandlerUid, CFSMailOutOfMemoryHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KMtmHandlerUid, CFSMailMtmHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KLedHandlerUid, CFSMailLedHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KSoundHandlerUid, CFSMailSoundHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KMailIconHandlerUid, CFSMailIconHandler::NewL ),
    };

// ----------------------------------------------------
// ImplementationGroupProxy
//
// ----------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    FUNC_LOG;
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

