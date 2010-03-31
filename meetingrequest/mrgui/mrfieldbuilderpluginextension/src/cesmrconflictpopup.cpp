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

#include <akninfopopupnotecontroller.h>
#include <stringloader.h>
#include <eikenv.h>
#include <esmrgui.rsg>
#include <avkon.rsg>

#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace{

const TInt KTimeDelayBeforeShow(0);  // 0 Seconds
const TInt KTimeForView(5000);          // 5 Seconds
const TInt KZero(0);
const TInt KTimeStringLength(20);
const TInt KMaxSummaryLength(25);

_LIT( KNewLine, "\n");
_LIT( KMessageStart, "(");
_LIT( KMessageEnd, ")");
_LIT( KStringCont, "...\n");
_LIT (KSpace , " " );
const TInt KFullTimeStringSize(256);

void CalEntryPointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CCalEntry>* entryArray =
        static_cast<RPointerArray<CCalEntry>*>( aArray );

    entryArray->ResetAndDestroy();
    entryArray->Close();
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
    RPointerArray<CCalEntry> entryArray;
    CleanupStack::PushL(
            TCleanupItem(
                CalEntryPointerArrayCleanup,
                &entryArray ) );
    
    TInt ret = iEntry->FetchConflictingEntriesL( entryArray );
    if( ret == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }

    TInt numEnteries = entryArray.Count();
    TInt dispStrLength(KZero);
    TBuf <KFullTimeStringSize> meetingTitle     ;
    TBuf <KFullTimeStringSize> meetingLocation ;
    TTime startTime     ;
    TTime endTime       ;

    if (numEnteries != KZero)
        {// there is atleast one entry
        // get the first conflicting entry and set the string for display
        meetingTitle    = entryArray[KZero]->SummaryL();
        meetingLocation = entryArray[KZero]->LocationL();

        
        if ( ( meetingLocation.Length()== 0 ) && ( meetingTitle.Length() == 0 ) )
        	{
            // if no title, set unnamed text:
            HBufC* title = StringLoader::LoadLC ( R_QTN_MEET_REQ_CONFLICT_UNNAMED );
            meetingTitle.Copy( *title );
            CleanupStack::PopAndDestroy( title );
        	}
        
        dispStrLength = meetingTitle.Length() + meetingLocation.Length();
        startTime   = entryArray[KZero]->StartTimeL().TimeLocalL();
        endTime     = entryArray[KZero]->EndTimeL().TimeLocalL();
        }

    HBufC*  stringHolder =
            StringLoader::LoadLC(R_QTN_MEET_REQ_CONFLICTS_WITH_LABEL);
    dispStrLength = stringHolder->Length() + meetingTitle.Length()
                    +  meetingLocation.Length() + KFullTimeStringSize;

    HBufC* timeFormatString = CCoeEnv::Static()->AllocReadResourceLC(R_QTN_TIME_USUAL_WITH_ZERO);

    TBuf<KTimeStringLength> startBuf;
    TBuf<KTimeStringLength> endBuf;

    startTime.FormatL( startBuf, *timeFormatString );
    endTime.FormatL( endBuf, *timeFormatString );

    CDesCArrayFlat* strings = new(ELeave) CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );
    strings->AppendL( startBuf ); //First string
    strings->AppendL( endBuf ); //Second string

    HBufC* finalTimeBuf =
        StringLoader::LoadL(
                R_QTN_MEET_REQ_TIME_SEPARATOR,
                *strings,
                CEikonEnv::Static() ); // codescanner::eikonenvstatic
    CleanupStack::PopAndDestroy( strings );
    CleanupStack::PushL( finalTimeBuf );

    HBufC* displayString = HBufC::NewLC(dispStrLength);
    displayString->Des().Append(stringHolder->Des());
    displayString->Des().Append(KNewLine);
    displayString->Des().Append(finalTimeBuf->Des());


    displayString->Des().Append(KMessageStart);
    // Check the length of meetingTitle
    // If its greater than some KMaxSummaryLength value then
    // truncate to some reasonable value & append KStringCont
    if ( meetingTitle.Length() > KMaxSummaryLength )
        {
        displayString->Des().Append(meetingTitle.Ptr(),KMaxSummaryLength);
        displayString->Des().Append(KStringCont);
        }
    else
        {
        displayString->Des().Append(meetingTitle);
        displayString->Des().Append(KSpace);
        }

    displayString->Des().Append(meetingLocation);
    displayString->Des().Append(KMessageEnd);

    iNote->SetTextL(displayString->Des() );

    CleanupStack::PopAndDestroy( displayString );
    CleanupStack::PopAndDestroy( finalTimeBuf );
    CleanupStack::PopAndDestroy( timeFormatString );
    CleanupStack::PopAndDestroy( stringHolder );
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
        
        // TODO: use XML layout data. Use this one popup_preview_text_window
        /*iNote->SetPositionAndAlignment( CESMRLayoutManager::ConflictPopupPosition(),
                EHLeftVCenter );*/
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

