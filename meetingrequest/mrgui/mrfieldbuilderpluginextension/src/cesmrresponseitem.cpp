/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Meeting Request viewer response field implementation
*
*/
#include "cesmrresponseitem.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cesmrlayoutmgr.h"
#include "cmrimage.h"
#include "cmrlabel.h"

#include <esmrgui.rsg>

// DEBUG
#include "emailtrace.h"

namespace // codescanner::namespace
    {
    const TInt KMaxLinesInResponseTopicItem( 2 );
    const TInt KEdge(8);
    } // unnamed namespace
// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRResponseItem::NewL
// -----------------------------------------------------------------------------
//
CESMRResponseItem* CESMRResponseItem::NewL( TESMRCommand aCmd,
                                            const TDesC& aItemText,
                                            TBool aHasIcon )
    {
    FUNC_LOG;
    CESMRResponseItem* self = CESMRResponseItem::NewLC( aCmd,
                                                        aItemText,
                                                        aHasIcon );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::NewL
// -----------------------------------------------------------------------------
//
CESMRResponseItem* CESMRResponseItem::NewLC( TESMRCommand aCmd,
                                             const TDesC& aItemText,
                                             TBool aHasIcon )
    {
    FUNC_LOG;
    CESMRResponseItem* self = new (ELeave) CESMRResponseItem( aCmd );
    CleanupStack::PushL( self );
    self->ConstructL( aItemText, aHasIcon );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::~CESMRResponseItem
// -----------------------------------------------------------------------------
//
CESMRResponseItem::~CESMRResponseItem()
    {
    FUNC_LOG;
    delete iSelectionLabel;
    delete iIcon;
    delete iItemText;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::CESMRResponseItem
// -----------------------------------------------------------------------------
//
CESMRResponseItem::CESMRResponseItem(TESMRCommand aCmd)
: iCmd (aCmd)
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::ConstructL( const TDesC& aItemText, TBool aHasIcon )
    {
    FUNC_LOG;
    iSelectionLabel = CMRLabel::NewL();
    iSelectionLabel->SetTextL( aItemText );

    // Response item might not have icon. e.g. topic line
    iHasIcon = aHasIcon;
    if( aHasIcon )
        {
        iIcon = IconL( EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::Draw
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;
    //switch font color to unhighlighted, leave can be ignored
    //since it's just a color change
    TRAP_IGNORE(AknLayoutUtils::OverrideControlColorL(
            *iSelectionLabel,
            EColorLabelText,
            KRgbBlack ));
    if( iHighlighted )
        {
        //switch font color to highlighted, leave can be ignored
        //since it's just a color change
        TRAP_IGNORE(AknLayoutUtils::OverrideControlColorL(
                *iSelectionLabel,
                EColorLabelText,
                iLayout->ViewerListAreaHighlightedTextColor() ));

        CWindowGc& gc = SystemGc();
        TRect rect = aRect;
        TBool enableEdges(ETrue);

        CFbsBitmap* selector = NULL;
        CFbsBitmap* selectorMask = NULL;
        
        TSize corner(KEdge, KEdge);
        NMRBitmapManager::GetSkinBasedBitmap( 
                NMRBitmapManager::EMRBitmapListTopLeft, 
                selector, selectorMask, corner );

        if( selector && selectorMask && enableEdges)
            {
            //corner TL
            gc.BitBltMasked( rect.iTl, selector, corner, selectorMask, EFalse );

            //side L
            TSize side(KEdge, (rect.Height() - 2 * KEdge) );
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListLeft, 
                    selector, selectorMask, side );
            gc.BitBltMasked( TPoint(rect.iTl.iX, rect.iTl.iY + KEdge),
                             selector, side, selectorMask, EFalse );

            //corner BL
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListBottomLeft, 
                    selector, selectorMask, corner );
            gc.BitBltMasked( TPoint(rect.iTl.iX, 
                            rect.iTl.iY + KEdge + side.iHeight),
                            selector, corner, selectorMask, EFalse );

            //top
            TSize top( (rect.Width() - 2 * KEdge) , KEdge);
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListTop, 
                    selector, selectorMask, top );
            gc.BitBltMasked( TPoint(rect.iTl.iX + KEdge, rect.iTl.iY),
                             selector, top, selectorMask, EFalse );

            //center
            TSize center( top.iWidth, side.iHeight);
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListCenter, 
                    selector, selectorMask, center );
            gc.BitBltMasked( TPoint(rect.iTl.iX + KEdge, rect.iTl.iY + KEdge),
                             selector, center, selectorMask, EFalse );

            //bottom
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListBottom, 
                    selector, selectorMask, top );
            gc.BitBltMasked( TPoint(rect.iTl.iX + KEdge, 
                            rect.iTl.iY + side.iHeight + KEdge),
                            selector, top, selectorMask, EFalse );

            //corner TR
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListTopRight, 
                    selector, selectorMask, corner );
            gc.BitBltMasked( TPoint(rect.iTl.iX + KEdge + top.iWidth, 
                            rect.iTl.iY),
                            selector, corner, selectorMask, EFalse );

            //side R
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListRight, 
                    selector, selectorMask, side );
            gc.BitBltMasked( TPoint(rect.iTl.iX + KEdge + top.iWidth, 
                            rect.iTl.iY + KEdge),
                            selector, side, selectorMask, EFalse );

            //corner Br
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListBottomRight, 
                    selector, selectorMask, corner );
            gc.BitBltMasked( TPoint(rect.iTl.iX + KEdge + top.iWidth, 
                            rect.iTl.iY + KEdge + side.iHeight),
                            selector, corner, selectorMask, EFalse );
            }
        else if(!enableEdges)
            {
            //center
            TSize center( rect.Width(), rect.Height() );
            NMRBitmapManager::GetSkinBasedBitmap( 
                    NMRBitmapManager::EMRBitmapListCenter, 
                    selector, selectorMask, center );
            gc.BitBltMasked( TPoint(rect.iTl.iX, rect.iTl.iY), 
                    selector, center, selectorMask, EFalse );
            }
        else // This should NOT be called ever.
            {
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.SetPenStyle( CGraphicsContext::ENullPen );
            gc.SetBrushColor( KRgbGreen );
            rect.SetSize( TSize(rect.Size().iWidth - 1,
                                rect.Size().iHeight ) );
            gc.DrawRect( rect );
            }

        delete selector;
        delete selectorMask;
        }
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CESMRResponseItem::CountComponentControls() const
    {
    FUNC_LOG;
    TInt itemCount = 0;
    if( iSelectionLabel )
        {
        itemCount++;
        }
    if( iIcon )
        {
        itemCount++;
        }

    return itemCount;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CESMRResponseItem::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    CCoeControl* control = NULL;
    if ( aInd == 0 )
        {
        control = iSelectionLabel;
        }

    if ( aInd == 1 )
        {
        control = iIcon;
        }
    return control;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::SizeChanged
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( this->Rect() );

    // Icon exists, this is answer item ( accept / tentative / decline )
    if( iIcon )
        {
        // Icon
        TAknWindowComponentLayout iconLayout = 
            NMRLayoutManager::GetWindowComponentLayout( 
                    NMRLayoutManager::EMRLayoutCheckboxIcon );
        AknLayoutUtils::LayoutImage( iIcon, rect, iconLayout );
        // Label 
        TAknTextComponentLayout labelLayout =
            NMRLayoutManager::GetTextComponentLayout( 
                    NMRLayoutManager::EMRTextLayoutCheckboxEditor );
        AknLayoutUtils::LayoutLabel( iSelectionLabel, rect, labelLayout );
        }
    else // There is no icon (this is the topic item)
        {
        TAknTextComponentLayout labelLayout =
            NMRLayoutManager::GetTextComponentLayout( 
                    NMRLayoutManager::EMRTextLayoutText );
        AknLayoutUtils::LayoutLabel( iSelectionLabel, rect, labelLayout );
        }


    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::IconL
// -----------------------------------------------------------------------------
//
CMRImage* CESMRResponseItem::IconL( TBool aChecked )
    {
    FUNC_LOG;
    NMRBitmapManager::TMRBitmapId iconID( 
            NMRBitmapManager::EMRBitmapCheckBoxOff );

    if( aChecked )
        {
        iconID = NMRBitmapManager::EMRBitmapCheckBoxOn;
        }
    CMRImage* icon = CMRImage::NewL( iconID );
    return icon;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::SetHighlight
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::SetHighlight()
    {
    FUNC_LOG;
    iHighlighted = ETrue;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::RemoveHighlight
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::RemoveHighlight()
    {
    FUNC_LOG;
    iHighlighted = EFalse;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::ChangeIconL
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::ChangeIconL( TBool aChecked )
    {
    FUNC_LOG;
    delete iIcon;
    iIcon = NULL;
    iIcon = IconL( aChecked );
    SizeChanged();
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::SetFont
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::SetFont( const CFont* aFont )
    {
    FUNC_LOG;
    iSelectionLabel->SetFont( aFont );
    // Leave case can be ignored here because there is nothing to do about it
    TRAP_IGNORE(AknLayoutUtils::OverrideControlColorL(
                                      *iSelectionLabel,
                                      EColorLabelText,
                                      KRgbBlack ) );
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::SetTextL
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::SetTextL( const TDesC& aItemText )
    {
    FUNC_LOG;
    // When this method is called for the first time(with valid text)
    // text is stored to member variable. This is done because
    // text needs to be wrapped again (orientation might have
    // changed) when SizeChanged() gets called.
    if( !iItemText &&  aItemText.Length() > 0 )
        {
        iOriginalTextLength = aItemText.Length();
        iItemText = HBufC::NewL( aItemText.Length() );
        *iItemText = aItemText;
        }
    else if( !iItemText &&  aItemText.Length() <= 0 )
        {
        // if there is no text yet, there is nothing to do here.
        return;
        }

    TInt lineCount = 1; // Default line count.
    // If this item has no icon, it may use two lines for the text
    if( !iIcon )
        {
        lineCount = KMaxLinesInResponseTopicItem;
        }
    // Text wrapping
    CArrayFixFlat<TInt>* widthArray =
                        new (ELeave) CArrayFixFlat<TInt>( lineCount );
    CleanupStack::PushL( widthArray );
    for ( TInt i(0); i < lineCount; i++ )
        {
        // If this item has no icon, all the space is for text
        if( !iIcon )
            {
            TAknLayoutText layout = NMRLayoutManager::GetLayoutText( Rect(), NMRLayoutManager::EMRTextLayoutText );            
            widthArray->AppendL( layout.TextRect().Width() );
            }
        else
            {
            widthArray->AppendL( iLayout->ResponseAreaAnswerTextSize().iWidth );
            }
        }

    HBufC* wrappedText;
    // Set the font for the text
    const CFont* font = iSelectionLabel->Font();
    RBuf buffer; // codescanner::resourcenotoncleanupstack
    buffer.CreateL( iOriginalTextLength + widthArray->Count() );
    buffer.CleanupClosePushL();

    // Wrap the text
    AknTextUtils::WrapToStringAndClipL( iItemText->Des(), *widthArray, *font, buffer );
    wrappedText = buffer.AllocLC();

    // Set the text to label
    iSelectionLabel->SetTextL( *wrappedText );
    CleanupStack::PopAndDestroy(3); // widthArray, wrappedText, buffer
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::SetLayoutManager
// -----------------------------------------------------------------------------
//
void CESMRResponseItem::SetLayoutManager( CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    iLayout = aLayout;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::ItemTextLineCount
// -----------------------------------------------------------------------------
//
TInt CESMRResponseItem::ItemTextLineCount()
    {
    FUNC_LOG;
    TInt retValue( KErrNotFound );
    if( iSelectionLabel )
        {
        retValue = iSelectionLabel->NumberOfLines();
        }
    return retValue;
    }

// -----------------------------------------------------------------------------
// CESMRResponseItem::CommandId
// -----------------------------------------------------------------------------
//
TESMRCommand CESMRResponseItem::CommandId() const
    {
    FUNC_LOG;
    return iCmd;
    }

