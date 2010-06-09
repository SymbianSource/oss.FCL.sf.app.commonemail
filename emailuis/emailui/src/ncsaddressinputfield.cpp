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



// INCLUDE FILES
#include "emailtrace.h"
#include <AknIconUtils.h>
#include <avkon.hrh>
#include <aknlists.h>
#include <aknPopup.h>
#include <aknbutton.h>
#include <StringLoader.h>
#include <eiklabel.h>
#include <eikimage.h>
#include <eikenv.h>
#include <AknsDrawUtils.h>
#include <sysutil.h>
#include <AknUtils.h>
#include <FreestyleEmailUi.rsg>
#include <aknphysics.h>

#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLayoutData.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "ncsaddressinputfield.h"
#include "ncsconstants.h"
#include "ncsaifeditor.h"
#include "ncsfieldsizeobserver.h"
#include "ncsutility.h"
#include "ncsheadercontainer.h"
#include "txtrich.h"

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CNcsAddressInputField::CNcsAddressInputField(
	TNcsInputFieldType aFieldType,
	MNcsFieldSizeObserver* aSizeObserver,
	MNcsAddressPopupList* aAddressPopupList,
	CNcsHeaderContainer* aParentControl ):
	MNcsControl( aSizeObserver ),
	iParentControl( aParentControl ),
	iAddressPopupList( aAddressPopupList ),
	iFieldType( aFieldType )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNcsAddressInputField* CNcsAddressInputField::NewL(
	TInt aLabelTextId,
	TNcsInputFieldType aFieldType,
	MNcsFieldSizeObserver* aSizeObserver,
	MNcsAddressPopupList* aAddressPopupList,
	CNcsHeaderContainer* aParentControl )
    {
    FUNC_LOG;
    CNcsAddressInputField* self = new ( ELeave ) CNcsAddressInputField(
            aFieldType, aSizeObserver, aAddressPopupList, aParentControl );
    CleanupStack::PushL( self );
    self->ConstructL( aLabelTextId );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CNcsAddressInputField::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::ConstructL( TInt aLabelTextId )
	{
    FUNC_LOG;
	// Create label
	//Load the label string
	HBufC* aTextBuf = StringLoader::LoadLC( aLabelTextId );

	TPtrC captionText = aTextBuf ? aTextBuf->Des() : TPtrC();
	CreateControlsL( captionText );
	
	iTextEditor = new ( ELeave ) CNcsAifEditor( iSizeObserver,
                                                captionText );
    // iTextEditor is not completely constructed until in SetContainerWindowL()

	iTextEditor->SetPopupList(iAddressPopupList);

	CleanupStack::PopAndDestroy( aTextBuf );
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcsAddressInputField::~CNcsAddressInputField()
    {
    FUNC_LOG;
    delete iTextEditor;
    delete iButton;
    delete iLabel;
// <cmail> Platform layout change
    /*if ( iFont )
        {
        ControlEnv()->ScreenDevice()->ReleaseFont( iFont );
        iFont = NULL;
        }</cmail>*/
    }

//<cmail>
// -----------------------------------------------------------------------------
// CNcsHeaderContainer::CreateControlsL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::CreateControlsL( const TDesC& aControlText )
    {
    FUNC_LOG;
    if( AknLayoutUtils::PenEnabled() )
        {
        //Create button
        TRect buttRect( 10, 10, 70, 100 );
        iButton = CAknButton::NewL( NULL, NULL, NULL, NULL, aControlText, TPtrC(), KAknButtonTextLeft, 0 );
        iButton->SetTextColorIds( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG63 );
        iButton->SetContainerWindowL( *iParentControl );
        iButton->SetObserver( this );
        iButton->SetRect( buttRect );
        iButton->MakeVisible( ETrue );
        iButton->ActivateL();
        }
    else
        {
        //Create label
        iLabel = new ( ELeave ) CNcsLabel( *this, NULL );
        iLabel->SetTextL( aControlText );
        iLabel->SetBrushStyle(CWindowGc::ENullBrush);
        }
    }
//</cmail>

// ---------------------------------------------------------------------------
// SetContainerWindow
// Don't construct the window dependent elements until the window
// container is set
// ---------------------------------------------------------------------------
//
void CNcsAddressInputField::SetContainerWindowL( const CCoeControl& aContainer )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL(aContainer);

	InitComponentArrayL();
	// Check if we need to construct the components
	if (Components().Count() == 2) return;

	// Load the controls into the compoent array
	CCoeControlArray& controls = Components();
	controls.SetControlsOwnedExternally(ETrue);
	//<cmail>
	if( iButton )
	    {
	    controls.AppendLC( iButton );
	    CleanupStack::Pop( iButton );
	    }
	else
	    {
	    controls.AppendLC( iLabel );
	    CleanupStack::Pop( iLabel );
	    }
	//</cmail>
	controls.AppendLC(iTextEditor);
    CleanupStack::Pop(iTextEditor);

// <cmail>

	// Setup the text editor
	iTextEditor->ConstructL( &aContainer, KMaxAddressFieldLines, 0 );
// </cmail>

    iTextEditor->SetBorder( TGulBorder::ENone );
	iTextEditor->SetAknEditorInputMode( EAknEditorTextInputMode );
	iTextEditor->SetAknEditorFlags( EAknEditorFlagNoT9 | EAknEditorFlagUseSCTNumericCharmap );
	iTextEditor->SetAknEditorCurrentCase( EAknEditorLowerCase );
	iTextEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
    iTextEditor->SetEdwinSizeObserver( this );
    iTextEditor->SetupEditorL();

    // Setup the label
    UpdateFontSize();

    // Setup text alignment according the mirrored/normal layout.
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        if( iLabel )
            iLabel->SetAlignment( EHRightVCenter );
        iTextEditor->SetAlignment( EAknEditorAlignRight );
        }
    else
        {
        if( iLabel )
            iLabel->SetAlignment( EHLeftVCenter );
        iTextEditor->SetAlignment( EAknEditorAlignLeft );
        }

    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::Draw() const
// Draws the display
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::Draw( const TRect& /*aRect*/ ) const
	{
    FUNC_LOG;
 	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::GetMinLabelLength()
// -----------------------------------------------------------------------------
//
TInt CNcsAddressInputField::GetMinLabelLength() const
	{
    FUNC_LOG;
    return iLabel ? iLabel->Font()->TextWidthInPixels( *iLabel->Text() ) : 0;
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::SizeChanged()
// set size
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::SizeChanged()
	{
    FUNC_LOG;

   	TRect rect = Rect();
 
   	if( AknLayoutUtils::PenEnabled() )
   	    {
   	    LayoutTouch();
   	    }
   	  else
   	    {
   	    LayoutNonTouch();
   	    }
   	    
    UpdateFontSize();
    
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
    
    PositionChanged();
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::PositionChanged()
// set size
// -----------------------------------------------------------------------------
void CNcsAddressInputField::PositionChanged()
	{
    FUNC_LOG;
    
    // keep the button in view as long as possible
    if( iButton && iParentControl->IsVisible() )
    	{
		const TRect rect( Rect() );
		const TRect buttonRect( iButton->Rect() );
		
		TInt newButtonPos( iOriginalButtonPos.iY - iOriginalFieldPos.iY );
		
		if( rect.iTl.iY < 0 && newButtonPos + buttonRect.Height() < rect.iBr.iY )
			{
			iButton->SetPosition( TPoint(iOriginalButtonPos.iX, newButtonPos) );
			}
		else
			{
			iButton->SetPosition( TPoint(iOriginalButtonPos.iX, newButtonPos + rect.iTl.iY) );
			}
    	}
	}

//<cmail>
// -----------------------------------------------------------------------------
// CNcsAddressInputField::LayoutNonTouch()
// 
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::LayoutNonTouch()
    {
    const TRect rect( Rect() );
    NcsUtility::LayoutCaptionLabel( iLabel, rect );
    NcsUtility::LayoutDetailEdwin( iTextEditor, rect, iEditorLineCount, EFalse );  
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::LayoutTouch()
// 
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::LayoutTouch()
    {
    const TRect rect( Rect() );
    NcsUtility::LayoutCaptionButton( iButton, rect );
    NcsUtility::LayoutDetailEdwinTouch( iTextEditor, rect, iEditorLineCount, EFalse );  

    // save the original field positions so that we can the button in view
    iOriginalFieldPos = rect.iTl;
    iOriginalButtonPos = iButton->Position();    

    iTextEditor->UpdateCustomDrawer();
    }
    
// -----------------------------------------------------------------------------
// CNcsAddressInputField::HandlePointerEventL()
// Handles pointer events
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
	FUNC_LOG;
    iTextEditor->HandlePointerEventL(aPointerEvent);
    if( iButton )
        {
        iButton->HandlePointerEventL( aPointerEvent );
        }
    iTextEditor->HandleTextChangedL();
    
    switch( aPointerEvent.iType )
    	{
    	case TPointerEvent::EButton1Down:
    		{
            // Save start position so that it can be used in
            // drag/scrolling calculations
            iStartPosition = aPointerEvent.iPosition;
            iIsDraggingStarted = EFalse;
            break;
    		}
    		
    	case TPointerEvent::EDrag:
    		{
            if ( !iIsDraggingStarted && iPhysics )
                {
                TInt drag( iStartPosition.iY - aPointerEvent.iPosition.iY );
                if ( Abs( drag ) > iPhysics->DragThreshold() )
                    {
					iIsDraggingStarted = ETrue;
                    }
                }
            break;
    		}
    	}
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::HandleControlEventL()
// 
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
    {
    if( aEventType == EEventStateChanged )
        {
        if( aControl == iButton && !iIsDraggingStarted )
            {
            iParentControl->OpenPhonebookL();
            }
        }
    }
//</cmail>

// -----------------------------------------------------------------------------
// CNcsAddressInputField::OfferKeyEventL()
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse 
CNcsAddressInputField::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
	{
    FUNC_LOG;
	TKeyResponse ret( EKeyWasNotConsumed );

	if( aKeyEvent.iCode == EKeyOK )
		{
		iTextEditor->SetCursorPosL( iTextEditor->TextLength(), EFalse );
		iTextEditor->UpdateAddressListAllL();
		}
	else
		{
		ret = iTextEditor->OfferKeyEventL( aKeyEvent, aType );
		}

	return ret;
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::FocusChanged()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::FocusChanged( TDrawNow aDrawNow )
	{
    FUNC_LOG;
	if ( IsFocused() )
		{
		iTextEditor->SetFocus( ETrue, aDrawNow );
        //TRAP_IGNORE( iTextEditor->SetCursorPosL( iTextEditor->TextLength(), EFalse ) );

        // make sure that control is visible on screen
		if ( Rect().iTl.iY < 0 )
			{
			TPoint pt = TPoint( 0, 0 );
			Reposition( pt,Rect().Width() );
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
    			Reposition( pt,Rect().Width() );
                iSizeObserver->UpdateFieldPosition( this );
    		    }
		    }

        if ( iParentControl )
            {
            TRAP_IGNORE( iParentControl->SetMskL() );
            }
		}
    else
        {
        if ( iTextEditor->SelectionLength()== 0 )
            {
            TRAP_IGNORE( iTextEditor->HandleTextChangedL() );
            }
        
        iTextEditor->SetFocus( EFalse );
        TRAP_IGNORE( iAddressPopupList->ClosePopupContactListL() );
        }

	if ( aDrawNow )
		{
		DrawNow();
		}
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::MinimumHeight()
// -----------------------------------------------------------------------------
//
TInt CNcsAddressInputField::MinimumHeight() const
    {
    FUNC_LOG;
    // <cmail> Platform layout changes
    /*TNcsMeasures m = NcsUtility::Measures();
	TInt height = m.iAifHeight - m.iAifEditorHeight + iEditorMinimumHeight;
	TInt height2 = m.iAifHeight;
    return Max( height, height2 );
    */

    //return NcsUtility::MinimumHeaderSize( Rect(), iEditorLineCount ).iHeight;
    // </cmail> Platform layout changes
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::HandleEdwinSizeEventL()
// -----------------------------------------------------------------------------
//
TBool CNcsAddressInputField::HandleEdwinSizeEventL(
	CEikEdwin* /*aEdwin*/,
    TEdwinSizeEvent /*aEventType*/,
    TSize aDesirableEdwinSize )
    {
    FUNC_LOG;
// <cmail> Platform layout change
    /*TInt ret( EFalse );
    TInt htLine = iTextEditor->GetLineHeightL();
    TInt numLinesDesired = aDesirableEdwinSize.iHeight / htLine;

    if( numLinesDesired < KMaxAddressFieldLines &&
        iEditorMinimumHeight != aDesirableEdwinSize.iHeight )
        {
        iEditorMinimumHeight = aDesirableEdwinSize.iHeight;

        if( iSizeObserver )
            ret = iSizeObserver->UpdateFieldSizeL();
        iTextEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );

        }
    else if (iEditorMinimumHeight != htLine * KMaxAddressFieldLines)
        {
        //We may still need to resize the controll.
        iEditorMinimumHeight = htLine * KMaxAddressFieldLines;
        iTextEditor->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn );
        if (iSizeObserver)
            ret = iSizeObserver->UpdateFieldSizeL();
        }
    return ret;*/

    TInt ret( EFalse );
    TInt htLine = iTextEditor->GetLineHeightL();
    TInt numLinesDesired = aDesirableEdwinSize.iHeight / htLine;
    // commented because of EGWG-83ECSR, no negative feedback after removing
    //CEikScrollBarFrame& scrollBarFrame( *iTextEditor->CreateScrollBarFrameL() );
    iEditorLineCount = numLinesDesired;
    //scrollBarFrame.SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
    if ( iSizeObserver )
        {
        ret = iSizeObserver->UpdateFieldSizeL();
        }
    return ret;
// </cmail>
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::AddAddressL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::AddAddressL( const CNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    iTextEditor->AddAddressL( aAddress );
    }

// -----------------------------------------------------------------------------
// CNcsAddressInputField::AddAddressL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::AddAddressL( const TDesC& aEmail )
	{
    FUNC_LOG;
	iTextEditor->AddAddressL( KNullDesC(), aEmail );
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::SelectAllTextL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::SelectAllTextL()
	{
    FUNC_LOG;
	iTextEditor->SelectAllL();
	}

// <cmail> Platform layout changes
// -----------------------------------------------------------------------------
// CNcsAddressInputField::Reposition()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::Reposition( TPoint& aPt, TInt /*aWidth*/ )
	{
    FUNC_LOG;
	/*
    TSize sz( aWidth, MinimumHeight() );
	if ( Rect() != TRect( aPt, sz ) )
		{
		SetExtent( aPt, sz);
		}
    aPt.iY += Size().iHeight;
    */
    SetPosition( aPt );
    // </cmail> Platform layout changes
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::GetLabelText()
// -----------------------------------------------------------------------------
//
const TDesC& CNcsAddressInputField::GetLabelText() const
	{
    FUNC_LOG;
	return iLabel ? *iLabel->Text() : KEmptyReturnValue();
	}

// <cmail> Platform layout change
// ---------------------------------------------------------------------------
// CNcsAddressInputField::LayoutLineCount
// ---------------------------------------------------------------------------
//
TInt CNcsAddressInputField::LayoutLineCount() const
    {
    FUNC_LOG;
    if( iLabel && !AknLayoutUtils::PenEnabled() )
        {
        return ( IsVisible() ? Max( 1, iEditorLineCount ) : 0 ) + iLabel->LayoutLineCount();
        }
    else
        {
        return ( IsVisible() ? Max( 1, iEditorLineCount ) : 0 );
        }
    }
// </cmail> Platform layout change

// -----------------------------------------------------------------------------
// CNcsAddressInputField::LineCount()
// -----------------------------------------------------------------------------
//
TInt CNcsAddressInputField::LineCount() const
	{
    FUNC_LOG;
	return iTextEditor->LineCount();
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::ScrollableLines()
// -----------------------------------------------------------------------------
//
TInt CNcsAddressInputField::ScrollableLines() const
	{
    FUNC_LOG;
	return iTextEditor->ScrollableLines();
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::GetNumChars()
// -----------------------------------------------------------------------------
//
TInt CNcsAddressInputField::GetNumChars() const
	{
    FUNC_LOG;
	return iTextEditor->GetNumChars();
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::CursorLineNumber()
// -----------------------------------------------------------------------------
//
TInt CNcsAddressInputField::CursorLineNumber() const
	{
    FUNC_LOG;
	return iTextEditor->CursorLineNumber();
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::CursorPosition()
// -----------------------------------------------------------------------------
//
TInt CNcsAddressInputField::CursorPosition() const
	{
    FUNC_LOG;
	TInt pos = iTextEditor->CursorPosition();
	//Add the delta between the top of the edit control
	//and the top of the parent control
	pos += iTextEditor->Rect().iTl.iY - Rect().iTl.iY ;
	return pos;
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::Editor()
// -----------------------------------------------------------------------------
//
CNcsEditor* CNcsAddressInputField::Editor() const
	{
    FUNC_LOG;
	return iTextEditor;
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::IsEmpty()
// -----------------------------------------------------------------------------
//
TBool CNcsAddressInputField::IsEmpty() const
	{
    FUNC_LOG;
	return iTextEditor->TextLength() ==  0;
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::GetLookupTextLC()
// -----------------------------------------------------------------------------
//
HBufC* CNcsAddressInputField::GetLookupTextLC() const
	{
    FUNC_LOG;
	return iTextEditor->GetLookupTextLC();
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::GetAddressesL()
// -----------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>&
CNcsAddressInputField::GetAddressesL( TBool aParseNow )
	{
	if ( aParseNow )
		{
		iTextEditor->CheckAddressWhenFocusLostL();
		}
	return iTextEditor->GetAddressesL();
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::EmailAddressObjectBySelection()
// -----------------------------------------------------------------------------
//
const CNcsEmailAddressObject* CNcsAddressInputField::EmailAddressObjectBySelection() const
	{
    FUNC_LOG;
	return iTextEditor->EmailAddressObjectBySelection();
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::TextEditor()
// -----------------------------------------------------------------------------
//
CEikEdwin* CNcsAddressInputField::TextEditor() const
	{
    FUNC_LOG;
	return iTextEditor;
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::GetLineRectL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::GetLineRectL( TRect& aLineRect ) const
	{
    FUNC_LOG;
    return iTextEditor->GetLineRectL( aLineRect );
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::SetMaxLabelLength()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::SetMaxLabelLength( TInt aMaxLength )
	{
    FUNC_LOG;
	iMaximumLabelLength = aMaxLength;
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::SetTextColorL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::SetTextColorL( TLogicalRgb aColor )
	{
    FUNC_LOG;
	iTextEditor->SetTextColorL( aColor );
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::SetAddressesL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::SetAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddresses )
	{
    FUNC_LOG;
	iTextEditor->SetAddressesL( aAddresses );
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::AppendAddressesL()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::AppendAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddresses )
	{
    FUNC_LOG;
	iTextEditor->AppendAddressesL( aAddresses );
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::UpdateFontSize()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::UpdateFontSize()
	{
    FUNC_LOG;
    // <cmail> Platform layout change
    /*if ( iFont )
        {
        ControlEnv()->ScreenDevice()->ReleaseFont( iFont );
        iFont = NULL;
        }

	TNcsMeasures m = NcsUtility::Measures();
	if ( !iFont )
	    {
	    TRAP_IGNORE( iFont = NcsUtility::GetNearestFontL(EAknLogicalFontPrimarySmallFont, m.iLabelFontHeightPx) );
	    }*/
    CCoeControl* control = iButton ? ( CCoeControl* )iButton : ( CCoeControl* )iLabel;
	iFont = NcsUtility::GetLayoutFont( control->Rect(), NcsUtility::ENcsHeaderCaptionFont );
	if( control == iButton )
	    {
	    iButton->SetTextFont( iFont );
	    }
	else
	    {
	    iLabel->SetFont( iFont );
	    }
    iTextEditor->UpdateFontSize();
    // </cmail> Platform layout change
	}

// -----------------------------------------------------------------------------
// CNcsAddressInputField::HandleResourceChange()
// -----------------------------------------------------------------------------
//
void CNcsAddressInputField::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikMessageColorSchemeChange )
        {
        UpdateFontSize();
        // <cmail> Platform layout change
        iTextEditor->HandleResourceChange( aType ); // CreateScrollBarFrameL()->DrawScrollBarsNow();
        // </cmail> Platform layout change
        }
    }

void CNcsAddressInputField::FixSemicolonAtTheEndL()
	{
	FUNC_LOG;

    if ( 0 != iTextEditor->TextLength() &&
         !iTextEditor->HasSemicolonAtTheEndL() )
        {
        CRichText* rt = iTextEditor->RichText();
        if ( rt )
            {
            rt->InsertL( rt->DocumentLength(), _L(";") );
            iTextEditor->SetCursorPosL( rt->DocumentLength(), EFalse );
            iTextEditor->CheckAddressWhenFocusLostL();
            }
        }
	}

void CNcsAddressInputField::EnableKineticScrollingL( CAknPhysics* aPhysics )
    {
	iPhysics = aPhysics;
    iTextEditor->EnableKineticScrollingL( aPhysics );
    iTextEditor->TextLayout()->RestrictScrollToTopsOfLines( ETrue );
    }

void CNcsAddressInputField::SetPhysicsEmulationOngoing( TBool aPhysOngoing )
    {
    iTextEditor->SetPhysicsEmulationOngoing( aPhysOngoing );
    }
