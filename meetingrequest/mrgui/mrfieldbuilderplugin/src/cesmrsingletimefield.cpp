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
* Description:  ESMR single time field implementation
 *
*/


#include "emailtrace.h"
#include <eikmfne.h>
#include <eiklabel.h>
#include <AknsConstants.h>
#include <avkon.hrh>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>

#include "cesmrsingletimefield.h"
#include "mesmrfieldvalidator.h"
#include "cesmrglobalnote.h"
#include "esmrfieldbuilderdef.h"
#include <AknUtils.h>

namespace{  // codescanner::namespace
#define KMinimumTime (TTime(0)) // codescanner::baddefines

// 24 hours in microseconds
const TInt64 KDayInMicroSeconds = 86400000000;
#define KMaximumTime (TTime(KDayInMicroSeconds)) // codescanner::baddefines 
}

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::CESMRSingleTimeField
// ---------------------------------------------------------------------------
//
CESMRSingleTimeField::CESMRSingleTimeField(
        MESMRFieldValidator* aValidator ) :
    iValidator( aValidator )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::~CESMRSingleTimeField
// ---------------------------------------------------------------------------
//
CESMRSingleTimeField::~CESMRSingleTimeField( )
    {
    FUNC_LOG;
    delete iFrameBgContext;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::NewL
// ---------------------------------------------------------------------------
//
CESMRSingleTimeField* CESMRSingleTimeField::NewL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRSingleTimeField* self =
            new (ELeave) CESMRSingleTimeField( aValidator );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::ConstructL( )
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldAlarmTime );

    TTime startTime;
    startTime.HomeTime();

    iTime = new (ELeave) CEikTimeEditor();
    iTime->ConstructL(
            KMinimumTime,
            KMaximumTime,
            startTime,
            EEikTimeWithoutSecondsField );
    iTime->SetUpAndDownKeysConsumed( EFalse );

    iBackground = AknsDrawUtils::ControlContext( this );

    CESMRIconField::ConstructL(
            KAknsIIDQgnFscalIndiAlarmTime,
            iTime );

    if ( iValidator )
        {
        iValidator->SetAlarmTimeFieldL( *iTime );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::InitializeL()
    {
    FUNC_LOG;
    iTime->SetFont ( iLayout->Font (iCoeEnv, iFieldId ) );
    iTime->SetMfneAlignment ( CESMRLayoutManager::IsMirrored ( ) ? EAknEditorAlignRight
            : EAknEditorAlignLeft );

    iTime->SetBorder( TGulBorder::ENone );
    iTime->SetSkinTextColorL( iLayout->NormalTextColorID ( ) );
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRSingleTimeField::OkToLoseFocusL(
        TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    TInt err(KErrNone );
    if ( iValidator )
        {
        TRAP( err, iValidator->AlarmTimeChangedL() );
        }
    if ( err != KErrNone )
        {
        CESMRGlobalNote::ExecuteL (
                CESMRGlobalNote::EESMRCalenLaterDate );
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRSingleTimeField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( aType == EEventKey )
        {
        TInt fieldIndex( iTime->CurrentField() );

        switch ( aEvent.iScanCode )
            {
            // make sure these events are NOT consumed
            case EStdKeyUpArrow:
            case EStdKeyDownArrow:
                break;

            default:
                response = iTime->OfferKeyEventL ( aEvent, aType );
                break;
            }

        CheckIfValidatingNeededL( fieldIndex );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    TRect rect(TPoint(iTime->Position()), iTime->Size());
    TRect inner(rect);
    TRect outer(rect);

    iTime->SetSize(iLayout->FieldSize( FieldId() ));
    
    delete iFrameBgContext;
    iFrameBgContext = NULL;
    iFrameBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrInput, outer, inner, EFalse ) ;

    iFrameBgContext->SetParentContext( iBackground );
    iTime->SetSkinBackgroundControlContextL(iFrameBgContext);
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::PositionChanged
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::PositionChanged()
    {
    FUNC_LOG;
    CCoeControl::PositionChanged();
    if( iFrameBgContext )
        {
        iFrameBgContext->SetFrameRects( iTime->Rect(), iTime->Rect() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::CheckIfValidatingNeededL(
        TInt aStartFieldIndex )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TInt fieldIndex( iTime->CurrentField() );

    if ( fieldIndex != aStartFieldIndex )
        {
        TRAP( err, iValidator->AlarmTimeChangedL() );
        }

    if ( err != KErrNone )
        {
        CESMRGlobalNote::ExecuteL (
                CESMRGlobalNote::EESMRCalenLaterDate );
        }
    }
// ---------------------------------------------------------------------------
// CESMRSingleTimeField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// EOF

