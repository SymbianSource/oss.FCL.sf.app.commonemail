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
* Description:  Implementation of the class CESMRGenericFieldEvent
*
*/

#include "emailtrace.h"
#include "cesmrgenericfieldevent.h"

namespace { // codescanner::namespace

#ifdef _DEBUG

enum TPanic
    {
    EInvalidCast,
    };

_LIT( KPanicCategory, "CESMRFieldEventValue");

#endif

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRGenericFieldEvent::CESMRGenericFieldEvent
// ---------------------------------------------------------------------------
//
CESMRGenericFieldEvent::CESMRGenericFieldEvent(
        MESMRFieldEventNotifier* aSource,
        MESMRFieldEvent::TEventType aType )
    : iType( aType ),
      iSource( aSource )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRGenericFieldEvent::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRGenericFieldEvent*
    CESMRGenericFieldEvent::NewL( MESMRFieldEventNotifier* aSource,
                                  MESMRFieldEvent::TEventType aType )
    {
    FUNC_LOG;
    CESMRGenericFieldEvent* self = CESMRGenericFieldEvent::NewLC( aSource,
                                                                  aType );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRGenericFieldEvent::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRGenericFieldEvent*
    CESMRGenericFieldEvent::NewLC( MESMRFieldEventNotifier* aSource,
                                   MESMRFieldEvent::TEventType aType )
    {
    FUNC_LOG;
    CESMRGenericFieldEvent* self =
        new( ELeave ) CESMRGenericFieldEvent( aSource, aType );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRGenericFieldEvent::~CESMRGenericFieldEvent
// ---------------------------------------------------------------------------
//
CESMRGenericFieldEvent::~CESMRGenericFieldEvent()
    {
    FUNC_LOG;
    iParams.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// CESMRGenericFieldEvent::AddParamL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRGenericFieldEvent::AddParamL(
        CESMRFieldEventValue* aValue,
        TBool aEncapsulate)
    {
    FUNC_LOG;
    MESMRFieldEventValue* value = aValue;
    if ( aEncapsulate )
        {
        // Reserve space so next append will succeed
        iParams.ReserveL( iParams.Count() + 1 );
        value = CESMRFieldEventValue::NewL( // codescanner::forgottoputptroncleanupstack
                    CESMRFieldEventValue::EESMRFieldEventValue,
                    value );
        }
    iParams.AppendL( value );
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRGenericFieldEvent::Type
// ---------------------------------------------------------------------------
//
MESMRFieldEvent::TEventType CESMRGenericFieldEvent::Type() const
    {
    FUNC_LOG;
    return iType;
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRGenericFieldEvent::Source
// ---------------------------------------------------------------------------
//
MESMRFieldEventNotifier* CESMRGenericFieldEvent::Source() const
    {
    FUNC_LOG;
    return iSource;
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRGenericFieldEvent::ParamCount
// ---------------------------------------------------------------------------
//
TInt CESMRGenericFieldEvent::ParamCount() const
    {
    FUNC_LOG;
    return iParams.Count();
    }

// ---------------------------------------------------------------------------
// From class MESMRFieldEvent.
// CESMRGenericFieldEvent::Param
// ---------------------------------------------------------------------------
//
TAny* CESMRGenericFieldEvent::Param( TInt aIndex ) const
    {
    FUNC_LOG;
    return iParams[ aIndex ]->Value(); // codescanner::accessArrayElementWithoutCheck2
    }

// Class CESMRFieldEventValue

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::CESMRFieldEventValue
// ---------------------------------------------------------------------------
//
CESMRFieldEventValue::CESMRFieldEventValue(
        MESMRFieldEventValue::TValueType aType )
    : iType( aType )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRFieldEventValue::ConstructL( TAny* aValue )
    {
    FUNC_LOG;
    switch ( iType )
        {
        case EESMRTTime:
            {
            // Copy value
            TTime* time = static_cast< TTime* >( aValue );
            iValue = new( ELeave ) TTime( *time );
            break;
            }
        case EESMRInteger:
            {
            TInt* value = static_cast< TInt* >( aValue );
            iValue = new( ELeave ) TInt( *value );
            break;
            }
        default:
            {
            iValue = aValue;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFieldEventValue* CESMRFieldEventValue::NewL(
            MESMRFieldEventValue::TValueType aType,
            TAny* aValue )
    {
    FUNC_LOG;
    CESMRFieldEventValue* self = CESMRFieldEventValue::NewLC( aType, aValue );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFieldEventValue* CESMRFieldEventValue::NewLC(
            MESMRFieldEventValue::TValueType aType,
            TAny* aValue )
    {
    FUNC_LOG;
    CESMRFieldEventValue* self = new( ELeave ) CESMRFieldEventValue( aType );
    CleanupStack::PushL( self );
    self->ConstructL( aValue );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::~CESMRFieldEventValue
// ---------------------------------------------------------------------------
//

CESMRFieldEventValue::~CESMRFieldEventValue()
    {
    FUNC_LOG;
    // Delete owned values
    switch ( iType )
        {
        case EESMRString:
            {
            TDesC* value = static_cast< TDesC* >( iValue );
            delete value;
            break;
            }
        case EESMRString8:
            {
            TDesC8* value = static_cast< TDesC8* >( iValue );
            delete value;
            break;
            }
        case EESMRFieldEventValue:
        case EESMRCBase:
            {
            CBase* value = static_cast< CBase* >( iValue );
            delete value;
            break;
            }
        case EESMRTTime:
            {
            TTime* time = static_cast< TTime* >( iValue );
            delete time;
            break;
            }
        case EESMRInteger:
            {
            TInt* value = static_cast< TInt* >( iValue );
            delete value;
            break;            
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::Type
// ---------------------------------------------------------------------------
//
MESMRFieldEventValue::TValueType CESMRFieldEventValue::Type() const
    {
    FUNC_LOG;
    return iType;
    }
 
// ---------------------------------------------------------------------------
// CESMRFieldEventValue::Value
// ---------------------------------------------------------------------------
//

TAny* CESMRFieldEventValue::Value() const
    {
    FUNC_LOG;
    return iValue;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::IntValue
// ---------------------------------------------------------------------------
//
TInt CESMRFieldEventValue::IntValue() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iType == EESMRInteger,
                    User::Panic( KPanicCategory, EInvalidCast ) );
    TInt* value = static_cast< TInt* >( iValue );
    return *value;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::StringValue
// ---------------------------------------------------------------------------
//
const TDesC& CESMRFieldEventValue::StringValue() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( ( iType == EESMRString
                      || iType == EESMRStringRef ),
                    User::Panic( KPanicCategory, EInvalidCast ) );
    TDesC* string = static_cast< TDesC* >( iValue );
    return *string;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::String8Value
// ---------------------------------------------------------------------------
//
const TDesC8& CESMRFieldEventValue::String8Value() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( ( iType == EESMRString8
                      || iType == EESMRString8Ref ),
                    User::Panic( KPanicCategory, EInvalidCast ) );
    TDesC8* string = static_cast< TDesC8* >( iValue );
    return *string;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::FieldValue
// ---------------------------------------------------------------------------
//
const CESMRField& CESMRFieldEventValue::FieldValue() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iType == EESMRFieldRef,
                    User::Panic( KPanicCategory, EInvalidCast ) );
    CESMRField* field = static_cast< CESMRField* >( iValue );
    return *field;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::CBaseValue
// ---------------------------------------------------------------------------
//
const CBase* CESMRFieldEventValue::CBaseValue() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( ( iType == EESMRCBase
                      || iType == EESMRCBaseRef
                      || iType == EESMRFieldEventValue
                      || iType == EESMRFieldRef ),
                    User::Panic( KPanicCategory, EInvalidCast ) );
    CBase* base = static_cast< CBase* >( iValue );
    return base;
    }

// ---------------------------------------------------------------------------
// CESMRFieldEventValue::TimeValue
// ---------------------------------------------------------------------------
//
TTime CESMRFieldEventValue::TimeValue() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iType == EESMRTTime,
                    User::Panic( KPanicCategory, EInvalidCast ) );
    TTime* time = static_cast< TTime* >( iValue );
    return *time;
    }

