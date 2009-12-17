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
* Description:  Definition of class TFscContactActionMenuTimedValue.
*
*/


#ifndef T_FSCCONTACTACTIONMENUTIMEDVALUE_H
#define T_FSCCONTACTACTIONMENUTIMEDVALUE_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CGulIcon;

/**
 *  Contact Action Menu Timed Value implementation.
 *
 *  @since S60 3.1
 */
class TFscContactActionMenuTimedValue
    {

public: // Public constructor

    /**
     * Constructor.
     *
     * @param aInitialValue Initial value
     */
    TFscContactActionMenuTimedValue(
        TReal32 aInitialValue = 0 );

public: // Public methods from base class

    /**
     * Set new target
     *
     * @param aTargetValue Target value
     * @param aTimeMs Transition time
     */
    void Set( TReal32 aTargetValue, TInt aTimeMs = 0 );

    /**
     * Set new target with speed
     *
     * @param aTargetValue Target value
     * @param aUnitsPerSecond speed
     */
    void SetWithSpeed( TReal32 aTargetValue, TInt aUnitsPerSecond );

    /**
     * Get current value
     *
     * @return Current value
     */
    TReal Now();

    /**
     * Get target value
     *
     * @return Target value
     */
    TReal Target();

    /**
     * Is value still interpolating
     *
     * @return ETrue if value is interpolating
     */
    TBool Interpolating();

private:

    /**
     * Fixed div
     *
     * @param aVal1 First value
     * @param aVal2 Second value
     * @return result
     */
    TInt FixDiv( TInt aVal1, TInt aVal2 );

    /**
     * Fix to float conversion
     *
     * @param aVal Fix value
     * @return result
     */
    TReal32 FixToFloat( TInt aVal );

    /**
     * Time
     *
     * @return Current time in ms
     */
    TInt64 Time() const;
                 
private: // data

    enum TFlags
        {
        EFlagChanged = 0x0001,
        EFlagInterpolating = 0x0002
        };

    /**
     * Internal state
     */
    TInt iInternalState;

    /**
     * Start time
     *  Time when the current interpolation was started (milliseconds).
     */
    TInt64 iStartTime;
    
    /**
     * End time
     *  Time when the target value will be in effect in milliseconds. 
     */
    TInt64 iEndTime;
    
    /**
     * Start value
     *  Start value of current interpolation. 
     */
    TReal32 iStartValue;
    
    /**
     * Target value
     *  End value of current interpolation. 
     */
    TReal32 iTargetValue;

    };

#endif // T_FSCCONTACTACTIONMENUTIMEDVALUE_H

