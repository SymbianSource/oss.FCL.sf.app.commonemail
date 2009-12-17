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


#include "emailtrace.h"
#include "cesmrpolicymanager.h"
//<cmail>
#include "cesmrpolicy.h"
#include <esmrpolicies.rsg>
#include "esmrdef.h"
//</cmail>
#include "esmrinternaluid.h"
#include "cesmrcalimportexporter.h"
#include "tesmrentryfield.h"
#include "cesmrpolicychecker.h"
#include "cesmrpolicyextensionmanager.h"

#include <coemain.h>// CCoeEnv
#include <barsread.h>


/// Unnamed namespace for local definitions
namespace {

// Definition for resource id array granularity
const TInt KESMRPolicyIdArrayGranularity = 4;

/**
 * Reads ESMR policy resource id table from resource.
 *
 * @param aReader Reference to resource reader.
 * @param aResourceIdTable Reference to resource id table.
 */
void ReadResourceIdArrayL(
        TResourceReader& aReader,
        RArray<TInt>& aResourceIdTable )
    {
    TInt numOfFields = aReader.ReadInt16();
    for (TInt i(0); i < numOfFields; i++ )
        {
        TInt resourceId= aReader.ReadInt32();
        aResourceIdTable.AppendL(resourceId);
        }
    }


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
:   iPolicyResourceIds( KESMRPolicyIdArrayGranularity )
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
    delete iDefaultPolicyChecker;
    delete iCurrentPolicy;
    delete iExtension;
    iPolicyResourceFile.Close();
    iPolicyResourceIds.Close();
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
    iDefaultPolicyChecker = CESMRPolicyChecker::NewL();
    iCoeEnv = CCoeEnv::Static();
    iExtension = CESMRPolicyExtensionManager::NewL( *iCoeEnv );
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::ExtensionUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CESMRPolicyManager::ExtensionUid()
    {
    FUNC_LOG;
    return TUid::Uid( KESMRPolicyMgrUid );
    }

// ---------------------------------------------------------------------------
// CESMRPolicyManager::ResolvePolicyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRPolicyManager::ResolvePolicyL(
        const TESMRScenarioData& aScenarioData,
        MESMRCalEntry& aESMREntry,
        MESMRPolicyChecker* aPolicyChecker )
    {
    FUNC_LOG;

    MESMRPolicyChecker* policyChecker = iDefaultPolicyChecker;
    if ( aPolicyChecker )
        {
        policyChecker = aPolicyChecker;
        }

    delete iCurrentPolicy;
    iCurrentPolicy = NULL;

    TInt policyCount( iPolicyResourceIds.Count() );
    for ( TInt i( 0 );
        ( i < policyCount) && !iCurrentPolicy;
          ++i )
        {
        ReadPolicyL( iPolicyResourceIds[i], aESMREntry );
        if ( !policyChecker->MatchesL( *iCurrentPolicy, aScenarioData ) )
            {
            // Policy did not match --> Reading next
            delete iCurrentPolicy;
            iCurrentPolicy = NULL;
            }
        }

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
// CESMRPolicyManager::ReadPolicyFromResourceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRPolicyManager::ReadPolicyFromResourceL(
        const TDesC& aPolicyFile,
        TInt aPolicyArrayResourceId )
    {
    FUNC_LOG;


    iPolicyResourceFile.Close();
    iPolicyResourceFile.OpenL( iCoeEnv->FsSession(), aPolicyFile );
    iPolicyResourceFile.ConfirmSignatureL();

    // R_ESMR_POLICIES
    HBufC8* resourceIdBuffer =
        iPolicyResourceFile.AllocReadLC( aPolicyArrayResourceId );

    // Construct resource reader
    TResourceReader reader;
    reader.SetBuffer(resourceIdBuffer);

    // Read policy resource ids
    iPolicyResourceIds.Reset();
    ReadResourceIdArrayL(reader, iPolicyResourceIds );

    CleanupStack::PopAndDestroy( resourceIdBuffer );

    // Load extension resources
    iExtension->ReadResourcesFromExtensionsL();

    }


// ---------------------------------------------------------------------------
// CESMRPolicyManager::ReadPolicyL
// ---------------------------------------------------------------------------
//
void CESMRPolicyManager::ReadPolicyL(
        TInt aResourceId,
        MESMRCalEntry& /*aESMREntry */)
    {
    FUNC_LOG;

    HBufC8* resourceBuffer = NULL;
    // First try to find policy from static policies and then from extensions
    TRAPD( error, resourceBuffer = iPolicyResourceFile.AllocReadL( aResourceId ) );
    if ( resourceBuffer )
        {
        CleanupStack::PushL( resourceBuffer );
        }

    TResourceReader reader;
    CESMRPolicy* policy = NULL;
    if ( error == KErrNone && resourceBuffer )
        {
        reader.SetBuffer(resourceBuffer);
        policy = CESMRPolicy::NewL( iPolicyResourceFile, reader );
        }
    else
        {
        resourceBuffer = iExtension->ReadBufferL( aResourceId );
        CESMRExtensionResourceFile* file = NULL;
        if ( resourceBuffer )
            {
            CleanupStack::PushL( resourceBuffer );
            file = iExtension->ExtensionResourceFile( aResourceId );
            }

        if ( resourceBuffer && file )
            {
            reader.SetBuffer(resourceBuffer);
            policy = CESMRPolicy::NewL( file->ResFile(), reader );
            }
        }

    // Logging read policy
    LOG_POLICY( *policy )

    delete iCurrentPolicy; 
    iCurrentPolicy = policy;

    if ( resourceBuffer )
        {
        CleanupStack::PopAndDestroy( resourceBuffer );
        }

    }

// EOF

