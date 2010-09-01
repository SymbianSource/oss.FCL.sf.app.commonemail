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
* Description:  Definition of the class CESMRNcsPopupListBox
*
*/

#include "cesmrncspopuplistbox.h"
#include "cesmrncsemailaddressobject.h"
#include "cesmrcontacthandler.h"
#include "nmrbitmapmanager.h"
#include "nmrcolormanager.h"

#include <eikclbd.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <StringLoader.h>                       // StringLoader
#include <ct/rcpointerarray.h>
//text truncation
#include <AknBidiTextUtils.h>//line wrapping and mirroring
#include <aknlayoutscalable_apps.cdl.h> //xml layout data for applications
#include <aknlayoutscalable_avkon.cdl.h> //xml layout data of avkon components
#include <esmrgui.rsg>
#include <eikscrlb.h>
// DEBUG
#include "emailtrace.h"

namespace { // codescanner::namespace
const TInt KItemExtraHeight = 8;
const TInt KEdge (8);
const TInt KListBoxDrawMargin (4);
//drop down list colors since we have no official LAF
#define KWhite TRgb( 255,255,255 )
#define KGraySelectable TRgb( 30,30,30 )
#define KGrayNoEmail TRgb( 215,215,215 )
#define KGrayBackground TRgb( 140,140,140 )
#define KSelectorFallbackColor TRgb( 0,200,200 )
}

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::NewL
// -----------------------------------------------------------------------------
//
CESMRNcsPopupListBox* CESMRNcsPopupListBox::NewL( const CCoeControl* aParent,
                                                  CESMRContactHandler& aContactHandler )
    {
    FUNC_LOG;
    CESMRNcsPopupListBox* self = new (ELeave) CESMRNcsPopupListBox( aContactHandler );
    CleanupStack::PushL(self);
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::CESMRNcsPopupListBox
// -----------------------------------------------------------------------------
//
CESMRNcsPopupListBox::CESMRNcsPopupListBox( CESMRContactHandler& aContactHandler )
    :
    iContactHandler( aContactHandler )
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::ConstructL( const CCoeControl* aParent )
    {
    FUNC_LOG;
    CEikTextListBox::ConstructL( aParent, CEikListBox::EPopout );

    CEikTextListBox::SetBorder( TGulBorder::EWithOutline|
                                TGulBorder::EAddTwoPixels|
                                TGulBorder::EAddOneRoundingPixel );
    
    CreateScrollBarFrameL();

    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont );
    CEikTextListBox::SetItemHeightL( font->HeightInPixels() + KItemExtraHeight );

    iBaseBackroundContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQgnFsGrafEmailContent,
        Rect(),
        EFalse );
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::InitAndSearchL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::InitAndSearchL( const TDesC& aText )
    {
    FUNC_LOG;
    RCPointerArray<CESMRClsItem> matchingArray; // Empty array
    CleanupClosePushL( matchingArray );
    SetSearchTextL( aText );
    CleanupStack::PopAndDestroy( &matchingArray );
    }

// ---------------------------------------------------------------------------
// CESMRNcsPopupListBox::~CESMRNcsPopupListBox
// ---------------------------------------------------------------------------
//
CESMRNcsPopupListBox::~CESMRNcsPopupListBox()
    {
    FUNC_LOG;
    delete iBaseBackroundContext;
    iMatchingArray.ResetAndDestroy();

    delete iItemTextsArray;
    delete iCurrentSearchText;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::CreateItemDrawerL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::CreateItemDrawerL()
    {
    FUNC_LOG;
    CESMRNcsListItemDrawer* drawer = new (ELeave) CESMRNcsListItemDrawer( *this );
    iItemDrawer = drawer;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CESMRNcsPopupListBox::MopSupplyObject(TTypeUid aId)
    {
    FUNC_LOG;
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBaseBackroundContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::SizeChanged
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::SizeChanged()
    {
    FUNC_LOG;
    CEikTextListBox::SizeChanged();
    iBaseBackroundContext->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CNcsPopupListBox::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CESMRNcsPopupListBox::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse ret( EKeyWasNotConsumed );
    if( aKeyEvent.iCode == EKeyDownArrow )
        {
        iView->MoveCursorL( CListBoxView::ECursorNextItem, CListBoxView::ENoSelection );
        ret = EKeyWasConsumed;
        }
    else if( aKeyEvent.iCode == EKeyUpArrow )
        {
        TBool stay = EFalse;
        // Move cursor separator line over
        if( CurrentItemIndex() - 1 > 0 && CurrentItemIndex() - 1 == iRMLUItemPosition )
            {
            iView->MoveCursorL( CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection );
            stay = ETrue;
            }

        iView->MoveCursorL( CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection );
        if( stay )
            {
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

// ---------------------------------------------------------------------------
// CESMRNcsPopupListBox::OperationCompleteL
// ---------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::OperationCompleteL(
        TContactHandlerCmd /*aCmd*/,
        const RPointerArray<CESMRClsItem>* aMatchingItems )
    {
    FUNC_LOG;
    if ( aMatchingItems )
        {
        iMatchingArray.ResetAndDestroy();
        // Replace old matcing items.

        for ( TInt ii = 0; ii < aMatchingItems->Count(); ++ii )
         {
         if ( (*aMatchingItems)[ii] )
            {
            CESMRClsItem* item = (*aMatchingItems)[ii]->CloneLC();
            iMatchingArray.AppendL( item );
            CleanupStack::Pop( item );
            }
         }
        SetListItemsFromArrayL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRNcsPopupListBox::OperationErrorL
// ---------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::OperationErrorL( TContactHandlerCmd /*aCmd*/,
                                            TInt /*aError*/ )
    {
    FUNC_LOG;
    //no errors handled here
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::SetSearchTextL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::SetSearchTextL( const TDesC& aText )
    {
    FUNC_LOG;
    delete iCurrentSearchText;
    iCurrentSearchText = NULL; // to remove code scanner warning
    iCurrentSearchText = aText.AllocL();
    iContactHandler.SearchMatchesL( aText, this );
    
    if(!iRemoteLookupSupported)
        {
        iRemoteLookupSupported = iContactHandler.RemoteLookupSupportedL();
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::ReturnCurrentEmailAddressLC
// -----------------------------------------------------------------------------
//
CESMRNcsEmailAddressObject* CESMRNcsPopupListBox::ReturnCurrentEmailAddressLC()
    {
    FUNC_LOG;
    CESMRNcsEmailAddressObject* addressObject = NULL;

    if( iMatchingArray.Count() > 0 )
        {
        CESMRClsItem* clsItem = NULL;
        clsItem =iMatchingArray[CurrentItemIndex()];
        addressObject= CESMRNcsEmailAddressObject::NewL( clsItem->DisplayName(), clsItem->EmailAddress() );
        CleanupStack::PushL( addressObject );
        if ( clsItem->MultipleEmails() )
            {
            addressObject->SetDisplayFull( ETrue );
            }
        }

    return addressObject;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::SetPopupMaxRectL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::SetPopupMaxRectL( const TRect& aPopupMaxRect )
    {
    FUNC_LOG;
    iPopupMaxRect = aPopupMaxRect;
    SetPopupHeightL();
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::IsPopupEmpty
// -----------------------------------------------------------------------------
//
TBool CESMRNcsPopupListBox::IsPopupEmpty() const
    {
    FUNC_LOG;
    if( Model()->NumberOfItems() > 0 )
        {
        return EFalse;
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::IsRemoteLookupItemSelected
// -----------------------------------------------------------------------------
//
TBool CESMRNcsPopupListBox::IsRemoteLookupItemSelected() const
    {
    FUNC_LOG;
    if( CurrentItemIndex() == iRMLUItemPosition && iRemoteLookupSupported )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::CurrentPopupClsItemsArray
// -----------------------------------------------------------------------------
//
const RPointerArray<CESMRClsItem>& CESMRNcsPopupListBox::CurrentPopupClsItemsArray() const
    {
    return iMatchingArray;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::RMLUItemPosition
// -----------------------------------------------------------------------------
//
TInt CESMRNcsPopupListBox::RMLUItemPosition() const
    {
    FUNC_LOG;
    return iRMLUItemPosition;
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::SetListItemsFromArrayL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::SetListItemsFromArrayL()
    {
    FUNC_LOG;
    // Create totally new text array
    Reset();
    CreateTextArrayAndSetToTheListboxL();

    // append texts to text array
    for( TInt i=0; i < iMatchingArray.Count(); i++ )
        {
        iItemTextsArray->AppendL( iMatchingArray[i]->FullTextL() );
        }

    // Update rmlu item
    SetRemoteLookupItemLastToTheListL();

    HandleItemAdditionL();

    if( iItemTextsArray && iItemTextsArray->Count() > 0 )
        {
        SetCurrentItemIndex( 0 );
        }

    if ( Observer() )
        {
        Observer()->HandleControlEventL( this,
                MCoeControlObserver::EEventStateChanged );
        }

    if( IsVisible() )
        {
        SetPopupHeightL();
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::SetPopupHeightL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::SetPopupHeightL()
    {
    FUNC_LOG;
    TInt totalHeight = CalcHeightBasedOnNumOfItems( Model()->NumberOfItems() );
    TRect rect = iPopupMaxRect;

    ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn );

    //shrink listbox if less than max area needed to show items
    if( rect.Height() >= totalHeight )
        {
        ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );

        //latch listbox on top of the editorfield
        TRect fieldArea = Parent()->Rect();

        if ( rect.iBr.iY < fieldArea.iBr.iY )
             {
             rect.Move(0, (rect.Height() - totalHeight) );
             }

        rect.SetHeight( totalHeight );
        }

    UpdateScrollBarsL();
    SetRect( rect );
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::SetRemoteLookupItemLastToTheListL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::SetRemoteLookupItemLastToTheListL()
    {
    FUNC_LOG;
    
    if( iRemoteLookupSupported )
        {
        HBufC* rmluText = StringLoader::LoadLC( 
                R_MEET_REQ_EDITOR_ADDRESS_LIST_REMOTE_LOOKUP_SEARCH, 
                *iCurrentSearchText );

        iItemTextsArray->AppendL( *rmluText );
        CleanupStack::PopAndDestroy( rmluText );
        iRMLUItemPosition = iItemTextsArray->Count() - 1;
        }
    else
        {
        iRMLUItemPosition = KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsPopupListBox::CreateTextArrayAndSetToTheListboxL
// -----------------------------------------------------------------------------
//
void CESMRNcsPopupListBox::CreateTextArrayAndSetToTheListboxL()
    {
    FUNC_LOG;
    if( iItemTextsArray )
        {
        delete iItemTextsArray;
        iItemTextsArray = NULL;
        }

    const TInt KItemTextArrayLen = 8;
    
    if( !iItemTextsArray )
        {
        iItemTextsArray = new ( ELeave ) CDesCArraySeg( KItemTextArrayLen );
        // Set the popup list data
        Model()->SetItemTextArray( iItemTextsArray );
        Model()->SetOwnershipType( ELbmDoesNotOwnItemArray  );
        }
    }

// -----------------------------------------------------------------------------
// CESMRNcsListItemDrawer::CESMRNcsListItemDrawer
// -----------------------------------------------------------------------------
//
CESMRNcsListItemDrawer::CESMRNcsListItemDrawer( CESMRNcsPopupListBox& aListBox )
:CListItemDrawer(),
iListBox(aListBox)
    {
    FUNC_LOG;
    // Store a GC for later use
    iGc = &CCoeEnv::Static()->SystemGc();
    SetGc(iGc);
    }

// -----------------------------------------------------------------------------
// CESMRNcsListItemDrawer::DrawActualItemL
// -----------------------------------------------------------------------------
//
void CESMRNcsListItemDrawer::DrawActualItem( TInt aItemIndex,
	const TRect& aActualItemRect, TBool aItemIsCurrent,
    TBool aViewIsEmphasized, TBool aViewIsDimmed,
    TBool aItemIsSelected ) const
    {
    FUNC_LOG;
    TRAP_IGNORE( DoDrawActualItemL( aItemIndex,
                                    aActualItemRect,
                                    aItemIsCurrent,
                                    aViewIsEmphasized,
                                    aViewIsDimmed,
                                    aItemIsSelected ) )
    }

// -----------------------------------------------------------------------------
// CESMRNcsListItemDrawer::DoDrawActualItemL
// -----------------------------------------------------------------------------
//
void CESMRNcsListItemDrawer::DoDrawActualItemL( TInt aItemIndex,
    const TRect& aActualItemRect, TBool aItemIsCurrent,
    TBool /*aViewIsEmphasized*/, TBool /*aViewIsDimmed*/,
    TBool /*aItemIsSelected*/ ) const
    {
    FUNC_LOG;
    DrawPopUpBackGroundL( aActualItemRect );
    
    // Draw the selector if current item is the focused one
    if( aItemIsCurrent )
        {
        DrawPopUpSelectorL( aActualItemRect );
        }

    DrawPopUpTextL( aItemIndex, aActualItemRect );
    }

// -----------------------------------------------------------------------------
// CESMRNcsListItemDrawer::DrawPopUpBackGroundL
// -----------------------------------------------------------------------------
//
void CESMRNcsListItemDrawer::DrawPopUpBackGroundL(
        const TRect& aActualItemRect ) const
    {
    iGc->SetPenColor( KGrayBackground );
    iGc->SetBrushColor( KGrayBackground );

    iGc->SetPenStyle( CGraphicsContext::ESolidPen );
    iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );

    iGc->DrawRect( aActualItemRect );
    }
    
// -----------------------------------------------------------------------------
// CESMRNcsListItemDrawer::DrawPopUpSelectorL
// -----------------------------------------------------------------------------
//
void CESMRNcsListItemDrawer::DrawPopUpSelectorL( 
        const TRect& aActualItemRect ) const
    {
    CFbsBitmap* selector = NULL;
    CFbsBitmap* selectorMask = NULL;

    // Highlight bitmap target rect:
    TRect rect( aActualItemRect );

    TSize corner( KEdge, KEdge );
    NMRBitmapManager::GetSkinBasedBitmap( 
            NMRBitmapManager::EMRBitmapListTopLeft, 
            selector, selectorMask, corner );            
    
    // Adjust selector size if scrollbar is present
    if ( iListBox.ScrollBarFrame()->
            ScrollBarVisibility( CEikScrollBar::EVertical ) == 
                CEikScrollBarFrame::EOn )
        {
        TInt scrollBarWidth = 
                iListBox.ScrollBarFrame()->VerticalScrollBar()->Rect().Width();
        rect.SetWidth( ( rect.Width() - scrollBarWidth ) );
        }
               
    if( selector && selectorMask )
        {
        //corner TL
        iGc->BitBltMasked( 
                rect.iTl, selector, corner, selectorMask, EFalse );

        //side L
        TSize side( KEdge, ( rect.Height() - 2 * KEdge ) );
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListLeft, 
                selector, selectorMask, side );
        iGc->BitBltMasked( TPoint(rect.iTl.iX, rect.iTl.iY + KEdge ),
                          selector, side, selectorMask, EFalse );

        //corner BL
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListBottomLeft, 
                selector, selectorMask, corner );
        iGc->BitBltMasked( 
                TPoint(rect.iTl.iX, rect.iTl.iY + KEdge + side.iHeight ),
                selector, corner, selectorMask, EFalse );

        //top
        TSize top( ( rect.Width() - 2 * KEdge ) , KEdge );
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListTop, 
                selector, selectorMask, top );
        iGc->BitBltMasked( TPoint( rect.iTl.iX + KEdge, rect.iTl.iY ),
                          selector, top, selectorMask, EFalse );

        //center
        TSize center( top.iWidth, side.iHeight );
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListCenter, 
                selector, selectorMask, center );
        iGc->BitBltMasked( 
                TPoint( rect.iTl.iX + KEdge, rect.iTl.iY + KEdge ),
                selector, center, selectorMask, EFalse );

        //bottom
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListBottom, 
                selector, selectorMask, top );
        iGc->BitBltMasked( 
        TPoint( rect.iTl.iX + KEdge, rect.iTl.iY + side.iHeight + KEdge ),
        selector, top, selectorMask, EFalse );

        //corner TR
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListTopRight, 
                selector, selectorMask, corner );
        iGc->BitBltMasked( 
                TPoint( rect.iTl.iX + KEdge + top.iWidth, rect.iTl.iY ),
                selector, corner, selectorMask, EFalse );

        //side R
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListRight, 
                selector, selectorMask, side );

        iGc->BitBltMasked( 
         TPoint( rect.iTl.iX + KEdge + top.iWidth, rect.iTl.iY + KEdge ),
         selector, side, selectorMask, EFalse );

        //corner Br
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListBottomRight, 
                selector, selectorMask, corner );
        iGc->BitBltMasked( 
                TPoint( rect.iTl.iX + KEdge + top.iWidth, 
                       rect.iTl.iY + KEdge + side.iHeight ),
                selector, corner, selectorMask, EFalse );
        }
    else
        {
                iGc->SetBrushColor( KSelectorFallbackColor );
        }            
    delete selector;
    delete selectorMask;
    }

// -----------------------------------------------------------------------------
// CESMRNcsListItemDrawer::DrawPopUpTextL
// -----------------------------------------------------------------------------
//
void CESMRNcsListItemDrawer::DrawPopUpTextL( 
        TInt aItemIndex, 
        const TRect& aActualItemRect ) const
    {
    // Get reference to curren popup cls item list.
    const RPointerArray<CESMRClsItem>& clsItemArray = 
        iListBox.CurrentPopupClsItemsArray();
    TInt rmluPosition = iListBox.RMLUItemPosition();

    iGc->SetPenColor( NMRColorManager::Color( 
                NMRColorManager::EMRMainAreaTextColor ) );
    
    // Sets all the attributes, like font, text color and background color.
    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont );
    iGc->UseFont( font );
    
    iGc->SetPenStyle(CGraphicsContext::ESolidPen);
    iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
    TInt topToBaseline = ( aActualItemRect.Height() - font->HeightInPixels() ) / 2
                        + font->AscentInPixels();

    TPtrC itemText( iListBox.Model()->ItemText( aItemIndex ) );

    // Construct bidirectional text object
    TBidiText* bidiText = TBidiText::NewL( itemText, 1 );
    CleanupStack::PushL( bidiText );
        bidiText->WrapText( aActualItemRect.Width(), *font, NULL );
    TPoint leftBase = aActualItemRect.iTl + TPoint( 0, topToBaseline );
    leftBase.iX += KListBoxDrawMargin;

    // check if we are drawing remote lookup item or contact match
    if ( rmluPosition == aItemIndex )
        {
        iGc->SetUnderlineStyle( EUnderlineOff );
        bidiText->DrawText( *iGc, leftBase );
        }
    else
        {
        // if list has rmlu item change item index right
//        if ( rmluPosition >= 0 )
//            {
//            --aItemIndex;
//            aItemIndex = Max( 0, aItemIndex );
//            }

        TPtrC email = clsItemArray[aItemIndex]->EmailAddress().Ptr();
        // change color to gray if match doesn't have email address.
        if ( clsItemArray[aItemIndex]->EmailAddress().Compare( KNullDesC ) == 0 )
            {
            iGc->SetPenColor( KGrayNoEmail );
            iGc->SetBrushColor( KGrayNoEmail );
            }

        // TO-DO: For now, we support underlining the matching part only if the
        // text is written completely with left-to-right script
        
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
    CleanupStack::PopAndDestroy( bidiText );
	}

// -----------------------------------------------------------------------------
// CESMRNcsListItemDrawer::DrawPartOfItem
// -----------------------------------------------------------------------------
void CESMRNcsListItemDrawer::DrawPartOfItem( const TRect& aItemRect, const CFont& aFont,
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
    currentTextRect.iTl.iX = currentTextRect.iTl.iX + pixels + KListBoxDrawMargin;
    
    // adjust selector size for if scrollbar is on screen
    if ( iListBox.ScrollBarFrame()->
            ScrollBarVisibility( CEikScrollBar::EVertical ) ==
                CEikScrollBarFrame::EOn )
        {
        TInt scrollBarWidth = 
                iListBox.ScrollBarFrame()->VerticalScrollBar()->Rect().Width();
        currentTextRect.iBr.iX = 
            currentTextRect.iBr.iX - KListBoxDrawMargin - scrollBarWidth;
        }
    else
        {
        currentTextRect.iBr.iX = currentTextRect.iBr.iX - KListBoxDrawMargin;
        }
    
    iGc->DrawText( aDes.Mid( aStartPos, aLength ), currentTextRect, aBaselineOffsetFromTop );
    
    }

// End of File
