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
* Description:  Recurrence date field for viewers
 *
*/


#include "cesmrviewerrecurrencedatefield.h"
#include "mesmrresponseobserver.h"
#include "mesmrlistobserver.h"
#include "esmrfieldbuilderdef.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "nmrcolormanager.h"
#include "cesmrglobalnote.h"

#include <eikenv.h>
#include <avkon.rsg>
#include <avkon.hrh>
#include <StringLoader.h>
#include <AknUtils.h>
#include <esmrgui.rsg>
#include <caltime.h>
// DEBUG
#include "emailtrace.h"
namespace // codescanner::namespace
    {
    const TInt KComponentCount( 2 );
    }
// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::CESMRViewerRecurrenceDateField
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceDateField::CESMRViewerRecurrenceDateField()
:   iRepeatUntilDate( Time::NullTTime() )
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldRecurrenceDate );
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::~CESMRViewerRecurrenceDateField
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceDateField::~CESMRViewerRecurrenceDateField()
    {
    FUNC_LOG;
    delete iRepeatTopic;
    delete iRepeatDate;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::NewL
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceDateField* CESMRViewerRecurrenceDateField::NewL()
    {
    FUNC_LOG;
    CESMRViewerRecurrenceDateField* self =
            new (ELeave) CESMRViewerRecurrenceDateField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceDateField::ConstructL()
    {
    FUNC_LOG;
    CESMRField::ConstructL( NULL );
    iRepeatTopic = CMRLabel::NewL( this );
    iRepeatDate = CMRLabel::NewL( this );

    iRepeatTopic->SetTextL( KNullDesC );
    iRepeatDate->SetTextL( KNullDesC );
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::InitializeL
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceDateField::InitializeL()
    {
    FUNC_LOG;
    NMRColorManager::SetColor( *iRepeatTopic,
                               NMRColorManager::EMRMainAreaTextColor );

    NMRColorManager::SetColor( *iRepeatDate,
                               NMRColorManager::EMRMainAreaTextColor );

    //this control cannot ever get keyboard focus
    SetFocusing(EFalse);
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::ExecuteGenericCommandL
// -----------------------------------------------------------------------------
//
TBool CESMRViewerRecurrenceDateField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;

    TBool retValue( EFalse );

    if( (aCommand == EAknCmdOpen) && IsLocked()  )
    	{
		HandleTactileFeedbackL();

    	CESMRGlobalNote::ExecuteL(
    			CESMRGlobalNote::EESMRUnableToEdit );
    	retValue = ETrue;
    	}
    else if ( EMRCmdDoEnvironmentChange == aCommand )
        {
        FormatRepeatUntilDateL();
        retValue = ETrue;
        }

    return retValue;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::SizeChanged
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceDateField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );
    // First row
    TAknLayoutRect row1LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = row1LayoutRect.Rect();

    TAknTextComponentLayout topicLayoutText =
            NMRLayoutManager::GetTextComponentLayout(
                    NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );

    AknLayoutUtils::LayoutLabel( iRepeatTopic, rect, topicLayoutText );

    rect = Rect();
    // Move the iY down the height of the topic field
    TInt movement = row1LayoutRect.Rect().Height();
    rect.Move( 0, movement );

    // Second row
    TAknLayoutRect row2LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 2 );
    rect = row2LayoutRect.Rect();

    TAknTextComponentLayout dateLayoutText =
            NMRLayoutManager::GetTextComponentLayout(
                    NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );

    AknLayoutUtils::LayoutLabel( iRepeatDate, rect, dateLayoutText );
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::MinimumSize
// -----------------------------------------------------------------------------
//
TSize CESMRViewerRecurrenceDateField::MinimumSize()
    {
    TInt height( 0 );
    // First row
    TAknLayoutRect row1LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( Rect(), 1 );
    height += row1LayoutRect.Rect().Height();
    // Second row
    TAknLayoutRect row2LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( Rect(), 2 );
    height += row2LayoutRect.Rect().Height();

    TInt width( Parent()->Size().iWidth );
    return TSize( width, height );
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::InternalizeL
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceDateField::InternalizeL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    // Get recurrence
    TESMRRecurrenceValue recValue( ERecurrenceNot );
    aEntry.GetRecurrenceL( recValue, iRepeatUntilDate );

    // Recurrence time has to be shown in the viewer as local time
    TCalTime recurrenceTime;
    recurrenceTime.SetTimeUtcL( iRepeatUntilDate );
    iRepeatUntilDate = recurrenceTime.TimeLocalL();

    if( aEntry.IsRecurrentEventL() &&
        recValue != ERecurrenceNot &&
        Time::NullTTime() != iRepeatUntilDate )
        {
        FormatRepeatUntilDateL();
        }
    else
        {
        MakeVisible( EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CESMRViewerRecurrenceDateField::CountComponentControls() const
    {
    FUNC_LOG;
    return KComponentCount;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CESMRViewerRecurrenceDateField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    CCoeControl* retVal = NULL;
    switch( aInd )
        {
        case 0:
            {
            retVal = iRepeatTopic;
            break;
            }
        case 1:
            {
            retVal = iRepeatDate;
            break;
            }
        default:;
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::FormatRepeatUntilDateL
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceDateField::FormatRepeatUntilDateL()
    {
    FUNC_LOG;

    HBufC* topicHolder =
            StringLoader::LoadLC(
                    R_QTN_MEET_REQ_REPEAT_UNTIL,
                    iCoeEnv );

    // Set text for repeat topic (e.g. "Repeat until")
    iRepeatTopic->SetTextL( *topicHolder );
    CleanupStack::PopAndDestroy( topicHolder );

    // Format date string
    HBufC* timeFormatString = iCoeEnv->AllocReadResourceLC(
    		R_QTN_DATE_USUAL_WITH_ZERO );

    TBuf< KDateStringLength > finalBuf;
    iRepeatUntilDate.FormatL( finalBuf, *timeFormatString );

    // Set repeat until date value
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( finalBuf );
    iRepeatDate->SetTextL( finalBuf );
    CleanupStack::PopAndDestroy( timeFormatString );
    timeFormatString = NULL;
    }

// End of file


