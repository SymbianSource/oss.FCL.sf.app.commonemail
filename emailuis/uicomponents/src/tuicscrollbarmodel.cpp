/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: tuicscrollbarmodel.cpp
*
*/

#include "tuicscrollbarmodel.h"
#include "emailtrace.h"

#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::TUiCScrollbarModel
// ---------------------------------------------------------------------------
//
TUiCScrollBarModel::TUiCScrollBarModel()
    : iHeight( 0 ), iWindowHeight( 0 ), iWindowPosition( 0 )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::SetHeight
// ---------------------------------------------------------------------------
//
void TUiCScrollBarModel::SetHeight( TInt aHeight )
    {
    FUNC_LOG;
    iHeight = aHeight;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::Height
// ---------------------------------------------------------------------------
//
TInt TUiCScrollBarModel::Height() const
    {
    FUNC_LOG;
    return iHeight;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::SetWindowHeight
// ---------------------------------------------------------------------------
//
void TUiCScrollBarModel::SetWindowHeight( TInt aHeight )
    {
    FUNC_LOG;
    iWindowHeight = aHeight;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::WindowHeight
// ---------------------------------------------------------------------------
//
TInt TUiCScrollBarModel::WindowHeight() const
    {
    FUNC_LOG;
    return iWindowHeight;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::SetWindowPosition
// ---------------------------------------------------------------------------
//
void TUiCScrollBarModel::SetWindowPosition( TInt aPosition )
    {
    FUNC_LOG;
    iWindowPosition = aPosition;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::WindowPosition
// ---------------------------------------------------------------------------
//
TInt TUiCScrollBarModel::WindowPosition() const
    {
    FUNC_LOG;
    return iWindowPosition;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::ThumbHeight
// ---------------------------------------------------------------------------
//
TInt TUiCScrollBarModel::ThumbHeight() const
    {
    FUNC_LOG;
    TInt thumbHeight( 0 );
    if ( iHeight > 0 )
        {
        thumbHeight = Min( 
                Max( CalculatedThumbHeight(), MinimumThumbHeight() ), 
                    iWindowHeight );
        }
    return thumbHeight;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::ThumbPosition
// ---------------------------------------------------------------------------
//
TInt TUiCScrollBarModel::ThumbPosition() const
    {
    FUNC_LOG;    
    TInt thumbPosition( 0 );
    if ( iHeight > 0 )
        {
        thumbPosition = iWindowHeight * iWindowPosition / iHeight;
        if ( iWindowHeight > 0 )
            {
            thumbPosition -= (ThumbHeight() - CalculatedThumbHeight()) 
                    * thumbPosition / iWindowHeight;
            }
        }    
    return thumbPosition;
    } 

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::ScrollBarNeeded
// ---------------------------------------------------------------------------
//
TBool TUiCScrollBarModel::ScrollBarNeeded() const
    {
    FUNC_LOG;
    return iWindowHeight < iHeight;
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::MinimumThumbHeight
// ---------------------------------------------------------------------------
//
TInt TUiCScrollBarModel::MinimumThumbHeight() const
    {
    FUNC_LOG;
    TAknLayoutRect layoutRect;
    TAknWindowComponentLayout layout = AknLayoutScalable_Avkon::aid_size_min_handle();
    layoutRect.LayoutRect( TRect(), layout.LayoutLine() );
    return layoutRect.Rect().Height();
    }

// ---------------------------------------------------------------------------
// TUiCScrollBarModel::CalculatedThumbHeight
// ---------------------------------------------------------------------------
//
TInt TUiCScrollBarModel::CalculatedThumbHeight() const
    {
    FUNC_LOG;
    TInt thumbHeight( 0 );
    if ( iHeight > 0 )
        {
        thumbHeight = iWindowHeight * iWindowHeight / iHeight;
        }
    return thumbHeight;
    }

// end of file
