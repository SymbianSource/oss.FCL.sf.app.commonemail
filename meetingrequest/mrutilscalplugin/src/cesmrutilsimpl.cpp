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
#include "cesmrutilsimpl.h"
#include "cesmrutils.h"
#include "cesmrcaldbmgr.h"

// From ESMRSERVICES
//<cmail>
#include "cesmrpolicymanager.h"
#include "cesmrentryprocessor.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrtaskfactory.h"
#include "mesmrtask.h"
#include "cesmrtaskextensionimpl.h"
#include "esmrinternaluid.h"
#include "tesmrscenariodata.h"
//</cmail>

// From System
#include <calentry.h>
#include <calinstance.h>
#include <calsession.h>
#include <calcommon.h>
#include <caluser.h>
#include <cmrmailboxutils.h>
#include <msvapi.h>
#include <msvstd.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG

// Definition for MR UTILS panic text
_LIT(KESMRUtilsPanicTxt, "ESMRUtilsImpl" );

// MR VIEWER ctrl panic codes
enum TESMRUtilsImplPanicCode
    {
    EESMRUtilsObserverMissing = 0,      // Observer is missing
    };

void Panic( TESMRUtilsImplPanicCode aPanicCode)
    {
    User::Panic( KESMRUtilsPanicTxt, aPanicCode );
    }

#endif

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::NewL
// ----------------------------------------------------------------------------
//
CESMRUtilsImpl* CESMRUtilsImpl::NewL()
    {
    FUNC_LOG;
    CESMRUtilsImpl* self = new ( ELeave ) CESMRUtilsImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::CESMRUtilsImpl
// ----------------------------------------------------------------------------
//
CESMRUtilsImpl::CESMRUtilsImpl()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::~CESMRUtilsImpl
// ----------------------------------------------------------------------------
//
CESMRUtilsImpl::~CESMRUtilsImpl()
    {
    FUNC_LOG;
    delete iEsmrUtils;
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::ConstructL
// ----------------------------------------------------------------------------
//
void CESMRUtilsImpl::ConstructL()
    {
    FUNC_LOG;
    iEsmrUtils = CESMRUtils::NewL( *this );
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::DeleteWithUiL
// ----------------------------------------------------------------------------
//
TInt CESMRUtilsImpl::DeleteWithUiL( // codescanner::intleaves
    const CCalEntry& aEntry,
    TMsvId aMailbox )
    {
    FUNC_LOG;
    return iEsmrUtils->DeleteWithUiL( aEntry, aMailbox );
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::DeleteWithUiL
// ----------------------------------------------------------------------------
//
TInt CESMRUtilsImpl::DeleteWithUiL( // codescanner::intleaves
    CCalInstance* aInstance,
    TMsvId aMailbox )
    {
    FUNC_LOG;
    return iEsmrUtils->DeleteWithUiL( aInstance, aMailbox );
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::SendWithUiL
// ----------------------------------------------------------------------------
//
void CESMRUtilsImpl::SendWithUiL(
    const CCalEntry& aEntry,
    TMsvId aMailbox )
    {
    FUNC_LOG;
    iEsmrUtils->SendWithUiL( aEntry, aMailbox );
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::ExtensionL
// ----------------------------------------------------------------------------
//
TAny* CESMRUtilsImpl::ExtensionL( TUid aExtensionUid )
    {
    FUNC_LOG;
    TAny* extension = iEsmrUtils->ExtensionL( aExtensionUid );
    return extension;
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::SessionSetupL
// This template method is called as a part of the ECom plugin construction.
// This replaces much of the ordinary ConstructL() functionality, a template
// method is easier for transmitting parameters.
// ----------------------------------------------------------------------------
//
void CESMRUtilsImpl::SessionSetupL(
    CCalSession& aCalSession,
    CMsvSession* aMsvSession )
    {
    FUNC_LOG;
    iEsmrUtils->SessionSetupL( aCalSession, aMsvSession );
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::HandleCalEngStatus
// ----------------------------------------------------------------------------
//
void CESMRUtilsImpl::HandleCalEngStatus( TMRUtilsCalEngStatus aStatus )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iObserver, Panic(EESMRUtilsObserverMissing ) );

    iObserver->HandleCalEngStatus( aStatus );
    }

// ----------------------------------------------------------------------------
// CESMRUtilsImpl::HandleOperation
// ----------------------------------------------------------------------------
//
void CESMRUtilsImpl::HandleOperation(
        TInt /*aType*/,
        TInt /*aPercentageCompleted*/,
        TInt /*aStatus*/ )
    {
    FUNC_LOG;

    }

// End of file

