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
    iLabel = new ( ELeave ) CNcsLabel( *this, NULL );
    iLabel->SetTextL( aTextBuf->Des() );
	CleanupStack::PopAndDestroy( aTextBuf );
	// S60 Skin support
	iLabel->SetBrushStyle(CWindowGc::ENullBrush);

    iTextEditor = new ( ELeave ) CNcsEditor( iSizeObserver, ETrue, ENcsEditorSubject );
    // iTextEditor is not completely constructed until in SetContainerWindowL()
    iFocusChangeHandler = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
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
    // Platform layout change
    /*if ( iFont )
        {
        ControlEnv()->ScreenDevice()->ReleaseFont( iFont );
        iFont = NULL;
        }*/

    delete iFocusChangeHandler;
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::SetContainerWindowL() const
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::SetContainerWindowL(const CCoeControl& aContainer)
	{
    FUNC_LOG;
	CCoeControl::SetContainerWindowL(aContainer);

	// Create the component array
    InitComponentArrayL();
	CCoeControlArray& controls = Components();
	controls.SetControlsOwnedExternally(ETrue);
	controls.AppendLC(iLabel);
	CleanupStack::Pop(iLabel);
	controls.AppendLC(iTextEditor);
    CleanupStack::Pop(iTextEditor);

    // Setup text editor
    iTextEditor->ConstructL( &aContainer, KMaxAddressFieldLines, 0 );
	iTextEditor->SetBorder( TGulBorder::ENone );
    iTextEditor->SetAknEditorInputMode( EAknEditorTextInputMode );
	iTextEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
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
    return iLabel->Font()->TextWidthInPixels( *iLabel->Text() );
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
    NcsUtility::LayoutCaptionLabel( iLabel, rect );
    NcsUtility::LayoutDetailEdwinTouch( iTextEditor, rect, iEditorLineCount, EFalse );  
    
    if (iTextEditor->ScrollBarFrame())
        {
        TRect rc = iTextEditor->Rect();
        if (AknLayoutUtils::LayoutMirrored())
            {
            rc.iTl.iX += iTextEditor->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EVertical);
            }
        else
            {
            rc.iBr.iX -= iTextEditor->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EVertical);
            }
        iTextEditor->SetRect(rc);
        }

    // Setup text alignment according the mirrored/normal layout.
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iLabel->SetAlignment( EHLeftVCenter );
        iLabel->SetLabelAlignment( ELayoutAlignLeft );
        iTextEditor->SetAlignment( EAknEditorAlignRight );
        }
    else
        {
        iLabel->SetAlignment( EHRightVCenter );
        iLabel->SetLabelAlignment( ELayoutAlignRight );
        iTextEditor->SetAlignment( EAknEditorAlignLeft );
        }
    
	UpdateFontSize();

	iTextEditor->UpdateCustomDrawer();
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::PositionChanged()
// set size
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::PositionChanged()
    {
    FUNC_LOG;
    //SizeChanged();
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::OfferKeyEventL()
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CNcsSubjectField::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
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
    iTextEditor->HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::FocusChanged()
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::FocusChanged( TDrawNow aDrawNow )
	{
    FUNC_LOG;
	// The focus change gaining handling needs to be deferred until AknFep is fully
	// initialised. Otherwise there will be problems when moving focus from
	// body to subject. Focus losing can be handled immediately
	iDrawAfterFocusChange = aDrawNow;
	if ( IsFocused() )
	    {
	TCallBack callback( DoHandleFocusChanged, this );
	iFocusChangeHandler->Cancel();
	iFocusChangeHandler->Set( callback );
	iFocusChangeHandler->CallBack();
	}
	else
	    {
	    DoHandleFocusChanged( this );
	    }
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::HandleFocusChangedL()
// -----------------------------------------------------------------------------
//
void CNcsSubjectField::HandleFocusChangedL()
    {
    FUNC_LOG;
    if ( IsFocused() )
        {
        iTextEditor->SetFocus( ETrue );
        iTextEditor->SetCursorPosL( iTextEditor->TextLength(), EFalse );

        // make sure that control is visible on screen
        if ( Rect().iTl.iY < 0 )
            {
            TPoint pt = TPoint( 0, 0 );
            Reposition( pt, Rect().Width() );
            iSizeObserver->UpdateFieldPosition( this );
            }
        else
            {
            TPoint pos = PositionRelativeToScreen();
            pos.iY += Size().iHeight;
            CWsScreenDevice* screenDev = ControlEnv()->ScreenDevice();
            TPixelsAndRotation pix;
            screenDev->GetDefaultScreenSizeAndRotation( pix );
            const TInt h = pix.iPixelSize.iHeight;
            if ( pos.iY >= h - h / 3 )
                {
                TPoint pt = TPoint( 0, h / 3 );
                Reposition( pt, Rect().Width() );
                iSizeObserver->UpdateFieldPosition( this );
                }
            }

        if ( iParentControl )
            {
            iParentControl->SetMskL();
            }
        }
    else
        {
        iTextEditor->HandleTextChangedL();
        iTextEditor->SetFocus( EFalse );
        }

    if ( iDrawAfterFocusChange )
        {
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::DoHandleFocusChanged()
// -----------------------------------------------------------------------------
//
TInt CNcsSubjectField::DoHandleFocusChanged( TAny* aSelfPtr )
    {
    FUNC_LOG;
    CNcsSubjectField* self = static_cast<CNcsSubjectField*>( aSelfPtr );
    TRAPD( err, self->HandleFocusChangedL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CNcsSubjectField::MinimumHeight()
// return height of subject control
// -----------------------------------------------------------------------------
//
TInt CNcsSubjectField::MinimumHeight()
    {
    FUNC_LOG;
    // Platform layout changes
    /*
    TNcsMeasures m = NcsUtility::Measures();
	TInt height = m.iAifHeight - m.iAifEditorHeight + iEditorMinimumHeight + m.iSubjectExtraHeightBottom;
	TInt height2 = m.iAifHeight + m.iSubjectExtraHeightBottom;
	// if followup icon and priority icon are both visible increase the subject field height
	if ( iFollowUp && iPriority != EMsgPriorityNormal )
	    {
	    height2 += KIconHeightFollowUp;
	    }
    return Max( height, height2 );
    */
    //return NcsUtility::MinimumHeaderSize( Rect(), iEditorLineCount ).iHeight;
    // Platform layout changes
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
	    RMemReadStream inputStream( aSubject.Ptr(), aSubject.Size() );
	    CleanupClosePushL( inputStream );

	  	iTextEditor->RichText()->ImportTextL( 0, inputStream, CPlainText::EOrganiseByParagraph );

		// Activating the field will set the control to the end of the text
	    iTextEditor->ActivateL();
	    iTextEditor->SetCursorPosL(0,EFalse);

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
    //  Platform layout chage
    /*
    TInt ret( EFalse );
    TInt htLine = iTextEditor->GetLineHeightL();
    TInt numLinesDesired = aDesirableEdwinSize.iHeight / htLine;

    if ( numLinesDesired < KMaxAddressFieldLines &&
         iEditorMinimumHeight != aDesirableEdwinSize.iHeight )
        {
        iEditorMinimumHeight = aDesirableEdwinSize.iHeight;
        iTextEditor->CreateScrollBarFrameL()->SetScjrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
        if ( iSizeObserver )
            {
            ret = iSizeObserver->UpdateFieldSizeL();
            }
        }
    else if ( iEditorMinimumHeight != htLine * KMaxAddressFieldLines )
        {
        //We may still need to resize the controll.
        iEditorMinimumHeight = htLine * KMaxAddressFieldLines;
        iTextEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn );
        if ( iSizeObserver )
            {
            ret = iSizeObserver->UpdateFieldSizeL();
            }
        }
    */

    TInt ret( EFalse );
    TInt htLine = iTextEditor->GetLineHeightL();
    TInt numLinesDesired = aDesirableEdwinSize.iHeight / htLine;
    CEikScrollBarFrame& scrollBarFrame( *iTextEditor->CreateScrollBarFrameL() );
    iEditorLineCount = numLinesDesired;
    scrollBarFrame.SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
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

// Platform layout change
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
    /*
	const TSize size( aWidth, MinimumHeight() );
    SetExtent( aPt, size );
    aPt.iY += size.iHeight;
    */
    SetPosition( aPt );
    // Platform layout changes
	}

// -----------------------------------------------------------------------------
// CNcsSubjectField::GetLineRectL
// -----------------------------------------------------------------------------
void CNcsSubjectField::GetLineRectL( TRect& aLineRect ) const
	{
    FUNC_LOG;
    return iTextEditor->GetLineRectL( aLineRect );
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
    // Platform layout change
    TRgb textColor = KRgbBlack;
    if ( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), textColor,
        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
        textColor = KRgbBlack;
        }
    TRAP_IGNORE( iLabel->OverrideColorL( EColorLabelText, textColor ) );
    // Platform layout change

    CFreestyleEmailUiAppUi* appUi =
        static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    iBorderColor = appUi->LayoutHandler()->ComposerFieldBorderColor();
    iBgColor = appUi->LayoutHandler()->ComposerFieldBackgroundColor();
    }

// Platform layout changes
// ---------------------------------------------------------------------------
// CNcsSubjectField::UpdateFontSize()
// ---------------------------------------------------------------------------
//
void CNcsSubjectField::UpdateFontSize()
	{
    FUNC_LOG;
    /*if ( iFont )
        {
        ControlEnv()->ScreenDevice()->ReleaseFont( iFont );
        iFont = NULL;
        }

	TNcsMeasures m = NcsUtility::Measures();
	if ( !iFont )
	    {
	    TRAP_IGNORE( iFont = NcsUtility::GetNearestFontL(EAknLogicalFontPrimarySmallFont,
	    m.iLabelFontHeightPx) );
	    }*/
	iFont = NcsUtility::GetLayoutFont( iLabel->Rect(), NcsUtility::ENcsHeaderCaptionFont );
	iLabel->SetFont( iFont );
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
        // Platform layout changes
        iTextEditor->HandleResourceChange( aType ) ;//CreateScrollBarFrameL()->DrawScrollBarsNow();
        // Platform layout changes
        }
    }

