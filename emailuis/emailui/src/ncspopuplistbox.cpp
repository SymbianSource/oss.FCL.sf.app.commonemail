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
* Description: This file implements classes CNcsPopupListBox, CNcsListItemDrawer. 
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <eikclbd.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <FreestyleEmailUi.rsg>						// R_FSE_EDITOR_ADDRESS_LIST_REMOTE_LOOKUP_SEARCH
#include <StringLoader.h>						// StringLoader
#include <CPbkContactEngine.h>
#include <aknnotewrappers.h>					//For LanguageNotSupported errorNote

#include "ncspopuplistbox.h"
#include "ncsemailaddressobject.h"					// CNcsEmailAddressObject
#include "ncsconstants.h"
#include "ncsheadercontainer.h"						// CNcsHeaderContainer
#include "FreestyleEmailUiContactHandler.h"		// CFSEmailUiClsListsHandler
#include "ncsutility.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FSDelayedLoader.h"
#include "FreestyleEmailUiCLSItem.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CNcsPopupListBox::NewL
// -----------------------------------------------------------------------------
CNcsPopupListBox* CNcsPopupListBox::NewL( const CCoeControl* aParent, 
                                          CFSMailBox& aMailBox, 
                                          CNcsHeaderContainer& aHeaderContainer, 
                                          TBool aRemoteLookupSupported  )
	{
    FUNC_LOG;
	CNcsPopupListBox* self = 
	    new (ELeave) CNcsPopupListBox( aHeaderContainer, aRemoteLookupSupported, aMailBox );
	CleanupStack::PushL( self );
	self->ConstructL( aParent );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::CNcsPopupListBox
// -----------------------------------------------------------------------------
CNcsPopupListBox::CNcsPopupListBox( CNcsHeaderContainer& aHeaderContainer, 
                                    TBool aRemoteLookupSupported, 
                                    CFSMailBox& aMailBox)
	: iHeaderContainer( aHeaderContainer ),iMailBox( aMailBox ),
	  iRemoteLookupSupported( aRemoteLookupSupported ),iCachingInProgress( EFalse )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::ConstructL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::ConstructL( const CCoeControl* aParent )
    {
    FUNC_LOG;
    CEikTextListBox::ConstructL( aParent, CEikListBox::EPopout );
    CreateScrollBarFrameL();

	CEikTextListBox::SetBorder( TGulBorder::ESingleGray );

	const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont );
	CEikTextListBox::SetItemHeightL( font->HeightInPixels() + 8 );

    iBaseBackroundContext = CAknsBasicBackgroundControlContext::NewL( 
    	KAknsIIDQgnFsGrafEmailContent, 
    	Rect(), 
    	EFalse );
	iContactHandler = CFsDelayedLoader::InstanceL()->GetContactHandlerL();
	if ( !iContactHandler->IsLanguageSupportedL() )
		{
		iContactHandler = NULL;
		}
	
	iAppUi =  static_cast<CFreestyleEmailUiAppUi*>(iEikonEnv->AppUi()); //<cmail>
	
    // <cmail>
    SetListBoxObserver( this );
    // </cmail>
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::InitAndSearchL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::InitAndSearchL( const TDesC& aText )
	{
    FUNC_LOG;
 	SetSearchTextL( aText );
 	RPointerArray<CFSEmailUiClsItem> emptyArray;
 	CleanupClosePushL( emptyArray );
	OperationCompleteL( ESearchContacts, emptyArray );
	CleanupStack::PopAndDestroy( &emptyArray );
	}

// ---------------------------------------------------------------------------
// CNcsPopupListBox::~CNcsPopupListBox
// ---------------------------------------------------------------------------
CNcsPopupListBox::~CNcsPopupListBox()
    {
    FUNC_LOG;
    delete iBaseBackroundContext;
	delete iItemTextsArray;
	delete iCurrentSearchText;
	iMatchingItems.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::CreateItemDrawerL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::CreateItemDrawerL()
	{
    FUNC_LOG;
	CNcsListItemDrawer* drawer = new (ELeave) CNcsListItemDrawer( *this );
	iItemDrawer = drawer;
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::MopSupplyObject
// -----------------------------------------------------------------------------
TTypeUid::Ptr CNcsPopupListBox::MopSupplyObject(TTypeUid aId)
    {
    FUNC_LOG;
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBaseBackroundContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SizeChanged
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SizeChanged()
    {
    FUNC_LOG;
    CEikTextListBox::SizeChanged();
    iBaseBackroundContext->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::OfferKeyEventL
// -----------------------------------------------------------------------------
//    
TKeyResponse CNcsPopupListBox::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret( EKeyWasNotConsumed );
    if( aKeyEvent.iCode == EKeyDownArrow )
        {
        MoveRemoteLookupItemL( ERemoteLookupItemDown );
        iView->MoveCursorL( CListBoxView::ECursorNextItem, CListBoxView::ENoSelection );
		ret = EKeyWasConsumed;
        } 
    else if( aKeyEvent.iCode == EKeyUpArrow )
        {
		TBool stay = EFalse;
        // Move cursor separator line over
		if ( CurrentItemIndex() - 1 > 0 && CurrentItemIndex() - 1 == iRemoteLookupItemPos )
			{
	        MoveRemoteLookupItemL( ERemoteLookupItemUp );
	        iView->MoveCursorL( CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection );
			stay = ETrue;
			}

        MoveRemoteLookupItemL( ERemoteLookupItemUp );
        iView->MoveCursorL( CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection );
	    if( stay )
			{
	        MoveRemoteLookupItemL( ERemoteLookupItemDown );

			iView->MoveCursorL( CListBoxView::ECursorNextItem, CListBoxView::ENoSelection );
			}


    	ret = EKeyWasConsumed;
        }
       
    if( ret == EKeyWasNotConsumed )
        {
        ret = CEikListBox::OfferKeyEventL( aKeyEvent, aType );
        }
	// call HandleItemAdditionL just in case. There might be changes on remote lookup item place.
	// The call is here, because we don't want to have extra redraw events when the popuplist
	// is not fully updated.
	HandleItemAdditionL(); 
    return ret;
    }

// <cmail> Touch
// ---------------------------------------------------------------------------
// CNcsPopupListBox::HandleListBoxEventL
// ---------------------------------------------------------------------------
//      
void CNcsPopupListBox::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if ( aEventType == EEventItemClicked || 
    	 aEventType == EEventItemSingleClicked )
        {
        iHeaderContainer.DoPopupSelectL();
        }
    }
// </cmail>

// -----------------------------------------------------------------------------
// CNcsPopupListBox::OperationComplete
// -----------------------------------------------------------------------------
//
void CNcsPopupListBox::OperationCompleteL( TContactHandlerCmd /*aCmd*/,
    const RPointerArray<CFSEmailUiClsItem>& aMatchingItems )
    {
    FUNC_LOG;
    iMatchingItems.ResetAndDestroy();
    // Replace old matcing items.

    for ( TInt ii = 0; ii < aMatchingItems.Count(); ++ii )
    	{
    	if ( aMatchingItems[ii] )
    		{
    		CFSEmailUiClsItem* item = aMatchingItems[ii]->CloneLC();  		
    		iMatchingItems.AppendL( item );
    		CleanupStack::Pop( item );
    		}
    	}
    
    UpdateListL();
    }

void CNcsPopupListBox::OperationErrorL( TContactHandlerCmd aCmd, TInt aError )
	{
    FUNC_LOG;
	
	if ( aCmd == ESearchContacts )
		{
		// KerrNotReady --> caching in progress, KErrNotFound --> caching not started yet
		if ( (aError == KErrNotReady) || (aError == KErrNotFound) )
			{
			if(!iAppUi->AppUiExitOngoing()) //<cmail>
			    TFsEmailUiUtility::ShowInfoNoteL( R_FSE_EDITOR_INFO_BUILDING_CACHE );
			iCachingInProgress = ETrue;
			}
		if ( aError == KErrNone && iCachingInProgress )
			{
			if(!iAppUi->AppUiExitOngoing()) //<cmail>
			    TFsEmailUiUtility::ShowInfoNoteL( R_FSE_EDITOR_INFO_CACHING_COMPLETED );
			iCachingInProgress = EFalse;
			}
		}
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetSearchTextL
// -----------------------------------------------------------------------------
//
void CNcsPopupListBox::SetSearchTextL( const TDesC& aText )
    {
    FUNC_LOG;
    delete iCurrentSearchText;
    iCurrentSearchText = NULL;
    iCurrentSearchText = aText.AllocL();
    if ( !iCachingInProgress  )
        {
        if ( iContactHandler  )
            {
            iContactHandler->SearchMatchesL( aText, this, &iMailBox );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::ReturnCurrentEmailAddressLC
// -----------------------------------------------------------------------------
//
CNcsEmailAddressObject* CNcsPopupListBox::ReturnCurrentEmailAddressLC()
	{
    FUNC_LOG;
	CNcsEmailAddressObject* addressObject = NULL;

    if ( iMatchingItems.Count() )
		{
		CFSEmailUiClsItem* clsItem = NULL;
		if ( iRemoteLookupSupported )
			{
			clsItem = iMatchingItems[CurrentItemIndex()-1];  // -1 because of iRemoteLookupItemPos
			}
		else
			{
			clsItem = iMatchingItems[CurrentItemIndex()]; // no iRemoteLookupItemPos			
			}
		addressObject= CNcsEmailAddressObject::NewL( clsItem->DisplayName(), clsItem->EmailAddress() );
        CleanupStack::PushL( addressObject );			
        if ( clsItem->MultipleEmails() )
        	{

        	addressObject->SetDisplayFull( ETrue );
        	}
      
        }
	return addressObject;
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetPopupMaxRect
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetPopupMaxRect( const TRect& aPopupMaxRect )
	{
    FUNC_LOG;
	iPopupMaxRect = aPopupMaxRect;
	SetPopupHeight();
	TRAP_IGNORE( UpdateScrollBarsL() );
 	TRAP_IGNORE( SetScrollBarVisibilityL() );
	}
	
// -----------------------------------------------------------------------------
// CNcsPopupListBox::IsPopupEmpty
// -----------------------------------------------------------------------------
TBool CNcsPopupListBox::IsPopupEmpty() const
	{
    FUNC_LOG;
	if( Model()->NumberOfItems() > 0 )
		return EFalse;
	else
		return ETrue;
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::IsRemoteLookupItemSelected
// -----------------------------------------------------------------------------
TBool CNcsPopupListBox::IsRemoteLookupItemSelected() const
	{
    FUNC_LOG;
	if( CurrentItemIndex() == iRemoteLookupItemPos && iRemoteLookupSupported )
		return ETrue;
	else
		return EFalse;
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::CurrentPopupClsItemsArray
// -----------------------------------------------------------------------------

const RPointerArray<CFSEmailUiClsItem>& CNcsPopupListBox::CurrentPopupClsItemsArray() const
	{
	return iMatchingItems;
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::RemoteLookupItemPos
// -----------------------------------------------------------------------------
TInt CNcsPopupListBox::RemoteLookupItemPos() const
	{
    FUNC_LOG;
	return iRemoteLookupItemPos;
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::LayoutHandler
// -----------------------------------------------------------------------------
CFSEmailUiLayoutHandler& CNcsPopupListBox::LayoutHandler() const
    {
    FUNC_LOG;
    CFreestyleEmailUiAppUi* appUi = 
        static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    return *appUi->LayoutHandler();
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetListItemsFromArrayL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetListItemsFromArrayL()
	{
    FUNC_LOG;
	// reset the cursor to point to the first item
    //iView->MoveCursorL( CListBoxView::ECursorFirstItem, CListBoxView::ENoSelection);
	Reset();
	
	// Create totally new text array
	CreateTextArrayAndSetToTheListboxL( ETrue );

	// append texts to text array
	for( TInt i=0; i < iMatchingItems.Count(); i++ )
		{
		iItemTextsArray->AppendL( iMatchingItems[i]->FullTextL() );
		}

	// Update rmlu item
	SetRemoteLookupItemFirstToTheListL();
	
	SetPopupHeight();
	SetScrollBarVisibilityL();
	HandleItemAdditionL();
	
	if( iItemTextsArray && iItemTextsArray->Count() > 0 ) 
		SetCurrentItemIndex( 0 );

	if( IsVisible() )
		DrawDeferred();
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetPopupHeight
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetPopupHeight()
	{
    FUNC_LOG;
	TRect newRect = iPopupMaxRect;

	// This is the total height in pixels needed to show all items
    TInt itemCount = Model()->NumberOfItems();
 	TInt minimumHeight = CalcHeightBasedOnNumOfItems( itemCount );
 	
 	// Shrink list size if maximum size is not needed
	if( iPopupMaxRect.Height() > minimumHeight )
	    {
		newRect.SetHeight( minimumHeight );
	    }
	SetRect( newRect );
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetScrollBarVisibilityL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetScrollBarVisibilityL()
	{
    FUNC_LOG;
	// This is the total height in pixels needed to show all items
	TInt minimumHeight = CalcHeightBasedOnNumOfItems( Model()->NumberOfItems() );

	// Show scroll bar if there is more items we can show at once.
	if( iPopupMaxRect.Height() > minimumHeight ) 
		{
		ScrollBarFrame()->SetScrollBarVisibilityL(
			CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff ); 
		}
	else 
		{
		ScrollBarFrame()->SetScrollBarVisibilityL(
			CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn ); 
		}
	}


// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetRemoteLookupItemFirstToTheListL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetRemoteLookupItemFirstToTheListL()
	{
    FUNC_LOG;
	if( iRemoteLookupSupported )
		{
		HBufC* rmluText = StringLoader::LoadLC( R_FSE_EDITOR_ADDRESS_LIST_REMOTE_LOOKUP_SEARCH, *iCurrentSearchText );
		
		iItemTextsArray->InsertL( 0, *rmluText );
		CleanupStack::PopAndDestroy( rmluText );
		iRemoteLookupItemPos = 0;
		}
	else
		{
		iRemoteLookupItemPos = -1;
		}
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::MoveRemoteLookupItemL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::MoveRemoteLookupItemL( TRemoteLookupItemMoveDirection aDirection )
	{
    FUNC_LOG;
	if( iRemoteLookupSupported )
		{
		TInt newRMLUItemIndex = -1;
		TInt newCurrentItem = -1;
		if( aDirection == ERemoteLookupItemUp &&
			 iView->CurrentItemIndex() == iView->TopItemIndex() )
			{
			newRMLUItemIndex = iRemoteLookupItemPos - 1;
			newCurrentItem = CurrentItemIndex() - 1;
			}
		else if( aDirection == ERemoteLookupItemDown &&
				 iView->CurrentItemIndex() == iView->BottomItemIndex() )
			{
			newRMLUItemIndex = iRemoteLookupItemPos + 1;
			newCurrentItem = CurrentItemIndex() + 1;
			}


		if( ItemExists ( newCurrentItem ) )
			{
			iItemTextsArray->Delete( iRemoteLookupItemPos );

			HBufC* rmluText = StringLoader::LoadLC( R_FSE_EDITOR_ADDRESS_LIST_REMOTE_LOOKUP_SEARCH, *iCurrentSearchText );
			
			iItemTextsArray->InsertL( newRMLUItemIndex, *rmluText );
			CleanupStack::PopAndDestroy( rmluText );
			iRemoteLookupItemPos = newRMLUItemIndex;
			}
		}
	}


// -----------------------------------------------------------------------------
// CNcsPopupListBox::CreateTextArrayAndSetToTheListboxL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::CreateTextArrayAndSetToTheListboxL( const TBool& aResetIfExists )
	{
    FUNC_LOG;
	if( !iItemTextsArray )
		{
		iItemTextsArray = new ( ELeave ) CDesCArraySeg( 8 );
		// Set the popup list data
		Model()->SetItemTextArray( iItemTextsArray );
		Model()->SetOwnershipType( ELbmDoesNotOwnItemArray  );
		}
		
	if( aResetIfExists )
		{
		delete iItemTextsArray;
		iItemTextsArray = NULL;
		CreateTextArrayAndSetToTheListboxL( EFalse );
		}
	}


// -----------------------------------------------------------------------------
// CNcsListItemDrawer::CNcsListItemDrawer
// -----------------------------------------------------------------------------
CNcsListItemDrawer::CNcsListItemDrawer( const CNcsPopupListBox& aListBox )
:CListItemDrawer(), iListBox(aListBox)
	{
    FUNC_LOG;
	// Store a GC for later use
	iGc = &CCoeEnv::Static()->SystemGc();
	SetGc(iGc);
	}

// -----------------------------------------------------------------------------
// CNcsListItemDrawer::DrawActualItem
// -----------------------------------------------------------------------------
void CNcsListItemDrawer::DrawActualItem(TInt aItemIndex,
	const TRect& aActualItemRect, TBool aItemIsCurrent,
	TBool /*aViewIsEmphasized*/, TBool /*aViewIsDimmed*/,
	TBool /*aItemIsSelected*/) const
	{
    FUNC_LOG;
	// <cmail>
	iGc->Reset();
	// </cmail>
	
	// Get reference to curren popup cls item list.
	const RPointerArray<CFSEmailUiClsItem>& clsItemArray = 	iListBox.CurrentPopupClsItemsArray();
	TInt rmluPosition = iListBox.RemoteLookupItemPos();

	// Sets all the attributes, like font, text color and background color.
	const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont );
	iGc->UseFont(font);

	// We have to draw the item in layered fassion in order to do the skin
	// First clear the backround by drawing a solid white rect.	
	iGc->SetPenColor(iBackColor);
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetBrushColor(iBackColor);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	// Now draw the highlight
	if( aItemIsCurrent ) 
		{
		TRgb highlightColor = iListBox.LayoutHandler().PcsPopupHighlightColor();
		iGc->SetBrushColor(highlightColor);
//	    AknsDrawUtils::DrawFrame( AknsUtils::SkinInstance(), *iGc,
//	    	aActualItemRect, aActualItemRect,
//	    	KNcsIIDHighlightBackground, KNcsIIDHighlightBackground );
		}
	iGc->DrawRect(aActualItemRect);


	iGc->SetPenColor(iTextColor);
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	TInt topToBaseline = ( aActualItemRect.Height() - font->HeightInPixels() ) / 2
						+ font->AscentInPixels();

	TPtrC itemText = iListBox.Model()->ItemText( aItemIndex );

	// Construct bidirectional text object
	// <cmail> Removed leave from non-leaving function.
	TBidiText* bidiText = NULL;
	TRAPD( err, bidiText = TBidiText::NewL( itemText, 1 ) );
	if ( err == KErrNone )
		{
		bidiText->WrapText( aActualItemRect.Width(), *font, NULL );
		}
		
	TPoint leftBase = aActualItemRect.iTl + TPoint( 0, topToBaseline );

    if ( err != KErrNone )
        {
        bidiText = NULL; // to be on the safe side
        }

	// check if we are drawing remote lookup item or contact match
	else if ( rmluPosition == aItemIndex )
		{
		iGc->SetUnderlineStyle( EUnderlineOff );
		bidiText->DrawText( *iGc, leftBase );
		}
	else
		{
		// if list has rmlu item change item index right
		if ( rmluPosition >= 0 )
			{
			aItemIndex--;
			}

		// change color to gray if match doesn't have email address.
		if( clsItemArray[aItemIndex]->EmailAddress().Compare( KNullDesC ) == 0 )
			{
			TRgb dimmedColor = iListBox.LayoutHandler().PcsPopupDimmedFontColor();
			iGc->SetPenColor( dimmedColor );
			}

		// For now, we support underlining the matching part only if the
		//       text is written completely with left-to-right script
		
		// We know the text contains RTL script if the display string is not just
		// truncated version of the original string.
		TPtrC dispText = bidiText->DisplayText();
		TInt compLength = dispText.Length() - 1; // -1 to omit the truncation character
		TBool textContainsRtl = 
            ( itemText.Left(compLength) != dispText.Left(compLength) );
		
        const RArray<TPsMatchLocation>& underlines = clsItemArray[aItemIndex]->Highlights();
        
		if ( underlines.Count() > 0 && !textContainsRtl )
			{
			TInt i = 0;
			TBool partsLeft = ETrue;
			TInt currentTextStart = 0;
			TInt currentTextLength = 0;

			while ( partsLeft )
				{
				if ( currentTextStart < underlines[i].index )
					{
					// draw letters to the start of the underlined part
					currentTextLength = underlines[i].index - currentTextStart;
					DrawPartOfItem( aActualItemRect, *font, currentTextStart, currentTextLength, itemText,
									EFalse, topToBaseline );
					}
				else if ( currentTextStart == underlines[i].index )
					{
					// draw underlined letters
					currentTextLength = underlines[i].length;
					
					DrawPartOfItem( aActualItemRect, *font, currentTextStart, currentTextLength, itemText,
									ETrue, topToBaseline );
					i++;
					}
				else 
					{
					// This is here, because PCS Engine might give you duplicate match entries,
					// in this case we're not advancing text but we'll skip that match
					currentTextLength = 0;
					i++;
					}
					// update text start point
					currentTextStart += currentTextLength;
				
				if ( i >= underlines.Count() )
					{
					partsLeft = EFalse;
					// draw rest of the letters, if there are any after the last underlined part
					if ( currentTextStart < itemText.Length() )
						{
						currentTextLength = itemText.Length() - currentTextStart;
						DrawPartOfItem( aActualItemRect, *font, currentTextStart, currentTextLength, itemText,
										EFalse, topToBaseline );
						}
					}
				
				}
			}
		else
			{
			iGc->SetUnderlineStyle( EUnderlineOff );
			bidiText->DrawText( *iGc, leftBase );
			}		
		}
		
	delete bidiText;
	bidiText = NULL;
	}

// -----------------------------------------------------------------------------
// CNcsListItemDrawer::DrawPartOfItem
// -----------------------------------------------------------------------------
void CNcsListItemDrawer::DrawPartOfItem( const TRect& aItemRect, const CFont& aFont,
						 TInt aStartPos, TInt aLength, const TDesC& aDes,
						 TBool aUnderlined, TInt aBaselineOffsetFromTop  ) const
	{
    FUNC_LOG;
	if( aUnderlined )
		{
		iGc->SetUnderlineStyle( EUnderlineOn );
		}
	else
		{
		iGc->SetUnderlineStyle( EUnderlineOff );
		}
	TRect currentTextRect( aItemRect );
	TInt pixels = aFont.TextWidthInPixels( aDes.Left( aStartPos ) );
	currentTextRect.iTl.iX = currentTextRect.iTl.iX + pixels;
	iGc->DrawText( aDes.Mid( aStartPos, aLength ), 
	        currentTextRect, aBaselineOffsetFromTop );
	}


void CNcsPopupListBox::UpdateListL()
    {
    FUNC_LOG;
    SetListItemsFromArrayL();

    // Close the popup if it's empty and make sure it's visible if it's not empty.
    if ( IsPopupEmpty() )
        {
        iHeaderContainer.ClosePopupContactListL();
        }
    else
        {
        MakeVisible( ETrue );
        iHeaderContainer.ShowPopupMenuBarL( ETrue );
        }
    }

// End of File

