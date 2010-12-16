/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  POP3 connect operation
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

// Constants and defines
const TInt KConnectOpPriority = CActive::EPriorityStandard;
const TInt KIpsPlgPop3PopulateLimitInitValue = 50;

_LIT( KIpsPlgPopConnectPanic, "PopConnectOp" );

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::NewL()
// ----------------------------------------------------------------------------
//
CIpsPlgPop3ConnectOp* CIpsPlgPop3ConnectOp::NewL(
    CMsvSession& aMsvSession,                           
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    TBool aForcePopulate,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler,
    TBool aSignallingAllowed )
    {
    FUNC_LOG;
    CIpsPlgPop3ConnectOp* op = 
        new(ELeave) CIpsPlgPop3ConnectOp( aMsvSession, aObserverRequestStatus, 
            aService, aForcePopulate, aActivityTimer, aFSMailBoxId,
            aFSOperationObserver, aFSRequestId, 
            aEventHandler, aSignallingAllowed );
        
    CleanupStack::PushL( op );
    op->ConstructL();
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::~CIpsPlgPop3ConnectOp()
// ----------------------------------------------------------------------------
//
CIpsPlgPop3ConnectOp::~CIpsPlgPop3ConnectOp()
    {
    FUNC_LOG;
    delete iEntry;
    delete iSelection;

    iState = EIdle;
    }

void CIpsPlgPop3ConnectOp::ReleaseOperationL()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    DoDisconnectL();
    iState = EFinalize;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgPop3ConnectOp::ProgressL()
    {
    FUNC_LOG;
    if( iError != KErrNone )
        {
        return GetErrorProgressL( iError );
        }
    else if( iOperation )
        {
        return iOperation->ProgressL();
        }
    
    iProgress().iErrorCode = KErrNone;
    return iProgress;    
    }   

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoCancel()
    {
    FUNC_LOG;
    if( iOperation )
        {
        iOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::DoRunL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoRunL()
    {
    FUNC_LOG;
    // handle these error situations properly, 
    // and report somehow back to framework
    TInt err  = KErrNone;
 
    if ( iState == EQueryingDetails )
        {
        // Retry connect.
        DoConnectL();
        iState = EConnected;
        }       
    else if ( iState == EStartConnect )
        {
        
        if ( iOperation )
            {
            // operation exist and it means 
            // disconnect operation was ongoing.
            // How handle errors
            delete iOperation;
            iOperation = NULL;
            }
        
        if ( ValidateL() )
            {
            // Begin Connect.
            DoConnectL();
            iState = EConnected;
            }
        else
            {
            User::Leave( KErrCancel );    // User cancelled.
            }
        }
    else if ( iState == EConnected )
        {
        // Connect completed.
        err = GetOperationErrorCodeL( );
            
        if ( ( err == KPop3InvalidUser ) ||
             ( err == KPop3InvalidLogin )  ||
             ( err == KPop3InvalidApopLogin ) )
            {
            // Login details are wrong. Trying to ask for password
            if ( !QueryUserPassL() )
                {
                CompleteObserver( err );
                }
            }
        else if ( err == KErrNone )
            {
            if ( iForcePopulate && Connected() )
                {
                iState = EPopulate;
                DoPopulateL();
                }
            else
                {
                iState = EIdle;
                CompleteObserver( KErrNone );
                }
            }
        else
            {
            // We can't do this before, because 
            // we want to handle KPop3InvalidUser,
            // KPop3InvalidLogin and KPop3InvalidApopLogin separately.
            User::Leave( err );
            }
        }
    else if ( iState == EPopulate )
        {
        err = GetOperationErrorCodeL( );
        if ( iEventHandler )
            {
            iEventHandler->SetNewPropertyEvent( 
                    iService, KIpsSosEmailSyncCompleted, err );
            }
        CIpsPlgSyncStateHandler::SaveSuccessfulSyncTimeL(
                iMsvSession, iService );
        CompleteObserver( err ); 
        }
    else if ( iState == EFinalize )
        {
        User::Leave( KErrAbort );
        }
    else
        {
        User::Panic( KIpsPlgPopConnectPanic ,KErrNotFound);
        }
    
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::GetErrorProgressL
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgPop3ConnectOp::GetErrorProgressL( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    if ( iOperation && iError == KErrNone )
        {
        return iOperation->ProgressL();
        }
    TPop3Progress& progress = iProgress();
    progress.iPop3Progress = TPop3Progress::EPopConnecting;
    progress.iTotalMsgs = 0;
    progress.iMsgsToProcess = 0;
    progress.iBytesDone = 0;
    progress.iTotalBytes = 0;
    progress.iErrorCode = iError;
    progress.iPop3SubStateProgress = TPop3Progress::EPopConnecting;
    progress.iServiceId = iService;
    return iProgress;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::GetFSProgressL()
// ----------------------------------------------------------------------------
//
TFSProgress CIpsPlgPop3ConnectOp::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    result.iError = KErrNone;
    switch( iState )
        {
        case EQueryingDetails:
        case EQueryingDetailsBusy:
            result.iProgressStatus = TFSProgress::EFSStatus_Authenticating;
            break;
        case EStartConnect:
        case EConnected:
            result.iProgressStatus = TFSProgress::EFSStatus_Connecting;
            break;
        case EPopulate:
            // staus is used to inform fetching in base plugin
            result.iProgressStatus = TFSProgress::EFSStatus_Status;
            break;
        default:
            result.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
            break;
        }
    if ( iStatus.Int() == KErrCancel )
        {
        result.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
        result.iError = KErrCancel;
        }
    
    return result;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::CIpsPlgPop3ConnectOp
// ----------------------------------------------------------------------------
//
CIpsPlgPop3ConnectOp::CIpsPlgPop3ConnectOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aServiceId,
    TBool aForcePopulate,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler,
    TBool aSignallingAllowed )
    :
    CIpsPlgOnlineOperation( aMsvSession, KConnectOpPriority,
        aObserverRequestStatus, aActivityTimer, aFSMailBoxId,
        aFSOperationObserver, aFSRequestId, aSignallingAllowed ),
    iState( EIdle ),
    iEntry( NULL ),
    iPopulateLimit( KIpsPlgPop3PopulateLimitInitValue ),
    iForcePopulate( aForcePopulate ),
    iSelection( NULL ),
    iEventHandler( aEventHandler ),
    iAlreadyConnected( EFalse )
    {
    iService = aServiceId; 
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypePOP3 );
   	
    iEntry = iMsvSession.GetEntryL( iService );
    if ( !iEntry )
        {
        User::Leave( KErrGeneral );
        }
    iSelection = new(ELeave) CMsvEntrySelection;
    const TMsvEntry& tentry = iEntry->Entry();
    
    if ( tentry.iType.iUid != KUidMsvServiceEntryValue )
        {
        // should we panic with own codes?
        User::Leave( KErrNotSupported );
        }
    
    // get correct populate and sync limet values from settings
    CImPop3Settings* settings = new(ELeave) CImPop3Settings();
    CleanupStack::PushL( settings );
    TPopAccount popAccountId;
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    accounts->GetPopAccountL( iService , popAccountId );
    accounts->LoadPopSettingsL( popAccountId, *settings );
    iPopulateLimit = settings->PopulationLimit();
    if ( iPopulateLimit > 0 )
        {
        iPopulateLimit = ( iPopulateLimit * 1024 ) / 75;
        }
    else if ( iPopulateLimit == KIpsSetDataHeadersOnly )    
        {
        iPopulateLimit = 0;
        }
    else
        {
        //GMail cannot support the TOP xx 2147483647
        //the max line for GMail is 99999999
        //Change the KMaxTInt to KIpsSetDataMaxLines
        iPopulateLimit = KIpsSetDataMaxLines;
        }
    
    CleanupStack::PopAndDestroy( 2, settings );
       
    if ( tentry.Connected() )
        {      
        iState = EConnected; 
        iAlreadyConnected = ETrue;
        SetActive();
        CompleteThis();
        }
    else
        {
        iState = EStartConnect; 
        SetActive();
        CompleteThis();
        }    
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::DoConnectL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoConnectL()
    {
    FUNC_LOG;
    iStatus = KRequestPending;
    InvokeClientMtmAsyncFunctionL( KPOP3MTMConnect, iService, iService );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::DoPopulateL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoPopulateL()
    {
    FUNC_LOG;
    iStatus = KRequestPending;

    // Prepare parameters and include filtering
    TImPop3PopulateOptions pop3GetMailInfo;
    pop3GetMailInfo.SetMaxEmailSize( KMaxTInt32 );
    pop3GetMailInfo.SetPopulationLimit( iPopulateLimit );
    TPckgBuf<TImPop3PopulateOptions> params( pop3GetMailInfo );

    iSelection->InsertL(0, iService);
    iBaseMtm->SwitchCurrentEntryL( iService );
    // Start the fetch operation
    InvokeClientMtmAsyncFunctionL( KPOP3MTMPopulateAll, *iSelection,
        iService, params);

    SetActive();

    if ( iEventHandler )
        {
        iEventHandler->SetNewPropertyEvent( 
                iService, KIpsSosEmailSyncStarted, KErrNone );
        } 
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TBool CIpsPlgPop3ConnectOp::Connected() const
    {
    FUNC_LOG;
    TMsvEntry tentry;
    TMsvId service;
    iMsvSession.GetEntry(iService, service, tentry );
    return tentry.Connected();
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::IpsOpType()
// ----------------------------------------------------------------------------
// 
TInt CIpsPlgPop3ConnectOp::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypePop3SyncOp;
    }


// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::ValidateL()
// ----------------------------------------------------------------------------
//
TBool CIpsPlgPop3ConnectOp::ValidateL()
    {
    // do proper validation. OR: not needed?
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::QueryUserPassL()
// ----------------------------------------------------------------------------
//
TBool CIpsPlgPop3ConnectOp::QueryUserPassL()
    {
    FUNC_LOG;

    if ( iEventHandler )
        {
        // ask for credentials for pop3 account and wait for callback
        if ( iEventHandler->QueryUsrPassL( iEntry->EntryId(), this ) )
            {
            iState = EQueryingDetails;
            }
        else
            {
            // another operation is waiting for password
            iState = EQueryingDetailsBusy;
            }

        return ETrue;
        }

    return EFalse;
    }


// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::GetOperationErrorCodeL()
// ----------------------------------------------------------------------------    
// 
TInt CIpsPlgPop3ConnectOp::GetOperationErrorCodeL( )
    {
    FUNC_LOG;
    
    if ( iAlreadyConnected )
        {
        // Connected state was set in CIpsPlgPop3ConnectOp::ConstructL()
        // so iOperation is null
        return KErrNone;
        }
        
    if ( !iOperation )
        {
        return KErrNotFound;
        }
    if ( !iOperation->IsActive() && iOperation->iStatus.Int() != KErrNone )
        {
        return iOperation->iStatus.Int();
        }
    
    TPckgBuf<TPop3Progress> paramPack;
    paramPack.Copy( iOperation->ProgressL() );
    const TPop3Progress& progress = paramPack();
    
    return progress.iErrorCode;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3ConnectOp::CredientialsSetL( TInt aEvent )
    {
    FUNC_LOG;

    if ( iState == EQueryingDetails )
        {
        // response for our operation`s query
        if ( aEvent == EIPSSosCredientialsCancelled )
            {
            // user canceled operation
            CompleteObserver( KErrCancel );
            }

        // password has been set, continue with operation
        SetActive();
        CompleteThis();
        }
    else if ( iState == EQueryingDetailsBusy )
        {
        // response for other operation`s query
        // we could try to ask for password now,
        // but decision was made to cancel operation
        CompleteObserver( KErrCancel );
        SetActive();
        CompleteThis();
        }
    }

void CIpsPlgPop3ConnectOp::DoDisconnectL()
    {
    FUNC_LOG;
    SetActive();
    InvokeClientMtmAsyncFunctionL( KPOP3MTMDisconnect, iService, iService );
    }

//EOF


