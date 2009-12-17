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
* Description:  ESMR time (start-end) field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrtimefield.h"

#include <eikmfne.h>
#include <avkon.hrh>
#include <eiklabel.h>
#include <AknUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>

#include "mesmrfieldvalidator.h"
#include "cesmrglobalnote.h"
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "esmrfieldbuilderdef.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTimeField::CESMRTimeField
// ---------------------------------------------------------------------------
//
CESMRTimeField::CESMRTimeField( )
    {
    FUNC_LOG;
    // do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::~CESMRTimeField
// ---------------------------------------------------------------------------
//
CESMRTimeField::~CESMRTimeField( )
    {
    FUNC_LOG;
    // iContainer is deleted by framework
    // do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::NewL
// ---------------------------------------------------------------------------
//
CESMRTimeField* CESMRTimeField::NewL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRTimeField* self = new (ELeave) CESMRTimeField;
    CleanupStack::PushL ( self );
    self->ConstructL ( aValidator );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::ConstructL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldMeetingTime );
    iContainer =
            CTimeContainer::NewL(
                    aValidator,
                    this,
                    EESMRFieldMeetingTime );

    CESMRIconField::ConstructL(
            KAknsIIDQgnMeetReqIndiClock,
            iContainer );
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::SetContainerWindowL(
        const CCoeControl& aControl )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL ( aControl );
    if ( iContainer )
        {
        iContainer->SetContainerWindowL ( aControl );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRTimeField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( iContainer )
        {
        response = iContainer->OfferKeyEventL ( aEvent, aType );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::InitializeL()
    {
    FUNC_LOG;
    if ( iContainer )
        {
        iContainer->SetFontL( iLayout->Font ( iCoeEnv, iFieldId ), iLayout );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRTimeField::OkToLoseFocusL(
        TESMREntryFieldId aNext )
    {
    FUNC_LOG;
    if ( iContainer )
        {
        return iContainer->OkToLoseFocusL ( FieldId ( ), aNext );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// ---------------------------------------------------------------------------
// CTimeContainer::CTimeContainer
// ---------------------------------------------------------------------------
//
CTimeContainer::CTimeContainer(
        MESMRFieldValidator* aValidator,
        TESMREntryFieldId aFieldId  ) :
    iValidator(aValidator),
    iFieldId( aFieldId )
    {
    FUNC_LOG;
    // do nothing
    }

// ---------------------------------------------------------------------------
// CTimeContainer::~CTimeContainer
// ---------------------------------------------------------------------------
//
CTimeContainer::~CTimeContainer( )
    {
    FUNC_LOG;
    delete iStartTime;
    delete iEndTime;
    delete iSeparator;
    delete iStartTimeBgContext;
    delete iEndTimeBgContext;
    }

// ---------------------------------------------------------------------------
// CTimeContainer::NewL
// ---------------------------------------------------------------------------
//
CTimeContainer* CTimeContainer::NewL(
        MESMRFieldValidator* aValidator,
        CCoeControl* aParent,
        TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    CTimeContainer* self =
            new (ELeave) CTimeContainer( aValidator, aFieldId );
    CleanupStack::PushL ( self );
    self->ConstructL ( aParent );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CTimeContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CTimeContainer::ConstructL(
        CCoeControl* /*aParent*/)
    {
    FUNC_LOG;
    TTime startTime;
    startTime.UniversalTime ( );

    iStartTime = new (ELeave) CEikTimeEditor;
    iEndTime = new (ELeave) CEikTimeEditor;
    iSeparator = new (ELeave) CEikLabel;

    iStartTime->ConstructL ( TTIME_TIMEFIELDMINIMUMDATE, 
                             TTIME_TIMEFIELDMAXIMUMDATE,
                             startTime, 
                             EEikTimeWithoutSecondsField );
    
    iEndTime->ConstructL ( TTIME_TIMEFIELDMINIMUMDATE, 
                           TTIME_TIMEFIELDMAXIMUMDATE,
                           startTime, 
                           EEikTimeWithoutSecondsField );

    iBackground = AknsDrawUtils::ControlContext( this );

    TInt align = CESMRLayoutManager::IsMirrored ( ) ? EAknEditorAlignRight
            : EAknEditorAlignLeft;
    iStartTime->SetMfneAlignment ( align );
    iEndTime->SetMfneAlignment ( align );

    iSeparator->SetTextL ( KTimeFieldSeparator );

    if ( iValidator )
        {
        iValidator->SetStartTimeFieldL( *iStartTime );
        iValidator->SetEndTimeFieldL( *iEndTime );
        }
    }

// ---------------------------------------------------------------------------
// CTimeContainer::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CTimeContainer::SetContainerWindowL(
        const CCoeControl& aControl )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL ( aControl );
    iStartTime->SetContainerWindowL ( aControl );
    iEndTime->SetContainerWindowL ( aControl );
    }

// ---------------------------------------------------------------------------
// CTimeContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CTimeContainer::CountComponentControls() const
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
// CTimeContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CTimeContainer::ComponentControl(
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
// CTimeContainer::PositionChanged()
// ---------------------------------------------------------------------------
//
void CTimeContainer::PositionChanged()
    {
    FUNC_LOG;
    CCoeControl::PositionChanged();
    if( iStartTimeBgContext )
        {
        iStartTimeBgContext->SetFrameRects(
                iStartTime->Rect(),
                iStartTime->Rect() );
        }

    if( iEndTimeBgContext )
        {
        iEndTimeBgContext->SetFrameRects(
                iEndTime->Rect(),
                iEndTime->Rect() );
        }
    }

// ---------------------------------------------------------------------------
// CTimeContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CTimeContainer::SizeChanged()
    {
    FUNC_LOG;
    TInt x = CESMRLayoutManager::IsMirrored ( ) ? Rect().iBr.iX : Rect().iTl.iX;
    TPoint p( x, Rect().iTl.iY);

    TSize editorSize;
    TSize separatorSize;

    editorSize = iStartTime->MinimumSize();
    editorSize.iHeight = Rect().Height();

    separatorSize = iSeparator->MinimumSize();
    separatorSize.iHeight = Rect().Height();

    iStartTime->SetBorder( TGulBorder::ENone );
    iEndTime->SetBorder( TGulBorder::ENone );

    if ( CESMRLayoutManager::IsMirrored ( ) )
        {
        p.iX -= editorSize.iWidth;
        iStartTime->SetExtent ( p, editorSize );

        p.iX -= separatorSize.iWidth;
        iSeparator->SetExtent ( p, separatorSize );

        p.iX -= editorSize.iWidth;
        iEndTime->SetExtent ( p, editorSize );
        }
    else
        {
        iStartTime->SetExtent ( p, editorSize );
        p.iX += editorSize.iWidth;

        iSeparator->SetExtent ( p, separatorSize );
        p.iX += separatorSize.iWidth;

        iEndTime->SetExtent ( p, editorSize );
        }
    }

// ---------------------------------------------------------------------------
// CTimeContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CTimeContainer::OfferKeyEventL(
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
// CTimeContainer::FocusChanged
// ---------------------------------------------------------------------------
//
void CTimeContainer::FocusChanged(
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
// CTimeContainer::SetFont
// ---------------------------------------------------------------------------
//
void CTimeContainer::SetFontL(
        const CFont* aFont,
        CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    iLayout = aLayout;

    iStartTime->SetFont ( aFont );
    iSeparator->SetFont ( aFont );
    iEndTime->SetFont ( aFont );

    iStartTime->SetSkinTextColorL( aLayout->NormalTextColorID() );
    iEndTime->SetSkinTextColorL( aLayout->NormalTextColorID() );

    AknLayoutUtils::OverrideControlColorL ( *iSeparator, EColorLabelText,
                                             aLayout->NormalTextColor() );
    }

// ---------------------------------------------------------------------------
// CTimeContainer::FocusedEditor
// ---------------------------------------------------------------------------
//
CEikTimeEditor* CTimeContainer::FocusedEditor( )
    {
    FUNC_LOG;
    return iStartTime->IsFocused ( ) ? iStartTime : iEndTime;
    }

// ---------------------------------------------------------------------------
// CTimeContainer::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CTimeContainer::OkToLoseFocusL(
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
// CTimeContainer::ActivateL
// ---------------------------------------------------------------------------
//
void CTimeContainer::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    TRect rect(TPoint(iStartTime->Position()), iStartTime->Size());
    TRect inner(rect);
    TRect outer(rect);

    delete iStartTimeBgContext;
    delete iEndTimeBgContext;
    iStartTimeBgContext = NULL;
    iEndTimeBgContext = NULL;

    iStartTimeBgContext =
            CAknsFrameBackgroundControlContext::NewL(
                    KAknsIIDQsnFrInput,
                    outer,
                    inner,
                    EFalse ) ;

    rect.SetRect( iEndTime->Position(), iEndTime->Size());
    inner = rect;
    iEndTimeBgContext =
            CAknsFrameBackgroundControlContext::NewL(
                    KAknsIIDQsnFrInput,
                    outer,
                    inner,
                    EFalse ) ;

    iStartTimeBgContext->SetParentContext( iBackground );
    iEndTimeBgContext->SetParentContext( iBackground );
    iStartTime->SetSkinBackgroundControlContextL(iStartTimeBgContext);
    iEndTime->SetSkinBackgroundControlContextL(iEndTimeBgContext);
    }

// ---------------------------------------------------------------------------
// CTimeContainer::ChangeFocusRightL
// ---------------------------------------------------------------------------
//
TKeyResponse CTimeContainer::ChangeFocusRightL(const TKeyEvent& aEvent,
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
// CTimeContainer::ChangeFocusLeftL
// ---------------------------------------------------------------------------
//
TKeyResponse CTimeContainer::ChangeFocusLeftL(
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
// CTimeContainer::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CTimeContainer::CheckIfValidatingNeededL(
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
    }

// ---------------------------------------------------------------------------
// CTimeContainer::TriggerStartTimeChangedL
// ---------------------------------------------------------------------------
//
void CTimeContainer::TriggerStartTimeChangedL()
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
// CTimeContainer::TriggerEndTimeChangedL
// ---------------------------------------------------------------------------
//
void CTimeContainer::TriggerEndTimeChangedL()
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

// EOF

