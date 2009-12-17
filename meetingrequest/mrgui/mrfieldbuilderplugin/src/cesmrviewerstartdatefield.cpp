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
* Description:  Start date field for viewers
 *
*/

#include "cesmrviewerstartdatefield.h"

#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include <esmrgui.rsg>

#include <eiklabel.h>
#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <StringLoader.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <AknLayout2ScalableDef.h>

// ======== LOCAL FUNCTIONS ========
namespace // codescanner::namespace
    {
    const TInt KComponentCount( 2 ); // icon and label
    const TInt KMaxTimeBuffer( 32 ); // buffer for date formatting
    } // unnamed namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::NewL()
// -----------------------------------------------------------------------------
//
CESMRViewerStartDateField* CESMRViewerStartDateField::NewL()
    {
    CESMRViewerStartDateField* self = new (ELeave) CESMRViewerStartDateField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::~CESMRViewerStartDateField()
// -----------------------------------------------------------------------------
//
CESMRViewerStartDateField::~CESMRViewerStartDateField()
    {
    delete iLabel;
    delete iIcon;
    }

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::CESMRViewerStartDateField()
// -----------------------------------------------------------------------------
//
CESMRViewerStartDateField::CESMRViewerStartDateField()
    {
    SetFieldId ( EESMRFieldStartDate );    
    SetFocusType( EESMRHighlightFocus );
    }

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::ConstructL()
// -----------------------------------------------------------------------------
//
void CESMRViewerStartDateField::ConstructL()
    {
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapDateStart );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStartDateField::InitializeL()
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

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::InternalizeL()
// -----------------------------------------------------------------------------
//
void CESMRViewerStartDateField::InternalizeL( MESMRCalEntry& aEntry )
    {
    // Read format string from AVKON resource
    HBufC* dateFormatString = iEikonEnv->AllocReadResourceLC
                               (R_QTN_DATE_USUAL_WITH_ZERO);
    TBuf<KMaxTimeBuffer> buf;
    CCalEntry& entry = aEntry.Entry();
    TTime startTime = entry.StartTimeL().TimeLocalL();

    startTime.FormatL(buf,*dateFormatString);

    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( buf );
    iLabel->SetTextL( buf );
    CleanupStack::PopAndDestroy( dateFormatString );
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerStartDateField::SizeChanged()
    {
    TRect rect = Rect();
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = rowLayoutRect.Rect();
    
    TAknWindowComponentLayout iconLayout =
        NMRLayoutManager::GetWindowComponentLayout( 
                NMRLayoutManager::EMRLayoutTextEditorIcon );
    AknLayoutUtils::LayoutImage( iIcon, rect, iconLayout );
    
    TAknLayoutRect bgLayoutRect = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    bgRect.SetHeight( bgRect.Height() + 1 );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    TAknLayoutText labelLayout = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutTextEditor );
    iLabel->SetRect( labelLayout.TextRect() );    
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerStartDateField::CountComponentControls() const
    {
    TInt count( KComponentCount );
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerStartDateField::ComponentControl( TInt aIndex ) const
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
void CESMRViewerStartDateField::SetOutlineFocusL( TBool aFocus )
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
