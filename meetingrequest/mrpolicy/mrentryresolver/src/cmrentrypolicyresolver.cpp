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


#include "cmrentrypolicyresolver.h"
#include "cmrpolicyresourceiterator.h"
#include "cesmrpolicy.h"
#include "esmrhelper.h"
#include "tesmrscenariodata.h"

#include <esmrpolicies.rsg>
#include <data_caging_path_literals.hrh>

#include "emailtrace.h"

namespace // codescanner::namespace
    {
    _LIT( KPolicyResourceFile, "esmrpolicies.rsc" );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMREntryPolicyResolver::CMREntryPolicyResolver
// ---------------------------------------------------------------------------
//
CMREntryPolicyResolver::CMREntryPolicyResolver()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMREntryPolicyResolver::~CMREntryPolicyResolver
// ---------------------------------------------------------------------------
//
CMREntryPolicyResolver::~CMREntryPolicyResolver()
    {
    FUNC_LOG;
    
    delete iIterator;
    }

// ---------------------------------------------------------------------------
// CMREntryPolicyResolver::NewL
// ---------------------------------------------------------------------------
//
CMREntryPolicyResolver* CMREntryPolicyResolver::NewL()
    {
    FUNC_LOG;
    CMREntryPolicyResolver* self = new (ELeave) CMREntryPolicyResolver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMREntryPolicyResolver::ResolveL
// ---------------------------------------------------------------------------
//
CESMRPolicy* CMREntryPolicyResolver::ResolveL( const TESMRScenarioData& aScenData )
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
// CMREntryPolicyResolver::SupportsTypeL
// ---------------------------------------------------------------------------
//
TBool CMREntryPolicyResolver::SupportsTypeL( TESMRCalendarEventType aType )
    {
    return ( EESMREventTypeMeetingRequest == aType );
    }

// ---------------------------------------------------------------------------
// CMREntryPolicyResolver::MatchesL
// ---------------------------------------------------------------------------
//
TBool CMREntryPolicyResolver::MatchesL(
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
// CMREntryPolicyResolver::ConstructL
// ---------------------------------------------------------------------------
//
void CMREntryPolicyResolver::ConstructL()
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
            R_ESMR_POLICIES );
    }

// EOF

