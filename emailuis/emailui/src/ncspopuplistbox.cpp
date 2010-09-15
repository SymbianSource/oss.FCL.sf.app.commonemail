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
#include <AknsFrameBackgroundControlContext.h>
#include <FreestyleEmailUi.rsg>
#include <StringLoader.h>
#include <CPbkContactEngine.h>
#include <AknsDrawUtils.h>
#include <AknsUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include "ncspopuplistbox.h"
#include "ncsemailaddressobject.h"
#include "ncsconstants.h"
#include "ncsheadercontainer.h"
#include "FreestyleEmailUiContactHandler.h"
#include "ncsutility.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FSDelayedLoader.h"
#include "FreestyleEmailUiCLSItem.h"

const TInt KHighlightFrameWidth = 2;

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CNcsPopupListBox::NewL
// -----------------------------------------------------------------------------
CNcsPopupListBox* CNcsPopupListBox::NewL( const CCoeControl* aParent, 
                                          CFSMailBox& aMailBox, 
                                          MNcsPopupListBoxObserver& aObserver,
                                          TBool aRemoteLookupSupported  )
    {
    FUNC_LOG;
    CNcsPopupListBox* self =  new (ELeave) CNcsPopupListBox( aMailBox,
        aObserver, aRemoteLookupSupported );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::CNcsPopupListBox
// -----------------------------------------------------------------------------
CNcsPopupListBox::CNcsPopupListBox( CFSMailBox& aMailBox,
    MNcsPopupListBoxObserver& aObserver, TBool aRemoteLookupSupported )
    : iMailBox( aMailBox ),
      iObserver( aObserver ),
      iRemoteLookupSupported( aRemoteLookupSupported ),
      iCachingInProgress( EFalse ),
      iAppUi( static_cast<CFreestyleEmailUiAppUi*>( iEikonEnv->AppUi() ) )
    {
    FUNC_LOG;
    iPopupMaxRect = TRect( 100, 100, 100, 100 );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::ConstructL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::ConstructL( const CCoeControl* aParent )
    {
    FUNC_LOG;
    CEikTextListBox::ConstructL( NULL, CEikListBox::EPopout );
    SetMopParent( const_cast<CCoeControl*>( aParent ) );
    User::LeaveIfError( SetParent( const_cast<CCoeControl*>( aParent ) ) );

    CreateScrollBarFrameL( EFalse, EFalse, ETrue );

    CEikTextListBox::SetBorder( TGulBorder::ENone );

    const CFont* font = 
            AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont );
    CEikTextListBox::SetItemHeightL( font->FontMaxHeight() + 12 );

    iBackgroundContext = CAknsFrameBackgroundControlContext::NewL(
        KAknsIIDQsnFrPopupSub, Rect(), Rect(), EFalse );
    UpdateTextColors();

    iContactHandler = CFsDelayedLoader::InstanceL()->GetContactHandlerL();
    if ( !iContactHandler->IsLanguageSupportedL() )
        {
        iContactHandler = NULL;
        }

    SetListBoxObserver( this );
    MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::InitAndSearchL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::InitAndSearchL( const TDesC& aText, TInt aMode )
    {
    FUNC_LOG;
    SetSearchTextL( aText, aMode );
    RPointerArray<CFSEmailUiClsItem> emptyArray;
    CleanupClosePushL( emptyArray );
    OperationCompleteL( ESearchContacts, emptyArray );
    CleanupStack::PopAndDestroy( &emptyArray );
    }

// ---------------------------------------------------------------------------
// Hides the popup window.
// ---------------------------------------------------------------------------
void CNcsPopupListBox::ClosePopupL()
    {
    UpdateVisibilityL( EFalse );
    }

// ---------------------------------------------------------------------------
// CNcsPopupListBox::~CNcsPopupListBox
// ---------------------------------------------------------------------------
CNcsPopupListBox::~CNcsPopupListBox()
    {
    FUNC_LOG;
    delete iBackgroundContext;
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
    iItemDrawer = new (ELeave) CNcsListItemDrawer( *this );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::MopSupplyObject
// -----------------------------------------------------------------------------
TTypeUid::Ptr CNcsPopupListBox::MopSupplyObject( TTypeUid aId )
    {
    FUNC_LOG;
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBackgroundContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SizeChanged
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SizeChanged()
    {
    FUNC_LOG;
    const TRect outerRect = Rect();

    TAknLayoutRect subpane;
    subpane.LayoutRect( outerRect, 
        AknLayoutScalable_Avkon::bg_popup_sub_pane_g1() );
    const TRect innerRect = subpane.Rect();

    iBackgroundContext->SetFrameRects( outerRect, innerRect );

    SetViewRectFromClientRect( innerRect );
    TRAP_IGNORE( HandleViewRectSizeChangeL() );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CNcsPopupListBox::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CEikTextListBox::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange )
        {
        UpdateTextColors();
        }
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CNcsPopupListBox::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    CEikTextListBox::HandlePointerEventL( aPointerEvent );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CNcsPopupListBox::OfferKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret( EKeyWasNotConsumed );
    if( aKeyEvent.iCode == EKeyDownArrow )
        {
        MoveRemoteLookupItemL( ERemoteLookupItemDown );
        iView->MoveCursorL( 
                CListBoxView::ECursorNextItem, CListBoxView::ENoSelection );
		ret = EKeyWasConsumed;
        } 
    else if( aKeyEvent.iCode == EKeyUpArrow )
        {
		TBool stay = EFalse;
        // Move cursor separator line over
		if ( CurrentItemIndex() - 1 > 0 && 
		     CurrentItemIndex() - 1 == iRemoteLookupItemPos )
			{
	        MoveRemoteLookupItemL( ERemoteLookupItemUp );
	        iView->MoveCursorL( CListBoxView::ECursorPreviousItem, 
	                            CListBoxView::ENoSelection );
			stay = ETrue;
			}

        MoveRemoteLookupItemL( ERemoteLookupItemUp );
        iView->MoveCursorL( CListBoxView::ECursorPreviousItem, 
                            CListBoxView::ENoSelection );
	    if( stay )
			{
	        MoveRemoteLookupItemL( ERemoteLookupItemDown );

			iView->MoveCursorL( CListBoxView::ECursorNextItem, 
			                    CListBoxView::ENoSelection );
			}


    	ret = EKeyWasConsumed;
        }
       
    if( ret == EKeyWasNotConsumed )
        {
        ret = CEikListBox::OfferKeyEventL( aKeyEvent, aType );
        }
	// call HandleItemAdditionL just in case. There might be changes on remote
    // lookup item place.
	// The call is here, because we don't want to have extra redraw events when
    // the popuplist is not fully updated.
	HandleItemAdditionL(); 
    return ret;
    }

// ---------------------------------------------------------------------------
// CNcsPopupListBox::HandleListBoxEventL
// ---------------------------------------------------------------------------
//      
void CNcsPopupListBox::HandleListBoxEventL( CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    if ( aEventType == EEventItemClicked || 
         aEventType == EEventItemSingleClicked )
        {
        iObserver.PopupItemSelectedL();
        }
    }

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
		// KerrNotReady --> caching in progress,
	    // KErrNotFound --> caching not started yet
		if ( ( aError == KErrNotReady ) || ( aError == KErrNotFound ) )
			{
			if( !iAppUi->AppUiExitOngoing() )
			    {
			    TFsEmailUiUtility::ShowInfoNoteL( 
			            R_FSE_EDITOR_INFO_BUILDING_CACHE );
			    }
			iCachingInProgress = ETrue;
			}
		if ( aError == KErrNone && iCachingInProgress )
			{
			if ( !iAppUi->AppUiExitOngoing() )
			    {
			    TFsEmailUiUtility::ShowInfoNoteL( 
			            R_FSE_EDITOR_INFO_CACHING_COMPLETED );
			    }
			iCachingInProgress = EFalse;
			}
		}
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetSearchTextL
// -----------------------------------------------------------------------------
//
void CNcsPopupListBox::SetSearchTextL( const TDesC& aText, TInt aMode )
    {
    FUNC_LOG;
    delete iCurrentSearchText;
    iCurrentSearchText = NULL;
    iCurrentSearchText = aText.AllocL();
    if ( !iCachingInProgress )
        {
        if ( iContactHandler )
            {
            iContactHandler->SearchMatchesL( aText, this, &iMailBox, aMode );
            }
        else
            {
            // update list for languages which don't support PCS
            UpdateListL();
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
            // calculate index of item
            TInt index = ( CurrentItemIndex() > iRemoteLookupItemPos ? 
                CurrentItemIndex() - 1 : CurrentItemIndex() );
            clsItem = iMatchingItems[index];
            }
        else
            {
            clsItem = iMatchingItems[CurrentItemIndex()];
            }

        addressObject= CNcsEmailAddressObject::NewL( 
                clsItem->DisplayName(), clsItem->EmailAddress() );
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

    SetPopupRect();
    TRAP_IGNORE( UpdateScrollBarsL() );
    TRAP_IGNORE( SetScrollBarVisibilityL() );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::IsPopupEmpty
// -----------------------------------------------------------------------------
TBool CNcsPopupListBox::IsPopupEmpty() const
    {
    FUNC_LOG;
    if ( Model()->NumberOfItems() > 0 )
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
    if ( CurrentItemIndex() == iRemoteLookupItemPos && iRemoteLookupSupported )
        return ETrue;
    else
        return EFalse;
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::CurrentPopupClsItemsArray
// -----------------------------------------------------------------------------
const RPointerArray<CFSEmailUiClsItem>& 
CNcsPopupListBox::CurrentPopupClsItemsArray() const
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
    return *iAppUi->LayoutHandler();
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetListItemsFromArrayL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetListItemsFromArrayL()
	{
    FUNC_LOG;

	Reset();
	
	// Create totally new text array
	CreateTextArrayAndSetToTheListboxL( ETrue );

	TInt matchCount = iMatchingItems.Count();
	// Remote lookup item's index is set as last item by default
	// (in case of empty matching items list it is the only item)
	TInt rmluIndex = matchCount;

	// append texts to text array
    for( TInt i=0; i < matchCount; i++ )
        {
        iItemTextsArray->AppendL( iMatchingItems[i]->FullTextL() );
        if ( iRemoteLookupSupported && 
             rmluIndex == matchCount &&
             iMatchingItems[i]->EmailAddress().Compare( KNullDesC ) == 0 )
            {
            rmluIndex = i;
            }
        }

	// Update rmlu item
	SetRemoteLookupItemToTheListL( rmluIndex );

	SetPopupRect();
	SetScrollBarVisibilityL();
	HandleItemAdditionL();

    if ( iItemTextsArray && iItemTextsArray->Count() > 0 )
        {
        SetCurrentItemIndex( 0 );
        }

    if ( IsVisible() )
        {
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::RoundToItemHeight
// -----------------------------------------------------------------------------
TInt CNcsPopupListBox::RoundToItemHeight( const TInt aPopupHeight ) const
    {
    TReal fullItems; // number of full visible items in window
    TInt err = Math::Round( fullItems, ( aPopupHeight / ItemHeight() ), 0 );
    if ( err == KErrNone )
        {
        return (TInt)( fullItems * ItemHeight() ); 
        }
    return aPopupHeight; // in case of error
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetPopupRect
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetPopupRect()
    {
    FUNC_LOG;

    // The popup height and vertical position is adjusted based on the
    // available space below and above the cursor and the number of items
    // in the list.
    TRect newRect = iPopupMaxRect;
    TAknLayoutRect subpane;
    subpane.LayoutRect( newRect, 
            AknLayoutScalable_Avkon::bg_popup_sub_pane_g1() );
    const TInt frameHeights = 
            iPopupMaxRect.Height() - subpane.Rect().Height();

    // This is the total height in pixels needed to show all items
    const TInt minimumHeight = frameHeights + 
        CalcHeightBasedOnNumOfItems( Model()->NumberOfItems() );

    // Get height of one line in Address field
    TInt toLineHeight =
            static_cast<CNcsHeaderContainer*>( Parent() )->GetToLineHeight();
    if ( toLineHeight == 0 )
        {
        toLineHeight = ItemHeight();
        }

    TInt newHeight = minimumHeight; // default window height
    CCoeControl* container = Parent()->Parent();
    const TInt maxHeight = 
            container->Rect().Height() - 
            ( newRect.iTl.iY - container->PositionRelativeToScreen().iY );
    newHeight = 
            RoundToItemHeight( maxHeight - frameHeights ) + frameHeights;
    if ( newHeight > minimumHeight )
        {
        newHeight = minimumHeight; // shrink window (if needed)
        }

    newRect.SetHeight( newHeight );
    SetRect( newRect );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::SetScrollBarVisibilityL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetScrollBarVisibilityL()
	{
    FUNC_LOG;
	// This is the total height in pixels needed to show all items
	TInt minimumHeight =
	        CalcHeightBasedOnNumOfItems( Model()->NumberOfItems() );

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
// CNcsPopupListBox::SetRemoteLookupItemToTheListL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::SetRemoteLookupItemToTheListL( TInt aIndex )
	{
    FUNC_LOG;
	if( iRemoteLookupSupported )
		{
		HBufC* rmluText = StringLoader::LoadLC( 
		    R_FSE_EDITOR_ADDRESS_LIST_REMOTE_LOOKUP_SEARCH, 
		    *iCurrentSearchText );
		
		iItemTextsArray->InsertL( aIndex, *rmluText );
		CleanupStack::PopAndDestroy( rmluText );
		iRemoteLookupItemPos = aIndex;
		}
	else
		{
		iRemoteLookupItemPos = -1;
		}
	}

// -----------------------------------------------------------------------------
// CNcsPopupListBox::MoveRemoteLookupItemL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::MoveRemoteLookupItemL( 
        TRemoteLookupItemMoveDirection aDirection )
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


		if( ItemExists ( newCurrentItem ) && newRMLUItemIndex != -1 )
			{
			iItemTextsArray->Delete( iRemoteLookupItemPos );

			HBufC* rmluText = StringLoader::LoadLC(
			        R_FSE_EDITOR_ADDRESS_LIST_REMOTE_LOOKUP_SEARCH,
			        *iCurrentSearchText );
			
			iItemTextsArray->InsertL( newRMLUItemIndex, *rmluText );
			CleanupStack::PopAndDestroy( rmluText );
			iRemoteLookupItemPos = newRMLUItemIndex;
			}
		}
	}


// -----------------------------------------------------------------------------
// CNcsPopupListBox::CreateTextArrayAndSetToTheListboxL
// -----------------------------------------------------------------------------
void CNcsPopupListBox::CreateTextArrayAndSetToTheListboxL(
        const TBool& aResetIfExists )
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
:CListItemDrawer(), iListBox( aListBox )
    {
    FUNC_LOG;
    // Store a GC for later use
    iGc = &CCoeEnv::Static()->SystemGc();
    SetGc( iGc );
    }

// -----------------------------------------------------------------------------
// CNcsListItemDrawer::DrawActualItem
// -----------------------------------------------------------------------------
void CNcsListItemDrawer::DrawActualItem( TInt aItemIndex,
    const TRect& aActualItemRect, TBool aItemIsCurrent,
    TBool /*aViewIsEmphasized*/, TBool /*aViewIsDimmed*/,
    TBool /*aItemIsSelected*/ ) const
    {
    FUNC_LOG;
    iGc->DiscardFont();

    // Get reference to curren popup cls item list.
    const RPointerArray<CFSEmailUiClsItem>& clsItemArray =
        iListBox.CurrentPopupClsItemsArray();
    TInt rmluPosition = iListBox.RemoteLookupItemPos();

    // Sets all the attributes, like font, text color and background color.
    const CFont* font = AknLayoutUtils::FontFromId(
            EAknLogicalFontPrimarySmallFont );
    iGc->UseFont( font );
    iGc->SetPenStyle( CGraphicsContext::ESolidPen );
    iGc->SetBrushStyle( CGraphicsContext::ENullBrush );

    TRect itemRect = aActualItemRect;
    { 
    // temporary const_cast to get the scrollbar width
    CNcsPopupListBox& tmpListBox = const_cast<CNcsPopupListBox&>( iListBox );
    const TInt scrollbarBreadth = tmpListBox.ScrollBarFrame()->
        ScrollBarBreadth( CEikScrollBar::EVertical );
    if ( scrollbarBreadth > 0 )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            itemRect.iTl.iX = iListBox.Rect().iTl.iX + scrollbarBreadth;
            }
        else
            {
            itemRect.iBr.iX = iListBox.Rect().iBr.iX - scrollbarBreadth;
            }
        }
    }

    TRect textRect = itemRect;
    textRect.Shrink( KHighlightFrameWidth, KHighlightFrameWidth );

    // Get skin instance.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Draw background using the control context from the list box.
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( &iListBox );
    if ( cc )
        {
        AknsDrawUtils::Background( skin, cc, *iGc, aActualItemRect );
        }
    else
        {
        iGc->Clear( aActualItemRect );
        }

    // Draw the highlight, when necessary.
    if ( aItemIsCurrent ) 
        {
        AknsDrawUtils::DrawFrame( skin, *iGc, itemRect, textRect,
            KAknsIIDQsnFrList, KAknsIIDQsnFrListCenter );
        }

    // Get text color.
    TRgb textColor = TextColor();
    if ( aItemIsCurrent )
        {
        textColor = HighlightedTextColor();
        }

    iGc->SetPenColor( textColor );
    TInt topToBaseline = ( textRect.Height() - font->FontMaxHeight() ) / 2
        + font->FontMaxAscent();

    TPtrC itemText = iListBox.Model()->ItemText( aItemIndex );

	// Construct bidirectional text object
	// <cmail> Removed leave from non-leaving function.
	TBidiText* bidiText = NULL;
	TRAPD( err, bidiText = TBidiText::NewL( itemText, 1 ) );
	if ( err == KErrNone )
		{
		bidiText->WrapText( textRect.Width(), *font, NULL );
		}
		
	TPoint leftBase = textRect.iTl + TPoint( 0, topToBaseline );

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
		if ( rmluPosition >= 0 && aItemIndex > rmluPosition )
			{
			aItemIndex--;
			}

    // Change text transparency if match doesn't have email address.
    if ( clsItemArray[aItemIndex]->EmailAddress().Compare( KNullDesC ) == 0 )
        {
        const TInt KTransparency = 
            iListBox.LayoutHandler().PcsPopupDimmedFontTransparency();
        textColor.SetAlpha( KTransparency );
        iGc->SetPenColor( textColor );
        }

		// For now, we support underlining the matching part only if the
		//       text is written completely with left-to-right script
		
		// We know the text contains RTL script if the display string is not
		// just truncated version of the original string.
		TPtrC dispText = bidiText->DisplayText();
		// -1 to omit the truncation character
		TInt compLength = dispText.Length() - 1;
		TBool textContainsRtl = 
            ( itemText.Left( compLength ) != dispText.Left( compLength ) );
		
        const RArray<TPsMatchLocation>& underlines = 
                clsItemArray[aItemIndex]->Highlights();
        
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
					DrawPartOfItem( textRect, *font, currentTextStart,
                                    currentTextLength, itemText,
									EFalse, topToBaseline );
					}
				else if ( currentTextStart == underlines[i].index )
					{
					// draw underlined letters
					currentTextLength = underlines[i].length;
					
					DrawPartOfItem( textRect, *font, currentTextStart,
                                    currentTextLength, itemText,
									ETrue, topToBaseline );
					i++;
					}
				else 
					{
					// This is here, because PCS Engine might give you 
				    // duplicate match entries, in this case we're not 
                    // advancing text but we'll skip that match
					currentTextLength = 0;
					i++;
					}
					// update text start point
					currentTextStart += currentTextLength;
				
				if ( i >= underlines.Count() )
					{
					partsLeft = EFalse;
					// draw rest of the letters, if there are any after the
					// last underlined part
					if ( currentTextStart < itemText.Length() )
						{
						currentTextLength = 
						        itemText.Length() - currentTextStart;
						DrawPartOfItem( textRect, *font, currentTextStart,
                                        currentTextLength, itemText,
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
void CNcsListItemDrawer::DrawPartOfItem(
        const TRect& aItemRect, const CFont& aFont,
        TInt aStartPos, TInt aLength, const TDesC& aDes,
        TBool aUnderlined, TInt aBaselineOffsetFromTop ) const
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


// ---------------------------------------------------------------------------
// Updates popup list content.
// ---------------------------------------------------------------------------
//
void CNcsPopupListBox::UpdateListL()
    {
    FUNC_LOG;
    SetListItemsFromArrayL();

    // Close the popup if it's empty and make sure it's visible if it's not empty.
    UpdateVisibilityL( !IsPopupEmpty() );
    }

// ---------------------------------------------------------------------------
// Shows or hides popup if necessary.
// ---------------------------------------------------------------------------
//
void CNcsPopupListBox::UpdateVisibilityL( TBool aVisible )
    {
    if ( IsVisible() != aVisible )
        {
        if ( iObserver.PopupVisibilityChangingL( aVisible ) )
            {
            MakeVisible( aVisible );
            if ( aVisible )
                {
                UpdateScrollBarsL();
                SetScrollBarVisibilityL();
                // Popup is always on top (also on top of e.g. toolbar buttons)
                DrawableWindow()->SetOrdinalPosition( 0 );
                }
            }
        }
    else if ( IsVisible() )
        {
        UpdateScrollBarsL();
        SetScrollBarVisibilityL();
        }
    }

// ---------------------------------------------------------------------------
// Updates popup text colors.
// ---------------------------------------------------------------------------
//
void CNcsPopupListBox::UpdateTextColors()
    {
    if ( iItemDrawer )
        {
        // Get skin instance.
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();

        // Get color for popup text from current skin.
        TRgb textColor = KRgbBlack;
        AknsUtils::GetCachedColor( skin, textColor, KAknsIIDQsnTextColors,
            EAknsCIQsnTextColorsCG20 );
        iItemDrawer->SetTextColor( textColor );

        // Get text color for highlighted list item from current skin.
        AknsUtils::GetCachedColor( skin, textColor, KAknsIIDQsnTextColors,
            EAknsCIQsnTextColorsCG10 );
        iItemDrawer->SetHighlightedTextColor( textColor );
        }
    }

void CNcsPopupListBox::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( iBackgroundContext )
        {
        AknsDrawUtils::DrawBackground( skin, iBackgroundContext, this, gc,
            Rect().iTl, Rect(), KAknsDrawParamNoClearUnderImage );
        }
    else
        {
        gc.Clear( Rect() );
        }
    CEikListBox::Draw( Rect() );
    }

// End of File

