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
* Description:  ESMR policy implementation
*
*/


//<cmail>
#include "emailtrace.h"
#include "cesmrpolicy.h"

#include <e32base.h>
#include <coemain.h>
#include <barsread.h>

#include "esmrdef.h"
//</cmail>
#include "tesmrentryfield.h"

/// Unnamed namespace for local definitions
namespace {

// Definition for flag position
const TInt KFlagPosition = 1;

#ifdef _DEBUG

// Definition for min flag position
const TInt KMinFlagPosition = 0;

// Definition for policy panic string
_LIT( KESMRPolicy, "ESMRPolicy" );

// Policy panic codes
enum TESMRPolicyPanic
    {
    EESMRPolicyInvalidCommand = 1,  // Invalid command
    EESMRPolicyInvalidRole          // Invalid role
    };

/**
 * Raises policy panic.
 *
 * @param aPanic Panic code.
 */
void Panic( TESMRPolicyPanic aPanic )
    {

    User::Panic( KESMRPolicy, aPanic );
    }

#endif

/**
 * Reads ESMR field table from resource.
 *
 * @param aReader Reference to resource reader.
 * @param aFieldTable Reference to field table.
 */
void ReadESMRFieldTableL(
        TResourceReader& aReader,
        RArray<TESMREntryField>& aFieldTable )
    {
    // The first WORD contains the number
    // of DATA structs within the resource
    TInt numOfFields = aReader.ReadInt16();

    for (TInt i(0); i < numOfFields; i++ )
        {
        TESMREntryField field;

        field.iFieldId =
            static_cast<TESMREntryFieldId>( aReader.ReadInt16() );

        field.iFieldViewMode =
            static_cast<TESMRFieldType>( aReader.ReadInt16() );

        field.iFieldMode =
            static_cast<TESMRFieldMode>( aReader.ReadInt16() );

        aFieldTable.AppendL(field);
        }
    }

/**
 * Reads ESMR command table from resource.
 *
 * @param aReader Reference to resource reader.
 * @param aCommandTable Reference to command table.
 */
void ReadESMRCommandTableL(
        TResourceReader& aReader,
        RArray<TESMRCommand>& aCommandTable )
    {
    // The first WORD contains the number
    // of DATA structs within the resource
    TInt numOfFields = aReader.ReadInt16();
    for (TInt i(0); i < numOfFields; i++ )
        {
        TESMRCommand command =
            static_cast<TESMRCommand>( aReader.ReadInt32());
        aCommandTable.AppendL(command);
        }
    }

/**
 * Compares entry fields by id.
 *
 * @param @aLhs Reference to left hand side object
 * @param @aRhs Reference to right hand side object
 */
TBool MatchesById(
        const TESMREntryField &aLhs,
        const TESMREntryField &aRhs )
    {
    return aLhs.iFieldId == aRhs.iFieldId;
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRPolicy::CESMRPolicy
// ---------------------------------------------------------------------------
//
inline CESMRPolicy::CESMRPolicy()
:   iPolicyId(EESMRPolicyUnspecified),
    iViewMode(EESMRViewUndef)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRPolicy::~CESMRPolicy
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicy::~CESMRPolicy()
    {
    FUNC_LOG;
    iMRFields.Reset();
    iDefaultCommands.Reset();
    iAdditionalCommands.Reset();
    }

// ---------------------------------------------------------------------------
// CESMRPolicy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicy* CESMRPolicy::NewL()
    {
    FUNC_LOG;

    CESMRPolicy* self = new (ELeave) CESMRPolicy();

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRPolicy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRPolicy* CESMRPolicy::NewL(
        RResourceFile& aRFile,
        TResourceReader& aReader )
    {
    FUNC_LOG;

    CESMRPolicy* self = new (ELeave) CESMRPolicy();
    CleanupStack::PushL( self );
    self->ConstructL( aRFile, aReader );
    CleanupStack::Pop(self);


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRPolicy::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRPolicy::ConstructL(
        RResourceFile& aRFile,
        TResourceReader& aReader )
    {
    FUNC_LOG;

    ReadFromResourceL( aRFile, aReader );

    }

// ---------------------------------------------------------------------------
// CESMRPolicy::ReadFromResourceL
// ---------------------------------------------------------------------------
//
void CESMRPolicy::ReadFromResourceL(
        RResourceFile& aRFile,
        TResourceReader& aReader )
    {
    FUNC_LOG;

    iEventType = static_cast<TESMRCalendarEventType>(aReader.ReadInt16() );
    iPolicyId = static_cast<TESMRPolicyID>( aReader.ReadInt16() );
    iViewMode = static_cast<TESMRViewMode>( aReader.ReadInt16() );
    iRoleFlags.iFlags = aReader.ReadInt16();
    iAllowedApp = static_cast<TESMRUsingApp>( aReader.ReadInt16() );

    TInt defaultFieldIdLink    = aReader.ReadInt32();
    TInt defaultCommandLink    = aReader.ReadInt32();
    TInt additionalCommandLink = aReader.ReadInt32();

    HBufC8* resourceBuffer = NULL;

    iMRFields.Reset();
    if ( defaultFieldIdLink )
        {
        resourceBuffer = aRFile.AllocReadLC(defaultFieldIdLink);
        aReader.SetBuffer(resourceBuffer);
        ReadESMRFieldTableL( aReader, iMRFields );
        CleanupStack::PopAndDestroy( resourceBuffer );
        resourceBuffer = NULL;
        }

    iDefaultCommands.Reset();
    if ( defaultCommandLink )
        {
        resourceBuffer = aRFile.AllocReadLC(defaultCommandLink);
        aReader.SetBuffer(resourceBuffer);
        ReadESMRCommandTableL( aReader, iDefaultCommands );
        CleanupStack::PopAndDestroy( resourceBuffer );
        resourceBuffer = NULL;
        }

    iAdditionalCommands.Reset();
    if ( additionalCommandLink )
        {
        resourceBuffer = aRFile.AllocReadLC(additionalCommandLink);
        aReader.SetBuffer(resourceBuffer);
        ReadESMRCommandTableL( aReader, iAdditionalCommands );
        CleanupStack::PopAndDestroy( resourceBuffer );
        resourceBuffer = NULL;
        }

    }

// ---------------------------------------------------------------------------
// CESMRPolicy::IsFieldIncluded
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRPolicy::IsFieldIncluded(
        TESMREntryFieldId aFieldId ) const
    {
    FUNC_LOG;

    TESMREntryField field;
    field.iFieldId = aFieldId;

    TInt index = iMRFields.Find( field, MatchesById );


    return index != KErrNotFound;
    }


// ---------------------------------------------------------------------------
// CESMRPolicy::FieldL
// ---------------------------------------------------------------------------
//
EXPORT_C const TESMREntryField& CESMRPolicy::FieldL(
        TESMREntryFieldId aFieldId ) const
    {
    FUNC_LOG;

    TESMREntryField field;
    field.iFieldId = aFieldId;

    TInt index = iMRFields.Find( field, MatchesById );
    if ( KErrNotFound == index )
        {

        User::Leave( KErrNotFound );
        }


    return iMRFields[index];
    }

// ---------------------------------------------------------------------------
// CESMRPolicy::IsDefaultCommand
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRPolicy::IsDefaultCommand( TInt aCommand ) const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aCommand >= EESMRCmdAcceptMR,
                    Panic(EESMRPolicyInvalidCommand) );

    TESMRCommand command =
            static_cast<TESMRCommand>( aCommand );

    TInt commandFound( iDefaultCommands.Find( command ) );


    return KErrNotFound !=  commandFound;
    }

// ---------------------------------------------------------------------------
// CESMRPolicy::IsAdditionalCommand
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRPolicy::IsAdditionalCommand( TInt aCommand ) const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aCommand > EESMRCmdAcceptMR &&
                    aCommand <= EESMRCmdDeleteMR,
                    Panic(EESMRPolicyInvalidCommand) );

    TESMRCommand command =
            static_cast<TESMRCommand>( aCommand );

    TInt commandFound( iAdditionalCommands.Find( command ) );


    return KErrNotFound !=  commandFound;
    }


// -----------------------------------------------------------------------------
// CESMRPolicy::IsRoleIncluded
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CESMRPolicy::IsRoleIncluded( TESMRRole aRole ) const
    {
    FUNC_LOG;

    TInt flagPosition = static_cast<TInt>(aRole) - KFlagPosition;
    __ASSERT_DEBUG( aRole >= KMinFlagPosition, Panic(EESMRPolicyInvalidRole) );


    return iRoleFlags.IsSet( flagPosition );
    }

// -----------------------------------------------------------------------------
// CESMRPolicy::EventType
// -----------------------------------------------------------------------------
//
EXPORT_C TESMRCalendarEventType CESMRPolicy::EventType() const
    {
    FUNC_LOG;
    return iEventType;
    }

// EOF

