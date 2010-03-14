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
* Description:  IMAP4 connect operation
*
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"


_LIT( KIpsPlgIpsConnPanic, "IpsConn" ); 

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::NewL()
// ----------------------------------------------------------------------------
// 
CIpsPlgImap4ConnectOp* CIpsPlgImap4ConnectOp::NewL(
    CMsvSession& aMsvSession,
	TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler,
    TBool aDoPlainConnect,
    TBool aSignallingAllowed )
    {
    FUNC_LOG;
    CIpsPlgImap4ConnectOp* self = new (ELeave) CIpsPlgImap4ConnectOp(
        aMsvSession, 
        aPriority, 
        aObserverRequestStatus,
        aService, 
        aActivityTimer,
        aFSMailBoxId, 
        aFSOperationObserver,
        aFSRequestId,
        aDoPlainConnect,
        aSignallingAllowed,
        aEventHandler );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::CIpsPlgImap4ConnectOp()
// ----------------------------------------------------------------------------
// 
CIpsPlgImap4ConnectOp::CIpsPlgImap4ConnectOp(
    CMsvSession& aMsvSession,
	TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId,
    TBool aDoPlainConnect,
    TBool aSignallingAllowed,
    CIpsPlgEventHandler* aEventHandler)
    :
    CIpsPlgOnlineOperation(
	    aMsvSession,
	    aPriority,
	    aObserverRequestStatus,
	    aActivityTimer,
	    aFSMailBoxId,
	    aFSOperationObserver,
	    aFSRequestId,
    	aSignallingAllowed ),
    iDoPlainConnect( aDoPlainConnect ),
    iEventHandler( aEventHandler ),
    iIsSyncStartedSignaled( EFalse ),
    iAlreadyConnected( EFalse )
    {
    FUNC_LOG;
    iService = aService;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::~CIpsPlgImap4ConnectOp()
// ----------------------------------------------------------------------------
// 
CIpsPlgImap4ConnectOp::~CIpsPlgImap4ConnectOp()
    {
    FUNC_LOG;
    Cancel();
    delete iSelection;
    iState = EStateIdle;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::ConstructL()
// ----------------------------------------------------------------------------
// 
void CIpsPlgImap4ConnectOp::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypeIMAP4 ); 
    iSelection = new(ELeave) CMsvEntrySelection;
    
    TMsvEntry tentry;
    TMsvId service;
    iMsvSession.GetEntry( iService, service, tentry );
    
    if ( tentry.iType.iUid != KUidMsvServiceEntryValue )
        {
        User::Panic( KIpsPlgIpsConnPanic, KErrNotSupported );
        }
    
    if ( tentry.Connected() )
        {      
        iState = EStateCompleted; 
        iAlreadyConnected = ETrue;
        SetActive();
        CompleteThis();
        }
    else
        {
        iState = EStateStartConnect;
        iStatus = KRequestPending;    
        SetActive();
       CompleteThis();
        }    
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgImap4ConnectOp::ProgressL()
    {
    FUNC_LOG;
    if( iError != KErrNone )
        {
        return GetErrorProgressL( iError );
        }
    else if(iOperation)
        {
        return iOperation->ProgressL();
        }
        
    TImap4CompoundProgress& prog = iProgressBuf();
    prog.iGenericProgress.iErrorCode = KErrNone;
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::GetErrorProgressL()
// ----------------------------------------------------------------------------    
//   
const TDesC8& CIpsPlgImap4ConnectOp::GetErrorProgressL(TInt aError)
    {
    FUNC_LOG;
    TImap4CompoundProgress& prog = iProgressBuf();
    prog.iGenericProgress.iErrorCode = aError;      
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::GetFSProgressL()
// ----------------------------------------------------------------------------
// 
TFSProgress CIpsPlgImap4ConnectOp::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    result.iError = KErrNone;
    switch( iState )
        {
        case EStateQueryingDetails:
        case EStateQueryingDetailsBusy:
            result.iProgressStatus = TFSProgress::EFSStatus_Authenticating;
            break;
        case EStateStartConnect:
            result.iProgressStatus = TFSProgress::EFSStatus_Started;
            break;
        case EStateConnectAndSync:
            result.iProgressStatus = TFSProgress::EFSStatus_Connecting;
            break;
        case EStateCompleted:
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
// CIpsPlgImap4ConnectOp::IpsOpType()
// ----------------------------------------------------------------------------
// 
TInt CIpsPlgImap4ConnectOp::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeImap4SyncOp;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::Connected()
// ----------------------------------------------------------------------------
// 
TBool CIpsPlgImap4ConnectOp::Connected() const
    {
    FUNC_LOG;
    TMsvEntry tentry;
    TMsvId service;
    iMsvSession.GetEntry(iService, service, tentry );
    return tentry.Connected();
    }


// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoCancel()
// ----------------------------------------------------------------------------
// 
void CIpsPlgImap4ConnectOp::DoCancel()
    {
    FUNC_LOG;
    if( iOperation )
        {
        iOperation->Cancel();
        }
    SignalSyncCompleted( KErrCancel );
    CompleteObserver( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoRunL()
// ----------------------------------------------------------------------------    
//  
void CIpsPlgImap4ConnectOp::DoRunL()
    {
    FUNC_LOG;
    TInt err = KErrNone;
    __ASSERT_DEBUG( !(iOperation && iOperation->IsActive()), 
            User::Panic( KIpsPlgPanicCategory, KErrGeneral ) );
    if ( iOperation )
        {
        err = iOperation->iStatus.Int();
        delete iOperation;
        iOperation = NULL;
        }
    
    switch( iState )
        {
        case EStateQueryingDetails:
            // querying pass finished - try to reconnect
            DoConnectOpL();
            break;
        case EStateStartConnect:
        	StartL();
        	break;
        case EStateConnectAndSync:
            // Connection completed

            if ( err == KErrImapBadLogon )
                {
                // Login details are wrong. Trying to ask for password
                if ( QueryUserPassL() )
                    {
                    err = KErrNone;
                    }
                }
            else if ( err == KErrNone )
                {
                // no errors in connection
                if( !iDoPlainConnect )
                    {
                    DoPopulateAllL();
                    }
                else
                    {
                    // Get on with others using this class for connection only
                    iState = EStateIdle;
                    SetActive();
                    CompleteThis();
                    }
                }
            break;
        case EStatePopulateAllCompleted:
            CIpsPlgSyncStateHandler::SaveSuccessfulSyncTimeL(
                    iMsvSession, iService );
            // break command is intentially left out
        case EStateCompleted:
            if ( err == KErrNone )
                {
                iState = EStateIdle;
                SetActive();
                CompleteThis();
                }
            break;
        case EStateIdle:
        default:
            if ( iOperation )
                {
                delete iOperation;
                iOperation = NULL;
                }
            CompleteObserver();
            break;
        }
   
    // if iError < 0, observer is completed in base class
    iError = err;
    if ( err != KErrNone )
        {
        SignalSyncCompleted( err );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::HandleImapConnectionEvent()
// ----------------------------------------------------------------------------
// 
void CIpsPlgImap4ConnectOp::HandleImapConnectionEvent(
    TImapConnectionEvent aConnectionEvent )
    {
    FUNC_LOG;
    switch ( aConnectionEvent )
        {
        case EConnectingToServer:
            break;
        case ESynchronisingFolderList:
        case ESynchronisingInbox:
		case ESynchronisingFolders:
		    // send sync started event in any of these sync events
		    SignalSyncStarted();
		    break;
		case ESynchronisationComplete:
		    break;
		case EDisconnecting:
		    break;
		case EConnectionCompleted:
	    default:
	        break;
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::RequestResponseL()
// ----------------------------------------------------------------------------    
//   
void CIpsPlgImap4ConnectOp::RequestResponseL( 
    TFSProgress /*aEvent*/, TInt /*aRequestId*/ )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoConnectOpL()
// ----------------------------------------------------------------------------    
//   
void CIpsPlgImap4ConnectOp::DoConnectOpL()
	{
    FUNC_LOG;
    iBaseMtm->SwitchCurrentEntryL( iService );

    iSelection->ResizeL(0);
    iSelection->AppendL(iService);
    
    if ( iDoPlainConnect && !Connected() )
        {
        TBuf8<1> parameter;
        iStatus = KRequestPending;
        // connect and synchronise starts background sync or idle
        iOperation = iBaseMtm->InvokeAsyncFunctionL(
                KIMAP4MTMConnect, *iSelection, parameter, iStatus);
        iState = EStateConnectAndSync;
        SetActive();
        }
    else if ( Connected() )
        {
        // in this point cant use "connect and do something" commands,
        // use regular sync, when new mails is populated elsewhere.
        TBuf8<1> parameter;
        iStatus = KRequestPending;
        iOperation = iBaseMtm->InvokeAsyncFunctionL(
                KIMAP4MTMFullSync, *iSelection, parameter, iStatus);
        // also set sync started
        SignalSyncStarted();
        iState = EStateConnectAndSync;
        SetActive();
        }
    else
        {
        TPckg<MMsvImapConnectionObserver*> parameter(this);
        // connect and synchronise starts background sync or idle
        iStatus = KRequestPending;
        iOperation = iBaseMtm->InvokeAsyncFunctionL(
                KIMAP4MTMConnectAndSyncCompleteAfterFullSync, 
                *iSelection, parameter, iStatus);
        iState = EStateConnectAndSync;
        SetActive();
        }
	}

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoPopulateAll()
// ---------------------------------------------------------------------------- 
void CIpsPlgImap4ConnectOp::DoPopulateAllL()
    {
    FUNC_LOG;
    
    // construct partial fetch info according to imap settings
    CImImap4Settings* settings = new ( ELeave ) CImImap4Settings();
    CleanupStack::PushL( settings );
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TImapAccount imapAcc;
    accounts->GetImapAccountL(iService, imapAcc );
    accounts->LoadImapSettingsL( imapAcc, *settings );
    TImImap4GetPartialMailInfo info;
    CIpsSetDataApi::ConstructImapPartialFetchInfo( info, *settings );
    TPckgBuf<TImImap4GetPartialMailInfo> package(info);
    CleanupStack::PopAndDestroy( 2, settings );
    
    if ( info.iTotalSizeLimit != KIpsSetDataHeadersOnly )
        {
        SignalSyncStarted();
        CMsvEntry* cEntry = iMsvSession.GetEntryL( iService );
        CleanupStack::PushL( cEntry );
        CMsvEntrySelection* childrenSelection = cEntry->ChildrenL();
        CleanupStack::PushL( childrenSelection );
        if ( childrenSelection->Count() )
            {
            // only inbox is set, do we have to populate other folders also
            TMsvId id = (*childrenSelection)[0];
            CMsvEntry* cEntry2 = iMsvSession.GetEntryL( id );
            CleanupStack::PushL( cEntry2 );
            delete iSelection;
            iSelection = NULL;
            iSelection = cEntry2->ChildrenWithTypeL( KUidMsvMessageEntry );
            CleanupStack::PopAndDestroy( cEntry2 );
            
            }
        CleanupStack::PopAndDestroy( childrenSelection );
    
        iStatus = KRequestPending;
        CIpsPlgTimerOperation* dummy = NULL;
        iBaseMtm->SwitchCurrentEntryL( iService );
        iOperation = CIpsPlgImap4PopulateOp::NewL(
                iMsvSession,
                this->iStatus,
                CActive::EPriorityLow,
                iService,
                *dummy,
                info,
                *iSelection,
                iFSMailboxId,
                *this,
                0,
                iEventHandler );
        
        SetActive();
        CleanupStack::PopAndDestroy( cEntry );
        }
    else
        {
        SetActive();
        CompleteThis();
        }
    iState = EStatePopulateAllCompleted;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::StartL()
// ----------------------------------------------------------------------------    
// 
void CIpsPlgImap4ConnectOp::StartL()
    {
    FUNC_LOG;
    if ( Connected() && iDoPlainConnect )
        {
        // sync is done background, no explicit supported
        iState = EStateIdle;
        SetActive();
        CompleteThis();
        }
    else 
        {
        DoConnectOpL();
        }
    }    

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::QueryUserPwdL()
// ----------------------------------------------------------------------------
// 
TBool CIpsPlgImap4ConnectOp::QueryUserPassL()
    {
    if ( iEventHandler )
        {
        // ask for credentials for imap account and wait for callback
        if ( iEventHandler->QueryUsrPassL( iService, this ) )
            {
            iState = EStateQueryingDetails;
            }
        else
            {
            // another operation is waiting for password
            iState = EStateQueryingDetailsBusy;
            }

        return ETrue;
        }

    return EFalse;
    }

    
// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::GetOperationErrorCodeL()
// ----------------------------------------------------------------------------    
//     
/*TInt CIpsPlgImap4ConnectOp::GetOperationErrorCodeL( )
    {
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
     
    TPckgBuf<TImap4CompoundProgress> paramPack;
    paramPack.Copy( iOperation->ProgressL() );
    const TImap4CompoundProgress& comProgg = paramPack();

    return comProgg.iGenericProgress.iErrorCode;
    }*/

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------       
void CIpsPlgImap4ConnectOp::SignalSyncStarted()
    {
    FUNC_LOG;
    if ( iEventHandler && !iIsSyncStartedSignaled )
        {
        // mark that sync is signaled to prevent 
        // sending necessary event
        iIsSyncStartedSignaled = ETrue;
        iEventHandler->SetNewPropertyEvent( 
                iService, KIpsSosEmailSyncStarted, KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------       
void CIpsPlgImap4ConnectOp::SignalSyncCompleted( TInt aError )
    {
    FUNC_LOG;
    if ( iEventHandler && aError == KErrImapBadLogon )
        {
        /*
        iEventHandler->SetNewPropertyEvent( 
                iService, KIpsSosEmailSyncCompleted, aError );
        */
        iIsSyncStartedSignaled = EFalse;
        }
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4ConnectOp::CredientialsSetL( TInt aEvent )
    {
    FUNC_LOG;

    if ( iState == EStateQueryingDetails )
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
    else if ( iState == EStateQueryingDetailsBusy )
        {
        // response for other operation`s query
        // we could try to ask for password now,
        // but decision was made to cancel operation
        CompleteObserver( KErrCancel );
        SetActive();
        CompleteThis();
        }
    }
// End of File

