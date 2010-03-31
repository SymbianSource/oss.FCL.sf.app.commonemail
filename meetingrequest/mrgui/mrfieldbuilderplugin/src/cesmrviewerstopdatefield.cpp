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
* Description:  End date field for viewers
*
*/

#include "cesmrviewerstopdatefield.h"
#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "nmrbitmapmanager.h"
#include "emailtrace.h"
#include "cesmrglobalnote.h"

#include <esmrgui.rsg>
#include <aknutils.h>
#include <eiklabel.h>
#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <stringloader.h>
#include <aknsconstants.h>
#include "esmrfieldbuilderdef.h"

// ======== LOCAL FUNCTIONS ========
namespace
    { // codescanner::namespace

const TInt KComponentCount( 2 ); // icon and label
const TInt KMaxTimeBuffer( 32 ); // buffer for date formatting    

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
    delete iIcon;
    delete iLockIcon;
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
    CESMRField::ConstructL( iLabel ); // ownership transfered

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
        iObserver->HideControl( FieldId() );
        }
    else // else set the text for end time label
        {
        // Read format string from AVKON resource
        iStopTime = endTime;
        FormatDateStringL();
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

    // Layouting lock icon
    if( iLockIcon )
    	{
    	TAknWindowComponentLayout iconLayout( 
    			NMRLayoutManager::GetWindowComponentLayout( 
    					NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
    	AknLayoutUtils::LayoutImage( iLockIcon, rect, iconLayout );
    	}
        
    // Layouting label
    TAknLayoutText viewerLayoutText;
    if( iLockIcon )
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rect, 
    			NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
    	}
    else
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rect, 
    			NMRLayoutManager::EMRTextLayoutTextEditor );
    	}

    TRect viewerRect( viewerLayoutText.TextRect() );    
    iLabel->SetRect( viewerRect );

    // Move focus rect so that it's relative to field's position.
    viewerRect.Move( -Position() );
    SetFocusRect( viewerRect );

    // Setting font also for the label
    iLabel->SetFont( viewerLayoutText.Font() );
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerStopDateField::CountComponentControls() const
    {
    TInt count( KComponentCount );
    if( iLockIcon )
    	{
    	++count;
    	}
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
        case 2:
        	return iLockIcon;
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
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerStopDateField::ExecuteGenericCommandL( TInt aCommand )
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
    
    if ( EMRCmdDoEnvironmentChange == aCommand )
        {
        FormatDateStringL();
        retValue = ETrue;
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStopDateField::LockL()
	{
	FUNC_LOG;
	if( IsLocked() )
		{
		return;
		}
	
	CESMRField::LockL();
	
	delete iLockIcon;
	iLockIcon = NULL;
	iLockIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapLockField, ETrue );
	iLockIcon->SetParent( this );
	}

// ---------------------------------------------------------------------------
// CESMRViewerStopDateField::FormatDateStringL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStopDateField::FormatDateStringL()
    {
    FUNC_LOG;
    
    // Read format string from AVKON resource
    HBufC* dateFormatString = 
            iEikonEnv->AllocReadResourceLC(
                    R_QTN_DATE_USUAL_WITH_ZERO);
    
    TBuf<KMaxTimeBuffer> buf;
    iStopTime.FormatL(buf, *dateFormatString);

    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( buf );
    iLabel->SetTextL( buf );
    CleanupStack::PopAndDestroy( dateFormatString );    
    }

// EOF
