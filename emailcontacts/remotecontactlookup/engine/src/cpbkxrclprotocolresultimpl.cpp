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
* Description:  Definition of the class CPbkxRclProtocolResultImpl.
*
*/


#include "emailtrace.h"
#include "cpbkxrclprotocolresultimpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolResultImpl* CPbkxRclProtocolResultImpl::NewL(
    TBool aIsComplete,
    CContactItem& aContactItem,
    const TDesC& aExtraProtocolData )
    {
    FUNC_LOG;
    CPbkxRclProtocolResultImpl* result = CPbkxRclProtocolResultImpl::NewLC(
        aIsComplete,
        aContactItem,
        aExtraProtocolData );
    CleanupStack::Pop( result );
    return result;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::NewLC
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolResultImpl* CPbkxRclProtocolResultImpl::NewLC(
    TBool aIsComplete,
    CContactItem& aContactItem,
    const TDesC& aExtraProtocolData )
    {
    FUNC_LOG;
    CPbkxRclProtocolResultImpl* result = new ( ELeave ) CPbkxRclProtocolResultImpl(
        aIsComplete,
        aContactItem );
    CleanupStack::PushL( result );
    result->ConstructL( aExtraProtocolData );
    return result;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::CPbkxRclProtocolResultImpl
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolResultImpl::CPbkxRclProtocolResultImpl(
    TBool aIsComplete,
    CContactItem& aContactItem ) : iIsComplete( aIsComplete ), iContactItem( &aContactItem )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::~CPbkxRclProtocolResultImpl
// ---------------------------------------------------------------------------
//
CPbkxRclProtocolResultImpl::~CPbkxRclProtocolResultImpl()
    {
    FUNC_LOG;
    delete iContactItem;
    iExtraProtocolData.Close();
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclProtocolResultImpl::ConstructL( const TDesC& aExtraProtocolData )
    {
    FUNC_LOG;
    iExtraProtocolData.CreateL( aExtraProtocolData );
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::IsComplete
// ---------------------------------------------------------------------------
//
TBool CPbkxRclProtocolResultImpl::IsComplete() const
    {
    FUNC_LOG;
    return iIsComplete;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::Complete
// ---------------------------------------------------------------------------
//
void CPbkxRclProtocolResultImpl::Complete()
    {
    FUNC_LOG;
    iIsComplete = ETrue;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::ExtraProtocolData
// ---------------------------------------------------------------------------
//
const TDesC& CPbkxRclProtocolResultImpl::ExtraProtocolData() const
    {
    FUNC_LOG;
    return iExtraProtocolData;
    }

// ---------------------------------------------------------------------------
// CPbkxRclProtocolResultImpl::ContactItem
// ---------------------------------------------------------------------------
//
CContactItem& CPbkxRclProtocolResultImpl::ContactItem()
    {
    FUNC_LOG;
    return *iContactItem;
    }

