/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class TFscContactActionMenuTimedValue.
*
*/


// INCUDES
#include "emailtrace.h"
#include "tfsccontactactionmenutimedvalue.h"

// CONSTS
const TReal KMilliSecondsInSecond = 1000.0;

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::TFscContactActionMenuTimedValue
// ---------------------------------------------------------------------------
//
TFscContactActionMenuTimedValue::TFscContactActionMenuTimedValue(
    TReal32 aInitialValue ) : iStartValue( aInitialValue )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::TFscContactActionMenuTimedValue
// ---------------------------------------------------------------------------
//
void TFscContactActionMenuTimedValue::Set( 
    TReal32 aTargetValue, TInt aTimeMs )
    {
    FUNC_LOG;
    if( aTimeMs <= 0 )
        {
        iInternalState &= ~EFlagInterpolating;
        iInternalState |= EFlagChanged;
        iStartValue = aTargetValue;
        iTargetValue = aTargetValue;
        return;
        }
        
    if( iInternalState & EFlagInterpolating )
        {
        iStartValue = Now();
        }
    else
        {
        if( iStartValue == aTargetValue )
            {
            // This is the current value of the timed value, and it won't
            // change, so we are already at the target.
            return;
            }
        }        

    // If the difference between the current value and the target value
    // is insignificant, no interpolation is done (it would be subject
    // to round-off problems).
    const TReal32 KEpsilon = .0001;
    TReal32 delta = aTargetValue - iStartValue;
    if( Abs(delta) < KEpsilon )
        {
        iStartValue = aTargetValue;
        iTargetValue = aTargetValue;
        iInternalState &= ~EFlagInterpolating;
        iInternalState |= EFlagChanged;
        return;
        }

    iStartTime = Time();
    
    // Target time is an offset to the current time.
    iEndTime = iStartTime + aTimeMs;
    iTargetValue = aTargetValue;
    iInternalState |= EFlagChanged | EFlagInterpolating;

    }

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::SetWithSpeed
// ---------------------------------------------------------------------------
//
 void TFscContactActionMenuTimedValue::SetWithSpeed( 
    TReal32 aTargetValue, TInt aUnitsPerSecond )
    {
    FUNC_LOG;
    TReal32 delta = Now() - aTargetValue;
    if( delta == 0 || aUnitsPerSecond <= 0 )
        {
        // Already there.
        Set( aTargetValue );
        return;
        }
    
    delta = Abs(delta);
    Set( aTargetValue, ( TInt ) ( ( delta / aUnitsPerSecond ) * KMilliSecondsInSecond ) );
    }

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::Now
// ---------------------------------------------------------------------------
//
 TReal TFscContactActionMenuTimedValue::Now()
    {
    FUNC_LOG;
    
    if( ( iInternalState & EFlagInterpolating ) && iEndTime > iStartTime )
        {
        iInternalState |= EFlagChanged;
        
        TInt duration = iEndTime - iStartTime; // Convert from TInt64 -> TInt
        TInt elapsed = Time() - iStartTime; // Convert from TInt64 -> TInt
        
        if( elapsed > duration )
            {
            // We have reached the destination.
            iStartValue = iTargetValue;
            iStartTime = iEndTime;
            iInternalState &= ~EFlagInterpolating;
            return iStartValue;
            }
        
        const TInt fixt = FixDiv( elapsed, duration );
        TReal32 t = FixToFloat( fixt );
        TReal result = ( iStartValue * ( 1 - t ) + iTargetValue * t );
        return result;
        }
    else
        {
        if( iInternalState & EFlagInterpolating )
            {
            iInternalState |= EFlagChanged;
            iInternalState &= ~EFlagInterpolating;
            }
        return iStartValue;
        }

    }

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::Target
// ---------------------------------------------------------------------------
//
TReal TFscContactActionMenuTimedValue::Target()
    {
    FUNC_LOG;
    return iTargetValue;
    }

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::Interpolating
// ---------------------------------------------------------------------------
//
TBool TFscContactActionMenuTimedValue::Interpolating()
    {
    FUNC_LOG;
    return ( ( iInternalState & EFlagInterpolating ) != 0 );
    }

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::FixDiv
// ---------------------------------------------------------------------------
//
TInt TFscContactActionMenuTimedValue::FixDiv( TInt aVal1, TInt aVal2 )
    {
    FUNC_LOG;
    TUint64 temp = aVal1;
    temp <<= 16;
    TInt result = 0;
    if ( aVal2 != 0 )
        {
        result = TInt( temp / aVal2 );
        }
    return result;
    };

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::FixToFloat
// ---------------------------------------------------------------------------
//
TReal32 TFscContactActionMenuTimedValue::FixToFloat( TInt aVal )
    {
    FUNC_LOG;
    TReal result = TReal32( ( TReal32( aVal ) ) / 65536.0f );
    return result;
    }

// ---------------------------------------------------------------------------
// TFscContactActionMenuTimedValue::Time
// ---------------------------------------------------------------------------
//
TInt64 TFscContactActionMenuTimedValue::Time() const
    {
    FUNC_LOG;
    TTime curTime;
    curTime.UniversalTime();
    TTimeIntervalMicroSeconds interval = 
        curTime.MicroSecondsFrom( TTime( 0 ) );
        
    return interval.Int64() / KMilliSecondsInSecond;
    }

