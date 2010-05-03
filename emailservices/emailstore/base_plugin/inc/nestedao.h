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
* Description:  Class used to implement nested active object used to handle 
* deletion of large number of messages by breaking them into smaller chunks
*
*/

#ifndef __NESTED_AO_H__
#define __NESTED_AO_H__

//  Include Files


#include <e32base.h>


class MDeletionHandler
    {
public:
    enum TState
        {
        EInProgress,
        EFree
        };
    //Deletes messages from aStartIndex KSizeOfChunk at a time
    virtual TBool DeleteMessagesInChunksL( TInt aStartIndex ) = 0;
    };
//Time interval between issuing 2 requests to delete messages
const TInt KWaitIntervalInMicroSecs=500;
//Maximum number of messages deleted in one go
const TInt KSizeOfChunk=10;

NONSHARABLE_CLASS ( CNestedAO ) : public CTimer
    {
    public:
        
        static CNestedAO* NewL( MDeletionHandler& aDeletionHandler );
        virtual ~CNestedAO();
        // --------------------------------------------------------------------------
        // CNestedAO::DeleteMessagesAsync
        // Used to delete messages asynchronously 
        // Deleting a small number of messages at a time
        // ------------
        void DeleteMessagesAsync();
        // --------------------------------------------------------------------------
        // CNestedAO::Rc
        // Used to return the iStatus return value
        // ------------        
        TInt Rc() { return iRc; }
        
    private:
        CNestedAO( MDeletionHandler& aDeletionHandler );
        void ConstructL();
        
        // inherited from CActive
        virtual void RunL();
        virtual void DoCancel();
        // --------------------------------------------------------------------------
        // CNestedAO::Stop
        // Stops the scheduler from running
        // Called after all messages are processed
        // or in error condition
        // ------------           
        void Stop();
        
    private:
        CActiveSchedulerWait* iNestedWait;
        MDeletionHandler& iDeletionHandler;
        TInt iDeletionIndex;
        TInt iRc;
    };


#endif  //__NESTED_AO_H__
