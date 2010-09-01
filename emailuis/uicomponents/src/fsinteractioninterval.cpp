/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CFsInteractionInterval is a utility class that can be used 
*                as specify intervals that adapt to user input.
*
*/


#include "emailtrace.h"
#include "fsinteractioninterval.h"  // Class definition


CFsInteractionInterval::CFsInteractionInterval(TReal32 aScalar)
        : iScalar(aScalar), iLastTimeMs(0)
    {
    FUNC_LOG;
    iLastTime.UniversalTime();
    }


void CFsInteractionInterval::SetScalar(TReal32 aScalar)
    {
    FUNC_LOG;
    iScalar = aScalar;
    }
    
    
TReal32 CFsInteractionInterval::Scalar() const
    {
    FUNC_LOG;
    return iScalar;
    }
    

TInt CFsInteractionInterval::Interval(TInt aIntervalTime)
    {
    FUNC_LOG;
    TTime now;
    
    now.UniversalTime();
    
    TInt64 delta;
    TUint32 elapsed(0);
    
    delta = now.MicroSecondsFrom( iLastTime ).Int64() / 1000;
    elapsed = delta;
    
    if(iScalar * elapsed < aIntervalTime)
        {
        // The last movement is probably still going on.
        aIntervalTime = TInt(elapsed * iScalar);
        }
    
    iLastTime = now;
    return aIntervalTime;
    /*
    TUint32 now = CHuiStatic::MilliSecondsSinceStart();
    TUint32 elapsed = now - iLastTimeMs;
    
    if(iScalar * elapsed < aIntervalTime)
        {
        // The last movement is probably still going on.
        aIntervalTime = TInt(elapsed * iScalar);
        }
        
    iLastTimeMs = now;
    
    return aIntervalTime;
    */
    }

