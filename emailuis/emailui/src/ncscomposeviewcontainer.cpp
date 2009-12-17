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
CNcsComposeViewContainer* CNcsComposeViewContainer::NewL( CNcsComposeView& aView,
    const TRect& aRect, CFSMailBox& aMailBox, CFsAutoSaver& aAutoSaver,
    TInt aFlags )
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
	// <cmail>
    // iApplicationRect = aRect;
	// </cmail>

    // Setup the control array
    InitComponentArrayL();
    CCoeControlArray& controls = Components();
    controls.SetControlsOwnedExternally( ETrue );

    // Create the header field
    iHeader = CNcsHeaderContainer::NewL( *this, iMailBox, aFlags );
    controls.AppendLC( iHeader );
    CleanupStack::Pop( iHeader );

    // Create 'message body' field, default text is empty for that field
    iMessageField = new ( ELeave ) CNcsEditor( this, EFalse, ENcsEditorBody );
    iMessageField->SetContainerWindowL( *this );
    iMessageField->ConstructL( this, 0, 0 );
    iMessageField->SetAlignment( EAknEditorAlignBidi );
    iMessageField->SetAknEditorInputMode( EAknEditorTextInputMode );
    controls.AppendLC( iMessageField );
    CleanupStack::Pop( iMessageField );

    // <cmail> Platform layout change
    iMessageField->SetNcsFontType( NcsUtility::ENcsBodytextFont );
    // </cmail>

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

    // Create the scroll bar
    iScrollBar = new( ELeave ) CAknDoubleSpanScrollBar( this );
    iScrollBar->ConstructL( this, this, CEikScrollBar::EVertical, aRect.Height() );
    iScrollBar->MakeVisible( ETrue );
    iScrollBar->SetModelL( &iScrollBarModel );

    iMessageField->SetupEditorL();
    iReadOnlyQuoteField->SetupEditorL();

    SetRect( aRect );

    //<cmail> S60 skin support
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, Rect(), EFalse );
    // <cmail>
	//iMessageField->SetSkinBackgroundControlContextL( iBgContext );
	// </cmail>
    iLongTapDetector = CAknLongTapDetector::NewL( this );
    //</cmail>

    UpdateScrollBarL();

    ActivateL();

    // activate auto save functionality
    iAutoSaver.Enable( ETrue );

    if ( CAknPhysics::FeatureEnabled() )
        {
        iPhysics = CAknPhysics::NewL( *this, this );
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
    if ( iContactHandler )
        {
        iContactHandler->ClearObservers();
        }
	delete iHeader;
    delete iScrollBar;
    delete iMessageField;
    // <cmail>
    delete iBgContext;
	delete iLongTapDetector;
    // </cmail>
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
void CNcsComposeViewContainer::FocusChanged(TDrawNow aDrawNow)
    {
    FUNC_LOG;

	if ( iFocused != iHeader &&
	     iFocused != iMessageField &&
	     iFocused != iReadOnlyQuoteField )
        {
		iFocused = iHeader;
        }
	iFocused->SetFocus( ETrue, aDrawNow );
	// <cmail>
	iView.HandleContainerChangeRequiringToolbarRefresh();
	// </cmail>
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

//<cmail>

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::HandlePointerEventL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
	FUNC_LOG;

    CCoeControl::HandlePointerEventL( aPointerEvent );

    if ( iHeader->NeedsLongTapL( aPointerEvent.iPosition ) )
        {
    	iLongTapDetector->PointerEventL( aPointerEvent );
        }
	   
    switch( aPointerEvent.iType )
    	{
    	case TPointerEvent::EButton1Down:
    		{
            // Save current and original position so that those can be used in
            // drag/scrolling calculations
            iPreviousPosition = iOriginalPosition = aPointerEvent.iParentPosition;
            iIsDragging = EFalse;
            iIsFlicking = EFalse;

            if( iPhysics )
                {
                iPhysics->StopPhysics();
                iPhysics->ResetFriction();
                iStartTime.HomeTime();
                UpdatePhysicsL();
                }            

            
			//Closes the attachment action menu, if it is open.
			//Otherwise does nothing
			iView.DismissAttachmentActionMenuL();

            if ( iHeader->Rect().Contains(aPointerEvent.iPosition) )
                {
                if (iFocused == iMessageField)
                    {
                    iFocused = iHeader;
                    iHeader->SetFocus( ETrue,EDrawNow );
                    iMessageField->SetFocus( EFalse, EDrawNow );
                    iHeader->MakeVisible( ETrue );
                    CommitL(EBodyField);
                    iView.HandleContainerChangeRequiringToolbarRefresh();
                    }
                else if (iFocused == iReadOnlyQuoteField)
                    {
                    iFocused = iMessageField;
                    iReadOnlyQuoteField->SetFocus( EFalse, EDrawNow );
                    iHeader->SetFocus( ETrue, EDrawNow );
                    iView.HandleContainerChangeRequiringToolbarRefresh();
                    }
                }
            else if (iReadOnlyQuoteField->Rect().Contains(aPointerEvent.iPosition))
                {
                if (iFocused == iMessageField)
                    {
                    iFocused = iReadOnlyQuoteField;
                    iReadOnlyQuoteField->SetFocus( ETrue, EDrawNow );
                    iMessageField->SetFocus( EFalse, EDrawNow );
                    iReadOnlyQuoteField->SetCursorPosL( 0, EFalse );
                    iView.HandleContainerChangeRequiringToolbarRefresh();
                    }
                else if (iFocused == iHeader)
                    {
                    iFocused = iReadOnlyQuoteField;
                    iReadOnlyQuoteField->SetFocus( ETrue, EDrawNow );
                    iHeader->SetFocus( EFalse, EDrawNow );
                    iReadOnlyQuoteField->SetCursorPosL( 0, EFalse );
                    iView.HandleContainerChangeRequiringToolbarRefresh();
                    }
                }
            else 
                {
                if (iFocused == iHeader)
                    {
                    iFocused = iMessageField;
                    iHeader->SetFocus( EFalse, EDrawNow );
                    iMessageField->SetFocus( ETrue, EDrawNow );
                    //iMessageField->SetCursorPosL( iMessageField->TextLength(), EFalse );
                    iView.HandleContainerChangeRequiringToolbarRefresh();
                    }
                else if (iFocused == iReadOnlyQuoteField)
                    {
                    iFocused = iMessageField;
                    iReadOnlyQuoteField->SetFocus( EFalse, EDrawNow );
                    iMessageField->SetFocus( ETrue, EDrawNow );
                    iView.HandleContainerChangeRequiringToolbarRefresh();
                    }
                else 
                    {
                    //iMessageField->SetCursorPosL( iMessageField->TextLength(), EFalse );
                    const TRect messageFieldRect(iMessageField->Rect());
                    if (!messageFieldRect.Contains(aPointerEvent.iPosition) && 
                            aPointerEvent.iPosition.iY >= messageFieldRect.iTl.iY ) 
                        {
                        iMessageField->OpenVirtualKeyBoardL();
                        }
                    }
                }

			break;
    		}
    	
    	case TPointerEvent::EButton1Up:    		
    		{
            if( iIsDragging && iPhysics )
                {
                TPoint drag(iOriginalPosition - aPointerEvent.iParentPosition);
                iPhysics->StartPhysics(drag, iStartTime);
                iIsFlicking = ETrue;
                iIsDragging = EFalse;
                }
    		break;
    		}
    	
    	case TPointerEvent::EDrag:
    		{
    		// Get current pointer position
    		TPoint position = aPointerEvent.iParentPosition;

    		// If user started dragging, cancel hotspot actions
            iIsDragging = ETrue;

           if( iPhysics )
                {
                TPoint delta( 0, iPreviousPosition.iY - position.iY );
                iPhysics->RegisterPanningPosition( delta );
                }
            else
                {
    			TInt topPosition( -iHeader->Position().iY );
    						
    			TInt totalHeight( ContentTotalHeight() );

    			TInt areaHeight( Rect().Size().iHeight );
    			TInt scrollOffset( 0 );
    			
				if( totalHeight > areaHeight )
					{
					// Calculate new scroll offset based on current and previous Y-positions
					scrollOffset = topPosition + ( iPreviousPosition.iY - position.iY );
					// Ensure that thumb position is in correct range
					scrollOffset = Max( scrollOffset, 0 );
					scrollOffset = Min( scrollOffset, totalHeight - areaHeight );
					}

				ScrollL( scrollOffset );
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
    
    }

//</cmail>

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::OfferKeyEventL()
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CNcsComposeViewContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;

    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }

    TKeyResponse ret( EKeyWasNotConsumed );

    //<cmail>
    //we are allowing to change MSK label 
    SwitchChangeMskOff(EFalse);
    //</cmail>
    
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
    else if ( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        ret = iFocused->OfferKeyEventL( aKeyEvent, aType );

        if ( ret == EKeyWasNotConsumed )
            {
            ret = ChangeFocusL( aKeyEvent );
            }

        if ( ret != EKeyWasConsumed )
            {
        	DrawNow();
            }
        }

    if ( ret == EKeyWasNotConsumed )
        {
        ret = iFocused->OfferKeyEventL( aKeyEvent, aType );

        // Report user activity to auto saver if editor field handled the
        // event. In case of message field (body text), EKeyWasConsumed
        // condition does not come true because AknFep consumes EKeyEvent
        // and calls HandleEdwinEventL. This OfferKeyEventL gets only
        // EEventKeyUp and EEventKeyDown events. Thus, when typing body text,
        // autosaver is notified in HandleEdwinEventL()
        if ( ( ret == EKeyWasConsumed ) && ( iFocused != iMessageField ) )
            {
            iView.HandleContainerChangeRequiringToolbarRefresh();
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
void CNcsComposeViewContainer::UpdateScreenPositionL( const TUint& /*aKeyCode*/ )
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
        // This function should be used only when focus is in body or read-only quote
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
    if ( quotePos.iY + moveY + iReadOnlyQuoteField->Size().iHeight < screenRect.Height() )
        {
        moveY = screenRect.Height() - quotePos.iY - iReadOnlyQuoteField->Size().iHeight;
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
        UpdateScrollBarL();

        //DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::UpdateFieldPosition
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::UpdateFieldPosition(CCoeControl* aAnchor)
    {
    FUNC_LOG;

	// We may not be completely constructed
	if ( iHeader && iMessageField && iReadOnlyQuoteField )
	    {
	    // First, move all the controls in the header
	    iHeader->UpdateFieldPosition(aAnchor);

	    // Then, move the body field below the header
	    TRect headerRect = iHeader->Rect();
	            
		const TSize separatorSize( NcsUtility::SeparatorSizeInThisResolution() );
		iSeparatorLineYPos = headerRect.iBr.iY;
			
		TPoint bodyPos( iMessageField->Position() );
		bodyPos.iY = iSeparatorLineYPos + separatorSize.iHeight;
		iMessageField->SetPosition( bodyPos );
		
		TPoint quotePos( iReadOnlyQuoteField->Position() );
		quotePos.iY = iMessageField->Rect().iBr.iY;
		iReadOnlyQuoteField->SetPosition( quotePos );

	    TRAP_IGNORE(UpdateScrollBarL());

	    DrawDeferred();
	    }

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ChangeFocusL()
// Handles key events
// -----------------------------------------------------------------------------
//

TKeyResponse CNcsComposeViewContainer::ChangeFocusL( const TKeyEvent& aKeyEvent )
    {
    FUNC_LOG;

    TKeyResponse ret( EKeyWasNotConsumed );

    if ( iFocused == iReadOnlyQuoteField && aKeyEvent.iCode == EKeyUpArrow )
        {
        iFocused = iMessageField;
        iReadOnlyQuoteField->SetFocus( EFalse, EDrawNow );
        iMessageField->SetFocus( ETrue, EDrawNow );
        ret = EKeyWasConsumed;
        }
    else if ( iFocused == iMessageField )
	    {
	    if ( aKeyEvent.iCode == EKeyUpArrow )
	        {
            iFocused = iHeader;
    		iHeader->SetFocus( ETrue,EDrawNow );
    		iMessageField->SetFocus( EFalse, EDrawNow );
    		iHeader->MakeVisible( ETrue );
            CommitL(EBodyField);
            ret = EKeyWasConsumed;
	        }
	    else if ( aKeyEvent.iCode == EKeyDownArrow && iReadOnlyQuoteField->TextLength() )
	        {
	        // Focus move to read-only quote only if there is some text in it.
	        iFocused = iReadOnlyQuoteField;
	        iReadOnlyQuoteField->SetFocus( ETrue, EDrawNow );
	        iMessageField->SetFocus( EFalse, EDrawNow );
	        iReadOnlyQuoteField->SetCursorPosL( 0, EFalse );
	        ret = EKeyWasConsumed;
	        }
        }
	else if ( iFocused == iHeader && aKeyEvent.iCode == EKeyDownArrow )
        {
		iFocused = iMessageField;
		iHeader->SetFocus( EFalse, EDrawNow );
		iMessageField->SetFocus( ETrue, EDrawNow );
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
TBool CNcsComposeViewContainer::UpdateFieldSizeL()
    {
    FUNC_LOG;

	// We may not have finished construction
	if ( iHeader && iMessageField && iReadOnlyQuoteField )
	    {
	    SizeChanged();
	    DrawDeferred();
	    }

	return EFalse;
    }

// <cmail> Platform layout changes
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
    const TPoint headerPos( NcsUtility::HeaderControlPosition( cmailPaneRect, 0 ) );
    cmailPaneRect.Move( 0, iHeader->Position().iY - headerPos.iY );

    NcsUtility::LayoutHeaderControl( iHeader, cmailPaneRect, 0, headerLineCount );

    // do not used stored headerLineCount, because relayout above may grow or
    // shrink edit fields
    TRAP_IGNORE( iMessageField->FormatAllTextNowL() );
    NcsUtility::LayoutBodyEdwin( iMessageField, cmailPaneRect, iHeader->LayoutLineCount(), iMessageField->LineCount(),
            iSeparatorLineYPos );

    iMessageField->UpdateFontSize();

    TInt readOnlyQuoteFieldHeight( 0 );
    if ( iReadOnlyQuoteField->TextLength() )
        {
        TInt dummySeparatorPos;
        NcsUtility::LayoutBodyEdwin( iReadOnlyQuoteField, cmailPaneRect, headerLineCount + iMessageField->LineCount(), iReadOnlyQuoteField->LineCount(), 
        		dummySeparatorPos );
        iReadOnlyQuoteField->UpdateFontSize();
        readOnlyQuoteFieldHeight = iReadOnlyQuoteField->Rect().Height();
        }

    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        }  
    iMessageField->SetRealRect( Rect() );
    iReadOnlyQuoteField->SetRealRect( Rect() );
    
    TInt messageLineHeigth = NcsUtility::HeaderCaptionPaneRect( cmailPaneRect ).Height();
    iMessageEditorMinHeigth = cmailPaneRect.Height() - iHeader->Size().iHeight -
                              NcsUtility::SeparatorSizeInThisResolution().iHeight -
                              readOnlyQuoteFieldHeight;
    
    iMessageEditorMinHeigth -= iMessageEditorMinHeigth % messageLineHeigth;
    if ( iMessageEditorMinHeigth > messageLineHeigth )
        {
        TSize newSize( iMessageField->Size().iWidth,
                       Max( iMessageField->Size().iHeight, iMessageEditorMinHeigth ));
            
        iMessageField->SetSize( newSize );
        }
    else
        {
        iMessageEditorMinHeigth = messageLineHeigth;
        }

    TRAP_IGNORE( UpdateScrollBarL() );
    DrawNow();
    
    }
// </cmail> Platform layout changes

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::Draw() const
// Draws the display
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::Draw( const TRect& /*aRect*/ ) const
    {
    FUNC_LOG;
	// <cmail> S60 skin support
	if ( iBgContext )
	    {
    	CWindowGc& gc = SystemGc();

    	MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    	if ( skin )
    		{
    		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
    		}
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
    iMessageField->SetCursorPosL(0,EFalse);

    inputStream.Open( aReadOnlyQuote.Ptr(), aReadOnlyQuote.Size() );
    CleanupClosePushL( inputStream );
    iReadOnlyQuoteField->RichText()->ImportTextL( 0, inputStream,
        CPlainText::EOrganiseByParagraph );
    CleanupStack::PopAndDestroy( &inputStream );

    // Activating the field will set the control to the end of the text
    iReadOnlyQuoteField->ActivateL();
    iReadOnlyQuoteField->SetCursorPosL(0,EFalse);

    // Recalculate the size of the body field
    SizeChanged();
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
    NcsUtility::ConvertLineEndingsL( message );
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
void CNcsComposeViewContainer::HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent /*aEventType*/ )
    {
    // <cmail> Platform layout change
    FUNC_LOG;
    
    if( iPhysics )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        }

    ScrollL( aScrollBar->ThumbPosition() );
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
			CNcsEmailAddressObject* object = CNcsEmailAddressObject::NewL( aContacts[i]->DisplayName(),
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

void CNcsComposeViewContainer::OperationErrorL( TContactHandlerCmd, TInt /*aError*/ )
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
    if ( aEdwin == iMessageField || aEdwin == iReadOnlyQuoteField )
        {
        if ( aEventType == MEikEdwinObserver::EEventTextUpdate ||
             aEventType == MEikEdwinObserver::EEventNavigation )
            {
            iAutoSaver.ReportActivity();

            // Update screen position and scroll bar when text changed or cursor moved
            UpdateScreenPositionL();
            UpdateScrollBarL();
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
            ScrollL( 0 );
            }
        }

    if ( aEdwin == iMessageField ) 
        {
        if ( aDesirableEdwinSize.iHeight >= iMessageEditorMinHeigth )
            {
            aEdwin->SetSize( aDesirableEdwinSize );
            }        
        }
    else
        {
        aEdwin->SetSize( aDesirableEdwinSize );
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
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::UpdateScrollBarL()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::UpdateScrollBarL()
	{
    FUNC_LOG;

    TRect headerRect = iHeader->Rect();
    TRect bodyRect = iMessageField->Rect();
    TRect quoteRect = iReadOnlyQuoteField->Rect();
    const TSize separatorSize( NcsUtility::SeparatorSizeInThisResolution() );

    TInt totalComposerHeight = headerRect.Height() + separatorSize.iHeight*2 + 
							   bodyRect.Height() + quoteRect.Height();
    
    TInt visibleAreaHeight = Rect().Height();
    TInt visiblePosition = -headerRect.iTl.iY;

    iScrollBarModel.SetScrollSpan( totalComposerHeight );
    iScrollBarModel.SetWindowSize( visibleAreaHeight );
    iScrollBarModel.SetFocusPosition( visiblePosition );

    iScrollBar->SetModelL( &iScrollBarModel );
    iScrollBar->DrawNow();
	}

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetApplicationRect()
// -----------------------------------------------------------------------------
//
// <cmail>
/*
void CNcsComposeViewContainer::SetApplicationRect( TRect aRect )
	{
    FUNC_LOG;

	iApplicationRect = aRect;

	}
*/
// </cmail>


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
        iMessageField->SetFocus( ETrue, EDrawNow );
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

	TInt pos = iMessageField->CursorPos();
	TCursorSelection selIns( pos, pos );
	iMessageField->InsertDeleteCharsL( pos, aText, selIns );
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
// CNcsComposeViewContainer::LaunchAttachmentActionMenuL
// Called when right-click happens in the attahcments field
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::LaunchAttachmentActionMenuL()
    {
    FUNC_LOG;
    iView.LaunchAttachmentActionMenuL();
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetToFieldAddressesL
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& CNcsComposeViewContainer::GetToFieldAddressesL( TBool aParseNow )
    {
    return iHeader->GetToFieldAddressesL( aParseNow );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetCcFieldAddressesL
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& CNcsComposeViewContainer::GetCcFieldAddressesL( TBool aParseNow )
    {
    return iHeader->GetCcFieldAddressesL( aParseNow );
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::GetBccFieldAddressesL
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& CNcsComposeViewContainer::GetBccFieldAddressesL( TBool aParseNow )
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

// <cmail> Removed unneeded IsFocusAif
// </cmail>

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
void CNcsComposeViewContainer::SetToFieldAddressesL( RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;

    iHeader->SetToFieldAddressesL(aAddress);

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetCcFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetCcFieldAddressesL( RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;

    iHeader->SetCcFieldAddressesL( aAddress );

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetBccFieldAddressesL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetBccFieldAddressesL( RPointerArray<CNcsEmailAddressObject>& aAddress )
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
// CNcsComposeViewContainer::SetAttachmentLabelTextL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetAttachmentLabelTextL( const TDesC& aText )
    {
    FUNC_LOG;

	iHeader->SetAttachmentLabelTextL( aText );

    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SetAttachmentLabelTextL
//
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::SetAttachmentLabelTextL( const TDesC& aAttachmentName,
                                                        const TDesC& aAttachmentSizeDesc )
    {
    FUNC_LOG;

    iHeader->SetAttachmentLabelTextL( aAttachmentName, aAttachmentSizeDesc );

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
    // <cmail> Platform layout change
    const TRect rect( NcsUtility::ListscrollPaneRect() );
    SetRect( rect );
    // </cmail> Platform layout change
    if ( iHeader )
        {
        iHeader->HandleDynamicVariantSwitchL();
        }
    // <cmail> Platform layout change
    //TRect rect = iView.ClientRect();
    //SetApplicationRect( rect );
    //SetRect( rect );
	//UpdateScreenPositionL();
    // </cmail> Platform layout change
    UpdateScrollBarL();
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
        iHeader->HandleDynamicVariantSwitchL();
        }

// <cmail>
    if ( iMessageField )
        {
        iMessageField->UpdateFontSize();
        }

    if ( iScrollBar )
        {
        iScrollBar->HandleResourceChange( KAknsMessageSkinChange );
        iScrollBar->DrawNow();
        }
// </cmail>
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ContentTotalHeight
// Return the total height of the components shown in the view
// -----------------------------------------------------------------------------
//
TInt CNcsComposeViewContainer::ContentTotalHeight()
    {
    FUNC_LOG;
    const TSize separatorSize( NcsUtility::SeparatorSizeInThisResolution() );
	
	TInt totalHeight( iHeader->Size().iHeight + 
					  separatorSize.iHeight*2 + iMessageField->Size().iHeight );

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
    iView.CommitL(ETrue,aFieldToCommit);
    iView.HandleContainerChangeRequiringToolbarRefresh();
    }

//<cmail>
// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::SwitchChangeMskOff
//sets up iSwitchChangeMskOff falg, which disables changes of MSK label if any popup dialog is open
// -----------------------------------------------------------------------------
void CNcsComposeViewContainer::SwitchChangeMskOff(TBool aTag)
    {
    FUNC_LOG;
    iHeader->SwitchChangeMskOff(aTag);
    }
//</cmail>

//<cmail>
// -----------------------------------------------------------------------------
// CNcsHeaderContainer::HandleLongTapEventL()
// Handles key events
// -----------------------------------------------------------------------------
void CNcsComposeViewContainer::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& /*aPenEventScreenLocation*/ )
    {
    FUNC_LOG;
    iHeader->HandleLongTapL( aPenEventLocation );
    }
//</cmail>


//<cmail>
void CNcsComposeViewContainer::FixSemicolonL()
	{
    FUNC_LOG;
	iHeader->FixSemicolonInAddressFieldsL();
	}
//</cmail>

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
                Max(ContentTotalHeight(), viewSize.iHeight) );
        iPhysics->InitPhysicsL( worldSize, viewSize, EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeViewContainer::ScrollL()
// -----------------------------------------------------------------------------
//
void CNcsComposeViewContainer::ScrollL( TInt aTargetPos )
	{
    FUNC_LOG;
	TPoint headerPos( iHeader->Position() );
	
	aTargetPos = Max( 0, Min(aTargetPos, ContentTotalHeight() - Rect().Height()) );
	
	TInt moveY = -headerPos.iY - aTargetPos;
	
	if( moveY )
		{
		headerPos.iY += moveY;
		iHeader->SetPosition( headerPos );
		
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
	
		UpdateScrollBarL();
		DrawNow();
		}
	}

/**
 * @see MAknPhysicsObserver::ViewPositionChanged
 */
void CNcsComposeViewContainer::ViewPositionChanged(
        const TPoint& aNewPosition,
        TBool /*aDrawNow*/,
        TUint /*aFlags*/ )
    {
    FUNC_LOG;
	if( iIsFlicking )
		{
		TInt scrollOffset = aNewPosition.iY - Rect().Size().iHeight / 2;

		TRAP_IGNORE( ScrollL( scrollOffset ) );
		}
	}

/**
 * @see MAknPhysicsObserver::PhysicEmulationEnded
 */
void CNcsComposeViewContainer::PhysicEmulationEnded()
    {
    FUNC_LOG;
    iIsFlicking = EFalse;
    }

/**
 * @see MAknPhysicsObserver::ViewPosition
 */
TPoint CNcsComposeViewContainer::ViewPosition() const
    {
    FUNC_LOG;
    return TPoint(0, -iHeader->Position().iY + Rect().Size().iHeight / 2 );
    }


TBool CNcsComposeViewContainer::IsRemoteSearchInprogress() const
    {
    FUNC_LOG;
    return iHeader->IsRemoteSearchInprogress();
    }
