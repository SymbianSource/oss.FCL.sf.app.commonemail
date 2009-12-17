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
* Description:  ESMR repeat until field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrrepeatuntil.h"

#include <AknUtils.h>
#include <eikmfne.h>
#include <avkon.hrh>
#include <eiklabel.h>
#include <StringLoader.h>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <esmrgui.rsg>

#include "cesmrborderlayer.h"
#include "mesmrfieldvalidator.h"
#include "cesmrglobalnote.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrlayoutmgr.h"

using namespace ESMRLayout;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::CESMRRepeatUntilField
// ---------------------------------------------------------------------------
//
CESMRRepeatUntilField::CESMRRepeatUntilField(
        MESMRFieldValidator* aValidator )
:   iValidator(aValidator)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::~CESMRRepeatUntilField
// ---------------------------------------------------------------------------
//
CESMRRepeatUntilField::~CESMRRepeatUntilField( )
    {
    FUNC_LOG;
    delete iLabel;
    delete iFrameBgContext;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::NewL
// ---------------------------------------------------------------------------
//
CESMRRepeatUntilField* CESMRRepeatUntilField::NewL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRRepeatUntilField* self =
            new (ELeave) CESMRRepeatUntilField( aValidator );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::ConstructL( )
    {
    FUNC_LOG;
    HBufC* label = StringLoader::LoadLC( R_QTN_MEET_REQ_REPEAT_UNTIL );
    SetFieldId ( EESMRFieldRecurrenceDate );
    iLabel = new (ELeave) CEikLabel();
    iLabel->SetTextL ( *label );
    CleanupStack::PopAndDestroy( label );

    TTime startTime;
    startTime.UniversalTime ( );

    iDate = new (ELeave) CEikDateEditor;
    iDate->ConstructL(
            KAknMinimumDate,
            TTIME_MAXIMUMDATE,
            startTime,
            EFalse );
    iDate->SetUpAndDownKeysConsumed ( EFalse );

    iBackground = AknsDrawUtils::ControlContext( this );
    CESMRField::ConstructL ( iDate );

    if ( iValidator )
        {
        iValidator->SetRecurrenceUntilDateFieldL( *iDate );
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::InitializeL()
    {
    FUNC_LOG;
    iLabel->SetFont( iLayout->Font( iCoeEnv, iFieldId ) );
    iLabel->SetLabelAlignment( CESMRLayoutManager::IsMirrored ( )
                               ? ELayoutAlignRight : ELayoutAlignLeft );
    AknLayoutUtils::OverrideControlColorL(
            *iLabel,
            EColorLabelText,
            iLayout->GeneralListAreaTextColor() );

    iDate->SetFont( iLayout->Font( iCoeEnv, iFieldId ) );
    iDate->SetMfneAlignment( CESMRLayoutManager::IsMirrored ( )
                             ? EAknEditorAlignRight : EAknEditorAlignLeft );
    iDate->SetSkinTextColorL( iLayout->NormalTextColorID() );
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRRepeatUntilField::OkToLoseFocusL( TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    TRAPD(err, iValidator->RecurrenceEndDateChangedL() );

    if ( KErrNone != err )
        {
        CESMRGlobalNote::ExecuteL(
                CESMRGlobalNote::EESMRRepeatEndEarlierThanItStart );
        }
    return (KErrNone == err);
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRRepeatUntilField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = CESMRField::CountComponentControls();

    if ( iLabel )
        {
        ++count;
        }

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRRepeatUntilField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    if ( aInd == 0 )
        {
        return iLabel;
        }

    return CESMRField::ComponentControl ( aInd );
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::SizeChanged( )
    {
    FUNC_LOG;
    TRect rect = Rect();
    TPoint titlePos;
    TSize titleSize;

    if ( CESMRLayoutManager::IsMirrored ( ) )
        {
        titleSize = TSize( rect.Width()- ( KIconSize.iWidth + KIconBorderMargin ),
                iLayout->FieldSize( EESMRFieldAlarmDate ).iHeight );

        TPoint titlePos( rect.iBr.iX - KIconSize.iWidth - titleSize.iWidth,
                         rect.iTl.iY);
        iLabel->SetExtent ( titlePos, titleSize );

        TSize dateSize( rect.Width()- ( KIconSize.iWidth + KIconBorderMargin ),
                rect.Height() - titleSize.iHeight );
        TPoint datePos(rect.iBr.iX - KIconSize.iWidth - dateSize.iWidth,
                rect.iTl.iY + titleSize.iHeight);
        iBorder->SetExtent ( datePos, dateSize );
        }
    else
        {
        // title
        titlePos = TPoint( rect.iTl.iX + KIconSize.iWidth + KIconBorderMargin,
                           rect.iTl.iY);
        titleSize = TSize (
                    rect.Width() - KIconSize.iWidth - KIconBorderMargin,
                    iLayout->FieldSize( EESMRFieldAlarmDate ).iHeight );

        iLabel->SetExtent ( titlePos, titleSize );

        // Current implemenattion does not need the ID here
        CESMRLayoutManager::TMarginsId nullID = CESMRLayoutManager::EFieldMargins;
        TInt topMargin = iLayout->Margins( nullID ).iTop;

        TPoint datePosition = TPoint (
                   rect.iTl.iX + KIconSize.iWidth + KIconBorderMargin,
                   rect.iTl.iY + iLabel->Size().iHeight + topMargin );

        TSize dateSize = TSize (
                    rect.Width() - KIconSize.iWidth - KIconBorderMargin,
                    iLayout->FieldSize( EESMRFieldAlarmDate ).iHeight );

        iBorder->SetPosition( datePosition );
        iBorder->SetSize( dateSize );

        TInt h = iLabel->Size().iHeight + iDate->Size().iHeight;

        if( iFrameBgContext )
            {
            iFrameBgContext->SetFrameRects( iDate->Rect(), iDate->Rect() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    TRect rect(TPoint(iDate->Position()), iDate->Size());
    TRect inner(rect);
    TRect outer(rect);

    iDate->SetSize( TSize( iDate->Size().iWidth,
                    iLayout->FieldSize( FieldId() ).iHeight ));

    iDate->SetBorder( TGulBorder::ENone );

    delete iFrameBgContext;
    iFrameBgContext = NULL;
    iFrameBgContext = CAknsFrameBackgroundControlContext::NewL(
                                KAknsIIDQsnFrInput, outer, inner, EFalse ) ;

    iFrameBgContext->SetParentContext( iBackground );
    iDate->SetSkinBackgroundControlContextL(iFrameBgContext);
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::PositionChanged
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::PositionChanged()
    {
    FUNC_LOG;
    CCoeControl::PositionChanged();
    if( iFrameBgContext )
        {
        iFrameBgContext->SetFrameRects( iDate->Rect(), iDate->Rect() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRRepeatUntilField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);

    if ( aType == EEventKey )
        {
        TInt fieldIndex( iDate->CurrentField() );
        response = CESMRField::OfferKeyEventL( aEvent, aType );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::CheckIfValidatingNeededL(
        TInt aStartFieldIndex )
    {
    FUNC_LOG;
    TInt fieldIndex( iDate->CurrentField() );

    if ( fieldIndex != aStartFieldIndex && iValidator )
        {
        TRAPD(err, iValidator->RecurrenceEndDateChangedL() );

        if ( KErrNone != err )
            {
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRRepeatEndEarlierThanItStart );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// EOF

