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
* Description: cuicaknscrollbaradapter.cpp
*
*/

#include "cuicaknscrollbaradapter.h"
#include "emailtrace.h"

#include <AknsConstants.h>

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::NewL
// ---------------------------------------------------------------------------
//
MUiCScrollBar* CUiCAknScrollBarAdapter::NewL( MUiCScrollBar::MObserver& aObserver )
    {
    FUNC_LOG;
    CUiCAknScrollBarAdapter* self = 
            new (ELeave) CUiCAknScrollBarAdapter( aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::Destroy
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::Destroy()
    {
    FUNC_LOG;
    delete this;
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::Show
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::Show( const TBool aShow )
    {
    FUNC_LOG;
    iScrollBar->MakeVisible( aShow );
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::~CUiCScrollBar
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::UpdateModelL( const TUiCScrollBarModel& aModel )
    {
    FUNC_LOG;
    iModel.SetScrollSpan( aModel.Height() );
    iModel.SetFocusPosition( aModel.WindowPosition() );
    iModel.SetWindowSize( aModel.WindowHeight() );
    iScrollBar->SetModelL( &iModel );
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::ThumbPosition
// ---------------------------------------------------------------------------
//
TInt CUiCAknScrollBarAdapter::ThumbPosition() const
    {
    FUNC_LOG;
    return iScrollBar->ThumbPosition();
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::SetRect
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::SetRect( const TRect& aRect )
    {
    FUNC_LOG;
    iScrollBar->SetRect( aRect );
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::Redraw
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::Redraw()
    {
    FUNC_LOG;
    iScrollBar->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::NotifyThemeChanged
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::NotifyThemeChanged()
    {
    FUNC_LOG;
    iScrollBar->HandleResourceChange( KAknsMessageSkinChange );
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::LayoutHints
// ---------------------------------------------------------------------------
//
TInt CUiCAknScrollBarAdapter::LayoutHints() const
    {
    FUNC_LOG;
    return 0;
    }

// ---------------------------------------------------------------------------
//  CUiCAknScrollBarAdapter::HandleScrollEventL
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::HandleScrollEventL( CEikScrollBar* aScrollBar,
        TEikScrollEvent aEventType )
    {
    FUNC_LOG;
    if ( aScrollBar == iScrollBar )
        {
        switch ( aEventType )
            {
            case EEikScrollHome:
                iObserver.HandleScrollEventL( *this, EScrollHome );
                break;
            case EEikScrollEnd:
                iObserver.HandleScrollEventL( *this, EScrollEnd );
                break;
            default:
                iObserver.HandleScrollEventL( *this, EScrollPosition );
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::CUiCAknScrollBarAdapter
// ---------------------------------------------------------------------------
//
CUiCAknScrollBarAdapter::CUiCAknScrollBarAdapter( MUiCScrollBar::MObserver& aObserver ) 
    : iObserver( aObserver )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::~CUiCAknScrollBarAdapter
// ---------------------------------------------------------------------------
//
CUiCAknScrollBarAdapter::~CUiCAknScrollBarAdapter()
    {
    FUNC_LOG;
    delete iScrollBar;
    }

// ---------------------------------------------------------------------------
// CUiCAknScrollBarAdapter::ConstructL
// ---------------------------------------------------------------------------
//
void CUiCAknScrollBarAdapter::ConstructL()
    {
    FUNC_LOG;
    iScrollBar = new (ELeave) CAknDoubleSpanScrollBar( NULL );
    iScrollBar->ConstructL( ETrue, this, 0, CEikScrollBar::EVertical, 1000 );
    }

// end of file
