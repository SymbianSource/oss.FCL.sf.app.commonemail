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
* Description:  Thumb size and position adjustment.
*
*/


//<cmail> SF
#include "emailtrace.h"
#include <uiacceltk/HuiGridLayout.h>
#include <uiacceltk/HuiDeckLayout.h>
#include <uiacceltk/HuiImageVisual.h>
//</cmail>
#include "fsscrollbarclet.h"


// ======== MEMBER FUNCTIONS ========

CFsScrollbarClet::CFsScrollbarClet()
    : iThumbSpan( 1 )
    , iVisibility( EFsScrollbarAuto )
    , iUseDefaultBarImages( ETrue )
    , iUseDefaultThumbImages( ETrue )
    {
    FUNC_LOG;
    }


CFsScrollbarClet::~CFsScrollbarClet()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Set the scrollbar visuals.
// The scrollbar is build from these images by changing their places.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetVisuals(
    CHuiDeckLayout* aLayout,
    CHuiDeckLayout* aThumb )
    {
    FUNC_LOG;
    iScrollbarLayout = aLayout;
    iThumbLayout = aThumb;
    if ( !iScrollbarLayout || !iThumbLayout )
        {
        SetVisible( EFalse );
        }
    else if ( !iUseDefaultThumbImages )
        {
        TSize thumbSize(
            aLayout->Size().Target().iX > aThumb->Size().Target().iX
                ? aLayout->Size().Target().iX : aThumb->Size().Target().iX,
            aLayout->Size().Target().iY > aThumb->Size().Target().iY
                ? aLayout->Size().Target().iY : aThumb->Size().Target().iY
            );
        SetThumbSize( thumbSize );
        }
    else
        {
        // Calculate the minimum width/height from used images. Thumb is
        // created from 3 different images and the minimum size constructs
        // from the top and bottom images added together.
        if ( iHorizontal )
            {
            TInt bgWidth =
                static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 0 ).Visual( 0 )
                        )->Image().Texture().Size().iHeight +
                static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 0 ).Visual( 2 )
                        )->Image().Texture().Size().iHeight;
            TInt topWidth =
                static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 1 ).Visual( 0 )
                        )->Image().Texture().Size().iHeight +
                static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 1 ).Visual( 2 )
                        )->Image().Texture().Size().iHeight;

            iMinThumbSize.iWidth = bgWidth > topWidth ? bgWidth : topWidth;
            iMinThumbSize.iHeight = iThumbLayout->Size().Target().iY;

            SetThumbSize( iThumbLayout->Size().Target().AsSize() );
            }
        else
            {
            TInt bgHeight(
                static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 0 ).Visual( 0 )
                        )->Image().Texture().Size().iWidth
                + static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 0 ).Visual( 2 )
                        )->Image().Texture().Size().iWidth );
            TInt topHeight(
                static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 1 ).Visual( 0 )
                        )->Image().Texture().Size().iWidth
                + static_cast<CHuiImageVisual*>(
                    &iThumbLayout->Visual( 1 ).Visual( 2 )
                        )->Image().Texture().Size().iWidth );

            iMinThumbSize.iWidth = iThumbLayout->Size().Target().iX;
            iMinThumbSize.iHeight =
                bgHeight > topHeight ? bgHeight : topHeight;

            SetThumbSize( iThumbLayout->Size().Target().AsSize() );
            }

        if ( iVisible )
            {
            iScrollbarLayout->UpdateChildrenLayout();
            }
        }
    }


// ---------------------------------------------------------------------------
// Define the layout to use.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetHorizontal( TBool aHorizontal )
    {
    FUNC_LOG;
    iHorizontal = aHorizontal;
    }


// ---------------------------------------------------------------------------
// Set the visibility mode of this scrollbar.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetVisibleMode( TFsScrollbarVisibility aMode )
    {
    FUNC_LOG;
    iVisibility = aMode;
    SetThumbSpan( ThumbSpan() );
    }


// ---------------------------------------------------------------------------
// Resolve the visibility mode.
// ---------------------------------------------------------------------------
//
TFsScrollbarVisibility CFsScrollbarClet::VisibilityMode()
    {
    FUNC_LOG;
    return iVisibility;
    }


// ---------------------------------------------------------------------------
// Resolve if this scrollbar is visible.
// ---------------------------------------------------------------------------
//
TBool CFsScrollbarClet::IsVisible()
    {
    FUNC_LOG;
    return iVisible;
    }


// ---------------------------------------------------------------------------
// Set the range of the scrollbar. Will not actually move the scrollbar until
// Update() is called.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetRange( TInt aStart, TInt aEnd )
    {
    FUNC_LOG;
    iStart = aStart;
    iEnd = aEnd;

    SetThumbSpan( ThumbSpan() );
    }


// ---------------------------------------------------------------------------
// Resolve the minimum range value.
// ---------------------------------------------------------------------------
//
TInt CFsScrollbarClet::RangeStart()
    {
    FUNC_LOG;
    return iStart;
    }


// ---------------------------------------------------------------------------
// Resolve the maximum range value.
// ---------------------------------------------------------------------------
//
TInt CFsScrollbarClet::RangeEnd()
    {
    FUNC_LOG;
    return iEnd;
    }


// ---------------------------------------------------------------------------
// Set the count of visible items.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetThumbSpan( TInt aThumbSpan )
    {
    FUNC_LOG;
    iThumbSpan = aThumbSpan;
    if ( iEnd - iStart < iThumbSpan )
        {
        iThumbSpan = iEnd - iStart;
        }
    if ( iThumbSpan < 1 )
        {
        iThumbSpan = 1;
        }

    if ( EFsScrollbarHideAlways == iVisibility
        || ( EFsScrollbarAuto == iVisibility
            && iEnd - iStart <= aThumbSpan ) )
        {
        SetVisible( EFalse );
        }
    else
        {
        SetVisible( ETrue );
        }
    }


// ---------------------------------------------------------------------------
// Resolve the count of total items on the list.
// ---------------------------------------------------------------------------
//
TInt CFsScrollbarClet::ThumbSpan()
    {
    FUNC_LOG;
    return iThumbSpan;
    }


// ---------------------------------------------------------------------------
// Set the position of the scrollbar. Will not actually move the scrollbar
// until Update() is called.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetPos( const TInt aPos )
    {
    FUNC_LOG;
    if ( aPos < iStart )
        {
        iPos = iStart;
        }
    else if ( aPos > iEnd - iThumbSpan )
        {
        iPos = iEnd - iThumbSpan;
        }
    else
        {
        iPos = aPos;
        }
    }


// ---------------------------------------------------------------------------
// Get the index of the first position.
// ---------------------------------------------------------------------------
//
TInt CFsScrollbarClet::Pos()
    {
    FUNC_LOG;
    return iPos;
    }


// ---------------------------------------------------------------------------
// Set the height or width of the thumb item depending of the layout mode.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetThumbSize( const TSize& aSize )
    {
    FUNC_LOG;
    // Size must be bigger than the minimum size.
    iThumbSize.iWidth = iMinThumbSize.iWidth < aSize.iWidth
        ? aSize.iWidth : iMinThumbSize.iWidth;
    iThumbSize.iHeight = iMinThumbSize.iHeight < aSize.iHeight
        ? aSize.iHeight : iMinThumbSize.iHeight;
    }


// ---------------------------------------------------------------------------
// Resolve the thumb size.
// ---------------------------------------------------------------------------
//
TSize CFsScrollbarClet::ThumbSize()
    {
    FUNC_LOG;
    return iThumbSize;
    }


// ---------------------------------------------------------------------------
// Update scrollbar thumb positions.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::Update( TInt aLayoutTransitionTime )
    {
    FUNC_LOG;
    if ( !IsVisible() || !iScrollbarLayout || !iThumbLayout )
        {
        return;
        }

    TSize bounds( iScrollbarLayout->Size().Target().AsSize() );

    const TInt length( iEnd - iStart );
    TInt x( 0 );
    TInt y( 0 );
    TSize thumbSize( 0, 0 );

    if ( iHorizontal )
        {
        thumbSize.iHeight = iThumbSize.iHeight;
        }
    else
        {
        thumbSize.iWidth = iThumbSize.iWidth;
        }

    if ( length <= 0 )
        {
        if ( iHorizontal )
            {
            thumbSize.iWidth = bounds.iWidth;
            }
        else
            {
            thumbSize.iHeight = bounds.iHeight;
            }
        }
    else
        {
        // Calculate thumb size and position.
        if ( iHorizontal )
            {
            thumbSize.iWidth = bounds.iWidth * iThumbSpan / length;
            if ( iMinThumbSize.iWidth > thumbSize.iWidth )
                {
                bounds.iWidth -= iMinThumbSize.iWidth - thumbSize.iWidth;
                thumbSize.iWidth = iMinThumbSize.iWidth;
                }
            x = (TReal)( bounds.iWidth * ( iPos - iStart ) ) / length + 0.5;
            }
        else
            {
            thumbSize.iHeight = bounds.iHeight * iThumbSpan / length;
            if ( iMinThumbSize.iHeight > thumbSize.iHeight )
                {
                bounds.iHeight -= iMinThumbSize.iHeight - thumbSize.iHeight;
                thumbSize.iHeight = iMinThumbSize.iHeight;
                }
            y = (TReal)( bounds.iHeight * ( iPos - iStart ) ) / length + 0.5;
            }
        }

    // Center around the middle along the lesser axis.
    if ( iHorizontal )
        {
        y = ( bounds.iHeight - iThumbSize.iHeight ) / 2;
        }
    else
        {
        x = ( bounds.iWidth - iThumbSize.iWidth ) / 2;
        }

    iThumbLayout->SetPos( THuiRealPoint( x, y ), aLayoutTransitionTime );
    iThumbLayout->SetSize( thumbSize, aLayoutTransitionTime );

    if ( iUseDefaultThumbImages )
        {
        AdjustSize( static_cast<CHuiGridLayout*>(
            &iThumbLayout->Visual( 0 ) ), thumbSize, aLayoutTransitionTime );
        AdjustSize( static_cast<CHuiGridLayout*>(
            &iThumbLayout->Visual( 1 ) ), thumbSize, aLayoutTransitionTime );
        }
    }


// ---------------------------------------------------------------------------
// Inform that the custom bar image is used.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::EnableCustomBarImage()
    {
    FUNC_LOG;
    iUseDefaultBarImages = EFalse;
    }


// ---------------------------------------------------------------------------
// Inform that the custom thumb image is used.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::EnableCustomThumbImage()
    {
    FUNC_LOG;
    iUseDefaultThumbImages = EFalse;
    }


// ---------------------------------------------------------------------------
// Make this component hidden or visible.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::SetVisible( TBool aIsVisible )
    {
    FUNC_LOG;
    TBool changed( iVisible != aIsVisible );

    iVisible = aIsVisible;

    if ( aIsVisible )
        {
        SetPos( Pos() );

        if ( iUseDefaultBarImages && iScrollbarLayout && changed )
            {
            TSize size( iScrollbarLayout->Size().Target().AsSize() );
            AdjustSize( static_cast<CHuiGridLayout*>(
                &iScrollbarLayout->Visual( 0 ) ), size );
            iScrollbarLayout->UpdateChildrenLayout();
            }
        }
    }


// ---------------------------------------------------------------------------
// Adjust the scrollbar visual size. The visual constructs from three visual
// which size relations need to be calculated when the parent visual's size
// changes.
// ---------------------------------------------------------------------------
//
void CFsScrollbarClet::AdjustSize(
    CHuiGridLayout* aLayout,
    TSize& aTarget,
    TInt aLayoutTransitionTime )
    {
    FUNC_LOG;
    // By default use the vertical scrollbar values.
    TInt weight( aTarget.iHeight );
    THuiGridDimension gridDimension( EHuiGridRow );

    if ( aLayout->ColumnCount() > 1 )
        {
        // Horizontal scrollbar.
        weight = aTarget.iWidth;
        gridDimension = EHuiGridColumn;
        }

    weight -= aLayout->Weight( gridDimension, 0 ).iMagnitude;
    weight -= aLayout->Weight( gridDimension, 2 ).iMagnitude;

    if ( weight < 0 )
        {
        weight = 0;
        }

    TRAP_IGNORE( aLayout->ReplaceWeightL( gridDimension, weight, 1 ) ); // <cmail>

    aLayout->SetSize( aTarget, aLayoutTransitionTime );
    }

