/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsSetUtilsOperationWait.
*
*/

// INCLUDE FILES
#include "emailtrace.h"
#include <aknenv.h>
#include <AknWaitDialog.h> // CAknWaitDialog 
#include <ipssossettings.rsg>

#include "ipssetutilsoperationwait.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUtilsOperationWait* CIpsSetUtilsOperationWait::NewLC( 
    TInt aPriority )
    {
    FUNC_LOG;
    CIpsSetUtilsOperationWait* self = 
        new ( ELeave ) CIpsSetUtilsOperationWait( aPriority );
    CleanupStack::PushL( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::CIpsSetUtilsOperationWait()
// ----------------------------------------------------------------------------
//
CIpsSetUtilsOperationWait::CIpsSetUtilsOperationWait( TInt aPriority )
    : 
    CActive( aPriority )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::~CIpsSetUtilsOperationWait()
// ----------------------------------------------------------------------------
//
CIpsSetUtilsOperationWait::~CIpsSetUtilsOperationWait()
    {
    FUNC_LOG;
    
    delete iWaitDialog;
    iWaitDialog = NULL;
    Cancel();
    }

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::Start()
// ----------------------------------------------------------------------------
//
void CIpsSetUtilsOperationWait::Start()
    {
    FUNC_LOG;
    SetActive();
    iWait.Start();
    }

// <cmail>
// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::SimpleSetActive()
// ----------------------------------------------------------------------------
//
void CIpsSetUtilsOperationWait::StartAndShowWaitDialogL()
    {
    FUNC_LOG;	
    SetActive();
    StartWaitDialogL(); 
    iWait.Start();
    }
// </cmail>

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::RunL()
// ----------------------------------------------------------------------------
//
void CIpsSetUtilsOperationWait::RunL()
    {
    FUNC_LOG;
    if ( iWaitDialog )
    	{
    	StopWaitDialogL();
    	}

    if ( iWait.IsStarted() )
    	{
    	CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
    	}
    }

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::DoCancel()
// ----------------------------------------------------------------------------
//
void CIpsSetUtilsOperationWait::DoCancel()
    {
    FUNC_LOG;
    if( iStatus == KRequestPending )
        {
        TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrCancel );
        }
    if ( iWaitDialog )
    	{
    	TRAP_IGNORE( StopWaitDialogL() );
    	}

    CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
    }

// <cmail>
// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::DialogDismissedL()
// ----------------------------------------------------------------------------
//
void CIpsSetUtilsOperationWait::DialogDismissedL(TInt /*aButtonId*/ )
	{
	FUNC_LOG;
	iWaitDialog = NULL;
	Cancel();
	}

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::StartWaitDialog()
// ----------------------------------------------------------------------------
//
void CIpsSetUtilsOperationWait::StartWaitDialogL()
	{
	FUNC_LOG;
	if( iWaitDialog )
		{
		delete iWaitDialog;
		iWaitDialog = NULL;
		}

	iWaitDialog = new(ELeave)CAknWaitDialog(
	                        (REINTERPRET_CAST(CEikDialog**,&iWaitDialog)));
	iWaitDialog->SetCallback(this);
	iWaitDialog->ExecuteLD( R_IPS_SETUI_WAIT_REFRESH_FOLDER_LIST_DIALOG );

	}

// ----------------------------------------------------------------------------
// CIpsSetUtilsOperationWait::StopWaitDialog()
// ----------------------------------------------------------------------------
//
void CIpsSetUtilsOperationWait::StopWaitDialogL()
	{
	FUNC_LOG;
	if ( iWaitDialog )
		{
		iWaitDialog->ProcessFinishedL();
		delete iWaitDialog;
		iWaitDialog = NULL;
		}
	}
// </cmail>
// End of file

