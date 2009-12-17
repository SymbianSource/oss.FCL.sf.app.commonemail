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
* Description:  ESMR editor impl.
*
*/


#include "emailtrace.h"
#include <txtfrmat.h>
#include <txtglobl.h>
#include <txtrich.h>
#include <gulfont.h>
#include <eikenv.h>
#include <avkon.hrh>

#include "cesmreditor.h"
#include "cesmrlayoutmgr.h"
#include "esmrfieldbuilderdef.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMREditor::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMREditor* CESMREditor::NewL( )
    {
    FUNC_LOG;
    /*
     Default parameters for edwin. Note that as the parent
     is NULL, the edwin MUST be EOwnsWindow-type edwin.
     */
    return CESMREditor::NewL (
		NULL, // parent
		1, // number of lines
		KTextLimit, // text limit
		CEikEdwin::EResizable | CEikEdwin::EOwnsWindow );
    }

// ---------------------------------------------------------------------------
// CESMREditor::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMREditor* CESMREditor::NewL(
		const CCoeControl* aParent, TInt aNumberOfLines,
        TInt aTextLimit, TInt aEdwinFlags )
    {
    FUNC_LOG;
    CESMREditor* self = new (ELeave) CESMREditor();
    CleanupStack::PushL ( self );
    self->ConstructL ( aParent, aNumberOfLines, aTextLimit, aEdwinFlags );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMREditor::~CESMREditor
// ---------------------------------------------------------------------------
//
EXPORT_C CESMREditor::~CESMREditor( )
    {
    FUNC_LOG;
    delete iDefaultText;
    }

// ---------------------------------------------------------------------------
// CESMREditor::PositionChanged
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREditor::PositionChanged( )
    {
    FUNC_LOG;
    CTextView* view = TextView();
    if ( view )
        {
        view->SetViewRect ( Rect() );
        }
    }

// ---------------------------------------------------------------------------
// CESMREditor::OfferKeyEventL
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CESMREditor::OfferKeyEventL(
		const TKeyEvent& aKeyEvent,TEventCode aType )
    {
    FUNC_LOG;

    TKeyResponse ret = CEikRichTextEditor::OfferKeyEventL(aKeyEvent, aType);

    if ( aType == EEventKey )
        {
        TPoint firstRowExtra(0,RowHeight());
        TextLayout()->DocPosToXyPosL( 0, firstRowExtra);
        //firstRowExtra.iY
        TPoint rel(PositionRelativeToScreen());
        TPoint point(0,0);
        TextLayout()->DocPosToXyPosL( CursorPos(), point);

        // +1 because first line is zero
        TInt currentLineNumber = TextLayout()->GetLineNumber( CursorPos() ) + 1;
        TInt cursorLowerPosition = currentLineNumber * RowHeight();
        TInt cursorUpperPosition = ( currentLineNumber -1 )* RowHeight();

        if ( aKeyEvent.iCode == EKeyDownArrow && iObserver)
            {
            TPoint firstRowExtra(0,RowHeight());
            TextLayout()->DocPosToXyPosL( 0, firstRowExtra);
            TPoint cursorPos(0,0);
            TextLayout()->DocPosToXyPosL( CursorPos(), cursorPos);
            TInt edwinTlY = Position().iY;
            TInt listHeight = iObserver->ListHeight();

            // edwinTlY negative
            if ( cursorLowerPosition > ( listHeight - edwinTlY )) 
                {
                iObserver->MoveListAreaUpL(RowHeight());
                }
            }
        else if ( aKeyEvent.iCode == EKeyUpArrow && iObserver)
            {
            TPoint firstRowExtra(0,RowHeight());
            TextLayout()->DocPosToXyPosL( 0, firstRowExtra);
            TPoint cursorPos(0,0);
            TextLayout()->DocPosToXyPosL( CursorPos(), cursorPos);
            TInt hiddenHeight = Position().iY;
            // upper corner of field not visible
            if ( hiddenHeight < 0 )
                {
                if ( (cursorPos.iY - firstRowExtra.iY * 2) < (-hiddenHeight))
                    {
                    iObserver->MoveListAreaDownL(RowHeight());
                    }
                }
            }
        }


    return ret;
    }

// ---------------------------------------------------------------------------
// CESMREditor::ClearSelectionAndSetTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREditor::FocusChanged(TDrawNow aDrawNow)
    {
    FUNC_LOG;
    CEikRichTextEditor::FocusChanged( aDrawNow );
    TRAPD( error, TryToSetSelectionL() );
    if ( error != KErrNone )
        {
        CEikonEnv::Static()->HandleError( error );// codescanner::eikonenvstatic
        }
    }

// ---------------------------------------------------------------------------
// CESMREditor::TryToSetSelectionL
// ---------------------------------------------------------------------------
//
void CESMREditor::TryToSetSelectionL()
    {
    FUNC_LOG;
    if ( IsFocused() )
        {
        // set selection to text if it is the default text
        HBufC* text = GetTextInHBufL();
        if ( text )
            {
            CleanupStack::PushL( text );
            if ( iDefaultText->Compare( *text ) == KErrNone )
                {
                SetSelectionL(0, text->Length() );
                }
            CleanupStack::PopAndDestroy( text );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMREditor::HandleEdwinEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREditor::HandleEdwinEventL(
		CEikEdwin* aEdwin,TEdwinEvent aEventType )
    {
    FUNC_LOG;
    if ( aEdwin == this && aEventType == EEventNavigation && iObserver)
        {
        TInt curPos = CursorPos ();
        TInt textLength = TextLength();
        // Update viewarea scrolling in cases where the cursor location 
        // is changed directly from bottom to upmost position or vise versa.
        if ( CursorPos () == TextLength() )
            {
            // KErrNotFound as field id is interpret as focused field:
            iObserver->ScrollItemVisible( KErrNotFound );
            }
        if ( CursorPos () == 0 )
            {
            // KErrNotFound as field id is interpret as focused field:
            iObserver->ScrollItemVisible( KErrNotFound );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMREditor::ClearSelectionAndSetTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREditor::ClearSelectionAndSetTextL( const TDesC& aText )
    {
    FUNC_LOG;
    ClearSelectionL();
    SetCursorPosL(0, EFalse );
    SetTextL( &aText );
    SetCursorPosL(0, EFalse );
    HandleTextChangedL();
    }

// ---------------------------------------------------------------------------
// CESMREditor::SetFontL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREditor::SetFontL( 
		const CFont* aFont, CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    const CFont* font = aFont;
    TFontSpec fontSpec = font->FontSpecInTwips();

    CParaFormat* paraFormat = CParaFormat::NewLC();
    TParaFormatMask paraFormatMask;
    paraFormat->iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels;

    paraFormatMask.SetAttrib( EAttLineSpacing );
    paraFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
    paraFormatMask.SetAttrib( EAttAlignment );

    TCharFormat charFormat;
    TCharFormatMask formatMask;
    charFormat.iFontSpec = fontSpec;

    formatMask.SetAttrib( EAttFontTypeface );
    formatMask.SetAttrib( EAttFontHeight );
    formatMask.SetAttrib( EAttFontPosture );
    formatMask.SetAttrib( EAttFontStrokeWeight );

    charFormat.iFontPresentation.iTextColor = aLayout->NormalTextColor();
    formatMask.SetAttrib( EAttColor );

    CParaFormatLayer* paraFormatLayer =
        CParaFormatLayer::NewL( paraFormat, paraFormatMask );
    CleanupStack::PushL( paraFormatLayer );
    CCharFormatLayer* charFormatLayer =
        CCharFormatLayer::NewL( charFormat, formatMask );

    SetParaFormatLayer( paraFormatLayer );
    CleanupStack::Pop( paraFormatLayer );
    CleanupStack::PopAndDestroy( paraFormat );
    SetCharFormatLayer( charFormatLayer );
    
    // This forces CEikEdwin::OnReformatL to notify observer
    // through HandleEdwinSizeEventL about changed size.
    // It is notified only if linecount changes.
    CEikEdwin::iNumberOfLines = 0;
    CEikEdwin::FormatTextL();
    }

// ---------------------------------------------------------------------------
// CESMREditor::CursorLinePos
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMREditor::CursorLinePos( ) const
    {
    FUNC_LOG;
    return TextLayout()->GetLineNumber ( CursorPos ( ) );
    }

// ---------------------------------------------------------------------------
// CESMREditor::LineCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMREditor::LineCount( ) const
    {
    FUNC_LOG;
    return TextLayout()->GetLineNumber ( TextLength ( ) );
    }

// ---------------------------------------------------------------------------
// CESMREditor::RowHeight
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMREditor::RowHeight()
    {
    FUNC_LOG;
    TInt bandHeight = TextLayout()->BandHeight();
    // Starts from zero, so let's increase the count by one.
    TInt lineNumberCount = TextLayout()->GetLineNumber(TextLength()) + 1;

    return ( bandHeight / lineNumberCount );
    }

// ---------------------------------------------------------------------------
// CESMREditor::CurrentLineNumber
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMREditor::CurrentLineNumber()
    {
    FUNC_LOG;
    // first line is zero, let's increase it by one
    return TextLayout()->GetLineNumber( CursorPos() ) + 1;
    }

// ---------------------------------------------------------------------------
// CESMREditor::LineCount
// ---------------------------------------------------------------------------
//
EXPORT_C  void CESMREditor::SetDefaultTextL(
        HBufC* aDefaultText ) // codescanner::nonconsthbufc
    {
    FUNC_LOG;
    SetTextL( aDefaultText );
    
    if ( iDefaultText )
        {
        delete iDefaultText;
        iDefaultText = NULL;
        }
    iDefaultText = aDefaultText;
    }

// ---------------------------------------------------------------------------
// CESMREditor::LineCount
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CESMREditor::DefaultText()
    {
    FUNC_LOG;
    return *iDefaultText;
    }

// ---------------------------------------------------------------------------
// CESMREditor::LineCount
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREditor::SetListObserver( MESMRListObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CESMREditor::CESMREditor
// ---------------------------------------------------------------------------
//
CESMREditor::CESMREditor( )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMREditor::ConstructL
// ---------------------------------------------------------------------------
//
void CESMREditor::ConstructL(const CCoeControl* aParent, TInt aNumberOfLines,
        TInt aTextLimit, TInt aEdwinFlags )
    {
    FUNC_LOG;
    CEikRichTextEditor::ConstructL (aParent, aNumberOfLines, aTextLimit,
            aEdwinFlags );

    AddEdwinObserverL(this);

    SetUpperFullFormattingLength( aTextLimit );

    iLimitLength = aTextLimit;
    
    SetAlignment ( CESMRLayoutManager::IsMirrored ( ) ? EAknEditorAlignRight
            : EAknEditorAlignLeft );
    SetHighlightStyleL ( EEikEdwinHighlightLink );
    }

// ---------------------------------------------------------------------------
// CESMREditor::GetLimitLength
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMREditor::GetLimitLength() const
	{
    FUNC_LOG;
	return iLimitLength;
	}

//EOF
