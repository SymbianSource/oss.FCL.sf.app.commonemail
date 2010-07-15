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
* Description:  ESMR UI Container class
*
*/

#include "cesmrviewerattendeesfield.h"

#include "mesmrlistobserver.h"
#include "cesmrrichtextlink.h"
#include "esmrfieldbuilderdef.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"

#include <caluser.h>
#include <calentry.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <CPbkContactEngine.h>
#include <AknBidiTextUtils.h>

// DEBUG
#include "emailtrace.h"


// CONSTANTS
/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

_LIT( KNewLine, "\f" );
const TInt KMaxLineCount = 4;
const TInt KNumberOfComponents = 2;
const TInt KMargin = 5;

static void RPointerArrayResetAndDestroy( TAny* aPtr )
    {
    static_cast<RPointerArray<CESMRRichTextLink>*>( aPtr )->ResetAndDestroy();
    }

} // namespace


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::NewL
// -----------------------------------------------------------------------------
//
CESMRViewerAttendeesField* CESMRViewerAttendeesField::NewL(
        CCalAttendee::TCalRole aRole )
    {
    FUNC_LOG;
    CESMRViewerAttendeesField* self =
        new (ELeave) CESMRViewerAttendeesField( aRole );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::ConstructL( )
    {
    FUNC_LOG;

    SetFieldId ( (iRole == CCalAttendee::EReqParticipant) ? EESMRFieldAttendee
            : EESMRFieldOptAttendee );

    iTitle = CMRLabel::NewL( this );

    iRichTextViewer = CESMRRichTextViewer::NewL (this );
    CESMRField::ConstructL( iRichTextViewer ); // ownership transferred
    iRichTextViewer->SetEdwinSizeObserver ( this );
    iRichTextViewer->SetLinkObserver (this );
    iRichTextViewer->SetParent( this );

    HBufC* label = StringLoader::LoadLC (iRole == CCalAttendee::EReqParticipant ?
            R_QTN_MEET_REQ_LABEL_REQUIRED : R_QTN_MEET_REQ_LABEL_OPT );
    iTitle->SetTextL ( *label );

    CleanupStack::PopAndDestroy( label );
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::InitializeL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::InitializeL()
    {
    FUNC_LOG;
    TAknLayoutText layoutText =
        NMRLayoutManager::GetLayoutText( Rect(),
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    iRichTextViewer->SetFontL( layoutText.Font() );
    iRichTextViewer->ApplyLayoutChangesL();
    UpdateAttendeesListL();
    //wake up current contact menu selection by calling this
    iRichTextViewer->FocusChanged( ENoDrawNow );
    iRichTextViewer->SetEventQueue( iEventQueue );
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::SetOutlineFocusL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL (aFocus );
    iRichTextViewer->SetFocus( aFocus );

    if ( aFocus )
        {
        SetMiddleSoftKeyVisible( ETrue );
        }
    else
        {
        iShowAllAttendees = EFalse;
        UpdateAttendeesListL();
        iRichTextViewer->ResetActionMenuL();
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::InternalizeL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry();
    RPointerArray<CCalAttendee>& attendees = entry.AttendeesL();
    iCalAttendees.ReserveL(attendees.Count());
    for (TInt i=0; i<attendees.Count ( ); ++i )
        {
        CCalAttendee* attendee = attendees[i];
        if ( attendee->RoleL() == iRole )
            {
            iCalAttendees.AppendL( attendee );
            }
        }

    UpdateAttendeesListL();

    if ( iRichTextViewer->TextLength() > 0 )
        {
        //wake up current contact menu selection by calling this
        iRichTextViewer->FocusChanged( ENoDrawNow );
        }
    else
        {
        iObserver->HideControl( iFieldId );
        }
    iDisableRedraw = ETrue;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::GetMinimumVisibleVerticalArea
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::GetMinimumVisibleVerticalArea(
        TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    aLower = iRichTextViewer->CurrentLineNumber() * iRichTextViewer->RowHeight();
    aUpper = aLower - iRichTextViewer->RowHeight();
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::GetCursorLineVerticalPos
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    aLower = iRichTextViewer->CurrentLineNumber() * iRichTextViewer->RowHeight();
    aUpper = aLower - iRichTextViewer->RowHeight();
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::ListObserverSet
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::ListObserverSet( )
    {
    FUNC_LOG;
    iRichTextViewer->SetListObserver( iObserver );
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::SizeChanged
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::SizeChanged( )
    {
    FUNC_LOG;
    // Store iRichTextViewer original width.
    TInt richTextViewerWidth = iRichTextViewer->Size().iWidth;

    // Get field's rect
    TRect rect( Rect() );

    // First row for title
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TRect rowRect = rowLayoutRect.Rect();

    // Layout title to first row's rect
    TAknTextComponentLayout titleLayout =
        NMRLayoutManager::GetTextComponentLayout(
                NMRLayoutManager::EMRTextLayoutText );
    AknLayoutUtils::LayoutLabel( iTitle, rect, titleLayout );

    TAknLayoutText labelLayout =
            NMRLayoutManager::GetLayoutText(
                    rect, NMRLayoutManager::EMRTextLayoutText );

    // Setting font for the label
    iTitle->SetFont( labelLayout.Font() );

    // Move upper left corner below first line and get second row's rect.
    rect.iTl.iY += rowRect.Height();
    rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 2 );
    rowRect = rowLayoutRect.Rect();

    // Get default 1 row editor rect.
    TAknLayoutText viewerLayoutText =
        NMRLayoutManager::GetLayoutText( rowRect,
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    TRect viewerRect = viewerLayoutText.TextRect();
    // Resize resize it's height according to actual height required by edwin.
    viewerRect.Resize( 0, iExpandedSize.iHeight - viewerRect.Height() );

    // Layouting focus
    TRect bgRect( 0, 0, 0, 0 );
    if( iCalAttendees.Count() > 1 )
    	{
    	bgRect.SetRect(
    			TPoint( viewerRect.iTl.iX,
    					viewerRect.iTl.iY - FocusMargin() ),
    					TPoint( viewerRect.iBr.iX,
    							viewerRect.iBr.iY + FocusMargin() ) );
    	}
    else
    	{
    	TAknLayoutRect bgLayoutRect =
    	NMRLayoutManager::GetLayoutRect(
    			rowRect, NMRLayoutManager::EMRLayoutTextEditorBg );
    	bgRect = bgLayoutRect.Rect();
    	}

    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );

    iRichTextViewer->SetRect( viewerRect );

    // Failures are ignored.
    TRAP_IGNORE(
            iRichTextViewer->SetFontL( viewerLayoutText.Font() );
            if( iCalAttendees.Count() > 0 )
                {
                iRichTextViewer->SetLineSpacingL( LineSpacing() );
                }
            iRichTextViewer->ApplyLayoutChangesL();
            );

    // Update AttendeesList only if iRichTextViewer width was changed.
    // This happens when screen orientation changes e.g. This check
    // also prevents recursive calls to UpdateAttendeesListL since
    // it causes edwin size changed event.
    if ( iRichTextViewer->Size().iWidth != richTextViewerWidth )
        {
        // Most of this case is screen orientation, in this case we need to
        // Record the index of focusing link, after updating link array, then
        // reset the focusing to original one.
        TInt focusingIndex = iRichTextViewer->GetFocusLink();
        // Ignore leave, there's nothing we can do if leave occurs.
        TRAP_IGNORE( UpdateAttendeesListL() );
        if ( KErrNotFound != focusingIndex )
            {
            iRichTextViewer->SetFocusLink( focusingIndex );
            //wake up current contact menu selection by calling this
            iRichTextViewer->FocusChanged(ENoDrawNow);
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRViewerAttendeesField::OfferKeyEventL( const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response = EKeyWasNotConsumed;
    response = iRichTextViewer->OfferKeyEventL ( aEvent, aType );

    //track up and down events to change MSK
    if ( aType == EEventKey )
        {
        if ( aEvent.iScanCode == EStdKeyUpArrow
             || aEvent.iScanCode == EStdKeyDownArrow )
            {
            const CESMRRichTextLink* link =
                iRichTextViewer->GetSelectedLink ( );
            if( link )
                {
                if (link->Type() == CESMRRichTextLink::ETypeShowAll )
                    {
                    ChangeMiddleSoftKeyL(
                            EESMRCmdShowAllAttendees, R_QTN_MEET_REQ_SHOW_ALL);
                    }
                else
                    {
                    RestoreMiddleSoftKeyL();
                    SetMiddleSoftKeyVisible( ETrue );
                    }
                }
            }
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttendeesField::MinimumSize()
// ---------------------------------------------------------------------------
//
TSize CESMRViewerAttendeesField::MinimumSize()
    {
    // Minimum size ->  Height: TitleRow + Editor size + Margin
    //                  Width: Parent's Width
    //                   (so the content pane that holds all the fields)
    TRect rect( Rect() );
    TAknLayoutRect row1LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TInt titleHeight = row1LayoutRect.Rect().Height();

    // Add title field height
    TInt totalHeight = titleHeight;

    TAknLayoutRect row2LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 2 );
    TInt editorRowHeight = row2LayoutRect.Rect().Height();

    TAknTextComponentLayout editorLayout =
        NMRLayoutManager::GetTextComponentLayout(
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );

    // Editor height from Layout data
    TInt editorHeight = editorLayout.H();
    // Margin is the outer row's height - editor's layout height
    TInt margin = editorRowHeight - editorHeight;
    // Parent has the width
    TInt width( Parent()->Size().iWidth );
    // Count the total height of the attendee field.
    // iExpandedSize is used because the Editor size might be something else
    // than what is stated in Layout data.
    totalHeight += iExpandedSize.iHeight + margin;
    return TSize( width, totalHeight );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttendeesField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerAttendeesField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;

    TBool isUsed( EFalse );
    if (aCommand == EESMRCmdShowAllAttendees)
        {
        iShowAllAttendees = ETrue;
        UpdateAttendeesListL();

        RestoreMiddleSoftKeyL();
        //wake up current contact menu selection by calling this
        iRichTextViewer->FocusChanged(ENoDrawNow);

        isUsed = ETrue;
        }
    if ( aCommand == EAknSoftkeySelect )
        {
        iRichTextViewer->LinkSelectedL();
        isUsed = ETrue;
        }

    return isUsed;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::HandleEdwinSizeEventL
// -----------------------------------------------------------------------------
//
TBool CESMRViewerAttendeesField::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    iExpandedSize = aSize;
    
    if ( iObserver && iDisableRedraw )
        {
        iObserver->ControlSizeChanged ( this );
        
        if ( !iOutlineFocus )
            {
            RecordField();
            }
        }
    
    return iDisableRedraw;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::HandleRichTextLinkSelection
// -----------------------------------------------------------------------------
//
TBool CESMRViewerAttendeesField::HandleRichTextLinkSelection(
        const CESMRRichTextLink* aLink )
    {
    FUNC_LOG;
    if (aLink->Type ( )== CESMRRichTextLink::ETypeShowAll )
        {
        iShowAllAttendees = ETrue;
        TRAPD( err, UpdateAttendeesListL() );

        if (err == KErrNone )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CESMRViewerAttendeesField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = KNumberOfComponents;
    return count;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAttendeesField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            {
            return iTitle;
            }
        case 1:
            {
            return iRichTextViewer;
            }
        default:
            {
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::~CESMRViewerAttendeesField
// -----------------------------------------------------------------------------
//
CESMRViewerAttendeesField::~CESMRViewerAttendeesField( )
    {
    FUNC_LOG;
    delete iTitle;
    iCalAttendees.Reset();
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::CESMRViewerAttendeesField
// -----------------------------------------------------------------------------
//
CESMRViewerAttendeesField::CESMRViewerAttendeesField(
        CCalAttendee::TCalRole aRole ) :
    iRole( aRole)
    {
    FUNC_LOG;
    SetFocusType( EESMRHighlightFocus );
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::ClipTextLC
// -----------------------------------------------------------------------------
//
HBufC* CESMRViewerAttendeesField::ClipTextLC(
        const TDesC& aText, const CFont& aFont, TInt aWidth )
    {
    FUNC_LOG;
    HBufC* text = HBufC::NewLC( aText.Length() + KAknBidiExtraSpacePerLine );
    TPtr textPtr = text->Des();
    AknBidiTextUtils::ConvertToVisualAndClip(
            aText, textPtr, aFont, aWidth, aWidth );
    return text;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::UpdateAttendeesListL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::UpdateAttendeesListL()
    {
    FUNC_LOG;
    RBuf buffer; // codescanner::resourcenotoncleanupstack
    buffer.CleanupClosePushL();

    TAknLayoutText text = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );
    const CFont* font = text.Font();

    TSize iconSize( 20, 20);

    TInt maxLineWidth = iRichTextViewer->LayoutWidth();
    maxLineWidth -= font->TextWidthInPixels( KAddressDelimeterSemiColon );
    maxLineWidth -= iconSize.iWidth;

    RPointerArray<CESMRRichTextLink> attendeeLinks;  // codescanner::resourcenotoncleanupstack
    TCleanupItem arrayCleanup( RPointerArrayResetAndDestroy, &attendeeLinks );
    CleanupStack::PushL( arrayCleanup );
    attendeeLinks.ReserveL(iCalAttendees.Count());
    for ( TInt i = 0; i < iCalAttendees.Count(); ++i )
        {
        CCalAttendee* attendee = iCalAttendees[i];

        // Attendee email address.
        TPtrC addr = attendee->Address();
        // Attendee common name.
        TPtrC name = attendee->CommonName();
        // Actual text to be added to attendee field (email or common name).
        TPtrC text = addr;

        // If attendee has common name, use it instead.
        if ( name.Length() > 0 )
            {
            text.Set( name );
            }

        HBufC* clippedTextHBufC = ClipTextLC( text, *font, maxLineWidth );
        TPtr clippedText = clippedTextHBufC->Des();
        clippedText.Trim();

        if ( clippedText.Length() > 0 )
            {
            CESMRRichTextLink* link = CESMRRichTextLink::NewL(
                    buffer.Length(),
                    clippedText.Length() +
                        KAddressDelimeterSemiColon().Length(),
                    addr,
                    CESMRRichTextLink::ETypeEmail,
                    CESMRRichTextLink::ETriggerKeyRight );
            CleanupStack::PushL( link );
            attendeeLinks.AppendL( link );
            CleanupStack::Pop( link );

            // Append text and semicolon to buffer.
            buffer.ReAllocL( buffer.Length() +
                             clippedText.Length() +
                             KAddressDelimeterSemiColon().Length() +
                             KNewLine().Length() );
            buffer.Append( clippedText );
            buffer.Append( KAddressDelimeterSemiColon );
            buffer.Append( KNewLine );
            }

        CleanupStack::PopAndDestroy( clippedTextHBufC );

        // End loop if we have exceeded KMaxLineCount and
        // we don't want to show all attendees
        if ( attendeeLinks.Count() > KMaxLineCount && !iShowAllAttendees )
            {
            break;
            }
        }

    // If there are more lines in use than KMaxLineCount and
    // we don't want to show all attendees, leave only
    // KMaxLineCount-1 attendees and add 'show all'
    // link to the end.
    if ( attendeeLinks.Count() > KMaxLineCount && !iShowAllAttendees )
        {
        // Remove unnecessary part of buffer.
        buffer.SetLength( attendeeLinks[KMaxLineCount - 1]->StartPos() );

        // Remove links from the list.
        while ( attendeeLinks.Count() >= KMaxLineCount )
            {
            delete attendeeLinks[KMaxLineCount - 1];
            attendeeLinks.Remove( KMaxLineCount - 1 );
            }

        // Create and append 'show all' link.
        CESMRRichTextLink* showAllLink;
        HBufC* showAllBuf = StringLoader::LoadLC( R_QTN_MEET_REQ_SHOW_ALL );
        buffer.ReAllocL( buffer.Length() +
                         showAllBuf->Length() );
        showAllLink = CESMRRichTextLink::NewL( buffer.Length ( ),
                showAllBuf->Length(), KNullDesC,
                CESMRRichTextLink::ETypeShowAll,
                CESMRRichTextLink::ETriggerKeyOk );
        buffer.Append( *showAllBuf );
        CleanupStack::PopAndDestroy( showAllBuf );
        CleanupStack::PushL( showAllLink );
        attendeeLinks.AppendL( showAllLink );
        CleanupStack::Pop( showAllLink );
        }
    // Remove unnecessary new line from the end of buffer.
    else if ( buffer.Length() >= KNewLine().Length() )
        {
        buffer.SetLength( buffer.Length() - KNewLine().Length() );
        }

    if ( buffer.Length() > 0 )
        {
        iRichTextViewer->SetTextL( &buffer );
        iRichTextViewer->SetMargins( KMargin );

        // Add all links to iRichTextViewer.
        while ( attendeeLinks.Count() > 0 )
            {
            CESMRRichTextLink* link = attendeeLinks[0];
            CleanupStack::PushL( link );
            attendeeLinks.Remove( 0 );
            iRichTextViewer->AddLinkL( link );
            CleanupStack::Pop( link );
            }
        }
    SizeChanged();
    CleanupStack::PopAndDestroy( &attendeeLinks );
    CleanupStack::PopAndDestroy( &buffer );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttendeesField::LineSpacing
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAttendeesField::LineSpacing()
    {
    FUNC_LOG;
    // Calculates the line spacing based on default one line layout data
    TInt lineSpacing( 0 );

    TRect rect( Rect() );

    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TRect rowRect = rowLayoutRect.Rect();

    TAknLayoutText labelLayout =
     NMRLayoutManager::GetLayoutText(
             rect, NMRLayoutManager::EMRTextLayoutTextEditor );

    TRect defaultTextRect = labelLayout.TextRect();

    TInt difference = rowRect.Height() - defaultTextRect.Height();

    lineSpacing = difference * 2;

    return lineSpacing;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttendeesField::FocusMargin
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAttendeesField::FocusMargin()
    {
    FUNC_LOG;
    // Calculates focus margin based on default one line layout data
    TInt focusMagin( 0 );

    TRect rect( Rect() );

    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TRect rowRect = rowLayoutRect.Rect();

    TAknLayoutText labelLayout =
     NMRLayoutManager::GetLayoutText(
             rect, NMRLayoutManager::EMRTextLayoutTextEditor );

    TRect defaultTextRect = labelLayout.TextRect();

    TAknLayoutRect bgLayoutRect =
        NMRLayoutManager::GetLayoutRect(
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect defaultBgRect( bgLayoutRect.Rect() );

    TInt difference = defaultBgRect.Height() - defaultTextRect.Height();

    focusMagin = TReal( difference / 2 );

    return focusMagin;
    }

// End of file

