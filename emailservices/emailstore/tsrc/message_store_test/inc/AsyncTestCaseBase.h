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
* Description:  t_ImsIdsPlugin test module.
*
*/



#ifndef ASYNC_TESTCASE_BASE_H
#define ASYNC_TESTCASE_BASE_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <NormalHardcodedAssert.h>
#include <msgstoreapi.h>

//Forward declaration

class CYieldHelper : public CTimer
    {
    public:
        static CYieldHelper* NewL();
        virtual ~CYieldHelper();

        void Yield( TInt aMicroSeconds );
        
    private:
        CYieldHelper();
        void ConstructL();
        
        // inherited from CActive
        virtual void RunL();
        virtual void DoCancel();
        
    private:
        CActiveSchedulerWait* iYieldWait;        
    };

// CLASS DECLARATION
class RDebugSession : public RSessionBase 
    {
    public:
    
        // SERVER MUST BE ALREADY RUNNING!
        TInt Connect();

        void Close();
                
        void SendBackupRestoreEventL( TUint aEvent );  
        
        void SendPointSecEventL( TBool aLock );
        
        void SimulateLowDiskSpaceL( TUint aLatency );        
        
        TInt GetEncryptionStateL();
        
        void ShutdownServerL();
    };


// CLASS DECLARATION
class CAsyncTestCaseBase : public CTimer
    {
    public:  // Constructors and destructor

        virtual ~CAsyncTestCaseBase();
        
        inline TInt Result() { return iResult; }

    protected:
        enum TState
            {
            EStateInit,
            EStateExecute,
            EStateDone,
            };
        
        /**
        * C++ default constructor.
        */
        CAsyncTestCaseBase( CStifLogger* aLogger );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        //steps for preparing the test cases
        virtual void BeforeTestL();
        
        //abstract method to start executing the real test case
        // return ETrue if execution is done
        virtual TBool ExecuteL() = 0;

        //steps for after the test cases, make sure this is always run
        virtual void AfterTestL();
        
        //stop the execution
        virtual void Stop();
        
        //From CTimer
        virtual void RunL();
        virtual TInt RunError(TInt aError);
        virtual void DoCancel();
        
        void LogHeader( const TDesC& aTestType );
        
        void CheckCondition( const TDesC& aCondition, TBool aPassed, TBool aLog=ETrue );
        
        void ComparePropertiesL( const CMsgStorePropertyContainer& aProperties1, 
                                 const CMsgStorePropertyContainer& aProperties2 );

        void SimulateLowDiskSpaceL( TUint aLatency );
        void TriggerBackupRestoreEventL( TUint aEvent );
        void TriggerPointSecEventL( TBool aLock );
        void ShutdownServerL();
        void Yield( TInt aMicroSeconds  );
        void WaitForServerToTerminate();
        
        TUint TestAddIntegerPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, TUint32 aValue, TBool aUseAddOrUpdate = EFalse );
        TUint TestAddBoolPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, TBool aValue, TBool aUseAddOrUpdate = EFalse  );
        TUint TestAddDes8PropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const TDesC8& aValue, TBool aUseAddOrUpdate = EFalse  );
        TUint TestAddDesPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const TDesC& aValue, TBool aUseAddOrUpdate = EFalse  );
        TUint TestAddContainerPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const CMsgStorePropertyContainer& aValue, TBool aUseAddOrUpdate = EFalse );
        TUint TestAddTimePropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const TTime& aValue, TBool aUseAddOrUpdate = EFalse );
        TUint TestAddAddressPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const RMsgStoreAddress& aValue, TBool aUseAddOrUpdate = EFalse );

        void TestRemovePropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex );

        void TestUpdateDes8PropertyL( CMsgStorePropertyContainer& aProperties, 
                                      TUint aIndex, 
                                      const TDesC8& aNewValue, 
                                      TBool aUseAddOrUpdate = EFalse );
        
        void TestUpdateDesPropertyL( CMsgStorePropertyContainer& aProperties, 
                                     TUint aIndex, 
                                     const TDesC& aNewValue, 
                                     TBool aUseAddOrUpdate = EFalse );
        
        void TestUpdateContainerPropertyL( CMsgStorePropertyContainer& aProperties, 
                                           TUint aIndex, 
                                           CMsgStorePropertyContainer& aNewValue, 
                                           TBool aUseAddOrUpdate = EFalse );
        
        void TestUpdateBoolPropertyL( CMsgStorePropertyContainer& aProperties, 
                                      TUint aIndex, 
                                      TBool aNewValue, 
                                      TBool aUseAddOrUpdate = EFalse );
        
        void TestUpdateIntegerPropertyL( CMsgStorePropertyContainer& aProperties, 
                                         TUint aIndex, 
                                         TUint32 aNewValue, 
                                         TBool aUseAddOrUpdate = EFalse );
        
        void TestUpdateTimePropertyL( CMsgStorePropertyContainer& aProperties, 
                                      TUint aIndex, 
                                      const TTime& aNewValue, 
                                      TBool aUseAddOrUpdate = EFalse );
        
        void TestUpdateAddressPropertyL( CMsgStorePropertyContainer& aProperties, 
                                         TUint aIndex, 
                                         const RMsgStoreAddress& aNewValue, 
                                         TBool aUseAddOrUpdate = EFalse );
        
        void CopyDesToDes8( const TDesC& aDes, TDes8& aDes8 );
        
        void CreateFileL( const TDesC& aName, 
                         TUint aTotalLength,
                         TChar aFillChar, 
                         const TDesC& aString, 
                         TUint aOffset );
        
        void CreateRandomMessagesL( CMsgStoreMailBox* aMailBox1, 
                                    TMsgStoreId aFolder1Id, 
                                    TInt aCount, 
                                    TBool aKeepIds = EFalse, 
                                    TBool aLog = EFalse );
        
        void CreateRandomString( TDes& aString, TInt aLength, TInt64& aSeed );
        
        void SetMessageContentL( CMsgStoreMessage* aMessage, const TDesC8& aContent );
        
    protected:
        // Pointer to logger
        CStifLogger*       iLog;
        
        CActiveScheduler*  iActiveScheduler;
        CMsgStore*         iMsgStore;
        
        TState             iState;
        TInt               iResult;
        TInt               iCaseId;
        
        RArray<TMsgStoreId> iMatches;
        RPointerArray<CMsgStoreMessage> iMessages;
        
    private:
        CYieldHelper*      iYieldHelper;
    };

#endif      // ASYNC_TESTCASE_BASE_H

// End of File
