/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file implements class CNcsEditor.
*
*/



#include "emailtrace.h"
#include <avkon.hrh>
#include <aknenv.h> 		// CAknEnv
#include <txtrich.h>
#include <txtglobl.h>
#include <AknUtils.h>		// AknLayoutUtils
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <aknedsts.h>
#include <AknsBasicBackgroundControlContext.h>

#include "ncseditor.h"
#include "ncsutility.h"
#include "ncscustomdraw.h"
#include "ncseditorcustomdraw.h"

// ========================= MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CNcsEditor::CNcsEditor( MNcsFieldSizeObserver* aSizeObserver,
                        TBool aHeaderField,
                        TNcsEditorUsage aEditorUsage,
                        const TDesC& aCaptionText ) :
    MNcsControl( aSizeObserver ),
    iPreviousFontHeight( -1 ),
    iPreviousLineSpacingInTwips( -1 ),
    // Platform layout change
    iHeaderField( aHeaderField ),
    iNcsFontType ( NcsUtility::ENcsHeaderDetailFont ),
    iEditorUsage( aEditorUsage ),
    iRealRect( 0, 0, 0, 0 ),
    iCaptionText( aCaptionText.Alloc() )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
//
void CNcsEditor::ConstructL( const CCoeControl* aParent,
                             TInt aNumberOfLines,
                             TInt aTextLimit )
    {
    FUNC_LOG;
    CEikRichTextEditor::ConstructL( aParent, aNumberOfLines, aTextLimit,
            CEikEdwin::EAvkonEditor | CEikEdwin::EResizable |
            CEikEdwin::ENoAutoSelection | CEikEdwin::EInclusiveSizeFixed |
            CEikEdwin::ENoHorizScrolling | CEikRichTextEditor::EPasteAsPlainText );
    
    iGlobalCharFormat = CCharFormatLayer::NewL();
    iGlobalCharFormat->SetBase( GlobalText()->GlobalCharFormatLayer() );
    GlobalText()->SetGlobalCharFormat( iGlobalCharFormat );
    }

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
CNcsEditor::~CNcsEditor()
    {
    FUNC_LOG;
    delete iGlobalCharFormat;
    delete iEditorCustomDrawer;
    delete iCaptionText;
    }

// -----------------------------------------------------------------------------
// CNcsEditor::OpenVirtualKeyBoardL()
// -----------------------------------------------------------------------------
//
void CNcsEditor::OpenVirtualKeyBoardL()
    {
    FUNC_LOG;
    MCoeFepAwareTextEditor_Extension1* fepAwareTextEditor( 
            InputCapabilities().FepAwareTextEditor()->Extension1() );
    CAknEdwinState* state( NULL );
    if ( fepAwareTextEditor )
        {
        state = static_cast<CAknEdwinState*>( 
                fepAwareTextEditor->State( KNullUid ) );
        }
    if (state)
        {
        state->ReportAknEdStateEventL( 
                MAknEdStateObserver::EAknActivatePenInputRequest );
        }
    }

// -----------------------------------------------------------------------------
// CNcsEditor::GetLineRectL() const
// -----------------------------------------------------------------------------
//
void CNcsEditor::GetLineRectL( TRect& aLineRect ) const
    {
    FUNC_LOG;
    TPoint position;
    iLayout->DocPosToXyPosL( CursorPos(), position );
    iLayout->GetLineRect( position.iY, aLineRect );
    }

// -----------------------------------------------------------------------------
// CNcsEditor::CursorLineNumber() const
// -----------------------------------------------------------------------------
//
TInt CNcsEditor::CursorLineNumber() const
    {
    FUNC_LOG;
    TInt ret = iLayout->GetLineNumber( CursorPos() );
    ret++;
    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsEditor::CursorPosition() const
// -----------------------------------------------------------------------------
//
TInt CNcsEditor::CursorPosition() const
    {
    FUNC_LOG;
	// Return the position of the cursor relative
	// to the top of the control (in pixels)
    TPoint position;
    TInt cp = CursorPos();
    TRAP_IGNORE( iLayout->DocPosToXyPosL( cp , position ) );
    position.iY += iLayout->PixelsAboveBand();
    return position.iY;
    }


// -----------------------------------------------------------------------------
// CNcsEditor::OfferKeyEventL() const
// -----------------------------------------------------------------------------
//
TKeyResponse CNcsEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
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
// CNcsEditor::SetTextL
// -----------------------------------------------------------------------------
//
void CNcsEditor::SetTextL( const TDesC* aDes )
    {
    FUNC_LOG;
    CEikEdwin::SetTextL( aDes );
    if ( ( *aDes ).Compare( KNullDesC ) == 0 )
        {
        // when text is emptied, set text color again
        SetTextColorL( iTextColor );
        UpdateFontSize();
        }
    }

// -----------------------------------------------------------------------------
// CNcsEditor::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CNcsEditor::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CEikRichTextEditor::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange )
        {
        UpdateFontSize();
        UpdateColors();
        if ( iCustomDrawer )
            {
            iCustomDrawer->UpdateColors();
            }
        }
    else if ( aType == KEikMessageColorSchemeChange )
        {
        UpdateFontSize();
        UpdateColors();
        if ( iCustomDrawer )
            {
            iCustomDrawer->UpdateColors();
            }
        }
    else if ( aType == KEikMessageZoomChange )
        {
        UpdateFontSize();
        }

    if ( iEditorCustomDrawer )
        {
        iEditorCustomDrawer->UpdateLayout( GetLayout() );
        }
    }

//<cmail> Platform layout change
// -----------------------------------------------------------------------------
// CNcsEditor::CreateCustomDrawL
// -----------------------------------------------------------------------------
//
/*
CLafEdwinCustomDrawBase* CNcsEditor::CreateCustomDrawL()
    {
    CLafEdwinCustomDrawBase* drawer = CEikEdwin::CreateCustomDrawL();
    iCustomDrawer = new ( ELeave ) CNcsCustomDraw(
        static_cast<CEikonEnv*>( ControlEnv() )->LafEnv(),
        *this,
        drawer,
        this,
        iHeaderField );
    iCustomDrawer->UpdateColors();
    return iCustomDrawer;
    }
*/    
// </cmail>

// -----------------------------------------------------------------------------
// CNcsEditor::SetupEditorL()
// -----------------------------------------------------------------------------
//
void CNcsEditor::SetupEditorL()
    {
    FUNC_LOG;
    // <cmail> Added text view creation.
    // In S60 5.0 we end up here before this control is "activated". Normally
    // text view is created in CEikGlobalTextEditor::ActivateL
    // Find out why this problem doesn't exist in Freestyle/S60 3.2.3.
    if ( !iTextView )
        {
        CreateTextViewL();
        }
    // </cmail>

    // <cmail> Platform layout change
    //TextLayout()->RestrictScrollToTopsOfLines( EFalse );
    // </cmail>

    // Set the cursor position to initialize the edit control
    SetCursorPosL(0,EFalse);
    UpdateFontSize();
    UpdateColors();
    }

// -----------------------------------------------------------------------------
// CNcsEditor::LineCount() const
// -----------------------------------------------------------------------------
//
TInt CNcsEditor::LineCount() const
    {
    FUNC_LOG;
    TInt lineCount = iLayout->GetLineNumber( TextLength() );
    lineCount++;
    return lineCount;
    }

TInt CNcsEditor::ScrollableLines() const
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
// CNcsEditor::PositionChanged() const
// -----------------------------------------------------------------------------
//
void CNcsEditor::PositionChanged()
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
// CNcsEditor::TextHeight() const
// -----------------------------------------------------------------------------
//
TInt CNcsEditor::TextHeight() const
    {
    FUNC_LOG;
    return iLayout->FormattedHeightInPixels();
    }

#ifdef _DEBUG
// Is is meant for debugging.
// Break here to see what the values are.
void CNcsEditor::DebugDump()
    {
    FUNC_LOG;
	TInt bandHeight = iLayout->BandHeight();
	TInt firstDocPosFullyInBad = iLayout->FirstDocPosFullyInBand();
	TInt firstLineInBand = iLayout->FirstLineInBand();
	TInt pixelsAboveBand = iLayout->PixelsAboveBand();
    }
#endif

// -----------------------------------------------------------------------------
// CNcsEditor::SetTextColorL()
// -----------------------------------------------------------------------------
//
void CNcsEditor::SetTextColorL( TLogicalRgb aColor )
	{
    FUNC_LOG;
    TCharFormat charFormat;
    charFormat.iFontPresentation.iTextColor = aColor;
    TCharFormatMask charFormatMask;
    charFormatMask.SetAttrib( EAttColor );
    iGlobalCharFormat->SetL( charFormat, charFormatMask );
    GlobalText()->SetGlobalCharFormat( iGlobalCharFormat );
    NotifyNewFormatL();
    }

TBool CNcsEditor::HasSemicolonAtTheEndL() const
{
    HBufC* text = GetTextInHBufL();
    
    TBool result(EFalse);
    if ( text && text->Right(1).Compare(_L(";") ) == 0 )
        {
        result = ETrue;
        }
    delete text;
    text = NULL;
    
    return result;
}

// -----------------------------------------------------------------------------
// CNcsEditor::TrimmedTextLength()
// -----------------------------------------------------------------------------
//
TInt CNcsEditor::TrimmedTextLength()
    {
    FUNC_LOG;
    HBufC* text = NULL;
    TRAP_IGNORE( text = GetTextInHBufL() );
    TInt length = 0;
    if ( text )
        {
        text->Des().TrimAll();
        length = text->Des().Length();
        delete text;
        }
    return length;
    }

// -----------------------------------------------------------------------------
// CNcsEditor::EditObserver()
// -----------------------------------------------------------------------------
//
void CNcsEditor::EditObserver( TInt aStart, TInt aExtent )
    {
    FUNC_LOG;
    CEikRichTextEditor::EditObserver( aStart, aExtent );
    }

// -----------------------------------------------------------------------------
// CNcsEditor::UpdateColors
// -----------------------------------------------------------------------------
//
void CNcsEditor::UpdateColors()
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	// <cmail> S60 Skin support
	const TInt error( AknsUtils::GetCachedColor( skin, iTextColor,
    	KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) );
    if ( error )
    	{
        iTextColor = KRgbBlack;
        }

    // </cmail>
    TRAP_IGNORE( SetTextColorL( iTextColor ) );
    }

// <cmail>
// -----------------------------------------------------------------------------
// CNcsEditor::SetNcsFontType
// -----------------------------------------------------------------------------
//
void CNcsEditor::SetNcsFontType( NcsUtility::TNcsLayoutFont aFontType )
    {
    FUNC_LOG;
    iNcsFontType = aFontType;
    }

// -----------------------------------------------------------------------------
// CNcsEditor::UpdateFontSize
// -----------------------------------------------------------------------------
//
void CNcsEditor::UpdateFontSize()
    {
    FUNC_LOG;
    /*TCharFormat charFormat;
    TCharFormatMask charFormatMask;

    TNcsMeasures m = NcsUtility::Measures();

    // <cmail>
    CFont* editorFont = NULL;

    TRAPD( err, editorFont = NcsUtility::GetNearestFontL(
    	EAknLogicalFontPrimarySmallFont,
    	m.iEditorFontHeightPx)
    	);

    if( err == KErrNone )
    	{
    	*/

    TRAP_IGNORE( UpdateFontL() );
//    CEikonEnv::Static()->ScreenDevice()->ReleaseFont( editorFont );
//	}
    }

// -----------------------------------------------------------------------------
// CNcsEditor::UpdateFontL
// -----------------------------------------------------------------------------
//
void CNcsEditor::UpdateFontL()
    {
	FUNC_LOG;
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontSpec = NcsUtility::GetLayoutFont( Rect(), iNcsFontType )->FontSpecInTwips();
    charFormat.iFontPresentation.iTextColor = iTextColor;
    charFormatMask.SetAttrib(EAttColor);
    charFormatMask.SetAttrib(EAttFontTypeface);
    charFormatMask.SetAttrib(EAttFontHeight);
    iGlobalCharFormat->SetL(charFormat, charFormatMask);
    GlobalText()->SetGlobalCharFormat( iGlobalCharFormat );
    }

// ---------------------------------------------------------------------------
// CNcsEditor::GetLabelText
// ---------------------------------------------------------------------------
//
const TDesC& CNcsEditor::GetLabelText() const
    {
	return KNullDesC;
    }

// <cmail> Platform layout change
// ---------------------------------------------------------------------------
// CNcsEditor::LayoutLineCount
// ---------------------------------------------------------------------------
//
TInt CNcsEditor::LayoutLineCount() const
    {
    return IsVisible() ? LineCount() : 0;
    }
// </cmail> Platform layout change

// ---------------------------------------------------------------------------
// CNcsEditor::GetNumChars
// ---------------------------------------------------------------------------
//
TInt CNcsEditor::GetNumChars() const
    {
    FUNC_LOG;
	TInt cnt = iLayout->DocumentLength();
	return cnt;
    }

TInt CNcsEditor::GetLineHeightL() const
    {
    FUNC_LOG;
	TPoint pos;
	TRect rect;
	iLayout->DocPosToXyPosL(0, pos);
	iLayout->GetLineRect(pos.iY,rect);
	return rect.Height();
    }

TInt CNcsEditor::DocumentLength() const
    {
    FUNC_LOG;
	return iLayout->DocumentLength();
    }

TInt CNcsEditor::PixelsAboveBand() const
    {
    FUNC_LOG;
	return iLayout->PixelsAboveBand();
    }

TInt CNcsEditor::ChangeBandTopL(TInt aPixels)
    {
    FUNC_LOG;
	TInt px = aPixels;
	return iLayout->ChangeBandTopL(px);
    }

void CNcsEditor::ActivateL()
    {
    if ( IsActivated() )
        {
        return;
        }

    CEikRichTextEditor::ActivateL();
        
    if ( !iEditorCustomDrawer && iEditorUsage != ENcsEditorDefault )
        {
        TAknTextComponentLayout layout;
        if ( iEditorUsage == ENcsEditorBody )
            {
            layout = AknLayoutScalable_Apps::list_cmail_body_pane_t1();
            }
        else if ( iEditorUsage == ENcsEditorAddress ||
                  iEditorUsage == ENcsEditorSubject ) 
            {
            layout = AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( KHeaderDetailTextPaneVariety );
            }

        // Swap the custom drawer to our own to show text lines.
        const MFormCustomDraw* customDraw = TextLayout()->CustomDraw();
    
        iEditorCustomDrawer = CNcsEditorCustomDraw::NewL( customDraw, this, GetLayout() );
        
        TextLayout()->SetCustomDraw( iEditorCustomDrawer );
        }
    }

//<cmail>
void CNcsEditor::SetRealRect( TRect aRect )
	{
	iRealRect = aRect;
	}

void CNcsEditor::Draw( const TRect& /*aRect*/ ) const
    {
    if ( iTextView )
        {
        CWindowGc& gc=SystemGc();
        TRect viewRect( iTextView->ViewRect() );
        if( iRealRect.Height() > 0 )
        	{
        	if( !( viewRect.iTl.iY > iRealRect.iBr.iY || viewRect.iBr.iY < iRealRect.iTl.iY  ) )
        		{
        		TrappedDraw( iRealRect );
        		}
        	}
        else
        	{
        	TrappedDraw( viewRect );
        	}
        
        
    #ifdef RD_UI_TRANSITION_EFFECTS_POPUPS
        // Workaround for clipping rect problem in multiline queries with text
        // entries.  Because of differences between CRemoteGc and CWindowGc, 
        // parts of the query wouldn't be drawn by CRemoteGc.  The Reset() call
        // is to cancel the clipping rect.  For some reason, CancelClippingRect()
        // and CancelClippingRegion() don't work.
        gc.Reset();
    #endif
        }
    }

void CNcsEditor::FormatAllTextNowL()
	{
	iLayout->SetAmountToFormat(CTextLayout::EFFormatAllText);
	iTextView->FormatTextL();
	}
//</cmail>

TAknTextComponentLayout CNcsEditor::GetLayout()
    {
    TAknTextComponentLayout layout;

    if ( iEditorUsage == ENcsEditorBody )
        {
        layout = AknLayoutScalable_Apps::list_cmail_body_pane_t1();
        }
    else if ( iEditorUsage == ENcsEditorAddress ||
              iEditorUsage == ENcsEditorSubject ) 
        {
        layout = AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( KHeaderDetailTextPaneVariety );
        }

    return layout;
    }


void CNcsEditor::UpdateCustomDrawer()
    {
    if ( iEditorCustomDrawer )
        {
        iEditorCustomDrawer->UpdateLayout( GetLayout() );
        }
    }

void CNcsEditor::GetCaptionForFep(TDes& aCaption) const
    {
    const TInt maximumLength = aCaption.MaxLength();    
    if ( iCaptionText && (iCaptionText->Length() < maximumLength) )
        {
        TPtr ptr = iCaptionText->Des();
        aCaption.Copy( ptr ); 
        }                   
    }
 
TCoeInputCapabilities CNcsEditor::InputCapabilities() const
    {
    TCoeInputCapabilities caps = CEikEdwin::InputCapabilities();
    TCoeInputCapabilities caps2 = TCoeInputCapabilities(TCoeInputCapabilities::ENone, NULL, const_cast<CNcsEditor*>(this));
    caps.MergeWith(caps2);
    return caps;
    }


void CNcsEditor::SetPhysicsEmulationOngoing( TBool aPhysOngoing )
    {
    iPhysOngoing = aPhysOngoing;
    this->SetCursorVisible( !aPhysOngoing );
    }

TBool CNcsEditor::IsPhysicsEmulationOngoing() const
    {
    return iPhysOngoing;
    }
