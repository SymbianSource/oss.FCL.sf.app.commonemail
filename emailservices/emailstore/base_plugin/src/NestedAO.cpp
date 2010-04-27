/*
* ==============================================================================
*  Name        : NestedAO.cpp
*  Part of     : Base Plugin
*  Description : Implementation of a nested active object used to handle deletion
*                of large number of messages by breaking them into smaller chunks
*  Version     : 
*
*  Copyright (c) 2005 - 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/
//  Include Files
//


#include "NestedAO.h"
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
