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
* Description:  ESMR UI launcher object implementation
*
*/


#include "emailtrace.h"
#include "cesmruilauncher.h"
#include "tesmrinputparams.h"
#include "cesmrviewerctrl.h"

#include <utf.h>
#include <CAgnEntryUi.h>
#include <cmrmailboxutils.h>
#include <CalenInterimUtils2.h>
//<cmail>
#include "FreestyleEmailUiConstants.h"
#include "CFSMailMessage.h"
#include "fsmtmsuids.h"
#include <coemain.h>
#include <coeaui.h>
#include "esmricalvieweropcodes.hrh"
//</cmail>

// Unnamed namespace for local definitions
namespace {

/**
 * Test if a calendar entry can be shown in mr viewer
 *
 * @param aEntry Reference to calendar entry.
 * @return ETrue if this entry can be shown in viewer 
 */
TBool CanBeShownInMRViewerL(
        const CCalEntry& aEntry )
    {
    TBool retValue( EFalse );
    CCalEntry::TMethod method( aEntry.MethodL() );

    if ( CCalEntry::EMethodRequest == method ||
         CCalEntry::EMethodReply == method ||         
         CCalEntry::EMethodCancel == method )
        {
        retValue = ETrue;
        }
    return retValue;
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRUiLauncher::CESMRUiLauncher
// ---------------------------------------------------------------------------
//
inline CESMRUiLauncher::CESMRUiLauncher(
        CCalSession& aCalSession,
        TESMRInputParams& aInputParameters,
        MAgnEntryUiCallback& aAgnCallback )
:   CESMRIcalViewerAsyncCommand( EESMRLaunchUI, aCalSession ),
    iInputParameters( aInputParameters ),
    iAgnCallback( aAgnCallback )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::~CESMRUiLauncher
// ---------------------------------------------------------------------------
//
CESMRUiLauncher::~CESMRUiLauncher()
    {
    FUNC_LOG;
    delete iViewerController; iViewerController = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::NewL
// ---------------------------------------------------------------------------
//
CESMRUiLauncher* CESMRUiLauncher::NewL(
        CCalSession& aCalSession,
        TESMRInputParams& aInputParameters,
        MAgnEntryUiCallback& aAgnCallback  )
    {
    FUNC_LOG;
    CESMRUiLauncher* self =
            NewLC(
                    aCalSession,
                    aInputParameters,
                    aAgnCallback );

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::NewLC
// ---------------------------------------------------------------------------
//
CESMRUiLauncher* CESMRUiLauncher::NewLC(
        CCalSession& aCalSession,
        TESMRInputParams& aInputParameters,
        MAgnEntryUiCallback& aAgnCallback )
    {
    FUNC_LOG;
    CESMRUiLauncher* self =
            new (ELeave) CESMRUiLauncher(
                    aCalSession,
                    aInputParameters,
                    aAgnCallback );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRUiLauncher::ConstructL()
    {
    FUNC_LOG;
    iResult.iOpType     = OperationType();
    iResult.iResultCode = KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::ExecuteAsyncCommandL
// ---------------------------------------------------------------------------
//
void CESMRUiLauncher::ExecuteAsyncCommandL()
    {
    FUNC_LOG;
    iResult.iMessage = Message();

    TRAP( iResult.iResultCode, ExecuteAsyncCommandInternalL() );

    if ( KErrArgument == iResult.iResultCode )
        {
        iResult.iResultCode = KErrNotSupported;
        Observer()->OperationError( iResult );
        }
    else if ( KErrNone != iResult.iResultCode )
        {
        Observer()->OperationError( iResult );
        }
    else
        {
        Observer()->OperationCompleted( iResult );
        }
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::CancelAsyncCommand
// ---------------------------------------------------------------------------
//
void CESMRUiLauncher::CancelAsyncCommand()
    {
    FUNC_LOG;
    delete iViewerController;
    iViewerController = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::LaunchMRViewerL
// ---------------------------------------------------------------------------
//
void CESMRUiLauncher::LaunchMRViewerL()
    {
    FUNC_LOG;
    // Constructing input parameters
    MAgnEntryUi::TAgnEntryUiInParams inParams(
            KFSEmailUiUid,
            CalSession(),
            MAgnEntryUi::EViewEntry );

    // Set time for input parameters
    TCalTime startTime = iInputParameters.iCalEntry->StartTimeL();
    TCalTime::TTimeMode timemode = startTime.TimeMode();
    if (timemode == TCalTime::EFloating )
        {
        inParams.iInstanceDate.SetTimeLocalFloatingL (
                startTime.TimeLocalL() );
        }
    else
        {
        inParams.iInstanceDate.SetTimeLocalFloatingL (
                startTime.TimeUtcL() );
        }
    inParams.iMsgSession = NULL;
    inParams.iSpare = (TInt)(&iInputParameters);

    // Output parameters
    MAgnEntryUi::TAgnEntryUiOutParams outParams;

    // Launch Entry UI
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL(entries);
    entries.AppendL(iInputParameters.iCalEntry);

    delete iViewerController; iViewerController = NULL;

    TUid mtmUid( TUid::Uid(KUidMsgTypeFsMtmVal) );
    //CAgnEntryUi resolves Meeting request viewer based on mtm uid
    TBuf8<KMaxUidName> mrMtm;
    CnvUtfConverter::ConvertFromUnicodeToUtf8(
            mrMtm,
            mtmUid.Name() );

    // If meeting requests are not supported --> Opened in meeting viewer
    // 1) FS meeting viewer is FS calendar is used
    // 2) S60 meeting editor if S60 calendar is used.
    delete iViewerController; iViewerController = NULL;
    iViewerController =
            CESMRViewerController::NewL(
                    mrMtm,
                    entries,
                    inParams,
                    outParams,
                    iAgnCallback );

    iViewerController->ExecuteL();
    CleanupStack::PopAndDestroy(&entries);

    delete iViewerController; iViewerController = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::LaunchEmailViewerUiL
// ---------------------------------------------------------------------------
//
void CESMRUiLauncher::LaunchEmailViewerUiL()
    {
    FUNC_LOG;
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CESMRUiLauncher::ExecuteAsyncCommandInternalL
// ---------------------------------------------------------------------------
//
void CESMRUiLauncher::ExecuteAsyncCommandInternalL()
    {
    FUNC_LOG;
    if ( iInputParameters.iCalEntry &&
            CanBeShownInMRViewerL(*iInputParameters.iCalEntry ) )
        {
        LaunchMRViewerL();
        }
    else
        {
        LaunchEmailViewerUiL();
        }
    }

// EOF

