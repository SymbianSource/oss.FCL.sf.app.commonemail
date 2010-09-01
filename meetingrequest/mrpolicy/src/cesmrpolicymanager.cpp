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
* Description:  ESMR policymmanager implementation
*
*/


#include "cesmrpolicymanager.h"
#include "cesmrpolicy.h"
#include "esmrdef.h"
#include "esmrinternaluid.h"
#include "cesmrcalimportexporter.h"
#include "tesmrentryfield.h"
#include "cmrpolicyresolver.h"
#include "esmrentryhelper.h"
#include "mesmrcalentry.h"

#include "emailtrace.h"

#include <esmrpolicies.rsg>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

_LIT( KESMRPolicyMgrPanic, "ESMRPolicyManager" );

enum TESMRPolicyManagerPanic
    {
    EESMRPolicyMgrNoPolicyFound = 1,
    EESMRPolicyMgrPopWithoutPush
    };

void Panic( TESMRPolicyManagerPanic aPanic )
    {
    User::Panic( KESMRPolicyMgrPanic, aPanic );
    }

/**
 * Logs policy information.
 *
 * @param aPolicy Reference to policy.
 */
void LogPolicy(
        const CESMRPolicy& aPolicy )
    {





    const RArray<TESMREntryField>& fieldArray = aPolicy.Fields();
    TInt fieldCount( fieldArray.Count() );


    for ( TInt i(0); i < fieldCount; ++i )
        {
        const TESMREntryField& field = fieldArray[i];

        }
    }

#define LOG_POLICY( x ) { LogPolicy( x ); } 

#else

#define LOG_POLICY( x )

#endif // _DEBUG

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRPolicyManager::CESMRPolicyManager
// ---------------------------------------------------------------------------
//
inline CESMRPolicyManager::CESMRPolicyManager()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::~CESMRPolicyManager
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicyManager::~CESMRPolicyManager()
    {
    FUNC_LOG;
    delete iResolverPlugin;
    delete iCurrentPolicy;
    iPolicyStack.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicyManager* CESMRPolicyManager::NewL()
    {
    FUNC_LOG;

    CESMRPolicyManager* self = new (ELeave) CESMRPolicyManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRPolicyManager::ConstructL()
    {
    FUNC_LOG;
    
    }

// -----------------------------------------------------------------------------
// CESMRPolicyManager::CurrentPolicy
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRPolicy& CESMRPolicyManager::CurrentPolicy()
    {
    CESMRPolicy* currentPolicy = iCurrentPolicy;
    if ( !currentPolicy && iPolicyStack.Count() )
        {
        currentPolicy = iPolicyStack[ iPolicyStack.Count() - 1 ];
        }
    
    ASSERT( currentPolicy );
    return *currentPolicy;
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::ResolvePolicyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRPolicyManager::ResolvePolicyL(
        const TESMRScenarioData& aScenarioData,
        MESMRCalEntry& aESMREntry,
        MMRPolicyResolver* aPolicyResolver )
    {
    FUNC_LOG;

    MMRPolicyResolver* policyResolver = NULL;
    
    if ( aPolicyResolver )
        {
        policyResolver = aPolicyResolver;
        }
    else
        {
        TESMRCalendarEventType type =
            ESMREntryHelper::EventTypeL( aESMREntry.Entry() );
        
        if ( !iResolverPlugin || !iResolverPlugin->SupportsTypeL( type ) )
            {
            // Instantiate correct resolver
            policyResolver = CMRPolicyResolver::NewL( type );
            delete iResolverPlugin;
            iResolverPlugin = policyResolver;
            }
        else
            {
            // Use default resolver
            policyResolver = iResolverPlugin;
            }
        }

    delete iCurrentPolicy;
    iCurrentPolicy = NULL;
    
    iCurrentPolicy = policyResolver->ResolveL( aScenarioData );

    // Matching policy not found --> Panic
    __ASSERT_DEBUG( iCurrentPolicy, Panic(EESMRPolicyMgrNoPolicyFound) );

    if ( !iCurrentPolicy )
        {
        User::Leave( KErrNotFound );
        }

    // Logging policy for debugging purposes
    LOG_POLICY( *iCurrentPolicy )
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::PushPolicyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRPolicyManager::PushPolicyL()
    {
    if ( iCurrentPolicy )
        {
        iPolicyStack.AppendL( iCurrentPolicy );
        iCurrentPolicy = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::PopPolicy
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicy* CESMRPolicyManager::PopPolicy()
    {
    delete iCurrentPolicy;
    iCurrentPolicy = NULL;
    
    TInt stackCount = iPolicyStack.Count(); 
    if ( stackCount )
        {
        iCurrentPolicy = iPolicyStack[ --stackCount ];
        iPolicyStack.Remove( stackCount );
        }
    
    __ASSERT_DEBUG( iCurrentPolicy, Panic( EESMRPolicyMgrPopWithoutPush ) );
    
    return iCurrentPolicy;
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::CurrentPolicy
// ---------------------------------------------------------------------------
//
const CESMRPolicy& CESMRPolicyManager::CurrentPolicy() const
    {
    CESMRPolicy& policy =
        const_cast< CESMRPolicyManager* >( this )->CurrentPolicy();
    
    return policy;
    }

// EOF

