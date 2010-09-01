/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the class CESMRMixedFieldStorage
*
*/

#include "emailtrace.h"
#include "cesmrmixedfieldstorage.h"

//<cmail>
#include "cesmrpolicy.h"
#include "esmrdef.h"
//</cmail>

#include "cesmrmeetingtimevalidator.h"
#include "tesmrentryfield.h"
#include "mesmrresponseobserver.h"
#include "mesmrcalentry.h"
#include "cesmrvalidatorfactory.h"
#include "cesmrglobalnote.h"
#include "cesmrfield.h"
#include "cesmrfieldeventqueue.h"

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

/**
 * Tests, if entry has valid recurrence for editing.
 * @param aEntry Reference to entry.
 * @param aRecurrenceValue On return contains entrys recurrence
 * @return ETrue, if entry has valid recurrence for editing.
 *         EFalse, if entry hasn't valid recurrence for editing.
 */
TBool HasValidRecurrenceForEditingL(
        const MESMRCalEntry& aEntry,
        TESMRRecurrenceValue& aRecurrenceValue )
    {
    TBool retValue( ETrue );

    if ( aEntry.IsRecurrentEventL() )
        {
        TTime until;

        aEntry.GetRecurrenceL( aRecurrenceValue, until );

        if ( ERecurrenceNot == aRecurrenceValue ||
             ERecurrenceUnknown == aRecurrenceValue )
            {
            // Entry has unrecognized recurrende type
            // --> Cannot be edited.
            retValue = EFalse;
            }
        }
    return retValue;
    }

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
            }
            break;

        case MESMRFieldValidator::EErrorRecDifferetStartAndEnd:
            {
            aUpdatedFocus = EESMRFieldStopDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRRepeatDifferentStartAndEndDate );
            }
            break;

        case MESMRFieldValidator::EErrorRecUntilEarlierThanStart:
            {
            aUpdatedFocus = EESMRFieldRecurrenceDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRRepeatEndEarlierThanItStart );
            }
            break;

        case MESMRFieldValidator::EErrorAlarmLaterThanStart:
            {
            aUpdatedFocus = EESMRFieldAlarmDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRCalenLaterDate );
            }
            break;

        case MESMRFieldValidator::EErrorAlarmInPast:
            {
            aUpdatedFocus = EESMRFieldAlarmDate;
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRAlarmAlreadyPassed );
            }
            break;
            
        case MESMRFieldValidator::EErrorRescheduleInstance:
            {
            CESMRGlobalNote::ExecuteL(
                                CESMRGlobalNote::EESMRRepeatReSchedule );
            }
            break;
            
        case MESMRFieldValidator::EErrorInstanceOverlapsExistingOne:
            {
            CESMRGlobalNote::ExecuteL(
                                CESMRGlobalNote::EESMROverlapsExistingInstance );
            }
            break;
            
        case MESMRFieldValidator::EErrorInstanceAlreadyExistsOnThisDay:
            {
            CESMRGlobalNote::ExecuteL(
                                CESMRGlobalNote::EESMRInstanceAlreadyExistsOnThisDay );
            }
            break;
            
        case MESMRFieldValidator::EErrorInstanceOutOfSequence:
            {
            CESMRGlobalNote::ExecuteL(
                                CESMRGlobalNote::EESMRInstanceOutOfSequence );
            }
            break;
            
        default:
            err = KErrNone;
            break;
        }

    User::LeaveIfError( err );
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::CESMRMixedFieldStorage
// ---------------------------------------------------------------------------
//
CESMRMixedFieldStorage::CESMRMixedFieldStorage(
        MESMRFieldEventObserver& aEventObserver,
        MESMRResponseObserver* aResponseObserver ) :
    CESMRFieldStorage( aEventObserver),
    iResponseObserver(aResponseObserver)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::~CESMRMixedFieldStorage
// ---------------------------------------------------------------------------
//
CESMRMixedFieldStorage::~CESMRMixedFieldStorage( )
    {
    FUNC_LOG;
    delete iValidator;
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::NewL
// ---------------------------------------------------------------------------
//
CESMRMixedFieldStorage* CESMRMixedFieldStorage::NewL(
        MESMRFieldEventObserver& aEventObserver,
        const CESMRPolicy& aPolicy,
        MESMRResponseObserver* aResponseObserver,
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRMixedFieldStorage* self =
            new (ELeave) CESMRMixedFieldStorage(
                    aEventObserver,
                    aResponseObserver );

    CleanupStack::PushL( self );
    self->ConstructL( aPolicy, aEntry );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::ConstructL(
        const CESMRPolicy& aPolicy,
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRFieldStorage::BaseConstructL();
    
    DoChangePolicyL( aPolicy, aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRFieldStorage::ExternalizeL ( aEntry );
    if ( iValidator )
        {
        iValidator->StoreValuesToEntryL( aEntry );
        }
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRFieldStorage::InternalizeL ( aEntry );
    if ( iValidator )
        {
        iValidator->ReadValuesFromEntryL( aEntry );
        }
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::Validate
// ---------------------------------------------------------------------------
//
TInt CESMRMixedFieldStorage::Validate(
        TESMREntryFieldId& aUpdatedFocus, TBool aForceValidation )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    if ( iValidator )
        {
        MESMRFieldValidator::TESMRFieldValidatorError error;
        TRAP( err, error = iValidator->ValidateL( aForceValidation ) );

        if ( !aForceValidation )
            {
            // If error note does not success, there is much we can do.
            TRAP( err, ShowValidationErrorL( error, aUpdatedFocus ) );
            }
        else
            {
            // Force exit is used.
            err = KErrNone;
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::ChangePolicyL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::ChangePolicyL(
        const CESMRPolicy& aNewPolicy,
        MESMRCalEntry& aEntry )
    {
    // Delegate to non-virtual
    DoChangePolicyL( aNewPolicy, aEntry );
    }


// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::ConstructForwardEventL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::ConstructForwardEventL(
        const CESMRPolicy& aPolicy,
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    iValidator = aValidator;
    
    RArray< TESMREntryField > array = aPolicy.Fields();

    TInt fieldCount( array.Count() );
    for (TInt i(0); i < fieldCount; i++ )
        {
        CESMRField* field =  NULL;
        TBool visible = ( array[i].iFieldViewMode == EESMRFieldTypeDefault );
        switch ( array[i].iFieldId )
            {            
            case EESMRFieldAttendee: //Fall through 
            case EESMRFieldOptAttendee: //Fall through
            case EESMRFieldDescription:
                {
                field = CreateEditorFieldL( iValidator, array[i] );
                }
                break;
            default:
                {
                field =  
					CreateViewerFieldL( iResponseObserver, array[i], visible );
                field->LockL();
                }
                break;
            }
        
        if( field->FieldViewMode() != EESMRFieldTypeDisabled )
            {
            CleanupStack::PushL( field );
            AddFieldL( field, visible );
            CleanupStack::Pop( field );
            }
        else
            {
            delete field;
            field = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::ConstructRecurrentEventL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::ConstructRecurrentEventL(
        const CESMRPolicy& aPolicy,
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    iValidator = aValidator;
    iValidator->SetFieldEventQueue( &EventQueueL() );

    const RArray<TESMREntryField>& array = aPolicy.Fields();
    TInt fieldCount( array.Count() );
    for (TInt i(0); i < fieldCount; ++i )
        {
        CESMRField* field = NULL;
        TBool visible = array[i].iFieldViewMode == EESMRFieldTypeDefault;
        switch ( array[i].iFieldId )
            {
            case EESMRFieldRecurrence: //Fall through
            case EESMRFieldRecurrenceDate:
            default:
                field =  CreateEditorFieldL( iValidator, array[i] );
                break;
            }

        if ( field )
            {
            CleanupStack::PushL( field );
            AddFieldL(field, visible );
            CleanupStack::Pop( field );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::ConstructEditSeriesEventL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::ConstructEditSeriesEventL(
        const CESMRPolicy& aPolicy,
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    TESMRRecurrenceValue recurrenceType;
    TBool validRecurrence(
            HasValidRecurrenceForEditingL(
                    *iEntry,
                    recurrenceType ) );

    iValidator = aValidator;
    iValidator->SetFieldEventQueue( &EventQueueL() );
    
    const RArray<TESMREntryField>& array = aPolicy.Fields();
    CESMRField* field =  NULL;
    
    TInt fieldCount(  array.Count() );
    for (TInt i(0); i < fieldCount; i++ )
        {
        const TESMREntryField& tfield = array[i];
        TBool visible = tfield.iFieldViewMode == EESMRFieldTypeDefault;
        switch ( tfield.iFieldId )
            {
            // Flowthrough
            case EESMRFieldRecurrence:
            case EESMRFieldRecurrenceDate:
                {
                if ( validRecurrence )
                    {
                    field = CreateEditorFieldL( iValidator, tfield );
                    }
                else
                    {
                    // Entry has unrecognized recurrende type
                    // --> Cannot be edited.
                    field = CreateViewerFieldL( 
								iResponseObserver, tfield, visible );
                    field->LockL();
                    }
                }
                break;
            default:
                {
                field = CreateEditorFieldL( iValidator, tfield );
                }
                break;
            }

        if( field->FieldViewMode() != EESMRFieldTypeDisabled )
            {
            CleanupStack::PushL( field );
            AddFieldL( field, visible );
            CleanupStack::Pop( field );
            }
        else
            {
            delete field;
            field = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::ConstructEditSeriesEventL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::Reset()
    {
    CESMRFieldStorage::Reset();
    delete iValidator;
    iValidator = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRMixedFieldStorage::DoChangePolicyL
// ---------------------------------------------------------------------------
//
void CESMRMixedFieldStorage::DoChangePolicyL(
        const CESMRPolicy& aNewPolicy,
        MESMRCalEntry& aEntry )
    {
    Reset();
    iEntry = &aEntry;
    
    // FORWARD allows only attendee fields and description to be edited.
    // EDIT RECURRENT EVENT allows only start-end time and start date
    // to be edited.

    MESMRCalEntry::TESMRRecurrenceModifyingRule rule(
            iEntry->RecurrenceModRule() );

    if ( iEntry->IsRecurrentEventL() &&
         rule == MESMRCalEntry::EESMRAllInSeries  &&
         EESMREditMR == aNewPolicy.ViewMode())
        {
        // Contruct edit series
        // validator ownership is transferred
        MESMRFieldValidator* validator =
        CESMRValidatorFactory::CreateValidatorL (
                aNewPolicy.EventType() );

        ConstructEditSeriesEventL(
                aNewPolicy,
                validator );

        iEventType = EMixedFieldStorageEditSeriesEvent;
        }

    else if ( aNewPolicy.ViewMode() != EESMRForwardMR )
        {
        MESMRFieldValidator* validator =
                CESMRValidatorFactory::CreateValidatorL (
                        aNewPolicy.EventType() );

        ConstructRecurrentEventL ( aNewPolicy,
                validator );

        iEventType = EMixedFieldStorageRecurrentEvent;
        }
    else
        {
        // No validator is needed because forwarding does not
        // affecto to any time fields.
        MESMRFieldValidator* validator = NULL;
        ConstructForwardEventL (
                aNewPolicy,
                validator );

        iEventType = EMixedFieldStorageForward;
        }
    }

// EOF

