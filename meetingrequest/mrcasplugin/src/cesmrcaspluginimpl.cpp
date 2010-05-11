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
 *  Description : Implementation of the class CESMRCasPluginImpl.
 *  Version     : %version: e002sa32#9.1.1 % << Don't touch! Updated by Synergy at check-out.
 *
 */

#include "emailtrace.h"
#include "cesmrcaspluginimpl.h"
#include "esmrhelper.h"
#include "cesmrcaspluginuilauncher.h"
#include "cesmrcascontactaction.h"
#include "cesmrcasplugincontactretriever.h"
// <cmail> Profiler removed. </cmail>

#include <esmrcasplugindata.rsg>
#include <esmrcasplugin.mbg>

#include <MVPbkStoreContact.h>
#include <data_caging_path_literals.hrh>
#include <coemain.h>
#include <calsession.h>
#include <cmrmailboxutils.h>
#include <CalenInterimUtils2.h>
#include <badesca.h>

#include <ecom/implementationproxy.h>
#include <StringLoader.h>
#include <AknIconUtils.h>
#include <bautils.h>
//<cmail>
#include "mfscactionutils.h"
#include "mfsccontactset.h"
//</cmail>

#include <utf.h>
//<cmail>
#include "mfsccontactactionpluginobserver.h"
#include "mfsccontactsetobserver.h"
//</cmail>
#include <gulicon.h>

//<cmail>
#include "fsccontactactionserviceuids.hrh"
#include "fscactionpluginactionuids.h"
#include "fscactionplugincrkeys.h"
//</cmail>

// CONSTANTS DECLARATIONS
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
            KFscMruiPluginImplImpUid,
            CESMRCasPluginImpl::NewL )
    };

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

// Action Priority
const TInt KPbkxActionPriorityMeetingRequest = 450 /* cenrep default value */;

const TInt KActionCount = 1;
const TInt KInvalidReasonId = 0;

_LIT( KMeetingRequestPluginIconPath, "esmrcasplugin.mif" );
_LIT( KResourceFileName, "esmrcasplugindata.rsc");

#ifdef _DEBUG

_LIT( KTESMRCasPluginPanicTxt, "TESMRCasPluginPanic");

enum TESMRCasPluginPanic
    {
    EESMRCasPluginNotSupported,
    EESMRCasPluginNoCoeEnv
    };

void Panic( TESMRCasPluginPanic aPanic )
    {
    User::Panic ( KTESMRCasPluginPanicTxt, aPanic );
    }

#endif // DEBUG
}//namespace

// ======== GLOBAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    FUNC_LOG;

    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );


    return ImplementationTable;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::CESMRCasPluginImpl
// ---------------------------------------------------------------------------
//
CESMRCasPluginImpl::CESMRCasPluginImpl( const TFscContactActionPluginParams& aParams ) :
    iParams(aParams),
    iResourceHandle(0)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::~CESMRCasPluginImpl
// ---------------------------------------------------------------------------
//
CESMRCasPluginImpl::~CESMRCasPluginImpl( )
    {
    FUNC_LOG;

    delete iMeetingRequestContactAction;
    delete iActionList;

    CCoeEnv::Static()->DeleteResourceFile( iResourceHandle );

    delete iMailboxUtils;
    delete iCalUtils;
    delete iCalSession;
    delete iUiLauncher;
    delete iContactRetriever;

    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::NewL
// ---------------------------------------------------------------------------
//
CESMRCasPluginImpl* CESMRCasPluginImpl::NewL( TAny* aParams )
    {
    FUNC_LOG;

    TFscContactActionPluginParams* params =
            reinterpret_cast< TFscContactActionPluginParams* >( aParams );

    CESMRCasPluginImpl* self = new (ELeave) CESMRCasPluginImpl(*params );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::ConstructL()
    {
    FUNC_LOG;

    iCalSession = CCalSession::NewL();
    iCalSession->OpenL( KNullDesC );

    iMailboxUtils = CMRMailboxUtils::NewL();
    iCalUtils = CCalenInterimUtils2::NewL();

    iUiLauncher = CESMRCasPluginUiLauncher::NewL(
                        *iCalSession,
                        *iCalUtils,
                        *iMailboxUtils,
                        iParams,
                        *this );

    TFileName resourceFileName;
    User::LeaveIfError(
            ESMRHelper::LocateResourceFile(
                    KResourceFileName,
                    KDC_RESOURCE_FILES_DIR ,
                    resourceFileName ) );

    CCoeEnv* coeEnv = CCoeEnv::Static();

    __ASSERT_DEBUG( coeEnv, Panic(EESMRCasPluginNoCoeEnv) );

    BaflUtils::NearestLanguageFile(
            coeEnv->FsSession(),
            resourceFileName); //for localization
    iResourceHandle =coeEnv->AddResourceFileL( resourceFileName );

    iActionList = new( ELeave )CArrayFixFlat<TUid>( KActionCount );
    iActionList->AppendL ( KFscActionUidMeetingReq );
// <cmail>
    iMeetingRequestContactAction =
            CESMRCasContactAction::NewL(
                *this,
                KFscActionUidMeetingReq,
                KFscAtComSendCalReq,
                R_FS_ACTION_MEETING,
                IconL(  EMbmEsmrcaspluginQgn_prop_cmail_action_meeting,
                        EMbmEsmrcaspluginQgn_prop_cmail_action_meeting_mask ) );
// </cmail>
    iContactRetriever = CESMRCasPluginContactRetriever::NewL( iParams );

    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::Uid
// ---------------------------------------------------------------------------
//
TUid CESMRCasPluginImpl::Uid( ) const
    {
    FUNC_LOG;
    TUid uid = TUid::Uid( KFscMruiPluginImplImpUid );


    return uid;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::ActionList
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>* CESMRCasPluginImpl::ActionList( ) const
    {

    return iActionList;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::GetActionL
// ---------------------------------------------------------------------------
//
const MFscContactAction& CESMRCasPluginImpl::GetActionL(TUid aActionUid ) const
    {
    FUNC_LOG;

    const MFscContactAction* action=  NULL;
    if ( aActionUid == KFscActionUidMeetingReq )
        {
        action = iMeetingRequestContactAction;
        }
    else
        {
        User::Leave( KErrNotFound ); // codescanner::leave
        }


    return *action;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::PriorityForContactSetL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::PriorityForContactSetL(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        TFscContactActionVisibility& aActionMenuVisibility,
        TFscContactActionVisibility& aOptionsMenuVisibility,
        MFscContactActionPluginObserver* aObserver )
    {
    FUNC_LOG;

    iActionPriority = iParams.iUtils->ActionPriority (
                    KFscCrUidMeetingRequest,
                    KPbkxActionPriorityMeetingRequest );

    iPluginObserver = aObserver;
    iActionMenuVisibility = &aActionMenuVisibility;
    iOptionsMenuVisibility = &aOptionsMenuVisibility;

    if ( !MRViewersEnabledL() )
        {
        iActionPriority = KFscActionPriorityNotAvailable;
        aActionMenuVisibility.iVisibility = TFscContactActionVisibility::EFscActionHidden;
        aActionMenuVisibility.iReasonId = KInvalidReasonId;
        aOptionsMenuVisibility.iVisibility = TFscContactActionVisibility::EFscActionHidden;
        aOptionsMenuVisibility.iReasonId = KInvalidReasonId;
        aObserver->PriorityForContactSetComplete( iActionPriority );
        }
    else if ( aActionUid == KFscActionUidMeetingReq )
        {
        iContactRetriever->CanExecuteL( aContactSet, *this );
        }
    else
        {
        aObserver->PriorityForContactSetComplete( iActionPriority );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::ExecuteL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::ExecuteL(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        MFscContactActionPluginObserver* aObserver )
    {
    FUNC_LOG;
    
    if ( aActionUid == KFscActionUidMeetingReq )
        {
        iPluginObserver = aObserver;
        iContactRetriever->GetAttendeesL(
                aContactSet,
                *this );
        }
    else
        {
        aObserver->ExecuteFailed( KErrArgument );
        }

    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::CancelExecute
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::CancelExecute()
    {
    FUNC_LOG;

       iContactRetriever->Cancel();

    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::CancelPriorityForContactSet
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::CancelPriorityForContactSet()
    {
    FUNC_LOG;

    iContactRetriever->Cancel();
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::GetReasonL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::GetReasonL(TUid /*aActionUid*/,
                                    TInt /*aReasonId*/,
                                    HBufC*& /*aReason */) const
    {
    FUNC_LOG;
    //nothing to do
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::MRCanExecuteComplete
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::MRCanExecuteComplete(
        TBool aCanExecute )
    {
    FUNC_LOG;
    if ( aCanExecute )
        {
        iActionMenuVisibility->iVisibility =
                TFscContactActionVisibility::EFscActionVisible;
        iOptionsMenuVisibility->iVisibility =
                TFscContactActionVisibility::EFscActionVisible;
        }
    else
        {
        iActionPriority = KFscActionPriorityNotAvailable;
        iActionMenuVisibility->iVisibility =
                TFscContactActionVisibility::EFscActionHidden;
        iActionMenuVisibility->iReasonId = KInvalidReasonId;

        iOptionsMenuVisibility->iVisibility =
                TFscContactActionVisibility::EFscActionHidden;
        iOptionsMenuVisibility->iReasonId = KInvalidReasonId;
        }

    if ( iPluginObserver )
        {
        iPluginObserver->PriorityForContactSetComplete( iActionPriority );
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::MRCanExecuteError
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::MRCanExecuteError(
        TInt aError )
    {
    FUNC_LOG;
    if ( iPluginObserver )
        {
        iPluginObserver->PriorityForContactSetFailed( aError );
        }    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::MRGetAttendeesComplete
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::MRGetAttendeesComplete(
        TInt aContactCount,
        const RPointerArray<CCalUser>& aContacts )
    {
    FUNC_LOG;
    
    // MRGetAttendeesCompleteL is synchronous function and will
    // return only after MR Editor is closed or opening editor leaves.
    TRAPD( error, MRGetAttendeesCompleteL( aContactCount, aContacts ) );
    
    // If editor was opened properly MREditorInitializationComplete should
    // have been called and iPluginObserver should be NULL.
    if ( iPluginObserver )
        {
        if ( error != KErrNone )
            {
            iPluginObserver->ExecuteFailed( error );
            }
        else
            {
            iPluginObserver->ExecuteComplete();
            }
        // Set iPluginObserver to NULL to avoid more calls to observer.
        iPluginObserver = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::MRGetAttendeesCompleteL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::MRGetAttendeesCompleteL(
        TInt aContactCount,
        const RPointerArray<CCalUser>& aContacts )
    {
    FUNC_LOG;
    TInt invalidContactCount = aContactCount - aContacts.Count();
    if ( invalidContactCount > 0 &&
        !iParams.iUtils->ShowNumberMissingNoteL(
            invalidContactCount,
            aContactCount,
            R_FS_QUERY_INFO_MISSING,
            R_FS_EMAIL_MISSING_QUERY_DIALOG ) )
        {
        // User selected cancel
        User::Leave( KErrCancel );
        }
    else
        {
        iUiLauncher->LaunchMeetingRequestViewL( aContacts );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::MRGetAttendeesError
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::MRGetAttendeesError(
        TInt aError )
    {
    FUNC_LOG;
    if ( iPluginObserver )
        {
        iPluginObserver->ExecuteFailed( aError );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::MREditorInitializationComplete
// ---------------------------------------------------------------------------
//
void CESMRCasPluginImpl::MREditorInitializationComplete()
    {
    FUNC_LOG;
   
    if ( iPluginObserver )
        {
        // This is asunchronous callback while MRGetAttendeesCompleteL
        // still executes. Notify iPluginObserver so that Action Menu is
        // removed.
        iPluginObserver->ExecuteComplete();
        // Set iPluginObserver to NULL to avoid more calls to
        // iPluginObserver.
        iPluginObserver = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::IconL
// ---------------------------------------------------------------------------
//
CGulIcon* CESMRCasPluginImpl::IconL( TInt aBitmapId, TInt aMaskId ) const
    {
    FUNC_LOG;

    CGulIcon* returnIcon( NULL );
    TFileName iconFileName;
    User::LeaveIfError(
    ESMRHelper::LocateResourceFile(
            KMeetingRequestPluginIconPath,
            KDC_APP_BITMAP_DIR ,
            iconFileName ) );

    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );

    AknIconUtils::CreateIconL( bitmap, mask, iconFileName, aBitmapId,
            aMaskId );
    CleanupStack::PushL( mask );
    CleanupStack::PushL( bitmap );
    returnIcon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( bitmap );
    CleanupStack::Pop( mask );


    return returnIcon;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::MRViewersEnabledL
// ---------------------------------------------------------------------------
//
TBool CESMRCasPluginImpl::MRViewersEnabledL()
    {
    FUNC_LOG;
    TBool retValue( EFalse );

    if ( iCalUtils->MRViewersEnabledL( ETrue ) )
        {
        retValue = ETrue;
        }

    return retValue;
    }

// EOF

