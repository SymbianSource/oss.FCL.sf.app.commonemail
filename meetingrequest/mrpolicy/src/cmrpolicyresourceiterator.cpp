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
#include "cmrpolicyresourceiterator.h"
//<cmail>
#include "cesmrpolicy.h"
#include <esmrpolicies.rsg>
#include "esmrdef.h"
//</cmail>
#include "esmrinternaluid.h"
#include "cesmrcalimportexporter.h"
#include "tesmrentryfield.h"

#include <coemain.h>// CCoeEnv
#include <barsread.h>


/// Unnamed namespace for local definitions
namespace {

// Definition for resource id array granularity
const TInt KMRPolicyIdArrayGranularity = 4;

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
    aResourceIdTable.ReserveL( numOfFields );
    
    for (TInt i = 0; i < numOfFields; ++i )
        {
        TInt resourceId= aReader.ReadInt32();
        aResourceIdTable.AppendL(resourceId);
        }
    }

#ifdef _DEBUG

_LIT( KPanicCategory, "MR Policy Resource Iterator" );

enum TMRPolicyResourceIteratorPanic
    {
    EMRPolicyIteratorExhausted = 0
    };

void Panic( TMRPolicyResourceIteratorPanic aPanic )
    {
    User::Panic( KPanicCategory, aPanic );
    }

#endif
}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::CMRPolicyResourceIterator
// ---------------------------------------------------------------------------
//
inline CMRPolicyResourceIterator::CMRPolicyResourceIterator()
    : iPolicyResourceIds( KMRPolicyIdArrayGranularity ),
      iArrayIndex( KErrNotFound )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::~CMRPolicyResourceIterator
// ---------------------------------------------------------------------------
//
EXPORT_C CMRPolicyResourceIterator::~CMRPolicyResourceIterator()
    {
    FUNC_LOG;
    iPolicyResourceFile.Close();
    iPolicyResourceIds.Close();
    }

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CMRPolicyResourceIterator* CMRPolicyResourceIterator::NewL(
        const TDesC& aResourceFile,
        TInt aResourceId )
    {
    FUNC_LOG;

    CMRPolicyResourceIterator* self = new (ELeave) CMRPolicyResourceIterator;
    CleanupStack::PushL( self );
    self->ConstructL( aResourceFile,
                      aResourceId );
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::ConstructL
// ---------------------------------------------------------------------------
//
void CMRPolicyResourceIterator::ConstructL(
        const TDesC& aResourceFile,
        TInt aResourceId)
    {
    FUNC_LOG;
    ReadPolicyFromResourceL( aResourceFile, aResourceId );
    }

// -----------------------------------------------------------------------------
// CMRPolicyResourceIterator::HasNext
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMRPolicyResourceIterator::HasNext() const
    {
    return ( iArrayIndex < iPolicyResourceIds.Count() - 1 );
    }

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::NextPolicyL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicy* CMRPolicyResourceIterator::NextPolicyL()
    {
    FUNC_LOG;
    
    CESMRPolicy* policy = NextPolicyLC();
    CleanupStack::Pop( policy );
    
    return policy;
    }

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::NextPolicyLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicy* CMRPolicyResourceIterator::NextPolicyLC()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( HasNext(), Panic( EMRPolicyIteratorExhausted ) );
    
    CESMRPolicy* policy = ReadPolicyLC( iPolicyResourceIds[ ++iArrayIndex ] );
    
    return policy;
    }

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::Reset
// ---------------------------------------------------------------------------
//
EXPORT_C void CMRPolicyResourceIterator::Reset()
    {
    FUNC_LOG;
    
    iArrayIndex = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::ReadPolicyFromResourceL
// ---------------------------------------------------------------------------
//
void CMRPolicyResourceIterator::ReadPolicyFromResourceL(
        const TDesC& aPolicyFile,
        TInt aPolicyArrayResourceId )
    {
    FUNC_LOG;

    iPolicyResourceFile.Close();
    iPolicyResourceFile.OpenL( CCoeEnv::Static()->FsSession(), aPolicyFile );
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

    }


// ---------------------------------------------------------------------------
// CMRPolicyResourceIterator::ReadPolicyL
// ---------------------------------------------------------------------------
//
CESMRPolicy* CMRPolicyResourceIterator::ReadPolicyLC( TInt aResourceId )
    {
    FUNC_LOG;

    HBufC8* resourceBuffer = iPolicyResourceFile.AllocReadLC( aResourceId );
    
    TResourceReader reader;
    reader.SetBuffer(resourceBuffer);
    
    CESMRPolicy* policy = CESMRPolicy::NewL( iPolicyResourceFile, reader );
    
    CleanupStack::PopAndDestroy( resourceBuffer );
    CleanupStack::PushL( policy );
    
    return policy;
    }

// EOF

