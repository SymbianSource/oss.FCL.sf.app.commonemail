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
*  Description : email header container
*
*/



#include "emailtrace.h"
#include <StringLoader.h>
#include <aknViewAppUi.h>
#include <aknnotewrappers.h> //CAknInformationNote
#include <FreestyleEmailUi.rsg>

#include "cfsmailbox.h"
#include <FreestyleEmailUi.rsg>

#include "FSEmailBuildFlags.h"
#include "ncsheadercontainer.h"
#include "ncscomposeviewcontainer.h"
#include "ncsemailaddressobject.h"
#include "ncsutility.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiAppui.h"
#include "FSEmail.pan"
#include "ncsattachmentfield.h"
#include "ncssubjectfield.h"
#include "ncspopuplistbox.h"

_LIT( KAddressDelimeterSemiColon, ";" );


// ---------------------------------------------------------------------------
// ToNcsControl
// ---------------------------------------------------------------------------
//
static MNcsControl* ToNcsControl( CCoeControlArray::TCursor cur )
	{
    FUNC_LOG;
	CCoeControl* coe = cur.Control<CCoeControl>();
	return dynamic_cast<MNcsControl*>(coe);
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::NewL
// ---------------------------------------------------------------------------
//
CNcsHeaderContainer* CNcsHeaderContainer::NewL( CCoeControl& aParent, 
    CFSMailBox& aMailBox, TInt aFlags )
    {
    FUNC_LOG;
    CNcsHeaderContainer* self = 
        new ( ELeave ) CNcsHeaderContainer( aParent, aMailBox );
    CleanupStack::PushL( self );
    self->ConstructL( aFlags );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::CNcsHeaderContainer
// ---------------------------------------------------------------------------
//
CNcsHeaderContainer::CNcsHeaderContainer(
	CCoeControl& aParent, 
	CFSMailBox& aMailBox ): 
	iParent( aParent ),
	iFieldSizeObserver( static_cast< CNcsComposeViewContainer& >( aParent ) ),
	iMailBox( aMailBox ),
	iLongTapEventConsumed( EFalse )
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::ConstructL( TInt aFlags )
	{
    FUNC_LOG;
    
	SetContainerWindowL( iParent );
    
	CFreestyleEmailUiAppUi* fsAppUi = 
        static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );

	// Create 'To' field
	iToField = CNcsAddressInputField::NewL( 
		R_NCS_TO_FIELD_TEXT,
		CNcsAddressInputField::EInputFieldTo,
		&iFieldSizeObserver, 
		this, this );

	// Create 'Cc' field
	iCcField = CNcsAddressInputField::NewL( 
		R_NCS_CC_FIELD_TEXT,
		CNcsAddressInputField::EInputFieldCc,
		&iFieldSizeObserver, 
		this, this );
	iCcField->MakeVisible( aFlags&ECcFieldVisible );

	// Create 'Bcc' field
	iBccField = CNcsAddressInputField::NewL( 
		R_NCS_BCC_FIELD_TEXT,
		CNcsAddressInputField::EInputFieldBcc,
		&iFieldSizeObserver, 
		this, this );
	iBccField->MakeVisible( aFlags&EBccFieldVisible );

	// Create 'subject' field
	iSubjectField = CNcsSubjectField::NewL( R_FSE_EDITOR_HEADER_SUBJECT, 
	        &iFieldSizeObserver, this );
    
    iAttachmentField = CNcsAttachmentField::NewL( R_NCS_ATTACHMENT_LABEL_TEXT, 
            &iFieldSizeObserver, this );

    // Setup the control array
    // Add all of them now so the container and parent is set correctly
    InitComponentArrayL();
    CCoeControlArray& controls = Components();
    controls.SetControlsOwnedExternally( ETrue );
    
    controls.AppendLC( iToField );
    CleanupStack::Pop( iToField );
    controls.AppendLC( iCcField );
    CleanupStack::Pop( iCcField );
    controls.AppendLC( iBccField );
    CleanupStack::Pop( iBccField );
    controls.AppendLC( iSubjectField );
    CleanupStack::Pop( iSubjectField );
    controls.AppendLC( iAttachmentField );
    CleanupStack::Pop( iAttachmentField );

    iToField->SetFocus( ETrue );

    if ( !(aFlags&ECcFieldVisible) )
        {
        controls.Remove( iCcField );
        }

    if ( !(aFlags&EBccFieldVisible) )
        {
        controls.Remove( iBccField );
        }

    // initially attachments field is hidden
	iAttachmentField->MakeVisible( EFalse );
	controls.Remove( iAttachmentField );

	// test whether mailbox supports remote lookup
	TBool remoteLookupSupported = 
        TFsEmailUiUtility::IsRemoteLookupSupported( iMailBox );
	
	iAacListBox = CNcsPopupListBox::NewL( 
	        this, iMailBox, *this, remoteLookupSupported );
	iAacListBox->MakeVisible( EFalse );

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, Rect(), EFalse );

    iRALInProgress = EFalse;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::~CNcsHeaderContainer
// ---------------------------------------------------------------------------
CNcsHeaderContainer::~CNcsHeaderContainer()
	{
    FUNC_LOG;
	delete iToField;
	delete iCcField;
	delete iBccField;
	delete iSubjectField;
    delete iAttachmentField;
	delete iAacListBox;
	delete iLongTapDetector;
	delete iBgContext;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::FocusChanged
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::FocusChanged( TDrawNow aDrawNow )
	{
    FUNC_LOG;
    
    CCoeControl* focused = iFocused;
    
    if ( !focused )
        {
        focused = FindFocused();
        iFocused = focused;
        }
    
    if ( !IsFocused() )
        {
        if ( focused )
            {
            // We're loosing focus (probably going to message body)
            // Commit changes and make sure no controls are focused.
            TRAP_IGNORE( CommitFieldL( focused ) );
            focused->SetFocus( EFalse, aDrawNow );
            iFocused = NULL;
            }

        // Remove MSK label when header loses focus
        TRAP_IGNORE( SetMskL() );
		}
	else if ( IsFocused() && !focused )
		{
		// We're gaining focus from the message body
		// Set the focus to the last control in the control array
		// (either attachment line or subject field)
		CCoeControlArray::TCursor cur = Components().End();
		cur.Prev();
		cur.Control<CCoeControl>()->SetFocus( ETrue, aDrawNow );
		}
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::ShowCursor
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::ShowCursor( TBool aShow )
    {
    CCoeControl* focused = FindFocused();
    if ( focused )
        {
        iFocused = focused;
        }
    if ( iFocused ) 
        {
        iFocused->SetFocus( aShow, EDrawNow );
        }
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::Draw() const
// Draws the display
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::Draw( const TRect& /*aRect*/ ) const
	{
    FUNC_LOG;

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
// CNcsHeaderContainer::DrawAttachmentFocusNow() const
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::DrawAttachmentFocusNow()
	{
    FUNC_LOG;
    iAttachmentField->DrawDeferred();
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::HandleControlArrayEventL()
// Handles removal or additons of controls to the header.
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::HandleControlArrayEventL(
	CCoeControlArray::TEvent aEvent,
	const CCoeControlArray* /*aArray*/,
	CCoeControl* aControl,
	TInt /*aControlId*/ )
	{
    FUNC_LOG;
	if ( aEvent == CCoeControlArray::EControlAdded ) 
		{
		aControl->SetContainerWindowL( iParent );
		aControl->MakeVisible( ETrue );
		aControl->ActivateL();
		}
	else if ( aEvent == CCoeControlArray::EControlRemoved )
		{
		aControl->MakeVisible( EFalse );
		}
	// Tell the parent to recalculate everything
	iFieldSizeObserver.UpdateFieldPosition( NULL );
	}

void CNcsHeaderContainer::SetMskL()
    {
    FUNC_LOG;
    CCoeControl* focused = FindFocused();
    if ( focused == iToField || focused == iCcField || focused == iBccField )
        {
        if( iSwitchChangeMskOff == EFalse )
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_ADD );
            }
        }
    else if ( focused == iAttachmentField ) 
        {
            if ( GetAttachmentCount() > 1 )
                {
            ChangeMskCommandL( R_FSE_QTN_MSK_VIEWATTACHMENTS );
                }
            else if ( !HasRemoteAttachments() )
                {
            ChangeMskCommandL( R_FSE_QTN_MSK_VIEWATTACHMENT );                
                }
            else // message has single remote attachment => no MSK function
                {
            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
            }
        }
    else if ( focused == iSubjectField )
        {
        ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );       
        }
    else 
        {
        ChangeMskCommandL( R_FSE_QTN_MSK_BODY_MENU );
        }
    }  

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::HandlePointerEventL()
// 
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::HandlePointerEventL( 
        const TPointerEvent& aPointerEvent )
    {
	FUNC_LOG;
	CCoeControl* clicked = 0;
    for ( TInt i=0; i < Components().Count(); ++i )
        {
        TRect rc = Components().At( i ).iControl->Rect();
        if ( rc.Contains( aPointerEvent.iPosition ) )
            {
            clicked = Components().At( i ).iControl;
            }
        }
    if ( clicked )
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            CCoeControl* pOldCtrl = FindFocused();
            CCoeControl* pNewCtrl= clicked;
            
            if ( pOldCtrl != pNewCtrl )
                {
                // Unfocus the control
                if ( pOldCtrl )
                    {
                    pOldCtrl->SetFocus( EFalse, ENoDrawNow );
                    }
                pNewCtrl->SetFocus( ETrue, ENoDrawNow );
                iFocused = pNewCtrl;
                // Commit changes to previously focused field.
                if ( pOldCtrl )
                    {
                    CommitFieldL( pOldCtrl );
                    }

                // If the attachments label has changed focus
                if ( pOldCtrl == iAttachmentField || 
                     pNewCtrl == iAttachmentField )
                    {
                    DrawAttachmentFocusNow();
                    }
                
                CNcsComposeViewContainer* container = 
                    static_cast<CNcsComposeViewContainer*>( &iParent );
                container->UpdateScrollBarL();
                }
            
            if( iLongTapEventConsumed )
        		{
        		iLongTapEventConsumed = EFalse;
        		return;
        		}        	
            
            if( pNewCtrl == iAttachmentField )
            	{
				CNcsComposeViewContainer& parent = 
                    static_cast<CNcsComposeViewContainer&>( iParent );
				parent.HandleAttachmentsOpenCommandL();
            	}          
            }
        }
    for ( TInt i=0; i < Components().Count(); ++i )
        {
        Components().At( i ).iControl->HandlePointerEventL( aPointerEvent );
        }
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::HandleLongTapL()
// 
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::HandleLongTap( const TPoint& aPenEventLocation, 
										  const TPoint& aPenEventScreenLocation )
    {
	FUNC_LOG;
	iLongTapEventConsumed = EFalse;
	
    CCoeControl* control = FindFocused();
    TRect rect = iAttachmentField->Rect();
    	if( iAttachmentField->IsVisible() && 
    	    rect.Contains( aPenEventLocation ) )
        {
        iLongTapEventConsumed = ETrue;
        
        CNcsComposeViewContainer& parent = 
            static_cast<CNcsComposeViewContainer&>( iParent );
        parent.LaunchStylusPopupMenu( aPenEventScreenLocation );
        }
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::NeedsLongTapL()
// Whether long tap detection is needed or not (depends on whether touch 
// location is within attachment field or not)
// -----------------------------------------------------------------------------
TBool CNcsHeaderContainer::NeedsLongTapL( const TPoint& aPenEventLocation )
    {
	FUNC_LOG;

    CCoeControl* control = FindFocused();
    TRect rect = iAttachmentField->Rect();
    TBool result( EFalse );
	if( iAttachmentField->IsVisible() && rect.Contains( aPenEventLocation ) &&
	    KNoAttachmentLabelFocused !=
            iAttachmentField->FocusedAttachmentLabelIndex() )
        {
        result = ETrue;
        }
    return result;    
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::OfferKeyEventL()
// Handles key events
// -----------------------------------------------------------------------------
TKeyResponse CNcsHeaderContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret( EKeyWasNotConsumed );
    
    TBool doScroll( EFalse );
    
    if( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
    	{
        if ( iAacListBox && iAacListBox->IsVisible() && 
             !iAacListBox->IsPopupEmpty() )
        	{
           	return iAacListBox->OfferKeyEventL( aKeyEvent, aType );
        	}
        else
	        {
		    ret = FindFocused()->OfferKeyEventL( aKeyEvent, aType );
		    
    		doScroll = ( ret == EKeyWasConsumed ); 
	        }

        if ( ret == EKeyWasNotConsumed ) 
        	{
            ret = ChangeFocusL( aKeyEvent );
        	DrawDeferred();
        	}
    	}
    else
    	{
        if ( FindFocused() == iAttachmentField )
        	{
        	if ( aType == EEventKey )
        	    {
                CNcsComposeViewContainer& parent = 
                    static_cast<CNcsComposeViewContainer&>( iParent );

                if ( aKeyEvent.iCode == EKeyEnter || 
                     aKeyEvent.iScanCode == EStdKeyEnter ||  
                     aKeyEvent.iCode == EKeyOK || 
                     aKeyEvent.iScanCode == EStdKeyDevice3 )
            	    {   
            	    // open list or attachment
            	    parent.HandleAttachmentsOpenCommandL();
            	    ret = EKeyWasConsumed;
            	    }
        	    }
            }
		else if ( IsPopupActive() && aType == EEventKey )
			{
			// select current
			if( aKeyEvent.iCode == EKeyEnter || 
			    aKeyEvent.iCode == EKeyDevice4 || 
				aKeyEvent.iCode == EKeyOK )
				{
				DoPopupSelectL();
				ret = EKeyWasConsumed;
				}
			}
		else
		    {
		    // Don't allow line feeds in header fields.
		    // Could be nice if enter committed the field and moved the focus
		    // to next one
		    if ( aType == EEventKey && 
		         ( aKeyEvent.iCode == EKeyEnter || 
		           aKeyEvent.iScanCode == EStdKeyEnter) )
		        {
		        FindFocused()->OfferKeyEventL( aKeyEvent, aType );
		        ret = EKeyWasConsumed;
		        }
		    }
    	}
    if ( ret == EKeyWasNotConsumed )
    	{
    	CCoeControl* focused = FindFocused();
    	if ( focused )
    		{
    		ret = focused->OfferKeyEventL( aKeyEvent, aType );
    		
    		if(aType==EEventKeyDown)
    		    {
    		    doScroll = ETrue; 
    		    }
    		else 
    		    {
    		    doScroll = (ret == EKeyWasConsumed); 
    		    }
    		}
    	}

    if( doScroll )
    	{
    	DoScrollL();
    	}

    return ret;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::FindFocused
// ---------------------------------------------------------------------------
//
CCoeControl* CNcsHeaderContainer::FindFocused() const
	{
    FUNC_LOG;
	CCoeControlArray::TCursor cur = Components().Begin();
	do 
		{
		if ( cur.Control<CCoeControl>()->IsFocused() )
			{
			return cur.Control<CCoeControl>();
			}
		} 
	while ( cur.Next() );
	return NULL;
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::ChangeFocusL()
// Handles key events
// -----------------------------------------------------------------------------
TKeyResponse CNcsHeaderContainer::ChangeFocusL( const TKeyEvent& aKeyEvent )
	{
    FUNC_LOG;
	ASSERT( aKeyEvent.iCode == EKeyDownArrow || 
	        aKeyEvent.iCode == EKeyUpArrow );
    TKeyResponse ret( EKeyWasNotConsumed );

    CCoeControl* pOldCtrl = FindFocused();

    // If nothing is focused we return
    if ( !pOldCtrl ) return ret;
    
    //If this is the first control in the list, don't change focus
    CCoeControlArray::TCursor cur = Components().Find(pOldCtrl);
    if ( aKeyEvent.iCode == EKeyUpArrow && cur == Components().Begin() )
    	{
    	return ret;
    	}
    
    // Unfocus the control
	pOldCtrl->SetFocus( EFalse, ENoDrawNow );
    
	CCoeControl* pNewCtrl= NULL;
    if ( aKeyEvent.iCode == EKeyDownArrow && cur.Next() ) 
    	{
	    pNewCtrl = cur.Control<CCoeControl>();
		pNewCtrl->SetFocus( ETrue, ENoDrawNow );
        ret = EKeyWasConsumed;
    	}
    else if ( aKeyEvent.iCode == EKeyUpArrow && cur.Prev() )
    	{
	    pNewCtrl = cur.Control<CCoeControl>();
		pNewCtrl->SetFocus( ETrue, ENoDrawNow );
        ret = EKeyWasConsumed;
    	}

        CNcsComposeViewContainer* container = 
            static_cast<CNcsComposeViewContainer*>( &iParent );
    if ( pOldCtrl == iToField )
        {
        container->CommitL( EToField );
        }
    else if ( pOldCtrl == iCcField) 
        {
        container->CommitL( ECcField );
        }
    else if ( pOldCtrl == iBccField)
        {
        container->CommitL( EBccField );
        }
    else if ( pOldCtrl == iSubjectField)
        {
        container->CommitL( ESubjectField );
        }
    
	// If the attachments label has changed focus
	if ( pOldCtrl == iAttachmentField || pNewCtrl == iAttachmentField )
		{
		DrawAttachmentFocusNow();
		}
	
    // if focus was changed, update scroll bar
    if ( ret == EKeyWasConsumed )
        {
        container->UpdateScrollBarL();
        DoScrollL();
        }

	// NOTE: If we're leaving the header (down was pushed on last control)
	//       then we return EKeyWasNotConsumed to make sure the
	//		 parent moves the focus to the control below this container
    return ret;
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::UpdateFieldPosition()
// One of the controls want me to readjust myself to a new position,'
// anchoring arround the given control
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::UpdateFieldPosition( CCoeControl* aAnchor )
	{
    FUNC_LOG;
	// if the anchor is NULL just call SizeChanged
	if ( !aAnchor )
		{
		SizeChanged();
		return;
		}
	
	CCoeControlArray::TCursor cur = Components().Find( aAnchor );
	ASSERT( cur.IsValid() );

	// figure out the new top position of the container
	TInt top = aAnchor->Rect().iTl.iY;
	while ( cur.Prev() )
		{
		CCoeControl* ctrl = cur.Control<CCoeControl>();
		top -= ctrl->Rect().Height();
		}

	// Then check we didn't move too much and composer still fills the whole
	// visible area on the screen (i.e. don't scroll below the bottom of the
	// body field)
	CNcsComposeViewContainer& parent = 
        static_cast<CNcsComposeViewContainer&>( iParent );
	TInt composerHeight = parent.ContentTotalHeight();
    TInt screenHeight = parent.Rect().Height();
    if ( composerHeight <= screenHeight )
        { // no scrolling is needed at all since everything fits on screen
        top = 0;
        }
    else
        {
        top = Max( top, screenHeight - composerHeight );
        }

    // The top edge of the header should never be lower than on the top edge
    // of the screen. For some reason, the calculation above leads to such
    // situation if recipient and subject fields are scrollable. If that 
    // happens, increase the top value to 0 to prevent empty space showing up
    // above the header area.
	top = Min( top, 0 );
	
	// set the new position of the container
	SetExtent( TPoint( Rect().iTl.iX, top ), Size() );
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::SizeChanged()
// set size
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::SizeChanged()
	{
    FUNC_LOG;

    const TRect rect( Rect() );

    TInt currentLine( 0 );
    CCoeControlArray::TCursor cur = Components().Begin();
    do
        {
        const TInt lineCount( ToNcsControl( cur )->LayoutLineCount() );
        if ( lineCount > 0 )
            {
            NcsUtility::LayoutHeaderControl( cur.Control<CCoeControl>(), 
                    rect, currentLine, lineCount );
            // Do not use stored value lineCount because count may change
            // during layout (e.g. when orientation is changed => edit field
            // length may change => it grows or shrinks)
            currentLine += ToNcsControl( cur )->LayoutLineCount();
            }
        } while ( cur.Next() );
        
	if( iAacListBox->IsVisible() && !iAacListBox->IsPopupEmpty() )
		{
		iAacListBox->SetPopupMaxRect( CalculatePopupRect() );
		}
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::PositionChanged()
// set size
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::PositionChanged()
	{
    FUNC_LOG;
	DrawDeferred();
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::ChangePositions()
// set positions
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::ChangePositions()
	{
    FUNC_LOG;
	TPoint nextPoint( Rect().iTl );

	CCoeControl* ctrl;
	CCoeControlArray::TCursor cur = Components().Begin();
	do 
		{
		ctrl = cur.Control<CCoeControl>();
		ctrl->SetPosition( nextPoint );
		nextPoint.iY += ctrl->Size().iHeight;
		} 
    while ( cur.Next() );
	}


// ---------------------------------------------------------------------------
// CNcsHeaderContainer::NeedsAifMenu
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::NeedsAifMenu() const
	{
    FUNC_LOG;

	CCoeControl* focused = FindFocused();

	// Has to be an AIF field
	TBool ret = EFalse;
	
    if ( IsAddressInputField( focused ) )
        {
        CNcsAddressInputField* aifFocused = NULL;
        aifFocused = static_cast<CNcsAddressInputField*>( focused );
        ret = ( aifFocused->TextEditor()->SelectionLength() > 1 );
        }
	
	return ret;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetToLineHeight
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetToLineHeight() const
	{
	FUNC_LOG;
	TInt lineHeight = 0;
	
	if(iToField)
		{
	    TRect lineRect;
	    TRAPD(err, iToField->GetLineRectL(lineRect) );
	   	if(err == KErrNone)
	        {
	        lineHeight = lineRect.iBr.iY - lineRect.iTl.iY;
	        }
		}
	return lineHeight;
	}
      
// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetTotalHeight
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetTotalHeight() const
	{
    FUNC_LOG;
	TInt ret = 0;
	CCoeControlArray::TCursor cur = Components().Begin();
	do
		{
		CCoeControl* ctrl = cur.Control<CCoeControl>();
		ret += ctrl->Size().iHeight;
		} 
	while ( cur.Next() );
	return ret;
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::SetAttachmentLabelTextsLD
//
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::SetAttachmentLabelTextsLD( 
    CDesCArray* aAttachmentNames, CDesCArray* aAttachmentSizes )
    {
    FUNC_LOG;   
    iAttachmentField->SetTextsLD( aAttachmentNames, aAttachmentSizes );
    if( aAttachmentNames )
        {
        ShowAttachmentLabelL();
        DrawAttachmentFocusNow();
        }
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::FocusedAttachmentLabelIndex
//
// -----------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::FocusedAttachmentLabelIndex()
    {
    FUNC_LOG;
    return iAttachmentField->FocusedAttachmentLabelIndex();
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::ShowAttachmentLabel()
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::ShowAttachmentLabelL()
    {
    FUNC_LOG;
    if ( !iAttachmentField->IsVisible() )
        {
        CCoeControlArray::TCursor cur = Components().End();
        Components().InsertLC( cur, iAttachmentField );
        CleanupStack::Pop( iAttachmentField );

        TRAP_IGNORE( iFieldSizeObserver.UpdateFieldSizeL() );

        CCoeControl* pOldCtrl = FindFocused();
        if ( pOldCtrl )
            {
            pOldCtrl->SetFocus( EFalse, ENoDrawNow );
            }

        iAttachmentField->SetFocus( ETrue, ENoDrawNow );
        iFieldSizeObserver.UpdateFieldPosition( iAttachmentField );
        }
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::HideAttachmentLabel()
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::HideAttachmentLabel()
	{
    FUNC_LOG;
	// check if we are focused and in that case
	// unfocus attachment filed and focus previous control
    CCoeControl* pOldCtrl = FindFocused();
    CCoeControl* pNewCtrl = NULL;
    
    if ( pOldCtrl == iAttachmentField )
        {
        CCoeControlArray::TCursor cur = Components().Find( pOldCtrl );
	    pOldCtrl->SetFocus( EFalse, ENoDrawNow );

	    if ( cur.Prev() )
	        {
	        pNewCtrl = cur.Control<CCoeControl>();
		    pNewCtrl->SetFocus( ETrue, ENoDrawNow );
	        }
        }
    else
        {
        pNewCtrl = pOldCtrl; // no need to move focus
        }

	if ( pOldCtrl == iAttachmentField || pNewCtrl == iAttachmentField )
		{
		DrawAttachmentFocusNow();
		}

	iAttachmentField->MakeVisible( EFalse );
	Components().Remove( iAttachmentField );
    
	TRAP_IGNORE( iFieldSizeObserver.UpdateFieldSizeL() );

    // Scroll to currently focused field
	if ( pNewCtrl )
	    {
	    iFieldSizeObserver.UpdateFieldPosition( pNewCtrl );
	    }
	
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::SetBccFieldVisibleL()
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::SetBccFieldVisibleL( 
        TBool aVisible, TDrawNow aDrawNow, TBool aFocus ) 
    {
    FUNC_LOG;
	if ( iBccField->IsVisible() == aVisible )
		{
		return;
		}

	if ( aVisible )
		{
		CCoeControlArray::TCursor cur = Components().Find( iSubjectField );
		Components().InsertLC( cur, iBccField );
		CleanupStack::Pop( iBccField );

		if ( aFocus )
			{
			CCoeControl* focused = FindFocused();

			if ( focused )
				{
				focused->SetFocus( EFalse );
				}
			iBccField->SetFocus( ETrue );
			iBccField->SelectAllTextL();
			}
		}
	else
		{
		if ( iBccField->IsFocused() )
			{
			CCoeControlArray::TCursor cur = Components().Find( iBccField );
			ASSERT( cur.IsValid() );
			
			iBccField->SetFocus( EFalse );

			cur.Prev(); // Get the control before this field
			cur.Control<CCoeControl>()->SetFocus( ETrue );
			}
		Components().Remove( iBccField );
		}

    TRAP_IGNORE( iFieldSizeObserver.UpdateFieldSizeL() );

	if ( aDrawNow == EDrawNow )
		{
	    DrawDeferred();
		}
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::SetCcFieldVisibleL()
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::SetCcFieldVisibleL( 
        TBool aVisible, TDrawNow aDrawNow, TBool aFocus ) 
	{
    FUNC_LOG;
	if ( iCcField->IsVisible() == aVisible )
		{
		return;
		}

	if ( aVisible )
		{
		Components().InsertAfterLC( Components().Id( *iToField ), iCcField );
		CleanupStack::Pop( iCcField );

		if ( aFocus )
			{
			CCoeControl* focused = FindFocused();
			if ( focused )
				{
				focused->SetFocus( EFalse );
				}
			iCcField->SetFocus( ETrue );
			iCcField->SelectAllTextL();
			}
		}
	else
		{
		if( iCcField->IsFocused() )
			{
			iCcField->SetFocus( EFalse );
			iToField->SetFocus( ETrue );
			}
		Components().Remove( iCcField );
		}
    
	TRAP_IGNORE( iFieldSizeObserver.UpdateFieldSizeL() );

    if ( aDrawNow == EDrawNow )
		{
	    DrawDeferred();
		}
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::MopSupplyObject
// From CCoeControl.
// -----------------------------------------------------------------------------
TTypeUid::Ptr CNcsHeaderContainer::MopSupplyObject( TTypeUid aId )
	{
    FUNC_LOG;
    return CCoeControl::MopSupplyObject( aId );
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::LineCount
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::LineCount() const
	{
    FUNC_LOG;
	TInt cnt = 0;
	CCoeControlArray::TCursor cur = Components().Begin();
	do 
		{
        MNcsControl* ctrl = ToNcsControl( cur );
        cnt += ctrl->LineCount();
        } 
    while ( cur.Next() );
	return cnt;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::ScrollableLines
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::ScrollableLines() const
	{
    FUNC_LOG;
	TInt cnt = 0;
	CCoeControlArray::TCursor cur = Components().Begin();
	do 
		{
		MNcsControl* ctrl = ToNcsControl( cur );
		cnt += ctrl->ScrollableLines();
		} 
	while ( cur.Next() );
	return cnt;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::CursorPosition
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::CursorPosition() const
	{
    FUNC_LOG;
	CCoeControl* coe = FindFocused();
	if ( !coe )
		{
		return 0;
		}
	
	MNcsControl* ncsCtrl = dynamic_cast<MNcsControl*>( coe );
	// This will give the the position relative to the top of the control
	TInt pos = ncsCtrl->CursorPosition();
	// add the location of the top of the control relative to the top
	// of the header.
	pos += coe->Rect().iTl.iY - Rect().iTl.iY;
	return pos;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::CursorLineNumber
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::CursorLineNumber() const
    {
    FUNC_LOG;
    TInt cnt = 0;
	CCoeControlArray::TCursor cur = Components().Begin();
	do 
		{
        CCoeControl* coe = cur.Control<CCoeControl>();
        MNcsControl* ctrl = ToNcsControl( cur );
        if ( !coe->IsFocused() )
            {
            cnt += ctrl->LineCount();
            }
        else
            {
            cnt += ctrl->CursorLineNumber();
            break;
            }
        }
    while ( cur.Next() );
	return cnt;
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetNumChars
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetNumChars() const
	{
    FUNC_LOG;
	TInt cnt = iToField->GetNumChars() + 
               iCcField->GetNumChars() + 
               iBccField->GetNumChars() + 
               iSubjectField->GetNumChars();

	return cnt;
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::UpdatePopupContactListL()
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::UpdatePopupContactListL( 
        const TDesC& aMatchString, TBool /*aListAll*/ )
	{
    FUNC_LOG;

    // do update only for address fields
	CCoeControl* focused = FindFocused();
	
	if ( !IsAddressInputField( focused ) )
	    {
	    return;
	    }
	    
	if ( aMatchString.CompareC( KNullDesC ) == 0 || 
	     aMatchString.Compare( KAddressDelimeterSemiColon ) == 0 )
		{
		ClosePopupContactListL();
		return;
		}
	
	if( !iAacListBox->IsVisible() )
		{
        iAacListBox->SetPopupMaxRect( CalculatePopupRect() );
        iAacListBox->InitAndSearchL( aMatchString );
        iAacListBox->ActivateL();
		}
	else
		{
		iAacListBox->SetSearchTextL( aMatchString );
		}
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::ClosePopupContactListL()
// -----------------------------------------------------------------------------
//
void CNcsHeaderContainer::ClosePopupContactListL()
	{
    FUNC_LOG;

    if ( iAacListBox->IsVisible() )
        {
        iAacListBox->MakeVisible( EFalse );
        ShowPopupMenuBarL( EFalse );
        
        // The focused address field should be redrawn after the popup is
        // closed to fix the field border.
        if ( iToField->IsFocused() )
            {
            iToField->DrawDeferred();
            }
        else if ( iCcField->IsFocused() )
            {
            iCcField->DrawDeferred();
            }
        else if ( iBccField->IsFocused() )
            {
            iBccField->DrawDeferred();
            }
        }
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::ShowPopupMenuBarL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::ShowPopupMenuBarL( TBool aShow )
	{
    FUNC_LOG;
	if ( iMenuBar ) 
		{
		if( aShow )
			{
			iMenuBar->SetCommandSetL( R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT );
			}
		else
			{
			iMenuBar->SetCommandSetL( R_FSE_EDITOR_SOFTKEYS_OPTIONS_CLOSE );
			SetMskL();
			}
        iMenuBar->DrawDeferred();
		}
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::DeleteSelectionL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::DeleteSelectionL()
	{
    FUNC_LOG;

    CCoeControl* focused = FindFocused();

    if ( IsAddressInputField( focused ) )
        {
    	CNcsAddressInputField* field = NULL;
    	field = static_cast<CNcsAddressInputField*>( focused );
    	TKeyEvent event;  // This mimics c -button pressing
    	event.iCode = EKeyBackspace;
    	event.iScanCode = EStdKeyBackspace;
    	event.iModifiers = EModifierAutorepeatable|EModifierNumLock;
    	event.iRepeats = 0;
    	TEventCode code = EEventKey;
    	field->OfferKeyEventL( event, code );
        }
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SelectAllToFieldTextL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SelectAllToFieldTextL()
	{
    FUNC_LOG;
	iToField->SelectAllTextL();
	}
		
// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SelectAllCcFieldTextL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SelectAllCcFieldTextL()
	{
    FUNC_LOG;
	iCcField->SelectAllTextL();
	}
	
// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SelectAllBccFieldTextL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SelectAllBccFieldTextL()
	{
    FUNC_LOG;
	iBccField->SelectAllTextL();
	}
	
// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SelectAllSubjectFieldTextL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SelectAllSubjectFieldTextL()
	{
    FUNC_LOG;
	iSubjectField->SelectAllTextL();
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::CalculatePopupRect
// -----------------------------------------------------------------------------
TRect CNcsHeaderContainer::CalculatePopupRect()
	{
    FUNC_LOG;
    // get focused control rect
	TRect popupRect;

    CCoeControl* focused = FindFocused();
    if ( IsAddressInputField( focused ) )
        {
        CNcsAddressInputField* aifEditor =
            static_cast<CNcsAddressInputField*>( focused );

        TRect editorRect = aifEditor->Editor()->Rect();
        
        popupRect.iTl =
            TPoint( editorRect.iTl.iX - 1, 
                    editorRect.iTl.iY + aifEditor->CursorPosition() + 1 );
        
        popupRect.iBr = 
            TPoint( editorRect.iBr.iX + 1, iParent.Rect().iBr.iY );
        }

    return popupRect;
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::DoPopupSelect
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::DoPopupSelectL()
	{
    FUNC_LOG;
    __ASSERT_DEBUG( iAacListBox, Panic( ENcsBasicUi ) );
    __ASSERT_DEBUG( &iMailBox, Panic( ENcsBasicUi ) );

	if( iAacListBox->IsRemoteLookupItemSelected() )
		{
		CCoeControl* focused = FindFocused();
		
		if ( IsAddressInputField( focused ) )
		    {
    		// Launch remote lookup
		    iRALInProgress = ETrue;
    		CNcsAddressInputField* addressField = NULL;
    		addressField = static_cast<CNcsAddressInputField*>( focused );
    		HBufC* lookupText = addressField->GetLookupTextLC();
    		CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason ex;
            CNcsEmailAddressObject* address = ExecuteRemoteSearchL(
                ex, *lookupText );
            iRALInProgress = EFalse;
            if ( address )
                {
                CleanupStack::PushL( address );
                IncludeAddressL( *address );
                CleanupStack::PopAndDestroy( address );
                }
            CleanupStack::PopAndDestroy( lookupText );
            CNcsComposeViewContainer& parent = 
                static_cast<CNcsComposeViewContainer&>( iParent );
            parent.FocusChanged( EDrawNow );
		    }
		}
	else if( !iAacListBox->IsPopupEmpty() )
		{
		CNcsEmailAddressObject* emailAddress  = 
            iAacListBox->ReturnCurrentEmailAddressLC();
		if( emailAddress )
			{
            if ( emailAddress->EmailAddress().Compare( KNullDesC ) != 0 )
                {
                IncludeAddressL( *emailAddress );
                }
            else
                {           
                // selected contact doesn't have email address, launch remote
                // contact lookup rcl must be usable, since otherwise there 
                // couldn't be any items without email addresses
                CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason
                    exitReason;
                CNcsEmailAddressObject* remAddress = ExecuteRemoteSearchL(
                    exitReason,
                    emailAddress->DisplayName() );
                if ( remAddress )
                    {
                    CleanupStack::PushL( remAddress );
                    IncludeAddressL( *remAddress );
                    CleanupStack::PopAndDestroy( remAddress );
                    }           
                }
            CleanupStack::PopAndDestroy( emailAddress );
            }
        }
	ClosePopupContactListL();
	}

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::IsPopupActive
// -----------------------------------------------------------------------------
TBool CNcsHeaderContainer::IsPopupActive() const
	{
    FUNC_LOG;
	if ( iAacListBox && iAacListBox->IsVisible() )
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsToFieldEmpty
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsToFieldEmpty() const
	{
    FUNC_LOG;
	return ( GetToFieldLength() < 1 );
	}
	
// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsCcFieldEmpty
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsCcFieldEmpty() const
	{
    FUNC_LOG;
   	return ( !IsCcFieldVisible() || GetCcFieldLength() < 1 );
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsBccFieldEmpty
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsBccFieldEmpty() const
	{
    FUNC_LOG;
	return ( !IsBccFieldVisible() || GetBccFieldLength() < 1 );
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsSubjectFieldEmpty
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsSubjectFieldEmpty() const
	{
    FUNC_LOG;
	return ( GetSubjectFieldLength() < 1 );
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetToFieldAddressesL
// ---------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& 
CNcsHeaderContainer::GetToFieldAddressesL( TBool aParseNow )
    {
	return iToField->GetAddressesL( aParseNow );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetCcFieldAddressesL
// ---------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& 
CNcsHeaderContainer::GetCcFieldAddressesL( TBool aParseNow )
    {
	return iCcField->GetAddressesL( aParseNow );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetBccFieldAddressesL
// ---------------------------------------------------------------------------
//
const RPointerArray<CNcsEmailAddressObject>& 
CNcsHeaderContainer::GetBccFieldAddressesL( TBool aParseNow )
    {
	return iBccField->GetAddressesL( aParseNow );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetSubjectLC
// ---------------------------------------------------------------------------
//
HBufC* CNcsHeaderContainer::GetSubjectLC() const
    {
    FUNC_LOG;
	HBufC* subject = 
        HBufC::NewLC( iSubjectField->Editor()->TextLength() + 2 );
	TPtr des = subject->Des();
	iSubjectField->Editor()->GetText( des );
	return subject;
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsBccFieldVisible
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsBccFieldVisible() const
    {
    FUNC_LOG;
	return iBccField->IsVisible(); 
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsCcFieldVisible
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsCcFieldVisible() const
    {
    FUNC_LOG;
	return iCcField->IsVisible(); 
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsFocusAttachments
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsFocusAttachments() const
    {
    FUNC_LOG;
	return ( FindFocused() == iAttachmentField );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsFocusTo
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsFocusTo() const
    {
    FUNC_LOG;
	return ( FindFocused() == iToField );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsFocusCc
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsFocusCc() const
    {
    FUNC_LOG;
	return ( FindFocused() == iCcField );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsFocusBcc
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsFocusBcc() const
    {
    FUNC_LOG;
	return ( FindFocused() == iBccField );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetCcFieldLength
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetCcFieldLength() const
    {
    FUNC_LOG;
	return iCcField->Editor()->TrimmedTextLength();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetBccFieldLength
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetBccFieldLength() const
    {
    FUNC_LOG;
	return iBccField->Editor()->TrimmedTextLength();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetToFieldLength
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetToFieldLength() const
    {
    FUNC_LOG;
	return iToField->Editor()->TrimmedTextLength();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetSubjectFieldLength
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetSubjectFieldLength() const
    {
    FUNC_LOG;
	return iSubjectField->Editor()->TrimmedTextLength();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetAttachmentCount
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetAttachmentCount() const
    {
    FUNC_LOG;
    CNcsComposeViewContainer& parent = 
        static_cast<CNcsComposeViewContainer&>( iParent );
    return parent.GetAttachmentCount();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::HasRemoteAttachments
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::HasRemoteAttachments() const
    {
    FUNC_LOG;
    CNcsComposeViewContainer& parent = 
        static_cast<CNcsComposeViewContainer&>( iParent );
    return parent.HasRemoteAttachments();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetToFieldSelectionLength
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetToFieldSelectionLength() const
    {
    FUNC_LOG;
	return iToField->TextEditor()->SelectionLength();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetCcFieldSelectionLength
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetCcFieldSelectionLength() const
    {
    FUNC_LOG;
	return iCcField->TextEditor()->SelectionLength();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetBccFieldSelectionLength
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::GetBccFieldSelectionLength()	const
    {
    FUNC_LOG;
	return iBccField->TextEditor()->SelectionLength();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::GetLookupTextLC
// ---------------------------------------------------------------------------
//
HBufC* CNcsHeaderContainer::GetLookupTextLC() const
    {
    FUNC_LOG;
	CCoeControl* focused = FindFocused();
	if ( !focused ) return NULL;
	
	CNcsAddressInputField* aif = dynamic_cast<CNcsAddressInputField*>(focused);
	if ( !aif ) return NULL;
	return aif->GetLookupTextLC();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SetToFieldAddressesL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SetToFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;
	iToField->SetAddressesL( aAddress );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SetCcFieldAddressesL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SetCcFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;
	iCcField->SetAddressesL( aAddress );
	if ( !iCcField->IsEmpty() )
		SetCcFieldVisibleL( ETrue, ENoDrawNow );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SetBccFieldAddressesL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SetBccFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddress )
    {
    FUNC_LOG;
	iBccField->SetAddressesL( aAddress );
	if ( !iBccField->IsEmpty() )
		SetBccFieldVisibleL( ETrue, ENoDrawNow );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::AppendToFieldAddressesL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::AppendToFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
	iToField->AppendAddressesL( aAddresses );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::AppendCcFieldAddressesL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::AppendCcFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
	iCcField->AppendAddressesL( aAddresses );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::AppendBccFieldAddressesL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::AppendBccFieldAddressesL( 
        const RPointerArray<CNcsEmailAddressObject>& aAddresses )
    {
    FUNC_LOG;
	iBccField->AppendAddressesL( aAddresses );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SetSubjectL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SetSubjectL( const TDesC& aSubject )
    {
    FUNC_LOG;
	iSubjectField->SetSubjectL( aSubject );
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::SetMenuBar
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::SetMenuBar( CEikButtonGroupContainer* aMenuBar )
    {
    FUNC_LOG;
	iMenuBar = aMenuBar;
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IncludeAddressL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::IncludeAddressL(const CNcsEmailAddressObject& aEml )
    {
    FUNC_LOG;

	CCoeControl* focused = FindFocused();
	if ( IsAddressInputField( focused ) )
	    {
    	CNcsAddressInputField* aifFocused = NULL;
    	aifFocused = static_cast<CNcsAddressInputField*>( focused );
    	aifFocused->AddAddressL( aEml );
	    }
	DoScrollL();
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IncludeAddressL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::IncludeAddressL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::FocusToField
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::FocusToField()
	{
    FUNC_LOG;
    CCoeControl* oldCtrl = FindFocused();
    if ( !oldCtrl )
    	{
    	return;
    	}
	oldCtrl->SetFocus( EFalse, ENoDrawNow );
	iToField->SetFocus( ETrue, ENoDrawNow );
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::FocusAttachmentField
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::FocusAttachmentField()
	{
    FUNC_LOG;
    CCoeControl* oldCtrl = FindFocused();
    if ( !oldCtrl )
    	{
    	return;
    	}
	oldCtrl->SetFocus( EFalse, ENoDrawNow );
	iAttachmentField->SetFocus( ETrue, ENoDrawNow );
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::AreAddressFieldsEmpty
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::AreAddressFieldsEmpty() const
	{
    FUNC_LOG;
    return ( IsToFieldEmpty() && IsCcFieldEmpty() && IsBccFieldEmpty() ) || 
            iRALInProgress;
	}

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::HandleDynamicVariantSwitchL
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::HandleDynamicVariantSwitchL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::LayoutLineCount
// ---------------------------------------------------------------------------
//
TInt CNcsHeaderContainer::LayoutLineCount() const
    {
    FUNC_LOG;
    TInt totalLineCount( 0 );
    CCoeControlArray::TCursor cur = Components().Begin();
    do
        {
        totalLineCount += ToNcsControl( cur )->LayoutLineCount();                       
        } 
    while ( cur.Next() );
    
    return totalLineCount;
    }


// ---------------------------------------------------------------------------
// CNcsHeaderContainer::IsAddressInputField
// ---------------------------------------------------------------------------
//
TBool CNcsHeaderContainer::IsAddressInputField(
        const CCoeControl* aControl ) const
    {
    FUNC_LOG;

    TBool ret = EFalse;

    if ( aControl != NULL && 
            ( aControl == iToField ||
              aControl == iCcField ||
              aControl == iBccField ) )
        {
        ret = ETrue;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CNcsHeaderContainer::ExecuteRemoteSearchL
// ---------------------------------------------------------------------------
//
CNcsEmailAddressObject* CNcsHeaderContainer::ExecuteRemoteSearchL(
    CPbkxRemoteContactLookupServiceUiContext::TResult::TExitReason& /*aExitReason*/,
    const TDesC& aSearchText )
    {
    FUNC_LOG;
    const TInt KMaxLength = 255;
    RBuf displayname;
    displayname.CreateL( KMaxLength );
    CleanupClosePushL( displayname );

    RBuf emailAddress;
    emailAddress.CreateL( KMaxLength );
    CleanupClosePushL( emailAddress );

    TBool contactSelected = 
        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->
            GetNameAndEmailFromRemoteLookupL( 
                iMailBox, aSearchText, displayname, emailAddress );

    CNcsEmailAddressObject* address = NULL;
    if ( contactSelected )
        {
        if ( !displayname.Length() )
            {
            address = 
                CNcsEmailAddressObject::NewL( emailAddress, emailAddress );
            }
        else
            {
            address = 
                CNcsEmailAddressObject::NewL( displayname, emailAddress );
            }
        }

    CleanupStack::PopAndDestroy( &emailAddress );
    CleanupStack::PopAndDestroy( &displayname );

    return address;
    }
   	
// -----------------------------------------------------------------------------
// CNcsHeaderContainer::ChangeMskCommandL
// Utility function to change the command ID and label on the middle soft key
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::ChangeMskCommandL( TInt aLabelResourceId )
    {
    FUNC_LOG;
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    if ( cba ) 
        {
        cba->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
                            aLabelResourceId );
        cba->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::SwitchChangeMskOff
// sets up iSwitchChangeMskOff falg 
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::SwitchChangeMskOff( TBool aTag )
    {
    iSwitchChangeMskOff = aTag;
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::OpenPhonebookL
// 
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::OpenPhonebookL()
    {
    CNcsComposeViewContainer* container = 
        static_cast<CNcsComposeViewContainer*>( &iParent );
    container->AppendAddressesL();
    }

void CNcsHeaderContainer::FixSemicolonInAddressFieldsL()
	{
		iToField->FixSemicolonAtTheEndL();
		iCcField->FixSemicolonAtTheEndL();
		iBccField->FixSemicolonAtTheEndL();
	}

TBool CNcsHeaderContainer::IsRemoteSearchInprogress() const
    {
    return iRALInProgress;
    }

// ---------------------------------------------------------------------------
// Commits changes made to given input field.
// ---------------------------------------------------------------------------
//
void CNcsHeaderContainer::CommitFieldL( CCoeControl* aField )
    {
    FUNC_LOG;
    CNcsComposeViewContainer* container = 
        static_cast<CNcsComposeViewContainer*>( &iParent );
    if ( container && aField )
        {
        if ( aField == iToField )
            {
            container->CommitL( EToField );
            }
        else if ( aField == iCcField )
            {
            container->CommitL( ECcField );
            }
        else if ( aField == iBccField )
            {
            container->CommitL( EBccField );
            }
        else if ( aField == iSubjectField ) 
            {
            container->CommitL( ESubjectField );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsHeaderContainer::DoScrollL
// 
// -----------------------------------------------------------------------------
void CNcsHeaderContainer::DoScrollL()
    {
    // scroll the screen if the cursor goes beyond the screen
    CNcsComposeViewContainer& parent = static_cast<CNcsComposeViewContainer&>( iParent );
    
    TInt screenPos( -Position().iY );
    TInt cursorPos( CursorPosition() );
    TInt lineHeight( Rect().Height() / LineCount() );
    TInt screenHeight( parent.Rect().Height() );
    
    if( cursorPos - lineHeight < screenPos )
        {
        screenPos = cursorPos - lineHeight;             
        }
    else
    if( cursorPos + lineHeight > screenPos + screenHeight )
        {
        screenPos = cursorPos + lineHeight - screenHeight;
        }
    
    parent.ScrollL( screenPos );
    PositionChanged();    
    }
