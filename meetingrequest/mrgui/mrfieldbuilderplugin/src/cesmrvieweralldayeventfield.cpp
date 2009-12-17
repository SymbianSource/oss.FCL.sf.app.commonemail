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
* Description:  ESMR UI Container class
*
*/


#include "cesmrvieweralldayeventfield.h"
#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"

#include <caluser.h>
#include <calalarm.h>
#include <calentry.h>
#include <StringLoader.h>
#include <AknLayout2ScalableDef.h>
#include <esmrgui.rsg>

#include "emailtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerAllDayEventField* CESMRViewerAllDayEventField::NewL()
    {
    FUNC_LOG;
    CESMRViewerAllDayEventField* self = new (ELeave) CESMRViewerAllDayEventField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::~CESMRViewerAllDayEventField()
// ---------------------------------------------------------------------------
//
CESMRViewerAllDayEventField::~CESMRViewerAllDayEventField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLabel;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::CESMRViewerAllDayEventField()
// ---------------------------------------------------------------------------
//
CESMRViewerAllDayEventField::CESMRViewerAllDayEventField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldAllDayEvent );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( KAknsIIDQgnMeetReqIndiCheckboxMark );
    iIcon->SetParent( this );
    
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.IsAllDayEventL() )
        {
        HBufC* allDayEventText =
            StringLoader::LoadLC( R_QTN_MEET_REQ_ALL_DAY_EVENT, iEikonEnv );
        iLabel->SetTextL( *allDayEventText );
        CleanupStack::PopAndDestroy( allDayEventText );
        }
    else
        {
        // set the field as hidden:
        MakeVisible( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::InitializeL()
    {
    TAknLayoutText text = NMRLayoutManager::GetLayoutText( 
            Rect(), 
            NMRLayoutManager::EMRTextLayoutTextEditor );
    
    iLabel->SetFont( text.Font() );
    // This is called so that theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iLabel->FocusChanged( EDrawNow );
        }

    AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                             KRgbBlack );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::SizeChanged()
    {
    TRect rect = Rect();
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = rowLayoutRect.Rect();
    
    TAknWindowComponentLayout iconLayout =
        NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutTextEditorIcon );
    AknLayoutUtils::LayoutImage( iIcon, rect, iconLayout );
    
    TAknLayoutRect bgLayoutRect =
        NMRLayoutManager::GetLayoutRect( rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
 
    TAknLayoutText labelLayout = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutTextEditor );
    iLabel->SetRect( labelLayout.TextRect() );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAllDayEventField::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iIcon )
        {
        ++count;
        }

    if ( iLabel )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAllDayEventField::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iIcon;
        case 1:
            return iLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::SetOutlineFocusL( TBool aFocus )
    {
    CESMRField::SetOutlineFocusL ( aFocus );
    
    iLabel->SetFocus( aFocus );    

    if ( !aFocus )
        {
        AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                                 KRgbBlack );
        }
    }

// EOF
