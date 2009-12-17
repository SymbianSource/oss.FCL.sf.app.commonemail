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
* Description:  ESMR async command base class implementation
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmricalviewerasynccommand.h"
#include <calsession.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * ASync callback method.
 * @param aParam Poiter to async command object 
 */
TInt ASyncCallback( TAny* aParam )
	{
	CESMRIcalViewerAsyncCommand* asyncCommand = 
			static_cast<CESMRIcalViewerAsyncCommand*>( aParam );

    TRAPD( err, asyncCommand->ExecuteAsyncCommandL() );
    
    return err;
	}

}  // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerAsyncCommand::CESMRIcalViewerAsyncCommand
// -----------------------------------------------------------------------------
//
CESMRIcalViewerAsyncCommand::CESMRIcalViewerAsyncCommand(
			TESMRIcalViewerOperationType aType,
			CCalSession& aCalSession )
:	CESMRIcalViewerCommandBase( aType, aCalSession )
	{
    FUNC_LOG;
	
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerAsyncCommand::CESMRIcalViewerAsyncCommand
// -----------------------------------------------------------------------------
//
CESMRIcalViewerAsyncCommand::~CESMRIcalViewerAsyncCommand()
	{
    FUNC_LOG;
	delete iAsyncInvoker;
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerAsyncCommand::CESMRIcalViewerAsyncCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerAsyncCommand::ExecuteCommandL(
		CFSMailMessage& aMessage,
		MESMRIcalViewerObserver& aObserver )
	{
    FUNC_LOG;
	if ( !iAsyncInvoker )
		{
		TCallBack callback( ASyncCallback, this );
		iAsyncInvoker = new (ELeave) CAsyncCallBack(
				callback, EPriorityNormal );
		}
	
	SetMessage( &aMessage );
	SetObserver( &aObserver );
	
	// Invoking async command
	iAsyncInvoker->CallBack();
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerAsyncCommand::CESMRIcalViewerAsyncCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerAsyncCommand::CancelCommand()
	{
    FUNC_LOG;
	if ( iAsyncInvoker )
		{
		CancelAsyncCommand();
		}
	
	delete iAsyncInvoker; iAsyncInvoker = NULL; 
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerAsyncCommand::CESMRIcalViewerAsyncCommand
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerAsyncCommand::BaseConstructL()
	{
    FUNC_LOG;
	CESMRIcalViewerCommandBase::BaseConstructL();
	}

// EOF

