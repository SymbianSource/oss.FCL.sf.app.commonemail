/*
* ============================================================================
*  Name     : NestedAO.h
*  Part of  : Base Plugin
*
*  Description:
*       Class used to implement nested active object used to handle deletion
*       of large number of messages by breaking them into smaller chunks
*  Version:
*
*  Copyright (C) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing,  adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ============================================================================
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
