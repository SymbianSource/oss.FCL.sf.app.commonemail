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
* Description:  Response dialog's custom control implementation
*
*/
#include "cmrresponsedialogview.h"
// System includes
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <eikrted.h>
#include <eiksbfrm.h>
#include <AknDef.h>
#include <AknUtils.h>
#include <gdi.h>

// DEBUG
#include "emailtrace.h"

// LOCAL DEFINITIONS
namespace // codescanner::namespace 
    { 
    #define KDefaultTextColor TRgb( 0x000000 );
    /// Get the text color from skin
    TRgb TextColor( TInt aSkinColorId )
        {
        TRgb bgColor;
        TInt err;

        err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                         bgColor,
                                         KAknsIIDQsnTextColors,
                                         aSkinColorId );
        if( err != KErrNone )
            {
            bgColor = KDefaultTextColor;
            }
        return bgColor;
        }    
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::NewL
// ---------------------------------------------------------------------------
//
CESMRResponseDialogView* CESMRResponseDialogView::NewL()
    {
    FUNC_LOG;
    CESMRResponseDialogView* self =
        new (ELeave) CESMRResponseDialogView();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::CESMRResponseDialogView
// ---------------------------------------------------------------------------
//
CESMRResponseDialogView::CESMRResponseDialogView()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRResponseDialogView::ConstructL()
    {
    FUNC_LOG;
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
                        KAknsIIDQsnBgAreaMain, Rect() , ETrue );
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::~CESMRResponseDialogView
// ---------------------------------------------------------------------------
//
CESMRResponseDialogView::~CESMRResponseDialogView()
    {
    FUNC_LOG;    
    delete iEditor;
    delete iBgContext;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRResponseDialogView::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType)
    {
    FUNC_LOG;
    return iEditor->OfferKeyEventL( aEvent, aType );
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::Draw
// ---------------------------------------------------------------------------
//
void CESMRResponseDialogView::Draw(const TRect& aRect) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();
      
    // Redraw the background using the default skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );    
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::MopSupplyObject
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CESMRResponseDialogView::MopSupplyObject(TTypeUid aId)
    {
    if (iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject(aId);
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRResponseDialogView::CountComponentControls() const
    {
    FUNC_LOG;
    return 1;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRResponseDialogView::ComponentControl( TInt /*aInd*/ ) const
    {
    FUNC_LOG;
    return iEditor;
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::GetTextL
// ---------------------------------------------------------------------------
//
HBufC* CESMRResponseDialogView::GetTextL()
    {
    FUNC_LOG;
    return iEditor->GetTextInHBufL();
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRResponseDialogView::SetContainerWindowL(
        const CCoeControl& aContainer)
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );
    iEditor = new (ELeave )CEikRichTextEditor();
    iEditor->ConstructL( this, 0, 0, CEikEdwin::ENoAutoSelection, 0, 0 );
    iEditor->SetFocus( ETrue );
    iEditor->SetContainerWindowL( *this );
    iEditor->SetSize( Rect().Size() );
    iEditor->SetSkinBackgroundControlContextL( iBgContext );
    
    // Create vertical scrollbar for editor
    iScrollBarFrame = iEditor->CreateScrollBarFrameL();
    iScrollBarFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn );

    TRAP_IGNORE( SetFontColorL() );
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::HandleResourceChange()
// ---------------------------------------------------------------------------
//
void CESMRResponseDialogView::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();
        }
    else if ( aType == KAknsMessageSkinChange ||
    		  aType == KEikMessageColorSchemeChange )
        {
        TRAP_IGNORE( SetFontColorL() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRResponseDialogView::SizeChanged()
    {
    if(iBgContext)
        {
        iBgContext->SetRect(Rect());
        if ( &Window() )
            {
            iBgContext->SetParentPos( PositionRelativeToScreen() );
            }
        }
    if( iEditor )
        {
        // Reduce scrollbar width from the editor width 
        TRect rect = Rect();
        TInt scrollbarWidth = iEditor->ScrollBarFrame()->ScrollBarBreadth( CEikScrollBar::EVertical );
        TInt editorWidth = rect.Width() - scrollbarWidth;
        if ( AknLayoutUtils::LayoutMirrored() )
        	{
            iEditor->SetExtent( TPoint( scrollbarWidth, 0 ), TSize( editorWidth, rect.Height() ) );       	
        	}
        else
        	{
            iEditor->SetExtent( TPoint( 0, 0 ), TSize( editorWidth, rect.Height() ) );
        	}
        }
    }

// ---------------------------------------------------------------------------
// CESMRResponseDialogView::MinimumSize()
// ---------------------------------------------------------------------------
//
TSize CESMRResponseDialogView::MinimumSize()
    {
    FUNC_LOG;
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    return rect.Size();
    }

// -----------------------------------------------------------------------------
// CESMRResponseDialogView::SetFontColorL()
// -----------------------------------------------------------------------------
//
void CESMRResponseDialogView::SetFontColorL()
    {
    const CFont* sysfont = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont, NULL );
    TFontSpec fontSpec = sysfont->FontSpecInTwips();
    TCharFormat charFormat;
    TCharFormatMask formatMask;
    charFormat.iFontSpec = fontSpec;

    formatMask.SetAttrib( EAttFontTypeface );
    formatMask.SetAttrib( EAttFontHeight );
    formatMask.SetAttrib( EAttFontPosture );

    charFormat.iFontPresentation.iTextColor = TextColor( EAknsCIQsnTextColorsCG6 );
    formatMask.SetAttrib( EAttColor );
  
    CCharFormatLayer* charFormatLayer =
        CCharFormatLayer::NewL( charFormat, formatMask );

    iEditor->SetCharFormatLayer( charFormatLayer );
    }

// end of file
