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
* Description: This file implements class CIpsSetUtilsDisconnectLogic.
*
*/


#include "emailtrace.h"
#include "ipssetutilsdisconnectlogic.h"

#include <SendUiConsts.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <e32des8.h>
#include <imapcmds.h>
#include <pop3cmds.h>
#include <msvstd.h>
#include <msvids.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <ipssossettings.rsg>


_LIT( KIpsSetUtilsSrvType, "SrvType");

#ifdef _DEBUG
_LIT( KIpsSetUtilsInitError, "InitError");
#endif

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsSetUtilsDisconnectLogic* CIpsSetUtilsDisconnectLogic::NewL( 
    CMsvSession& aSession, TMsvId aMailboxId )
    {
    FUNC_LOG;
    CIpsSetUtilsDisconnectLogic* self = 
        new( ELeave ) CIpsSetUtilsDisconnectLogic( 
                    aSession, aMailboxId );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsSetUtilsDisconnectLogic::CIpsSetUtilsDisconnectLogic( 
    CMsvSession& aSession, TMsvId aMailboxId ) : 
        CActive( EPriorityStandard ), iSession( aSession ), 
        iMailboxId( aMailboxId ), iIsDisconnectComplete( EFalse )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsSetUtilsDisconnectLogic::~CIpsSetUtilsDisconnectLogic()
    {
    FUNC_LOG;
    Cancel();
    delete iOperation;
    delete iAsyncWaitNote;
    delete iBaseMtm;
    delete iMtmReg;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsSetUtilsDisconnectLogic::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsSetUtilsDisconnectLogic::StepL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CIpsSetUtilsDisconnectLogic::IsProcessDone() const
    {
    FUNC_LOG;
    return iIsDisconnectComplete;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsSetUtilsDisconnectLogic::ProcessFinished()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsSetUtilsDisconnectLogic::DialogDismissedL( TInt /*aButtonId*/ )
    {
    FUNC_LOG;
    //EAknSoftkeyCancel when red key or cancel pressed
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CIpsSetUtilsDisconnectLogic::CycleError( TInt /*aError*/ )
    {
    FUNC_LOG;
    // called when StepL leaves
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsSetUtilsDisconnectLogic::DoCancel()
    {
    FUNC_LOG;
    if ( iOperation && iOperation->IsActive() )
        {
        iOperation->Cancel();
        }
    iIsDisconnectComplete = ETrue;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsSetUtilsDisconnectLogic::RunL()
    {
    FUNC_LOG;
    if ( iOperation && iOperation->iStatus.Int() != KErrNone )
        {
        // error notes?
        TInt error = iStatus.Int();
        }
    iIsDisconnectComplete = ETrue;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CIpsSetUtilsDisconnectLogic::RunError( TInt aError )
    {
    FUNC_LOG;
    iIsDisconnectComplete = ETrue;
    aError = KErrNone; 
    return aError;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CIpsSetUtilsDisconnectLogic::RunDisconnectLogicL( TBool aDoShowQuery )
    {
    FUNC_LOG;
    // NOTE: this not react "red key" same way as rest of FSEmail, when
    // redkey is pressed during progress bar, only focus is lost but application
    // not exits
    TInt error = KErrNone;
    TMsvEntry entry;
    error = CheckConnectionAndQueryDisconnectL( entry, aDoShowQuery );
    
    if ( error == KErrDisconnected )
        {
        // not connected
        return KErrNone;
        }
    else if ( error == KErrNone )
        {
        // connected and user want's to disconnect
        CreateDisconnectOperationL( entry );
        
        // show wait note
        iAsyncWaitNote = CAknWaitNoteWrapper::NewL();
        TBool result = iAsyncWaitNote->ExecuteL( 
                R_IPS_SETUI_WAIT_DISCONNECTING_DIALOG, *this );
        
        if ( !result )
            {
            error = KErrCancel;
            }
        }
    
    // only possible return codes are KErrNone and KErrCancel
    return error;
    }



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CIpsSetUtilsDisconnectLogic::CheckConnectionAndQueryDisconnectL( 
    TMsvEntry& aEntry, TBool aDoShowQuery )
    {
    FUNC_LOG;
    TMsvId service;
    User::LeaveIfError( iSession.GetEntry( iMailboxId, service, aEntry ) );
    
    TInt error = KErrNone;
    if ( !aEntry.Connected() )
        {
        error = KErrDisconnected;
        }
    else if ( aDoShowQuery )
        {
        HBufC* message = StringLoader::LoadLC( R_FSE_SETTINGS_MAIL_QUERY_DISCONNECT  );
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL(dlg);
        dlg->SetPromptL(*message);
        CleanupStack::Pop(dlg);
        TBool result = dlg->ExecuteLD( R_IPS_SETUI_CONFIRMATION_QUERY );
        CleanupStack::PopAndDestroy(message);
        if ( !result )
            {
            error = KErrCancel;
            }
        }
    return error;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsSetUtilsDisconnectLogic::CreateDisconnectOperationL( 
    const TMsvEntry& aEntry )
    {
    FUNC_LOG;
    // check mtm type and make sure that this function is not called 
    // twice without releasing data
    __ASSERT_ALWAYS(
            ( aEntry.iMtm.iUid == KSenduiMtmImap4UidValue 
            || aEntry.iMtm.iUid == KSenduiMtmPop3UidValue ), 
        User::Panic( KIpsSetUtilsSrvType, KErrGeneral ) );
    
    __ASSERT_DEBUG( ( !iAsyncWaitNote && !iMtmReg && !iBaseMtm ), 
        User::Panic( KIpsSetUtilsInitError, KErrGeneral ) );
    
    iMtmReg = CClientMtmRegistry::NewL( iSession );
    iBaseMtm = iMtmReg->NewMtmL( aEntry.iMtm );
    
    iBaseMtm->SwitchCurrentEntryL( aEntry.Id() );
    // create and invoke disconnect operation
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    sel->AppendL( aEntry.Id() );
    TBuf8<1> dummy;
    if ( aEntry.iMtm.iUid == KSenduiMtmImap4UidValue )
        {
        iOperation = iBaseMtm->InvokeAsyncFunctionL(
                KIMAP4MTMDisconnect, *sel, dummy, iStatus );
        }
    else
        {
        iOperation = iBaseMtm->InvokeAsyncFunctionL(
                KPOP3MTMDisconnect, *sel, dummy, iStatus );
        }
    CleanupStack::PopAndDestroy( sel );
    
    SetActive();
    }




