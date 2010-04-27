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
*  Description : CEikRichTextEditor based Rich Text viewer
*  Version     : %version: e002sa33#40 %
*
*/

#include "cesmrrichtextviewer.h"

#include "mesmrlistobserver.h"// SCROLLING_MOD: List observer header
#include "esmrconfig.hrh"
#include "esmrhelper.h"
#include "cesmrfieldcommandevent.h"
#include "mesmrfieldeventqueue.h"
#include "cesmrrichtextlink.h"
#include "cesmrcontactmenuhandler.h"
#include "nmrbitmapmanager.h"
#include "nmrcolormanager.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrfield.h"

#include <esmrgui.rsg>
#include <commonphoneparser.h>
#include <finditemengine.h>
#include <txtrich.h>
#include <AknsUtils.h>
#include <AknBidiTextUtils.h>
#include <AknUtils.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <baclipb.h> // for clipboard copy
#include <txtclipboard.h>

// DEBUG
#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace{ // codescanner::namespace

#ifdef _DEBUG
    enum TPanic
        {
        EParaFormatNotInitialised = 1
        };
    void Panic( TPanic aPanic )
        {
        _LIT( KCategory, "CESMRRichTextViewer" );
        User::Panic( KCategory(), aPanic );
        }
#endif // DEBUG

// Side margin in twips, needed because not possible to set it in pixels
const TInt KDefaultTextSideMargin( 1 );

}//namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRRichTextViewer* CESMRRichTextViewer::NewL(
        const CCoeControl* aParent)
    {
    FUNC_LOG;
    CESMRRichTextViewer* self = new (ELeave) CESMRRichTextViewer;
    CleanupStack::PushL ( self );
    self->ConstructL( aParent );
    CleanupStack::Pop ( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::~CESMRRichTextViewer
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRRichTextViewer::~CESMRRichTextViewer( )
    {
    FUNC_LOG;
    iLinkList.ResetAndDestroy ( );
    iESMRStatic.Close ( );
    delete iParaFormat;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::PositionChanged
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::PositionChanged( )
    {
    FUNC_LOG;
    CTextView* view = TextView ( );
    if ( view )
        {
        view->SetViewRect ( Rect ( ) );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::FocusChanged
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::FocusChanged(
        TDrawNow aDrawNow )
    {
    FUNC_LOG;
    CEikRichTextEditor::FocusChanged( aDrawNow );
    if ( !TextLayout() || !TextView() )
        {
        // not constructed totally:
        return;
        }

    // Check the current selection.
    // If a link has been highlighted, don't change it.
    TCursorSelection selection = Selection();

    // Gaining focus
    if ( IsFocused()
         && iLinkList.Count() > 0
         && ( selection.Length() == 0 ) )
        {
        // We need the field indexes, cast required
        CESMRField* parent = static_cast< CESMRField* >( Parent() );

        iCntMenuHdlr->SetContactMenuObserver( this );
        // Focus coming from above ...
        if( parent->CurrentItemIndex() > parent->PreItemIndex() )
            {
            // ... Highlight first link
            TRAP_IGNORE( HighlightLinkL( *( iLinkList[0] ) ) );
            }
        // Focus coming from below ...
        else
            {
            // ... Highlight last link
            TRAP_IGNORE( HighlightLinkL( *( iLinkList[iLinkList.Count() - 1 ] ) ) );
            }
        DrawDeferred();
        }

    // Losing focus
    if ( !IsFocused() )
        {
        TRAP_IGNORE(
                SetSelectionL( CursorPos(), CursorPos() );
                ResetActionMenuL();
                );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::FindTextLinkBetweenNextScrollArea
// -----------------------------------------------------------------------------
//
TInt CESMRRichTextViewer::FindTextLinkBetweenNextScrollArea(
        TInt aStartRow,
        TInt aEndRow,
        TCursorPosition::TMovementType aDirection)
    {
    FUNC_LOG;
    TInt linkRow(KErrNotFound);

    // fetch the line number where the next link is located:
    if ( aDirection == TCursorPosition::EFLineDown )
        {
        for (TInt i = 0; i<iLinkList.Count(); i++)
            {
            linkRow = ValidLinkForFocusing(i, aDirection, aStartRow, aEndRow);
            if ( linkRow != KErrNotFound)
                {
                return linkRow;
                }
            }
        }
    else
        for (TInt i = iLinkList.Count()-1; i>=0 ; i--)
            {
            linkRow = ValidLinkForFocusing(i, aDirection, aStartRow, aEndRow);
            if ( linkRow != KErrNotFound)
                {
                return linkRow;
                }
            }

    return linkRow;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ValidLinkForFocusing
// -----------------------------------------------------------------------------
//
TInt CESMRRichTextViewer::ValidLinkForFocusing(
        TInt aIndex,
        TCursorPosition::TMovementType aDirection,
        TInt aStartRow,
        TInt aEndRow )
    {
    FUNC_LOG;

    if ( iLinkList.Count() > aIndex)
	    {
	    TInt currPos( KErrNotFound );
        const CESMRRichTextLink* currentLink = GetSelectedLink();
        if ( currentLink )
            {
            currPos = currentLink->StartPos();
            }

        TInt pos = iLinkList[ aIndex ]->StartPos();

	    TInt checkRow = TextLayout()->GetLineNumber( pos );

	    if ( checkRow >= aStartRow && checkRow <= aEndRow )
	        {
	        if ( aDirection == TCursorPosition::EFLineDown &&
	             pos > currPos ||
	             aDirection == TCursorPosition::EFLineUp &&
	             ( pos < currPos || currPos == KErrNotFound ) )
	            {
	            // link found between next scroll area.
	            return checkRow;
	            }
	        }
	    }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::OfferKeyEventL
// -----------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CESMRRichTextViewer::OfferKeyEventL(
        const TKeyEvent &aKeyEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );

    // Handle only event keys.
    if( aType != EEventKey )
        {
        return response;
        }

    /*
     * Handles scrolling between rich text links.
     */
    const CESMRRichTextLink* selectedLink = GetSelectedLink();
    TInt linkCount = iLinkList.Count();

    // If a link is selected, it means that we are operating within
    // the field and can move focus between the links.
    if( linkCount && selectedLink )
        {
        TInt selectedLinkIndex = iLinkList.Find( selectedLink );

        switch ( aKeyEvent.iCode )
            {
            case EKeyLeftArrow:
            case EKeyUpArrow:
                {
                // If possible and exists ...
                if( selectedLinkIndex > 0 &&
                        selectedLinkIndex < linkCount )
                    {
                    // ...Highlight previous link.
                    HighlightLinkL( *( iLinkList[ selectedLinkIndex - 1 ] ) );
                    response = EKeyWasConsumed;

                    // View update required, for example if link is out of
                    // viewable area.
                    UpdateViewL( aKeyEvent );
                    }

                break;
                }
            case EKeyRightArrow:
            case EKeyDownArrow:
                {
                // If possible and exists ...
                if( selectedLinkIndex + 1 < linkCount &&
                        selectedLinkIndex >= 0 )
                    {
                    // ...Highlight next link.
                    HighlightLinkL( *( iLinkList[ selectedLinkIndex + 1 ] ) );
                    response = EKeyWasConsumed;

                    // View update required, for example if link is out of
                    // viewable area.
                    UpdateViewL( aKeyEvent );
                    }

                break;
                }
            case EKeyDevice3: // Selection key
                {
                // No implementation. Non-MSK devices might require this.
                break;
                }
            default:
                break;
            }
        }

    return response;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( Rect().Contains( aPointerEvent.iPosition) )
        {
        TBool linkFound( EFalse );

        switch ( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Down:
            case TPointerEvent::EDrag:
                {
                RRegion linkArea;
                CleanupClosePushL( linkArea );

                // Find matching link
                TInt count = iLinkList.Count();

                for ( TInt i = 0; i < count; ++i )
                    {
                    CESMRRichTextLink* link = iLinkList[ i ];
                    GetLinkAreaL( linkArea, *link );

                    if ( linkArea.Contains( aPointerEvent.iPosition ) )
                        {
                        if ( link != GetSelectedLink() )
                            {
                            iCntMenuHdlr->Reset();
                            iCntMenuHdlr->SetContactMenuObserver( this );
                            HighlightLinkL( *link );
                            DrawDeferred();
                            }

                        linkFound = ETrue;
                        break;
                        }
                    }

                CleanupStack::PopAndDestroy( &linkArea );

                break;
                }

            case TPointerEvent::EButton1Up:
                {
                const CESMRRichTextLink* link = GetSelectedLink();

                TBool menuAvailable = iCntMenuHdlr->OptionsMenuAvailable();

                if ( link
                     && ( menuAvailable
                     || link->Type() == CESMRRichTextLink::ETypeLocationUrl
                     || link->Type() == CESMRRichTextLink::ETypeAttachment
                     || link->Type() == CESMRRichTextLink::ETypeShowAll ) )
                    {
                    linkFound = ETrue;

                    LinkSelectedL();
                    }
                else if ( link && !menuAvailable )
                    {
                    linkFound = ETrue;
                    iOpenActionMenu = ETrue;
                    }
                break;
                }
            default:
                {
                break;
                }
            }

        if ( !linkFound )
            {
            // Tap on plain text
            TextView()->ClearSelectionL();
            ResetActionMenuL();
            DrawDeferred();
            }

        }

    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetMargins
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetMargins( TInt sMargin )
	{
	// Set new value for left and right margins
    TextView()->SetMarginWidths( sMargin ,0);
	}

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetTextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetTextL(
        const TDesC* aText,
        TBool aSearchLinks )
    {
    FUNC_LOG;

    iLinkList.ResetAndDestroy();

    TextView()->SetMarginWidths( KDefaultTextSideMargin ,0);

    // Clear edwin text
    CEikEdwin::SetCursorPosL( 0, EFalse );
    CEikRichTextEditor::SetTextL( &KNullDesC );
    RichText()->Reset();

    // text lenght plus one to ensure the formatting
    // used is full, not band formatting.
    SetUpperFullFormattingLength( aText->Length() + 1 );

    // Set new edwin text
    CEikEdwin::SetTextLimit( aText->Length ( ) );
    CEikRichTextEditor::SetTextL( aText );

    //Make sure cursor is invisible and selection visible
    TextView()->SetCursorVisibilityL(
            TCursor::EFCursorInvisible,
            TCursor::EFCursorInvisible );
    TextView()->SetSelectionVisibilityL( ETrue );

    // Search text for links (highlights)
    if ( aSearchLinks )
        {
        SearchLinksL( *aText );
        // find first link.
        FindTextLinkBetweenNextScrollArea(0, KMaxAddressFieldLines, TCursorPosition::EFLineDown);
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetLinkObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetLinkObserver(
        MESMRRichTextObserver* aLinkObserver )
    {
    FUNC_LOG;
    iLinkObserver = aLinkObserver;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::AddLinkL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::AddLinkL( CESMRRichTextLink* aLink )
    {
    FUNC_LOG;
    // If this is first link, set it highlighted
    TBool highlight = IsFocused()&& ( iLinkList.Count() == 0 );

    // Reserve space for new link
    iLinkList.ReserveL( iLinkList.Count() + 1 );

    RichText()->ApplyCharFormatL( iRichTextLinkFormat,
                                  iRichTextLinkFormatMask,
                                  aLink->StartPos(),
                                  aLink->Length() );

    // Append aLink to link list. Space has been reserved, so this cannot fail.
    iLinkList.AppendL( aLink );

    if ( highlight )
        {
        HighlightLinkL( *aLink );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::InsertLinkL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::InsertLinkL( CESMRRichTextLink* aLink,
                                                TInt aPosition )
    {
    FUNC_LOG;
    // If this is first link, set it highlighted
    TBool highlight = IsFocused()&& ( iLinkList.Count() == 0 );

    // Reserve space for new link
    iLinkList.ReserveL( iLinkList.Count() + 1 );

    RichText()->ApplyCharFormatL( iRichTextLinkFormat,
                                  iRichTextLinkFormatMask,
                                  aLink->StartPos(),
                                  aLink->Length() );

    // Insert aLink to link list. Space has been reserved, so this cannot fail.
    // InsertL panics if aPosition is illegal.
    iLinkList.InsertL( aLink, aPosition );

    if ( highlight )
        {
        HighlightLinkL( *aLink );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::GetSelectedLink
// -----------------------------------------------------------------------------
//
EXPORT_C const CESMRRichTextLink* CESMRRichTextViewer::GetSelectedLink( ) const
    {
    FUNC_LOG;
    CESMRRichTextLink* link( NULL );

    TCursorSelection currentSelection = Selection();

    for ( TInt i = 0; i < iLinkList.Count(); ++i )
        {
        CESMRRichTextLink* tempLink = iLinkList[i];
        TInt startPos = tempLink->StartPos();
        TInt length = tempLink->Length();

        TCursorSelection linkSelection( startPos, startPos + length );

        if( currentSelection.iCursorPos == linkSelection.iCursorPos &&
                currentSelection.iAnchorPos == linkSelection.iAnchorPos )
            {
            link = tempLink;
            break;
            }
        }

    return link;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::GetLinkTextL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CESMRRichTextViewer::GetLinkTextLC(
        const CESMRRichTextLink& aLink ) const
    {
    FUNC_LOG;
    return RichText()->Read( aLink.StartPos(), aLink.Length() ).AllocLC();
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::CESMRRichTextViewer
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetListObserver(
        MESMRListObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::RowHeight
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRRichTextViewer::RowHeight()
    {
    FUNC_LOG;
    TInt bandHeight = TextLayout()->BandHeight();
    // Starts from zero, so let's increase the count by one.
    TInt lineNumberCount = TextLayout()->GetLineNumber( TextLength() ) + 1;

    return ( bandHeight / lineNumberCount );
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::LineCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRRichTextViewer::LineCount()
    {
    FUNC_LOG;

    // First row is 0, so let's add one...
    return ( TextLayout()->GetLineNumber( TextLength() ) + 1 );
    }
// -----------------------------------------------------------------------------
// CESMRRichTextViewer::CurrentLineNumber
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRRichTextViewer::CurrentLineNumber()
    {
    FUNC_LOG;
    // first line is zero, let's increase it by one
    return TextLayout()->GetLineNumber( CursorPos() ) + 1;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetFontL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetFontL( const CFont* aFont )
    {
    FUNC_LOG;
    // iFont is stored for changing font color when losing or
    // gaining focus
    iFont = aFont;
    if ( !iParaFormat )
        {
        iParaFormat = new ( ELeave ) CParaFormat();
        }

    // All this required, otherwise the font loses its
    // antialiasing drawing.
    iParaFormat->iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels;

    iParaFormatMask.SetAttrib( EAttLineSpacing );
    iParaFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
    iParaFormat->iVerticalAlignment = CParaFormat::ECenterAlign;
    iParaFormatMask.SetAttrib( EAttAlignment );
    iParaFormatMask.SetAttrib( EAttVerticalAlignment );

    iCharFormat.iFontSpec = iFont->FontSpecInTwips();

    iCharFormatMask.SetAttrib( EAttFontTypeface );
    iCharFormatMask.SetAttrib( EAttFontHeight );
    iCharFormatMask.SetAttrib( EAttFontPosture );
    iCharFormatMask.SetAttrib( EAttFontStrokeWeight );

    iCharFormat.iFontPresentation.iTextColor =
        NMRColorManager::Color( NMRColorManager::EMRMainAreaTextColor );

    iCharFormatMask.SetAttrib( EAttColor );
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetLineSpacingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetLineSpacingL( TInt aLineSpacingInTwips )
    {
    __ASSERT_DEBUG( iParaFormat, Panic( EParaFormatNotInitialised ) );
    iParaFormatMask.SetAttrib( EAttLineSpacing );
    iParaFormatMask.SetAttrib( EAttLineSpacingControl );
    iParaFormat->iLineSpacingControl =
        CParaFormat::ELineSpacingExactlyInPixels;

    iParaFormat->iLineSpacingInTwips = aLineSpacingInTwips;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ApplyLayoutChanges
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::ApplyLayoutChangesL()
    {
    __ASSERT_DEBUG( iParaFormat, Panic( EParaFormatNotInitialised ) );
    CRichText* richtext = RichText();
    TInt paraCount( richtext->ParagraphCount() );
    // Go through each paragraph and apply the same format
    for( TInt i = 0; i < paraCount; ++i  )
        {
        TInt paraLen( 0 );   // Length of paragraph
        TInt fiChPos( 0 );   // First character position of paragraph
        // Get the length of the paragraph
        fiChPos = richtext->CharPosOfParagraph( paraLen, i );
        richtext->ApplyParaFormatL(
                iParaFormat, iParaFormatMask, fiChPos, paraLen );
        richtext->ApplyCharFormatL(
                iCharFormat, iCharFormatMask, fiChPos, paraLen );
        }
    // This forces CEikEdwin::OnReformatL to notify observer
    // through HandleEdwinSizeEventL about changed size.
    // It is notified only if linecount changes.
    CEikEdwin::iNumberOfLines = 0;
    CEikEdwin::FormatTextL();
    }


// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetSelectedLink
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetFocusLink( TInt aLinkIndex )
    {
    if ( ( aLinkIndex >= 0 ) && ( aLinkIndex < iLinkList.Count() ) )
        {
        TRAP_IGNORE( HighlightLinkL( *( iLinkList[aLinkIndex] ) ) );
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::GetSelectedLink
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRRichTextViewer::GetFocusLink( ) const
    {
    FUNC_LOG;
    // "-1" stand for no link be selected now.
    TInt linkIndex( KErrNotFound );
    if ( 0 == iLinkList.Count() )
        {
        return linkIndex;
        }
    
    TCursorSelection currentSelection = Selection();

    for ( TInt i = 0; i < iLinkList.Count(); ++i )
        {
        CESMRRichTextLink* tempLink = iLinkList[i];
        TInt startPos = tempLink->StartPos();
        TInt length = tempLink->Length();

        TCursorSelection linkSelection( startPos, startPos + length );

        if( currentSelection.iCursorPos == linkSelection.iCursorPos &&
                currentSelection.iAnchorPos == linkSelection.iAnchorPos )
            {
            linkIndex = i;
            break;
            }
        }

    return linkIndex;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::CESMRRichTextViewer
// -----------------------------------------------------------------------------
//
CESMRRichTextViewer::CESMRRichTextViewer( )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::ConstructL(const CCoeControl* aParent)
    {
    FUNC_LOG;

    TInt flags = CEikEdwin::EReadOnly |
                 CEikEdwin::EResizable |
                 CEikEdwin::ENoAutoSelection |
                 CEikEdwin::EAvkonDisableCursor;
    if( !aParent )
        {
        flags |= CEikEdwin::EOwnsWindow;
        }

    CEikRichTextEditor::ConstructL( aParent, 1, 1, flags );
    SetSuppressBackgroundDrawing( ETrue );

    iESMRStatic.ConnectL();
    iCntMenuHdlr = &iESMRStatic.ContactMenuHandlerL();
    iRichTextLinkFormatMask.SetAttrib( EAttFontUnderline );
    iRichTextLinkFormat.iFontPresentation.iUnderline = EUnderlineOn;
    iRichTextLinkFormatMask.SetAttrib( EAttColor );
    iRichTextLinkFormat.iFontPresentation.iTextColor =
        NMRColorManager::Color(
                NMRColorManager::EMRCutCopyPasteHighlightColor );
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::HighlightLinkL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::HighlightLinkL(const CESMRRichTextLink& aLink )
    {
    FUNC_LOG;
    TCursorSelection selection( aLink.StartPos(), aLink.StartPos() + aLink.Length() );

    // If TextView is not constructed yet.
    if ( !TextView() )
        {
        SetSelectionL( selection.iCursorPos, selection.iAnchorPos );
        }
    else
        {
        TextView()->SetPendingSelection( selection );
        }

    SetValueL( aLink );
    ChangeMiddleSoftkeyL( aLink );
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetValueL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::SetValueL( const CESMRRichTextLink& aLink )
    {
    FUNC_LOG;
    // Instantiate contact action menu for selected link
    switch ( aLink.Type() )
        {
        case CESMRRichTextLink::ETypeEmail:
            {
            iCntMenuHdlr->SetValueL( aLink.Value(),
            		CESMRContactMenuHandler::EValueTypeEmail );
            break;
            }
        case CESMRRichTextLink::ETypePhoneNumber:
            {
            iCntMenuHdlr->SetValueL( aLink.Value(),
            		CESMRContactMenuHandler::EValueTypePhoneNumber );
            break;
            }
        case CESMRRichTextLink::ETypeURL:
            {
            iCntMenuHdlr->SetValueL( aLink.Value(),
            		CESMRContactMenuHandler::EValueTypeURL );
            break;
            }
        case CESMRRichTextLink::ETypeAttachment:
            {

            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SearchLinksL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::SearchLinksL(const TDesC& aText )
    {
    FUNC_LOG;
    // Search for emails, phone numbers and urls
    TInt searchCases = CFindItemEngine::EFindItemSearchMailAddressBin |
                       CFindItemEngine::EFindItemSearchPhoneNumberBin |
                       CFindItemEngine::EFindItemSearchURLBin;

    CFindItemEngine* itemEngine =
              CFindItemEngine::NewL ( aText,
                       ( CFindItemEngine::TFindItemSearchCase ) searchCases );
    CleanupStack::PushL (itemEngine );

    const CArrayFixFlat<CFindItemEngine::SFoundItem>
            * foundItems = itemEngine->ItemArray ( );

    // For each found item
    for (TInt i=0; i<foundItems->Count ( ); ++i )
        {
        // iItemType, iStartPos, iLength
        const CFindItemEngine::SFoundItem& item = foundItems->At (i );
        HBufC* valueBuf = aText.Mid (item.iStartPos, item.iLength ).AllocL ( );
        CleanupStack::PushL (valueBuf );

        CESMRRichTextLink::TType type = (CESMRRichTextLink::TType) -1;
        switch (item.iItemType )
            {
            case CFindItemEngine::EFindItemSearchMailAddressBin:
                {
                type = CESMRRichTextLink::ETypeEmail;
                break;
                }
            case CFindItemEngine::EFindItemSearchPhoneNumberBin:
                {
                type = CESMRRichTextLink::ETypePhoneNumber;

                // Remove unsupported characters from phone number
                TPtr phonePtr = valueBuf->Des ( );
                CommonPhoneParser::ParsePhoneNumber (phonePtr,
                        CommonPhoneParser::EContactCardNumber );
                break;
                }
            case CFindItemEngine::EFindItemSearchURLBin:
                {
                type = CESMRRichTextLink::ETypeURL;
                break;
                }
            default:
                {
                break;
                }
            }

        if (valueBuf->Length ( )> 0 && type >= 0 )
            {
            CESMRRichTextLink* link = CESMRRichTextLink::NewL (item.iStartPos,
                    item.iLength, *valueBuf, type,
                    CESMRRichTextLink::ETriggerKeyRight );
            CleanupStack::PushL( link );
            AddLinkL( link );
            CleanupStack::Pop( link );
            }

        CleanupStack::PopAndDestroy (valueBuf );
        }

    // T
    HandleTextChangedL();

    CleanupStack::PopAndDestroy (itemEngine );
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::CopyCurrentLinkToClipBoardL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::CopyCurrentLinkToClipBoardL() const
    {
    FUNC_LOG;
    const CESMRRichTextLink* link = GetSelectedLink();
    HBufC* clipBoardText = NULL;

    if ( link )
    	{
    	clipBoardText = GetLinkTextLC( *link );
    	}

    if ( clipBoardText )
        {
        CClipboard* cb =
        CClipboard::NewForWritingLC( CCoeEnv::Static()->FsSession() );
        cb->StreamDictionary().At( KClipboardUidTypePlainText );
        CPlainText* plainText = CPlainText::NewL();
        CleanupStack::PushL( plainText );
        plainText->InsertL( 0 , *clipBoardText);
        plainText->CopyToStoreL( cb->Store(),
        		cb->StreamDictionary(), 0, plainText->DocumentLength());
        CleanupStack::PopAndDestroy( plainText );
        cb->CommitL();
        CleanupStack::PopAndDestroy( cb );
        CleanupStack::PopAndDestroy( clipBoardText );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ResetActionMenu
// -----------------------------------------------------------------------------
EXPORT_C void CESMRRichTextViewer::ResetActionMenuL() const // codescanner::LFunctionCantLeave
    {
    FUNC_LOG;
    if ( iCntMenuHdlr )
        {
        iCntMenuHdlr->Reset();
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetEventObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetEventQueue(
        MESMRFieldEventQueue* aEventQueue )
    {
    FUNC_LOG;
    iEventQueue = aEventQueue;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::LinkSelectedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CESMRRichTextViewer::LinkSelectedL()
    {
    FUNC_LOG;
    TBool linkSelected = EFalse;

    const CESMRRichTextLink* link = GetSelectedLink();
    if ( link )
        {
        switch ( link->TriggerKey() )
            {
            case CESMRRichTextLink::ETriggerKeyRight:
                {
                if ( !iLinkObserver ||
                     !iLinkObserver->HandleRichTextLinkSelection(link) )
                    {
                    ShowContextMenuL();
                    }
                linkSelected = ETrue;
                break;
                }
            case CESMRRichTextLink::ETriggerKeyOk:
                {
                if (iLinkObserver &&
                    iLinkObserver->HandleRichTextLinkSelection( link ) )
                    {
                    linkSelected = ETrue;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }

    return linkSelected;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::ProcessCommandL( TInt aCommandId )
    {
    FUNC_LOG;
    if ( iEventQueue )
        {
        // Forward commands from contact menu handler as command events.
        // Contact menu handler has the command observer for binary
        // compatibility reason.
        CESMRFieldCommandEvent* event =
            CESMRFieldCommandEvent::NewLC( NULL, aCommandId );
        iEventQueue->NotifyEventL( *event );
        CleanupStack::PopAndDestroy( event );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ActivateL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::ActivateL()
    {
    FUNC_LOG;
    CEikRichTextEditor::ActivateL();

    // CEikEdwin::ActivateL removes selection, re-set highlight
    // if focused and there's a selected link.
    const CESMRRichTextLink* link = GetSelectedLink();
    if ( link && IsFocused() )
        {
        HighlightLinkL( *link );
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ContactActionQueryComplete
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::ContactActionQueryComplete()
    {
    FUNC_LOG;
    if ( iOpenActionMenu )
        {
        // Activate link as actions have been discovered
        TRAP_IGNORE(  LinkSelectedL() );
        }

    // Reset menu observer
    iCntMenuHdlr->SetContactMenuObserver( NULL );
    iOpenActionMenu = EFalse;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ShowContextMenuL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::ShowContextMenuL()
    {
    FUNC_LOG;
    iOpenActionMenu = EFalse;
    ProcessCommandL( EAknSoftkeyContextOptions );
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::GetLinkAreaL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::GetLinkAreaL(
        TRegion& aRegion,
        const CESMRRichTextLink& aLink ) const
    {
    FUNC_LOG;
    TTmDocPosSpec posSpec;
    TTmPosInfo2 posInfo;

    posSpec.iPos = aLink.StartPos();
    posSpec.iType = TTmDocPosSpec::ELeading;

    aRegion.Clear();

    if ( TextView()->FindDocPosL( posSpec, posInfo ) )
        {
        TRect linkRect( 0, 0, 0, 0 );

        // Create link surrounding rectangle
        HBufC* text = GetLinkTextLC( aLink );
        TInt textWidth = AknBidiTextUtils::MeasureTextBoundsWidth(
                *iFont,
                *text,
                CFont::TMeasureTextInput::EFVisualOrder );

        TPoint tl( posInfo.iEdge );

        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // move top left x to end of text
            tl.iX -= textWidth;
            }

        tl.iY -= iFont->FontMaxAscent();
        TPoint br( tl.iX + textWidth, tl.iY + iFont->FontMaxHeight() );

        TRect rect( Rect() );

        while ( ( tl.iX < rect.iTl.iX || br.iX > rect.iBr.iX )
                && !aRegion.CheckError() )
            {
            // Link on multiple lines

            tl.iX = Max( rect.iTl.iX, tl.iX );
            br.iX = Min( br.iX, rect.iBr.iX );
            linkRect.SetRect( tl, br );
            aRegion.AddRect( linkRect );

            TPtrC remainder( text->Mid( posSpec.iPos - aLink.StartPos() ) );
            TInt numChars = iFont->TextCount( remainder,
                                              linkRect.Width() );
            posSpec.iPos += numChars;
            remainder.Set( remainder.Mid( numChars) );

            if ( TextView()->FindDocPosL( posSpec, posInfo ) )
                {
                textWidth = AknBidiTextUtils::MeasureTextBoundsWidth(
                                *iFont,
                                remainder,
                                CFont::TMeasureTextInput::EFVisualOrder );

                tl = posInfo.iEdge;

                if ( AknLayoutUtils::LayoutMirrored() )
                    {
                    // move top left x to end of text
                    tl.iX -= textWidth;
                    }

                tl.iY -= iFont->FontMaxAscent();
                br.SetXY( tl.iX + textWidth, tl.iY + iFont->FontMaxHeight() );
                }
            }

        linkRect.SetRect( tl, br );
        aRegion.AddRect( linkRect );

        CleanupStack::PopAndDestroy( text );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ChangeMiddleSoftkeyL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::ChangeMiddleSoftkeyL( const CESMRRichTextLink& aLink )
    {
    FUNC_LOG;
    CESMRField* field = static_cast< CESMRField* >( Parent() );

    if ( field )
        {
        field->ChangeMiddleSoftKeyL( aLink.MSKCommand(), aLink.MSKText() );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::UpdateViewL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::UpdateViewL( const TKeyEvent &aKeyEvent )
    {
    FUNC_LOG;
    const CESMRRichTextLink* selectedLink = GetSelectedLink();

    if( !selectedLink )
        {
        return;
        }

    RRegion linkRegion;
    CleanupClosePushL( linkRegion );

    GetLinkAreaL( linkRegion, *selectedLink );
    TRect linkRect = linkRegion.BoundingRect();

    CleanupStack::PopAndDestroy( &linkRegion );

    TRect viewableAreaRect = iObserver->ViewableAreaRect();


    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyUpArrow:
            {
            if( linkRect.iTl.iY < viewableAreaRect.iTl.iY )
                {
                // Move fields down
                iObserver->RePositionFields(
                        -( linkRect.iTl.iY - viewableAreaRect.iTl.iY ) );
                }

            break;
            }
        case EKeyRightArrow:
        case EKeyDownArrow:
            {
            if( linkRect.iBr.iY > viewableAreaRect.iBr.iY )
                {
                // Move fields up
                iObserver->RePositionFields(
                        -( linkRect.iBr.iY - viewableAreaRect.iBr.iY ) );
                }
            break;
            }
        default:
            break;
        }
    }

// EOF

