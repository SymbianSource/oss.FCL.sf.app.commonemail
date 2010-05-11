/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR command base class implementation
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmricalviewercanviewmr.h"
//<cmail>
#include "esmricalvieweropcodes.hrh"
#include "cfsmailcommon.h"
#include "cfsmailmessage.h"
//</cmail>

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerCanViewMsgCmd::CESMRIcalViewerCanViewMsgCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCanViewMsgCmd::CESMRIcalViewerCanViewMsgCmd(
        CCalSession& aCalSession,
        TBool& aFlag )
:   CESMRIcalViewerCommandBase( EESMRCanViewMR, aCalSession ),
    iFlag( aFlag )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCanViewMsgCmd::CESMRIcalViewerCanViewMsgCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCanViewMsgCmd::~CESMRIcalViewerCanViewMsgCmd()
    {
    FUNC_LOG;
    CancelCommand();
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCanViewMsgCmd::CESMRIcalViewerCanViewMsgCmd
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCanViewMsgCmd* CESMRIcalViewerCanViewMsgCmd::NewL(
            CCalSession& aCalSession,
            TBool& aFlag )
    {
    FUNC_LOG;
    CESMRIcalViewerCanViewMsgCmd* self =
            new (ELeave) CESMRIcalViewerCanViewMsgCmd(
                    aCalSession,
                    aFlag );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCanViewMsgCmd::CESMRIcalViewerCanViewMsgCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCanViewMsgCmd::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCanViewMsgCmd::CESMRIcalViewerCanViewMsgCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCanViewMsgCmd::ExecuteCommandL(
        CFSMailMessage& aMessage,
        MESMRIcalViewerObserver& aObserver )
    {
    FUNC_LOG;
    iFlag = aMessage.IsFlagSet( EFSMsgFlag_CalendarMsg );

    iResult.iOpType = OperationType();
    iResult.iResultCode = KErrNone;
    iResult.iMessage = &aMessage;

    aObserver.OperationCompleted( iResult );
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewerCanViewMsgCmd::CESMRIcalViewerCanViewMsgCmd
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCanViewMsgCmd::CancelCommand()
    {
    FUNC_LOG;
    //do nothing
    }


