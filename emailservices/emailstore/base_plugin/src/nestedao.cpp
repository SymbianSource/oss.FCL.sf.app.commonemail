/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of a nested active object used to handle deletion
*                of large number of messages by breaking them into smaller chunks
*
*/

//  Include Files
//


#include "nestedao.h"
#include "baseplugindelayedops.h"
#include "baseplugindelayedopsprivate.h"

//Class CNestedAO

// --------------------------------------------------------------------------
//  CNestedAO::NewL
// ------------ 
CNestedAO* CNestedAO::NewL( MDeletionHandler& aDeletionHandler )
    {
    CNestedAO* self = new(ELeave) CNestedAO( aDeletionHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
// --------------------------------------------------------------------------
//  CNestedAO::CNestedAO
// ------------ 
CNestedAO::CNestedAO( MDeletionHandler& aDeletionHandler ) 
 : CTimer( EPriorityIdle ), iDeletionHandler( aDeletionHandler ), iRc( KErrNone )
    {
    }
// --------------------------------------------------------------------------
//  CNestedAO::ConstructL()
// ------------ 
void CNestedAO::ConstructL()
    {
    CTimer::ConstructL();
    iNestedWait = new( ELeave ) CActiveSchedulerWait();
    CActiveScheduler::Add( this );
    }
// --------------------------------------------------------------------------
//  CNestedAO::~CNestedAO
// ------------ 
CNestedAO::~CNestedAO()
    {
    Cancel();
    delete iNestedWait;
    }
// --------------------------------------------------------------------------
// CNestedAO::DeleteMessagesAsync
// Used to delete messages asynchronously 
// Deleting a small number of messages at a time
// ------------
void CNestedAO::DeleteMessagesAsync()
    {
    iDeletionIndex = 0;
    After( KWaitIntervalInMicroSecs );  //to invoke RunL after 100 microseconds
    iNestedWait->Start(); 
    }
// --------------------------------------------------------------------------
//  CNestedAO::DoCancel
// ------------ 
void CNestedAO::DoCancel()
    {
    iRc = KErrCancel;
    Stop();
    }
// --------------------------------------------------------------------------
//  CNestedAO::RunL
// ------------    
void CNestedAO::RunL()
    {
    iRc = iStatus.Int();
    if ( iStatus.Int() == KErrNone )
        {
        TBool done = iDeletionHandler.DeleteMessagesInChunksL( iDeletionIndex );
        if( !done )
        	{
        	//Increment deletionidex
        	iDeletionIndex+=KSizeOfChunk;
        	After( KWaitIntervalInMicroSecs ); 
        	}
        else
        	{
        	Stop();
        	}
		}
    else
        {
        Stop();
        }
    }

// --------------------------------------------------------------------------
// CNestedAO::Stop
// Stops the scheduler from running
// Called after all messages are processed
// or in error condition
// ------------    
void CNestedAO::Stop()
    {
    if( iNestedWait->IsStarted() )
        {
        // Stop the nested active scheduler (flow-of-control will pick up immediately following the
        // call to iWait.Start().
        iNestedWait->AsyncStop();
        } // end if
    }
