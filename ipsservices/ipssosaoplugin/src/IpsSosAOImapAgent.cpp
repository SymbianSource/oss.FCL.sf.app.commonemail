/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Contains imap specified sync logic
*
*/


#include "emailtrace.h"
#include <cemailaccounts.h>
#include <imapcmds.h>
#include <miutset.h>
#include <impcmtm.h>
#include <msvapi.h>
#include <AlwaysOnlineManagerCommon.h>
//<cmail>
#include "cfsmailcommon.h"
//</cmail>


#include "IpsSosAOImapAgent.h"
#include "IpsSosAOImapPopLogic.h"


// from settings
#include "ipssetdataapi.h"
#include "ipssetutilsconsts.h"

// from ipsplugin
#include "ipsplgimap4populateop.h"
#include "ipsplgcommon.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOImapAgent::CIpsSosAOImapAgent(             
    CMsvSession& aSession,
    MIpsSosAOAgentOperationResponse& aOpResponse,
    TMsvId aServiceId) : CIpsSosAOBaseAgent(), 
    iSession(aSession), iOpResponse(aOpResponse),
    iServiceId( aServiceId ),
    iState( EStateIdle ), iDoNotDisconnect( EFalse )
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOImapAgent::~CIpsSosAOImapAgent()
    {
    FUNC_LOG;
    Cancel();
    SignalSyncCompleted( iServiceId, iError );
    ClearSignaledFlags();
    delete iImapSettings;
    delete iOngoingOp;
    delete iImapClientMtm;
    delete iMtmReg;
    delete iDataApi;
    iFoldersArray.Close();

    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOImapAgent* CIpsSosAOImapAgent::NewL( 
    CMsvSession& aSession,
    MIpsSosAOAgentOperationResponse& aOpResponse,
    TMsvId aServiceId )
    {
    FUNC_LOG;
    CIpsSosAOImapAgent* self = new (ELeave) CIpsSosAOImapAgent( 
            aSession,  aOpResponse, aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    
    iMtmReg = CClientMtmRegistry::NewL( iSession );
    CBaseMtm* bmtm = iMtmReg->NewMtmL( KUidMsgTypeIMAP4 );
    iImapClientMtm = static_cast<CImap4ClientMtm*>(bmtm);
    iDataApi = CIpsSetDataApi::NewL( iSession );
    iState = EStateIdle; 
    }
 
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::DoCancel()
    {
    FUNC_LOG;
    if (iOngoingOp )
        {
        iOngoingOp->Cancel();
        }
    delete iOngoingOp;
    iOngoingOp = NULL;
    
    SignalSyncCompleted( iServiceId, iError );
    ClearSignaledFlags();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::HandleImapConnectionEvent(
    TImapConnectionEvent aConnectionEvent )
    {
    FUNC_LOG;
    switch ( aConnectionEvent )
        {
        case EConnectingToServer:
            INFO( "CIpsSosAOImapAgent::HandleImapConnectionEvent EConnectingToServer" );
            break;
        case ESynchronisingFolderList:
            INFO( "CIpsSosAOImapAgent::HandleImapConnectionEvent ESynchronisingFolderList" );
            SignalSyncStarted( iServiceId );
            break;
        case ESynchronisingInbox:
            INFO( "CIpsSosAOImapAgent::HandleImapConnectionEvent ESynchronisingInbox" );
            SignalSyncStarted( iServiceId );
            break;
        case ESynchronisingFolders:
            INFO( "CIpsSosAOImapAgent::HandleImapConnectionEvent ESynchronisingFolders" );
            SignalSyncStarted( iServiceId );
            break;
        case ESynchronisationComplete:
            INFO( "CIpsSosAOImapAgent::HandleImapConnectionEvent ESynchronisationComplete" );
            break;
        case EDisconnecting:
            INFO( "CIpsSosAOImapAgent::HandleImapConnectionEvent EDisconnecting" );
            break;
        case EConnectionCompleted:
            INFO( "CIpsSosAOImapAgent::HandleImapConnectionEvent EConnectionCompleted" );
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::RequestResponseL( 
        TFSProgress /*aEvent*/, 
        TInt /*aRequestId*/ )
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::RunL()
    {
    FUNC_LOG;
    if ( iOngoingOp )
        {
        // errors are not tolerated atm
        if (  iOngoingOp->iStatus.Int() != KErrNone )
            {
            iError = iOngoingOp->iStatus.Int();
            INFO( "CIpsSosAOImapAgent::RunL operation completed with error" );
            INFO_1( "CIpsSosAOImapAgent::RunL error: %d" , iError);
            SignalSyncCompleted( iServiceId, iError );
            CancelAllAndDisconnectL();
            }
        delete iOngoingOp;
        iOngoingOp = NULL;
        }
    
    switch( iState )
         {
         case EStateConnectAndSync:
             INFO( "CIpsSosAOImapAgent::RunL EStateConnectAndSync" );
             iState = EStateRefreshFolderArray;
             SetActiveAndCompleteThis();
             break;
         case EStateConnectAndSyncOnHold:
             INFO( "CIpsSosAOImapAgent::RunL EStateConnectAndSyncOnHold" );
             StartSyncL();
             break;
         case EStateRefreshFolderArray:
             INFO( "CIpsSosAOImapAgent::RunL EStateRefreshFolderArray" );
             iDataApi->GetSubscribedImapFoldersL( iServiceId , iFoldersArray );
             iState = EStatePopulateAll;
             SetActiveAndCompleteThis();
             break;
         case EStatePopulateAll:
             INFO( "CIpsSosAOImapAgent::RunL EStatePopulateAll" );
             PopulateAllL();
             break;
         case EStatePopulateOnHold:
             INFO( "CIpsSosAOImapAgent::RunL EStatePopulateOnHold" );
             PopulateAllL();
             break;
         case EStateFetchOngoing:
             INFO( "CIpsSosAOImapAgent::RunL EStateFetchOngoing" );
             break;
         case EStateFetchOnHold:
             INFO( "CIpsSosAOImapAgent::RunL EStateFetchOnHold" );
             break;
         case EStateDisconnect:
             INFO( "CIpsSosAOImapAgent::RunL EStateDisconnect" );
             if ( !iDoNotDisconnect )
                 {
                 CancelAllAndDisconnectL();
                 }
             else
                 {
                 iState = EStateCompleted;
                 SetActiveAndCompleteThis();
                 }
             break;
         case EStateCompleted:
             INFO( "CIpsSosAOImapAgent::RunL EStateCompleted" );
             TRAP_IGNORE( iOpResponse.OperationCompletedL( iError ) );
             SignalSyncCompleted( iServiceId, iError );
             iError = KErrNone;
             ClearSignaledFlags();
             iState = EStateIdle;
             break;
         case EStateIdle:
             INFO( "CIpsSosAOImapAgent::RunL EStateIdle" );
             break;
         default:
         break;
  
         }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOImapAgent::RunError( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    if ( IsConnected() && iState != EStateDisconnect )
        {
        TRAP_IGNORE( CancelAllAndDisconnectL() );
        }
    else if ( IsConnected() && iState == EStateDisconnect )
        {
        iSession.StopService( iServiceId );
        iState = EStateCompleted;
        SetActiveAndCompleteThis();
        }
    else
        {
        iState = EStateCompleted;
        iError = KErrCancel;
        SetActiveAndCompleteThis();
        }
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::GetServerAddress( 
        TBuf<KIpsSosAOTextBufferSize>& aIncomingServer ) const
    {
    FUNC_LOG;
    if ( !iImapSettings )
        {
        return;
        }
    TPtrC addr = iImapSettings->ServerAddress();
    __ASSERT_DEBUG( ( 
            addr.Length() <= KIpsSosAOTextBufferSize ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    if ( addr.Length() <= KIpsSosAOTextBufferSize )
        {
        aIncomingServer.Copy( addr );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::GetUsername( 
        TBuf8<KIpsSosAOTextBufferSize>& aUsername ) const
    {
    FUNC_LOG;
    if ( !iImapSettings )
        {
        return;
        }
    TPtrC8 usrn = iImapSettings->LoginName();
    __ASSERT_DEBUG( ( 
            usrn.Length() <= KIpsSosAOTextBufferSize ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    if ( usrn.Length() <= KIpsSosAOTextBufferSize )
        {
        aUsername.Copy( usrn );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOImapAgent::IsConnected() const
    {
    FUNC_LOG;
    TMsvEntry tentry;
    TMsvId service;
    iSession.GetEntry( iServiceId, service, tentry );
    return tentry.Connected();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOBaseAgent::TAgentState CIpsSosAOImapAgent::GetState() const
    {
    FUNC_LOG;
    return iState;
    }

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
void CIpsSosAOImapAgent::StartSyncL()
    {
    FUNC_LOG;
    if ( iOngoingOp )
        {
        User::Leave( KErrNotReady );
        }
    LoadSettingsL( );
    if ( !IsConnected() )
        {
        if ( IsActive() )
            {
            Cancel();
            }
        TPckg<MMsvImapConnectionObserver*> parameter(this);
        // connect and synchronise starts background sync or idle
        CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( sel );
        sel->AppendL( iServiceId );
        iImapClientMtm->SwitchCurrentEntryL( iServiceId );
        iStatus = KRequestPending;
        iOngoingOp = iImapClientMtm->InvokeAsyncFunctionL(
                KIMAP4MTMConnectAndSyncCompleteAfterFullSync, 
                *sel, parameter, iStatus);
        CleanupStack::PopAndDestroy( sel );
        SetActive();
        iState = EStateConnectAndSync;
        }
    else
        {
        // do not do anything if we are connected, especially do never
        // try to sync if sync is is already started (ex. from ips plugin)
        // that cause problems with imap flags etc.
        iError = KErrNone;
        iState = EStateCompleted;
        SetActiveAndCompleteThis();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::StartFetchMessagesL( 
        const RArray<TMsvId>& /*aFetchMsgArray*/ )
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::CancelAllAndDisconnectL()
    {
    FUNC_LOG;
    // if we are already idle state, do nothing,
    // completing in idle state might cause unvanted events to ui
    bool isConnected = IsConnected();
    if ( !isConnected && iState == EStateIdle) 
        {
        return;
        }
    iDoNotDisconnect = EFalse;
    iState = EStateCompleted;
    iFoldersArray.Reset();

    if ( IsActive() )
        {
        Cancel();
        }

    if ( isConnected )
        {
        TBuf8<1> dummy;
        CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( sel );
        sel->AppendL( iServiceId );
        iImapClientMtm->SwitchCurrentEntryL( iServiceId );
        TRAPD( error, iOngoingOp = iImapClientMtm->InvokeAsyncFunctionL(
            KIMAP4MTMDisconnect, 
            *sel, dummy, iStatus) );
        CleanupStack::PopAndDestroy( sel );
        
        if ( error == KErrNone )
            {
            SetActive();
            }
        else
            {
            iSession.StopService( iServiceId );
            SetActiveAndCompleteThis();
            }
        }
    else
        {
        SetActiveAndCompleteThis();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::CancelAllAndDoNotDisconnect()
    {    
    FUNC_LOG;
    // if we are already idle state, do nothing,
    // completing in idle state might cause unvanted events to ui
    if (iState == EStateIdle) 
        {
        return;
        }
    iDoNotDisconnect = ETrue;
    if ( IsActive() )
        {
        Cancel();
        }
    iFoldersArray.Reset();
    SignalSyncCompleted( iServiceId, KErrCancel );
    iState = EStateCompleted;
    SetActiveAndCompleteThis();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::DoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = ETrue;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::ClearDoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::HoldOperations()
    {
    FUNC_LOG;
    if ( IsActive() )
        {
        Cancel();
        }
    
    if ( iState == EStatePopulateAll )
        {
        iState = EStatePopulateOnHold;
        }
    else if ( iState == EStateFetchOngoing )
        {
        iState = EStateFetchOnHold;
        }
    else if ( iState == EStateConnectAndSync )
        {
        iState = EStateConnectAndSyncOnHold;
        }
    else 
        {
        iState = EStateIdle;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::ContinueHoldOperations()
    {
    FUNC_LOG;
    SetActiveAndCompleteThis();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::PopulateAllL()
    {
    FUNC_LOG;
    TImImap4GetPartialMailInfo info;
    CIpsSetDataApi::ConstructImapPartialFetchInfo( info, *iImapSettings );
    TBool syncStarting = EFalse;
    
    if ( !IsConnected() )
        {
        SignalSyncCompleted( iServiceId, iError );
        CancelAllAndDisconnectL();
        }
    if ( iFoldersArray.Count() > 0 && info.iTotalSizeLimit 
            != KIpsSetDataHeadersOnly )
         {
         CMsvEntry* cEntry = iSession.GetEntryL( iServiceId );
         CleanupStack::PushL( cEntry );
         CMsvEntrySelection* childrenSelection = cEntry->ChildrenL();
         CleanupStack::PushL( childrenSelection );
         if ( childrenSelection->Count() )
             {
             TPckgBuf<TImImap4GetPartialMailInfo> package(info);
             // only inbox is set, do we have to populate other folders also
             TMsvId inboxId = (*childrenSelection)[0];
             CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
             CleanupStack::PushL( sel );
             sel->AppendL(iServiceId);
             sel->AppendL(inboxId);
             iStatus = KRequestPending;
             iOngoingOp = iImapClientMtm->InvokeAsyncFunctionL(
                     KIMAP4MTMPopulateAllMailWhenAlreadyConnected, 
                     *sel, package, iStatus);
             SetActive();
             syncStarting = ETrue;
             SignalSyncStarted( iServiceId );
             CleanupStack::PopAndDestroy( sel );
             }
         CleanupStack::PopAndDestroy( 2, cEntry );
         
         }
     if (!syncStarting)
         {
         SignalSyncStarted( iServiceId );
         SetActiveAndCompleteThis();
         }
    iState = EStateDisconnect;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::LoadSettingsL( )
    {
    FUNC_LOG;
    delete iImapSettings;
    iImapSettings = NULL;
    iImapSettings = new ( ELeave ) CImImap4Settings();
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TImapAccount imapAcc;
    accounts->GetImapAccountL(iServiceId, imapAcc );
    accounts->LoadImapSettingsL( imapAcc, *iImapSettings );
    CleanupStack::PopAndDestroy( accounts );
    }

// ----------------------------------------------------------------------------
// inline
// ----------------------------------------------------------------------------    
void CIpsSosAOImapAgent::SetActiveAndCompleteThis()
    {
    FUNC_LOG;
    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        Cancel();
        SetActive();
        }
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }
