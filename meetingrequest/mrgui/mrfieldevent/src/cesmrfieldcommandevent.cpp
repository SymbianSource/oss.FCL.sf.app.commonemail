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
* Description:  Implementation of the class CESMRFieldCommandEvent
*
*/

#include "emailtrace.h"
#include "cesmrfieldcommandevent.h"

namespace { // codescanner::namespace

#ifdef _DEBUG

enum TPanic
    {
    EInvalidParamIndex = 0
    };

_LIT( KPanicCategory, "CESMRFieldCommandEvent" );

#endif //_DEBUG
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRFieldCommandEvent::CESMRFieldCommandEvent
// ---------------------------------------------------------------------------
//
CESMRFieldCommandEvent::CESMRFieldCommandEvent(
        MESMRFieldEventNotifier* aSource )
    : iSource( aSource)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRFieldCommandEvent::~CESMRFieldCommandEvent
// ---------------------------------------------------------------------------
//
CESMRFieldCommandEvent::~CESMRFieldCommandEvent()
    {
    FUNC_LOG;
    delete iCommand;
    }

// ---------------------------------------------------------------------------
// CESMRFieldCommandEvent::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFieldCommandEvent* CESMRFieldCommandEvent::NewL(
        MESMRFieldEventNotifier* aSource,
        TInt aCommand )
    {
    FUNC_LOG;
    CESMRFieldCommandEvent* self = CESMRFieldCommandEvent::NewLC( aSource,
                                                                  aCommand );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRFieldCommandEvent::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFieldCommandEvent* CESMRFieldCommandEvent::NewLC(
        MESMRFieldEventNotifier* aSource,
        TInt aCommand )
    {
    FUNC_LOG;
    CESMRFieldCommandEvent* self =
        new( ELeave ) CESMRFieldCommandEvent( aSource );
    self->ConstructL( aCommand );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRFieldCommandEvent::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRFieldCommandEvent::ConstructL( TInt aCommand )
    {
    FUNC_LOG;
    iCommand = new( ELeave ) TInt( aCommand );
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRFieldCommandEvent::Type
// Returns EESMRFieldCommandEvent
// ---------------------------------------------------------------------------
//
MESMRFieldEvent::TEventType CESMRFieldCommandEvent::Type() const
    {
    FUNC_LOG;
    return MESMRFieldEvent::EESMRFieldCommandEvent;
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRFieldCommandEvent::Source
// Returns event source
// ---------------------------------------------------------------------------
//
MESMRFieldEventNotifier* CESMRFieldCommandEvent::Source() const
    {
    FUNC_LOG;
    return iSource;
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRFieldCommandEvent::ParamCount
// Returns EESMRFieldCommandEvent
// ---------------------------------------------------------------------------
//
TInt CESMRFieldCommandEvent::ParamCount() const
    {
    FUNC_LOG;
    return 1;
    }
        
// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRFieldCommandEvent::Param
// Returns EESMRFieldCommandEvent
// ---------------------------------------------------------------------------
//
TAny* CESMRFieldCommandEvent::Param( TInt aIndex ) const
    {
    FUNC_LOG;
    if ( aIndex == 0 )
        {
        return const_cast< TInt* >( iCommand );
        }
    else
        {
        __ASSERT_DEBUG( EFalse, User::Panic( KPanicCategory,
                                             EInvalidParamIndex ) );
        }
    return NULL;
    }

