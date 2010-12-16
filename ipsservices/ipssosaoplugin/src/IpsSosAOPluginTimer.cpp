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
* Description: 
*     Timer for AO Plugin
*
*/


#include "emailtrace.h"
#include "IpsSosAOPluginTimer.h"

//<cmail> //internal dependency removed
/*#ifdef _DEBUG
#include "../../internal/IpsSosAOPluginTester/inc/IpsSosAOPluginTester.hrh"
#endif // _DEBUG*/
//</cmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOPluginTimer* CIpsSosAOPluginTimer::NewL( 
    TInt aPriority, 
    MIpsSosAOPluginTimerCallBack& aCallBack )
    {
    FUNC_LOG;
    CIpsSosAOPluginTimer* self = new( 
        ELeave ) CIpsSosAOPluginTimer( aPriority, aCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOPluginTimer::CIpsSosAOPluginTimer( 
    TInt aPriority, 
    MIpsSosAOPluginTimerCallBack& aCallBack )
    : CActive (aPriority), iCallBack( aCallBack )
    {
    FUNC_LOG;
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOPluginTimer::~CIpsSosAOPluginTimer()
    {
    FUNC_LOG;
    Cancel();
    iTimer.Close();
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPluginTimer::ConstructL()
    {
    FUNC_LOG;
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add(this);
    }



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPluginTimer::After(const TTimeIntervalSeconds& aSeconds)
    {
    FUNC_LOG;
    Cancel();
    TTime time;
    time.HomeTime();
    time = time + aSeconds;
    iTimer.At( iStatus, time );
    SetActive();
    INFO_1( "CIpsSosAOPluginTimer::After: next trick: %d",  aSeconds.Int());
//<cmail> removing flags
/*#ifdef IPSAOPLUGINTESTER_MBOXLOGICTEST   
    iTestActivationTime = time;
#endif // IPSAOPLUGINTESTER_MBOXLOGICTEST*/
//</cmail>    
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPluginTimer::DoCancel()
    {
    FUNC_LOG;
    iTimer.Cancel();
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPluginTimer::RunL()
    {
    FUNC_LOG;
    iCallBack.TimerFiredL();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOPluginTimer::RunError( TInt /* aError */ )
    {
    FUNC_LOG;
    // Just ignore any error and continue without
    // any handling to allow smooth execution. 
    return KErrNone;            
    }

//<cmail> removing flags
/*#ifdef IPSAOPLUGINTESTER_MBOXLOGICTEST
// ----------------------------------------------------------------------------
// TEST USAGE
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOPluginTimer::GetSecondsToTrick()
    {
    if ( !IsActive() )
        {
        return KErrNotFound;
        }
    
    TTime now;
    now.HomeTime();
    TTimeIntervalSeconds secs;
    iTestActivationTime.SecondsFrom( now, secs );
    return secs.Int();
    }
#endif // IPSAOPLUGINTESTER_MBOXLOGICTEST*/
//</cmail>    

//EOF

