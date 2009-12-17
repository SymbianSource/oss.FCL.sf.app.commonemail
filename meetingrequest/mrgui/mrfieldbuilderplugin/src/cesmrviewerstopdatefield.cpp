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
* Description:  End date field for viewers
*
*/

#include "cesmrviewerstopdatefield.h"
#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "emailtrace.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "nmrbitmapmanager.h"

#include <esmrgui.rsg>
#include <AknUtils.h>
#include <eiklabel.h>
#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <StringLoader.h>
#include <AknsConstants.h>
#include "esmrfieldbuilderdef.h"

// ======== LOCAL FUNCTIONS ========
namespace // codescanner::namespace
    {
    const TInt KComponentCount( 2 ); // icon and label
    } // unnamed namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::CESMRViewerStopDateField
// ---------------------------------------------------------------------------
//
CESMRViewerStopDateField::CESMRViewerStopDateField()
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldStopDate );
    SetFocusType( EESMRHighlightFocus );     
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::~CESMRViewerStopDateField
// ---------------------------------------------------------------------------
//
CESMRViewerStopDateField::~CESMRViewerStopDateField()
    {
    FUNC_LOG;
    delete iLabel;
    delete iIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::NewL
// ---------------------------------------------------------------------------
//
CESMRViewerStopDateField* CESMRViewerStopDateField::NewL()
    {
    FUNC_LOG;
    CESMRViewerStopDateField* self =
    new (ELeave) CESMRViewerStopDateField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRViewerStopDateField::ConstructL()
// -----------------------------------------------------------------------------
//
void CESMRViewerStopDateField::ConstructL()
    {
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapDateEnd );
    iIcon->SetParent( this );    
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStopDateField::InitializeL()
    {
    TAknLayoutText text = NMRLayoutManager::GetLayoutText( 
            Rect(), 
            NMRLayoutManager::EMRTextLayoutTextEditor );
    
    iLabel->SetFont( text.Font() );
    // This is called so that theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iLabel->FocusChanged( EDrawNow );
        }

    AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                             KRgbBlack );
}

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRViewerStopDateField::InternalizeL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry();
    TTime endTime = entry.EndTimeL().TimeLocalL();
    TTime startTime = entry.StartTimeL().TimeLocalL();
    TDateTime startDate = startTime.DateTime();
    TDateTime endDate = endTime.DateTime();

    if ( aEntry.IsAllDayEventL() )
        {
        // if the event is allday event, previous day for end date will be
        // shown in UI. See Outlook for reference...
        endTime -= TTimeIntervalDays( KAllDayEventDurationInDays );
        endDate = endTime.DateTime();
        }

    // If the entry start and end date for the event are the same, hide end time.
    // Hide only if the entry is meeting, meeting request or memo
    if( ( (aEntry.Entry().EntryTypeL() == CCalEntry::EAppt ) ||
    	  (aEntry.Entry().EntryTypeL() == CCalEntry::EEvent ) )&&
        ( startDate.Year() == endDate.Year() ) &&
        ( startDate.Month() == endDate.Month() ) &&
        ( startDate.Day() == endDate.Day() ) )
        {
        iObserver->RemoveControl( FieldId() );
        }
    else // else set the text for end time label
        {
        // Read format string from AVKON resource
        HBufC* dateFormatString =
            iEikonEnv->AllocReadResourceLC( R_QTN_DATE_USUAL_WITH_ZERO );
        HBufC* buf = HBufC::NewLC( KBufferLength );
        TPtr ptr( buf->Des() );

        endTime.FormatL( ptr, *dateFormatString );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
        iLabel->SetTextL( ptr );
        CleanupStack::PopAndDestroy( 2, dateFormatString );
        }
    }
// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerStopDateField::SizeChanged()
    {
    TRect rect = Rect();
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = rowLayoutRect.Rect();
    
    TAknWindowComponentLayout iconLayout =
        NMRLayoutManager::GetWindowComponentLayout( 
                NMRLayoutManager::EMRLayoutTextEditorIcon );
    AknLayoutUtils::LayoutImage( iIcon, rect, iconLayout );
    
    TAknLayoutRect bgLayoutRect = NMRLayoutManager::GetLayoutRect( rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
       
    TAknLayoutText labelLayout = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutTextEditor );
    iLabel->SetRect( labelLayout.TextRect() );
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerStopDateField::CountComponentControls() const
    {
    TInt count( KComponentCount );
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerStopDateField::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iIcon;
        case 1:
            return iLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStopDateField::SetOutlineFocusL( TBool aFocus )
    {
    CESMRField::SetOutlineFocusL ( aFocus );
    
    iLabel->SetFocus( aFocus );    

    if ( !aFocus )
        {
        AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                                 KRgbBlack );
        }
    }
// EOF

