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
* Description:  ESMR service policy checker implementation
*
*/


#include "emailtrace.h"
#include "cesmrpolicychecker.h"
//<cmail>
#include "cesmrpolicy.h"
#include "tesmrscenariodata.h"
//</cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRPolicyChecker::CESMRPolicyChecker
// ---------------------------------------------------------------------------
//
inline CESMRPolicyChecker::CESMRPolicyChecker()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRPolicyChecker::~CESMRPolicyChecker
// ---------------------------------------------------------------------------
//
CESMRPolicyChecker::~CESMRPolicyChecker()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRPolicyChecker::NewL
// ---------------------------------------------------------------------------
//
CESMRPolicyChecker* CESMRPolicyChecker::NewL()
    {
    FUNC_LOG;
    CESMRPolicyChecker* self = new (ELeave) CESMRPolicyChecker;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRPolicyChecker::MatchesL
// ---------------------------------------------------------------------------
//
TBool CESMRPolicyChecker::MatchesL(
    const CESMRPolicy& aPolicy,
    const TESMRScenarioData& aScenData )
    {
    FUNC_LOG;
    TBool matches( ETrue );

    if (  aPolicy.ViewMode() != aScenData.iViewMode ||
         !aPolicy.IsRoleIncluded( aScenData.iRole ) ||
          aPolicy.AllowedApp() != aScenData.iCallingApp )
        {
        matches = EFalse;
        }

    return matches;
    }

// ---------------------------------------------------------------------------
// CESMRPolicyChecker::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRPolicyChecker::ConstructL()
    {
    FUNC_LOG;
    // Do nothing
    }

// EOF

