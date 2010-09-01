/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Simple wait operation for async functions
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsPlgOperationWait* CIpsPlgOperationWait::NewL( 
    TInt aPriority )
    {
    FUNC_LOG;
    CIpsPlgOperationWait* self = CIpsPlgOperationWait::NewLC( aPriority );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsPlgOperationWait* CIpsPlgOperationWait::NewLC( 
    TInt aPriority )
    {
    FUNC_LOG;
    CIpsPlgOperationWait* self = new( ELeave ) CIpsPlgOperationWait( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsPlgOperationWait::CIpsPlgOperationWait( TInt aPriority ) : CActive( aPriority )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsPlgOperationWait::~CIpsPlgOperationWait()
    {
    FUNC_LOG;
    Cancel();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgOperationWait::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgOperationWait::Start( )
    {
    FUNC_LOG;
    SetActive();
    iWait.Start();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgOperationWait::DoCancel()
    {
    FUNC_LOG;
    if( iStatus == KRequestPending )
        {
        TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrCancel );
        }
    StopScheduler();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgOperationWait::RunL()
    {
    FUNC_LOG;
    StopScheduler();
    }

void CIpsPlgOperationWait::StopScheduler()
    {
    FUNC_LOG;
    if ( iWait.IsStarted() )
        {
        /*if (iWait.CanStopNow())
            {*/
        iWait.AsyncStop();
        /*    }
        else
            {
            iWait.AsyncStop(TCallBack(SchedulerStoppedCallBack, env));
            }*/
        }
    }

