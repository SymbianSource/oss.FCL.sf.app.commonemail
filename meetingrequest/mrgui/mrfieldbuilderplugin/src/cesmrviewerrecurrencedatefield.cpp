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
* Description:  Recurrence date field for viewers
 *
*/

#include "cesmrviewerrecurrencedatefield.h"
#include "mesmrresponseobserver.h"
#include "cesmrborderlayer.h"
#include "mesmrlistobserver.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrlayoutmgr.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"

#include <eikenv.h>
#include <avkon.rsg>
#include <avkon.hrh>
#include <StringLoader.h>
#include <AknUtils.h>
#include <esmrgui.rsg>
#include <caltime.h>
// DEBUG
#include "emailtrace.h"

using namespace ESMRLayout;

namespace // codescanner::namespace
    {
    const TInt KComponentCount( 2 );
    const TInt KMargin (5);
    }
// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::NewL
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceDateField* CESMRViewerRecurrenceDateField::NewL()
    {
    FUNC_LOG;
    CESMRViewerRecurrenceDateField* self = new (ELeave) CESMRViewerRecurrenceDateField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
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
// CESMRViewerRecurrenceDateField::CESMRViewerRecurrenceDateField
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceDateField::CESMRViewerRecurrenceDateField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldRecurrenceDate );
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceDateField::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceDateField::ConstructL()
    {
    FUNC_LOG;
    iRepeatTopic = CMRLabel::NewL();
    iRepeatDate = CMRLabel::NewL();

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
    TAknLayoutText editorRect = 
    NMRLayoutManager::GetLayoutText( 
            Rect(), NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    
    iRepeatTopic->SetFont( editorRect.Font() );
    iRepeatDate->SetFont( editorRect.Font() );

    AknLayoutUtils::OverrideControlColorL( *iRepeatTopic, EColorLabelText,
                                            KRgbBlack );
    AknLayoutUtils::OverrideControlColorL( *iRepeatDate, EColorLabelText,
                                            KRgbBlack );
    //this control cannot ever get keyboard focus
    SetFocusing(EFalse);
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
    
    TAknLayoutText topicRect = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutMultiRowTextEditor ); 
    
    TRect rectWithMargin = topicRect.TextRect();
    rectWithMargin.iTl.iX += KMargin;
    iRepeatTopic->SetRect( rectWithMargin );
    
    rect = Rect();
    // Move the iY down the height of the topic field
    TInt movement = row1LayoutRect.Rect().Height();
    rect.Move( 0, movement );    
    
    // Second row
    TAknLayoutRect row2LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 2 );
    rect = row2LayoutRect.Rect();    
    
    TAknLayoutText dateRect = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );    
    
    rectWithMargin = dateRect.TextRect();
    rectWithMargin.iTl.iX += KMargin;
    iRepeatDate->SetRect( rectWithMargin );
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
    TTime recTime( Time::NullTTime() );
    aEntry.GetRecurrenceL( recValue, recTime );

    // Recurrence time has to be shown in the viewer as local time
    TCalTime recurrenceTime;
    recurrenceTime.SetTimeUtcL( recTime );
    TTime localRecurrenceTime;
    localRecurrenceTime = recurrenceTime.TimeLocalL();

    if( aEntry.IsRecurrentEventL() &&
        recValue != ERecurrenceNot &&
        Time::NullTTime() != recTime )
        {
        HBufC* topicHolder =
                StringLoader::LoadLC(
                        R_QTN_MEET_REQ_REPEAT_UNTIL,
                        iEikonEnv );
        // Set text for repeat topic (e.g. "Repeat until")
        iRepeatTopic->SetTextL( topicHolder->Des() );
        CleanupStack::PopAndDestroy( topicHolder );

        // Format date string
        HBufC* timeFormatString = iEikonEnv->AllocReadResourceLC(
                                        R_QTN_DATE_USUAL);

        TBuf< KDateStringLength > finalBuf;
        localRecurrenceTime.FormatL( finalBuf, *timeFormatString );

        // Set repeat until date value
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( finalBuf );
        iRepeatDate->SetTextL( finalBuf );
        CleanupStack::PopAndDestroy( timeFormatString );
        timeFormatString = NULL;
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

// End of file


