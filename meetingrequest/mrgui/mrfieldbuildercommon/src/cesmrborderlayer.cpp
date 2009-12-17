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
* Description:  Class draws focus for the fields
*
*/

#include "emailtrace.h"
#include "cesmrborderlayer.h"

#include <AknUtils.h>
#include <AknsConstants.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRBorderLayer::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRBorderLayer* CESMRBorderLayer::NewL( 
		CCoeControl* aExt, TESMRFieldFocusType aFocusType )
    {
    FUNC_LOG;
    CESMRBorderLayer* self = new (ELeave) CESMRBorderLayer( aExt, aFocusType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::CESMRBorderLayer
// ---------------------------------------------------------------------------
//
CESMRBorderLayer::CESMRBorderLayer( 
		CCoeControl* aExtControl, 
		TESMRFieldFocusType aFocusType )
    :iExtControl( aExtControl ),
    iFocus( EFalse ),
    iFocusType( aFocusType )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::~CESMRBorderLayer
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRBorderLayer::~CESMRBorderLayer()
    {
    FUNC_LOG;
    delete iExtControl;
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRBorderLayer::ConstructL() // codescanner::LFunctionCantLeave
    {
    FUNC_LOG;
    SetBackground( this );
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::SetContainerWindowL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRBorderLayer::SetContainerWindowL(
		const CCoeControl& aContainer)
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );
    if ( iExtControl )
        {
        iExtControl->SetContainerWindowL( aContainer );
        }
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRBorderLayer::SetOutlineFocusL( // codescanner::LFunctionCantLeave
        TBool aFocus )
    {
    FUNC_LOG;
    iFocus = aFocus;
    if ( iExtControl )
        {
        iExtControl->SetFocus( aFocus );
        }
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::HasFocus
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRBorderLayer::HasFocus() const
    {
    FUNC_LOG;
    return iFocus;
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::Draw
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRBorderLayer::Draw(
		CWindowGc &/*aGc*/, 
		const CCoeControl &/*aControl*/, 
		const TRect& /*aRect*/) const
    {
    FUNC_LOG;
    // Do nothing. Deprecated.
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::GetTextDrawer
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRBorderLayer::GetTextDrawer(
		CCoeTextDrawerBase*& /*aTextDrawer*/, 
		const CCoeControl* /*aDrawingControl*/) const
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::CountComponentControls
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRBorderLayer::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iExtControl )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::ComponentControl
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CESMRBorderLayer::ComponentControl( TInt /*aInd*/ ) const
    {
    FUNC_LOG;
    return iExtControl;
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::SizeChanged
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRBorderLayer::SizeChanged()
    {
    FUNC_LOG;
    TRect rect = Rect();
    if ( iExtControl )
        {
        iExtControl->SetRect( rect );
        }
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CESMRBorderLayer::OfferKeyEventL( 
		const TKeyEvent& aEvent, TEventCode aType )
    {
    FUNC_LOG;
    if ( iExtControl )
        {
        return iExtControl->OfferKeyEventL( aEvent, aType );
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// CESMRBorderLayer::SetLayoutManager()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRBorderLayer::SetLayoutManager( 
		CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    iLayout = aLayout;
    }

// EOF

