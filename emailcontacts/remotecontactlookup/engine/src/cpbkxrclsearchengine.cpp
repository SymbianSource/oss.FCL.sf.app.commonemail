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
* Description:  Implementation of the class CPbkxRclSearchEngine.
*
*/


#include "emailtrace.h"
#include "cpbkxrclsearchengine.h"
#include "cpbkxrclprotocolenvimpl.h"
#include "cpbkxremotecontactlookupprotocoladapter.h"
#include "cpbkxrclserviceuicontextimpl.h"
#include "pbkxrclutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::CPbkxRclSearchEngine
// ---------------------------------------------------------------------------
//
CPbkxRclSearchEngine::CPbkxRclSearchEngine()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchEngine::ConstructL()
    {
    FUNC_LOG;
    iEnvironment = CPbkxRclProtocolEnvImpl::NewL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbkxRclSearchEngine* CPbkxRclSearchEngine::NewL()
    {
    FUNC_LOG;
    CPbkxRclSearchEngine* self = CPbkxRclSearchEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CPbkxRclSearchEngine* CPbkxRclSearchEngine::NewLC()
    {
    FUNC_LOG;
    CPbkxRclSearchEngine* self = new( ELeave ) CPbkxRclSearchEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::~CPbkxRclSearchEngine
// ---------------------------------------------------------------------------
//
CPbkxRclSearchEngine::~CPbkxRclSearchEngine()
    {
    FUNC_LOG;
    delete iEnvironment;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::DefaultAccountIdL
// ---------------------------------------------------------------------------
//
TPbkxRemoteContactLookupProtocolAccountId CPbkxRclSearchEngine::DefaultAccountIdL() const
    {
    FUNC_LOG;
    return PbkxRclUtils::DefaultProtocolAccountIdL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::NewAccountL
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupProtocolAccount* CPbkxRclSearchEngine::NewAccountL(
    const TPbkxRemoteContactLookupProtocolAccountId& aId ) const
    {
    FUNC_LOG;
    CPbkxRemoteContactLookupProtocolAdapter* adapter = CreateAdapterLC( aId );
    CPbkxRemoteContactLookupProtocolAccount* account = adapter->NewProtocolAccountL( aId );
    CleanupStack::PopAndDestroy( adapter );
    return account;
    }
       
// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::NewContextL
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupServiceUiContext* CPbkxRclSearchEngine::NewContextL( 
    TContextParams& aParams ) const
    {
    FUNC_LOG;
    CPbkxRclServiceUiContextImpl* context = CPbkxRclServiceUiContextImpl::NewL(
        aParams.iProtocolAccountId,
        aParams.iMode );
    return context;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchEngine::CreateAdapterLC
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupProtocolAdapter* CPbkxRclSearchEngine::CreateAdapterLC(
    const TPbkxRemoteContactLookupProtocolAccountId& aId ) const
    {
    FUNC_LOG;
    CPbkxRemoteContactLookupProtocolAdapter* adapter =
        CPbkxRemoteContactLookupProtocolAdapter::NewL( 
            aId.iProtocolUid, 
            *iEnvironment );
    CleanupStack::PushL( adapter );
    return adapter;
    }

