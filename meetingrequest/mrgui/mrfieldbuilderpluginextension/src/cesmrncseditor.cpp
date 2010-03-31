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
* Description: This file implements class CESMRNcsEditor.
*
*/

#include "emailtrace.h"
#include "cesmrncseditor.h"
#include "nmrcolormanager.h"

#include <avkon.hrh>
#include <aknenv.h>         // CAknEnv
#include <txtrich.h>
#include <aknutils.h>       // AknLayoutUtils
#include <txtglobl.h>

#include <aknsconstants.h>
#include <aknsutils.h>
#include <aknsskininstance.h>
#include <aknsbasicbackgroundcontrolcontext.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CESMRNcsEditor::CESMRNcsEditor( HBufC* aDefaultText ) :
    iPreviousFontHeight( -1 ),
    iPreviousLineSpacingInTwips( -1 ),
    iDefaultText( aDefaultText )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
CESMRNcsEditor::~CESMRNcsEditor()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::GetLineRectL() const
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::GetLineRectL( TRect& aLineRect ) const
    {
    FUNC_LOG;
    TPoint position;
    iLayout->DocPosToXyPosL( CursorPos(), position );
    iLayout->GetLineRect( position.iY, aLineRect );
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::CursorLineNumber() const
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::CursorLineNumber() const
    {
    FUNC_LOG;
    TInt ret = iLayout->GetLineNumber( CursorPos() );
    ret++;
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::CursorPositionL() const
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::CursorPositionL() const
    {
    FUNC_LOG;
    // Return the position of the cursor relative
    // to the top of the control (in pixels)
    TPoint position;
    TInt cp = CursorPos();
    iLayout->DocPosToXyPosL( cp , position );
    position.iY += iLayout->PixelsAboveBand();
    return position.iY;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::OfferKeyEventL() const
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRNcsEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret( EKeyWasNotConsumed );
    if( aKeyEvent.iCode == EKeyDelete && CursorPos() == TextLength() )
        {
        ClearSelectionL();
        return EKeyWasConsumed;
        }
    else
        {
        ret = CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::SetTextL
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::SetTextL( const TDesC* aDes )
    {
    FUNC_LOG;
    CEikEdwin::SetTextL( aDes );
    if ( ( *aDes ).Compare( KNullDesC ) == 0 )
        {
        // when text is emptied, set text color again
        UpdateColors();
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::FocusChanged
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::FocusChanged( TDrawNow aDrawNow )
    {
    CEikRichTextEditor::FocusChanged( aDrawNow );
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( Rect().Contains( aPointerEvent.iPosition ) )
        {
        switch ( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Down:
            case TPointerEvent::EButton1Up:
            	{
                CEikRichTextEditor::HandlePointerEventL( aPointerEvent );
                break;
            	}
            default:
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CEikEdwin::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange )
        {
        UpdateGraphics();
        UpdateColors();
        }
    else if ( aType == KEikMessageColorSchemeChange )
        {
        UpdateColors();
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::SetupEditorL()
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::SetupEditorL()
    {
    FUNC_LOG;
    // Set the cursor position to initialize the edit control
    SetCursorPosL(0,EFalse);

    UpdateColors();
    UpdateGraphics();
    UpdateEditorL();
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::UpdateEditorL()
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::UpdateEditorL()
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::LineCount() const
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::LineCount() const
    {
    FUNC_LOG;
    TInt lineCount = iLayout->GetLineNumber( TextLength() );
    lineCount++;
    return lineCount;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::ScrollableLines() const
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::ScrollableLines() const
    {
    FUNC_LOG;
    TRect lineRect;
    // First get the rect for one line
    iLayout->GetLineRect(0, lineRect);
    TRect rect = Rect();
    TInt lines = rect.Height()/lineRect.Height();
    return lines;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::PositionChanged()
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::PositionChanged()
    {
    FUNC_LOG;

    if( iTextView )
        {
        TRect displayRect=iBorder.InnerRect(Rect());
        displayRect=iMargins.InnerRect(displayRect);
        iTextView->SetViewRect(displayRect);
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::DebugDump() const
// Is is meant for debugging.
// Break here to see what the values are.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CESMRNcsEditor::DebugDump()
    {
    FUNC_LOG;
    TInt aBandHeight = iLayout->BandHeight();
    TInt aFirstDocPosFullyInBad = iLayout->FirstDocPosFullyInBand();
    TInt aFirstLineInBand = iLayout->FirstLineInBand();
    TInt aPixelsAboveBand = iLayout->PixelsAboveBand();
    }
#endif

// -----------------------------------------------------------------------------
// CESMRNcsEditor::SetTextColorL()
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::SetTextColorL( TLogicalRgb aColor )
    {
    FUNC_LOG;
    TCharFormat charFormat;
    charFormat.iFontPresentation.iTextColor = aColor;
    TCharFormatMask charFormatMask;
    charFormatMask.SetAttrib( EAttColor );
    ApplyCharFormatL( charFormat, charFormatMask );
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::HasDefaultText()
// -----------------------------------------------------------------------------
//
TBool CESMRNcsEditor::HasDefaultText()
    {
    FUNC_LOG;
    EditObserver(0,2);

    return iHasDefaultText;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::TrimmedTextLengthL()
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::TrimmedTextLengthL()
    {
    FUNC_LOG;
    HBufC* text = GetTextInHBufL();
    TInt length = 0;
    if ( text )
        {
        CleanupStack::PushL( text );
        text->Des().TrimAll();
        length = text->Des().Length();
        CleanupStack::PopAndDestroy( text );
        }
    return length;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::EditObserver()
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::EditObserver( TInt aStart, TInt aExtent )
    {
    FUNC_LOG;
    CEikRichTextEditor::EditObserver( aStart, aExtent );

    HBufC* text = NULL;
    TRAPD( error, text = GetTextInHBufL());
    if (error!= KErrNone)
        {
        iCoeEnv->HandleError(error);
        }

    // if we are adding or removing multiple characters check
    // if we have default text
    if ( iDefaultText && ( aExtent > 1 || aExtent < -1 ) )
        {
        if ( text )
            {
            text->Des().TrimAll();
            iHasDefaultText =
                ( ( *text ).Compare( *iDefaultText ) == 0 ) ? ETrue : EFalse;
            }
        else
            {
            // text reseted
            iHasDefaultText = EFalse;
            }
        }
    delete text;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::UpdateColors
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::UpdateColors()
    {
    // Nothing to do.
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::UpdateGraphics
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::UpdateGraphics()
    {
    FUNC_LOG;
    TRAP_IGNORE( DoUpdateGraphicsL() );
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::DoUpdateGraphicsL
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::DoUpdateGraphicsL()
    {
    // Nothing to do.
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::GetLabelText
// -----------------------------------------------------------------------------
//
const TDesC& CESMRNcsEditor::GetLabelText() const
    {
    FUNC_LOG;
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::GetNumChars
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::GetNumChars() const
    {
    FUNC_LOG;
    TInt cnt = iLayout->DocumentLength();
    return cnt;
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::GetLineHeightL
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::GetLineHeightL() const
    {
    FUNC_LOG;
    TPoint pos;
    TRect rect;
    iLayout->DocPosToXyPosL(0, pos);
    iLayout->GetLineRect(pos.iY,rect);
    return rect.Height();
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::DocumentLength
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::DocumentLength() const
    {
    FUNC_LOG;
    return iLayout->DocumentLength();
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::PixelsAboveBand
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::PixelsAboveBand() const
    {
    FUNC_LOG;
    return iLayout->PixelsAboveBand();
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::ChangeBandTopL
// -----------------------------------------------------------------------------
//
TInt CESMRNcsEditor::ChangeBandTopL(TInt aPixels)
    {
    FUNC_LOG;
    TInt px = aPixels;
    return iLayout->ChangeBandTopL(px);
    }

// -----------------------------------------------------------------------------
// CESMRNcsEditor::SetFontL
// -----------------------------------------------------------------------------
//
void CESMRNcsEditor::SetFontL( const CFont* aFont )
    {
    FUNC_LOG;
    const CFont* font = aFont;
    TFontSpec fontSpec = font->FontSpecInTwips();

    CParaFormat *paraFormat = CParaFormat::NewLC();
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
    formatMask.SetAttrib(EAttFontHighlightColor);
    formatMask.SetAttrib( EAttColor );

    charFormat.iFontPresentation.iTextColor = 
           NMRColorManager::Color( NMRColorManager::EMRMainAreaTextColor );

    charFormat.iFontPresentation.iHighlightColor =  
           NMRColorManager::Color( NMRColorManager::EMRCutCopyPasteHighlightColor );

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

//EOF

