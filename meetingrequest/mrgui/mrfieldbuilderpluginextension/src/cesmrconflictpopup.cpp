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
* Description:  ESMR Conflict popup implementation
*
*/
#include "cesmrconflictpopup.h"
#include "mesmrcalentry.h"
#include "mesmrmeetingrequestentry.h"

#include <AknInfoPopupNoteController.h>
#include <StringLoader.h>
#include <eikenv.h>
#include <esmrgui.rsg>
#include <avkon.rsg>
#include <ct/rcpointerarray.h>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>

#include "emailtrace.h"
#include "cesmrcaluserutil.h"

// Unnamed namespace for local definitions
namespace{

const TInt KTimeDelayBeforeShow(0);  // 0 Seconds
const TInt KTimeForView(5000);          // 5 Seconds
const TInt KTimeStringLength(20);
const TInt KArrayGranularity( 4 );

const TInt KPortraitWindowVariety( 4 );
const TInt KLandscapeWindowVariety( 7 );
const TInt KTextLineVariety( 5 );

const TInt KNumTitleLines( 1 );
const TInt KNumLocationLines( 1 );

/**
 * Clips given text and allocates new buffer if needed.
 * @param aSource text to clip
 * @param aNumLines number of popup note lines available for visual text.
 * @return new buffer or NULL if aSource must not be clipped
 */

HBufC* ClipTextL( const TDesC& aSource, TInt aNumLines )
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
                AknLayoutUtils::EMainPane,
                mainPane );

    // Get the correct popup window text line layout
    TInt windowVariety( KPortraitWindowVariety );
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        windowVariety = KLandscapeWindowVariety;
        }

    TAknWindowComponentLayout popupNoteWindow =
            AknLayoutScalable_Avkon::popup_preview_text_window( windowVariety );
    TAknLayoutRect popupNoteRect;
    popupNoteRect.LayoutRect( mainPane, popupNoteWindow );

    TAknTextComponentLayout popupNoteTextLine =
            AknLayoutScalable_Avkon::popup_preview_text_window_t3( KTextLineVariety );
    TAknLayoutText textLayout;
    textLayout.LayoutText( popupNoteRect.Rect(), popupNoteTextLine );

    TInt lineWidth( textLayout.TextRect().Width() );
    const CFont* font = textLayout.Font();

    HBufC* text = NULL;

    if ( font->TextWidthInPixels( aSource ) > lineWidth )
        {
        // Text must be clipped to fit
        // Allocate result buffer and reserve space for line feeds
        text = HBufC::NewLC(
                aSource.Length() + aNumLines );

        CArrayFix<TInt>* lineWidthArray =
                new( ELeave ) CArrayFixFlat<TInt>( aNumLines );
        CleanupStack::PushL( lineWidthArray );
        for ( TInt i = 0; i < aNumLines; ++i )
            {
            lineWidthArray->AppendL( lineWidth );
            }

        CPtrC16Array* strings = new( ELeave ) CPtrC16Array( aNumLines );
        CleanupStack::PushL( strings );
        TPtr des( text->Des() );

        // Wrap string
        AknTextUtils::WrapToStringAndClipL(
                aSource,
                *lineWidthArray,
                *font,
                des );

        // Trim trailing line feeds
        des.TrimRight();

        CleanupStack::PopAndDestroy( 2, lineWidthArray );
        CleanupStack::Pop( text );
        }

    return text;
    }

/**
 * Judge is one CCalEntry is a all day event
 * @param aEntry the entry be checked
 * @return ETure if it is a all day event
 */
TBool IsAllDayEventL( CCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRCalUserUtil* entryUtil = CESMRCalUserUtil::NewLC( aEntry );
    TBool allDayEvent( entryUtil->IsAlldayEventL() );
    CleanupStack::PopAndDestroy( entryUtil );
    return allDayEvent;
    }
}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRConflictPopup::NewL
// ---------------------------------------------------------------------------
//
CESMRConflictPopup* CESMRConflictPopup::NewL( MESMRCalEntry& aEntry)
    {
    FUNC_LOG;
    CESMRConflictPopup* self = new (ELeave) CESMRConflictPopup();
    CleanupStack::PushL( self );
    self->ConstructL(aEntry);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRConflictPopup::CESMRConflictPopup
// ---------------------------------------------------------------------------
//
CESMRConflictPopup::CESMRConflictPopup()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRConflictPopup::~CESMRConflictPopup
// ---------------------------------------------------------------------------
//
CESMRConflictPopup::~CESMRConflictPopup()
    {
    FUNC_LOG;
    delete iNote;
    }

// ---------------------------------------------------------------------------
// CESMRConflictPopup::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRConflictPopup::ConstructL(MESMRCalEntry& aEntry)
    {
    FUNC_LOG;
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == aEntry.Type() )
        {
        iEntry = static_cast<MESMRMeetingRequestEntry*>(&aEntry);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    iNote = CAknInfoPopupNoteController::NewL();

    InitializeL();

    }

// ---------------------------------------------------------------------------
// CESMRConflictPopup::PrepareDisplayStringL
// ---------------------------------------------------------------------------
//
void CESMRConflictPopup::PrepareDisplayStringL()
    {
    FUNC_LOG;
    RCPointerArray<CCalEntry> entryArray;
    CleanupClosePushL( entryArray );

    TInt ret = iEntry->FetchConflictingEntriesL( entryArray );
    if( ret == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    // Record the blank rows.
    TInt blankRow = 0;
    // Pointer descriptor for R_QTN_MEET_REQ_CONFLICT_UNNAMED
    TPtrC unnamedTitle( KNullDesC );

    // Actual title to shown in popup
    TPtrC meetingTitle( KNullDesC );

    // Location to show in popup
    TPtrC meetingLocation( KNullDesC );

    TTime startTime;
    TTime endTime;

    CCoeEnv* env = CCoeEnv::Static();

    if ( entryArray.Count() > 0 )
        {// there is atleast one entry
        // get the first conflicting entry and set the string for display
        meetingTitle.Set( entryArray[ 0 ]->SummaryL() );
        meetingLocation.Set( entryArray[ 0 ]->LocationL() );

        if ( ( meetingLocation.Length() == 0 ) && ( meetingTitle.Length() == 0 ) )
        	{
            // if no title, set unnamed text:
            HBufC* title = StringLoader::LoadLC(
                    R_QTN_MEET_REQ_CONFLICT_UNNAMED,
                    env );
            unnamedTitle.Set( *title );
            meetingTitle.Set( unnamedTitle );
            }

        startTime = entryArray[ 0 ]->StartTimeL().TimeLocalL();
        endTime = entryArray[ 0 ]->EndTimeL().TimeLocalL();
        
        //If the first entry is a all day event, it should be show as 
        //12:00Am - 11:59Pm, so we should subtract one min from end time.
        if ( IsAllDayEventL( *entryArray[ 0 ] ) )
            {
            endTime	-= TTimeIntervalMinutes( 1 );
            }
        }

    HBufC*  conflictLabel = StringLoader::LoadLC(
            R_QTN_MEET_REQ_CONFLICTS_WITH_LABEL,
            env );

    HBufC* timeFormatString = StringLoader::LoadLC(
            R_QTN_TIME_USUAL_WITH_ZERO,
            env );

    TBuf<KTimeStringLength> startBuf;
    TBuf<KTimeStringLength> endBuf;

    startTime.FormatL( startBuf, *timeFormatString );
    endTime.FormatL( endBuf, *timeFormatString );

    CPtrC16Array* strings = new( ELeave ) CPtrC16Array( KArrayGranularity );
    CleanupStack::PushL( strings );
    strings->AppendL( startBuf ); //start time string
    strings->AppendL( endBuf ); //end time string

    HBufC* finalTimeBuf =
        StringLoader::LoadLC(
                R_QTN_MEET_REQ_TIME_SEPARATOR,
                *strings,
                env );

    if ( meetingLocation.Length() > 0 )
        {
        // format Meeting location
        HBufC* formatText = StringLoader::LoadLC(
                R_MEET_REQ_CONFLICT_LOCATION_FORMAT,
                meetingLocation,
                env );
        meetingLocation.Set( *formatText );
        }

    // Format final display string
    strings->Reset();
    strings->AppendL( *conflictLabel );
    strings->AppendL( *finalTimeBuf );

    HBufC* titleBuf = NULL;
    HBufC* locationBuf = NULL;

    if ( meetingTitle.Length() > 0 )
        {
        // Prepare title
        titleBuf = ClipTextL( meetingTitle, KNumTitleLines );
        if ( titleBuf )
            {
            CleanupStack::PushL( titleBuf );
            meetingTitle.Set( *titleBuf );
            }
        strings->AppendL( meetingTitle );

        // Prepare location
        locationBuf = ClipTextL( meetingLocation, KNumLocationLines );
        if ( locationBuf )
            {
            CleanupStack::PushL( locationBuf );
            meetingLocation.Set( *locationBuf );
            }
        if ( meetingLocation.Length() > 0 )
        	{
            strings->AppendL( meetingLocation );
        	}
        else
        	{
            blankRow++ ;
        	}
    
        }
    else
        {
        blankRow++;
        // Prepare location
        locationBuf = ClipTextL(
                meetingLocation, KNumLocationLines );
        if ( locationBuf )
            {
            CleanupStack::PushL( locationBuf );
            meetingLocation.Set( *locationBuf );
            }
        if ( meetingLocation.Length() > 0 )
        	{
            strings->AppendL( meetingLocation );
        	}
        else
        	{
            blankRow++;
        	}
        }
    HBufC* otherconflictBuf = NULL ;
    if( entryArray.Count() == 2)
    	{
         otherconflictBuf = StringLoader::LoadLC(
        		 R_MEET_REQ_CONFLICT_OTHERCONFLICT_SINGULAR_FORMAT,
        		 entryArray.Count()-1,
                    env );
         strings->AppendL( *otherconflictBuf );
    	}
    else if ( entryArray.Count() > 2 )
    	{
         otherconflictBuf = StringLoader::LoadLC(
    		R_MEET_REQ_CONFLICT_OTHERCONFLICT_FORMAT,
    		entryArray.Count()-1,
    		env );
         strings->AppendL( *otherconflictBuf );
    	}
    else
    	{
        blankRow++;
    	}
    
    while( blankRow )
    	{
        strings->AppendL( KNullDesC() );
        blankRow-- ;
    	}
    HBufC* displayString = StringLoader::LoadLC(
            R_MEET_REQ_CONFLICT_TEXT_FORMAT,
            *strings,
            env );

    // Trim trailing white space
    displayString->Des().TrimRight();

    // Set display string to popup note
    iNote->SetTextL( *displayString );

    // Clean allocated buffers
    CleanupStack::PopAndDestroy( displayString );
    if ( otherconflictBuf )
    	{
        CleanupStack::PopAndDestroy( otherconflictBuf );
    	}
    if ( locationBuf )
        {
        CleanupStack::PopAndDestroy( locationBuf );
        }
    if ( titleBuf )
        {
        CleanupStack::PopAndDestroy( titleBuf );
        }
    if ( meetingLocation.Length() > 0 )
        {
        CleanupStack::PopAndDestroy(); // R_MEET_REQ_CONFLICT_LOCATION_FORMAT
        }
    CleanupStack::PopAndDestroy( finalTimeBuf );
    CleanupStack::PopAndDestroy( strings );
    CleanupStack::PopAndDestroy( timeFormatString );
    CleanupStack::PopAndDestroy( conflictLabel );
    if ( unnamedTitle.Length() > 0 )
        {
        CleanupStack::PopAndDestroy(); // R_QTN_MEET_REQ_CONFLICT_UNNAMED
        }
    CleanupStack::PopAndDestroy( &entryArray );
    }


// ---------------------------------------------------------------------------
// CESMRConflictPopup::ShowPopup
// ---------------------------------------------------------------------------
//
void CESMRConflictPopup::ShowPopup()
    {
    FUNC_LOG;
        TRAPD( error, PrepareDisplayStringL());
        if ( error != KErrNone )
            {
            CEikonEnv::Static()-> // codescanner::eikonenvstatic
                HandleError(error);
            }
        iNote->ShowInfoPopupNote();
    }


// ---------------------------------------------------------------------------
// CESMRConflictPopup::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRConflictPopup::InitializeL()
    {
    FUNC_LOG;
    iNote->SetTimeDelayBeforeShow(KTimeDelayBeforeShow); // 2 Seconds
    iNote->SetTimePopupInView(KTimeForView); // 5 Seconds
    }

// EOF

