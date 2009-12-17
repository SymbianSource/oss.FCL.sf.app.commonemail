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

#include "cesmrvieweralarmdatefield.h"

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

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmDateField* CESMRViewerAlarmDateField::NewL()
    {
    CESMRViewerAlarmDateField* self = new (ELeave) CESMRViewerAlarmDateField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::~CESMRViewerAlarmDateField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmDateField::~CESMRViewerAlarmDateField()
    {
    delete iLabel;
    delete iIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::CESMRViewerAlarmDateField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmDateField::CESMRViewerAlarmDateField()
    {
    SetFieldId ( EESMRFieldAlarmDate );
    SetFocusType( EESMRHighlightFocus );
    }

// -----------------------------------------------------------------------------
// CESMRViewerAlarmDateField::ConstructL()
// -----------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::ConstructL()
    {
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAlarmDate );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::InitializeL()
    {
    // Setting Font for the rich text viewer
    TAknLayoutText text = NMRLayoutManager::GetLayoutText( 
            Rect(), 
            NMRLayoutManager::EMRTextLayoutTextEditor );
    
    iLabel->SetFont( text.Font() );
        
    // This is called so theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iLabel->FocusChanged( EDrawNow );
        }

    AknLayoutUtils::OverrideControlColorL( *iLabel, EColorLabelText,
                                       KRgbBlack );
    }


// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::InternalizeL( MESMRCalEntry& aEntry )
    {
    MESMRCalEntry::TESMRAlarmType alarmType;
    TTime alarmTime;

    aEntry.GetAlarmL( alarmType, alarmTime );

    if( alarmType == MESMRCalEntry::EESMRAlarmAbsolute )
        {
        // Read format string from AVKON resource
        HBufC* dateFormatString = iEikonEnv->AllocReadResourceLC(
                R_QTN_DATE_USUAL_WITH_ZERO );
        TBuf<KMaxTimeBuffer> buf;
        
        alarmTime.FormatL( buf, *dateFormatString );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( buf );
        iLabel->SetTextL( buf );
        
        CleanupStack::PopAndDestroy( dateFormatString );
        }
    else // Remove the alarm fields
        {
        CCoeControl::MakeVisible(EFalse);
        iObserver->RemoveControl(EESMRFieldAlarmDate);
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::SizeChanged()
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
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    TAknLayoutText labelLayout = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutTextEditor );
    iLabel->SetRect( labelLayout.TextRect() );    
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAlarmDateField::CountComponentControls() const
    {
    TInt count( KComponentCount );
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAlarmDateField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerAlarmDateField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::SetOutlineFocusL( TBool aFocus )
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

