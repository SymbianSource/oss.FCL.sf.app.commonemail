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
* Description:  ESMR MRUtils implementation
*
*/


//INCLUDE FILES
#include "emailtrace.h"
#include "cesmrutils.h"
#include "cesmrcaldbmgr.h"

// From ESMRSERVICES
#include <esmrpolicies.rsg>
//<cmail>
#include "cesmrpolicymanager.h"
#include "tesmrscenariodata.h"
#include "cesmrpolicy.h"
//</cmail>

#include "cesmrentryprocessor.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrtaskfactory.h"
#include "mesmrtask.h"
#include "cesmrtaskextensionimpl.h"
#include "esmrinternaluid.h"
#include "esmrhelper.h"

// From System
#include <coemain.h>
#include <calentry.h>
#include <calinstance.h>
#include <calsession.h>
#include <calcommon.h>
#include <caluser.h>
#include <cmrmailboxutils.h>
#include <msvapi.h>
#include <msvstd.h>
#include <data_caging_path_literals.hrh>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KESMRPolicyResourceFile, "esmrpolicies.rsc" );

#ifdef _DEBUG

// Definition for MR UTILS panic text
_LIT(KESMRUtilsPanicTxt, "ESMRUtils" );

// MR VIEWER ctrl panic codes
enum TESMRUtilsPanicCode
    {
    EESMRUtilsEntryProcessorNull = 0,   // Entry processon is NULL
    EESMRUtilsInvalidEntry       = 1,   // Invalid calendar entry,
    EESMRUtilsNULLCoeEnv         = 2    // CCoeEnv is NULL
    };

void Panic(TESMRUtilsPanicCode aPanicCode)
    {
    User::Panic( KESMRUtilsPanicTxt, aPanicCode );
    }

#endif

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CESMRUtils::CESMRUtils
// ----------------------------------------------------------------------------
//
CESMRUtils::CESMRUtils(
        MMRUtilsObserver& aObserver )
:   iObserver( aObserver )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CESMRUtils::~CESMRUtils
// ----------------------------------------------------------------------------
//
EXPORT_C CESMRUtils::~CESMRUtils()
    {
    FUNC_LOG;
    delete iCalDBMgr;
    delete iPolicyManager;
    delete iEntryProcessor;
    delete iMRMailboxUtils;
    delete iTaskFactory;
    delete iTaskExtension;
    }

// ----------------------------------------------------------------------------
// CESMRUtils::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CESMRUtils* CESMRUtils::NewL(
        MMRUtilsObserver& aObserver )
    {
    FUNC_LOG;
    CESMRUtils* self = new ( ELeave ) CESMRUtils( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CESMRUtils::ConstructL
// ----------------------------------------------------------------------------
//
void CESMRUtils::ConstructL()
    {
    FUNC_LOG;
    CCoeEnv* coeEnv = CCoeEnv::Static();
    __ASSERT_DEBUG( coeEnv, Panic(EESMRUtilsNULLCoeEnv) );

    TFileName mruiPolicyResource;
    User::LeaveIfError(
            ESMRHelper::LocateResourceFile(
                    KESMRPolicyResourceFile,
                    KDC_RESOURCE_FILES_DIR,
                    mruiPolicyResource,
                    &coeEnv->FsSession() ) );

    iPolicyManager = CESMRPolicyManager::NewL();
    iPolicyManager->ReadPolicyFromResourceL(
            mruiPolicyResource,
            R_ESMR_POLICIES );
    }

// ----------------------------------------------------------------------------
// CESMRUtils::DeleteWithUiL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRUtils::DeleteWithUiL(
    const CCalEntry& aEntry,
    TMsvId /*aMailbox*/ )
    {
    FUNC_LOG;
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );
    User::LeaveIfError( entries.Append( &aEntry ) );

    CESMREntryProcessor* processor =
            CESMREntryProcessor::NewL(
                        *iMRMailboxUtils,
                        *iCalDBMgr );

    CleanupStack::PushL( processor );

    processor->ProcessL( &entries );

    MESMRMeetingRequestEntry& mrEntry = processor->ESMREntryL();
    if ( mrEntry.IsRecurrentEventL() )
        {
        mrEntry.SetModifyingRuleL(
                MESMRMeetingRequestEntry::EESMRAllInSeries );
        }

    mrEntry.MarkMeetingCancelledL();

    iTaskExtension->DeleteAndSendMRL(
            EESMRCmdDeleteMR,
            mrEntry );

    CleanupStack::PopAndDestroy( processor );
    CleanupStack::PopAndDestroy();  //entries

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CESMRUtils::DeleteWithUiL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRUtils::DeleteWithUiL(
    CCalInstance* aInstance,
    TMsvId /*aMailbox*/ )
    {
    FUNC_LOG;
    CESMREntryProcessor* processor =
            CESMREntryProcessor::NewL(
                        *iMRMailboxUtils,
                        *iCalDBMgr );

    CleanupStack::PushL( processor );

    processor->ProcessL( *aInstance );

    MESMRMeetingRequestEntry& mrEntry = processor->ESMREntryL();
    mrEntry.SetModifyingRuleL(
            MESMRMeetingRequestEntry::EESMRThisOnly );

    mrEntry.MarkMeetingCancelledL();

    iTaskExtension->DeleteAndSendMRL(
                EESMRCmdDeleteMR,
                mrEntry );

    // prosessor and aInstance
    CleanupStack::PopAndDestroy( processor );

    // aInstance is not added to CleanupStack, because calendar
    // already has it.
    delete aInstance;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CESMRUtils::SendWithUiL
// ----------------------------------------------------------------------------
//
EXPORT_C void CESMRUtils::SendWithUiL(
    const CCalEntry& aEntry ,
    TMsvId /*aMailbox*/ )
    {
    FUNC_LOG;
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );
    User::LeaveIfError( entries.Append( &aEntry ) );

    CESMREntryProcessor* processor =
            CESMREntryProcessor::NewL(
                        *iMRMailboxUtils,
                        *iCalDBMgr );

    CleanupStack::PushL( processor );

    processor->ProcessL( &entries );

    MESMRMeetingRequestEntry& mrEntry = processor->ESMREntryL();
    
    if ( mrEntry.IsRecurrentEventL() )
            {
            mrEntry.SetModifyingRuleL(
                    MESMRMeetingRequestEntry::EESMRAllInSeries );
            }
       
    iTaskExtension->ForwardMRAsEmailL(
    			EESMRCmdForwardAsMail,
                mrEntry );

    CleanupStack::PopAndDestroy( processor );
    CleanupStack::PopAndDestroy( &entries );
    }
// ----------------------------------------------------------------------------
// CESMRUtils::ExtensionL
// ----------------------------------------------------------------------------
//
EXPORT_C TAny* CESMRUtils::ExtensionL( TUid aExtensionUid )
    {
    FUNC_LOG;
    TAny* extension = NULL;

    switch ( aExtensionUid.iUid )
        {
        case KESMRPolicyMgrUid:
            extension = static_cast<TAny*>(iPolicyManager);
            break;
        case KESMRMREntryProcessorUid:
            extension = static_cast<TAny*>(iEntryProcessor);
            break;
        case KESMRTaskExtensionUid:
            extension = static_cast<TAny*>(iTaskExtension);
            break;
        default:
            extension = NULL;
            break;
        }
    return extension;
    }

// ----------------------------------------------------------------------------
// CESMRUtils::SessionSetupL
// This template method is called as a part of the ECom plugin construction.
// This replaces much of the ordinary ConstructL() functionality, a template
// method is easier for transmitting parameters.
// ----------------------------------------------------------------------------
//
EXPORT_C void CESMRUtils::SessionSetupL(
    CCalSession& aCalSession,
    CMsvSession* aMsvSession )
    {
    FUNC_LOG;
    // Asynchronous initialization of CESMRServicesCalDbMgr:
    iCalDBMgr = CESMRCalDbMgr::NewL( aCalSession,
                                     *this );

    iMRMailboxUtils = CMRMailboxUtils::NewL( aMsvSession );

    iTaskFactory = CESMRTaskFactory::NewL( *iCalDBMgr, *iMRMailboxUtils );

    iEntryProcessor = CESMREntryProcessor::NewL(
                        *iMRMailboxUtils,
                        *iCalDBMgr );

    iTaskExtension =
        CESMRTaskExtenstionImpl::NewL(
            *iCalDBMgr,
            *iMRMailboxUtils,
            *iPolicyManager,
            *iEntryProcessor,
            *iTaskFactory );
    }

// ----------------------------------------------------------------------------
// CESMRUtils::HandleCalEngStatus
// ----------------------------------------------------------------------------
//
void CESMRUtils::HandleCalEngStatus( TMRUtilsCalEngStatus aStatus )
    {
    FUNC_LOG;
    iCalEngStatus = aStatus;
    iObserver.HandleCalEngStatus( iCalEngStatus );
    }

// ----------------------------------------------------------------------------
// CESMRUtils::HandleOperation
// ----------------------------------------------------------------------------
//
void CESMRUtils::HandleOperation(
        TInt /*aType*/,
        TInt /*aPercentageCompleted*/,
        TInt /*aStatus*/ )
    {
    FUNC_LOG;
    // Do nothing
    }

// End of file

