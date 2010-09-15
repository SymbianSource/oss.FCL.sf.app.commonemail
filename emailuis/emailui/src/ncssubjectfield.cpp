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
* Description:  compose view container
*
*/



#include "emailtrace.h"
#include <AknIconUtils.h>
#include <avkon.hrh>
#include <eiklabel.h>
#include <aknbutton.h>
#include <eikimage.h>
#include <eikenv.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <StringLoader.h>
#include <s32mem.h>
#include <txtrich.h>
#include <FreestyleEmailUi.rsg>
#include <freestyleemailui.mbg>
#include "FreestyleEmailUiLayoutData.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiTextureManager.h"
#include "ncssubjectfield.h"
#include "ncsconstants.h"
#include "ncsfieldsizeobserver.h"
#include "ncsutility.h"
#include "ncsheadercontainer.h"

// ---------------------------------------------------------------------------
// CNcsSubjectField::CNcsSubjectField()
// ---------------------------------------------------------------------------
//
CNcsSubjectField::CNcsSubjectField( MNcsFieldSizeObserver* aSizeObserver,
        CNcsHeaderContainer* aParentControl ):
	MNcsControl( aSizeObserver ),
	iParentControl( aParentControl )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNcsSubjectField* CNcsSubjectField::NewL( TInt aLabelTextId,
	MNcsFieldSizeObserver* aSizeObserver, CNcsHeaderContainer* aParentControl )
    {
    FUNC_LOG;
    CNcsSubjectField* self =
        new ( ELeave ) CNcsSubjectField( aSizeObserver, aParentControl );
    CleanupStack::PushL( self );
    self->ConstructL( aLabelTextId );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CNcsSubjectField::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::ConstructL( TInt aLabelTextId )
    {
    FUNC_LOG;
    // Create label
	HBufC* aTextBuf = StringLoader::LoadLC( aLabelTextId );
    TPtrC captionText = aTextBuf ? aTextBuf->Des() : TPtrC();

    if( AknLayoutUtils::PenEnabled() )
        {
        // Create a frameless button that has no observer
        // This is done like this to make Subject: field appear similar as
        // other header fields (To:, Cc:), although it doesn't have other
        // button-like features
        TRect buttRect( 10, 10, 70, 100 );
        iButton = CAknButton::NewL( NULL, NULL, NULL, NULL, 
                                    captionText, TPtrC(), 
                                    KAknButtonTextLeft | KAknButtonNoFrame ,
                                    0 );
        iButton->SetTextColorIds( 
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG63 );
        iButton->SetContainerWindowL( *iParentControl );
        iButton->SetRect( buttRect );
        iButton->EnableFeedback( EFalse );
        iButton->MakeVisible( ETrue );
        iButton->ActivateL();
        }
    else
        {
        iLabel = new ( ELeave ) CNcsLabel( *this, NULL );
        iLabel->SetTextL( captionText );
    
        // S60 Skin support
        iLabel->SetBrushStyle(CWindowGc::ENullBrush);
        }
    
    iTextEditor = new ( ELeave ) CNcsEditor( 
            iSizeObserver, ETrue, ENcsEditorSubject, captionText );
    CleanupStack::PopAndDestroy( aTextBuf );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcsSubjectField::~CNcsSubjectField()
    {
    FUNC_LOG;
    delete iTextEditor;
    delete iLabel;
    delete iButton;
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::SetContainerWindowL() const
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::SetContainerWindowL( const CCoeControl& aContainer )
	{
    FUNC_LOG;
	CCoeControl::SetContainerWindowL( aContainer );

	// Create the component array
    InitComponentArrayL();
	CCoeControlArray& controls = Components();
	controls.SetControlsOwnedExternally( ETrue );
	if ( iButton )
	    {
        controls.AppendLC( iButton );
        CleanupStack::Pop( iButton );
	    }
	else
	    {
        controls.AppendLC( iLabel );
        CleanupStack::Pop( iLabel );
	    }
	controls.AppendLC( iTextEditor );
    CleanupStack::Pop( iTextEditor );

    // Setup text editor
    iTextEditor->ConstructL( &aContainer, KMaxAddressFieldLines, 0 );
	iTextEditor->SetBorder( TGulBorder::ENone );
    iTextEditor->SetAknEditorInputMode( EAknEditorTextInputMode );
	iTextEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL( 
	        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
    iTextEditor->SetEdwinSizeObserver( this );
    iTextEditor->SetupEditorL();
    // Setup label
    UpdateFontSize();
    UpdateColors();
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::Draw() const
// Draws the display
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::Draw( const TRect& /*aRect*/ ) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::GetMinLabelLength()
// -----------------------------------------------------------------------------
//
TInt CNcsSubjectField::GetMinLabelLength() const
	{
    FUNC_LOG;
    return iLabel ? iLabel->Font()->TextWidthInPixels( *iLabel->Text() ) : 0;
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::SizeChanged()
// set size
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::SizeChanged()
    {
    FUNC_LOG;
	
    const TRect rect( Rect() );
    if ( iButton )
        {
        NcsUtility::LayoutCaptionButton( iButton, rect );
        }
    else
        {
        NcsUtility::LayoutCaptionLabel( iLabel, rect );
        }

    NcsUtility::LayoutDetailEdwinTouch( 
            iTextEditor, rect, iEditorLineCount, EFalse );  
    
    if ( iTextEditor->ScrollBarFrame() )
        {
        TRect rc = iTextEditor->Rect();
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            rc.iTl.iX += iTextEditor->ScrollBarFrame()->ScrollBarBreadth( 
                    CEikScrollBar::EVertical );
            }
        else
            {
            rc.iBr.iX -= iTextEditor->ScrollBarFrame()->ScrollBarBreadth( 
                    CEikScrollBar::EVertical );
            }
        iTextEditor->SetRect( rc );
        }

    // Setup text alignment according the mirrored/normal layout.
    if ( iLabel )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            iLabel->SetAlignment( EHLeftVCenter );
            iLabel->SetLabelAlignment( ELayoutAlignLeft );
            }
        else
            {
            iLabel->SetAlignment( EHRightVCenter );
            iLabel->SetLabelAlignment( ELayoutAlignRight );
            }
        }

    // this needs to be bidi as in mirrored layout 
    // writing language left to right can be set 
    iTextEditor->SetAlignment( EAknEditorAlignBidi );
    
	UpdateFontSize();

	iTextEditor->UpdateCustomDrawer();
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::PositionChanged()
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::PositionChanged()
    {
    FUNC_LOG;
    // empty
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::OfferKeyEventL()
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CNcsSubjectField::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
	TKeyResponse ret( EKeyWasNotConsumed );

	ret = iTextEditor->OfferKeyEventL( aKeyEvent, aType );
    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::HandlePointerEventL()
// Handles pointer events
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;
    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::FocusChanged()
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::FocusChanged( TDrawNow aDrawNow )
    {
    FUNC_LOG;
    if ( IsFocused() )
        {
        iTextEditor->SetFocus( ETrue, aDrawNow );
        if ( iParentControl )
            {
            TRAP_IGNORE( iParentControl->SetMskL() );
            }
        }
    else
        {
        iTextEditor->SetFocus( EFalse, aDrawNow );
        }

    if ( aDrawNow )
        {
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::MinimumHeight()
// return height of subject control
// -----------------------------------------------------------------------------
//
TInt CNcsSubjectField::MinimumHeight()
    {
    FUNC_LOG;
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::SetSubjectL()
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::SetSubjectL( const TDesC& aSubject )
    {
    FUNC_LOG;
    if ( &aSubject && aSubject.Length() > 0 )
    	{
        // clear subject if necessary
        TInt documentLength = iTextEditor->RichText()->DocumentLength();
        if ( documentLength > 0 )
            {
            iTextEditor->RichText()->DeleteL( 0, documentLength );
            iTextEditor->HandleTextChangedL();
            }

        RMemReadStream inputStream( aSubject.Ptr(), aSubject.Size() );
	    CleanupClosePushL( inputStream );

	  	iTextEditor->RichText()->ImportTextL( 
	  	        0, inputStream, CPlainText::EOrganiseByParagraph );

		// Activating the field will set the control to the end of the text
	    iTextEditor->ActivateL();
	    iTextEditor->SetCursorPosL( 0, EFalse );

	    CleanupStack::PopAndDestroy( &inputStream );
    	}
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::HandleEdwinSizeEventL()
// -----------------------------------------------------------------------------
//
TBool CNcsSubjectField::HandleEdwinSizeEventL( CEikEdwin* /*aEdwin*/,
	TEdwinSizeEvent /*aEventType*/, TSize aDesirableEdwinSize )
    {
    FUNC_LOG;
    TInt ret( EFalse );
    TInt htLine = iTextEditor->GetLineHeightL();
    TInt numLinesDesired = aDesirableEdwinSize.iHeight / htLine;
    iEditorLineCount = numLinesDesired;
    if ( iSizeObserver )
        {
        ret = iSizeObserver->UpdateFieldSizeL();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::SelectAllTextL()
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::SelectAllTextL()
	{
    FUNC_LOG;
	iTextEditor->SelectAllL();
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::Editor
// -----------------------------------------------------------------------------
CNcsEditor* CNcsSubjectField::Editor() const
	{
    FUNC_LOG;
	return iTextEditor;
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::LineCount
// -----------------------------------------------------------------------------
TInt CNcsSubjectField::LineCount() const
	{
    FUNC_LOG;
	return iTextEditor->LineCount();
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::CursorLineNumber
// -----------------------------------------------------------------------------
TInt CNcsSubjectField::CursorLineNumber() const
	{
    FUNC_LOG;
	return iTextEditor->CursorLineNumber();
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::CursorPosition
// -----------------------------------------------------------------------------
TInt CNcsSubjectField::CursorPosition() const
	{
    FUNC_LOG;
	TInt pos = iTextEditor->CursorPosition();
	//Add the delta between the top of the edit control
	//and the top of the parent control
	pos += iTextEditor->Rect().iTl.iY - Rect().iTl.iY;
	return pos;
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::ScrollableLines
// -----------------------------------------------------------------------------
TInt CNcsSubjectField::ScrollableLines() const
	{
    FUNC_LOG;
	return iTextEditor->ScrollableLines();
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::GetNumChars
// -----------------------------------------------------------------------------
TInt CNcsSubjectField::GetNumChars() const
	{
    FUNC_LOG;
	return iTextEditor->GetNumChars();
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::GetLabelText
// -----------------------------------------------------------------------------
const TDesC& CNcsSubjectField::GetLabelText() const
	{
    FUNC_LOG;
	return KNullDesC;
	}

// ---------------------------------------------------------------------------
// CNcsSubjectField::LayoutLineCount
// ---------------------------------------------------------------------------
//
TInt CNcsSubjectField::LayoutLineCount() const
    {
    FUNC_LOG;
    return ( IsVisible() ? Max( 1, iEditorLineCount ) : 0 );
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::Reposition
// -----------------------------------------------------------------------------
void CNcsSubjectField::Reposition(TPoint& aPt, TInt /*aWidth*/)
	{
    FUNC_LOG;
    SetPosition( aPt );
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::GetLineRectL
// -----------------------------------------------------------------------------
void CNcsSubjectField::GetLineRect( TRect& aLineRect ) const
    {
    FUNC_LOG;
    return iTextEditor->GetLineRect( aLineRect );
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::SetMaxLabelLength
// -----------------------------------------------------------------------------
void CNcsSubjectField::SetMaxLabelLength( TInt aMaxLength )
	{
    FUNC_LOG;
	iMaximumLabelLength = aMaxLength;
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::SetTextColorL
// -----------------------------------------------------------------------------
void CNcsSubjectField::SetTextColorL( TLogicalRgb aColor )
	{
    FUNC_LOG;
	iTextEditor->SetTextColorL( aColor );
	}

// ---------------------------------------------------------------------------
// CNcsSubjectField::UpdateColors()
// ---------------------------------------------------------------------------
//
void CNcsSubjectField::UpdateColors()
    {
	FUNC_LOG;

	if ( iLabel )
	    {
        TRgb textColor = KRgbBlack;
        if ( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), textColor,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
            {
            textColor = KRgbBlack;
            }
        TRAP_IGNORE( iLabel->OverrideColorL( EColorLabelText, textColor ) );
        }
    }

// ---------------------------------------------------------------------------
// CNcsSubjectField::UpdateFontSize()
// ---------------------------------------------------------------------------
//
void CNcsSubjectField::UpdateFontSize()
	{
    FUNC_LOG;
    CCoeControl* control = 
            iButton ? ( CCoeControl* )iButton : ( CCoeControl* )iLabel;
    iFont = NcsUtility::GetLayoutFont( 
            control->Rect(), NcsUtility::ENcsHeaderCaptionFont );
    if( control == iButton )
        {
        iButton->SetTextFont( iFont );
        }
    else
        {
        iLabel->SetFont( iFont );
        }
	iTextEditor->UpdateFontSize();
	UpdateColors();
	}

// ---------------------------------------------------------------------------
// CNcsSubjectField::HandleResourceChange()
// ---------------------------------------------------------------------------
//
void CNcsSubjectField::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikMessageColorSchemeChange )
        {
        UpdateFontSize();
        UpdateColors();
        iTextEditor->HandleResourceChange( aType );
        }
    }

// ---------------------------------------------------------------------------
// CNcsSubjectField::EnableKineticScrollingL()
// ---------------------------------------------------------------------------
//
void CNcsSubjectField::EnableKineticScrollingL( CAknPhysics*  aPhysics )
    {
    iTextEditor->EnableKineticScrollingL( aPhysics );
    iTextEditor->TextLayout()->RestrictScrollToTopsOfLines( ETrue );
    }

// ---------------------------------------------------------------------------
// CNcsSubjectField::SetPhysicsEmulationOngoing()
// ---------------------------------------------------------------------------
//
void CNcsSubjectField::SetPhysicsEmulationOngoing( TBool aPhysOngoing )
    {
    iTextEditor->SetPhysicsEmulationOngoing( aPhysOngoing );
    }
