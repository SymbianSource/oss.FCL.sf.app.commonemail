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
* Description:  ESMR date field impl.
 *
*/

#include "emailtrace.h"
#include "cesmrdatefield.h"

#include <eikmfne.h>
#include <avkon.hrh>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

#include "cesmrmeetingtimevalidator.h"
#include "cesmrglobalnote.h"
#include "esmrfieldbuilderdef.h"
#include "mesmrlistobserver.h"
#include "cesmrgenericfieldevent.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRDateField::CESMRDateField
// ---------------------------------------------------------------------------
//
CESMRDateField::CESMRDateField(
        MESMRFieldValidator* aValidator ) :
    iValidator(aValidator)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRDateField::~CESMRDateField
// ---------------------------------------------------------------------------
//
CESMRDateField::~CESMRDateField( )
    {
    FUNC_LOG;
    delete iFrameBgContext;
    }

// ---------------------------------------------------------------------------
// CESMRDateField::NewL
// ---------------------------------------------------------------------------
//
CESMRDateField* CESMRDateField::NewL(
        MESMRFieldValidator* aValidator,
        TESMREntryFieldId aId )
    {
    FUNC_LOG;
    CESMRDateField* self = new (ELeave) CESMRDateField( aValidator );
    CleanupStack::PushL ( self );
    self->ConstructL( aId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRDateField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRDateField::ConstructL( TESMREntryFieldId aId )
    {
    FUNC_LOG;
    SetFieldId ( aId );

    TTime startTime;
    startTime.UniversalTime ( );

    iDate = new (ELeave) CEikDateEditor;
    iDate->ConstructL( TTIME_MINIMUMDATE, TTIME_MAXIMUMDATE, startTime, EFalse );
    iDate->SetUpAndDownKeysConsumed ( EFalse );
    
    TAknsItemID aIconID = (iFieldId == EESMRFieldStartDate) ? KAknsIIDQgnMeetReqIndiDateStart
            : KAknsIIDQgnMeetReqIndiDateEnd;

    iBackground = AknsDrawUtils::ControlContext( this );
    CESMRIconField::ConstructL( aIconID, iDate ); // ownership transferred

    // Initialize validator
    if ( iValidator )
        {
        switch ( iFieldId )
            {
            case EESMRFieldStartDate:
                iValidator->SetStartDateFieldL( *iDate );
                break;
            case EESMRFieldStopDate:
                iValidator->SetEndDateFieldL( *iDate );
                break;
            case EESMRFieldAlarmDate:
                iValidator->SetAlarmDateFieldL( *iDate );
                break;
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRDateField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRDateField::InitializeL()
    {
    FUNC_LOG;
    iDate->SetFont( iLayout->Font(iCoeEnv, iFieldId ) );
    iDate->SetMfneAlignment( CESMRLayoutManager::IsMirrored()
                             ? EAknEditorAlignRight : EAknEditorAlignLeft );

    iDate->SetSkinTextColorL( iLayout->NormalTextColorID() );
    iDate->SetBorder( TGulBorder::ENone );
    }

// ---------------------------------------------------------------------------
// CESMRDateField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRDateField::OkToLoseFocusL(
        TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    return TriggerValidatorL();
    }

// ---------------------------------------------------------------------------
// CESMRDateField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRDateField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);

    if ( aType == EEventKey )
        {
        TInt fieldIndex( iDate->CurrentField() );

        // flowthrough, these events shouldn't be consumed
        if ( aEvent.iScanCode != EStdKeyUpArrow &&
             aEvent.iScanCode != EStdKeyDownArrow )
            {
            response = iDate->OfferKeyEventL( aEvent, aType );
            }
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRDateField::PositionChanged()
// ---------------------------------------------------------------------------
//
void CESMRDateField::PositionChanged()
    {
    FUNC_LOG;
    CCoeControl::PositionChanged();
    if( iFrameBgContext )
        {
        iFrameBgContext->SetFrameRects( iDate->Rect(), iDate->Rect() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRDateField::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRDateField::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    TRect rect(TPoint(iDate->Position()), iDate->Size());
    TRect inner(rect);
    TRect outer(rect);

    iDate->SetSize(iLayout->FieldSize( FieldId() ));
    
    delete iFrameBgContext;
    iFrameBgContext = NULL;
    iFrameBgContext = CAknsFrameBackgroundControlContext::NewL(KAknsIIDQsnFrInput, outer, inner, EFalse ) ;

    iFrameBgContext->SetParentContext( iBackground );
    iDate->SetSkinBackgroundControlContextL(iFrameBgContext);
    }

// ---------------------------------------------------------------------------
// CESMRDateField::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CESMRDateField::CheckIfValidatingNeededL(
        TInt aStartFieldIndex )
    {
    FUNC_LOG;
    TInt fieldIndex( iDate->CurrentField() );

    if ( fieldIndex != aStartFieldIndex )
        {
        TriggerValidatorL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRDateField::TriggerValidatorL
// ---------------------------------------------------------------------------
//
TBool CESMRDateField::TriggerValidatorL()
    {
    FUNC_LOG;
    TInt err( KErrNone );

    if (iValidator )
        {
        switch ( iFieldId )
            {
            case EESMRFieldStartDate:
                TRAP( err, iValidator->StartDateChandedL() );
                break;
            case EESMRFieldStopDate:
                TRAP(err, iValidator->EndDateChangedL() );
                break;
            case EESMRFieldAlarmDate:
                TRAP(err, iValidator->AlarmDateChangedL() );
                break;
            default:
                break;
            }
        }


    if ( KErrNone != err )
        {
        switch ( iFieldId )
            {
            case EESMRFieldStartDate:
                {
                CESMRGlobalNote::ExecuteL(
                        CESMRGlobalNote::EESMRRepeatReSchedule );                
                }
                break;                
            case EESMRFieldStopDate:
                {
                if ( err == KErrArgument )
                    {
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMREndsBeforeStarts );
                    }
                else if ( err == KErrOverflow )
                    {
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatReSchedule );                    
                    }
                }
                break;

            case EESMRFieldAlarmDate:
                {
                CESMRGlobalNote::ExecuteL(
                        CESMRGlobalNote::EESMRCalenLaterDate );
                }
                break;
            
            default:
                break;
            }
        }
    
    return ( KErrNone == err );
    }

// ---------------------------------------------------------------------------
// CESMRDateField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRDateField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// EOF
