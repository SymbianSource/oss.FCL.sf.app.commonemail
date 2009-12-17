/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "cesmrrichtextviewer.h"

#include "mesmrlistobserver.h"// SCROLLING_MOD: List observer header
#include "esmrconfig.hrh"
#include "esmrhelper.h"
#include "cesmriconfield.h"
#include "cesmrfieldcommandevent.h"
#include "mesmrfieldeventqueue.h"
#include "cesmrlayoutmgr.h"
#include "cesmrrichtextlink.h"
#include "cesmrcontactmenuhandler.h"
#include "nmrbitmapmanager.h"

#include <esmrgui.rsg>
#include <commonphoneparser.h>
#include <finditemengine.h>
#include <txtrich.h>
#include <AknsUtils.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <baclipb.h> // for clipboard copy
#include <aknlongtapdetector.h>
#include <touchfeedback.h>

#ifndef FF_CMAIL_INTEGRATION
#include <txtclipboard.h>
#endif // FF_CMAIL_INTEGRATION

// DEBUG
#include "emailtrace.h"

// <cmail> Removed profiling. </cmail>

// Unnamed namespace for local definitions
namespace{ // codescanner::namespace

const TInt KArrowUpperMargin (2);
const TInt KArrowRightMargin (5);

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
    delete iActionMenuIcon;
    delete iActionMenuIconMask;
    iESMRStatic.Close ( );
    delete iLongTapDetector;
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

    if ( IsFocused() )
        {
        if( iLayout )
            {
            TRAP_IGNORE( SetFontColorL( ETrue ) );
            }

        if ( iCurrentLinkIndex == KErrNotFound )
            {
            TInt linkRow = KErrNotFound;
            if ( CursorPos() == 0 )
                {
                // focus is coming from field above:
                linkRow = FindTextLinkBetweenNextScrollArea(
                		0, 1, TCursorPosition::EFLineDown );
                }
            else
                {
                linkRow = FindTextLinkBetweenNextScrollArea(
                		LineCount()-2,
                        LineCount()-1,
                        TCursorPosition::EFLineUp);
                }
            if ( linkRow != KErrNotFound )
                {
                const CESMRRichTextLink* link = GetSelectedLink ( );
                if (link )
                    {
                    HighlightLink (*link );
                    DrawDeferred ( );
                    }
                }
            }
        else
            {
            const CESMRRichTextLink* link = GetSelectedLink ( );
            HighlightLink( *link );
            DrawDeferred();
            }
        }

    if (!IsFocused())
        {
        // losing focus
        // <cmail> codescanner
		TRAP_IGNORE(SetSelectionL(CursorPos(), CursorPos()));
        if( iLayout )
            {
            TRAP_IGNORE(SetFontColorL( EFalse ));
            }
        // </cmail>
        TRAP_IGNORE( TextView()->SetDocPosL( 0 ));
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

    iCurrentLinkIndex = KErrNotFound;
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


    if(iLinkList.Count() > aIndex)
	    {
        TInt pos = iLinkList[aIndex]->StartPos();

	    TInt checkRow = TextLayout()->GetLineNumber( pos );

	    if ( checkRow >= aStartRow && checkRow <= ( aEndRow ) )
	        {
	        if ( aDirection == TCursorPosition::EFLineDown &&
	             aIndex > iCurrentLinkIndex  ||
	             aDirection == TCursorPosition::EFLineUp &&
	             ( aIndex < iCurrentLinkIndex ||
	               iCurrentLinkIndex == KErrNotFound ))
	            {
	            iCurrentLinkIndex = aIndex;

	            // link found between next scroll area.
	            return checkRow;
	            }
	        }
	    }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetHighLightToNextLinkL
// -----------------------------------------------------------------------------
//
TBool CESMRRichTextViewer::SetHighLightToNextLinkL(
        TCursorPosition::TMovementType aDirection,
        TInt aStartRow,
        TInt aEndRow )
    {
    FUNC_LOG;
    TBool ret( EFalse );

    TInt currentLineNumber = TextLayout()->GetLineNumber( CursorPos() );

    // check is there a link before next scroll point:
    TInt linkRow = FindTextLinkBetweenNextScrollArea(
						aStartRow , aEndRow, aDirection);

    if ( linkRow != KErrNotFound && iCurrentLinkIndex != KErrNotFound )
        {
        const CESMRRichTextLink* link = GetSelectedLink ( );
        if (link )
            {
            HighlightLink (*link );

            if ( iObserver && linkRow != currentLineNumber )
                {
                TInt endOfLink = link->StartPos()+link->Length();
                TInt line = TextLayout()->GetLineNumber( endOfLink );
                if ( aDirection == TCursorPosition::EFLineUp ) // moving up:
                    {
                    iObserver->MoveListAreaDownL(
                            RowHeight() * ( currentLineNumber - line ));
                    }
                else
                    {
                    iObserver->MoveListAreaUpL(
                            RowHeight() * ( line - currentLineNumber ));
                    }
                }

            DrawDeferred ( );
            ret = ETrue;
            }
        }
    else
    	{
    	// no link are focused, do reset work to refresh the option menu.
    	ResetActionMenuL();
    	}

    return ret;
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
    // Handle only event keys
    if ( aType == EEventKey )
        {
        if ( iObserver ) // only description field has observer set.
            {
            // fetch the current line number:
            TInt currentLineNumber = TextLayout()->GetLineNumber( CursorPos() );

            if ( aKeyEvent.iCode == EKeyDownArrow && aType == EEventKey )
                {
                if ( !iObserver->IsFieldBottomVisible() &&
                     SetHighLightToNextLinkL(
                    		 TCursorPosition::EFLineDown,
                             currentLineNumber,
                             currentLineNumber + KMaxAddressFieldLines) )
                    {
                    return EKeyWasConsumed;
                    }
                else if ( iObserver->IsFieldBottomVisible() )
                    {
                    if ( SetHighLightToNextLinkL(TCursorPosition::EFLineDown,
                                            currentLineNumber,
                                            iNumberOfLines) )
                        {
                        return EKeyWasConsumed;
                        }
                    // means that the whole control is visible:
                    // and we can skip to next field
                    return EKeyWasNotConsumed;
                    }

                if ( currentLineNumber == iNumberOfLines )
                    {
                    // the end of text has been reached
                    return EKeyWasConsumed;
                    }

                // move three lines...
                ScrollViewL( KMaxAddressFieldLines,
							 TCursorPosition::EFLineDown);


                SetSelectionL( CursorPos(), CursorPos() );
                iCurrentLinkIndex = KErrNotFound;
                if ( iObserver )
                    {
                    iObserver->MoveListAreaUpL(
                    		RowHeight() * KMaxAddressFieldLines );
                    }

                return EKeyWasConsumed;
                }
            else if ( aKeyEvent.iCode == EKeyUpArrow && aType == EEventKey )
                {
                if (iPosition.iY < 0 &&
                    SetHighLightToNextLinkL(
                    		TCursorPosition::EFLineUp,
                            currentLineNumber - KMaxAddressFieldLines,
                            currentLineNumber))
                    {
                    return EKeyWasConsumed;
                    }
                if ( iPosition.iY > 0 )
                    {
                    // before changing the focus the field above,
                    // check is there any links in rest of text
                    if (SetHighLightToNextLinkL(TCursorPosition::EFLineUp,
                                        0,
                                        currentLineNumber))
                        {
                        return EKeyWasConsumed;
                        }

                    // means that the whole control is visible:
                    // and we can skip to next field
                    return EKeyWasNotConsumed;
                    }
                else
                    {
                    TInt currentLineNumber =
                    TextLayout()->GetLineNumber( CursorPos() );
                    // move three lines...
                    ScrollViewL( KMaxAddressFieldLines,
                    		     TCursorPosition::EFLineUp);

                    SetSelectionL( CursorPos(), CursorPos());
                    iCurrentLinkIndex = KErrNotFound;

                     if ( iObserver )
                        {
                        iObserver->MoveListAreaDownL(
                        		RowHeight() * KMaxAddressFieldLines );
                        }

                    currentLineNumber =
						TextLayout()->GetLineNumber( CursorPos() );
                    return EKeyWasConsumed;
                    }
                }
            }
        if ( aKeyEvent.iCode == EKeyRightArrow ||
        	 aKeyEvent.iCode == EKeyDevice3 ||
             aKeyEvent.iCode == EKeyDevice4 ||
             aKeyEvent.iCode == EKeyEnter )
            {
            // Show right click menu (action menu)
            const CESMRRichTextLink* link = GetSelectedLink();
            if (link &&
            	link->TriggerKey ( )== CESMRRichTextLink::ETriggerKeyRight )
                {
                if ( !iLinkObserver ||
                	 !iLinkObserver->HandleRichTextLinkSelection(link) )
                    {
                    iCntMenuHdlr->ShowActionMenuL();
                    }
                return EKeyWasConsumed;
                }
            }
        if ( aKeyEvent.iCode == EKeyLeftArrow )
            {
            const CESMRRichTextLink* link = GetSelectedLink();
            if ( link )
                {
                return EKeyWasConsumed;
                }
            }
        if ( aKeyEvent.iCode == EKeyDevice3 ||
        		aKeyEvent.iCode == EKeyDevice4 ||
        		aKeyEvent.iCode == EKeyEnter )
            {
            // Select link
            const CESMRRichTextLink* link = GetSelectedLink ( );
            if (link &&
            	link->TriggerKey ( )== CESMRRichTextLink::ETriggerKeyOk )
                {
                if (iLinkObserver &&
                	iLinkObserver->HandleRichTextLinkSelection (link ) )
                    {
                    return EKeyWasConsumed;
                    }
                }
            }
        }

    return EKeyWasNotConsumed;
    }


// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ScrollViewL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::ScrollViewL(
        TInt aNumberOfRows,
        TCursorPosition::TMovementType aDirection)
    {
    FUNC_LOG;
    for( TInt i=0; i < aNumberOfRows; i++ )
        {
        MoveCursorL( aDirection, EFalse);
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetMargins
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetMargins( TInt sMargin )
	{
	// Set new value for left and right margins
	if ( TextView() )
	    {
        TextView()->SetMarginWidths( sMargin ,0);
	    }
	}

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetFontL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetFontL( const CFont* aFont,
                                             CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    // These pointers are stored to able font color changing when losing or
    // gaining focus
    iLayout = aLayout;
    iFont = aFont;

    SetFontColorL( IsFocused() );

    // This forces CEikEdwin::OnReformatL to notify observer
    // through HandleEdwinSizeEventL about changed size.
    // It is notified only if linecount changes.
    CEikEdwin::iNumberOfLines = 0;
    CEikEdwin::FormatTextL();
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::SetFontColor
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::SetFontColorL( TBool aFocused )
    {
    FUNC_LOG;
    // all this stuff is needed to be set, otherwise the
    // font loses its antialiasing drawing

    TFontSpec fontSpec = iFont->FontSpecInTwips();

    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
    paraFormat.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels;

    paraFormatMask.SetAttrib( EAttLineSpacing );
    paraFormat.iHorizontalAlignment = CParaFormat::ELeftAlign;
    paraFormatMask.SetAttrib( EAttAlignment );

    TCharFormat charFormat;
    TCharFormatMask formatMask;
    charFormat.iFontSpec = fontSpec;

    formatMask.SetAttrib( EAttFontTypeface );
    formatMask.SetAttrib( EAttFontHeight );
    formatMask.SetAttrib( EAttFontPosture );
    formatMask.SetAttrib( EAttFontStrokeWeight );

    if( aFocused )
        {
        charFormat.iFontPresentation.iTextColor =
        iLayout->ViewerListAreaHighlightedTextColor();
        }
    else
        {
        charFormat.iFontPresentation.iTextColor = KRgbBlack;
        }
    formatMask.SetAttrib( EAttColor );

    CParaFormatLayer* paraFormatLayer =
        CParaFormatLayer::NewL( &paraFormat, paraFormatMask );
    CleanupStack::PushL( paraFormatLayer );

    CCharFormatLayer* charFormatLayer =
        CCharFormatLayer::NewL( charFormat, formatMask );

    SetParaFormatLayer( paraFormatLayer );
    SetCharFormatLayer( charFormatLayer );

    CleanupStack::Pop( paraFormatLayer );
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
    iCurrentLinkIndex = KErrNotFound;
    iLinkList.ResetAndDestroy ( );

    // Clear edwin text
    CEikEdwin::SetCursorPosL( 0, EFalse );

    // text lenght plus one to ensure the formatting
    // used is full, not band formatting.
    SetUpperFullFormattingLength( aText->Length() + 1 );

    // Set new edwin text
    CEikRichTextEditor::SetTextL( aText );

    //Make sure cursor is invisible and selection visible
    TextView()->SetCursorVisibilityL (TCursor::EFCursorInvisible,
            TCursor::EFCursorInvisible );
    TextView()->SetSelectionVisibilityL (ETrue );

    // Search text for links (highlights)
    if (aSearchLinks )
        {
        SearchLinksL( *aText );
        // find first link.
        FindTextLinkBetweenNextScrollArea(0, KMaxAddressFieldLines, TCursorPosition::EFLineDown);
        }

    // first row is 0, so let's add one...
    iNumberOfLines = TextLayout()->GetLineNumber( TextLength() ) + 1;
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

    RichText()->ApplyCharFormatL( iFormat,
                                  iFormatMask,
                                  aLink->StartPos(),
                                  aLink->Length() );

    // Append aLink to link list. Space has been reserved, so this cannot fail.
    iLinkList.AppendL( aLink );

    if ( highlight )
        {
        HighlightLink( *aLink );
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

    RichText()->ApplyCharFormatL( iFormat,
                                  iFormatMask,
                                  aLink->StartPos(),
                                  aLink->Length() );

    // Insert aLink to link list. Space has been reserved, so this cannot fail.
    // InsertL panics if aPosition is illegal.
    iLinkList.InsertL( aLink, aPosition );

    if ( highlight )
        {
        HighlightLink( *aLink );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::GetSelectedLink
// -----------------------------------------------------------------------------
//
EXPORT_C const CESMRRichTextLink* CESMRRichTextViewer::GetSelectedLink( ) const
    {
    FUNC_LOG;
    if (iCurrentLinkIndex >= 0 && iCurrentLinkIndex < iLinkList.Count ( ) )
        {
        return iLinkList[iCurrentLinkIndex];
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::GetLinkTextL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CESMRRichTextViewer::GetLinkTextL(
        const CESMRRichTextLink& aLink ) const
    {
    FUNC_LOG;
    return RichText()->Read( aLink.StartPos ( ), aLink.Length ( ) ).AllocL();
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
    return iNumberOfLines;
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
// CESMRRichTextViewer::CESMRRichTextViewer
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRRichTextViewer::SetActionMenuStatus( TBool aStatus )
    {
    FUNC_LOG;
    iActionMenuStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::CESMRRichTextViewer
// -----------------------------------------------------------------------------
//
CESMRRichTextViewer::CESMRRichTextViewer( )
:   iActionMenuStatus( ETrue ), iActionMenuOpen( EFalse )
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

    CEikRichTextEditor::ConstructL (aParent, 1, 1, flags );
    SetSuppressBackgroundDrawing (ETrue );

    User::LeaveIfError(
            NMRBitmapManager::GetSkinBasedBitmap(
                    NMRBitmapManager::EMRBitmapRightClickArrow, iActionMenuIcon,
                    iActionMenuIconMask, KIconSize ) );

    iESMRStatic.ConnectL ( );
    iCntMenuHdlr = &iESMRStatic.ContactMenuHandlerL();
    iCurrentLinkIndex = KErrNotFound;
    iFormatMask.SetAttrib( EAttFontUnderline );
    iFormat.iFontPresentation.iUnderline = EUnderlineOn;
    iFormatMask.SetAttrib( EAttColor );
    iFormat.iFontPresentation.iTextColor = KRgbBlue;

    iLongTapDetector = CAknLongTapDetector::NewL(this);
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::Draw
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;
    CEikEdwin::Draw( aRect );
    if (IsFocused ( ) && iActionMenuStatus )
        {
        const CESMRRichTextLink* link = GetSelectedLink ( );
        if (link && link->TriggerKey ( )== CESMRRichTextLink::ETriggerKeyRight )
            {
            TRAP_IGNORE(DrawRightClickIconL(*link));
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::DrawRightClickIconL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::DrawRightClickIconL(
		const CESMRRichTextLink& aLink ) const
    {
    FUNC_LOG;
    TTmDocPosSpec posSpec;
    TTmPosInfo2 posInfo;

    posSpec.iPos = aLink.StartPos ( )+ aLink.Length ( );
    posSpec.iType = TTmDocPosSpec::ETrailing;

    if (TextView()->FindDocPosL (posSpec, posInfo ) )
        {
        TPoint pt = posInfo.iEdge;
        pt -= iActionMenuIcon->SizeInPixels ( );

        pt.iY = pt.iY + KArrowUpperMargin;
        pt.iX = Parent()->Rect().iBr.iX -
                iActionMenuIcon->SizeInPixels().iWidth - KArrowRightMargin;
        TRect dst(pt, iActionMenuIcon->SizeInPixels ( ));
        TRect src(TPoint (0, 0 ), iActionMenuIcon->SizeInPixels ( ));

        CWindowGc& gc = SystemGc ( );
        gc.DrawBitmapMasked (dst, iActionMenuIcon, src, iActionMenuIconMask,
                EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::HighlightLink
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::HighlightLink(const CESMRRichTextLink& aLink )
    {
    FUNC_LOG;
    TInt error = KErrNone;
    TCursorSelection selection( aLink.StartPos(), aLink.StartPos() + aLink.Length() );

    // If TextView is not constructed yet.
    if ( !TextView() )
        {
        TRAP( error, SetSelectionL( selection.iCursorPos, selection.iAnchorPos ) );
        }
    else
        {
        TextView()->SetPendingSelection( selection );
        }

    if ( error == KErrNone )
        {
        TRAP( error, SetValueL( aLink ) );
        if ( error != KErrNone )
            {
            CEikonEnv::Static()->// codescanner::eikonenvstatic
                HandleError( error );
            }
        }
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
            // Set this as command observer.
            // Commands from contact menu handler are
            // processed in ProcessCommandL and forwarded
            // as field command events
            // <cmail>
            //iCntMenuHdlr->SetCommandObserver( this );
            // </cmail>
            iCntMenuHdlr->SetValueL( aLink.Value(),
            		CESMRContactMenuHandler::EValueTypeAttachment );
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
EXPORT_C void CESMRRichTextViewer::CopyCurrentLinkToClipBoardL() const
    {
    FUNC_LOG;
    const CESMRRichTextLink* link = GetSelectedLink();

    if( link )
        {
    	HBufC* clipBoardText = GetLinkTextL(*link);

    	if ( clipBoardText )
    	    {
    	    CleanupStack::PushL( clipBoardText );
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
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::CopyCurrentLinkValueToClipBoardL
// -----------------------------------------------------------------------------
EXPORT_C void CESMRRichTextViewer::CopyCurrentLinkValueToClipBoardL() const
    {
    FUNC_LOG;
    const CESMRRichTextLink* link = GetSelectedLink();

    if( link )
        {
    	TDesC& clipBoardText = link->Value();

	    CClipboard* cb =
        CClipboard::NewForWritingLC( CCoeEnv::Static()->FsSession() );
        cb->StreamDictionary().At( KClipboardUidTypePlainText );
        CPlainText* plainText = CPlainText::NewL();
        CleanupStack::PushL( plainText );
        plainText->InsertL( 0 , clipBoardText);
        plainText->CopyToStoreL( cb->Store(),
                cb->StreamDictionary(), 0, plainText->DocumentLength());
        CleanupStack::PopAndDestroy( plainText );
        cb->CommitL();
        CleanupStack::PopAndDestroy( cb );
        }
    }


// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ResetActionMenu
// -----------------------------------------------------------------------------
EXPORT_C void CESMRRichTextViewer::ResetActionMenuL() const // codescanner::LFunctionCantLeave
    {
    FUNC_LOG;
    if(iCntMenuHdlr)
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
        iActionMenuOpen = ETrue;
        switch ( link->TriggerKey() )
            {
            case CESMRRichTextLink::ETriggerKeyRight:
                {
                if ( !iLinkObserver ||
                     !iLinkObserver->HandleRichTextLinkSelection(link) )
                    {
                    iCntMenuHdlr->ShowActionMenuL();
                    }
                linkSelected = ETrue;
                break;
                }
            case CESMRRichTextLink::ETriggerKeyOk:
                {
                if (iLinkObserver &&
                    iLinkObserver->HandleRichTextLinkSelection (link ) )
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
        iActionMenuOpen = EFalse;
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

    // Make sure correct font color is in use.
    SetFontColorL( IsFocused() );

    // CEikEdwin::ActivateL removes selection, re-set highlight
    // if focused and there's a selected link.
    if ( IsFocused() && GetSelectedLink() )
        {
        HighlightLink( *GetSelectedLink() );
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TBool linkTapped( EFalse );
    CESMRRichTextLink* link = NULL;
    // fetch the current line number:
    TInt currentLineNumber = TextLayout()->GetLineNumber( CursorPos() );
    // find out tapped position
    TPoint touchPoint = aPointerEvent.iPosition;

    TInt tappedPos = TextView()->XyPosToDocPosL( touchPoint );
    for (TInt i = 0; i<iLinkList.Count(); i++)
        {
        link = iLinkList[ i ];
        TInt linkStart = link->StartPos();
        TInt linkEnd = link->StartPos() + link->Length() - 1;
        if ( tappedPos >= linkStart && tappedPos <= linkEnd )
            {
            // link tapped
            linkTapped = ETrue;
            iCurrentLinkIndex = i;
            break;
            }
        }

    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        if ( linkTapped )
            {
            // tactile feedback
            MTouchFeedback* feedback = MTouchFeedback::Instance();
            if ( feedback )
                {
                feedback->InstantFeedback( this, ETouchFeedbackBasic );
                }

            TPointerEvent pointerEvent = aPointerEvent;
            pointerEvent.iParentPosition = pointerEvent.iPosition
                + DrawableWindow()->AbsPosition();
            iLongTapDetector->PointerEventL( pointerEvent );
            HighlightLink( *link );
            if ( Parent() )
                {
                Parent()->DrawDeferred();
                }
            else
                {
                DrawDeferred();
                }
            }
        }
    else
        {
        TPointerEvent pointerEvent = aPointerEvent;
        pointerEvent.iParentPosition = pointerEvent.iPosition
            + DrawableWindow()->AbsPosition();
        iLongTapDetector->PointerEventL( pointerEvent );

        if ( aPointerEvent.iType  == TPointerEvent::EButton1Up )
            {
            if ( linkTapped && !iActionMenuOpen )
                {
                // tapped highlighted field, execute action
                LinkSelectedL();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::HandleLongTapEventL
// -----------------------------------------------------------------------------
//
void CESMRRichTextViewer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                              const TPoint& /*aPenEventScreenLocation*/ )
    {
    if ( !iActionMenuOpen )
        {
        LinkSelectedL();
        }
    }
// EOF

