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
* Description:  ESMR Field storage for editor dialog
*
*/

#include "emailtrace.h"
#include "cesmreditorfieldstorage.h"

//<cmail>
#include "cesmrpolicy.h"
#include "esmrdef.h"
//</cmail>

#include "mesmrfieldvalidator.h"
#include "tesmrentryfield.h"
#include "cesmrvalidatorfactory.h"
#include "cesmrglobalnote.h"
#include "cesmrfield.h"
#include "cesmrfieldeventqueue.h"

//logging
//performance profiling
// <cmail> Removed profiling. </cmail>

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG

// Definition for panic literal
_LIT( KESMREditorFieldStorage, "ESMREditorFieldStorage" );

/** CESMREditorFieldStorage panic codes */
enum TSMREditorFieldStoragePanic
    {
    /** No validator defined */
    EESMREditorFieldStorageNoValidator = 0
    };

void Panic( TSMREditorFieldStoragePanic aPanic )
    {

    User::Panic( KESMREditorFieldStorage, aPanic );
    }

#endif // _DEBUG

/**
 * Shows validation error.
 * @param aError Defines the validation error
 * @param aUpdatedFocus On return contains the id of the field,
 *                      which should be focused.
 */
void ShowValidationErrorL(
        const MESMRFieldValidator::TESMRFieldValidatorError& aError,
        TESMREntryFieldId& aUpdatedFocus )
    {
    FUNC_LOG;
    TInt err( KErrArgument );
    switch ( aError )
        {
        case MESMRFieldValidator::EErrorEndEarlierThanStart:
            {
            aUpdatedFocus = EESMRFieldMeetingTime;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMREntryEndEarlierThanItStart );
            break;
            }

        case MESMRFieldValidator::EErrorRecDifferetStartAndEnd:
            {
            aUpdatedFocus = EESMRFieldStopDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRRepeatDifferentStartAndEndDate );
            break;
            }

        case MESMRFieldValidator::EErrorRecUntilEarlierThanStart:
            {
            aUpdatedFocus = EESMRFieldRecurrenceDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRRepeatEndEarlierThanItStart );
            break;
            }

        case MESMRFieldValidator::EErrorAlarmLaterThanStart:
            {
            aUpdatedFocus = EESMRFieldAlarmDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRCalenLaterDate );
            break;
            }

        case MESMRFieldValidator::EErrorAlarmInPast:
            {
            aUpdatedFocus = EESMRFieldAlarmDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRAlarmAlreadyPassed );
            break;
            }

        case MESMRFieldValidator::EErrorAlarmTooMuchInPast:
            {
            aUpdatedFocus = EESMRFieldAlarmDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRDiffMoreThanMonth );
            break;
            }

        case MESMRFieldValidator::EErrorRelativeAlarmInPast:
            {
            aUpdatedFocus = EESMRFieldAlarm;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRAlarmAlreadyPassed );
            break;
            }

        case MESMRFieldValidator::EErrorRescheduleInstance:
            {
            CESMRGlobalNote::ExecuteL(
                                CESMRGlobalNote::EESMRRepeatReSchedule );
            }
            break;
        default:
            {
            err = KErrNone;
            break;
            }
        }

    User::LeaveIfError( err );
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMREditorFieldStorage::CESMREditorFieldStorage
// ---------------------------------------------------------------------------
//
CESMREditorFieldStorage::CESMREditorFieldStorage(
        MESMRFieldEventObserver& aEventObserver )
:   CESMRFieldStorage( aEventObserver )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMREditorFieldStorage::~CESMREditorFieldStorage
// ---------------------------------------------------------------------------
//
CESMREditorFieldStorage::~CESMREditorFieldStorage( )
    {
    FUNC_LOG;
    delete iValidator;
    }

// ---------------------------------------------------------------------------
// CESMREditorFieldStorage::NewL
// ---------------------------------------------------------------------------
//
CESMREditorFieldStorage* CESMREditorFieldStorage::NewL(
        CESMRPolicy* aPolicy,
        MESMRFieldEventObserver& aEventObserver )
    {
    FUNC_LOG;
    CESMREditorFieldStorage* self =
            new (ELeave) CESMREditorFieldStorage( aEventObserver );
    CleanupStack::PushL ( self );
    self->ConstructL ( aPolicy );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMREditorFieldStorage::ConstructL
// ---------------------------------------------------------------------------
//
void CESMREditorFieldStorage::ConstructL( CESMRPolicy* aPolicy )
    {
    FUNC_LOG;
    CESMRFieldStorage::BaseConstructL();
    iValidator =
            CESMRValidatorFactory::CreateValidatorL (
                    aPolicy->EventType() );
    
    __ASSERT_DEBUG( iValidator, Panic( EESMREditorFieldStorageNoValidator) );

    iValidator->SetFieldEventQueue( &EventQueueL() );
    
    RArray<TESMREntryField> fields = aPolicy->Fields();
    const TInt count(fields.Count());
    
    for (TInt i(0); i < count; i++ )
        {
        TESMREntryField entryField = fields[i];
        TBool visible( entryField.iFieldViewMode == EESMRFieldTypeDefault);
        CESMRField* field = CreateEditorFieldL( iValidator, entryField );
        
        CleanupStack::PushL( field );
        AddFieldL( field, visible );
        CleanupStack::Pop( field );
        }
    
    }
        
// ---------------------------------------------------------------------------
// CESMREditorFieldStorage::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMREditorFieldStorage::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRFieldStorage::ExternalizeL ( aEntry );
    iValidator->StoreValuesToEntryL( aEntry );
    }

// ---------------------------------------------------------------------------
// CESMREditorFieldStorage::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMREditorFieldStorage::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRFieldStorage::InternalizeL ( aEntry );
    iValidator->ReadValuesFromEntryL( aEntry );
    }

// ---------------------------------------------------------------------------
// CESMREditorFieldStorage::Validate
// ---------------------------------------------------------------------------
//
TInt CESMREditorFieldStorage::Validate(
        TESMREntryFieldId& aUpdatedFocus, TBool aForceValidation )
    {
    FUNC_LOG;
    MESMRFieldValidator::TESMRFieldValidatorError error;
    TRAPD( err, error = iValidator->ValidateL( aForceValidation ) );

    if ( !aForceValidation )
        {
        // If error note does not success, there is much we can do.
        TRAP( err, ShowValidationErrorL( error, aUpdatedFocus ) );
        }
    else
        {
        // force exit is used.
        err = KErrNone;
        }

    return err;
    }

// EOF

