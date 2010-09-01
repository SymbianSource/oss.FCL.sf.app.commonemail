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
* Description:  Base class for all test cases class member functions
*
*/



// [INCLUDE FILES] - do not remove
#include <tz.h>
#include <e32math.h>
#include <Stiftestinterface.h>
#include <SettingServerClient.h>
#include "AsyncTestCaseBase.h"
#include "messagestoreclientserver.h"

const TInt KExecutionTimeout = 30*1000000;   //30 seconds timelimit for each test case

//Class RDebugSession
TInt RDebugSession::Connect()
    {
#ifdef _DEBUG
    return CreateSession( KMsgStoreServerName, TVersion(KMsgStoreMajorVersion, KMsgStoreMinorVersion, KMsgStoreBuild), 3 );
#endif    
    }
  
void RDebugSession::Close()
    {
#ifdef _DEBUG
    RSessionBase::Close();  //basecall
#endif    
    }

void RDebugSession::SendBackupRestoreEventL( TUint aEvent )
    {
#ifdef _DEBUG
    User::LeaveIfError( SendReceive( EMsgStoreCmdInjectBackupRestoreEvent, TIpcArgs( aEvent ) ) );
#else
    User::Leave(KErrNotSupported);
#endif    
    }

void RDebugSession::SendPointSecEventL( TBool aLock )
    {
#ifdef _DEBUG
    User::LeaveIfError( SendReceive( EMsgStoreCmdInjectPointSecEvent, TIpcArgs( aLock ) ) );
#else
    User::Leave(KErrNotSupported);
#endif    
    }

void RDebugSession::SimulateLowDiskSpaceL( TUint aLatency )
    {
#ifdef _DEBUG
    User::LeaveIfError( SendReceive( EMsgStoreCmdSimulateLowDiskSpace, TIpcArgs( aLatency ) ) );
#else
    User::Leave(KErrNotSupported);
#endif    
    }    

TInt RDebugSession::GetEncryptionStateL()
    {
#ifdef _DEBUG
    TPckgBuf<TInt> resultPckg(0);
    
    User::LeaveIfError( SendReceive( EMsgStoreCmdGetEncryptionState, TIpcArgs( &resultPckg ) ) );
    
    return resultPckg();
#else
    User::Leave(KErrNotSupported);
#endif    
    }

void RDebugSession:: ShutdownServerL()
    {
#ifdef _DEBUG
    User::LeaveIfError( SendReceive( EMsgStoreCmdShutdownServer ) );
#else
    User::Leave(KErrNotSupported);
#endif    
    }

//Class CYieldHelper

CYieldHelper* CYieldHelper::NewL()
    {
    CYieldHelper* self = new(ELeave) CYieldHelper();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CYieldHelper::CYieldHelper() : CTimer( EPriorityIdle )
    {
    }

void CYieldHelper::ConstructL()
    {
    CTimer::ConstructL();
    iYieldWait = new(ELeave) CActiveSchedulerWait();
    CActiveScheduler::Add(this);
    }

CYieldHelper::~CYieldHelper()
    {
    delete iYieldWait;
    }

void CYieldHelper::Yield( TInt aMicroSeconds )
    {
    After( aMicroSeconds );
    iYieldWait->Start(); 
    }

void CYieldHelper::DoCancel()
    {
    //DO Nothing
    }

void CYieldHelper::RunL()
    {
    if( iYieldWait->IsStarted() )
        {
        // Stop the nested active scheduler (flow-of-control will pick up immediately following the
        // call to iWait.Start().
        iYieldWait->AsyncStop();
        } // end if
    }

//Class CAsyncTestCaseBase

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CAsyncTestCaseBase::CAsyncTestCaseBase( CStifLogger* aLog ) 
: CTimer( CActive::EPriorityStandard ), iLog( aLog ), iCaseId(1)  
    {
    // Construct and install active scheduler
    CActiveScheduler* iActiveScheduler = new CActiveScheduler;
    CActiveScheduler::Replace( iActiveScheduler );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CAsyncTestCaseBase::~CAsyncTestCaseBase()
    {
    Cancel();
    iMessages.ResetAndDestroy();
    delete iMsgStore;
    delete iActiveScheduler;
    delete iYieldHelper;
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CAsyncTestCaseBase::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);  
    
    //Construct the MsgStore client session
    iMsgStore = CMsgStore::NewL();
    
    iYieldHelper = CYieldHelper::NewL();
    
    iState = EStateInit;
    iResult = KErrNone;
    CTimer::After(0);
    }


//stop the execution
void CAsyncTestCaseBase::Stop()
    {
    TRAP_IGNORE( AfterTestL() );
    
    _LIT( KStop, "Ending test cases... iResult=%d" );
    iLog->Log( KStop, iResult );
    CActiveScheduler::Stop();
    }

void CAsyncTestCaseBase::BeforeTestL()
    {
    _LIT( KBeforeTestLEnter, ">> BeforeTestL" );
    iLog->Log( KBeforeTestLEnter );
    
    iMsgStore->WipeEverythingL();
    
    _LIT( KBeforeTestLExit, "<< BeforeTestL" );
    iLog->Log( KBeforeTestLExit );
    }

void CAsyncTestCaseBase::AfterTestL()
    {
    _LIT( KAfterTestLEnter, ">> AfterTestL" );
    iLog->Log( KAfterTestLEnter );
    
    delete iMsgStore;
    iMsgStore = NULL;
    
    _LIT( KAfterTestLExit, "<< AfterTestL" );
    iLog->Log( KAfterTestLExit );
    }


//From CTimer
void CAsyncTestCaseBase::RunL()
    {
    if ( iStatus == KErrNone )
        {
        if ( iState == EStateInit )
            {
            BeforeTestL();
            
            TBool done = EFalse;
            TRAP( iResult, done = ExecuteL() );
            if( iResult == KErrNone )
                {
                if ( done )
                    {
                    iState = EStateDone;
                    Stop();
                    }
                else
                    {
                    iState = EStateExecute;
                    CTimer::After( KExecutionTimeout );
                    }
                }
            else
                {
                iState = EStateDone;
                Stop();
                }
            }
        else if ( iState == EStateExecute )
            {
            //Timer has expired, error out
            iState = EStateDone;
            iResult = KErrTimedOut;
            Stop();
            }
        }
    else if ( iStatus != KErrCancel )
        {
        iState = EStateDone;
        iResult = KErrCorrupt;
        Stop();
        }
    
    }

TInt CAsyncTestCaseBase::RunError(TInt aError)
    {
    //log the error
    iResult = aError;
    return aError;
    }

void CAsyncTestCaseBase::DoCancel()
    {
    CTimer::DoCancel(); 
    }

void CAsyncTestCaseBase::LogHeader( const TDesC& aTestType )
    {
    _LIT(KDivider, "==============================================================");
    iLog->Log( KDivider );
    iLog->Log( _L("Executing %S Test Cases"), &aTestType );
    iLog->Log( KDivider );
    }

void CAsyncTestCaseBase::CheckCondition( const TDesC& aCondition, TBool aPassed,  TBool aLog  )
    {
    if( aPassed )
        {
        if ( aLog )
            {
            iLog->Log( _L("  PASSED!  Condition: %S"), &aCondition );
            }
        }
    else
        {
        iLog->Log( _L(  "*** FAILED!  Condition: %S"), &aCondition );     
        User::Panic( aCondition, 1 );
        } // end if
    }


void CAsyncTestCaseBase::ComparePropertiesL( const CMsgStorePropertyContainer& aProperties1, const CMsgStorePropertyContainer& aProperties2 )
    {
    iLog->Log( _L("  >> ComparePropertiesL") );
    
    TBool passed = (aProperties1.PropertyCount() == aProperties2.PropertyCount());
    
    for( TUint index1 = 0; index1 < aProperties1.PropertyCount(); index1++ )
        {
        TUint index2;
        
        const TPtrC8 property1Name( aProperties1.PropertyNameL(index1) );
        
        TBool matched = EFalse;
        
        if( aProperties2.FindProperty( property1Name, index2 ) )
            {
            TMsgStorePropertyValueType value1 = aProperties1.PropertyTypeL( index1 );
            TMsgStorePropertyValueType value2 = aProperties2.PropertyTypeL( index2 );
            
            // Found
            if( value1 != value2 )
                {
                iLog->Log( _L("type mismatch") );
                }
            else
                {
                switch( aProperties1.PropertyTypeL( index1 ) )
                    {
                    case EMsgStoreTypeBool:
                        {
                        const TBool property1Value = aProperties1.PropertyValueBoolL(index1);
                        const TBool property2Value = aProperties2.PropertyValueBoolL(index2);
                        
                        if( property1Value == property2Value )
                            {
                            matched = ETrue;
                            }
                        else
                            {
                            iLog->Log( _L("  property %S value mismatch"), &property1Name );                                                         
                            iLog->Log( _L("  expected %i"), &property1Value );                                                            
                            iLog->Log( _L("  found %i"), &property2Value );                                                           
                            }                                   
                        break;
                        }
                    case EMsgStoreTypeUint32:
                        {
                        const TUint32 property1Value = aProperties1.PropertyValueUint32L(index1);
                        const TUint32 property2Value = aProperties2.PropertyValueUint32L(index2);
                        
                        if( property1Value == property2Value )
                            {
                            matched = ETrue;
                            }
                        else
                            {
                            iLog->Log( _L("  property %S value mismatch"), &property1Name );                                                         
                            iLog->Log( _L("  expected %i"), &property1Value );                                                            
                            iLog->Log( _L("  found %i"), &property2Value );                                                           
                            }                                   
                        break;
                        }
                    case EMsgStoreTypeDes:
                        {
                        const TPtrC property1Value( aProperties1.PropertyValueDesL(index1) );
                        const TPtrC property2Value( aProperties2.PropertyValueDesL(index2) );           
                        
                        if( property1Value.Compare( property2Value ) == 0 )
                            {
                            matched = ETrue;
                            }
                        else
                            {
                            iLog->Log( _L("  property %S value mismatch"), &property1Name );                                                         
                            iLog->Log( _L("  expected %S"), &property1Value );                                                            
                            iLog->Log( _L("  found %S"), &property2Value );                                                           
                            }                                   
                        break;
                        }
                    case EMsgStoreTypeDes8:
                        {
                        const TPtrC8 property1Value( aProperties1.PropertyValueDes8L(index1) );
                        const TPtrC8 property2Value( aProperties2.PropertyValueDes8L(index2) );         
                        
                        if( property1Value.Compare( property2Value ) == 0 )
                            {
                            matched = ETrue;
                            }
                        else
                            {
                            iLog->Log( _L("  property %S value mismatch"), &property1Name );                                                         
                            iLog->Log( _L("  expected %S"), &property1Value );                                                           
                            iLog->Log( _L("  found %S"), &property2Value );                                                          
                            }
                        break;
                        }
                    case EMsgStoreTypeContainer:
                        {
                        CMsgStorePropertyContainer* property1Value = aProperties1.PropertyValueContainerL(index1);
                        CMsgStorePropertyContainer* property2Value = aProperties2.PropertyValueContainerL(index2);

                        ComparePropertiesL( *property1Value, *property2Value );                                                                 
                        
                        matched = ETrue;
                        
                        delete property1Value;
                        delete property2Value;
                        break;
                        }                           
                    case EMsgStoreTypeTime:
                        {
                        TTime time1, time2;
                        aProperties1.PropertyValueTimeL( index1, time1 );
                        aProperties2.PropertyValueTimeL( index2, time2 );
                        
                        if ( time1 == time2 )
                            {
                            matched = ETrue;
                            }
                        
                        break;
                        }
                    case EMsgStoreTypeAddress:
                        {
                        RMsgStoreAddress addr1, addr2;
                        
                        aProperties1.PropertyValueAddressL( index1, addr1 );
                        aProperties2.PropertyValueAddressL( index2, addr2 );
                        
                        if ( addr1.iEmailAddress == addr2.iEmailAddress && addr1.iDisplayName == addr2.iDisplayName )
                            {
                            matched = ETrue;
                            }
                        
                        addr1.Close();
                        addr2.Close();
                        break;
                        }

                    } // end switch
                    
                } // end if
            }
        else
            {
            iLog->Log( _L("  property %S not found"), &property1Name );                  
            }
        
        passed = passed && matched;
        
        } // end for            

    TBuf<30> text;
    text.Format( _L("compare %i properties"), aProperties1.PropertyCount() );
    
    CheckCondition( text, passed );
    iLog->Log( _L("  << ComparePropertiesL") );
    }


TUint CAsyncTestCaseBase::TestAddIntegerPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, TUint32 aValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();

    TUint index;
    if( aUseAddOrUpdate )
        {
        index = aProperties.AddOrUpdatePropertyL( aName, aValue );          
        }
    else
        {
        index = aProperties.AddPropertyL( aName, aValue );  
        } // end if
    
    TRAPD( des8Result, aProperties.PropertyValueDes8L( index ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( index ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( index ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( index ) );
                
    TBool passed = (aProperties.PropertyCount() == (countBefore+1) &&
                    aProperties.PropertyNameL( index ).Compare( aName ) == 0 &&
                    aProperties.PropertyTypeL( index ) == EMsgStoreTypeUint32 &&                        
                    aProperties.PropertyValueUint32L( index ) == aValue &&
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument );
                    
    CheckCondition( _L("add integer property"), passed );
    
    return index;
    } 

TUint CAsyncTestCaseBase::TestAddBoolPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, TBool aValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TUint index;
    if( aUseAddOrUpdate )
        {
        index = aProperties.AddOrUpdatePropertyL( aName, aValue );          
        }
    else
        {
        index = aProperties.AddPropertyL( aName, aValue );  
        } // end if
        
    TRAPD( des8Result, aProperties.PropertyValueDes8L( index ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( index ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( index ) );
    TRAPD( integerResult, aProperties.PropertyValueUint32L( index ) );
                
    TBool passed = (aProperties.PropertyCount() == (countBefore+1) &&
                    aProperties.PropertyNameL( index ).Compare( aName ) == 0 &&
                    aProperties.PropertyTypeL( index ) == EMsgStoreTypeBool &&
                    aProperties.PropertyValueBoolL( index ) == aValue &&
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    integerResult == KErrArgument );
                    
    CheckCondition( _L("add bool property"), passed );
    
    return index;
    } 
    
TUint CAsyncTestCaseBase::TestAddDes8PropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const TDesC8& aValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TUint index;
    if( aUseAddOrUpdate )
        {
        index = aProperties.AddOrUpdatePropertyL( aName, aValue );          
        }
    else
        {
        index = aProperties.AddPropertyL( aName, aValue );  
        } // end if
    
    TRAPD( boolResult, aProperties.PropertyValueBoolL( index ) );
    TRAPD( intResult, aProperties.PropertyValueUint32L( index ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( index ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( index ) );
    
    TBool passed = (aProperties.PropertyCount() == (countBefore+1) &&
                    aProperties.PropertyNameL( index ).Compare( aName ) == 0 && 
                    aProperties.PropertyTypeL( index ) == EMsgStoreTypeDes8 &&                      
                    aProperties.PropertyValueDes8L( index ).Compare( aValue ) == 0 &&
                    intResult == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument );
                    
    CheckCondition( _L("add des8 property"), passed );
    
    return index;
    } 

TUint CAsyncTestCaseBase::TestAddDesPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const TDesC& aValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TUint index;
    if( aUseAddOrUpdate )
        {
        index = aProperties.AddOrUpdatePropertyL( aName, aValue );          
        }
    else
        {
        index = aProperties.AddPropertyL( aName, aValue );  
        } // end if
    
    TRAPD( boolResult, aProperties.PropertyValueBoolL( index ) );
    TRAPD( intResult, aProperties.PropertyValueUint32L( index ) );
    TRAPD( des8Result, aProperties.PropertyValueDes8L( index ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( index ) );
    
    TBool passed = (aProperties.PropertyCount() == (countBefore+1) &&
                    aProperties.PropertyNameL( index ).Compare( aName ) == 0 &&
                    aProperties.PropertyTypeL( index ) == EMsgStoreTypeDes &&
                    aProperties.PropertyValueDesL( index ).Compare( aValue ) == 0 &&
                    intResult == KErrArgument &&
                    des8Result == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument );
                    
    CheckCondition( _L("add des property"), passed );
    
    return index;
    } 

TUint CAsyncTestCaseBase::TestAddContainerPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const CMsgStorePropertyContainer& aValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TUint index;
    if( aUseAddOrUpdate )
        {
        index = aProperties.AddOrUpdatePropertyL( aName, aValue );          
        }
    else
        {
        index = aProperties.AddPropertyL( aName, aValue );  
        } // end if
    
    CMsgStorePropertyContainer* container = aProperties.PropertyValueContainerL( index );
    ComparePropertiesL( aValue, *container );
    delete container;
                                
    TRAPD( boolResult, aProperties.PropertyValueUint32L( index ) );
    TRAPD( intResult, aProperties.PropertyValueUint32L( index ) );
    TRAPD( des8Result, aProperties.PropertyValueDes8L( index ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( index ) );
    
    TBool passed = (aProperties.PropertyCount() == (countBefore+1) &&
                    aProperties.PropertyNameL( index ).Compare( aName ) == 0 &&
                    aProperties.PropertyTypeL( index ) == EMsgStoreTypeContainer &&
                    desResult == KErrArgument &&
                    des8Result == KErrArgument &&
                    intResult == KErrArgument &&
                    boolResult == KErrArgument );

    CheckCondition( _L("add container property"), passed );
    
    return index;
    } 

TUint CAsyncTestCaseBase::TestAddTimePropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const TTime& aValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TUint index;
    if( aUseAddOrUpdate )
        {
        index = aProperties.AddOrUpdatePropertyL( aName, aValue );          
        }
    else
        {
        index = aProperties.AddPropertyL( aName, aValue );  
        } // end if
        
    TRAPD( des8Result, aProperties.PropertyValueDes8L( index ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( index ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( index ) );
    TRAPD( integerResult, aProperties.PropertyValueUint32L( index ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( index ) );
                
    TBool passed = (aProperties.PropertyCount() == (countBefore+1) &&
                    aProperties.PropertyNameL( index ).Compare( aName ) == 0 &&
                    aProperties.PropertyTypeL( index ) == EMsgStoreTypeTime &&
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    integerResult == KErrArgument &&
                    boolResult == KErrArgument );

    TTime time;
    aProperties.PropertyValueTimeL( index, time );
    
    passed = passed && time == aValue;
    
    CheckCondition( _L("add time property"), passed );
    
    return index;
    } 

TUint CAsyncTestCaseBase::TestAddAddressPropertyL( CMsgStorePropertyContainer& aProperties, const TDesC8& aName, const RMsgStoreAddress& aValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TUint index;
    if( aUseAddOrUpdate )
        {
        index = aProperties.AddOrUpdatePropertyL( aName, aValue );          
        }
    else
        {
        index = aProperties.AddPropertyL( aName, aValue );  
        } // end if
        
    TRAPD( des8Result, aProperties.PropertyValueDes8L( index ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( index ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( index ) );
    TRAPD( integerResult, aProperties.PropertyValueUint32L( index ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( index ) );
    TTime time;
    TRAPD( timeResult, aProperties.PropertyValueTimeL( index, time ) );
                
    TBool passed = (aProperties.PropertyCount() == (countBefore+1) &&
                    aProperties.PropertyNameL( index ).Compare( aName ) == 0 &&
                    aProperties.PropertyTypeL( index ) == EMsgStoreTypeAddress &&
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    integerResult == KErrArgument &&
                    boolResult == KErrArgument &&
                    timeResult == KErrArgument );

    RMsgStoreAddress address;
    aProperties.PropertyValueAddressL( index, address );
    
    iLog->Log( _L("  passed=%d"), passed );
    iLog->Log( _L("  email addr=%S, disp name=%S"), &address.iEmailAddress, &address.iDisplayName);
    
    passed = passed && (address.iEmailAddress == aValue.iEmailAddress) && (address.iDisplayName == aValue.iDisplayName);
    
    address.Close();
    
    CheckCondition( _L("add address property"), passed );
    
    return index;
    } 

void CAsyncTestCaseBase::TestRemovePropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    aProperties.RemovePropertyL( aIndex );  

    TUint index;
    TBool found = aProperties.FindProperty( oldName, index );

    TBool passed = (!found &&
                    aProperties.PropertyCount() == (countBefore-1) );
                    
    CheckCondition( _L("remove property"), passed );
    } 

void CAsyncTestCaseBase::TestUpdateDes8PropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex, const TDesC8& aNewValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    if( aUseAddOrUpdate )
        {
        aProperties.AddOrUpdatePropertyL( aProperties.PropertyNameL(aIndex), aNewValue );           
        }
    else
        {
        aProperties.UpdatePropertyL( aIndex, aNewValue );   
        } // end if
    
    TRAPD( intResult, aProperties.PropertyValueUint32L( aIndex ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( aIndex ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( aIndex ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( aIndex ) );
    
    TPtrC8 des8( aProperties.PropertyValueDes8L( aIndex ) );
    
    TBool passed = (aProperties.PropertyCount() == countBefore &&
                    aProperties.PropertyNameL( aIndex ).Compare( oldName ) == 0 &&
                    aProperties.PropertyTypeL( aIndex ) == EMsgStoreTypeDes8 &&                     
                    des8.Compare( aNewValue ) == 0 &&
                    intResult == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument );
                    
    CheckCondition( _L("update des8 property"), passed );
    } 

void CAsyncTestCaseBase::TestUpdateDesPropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex, const TDesC& aNewValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    if( aUseAddOrUpdate )
        {
        aProperties.AddOrUpdatePropertyL( aProperties.PropertyNameL(aIndex), aNewValue );           
        }
    else
        {
        aProperties.UpdatePropertyL( aIndex, aNewValue );   
        } // end if
        
    TRAPD( intResult, aProperties.PropertyValueUint32L( aIndex ) );
    TRAPD( des8Result, aProperties.PropertyValueDes8L( aIndex ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( aIndex ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( aIndex ) );

    TPtrC des( aProperties.PropertyValueDesL( aIndex ) );
                
    TBool passed = (aProperties.PropertyCount() == countBefore &&
                    aProperties.PropertyNameL( aIndex ).Compare( oldName ) == 0 &&
                    aProperties.PropertyTypeL( aIndex ) == EMsgStoreTypeDes &&
                    des.Compare( aNewValue ) == 0 &&
                    intResult == KErrArgument &&
                    des8Result == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument );
                    
    CheckCondition( _L("update string(16) property"), passed );
    } 

void CAsyncTestCaseBase::TestUpdateContainerPropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex, CMsgStorePropertyContainer& aNewValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    if( aUseAddOrUpdate )
        {
        aProperties.AddOrUpdatePropertyL( aProperties.PropertyNameL(aIndex), aNewValue );           
        }
    else
        {
        aProperties.UpdatePropertyL( aIndex, aNewValue );   
        } // end if
    
    CMsgStorePropertyContainer* container = aProperties.PropertyValueContainerL( aIndex );
    ComparePropertiesL( aNewValue, *container );
    delete container;
                                            
    TRAPD( intResult, aProperties.PropertyValueUint32L( aIndex ) );
    TRAPD( des8Result, aProperties.PropertyValueDes8L( aIndex ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( aIndex ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( aIndex ) );

    TBool passed = (aProperties.PropertyCount() == countBefore &&
                    aProperties.PropertyNameL( aIndex ).Compare( oldName ) == 0 &&
                    aProperties.PropertyTypeL( aIndex ) == EMsgStoreTypeContainer &&
                    intResult == KErrArgument &&
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    boolResult == KErrArgument );
                    
    CheckCondition( _L("update container property"), passed );
    } 

void CAsyncTestCaseBase::TestUpdateBoolPropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex, TBool aNewValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    if( aUseAddOrUpdate )
        {
        aProperties.AddOrUpdatePropertyL( aProperties.PropertyNameL(aIndex), aNewValue );           
        }
    else
        {
        aProperties.UpdatePropertyL( aIndex, aNewValue );   
        } // end if
    
    TRAPD( intResult, aProperties.PropertyValueUint32L( aIndex ) );
    TRAPD( des8Result, aProperties.PropertyValueDes8L( aIndex ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( aIndex ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( aIndex ) );
                
    TBool passed = (aProperties.PropertyCount() == countBefore &&
                    aProperties.PropertyNameL( aIndex ).Compare( oldName ) == 0 &&
                    aProperties.PropertyTypeL( aIndex ) == EMsgStoreTypeBool &&                     
                    aProperties.PropertyValueBoolL( aIndex ) == aNewValue &&
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    intResult == KErrArgument );
                    
    CheckCondition( _L("update bool property"), passed );
    } 

void CAsyncTestCaseBase::TestUpdateIntegerPropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex, TUint32 aNewValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    if( aUseAddOrUpdate )
        {
        aProperties.AddOrUpdatePropertyL( aProperties.PropertyNameL(aIndex), aNewValue );           
        }
    else
        {
        aProperties.UpdatePropertyL( aIndex, aNewValue );   
        } // end if
    
    TRAPD( des8Result, aProperties.PropertyValueDes8L( aIndex ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( aIndex ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( aIndex ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( aIndex ) );
                
    TBool passed = (aProperties.PropertyCount() == countBefore &&
                    aProperties.PropertyNameL( aIndex ).Compare( oldName ) == 0 &&
                    aProperties.PropertyTypeL( aIndex ) == EMsgStoreTypeUint32 &&                       
                    aProperties.PropertyValueUint32L( aIndex ) == aNewValue &&
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument );
                    
    CheckCondition( _L("update integer property"), passed );
    } 

void CAsyncTestCaseBase::TestUpdateTimePropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex, const TTime& aNewValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    if( aUseAddOrUpdate )
        {
        aProperties.AddOrUpdatePropertyL( aProperties.PropertyNameL(aIndex), aNewValue );           
        }
    else
        {
        aProperties.UpdatePropertyL( aIndex, aNewValue );   
        } // end if
    
    TRAPD( des8Result, aProperties.PropertyValueDes8L( aIndex ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( aIndex ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( aIndex ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( aIndex ) );
    TRAPD( intResult, aProperties.PropertyValueUint32L( aIndex ) );
                
    TBool passed = (aProperties.PropertyCount() == countBefore &&
                    aProperties.PropertyNameL( aIndex ).Compare( oldName ) == 0 &&
                    aProperties.PropertyTypeL( aIndex ) == EMsgStoreTypeTime &&                     
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument &&
                    intResult == KErrArgument );
    
    TTime time;
    aProperties.PropertyValueTimeL( aIndex, time );
    
    passed = passed && time == aNewValue;
    
                    
    CheckCondition( _L("update time property"), passed );
    } 


void CAsyncTestCaseBase::TestUpdateAddressPropertyL( CMsgStorePropertyContainer& aProperties, TUint aIndex, const RMsgStoreAddress& aNewValue, TBool aUseAddOrUpdate )
    {
    TUint countBefore = aProperties.PropertyCount();
    
    TBuf8<500> oldName;
    oldName.Copy( aProperties.PropertyNameL( aIndex ) );
    
    if( aUseAddOrUpdate )
        {
        aProperties.AddOrUpdatePropertyL( aProperties.PropertyNameL(aIndex), aNewValue );           
        }
    else
        {
        aProperties.UpdatePropertyL( aIndex, aNewValue );   
        } // end if
    
    TRAPD( des8Result, aProperties.PropertyValueDes8L( aIndex ) );
    TRAPD( desResult, aProperties.PropertyValueDesL( aIndex ) );
    TRAPD( containerResult, aProperties.PropertyValueContainerL( aIndex ) );
    TRAPD( boolResult, aProperties.PropertyValueBoolL( aIndex ) );
    TRAPD( intResult, aProperties.PropertyValueUint32L( aIndex ) );
    TTime time;
    TRAPD( timeResult, aProperties.PropertyValueTimeL( aIndex, time ) );
    
    TBool passed = (aProperties.PropertyCount() == countBefore &&
                    aProperties.PropertyNameL( aIndex ).Compare( oldName ) == 0 &&
                    aProperties.PropertyTypeL( aIndex ) == EMsgStoreTypeAddress &&                      
                    des8Result == KErrArgument &&
                    desResult == KErrArgument &&
                    containerResult == KErrArgument &&
                    boolResult == KErrArgument &&
                    intResult == KErrArgument,
                    timeResult == KErrArgument );
    
    RMsgStoreAddress address;
    aProperties.PropertyValueAddressL( aIndex, address );
    
    iLog->Log( _L("  email addr=%S, disp name=%S"), &address.iEmailAddress, &address.iDisplayName);
    
    passed = passed && (address.iEmailAddress == aNewValue.iEmailAddress) && (address.iDisplayName == aNewValue.iDisplayName);
    
    address.Close();
                    
    CheckCondition( _L("update address property"), passed );
    } 

void CAsyncTestCaseBase::CopyDesToDes8( const TDesC& aDes, TDes8& aDes8 )
    {
    const TUint8* ptr = reinterpret_cast<const TUint8*>( aDes.Ptr() );
    
    aDes8.Copy( ptr, aDes.Length() * 2 );
    }

void CAsyncTestCaseBase::SimulateLowDiskSpaceL( TUint aLatency )
    {
    RDebugSession session;
    User::LeaveIfError( session.Connect() );
    session.SimulateLowDiskSpaceL( aLatency );
    session.Close();    
    }

void CAsyncTestCaseBase::TriggerBackupRestoreEventL( TUint aEvent )
    {
    RDebugSession session;
    User::LeaveIfError( session.Connect() );
    session.SendBackupRestoreEventL( aEvent );
    session.Close();
    }

void CAsyncTestCaseBase::TriggerPointSecEventL( TBool aLock )
    {
    RDebugSession session;
    User::LeaveIfError( session.Connect() );
    session.SendPointSecEventL( aLock );
    session.Close();
    }

void CAsyncTestCaseBase::ShutdownServerL()
    {
    RDebugSession temp;
    User::LeaveIfError( temp.Connect() ); 
    temp.ShutdownServerL();
    temp.Close();
    }

void CAsyncTestCaseBase::Yield( TInt aMicroSeconds )
    {
    iYieldHelper->Yield( aMicroSeconds );
    }

void CAsyncTestCaseBase::CreateRandomMessagesL( CMsgStoreMailBox* aMailBox, TMsgStoreId aFolderId, TInt aCount,  TBool aKeepIds, TBool /* aLog */ )
    {
    iLog->Log( _L("  >> CreateRandomMessagesL, creating %d messages"), aCount );
    
    TTime time;
    time.HomeTime();
    TInt64 seed = time.Int64();
    
    CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( properties );              //+1
    
    RMsgStoreAddress address;
    CleanupClosePushL( address );                   //+2
    
    address.iDisplayName.Create( 50 );
    address.iEmailAddress.Create( 50 );
    
    TBuf<30> subject;
    TBuf<50> realSubject;                          
    TBuf<6> fname, lname;
    TTime startTime;
    
    if ( aKeepIds )
        {
        iMatches.Reset();
        }

    RTz timeZoneServer;
    User::LeaveIfError( timeZoneServer.Connect() );
    CleanupClosePushL( timeZoneServer );             //+3
    
    for (TInt i = 0 ; i < aCount ; i++ )
        {
        TTime received;
        received.HomeTime();
        TTimeIntervalDays days( (i+1) % 9 );
        TTimeIntervalHours hours( (i+1) %9 );
        received -= days;
        received += hours;
        
        timeZoneServer.ConvertToUniversalTime( received );
        properties->AddOrUpdatePropertyL( KMsgStorePropertyReceivedAt, received );
        
        TUint32 flags = Math::Rand( seed );
        properties->AddOrUpdatePropertyL( KMsgStorePropertyFlags, (TUint32) flags );
        
        TUint32 size = Math::Rand( seed );
        properties->AddOrUpdatePropertyL( KMsgStorePropertyMessageSizeOnServer, (TUint32) size );
        
        if ( i == 0 || i % 9 == 0 )
            {
            CreateRandomString( subject, 30, seed );
            CreateRandomString( fname, 6, seed );
            CreateRandomString( lname, 6, seed );
            }
        
        realSubject.SetLength(0);
        if ( i % 5 == 0 )
            {
            realSubject.Copy(_L("RE: "));
            }
        else if ( i % 10 == 0 )
            {
            realSubject.Copy(_L("RE: FWD:"));
            }
        else if ( i % 3 == 0 )
            {
            realSubject.Copy(_L("R&D:"));
            }
        realSubject.Append( subject );
        
        properties->AddOrUpdatePropertyL( KMsgStorePropertySubject, realSubject );
        
        if ( i % 10 > 0 )
            {
            //skip display name for every 10 messages
            address.iDisplayName.Format( _L("%S %S"), &fname, &lname );
            }
        _LIT(KDomainName, "nokia.com");
        address.iEmailAddress.Format( _L("%S.%S@%S"), &fname, &lname, &KDomainName);
        properties->AddOrUpdatePropertyL( KMsgStorePropertyFrom, address );
        
        properties->AddOrUpdatePropertyL( KMsgStorePropertyTo, address );
        
        if ( i == aCount - 1 )
            {
            startTime.HomeTime();
            }
        CMsgStoreMessage* msg = aMailBox->CreateMessageL( aFolderId, *properties );
        CleanupStack::PushL( msg );
        msg->CommitL();
        
        if ( i == aCount - 1 )
            {
            TTime endTime;
            endTime.HomeTime();
            // iLog->Log( _L("  PERFORMANCE: add messages #%d took %d ms"), aCount, (TInt) (endTime.MicroSecondsFrom( startTime ).Int64() / 1000)  );
            }
        
#if 0        
        if ( aLog )
            {
            iLog->Log( _L("  msgId=%x, flags=%x, size=%d"), msg->Id(), flags, size);
            iLog->Log( _L("  subject=%S"), &realSubject );
            }
        else if ( i % 10 == 9 )
            {
            iLog->Log( _L("  count=%d, msgId=%x"), i+1, msg->Id() );
            } 
#endif
        
        if ( aKeepIds )
            {
            iMatches.Append( msg->Id() );
            }
            
        CleanupStack::PopAndDestroy( msg );
        }
    
    CleanupStack::PopAndDestroy( 3 );
    
    iLog->Log( _L("  << CreateRandomMessagesL") );
    }

void CAsyncTestCaseBase::CreateFileL( const TDesC& aName, TUint aTotalLength, TChar aFillChar, const TDesC& aString, TUint aOffset )
    {
    RFs   fs;
    User::LeaveIfError( fs.Connect() );
    RFile file;
    User::LeaveIfError( file.Replace( fs, aName, EFileWrite ) );
    
    TUint currentLength = 0;
    
    TBuf<1000> buf;

    const TUint8* buf8Ptr = reinterpret_cast<const TUint8*>( buf.Ptr() );
    TPtrC8 buf8( buf8Ptr, 2000 );
    
    buf.Fill( aFillChar, 1000 );
    while( currentLength < aOffset )
        {
        TUint amountToWrite = aOffset - currentLength;
        if( amountToWrite > 1000 )
            {
            amountToWrite = 1000;
            }
        
        file.Write( buf8, amountToWrite*2 );
        
        currentLength += amountToWrite;             
        }
    
    buf.Copy( aString );
    file.Write( buf8, aString.Length() * 2 );
    currentLength += aString.Length();

    buf.Fill( aFillChar, 1000 );
    while( currentLength < aTotalLength )
        {
        TUint amountToWrite = aTotalLength - currentLength;
        if( amountToWrite > 1000 )
            {
            amountToWrite = 1000;
            }
        
        file.Write( buf8, amountToWrite*2 );
        
        currentLength += amountToWrite;             
        }
    
    file.Close();
    fs.Close();           
    }


void CAsyncTestCaseBase::CreateRandomString( TDes& aString, TInt aLength, TInt64& aSeed )
    {
    aString.SetLength(0);
    for ( TInt i = 0 ; i < aLength; i++ )
        {
        TUint val = static_cast<TUint>(Math::Rand( aSeed ));
        val = ( val % 52 );
        if ( val <= 25 )
            {
            val += 65;
            }
        else
            {
            val += ( 97 - 26 );
            }
        aString.Append( TChar(val) );
        }
    }

void CAsyncTestCaseBase::SetMessageContentL( CMsgStoreMessage* aMessage, const TDesC8& aContent )
    {
    CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );
    
    CMsgStoreMessagePart* body = aMessage->AddChildPartL( *props );
    CleanupStack::PushL( body );
    
    CMsgStoreMessagePart* plainTextbody = body->AddChildPartL( *props );
    CleanupStack::PushL( plainTextbody );
    
    plainTextbody->ReplaceContentL( aContent );
    
    CleanupStack::PopAndDestroy( plainTextbody );
    CleanupStack::PopAndDestroy( body );
    CleanupStack::PopAndDestroy( props );
    }

void CAsyncTestCaseBase::WaitForServerToTerminate()
    {
    TBool done = EFalse;
    
    RDebugSession temp;
    
    while( !done )
        {        
        TInt err = temp.Connect(); 
        
        if (err == KErrNone )
            {
            temp.Close();
            
            // Still running.  Wait.
            Yield( 1000000 );
            }
        else
            {
            done = ETrue;
            }
            
        } // end if
    
    temp.Close();
    }

