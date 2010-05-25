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
#include <s32mem.h>
#include <txtrich.h>
#include <AknIconUtils.h>
#include <StringLoader.h>
#include <avkon.hrh>
#include <AknsDrawUtils.h>
#include <aknview.h>
#include <AknUtils.h>
#include <FreestyleEmailUi.rsg>
#include <eikclbd.h>
#include <aknViewAppUi.h>
#include <aknnotewrappers.h> //CAknInformationNote
#include <txtetext.h>
#include <aknphysics.h>

#include "ncscomposeviewcontainer.h"
#include "ncsheadercontainer.h"
#include "ncsaddressinputfield.h"
#include "ncssubjectfield.h"
#include "ncsconstants.h"
#include "ncseditor.h"
#include "ncspopuplistbox.h"
#include "ncscomposeview.h"
#include "ncsutility.h"
#include "ncsemailaddressobject.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiSendAttachmentsListControl.h"
#include "FreestyleEmailUiSendAttachmentsListVisualiser.h"
#include "FreestyleEmailUiSendAttachmentsListModel.h"
#include "FreestyleEmailUi.hrh"
#include "FSAutoSaver.h"
#include "FreestyleEmailUiCLSItem.h"
#include "FSAsyncTextFormatter.h"

const TInt KHeaderVisibilityThreshold = -100;

// ========================= MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CNcsComposeViewContainer::CNcsComposeViewContainer(
    CNcsComposeView& aView,
    CFsAutoSaver& aAutoSaver,
	CFSMailBox& aMailBox ):
	iView( aView ), iAmountMesFieldMovedUp( 0 ),
	iAutoSaver( aAutoSaver ), iMailBox( aMailBox )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNcsComposeViewContainer* CNcsComposeViewContainer::NewL( 
    CNcsComposeView& aView, const TRect& aRect, CFSMailBox& aMailBox, 
    CFsAutoSaver& aAutoSaver, TInt aFlags )
    {
    FUNC_LOG;

    CNcsComposeViewContainer* self = new ( ELeave ) CNcsComposeViewContainer(
        aView, aAutoSaver, aMailBox );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aFlags );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::ConstructL( const TRect& aRect, TInt aFlags )
    {
    FUNC_LOG;

    SetMopParent( &iView );
    CreateWindowL();

    if ( CAknPhysics::FeatureEnabled() )
        {
        iPhysics = CAknPhysics::NewL( *this, this );
        }

    // Setup the control array
    InitComponentArrayL();
    CCoeControlArray& controls = Components();
    controls.SetControlsOwnedExternally( ETrue );

    // Create the header field
    iHeader = CNcsHeaderContainer::NewL( *this, iMailBox, aFlags, iPhysics );
    controls.AppendLC( iHeader );
    CleanupStack::Pop( iHeader );

    // Create 'message body' field, default text is empty for that field
    iMessageField = new ( ELeave ) CNcsEditor( this, EFalse, ENcsEditorBody );
    iMessageField->SetContainerWindowL( *this );
    iMessageField->ConstructL( this, 0, 0 );
    iMessageField->SetAlignment( EAknEditorAlignBidi );
    iMessageField->SetAknEditorInputMode( EAknEditorTextInputMode );
    iMessageField->EnableKineticScrollingL( iPhysics );
    controls.AppendLC( iMessageField );
    CleanupStack::Pop( iMessageField );

    iMessageField->SetNcsFontType( NcsUtility::ENcsBodytextFont );

    // Listen edwin events and size events from message body field
    iMessageField->AddEdwinObserverL( this );
    iMessageField->SetEdwinSizeObserver( this );

    // Create read-only field to be used for original message quote when
    // protocol doesn't allow editing the original message
    iReadOnlyQuoteField = new( ELeave ) CNcsEditor( this, EFalse );
    iReadOnlyQuoteField->SetContainerWindowL( *this );
    iReadOnlyQuoteField->ConstructL( this, 0, 0 );
    iReadOnlyQuoteField->SetAlignment( EAknEditorAlignBidi );
    iReadOnlyQuoteField->SetReadOnly( ETrue );
    controls.AppendLC( iReadOnlyQuoteField );
    CleanupStack::Pop( iReadOnlyQuoteField );

    // Listen edwin events and size events from smart quote field
    iReadOnlyQuoteField->AddEdwinObserverL( this );
    iReadOnlyQuoteField->SetEdwinSizeObserver( this );
   iReadOnlyQuoteField->EnableKineticScrollingL( iPhysics );

    // Create the scroll bar
    iScrollBar = new( ELeave ) CAknDoubleSpanScrollBar( this );
    iScrollBar->ConstructL( ETrue, this, this, CEikScrollBar::EVertical,
        aRect.Height() );
    iScrollBar->MakeVisible( EFalse );
    iScrollBar->SetModelL( &iScrollBarModel );
    controls.AppendLC( iScrollBar );
    CleanupStack::Pop( iScrollBar );

    iMessageField->SetupEditorL();
    iReadOnlyQuoteField->SetupEditorL();

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, Rect(), EFalse );

    SetRect( aRect );

    iLongTapDetector = CAknLongTapDetector::NewL( this );

    // activate auto save functionality
    iAutoSaver.Enable( ETrue );
    }

// ---------------------------------------------------------------------------
// CNcsComposeViewContainer::StopAsyncTextFormatter
// ---------------------------------------------------------------------------
//
void CNcsComposeViewContainer::StopAsyncTextFormatter()
    {
    if ( iAsyncTextFormatter )
        {
        iAsyncTextFormatter->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CNcsComposeViewContainer::~CNcsComposeViewContainer
// Destructor
// ---------------------------------------------------------------------------
//
CNcsComposeViewContainer::~CNcsComposeViewContainer()
    {
    FUNC_LOG;
    
    if ( iAsyncTextFormatter )
        {
        iAsyncTextFormatter->Cancel();
        delete iAsyncTextFormatter;
        }
    
    if ( iContactHandler )
        {
        iContactHandler->ClearObservers();
        }
	delete iHeader;
    delete iScrollBar;
    delete iMessageField;
    delete iBgContext;
	delete iLongTapDetector;
    delete iReadOnlyQuoteField;
    delete iReadOnlyQuote;

    delete iPhysics;
    iPhysics = NULL;
    }


// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::FocusChanged
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::FocusChanged( TDrawNow aDrawNow )
    {
    FUNC_LOG;

	if ( iFocused != iHeader &&
	     iFocused != iMessageField &&
	     iFocused != iReadOnlyQuoteField )
        {
		iFocused = iHeader;
        }
    if ( iFocused == iHeader )
        {
        iHeader->ShowCursor( IsFocused() );
        iFocused->SetFocus( ETrue, aDrawNow );
        }
    else if ( iFocused == iMessageField )
        {
        iFocused->SetFocus( IsFocused(), aDrawNow );
        }
    else 
        {
        iFocused->SetFocus( ETrue, aDrawNow );
        }

    iView.HandleContainerChangeRequiringToolbarRefresh();

	if ( aDrawNow )
	    {
		DrawNow();
	    }

    }
// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetMskL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetMskL()
    {
    FUNC_LOG;
    iHeader->SetMskL();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandlePointerEventL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HandlePointerEventL( 
        const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;
    if ( aPointerEvent.iType != TPointerEvent::EButton1Down &&
         iHeader->NeedsLongTapL( aPointerEvent.iPosition ) )
        {
    	iLongTapDetector->PointerEventL( aPointerEvent );
        }
	   
    switch( aPointerEvent.iType )
    	{
    	case TPointerEvent::EButton1Down:
    		{
            // Save current and original position so that those can be used in
            // drag/scrolling calculations
            iPreviousPosition = iOriginalPosition = 
                aPointerEvent.iPosition;
            iIsDragging = EFalse;
            iIsFlicking = EFalse;

            if( iPhysics )
                {
                iPhysics->StopPhysics();
                iPhysics->ResetFriction();
                iStartTime.HomeTime();
                UpdatePhysicsL();
                }            

            if ( iHeader->Rect().Contains( aPointerEvent.iPosition ) )
                {
                if ( iFocused == iMessageField )
                    {
					if( iMessageField->SelectionLength() )
						{
						iMessageField->ClearSelectionL();
						}
                    iMessageField->SetFocus( EFalse, ENoDrawNow );
                    iFocused = iHeader;
                    iHeader->SetFocus( ETrue,ENoDrawNow );
                    }
                else if ( iFocused == iReadOnlyQuoteField )
                    {
                    iReadOnlyQuoteField->SetFocus( EFalse, ENoDrawNow );
                    iFocused = iHeader;
                    iHeader->SetFocus( ETrue, ENoDrawNow );
                    }
                }
            else if ( iReadOnlyQuoteField->Rect().Contains( 
                        aPointerEvent.iPosition ) )
                {
                if ( iFocused == iMessageField )
                    {
                    iMessageField->SetFocus( EFalse, ENoDrawNow );
                    iFocused = iReadOnlyQuoteField;
                    iReadOnlyQuoteField->SetFocus( ETrue, ENoDrawNow );
                    iReadOnlyQuoteField->SetCursorPosL( 0, EFalse );
                    }
                else if ( iFocused == iHeader )
                    {
                    iHeader->SetFocus( EFalse, ENoDrawNow );
                    iFocused = iReadOnlyQuoteField;
                    iReadOnlyQuoteField->SetFocus( ETrue, ENoDrawNow );
                    iReadOnlyQuoteField->SetCursorPosL( 0, EFalse );
                    }
                }
            else 
                {
                if ( iFocused == iHeader )
                    {
                    iHeader->SetFocus( EFalse, ENoDrawNow );
                    iFocused = iMessageField;
                    iMessageField->SetFocus( ETrue, ENoDrawNow );
                    }
                else if ( iFocused == iReadOnlyQuoteField )
                    {
                    iReadOnlyQuoteField->SetFocus( EFalse, ENoDrawNow );
                    iFocused = iMessageField;
                    iMessageField->SetFocus( ETrue, ENoDrawNow );
                    }
                else
                    {
                    iMessageField->ClearSelectionL();
                    }
                }

			break;
    		}
    	
    	case TPointerEvent::EButton1Up:    		
    		{
            if( iIsDragging && iPhysics )
                {
                TPoint drag( iOriginalPosition - aPointerEvent.iPosition );
                iPhysics->StartPhysics( drag, iStartTime );
                iIsFlicking = ETrue;
                iIsDragging = EFalse;
                }
    		break;
    		}
    	
    	case TPointerEvent::EDrag:
    		{
            if ( !iIsDragging )
                {
                TInt drag( iOriginalPosition.iY - aPointerEvent.iPosition.iY );
                if ( Abs( drag ) > iPhysics->DragThreshold() )
                    {
					// If user started dragging, cancel hotspot actions
					iIsDragging = ETrue;
					iMessageField->SetPhysicsEmulationOngoing( ETrue );
					iReadOnlyQuoteField->SetPhysicsEmulationOngoing( ETrue );
					iHeader->SetPhysicsEmulationOngoing( ETrue );
                    }
                }

            // Get current pointer position
            TPoint position = aPointerEvent.iPosition;


           if( iPhysics )
               {
			   if ( iIsDragging )
				   {
                    TPoint delta( 0, iPreviousPosition.iY - position.iY );
                    if ( !iMessageField->SelectionLength() )
                    		iPhysics->RegisterPanningPosition( delta );
				   }
                }
            else
                {
    			TInt topPosition( -iHeader->Position().iY );
    						
    			TInt totalHeight( ContentTotalHeight() );

    			TInt areaHeight( Rect().Size().iHeight );
    			TInt scrollOffset( 0 );
    			
				if( totalHeight > areaHeight )
					{
					// Calculate new scroll offset based on current and
					// previous Y-positions
					scrollOffset = topPosition + 
                        ( iPreviousPosition.iY - position.iY );
					// Ensure that thumb position is in correct range
					scrollOffset = Max( scrollOffset, 0 );
					scrollOffset = Min( scrollOffset, 
                                        totalHeight - areaHeight );
					}

				Scroll( scrollOffset );
				}
           
			// Save current position as previous pos for future calculations
			iPreviousPosition = position;  
			
			break;
    		}
    		
    	default:
    		{
    		// unknown event, ignored
    		break;
    		}
    	}
    	
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down &&
         iHeader->NeedsLongTapL( aPointerEvent.iPosition ) )
        {
        iLongTapDetector->PointerEventL( aPointerEvent );
        }

    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::OfferKeyEventL()
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CNcsComposeViewContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;

    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }

    TKeyResponse ret( EKeyWasNotConsumed );

    //we are allowing to change MSK label 
    SwitchChangeMskOff( EFalse );
    
    if ( aKeyEvent.iCode == EKeyYes )
        {
        // check that at least one of the address fields has an address
        if ( !iHeader->AreAddressFieldsEmpty() )
            {
            iView.HandleCommandL( ENcsCmdSend );
            }
        else
            {
            AppendAddressesL();
            }
        ret = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyOK )
    	{
        if ( iHeader->IsPopupActive() )
            {
            DoPopupSelectL();
            ret = EKeyWasConsumed;
            }
        else if ( AppendAddressesL() )
            {
            ret = EKeyWasConsumed;
            }
        }
    else if ( aKeyEvent.iScanCode == 'S' &&
              aKeyEvent.iModifiers & EModifierCtrl &&
              aType == EEventKey )
        {
        iView.HandleCommandL( ENcsCmdSaveDraft );
        ret = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyUpArrow || 
              aKeyEvent.iCode == EKeyDownArrow )
        {
        ret = iFocused->OfferKeyEventL( aKeyEvent, aType );

        if ( ret == EKeyWasNotConsumed )
            {
            ret = ChangeFocusL( aKeyEvent );
            }

        if ( ret != EKeyWasConsumed )
            {
            DrawDeferred();
            }
        }

    if ( ret == EKeyWasNotConsumed )
        {
        if ( iFocused )
            ret = iFocused->OfferKeyEventL( aKeyEvent, aType );

        iView.HandleContainerChangeRequiringToolbarRefresh();
        
        // Report user activity to auto saver if editor field handled the
        // event. In case of message field (body text), EKeyWasConsumed
        // condition does not come true because AknFep consumes EKeyEvent
        // and calls HandleEdwinEventL. This OfferKeyEventL gets only
        // EEventKeyUp and EEventKeyDown events. Thus, when typing body text,
        // autosaver is notified in HandleEdwinEventL()
        if ( ( ret == EKeyWasConsumed ) && ( iFocused != iMessageField ) )
            {
            iAutoSaver.ReportActivity();
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::UpdateScreenPositionL()
// If the user scrolled down in the message field
// we want to scroll up the header and grow the message field.
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::UpdateScreenPositionL( 
        const TUint& /*aKeyCode*/ )
    {
    FUNC_LOG;

    // get screen rectangle
    const TRect screenRect( Rect() );

    TPoint msgPos( iMessageField->Position() );
    TPoint quotePos( iReadOnlyQuoteField->Position() );

    // find out cursor position in absolute coordinates
    TRect lineRect;
    if ( iFocused == iMessageField )
        {
        iMessageField->GetLineRectL( lineRect );
        lineRect.Move( msgPos );
        }
    else if ( iFocused == iReadOnlyQuoteField )
        {
        iReadOnlyQuoteField->GetLineRectL( lineRect );
        lineRect.Move( quotePos );
        }
    else
        {
        // This function should be used only when focus is in body or
        // read-only quote
        return;
        }
    TPoint linePos = lineRect.iTl;

    TInt minTargetY = lineRect.Height();
    TInt maxTargetY = screenRect.Height() - lineRect.Height() * 2;

    TInt moveY = 0;
    // if cursor goes out of screen then move the controls
    if ( linePos.iY > maxTargetY )
        {
        moveY = maxTargetY - linePos.iY;
        }
    else if ( linePos.iY < minTargetY )
        {
        moveY = minTargetY - linePos.iY;
        }

    // Check we don't scroll too low
    if ( quotePos.iY + moveY + iReadOnlyQuoteField->Size().iHeight < 
         screenRect.Height() )
        {
        moveY = screenRect.Height() - quotePos.iY - 
            iReadOnlyQuoteField->Size().iHeight;
        }

    // Check we don't scroll too high
    TPoint headerPos = iHeader->Position();
    if ( headerPos.iY + moveY > 0 )
        {
        moveY = -headerPos.iY;
        }

    if ( moveY )
        {
        headerPos.iY += moveY;
        iHeader->SetPosition( headerPos );

        msgPos.iY += moveY;
        iMessageField->SetPosition( msgPos );

        quotePos.iY += moveY;
        iReadOnlyQuoteField->SetPosition( quotePos );

        iSeparatorLineYPos += moveY;
        UpdateScrollBar();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::UpdateFieldPosition
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::UpdateFieldPosition( CCoeControl* aAnchor )
    {
    FUNC_LOG;

    // We may not be completely constructed
    if ( iHeader && iMessageField && iReadOnlyQuoteField )
        {
        // First, move all the controls in the header
        iHeader->UpdateFieldPosition( aAnchor );

        // Then, move the body field below the header
        TRect headerRect = iHeader->Rect();

        const TSize separatorSize( 
            NcsUtility::SeparatorSizeInThisResolution() );
        iSeparatorLineYPos = headerRect.iBr.iY;

        TPoint bodyPos( iMessageField->Position() );
        bodyPos.iY = iSeparatorLineYPos + separatorSize.iHeight;
        iMessageField->SetPosition( bodyPos );

        TPoint quotePos( iReadOnlyQuoteField->Position() );
        quotePos.iY = iMessageField->Rect().iBr.iY;
        iReadOnlyQuoteField->SetPosition( quotePos );

        UpdateScrollBar();
        DrawDeferred();
        }

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ChangeFocusL()
// Handles key events
// -----------------------------------------------------------------------------
//

TKeyResponse CNcsComposeViewContainer::ChangeFocusL( 
        const TKeyEvent& aKeyEvent )
    {
    FUNC_LOG;

    TKeyResponse ret( EKeyWasNotConsumed );

    if ( iFocused == iReadOnlyQuoteField && aKeyEvent.iCode == EKeyUpArrow )
        {
        iFocused = iMessageField;
        iReadOnlyQuoteField->SetFocus( EFalse, ENoDrawNow );
        iMessageField->SetFocus( ETrue, ENoDrawNow );
        ret = EKeyWasConsumed;
        }
    else if ( iFocused == iMessageField )
        {
        if ( aKeyEvent.iCode == EKeyUpArrow )
            {
            iFocused = iHeader;
            iHeader->SetFocus( ETrue, ENoDrawNow );
            iMessageField->SetFocus( EFalse, ENoDrawNow );
            iHeader->MakeVisible( ETrue );
            iHeader->ShowCursor( ETrue );
            CommitL( EBodyField );
            ret = EKeyWasConsumed;
            }
        else if ( aKeyEvent.iCode == EKeyDownArrow && 
                  iReadOnlyQuoteField->TextLength() )
            {
            // Focus move to read-only quote only if there is some text in it.
            iFocused = iReadOnlyQuoteField;
            iReadOnlyQuoteField->SetFocus( ETrue, ENoDrawNow );
            iMessageField->SetFocus( EFalse, ENoDrawNow );
            iReadOnlyQuoteField->SetCursorPosL( 0, EFalse );
            ret = EKeyWasConsumed;
            }
        }
    else if ( iFocused == iHeader && aKeyEvent.iCode == EKeyDownArrow )
        {
        iFocused = iMessageField;
        iHeader->SetFocus( EFalse, ENoDrawNow );
        iMessageField->SetFocus( ETrue, ENoDrawNow );
        iMessageField->SetCursorPosL( 0, EFalse );
        ret = EKeyWasConsumed;
        }

    if ( ret == EKeyWasConsumed )
        {
        UpdateScreenPositionL( aKeyEvent.iCode );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::UpdateFieldSizeL()
// This is called through MNcsFieldSizeObserver when the rects have changed
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::UpdateFieldSizeL( TBool aDoScroll )
    {
    FUNC_LOG;

    // We may not have finished construction
    if ( iHeader && iMessageField && iReadOnlyQuoteField && !aDoScroll )
        {
        SizeChanged();
        }
    if ( iHeader && aDoScroll )
        {
        iHeader->DoScroll();
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SizeChanged()
// set size
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SizeChanged()
    {
    FUNC_LOG;

    const TRect rect( Rect() );
    iScrollBar->SetRect( NcsUtility::ListCmailScrollbarRect( rect ) );
    TRect cmailPaneRect( NcsUtility::ListCmailPaneRect( rect ) );

    const TInt headerLineCount( iHeader->LayoutLineCount() );
    const TPoint headerPos( 
            NcsUtility::HeaderControlPosition( cmailPaneRect, 0 ) );
    cmailPaneRect.Move( 0, iHeader->Position().iY - headerPos.iY );
    iCmailPaneRect = cmailPaneRect;

    NcsUtility::LayoutHeaderControl( 
            iHeader, cmailPaneRect, 0, headerLineCount );

    // do not used stored headerLineCount, because relayout above may grow or
    // shrink edit fields
    NcsUtility::LayoutBodyEdwin( 
            iMessageField, cmailPaneRect, iHeader->LayoutLineCount(), 
            iMessageField->LineCount(), iSeparatorLineYPos );
    
    // we don't need format again when format was already done
    // during creation of forward/reply message
    if ( !iSwitchOffFormattingText )
        {
        TRAP_IGNORE( iMessageField->FormatAllTextNowL() );
        }

    iMessageField->UpdateFontSize();

    TInt readOnlyQuoteFieldHeight( 0 );
    if ( iReadOnlyQuoteField->TextLength() )
        {
        TInt dummySeparatorPos;
        NcsUtility::LayoutBodyEdwin(
                iReadOnlyQuoteField, cmailPaneRect, 
                iHeader->LayoutLineCount() + iMessageField->LineCount(), 
                iReadOnlyQuoteField->LineCount(), dummySeparatorPos );
        // we don't need format again when format was already done
		// during creation of forward/reply message
        if ( !iSwitchOffFormattingText )
            {
            TRAP_IGNORE( iReadOnlyQuoteField->FormatAllTextNowL() );
            }
        iReadOnlyQuoteField->UpdateFontSize();
        readOnlyQuoteFieldHeight = iReadOnlyQuoteField->Rect().Height();
        }

    iBgContext->SetRect( Rect() );
    iMessageField->SetRealRect( Rect() );
    iReadOnlyQuoteField->SetRealRect( Rect() );

    TInt messageLineHeigth = 
        NcsUtility::HeaderCaptionPaneRect( cmailPaneRect ).Height();
    iSeparatorHeight = NcsUtility::SeparatorSizeInThisResolution().iHeight;
    iMessageEditorMinHeigth = 
        cmailPaneRect.Height() - iHeader->Size().iHeight -
        iSeparatorHeight -
        readOnlyQuoteFieldHeight;
    
    iMessageEditorMinHeigth -= iMessageEditorMinHeigth % messageLineHeigth;
    if ( iMessageEditorMinHeigth > messageLineHeigth )
        {
        TSize newSize( iMessageField->Size().iWidth,
                       Max( iMessageField->Size().iHeight, 
                            iMessageEditorMinHeigth ) );

        if ( !iSwitchOffFormattingText ) 
            {
            iMessageField->SetSize( newSize );
            }
        }
    else
        {
        iMessageEditorMinHeigth = messageLineHeigth;
        }

	// update some layout variables
    iHeaderHeight = iHeader->Rect().Height();
    TRect bodyRect = iMessageField->Rect();
    TRect quoteRect = iReadOnlyQuoteField->Rect();

    iTotalComposerHeight = iHeaderHeight + iSeparatorHeight * 2 + 
							   bodyRect.Height() + quoteRect.Height();
    
    iVisibleAreaHeight = Rect().Height();
        
    UpdateScrollBar();

    iScrollBarModel.SetScrollSpan( iTotalComposerHeight );
    iScrollBarModel.SetWindowSize( iVisibleAreaHeight );
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::Draw() const
// Draws the display
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;

    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( skin )
        {
        AknsDrawUtils::Background( skin, iBgContext, this, gc, aRect );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetBodyContentL()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetBodyContentL( const TDesC& aMessage,
    const TDesC& aReadOnlyQuote )
    {
    FUNC_LOG;
    delete iReadOnlyQuote;
    iReadOnlyQuote = NULL;
    if ( aReadOnlyQuote.Length() )
        {
        iReadOnlyQuote = aReadOnlyQuote.AllocL();
        }

    RMemReadStream inputStream( aMessage.Ptr(), aMessage.Size() );
    CleanupClosePushL( inputStream );
    iMessageField->RichText()->ImportTextL( 0, inputStream,
        CPlainText::EOrganiseByParagraph );
    CleanupStack::PopAndDestroy( &inputStream );

    // Activating the field will set the control to the end of the text
    iMessageField->ActivateL();
    iMessageField->SetCursorPosL( 0, EFalse );
	iMessageField->FormatAllTextNowL();

    inputStream.Open( aReadOnlyQuote.Ptr(), aReadOnlyQuote.Size() );
    CleanupClosePushL( inputStream );
    iReadOnlyQuoteField->RichText()->ImportTextL( 0, inputStream,
        CPlainText::EOrganiseByParagraph );
    CleanupStack::PopAndDestroy( &inputStream );

    // Activating the field will set the control to the end of the text
    iReadOnlyQuoteField->ActivateL();
    iReadOnlyQuoteField->SetCursorPosL( 0, EFalse );

    // Recalculate the size of the body field
    SizeChanged();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetBodyContentAsyncL()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetBodyContentAsyncL( const TDesC& aMessage,
    const TDesC& aReadOnlyQuote )
    {
    FUNC_LOG;
    delete iReadOnlyQuote;
    iReadOnlyQuote = NULL;
    if ( aReadOnlyQuote.Length() )
        {
        iReadOnlyQuote = aReadOnlyQuote.AllocL();
        }

    RMemReadStream inputStream( aMessage.Ptr(), aMessage.Size() );
    CleanupClosePushL( inputStream );
    iMessageField->RichText()->ImportTextL( 0, inputStream,
        CPlainText::EOrganiseByParagraph );
    CleanupStack::PopAndDestroy( &inputStream );
    

    // Activating the field will set the control to the end of the text
    iMessageField->ActivateL();
    iMessageField->SetCursorPosL( 0, EFalse );
    
    iProcessedField = iMessageField;
    
	iAsyncTextFormatter = CFSAsyncTextFormatter::NewL();
    iAsyncTextFormatter->StartFormatting( iMessageField->TextLayout(), this );
        
    // callback from AO: FormatAllTextCompleteL
}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::FormatAllTextCancelled()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::FormatAllTextCancelled()
	{
	iView.SetBodyContentComplete();
	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::FormatAllTextComplete()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::FormatAllTextComplete()
	{
    if ( iReadOnlyQuote && iProcessedField == iMessageField )
        {
        iProcessedField = iReadOnlyQuoteField;
        if ( iReadOnlyQuote )
            {
			TInt dummySeparatorPos;
			NcsUtility::LayoutBodyEdwin( iReadOnlyQuoteField, iCmailPaneRect, 
					iHeader->LayoutLineCount() + iMessageField->LineCount(),
					iReadOnlyQuoteField->LineCount(), dummySeparatorPos );
	
			RMemReadStream inputStream;
            inputStream.Open( iReadOnlyQuote->Ptr(), iReadOnlyQuote->Size() );
            TRAP_IGNORE( iReadOnlyQuoteField->RichText()->ImportTextL( 0, inputStream,
                    CPlainText::EOrganiseByParagraph ) );
            inputStream.Close();
            
            // Activating the field will set the control to the end of the text
            TRAP_IGNORE( iReadOnlyQuoteField->ActivateL() );
            TRAP_IGNORE( iReadOnlyQuoteField->SetCursorPosL( 0, EFalse ) );
            
            if ( !iAsyncTextFormatter )
                {
                TRAP_IGNORE( iAsyncTextFormatter = CFSAsyncTextFormatter::NewL() );
                }
            iAsyncTextFormatter->StartFormatting( 
                    iReadOnlyQuoteField->TextLayout(), this );
            }
        }
    else
        {
        // Recalculate the size of the body field
        iSwitchOffFormattingText = ETrue;
        SizeChanged();
        iSwitchOffFormattingText = EFalse;
        
        iView.SetBodyContentComplete();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetBodyContentLC()
// -----------------------------------------------------------------------------
//
HBufC* CNcsComposeViewContainer::GetBodyContentLC()
    {
    FUNC_LOG;
    HBufC* body = NULL;

    TInt messageLength = iMessageField->TextLength();
    HBufC* message = HBufC::NewLC( messageLength );
    TPtr messagePtr = message->Des();
    iMessageField->GetText( messagePtr );

    CleanupStack::Pop();
    CleanupStack::PushL( message );
    messageLength = message->Length();

    // Append read-only quote if present
    if ( iReadOnlyQuote )
        {
        TInt quoteLength = iReadOnlyQuote->Length();
        TInt length = messageLength + KIMSLineFeed().Length() + quoteLength;
        body = message->ReAllocL( length );
        CleanupStack::Pop( message );
        CleanupStack::PushL( body );

        TPtr bodyPtr = body->Des();
        bodyPtr.Append( KIMSLineFeed() );
        bodyPtr.Append( *iReadOnlyQuote );
        }
    else
        {
        body = message;
        }

    return body;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::BodyText()
// -----------------------------------------------------------------------------
//
CRichText& CNcsComposeViewContainer::BodyText()
    {
    FUNC_LOG;
    return *iMessageField->RichText();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandleScrollEventL()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HandleScrollEventL( 
        CEikScrollBar* aScrollBar, TEikScrollEvent /*aEventType*/ )
    {
    FUNC_LOG;
    
    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }
        
    ClosePopupContactListL();

    Scroll( aScrollBar->ThumbPosition() );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::OperationCompleteL
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::OperationCompleteL( TContactHandlerCmd /*aCmd*/,
    const RPointerArray<CFSEmailUiClsItem>& aContacts )
	{
    FUNC_LOG;
	if ( aContacts.Count() )
		{
		RPointerArray<CNcsEmailAddressObject> ncsObjects;
		CleanupResetAndDestroyClosePushL( ncsObjects );

		for ( TInt i = 0 ; i < aContacts.Count() ; i++ )
			{
			CNcsEmailAddressObject* object = 
                CNcsEmailAddressObject::NewL( aContacts[i]->DisplayName(),
					aContacts[i]->EmailAddress() );
			if ( aContacts[i]->MultipleEmails() )
				{

				object->SetDisplayFull( ETrue );
				}
			CleanupStack::PushL( object );
			ncsObjects.AppendL( object );
			CleanupStack::Pop( object );
			}

		if ( iAppendAddresses == EAppendTo )
			{
			AppendToFieldAddressesL( ncsObjects );
			}
		// if Cc field is focused insert addresses in there
		else if ( iAppendAddresses == EAppendCc )
			{
			AppendCcFieldAddressesL( ncsObjects );
			}
		// if Bcc field is focused insert addresses in there
		else if ( iAppendAddresses == EAppendBcc )
			{
			AppendBccFieldAddressesL( ncsObjects );
			}

		CleanupStack::PopAndDestroy( &ncsObjects );
		}
	iView.HandleContainerChangeRequiringToolbarRefresh();
	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::OperationErrorL
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::OperationErrorL( 
        TContactHandlerCmd, TInt /*aError*/ )
	{
    FUNC_LOG;
	//Nothing to do
	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandleEdwinEventL
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HandleEdwinEventL( CEikEdwin* aEdwin,
    TEdwinEvent aEventType )
    {
    FUNC_LOG;
    if (( aEdwin == iMessageField || aEdwin == iReadOnlyQuoteField ) && 
         ( !iPhysics || iPhysics->OngoingPhysicsAction() == CAknPhysics::EAknPhysicsActionNone ))
        {
        if ( aEventType == MEikEdwinObserver::EEventTextUpdate ||
             aEventType == MEikEdwinObserver::EEventNavigation )
            {
            iAutoSaver.ReportActivity();

            // Update screen position and scroll bar when text changed
            // or cursor moved
            UpdateScreenPositionL();
            UpdateScrollBar();
            DrawDeferred();
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandleEdwinSizeEventL
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aEventType*/, TSize aDesirableEdwinSize )
    {
    FUNC_LOG;
    TBool ret = ETrue;
    
    const TRect rect( Rect() );
    TRect cmailPaneRect( NcsUtility::ListCmailPaneRect( rect ) );

    if ( aDesirableEdwinSize.iHeight < iPrevDesiredHeigth )
        {
        if ( aDesirableEdwinSize.iHeight < iMessageEditorMinHeigth )
            {
            Scroll( 0 );
            }
        }

    if ( aEdwin == iMessageField ) 
        {
        if ( aDesirableEdwinSize.iHeight >= iMessageEditorMinHeigth )
            {
            if ( !iSwitchOffFormattingText )
                {
                aEdwin->SetSize( aDesirableEdwinSize );
                }
            }        
        }
    else
        {
        if ( !iSwitchOffFormattingText )
            {
            aEdwin->SetSize( aDesirableEdwinSize );
            }
        }
    if ( aEdwin == iMessageField )
        {
        // move the quote field below the body field
        TPoint quotePos = iMessageField->Position();
        quotePos.iY += iMessageField->Size().iHeight;
        iReadOnlyQuoteField->SetPosition( quotePos );
		UpdateScreenPositionL(); 
        }

    iPrevDesiredHeigth = aDesirableEdwinSize.iHeight;

    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::MopSupplyObject
// From CCoeControl.
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CNcsComposeViewContainer::MopSupplyObject( TTypeUid aId )
    {
    FUNC_LOG;
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::UpdateScrollBar()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::UpdateScrollBar()
    {
    FUNC_LOG;

    TRect headerRect = iHeader->Rect();
    TInt visiblePosition = -headerRect.iTl.iY;

    iScrollBarModel.SetFocusPosition( visiblePosition );

    iScrollBar->SetModel( &iScrollBarModel );
    iScrollBar->MakeVisible( IsVisible() );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetFocusToMessageFieldL()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetFocusToMessageFieldL()
    {
    FUNC_LOG;

    if( iFocused != iMessageField )
        {
        iHeader->SetFocus( EFalse, ENoDrawNow );
        iMessageField->SetCursorPosL( 0, EFalse );
        iMessageField->SetFocus( ETrue, ENoDrawNow );
        iFocused = iMessageField;
        UpdateScreenPositionL();
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::AddQuickTextL()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::AddQuickTextL( const TDesC& aText )
    {
    FUNC_LOG;

    // If in 'To' field, go to msg. field
    if( !iMessageField->IsFocused() )
    	{
    	SetFocusToMessageFieldL();
    	}
	TInt pos = iMessageField->CursorPos();
	TCursorSelection selIns( pos, pos );
	iMessageField->InsertDeleteCharsL( pos, aText, selIns );
	// Fix for error EFTG-7Y63XG, moving cursor to the end of inserted text
	pos += aText.Length();
	iMessageField->SetCursorPosL( pos, EFalse );
	DrawDeferred();

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SelectAllToFieldTextL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SelectAllToFieldTextL()
	{
    FUNC_LOG;

	iHeader->SelectAllToFieldTextL();

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SelectAllCcFieldTextL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SelectAllCcFieldTextL()
	{
    FUNC_LOG;

	iHeader->SelectAllCcFieldTextL();

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SelectAllBccFieldTextL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SelectAllBccFieldTextL()
	{
    FUNC_LOG;

	iHeader->SelectAllBccFieldTextL();

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SelectAllSubjectFieldTextL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SelectAllSubjectFieldTextL()
	{
    FUNC_LOG;

	iHeader->SelectAllSubjectFieldTextL();

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsSubjectFieldEmpty
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsSubjectFieldEmpty()
    {
    FUNC_LOG;
    return iHeader->IsSubjectFieldEmpty();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::AppendAddressesL
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::AppendAddressesL()
	{
    FUNC_LOG;

    iContactHandler = CFsDelayedLoader::InstanceL()->GetContactHandlerL();
    if ( iContactHandler )
        {
        iContactHandler->GetAddressesFromPhonebookL( this );
        }
    
	if ( IsFocusCc() )
		{
		iAppendAddresses = EAppendCc;
		}
	else if ( IsFocusBcc() )
		{
		iAppendAddresses = EAppendBcc;
		}
	else
		{
		iAppendAddresses = EAppendTo;
		}

    return ETrue;
	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandleAttachmentsOpenCommand
// If attachment header field is focused and open key is pressed
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HandleAttachmentsOpenCommandL()
	{
    FUNC_LOG;

	iView.HandleCommandL( EFsEmailUiCmdOpenAttachment );

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::LaunchStylusPopupMenu
// Called when long tap happens in the attachments field
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::LaunchStylusPopupMenu( const TPoint& aPenEventScreenLocation )
    {
    FUNC_LOG;
    iView.LaunchStylusPopupMenu( aPenEventScreenLocation );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetToFieldAddressesL
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& 
CNcsComposeViewContainer::GetToFieldAddressesL( TBool aParseNow )
    {
    return iHeader->GetToFieldAddressesL( aParseNow );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetCcFieldAddressesL
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& 
CNcsComposeViewContainer::GetCcFieldAddressesL( TBool aParseNow )
    {
    return iHeader->GetCcFieldAddressesL( aParseNow );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetBccFieldAddressesL
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& 
CNcsComposeViewContainer::GetBccFieldAddressesL( TBool aParseNow )
    {
    return iHeader->GetBccFieldAddressesL( aParseNow );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetSubjectLC
//
// -----------------------------------------------------------------------------
//
HBufC* CNcsComposeViewContainer::GetSubjectLC() const
    {
    FUNC_LOG;
	return iHeader->GetSubjectLC();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsBccFieldVisible
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsBccFieldVisible() const
    {
    FUNC_LOG;
    return iHeader->IsBccFieldVisible();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsCcFieldVisible
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsCcFieldVisible() const
    {
    FUNC_LOG;
    return iHeader->IsCcFieldVisible();
    }


// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsFocusAttachments
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsFocusAttachments() const
    {
    FUNC_LOG;
    return iHeader->IsFocusAttachments();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsFocusTo
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsFocusTo() const
    {
    FUNC_LOG;
    return iHeader->IsFocusTo();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsFocusCc
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsFocusCc() const
    {
    FUNC_LOG;
    return iHeader->IsFocusCc();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsFocusBcc
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsFocusBcc() const
    {
    FUNC_LOG;
    return iHeader->IsFocusBcc();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetCcFieldLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetCcFieldLength() const
    {
    FUNC_LOG;
	return iHeader->GetCcFieldLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetBccFieldLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetBccFieldLength() const
    {
    FUNC_LOG;
	return iHeader->GetBccFieldLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetToFieldLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetToFieldLength() const
    {
    FUNC_LOG;
	return iHeader->GetToFieldLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetSubjectFieldLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetSubjectFieldLength() const
    {
    FUNC_LOG;
	return iHeader->GetSubjectFieldLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetMessageFieldLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetMessageFieldLength() const
    {
    FUNC_LOG;
	return iMessageField->TextLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetAttachmentCount
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetAttachmentCount() const
    {
    FUNC_LOG;
    TInt count = iView.AttachmentsListControl()->Model()->Count();
	return count;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HasRemoteAttachments
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::HasRemoteAttachments() const
    {
    FUNC_LOG;
    TBool ret = iView.AttachmentsListControl()->Model()->HasRemoteAttachments();
    return ret;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetLookupTextLC
//
// -----------------------------------------------------------------------------
//
HBufC* CNcsComposeViewContainer::GetLookupTextLC() const
    {
    FUNC_LOG;
	return iHeader->GetLookupTextLC();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetToFieldSelectionLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetToFieldSelectionLength() const
    {
    FUNC_LOG;
	return iHeader->GetToFieldSelectionLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetCcFieldSelectionLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetCcFieldSelectionLength() const
    {
    FUNC_LOG;
	return iHeader->GetCcFieldSelectionLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetBccFieldSelectionLength
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::GetBccFieldSelectionLength() const
    {
    FUNC_LOG;
	return iHeader->GetBccFieldSelectionLength();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetMenuBar
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetMenuBar( CEikButtonGroupContainer* aMenuBar )
    {
    FUNC_LOG;

	iHeader->SetMenuBar(aMenuBar);
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetToFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetToFieldAddressesL( 
        RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;

    iHeader->SetToFieldAddressesL(aAddress);
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetCcFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetCcFieldAddressesL( 
        RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;

    iHeader->SetCcFieldAddressesL( aAddress );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetBccFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetBccFieldAddressesL( 
        RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;

    iHeader->SetBccFieldAddressesL( aAddress );

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::AppendToFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::AppendToFieldAddressesL( RPointerArray<CNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
    FixSemicolonL();
    iHeader->AppendToFieldAddressesL(aAddresses);

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::AppendCcFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::AppendCcFieldAddressesL( RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;
    FixSemicolonL();
    iHeader->AppendCcFieldAddressesL( aAddress );

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::AppendBccFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::AppendBccFieldAddressesL( RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;
    FixSemicolonL();
    iHeader->AppendBccFieldAddressesL( aAddress );

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetSubjectL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetSubjectL( const TDesC& aSubject )
    {
    FUNC_LOG;

    iHeader->SetSubjectL( aSubject );

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetAttachmentLabelTextsLD
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetAttachmentLabelTextsLD( 
    CDesCArray* aAttachmentNames, CDesCArray* aAttachmentSizes )
    {
    FUNC_LOG;   
    iHeader->SetAttachmentLabelTextsLD( aAttachmentNames, aAttachmentSizes );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::FocusedAttachmentLabelIndex
//
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::FocusedAttachmentLabelIndex()
    {
    FUNC_LOG;   
    return iHeader->FocusedAttachmentLabelIndex();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HideAttachmentLabel
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HideAttachmentLabel()
    {
    FUNC_LOG;

	iHeader->HideAttachmentLabel();

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetCcFieldVisibleL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetCcFieldVisibleL( TBool aVisible, TBool aFocus )
	{
    FUNC_LOG;

	if ( aFocus )
		{
		iMessageField->SetCursorPosL( 0,EFalse );
		iMessageField->SetFocus( EFalse );
		iHeader->MakeVisible( ETrue );
		iHeader->SetFocus( ETrue );
		iFocused = iHeader;
		}
	iHeader->SetCcFieldVisibleL( aVisible, EDrawNow, aFocus );

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetBccFieldVisibleL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetBccFieldVisibleL( TBool aVisible, TBool aFocus )
	{
    FUNC_LOG;

	if ( aFocus )
		{
		iMessageField->SetCursorPosL( 0, EFalse );
		iMessageField->SetFocus( EFalse );
		iHeader->MakeVisible( ETrue );
		iHeader->SetFocus( ETrue );
		iFocused = iHeader;
		}
	iHeader->SetBccFieldVisibleL( aVisible, EDrawNow, aFocus );

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IncludeAddressL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::IncludeAddressL(const CNcsEmailAddressObject& eml)
    {
    FUNC_LOG;

	iHeader->IncludeAddressL(eml);

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IncludeAddressL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::IncludeAddressL()
    {
    FUNC_LOG;

	iHeader->IncludeAddressL();

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ClosePopupContactListL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::ClosePopupContactListL()
    {
    FUNC_LOG;

	iHeader->ClosePopupContactListL();

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::DeleteSelectionL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::DeleteSelectionL()
    {
    FUNC_LOG;

	iHeader->DeleteSelectionL();

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetFocusToToField
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetFocusToToField()
	{
    FUNC_LOG;

	iHeader->FocusToField();

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetFocusToAttachmentField
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetFocusToAttachmentField()
	{
    FUNC_LOG;

	if ( iFocused == iMessageField )
    	{
		iFocused = iHeader;
		iHeader->SetFocus( ETrue, EDrawNow );
		iMessageField->SetFocus( EFalse, EDrawNow );
		iHeader->MakeVisible( ETrue );
    	}
	iHeader->FocusAttachmentField();
	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::AreAddressFieldsEmpty
//
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::AreAddressFieldsEmpty()
	{
    FUNC_LOG;
	return iHeader->AreAddressFieldsEmpty();
	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::DoPopupSelectL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::DoPopupSelectL()
	{
    FUNC_LOG;

	iHeader->DoPopupSelectL();

	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandleLayoutChangeL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HandleLayoutChangeL()
    {
    FUNC_LOG;
    SetRect( iView.ClientRect() );
    if ( iHeader )
        {
        iHeader->HandleDynamicVariantSwitchL();
        }
    // force recalculation of the position of every component
    UpdateFieldPosition( NULL );
    UpdateScrollBar();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandleSkinChangeL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HandleSkinChangeL()
    {
    FUNC_LOG;
    if ( iHeader )
        {
        iHeader->HandleSkinChangeL();
        }

    if ( iMessageField )
        {
        iMessageField->UpdateFontSize();
        }

    if ( iScrollBar )
        {
        iScrollBar->HandleResourceChange( KAknsMessageSkinChange );
        iScrollBar->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ContentTotalHeight
// Return the total height of the components shown in the view
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::ContentTotalHeight()
    {
    FUNC_LOG;	
	TInt totalHeight( iHeader->Size().iHeight + 
	                  iSeparatorHeight * 2 + 
					  iMessageField->Size().iHeight );

	if( iReadOnlyQuoteField->IsVisible() )
		{
		totalHeight += iReadOnlyQuoteField->Size().iHeight;
		}	
	
	return totalHeight;
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::CommitL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::CommitL( TFieldToCommit aFieldToCommit )
    {
    FUNC_LOG;
    iView.CommitL( ETrue, aFieldToCommit );
    iView.HandleContainerChangeRequiringToolbarRefresh();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SwitchChangeMskOff
// sets up iSwitchChangeMskOff falg, which disables changes of MSK label if
// any popup dialog is open
// -----------------------------------------------------------------------------
void CNcsComposeViewContainer::SwitchChangeMskOff(TBool aTag)
    {
    FUNC_LOG;
    iHeader->SwitchChangeMskOff( aTag );
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::HandleLongTapEventL()
// Handles key events
// -----------------------------------------------------------------------------
void CNcsComposeViewContainer::HandleLongTapEventL( 
        const TPoint& aPenEventLocation, 
        const TPoint& aPenEventScreenLocation )
    {
    FUNC_LOG;
    iHeader->HandleLongTap( aPenEventLocation, aPenEventScreenLocation );
    }


void CNcsComposeViewContainer::FixSemicolonL()
	{
    FUNC_LOG;
	iHeader->FixSemicolonInAddressFieldsL();
	}

// ---------------------------------------------------------------------------
// CNcsComposeViewContainer::UpdatePhysicsL()
// ---------------------------------------------------------------------------
//
void CNcsComposeViewContainer::UpdatePhysicsL()
    {
    FUNC_LOG;
    if ( iPhysics )
        {
        const TSize viewSize( Rect().Size() );
        // We must ensure that world size is at least the size of the view
        const TSize worldSize( viewSize.iWidth, 
                Max( ContentTotalHeight(), viewSize.iHeight ) );
        iPhysics->InitPhysicsL( worldSize, viewSize, EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::Scroll()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::Scroll( TInt aTargetPos, TBool aDrawNow )
    {
    FUNC_LOG;
	TPoint headerPos( iHeader->Position() );
	TInt moveY = -headerPos.iY - aTargetPos;
	
    if ( aDrawNow )
    	{
		moveY = iTotalMoveY + moveY;
		iTotalMoveY = 0;
		if ( moveY )
			{
			headerPos.iY += moveY;
			iHeader->SetPosition( headerPos );

			// set header invisible if it is not in visible area
			// this is done to prevent drawing of header when it is not necessary
            if ( headerPos.iY + iHeaderHeight <= KHeaderVisibilityThreshold && iHeader->IsVisible() )
                {            
                iHeader->MakeVisible( EFalse );
                }
            // set header visible if it is in visible area
            else if ( headerPos.iY + iHeaderHeight > KHeaderVisibilityThreshold && !iHeader->IsVisible() ) 
                {
                iHeader->MakeVisible( ETrue );
                }
    
			TPoint msgPos( iMessageField->Position() );
			msgPos.iY += moveY;
			iMessageField->SetPosition( msgPos );
	
			if( iReadOnlyQuoteField->IsVisible() )
				{
				TPoint readOnlyPos( iReadOnlyQuoteField->Position() );
				readOnlyPos.iY += moveY;
				iReadOnlyQuoteField->SetPosition( readOnlyPos );
				}
	
			iSeparatorLineYPos += moveY;

			UpdateScrollBar();
			DrawDeferred();
			}
    	}
    else
    	{
		iTotalMoveY += moveY;
    	}    	
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ViewPositionChanged
// From MAknPhysicsObserver
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::ViewPositionChanged(
    const TPoint& aNewPosition, TBool aDrawNow, TUint /*aFlags*/ )
    {
    FUNC_LOG;
    TInt scrollOffset = aNewPosition.iY - iVisibleAreaHeight / 2;

    // when the composer view is overlapped by other view for instance task switcher or screensaver
    // physics sends a faulty event to move the composer view down. 
    // This action is ignored here.	
    if (aNewPosition.iY != 0)
        {
        Scroll( scrollOffset, aDrawNow );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::PhysicEmulationEnded
// From MAknPhysicsObserver
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::PhysicEmulationEnded()
    {
    FUNC_LOG;
    iIsFlicking = EFalse;
    iMessageField->SetPhysicsEmulationOngoing( EFalse );
    iReadOnlyQuoteField->SetPhysicsEmulationOngoing( EFalse );
    iHeader->SetPhysicsEmulationOngoing( EFalse );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ViewPosition
// From MAknPhysicsObserver
// -----------------------------------------------------------------------------
//
TPoint CNcsComposeViewContainer::ViewPosition() const
    {
    FUNC_LOG;
    return TPoint(0, -iHeader->Position().iY  + iVisibleAreaHeight / 2 );
    }


// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::IsRemoteSearchInprogress
// -----------------------------------------------------------------------------
//
TBool CNcsComposeViewContainer::IsRemoteSearchInprogress() const
    {
    FUNC_LOG;
    return iHeader->IsRemoteSearchInprogress();
    }
