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

#include "cesmrvieweralarmtimefield.h"
#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "nmrbitmapmanager.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"

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

#include <esmrgui.rsg>

//unnamed namespace for local constants functions
namespace{ // codescanner::namespace

const TInt KComponentCount( 2 ); // icon and label
const TInt KMaxTimeBuffer( 50 ); // buffer for time formatting

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::CESMRViewerAlarmTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmTimeField* CESMRViewerAlarmTimeField::NewL()
    {
    CESMRViewerAlarmTimeField* self = new (ELeave) CESMRViewerAlarmTimeField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::~CESMRViewerAlarmTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmTimeField::~CESMRViewerAlarmTimeField()
    {
    delete iIcon;
    delete iLabel;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::CESMRViewerAlarmTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmTimeField::CESMRViewerAlarmTimeField()
    {
    SetFieldId ( EESMRFieldAlarmTime );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::ConstructL()
    {
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAlarmTime );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::InitializeL()
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
// CESMRViewerAlarmTimeField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::InternalizeL( MESMRCalEntry& aEntry )
    {
    MESMRCalEntry::TESMRAlarmType alarmType;
    TTime alarmTime;

    aEntry.GetAlarmL( alarmType, alarmTime );

    if( alarmType == MESMRCalEntry::EESMRAlarmAbsolute )
        {
        // Read format string from AVKON resource
        HBufC* dateFormatString =
            iEikonEnv->AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );
        TBuf<KMaxTimeBuffer> buf;
            
        alarmTime.FormatL( buf, *dateFormatString );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( buf );
        iLabel->SetTextL( buf );
        CleanupStack::PopAndDestroy( dateFormatString );
        }
    else // Remove the alarm fields
        {
        CCoeControl::MakeVisible( EFalse );
        iObserver->RemoveControl( EESMRFieldAlarmTime );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::SizeChanged()
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
// CESMRViewerAlarmTimeField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAlarmTimeField::CountComponentControls() const
    {
    TInt count( KComponentCount );
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAlarmTimeField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerAlarmTimeField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::SetOutlineFocusL( TBool aFocus )
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

