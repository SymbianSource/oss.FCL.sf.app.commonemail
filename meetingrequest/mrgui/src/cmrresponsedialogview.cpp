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
* Description:  Response dialog's custom control implementation
*
*/
#include "cmrresponsedialogview.h"
// System includes
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <eikrted.h>
#include <AknDef.h>
// DEBUG
#include "emailtrace.h"

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
    iEditor->EnableCcpuSupportL(ETrue);
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
        iEditor->SetRect( Rect() );
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
        iEditor->SetRect( Rect() );
        }
    }

// end of file
