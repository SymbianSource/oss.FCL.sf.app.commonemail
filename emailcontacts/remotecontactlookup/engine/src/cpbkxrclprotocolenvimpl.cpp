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
* Description:  Implementation of the class CPbkxRclProtocolEnvImpl.
*
*/


#include "emailtrace.h"
#include "cpbkxrclprotocolenvimpl.h"
#include "cpbkxrclprotocolresultimpl.h"
#include "cpbkxrclprotocolaccountimpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclProtocolEnvImpl::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolEnvImpl* CPbkxRclProtocolEnvImpl::NewL()
    {
    FUNC_LOG;
    CPbkxRclProtocolEnvImpl* env = CPbkxRclProtocolEnvImpl::NewLC();
    CleanupStack::Pop( env );
    return env;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolEnvImpl::NewLC
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolEnvImpl* CPbkxRclProtocolEnvImpl::NewLC()
    {
    FUNC_LOG;
    CPbkxRclProtocolEnvImpl* env = new ( ELeave ) CPbkxRclProtocolEnvImpl();
    CleanupStack::PushL( env );
    env->ConstructL();
    return env;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolEnvImpl::CPbkxRclProtocolEnvImpl
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolEnvImpl::CPbkxRclProtocolEnvImpl() : CBase()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolEnvImpl::~CPbkxRclProtocolEnvImpl
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolEnvImpl::~CPbkxRclProtocolEnvImpl()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolEnvImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclProtocolEnvImpl::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolEnvImpl::NewProtocolAccountL
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupProtocolAccount* CPbkxRclProtocolEnvImpl::NewProtocolAccountL(
    TPbkxRemoteContactLookupProtocolAccountId aId,
    const TDesC& aName )
    {
    FUNC_LOG;
    CPbkxRclProtocolAccountImpl* account = CPbkxRclProtocolAccountImpl::NewL( aId, aName );
    return account;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolEnvImpl::NewProtocolResultL
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupProtocolResult* CPbkxRclProtocolEnvImpl::NewProtocolResultL(
    TBool aIsComplete,
    CContactItem& aContactItem,
    const TDesC& aExtraProtocolData )
    {
    FUNC_LOG;
    CPbkxRclProtocolResultImpl* result = CPbkxRclProtocolResultImpl::NewL(
        aIsComplete,
        aContactItem,
        aExtraProtocolData );
    return result;
    }

