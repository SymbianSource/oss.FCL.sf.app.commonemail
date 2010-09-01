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
* Description:  Custom draw component for message editor.
*
*/


#include "emailtrace.h"
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <AknsConstants.h>

#include "ncscustomdraw.h"
#include "ncseditor.h"

// ========================= MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// CNcsCustomDraw::CNcsCustomDraw
// ---------------------------------------------------------------------------
//
CNcsCustomDraw::CNcsCustomDraw( 
    const MLafEnv& aEnv,
    const CCoeControl& aControl,
    CLafEdwinCustomDrawBase* aCustomDrawer,
    CNcsEditor* aEditor,
    TBool aHeaderField ) :
    CLafEdwinCustomDrawBase( aEnv, aControl ), iCustomDrawer( aCustomDrawer ),
    iEditor( aEditor ), iHeaderField( aHeaderField )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CNcsCustomDraw::~CNcsCustomDraw
// ---------------------------------------------------------------------------
//
CNcsCustomDraw::~CNcsCustomDraw()
    {
    FUNC_LOG;
    delete iCustomDrawer;
    }

// ---------------------------------------------------------------------------
// CNcsCustomDraw::UpdateColors
// ---------------------------------------------------------------------------
//
void CNcsCustomDraw::UpdateColors()
    {
    FUNC_LOG;
    iColors = 0;

    TInt err = KErrNotFound;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    if ( iHeaderField )
        {
        err = AknsUtils::GetCachedColor(
            skin,
            iBackgroundColor,
            KAknsIIDFsOtherColors,
            EAknsCIFsOtherColorsCG12 );

        if ( err == KErrNone )
            {
            SetHasColor( EColorBackground );
            }
        }
     
    err = AknsUtils::GetCachedColor(
        skin,
        iSelectionTextColor,
        KAknsIIDFsTextColors,
        EAknsCIFsTextColorsCG5 );
   
    if ( err == KErrNone )
        {
        SetHasColor( EColorSelectionText );
        }

    err = AknsUtils::GetCachedColor(
        skin,
        iSelectionBackgroundColor,
        KAknsIIDFsHighlightColors,
        EAknsCIFsHighlightColorsCG2 );

    if ( err == KErrNone )
        {
        SetHasColor( EColorSelectionBackground );
        }
    }

// ---------------------------------------------------------------------------
// CNcsCustomDraw::DrawBackground
// ---------------------------------------------------------------------------
//
void CNcsCustomDraw::DrawBackground(
    const TParam& aParam,
    const TRgb& aBackground,
    TRect& aDrawn ) const
    {
    FUNC_LOG;
    // Use custom drawing for fields with background color. Othwerwise use
    // the base implementation which draws white background.
    if ( HasColor( EColorBackground ) )
        {
        iCustomDrawer->DrawBackground( aParam, iBackgroundColor, aDrawn );
        }
    else
        {
        CLafEdwinCustomDrawBase::DrawBackground( aParam, aBackground, aDrawn );
        }
    }

// ---------------------------------------------------------------------------
// CNcsCustomDraw::DrawLineGraphics
// ---------------------------------------------------------------------------
//
void CNcsCustomDraw::DrawLineGraphics(
    const TParam& aParam,
    const TLineInfo& aLineInfo ) const
    {
    FUNC_LOG;
    iCustomDrawer->DrawLineGraphics( aParam, aLineInfo );
    }
	
// ---------------------------------------------------------------------------
// CNcsCustomDraw::DrawText
// ---------------------------------------------------------------------------
//
void CNcsCustomDraw::DrawText(
    const TParam& aParam,
    const TLineInfo& aLineInfo,
    const TCharFormat& aFormat,
    const TDesC& aText,
    const TPoint& aTextOrigin,
    TInt aExtraPixels ) const
    {
    FUNC_LOG;
    iCustomDrawer->DrawText(
        aParam,
        aLineInfo,
        aFormat,
        aText,
        aTextOrigin,
        aExtraPixels );
    }

// ---------------------------------------------------------------------------
// CNcsCustomDraw::SystemColor
// ---------------------------------------------------------------------------
//
TRgb CNcsCustomDraw::SystemColor( TUint aColorIndex, TRgb aDefaultColor ) const
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TInt err = KErrNotFound;
    TRgb ret = aDefaultColor;

    if ( aColorIndex == TLogicalRgb::ESystemSelectionForegroundIndex )
        {
        if ( HasColor( EColorSelectionText ) )
            {
            err = KErrNone;
            ret = iSelectionTextColor;
            }
        }
    else if ( aColorIndex == TLogicalRgb::ESystemSelectionBackgroundIndex )
        {
        if ( HasColor( EColorSelectionBackground ) )
            {
            err = KErrNone;
            ret = iSelectionBackgroundColor;
            }
        }
    
    if ( err != KErrNone )
        {
        return iCustomDrawer->SystemColor( aColorIndex, aDefaultColor );
        }
    else
        {
        return ret;
        }
    }

// ---------------------------------------------------------------------------
// CNcsCustomDraw::SetHasColor
// ---------------------------------------------------------------------------
//
void CNcsCustomDraw::SetHasColor( TColors aColor )
    {
    FUNC_LOG;
    iColors |= ( TInt )aColor;
    }

// ---------------------------------------------------------------------------
// CNcsCustomDraw::HasColor
// ---------------------------------------------------------------------------
//
TBool CNcsCustomDraw::HasColor( TColors aColor ) const
    {
    FUNC_LOG;
    TInt color = ( TInt )aColor;
    return iColors & color;
    }

