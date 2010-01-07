/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  InitTestCase test module.
*
*/



#ifndef OBSERVER_TESTS_H
#define OBSERVER_TESTS_H

// INCLUDES
#include "AsyncTestCaseBase.h"
#include <msgstoreapi.h>

class CObserverTests : public CAsyncTestCaseBase, public MMsgStoreObserver, public MMsgStoreMailBoxObserver 
    {
    public:  // Constructors and destructor

        static CObserverTests* NewL( CStifLogger* aLog );
        
        virtual ~CObserverTests();

    private:
        class TObserverEvent
            {
            public:
                TInt    iMailBoxId;
                TInt    iOperation;
                TInt    iType;
                TInt    iId;
                TInt    iParentId;
                TInt    iOtherId;
                TUint32 iFlags;
                TBuf8<KMaxAccountNameLen>   iName;
                TBuf8<KMaxAccountNameLen>   iNewName;
            };
        
        void StartRecordingObserverEvents();
        
        void StopRecordingObserverEvents();
        
        void VerifyRecordedObserverEventsL( TBool aVerifyOrder, RArray<TObserverEvent>& aExpectedEvents );
        
        // inherited from MMsgStoreObserver
        virtual void ModificationNotify( TMsgStoreId aMailBoxId, 
                                         TMsgStoreOperation aOperation, 
                                         TMsgStoreContainerType aType, 
                                         TUint32 aFlags, 
                                         TMsgStoreId aId, 
                                         TMsgStoreId aParentId, 
                                         TMsgStoreId aOtherId );
        
        virtual void SystemEventNotify( TMsgStoreSystemEvent aEvent );
        
        virtual void AccountEventNotify( TMsgStoreAccountEvent aEvent, 
                                         TInt32 aOwnerId, 
                                         const TDesC& aName, 
                                         const TDesC& aNewName, 
                                         TMsgStoreId aMailboxId );
        
        CObserverTests( CStifLogger* aLogger );
        
        void ConstructL();
        
        //from CAsyncTestCaseBase
        TBool ExecuteL();
        
    private:
        TBool                  iRecordEvents;
        RArray<TObserverEvent> iRecordedEvents;
        
        
    };

#endif      // OBSERVER_TESTS_H

// End of File
