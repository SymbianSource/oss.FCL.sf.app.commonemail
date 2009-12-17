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
* Description:  Custom draw component for message editor.
*
*/
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayoutFont.h>
#include "emailtrace.h"
#include "ncseditorcustomdraw.h"
#include "ncseditor.h"
#include "ncsconstants.h"

const TInt KFSColorDarkeningDegree = 3;

// ========================= MEMBER FUNCTIONS ==================================

CNcsEditorCustomDraw* CNcsEditorCustomDraw::NewL( const MFormCustomDraw* aParentCustomDraw,
                                                  const CNcsEditor* aParentControl,
                                                  const TAknTextComponentLayout aLayout )
    {
    CNcsEditorCustomDraw* self = new (ELeave) CNcsEditorCustomDraw( aParentCustomDraw,
                                                                    aParentControl,
                                                                    aLayout );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CNcsEditorCustomDraw::CNcsEditorCustomDraw
// ---------------------------------------------------------------------------
//
CNcsEditorCustomDraw::CNcsEditorCustomDraw( const MFormCustomDraw* aParentCustomDraw,
                                            const CNcsEditor* aParentControl,
                                            const TAknTextComponentLayout aLayout ) :
                                            iCustomDrawer( aParentCustomDraw ),
                                            iEditor( aParentControl )
    {
    FUNC_LOG;
    UpdateLayout( aLayout );
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::ConstructL
// ---------------------------------------------------------
//
void CNcsEditorCustomDraw::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CNcsEditorCustomDraw::~CNcsEditorCustomDraw
// ---------------------------------------------------------------------------
//
CNcsEditorCustomDraw::~CNcsEditorCustomDraw()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CNcsEditorCustomDraw::DrawBackground
// ---------------------------------------------------------------------------
//
void CNcsEditorCustomDraw::DrawBackground(
    const TParam& aParam,
    const TRgb& aBackground,
    TRect& aDrawn ) const
    {
    FUNC_LOG;
    iCustomDrawer->DrawBackground( aParam, aBackground, aDrawn );
    TInt lineHeigth( 0 );
    TRAP_IGNORE( lineHeigth = iEditor->GetLineHeightL() );

    TAknTextDecorationMetrics decorationMetrics( iTextPaneLayout.LayoutLine().FontId() );
    TInt topMargin, bottomMargin;
    decorationMetrics.GetTopAndBottomMargins( topMargin, bottomMargin );

    TInt lineOffset = iTextPaneLayout.H() + topMargin + bottomMargin;
    
    TRgb lineColor = NcsUtility::CalculateMsgBodyLineColor( KFSColorDarkeningDegree, 
                                                            NcsUtility::SeparatorLineColor() );
    aParam.iGc.SetPenColor( lineColor );

    TInt margin( 0 );
    if ( aParam.iDrawRect.Height() < lineHeigth ||
         aParam.iDrawRect.Height() == lineOffset )
        {
        margin = 1;
        }

    TRect currentRect( aParam.iDrawRect.iTl , TPoint( aParam.iDrawRect.iBr.iX, aParam.iDrawRect.iTl.iY + lineOffset - margin ));
    
    while ( currentRect.iBr.iY <= aParam.iDrawRect.iBr.iY )
        {
        if ( currentRect.iTl.iY >= aParam.iDrawRect.iTl.iY  )
            {
            aParam.iGc.DrawLine( TPoint( currentRect.iTl.iX, currentRect.iBr.iY), currentRect.iBr );
            }
        currentRect.Move( 0, lineHeigth );
        }
    }

// ---------------------------------------------------------------------------
// CNcsEditorCustomDraw::DrawLineGraphics
// ---------------------------------------------------------------------------
//
void CNcsEditorCustomDraw::DrawLineGraphics( const TParam& aParam,
                                             const TLineInfo& aLineInfo ) const
    {
    FUNC_LOG;
    iCustomDrawer->DrawLineGraphics( aParam, aLineInfo );
    }
	
// ---------------------------------------------------------------------------
// CNcsEditorCustomDraw::DrawText
// ---------------------------------------------------------------------------
//
void CNcsEditorCustomDraw::DrawText( const TParam& aParam,
                               const TLineInfo& aLineInfo,
                               const TCharFormat& aFormat,
                               const TDesC& aText,
                               const TPoint& aTextOrigin,
                               TInt aExtraPixels ) const
    {
    FUNC_LOG;
    iCustomDrawer->DrawText( aParam,
                             aLineInfo,
                             aFormat,
                             aText,
                             aTextOrigin,
                             aExtraPixels );
    }

// ---------------------------------------------------------------------------
// CNcsEditorCustomDraw::SystemColor
// ---------------------------------------------------------------------------
//
TRgb CNcsEditorCustomDraw::SystemColor( TUint aColorIndex, TRgb aDefaultColor ) const
    {
    FUNC_LOG;
    return iCustomDrawer->SystemColor( aColorIndex, aDefaultColor );
    }


// ---------------------------------------------------------------------------
// CNcsEditorCustomDraw::UpdateLayout
// ---------------------------------------------------------------------------
//
void CNcsEditorCustomDraw::UpdateLayout( TAknTextComponentLayout aLayout )
    {
    iTextPaneLayout = aLayout;
    }
