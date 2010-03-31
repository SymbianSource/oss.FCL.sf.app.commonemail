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
* Description:  MR Cal Entry policy resolver ECom plugin implementation
*
*/


#include "cmrcalentrypolicyresolver.h"
#include "cmrpolicyresourceiterator.h"
#include "cesmrpolicy.h"
#include "esmrhelper.h"
#include "tesmrscenariodata.h"

#include <bccaleventpolicies.rsg>
#include <data_caging_path_literals.hrh>

#include "emailtrace.h"

namespace // codescanner::namespace
    {
    _LIT( KPolicyResourceFile, "bccaleventpolicies.rsc" );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRCalEntryPolicyResolver::CMRCalEntryPolicyResolver
// ---------------------------------------------------------------------------
//
CMRCalEntryPolicyResolver::CMRCalEntryPolicyResolver()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRCalEntryPolicyResolver::~CMRCalEntryPolicyResolver
// ---------------------------------------------------------------------------
//
CMRCalEntryPolicyResolver::~CMRCalEntryPolicyResolver()
    {
    FUNC_LOG;
    
    delete iIterator;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryPolicyResolver::NewL
// ---------------------------------------------------------------------------
//
CMRCalEntryPolicyResolver* CMRCalEntryPolicyResolver::NewL()
    {
    FUNC_LOG;
    CMRCalEntryPolicyResolver* self = new (ELeave) CMRCalEntryPolicyResolver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryPolicyResolver::ResolveL
// ---------------------------------------------------------------------------
//
CESMRPolicy* CMRCalEntryPolicyResolver::ResolveL( const TESMRScenarioData& aScenData )
    {
    iIterator->Reset();
    
    CESMRPolicy* policy = NULL;
    
    while ( !policy && iIterator->HasNext() )
        {
        policy = iIterator->NextPolicyLC();
        
        if ( MatchesL( *policy, aScenData ) )
            {
            CleanupStack::Pop( policy );
            }
        else
            {
            CleanupStack::PopAndDestroy( policy );
            policy = NULL;
            }
        }
    
    if ( !policy )
        {
        User::Leave( KErrNotFound );
        }
    
    return policy;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryPolicyResolver::SupportsTypeL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntryPolicyResolver::SupportsTypeL( TESMRCalendarEventType aType )
    {
    switch ( aType )
        {
        case EESMREventTypeAppt:
        case EESMREventTypeETodo:
        case EESMREventTypeEEvent:
        case EESMREventTypeEReminder:
        case EESMREventTypeEAnniv:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRCalEntryPolicyResolver::MatchesL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntryPolicyResolver::MatchesL(
    const CESMRPolicy& aPolicy,
    const TESMRScenarioData& aScenData )
    {
    FUNC_LOG;
    TBool matches( ETrue );
        
    if ( aPolicy.ViewMode() != aScenData.iViewMode
         || aPolicy.EventType() != aScenData.iEntryType
         || aPolicy.AllowedApp() != aScenData.iCallingApp )
        {
        matches = EFalse;
        }
        
    return matches;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryPolicyResolver::ConstructL
// ---------------------------------------------------------------------------
//
void CMRCalEntryPolicyResolver::ConstructL()
    {
    FUNC_LOG;
    
    TFileName resource;
    User::LeaveIfError( ESMRHelper::LocateResourceFile(
            KPolicyResourceFile,
            KDC_RESOURCE_FILES_DIR,
            resource,
            NULL ) );
    
    iIterator = CMRPolicyResourceIterator::NewL(
            resource,
            R_BCCALEVENT_POLICIES );
    }

// EOF

