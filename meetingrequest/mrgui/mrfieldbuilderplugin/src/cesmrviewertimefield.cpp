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
* Description:  Time field for viewers
*
*/

#include "cesmrviewertimefield.h"
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
// DEBUG
#include "emailtrace.h"

//unnamed namespace for local constants functions
namespace{ // codescanner::namespace

const TInt KTimeBufferSize( 50 );
const TInt KTempBufferSize( 20 );

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerTimeField* CESMRViewerTimeField::NewL()
    {
    FUNC_LOG;
    CESMRViewerTimeField* self = new (ELeave) CESMRViewerTimeField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::~CESMRViewerTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerTimeField::~CESMRViewerTimeField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLabel;
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.IsAllDayEventL() )
        {
        // set the field as hidden:
        iObserver->RemoveControl( FieldId() );
        }
    else
        {
        _LIT(KTimeSeparator," - ");
        CCalEntry& entry = aEntry.Entry();
        
        TTime startTime = entry.StartTimeL().TimeLocalL();
        TTime endTime = entry.EndTimeL().TimeLocalL();

        HBufC* timeFormatString =
            iEikonEnv->AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );
        HBufC* finalBuf = HBufC::NewLC( KTimeBufferSize );
        HBufC* startBuf = HBufC::NewLC( KTempBufferSize );
        HBufC* endBuf = HBufC::NewLC( KTempBufferSize );
        const TInt KNumBuffers( 4 );
        
        TPtr startPtr( startBuf->Des() );
        startTime.FormatL( startPtr, *timeFormatString );
        TPtr endPtr( endBuf->Des() );
        endTime.FormatL( endPtr, *timeFormatString );
        TPtr finalPtr( finalBuf->Des() );
        finalPtr.Append( startPtr );
        finalPtr.Append( KTimeSeparator );
        finalPtr.Append( endPtr );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( finalPtr );
        iLabel->SetTextL( finalPtr );
        CleanupStack::PopAndDestroy( KNumBuffers, timeFormatString );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::InitializeL()
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
// CESMRViewerTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::SizeChanged()
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
// CESMRViewerTimeField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerTimeField::CountComponentControls() const
    {
    TInt count( 2 );
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerTimeField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerTimeField::CESMRViewerTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerTimeField::CESMRViewerTimeField()
    {
    SetFieldId( EESMRFieldMeetingTime );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::ConstructL()
    {
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapClock );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::SetOutlineFocusL( TBool aFocus )
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
