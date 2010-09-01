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
* Description:  ESMR time (start-end) field implementation
 *
*/
#include "cmrtimecontainer.h"
#include "esmrfieldbuilderdef.h"
#include "mesmrfieldvalidator.h"
#include "nmrlayoutmanager.h"
#include "nmrcolormanager.h"
#include "cesmrglobalnote.h"
#include "nmrbitmapmanager.h"
#include "cmrlabel.h"

#include <layoutmetadata.cdl.h>
#include <eikmfne.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

// DEBUG
#include "emailtrace.h"

namespace  { // codescanner::namespace
// start time field count of 24-hour format
const TInt KNumsFieldOf24F = 3;

#define KMinimumTime (TTime(0)) // codescanner::baddefines

// 24 hours in microseconds
const TInt64 KDayInMicroSeconds = 86400000000;
#define KMaximumTime (TTime(KDayInMicroSeconds)) // codescanner::baddefines

}

// ---------------------------------------------------------------------------
// CMRTimeContainer::CMRTimeContainer
// ---------------------------------------------------------------------------
//
CMRTimeContainer::CMRTimeContainer(
        MESMRFieldValidator* aValidator,
        TESMREntryFieldId aFieldId  ) :
    iValidator(aValidator),
    iFieldId( aFieldId )
    {
    FUNC_LOG;
    // do nothing
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::~CMRTimeContainer
// ---------------------------------------------------------------------------
//
CMRTimeContainer::~CMRTimeContainer( )
    {
    FUNC_LOG;
    Reset();
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::NewL
// ---------------------------------------------------------------------------
//
CMRTimeContainer* CMRTimeContainer::NewL(
        MESMRFieldValidator* aValidator,
        CCoeControl* aParent,
        TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    CMRTimeContainer* self =
            new (ELeave) CMRTimeContainer( aValidator, aFieldId );
    CleanupStack::PushL ( self );
    self->ConstructL ( aParent );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::ConstructL(
        CCoeControl* /*aParent*/)
    {
    FUNC_LOG;
    InitializeL();
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::SetContainerWindowL(
        const CCoeControl& aControl )
    {
    FUNC_LOG;

    iContainerWindow = &aControl;

    CCoeControl::SetContainerWindowL ( *iContainerWindow );
    iStartTime->SetContainerWindowL ( *iContainerWindow );
    iEndTime->SetContainerWindowL ( *iContainerWindow );
    iSeparator->SetContainerWindowL( *iContainerWindow );

    iStartTime->SetParent( this );
    iEndTime->SetParent( this );
    iSeparator->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRTimeContainer::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0);

    if ( iStartTime )
        {
        ++count;
        }

    if ( iSeparator )
        {
        ++count;
        }

    if ( iEndTime )
        {
        ++count;
        }

    return count;
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRTimeContainer::ComponentControl(
        TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            return iStartTime;
        case 1:
            return iSeparator;
        case 2:
            return iEndTime;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );
    TAknTextComponentLayout startTimeLayout =
        NMRLayoutManager::GetTextComponentLayout(
                NMRLayoutManager::EMRTextLayoutTimeEditorStart );
    AknLayoutUtils::LayoutMfne( iStartTime, rect, startTimeLayout );

    NMRColorManager::SetColor( *iStartTime,
                               NMRColorManager::EMRMainAreaTextColor );

    TAknLayoutText hyphonLayout =
        NMRLayoutManager::GetLayoutText(
                rect, NMRLayoutManager::EMRTextLayoutTimeEditorSeparator );
    iSeparator->SetRect( hyphonLayout.TextRect() );

    NMRColorManager::SetColor( *iSeparator,
                               NMRColorManager::EMRMainAreaTextColor );

    TAknTextComponentLayout endTimeLayout =
        NMRLayoutManager::GetTextComponentLayout(
                NMRLayoutManager::EMRTextLayoutTimeEditorEnd );
    AknLayoutUtils::LayoutMfne( iEndTime, rect, endTimeLayout );

    NMRColorManager::SetColor( *iEndTime,
                               NMRColorManager::EMRMainAreaTextColor );

    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRTimeContainer::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TBool startFocusedBefore( iStartTime->IsFocused() );
    TInt startIndex( iStartTime->CurrentField() );

    TBool endFocusedBefore( iEndTime->IsFocused() );
    TInt endIndex( iEndTime->CurrentField() );

    TKeyResponse response = EKeyWasNotConsumed;
    if ( aType == EEventKey )
        {
        switch ( aEvent.iScanCode )
            {
            case EStdKeyLeftArrow:
                response = ChangeFocusLeftL( aEvent, aType );
                break;

            case EStdKeyRightArrow:
                response = ChangeFocusRightL( aEvent, aType );
                break;

            default: // other events than left/right arrows
                {
                response = FocusedEditor()->OfferKeyEventL( aEvent, aType );
                iStartTime->DrawDeferred();
                iEndTime->DrawDeferred();
                }
                break;
            }
        }

    if ( EKeyWasConsumed == response )
        {
        CheckIfValidatingNeededL(
                startFocusedBefore,
                startIndex,
                endFocusedBefore,
                endIndex );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::FocusChanged
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::FocusChanged(
        TDrawNow /*aDrawNow*/)
    {
    FUNC_LOG;
    if ( IsFocused ( ) ) // if focus is gained
        {
        iStartTime->SetFocus ( ETrue );
        }
    else
        {
        if ( iStartTime->IsFocused() )
            {
            iStartTime->SetFocus( EFalse, EDrawNow );
            }

        if ( iEndTime->IsFocused() )
            {
            iEndTime->SetFocus( EFalse, EDrawNow );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::FocusedEditor
// ---------------------------------------------------------------------------
//
CEikTimeEditor* CMRTimeContainer::FocusedEditor( )
    {
    FUNC_LOG;
    return iStartTime->IsFocused ( ) ? iStartTime : iEndTime;
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CMRTimeContainer::OkToLoseFocusL(
        TESMREntryFieldId /*aFrom*/,
        TESMREntryFieldId /*aTo*/ )
    {
    FUNC_LOG;
    // validate the current time
    FocusedEditor()->PrepareForFocusLossL();

    if ( iStartTime->IsFocused() )
        {
        TriggerStartTimeChangedL();
        }
    else
        {
        TriggerEndTimeChangedL();
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::SetBgRect
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::SetBgRect( TRect bgRect )
	{
	iBgRect = bgRect;
	}

// ---------------------------------------------------------------------------
// CMRTimeContainer::SetValidatorL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::SetValidatorL( MESMRFieldValidator* aValidator )
    {
    iValidator = aValidator;

    if ( iValidator )
        {
        iValidator->SetStartTimeFieldL( *iStartTime );
        iValidator->SetEndTimeFieldL( *iEndTime );
        }
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::SetValidatorL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::EnvironmentChangedL()
    {
    FUNC_LOG;

    Reset();
    InitializeL();

    SetContainerWindowL( *iContainerWindow );

    iStartTime->ActivateL();
    iEndTime->ActivateL();
    iSeparator->ActivateL();
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::ChangeFocusRightL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRTimeContainer::ChangeFocusRightL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response = EKeyWasNotConsumed;

    if ( iStartTime->IsFocused ( ) )
        {
        // if the focus is in the last field of start time, move focus
        // to end times first field.
        TInt current = iStartTime->CurrentField();
        TInt count = iStartTime->NumFields();
        if ( current == (count - 1) )
            {
            iStartTime->SetFocus( EFalse, EDrawNow );
            iEndTime->SetFocus( ETrue, EDrawNow );
            response = EKeyWasConsumed;
            }
        else
            {
            response = iStartTime->OfferKeyEventL ( aEvent, aType );
            }
        TriggerStartTimeChangedL();
        }

    else if ( iEndTime->IsFocused ( ) )
        {
        if ( iEndTime->CurrentField() == (iEndTime->NumFields() - 1) )
            {
            iEndTime->SetFocus ( EFalse, EDrawNow );
            iStartTime->SetFocus ( ETrue, EDrawNow );
            response = EKeyWasConsumed;
            }
        else
            {
            response = iEndTime->OfferKeyEventL ( aEvent, aType );
            }
        TriggerEndTimeChangedL();
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::ChangeFocusLeftL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRTimeContainer::ChangeFocusLeftL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response = EKeyWasNotConsumed;
    if ( iStartTime->IsFocused ( ) )
        {
        // if the focus is in the first field of start time, move focus
        // to end times last field.
        if ( iStartTime->CurrentField ( )== 0 )
            {
            iStartTime->SetFocus ( EFalse, EDrawNow );
            iEndTime->SetFocus ( ETrue, EDrawNow );
            // last item should highlighted, API does not export the highlight
            // function, so emulate key presses
            response = iEndTime->OfferKeyEventL ( aEvent, aType );
            }
        else
            {
            response = iStartTime->OfferKeyEventL ( aEvent, aType );
            }
        
        TriggerStartTimeChangedL();
        }

    else if ( iEndTime->IsFocused ( ) )
        {
        if ( iEndTime->CurrentField ( )== 0 )
            {
            iEndTime->SetFocus ( EFalse, EDrawNow );
            iStartTime->SetFocus ( ETrue, EDrawNow );
            // last item should highlighted, API does not export the highlight
            // function, so emulate key presses
            response = iStartTime->OfferKeyEventL ( aEvent, aType );
            }
        else
            {
            response = iEndTime->OfferKeyEventL ( aEvent, aType );
            }

        TriggerEndTimeChangedL();
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::CheckIfValidatingNeededL(
        TBool aStartFocusedBefore,
        TInt aStartFieldIndex,
        TBool aEndFocusedBefore,
        TInt aEndFieldIndex )
    {
    FUNC_LOG;
    TBool startFocusedAfter( iStartTime->IsFocused() );
    TBool endFocusedAfter( iEndTime->IsFocused() );

    if ( aStartFocusedBefore != startFocusedAfter )
        {
        if ( startFocusedAfter )
            {
            TriggerEndTimeChangedL();
            }
        else
            {
            TriggerStartTimeChangedL();
            }
        }

    else if ( startFocusedAfter &&
              aStartFocusedBefore == startFocusedAfter )
        {
        TInt fieldIndex( iStartTime->CurrentField() );
        if ( fieldIndex != aStartFieldIndex )
            {
            TriggerStartTimeChangedL();
            }
        }

    else if ( endFocusedAfter &&
              aEndFocusedBefore == endFocusedAfter )
        {
        TInt fieldIndex( iEndTime->CurrentField() );
        if ( fieldIndex != aEndFieldIndex )
            {
            TriggerEndTimeChangedL();
            }
        }
    
	if ( iStartTime->IsFocused ( ) )
		{
		// if the focus is in the last field of start time
		TInt current = iStartTime->CurrentField();
		TInt count = iStartTime->NumFields();
		
		if ( ( count > KNumsFieldOf24F ) && ( current == (count - 1) ) )
			{
			TriggerStartTimeChangedL();
			}
		}
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::TriggerStartTimeChangedL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::TriggerStartTimeChangedL()
    {
    FUNC_LOG;
    if ( iValidator )
        {
        TRAPD( err, iValidator->StartTimeChangedL() );
        if ( err != KErrNone )
            {
            switch ( err )
                {
                case KErrOverflow:
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatReSchedule );
                    break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::TriggerEndTimeChangedL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::TriggerEndTimeChangedL()
    {
    FUNC_LOG;
    if ( iValidator )
        {
        TRAPD( err, iValidator->EndTimeChangedL() );
        if ( err != KErrNone )
            {
            switch ( err )
                {
                case KErrArgument:
                    // Assuming that end date is different
                    // and this is repeating entry
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatDifferentStartAndEndDate );
                    break;
                case KErrOverflow:
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatReSchedule );
                    break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    TBool startTimeEvent =
            iStartTime->Rect().Contains( aPointerEvent.iPosition );
    TBool endTimeEvent =
            iEndTime->Rect().Contains( aPointerEvent.iPosition );

    if( startTimeEvent )
        {
        if( !iStartTime->IsFocused() )
            {
            iStartTime->SetFocus( ETrue, ENoDrawNow );
            iEndTime->SetFocus( EFalse, ENoDrawNow );
            TriggerEndTimeChangedL();
            }

        CCoeControl::HandlePointerEventL( aPointerEvent );
        }

    if( endTimeEvent )
        {
        if( !iEndTime->IsFocused() )
            {
            iEndTime->SetFocus( ETrue, ENoDrawNow );
            iStartTime->SetFocus( EFalse, ENoDrawNow );
            TriggerStartTimeChangedL();
            }

        CCoeControl::HandlePointerEventL( aPointerEvent );
        }
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::ActivateL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::InitializeL
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::InitializeL()
    {
    FUNC_LOG;

    SetComponentsToInheritVisibility( ETrue );
    
    TTime startTime;
    startTime.UniversalTime ( );

    iStartTime = new (ELeave) CEikTimeEditor;
    iEndTime = new (ELeave) CEikTimeEditor;
    iSeparator = CMRLabel::NewL();

    iStartTime->ConstructL (
            KMinimumTime,
            KMaximumTime,
            startTime,
            EEikTimeWithoutSecondsField );

    iEndTime->ConstructL ( 
            KMinimumTime,
            KMaximumTime,
            startTime,
            EEikTimeWithoutSecondsField );

    TInt align = Layout_Meta_Data::IsMirrored() ? EAknEditorAlignRight
            : EAknEditorAlignLeft;
    iStartTime->SetMfneAlignment ( align );
    iEndTime->SetMfneAlignment ( align );

    iSeparator->SetTextL ( KTimeFieldSeparator );

    if ( iValidator )
        {
        iValidator->SetStartTimeFieldL( *iStartTime );
        iValidator->SetEndTimeFieldL( *iEndTime );
        }

    // Setting background instead of theme skin
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( NMRBitmapManager::EMRBitmapInputCenter );

    iBgCtrlContext = CAknsBasicBackgroundControlContext::NewL(
                bitmapStruct.iItemId,
                iBgRect,
                EFalse );

    iStartTime->SetSkinBackgroundControlContextL( iBgCtrlContext );
    iEndTime->SetSkinBackgroundControlContextL( iBgCtrlContext );
    }

// ---------------------------------------------------------------------------
// CMRTimeContainer::Reset
// ---------------------------------------------------------------------------
//
void CMRTimeContainer::Reset()
    {
    FUNC_LOG;

    delete iStartTime;
    iStartTime = NULL;

    delete iEndTime;
    iEndTime = NULL;

    delete iSeparator;
    iSeparator = NULL;

    delete iStartTimeBgContext;
    iStartTimeBgContext = NULL;

    delete iEndTimeBgContext;
    iEndTimeBgContext = NULL;

    delete iBgCtrlContext;
    iBgCtrlContext = NULL;
    }
// End of file
