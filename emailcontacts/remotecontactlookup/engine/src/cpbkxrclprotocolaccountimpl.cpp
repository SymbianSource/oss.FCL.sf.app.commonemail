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
* Description:  Implementation of the class CPbkxRclProtocolAccountImpl.
*
*/


#include "emailtrace.h"
#include "cpbkxrclprotocolaccountimpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclProtocolAccountImpl::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolAccountImpl* CPbkxRclProtocolAccountImpl::NewL(
    TPbkxRemoteContactLookupProtocolAccountId aId,
    const TDesC& aName )
    {
    FUNC_LOG;
    CPbkxRclProtocolAccountImpl* account = CPbkxRclProtocolAccountImpl::NewLC( aId, aName );
    CleanupStack::Pop( account );
    return account;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolAccountImpl::NewLC
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolAccountImpl* CPbkxRclProtocolAccountImpl::NewLC(
    TPbkxRemoteContactLookupProtocolAccountId aId,
    const TDesC& aName )
    {
    FUNC_LOG;
    CPbkxRclProtocolAccountImpl* account = new ( ELeave ) CPbkxRclProtocolAccountImpl( aId );
    CleanupStack::PushL( account );
    account->ConstructL( aName );
    return account;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolAccountImpl::CPbkxRclProtocolAccountImpl
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolAccountImpl::CPbkxRclProtocolAccountImpl(
    TPbkxRemoteContactLookupProtocolAccountId aId ) : iId( aId )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolAccountImpl::~CPbkxRclProtocolAccountImpl
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolAccountImpl::~CPbkxRclProtocolAccountImpl()
    {
    FUNC_LOG;
    iName.Close();
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolAccountImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclProtocolAccountImpl::ConstructL( const TDesC& aName )
    {
    FUNC_LOG;
    iName.CreateL( aName );
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolAccountImpl::Id
// ---------------------------------------------------------------------------
//
TPbkxRemoteContactLookupProtocolAccountId CPbkxRclProtocolAccountImpl::Id() const
    {
    FUNC_LOG;
    return iId;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolAccountImpl::Name
// ---------------------------------------------------------------------------
//
const TDesC& CPbkxRclProtocolAccountImpl::Name() const
    {
    FUNC_LOG;
    return iName;
    }

