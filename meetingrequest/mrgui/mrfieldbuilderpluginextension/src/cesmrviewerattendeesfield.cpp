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
* Description:  ESMR UI Container class
*
*/

#include "cesmrviewerattendeesfield.h"

#include "mesmrlistobserver.h"
#include "cesmrborderlayer.h"
#include "cesmrrichtextlink.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrlayoutmgr.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"

#include <caluser.h>
#include <calentry.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <CPbkContactEngine.h>
#include <AknBidiTextUtils.h>
#include "cmrlistpane.h"

// DEBUG
#include "emailtrace.h"

using namespace ESMRLayout;

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

    iTitle = CMRLabel::NewL();
    iTitle->SetParent( this );
    
    iRichTextViewer = CESMRRichTextViewer::NewL (this );
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
    iRichTextViewer->SetFontL( layoutText.Font(), iLayout );
    UpdateAttendeesListL();
    //wake up current contact menu selection by calling this
    iRichTextViewer->FocusChanged( ENoDrawNow );
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::SetOutlineFocusL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );
    iRichTextViewer->SetFocus( aFocus );
    
    if ( aFocus )
        {
        SetMiddleSoftKeyVisible( ETrue );
        }
    else
        {
        TInt oldShowAttendees = iShowAllAttendees;
        iShowAllAttendees = EFalse;
        if (oldShowAttendees != iShowAllAttendees)
            {
            UpdateAttendeesListL();
            }
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
        iObserver->RemoveControl( iFieldId );
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
    aLower = iRichTextViewer->CurrentLineNumber() * iRichTextViewer->RowHeight() + iTitle->Size().iHeight;
    aUpper = aLower - iRichTextViewer->RowHeight() - iTitle->Size().iHeight;
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
    // Color should be overrided after layouting
    // If this function leaves we'll have to use default color
    TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( 
                                *iTitle, 
                                EColorLabelText,
                                KRgbBlack ));
    
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
    iRichTextViewer->SetRect( viewerRect );  
    
    // Update AttendeesList only if iRichTextViewer width was changed.
    // This happens when screen orientation changes e.g. This check
    // also prevents recursive calls to UpdateAttendeesListL since
    // it causes edwin size changed event.
    if ( iRichTextViewer->Size().iWidth != richTextViewerWidth )
        {
        // Ignore leave, there's nothing we can do if leave occurs.
        TRAP_IGNORE( UpdateAttendeesListL() );
        //wake up current contact menu selection by calling this
        iRichTextViewer->FocusChanged(ENoDrawNow);
        }

    DrawDeferred();
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
// CESMRViewerAttendeesField::SetContainerWindowL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    CESMRField::SetContainerWindowL( aContainer );
    iRichTextViewer->SetContainerWindowL( aContainer );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttendeesField::MinimumSize()
// ---------------------------------------------------------------------------
//
TSize CESMRViewerAttendeesField::MinimumSize()
    {
    // Parent rect will be list area later --> no need to calculate it manually.
    TRect parentRect = Parent()->Rect();
    TRect contentRect = NMRLayoutManager::GetLayoutRect( 
            parentRect, NMRLayoutManager::EMRLayoutListArea ).Rect();
    // We have two lines;  title and richtextviewer.
    TRect fieldRect = 
        NMRLayoutManager::GetFieldLayoutRect( contentRect, 2 ).Rect();
    // Get row size for second row (richtext viewer).
    TRect rowRect = 
        NMRLayoutManager::GetFieldRowLayoutRect( fieldRect, 2 ).Rect();
    // Get size for default 1 line editor.
    TRect viewerRect = NMRLayoutManager::GetLayoutText( 
            rowRect, 
            NMRLayoutManager::EMRTextLayoutMultiRowTextEditor ).TextRect();
    // Adjust field size so that there's room for expandable editor.
    fieldRect.Resize( 0, iExpandedSize.iHeight - viewerRect.Height() );
    return fieldRect.Size();
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttendeesField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case EESMRCmdClipboardCopy:
            {
            iRichTextViewer->CopyCurrentLinkValueToClipBoardL();
            break;
            }
        case EESMRCmdShowAllAttendees:
            {
            iShowAllAttendees = ETrue;
            UpdateAttendeesListL();
        
            RestoreMiddleSoftKeyL();
            //wake up current contact menu selection by calling this
            iRichTextViewer->FocusChanged(ENoDrawNow);
            break;
            }
        case EAknSoftkeySelect:
            {
            iRichTextViewer->LinkSelectedL();
            break;
            }
        default:
            // do nothing for now..
            break;
        }
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
    if ( aLink->Type() == CESMRRichTextLink::ETypeShowAll )
        {
        iShowAllAttendees = ETrue;
        TRAPD( err, UpdateAttendeesListL() );

        if ( err == KErrNone )
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
    delete iRichTextViewer;
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
    //do nothing
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

    iRichTextViewer->SetMargins( KMargin );
    
    TSize oldSize = Size();

    TRect rect( Rect() );
    TAknLayoutText layoutText =
        NMRLayoutManager::GetLayoutText( rect, 
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    TRect viewerRect = layoutText.TextRect();
    TInt maxLineWidth = viewerRect.Size().iWidth;
    const CFont* font = layoutText.Font();    

    if ( maxLineWidth > 0 )  // control size is set
        {
        TInt linkCount = iCalAttendees.Count();
        TBool notAllShown(EFalse);
        if ( !iShowAllAttendees )
            {
            linkCount = Min(KMaxLineCount, iCalAttendees.Count() );
            notAllShown = ( linkCount != iCalAttendees.Count() );
            }
        
        RPointerArray<CESMRRichTextLink> attendeeLinks;  // codescanner::resourcenotoncleanupstack
        TCleanupItem arrayCleanup( RPointerArrayResetAndDestroy, &attendeeLinks );
        CleanupStack::PushL( arrayCleanup );
        attendeeLinks.ReserveL( linkCount );
        
        for ( TInt i = 0; i < linkCount; ++i )
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
            }
        
        // If there are more lines in use than KMaxLineCount and
        // we don't want to show all attendees, leave only
        // KMaxLineCount-1 attendees and add 'show all'
        // link to the end.
        if ( notAllShown )
            {
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

        CleanupStack::PopAndDestroy( &attendeeLinks );
        
        TSize newSize = Size();
        if (oldSize != newSize)
            {
            CMRListPane* parent = static_cast<CMRListPane*>(Parent());
            parent->SizeChanged();
            }
        }
    CleanupStack::PopAndDestroy( &buffer );
    }


// -----------------------------------------------------------------------------
// CESMRViewerAttendeesField::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttendeesField::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    CCoeControl::HandlePointerEventL(aPointerEvent);
    }
// End of file

